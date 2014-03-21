/****************************************************************************/
/*! \file halSpiBus.c
 * 
 *  \brief  SPI bus interface
 *
 *  $Version: $ 0.1
 *  $Date:    $ 02.07.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage the on-chip SPI
 *      interfaces. Since SPI is a bus, where multiple bus members (slaves) 
 *      can be connected to, each interface can be opened several times - 
 *      once for each bus member. The transfer to or from a certain slave 
 *      is done using synchronous read and write functions, using the slave 
 *      handle returned by the open function. Synchronously means, that the 
 *      function returns not until the transaction is complete. Interrupts
 *      are not used in this implementation, but are prepared to be used
 *      in the future.
 *
 *      Attention: The stepper motors also use an SPI interface. Because
 *      of the very tight timing requirements, the stepper motor module
 *      directly accesses the SPI hardware instead of using this module.
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
#include "halCommon.h"
#include "halError.h"
#include "halClocks.h"
#include "halMain.h"
#include "halInterrupt.h"
#include "halPorts.h"
#include "halSpiBus.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define SPI_CR1_BIDIMODE    0x08000   //!< Bidirectional data mode enable
#define SPI_CR1_BIDIOE      0x04000   //!< Output enable in bidi mode
#define SPI_CR1_CRCEN       0x02000   //!< Hardware CRC calculation enable
#define SPI_CR1_CRCNEXT     0x01000   //!< Transmit CRC next
#define SPI_CR1_DFF         0x00800   //!< Data frame format (8/16 bits)
#define SPI_CR1_RXONLY      0x00400   //!< Receive only
#define SPI_CR1_SSM         0x00200   //!< Software slave management
#define SPI_CR1_SSI         0x00100   //!< Internal slave select
#define SPI_CR1_LSBFIRST    0x00080   //!< Frame format (LSB/MSB first)
#define SPI_CR1_SPE         0x00040   //!< SPI enable
#define SPI_CR1_BR          0x00038   //!< Baud rate control bitmask
#define SPI_CR1_MSTR        0x00004   //!< Master selection
#define SPI_CR1_CPOL        0x00002   //!< Clock polarity
#define SPI_CR1_CPHA        0x00001   //!< Clock phase

#define SPI_CR2_TXEIE       0x00080   //!< Tx buffer empty IRQ enable
#define SPI_CR2_RXNEIE      0x00040   //!< RX buffer not empty IRQ enable
#define SPI_CR2_ERRIE       0x00020   //!< Error IRQ enable
#define SPI_CR2_SSOE        0x00004   //!< Slave select output enable
#define SPI_CR2_TXDMAEN     0x00002   //!< Tx DMA enable
#define SPI_CR2_RXDMAEN     0x00001   //!< Rx DMA enable

#define SPI_SR_BSY          0x00080   //!< SPI busy flag
#define SPI_SR_OVR          0x00040   //!< Overrun flag
#define SPI_SR_MODF         0x00020   //!< Mode fault
#define SPI_SR_CRCERR       0x00010   //!< CRC error flag
#define SPI_SR_TXE          0x00002   //!< Transmit buffer empty
#define SPI_SR_RXNE         0x00001   //!< Receive buffer not empty
#define SPI_SR_ERROR        0x00070   //!< Error bits mask

#define SPI_BR_DIVIDE2      0x00000   //!< SPI clock divider = 2
#define SPI_BR_DIVIDE4      0x00008   //!< SPI clock divider = 4
#define SPI_BR_DIVIDE8      0x00010   //!< SPI clock divider = 8


//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

//! Register layout of processor internal SPI controllers
typedef volatile struct {
    UInt32 CR1;             //!< SPI control register 1
    UInt32 CR2;             //!< SPI control register 2
    UInt32 SR;              //!< SPI status register
    UInt32 DR;              //!< SPI data register
    UInt32 CRCPR;           //!< SPI CRC polynomial register
    UInt32 RXCRCR;          //!< SPI Rx CRC register
    UInt32 TXCRCR;          //!< SPI Tx CRC register
    UInt32 I2SCFGR;         //!< I2S configuration register
    UInt32 I2SPR;           //!< I2S prescaler register
} halSpiRegs_t;

//! Structure to hold the parameters of a SPI device
typedef struct {
    halSpiRegs_t *SPI;      //!< Pointer to register file
    UInt16 PeripheralID;    //!< Peripheral identifier
    UInt16 InterruptID;     //!< Interrupt identifier
} halSpiParams_t;

//! Structure to hold the state of a SPI slave device
typedef struct {
    UInt16  SpiNo;          //!< SPI bus number
    Handle_t HandleCS;      //!< SPI CS port handle
    UInt32 *PortCS;         //!< SPI CS port register pointer
    UInt32  MaskCS;         //!< SPI CS pin bitmask
    UInt16  Flags;          //!< Flag bits
    UInt16  Mode;           //!< Mode setting
} halSpiData_t;

//****************************************************************************/
// Private Variables 
//****************************************************************************/

