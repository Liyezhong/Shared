/****************************************************************************/
/*! \file halExtIntr.c
 *
 *  \brief  Extern interrupt input handling
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and control external
 *      interrupt inputs. Also the alternate function pin initialization
 *      is done in this module, based on the settings in the global HAL
 *      configuration module.
 *
 *      Each port pin of the microcontroller can be used as an external
 *      interrupt input, but there are only 16 external interrupt lines.
 *      Bit<n> of each port can be connected to interrupt line<n>. Each
 *      interrupt line can handle only one interrupt input. This rules
 *      lead to restrictions in using port bits as interrupt inputs.
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
#include "halCommon.h"
#include "halError.h"
#include "halClocks.h"
#include "halMain.h"
#include "halInterrupt.h"
#include "halUtilities.h"
#include "halExtIntr.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define MAPPING_FORCE_0   0x0810  //!< Inhibit TIM1/6/7 DMA channel remap
#define MAPPING_FORCE_1   0x2000  //!< Force DMA2.5 interrupt to vector 60 

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Register layout to define external interrupt inputs
typedef volatile struct {
    UInt32 IMR;             //!< Interrupt mask register
    UInt32 EMR;             //!< Event mask register
    UInt32 RTSR;            //!< Rising trigger selection register
    UInt32 FTSR;            //!< Falling trigger selection register
    UInt32 SWIER;           //!< Software interrupt event register
    UInt32 PR;              //!< Pending register
} ExtiRegFile_t;

//! Register layout to define alternate function pins
typedef volatile struct {
    UInt32 EVCR;            //!< Event control register
    UInt32 MAPR1;           //!< Alternate function remap register 1
    UInt32 EXTICR[4];       //!< External interrupt input register 1...4
    UInt32 MAPR2;           //!< Alternate function remap register 2
} AfioRegFile_t;

//! Structure to hold the state of an external interrupt
typedef struct {
    UInt16 InterruptID;     //!< Interrupt identifier
    UInt16 Flags;           //!< Open flags
} ExtiDevice_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Data table holding the variables for all external interrupt channels
static ExtiDevice_t DataTable[] = {
    { INTERRUPT_EXTERN0 },      //!< External interrupt 0
    { INTERRUPT_EXTERN1 },      //!< External interrupt 1
    { INTERRUPT_EXTERN2 },      //!< External interrupt 2
    { INTERRUPT_EXTERN3 },      //!< External interrupt 3
    { INTERRUPT_EXTERN4 },      //!< External interrupt 4
    { INTERRUPT_EXTERNx },      //!< External interrupt 5
    { INTERRUPT_EXTERNx },      //!< External interrupt 6
    { INTERRUPT_EXTERNx },      //!< External interrupt 7
    { INTERRUPT_EXTERNx },      //!< External interrupt 8
    { INTERRUPT_EXTERNx },      //!< External interrupt 9
    { INTERRUPT_EXTERNy },      //!< External interrupt 10
    { INTERRUPT_EXTERNy },      //!< External interrupt 11
    { INTERRUPT_EXTERNy },      //!< External interrupt 12
    { INTERRUPT_EXTERNy },      //!< External interrupt 13
    { INTERRUPT_EXTERNy },      //!< External interrupt 14
    { INTERRUPT_EXTERNy }       //!< External interrupt 15
};

//! Hardware registers to control external interrupt inputs
static ExtiRegFile_t *EXTI = (ExtiRegFile_t*) 0x40010400;

//! Hardware registers to control alternate function mapping
static AfioRegFile_t *AFIO = (AfioRegFile_t*) 0x40010000;

//! Interrupt vector table for all external interrupts
static InterruptVector_t ExtiIntrVectors[ELEMENTS(DataTable)] = {0};


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t halExtIntFindDevice (Device_t DeviceID);
static Error_t halExtIntGetIndex   (Handle_t Handle);



/*****************************************************************************/
/*!
 *  \brief   Opens an external interrupt
 *
 *      Opens an external interrupt and returns a handle to be used to
 *      access that interrupt. DeviceID must be the numerical name of
 *      an external interrupt defined in the HAL configuration file.
 *      The interrupt is still disabled after opening it. It must be
 *      explicytly enabled using halExtIntControl.
 *
 *      Handler must be a pointer to an interrupt handler function to be 
 *      called when the device signals an interrupt. It can be NULL if no
 *      interrupts are used. UserTag is a user defined number to be passed 
 *      to the handler function, e.g. as index into a data table in case 
 *      of multiple instances. 
 *
 *  \iparam  DeviceID = Logical external interrupt ID
 *  \iparam  UserTag  = User tag to pass thru to interrupt handler
 *  \iparam  Handler  = Interrupt handler function
 *
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

Error_t halExtIntOpen (Device_t DeviceID, UInt32 UserTag, halIntrHandler_t Handler) {

    const Int32 Index = halExtIntFindDevice(DeviceID);

    if (Index >= 0) {
        const Handle_t Handle = DEVICE_CLASS_IRQ + Index;

        if (DataTable[Index].Flags & HAL_OPEN_RW) {
            return (E_DEVICE_ALREADY_OPEN);
        }
        ExtiIntrVectors[Index].Handler = Handler;
        ExtiIntrVectors[Index].UserTag = UserTag;

        DataTable[Index].Flags |= HAL_OPEN_MODE;
        return (Handle);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Closes an external interrupt
 *
 *      Closes an external interrupt
 *
 *  \iparam  Handle = Handle of open external interrupt
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halExtIntClose (Handle_t Handle) {

    const Int32 Index = halExtIntGetIndex(Handle);

    if (Index >= 0) {
        DataTable[Index].Flags &= ~HAL_OPEN_MODE;
        EXTI->IMR &= ~BIT(Index);
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Control external interrupt channel
 *
 *      Controls the external interrupt channel associated with Handle.
 *      Depending on ControlID the following actions can be initiated:
 *
 *      - Enable/disable interrupt
 *      - Clear interrupt flag
 *      - Trigger interrupt by software
 *
 *      There is no need to clear the interrupt flag when a interrupt
 *      handler is called, because this is done automatically.
 *
 *  \iparam  Handle    = Handle of external interrupt channel
 *  \iparam  ControlID = Control type identifier
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halExtIntControl (Handle_t Handle, ExtCtrlID_t ControlID) {

    const Int32 Index = halExtIntGetIndex(Handle);

    if (Index >= 0) {
        switch (ControlID) {

            case EXT_CTRL_ENABLE:
                EXTI->IMR |= BIT(Index);
                break;

            case EXT_CTRL_DISABLE:
                EXTI->IMR &= ~BIT(Index);
                break;

            case EXT_CTRL_TRIGGER:
                EXTI->SWIER |= BIT(Index);
                break;

            case EXT_CTRL_CLEAR:
                EXTI->PR = BIT(Index);
                break;

            default:
                return (E_UNKNOWN_CONTROL_ID);
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Get status of external interrupt
 *
 *      Returns the status of the external interrupt associated with
 *      Handle. What status to return depends on the supplied StatusID.
 *      The following informations can be requested:
 *
 *      - Interrupt pending status
 *
 *      If a unsupported StatusID is supplied, an error is returned.
 *
 *  \iparam  Handle   = Handle of external interrupt channel
 *  \iparam  StatusID = Type of status
 *
 *  \return  Status value or (negative) error code
 *
 ****************************************************************************/

