/****************************************************************************/
/*! \file halStepper.c
 * 
 *  \brief  Stepper motor controller HAL
 *
 *  $Version: $ 0.1
 *  $Date:    $ 23.02.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access stepper motor controllers
 *      of the type "Trinamic TMC26x" connected via serial SPI ports. It
 *      supports any number of stepper motors. Only stepper motors using
 *      different SPI interfaces can run simultainously, steppers connected
 *      to the same SPI can run only alternative. Chip select outputs are
 *      used to distinguish different stepper controllers on the SPI port.
 *
 *      It would be possible to run multiple steppers on the same SPI port
 *      simultainously with a few modifications on this module. That solution
 *      would have the disadvantage that execution time would be longer than
 *      with the current implementation. Since minimising execution time
 *      was the most important goal, the non-sharing SPI was choosen.
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

#include "Global.h"
#include <stdlib.h>
#include "halDeviceID.h"
#include "halCommon.h"
#include "halError.h"
#include "halClocks.h"
#include "halMain.h"
#include "halInterrupt.h"
#include "halPorts.h"
#include "halStepper.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define SINUS_INDEX_MASK    0x007F    //!< Sinus table index mask
#define SINUS_SEGMENT       0x0080    //!< Sinus wave segment bit

#define TMC_CONFIG_MASK     0xFFFFF   //!< Configuration register bitmask
#define TMC_DRVCTR_PHASE    0x00100   //!< Phase polarity bitmask
#define TMC_DRVCTR_PHA      0x20000   //!< Phase A polarity
#define TMC_DRVCTR_CA       0x1FE00   //!< Phase A current mask
#define TMC_DRVCTR_PHB      0x00100   //!< Phase B polarity
#define TMC_DRVCTR_CB       0x000FF   //!< Phase B current mask
#define TMC_DRVCFG_CFR      0xE0000   //!< DRV configuration register select
#define TMC_DRVCFG_SDOFF    0x00080   //!< Step/direction mode off
#define TMC_DRVCFG_SGCS     0xC0000   //!< SGCS configuration register select
#define TMC_SGCSCONF_CS     0x0001F   //!< Current scale mask


#define TMC_STATUS_STST     0x00080   //!< Stand still step indicator
#define TMC_STATUS_OLB      0x00040   //!< Open load indicator (PHB)
#define TMC_STATUS_OLA      0x00020   //!< Open load indicator (PHA)
#define TMC_STATUS_S2GB     0x00010   //!< Short to GND detection (PHB)
#define TMC_STATUS_S2GA     0x00008   //!< Short to GND detection (PHA)
#define TMC_STATUS_OTPW     0x00004   //!< Overtemperature pre-warning
#define TMC_STATUS_OT       0x00002   //!< Overtemperature
#define TMC_STATUS_SG       0x00001   //!< StallGuard status
#define TMC_STATUS_SGR      0xFFC00   //!< StallGuard results
#define TMC_STATUS_ERROR    0x0007B   //!< Error bits mask

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
} SpiRegFile_t;

//! Structure to hold the state of a stepper motor controller
typedef struct {
    UInt16 Flags;           //!< Flag bits
    UInt16 SpiNo;           //!< SPI channel used to transfer data  
    UInt32 Status;          //!< Status word from stepper controller
    UInt32 SGCSConf;        //!< last SGCSCONF register value
    UInt32 SelMask;         //!< Chip select pin bitmask
    UInt32 *SelPort;        //!< Chip select port register pointer
    Handle_t HandleEN;      //!< Handle to enable output pin
    Handle_t HandleCS;      //!< Handle to chip select output
    SpiRegFile_t *SPI;      //!< Pointer to SPI register file
    Bool   OwnBusLock;      //!< SPI bus lock is owned by this stepper
} StepDevice_t;

//! Structure to hold the state of a SPI device
typedef struct {
    SpiRegFile_t *SPI;      //!< Pointer to register file
    UInt16 PeripheralID;    //!< Peripheral identifier
    UInt16 InterruptID;     //!< Interrupt identifier
    Bool Locked;            //!< Indicates if the bus is locked
    StepDevice_t *Stepper;  //!< Pointer to stepper descriptor
} StepBusData_t;


//****************************************************************************/
// Private Variables 
//****************************************************************************/

