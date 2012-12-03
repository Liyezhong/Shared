/****************************************************************************/
/*! \file halInterrupt.c
 *
 *  \brief  Interrupt Control Functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to virtualize interrupts. It controls
 *      the peripheral interrupt expansion unit and interrupt stub functions.
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
#include "halMain.h"
#include "halVectors.h"
#include "halInterrupt.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define HARDWARE_INTERRUPTS 16          //!< Offset to peripheral interrupts
#define UNDEFINED_DEVICE    255         //!< Undefined device number

#define AIRCR_SYSRESET      0x00000004  //!< System reset request
#define AIRCR_PRIGROUP      0x00000700  //!< Interrupt priority group

#define CCR_UNALIGN_TRAP    0x00000008  //!< Enable unaligned access traps
#define CCR_DIV0_TRAP       0x00000010  //!< Enable divide by 0 traps

#define CFSR_MMAR_VALID     0x00000080  //!< Mem Fault Address valid flag
#define CFSR_BFAR_VALID     0x00008000  //!< Bus Fault Address valid flag
#define ICSR_VECTACTIVE     0x000001FF  //!< Active vector number

#define SCSR_MEM_FAULT_ENA  0x00010000  //!< memory fault exception enable
#define SCSR_BUS_FAULT_ENA  0x00020000  //!< bus fault exception enable
#define SCSR_USG_FAULT_ENA  0x00040000  //!< Usage fault exception enable

#define VECTOR_KEY_VALUE    0x05FA0000  //!< Register key value

//! Macro to determine the group an interrupt belongs to
#define InterruptGroup(i)   (((i) - HARDWARE_INTERRUPTS) / 32)

//! Macro to determine the Interrupt number inside an interrupt group
#define InterruptMask(i)    (1UL << (((i) - HARDWARE_INTERRUPTS) % 32))


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Register layout of the Cortex system control block
typedef volatile struct {
    UInt32 CPUID;           //!< CPUID register
    UInt32 ICSR;            //!< Interrupt control and state register
    UInt32 VTOR;            //!< Vector table offset register
    UInt32 AIRCR;           //!< Application interrupt and reset control register
    UInt32 SCR;             //!< System control register
    UInt32 CCR;             //!< Configuration and control register
    UInt8  SHPR[12];        //!< System handler priority registers
    UInt32 SHCSR;           //!< System handler control and state register
    UInt32 CFSR;            //!< Configurable fault status register
    UInt32 HFSR;            //!< Hard fault status register
    UInt32 RESERVED1;       //!< Reserved
    UInt32 MMFAR;           //!< Memory management fault address register
    UInt32 BFAR;            //!< Bus fault address register
    UInt32 RESERVED2;       //!< Reserved
} ScbRegFile_t;

//! Register layout of the nested interrupt vector controller
typedef volatile struct {
    UInt32 ISER[2];         //!< Interrupt enable set-registers
    UInt32 RESERVED0[30];   //!< Reserved
    UInt32 ICER[2];         //!< Interrupt enable clear-registers
    UInt32 RSERVED1[30];    //!< Reserved
    UInt32 ISPR[2];         //!< Interrupt pending set-registers
    UInt32 RESERVED2[30];   //!< Reserved
    UInt32 ICPR[2];         //!< Interrupt pending clear-registers
    UInt32 RESERVED3[30];   //!< Reserved
    UInt32 IABR[2];         //!< Interrupt active bit registers
    UInt32 RESERVED4[62];   //!< Reserved
    UInt8  IPR[68];         //!< Interrupt priority registers
} VicRegFile_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Hardware registers of system control block
static ScbRegFile_t *SCB = (ScbRegFile_t*) 0xE000ED00;

//! Hardware registers to control nested vectored interrupt controller
static VicRegFile_t *VIC = (VicRegFile_t*) 0xE000E100;

//! Callback function in case of a hardware exception
halExceptionHandler_t halExceptionNotifier = NULL;

UInt32 halDynamicVectors[6];  //!< Dynamic interrupt vectors for some timers

                                                        
//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static Error_t halInterruptSetPriority (UInt8 InterruptID, UInt8 Priority);



/*****************************************************************************/
/*!
 *  \brief   Enable individual interrupt
 *
 *      Enables the interrupt with the supplied InterruptID by setting the
 *      interrupt enable bit in the peripheral interrupt controller. If
 *      the InterruptID INTERRUPTS_GLOBAL is given, the global processor
 *      interrupt mask is cleared.
 *
 *      Priority defines the interrupt priority. Lower values represent 
 *      higher priorities, higher values lower priorities.
 *
 *  \iparam  InterruptID = Interrupt identifier
 *  \iparam  Priority    = Priority of interrupt (0...15)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halInterruptEnable (UInt8 InterruptID, UInt8 Priority) {
                                     
    UInt16 Group = InterruptGroup(InterruptID);

    if (InterruptID >= NUMBER_OF_INTERRUPTS) {
        return (E_INVALID_INTERRUPT_ID);
    }
    if (InterruptID == INTERRUPTS_GLOBAL) {
        halGlobalInterruptEnable();
    }
    else if (InterruptID >= HARDWARE_INTERRUPTS) {

        // Timer 1 and 8 use two vectors
        if (InterruptID == INTERRUPT_TIMER1 || 
            InterruptID == INTERRUPT_TIMER8) {
            VIC->ISER[Group] = InterruptMask(InterruptID-2);
        }
        VIC->ISER[Group] = InterruptMask(InterruptID);
    }
    return (halInterruptSetPriority(InterruptID, Priority));
}


/*****************************************************************************/
/*!
 *  \brief   Disable individual interrupt
 *
 *      Disables the interrupt with the supplied InterruptID by clearing the
 *      interrupt enable bit in the peripheral interrupt controller. If
 *      the InterruptID INTERRUPTS_GLOBAL is given, the global processor
 *      interrupt mask is set.
 *
 *  \iparam  InterruptID = Interrupt identifier
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halInterruptDisable (UInt8 InterruptID) {

    UInt16 Group = InterruptGroup(InterruptID);

    if (InterruptID >= NUMBER_OF_INTERRUPTS) {
        return (E_INVALID_INTERRUPT_ID);
    }
    if (InterruptID == INTERRUPTS_GLOBAL) {
        halGlobalInterruptDisable();
    }
    else if (InterruptID >= HARDWARE_INTERRUPTS) {

        // Timer 1 and 8 use two vectors
        if (InterruptID == INTERRUPT_TIMER1 || 
            InterruptID == INTERRUPT_TIMER8) {
            VIC->ICER[Group] = InterruptMask(InterruptID-2);
        }
        VIC->ICER[Group] = InterruptMask(InterruptID);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Trigger individual interrupt
 *
 *      Triggers the interrupt with the supplied InterruptID by setting 
 *      the interrupt pending bit in the peripheral interrupt controller.
 *
 *  \iparam  InterruptID = Interrupt identifier
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halInterruptTrigger (UInt8 InterruptID) {

    UInt16 Group = InterruptGroup(InterruptID);

    if (InterruptID >= NUMBER_OF_INTERRUPTS ||
        InterruptID < HARDWARE_INTERRUPTS) {
        return (E_INVALID_INTERRUPT_ID);
    }
    VIC->ISPR[Group] = InterruptMask(InterruptID);

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Acquire dynamic interrupt vector
 *
 *      Some timer interrupts share a single interrupt vector. This function
 *      acquires a vector for one of those devices. If the vector is already
 *      allocated to another device, an error is returned.
 *
 *  \iparam  InterruptID = Interrupt identifier
 *  \iparam  DeviceNo    = Number to pass to interrupt handler
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halInterruptAcquire (UInt8 InterruptID, UInt8 DeviceNo) {

    UInt32 Index;

    switch (InterruptID) {
        case INTERRUPT_TIMER9:
        case INTERRUPT_TIMER10:
        case INTERRUPT_TIMER11:
            Index = InterruptID - INTERRUPT_TIMER9; break;

        case INTERRUPT_TIMER12:
        case INTERRUPT_TIMER13:
        case INTERRUPT_TIMER14:
            Index = InterruptID - INTERRUPT_TIMER12+3; break;

        case INTERRUPT_TIMER1:
            Index = 1; break;

        case INTERRUPT_TIMER8:
            Index = 4; break;

        default:
            return (NO_ERROR);
    }
    if (halDynamicVectors[Index] == UNDEFINED_DEVICE) {
        halDynamicVectors[Index] = DeviceNo;
    }
    else if (DeviceNo == UNDEFINED_DEVICE) {
        halDynamicVectors[Index] = UNDEFINED_DEVICE;
    }
    else if (DeviceNo != halDynamicVectors[Index]) {
        return (E_INVALID_INTERRUPT_ID);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Release dynamic interrupt vector
 *
 *      Some timer interrupts share a single interrupt vector. This function
 *      releases a previously acquires dynamic vector. Afterwards the vector
 *      can be acquired by another device.
 *
 *  \iparam  InterruptID = Interrupt identifier
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halInterruptRelease (UInt8 InterruptID) {

    return (halInterruptAcquire (InterruptID, UNDEFINED_DEVICE));
}


/*****************************************************************************/
/*!
 *  \brief   Set interrupt priority
 *
 *      Sets the priority of the interrupt specified by InterruptID.
 *      Lower values represent higher priorities, higher values represent
 *      lower priorities. Valid priorities are in the range 0...15.
 *
 *  \iparam  InterruptID = Interrupt identifier
 *  \iparam  Priority    = Priority of interrupt (0...15)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halInterruptSetPriority (UInt8 InterruptID, UInt8 Priority) {

    if (Priority > IRQ_PRIO_LOWEST) {
        return (E_INVALID_PRIORITY);
    }
    if (InterruptID >= 4) {

        if (InterruptID < 16 && InterruptID >= 4) {
            SCB->SHPR[InterruptID - 4] = Priority << 4;
        }
        else if ((UInt32)InterruptID-16 < ELEMENTS(VIC->IPR)) {

            // Timer 1 and 8 have two vectors
            if (InterruptID == INTERRUPT_TIMER1 || 
                InterruptID == INTERRUPT_TIMER8) {
                VIC->IPR[InterruptID - 14] = Priority << 4;
            }
            VIC->IPR[InterruptID - 16] = Priority << 4;
        }
        else {
            return (E_INVALID_INTERRUPT_ID);
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Get actual interrupt priority
 *
 *      Returns actual priority of the interrupt specified by InterruptID.
 *      Lower values represent higher priorities, higher values represent
 *      lower priorities. Valid priorities are in the range 0...15.
 *
 *  \iparam  InterruptID = Interrupt identifier
 *
 *  \return  Interrupt priority or (negative) error code
 *
 ****************************************************************************/

