/****************************************************************************/
/*! \file halAD779x.c
 * 
 *  \brief  Multi-channel 16/24 Bit ADC Interface for AD779x
 *
 *  $Version: $ 0.1
 *  $Date:    $ 18.07.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage the external
 *      6 channel analog-to-digital converter AD7794 from analog devices.
 *      The chip is connected to the microcontroller via one of the SPI
 *      serial busses.
 *
 *      The functions in this module are not intended to be used from
 *      outside of the HAL. The standard analog input functions can be
 *      used instead. The 24 bit resolution of the AD7794 is scaled down
 *      to 16 bit to match the resolution of the analog input interface.
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
#include "halDeviceID.h"
#include "halCommon.h"
#include "halError.h"
#include "halInterrupt.h"
#include "halMain.h"
#include "halSysTick.h"
#include "halSpiBus.h"
#include "halAD779x.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define MINIMAL_SCANTIME  20        //!< Minimal channel scan time
#define MAXIMAL_CHANNELS  6         //!< Max number of input channels

// Additional flag bits (additional to the standard flags)
#define FLAG_IS_SCANNED   0x0100    //!< Input at least once scanned
#define FLAG_ZERO_SCALED  0x0200    //!< Zero calibration done
#define FLAG_FULL_SCALED  0x0400    //!< Full scale calibration done
#define FLAG_SCAN_ENABLE  0x0800    //!< Scanning enabled
#define FLAG_SCAN_ERROR   0x1000    //!< Scan error detected

// AD7794 Communications register bit definition
#define ADC_CR_WEN        0x80      //!< Not write enable bit
#define ADC_CR_RxE        0x40      //!< Read enable bit
#define ADC_CR_RSx        0x38      //!< Register select bitmask
#define ADC_CR_RS2        0x20      //!< Register select bit 2
#define ADC_CR_RS1        0x10      //!< Register select bit 1
#define ADC_CR_RS0        0x08      //!< Register select bit 0
#define ADC_CR_CREAD      0x04      //!< Continuous read 

// AD7794 register select defines (for ADC_CR_RSx bits)
#define ADC_REG_SR        0x00      //!< Address of status register
#define ADC_REG_MR        0x08      //!< Address of mode register
#define ADC_REG_CR        0x10      //!< Address of configuration register
#define ADC_REG_DATA      0x18      //!< Address of data register
#define ADC_REG_IO        0x28      //!< Address of i/o register
#define ADC_REG_RESET     0xFF      //!< Pseudo register (32 '1' bits)

// AD7794 Status register bit definition
#define ADC_SR_RDY        0x80      //!< Data ready (conversion done)
#define ADC_SR_ERR        0x40      //!< Data error (saturated)
#define ADC_SR_NOxREF     0x20      //!< No external reference
#define ADC_SR_WIDTH      0x08      //!< Data width (0=16 bit, 1=24 bit)
#define ADC_SR_CHx        0x07      //!< Channel address bitmask
#define ADC_SR_CH2        0x04      //!< Channel address bit 2
#define ADC_SR_CH1        0x02      //!< Channel address bit 1
#define ADC_SR_CH0        0x01      //!< Channel address bit 0

// AD7794 I/O control register bit definition
#define ADC_IO_IOE        0x40      //!< Enable AIN6 (0) or P1/P2 (1)
#define ADC_IO_IO2DAT     0x20      //!< Output P2 state
#define ADC_IO_IO1DAT     0x10      //!< Output P1 state
#define ADC_IO_IEXDIR1    0x08      //!< Direction of current sources bit 1
#define ADC_IO_IEXDIR0    0x04      //!< Direction of current sources bit 0
#define ADC_IO_IEXEN1     0x02      //!< Enable current sources bit 1
#define ADC_IO_IEXEN0     0x01      //!< Enable current sources bit 0

// AD7794 Mode register bit definition
#define ADC_MR_MDx        0xE000    //!< Mode select bitmask
#define ADC_MR_MD2        0x8000    //!< Mode select bit 2
#define ADC_MR_MD1        0x4000    //!< Mode select bit 1
#define ADC_MR_MD0        0x2000    //!< Mode select bit 0
#define ADC_MR_PSW        0x1000    //!< Power switch control
#define ADC_MR_AMPCM      0x0200    //!< OpAmp common-mode bit
#define ADC_MR_CLK1       0x0080    //!< Clock source select 1
#define ADC_MR_CLK0       0x0040    //!< Clock source select 0
#define ADC_MR_CLKx       0x00C0    //!< Clock source bitmask
#define ADC_MR_NoCHOP     0x0010    //!< Chopper disable bit
#define ADC_MR_FS3        0x0008    //!< Filter update rate bit 3
#define ADC_MR_FS2        0x0003    //!< Filter update rate bit 2
#define ADC_MR_FS1        0x0002    //!< Filter update rate bit 1
#define ADC_MR_FS0        0x0001    //!< Filter update rate bit 0
#define ADC_MR_FSx        0x000F    //!< Filter update rate bitmask
#define ADC_MR_FS120      0x0009    //!< Filter settle time = 120ms

// AD7794 Mode select definition (ADC_MR_MDx bits) 
#define ADC_MD_IZERO      0x8000    //!< Internal zero-scale calibration
#define ADC_MD_IFS        0xA000    //!< Internal full-scale calibration
#define ADC_MD_CONVERT    0x2000    //!< Single conversion mode
#define ADC_MD_STANDBY    0x6000    //!< Power-down mode
#define ADC_MD_IDLE       0x4000    //!< Idle mode

// AD7794 Configuration register bit definition
#define ADC_CON_VBIAS1    0x8000    //!< Bias voltage generator enable 1
#define ADC_CON_VBIAS0    0x4000    //!< Bias voltage generator enable 0
#define ADC_CON_BO        0x2000    //!< Burnout current enable
#define ADC_CON_UB        0x1000    //!< Unipolar (1) / Bipolar (0) select
#define ADC_CON_BOOST     0x0800    //!< Boost bias current
#define ADC_CON_G2        0x0400    //!< Gain select bit 2
#define ADC_CON_G1        0x0200    //!< Gain select bit 1
#define ADC_CON_G0        0x0100    //!< Gain select bit 0
#define ADC_CON_REF1      0x0080    //!< Reference select bit 1
#define ADC_CON_REF0      0x0040    //!< Reference select bit 0
#define ADC_CON_REFDET    0x0020    //!< Reference detect enable
#define ADC_CON_BUF       0x0010    //!< Input buffer enable
#define ADC_CON_CH3       0x0008    //!< Channel select bit 3
#define ADC_CON_CH2       0x0008    //!< Channel select bit 2
#define ADC_CON_CH1       0x0008    //!< Channel select bit 1
#define ADC_CON_CH0       0x0008    //!< Channel select bit 0
#define ADC_CON_CHx       0x000F    //!< Channel select bitmask


//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

//! Structure to hold the state of a single AD779x input channel
typedef struct {
    UInt16 Flags;       //!< Flag bits
    UInt16 ScanRate;    //!< Relative scan frequency
    UInt16 Conf;        //!< Configuration register setting
    UInt16 Mode;        //!< Mode register setting
    UInt32 Data;         //!< Actual data register
} Ad779xData_t;


//****************************************************************************/
// Private Variables 
//****************************************************************************/