//! Data table holding the state of all SPI controllers
static StepBusData_t SpiData[] = {
    { (SpiRegFile_t*) 0x40013000, PERIPHERAL_SPI1, INTERRUPT_SPI1, FALSE },
    { (SpiRegFile_t*) 0x40003800, PERIPHERAL_SPI2, INTERRUPT_SPI2, FALSE },
    { (SpiRegFile_t*) 0x40003C00, PERIPHERAL_SPI3, INTERRUPT_SPI3, FALSE }
};

//! Data table holding the variables for all stepper motors
static StepDevice_t *DataTable = NULL;


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static inline Error_t halStepperGetIndex (Handle_t Handle);
static inline Error_t halStepperExchange (const StepDevice_t *Stepper, UInt32 Data);
static inline Error_t halStepperError (UInt32 ErrFlags);

static Error_t halStepperFindDevice (Device_t DeviceID);
static Error_t halStepperSpiSetup (UInt32 SpiNo);

UInt32* halPortAddress (Handle_t Handle, UInt32 *Bitmask);


/*****************************************************************************/
/*!
 *  \brief   Micro step table
 *
 *      This table contains the values for the dac converter in the
 *      stepper motor controller for 2 full steps divided into 64 micro 
 *      steps each. It describes a sinus halve wave. A full sinus wave
 *      will be generated by cycling through the table twice with
 *      alternating sign (the table only contains absolute values).
 *      The max. value used is 248 to leave enough headroom for the
 *      different offsets added by the controller depending on the
 *      programmed configuration.
 *
 *      The size of this table MUST BE a power of 2.
 *
 ****************************************************************************/

static const UInt8 SinusTable[] = {
      1,   7,  13,  19,  25,  31,  37,  43,  49,  55,  61,  67,  73,  79,  
     84,  90,  96, 101, 107, 112, 118, 123, 128, 133, 138, 143, 148, 153, 
    158, 163, 167, 172, 176, 180, 184, 188, 192, 196, 200, 203, 207, 210, 
    213, 216, 219, 222, 225, 227, 229, 232, 234, 236, 238, 239, 241, 242, 
    243, 244, 245, 246, 247, 247, 248, 248, 248, 248, 247, 247, 246, 245, 
    244, 243, 242, 241, 239, 238, 236, 234, 232, 229, 227, 225, 222, 219,
    216, 213, 210, 207, 203, 200, 196, 192, 188, 184, 180, 176, 172, 167, 
    163, 158, 153, 148, 143, 138, 133, 128, 123, 118, 112, 107, 101,  96,  
     90,  84,  79,  73,  67,  61,  55,  49,  43,  37,  31,  25,  19,  13,   
     7,   1
};

#define STEP_RESOLUTION (ELEMENTS(SinusTable) / 2) //!< Microstep resolution


