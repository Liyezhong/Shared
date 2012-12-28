/****************************************************************************/
/*! \file halInterrupt.c
 * 
 *  \brief  Interrupt Control Functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 11.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to virtualize interrupts. It controls
 *       the peripheral interrupt expansion unit and interrupt stub functions.
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

#include "Global.h"
#include "halError.h"
#include "halConfigure.h"
#include "halInterrupt.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define INTERRUPT_NESTING_ALLOWED   FALSE   //!< enable/disable nesting
#define INTERRUPT_VECTOR_TABLE      0x0D00  //!< interrupt vectors table base

#define InterruptGroup(id)          (((id) - 32) / 8)
#define InterruptBitMask(id)        (1 << (((id) - 32) % 8))

typedef interrupt void (*PINTERRUPT)(void);


//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! Layout of processor internal peripheral interrupt expansion registers */
typedef struct {
    UInt16 PieCtrl;         //!< PIE control register
    UInt16 PieAckn;         //!< PIE interrupt acknowledge register
    struct {
        UInt16 PieIE;       //!< PIE interrupt enable register
        UInt16 PieIF;       //!< PIE interrupt flag register
    } PieGroup[12];         //!< One set for each interrupt group    
} PIE_REGISTERS_t;


//****************************************************************************/
// Private Variables 
//****************************************************************************/

/*! Hardware registers to control peripheral interrupt expansion controller */
static volatile PIE_REGISTERS_t *pieRegisters = (PIE_REGISTERS_t*)0x0CE0;

/*! Interrupt vector table */
static HAL_INTERRUPT_HANDLER_t halInterruptVectorTable[NUMBER_OF_INTERRUPTS];

extern cregister volatile UInt16 IFR;  // processor interrupt flag register
extern cregister volatile UInt16 IER;  // processor interrupt enable register


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static interrupt void halMasterInterruptHandler (void);
static void halNonRegisteredInterrupt (INTERRUPT_ID_t VectorID);


/*****************************************************************************/
/*! 
 *  \brief   Enable individual interrupt
 *
 *      Enables the interrupt with the supplied InterruptID by setting the
 *      interrupt enable bit in the peripheral interrupt controller. If
 *      the InterruptID INTERRUPTS_GLOBAL is given, the global processor
 *      interrupt mask is cleared.
 * 
 *  \iparam  InterruptID = Interrupt identifier
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halEnableInterrupt (INTERRUPT_ID_t InterruptID) {
    
    UInt16 Group = InterruptGroup(InterruptID);

    if (InterruptID >= NUMBER_OF_INTERRUPTS)
       return (E_PARAMETER_OUT_OF_RANGE);

    // set interrupt enable bit in peripheral interrupt controller
    if (InterruptID != INTERRUPTS_GLOBAL)       
       pieRegisters->PieGroup[Group].PieIE |= InterruptBitMask(InterruptID);
    else
       halGlobalInterruptEnable();
    
    return (NO_ERROR);
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

ERROR_t halDisableInterrupt (INTERRUPT_ID_t InterruptID) {
    
    UInt16 Group = InterruptGroup(InterruptID);

    if (InterruptID >= NUMBER_OF_INTERRUPTS)
       return (E_PARAMETER_OUT_OF_RANGE);
       
    // clear interrupt enable bit in peripheral interrupt controller       
    if (InterruptID != INTERRUPTS_GLOBAL)       
       pieRegisters->PieGroup[Group].PieIE &= ~InterruptBitMask(InterruptID);
    else 
       halGlobalInterruptDisable();
    
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Register interrupt handler
 *
 *      Associates a interrupt handler function to the specified InterruptID. 
 *      Each time the interrupt with this ID raises, the registered handler 
 *      function is called. The individual interrupt is automatically enabled 
 *      when the handler is registered.  
 * 
 *      When calling this function with a NULL instead of a function pointer
 *      the interrupt handler is unregistered and the individual interrupt
 *      is disabled.
 * 
 *  \iparam  InterruptID = Interrupt identifier
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halRegisterInterrupt (
   INTERRUPT_ID_t InterruptID, HAL_INTERRUPT_HANDLER_t InterruptHandler) {
    
    UInt16 Group = InterruptGroup(InterruptID);
     
    if (InterruptID >= NUMBER_OF_INTERRUPTS) {
       return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (isPtrNull(InterruptHandler)) {            
       halInterruptVectorTable[InterruptID] = halNonRegisteredInterrupt;
    }
    else {
       halInterruptVectorTable[InterruptID] = InterruptHandler;
    }       
    // clear interrupt flag bit in PIE interrupt controller  
    pieRegisters->PieGroup[Group].PieIF &= ~InterruptBitMask(InterruptID);
    
    if (isPtrNull(InterruptHandler)) { 
        halDisableInterrupt(InterruptID);
    }
    else { 
        halEnableInterrupt(InterruptID);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Master Interrupt Handler
 *
 *      This function is called whenever a registered interrupt comes in.
 *      It reads the interrupt vector from the interrupt controller and
 *      calls the registered interrupt handler function. After the
 *      handler function returns, the interrupt acknowledge bit is set 
 *      in the interrupt controller's acknowledge register.  
 * 
 *  \return  Nothing
 *
 ****************************************************************************/

static interrupt void halMasterInterruptHandler (void) {
    
    register UInt16 VectorID = (pieRegisters->PieCtrl >> 1) & 0x7F;
    
    // re-enable interrupts if nesting allowed
    if (INTERRUPT_NESTING_ALLOWED) {
        halGlobalInterruptEnable();
    }
    // call registered user interrupt handler       
    (halInterruptVectorTable[VectorID]) ((INTERRUPT_ID_t) VectorID);
    
    // reset interrupt acknowledge bit in PIE controller
    if (VectorID > INTERRUPT_USER_TRAP12) {
       pieRegisters->PieAckn |= InterruptBitMask(VectorID);
    }
}


/*****************************************************************************/
/*! 
 *  \brief   Dummy Interrupt Handler
 *
 *      This function is called by the master interrupt handler whenever a 
 *      non-registered interrupt comes in. The interrupt vector table is
 *      setup to point to this function for all non-registered interrupts.
 *      It disables this particular interrupt only.
 * 
 *  \return  Nothing
 *
 ****************************************************************************/

static void halNonRegisteredInterrupt (INTERRUPT_ID_t VectorID) {
    
    halDisableInterrupt (VectorID);
}


/*****************************************************************************/
/*! 
 *  \brief   Initialize interrupt controller
 *
 *      Initializes the peripheral interrupt expansion controller by clearing
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

ERROR_t halInitializeInterrupts (void) {
         
    PINTERRUPT *pieInterruptVectorTable = (PINTERRUPT*)INTERRUPT_VECTOR_TABLE;
    UInt16 i;

    // disable and clear all CPU interrupts        
    IER = 0; IFR = 0;
    
    // direct all vectors to master interrupt handler
    for(i=12; i < NUMBER_OF_INTERRUPTS; i++) {
        halInterruptVectorTable[i] = halNonRegisteredInterrupt;
        pieInterruptVectorTable[i] = halMasterInterruptHandler;
    }
    // Clear all Interrupt Enable and Interrupt Flag registers
    for(i=0; i < ELEMENTS(pieRegisters->PieGroup); i++) {
       pieRegisters->PieGroup[i].PieIE = 
       pieRegisters->PieGroup[i].PieIF = 0;
    }
    // Enable the PIE Vector Table
    pieRegisters->PieCtrl |= 1;
    pieRegisters->PieAckn = ~0;
    IER |= ~BIT(15);     
    
    return (NO_ERROR);          
}

//****************************************************************************/