static UInt16 MaxInputNo = 0;   //!< Max. input channel number
static UInt16 Resolution = 0;   //!< ADC resolution (number of bits)
static Handle_t SpiBus;         //!< SPI bus handle

static Ad779xData_t *DataTable; //!< Data table for all input channels


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static Error_t halAdcSetRegister  (UInt16 RegisterID, UInt32 Value);
static Error_t halAdcGetRegister  (UInt16 RegisterID, UInt32 *Value);
static Bool    halAdcScanChannel  (Ad779xData_t *Channel);


/*****************************************************************************/
/*!
 *  \brief   Read AD779x input channel
 *
 *      Read an analog input value from the external analog converter 
 *      AD779x and returns the result in Buffer. Channel selects which
 *      input channel to be read. For time reasons this function doesn't
 *      access the AD779x directly, but returns the last scanned value 
 *      out of memory (a conversion takes up to 500ms). The scanning is
 *      done in background by a task function which is called regulary.
 *
 *      If the selected channel is not configured, a scan error was
 *      detected, the channel is not yet calibrated, or the channel
 *      hasn't been scanned yet, an error is returned.
 *
 *
 *  \iparam  Channel = Handle of open analog channel
 *  \iparam  Buffer  = Pointer to a buffer to store the result
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halAd779xRead (UInt16 Channel, Int32 *Buffer) {

    if (Channel > MaxInputNo) {
        return (E_DEVICE_NOT_EXISTS);
    }
    if (~DataTable[Channel].Flags & FLAG_SCAN_ENABLE) {
        return (E_DEVICE_UNCONFIGURED);
    }
    if (DataTable[Channel].Flags & FLAG_SCAN_ERROR) {
        return (E_ADC_INPUT_SCAN_ERROR);
    }
    if (~DataTable[Channel].Flags & FLAG_IS_SCANNED) {
        return (E_ADC_NOT_YET_SCANNED);
    }
    if (~DataTable[Channel].Flags & FLAG_FULL_SCALED) {
        return (E_ADC_NOT_CALIBRATED);
    }
    if (Buffer != NULL) {
        if ((DataTable[Channel].Conf & ADC_CON_UB) == 0) {
            *Buffer = (DataTable[Channel].Data << (32 - Resolution)) + MAX_INT32 + 1; 
        }
        else {
            *Buffer = DataTable[Channel].Data << (31 - Resolution);
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Scan AD7794 analog input channels
 *
 *      This task function is called periodically by the HAL control task.
 *      Its purpose is to select one of the analog inputs of the AD779x
 *      and perform a conversion. The update frequency is considered when
 *      selecting the channel to scan. On each call of this task function
 *      only one input channel is scanned. Which channel to scan next is
 *      automatically determined.
 *
 *      Only channels that are configured in the HAL configuration file 
 *      are scanned. Unused channels are skipped.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halAd779xTask (void) {

    static UInt16 ScanPasses = 0;
    static UInt16 Channel = 0;

    if (!halAdcScanChannel(&DataTable[Channel])) {

        // Find next input channel, consider scan priority
        do {
            do {
                Channel = (Channel + 1) % (MaxInputNo + 1);
                if (Channel == 0) {
                    ScanPasses++;
                }
            } while (~DataTable[Channel].Flags & FLAG_SCAN_ENABLE);

        } while (ScanPasses % DataTable[Channel].ScanRate);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Scan analog input channel
 *
 *      This function must be called periodically. Its purpose is to scan
 *      the input channel associated with Channel and save the result of the
 *      data acquision in the Channel structure. Requesting the input data
 *      using halAd779xRead returns this saved scan value. This indirection
 *      is required because one conversion may take up to 500ms per channel
 *      (depending on filter rate).
 *
 *      Before starting a conversion it checks if the selected channel is
 *      already calibrated. If this isn't the case, a calibration is done
 *      before the input is first sampled. If the chopper is disabled,
 *      calibration starts with a zero calibration, followed by a full
 *      scale calibration. If chopper is enabled, zero calibration is
 *      omitted. 
 *
 *      For each conversion a conversion time is calculated. The AD779x is
 *      not checked again, before this conversion time has elapsed. This
 *      reduces processing time and SPI bus load as much as possible.
 *
 *      Returns TRUE, if a scan/calibration was started and has not yet
 *      been finished. Returns FALSE, if the last operation is done.
 *
 *  \iparam  Channel = Pointer to channel data structure
 *
 *  \return  Scan-in-progress indicator
 *
 ****************************************************************************/

