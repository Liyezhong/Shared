/****************************************************************************/
/*! \file halAnalog.c
 *
 *  \brief  Analog channel i/o
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage logical analog
 *      inputs and outputs. Each logical channel is mapped to a physical
 *      input/output applying a logical channel descriptor table, which is
 *      part of the HAL configuration module. Access to a logical channel
 *      requires opening that channel. The open function returns a handle
 *      which has to be used as a parameter to the read/write functions.
 *      If the channel is not needed anymore, the channel can be closed
 *      using the close function. Analog output channels can be read or
 *      written, input channels can only be read.
 *
 *      This implementation uses only one of the ADCs to convert the up to
 *      16 analog input channels. It would be possible to use two ADCs and
 *      distribute the input channels on these two ADCs using simultainous
 *      or interleaved mode. This could halve the overall conversion time,
 *      with the drawback that the conversion times of the channels must
 *      be carefully paired.
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
#include "halDma.h"
#include "halAD779x.h"
#include "halAnalog.h"
#include "halPwm.h"
#include "halSpiBus.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

// Register bits of the on-chip ADC peripheral
#define ADC_SR_EOC        0x00000002    //!< ADC End of conversion
#define ADC_CR1_EOCIE     0x00000020    //!< ADC End of conversion interrupt enable
#define ADC_CR1_SCAN      0x00000100    //!< ADC Scan mode enable
#define ADC_CR2_ADON      0x00000001    //!< ADC converter on/off
#define ADC_CR2_CONT      0x00000002    //!< ADC Continuous conversion enable
#define ADC_CR2_CAL       0x00000004    //!< ADC Calibration
#define ADC_CR2_DMA       0x00000100    //!< ADC Direct memory access mode
#define ADC_CR2_ALIGN     0x00000800    //!< ADC Data alignment (1=left)
#define ADC_CR2_SWSTART   0x00400000    //!< ADC Start conversion
#define ADC_SEQ_BITS      5             //!< Bits per CHN in SEQ register
#define ADC_SMP_BITS      3             //!< Bits per CHN in SMP register

// Register bits of the on-chip DAC peripheral
#define DAC_CR_DMAEN1     0x00001000    //!< DAC1 DMA enable
#define DAC_CR_MAMP1      0x00000F00    //!< DAC1 mask/amplitude selector
#define DAC_CR_WAVE1      0x000000C0    //!< DAC1 wave generation enable
#define DAC_CR_TSEL1      0x0000003F    //!< DAC1 trigger selection
#define DAC_CR_TEN1       0x00000004    //!< DAC1 trigger enable
#define DAC_CR_BOFF1      0x00000002    //!< DAC1 output buffer disable
#define DAC_CR_EN1        0x00000001    //!< DAC1 enable
#define DAC_CR_DMAEN2     0x10000000    //!< DAC2 DMA enable
#define DAC_CR_MAMP2      0x0F000000    //!< DAC2 mask/amplitude selector
#define DAC_CR_WAVE2      0x00C00000    //!< DAC2 wave generation enable
#define DAC_CR_TSEL2      0x003F0000    //!< DAC2 trigger selection
#define DAC_CR_TEN2       0x00040000    //!< DAC2 trigger enable
#define DAC_CR_BOFF2      0x00020000    //!< DAC2 output buffer disable
#define DAC_CR_EN2        0x00010000    //!< DAC2 enable
#define DAC_DOR_DATA      0x00000FFF    //!< DAC output register data mask

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Register layout of processor internal A/D converters
typedef volatile struct {
    UInt32 SR;              //!< ADC status register
    UInt32 CR1;             //!< ADC control register 1
    UInt32 CR2;             //!< ADC control register 2
    UInt32 SMPR[2];         //!< ADC sample time register 2
    UInt32 JOFR[4];         //!< ADC injected channel data offset registers
    UInt32 HTR;             //!< ADC watchdog high threshold register
    UInt32 LTR;             //!< ADC watchdog low threshold register
    UInt32 SEQ[3];          //!< ADC regular sequence registers
    UInt32 JSEQ;            //!< ADC injected sequence register
    UInt32 JDR[4];          //!< ADC injected data registers
    UInt32 DR;              //!< ADC regular data register
} AdcRegFile_t;

//! Register layout of processor internal D/A converters
typedef volatile struct {
    UInt32 CR;              //!< DAC control register
    UInt32 SWTRIGGER;       //!< DAC software trigger register
    //! DAC channel data
    struct Channel {        
        UInt32 RIGHT12;     //!< 12-bit right-aligned data holding register
        UInt32 LEFT12;      //!< 12-bit left-aligned data holding register
        UInt32 RIGHT8;      //!< 8-bit right aligned data holding register
    } DHR[3];               //!< DAC Data registers for CH1, CH2, CH1/2
    UInt32 DOR[2];          //!< DAC output registers for CH1, CH2
} DacRegFile_t;

//! Structure to hold the state of a logical channel
typedef struct {
    UInt16 Flags;       //!< Port flags (access mode)
    UInt16 DataIdx;     //!< Scan buffer index
    Handle_t Handle;    //!< Handle of a peripheral (only for PWM, SPI)
    InterruptVector_t IntrVector;   //!< Interrupt vector of the channel
    Bool IntrActive;    //!< Enable / disable flag of the interrupt
} AioDevice_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Hardware registers to control internal D/A converters
static DacRegFile_t *DAC = (DacRegFile_t*) 0x40007400;

//! Hardware registers to control internal A/D converters
static AdcRegFile_t *ADC = (AdcRegFile_t*) 0x40012400;

//! Data table holding the variables for all analog channels
static AioDevice_t *DataTable = NULL;

//! array to hold the ADC results (updated via DMA)
static volatile UInt16 *ScanBuffer;  

                      
//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t halAnalogGetIndex    (Handle_t Handle, UInt16 Mode);
static Error_t halAnalogFindDevice  (Device_t DeviceID);
static Error_t halAnalogInitOutputs (void);
static Error_t halAnalogInitInputs  (void);

static Error_t halAnalogInitScanner (UInt16 Channels, UInt32 *Sequence, UInt32 *Sampling);


/*****************************************************************************/
/*!
 *  \brief   Opens an analog input/output
 *
 *      Opens an analog input or output channel and returns a handle to
 *      be used to read/write from/to that channel. DeviceID must be the 
 *      numerical name of an analog channel defined in the configuration
 *      file. Additionally, a analog channel decriptor must exist in the  
 *      external halAnalogChannelTable[] descibing the channel.
 *      This descriptor includes the following informations:
 *
 *      - Data direction (input, output)
 *      - Number of bits (resolution)
 *      - Hardware channel number
 *      - Scaling (full scale value)
 *      - Initial output value
 *
 *      Mode can be HAL_OPEN_READ, HAL_OPEN_WRITE, or a combination of
 *      both. Inputs can be read from only, outputs can be read from or
 *      written to. Reading an output returns the last value written
 *      to the output.
 *
 *  \iparam  DeviceID = Logical device ID
 *  \iparam  Mode     = Access mode (read and/or write)
 *  \iparam  UserTag  = User tag to pass thru to interrupt handler
 *  \iparam  Handler  = Interrupt handler function
 *
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

Error_t halAnalogOpen (Device_t DeviceID, UInt16 Mode, UInt32 UserTag, halIntrHandler_t Handler) {

    const Int32 Index = halAnalogFindDevice(DeviceID);

    if (Index >= 0) {
        const halAnalogDescriptor_t *Channel = &halAnalogDescriptors[Index];

        if (DataTable[Index].Flags & HAL_OPEN_RW) {
            return (E_DEVICE_ALREADY_OPEN);
        }
        if ((Mode & HAL_OPEN_WRITE) && (Channel->Direction != DIR_OUTPUT)) {
            return (E_DEVICE_READ_ONLY);
        }
        DataTable[Index].Flags |= (Mode & HAL_OPEN_MODE);

        if (Channel->Interface == BUS_TYPE_INTERN && Channel->Direction == DIR_INPUT) {
            if (Handler != NULL) {
                // Enable end of conversion interrupt
                DataTable[Index].IntrVector.Handler = Handler;
                DataTable[Index].IntrVector.UserTag = UserTag;
                DataTable[Index].IntrActive = TRUE;
                ADC->CR1 |= ADC_CR1_EOCIE;
                halInterruptEnable (INTERRUPT_ADC1_2, IRQ_PRIO_DEFAULT);
            }
        }

        return (DEVICE_CLASS_ANALOG + Index);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Closes an analog input/output
 *
 *      Closes an open analog input or output channel.
 *
 *  \iparam  Handle = Handle of open analog channel
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halAnalogClose (Handle_t Handle) {

    const Int32 Index = halAnalogGetIndex(Handle, HAL_OPEN_RW);

    if (Index >= 0) {
        const halAnalogDescriptor_t *Channel = &halAnalogDescriptors[Index];
    
        DataTable[Index].Flags &= ~HAL_OPEN_MODE;
        
        if (Channel->Interface == BUS_TYPE_INTERN && Channel->Direction == DIR_INPUT) {
            DataTable[Index].IntrVector.Handler = NULL;
            DataTable[Index].IntrActive = FALSE;
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Read from an analog channel
 *
 *      Read a signed 32 bit analog value from the hardware analog/digital
 *      converter associated with Handle, scales it according to the HAL
 *      channel descriptor and returns the result in the buffer pointed to by
 *      the supplied 32 bit Buffer parameter.
 *
 *      If the channel is not open for reading, an error is retuned.
 *
 *  \iparam  Handle = Handle of open analog channel
 *  \iparam  Buffer = Pointer to a 32 bit buffer to store the result
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halAnalogRead32 (Handle_t Handle, Int32 *Buffer) {

    const Int32 Index = halAnalogGetIndex(Handle, HAL_OPEN_READ);
    Error_t Status;
    Int32 Value;

    if (Index >= 0) {
        const halAnalogDescriptor_t *Channel = &halAnalogDescriptors[Index];

        if (Channel->Interface == BUS_TYPE_INTERN) {
            if (Channel->Direction == DIR_INPUT) {
                Value = ScanBuffer[DataTable[Index].DataIdx] << 15;
            }
            else {
                Value = DAC->DOR[Channel->PortNo] << 19;
            }
        }
        else if (Channel->Interface == BUS_TYPE_AD779x) {
             if ((Status = halAd779xRead(Channel->PortNo, &Value)) < 0) {
                return (Status);
             }
        }
        else if (Channel->Interface == BUS_TYPE_PWM) {
            return (E_DEVICE_WRITE_ONLY);
        }
        else if (Channel->Interface == BUS_TYPE_SPI) {
            return (E_DEVICE_WRITE_ONLY);
        }
        else {
            return (E_PORT_INTERFACE_TYPE);
        }
        if (Buffer != NULL) {
            *Buffer = (Int64) Value * Channel->MaxValue / (MAX_UINT32 >> 1);
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Read from an analog channel
 *
 *      Read a signed 16 bit analog value from the hardware analog/digital
 *      converter associated with Handle, scales it according to the HAL
 *      channel descriptor and returns the result in the buffer pointed to by
 *      the supplied 16 bit Buffer parameter.
 *
 *      If the channel is not open for reading, an error is retuned. When this
 *      function is used, MaxValue in the analog descriptor has to be in the
 *      range of signed 16 bit integers.
 *
 *  \iparam  Handle = Handle of open analog channel
 *  \iparam  Buffer = Pointer to a 16 bit buffer to store the result
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halAnalogRead (Handle_t Handle, Int16 *Buffer) {

    Error_t Status;
    Int32 Value;
    
    if ((Status = halAnalogRead32 (Handle, &Value)) >= 0) {
        if (Value < MIN_INT16 || Value > MAX_INT16) {
            Status = E_ADC_INTEGER_OVERFLOW;
        }
        else {
            *Buffer = Value;
        }
    }
    
    return (Status);
}


/*****************************************************************************/
/*!
 *  \brief   Write to an analog channel
 *
 *      Writes an analog value to the hardware digital/analog converter
 *      associated with Handle. Before writing, the supplied Data is de-
 *      scaled according to the HAL channel descriptor.
 *
 *      If the channel is not open for writing, an error is returned.
 *
 *  \iparam  Handle = Handle of open analog channel
 *  \iparam  Data   = Data value to write
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halAnalogWrite (Handle_t Handle, UInt16 Data) {

    const Int32 Index = halAnalogGetIndex(Handle, HAL_OPEN_WRITE);
    UInt32 Value;

    if (Index >= 0) {
        const halAnalogDescriptor_t *Channel = &halAnalogDescriptors[Index];

        Value = (UInt32) Data * MAX_UINT16 / (UInt32) Channel->MaxValue;

        if (Channel->Interface == BUS_TYPE_INTERN) {
            DAC->DHR[Channel->PortNo].LEFT12 = Value;
        }
        else if (Channel->Interface == BUS_TYPE_AD779x) {
            return (E_DEVICE_READ_ONLY);
        }
        else if (Channel->Interface == BUS_TYPE_PWM) {
            Error_t Status = halPwmWrite (DataTable[Index].Handle, Channel->PortNo, Data);
            if (Status < NO_ERROR) {
                return (Status);
            }
        }
        else if (Channel->Interface == BUS_TYPE_SPI) {
            Error_t Status;

            Data = Data >> (16 - Channel->BitCount);
            Status = halSpiTransfer (DataTable[Index].Handle, (UInt8 *)&Data, (Channel->BitCount > 8) ? 2 : 1);
            if (Status < NO_ERROR) {
                return (Status);
            }
        }
        else {
            return (E_PORT_INTERFACE_TYPE);
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Get status of an analog channel
 *
 *      Returns the status of the analog channel associated with Handle. What
 *      status to return depends on the supplied StatusID. The following data
 *      can be requested:
 *
 *      - Maximum analog value (scaled)
 *      - Number of bits (hardware converter)
 *
 *      If an unsupported StatusID is supplied, a 0 is returned.
 *
 *  \iparam  Handle   = Handle of open analog channel
 *  \iparam  StatusID = Type of status
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halAnalogStatus (Handle_t Handle, AnalogStatID_t StatusID) {

    const Int32 Index = halAnalogGetIndex(Handle, HAL_OPEN_RW);

    if (Index >= 0) {
        const halAnalogDescriptor_t *Channel = &halAnalogDescriptors[Handle];

        switch (StatusID) {
            // return maximum data value
            case AIO_STAT_MAXVALUE:
                return (Channel->MaxValue);

            // return channel size (number of bits)
            case AIO_STAT_BITCOUNT:
                return (Channel->BitCount);
        }
        return (E_UNKNOWN_STATUS_ID);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Controls an analog channel
 *
 *      Controls the operation of the analog channel associated with Handle.
 *      What operation to control depends on the supplied ControlID. The
 *      following controls can be set:
 *
 *      - Enable channel interrupt
 *      - Disable channel interrupt
 *
 *      If an unsupported ControlID is supplied, a 0 is returned.
 *
 *  \iparam  Handle   = Handle of open analog channel
 *  \iparam  ControlID = Type of control
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halAnalogControl (Handle_t Handle, AnalogCtrlID_t ControlID) {

    const Int32 Index = halAnalogGetIndex(Handle, HAL_OPEN_RW);

    if (Index >= 0) {
        const halAnalogDescriptor_t *Channel = &halAnalogDescriptors[Index];

        if (Channel->Interface == BUS_TYPE_INTERN && Channel->Direction == DIR_INPUT) {
            switch (ControlID) {
                case AIO_INTR_ENABLE:
                    DataTable[Index].IntrActive = TRUE;
                    break;
                case AIO_INTR_DISABLE:
                    DataTable[Index].IntrActive = FALSE;
                    break;
                default:
                    return (E_UNKNOWN_CONTROL_ID);
            }
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Analog end of conversion interrupt handler
 *
 *      This function will be called by the vectored interrupt controller in
 *      case of a analog input interrupt. It passes the interrupt flag bits
 *      and the user tag. Before exiting, the handled interrupt flag bits are
 *      cleared.
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void halAnalogInterruptHandler (void) {

    UInt32 Index;
    const UInt32 IntrFlags = ADC->SR & ADC_SR_EOC & (ADC->CR1 >> 4);

    for (Index = 0; Index < halAnalogDescriptorCount; Index++) {
        if (DataTable[Index].IntrVector.Handler != NULL && DataTable[Index].IntrActive == TRUE) {
            DataTable[Index].IntrVector.Handler (DataTable[Index].IntrVector.UserTag, IntrFlags);
        }
    }
    ADC->SR = ~IntrFlags;
}


/*****************************************************************************/
/*!
 *  \brief   Get index of a channel
 *
 *      Returns the index of the analog channel associated with Handle.
 *      The index is used to address the entry in the logical channel
 *      desciptor associated with Handle.
 *
 *      The channel must be opened in a mode compliant to the supplied
 *      Mode, else an error is returned.
 *
 *  \iparam  Handle = Handle of open analog channel
 *  \iparam  Mode   = Open mode
 *
 *  \return  Channel index or (negative) error code
 *
 ****************************************************************************/

