/****************************************************************************/
/*! \file halPorts.c
 * 
 *  \brief  Digital port i/o
 *
 *  $Version: $ 0.1
 *  $Date:    $ 11.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to access and manage logical ports,
 *       convert them to physical (processor) ports and controls associated
 *       peripherals. Also included is the initialization of the processor 
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
#include "halCommon.h"
#include "halConfigure.h"
#include "halSystem.h"
#include "halError.h"
#include "halPorts.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define PROCESSOR_PIN_COUNT     88   //!< Number of physical processor pins
#define PROCESSOR_PORT_COUNT    3    //!< Number of physical processor ports
#define PROCESSOR_XINT_COUNT    7    //!< Number of external interrupt pins

#define MASK_INT_NUMBER(i)      ((i) & 0x0F)
#define MASK_INT_EDGE(i)        (((i) >> 4) & 3)
#define MASK_INT_ENABLE_BIT     1

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! Layout of processor internal GPIO data registers */
typedef struct {
    UInt32  Data;               //!< GPIO immediate data register
    UInt32  Set;                //!< GPIO bit set register
    UInt32  Clr;                //!< GPIO bit clear register
    UInt32  Toggle;             //!< GPIO bit toggle register
} GPIO_DATA_REGISTERS_t;

/*! Layout of processor internal GPIO control registers */
typedef struct {
    UInt32  Control;            //!< GPIO control register
    UInt32  QSel0;              //!< GPIO input qualification register 1
    UInt32  QSel1;              //!< GPIO input qualification register 2
    UInt32  Mux0;               //!< GPIO pin multiplex register 1
    UInt32  Mux1;               //!< GPIO pin multiplex register 2
    UInt32  Direction;          //!< GPIO data direction register
    UInt32  Pullup;             //!< GPIO pullup resistor disable register
    UInt32  Unused;             //!< Reserved (don't delete it)
} GPIO_CTRL_REGISTERS_t;

/*! Layout of processor internal GPIO interrupt select registers */
typedef struct {
    UInt16  Select;             //!< External interrupt select register
} GPIO_XINT_REGISTERS_t;

/*! Layout of processor external interrupt constrol registers */
typedef struct {
    UInt16  Control;            //!< External interrupt control register
} XINT_CTRL_REGISTERS_t;

/*! Structure to hold the state on a logical port */
typedef struct {
    UInt16 Flags;               //!< Port flags (access mode)
    UInt32 Bitmask;             //!< Port bitmask (derived from descriptor data)
} LOGICAL_PORT_DATA_t;


//****************************************************************************/
// Private Variables 
//****************************************************************************/

/*! Hardware registers to control processor internal ports */
static volatile GPIO_DATA_REGISTERS_t 
                    *GpioDataRegister = (GPIO_DATA_REGISTERS_t*) 0x6FC0;
static volatile GPIO_CTRL_REGISTERS_t 
                    *GpioCtrlRegister = (GPIO_CTRL_REGISTERS_t*) 0x6F80;
static volatile GPIO_XINT_REGISTERS_t 
                    *GpioXIntRegister = (GPIO_XINT_REGISTERS_t*) 0x6FE0;
static volatile XINT_CTRL_REGISTERS_t 
                    *XIntCtrlRegister = (XINT_CTRL_REGISTERS_t*) 0x7070;  

/*! Pin allocation bitmask for all processor ports and pins */
static UInt32 AllocationBitmask[PROCESSOR_PORT_COUNT];

/*! Data table holding the variables for all analog channels */
static LOGICAL_PORT_DATA_t *PortData;

//****************************************************************************/
// External Descriptor Tables 
//****************************************************************************/

extern const PIN_DEFINITION_t halPinDescriptorTable[];
extern const int halPinDescriptorTableLength;

extern const PIN_QUALIFICATION_t halPinSamplePeriodTable[];
extern const int halPinSamplePeriodTableLength;

extern const PORT_DESCRIPTOR_t halLogicalPortsTable[];
extern const int halLogicalPortsTableLength;


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static ERROR_t halInitializeLogicalPorts   (void);
static ERROR_t halInitializePinMapping     (void);
static ERROR_t halInitializeInputFiltering (void);
static ERROR_t halConnectPinToInterrupt    (const PIN_DEFINITION_t *PD);
static ERROR_t halUnInitializePorts        (void);