static Bool halAdcScanChannel (Ad779xData_t *Channel) {

    static Bool ScanInProgress = FALSE;
    UInt32 ModeReg;
    UInt32 ConfReg;
    UInt32 DataReg;
    UInt32 StatReg;

    // Read status register of AD779x 
    if (halAdcGetRegister(ADC_REG_SR, &StatReg) != NO_ERROR) {
        Channel->Flags |= FLAG_SCAN_ERROR;
        return (ScanInProgress);
    }
    ConfReg = Channel->Conf;

    //--------------------------------------------------------
    // Do zero calibration on input (if not already done)
    //--------------------------------------------------------
    if (~Channel->Flags & FLAG_ZERO_SCALED) {
        if (!ScanInProgress) {
            ModeReg = Channel->Mode | ADC_MD_IZERO;
            if (halAdcSetRegister(ADC_REG_CR, ConfReg) == NO_ERROR &&
                halAdcSetRegister(ADC_REG_MR, ModeReg) == NO_ERROR) {
                ScanInProgress = TRUE;
            }
        }
        else if (~StatReg & ADC_SR_RDY) {
            Channel->Flags |= FLAG_ZERO_SCALED;
            ScanInProgress = FALSE;
        }
    }
    //--------------------------------------------------------
    // Do full scale calibration (if not already done)
    //--------------------------------------------------------
    else if (~Channel->Flags & FLAG_FULL_SCALED) {
        if (!ScanInProgress) {
            ModeReg = (Channel->Mode & ADC_MR_FSx) | ADC_MD_IFS | ADC_MR_FS120;
            if (halAdcSetRegister(ADC_REG_CR, ConfReg) == NO_ERROR &&
                halAdcSetRegister(ADC_REG_MR, ModeReg) == NO_ERROR) {
                ScanInProgress = TRUE;
            }
        }
        else if (~StatReg & ADC_SR_RDY) {
            Channel->Flags |= FLAG_FULL_SCALED;
            ScanInProgress = FALSE;
        }
    }
    //--------------------------------------------------------
    // Scan input channel (if channel attached)
    //--------------------------------------------------------
    else if (Channel->Flags & FLAG_SCAN_ENABLE) {
        if (!ScanInProgress) {
            ModeReg = Channel->Mode | ADC_MD_CONVERT;
            if (halAdcSetRegister(ADC_REG_CR, ConfReg) == NO_ERROR &&
                halAdcSetRegister(ADC_REG_MR, ModeReg) == NO_ERROR) {
                ScanInProgress = TRUE;
            }
        }
        else if (~StatReg & ADC_SR_RDY) {
            if (halAdcGetRegister (ADC_REG_DATA, &DataReg) == NO_ERROR) {
                Channel->Flags &= ~FLAG_SCAN_ERROR;
                Channel->Flags |=  FLAG_IS_SCANNED;
                Channel->Data = DataReg;
            }
            ScanInProgress = FALSE;
        }
    }
    return (ScanInProgress);
}