static Error_t halAnalogGetIndex (Handle_t Handle, UInt16 Mode) {

    register const UInt32 Index = Handle ^ DEVICE_CLASS_ANALOG;

    if (Index < halAnalogDescriptorCount) {

        register const UInt16 Flags = DataTable[Index].Flags;

        if (!(Flags & HAL_OPEN_RW)) {
            return (E_DEVICE_NOT_OPEN);
        }
        if (!(Flags & Mode)) {
            return (E_DEVICE_ACCESS_MODE);
        }
        return ((Error_t) Index);
    }
    return (E_DEVICE_HANDLE_INVALID);
}


/*****************************************************************************/
/*!
 *  \brief   Find logical channel index
 *
 *      Searches the logical channel descriptor for an entry with the asked
 *      DeviceID. If such an entry is found, the table index of that entry
 *      is returned. If no such entry exists, an error is returned.
 *
 *      The channel must not be open to use this function.
 *
 *  \iparam  DeviceID = Logical device ID
 *
 *  \return  Channel index or (negative) error code
 *
 ****************************************************************************/

static Error_t halAnalogFindDevice (Device_t DeviceID) {

    UInt32 Index;

    if ((DeviceID & DEVICE_CLASS_MASK) == DEVICE_CLASS_ANALOG) {

        halInitState_t InitState = halGetInitState();

        if (InitState == INIT_NOT_DONE || InitState == INIT_FAILED) {
            return (E_HAL_NOT_INITIALIZED);
        }
        for (Index=0; Index < halAnalogDescriptorCount; Index++) {

            if (halAnalogDescriptors[Index].DeviceID == DeviceID) {
                if (~DataTable[Index].Flags & HAL_FLAG_INITZED) {
                    return (E_DEVICE_UNCONFIGURED);
                }
                return ((Error_t)Index);
            }
        }
    }
    return (E_UNKNOWN_DEVICE_ID);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize analog output channels
 *
 *      Initializes the processor internal digital-to-analog converters by 
 *      applying the settings from the global analog i/o descriptor table. 
 *      That descriptor is defined in the HAL configuration module.
 *
 *      If the channel descriptor table contains errors, initialization
 *      is aborted and an error returned.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halAnalogInitOutputs (void) {

    const halAnalogDescriptor_t *Channel = halAnalogDescriptors;
    UInt32 Allocated = 0;
    UInt16 i;
    Error_t Status;

    for (i=0; i < halAnalogDescriptorCount; i++) {

        const UInt32 OutputNo = Channel->PortNo;

        if (Channel->Direction == DIR_OUTPUT) {
            if (Channel->Interface == BUS_TYPE_INTERN) {

                if (OutputNo >= halProcessorInfo.CountDAC) {
                    return (E_DAC_OUTPUT_NOT_EXISTS);
                }
                // Check if already allocated to another logical channel
                if (Allocated & BIT(OutputNo)) {
                   return (E_DAC_OUTPUT_USED_TWICE);
                }
                halPeripheralClockEnable (PERIPHERAL_DAC, ON);

                // Set initial data output value and enable output
                DAC->DHR[OutputNo].LEFT12 =
                    (UInt32)Channel->Initial * MAX_UINT16 / (UInt32) Channel->MaxValue;

                DAC->CR |= OutputNo ? DAC_CR_EN2 : DAC_CR_EN1;

                // Mark physical channel as used
                Allocated |= BIT(OutputNo);

                DataTable[i].Flags = HAL_FLAG_INITZED;
            }
            else if (Channel->Interface == BUS_TYPE_AD779x) {
                return (E_DEVICE_READ_ONLY);
            }
            else if (Channel->Interface == BUS_TYPE_PWM) {
                const PwmMode_t PwmMode = { TIM_MODE_COUNT_UP, TIM_MODE_INTERVAL, TIM_MODE_INTERNAL, 0 };

                DataTable[i].Handle = halPwmOpen (Channel->PeripheralID, Channel->PortNo, NULL);
                if (DataTable[i].Handle < NO_ERROR) {
                    return (DataTable[i].Handle);
                }
                Status = halPwmSetup (DataTable[i].Handle, PwmMode, Channel->MaxValue);
                if (Status < NO_ERROR) {
                    return (Status);
                }
                Status = halPwmWrite (DataTable[i].Handle, Channel->PortNo, Channel->Initial);
                if (Status < NO_ERROR) {
                    return (Status);
                }
                DataTable[i].Flags = HAL_FLAG_INITZED;
            }
            else if (Channel->Interface == BUS_TYPE_SPI) {
                const UInt32 SpiFormat = SPI_FORMAT_MSB1ST | SPI_FORMAT_CLK_PHASE2 | SPI_FORMAT_CLK_IDLE_1;
                UInt32 Initial = Channel->Initial;

                DataTable[i].Handle = halSpiOpen (Channel->PeripheralID, Channel->PortNo, NULL);
                if (DataTable[i].Handle < NO_ERROR) {
                    return (DataTable[i].Handle);
                }
                Status = halSpiSetup (DataTable[i].Handle, 0, SpiFormat);
                if (Status < NO_ERROR) {
                    return (Status);
                }
                Status = halSpiTransfer (DataTable[i].Handle, (UInt8 *)&Initial, (Channel->BitCount > 8) ? 2 : 1);
                if (Status < NO_ERROR) {
                    return (Status);
                }
                DataTable[i].Flags = HAL_FLAG_INITZED;
            }
        }
        Channel++;  // increment descriptor pointer
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize all analog input channels
 *
 *      Initializes the processor internal analog-to-digital converter by 
 *      applying the settings from the global analog i/o descriptor table. 
 *      That descriptor is defined in the HAL configuration module.
 *
 *      If the channel descriptor table contains errors, initialization
 *      is aborted and an error returned.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halAnalogInitInputs (void) {

    const halAnalogDescriptor_t *Channel = halAnalogDescriptors;
    UInt32 Sequence[3] = {0};
    UInt32 Sampling[2] = {0};
    UInt16 SequenceCnt = 0;
    UInt32 Allocated   = 0;
    UInt16 i;

    for (i=0; i < halAnalogDescriptorCount; i++) {

        const UInt32 InputNo = Channel->PortNo;

        if (Channel->Direction == DIR_INPUT) {
            if (Channel->Interface == BUS_TYPE_INTERN) {

                if (InputNo >= halProcessorInfo.CountADC) {
                    return (E_ADC_INPUT_NOT_EXISTS);
                }
                // Check if already allocated to another logical channel
                if (Allocated & BIT(InputNo)) {
                   return (E_ADC_INPUT_USED_TWICE);
                }
                // Setup channel sequence and sample rate registers
                Sampling[InputNo / 10] |=
                    Channel->ConvTime << (ADC_SMP_BITS * (InputNo % 10));
                Sequence[SequenceCnt / 6]  |=
                    Channel->PortNo << (ADC_SEQ_BITS * (SequenceCnt % 6));

                // Mark physical channel as used
                Allocated |= BIT(InputNo);

                DataTable[i].DataIdx = SequenceCnt++;
                DataTable[i].Flags = HAL_FLAG_INITZED;
            }
            else if (Channel->Interface == BUS_TYPE_AD779x) {
                Error_t Status = halAd779xAttach(InputNo);
                if (Status < 0) {
                    return (Status);
                }
                DataTable[i].Flags = HAL_FLAG_INITZED;
            }
            else if (Channel->Interface == BUS_TYPE_PWM) {
                return (E_DEVICE_WRITE_ONLY);
            }
            else if (Channel->Interface == BUS_TYPE_SPI) {
                return (E_DEVICE_WRITE_ONLY);
            }
        }
        Channel++;  // increment descriptor pointer
    }
    return (halAnalogInitScanner(SequenceCnt, Sequence, Sampling));
}


/*****************************************************************************/
/*!
 *  \brief   Initialize digital/analog scanner
 *
 *      Initializes the processor internal analog-to digital converter by
 *      applying the supplied parameters. Channels are the number of input
 *      channels, Sequence is the physical channel scan sequence, and
 *      Sampling is the sample time for each physical channel.
 *
 *      The ADC is configured to continuous scan mode using direct memory
 *      access to transfer the conversion results into memory. Only ADC1
 *      is used to convert the up to 16 analog inputs. Auto calibration
 *      is performed to enhance conversion accuracy.
 *
 *  \iparam  Channels = Number of input channels
 *  \iparam  Sequence = Sequence of inputs to scan
 *  \iparam  Sampling = Sampling time for each channel.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halAnalogInitScanner (
                UInt16 Channels, UInt32 *Sequence, UInt32 *Sampling) {

    Error_t Status;
    UInt16 i, k;

    if (Channels) {
        const UInt16 DmaMode = DMA_DEV_WIDTH16 | 
            DMA_MEM_WIDTH16 | DMA_MODE_INCREMENT | DMA_MODE_CIRCULAR;

        // enable clock and wakeup adc from standby
        halPeripheralClockEnable (PERIPHERAL_ADC1, ON);
        halPeripheralClockEnable (PERIPHERAL_DMA1, ON);
        ADC->CR2 = ADC_CR2_ADON;
        halShortDelay(5);

        // Allocate storage for the scan buffer
        ScanBuffer = calloc (Channels, sizeof(*ScanBuffer));
        if (ScanBuffer == NULL) {
            return (E_HEAP_MEMORY_FULL);
        }
        // Open DMA controller channel to scan inputs
        if ((Status = halDmaOpen (0, 0, NULL)) != NO_ERROR) {
            return (Status);
        }          
        // Setup DMA controller to transfer results into memory
        if ((Status = halDmaSetup (0, &ADC->DR, DmaMode)) != NO_ERROR) {
            return (Status);
        }
        // Start scanning with DMA controller
        if ((Status = halDmaRead (0, ScanBuffer, Channels)) != NO_ERROR) {
            return (Status);
        }
        // setup sample time and channel sequence registers
        for (i=0, k=1; i < ELEMENTS(ADC->SMPR); i++,k--) {
            ADC->SMPR[k] = Sampling[i];
        }
        for (i=0, k=2; i < ELEMENTS(ADC->SEQ); i++,k--) {
            ADC->SEQ[k] = Sequence[i];
        }
        ADC->SEQ[0] |= (Channels - 1) << 20;

        // Set continuous dma scan mode with left aligned data
        ADC->CR2 |= ADC_CR2_ALIGN | ADC_CR2_DMA | ADC_CR2_CONT;
        ADC->CR1 |= ADC_CR1_SCAN;
        ADC->CR2 |= ADC_CR2_ADON;

        // ADC auto calibration (approx. 6us @ 14MHz)
        ADC->CR2 |= ADC_CR2_CAL;
        while (ADC->CR2 & ADC_CR2_CAL) {}
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize analog inputs/outputs
 *
 *      Initializes this module by allocating memory for the variables
 *      for all analog channels and initializing the required hardware
 *      components.
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halAnalogInit (void) {

    Error_t Status;

    if (halGetInitState() == INIT_IN_PROGRESS) {
        if (halAnalogDescriptorCount) {
    
            DataTable = calloc(halAnalogDescriptorCount, sizeof(AioDevice_t));    
            if (DataTable == NULL) {
                return (E_HEAP_MEMORY_FULL);
            }
            if ((Status = halAd779xInit()) != NO_ERROR) {
                return (Status);
            }
            if ((Status = halAnalogInitOutputs()) != NO_ERROR) {
                return (Status);
            }
            if ((Status = halAnalogInitInputs()) != NO_ERROR) {
                return (Status);
            }
        }
    }
    return (NO_ERROR);
}

//****************************************************************************/
