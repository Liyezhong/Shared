/****************************************************************************/
/*! \file halAnalog.c
 * 
 *  \brief  Analog channel i/o
 *
 *  $Version: $ 0.1
 *  $Date:    $ 12.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to access and manage logical analog
 *       channels, maps them to physical (processor internal) analog/digital
 *       converters. Also included is the initialization of the processor 
 *       using external hardware configuration tables.
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
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
#include "halConfigure.h"
#include "halSystem.h"
#include "halTimer.h"
#include "halAnalog.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define PROCESSOR_ANALOG_CHANNELS   16      //!< Number of analog inputs

#define REFERENCE_INTERNAL          0       //!< Internal band-gap reference
#define REFERENCE_EXTERNAL_2048mV   0x4000  //!< External 2,048V reference
#define REFERENCE_EXTERNAL_1500mV   0x8000  //!< External 1,500V reference
#define REFERENCE_EXTERNAL_1024mV   0xC000  //!< External 1,024V reference

#define ADCCTRL1_ADC_RESET          0x4000  //!< ADC reset bit
#define ADCCTRL1_RUN_CONTINUOUS     0x0040  //!< Run continuous bit
#define ADCCTRL1_SEQ_CASCADE        0x0010  //!< Cascade sequencer bit
#define ADCCTRL2_RST_SEQ1           0x4000  //!< Reset sequencer 1 bit
#define ADCCTRL2_SOC_SEQ1           0x2000  //!< Start sequencer 1 bit
#define ADCCTRL3_BGRFDN             0x00C0  //!< Band gap reference bit mask
#define ADCCTRL3_BGRFDN_OFF         0x0000  //!< Band gap reference power off
#define ADCCTRL3_BGRFDN_ON          0x00C0  //!< Band gap reference power on
#define ADCCTRL3_POWER_UP           0x0020  //!< Power analog circuitry up
#define ADCCTRL3_POWER_DOWN         0x0000  //!< Power analog circuitry down
#define ADCCTRL3_SMODE_SEL          0x0001  //!< Sequencial sampling mode
#define ADCCTRL3_ADCCLKPS           0x001E  //!< Core clock divider bitmask

#define ADCLK_DIVIDER               0       //!< Core clock divider (hspclk/1)

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! Layout of processor internal A/D converter registers */
typedef struct {
    UInt16  AdcCtrl1;           //!< ADC Control Register 1
    UInt16  AdcCtrl2;           //!< ADC Control Register 2
    UInt16  AdcMaxConv;         //!< ADC Maximum Conversion Channels Register
    UInt16  AdcChSelSeq[4];     //!< ADC Channel Select Sequence Control 1..4
    UInt16  AdcAutoSeqSR;       //!< ADC Auto-Sequence Status Register
    UInt16  AdcResult[16];      //!< ADC Conversion Result Register 0..15
    UInt16  AdcCtrl3;           //!< ADC Control Register 3
    UInt16  AdcStatus;          //!< ADC Status Register
    UInt16  Reserved1[2];       //!< Reserved
    UInt16  AdcRefSel;          //!< ADC Reference Select Register
    UInt16  AdcOffTrim;         //!< ADC Offset Trim Register
    UInt16  Reserved2[2];       //!< Reserved    
} ADC_REGISTER_t;

/*! Structure to hold the state of a logical channel */
typedef struct {
    UInt16 Flags;               //!< Port flags (access mode)
} LOGICAL_CHANNEL_DATA_t;

//****************************************************************************/
// Private Variables 
//****************************************************************************/

/*! Hardware registers to control processor internal A/D converter */
static volatile ADC_REGISTER_t *AdcRegister = (ADC_REGISTER_t*) 0x7100;

/*! Data table holding the variables for all analog channels */
static LOGICAL_CHANNEL_DATA_t *ChannelData;

//****************************************************************************/
//! External Descriptor Tables defined in halConfigure
//****************************************************************************/

extern const CHANNEL_DESCRIPTOR_t halAnalogChannelTable[];
extern const int halAnalogChannelTableLength;

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static ERROR_t halSearchChannelID (CHANNEL_t ChannelID);
static ERROR_t halGetChannelIndex (HANDLE_t Handle, UInt16 Mode);
static ERROR_t halInitializeAnalogChannels (void);