/*****************************************************************************/
/*!
 *  \brief   Attach an AD779x analog input
 *
 *      Sets up an analog input channel on the AD779x analog frontend and
 *      enables it to be scanned by the task function halAd779xTask. The
 *      channel will be enabled to be calibrated before the first scan.
 *
 *  \iparam  Channel = Input channel number
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halAd779xAttach (UInt16 Channel) {

    const halAd779xDescriptor_t *Descriptor = halAd779xDescriptors;
    UInt32 i;

    for (i=0; i < halAd779xDescriptorCount; i++) {

        if (Descriptor->PortNo == Channel) {
            Ad779xData_t *Data = &DataTable[Channel];
            
            if (Channel > MaxInputNo) {
                return (E_ADC_INPUT_NOT_EXISTS);
            }
            // Check if channel already allocated
            if (Data->Flags & FLAG_SCAN_ENABLE) {
                return (E_ADC_INPUT_USED_TWICE);
            }
            // Zero calibration not required in chopper mode
            if (~Descriptor->Mode & ADC_MR_NoCHOP) {
                Data->Flags |= FLAG_ZERO_SCALED;
            }
            Data->ScanRate = Descriptor->ScanRate;
            if (!Data->ScanRate) {
                Data->ScanRate++;
            }
            Data->Mode = Descriptor->Mode & ~ADC_MR_MDx;
            Data->Conf = Descriptor->Conf;
            Data->Flags |= FLAG_SCAN_ENABLE;
            return (NO_ERROR);
        }
        Descriptor++;
    }
    return (E_ADC_INPUT_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief   Detach an AD779x input channel
 *
 *      Detaches an analog input channel on the AD779x analog frontend 
 *      to be not scanned anymore by the task function halAd779xTask. 
 *
 *  \iparam  Channel = AD779x input channel number
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halAd779xDetach (UInt16 Channel) {

    const halAd779xDescriptor_t *Descriptor = halAd779xDescriptors;
    UInt32 i;

    for (i=0; i < halAd779xDescriptorCount; i++) {

        if (Descriptor->PortNo == Channel) {
            if (Channel > MaxInputNo) {
                return (E_ADC_INPUT_NOT_EXISTS);
            }
            DataTable[Channel].Flags &= ~FLAG_SCAN_ENABLE;
            return (NO_ERROR);
        }
        Descriptor++;
    }
    return (E_ADC_INPUT_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief   Set AD779x register
 *
 *      Writes to the AD779x-internal register identified by RegisterID.
 *      The following registers can be written to:
 *
 *      - Status register 
 *      - Configuration register
 *      - Mode register
 *      - Data register
 *
 *  \iparam  RegisterID = Register to write to
 *  \iparam  Value      = Value to write into selected register
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halAdcSetRegister (UInt16 RegisterID, UInt32 Value) {

    UInt8 Buffer[4];
    UInt32 RegSize;
    UInt32 i;

    switch (RegisterID) {
        case ADC_REG_MR:
            RegSize = sizeof(UInt16); break;

        case ADC_REG_CR:
            RegSize = sizeof(UInt16); break;

        case ADC_REG_IO:
            RegSize = sizeof(UInt8); break;

        case ADC_REG_RESET:
            RegSize = sizeof(UInt32) - 1; break;

        default:
            return (E_INVALID_REGISTER_ID);
    }
    Buffer[0] = RegisterID;

    for (i=0; i < RegSize; i++) {
        Buffer[i+1] = Value >> ((RegSize - i - 1) * 8);
    }
    return (halSpiTransfer (SpiBus, Buffer, RegSize + 1));
}


/*****************************************************************************/
/*!
 *  \brief   Get AD779x register
 *
 *      Reads the AD779x-internal register identified by RegisterID and
 *      returns it's content. The following registers can be read:
 *
 *      - Status register 
 *      - Configuration register
 *      - Mode register
 *      - Data register
 *
 *  \iparam  RegisterID = Register to read from
 *  \oparam  Value      = Pointer to variable to return result
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halAdcGetRegister (UInt16 RegisterID, UInt32 *Value) {

    UInt8 Buffer[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
    UInt32 Result = 0;
    UInt32 RegSize;
    Error_t Status;
    UInt32 i;

    switch (RegisterID) {
        case ADC_REG_SR:
            RegSize = sizeof(UInt8); break;

        case ADC_REG_DATA:
            RegSize = (Resolution > 16) ? 3:2; break;

        case ADC_REG_MR:
            RegSize = sizeof(UInt16); break;

        case ADC_REG_CR:
            RegSize = sizeof(UInt16); break;

        default:
            return (E_INVALID_REGISTER_ID);
    }
    Buffer[0] = RegisterID | ADC_CR_RxE;

    Status = halSpiTransfer (SpiBus, Buffer, RegSize + 1);
    if (Status < 0) {
        return (Status);
    }
    for (i=0; i < RegSize; i++) {
        Result += Buffer[i+1] << ((RegSize - i - 1) * 8);
    }
    *Value = Result;
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize this AD779x device module
 *
 *      Initializes the AD779x device module by doing the following things:

 *      - Opens the SPI interface the AD779x is connected to
 *      - Resets the AD779x analog front-end
 *      - Determines the converter resolution
 *      - Programms the IO register in the AD779x
 *      
 *      Beside that, the memory for the configured input channels data
 *      structure is allocated.
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halAd779xInit (void) {

    const halAd779xDescriptor_t *Descriptor = halAd779xDescriptors;
    Error_t Status;
    UInt32 StatReg;
    UInt32 i;

    if (halGetInitState() == INIT_IN_PROGRESS) {
        if (halAd779xDescriptorCount) {
            const UInt32 SpiDataFormat =
                SPI_FORMAT_MSB1ST | SPI_FORMAT_CLK_PHASE2 | SPI_FORMAT_CLK_IDLE_1;

            // Open SPI bus interface
            SpiBus = halSpiOpen (Descriptor->DeviceID, 0, NULL);
            if (SpiBus < 0) {
                return (SpiBus);
            }
            // Init SPI data format (bitrate is left at default)
            Status = halSpiSetup (SpiBus, 0, SpiDataFormat);
            if (Status < 0) {
                return (Status);
            }
            // Reset AD779x analog frontend chip (send 32 Ones)
            Status = halAdcSetRegister (ADC_REG_RESET, 0xFFFFFFFF);
            if (Status < 0) {
                return (Status);
            }
            halShortDelay (500);
             
            // Read status register to determine resolution
            Status = halAdcGetRegister (ADC_REG_SR, &StatReg);
            if (Status < 0) {
                return (Status);
            }
            Resolution = (StatReg & ADC_SR_WIDTH) ? 24 : 16;
    
            // Setup i/o configuration register 
            Status = halAdcSetRegister(ADC_REG_IO, Descriptor->IoReg);
            if (Status < 0) {
                return (Status);
            }
            // Find highest port number
            for (i=0; i < halAd779xDescriptorCount; i++) {
                if (Descriptor->PortNo > MaxInputNo) {
                    MaxInputNo = Descriptor->PortNo;
                }
                Descriptor++;
            }
            if (MaxInputNo > MAXIMAL_CHANNELS) {
                return (E_ADC_INPUT_NOT_EXISTS);
            }
            // Allocated memory for channel structure
            DataTable = calloc(MaxInputNo+1, sizeof(Ad779xData_t));    
            if (DataTable == NULL) {
                return (E_HEAP_MEMORY_FULL);
            }
        }
    }
    return (NO_ERROR);
}

//****************************************************************************/