//! Table holding the constant parameters of all SPI controllers
static const halSpiParams_t SpiParams[] = {
    { (halSpiRegs_t*) 0x40013000, PERIPHERAL_SPI1, INTERRUPT_SPI1 },
    { (halSpiRegs_t*) 0x40003800, PERIPHERAL_SPI2, INTERRUPT_SPI2 },
    { (halSpiRegs_t*) 0x40003C00, PERIPHERAL_SPI3, INTERRUPT_SPI3 }
};

static halSpiData_t *DataTable = NULL; //!< State of all SPI slave devices

static UInt16 LockBitMask = 0;         //!< Locking bitmask
static UInt16 SlaveCount  = 0;         //!< Number of slave devices


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

UInt32* halPortAddress (Handle_t Handle, UInt32 *Bitmask);

static Error_t halSpiFindDevice (UInt32 *Index, Device_t DeviceID);
static Error_t halSpiGetIndex   (Handle_t Handle);
static Error_t halSpiLock       (Handle_t Handle, Bool State);


/*****************************************************************************/
/*!
 *  \brief   Open a SPI bus channel
 *
 *      Opens the slave device on the SPI bus specified by DeviceID. Several
 *      slave devices can be connected to a single SPI bus. DeviceID is the 
 *      identifier of the slave device, not the ID of the SPI interface. The
 *      SPI interface and the associated slave select line is taken from 
 *      the HAL configuration file.
 *
 *      The SPI channel is configured to the defaults defined in the HAL
 *      configuration file. Each slave device can use it's own settings,
 *      and multiple slaves on the same bus can use different settings.
 *      The default mode and baudrate can be changed using halSpiSetup 
 *      on a per-slave base.
 *
 *      Handler must be a pointer to an interrupt handler function to be 
 *      called when the device signals an interrupt. It can be NULL if no
 *      interrupts are used. UserTag is a user defined number to be passed 
 *      to the handler function, e.g. as index into a data table in case 
 *      of multiple instances. 
 *
 *  \iparam  DeviceID = SPI slave device ID
 *  \iparam  UserTag  = User tag to pass thru to interrupt handler
 *  \iparam  Handler  = Interrupt handler function
 *
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

Error_t halSpiOpen (Device_t DeviceID, UInt32 UserTag, halIntrHandler_t Handler) {

    UInt32 Index = 0;   // Logical SPI device index
    Int32 SerialIndex;  // Index in the serial descriptor table
    Error_t Status;

    SerialIndex = halSpiFindDevice(&Index, DeviceID);

    if (SerialIndex >= 0) {       
        const Int32 Handle = DEVICE_CLASS_SPI + Index;

        if (DataTable[Index].Flags & HAL_OPEN_RW) {
            return (E_DEVICE_ALREADY_OPEN);
        }
        DataTable[Index].Flags |= HAL_OPEN_RW;

        // Initialize transmission format
        Status = halSpiSetup (Handle, halSerialDescriptors[SerialIndex].Baudrate, 0);
        if (Status != NO_ERROR) {
            halSpiClose(Handle);
            return (Status);
        }
        return (Handle);
    }
    return (SerialIndex);
}


/*****************************************************************************/
/*!
 *  \brief   Closes a SPI slave device
 *
 *      Closes the open slave device on the SPI bus specified by Handle.
 *
 *  \iparam  Handle = Handle of SPI slave device
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halSpiClose (Handle_t Handle) {

    const Int32 Index = halSpiGetIndex(Handle);

    if (Index >= 0) {
        DataTable[Index].Flags &= ~HAL_OPEN_RW;
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Transfer data to/from SPI slave device
 *
 *      Transfers the data pointed to by Buffer to the SPI bus slave device
 *      associated with Handle. Exactly Count bytes of data are transfered.
 *      Sending n bytes automatically leads to n bytes receive. That is a
 *      typically behaivor of SPI. The Count bytes received are copied into
 *      Buffer, overwriting the data to transmit. 
 *
 *      Before starting transmission the SPI bus is checked to be free. If
 *      this is not the case, an error is returned. After that, the SPI
 *      controller is set to the mode/baudrate programmed for the addressed
 *      SPI slave device and the slave chip select output is activated.
 *      Then the data transfer takes place. After all bytes are transfered
 *      the slave chip select output is deactivated and the function
 *      returns to the caller. 
 *
 *  \iparam  Handle = Handle of SPI slave device
 *  \iparam  Buffer = Buffer containing/receiving data
 *  \iparam  Count  = Number of bytes to transfer
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halSpiTransfer (Handle_t Handle, UInt8 *Buffer, UInt32 Count) {

    const Int32 Index = halSpiGetIndex(Handle);
    UInt32 RxCount = 0;
    UInt32 TxCount = 0;

    if (Index >= 0) {
        halSpiData_t *Data = &DataTable[Index];
        halSpiRegs_t *SPI  = SpiParams[Data->SpiNo].SPI;

        if (BIT(Data->SpiNo) & LockBitMask) {
            if (~Data->Flags & HAL_FLAG_LOCK) {
                return (E_DEVICE_LOCKED);
            }
        }
        while (SPI->SR & SPI_SR_BSY) {}

        SPI->CR1  = 0;
        SPI->CR2 |= SPI_CR2_SSOE;
        SPI->CR1  = (Data->Mode & ~SPI_CR1_SPE);
        SPI->CR1 |= SPI_CR1_SPE;

        // Activate slave select output
        if (Data->PortCS != NULL) {
            *Data->PortCS = Data->MaskCS << 16;
        }
        while (RxCount < Count) {
            if (TxCount < Count) {
                if (SPI->SR & SPI_SR_TXE) {
                    SPI->DR = Buffer[TxCount++];
                }
            }
            while (RxCount < TxCount) {
                if (SPI->SR & SPI_SR_RXNE) {
                    if (Data->Flags & HAL_OPEN_READ) {
                        Buffer[RxCount] = SPI->DR;
                    }
                    RxCount++;
                }
            }
        }
        // Deactivate slave select output
        if (Data->PortCS != NULL) {
            *Data->PortCS = Data->MaskCS;
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Set mode of SPI channel
 *
 *      Sets the SPI data frame format and bitrate for the SPI slave device
 *      associated with Handle. The following parameters can be set:
 *
 *      - Shift direction (MSB/LSB first)
 *      - Clock phase (1st/2nd transition)
 *      - Clock polarity (0/1 when idle)
 *      - Baudrate 
 *
 *      The STM32 supports only a limit number of different baudrates. If
 *      the requested baudrate can't be configured exactly, the next lower
 *      value is used instead. If the requested baudrate is lower than the
 *      smallest baudrate supported by the hardware, an error is returned.
 *      The hardware supports baudrates between 18MHz and 140kHz, assuming
 *      a system clock of 72MHz. If Baudrate is 0, the baudrate is left
 *      unchanged and only the transmission format is changes.
 *
 *      The mode registers of the SPI controller are not set here. Instead
 *      of this, the control register bits are set in a shadow register 
 *      and written into the control register on each data transfer. This
 *      permits to use different formats/baudrates for different slaves 
 *      on the same SPI bus.
 *
 *  \iparam  Handle   = Handle of SPI slave device
 *  \iparam  Baudrate = Baudrate [Hz]
 *  \iparam  Format   = Data format
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halSpiSetup (Handle_t Handle, UInt32 Baudrate, UInt32 Format) {

    const Int32 Index = halSpiGetIndex(Handle);

    if (Index >= 0) {
        UInt16 Rate = DataTable[Index].Mode & SPI_CR1_BR;
        UInt16 Mode = SPI_CR1_MSTR | SPI_CR1_SPE;
        
        if (Format & SPI_FORMAT_LSB1ST) {
            Mode |= SPI_CR1_LSBFIRST;
        }
        if (Format & SPI_FORMAT_CLK_PHASE2) {
            Mode |= SPI_CR1_CPHA;
        }
        if (Format & SPI_FORMAT_CLK_IDLE_1) {
            Mode |= SPI_CR1_CPOL;
        }
        if (Baudrate) {
            UInt32 Divider = halGetPeripheralClockRate() / Baudrate;
            UInt32 Control = 0;

            if (Divider < 2 || Divider > 256) {
                return (E_BUS_ILLEGAL_BAUDRATE);
            }
            while (Divider > (2U << Control)) {
                Control++;
            }
            Rate = (Control << 3) & SPI_CR1_BR;
        }
        DataTable[Index].Mode = Mode | Rate;
        return (NO_ERROR);
    }
    return (Index);
} 

 
/*****************************************************************************/
/*!
 *  \brief   Get status of SPI channel
 *
 *      Returns the status of the SPI slave device associated with Handle.
 *      What status to return depends on StatusID. The following infos
 *      can be requested:
 *
 *      - Lock status (bus exclusive use)
 *      - Chip select handle (for HAL internal use only)
 *
 *      If a unsupported StatusID is supplied, an error is returned.
 *
 *  \iparam  Handle   = Handle of SPI slave device
 *  \iparam  StatusID = Type of status requested
 *
 *  \return  Status value or (negative) error code
 *
 ****************************************************************************/

