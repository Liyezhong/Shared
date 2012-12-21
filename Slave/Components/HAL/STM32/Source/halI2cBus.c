/****************************************************************************/
/*! \file halI2cBus.c
 *
 *  \brief   I2C Inter-IC-Bus interface
 *
 *  $Version: $ 0.1
 *  $Date:    $ 12.05.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage the on-chip
 *      Inter-IC-Bus (I2C) interfaces. Since I2C is a bus, where multiple
 *      bus members (slaves) can be connected to, each interface can be
 *      opened several times - once for each bus member. The transfer to
 *      or from a certain slave is done using asynchronous read and write
 *      functions, using the slave handle returned by the open function.
 *      Asynchronous means, that the function returns immediately, w/o
 *      waiting for the completion of the transaction. When the operation
 *      has finished, a user function is called to notify the caller. 
 *      The user can supply this callback function when opening the I2C
 *      interface for a slave. Each slave can have its own callback.
 *
 *      The read/write transactions are stored in a so called Job Queue,
 *      and processed from this queue asynchronously per interrupt. The
 *      I2C protocol is event driven, the data transfer is done using
 *      direct memory access (DMA) controller.
 *
 *      Attention: If using baudrates greater than the maximum specified
 *      for standard mode the interface is switched into fast mode. As a
 *      workaround according to STM errata sheet this limit is reduced
 *      from 100kHz to 88kHz.
 *
 *  \b Company:
 *
 *      Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/


#include <stdlib.h>
#include "Global.h"
#include "halError.h"
#include "halCommon.h"
#include "halClocks.h"
#include "halMain.h"
#include "halSysTick.h"
#include "halInterrupt.h"
#include "halUtilities.h"
#include "halDma.h"
#include "halI2cBus.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define MIN_RATE_STDMODE    5000     //!< Min baudrate in standard mode
#define MAX_RATE_STDMODE    88000    //!< Max baudrate in standard mode
#define MAX_RATE_FASTMODE   400000   //!< Max baudrate in fast mode
#define I2C_FIFO_SIZE       10       //!< Size of job queue

#define I2C_SR1_OVR         0x0800   //!< Overrun/Underrun
#define I2C_SR1_AF          0x0400   //!< Acknowledge failure
#define I2C_SR1_ARLO        0x0200   //!< Arbitration lost
#define I2C_SR1_BERR        0x0100   //!< Bus error (misplaced Start or Stop)
#define I2C_SR1_TxE         0x0080   //!< Transmit data register empty
#define I2C_SR1_RxNE        0x0040   //!< Receive data register not empty
#define I2C_SR1_STOPF       0x0010   //!< Stop detection (slave mode)
#define I2C_SR1_ADD10       0x0008   //!< 10-bit header sent (Master mode)
#define I2C_SR1_BTF         0x0004   //!< Byte transfer finished
#define I2C_SR1_ADDR        0x0002   //!< Address sent (master mode)
#define I2C_SR1_SB          0x0001   //!< Start bit generated
#define I2C_SR1_ERRORS      0xFF00   //!< Error flag bit mask

#define I2C_SR2_TRA         0x0004   //!< Transmitter/receiver
#define I2C_SR2_BUSY        0x0002   //!< Bus busy
#define I2C_SR2_MSL         0x0001   //!< Master/slave mode
#define I2C_SR2_GENCALL     0x0010   //!< General call address
#define I2C_SR2_DUALF       0x0080   //!< Dual address flag

#define I2C_CR1_SWRST       0x8000   //!< Software reset
#define I2C_CR1_ACKN        0x0400   //!< Acknowledge enable
#define I2C_CR1_STOP        0x0200   //!< Stop generation
#define I2C_CR1_START       0x0100   //!< Start generation
#define I2C_CR1_NOSTRETCH   0x0080   //!< Clock stretching disable
#define I2C_CR1_PE          0x0001   //!< Peripheral enable

#define I2C_CR2_LAST        0x1000   //!< Next DMA EOT is the last transfer
#define I2C_CR2_DMAEN       0x0800   //!< DMA requests enable
#define I2C_CR2_ITBUFEN     0x0400   //!< Buffer interrupt enable
#define I2C_CR2_ITEVTEN     0x0200   //!< Event interrupt enable
#define I2C_CR2_ITERREN     0x0100   //!< Error interrupt enable
#define I2C_CR2_FREQ        0x003F   //!< Peripheral clock frequency

#define I2C_CCR_FS          0x8000   //!< Fast/standard mode select
#define I2C_CCR_DUTY        0x4000   //!< Fast mode duty cycle
#define I2C_CCR_CCR         0x01FF   //!< Clock control register

#define I2C_OAR1_ADDMODE    0x8000   //!< Addressing mode (7/10 bit)

#define I2C_DIR_READ        0x01     //!< Slave address READ bit
#define I2C_DIR_WRITE       0x00     //!< Slave address WRITE bit

#define I2C_ADDR_ADDRESS11  0xF0     //!< Header pattern for 11 bit mode
#define I2C_MASK_BROADCAST  0xFE     //!< Header mask for 11 bit mode
#define I2C_ADDR_BROADCAST  0x00     //!< Header pattern for general call
#define I2C_MASK_ADDRESS11  0xF8     //!< Header mask for general call

//! DMA mode parameters for I2C data transfers
#define I2C_DMA_MODE        DMA_DEV_WIDTH8|DMA_MEM_WIDTH8|DMA_MODE_INCREMENT


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! I2C bus transmission states
typedef enum {
    I2C_STATE_IDLE,         //!< Bus free (inactive)
    I2C_STATE_START,        //!< Start transaction phase
    I2C_STATE_HEADER,       //!< Header transmission phase
    I2C_STATE_ADDRESS,      //!< Address transmission phase
    I2C_STATE_DATA,         //!< Data transmission phase
    I2C_STATE_STOP          //!< Stop transaction phase
} I2cState_t;

//! Layout of processor internal I2C interface registers
typedef volatile struct {
    UInt32 CR1;             //!< I2C Control register 1
    UInt32 CR2;             //!< I2C Control register 2
    UInt32 OAR1;            //!< I2C Own address register 1
    UInt32 OAR2;            //!< I2C Own address register 2
    UInt32 DR;              //!< I2C Data register
    UInt32 SR1;             //!< I2C Status register 1
    UInt32 SR2;             //!< I2C Status register 2
    UInt32 CCR;             //!< I2C Clock control register
    UInt32 TRISE;           //!< I2C TRISE register
} I2cRegFile_t;

//! Structure to hold the queue of jobs (one per interface)
typedef volatile struct {
    UInt16 Size;            //!< Size of queue (max. number of elements)
    UInt16 Count;           //!< Number of elements in queue
    UInt16 NextIn;          //!< Index to next free position
    UInt16 NextOut;         //!< Index to next-out message
    I2cJob_t *Jobs;         //!< Pointer to job queue
} I2cQueue_t;

//! Structure to hold the state of a I2C interface
typedef struct {
    I2cRegFile_t *I2C;      //!< Pointer to hardware registers
    UInt16 PeripheralID;    //!< Peripheral identifier
    UInt16 InterruptID;     //!< Interrupt identifier
    UInt16 DmaNo;           //!< DMA channel number
    UInt16 Flags;           //!< Flag bits
    UInt16 UseCount;        //!< Usage counter
    UInt16 Members;         //!< Max. number of bus members (slaves)
    UInt16 OpenFlag;        //!< Slave open bit field
    I2cJob_t *Job;          //!< Pointer to actual job
    I2cState_t State;       //!< I2C interface state
    I2cQueue_t Queue;       //!< Queue of jobs
    I2cCallback_t *Notifier; //!< Callback pointer array
} I2cDevice_t;
            
//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Data table holding the constants for all I2C busses
static I2cDevice_t DataTable[] = {
    { (I2cRegFile_t*) 0x40005400, PERIPHERAL_I2C1, INTERRUPT_I2C1_EVENT, 5 },
    { (I2cRegFile_t*) 0x40005800, PERIPHERAL_I2C2, INTERRUPT_I2C2_EVENT, 3 }
};

static UInt16 JobCount = 0;  //!< Job counter (for JobID generation)


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t halI2cFindDevice   (Device_t DeviceID, UInt8 SlaveNo);
static Error_t halI2cGetIndex     (Handle_t Handle);
static Error_t halI2cProtocol     (I2cDevice_t *Data);
static Error_t halI2cNotifyCaller (I2cJob_t *Job, UInt16 ErrFlags);

static I2cJob_t* halI2cPutJob (I2cDevice_t *Data, I2cJob_t *NewJob);
static I2cJob_t* halI2cGetJob (I2cDevice_t *Data);



/*****************************************************************************/
/*!
 *  \brief   Open a I2C bus channel
 *
 *      Opens a channel on the I2C bus specified by DeviceID. Channel is the
 *      number of the channel to open. Each slave device on the I2C bus has
 *      its own associated channel. Callback specifies the user callback
 *      function to be called when a transmission (read or write) has been
 *      finished. The callback function receives the job ID and a status
 *      value indicating if the transmission succeeded or failed for some
 *      reason. The job ID identifies a certain transfer job and is returned 
 *      by the read/write functions. It allows to identify the related 
 *      transaction in cases where more than one transfer is outstanding.
 *
 *  \iparam  DeviceID = I2C bus device ID
 *  \iparam  Channel  = Channel number (slave number)
 *  \iparam  Callback = Callback function pointer 
 *
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

Error_t halI2cOpen (Device_t DeviceID, UInt8 Channel, I2cCallback_t Callback) {

    const Int32 Index = halI2cFindDevice(DeviceID, Channel);
    Error_t Status;

    if (Index >= 0) {
        UInt32   UnitNo = halSerialDescriptors[Index].UnitNo;
        Handle_t Handle = DEVICE_CLASS_I2C + (UnitNo << 4) + Channel;

        DataTable[UnitNo].OpenFlag |= BIT(Channel);

        if (!DataTable[UnitNo].UseCount++) {
            // Setup I2C data structure
            DataTable[UnitNo].State  = I2C_STATE_START;
            DataTable[UnitNo].Flags |= HAL_OPEN_RW;
            DataTable[UnitNo].Job    = NULL;

            // Clear I2C job queue
            DataTable[UnitNo].Queue.NextIn  =
            DataTable[UnitNo].Queue.NextOut =
            DataTable[UnitNo].Queue.Count   = 0;

            // Setup I2C data rate and timing
            Status = halI2cSetup (Handle, halSerialDescriptors[Index].Baudrate, 0);
            if (Status != NO_ERROR) {
                halI2cClose(Handle);
                return (Status);
            }
        }
        DataTable[UnitNo].Notifier[Channel] = Callback;
        return (Handle);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Closes an I2C bus channel
 *
 *      Closes an open I2C bus channel. Since several channels may be opened
 *      on the bus at the same time, this function must be called for each
 *      channel separately. The bus itself is disabled not until the last 
 *      channel is closed.
 *
 *      If there is still any jobs in the queue the function waits until all
 *      jobs are done, or until a timeout exhausted. In case of a timeout
 *      an error is returned and the interface stays open.
 *
 *  \iparam  Handle = Handle of I2C bus channel
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halI2cClose (Handle_t Handle) {

    const Int32 Index = halI2cGetIndex(Handle);

    if (Index >= 0) {
        I2cDevice_t *Data = &DataTable[Index];

        if (~Data->OpenFlag & BIT(Handle & 0x0F)) {
            return (E_DEVICE_NOT_OPEN);                           
        }
        Data->OpenFlag &= ~BIT(Handle & 0x0F);

        if (--Data->UseCount == 0) {
            // Wait until all pending jobs are done
            if (halI2cWait (Handle, 1000) < 0) {
                return (E_BUS_JOB_TIMEOUT);
            }
            Data->Flags &= ~HAL_OPEN_RW;
            Data->I2C->CR1 &= ~I2C_CR1_PE;
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Exchange data with I2C slave
 *
 *      Exchange data with the slave identified by Handle on the I2C bus. 
 *      Buffer must point to the data array. Count gives the number of bytes
 *      to exchange. The transaction is performed asynchronously. That means, 
 *      that the function returns immediately. After data is exchanged the 
 *      registered callback function is called to notify the caller that the
 *      buffer can be released now. 
 * 
 *      A job ID is returned, to allow the callback function to identify
 *      which job has finished in cases where more than one transaction
 *      is outstanding. The same JobID is passed to the user callback
 *      to archieve this.
 *
 *  \iparam  Handle    = Handle of I2C bus channel
 *  \iparam  Address   = I2C header (slave address plus direction)
 *  \iparam  Buffer    = Pointer to data buffer
 *  \iparam  Count     = Number of bytes to read
 *  \iparam  Direction = Transfer direction (TRUE: read, FALSE: write)
 *
 *  \return  Job identifier or (negative) error code
 *
 ****************************************************************************/