/*****************************************************************************/
/*!
 *  \brief   Open a stepper motor controller with specific configuration mode
 *
 *      Opens a stepper motor controller and returns a handle to be used 
 *      to access it. DeviceID must be the numerical "name" of a stepper 
 *      motor controller defined in the HAL configuration file.
 *
 *      If parameter Mode is NULL the stepper controller is configured to
 *      default mode as specified in the HAL configuration file.
 *
 *      Otherwise the stepper controller is configured with the supplied
 *      mode parameters.
 *
 *      The driver stage enable output is automatically enabled.
 *
 *  \iparam  DeviceID = Stepper controller device ID
 *  \iparam  Mode     = Mode array
 *  \iparam  Count    = Number of elements in mode array
 *
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

Error_t halStepperOpenExt (Device_t DeviceID, const UInt32 *Mode, UInt16 Count) {

    const Int32 Index = halStepperFindDevice(DeviceID);
    Error_t Status;

    if (Index >= 0) {
        const Int32 Handle = DEVICE_CLASS_STEPPER + Index;
        const Int32 SpiNo  = DataTable[Index].SpiNo;

        if (DataTable[Index].Flags & HAL_OPEN_RW) {
            return (E_DEVICE_ALREADY_OPEN);
        }
        
        // Setup SPI interface the controller is connected to
        if ((Status = halStepperSpiSetup(SpiNo)) != NO_ERROR) {
            return (Status);
        }
        
//        SpiData[SpiNo].Stepper = &DataTable[Index];
//        SpiData[SpiNo].SPI->CR2 |= SPI_CR2_RXNEIE;
//        halInterruptEnable (
//            SpiData[SpiNo].InterruptID, IRQ_PRIO_HIGHEST);
        DataTable[Index].Flags |= HAL_OPEN_RW;

        // lock bus to init the controller and SPI interrupt handling
        Status = halStepperControl (Handle, STP_CTRL_LOCK_BUS, TRUE);
        if (NO_ERROR == Status) {
            // setup controller
            Status = halStepperSetup (Handle, Mode, Count);
            if (NO_ERROR == Status) {
                halStepperWrite (DeviceID, 0);
                // Enable driver transistors
                Status = halStepperControl (Handle, STP_CTRL_ENABLE, ON);
            }
        }
        // unlock bus
        halStepperControl (Handle, STP_CTRL_LOCK_BUS, FALSE);
    
        if (Status != NO_ERROR) {
            halStepperClose (DeviceID);
            return (Status);
        }
        return (Handle);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Open a stepper motor controller in default mode
 *
 *      Opens a stepper motor controller and returns a handle to be used 
 *      to access it. DeviceID must be the numerical "name" of a stepper 
 *      motor controller defined in the HAL configuration file. The
 *      stepper controller is configured to default mode as specified
 *      in the HAL configuration file. The driver stage enable output 
 *      is automatically enabled.
 *
 *  \iparam  DeviceID = Stepper controller device ID
 *
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

Error_t halStepperOpen (Device_t DeviceID) {

    return halStepperOpenExt (DeviceID,NULL, 0);
} 

/*****************************************************************************/
/*!
 *  \brief   Close a stepper motor controller
 *
 *      Closes an open stepper motor controller. 
 *
 *  \iparam  Handle = Handle of open stepper motor
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halStepperClose (Handle_t Handle) {

    const Int32 Index = halStepperGetIndex(Handle);

    if (Index >= 0) {
//        UInt32 SpiNo = DataTable[Index].SpiNo;

//        halInterruptDisable (SpiData[SpiNo].InterruptID);
        halStepperControl (Handle, STP_CTRL_ENABLE, OFF);
//        SpiData[SpiNo].SPI->CR2 &= ~SPI_CR2_RXNEIE;
//        SpiData[SpiNo].Stepper = NULL; 

        DataTable[Index].Flags &= ~HAL_OPEN_RW;
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Write step value to stepper motor
 *
 *      Writes a microstep value to the stepper motor controller. The
 *      analog current values for the two motor phases are looked up 
 *      in a sinus wave table. The given SinIndex is used as an index
 *      into that table to find the current for phase A. The cosinus
 *      used for phase B is looked up in the sinus table by adding an
 *      offset equivalent to a quarter sinus wave to SinIndex. The
 *      lookup table contains only a half sinus. The full wave is
 *      generated by alternating the sign bit.
 *
 *      To archieve a smooth clockwise rotation, 'SinIndex' must be 
 *      continuously incremented from 0...255 and than wrap arround.
 *      For counter clockwise rotation SinIndex must be decremented
 *      instead of incremented. A full sinus wave moves the stepper
 *      motor 4 full steps. A full step is equivalent to 64 index
 *      steps. If less than 64 micro steps per full step is needed,
 *      the index must be incremented/decremented by a value > 1.
 *      The step width defines the resulting micro steps. The full
 *      steps are on indices 32, 96, 160, 224. Half steps are on
 *      indices 0, 64, 128, 192.
 *
 *  \iparam  Handle   = Handle of open stepper motor
 *  \iparam  SinIndex = Index into sinus table
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halStepperWrite (Handle_t Handle, UInt32 SinIndex) {

    const UInt32 CosIndex = SinIndex + STEP_RESOLUTION;
    const Int32 Index = halStepperGetIndex(Handle); 
    Error_t Status;

    if (Index >= 0) {
        const StepDevice_t *Data = &DataTable[Index];
        UInt32 PhaseA = SinusTable[SinIndex & SINUS_INDEX_MASK];
        UInt32 PhaseB = SinusTable[CosIndex & SINUS_INDEX_MASK];
        UInt32 ErrFlags;
        UInt32 Control;

        if (!(SinIndex & SINUS_SEGMENT)) {
           PhaseA |= TMC_DRVCTR_PHASE;
        }
        if (!(CosIndex & SINUS_SEGMENT)) {
           PhaseB |= TMC_DRVCTR_PHASE;
        }
        Control  = (PhaseA << 9) | PhaseB;
        ErrFlags = Data->Status >> 12;

        Status = halStepperExchange (Data, Control);
        if (Status != NO_ERROR) {
            return (Status);
        }
        return (halStepperError(ErrFlags));
    }
    return (Index);
}

/*****************************************************************************/
/*!
 *  \brief   Write step value to stepper motor - Simplified version
 *
 *      ( \sa halStepperWrite ) 
 *       
 *      This is a simplified version of "halStepperWrite" to achieve
 *      shorter execution time. It should only be used if time
 *      optimization is important.
 *
 *      The ISR of the stepper function-module uses this version to minimize
 *      execution time of the ISR. This allows faster stepper movement.
 *
 *      To address the correct stepper motor the caller must submit the
 *      stepper motor instance instead of the handle.
 *
 *  \iparam  Index    = stepper motor instance
 *  \iparam  SinIndex = Index into sinus table
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halStepperWrite_Simplified (Int32 Index, UInt32 SinIndex) {

    const UInt32 CosIndex = SinIndex + STEP_RESOLUTION;

    const StepDevice_t *Data = &DataTable[Index];
    UInt32 PhaseA = SinusTable[SinIndex & SINUS_INDEX_MASK];
    UInt32 PhaseB = SinusTable[CosIndex & SINUS_INDEX_MASK];
    UInt32 Control;

    if (!(SinIndex & SINUS_SEGMENT)) {
       PhaseA |= TMC_DRVCTR_PHASE;
    }
    if (!(CosIndex & SINUS_SEGMENT)) {
       PhaseB |= TMC_DRVCTR_PHASE;
    }
    Control  = (PhaseA << 9) | PhaseB;

    return halStepperExchange (Data, Control);
}

/*****************************************************************************/
/*!
 *  \brief   Write current scale value to stepper motor
 *
 *      Writes current scale value to the stepper motor controller.
 *      Can be used to supply different current values for run and stop
 *      current.
 *
 *  \iparam  Handle   = Handle of open stepper motor
 *  \iparam  Current  = current scale from 0, 1/32, 2/32 ... up to 32/32
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halStepperCurrent (Handle_t Handle, UInt8 Current) {

    const Int32 Index = halStepperGetIndex(Handle); 
    Error_t Status;

    if (Index >= 0) {
        const StepDevice_t *Data = &DataTable[Index];
        UInt32 Control;

        Control  = ((Data->SGCSConf & ~TMC_SGCSCONF_CS) | (Current & TMC_SGCSCONF_CS));

        Status = halStepperExchange (Data, Control);
        return (Status);
    }
    return (Index);
}


/*****************************************************************************/
/*! 
 *  \brief   Stepper motor controller mode setup
 *
 *      Sets the mode of the external TMC26x stepper motor controller. The
 *      Mode parameter can contain any number of register settings which 
 *      are transfered to the controller chip. 'Count' must be the number
 *      of words in Mode[]. If Mode is NULL, the controller is set to the
 *      mode specified in the HAL configuration file. This function 
 *      shouldn't be called while the stepper motor is running.
 *
 *      Normally, there is no reason to use this function, because the 
 *      stepper controller is automatically setup by the HAL based on the
 *      settings in the configuration file. The only purpose of this function
 *      is to find the optimal settings by trying different operation modes
 *      before setting up the HAL configuration file.
 *
 *      The Mode parameter depends on the stepper motor controller used and
 *      is therefore not completely hardware independent. This is another
 *      reason not to use this function.
 *  
 *  \iparam  Handle = Handle of open stepper motor
 *  \iparam  Mode   = Mode array
 *  \iparam  Count  = Number of elements in mode array
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halStepperSetup (Handle_t Handle, const UInt32 *Mode, UInt16 Count) {

    const Int32 Index = halStepperGetIndex(Handle);
    UInt16 i;

    if (Index >= 0) {
        StepDevice_t *Data = &DataTable[Index];

        // if Mode is NULL use settings from configuration file
        if (Mode == NULL) {
            Count = ELEMENTS(halStepperDescriptors[0].Mode);
            Mode  = halStepperDescriptors[Index].Mode;
        }
        for (i=0; i < Count; i++) {

            UInt32 Control = Mode[i];

            // Check register value for validity
            if (Control > TMC_CONFIG_MASK) {
                return (E_STEPPER_ILLEGAL_MODE);
            }
            // Check if SGCS configuration register is initialized
            if (TMC_DRVCFG_SGCS == (Control & 0xE0000)) {
                Data->SGCSConf = Control; // store value for later modification of current scale
            }
            // Force SPI mode bit in DRVCONF register
            if ((~Control & TMC_DRVCFG_CFR) == 0) {
                Control |= TMC_DRVCFG_SDOFF;
            }
            // Transfer value to the stepper controller
            while (halStepperExchange (Data, Control) == E_STEPPER_BUSY) {}
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Get status of stepper motor controller
 *
 *      Returns the status of the stepper motor controller associated with
 *      Handle. What status to return depends on 'StatusID'. The following 
 *      informations can be requested:
 *
 *      - Number of micro steps per full step
 *      - Temperature status of controller
 *      - Stall-guard results
 *      - Error flag bits
 *
 *      If a unsupported StatusID is supplied, an error is returned.
 *
 *  \iparam  Handle   = Handle of open stepper motor
 *  \iparam  StatusID = Type of status requested
 *
 *  \return  Status value or (negative) error code
 *
 ****************************************************************************/