/*****************************************************************************/
/*! 
 *  \brief   Opens an analog input/output
 *
 *      Opens an analog input or output channel and returns a handle to 
 *      be used to read/write from/to that channel. The ChannelID must 
 *      be the numerical "name" of an analog channel defined in the HAL
 *      configuration file. Additionally a channel decriptor must exist
 *      in the external halAnalogChannelTable[] descibing the channel. 
 *      This includes the following informations: 
 * 
 *      - Data direction (input, output)
 *      - Number of bits
 *      - Hardware channel number  
 *      - Scaling (full scale value)
 *      - Offset to add to data value
 *      - Initial output value
 * 
 *      Mode can be HAL_OPEN_READ, HAL_OPEN_WRITE, or a combination of
 *      both. Inputs can be read from only, outputs can be read from or 
 *      written to. Reading an output returns the last value written
 *      to the output.
 * 
 *  \iparam  ChannelID = Logical channel ID
 *  \iparam  Mode      = Access mode (read and/or write)
 * 
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

HANDLE_t halAnalogOpen (CHANNEL_t ChannelID, UInt16 Mode) 
{
    const Int32 Index = halSearchChannelID (ChannelID);

    if (Index >= 0) {        
        const CHANNEL_DESCRIPTOR_t *Channel = &halAnalogChannelTable[Index];
        
        if (ChannelData[Index].Flags & HAL_OPEN_RW) {
            return (halSetError(E_HAL_DEVICE_IN_USE));
        }
        if ((Mode & HAL_OPEN_READ) && (Channel->Direction != PORT_INPUT)) {
            return (halSetError(E_HAL_DEVICE_WRITE_ONLY));
        }
        if ((Mode & HAL_OPEN_WRITE) && (Channel->Direction != PORT_OUTPUT)) {
            return (halSetError(E_HAL_DEVICE_READ_ONLY));
        }
        ChannelData[Index].Flags = Mode;
        
        return (CHANNEL_CLASS_ANALOG + Index);
    }
    return (halSetError(Index));
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

ERROR_t halAnalogClose (HANDLE_t Handle) 
{
    const Int32 Index = halGetChannelIndex(Handle, HAL_OPEN_RW);
    
    if (Index >= 0) {
        ChannelData[Index].Flags = 0;        
        return (NO_ERROR);
    }    
    return (halSetError(Index));
}


/*****************************************************************************/
/*! 
 *  \brief   Read from an analog channel
 *
 *      Read an analog value from the hardware analog/digital converter
 *      associated with Handle, scales it according to the HAL channel
 *      descriptor and returns the result in the buffer pointed to by
 *      the supplied Buffer parameter.
 *  
 *      If the channel is not open for reading, an error is retuned.
 * 
 *  \iparam  Handle = Handle of open analog channel
 *  \iparam  Buffer = Pointer to a buffer to store the result
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halAnalogRead (HANDLE_t Handle, UInt16 *Buffer) {
    
    const Int32 Index = halGetChannelIndex(Handle, HAL_OPEN_READ);
    UInt32 Value;
  
    if (Index >= 0) {
        const CHANNEL_DESCRIPTOR_t *Channel = &halAnalogChannelTable[Index]; 
          
        if (Channel->Interface == PORT_TYPE_INTERN) { 
            Value = AdcRegister->AdcResult[Channel->ChannelNr];
        }
        else {
            return (halSetError(E_HAL_PORT_TYPE));
        }
        if (!isPtrNull(Buffer)) {
            *Buffer = (Value * Channel->MaxValue / MAX_UINT16) + Channel->Offset;
        }
        return (NO_ERROR);
    }    
    return (halSetError(Index));
    
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

ERROR_t halAnalogWrite (HANDLE_t Handle, UInt16 Data) 
{    
    const Int32 Index = halGetChannelIndex(Handle, HAL_OPEN_WRITE);
    UInt32 Value;
    
    if (Index >= 0) {
        const CHANNEL_DESCRIPTOR_t *Channel = &halAnalogChannelTable[Index]; 
        
        Value = (UInt32)(Data - Channel->Offset) * MAX_UINT16 / Channel->MaxValue;
        
        if (Channel->Interface == PORT_TYPE_INTERN) {
            return (halSetError(E_RESSOURCE_NOT_EXISTS));
        }
        else {
            return (halSetError(E_HAL_PORT_TYPE));
        }       
        //return (NO_ERROR);
    }    
    return (halSetError(Index));
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
 *      - Offset value (scaled)
 * 
 *      If a unsupported StatusID is supplied, a 0 is returned.
 * 
 *  \iparam  Handle   = Handle of open analog channel
 *  \iparam  StatusID = Type of status
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halAnalogStatus (HANDLE_t Handle, ANAIO_STATUS_ID_t StatusID)
{
    const Int32 Index = halGetChannelIndex(Handle, HAL_OPEN_RW);
    
    if (Index >= 0) {
        const CHANNEL_DESCRIPTOR_t *Channel = &halAnalogChannelTable[Handle]; 
  
        switch (StatusID) {
            // return maximum data value
            case ANAIO_STAT_MAXVALUE:
                return (Channel->MaxValue);
                
            // return channel size (number of bits)
            case ANAIO_STAT_BITCOUNT: 
                return (Channel->BitCount);    
    
            // return analog offset value
            case ANAIO_STAT_OFFSET:
                return (Channel->Offset);
        }
        return (0);
    }
    return (halSetError(E_UNSUPPORTED_FUNCTION));
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

static ERROR_t halGetChannelIndex (HANDLE_t Handle, UInt16 Mode) 
{
    register const Int32 Index = Handle ^ CHANNEL_CLASS_ANALOG;

    if (Index < halAnalogChannelTableLength && Index >= 0) {
        
        register const UInt16 Flags = ChannelData[Index].Flags;
        
        if (!(Flags & HAL_OPEN_RW)) {
            return (E_HAL_DEVICE_NOT_OPEN);
        }                        
        if (!(Flags & Mode)) {
            return (E_HAL_INVALID_MODE);
        }                        
        return (Index);
    }
    return (E_INVALID_HANDLE);
}


/*****************************************************************************/
/*! 
 *  \brief   Search logical channel
 *
 *      Searches the logical channel descriptor for an entry with the asked
 *      ChannelID. If such an entry is found, the table index of that entry
 *      is returned. If no such entry exists, an error is returned.
 *  
 *      The channel must not be open to use this function.
 * 
 *  \iparam  ChannelID = Logical channel ID
 * 
 *  \return  Channel index or (negative) error code
 *
 ****************************************************************************/

