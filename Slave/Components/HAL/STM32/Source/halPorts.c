/****************************************************************************/
/*! \file halPorts.c
 *
 *  \brief  Digital port i/o
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage logical ports,
 *      mapping them to physical GPIO ports. The pin configuration is also
 *      done in this module, using external configuration tables which are
 *      contained in the HAL configuration file.
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
#include "halClocks.h"
#include "halError.h"
#include "halMain.h"
#include "halPorts.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

//! GPIO register bit definitions
#define GPIO_CR_MODE       0x3      //!< Control register mode bit mask
#define GPIO_CR_MODE0      0x0      //!< Control register mode = 0
#define GPIO_CR_MODE1      0x1      //!< Control register mode = 1
#define GPIO_CR_MODE2      0x2      //!< Control register mode = 2
#define GPIO_CR_MODE3      0x3      //!< Control register mode = 3

#define GPIO_CR_CNFG       0xC      //!< Control register config bit mask
#define GPIO_CR_CNFG0      0x0      //!< Control register config = 0
#define GPIO_CR_CNFG1      0x4      //!< Control register config = 1
#define GPIO_CR_CNFG2      0x8      //!< Control register config = 2
#define GPIO_CR_CNFG3      0xC      //!< Control register config = 3

//! GPIO lock register keywords
#define GPIO_LOCK_LCKK     0x10000  //!< GPIO lock key
#define GPIO_LOCK_LCK      0x0FFFF  //!< All port lock bits

//! Macro to shift mode bits to the correct register position */
#define SHIFT_MODE_BITS(val,bit)   (((UInt32)(val)) << (((bit) % 8) * 4))


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Register layout to control processor internal ports
typedef volatile struct {
    UInt32 CR[2];           //!< GPIO port configuration registers
    UInt32 IDR;             //!< GPIO port input data register
    UInt32 ODR;             //!< GPIO port output data register
    UInt32 BSR;             //!< GPIO port bit set/reset register
    UInt32 BRR;             //!< GPIO port bit reset register
    UInt32 LOCK;            //!< GPIO port configuration lock register
} GpioRegFile_t;

//! Structure to hold the state of a physical port
typedef struct {
    GpioRegFile_t *Regs;    //!< GPIO register file pointer
    UInt16 PeripheralID;    //!< GPIO peripheral identifier
    UInt16 Initialized;     //!< Pin initialized flag
    UInt16 Allocated;       //!< GPIO bit allocation bitmap
    UInt16 Direction;       //!< GPIO data direction bitmap
    UInt32 Mode[2];         //!< GPIO mode configuration
} PortPhysicalData_t;

//! Structure to define valid pin types
typedef struct {
    UInt16 Type   :4;       //!< Pin type (in/out/analog)
    UInt16 Option :4;       //!< Pin option (pull up/down, open drain
    UInt16 Mode   :4;       //!< Pin mode
} PhysicalPinType_t;

//! Structure to hold the state of a logical port
typedef struct {
    UInt16 Flags;           //!< Port flags (access mode)
    UInt32 Bitmask;         //!< Port bitmask (derived from descriptor data)
} PortLogicalData_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Hardware registers to control processor internal ports
static PortPhysicalData_t GPIO[] = {
    { (GpioRegFile_t*) 0x40010800, PERIPHERAL_GPIO_A },
    { (GpioRegFile_t*) 0x40010C00, PERIPHERAL_GPIO_B },
    { (GpioRegFile_t*) 0x40011000, PERIPHERAL_GPIO_C },
    { (GpioRegFile_t*) 0x40011400, PERIPHERAL_GPIO_D },
    { (GpioRegFile_t*) 0x40011800, PERIPHERAL_GPIO_E },
    { (GpioRegFile_t*) 0x40011C00, PERIPHERAL_GPIO_F },
    { (GpioRegFile_t*) 0x40012000, PERIPHERAL_GPIO_G }
};

//! Data table holding the variables for all analog channels
static PortLogicalData_t *DataTable;


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t halPortFindDevice (Device_t PortID);
static Error_t halPortGetIndex (Handle_t Handle, UInt16 Mode);
static Error_t halPortInitChannels (void);

static Error_t halPinInitMappings (void);
static Error_t halPinGetMode (const halPinDesciptor_t *Descriptor);
static Error_t halPinActivateMode (void);


