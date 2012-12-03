/****************************************************************************/
/*! \file halInterrupt.h
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

#ifndef HAL_INTERRUPT_H
#define HAL_INTERRUPT_H

#include "halVectors.h"

//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

#define EXCPT_DIVIDE_BY_ZERO    0x02000000  //!< Divide by zero exception
#define EXCPT_UNALIGNED_ACCESS  0x01000000  //!< Unaligned access exception
#define EXCPT_UNKNOWN_OPCODE    0x00010000  //!< Unknown instruction
#define EXCPT_INVALID_PC        0x00040000  //!< Invalid PC exception
#define EXCPT_CODE_ACCESS       0x00000001  //!< Code access exception
#define EXCPT_DATA_ACCESS       0x00000002  //!< Data access exception
#define EXCPT_DATA_BUS_ERROR    0x00000600  //!< Data bus exception
#define EXCPT_STACKING_ERROR    0x00001818  //!< Stacking exception

//! Macro for compiler specific intrinsic for enabling interrupts
#define halGlobalInterruptEnable()  __enable_irq()

//! Macro for compiler specific intrinsic for disabling interrupts
#define halGlobalInterruptDisable() __disable_irq()


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Pointer to standard interrupt handler in user space
typedef void (*halIntrHandler_t) (UInt32 UserTag, UInt32 IntrFlags);

//! Structure to store user registered interrupt informations
typedef struct {
    halIntrHandler_t Handler;  //!< User interrupt handler
    UInt32 UserTag;            //!< User tag to be passed to the handler
} InterruptVector_t;

//! Structure to describe an exception or trap
typedef  struct {
    UInt32 ExceptionID;     //!< Exception number
    UInt32 FaultAddress;    //!< Exception code address
    UInt32 FaultAccess;     //!< Exception bus/memory fault address
    UInt32 FaultReason;     //!< Exception reason bits
} halExceptionInfo_t;

//! Pointer to define an exception user callback function
typedef void (*halExceptionHandler_t) (halExceptionInfo_t *ExceptionInfo);


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halInterruptGetPriority (UInt8 InterruptID);
Error_t halInterruptEnable      (UInt8 InterruptID, UInt8 Priority);
Error_t halInterruptDisable     (UInt8 InterruptID);
Error_t halInterruptTrigger     (UInt8 InterruptID);
Error_t halInterruptAcquire     (UInt8 InterruptID, UInt8 DeviceNo);
Error_t halInterruptRelease     (UInt8 InterruptID);
Error_t halInterruptInit        (void);
Error_t halInterruptDeinit      (void);
void    halExceptionHandler     (UInt32 *StackFrame);
void    halHardwareReset        (void);

//****************************************************************************/

#endif /*HAL_INTERRUPT_H*/