Error_t halInterruptGetPriority (UInt8 InterruptID) {

    if (InterruptID < 16) {
        return (SCB->SHPR[InterruptID - 4] >> 4);
    }
    else if ((UInt32)InterruptID-16 < ELEMENTS(VIC->IPR)) {
        return (VIC->IPR[InterruptID - 16] >> 4);
    }
    return (E_INVALID_INTERRUPT_ID);
}


/*****************************************************************************/
/*!
 *  \brief   Unused Interrupt Handler
 *
 *      This function is called whenever an unused interrupt comes in (what
 *      never should happen). It disables that interrupt and returns. In
 *      case of an exception, the exception handler is called.
 *
 *  \return  Nothing
 *
 ****************************************************************************/

interrupt void halInterruptUnusedHandler (void) {
    
    const UInt8 InterruptID = SCB->ICSR & ICSR_VECTACTIVE;

    if (InterruptID < 16) {
        halExceptionHandler ((UInt32*)__current_sp());           //lint !e26
    }
    else {
        halInterruptDisable (SCB->ICSR & ICSR_VECTACTIVE);
    }
}
        

/*****************************************************************************/
/*!
 *  \brief   Exception Handler
 *
 *      This function is called whenever an exception is thrown. The reason,
 *      code address and data address are collected and (optionally) passed
 *      to a user callback function for notification. The global function 
 *      pointer halExceptionNotifier must be set accordingly.
 *
 *      The exception information structure contains:
 *
 *      - FaultReason contains the fault flags from the CFSR register
 *      - FaultAccess contains the memory/bus address
 *      - FaultAddress contains the code address
 *
 *      Further code processing is stopped after detecting an exception.
 *      If the watchdog is enabled, a system reset will follow.
 *
 *  \iparam  StackFrame = Exception stack pointer
 *
 *  \return  Nothing (never returns)
 *
 ****************************************************************************/