/*****************************************************************************/
/*!
 *  \brief   Opens a digital input/output
 *
 *      Opens a digital input or output port and returns a handle to
 *      be used to read/write from/to that port. DeviceID must be the
 *      numerical name of a digital port defined in the configuration
 *      file. Additionally a port decriptor must exist in the external 
 *      halLogicalPortsTable[] descibing the port. This describtor 
 *      contains the following informations:
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
 *  \iparam  PortID = Logical port ID
 *  \iparam  Mode   = Access mode (read and/or write)
 *
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

Error_t halPortOpen (Device_t PortID, UInt16 Mode) {

    const Int32 Index = halPortFindDevice(PortID);

    if (Index >= 0) {

        const halPortDescriptor_t *LPD = &halPortDescriptors[Index];

        if (DataTable[Index].Flags & HAL_OPEN_RW) {
            return (E_DEVICE_ALREADY_OPEN);
        }
        if ((Mode & HAL_OPEN_WRITE) && (LPD->Direction == DIR_INPUT)) {
            return (E_DEVICE_READ_ONLY);
        }
        if (!(Mode & HAL_OPEN_WRITE) && (LPD->Direction == DIR_OUTPUT)) {
            return (E_DEVICE_ACCESS_MODE);
        }
        DataTable[Index].Bitmask =
            ~(MAX_UINT32 << LPD->BitCount) << LPD->Shifts;
        DataTable[Index].Flags |= (Mode & HAL_OPEN_MODE);

        return ((PortID & DEVICE_CLASS_MASK) + Index);
    }
    return (Index);
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

Error_t halPortClose (Handle_t Handle) {

    const Int32 Index = halPortGetIndex(Handle, HAL_OPEN_READ | HAL_OPEN_WRITE);

    if (Index >= 0) {
        DataTable[Index].Flags &= ~HAL_OPEN_MODE;
        return (NO_ERROR);
    }
    return (Index);
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

Error_t halPortRead (Handle_t Handle, UInt16 *Buffer) {

    const Int32 Index = halPortGetIndex(Handle, HAL_OPEN_READ);
    UInt32 Value;

    if (Index >= 0) {
        const halPortDescriptor_t *LPD = &halPortDescriptors[Index];
        const UInt32 BitMask = DataTable[Index].Bitmask;

        if (LPD->Interface == BUS_TYPE_INTERN) {
            Value = GPIO[LPD->PortNo].Regs->IDR;
        }
        else {
            return (E_PORT_INTERFACE_TYPE);
        }
        if (Buffer != NULL) {
            *Buffer = ((Value & BitMask) >> LPD->Shifts) ^ LPD->Polarity;
        }
        return (NO_ERROR);
    }
    return (Index);
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

Error_t halPortWrite (Handle_t Handle, UInt16 Data) {

    const Int32 Index = halPortGetIndex(Handle, HAL_OPEN_WRITE);
    UInt32 Value;

    if (Index >= 0) {
        const halPortDescriptor_t *LPD = &halPortDescriptors[Index];
        const UInt32 BitMask = DataTable[Index].Bitmask;

        Value = (((UInt32)Data ^ LPD->Polarity) << LPD->Shifts) & BitMask;

        if (LPD->Interface == BUS_TYPE_INTERN) {
            GPIO[LPD->PortNo].Regs->BRR = Value ^ BitMask;
            GPIO[LPD->PortNo].Regs->BSR = Value;
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

Error_t halPortStatus (Handle_t Handle, PortStatID_t StatusID) {

    const Int32 Index = halPortGetIndex(Handle, HAL_OPEN_READ | HAL_OPEN_WRITE);

    if (Index >= 0) {
        const halPortDescriptor_t *LPD = &halPortDescriptors[Index];

        switch (StatusID) {
            // return port size (number of bits)
            case PORT_STAT_BITCOUNT:
                return (LPD->BitCount);

            // return maximum port value
            case PORT_STAT_MAXVALUE:
                return ~(MAX_UINT32 << LPD->BitCount);
        }
        return (E_UNKNOWN_STATUS_ID);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Get physical port address
 *
 *      Returns physical address of the port set/reset register the Handle
 *      belongs to. If Bitmask is not NULL, the bit mask to isolate the pin
 *      group is returned.
 *
 *      ATTENTION: This function is for internal use of the HAL only. It is
 *      strictly forbidden to use it from outside the HAL.
 *
 *  \iparam  Handle  = Handle of open digital channel
 *  \oparam  Bitmask = Pointer to variable to return pin mask
 *
 *  \return  Address of port register
 *
 ****************************************************************************/