static ERROR_t halGetPortIndex (HANDLE_t Handle, UInt16 Mode);
static ERROR_t halSearchChannelID (CHANNEL_t ChannelID);
 
  
/*****************************************************************************/
/*! 
 *  \brief   Opens a digital input/output
 *
 *      Opens a digital input or output port and returns a handle to 
 *      be used to read/write from/to that port. The ChannelID must 
 *      be the numerical "name" of a digital port defined in the HAL
 *      configuration file. Additionally a port decriptor must exist
 *      in the external halLogicalPortsTable[] descibing the port. 
 *      This includes the following informations: 
 * 
 *      - GPIO port number  
 *      - Data direction (input, output)
 *      - Number of bits
 *      - Bit offset (shift count)
 *      - Polarity (inversion) bitmask
 *      - Initial output value
 * 
 *      Mode can be HAL_OPEN_READ, HAL_OPEN_WRITE, or a combination of
 *      both. Inputs can be read from only, outputs can be read from or 
 *      written to. Reading an output returns the real port state, if
 *      the hardware supports this. Otherwise the last value written 
 *      is returned.
 * 
 *  \iparam  ChannelID = Logical channel ID
 *  \iparam  Mode      = Access mode (read and/or write)
 * 
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

HANDLE_t halDigitalOpen (CHANNEL_t ChannelID, UInt16 Mode) 
{
    const Int32 Index = halSearchChannelID (ChannelID);

    if (Index >= 0) {
         
        const PORT_DESCRIPTOR_t *LPD = &halLogicalPortsTable[Index];
        
        if (PortData[Index].Flags & HAL_OPEN_RW) {
            return (halSetError(E_HAL_DEVICE_IN_USE));
        }
        if ((Mode & HAL_OPEN_WRITE) && (LPD->Direction == PORT_INPUT)) {
            return (halSetError(E_HAL_INVALID_MODE));
        }
        if (!(Mode & HAL_OPEN_WRITE) && (LPD->Direction == PORT_OUTPUT)) {
            return (halSetError(E_HAL_INVALID_MODE));
        }
        PortData[Index].Bitmask = 
            ~(MAX_UINT32 << LPD->BitCount) << LPD->Shifts;    
        PortData[Index].Flags = Mode;
        
        return (CHANNEL_CLASS_DIGIO + Index);
    }
    return (halSetError(Index));
}


/*****************************************************************************/
/*! 
 *  \brief   Closes a digital input/output
 *
 *      Closes a open digital input or output port. 
 * 
 *  \iparam  Handle = Handle of open digital port
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halDigitalClose (HANDLE_t Handle) 
{
    const Int32 Index = halGetPortIndex(Handle, HAL_OPEN_READ | HAL_OPEN_WRITE);
    
    if (Index >= 0) {          
        PortData[Index].Flags = 0;
        return (NO_ERROR);
    }    
    return (halSetError(Index));
}


/*****************************************************************************/
/*! 
 *  \brief   Read from a digital port
 *
 *      Reads the actual state from the hardware port associated with Handle,
 *      masks, shifts, and inverts it according to the settings in the HAL
 *      port descriptor and returns the result in the buffer pointed to by
 *      the supplied Buffer parameter. The port state is returned right
 *      justified, regardless of the bit position in the physical port.
 *  
 *      If the port is not open for reading, an error is returned.
 * 
 *  \iparam  Handle = Handle of open analog channel
 *  \iparam  Buffer = Pointer to a buffer to store the result
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halDigitalRead (HANDLE_t Handle, UInt16 *Buffer) 
{    
    const Int32 Index = halGetPortIndex(Handle, HAL_OPEN_READ);
    UInt32 Value;
  
    if (Index >= 0) {
        const PORT_DESCRIPTOR_t *LPD = &halLogicalPortsTable[Index]; 
        const UInt32 BitMask = PortData[Index].Bitmask;    
          
        if (LPD->Interface == PORT_TYPE_INTERN) { 
            Value = GpioDataRegister[LPD->PortNr].Data;
        }
        else {
            return (halSetError(E_HAL_PORT_TYPE));
        }
        if (!isPtrNull(Buffer)) {
            *Buffer = ((Value & BitMask) >> LPD->Shifts) ^ LPD->Polarity;
        }
        return (NO_ERROR);
    }    
    return (halSetError(Index));
}


/*****************************************************************************/
/*! 
 *  \brief   Write to a digital port
 *
 *      Writes a value to the hardware port associated with Handle. Before 
 *      writing, the supplied Data is masked, shifted, and inverted accord-
 *      ing to the settings in the HAL port descriptor. Data must be right
 *      justified, regardless of the bit position inside the physical port.
 *  
 *      If the port is not open for writing, an error is returned.
 * 
 *  \iparam  Handle = Handle of open analog channel
 *  \iparam  Data   = Data value to write
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halDigitalWrite (HANDLE_t Handle, UInt16 Data) 
{    
    const Int32 Index = halGetPortIndex(Handle, HAL_OPEN_WRITE);
    UInt32 Value;
    
    if (Index >= 0) {
        const PORT_DESCRIPTOR_t *LPD = &halLogicalPortsTable[Index]; 
        const UInt32 BitMask = PortData[Index].Bitmask;    
        
        Value = (((UInt32)Data ^ LPD->Polarity) << LPD->Shifts) & BitMask;
        
        if (LPD->Interface == PORT_TYPE_INTERN) {
            GpioDataRegister[LPD->PortNr].Clr = Value ^ BitMask;
            GpioDataRegister[LPD->PortNr].Set = Value;
        }
        else {
            return (halSetError(E_HAL_PORT_TYPE));
        }       
        return (NO_ERROR);
    }    
    return (halSetError(Index));
}


/*****************************************************************************/
/*! 
 *  \brief   Get status of a digital port
 *
 *      Returns the status of the analog channel associated with Handle. What
 *      status to return depends on the supplied StatusID. The following data
 *      can be requested:
 *  
 *      - Maximum digital value
 *      - Number of bits (logical port width)
 * 
 *      If a unsupported StatusID is supplied, a 0 is returned.
 * 
 *  \iparam  Handle   = Handle of a open port
 *  \iparam  StatusID = Type of status
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halDigitalStatus (HANDLE_t Handle, DIGIO_STATUS_ID_t StatusID)
{
    const Int32 Index = halGetPortIndex(Handle, HAL_OPEN_READ | HAL_OPEN_WRITE);
    
    if (Index >= 0) {
        const PORT_DESCRIPTOR_t *LPD = &halLogicalPortsTable[Index]; 
        
        switch (StatusID) {
            // return port size (number of bits)
            case DIGIO_STAT_BITCOUNT: 
                return (LPD->BitCount);    
    
            // return maximum port value
            case DIGIO_STAT_MAXVALUE: 
                return ~(MAX_UINT32 << LPD->BitCount);     
        }
        return (0);
    }    
    return (halSetError(Index));
}


/*****************************************************************************/
/*! 
 *  \brief   Get index of a port
 *
 *      Returns the index of the logical port associated with Handle. 
 *      The index is used to address the entry in the logical port 
 *      desciptor associated with Handle.
 *  
 *      The port must be opened in a mode compliant to the supplied
 *      Mode, else an error is returned.
 * 
 *  \iparam  Handle = Handle of open analog channel
 *  \iparam  Mode   = Open mode
 * 
 *  \return  Channel index or (negative) error code
 *
 ****************************************************************************/