static ERROR_t halSearchChannelID (CHANNEL_t ChannelID) 
{    
    Int32 Index;
    
    if ((ChannelID & CHANNEL_CLASS_MASK) == CHANNEL_CLASS_ANALOG) {
          
        for (Index=0; Index < halAnalogChannelTableLength; Index++) {
                      
            if (halAnalogChannelTable[Index].ChannelID == ChannelID) {
                return (Index);
            }
        }
    }
    return (E_HAL_INVALID_CHANNEL);
}


/*****************************************************************************/
/*! 
 *  \brief   Initialize analog channels
 *
 *      Initializes the processor internal analog channels according to the
 *      settings configured in the extern channel descriptor by setting the
 *      appropriate peripheral registers. If the channel descriptor contains
 *      errors, initialization is aborted and an error code is returned.
 * 
 *      The ADC is configured to cascaded continuous mode using only the
 *      required channels. The internal band gap reference is used.
 *  
 *      This function is called only once during startup.
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static ERROR_t halInitializeAnalogChannels (void) {

    UInt16 AllocationBitmask = 0;
    UInt16 i, k;
    
    for (i=0; i < ELEMENTS(AdcRegister->AdcChSelSeq); i++) {
        AdcRegister->AdcChSelSeq[i] = 0;
    }
    AdcRegister->AdcCtrl1 = AdcRegister->AdcCtrl2 = 0;
    
    for (i=k=0; i < halAnalogChannelTableLength; i++) {       
        const UInt16 ChannelNr = halAnalogChannelTable[i].ChannelNr;
        
        if (halAnalogChannelTable[i].Interface == PORT_TYPE_INTERN) {
            
            if (ChannelNr >= PROCESSOR_ANALOG_CHANNELS) {
                return (halSetError(E_RESSOURCE_NOT_EXISTS)); 
            }
            if (halAnalogChannelTable[i].Direction != PORT_INPUT) {
                return (halSetError(E_RESSOURCE_NOT_EXISTS));
            }
            // Check if already allocated to another logical channel                   
            if (AllocationBitmask & (1 << ChannelNr)) {
               return (halSetError(E_HAL_DEVICE_IN_USE));
            }
            // Mark physical channel as used
            AllocationBitmask |= (1 << ChannelNr);
                               
            // each register holds up to 4 channels
            AdcRegister->AdcChSelSeq[k / 4] |= ChannelNr << (4 * (k % 4));
            AdcRegister->AdcMaxConv = k++;
            
            halPeripheralClockEnable (PERIPHERAL_ADC, ON);
        }
    }    
    // Enable bandgap reference and analog circuits 
    AdcRegister->AdcRefSel = REFERENCE_INTERNAL;
    AdcRegister->AdcCtrl3  = ADCCTRL3_BGRFDN_ON + ADCCTRL3_POWER_UP;
    AdcRegister->AdcCtrl3 |= (ADCLK_DIVIDER << 1) & ADCCTRL3_ADCCLKPS;
       
    halShortDelay (5000); // wait 5ms for analog circuitry to come up
    
    // Reset and start the sequencer in continuous mode
    AdcRegister->AdcCtrl1 = ADCCTRL1_RUN_CONTINUOUS + ADCCTRL1_SEQ_CASCADE;
    AdcRegister->AdcCtrl2 = ADCCTRL2_RST_SEQ1;
    AdcRegister->AdcCtrl2 = ADCCTRL2_SOC_SEQ1;
    
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Initialize analog input/output module
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

ERROR_t halAnalogInitialize (void) 
{
    ChannelData = calloc (
        halAnalogChannelTableLength, sizeof(LOGICAL_CHANNEL_DATA_t));
        
    if (isPtrNull(ChannelData)) {
        return (halSetError(E_MEMORY_FULL));
    }
    return (halInitializeAnalogChannels());
}

//****************************************************************************/