Error_t halExtIntStatus (Handle_t Handle, ExtStatID_t StatusID) {

    const Int32 Index = halExtIntGetIndex(Handle);

    if (Index >= 0) {
        switch (StatusID) {

            case EXT_STAT_PENDING:
                return (EXTI->PR & EXTI->IMR & BIT(Index));
        }
        return (E_UNKNOWN_STATUS_ID);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Master interrupt handler
 *
 *      This function will be called by the interrupt controller in case
 *      of pending external interrupts. ExtIntNo is the number of the ext.
 *      interrupt line. It calls the registered user handler, passing the
 *      external interrupt line number and the user tag to it. Before
 *      exiting, the handled interrupt flag bits are cleared.
 *
 *      The external interrupt lines 5..9 share one interrupt vector. The
 *      same is true for interrupts 10...15. Nevertheless those interrupts
 *      can use different handlers, since the shared interrupts are split
 *      up by evaluating the external interrupt pending register.
 *
 *  \iparam  ExtIntNo = External interrupt line
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void halExtIntInterruptHandler (UInt32 ExtIntNo) {

    if (ExtIntNo >= 10) {
        ExtIntNo = halGetHighestBitSet(EXTI->PR & EXTI->IMR & 0x7C00);
    }
    else if (ExtIntNo >= 5) {
        ExtIntNo = halGetHighestBitSet(EXTI->PR & EXTI->IMR & 0x03E0);
    }
    if (ExtiIntrVectors[ExtIntNo].Handler != NULL) {
        ExtiIntrVectors[ExtIntNo].Handler (ExtiIntrVectors[ExtIntNo].UserTag, ExtIntNo);
    }
    EXTI->PR = BIT(ExtIntNo);
}


/*****************************************************************************/
/*!
 *  \brief   Get index of external interrupt
 *
 *      Returns the index of the external interrupt associated with Handle.
 *      The index is used to address the entry in the ExtIntData table
 *      and is equal to the external interrupt number.
 *
 *  \iparam  Handle = Handle of external interrupt
 *
 *  \return  Channel index or (negative) error code
 *
 ****************************************************************************/

static Error_t halExtIntGetIndex (Handle_t Handle) {

    register const UInt32 Index = Handle ^ DEVICE_CLASS_IRQ;

    if (Index < ELEMENTS(DataTable)) {

        const UInt16 Flags = DataTable[Index].Flags;

        if (!(Flags & HAL_OPEN_RW)) {
            return (E_DEVICE_NOT_OPEN);
        }
        return ((Error_t) Index);
    }
    return (E_DEVICE_HANDLE_INVALID);
}


/*****************************************************************************/
/*!
 *  \brief   Search logical interrupt input
 *
 *      Searches the logical external interrupt descriptor table for an entry
 *      with the asked DeviceID. If successful, the physical interrupt line
 *      number (which is the same as the port bit number) is returned.
 *      If no such entry exists, an error is returned.
 *
 *  \iparam  DeviceID = Logical device ID
 *
 *  \return  Interrupt line number or (negative) error code
 *
 ****************************************************************************/

static Error_t halExtIntFindDevice (Device_t DeviceID) {

    UInt32 Index;

    if ((DeviceID & DEVICE_CLASS_MASK) == DEVICE_CLASS_IRQ) {

        halInitState_t InitState = halGetInitState();

        if (InitState == INIT_NOT_DONE || InitState == INIT_FAILED) {
            return (E_HAL_NOT_INITIALIZED);
        }
        for (Index=0; Index < halExtIntrDescriptorCount; Index++) {

            if (halExtIntrDescriptors[Index].DeviceID == DeviceID) {
                if (~DataTable[Index].Flags & HAL_FLAG_INITZED) {
                    return (E_DEVICE_UNCONFIGURED);
                }
                return (halExtIntrDescriptors[Index].PortBit);
            }
        }
    }
    return (E_UNKNOWN_DEVICE_ID);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize external interrupt inputs
 *
 *      Initializes the external interrupt control registers based on the
 *      halExternalInterrupts table defined in the HAL configuration file.
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halExtIntInit (void) {

    UInt32 *Mappings = (UInt32*)&halPinMappings;
    UInt32 Allocated = 0;
    UInt32 i;

    if (halGetInitState() == INIT_IN_PROGRESS) {

        // Setup alternate pin mapping registers
        if (Mappings[0] || Mappings[1]) {
            halPeripheralClockEnable(PERIPHERAL_AFIO, ON);
            AFIO->MAPR1 = (Mappings[0]);
            AFIO->MAPR2 = (Mappings[1] & ~MAPPING_FORCE_0) | MAPPING_FORCE_1;
        }
        // Clear all external interrupt related registers
        for (i=0; i < ELEMENTS(AFIO->EXTICR); i++) {
            AFIO->EXTICR[i] = 0;
        }
        EXTI->RTSR = EXTI->FTSR = EXTI->IMR = EXTI->EMR = 0;

        // Connect interrupt logic to configured pins/ports
        for (i=0; i < halExtIntrDescriptorCount; i++) {

            UInt16 LineNo = halExtIntrDescriptors[i].PortBit;
            UInt16 PortNo = halExtIntrDescriptors[i].PortNo;
            UInt16 GpioNo = LineNo + (PortNo * BITS_PER_PORT);

            if (LineNo >= ELEMENTS(DataTable)) {
                return (E_EXTINT_NOT_EXISTS);
            }
            if (PortNo >= halProcessorInfo.CountPort) {
                return (E_PORT_NOT_EXISTS);
            }
            if (GpioNo >= halProcessorInfo.CountGPIO) {
                return (E_PIN_NUMBER_INVALID);
            }
            if (Allocated & BIT(LineNo)) {
                return (E_EXTINT_USED_TWICE);
            }
            AFIO->EXTICR[LineNo / 4] |= PortNo << ((LineNo % 4) * 4);

            // Enable active edges for external interrupts
            if (halExtIntrDescriptors[i].Edge & IRQ_EDGE_RISING) {
                EXTI->RTSR |= BIT(LineNo);
            }
            if (halExtIntrDescriptors[i].Edge & IRQ_EDGE_FALLING) {
                EXTI->FTSR |= BIT(LineNo);
            }
            // Reset interrupt pending bits
            EXTI->PR = BIT(LineNo);

            halInterruptEnable (DataTable[LineNo].InterruptID,
                halExtIntrDescriptors[i].Priority);

            DataTable[LineNo].Flags = HAL_FLAG_INITZED;
            Allocated |= BIT(LineNo);
        }
    }
    return (NO_ERROR);
}

//****************************************************************************/