Error_t halI2cExchange (Handle_t Handle, 
          UInt32 Address, UInt8 *Buffer, UInt16 Count, Bool Direction) {

    const Int32 Index = halI2cGetIndex(Handle);
    I2cJob_t NewJob;

    if (Index >= 0) {
        UInt32 AddrLen = halGetHighestBitSet(Address) / 8;

        if (!Address) {                                      
            return (E_BUS_INVALID_ADDRESS);
        }
        NewJob.Header = (Address >> (AddrLen * 8)) << 1;
        if (Direction) {
            NewJob.Header |= I2C_DIR_READ;
        }
        NewJob.Address = Address & ~(MAX_UINT32 << (AddrLen * 8));
        NewJob.AddrLen = AddrLen;
        NewJob.Buffer  = Buffer;
        NewJob.Count   = Count;
        NewJob.Notify  = DataTable[Index].Notifier[Handle & 0x0F];
        NewJob.JobID   = ++JobCount;

        // Sort out general call and 11 bit header pattern
        if ((NewJob.Header & I2C_MASK_BROADCAST) == I2C_ADDR_BROADCAST || 
            (NewJob.Header & I2C_MASK_ADDRESS11) == I2C_ADDR_ADDRESS11) {
            return (E_BUS_INVALID_ADDRESS);
        }
        if (halI2cPutJob (&DataTable[Index], &NewJob) == NULL) {
            return (E_DEVICE_BUSY);
        }
        if (~DataTable[Index].I2C->CR2 & I2C_CR2_ITEVTEN) {
            DataTable[Index].I2C->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN;
            halInterruptTrigger (DataTable[Index].InterruptID);
        }
        return (JobCount);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Wait until all jobs done
 *
 *      Wait until all queued transactions are processed or the given 
 *      timeout expires. If Timeout is 0, wait time is unlimited.
 *
 *  \iparam  Handle  = Handle of I2C bus channel
 *  \iparam  Timeout = Timeout [ms]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halI2cWait (Handle_t Handle, UInt32 Timeout) {

    const Int32 Index = halI2cGetIndex(Handle);

    if (Index >= 0) {
        const UInt32 StartTime = halSysTickRead();

        while (DataTable[Index].Job != NULL) {
            if (Timeout) {
                if (halSysTickRead() - StartTime > Timeout) {
                    return (E_BUS_JOB_TIMEOUT);
                }
            }
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Setup I2C interface
 *
 *      Configures the I2C interface associated with Handle to the given
 *      Baudrate. If Baudrate is above 100kHz, I2C Fast Mode is used, if
 *      below 100kHz I2C Standard Mode is used. If the requested baudrate
 *      can't be setup exacly and differs more than 4% from nominal, an
 *      error is returned. RiseTime must be given in ns or can be set to
 *      zero for automatic rise time calculation. If any jobs are pending
 *      halI2cSetup waits until all jobs are done before changing the
 *      bus setup. If the jobs are not finished in a certain time, an
 *      error is returned.
 *
 *      The new setting of the I2C interface is applied not only to the
 *      I2C channel associated with Handle, but to all channels of that
 *      interface/bus. Normally there is no reason to call this function
 *      since these parameters are initialized automatically using the
 *      settings from the HAL configuration file.
 *
 *  \iparam  Handle   = Handle of I2C bus channel
 *  \iparam  Baudrate = Data rate [Hz]
 *  \iparam  RiseTime = Rise time [ns]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halI2cSetup (Handle_t Handle, UInt32 Baudrate, UInt32 RiseTime) {

    const Int32 Index = halI2cGetIndex(Handle);
    UInt32 Frequency;
    UInt32 Division;
    UInt32 RealRate;
    UInt32 Quantas;

    if (Index >= 0) {  
        I2cRegFile_t *I2C = DataTable[Index].I2C;

        if (I2C->SR2 & I2C_SR2_BUSY) {
            return (E_BUS_BUSY);
        }

        if (Baudrate > MAX_RATE_FASTMODE || Baudrate < MIN_RATE_STDMODE) {
            return (E_BUS_ILLEGAL_BAUDRATE);
        }
        Frequency = halGetPeripheralClockRate() / 1000000;
        if (Frequency < 2 || Frequency > 36) {
            return (E_BUS_ILLEGAL_CLOCKRATE);
        }
        // Wait until all pending jobs are done
        if (halI2cWait (Handle, 1000) < 0) {
            return (E_BUS_JOB_TIMEOUT);
        }
        Quantas  = (Baudrate > MAX_RATE_STDMODE) ? 3 : 2;
        Division = halGetPeripheralClockRate() / (Baudrate * Quantas);
        RealRate = halGetPeripheralClockRate() / (Division * Quantas);

        // Return error if baudrate differs more than 5% from nominal
        if (labs (RealRate-Baudrate) > (long)(Baudrate/20)) {
            return (E_BUS_INEXACT_BAUDRATE);
        }
        if (!RiseTime) {
            RiseTime = (Baudrate > MAX_RATE_STDMODE) ? 300 : 1000;
        }
        I2C->CR1 &= ~I2C_CR1_PE;
        I2C->CR2  = Frequency & I2C_CR2_FREQ;
        if (Baudrate > MAX_RATE_STDMODE) {
            I2C->CCR = Division | I2C_CCR_FS;
        }
        else {
            I2C->CCR = Division;
        }
        I2C->TRISE = (Frequency * RiseTime / 1000) + 1;
        I2C->CR1 = I2C_CR1_PE;

        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   I2C protocol state maschine
 *
 *      This function handles the I2C bus protocol and is called each time
 *      an I2C event interrupt arised. It is implemented as state maschine
 *      with the states HEADER (sending the transaction header), ADDRESS
 *      (sending the slave word address), STOP (waiting for the data to
 *      be transfered by DMA), then stopping the transaction, and START
 *      (getting the next transaction from the job queue and starting the
 *      bus transfer). When a transaction has ended the user callback
 *      notification function is called.
 *
 *  \iparam  Device = I2C device data pointer
 *
 *  \return  Nothing
 *
 ****************************************************************************/

static Error_t halI2cProtocol (I2cDevice_t *Device) {                

    I2cRegFile_t *I2C = Device->I2C;
    I2cJob_t *Job = Device->Job;

    //------------------------------------------------------------
    if (Device->State == I2C_STATE_HEADER) {
    //------------------------------------------------------------
        // Transmit slave device address
        UInt32 SR1 = I2C->SR1;
        if (SR1 & I2C_SR1_SB) {
            if (Job->AddrLen) {
                I2C->DR = Job->Header & ~I2C_DIR_READ; 
            }
            else {
                I2C->DR = Job->Header;
            }
        }
        else {
            if (SR1 & I2C_SR1_ADDR) {
                if (Job->AddrLen) {
                    I2C->CR2 |= I2C_CR2_ITBUFEN;
                    Device->State = I2C_STATE_ADDRESS;
                }
                else {
                    Device->State = I2C_STATE_DATA;
                }
                I2C->SR2 = I2C->SR2;
            }
            else {
                if (Job->AddrLen) {
                    I2C->CR2 |= I2C_CR2_ITBUFEN;
                    Device->State = I2C_STATE_ADDRESS;
                }
//                else {
//                    if (!(SR1 & I2C_SR1_TxE)) {
//                        I2C->CR2 |= I2C_CR2_ITBUFEN;
//                        Device->State = I2C_STATE_DATA;
//                    }
//                }
            }
        }
    }
    //------------------------------------------------------------
    if (Device->State == I2C_STATE_ADDRESS) {
    //------------------------------------------------------------
        // Transmit extended slave address 
        if ((I2C->SR1 & I2C_SR1_TxE)) {
            if (Job->AddrLen) {  
                I2C->DR = Job->Address >> (--Job->AddrLen * 8);
            }
        }
        if (!Job->AddrLen) {  
            if (Job->Header & I2C_DIR_READ) {
                I2C->CR1 |= I2C_CR1_START; 
                Device->State = I2C_STATE_HEADER;                
            }
            else {
                Device->State = I2C_STATE_DATA;
            }
            I2C->CR2 &= ~I2C_CR2_ITBUFEN;
        }
    }
    //------------------------------------------------------------
    if (Device->State == I2C_STATE_DATA) {
    //------------------------------------------------------------
        // Initiate data transfer using DMA
        if (Job->Count) {
            if (Job->Count == 1) {
                I2C->CR1 &= ~I2C_CR1_ACKN;                  
            }
            if (Job->Header & I2C_DIR_READ) {
                halDmaRead (Device->DmaNo+1, Job->Buffer, Job->Count);
                if (Job->Count == 1) {
                    I2C->CR1 |= I2C_CR1_STOP; 
                }
            }
            else {
                halDmaWrite (Device->DmaNo, Job->Buffer, Job->Count);
            }
            I2C->CR2 |= I2C_CR2_DMAEN | I2C_CR2_LAST;
        }
        I2C->CR2 &= ~I2C_CR2_ITBUFEN;
        Device->State = I2C_STATE_STOP;
    }
    //------------------------------------------------------------
    if (Device->State == I2C_STATE_STOP) {
    //------------------------------------------------------------
        // Stop when data transmission done
        if (!Job->Count) {
            if ((I2C->SR1 & (I2C_SR1_BTF | I2C_SR1_ERRORS)) || 
               (~I2C->SR2 &  I2C_SR2_TRA)) {                 

                I2C->CR1 |= I2C_CR1_STOP; 
                if (Job->Header & I2C_DIR_READ) {
                    halDmaCancel (Device->DmaNo+1);
                }
                else {
                    halDmaCancel (Device->DmaNo);
                }
                halI2cNotifyCaller (Job, I2C->SR1);                                    
                while (I2C->CR1 & I2C_CR1_STOP);
                Device->State = I2C_STATE_START;
                Device->Job = NULL;
            }
            I2C->CR2 &= ~(I2C_CR2_DMAEN | I2C_CR2_LAST);
        }
    }
    //------------------------------------------------------------
    if (Device->State == I2C_STATE_START) {
    //------------------------------------------------------------
        // Get next job from queue and start protocol
        if (Device->Job == NULL) {
            Job = halI2cGetJob (Device);
            if (Job == NULL) {
                Device->I2C->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);
            }
            Device->Job = Job;
        }
        if (Device->Job != NULL) {
            I2C->SR1 &= ~I2C_SR1_ERRORS;
            I2C->CR1 |= I2C_CR1_ACKN | I2C_CR1_START; 
            Device->State = I2C_STATE_HEADER;
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   I2C event interrupt handler
 *
 *      This function will be called by the vectored interrupt controller
 *      in case of an I2C interrupt. Channel is the I2C interface number.
 *      It checks the I2C interface for errors and calls the I2C state
 *      maschine. In case of a I2C bus error, which might be a negative
 *      acknowledge, an arbitration loss, or a bus error - the transaction
 *      is stopped.
 *
 *  \iparam  Channel = I2C channel number
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void halI2cInterruptHandler (UInt32 Channel) {

    I2cDevice_t *Data = &DataTable[Channel];

    if (Channel < ELEMENTS(DataTable)) {
        // Stop transaction in case of I2C bus error
        if (Data->I2C->SR1 & I2C_SR1_ERRORS) {
            if (Data->Job != NULL) {
                Data->Job->Count = 0;
            }
            Data->State = I2C_STATE_STOP;
        }
        halI2cProtocol(Data);    
    }
}


/*****************************************************************************/
/*!
 *  \brief   I2C DMA interrupt handler
 *
 *      This function will be called by the vectored interrupt controller
 *      in case of a DMA terminal count interrupt, which indicates that the
 *      pending DMA transfer has finished. Channel is the I2C bus number. 
 *      When the DMA has finished the transfer, the count member of the
 *      actual transaction is set to 0 and the I2C state maschine called.
 *      The state maschine evaluates the count variable and recognizes
 *      that the data transfer phase has ended.
 *
 *  \iparam  Channel   = I2C channel number (UserTag)
 *  \iparam  IntrFlags = DMA interrupt flag bits
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void halI2cInterruptXferDone (UInt32 Channel, UInt32 IntrFlags) {

    if (IntrFlags & DMA_INTR_COMPLETE) {
        if (DataTable[Channel].Job != NULL) {
            DataTable[Channel].Job->Count = 0;
        }
        halI2cProtocol(&DataTable[Channel]);
    }
}


/*****************************************************************************/
/*!
 *  \brief   Call user notification function
 *
 *      Each time an I2C transaction terminates, this function is called. 
 *      It analyzes the I2C status register to see if the transaction
 *      succeeded or failed for some reason and then calls the user
 *      notification function registered when opening the channel. The
 *      determined transaction status and the JobID is passed to the user
 *      notification function as parameters. This allows the callback to
 *      find out which job has terminated and if the transaction was 
 *      successful or failed. The passed status is NO_ERROR in case of
 *      successful termination, or an error code in case of failure.
 *
 *  \iparam  Job      = Job decriptor
 *  \iparam  ErrFlags = I2C bus error bits
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halI2cNotifyCaller (I2cJob_t *Job, UInt16 ErrFlags) {

    Error_t Status = NO_ERROR;

    if (Job != NULL) {
        if (Job->Notify != NULL) {

            if (ErrFlags & I2C_SR1_ERRORS) {
                if (ErrFlags & I2C_SR1_BERR) {
                    Status = E_BUS_ERROR;
                }
                else if (ErrFlags & I2C_SR1_AF) {
                    Status = E_BUS_ACKNOWLEDGE;
                }
                else if (ErrFlags & I2C_SR1_ARLO) {
                    Status = E_BUS_ARBITRATION;
                }
                else {
                    Status = E_BUS_UNSPECIFIED;
                }
            }
            Job->Notify (Job->JobID, Status);
        }
    }
    return (Status);
}


/*****************************************************************************/
/*!
 *  \brief   Get next job from queue
 *
 *      Returns the next job from the job queue or NULL if no more jobs 
 *      are in the queue. 
 *
 *  \iparam  Device = I2C device data 
 *
 *  \return  Pointer to next job or NULL
 *
 ****************************************************************************/

static I2cJob_t* halI2cGetJob (I2cDevice_t *Device) {

    I2cQueue_t *Queue = &Device->Queue; 

    if (Queue->Count) {
        I2cJob_t *Job = &Queue->Jobs[Queue->NextOut];

        if (++Queue->NextOut >= Queue->Size) {
            Queue->NextOut = 0;
        }
        halGlobalInterruptDisable();
        Queue->Count--;
        halGlobalInterruptEnable();

        return (Job);
    }
    return (NULL); // no more jobs in queue
}


/*****************************************************************************/
/*!
 *  \brief   Put new job into queue
 *
 *      Inserts new job at the end of the job queue and returns a pointer
 *      to the job or NULL if the queue is full.
 *
 *  \iparam  Device = I2C device data 
 *  \iparam  NewJob = Job to enqueue
 *
 *  \return  Pointer to new job or NULL
 *
 ****************************************************************************/

static I2cJob_t* halI2cPutJob (I2cDevice_t *Device, I2cJob_t *NewJob) {

    I2cQueue_t *Queue = &Device->Queue;

    while (Queue->Count >= Queue->Size-1) {}

    if (Queue->Count < Queue->Size) {
        Queue->Jobs[Queue->NextIn] = *NewJob;

        if (++Queue->NextIn >= Queue->Size) {
            Queue->NextIn = 0;
        }
        halGlobalInterruptDisable();
        Queue->Count++;
        halGlobalInterruptEnable();

        return (NewJob);
    }
    return (NULL);  // no free entries in queue
}


/*****************************************************************************/
/*!
 *  \brief   Get index of I2C interface
 *
 *      Returns the index of the I2C interface associated with Handle. The
 *      index is used to address an entry in the Devices table, containing
 *      all variables of that interface. If Handle is invalid or the device
 *      not opened, an error is returned.
 *
 *  \iparam  Handle = Handle of CAN controller
 *
 *  \return  I2C index or (negative) error code
 *
 ****************************************************************************/

static Error_t halI2cGetIndex (Handle_t Handle) {

    register const UInt32 Index = (Handle ^ DEVICE_CLASS_I2C) >> 4;

    if (Index < ELEMENTS(DataTable)) {

        const UInt32 Channel = Handle & 0x0F;

        if (Channel > DataTable[Index].Members) {
            return (E_BUS_MEMBER_INVALID);
        }
        if (~DataTable[Index].OpenFlag & BIT(Channel)) {
            return (E_DEVICE_NOT_OPEN);
        }
        return ((Error_t) Index);
    }
    return (E_DEVICE_HANDLE_INVALID);
}


/*****************************************************************************/
/*!
 *  \brief   Find logical I2C bus
 *
 *      Searches the I2C descriptor table for the asked DeviceID. If found,
 *      the index of the entry into the descriptor table is returned. If
 *      the DeviceID is not found, an error is returned. An error is also 
 *      returned if the Channel number is greater than the number ob bus 
 *      members defined in the descriptor or the module is not initialized.
 *
 *  \iparam  DeviceID = Logical I2C bus device ID
 *  \iparam  Channel  = I2C channel number (slave)
 *
 *  \return  Channel index or (negative) error code
 *
 ****************************************************************************/

static Error_t halI2cFindDevice (Device_t DeviceID, UInt8 Channel) {

    UInt32 Index;

    if ((DeviceID & DEVICE_CLASS_MASK) == DEVICE_CLASS_I2C) {

        halInitState_t InitState = halGetInitState();

        if (InitState == INIT_NOT_DONE || InitState == INIT_FAILED) {
            return (E_HAL_NOT_INITIALIZED);
        }
        for (Index=0; Index < halSerialDescriptorCount; Index++) {

            if (halSerialDescriptors[Index].DeviceID == DeviceID) {
                const UInt32 UnitNo = halSerialDescriptors[Index].UnitNo;

                if (UnitNo >= ELEMENTS(DataTable)) {
                    return (E_PORT_NOT_EXISTS);
                }
                if (Channel > halSerialDescriptors[Index].Members) {
                    return (E_BUS_MEMBER_INVALID);
                }
                if (~DataTable[UnitNo].Flags & HAL_FLAG_INITZED) {
                    return (E_DEVICE_UNCONFIGURED);
                }
                if (DataTable[UnitNo].OpenFlag & BIT(Channel)) {
                    return (E_DEVICE_ALREADY_OPEN);                                            
                }
                return (Index);
            }
        }
    }
    return (E_UNKNOWN_DEVICE_ID);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize I2C interfaces
 *
 *      Initializes the processor's on-chip I2C interfaces based on the
 *      parameters in the global halBusDescriptor table defined in the
 *      HAL configuration file. If the descriptor table contains invalid
 *      parameters, an error is returned. The Job queues are allocated
 *      and also the array of user callbacks. Beside this, the 2 DMA
 *      controller channels (1 for receive, 1 for transmission) are
 *      initialized and the DMA interrupt handler registered.
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halI2cInit (void) {

    UInt32 SupportedI2Cs = MIN(halProcessorInfo.CountI2C, ELEMENTS(DataTable));
    Error_t Status;
    UInt32 i;

    if (halGetInitState() == INIT_IN_PROGRESS) {
        halPeripheralClockEnable (PERIPHERAL_DMA1, ON);
    
        for (i=0; i < halSerialDescriptorCount; i++) {
            const UInt32 UnitNo  = halSerialDescriptors[i].UnitNo;
    
            if (halSerialDescriptors[i].BusType == SIO_TYPE_I2C) {
                I2cDevice_t *Data = &DataTable[UnitNo];
    
                // Check if port number is legal
                if (UnitNo >= SupportedI2Cs) {
                    return (E_PORT_NOT_EXISTS);
                }
                // Check if interface already initialized
                if (Data->Flags & HAL_FLAG_INITZED) {
                    return (E_BUS_ALLOCATED_TWICE);
                }
                //Allocate storage for the bus member callbacks
                Data->Members  = halSerialDescriptors[i].Members;
                Data->Notifier = calloc (Data->Members, sizeof(I2cCallback_t));
                if (Data->Notifier == NULL) {
                    return (E_HEAP_MEMORY_FULL);
                }
                // Allocate storage for job queue
                Data->Queue.Size = I2C_FIFO_SIZE + 1;
                Data->Queue.Jobs = calloc (Data->Queue.Size, sizeof(I2cJob_t));
                if (Data->Queue.Jobs == NULL) {
                    return (E_HEAP_MEMORY_FULL);
                }
                // Setup DMA for I2C transmission
                if ((Status = halDmaOpen (
                       Data->DmaNo, UnitNo, halI2cInterruptXferDone)) < 0) {
                    return (Status);
                }
                Status = halDmaSetup (Data->DmaNo, &Data->I2C->DR, I2C_DMA_MODE);
                if (Status != NO_ERROR) {
                    return (Status);
                }          
                halDmaControl (Data->DmaNo, DMA_INTR_COMPLETE, ON);
    
                // Setup DMA for I2C reception
                if ((Status = halDmaOpen (
                       Data->DmaNo+1, UnitNo, halI2cInterruptXferDone)) < 0) {
                    return (Status);
                }
                Status = halDmaSetup (Data->DmaNo+1, &Data->I2C->DR, I2C_DMA_MODE);
                if (Status != NO_ERROR) {
                    return (Status);
                }
                halDmaControl (Data->DmaNo+1, DMA_INTR_COMPLETE, ON);
    
                // Enable clocks and interrupts
                halInterruptEnable (Data->InterruptID+0, IRQ_PRIO_DEFAULT);
                halInterruptEnable (Data->InterruptID+1, IRQ_PRIO_DEFAULT);
                halPeripheralClockEnable (Data->PeripheralID, ON);

                Data->Flags = HAL_FLAG_INITZED;
            }
        }
    }
    return (NO_ERROR);
}

//****************************************************************************/