UInt32 *halPortAddress (Handle_t Handle, UInt32 *Bitmask) {

    const Int32 Index = halPortGetIndex(Handle, HAL_OPEN_READ | HAL_OPEN_WRITE);

    if (Index >= 0) {
        const halPortDescriptor_t *LPD = &halPortDescriptors[Index];

        if (Bitmask != NULL) {
            *Bitmask = DataTable[Index].Bitmask;
        }
        return ((UInt32*)&GPIO[LPD->PortNo].Regs->BSR);
    }
    return (NULL);
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

static Error_t halPortGetIndex (Handle_t Handle, UInt16 Mode) {

    register const UInt32 Index = Handle & DEVICE_INDEX_MASK;

    if ((DEVICE_CLASS_DIGIN != (Handle & DEVICE_CLASS_MASK)) &&
        (DEVICE_CLASS_DIGOUT != (Handle & DEVICE_CLASS_MASK))) {
        return (E_DEVICE_HANDLE_INVALID);
    }

    if (Index < halPortDescriptorCount) {

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
 *  \brief   Find logical port channel
 *
 *      Searches the logical port descriptor for an entry with the asked
 *      DeviceID. If such an entry is found, the table index of that entry
 *      is returned. If no such entry exists, an error is returned.
 *
 *      The port must not be open to use this function.
 *
 *  \iparam  PortID = Logical port ID
 *
 *  \return  Channel index or (negative) error code
 *
 ****************************************************************************/

static Error_t halPortFindDevice (Device_t PortID) {

    UInt32 Index;

    if (((PortID & DEVICE_CLASS_MASK) == DEVICE_CLASS_DIGIN) ||
        ((PortID & DEVICE_CLASS_MASK) == DEVICE_CLASS_DIGOUT)) {

        halInitState_t InitState = halGetInitState();

        if (InitState == INIT_NOT_DONE || InitState == INIT_FAILED) {
            return (E_HAL_NOT_INITIALIZED);
        }

        for (Index=0; Index < halPortDescriptorCount; Index++) {

            if (halPortDescriptors[Index].DeviceID == PortID) {
                if (~DataTable[Index].Flags & HAL_FLAG_INITZED) {
                    return (E_DEVICE_UNCONFIGURED);
                }
                return ((Error_t) Index);
            }
        }
    }
    return (E_UNKNOWN_DEVICE_ID);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize logical ports
 *
 *      Sets up all logical ports defined in the halLogicalPortsTable. The
 *      logical port descriptor is checked for validity. If it contains
 *      errors, initialization is aborted and an error code is returned
 *      to the caller. To prevent using a pin more than once or use a pin
 *      that isn't configured as GPIO, a pin allocated bitmap is maintained.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halPortInitChannels (void) {

    const halPortDescriptor_t *LPD = halPortDescriptors;
    UInt16 i;

    for (i=0; i < halPortDescriptorCount; i++) {

        if (LPD->Interface == BUS_TYPE_INTERN) {

            const UInt32 Bitmask = ~(MAX_UINT32 << LPD->BitCount) << LPD->Shifts;
            const UInt16 PortNo  = LPD->PortNo;

            // Check if legal port number given
            if (PortNo >= halProcessorInfo.CountPort ||
                PortNo >= ELEMENTS(GPIO)) {
               return (E_PORT_NOT_EXISTS);
            }
            // Check if port bits already allocated to another port
            if (~GPIO[PortNo].Allocated & Bitmask) {
               return (E_PIN_ALLOCATED_TWICE);
            }
            // Check if initial value fits into port width
            if ((LPD->Initial << LPD->Shifts) & ~Bitmask) {
               return (E_PORT_DEFAULT_VALUE);
            }
            // Check if polarity bitmask fits into port width
            if ((LPD->Polarity << LPD->Shifts) & ~Bitmask) {
               return (E_PORT_POLARITY_MASK);
            }
            if (LPD->Direction == DIR_INPUT) {
                // Check if all pins have the desired direction
                if (GPIO[PortNo].Direction & Bitmask) {
                   return (E_PORT_DIRECTION);
                }
            }
            // In case of output set initial data value
            if (LPD->Direction == DIR_OUTPUT) {
                // Check if all pins have the desired direction
                if (~GPIO[PortNo].Direction & Bitmask) {
                   return (E_PORT_DIRECTION);
                }
                GPIO[PortNo].Regs->BRR = Bitmask;
                GPIO[PortNo].Regs->BSR = (LPD->Initial ^ LPD->Polarity) << LPD->Shifts;
            }
            // mark pins as general purpose pins
            GPIO[PortNo].Allocated &= ~Bitmask;
            DataTable[i].Flags = HAL_FLAG_INITZED;

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
 *      contained in the global HAL configuration file.
 *
 *      Beside configuring the pin multiplexers, additional pin related
 *      options are setup in this function: the optional pullup/pulldown
 *      resistors for input pins, and the open-drain option for outputs.
 *      These parameters are taken from the pin definition table as well.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halPinInitMappings (void) {

    const halPinDesciptor_t *Descriptor = halPinDescriptors;
    Error_t Mode;
    UInt16 i;

    for (i=0; i < halPinDescriptorCount; i++) {

        const UInt16 PortNo  = Descriptor->PortNo;
        const UInt16 PortBit = Descriptor->PortBit;
        const UInt16 GpioNo  = PortBit + (PortNo * BITS_PER_PORT);

        if (PortNo >= halProcessorInfo.CountPort ||
            PortNo >= ELEMENTS(GPIO)) {
            return (E_PORT_NOT_EXISTS);
        }
        if (PortBit >= BITS_PER_PORT) {
            return (E_PIN_NUMBER_INVALID);
        }
        if (GpioNo >= halProcessorInfo.CountGPIO) {
            return (E_PIN_NUMBER_INVALID);
        }
        if (GPIO[PortNo].Initialized & BIT(PortBit)) {
            return (E_PIN_ALLOCATED_TWICE);
        }
        if ((Mode = halPinGetMode(Descriptor)) < 0) {
            return (Mode);
        }
        halPeripheralClockEnable (GPIO[PortNo].PeripheralID, ON);

        if (Descriptor->Option == PIN_OPT_PULLUP) {
            GPIO[PortNo].Regs->BSR = BIT(PortBit);
        }
        else if (Descriptor->Option == PIN_OPT_PULLDN) {
            GPIO[PortNo].Regs->BRR = BIT(PortBit);
        }
        if (Descriptor->Multiplex == PIN_MUX_GPIO) {
            if (Descriptor->Type == PIN_TYP_DINPUT) {
                GPIO[PortNo].Direction &= ~BIT(PortBit);
            }
            else {
                GPIO[PortNo].Direction |= BIT(PortBit);
            }
            GPIO[PortNo].Allocated |= BIT(PortBit);
        }
        GPIO[PortNo].Initialized |= BIT(PortBit);

        // Setup configuration (assigned to control register later)
        GPIO[PortNo].Mode[PortBit/8] =
            (~SHIFT_MODE_BITS(0x0F, PortBit) & GPIO[PortNo].Mode[PortBit/8]) |
            ( SHIFT_MODE_BITS(Mode, PortBit));

        Descriptor++; // increment pin descriptor pointer
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Get pin mode
 *
 *      Analyses the supplied pin descriptor and returns the matching mode
 *      and configuration bits to assign to the GPIO control register.
 *      If the pin descriptor contains invalid settings or a combination
 *      of settings that isn't supported, an error is returned.
 *
 *  \iparam  Descriptor = Physical pin descriptor
 *
 *  \return  Mode or (negative) error code
 *
 ****************************************************************************/

static Error_t halPinGetMode (const halPinDesciptor_t *Descriptor) {

    const PhysicalPinType_t halPinTypeTable[] = {
        { PIN_TYP_ANALOG, PIN_OPT_NONE,   GPIO_CR_MODE0 | GPIO_CR_CNFG0 },
        { PIN_TYP_DINPUT, PIN_OPT_NONE,   GPIO_CR_MODE0 | GPIO_CR_CNFG1 },
        { PIN_TYP_DINPUT, PIN_OPT_PULLUP, GPIO_CR_MODE0 | GPIO_CR_CNFG2 },
        { PIN_TYP_DINPUT, PIN_OPT_PULLDN, GPIO_CR_MODE0 | GPIO_CR_CNFG2 },
        { PIN_TYP_DOUT2,  PIN_OPT_NONE,   GPIO_CR_MODE2 | GPIO_CR_CNFG0 },
        { PIN_TYP_DOUT10, PIN_OPT_NONE,   GPIO_CR_MODE1 | GPIO_CR_CNFG0 },
        { PIN_TYP_DOUT50, PIN_OPT_NONE,   GPIO_CR_MODE3 | GPIO_CR_CNFG0 },
        { PIN_TYP_DOUT2,  PIN_OPT_OPENDR, GPIO_CR_MODE2 | GPIO_CR_CNFG1 },
        { PIN_TYP_DOUT10, PIN_OPT_OPENDR, GPIO_CR_MODE1 | GPIO_CR_CNFG1 },
        { PIN_TYP_DOUT50, PIN_OPT_OPENDR, GPIO_CR_MODE3 | GPIO_CR_CNFG1 }
    };
    const UInt16 IoType = Descriptor->Type;
    const UInt16 Option = Descriptor->Option;
    UInt16 i;

    if (Descriptor->Multiplex == PIN_MUX_GPIO && IoType == PIN_TYP_ANALOG) {
        return (E_PIN_TYPE_MISMATCH);
    }

    for (i=0; i < ELEMENTS(halPinTypeTable); i++) {
        if (IoType == halPinTypeTable[i].Type &&
            Option == halPinTypeTable[i].Option) {

            UInt16 Mode = halPinTypeTable[i].Mode;

            if (Descriptor->Multiplex == PIN_MUX_SFIO) {
                if (Mode & GPIO_CR_MODE) {
                    Mode |= GPIO_CR_CNFG2;
                }
            }
            return (Mode);
        }
    }
    return (E_PIN_TYPE_MISMATCH);
}


/*****************************************************************************/
/*!
 *  \brief   Activate pin mode and configuration
 *
 *      Activates the prepared pin configuration by assigning the mode and
 *      configuration patterns to the GPIO configuration registers. After
 *      that the GPIO configuration is locked to prevent for accidental 
 *      modifications. 
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halPinActivateMode (void) {

    UInt32 PortCount = MIN (halProcessorInfo.CountPort, ELEMENTS(GPIO));
    UInt32 i;

    for (i=0; i < PortCount; i++) {
        GPIO[i].Regs->CR[0] = GPIO[i].Mode[0];
        GPIO[i].Regs->CR[1] = GPIO[i].Mode[1];
        GPIO[i].Regs->LOCK  = GPIO_LOCK_LCK | GPIO_LOCK_LCKK;
        GPIO[i].Regs->LOCK  = GPIO_LOCK_LCK;
        GPIO[i].Regs->LOCK  = GPIO_LOCK_LCK | GPIO_LOCK_LCKK;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Activate pin mode and configuration
 *
 *      It is a hardware requirement that unused pins should not be open.
 *      Therefore, all pins are configured as input with internal pull down
 *      resistors here. In the later initialization these settings will be
 *      overwritten for all pins that are defined in the hardware config-
 *      uration file, leaving only the unused pins in the default mode.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halPinSetToDefault (void) {

    UInt32 PortCount = MIN (halProcessorInfo.CountPort, ELEMENTS(GPIO));
    UInt32 i;

    for (i=0; i < PortCount; i++) {
        GPIO[i].Mode[0]   = 0x44444444;
        GPIO[i].Mode[1]   = 0x44444444;
        GPIO[i].Regs->BRR = MAX_UINT32;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize digital input/output module
 *
 *      Initializes this module by allocating memory for the variables
 *      for all logical ports and initializing the required hardware
 *      components. The pin configuration is not yet activated; this
 *      should be done after all peripherals are initialized.
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halPortInit (void) {

    Error_t Status;

    if (halGetInitState() == INIT_IN_PROGRESS) {

        DataTable = calloc (halPortDescriptorCount, sizeof(PortLogicalData_t));
        if (DataTable == NULL) {
            return (E_HEAP_MEMORY_FULL);
        }        
        // Preset all pins to input mode with pull-down resistor
        if ((Status = halPinSetToDefault()) != NO_ERROR) {
            return (Status);
        }
        // Initialize physical pin mappings
        if ((Status = halPinInitMappings()) != NO_ERROR) {
            return (Status);
        }
        // Initialize logical ports
        if ((Status = halPortInitChannels()) != NO_ERROR) {
            return (Status);
        }
        // Activate pin mode and configuration
        if ((Status = halPinActivateMode()) != NO_ERROR) {
            return (Status);
        }
    }
    return (NO_ERROR);
}

//****************************************************************************/