Error_t halSpiStatus (Handle_t Handle, SpiStatID_t StatusID) {

    const Int32 Index = halSpiGetIndex(Handle);

    if (Index >= 0) {
        switch (StatusID) {

            case SPI_STAT_LOCKED:
                return (LockBitMask & BIT(DataTable[Index].SpiNo));

            // For HAL internal use only (return CS handle)
            case SPI_STAT_CSPORT:
                return (DataTable[Index].HandleCS);

            // For HAL internal use only (return mode word)
            case SPI_STAT_MODE:
                return (DataTable[Index].Mode);
        }
        return (E_UNKNOWN_STATUS_ID);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Get status of SPI channel
 *
 *      Returns the status of the SPI slave device associated with Handle.
 *      What status to return depends on StatusID. The following infos
 *      can be requested:
 *
 *      - Lock status (bus exclusive use)
 *      - Chip select handle (for HAL internal use only)
 *
 *      If a unsupported StatusID is supplied, an error is returned.
 *
 *  \iparam  Handle    = Handle of SPI slave device
 *  \iparam  ControlID = Control type identifier
 *  \iparam  State     = Enable: True, Disable: False
 *
 *  \return  Status value or (negative) error code
 *
 ****************************************************************************/

Error_t halSpiControl (Handle_t Handle, SpiCtrlID_t ControlID, Bool State) {

    const Int32 Index = halSpiGetIndex(Handle);

    if (Index >= 0) {
        switch (ControlID) {

            case SPI_CTRL_LOCK:
                return (halSpiLock(Handle, State));
        }
        return (E_UNKNOWN_STATUS_ID);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Lock/Unlock SPI bus
 *
 *      Locks or unlocks the SPI bus associated with Handle for exclusive
 *      use. If State is TRUE the bus is locked, if State is FALSE the bus
 *      is unlocked. If trying to lock a SPI bus, that is already locked
 *      by another bus member, an error is returned. If trying to unlock
 *      a SPI bus without having the lock, an error is returned. 
 *
 *  \iparam  Handle = Handle of SPI slave device
 *  \iparam  State  = Lock state (TRUE: lock, FALSE: unlock)
 *
 *  \return  SPI device number or (negative) error code
 *
 ****************************************************************************/

static Error_t halSpiLock (Handle_t Handle, Bool State) {

    const Int32 Index = halSpiGetIndex(Handle);

    if (Index >= 0) {
        const UInt32 SpiNo = DataTable[Index].SpiNo;

        if (State) {
            if (LockBitMask & BIT(SpiNo)) {
                if (~DataTable[Index].Flags & HAL_FLAG_LOCK) {
                    return (E_DEVICE_LOCKED);
                }
                return (SpiNo);
            }
            DataTable[Index].Flags |= HAL_FLAG_LOCK;
            LockBitMask |= BIT(SpiNo);
        }
        else {
            if (~DataTable[Index].Flags & HAL_FLAG_LOCK) {
                if (LockBitMask & BIT(SpiNo)) {
                    return (E_DEVICE_LOCKED);
                }
                return (SpiNo);
            }
            DataTable[Index].Flags &= ~HAL_FLAG_LOCK;
            LockBitMask &= ~BIT(SpiNo);
        }
        return (SpiNo);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Find logical SPI controller
 *
 *      Searches the serial device descriptor for an entry with the asked
 *      DeviceID. If found, the index into the descrptor table is returned.
 *      If this module isn't initialized, the requested hardware doesn't
 *      exist or the device isn't initialized, an error is returned.
 *
 *  \oparam  Index = Logical SPI device number
 *  \iparam  DeviceID = Logical SPI device ID
 *
 *  \return  Descriptor table index or (negative) error code
 *
 ****************************************************************************/

static Error_t halSpiFindDevice (UInt32 *Index, Device_t DeviceID) {

    UInt32 i;
    UInt32 k;

    if ((DeviceID & DEVICE_CLASS_MASK) == DEVICE_CLASS_SPI) {

        halInitState_t InitState = halGetInitState();

        if (InitState == INIT_NOT_DONE || InitState == INIT_FAILED) {
            return (E_HAL_NOT_INITIALIZED);
        }
        for (i=0,k=0; i < halSerialDescriptorCount; i++) {
            if (halSerialDescriptors[i].BusType == SIO_TYPE_SPI) {
                if (halSerialDescriptors[i].DeviceID == DeviceID) {
    
                    UInt32 UnitNo = halSerialDescriptors[i].UnitNo;
    
                    if (UnitNo >= ELEMENTS(SpiParams)) { 
                       return (E_DEVICE_NOT_EXISTS);
                    }
                    if (~DataTable[k].Flags & HAL_FLAG_INITZED) {
                        return (E_DEVICE_UNCONFIGURED);
                    }
                    *Index = k;
                    return (i);
                }
                k++;
            }
        }
    }
    return (E_UNKNOWN_DEVICE_ID);
}


/*****************************************************************************/
/*!
 *  \brief   Get index of CAN controller
 *
 *      Returns the index of the CAN controller associated with Handle. 
 *      The index can be used to address an entry in the Devices table,
 *      which contains all variables of that controller. If the Handle
 *      is invalid or the controller not opened, an error is returned.
 *
 *  \iparam  Handle = Handle of CAN controller device
 *
 *  \return  SPI slave index or (negative) error code
 *
 ****************************************************************************/

static Error_t halSpiGetIndex (Handle_t Handle) {

    const UInt32 Index = Handle ^ DEVICE_CLASS_SPI;

    if (Index < SlaveCount) {

        if (!(DataTable[Index].Flags & HAL_OPEN_RW)) {
            return (E_DEVICE_NOT_OPEN);
        }
        return ((Error_t)Index);
    }
    return (E_DEVICE_HANDLE_INVALID);
}


/*****************************************************************************/
/*!
 *  \brief   Get number of SPI slave devices
 *
 *      Returns the number of SPI slaves configured in the serial device
 *      descriptor, contained in the HAL configuration file. This is not
 *      the same than the number of SPI interfaces, since each interface
 *      can support several slave devices.
 *
 *  \return  SPI slave index or (negative) error code
 *
 ****************************************************************************/

static UInt32 halSpiSlaveCount (void) {

    UInt32 Counter = 0;
    UInt32 i;

    for (i=0; i < halSerialDescriptorCount; i++) {
        if (halSerialDescriptors[i].BusType == SIO_TYPE_SPI) {
            Counter++;
        }
    }
    return (Counter);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize SPI bus module
 *
 *      Initializes the processor's on-chip SPI interfaces based on the
 *      parameters in the global halBusDescriptor table defined in the
 *      HAL configuration file. If the descriptor table contains invalid
 *      parameters, an error is returned. Beside the SPI controller a
 *      slave chip select output is opened and prepared for direct
 *      set/reset access. 
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halSpiInit (void) {

    UInt32 SupportedSPIs = MIN(halProcessorInfo.CountSPI, ELEMENTS(SpiParams));
    UInt32 i, k;

    if (halGetInitState() == INIT_IN_PROGRESS) {
        const halSerialDescriptor_t *Descriptor = halSerialDescriptors;

        SlaveCount = halSpiSlaveCount();

        // Allocate data tables for all slave devices
        DataTable = calloc (SlaveCount, sizeof(halSpiData_t));
        if (DataTable == NULL) {
            return (E_HEAP_MEMORY_FULL);
        }
    
        for (i=k=0; i < halSerialDescriptorCount; i++) {
            if (Descriptor->BusType == SIO_TYPE_SPI) {
                const UInt16 UnitNo = Descriptor->UnitNo;
    
                // Check if port number is legal
                if (UnitNo >= SupportedSPIs) {
                    return (E_PORT_NOT_EXISTS);
                }
                // Check if interface already initialized
                if (DataTable[k].Flags & HAL_FLAG_INITZED) {
                    return (E_BUS_ALLOCATED_TWICE);
                }
                halPeripheralClockEnable (SpiParams[UnitNo].PeripheralID, ON);

                DataTable[k].SpiNo = UnitNo;

                // Open slave chip select output pin 
                DataTable[k].HandleCS = halPortOpen(Descriptor->SelectID, HAL_OPEN_WRITE);
                if (DataTable[k].HandleCS < 0) {
                    return (DataTable[k].HandleCS);
                }
                // Get GPIO hardware parameters for direct port access
                DataTable[k].PortCS = 
                    halPortAddress(DataTable[k].HandleCS, &DataTable[k].MaskCS); 

                if (DataTable[k].PortCS == NULL) {
                    return (E_PORT_DESCRIPTOR);
                }
                DataTable[k++].Flags = HAL_FLAG_INITZED;
            }
            Descriptor++;
        }
    }    
    return (NO_ERROR);
}

//****************************************************************************/