static ERROR_t halGetPortIndex (HANDLE_t Handle, UInt16 Mode) 
{
    register const Int32 Index = Handle ^ CHANNEL_CLASS_DIGIO;

    if (Index < halLogicalPortsTableLength && Index >= 0) {
        
        register const UInt16 Flags = PortData[Index].Flags;
        
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
 *  \brief   Search logical port
 *
 *      Searches the logical port descriptor for an entry with the asked
 *      ChannelID. If such an entry is found, the table index of that entry
 *      is returned. If no such entry exists, an error is returned.
 *  
 *      The port must not be open to use this function.
 * 
 *  \iparam  ChannelID = Logical channel ID
 * 
 *  \return  Channel index or (negative) error code
 *
 ****************************************************************************/

static ERROR_t halSearchChannelID (CHANNEL_t ChannelID) 
{    
    Int32 Index;
    
    if ((ChannelID & CHANNEL_CLASS_MASK) == CHANNEL_CLASS_DIGIO) {
          
        for (Index=0; Index < halLogicalPortsTableLength; Index++) {
                      
            if (halLogicalPortsTable[Index].ChannelID == ChannelID) {
                return (Index);
            }
        }
    }
    return (E_HAL_INVALID_CHANNEL);
}


/*****************************************************************************/
/*! 
 *  \brief   Initialize logical ports
 *
 *      Sets up all logical ports defined in the halLogicalPortsTable by
 *      programming the data direction registers and the inital output
 *      value (in case of an output). The logical port descriptor is
 *      checked for validity. If it contains errors, initialization is 
 *      aborted and an error code is returned to the caller. To prevent
 *      using a pin more than once or use a pin that isn't configured
 *      as GPIO, a pin allocation bitmap is maintained.
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static ERROR_t halInitializeLogicalPorts (void) 
{    
    const PORT_DESCRIPTOR_t *LPD = halLogicalPortsTable;
    UInt16 i;
    
    for (i=0; i < halLogicalPortsTableLength; i++) {
        
        if (LPD->Interface == PORT_TYPE_INTERN) {
        
            const UInt32 Bitmask = ~(MAX_UINT32 << LPD->BitCount) << LPD->Shifts;  
            const UInt16 PortNr  = LPD->PortNr;
             
            // Check if legal port number given           
            if (PortNr >= PROCESSOR_PORT_COUNT) {
               return (E_HAL_PORT_DESCRIPTOR);
            }
            // Check if port bits already allocated to another port                   
            if (~AllocationBitmask[PortNr] & Bitmask) {
               return (E_HAL_PORT_DESCRIPTOR);
            }                   
            // Check if initial value fits into port width
            if ((LPD->Initial << LPD->Shifts) & ~Bitmask) {
               return (E_HAL_PORT_DESCRIPTOR);
            }
            // Check if polarity bitmask fits into port width
            if ((LPD->Polarity << LPD->Shifts) & ~Bitmask) {
               return (E_HAL_PORT_DESCRIPTOR);
            }
            if (LPD->Direction == PORT_OUTPUT) {
                
                // setting initial data value
                GpioDataRegister[PortNr].Clr = Bitmask; 
                GpioDataRegister[PortNr].Set = 
                    (LPD->Initial ^ LPD->Polarity) << LPD->Shifts; 
                    
                // setting data direction to "output"
                GpioCtrlRegister[PortNr].Direction |= Bitmask;
            }
            else {
                // setting data direction to "input"
                GpioCtrlRegister[PortNr].Direction &= ~Bitmask;
            }                       
            // mark pins as general purpose pins
            AllocationBitmask[PortNr] &= ~Bitmask;
        }
        LPD++; // increment port descriptor pointer
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Initialize pin mapping
 *
 *      Processor internal port pins can be used as general purpose I/O 
 *      or as special function I/Os. Special function I/Os are pins routed 
 *      to peripheral units inside the processor, like timers, capture   
 *      units, quadrature encoders, and many more. 
 * 
 *      Routing a function to a pin is called pin mapping. This is done by
 *      programming pin multiplexer registers inside the processor. Which
 *      mapping to use for each pin is taken from the pin definition table
 *      contained in the HAL configuration file.
 * 
 *      Beside configuring the pin multiplexers, additional pin related
 *      options are setup in this function: the optional pullup resistor
 *      for input pins, and the qualification enable. These parameters
 *      are taken from the pin definition table as well.
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static ERROR_t halInitializePinMapping (void) 
{
    const PIN_DEFINITION_t *PD = halPinDescriptorTable;
    UInt16 i;
    
    for (i=0; i < halPinDescriptorTableLength; i++) {
        
        const UInt16 PortNr = PD->PortNum;
        const UInt16 Shifts = PD->PortBit;
        
        if (PortNr >= PROCESSOR_PORT_COUNT) {
            return (E_HAL_PIN_DESCRIPTOR);
        }
        if (Shifts < 16) {
            GpioCtrlRegister[PortNr].Mux0  |= 
                (UInt32) PD->Multiplexer << (Shifts * 2);
            GpioCtrlRegister[PortNr].QSel0 |= 
                (UInt32) PD->Qualifier << (Shifts * 2);
        }
        else {
            GpioCtrlRegister[PortNr].Mux1  |= 
                (UInt32) PD->Multiplexer << ((Shifts-16) * 2);
            GpioCtrlRegister[PortNr].QSel1 |= 
                (UInt32) PD->Qualifier << ((Shifts-16) * 2);
        }
        // if pin defined as digital i/o, mark it as usable
        if (PD->Multiplexer == 0) {
            AllocationBitmask[PortNr] |= (1L << Shifts);
        }
        // if requested, connect pin to interrupt line       
        if (PD->Interrupt != NO_INTERRUPT) {
            halConnectPinToInterrupt(PD);
        }
        GpioCtrlRegister[PortNr].Pullup |= ~((UInt32)PD->Pullup << Shifts);
        
        PD++; // increment pin descriptor pointer
    }
    return (NO_ERROR);    
}


/*****************************************************************************/
/*! 
 *  \brief   Connect ports to interrupts
 *
 *      The processor offers the capability to use selected pins as external
 *      interrupt inputs. The appropriate registers are initialized in this
 *      function, based on the settings in the pin definition table.
 * 
 *  \iparam  PD = Pin definition table
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static ERROR_t halConnectPinToInterrupt (const PIN_DEFINITION_t *PD)
{
    const UInt16 PortNr = PD->PortNum;         
    
    if (PD->Interrupt != NO_INTERRUPT) {
        const UInt16 XInterruptNr = MASK_INT_NUMBER(PD->Interrupt);
        
        if ((XInterruptNr > PROCESSOR_XINT_COUNT) ||
            (XInterruptNr > 2 && PortNr == 0) ||
            (XInterruptNr < 3 && PortNr == 1) ||
            (PortNr > 1)) {
                               
            return (E_HAL_PIN_DESCRIPTOR);
        }
        XIntCtrlRegister[XInterruptNr].Control = 
            (MASK_INT_EDGE(PD->Interrupt) << 2) | MASK_INT_ENABLE_BIT;
            
        GpioXIntRegister[XInterruptNr].Select = PD->PortBit; 
    }
    return (NO_ERROR);    
}


/*****************************************************************************/
/*! 
 *  \brief   Initialize port qualification
 *
 *      The processor offers the capability to filter digital inputs by
 *      sampling pins a configurablle number of times before taking over
 *      a new state. This filtering is called 'Qualification' by the
 *      chip maker. Qualification can not be configured for each port
 *      bit, but byte-wise.
 * 
 *      The filtering is taken from the halPinSamplePeriodTable defined
 *      in the HAL configuration file.
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static ERROR_t halInitializeInputFiltering (void) 
{
    UInt16 i;

    for (i=0; i < halPinSamplePeriodTableLength; i++) {
            
        UInt16 PortNr = halPinSamplePeriodTable[i].PortNum;
        UInt16 Shifts = halPinSamplePeriodTable[i].ByteNum * 8;
        
        if (PortNr >= PROCESSOR_PORT_COUNT) {
            return (E_HAL_PIN_DESCRIPTOR);
        }
        GpioCtrlRegister[PortNr].Control |= 
            (UInt32) halPinSamplePeriodTable[i].SampleRate << Shifts; 
    }
    return (NO_ERROR);    
}


/*****************************************************************************/
/*! 
 *  \brief   Un-Initialize physical ports
 *
 *      Sets all hardware registers controlling processor internal ports
 *      to a reset (inactive) state. This function must be called before
 *      calling the hardware initialization functions to be sure that the
 *      hardware is in a known state.
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static ERROR_t halUnInitializePorts (void) 
{
    UInt16 i;
    
    for (i=0; i < PROCESSOR_PORT_COUNT; i++) {
        
        GpioCtrlRegister[i].Direction = 0;
        GpioCtrlRegister[i].Control   = 0;
        GpioCtrlRegister[i].Pullup    = 0;
        GpioCtrlRegister[i].Mux0      = 0;
        GpioCtrlRegister[i].Mux1      = 0;
        GpioCtrlRegister[i].QSel0     = 0;
        GpioCtrlRegister[i].QSel1     = 0;
        GpioDataRegister[i].Data      = 0;
    }
    for (i=0; i < PROCESSOR_XINT_COUNT; i++) {
        
        XIntCtrlRegister[i].Control = 0;
        GpioXIntRegister[i].Select  = 0;
    }         
    return (NO_ERROR);   
}


/*****************************************************************************/
/*! 
 *  \brief   Initialize digital input/output module
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

ERROR_t halInitializePorts (void) 
{
    ERROR_t Status;
    
    halUnInitializePorts();  // reset all port control registers
    
    PortData = calloc (halLogicalPortsTableLength, sizeof(LOGICAL_PORT_DATA_t));
    if (isPtrNull(PortData)) {
        return (halSetError(E_MEMORY_FULL));
    }
    if ((Status = halInitializePinMapping()) != NO_ERROR) {
        return (halSetError(Status));
    }
    if ((Status = halInitializeLogicalPorts()) != NO_ERROR) {
        return (halSetError(Status));
    }
    if ((Status = halInitializeInputFiltering()) != NO_ERROR) {
        return (halSetError(Status));
    }  
    halPeripheralClockEnable (PERIPHERAL_GPIO, ON);      
    
    return (NO_ERROR);
}

//****************************************************************************/