void halExceptionHandler (UInt32 *StackFrame) {

    UInt32 MemFaultAddress = SCB->MMFAR;
    UInt32 BusFaultAddress = SCB->BFAR;
    halExceptionInfo_t Exception;

    Exception.ExceptionID = SCB->ICSR & ICSR_VECTACTIVE;
    Exception.FaultReason = SCB->CFSR;

    if (Exception.FaultReason & CFSR_MMAR_VALID) {
        Exception.FaultAccess = MemFaultAddress;
    }    
    else if (Exception.FaultReason & CFSR_BFAR_VALID) {
        Exception.FaultAccess = BusFaultAddress;
    }
    else {
        Exception.FaultAccess = MAX_UINT32;
    }
    Exception.FaultAddress = StackFrame[6];

    if (halExceptionNotifier != NULL) {
        halExceptionNotifier (&Exception);
    }
    // Stop further processing
    for(;;) {}
}


/*****************************************************************************/
/*!
 *  \brief   Request a hardware reset
 *
 *      Requests a hardware reset by setting the SYSRESET bit in the
 *      processors system control block register AIRCR. Never returns.
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void halHardwareReset (void) {

    SCB->AIRCR =
        (SCB->AIRCR & AIRCR_PRIGROUP) | VECTOR_KEY_VALUE | AIRCR_SYSRESET;

    for(;;) {}  /* wait until reset */
}