Error_t halStepperStatus (Handle_t Handle, StepStatID_t StatusID) {

    const Int32 Index = halStepperGetIndex(Handle);

    if (Index >= 0) {
        const StepDevice_t *Data = &DataTable[Index];
        const UInt32 Flagbits = (Data->Status >> 12);

        switch (StatusID) {

            case STP_STAT_STALLGUARD:
                return ((Flagbits & TMC_STATUS_SGR) >> 10);

            case STP_STAT_ERRORFLAGS:
                return (Flagbits & TMC_STATUS_ERROR);

            case STP_STAT_TEMPERATURE:
                if (Flagbits & TMC_STATUS_OT) {
                    return (STP_TEMP_FAILURE);
                }
                if (Flagbits & TMC_STATUS_OTPW) {
                    return (STP_TEMP_WARNING);
                }
                return (STP_TEMP_GOOD);

            case STP_STAT_MICROSTEPS:
                return (STEP_RESOLUTION);
                
            case STP_STAT_LOCKED:
                return (SpiData[Data->SpiNo].Locked);
        }
        return (E_UNKNOWN_STATUS_ID);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Control stepper motor controller
 *
 *      Controls the stepper motor controller associated with Handle. 
 *      Depending on ControlID the following actions can be initiated:
 *
 *      - Enable/disable stepper motor driver stage
 *      - Reset stepper controller error bits
 *      - lock SPI used by stepper (needed for shared SPI)
 *
 *      State defines the new state the selected parameter is set to.
 *      The driver stage is enabled by default after opening a stepper.
 *
 *  \iparam  Handle    = Handle of open stepper motor
 *  \iparam  ControlID = Control type identifier
 *  \iparam  State     = State to set the selected parameter to
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halStepperControl (Handle_t Handle, StepCtrlID_t ControlID, Bool State) {

    const Int32 Index = halStepperGetIndex(Handle);

    if (Index >= 0) {
        StepDevice_t *Data = &DataTable[Index];

        switch (ControlID) {

            case STP_CTRL_ENABLE:
                return (halPortWrite (Data->HandleEN, State));

            case STP_CTRL_CLEAR_ERROR:
                if (State) {
                    Data->Status = 0;
                }
                return (NO_ERROR);
                
            case STP_CTRL_LOCK_BUS:
                {
                    UInt32 SpiNo =Data->SpiNo;
                    // check if lock / unlock is allowed
                    if (State) { // only can get lock if not owned by another fm
                        if (!Data->OwnBusLock && SpiData[SpiNo].Locked) {
                            return E_DEVICE_LOCKED;
                        }
                    }
                    else {  // only owner can unlock
                        if (!Data->OwnBusLock) {
                            return E_STEPPER_NO_BUS;
                        }
                    }
                    // perform lock / unlock
                    SpiData[SpiNo].Locked = State;
                    Data->OwnBusLock = State;
                    // enable / disable SPI interrupt handling after pending data transfer is finished
                    while ((SpiData[SpiNo].SPI->SR & SPI_SR_BSY)) {}
                    if (State)
                    {
                        SpiData[SpiNo].Stepper = Data;
                        SpiData[SpiNo].SPI->CR2 |= SPI_CR2_RXNEIE;
                        halInterruptEnable (
                            SpiData[SpiNo].InterruptID, IRQ_PRIO_HIGHEST);
                    }
                    else
                    {
                        halInterruptDisable (SpiData[SpiNo].InterruptID);
                        SpiData[SpiNo].SPI->CR2 &= ~SPI_CR2_RXNEIE;
                        SpiData[SpiNo].Stepper = NULL; 
                    }
                }
                return (NO_ERROR);
        }
        return (E_UNKNOWN_CONTROL_ID);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Convert error flags to error code
 *
 *      Translates supplied 'ErrFlags' into an error code and returns it.
 *      If more than one error flags are set, the most significant error
 *      is returned. If no error bits are set NO_ERROR is returned.
 *
 *  \iparam  ErrFlags = Error flag bits from stepper controller
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static inline Error_t halStepperError (UInt32 ErrFlags) {

    if (ErrFlags & TMC_STATUS_ERROR) {     

        if (ErrFlags & (TMC_STATUS_S2GA | TMC_STATUS_S2GB)) {
            return (E_STEPPER_SHORT_CIRCUIT);
        }
        if (ErrFlags & TMC_STATUS_OT) {
            return (E_STEPPER_TEMPERATURE);
        }
        if (ErrFlags & (TMC_STATUS_OLA | TMC_STATUS_OLB)) {
            return (E_STEPPER_OPEN_LOAD);
        }
        if (ErrFlags & TMC_STATUS_SG) {
            return (E_STEPPER_MOTOR_STALLED);
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Transfer data stepper motor controller
 *
 *      Transfers the supplied Data word via SPI to the stepper controller
 *      defined by Stepper. Before sending, the SPI controller is checked
 *      if it is still busy. If this is the case, the data is not send and
 *      FALSE is returned. In the opposite case data is send, split up in-
 *      to two 16 bit words, and TRUE is returned. Before sending, the
 *      slave select output NSS is activated.
 *
 *  \iparam  Stepper = Pointer to stepper descriptor
 *  \iparam  Data    = Data word to transmit
 *
 *  \return  TRUE: Successful, FALSE: Failed, SPI busy
 *
 ****************************************************************************/

static inline Error_t halStepperExchange (const StepDevice_t *Stepper, UInt32 Data) {

    SpiRegFile_t *SPI = Stepper->SPI;     

    if (!Stepper->OwnBusLock) {
        return (E_STEPPER_NO_BUS);
    }

    if (!(SPI->SR & SPI_SR_BSY)) {
        halGlobalInterruptDisable();

        // Activate slave select output (NSS)
        *Stepper->SelPort = Stepper->SelMask << 16;

        //while (!(SPI->SR & SPI_SR_TXE)) {}
        SPI->DR = (UInt16) (Data >> 16);

        while (!(SPI->SR & SPI_SR_TXE)) {}
        SPI->DR = (UInt16) Data;
        halGlobalInterruptEnable();

        return (NO_ERROR);
    }
    return (E_STEPPER_BUSY);
}


/*****************************************************************************/
/*!
 *  \brief   Stepper motor interrupt handler
 *
 *      This is the interrupt handler which is called when the SPI controller
 *      receives data. It reads the SPI receive data register and saves it
 *      as stepper motor controller status word. After the last word of a
 *      transmission is received, the chip select NSS is deactivated.
 *
 *  \iparam  SpiNo = SPI device number
 *
 ****************************************************************************/

void halStepperInterruptHandler (UInt32 SpiNo) {

    if (SpiNo < ELEMENTS(SpiData)) {
        StepDevice_t *Stepper = SpiData[SpiNo].Stepper;

        // Deactivate slave select output 
        if (!(Stepper->SPI->SR & SPI_SR_BSY)) {
            *Stepper->SelPort = Stepper->SelMask;
        }
        Stepper->Status = (Stepper->Status << 16) | Stepper->SPI->DR;
    }
}


/*****************************************************************************/
/*!
 *  \brief   Set mode of SPI channel
 *
 *      Sets the data frame format and bitrate for the communication. The 
 *      following parameters are setup to meet the requirements of the
 *      TMC 26x stepper controller.
 *
 *      - Shift direction (MSB first)
 *      - Clock Phase
 *      - Clock Polarity
 *      - Frame Format (16 bits)
 *
 *      The SPI baudrate is set to 4,5MHz (divider = 8). This is the maximum
 *      allowed for the stepper controller as long as it is clocked with a 
 *      9...16 MHz system clock. The next higher baud rate (9 MHz) whould 
 *      require a driver clock rate of at least 18 MHz.
 *
 *  \iparam  SpiNo = SPI channel number
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halStepperSpiSetup (UInt32 SpiNo) {

    if (SpiNo < ELEMENTS(SpiData)) {
        halPeripheralClockEnable (SpiData[SpiNo].PeripheralID, ON);
        
        SpiData[SpiNo].SPI->CR2 |= SPI_CR2_SSOE;
        SpiData[SpiNo].SPI->CR1 = 
            SPI_CR1_MSTR | SPI_BR_DIVIDE8 | SPI_CR1_CPOL | SPI_CR1_CPHA| SPI_CR1_DFF;
        SpiData[SpiNo].SPI->CR1 |= SPI_CR1_SPE;
//        SpiData[SpiNo].Stepper = NULL;

        return (NO_ERROR);
    }
    return (E_DEVICE_NOT_EXISTS);
}

  
/*****************************************************************************/
/*!
 *  \brief   Find logical stepper motor
 *
 *      Searches the stepper motor descriptor for an entry with the asked
 *      DeviceID. If found, the index into the descrptor table is returned.
 *      If this module isn't initialized, the requested hardware doesn't
 *      exist or the device isn't initialized, an error is returned.
 *
 *  \iparam  DeviceID = Logical stepper motor device ID
 *
 *  \return  Data table index or (negative) error code
 *
 ****************************************************************************/

static Error_t halStepperFindDevice (Device_t DeviceID) {

    UInt32 Index;

    if ((DeviceID & DEVICE_CLASS_MASK) == DEVICE_CLASS_STEPPER) {

        halInitState_t InitState = halGetInitState();

        if (InitState == INIT_NOT_DONE || InitState == INIT_FAILED) {
            return (E_HAL_NOT_INITIALIZED);
        }
        for (Index=0; Index < halStepperDescriptorCount; Index++) {

            if (halStepperDescriptors[Index].DeviceID == DeviceID) {

                if (~DataTable[Index].Flags & HAL_FLAG_INITZED) {
                    return (E_DEVICE_UNCONFIGURED);
                }
                return (Index);
            }
        }
    }
    return (E_UNKNOWN_DEVICE_ID);
}


/*****************************************************************************/
/*!
 *  \brief   Get index into stepper motor data table
 *
 *      Returns the index of the stepper motor controller associated with
 *      Handle. The index is used to address an entry in the 'DataTable',
 *      which contains the complete state of that channel. The channel 
 *      must be open, else an error is returned.
 *
 *  \iparam  Handle = Handle of open stepper motor
 *
 *  \return  DataTable index or (negative) error code
 *
 ****************************************************************************/

static inline Error_t halStepperGetIndex (Handle_t Handle) {

    UInt32 Index = Handle ^ DEVICE_CLASS_STEPPER;

    if (Index < halStepperDescriptorCount) {

        if (!(DataTable[Index].Flags & HAL_OPEN_RW)) {
            return (E_DEVICE_NOT_OPEN);
        }
        return ((Error_t)Index);
    }
    return (E_DEVICE_HANDLE_INVALID);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize stepper controller module
 *
 *      Initializes this module by evaluating the stepper motor descriptor 
 *      table from the HAL configuration file and setting up the local
 *      data structures and acquiring the necessary hardware resources.
 *      The following resources are required per motor:
 *
 *      - SPI interface (may be shared between motors)
 *      - Stepper driver stage enable output (ENN)
 *      - Stepper controller chip select output (NSS)
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halStepperInit (void) {

    UInt32 i;

    if (halGetInitState() == INIT_IN_PROGRESS) {

        DataTable = calloc (halStepperDescriptorCount, sizeof(StepDevice_t));
        if (DataTable == NULL) {
            return (E_HEAP_MEMORY_FULL);
        }    
        for (i=0; i < halStepperDescriptorCount; i++) {
    
            StepDevice_t *Data = &DataTable[i];

            // Get SPI channel number and verify it
            Data->SpiNo = halStepperDescriptors[i].SpiNo;
            if (Data->SpiNo >= halProcessorInfo.CountSPI ||
                Data->SpiNo >= ELEMENTS(SpiData)) {
                return (E_STEPPER_DESCRIPTOR);
            }        
            // Open stepper motor driver enable output pin 
            Data->HandleEN = halPortOpen(HAL_STEPPER_ENABLE+i, HAL_OPEN_WRITE);
            if (Data->HandleEN < 0) {
                return (Data->HandleEN);
            }
            // Open stepper motor controller select output pin 
            Data->HandleCS = halPortOpen(HAL_STEPPER_SELECT+i, HAL_OPEN_WRITE);
            if (Data->HandleCS < 0) {
                return (Data->HandleCS);
            }
            // Get GPIO port parameters for direct port access
            Data->SelPort = halPortAddress(Data->HandleCS, &Data->SelMask); 
            if (Data->SelPort == NULL) {
                return (E_PORT_DESCRIPTOR);
            }
            *Data->SelPort = Data->SelMask;
            Data->SPI = SpiData[Data->SpiNo].SPI;
            Data->Flags = HAL_FLAG_INITZED;
        }
    }
    return (NO_ERROR);
}

//****************************************************************************/