/*****************************************************************************/
/*!
 *  \brief   Initialize interrupt controller
 *
 *      Initializes the peripheral vectored interrupt controller by clearing
 *      all interrupt flags and enables, link all interrupts in the original
 *      interrupt vector table to the halMasterInterruptHandler function and
 *      let all entries of the replacement vector table point to the dummy
 *      interrupt handler (halNonRegisteredInterrupt).
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halInterruptInit (void) {

    UInt32 i;

    if (halGetInitState() == INIT_IN_PROGRESS) {

        // Clear all Interrupt Enable and Interrupt Flag registers
        for (i=0; i < ELEMENTS(VIC->ICER); i++) {
            VIC->ICER[i] = MAX_UINT32;
            VIC->ICPR[i] = MAX_UINT32;
        }
        // Set dynamic interrupts to "undefined"
        for (i=0; i < ELEMENTS(halDynamicVectors); i++) {
            halDynamicVectors[i] = UNDEFINED_DEVICE;
        }
        // Redirect interrupt vector table
        SCB->VTOR = (UInt32) halInterruptVectorTable;

        // Configure exception handling
        SCB->SHCSR = SCSR_MEM_FAULT_ENA |
                     SCSR_BUS_FAULT_ENA |
                     SCSR_USG_FAULT_ENA;

        SCB->CCR   = CCR_UNALIGN_TRAP |
                     CCR_DIV0_TRAP;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Deinitialize interrupt controller
 *
 *      Deinitializes the peripheral vectored interrupt controller by clearing
 *      all interrupt flags and enables, link all interrupts in the original
 *      interrupt vector table to the halMasterInterruptHandler function and
 *      let all entries of the replacement vector table point to the dummy
 *      interrupt handler (halNonRegisteredInterrupt).
 *
 *      This function is called only by the bootloader to switch the firmware.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halInterruptDeinit (void) {

    UInt32 i;

    halGlobalInterruptDisable();

    if (halGetInitState() == INIT_SUCCEEDED) {

        // Clear all Interrupt Enable and Interrupt Flag registers
        for (i=0; i < ELEMENTS(VIC->ICER); i++) {
            VIC->ICER[i] = MAX_UINT32;
            VIC->ICPR[i] = MAX_UINT32;
        }
        // Set dynamic interrupts to "undefined"
        for (i=0; i < ELEMENTS(halDynamicVectors); i++) {
            halDynamicVectors[i] = UNDEFINED_DEVICE;
        }

        // Disable exception handling
        SCB->CCR = 0;
    }
    return (NO_ERROR);
}

//****************************************************************************/
