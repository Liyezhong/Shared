/****************************************************************************/
/*! \file halTimer.c
 * 
 *  \brief  Timer access function
 *
 *  $Version: $ 0.1
 *  $Date:    $ 12.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to access and manage hardware timers.
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
#include "halSystem.h"
#include "halCommon.h"
#include "halTimer.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define PROCESSOR_TIMER_COUNT   3       //!< Number of system timers

#define TIMER_INTERRUPT_ENABLE  0x4000  //!< Timer interrupt enable bit
#define TIMER_INTERRUPT_FLAG    0x8000  //!< Timer interrupt flag bit
#define TIMER_RELOAD_BIT        0x0020  //!< Timer reload bit
#define TIMER_STOP_BIT          0x0010  //!< Timer stop bit
  
//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! Layout of processor internal timer control registers */
typedef struct {
    UInt32 Counter;             //!< Timer counter register
    UInt32 Period;              //!< Timer period register
    UInt16 Control;             //!< Timer control register
    UInt16 Reserved;            //!< Reserved
    UInt16 LPreScale;           //!< Timer pre-scale low
    UInt16 HPreScale;           //!< Timer pre-scale high
} TIMER_REGISTERS_t;

/*! Structure to hold the state of a logical timer */
typedef struct {
    UInt16 Flags;               //!< Port flags (access mode)
} LOGICAL_TIMER_DATA_t;

//****************************************************************************/
// Private Variables 
//****************************************************************************/

static volatile TIMER_REGISTERS_t *TimerRegs = (TIMER_REGISTERS_t*) 0x0C00;

static LOGICAL_TIMER_DATA_t TimerData[PROCESSOR_TIMER_COUNT];


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static ERROR_t halGetTimerIndex (HANDLE_t Handle);


/*****************************************************************************/
/*! 
 *  \brief   Opens a timer
 *
 *      Opens a hardware timer and returns a handle to be used to access it. 
 *      The ChannelID must be the numerical "name" of a timer defined in the
 *      HAL configuration file. The parameter Resolution can be used to
 *      specify a prescaler, i.e. the count rate, for the timer. 
 * 
 *      The timer remains in a stopped state and must manually be started
 *      using the control function.
 * 
 *  \iparam  Channel    = Logical channel ID
 *  \iparam  Resolution = Count rate (prescaler)
 * 
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

HANDLE_t halTimerOpen (CHANNEL_t Channel, UInt16 Resolution) {
    
    const Int32 Index = Channel & CHANNEL_INDEX_MASK;
    
    if ((Channel & CHANNEL_CLASS_MASK) != CHANNEL_CLASS_TIMER) {
       return (halSetError(E_HAL_INVALID_CHANNEL));
    }    
    if (Index >= PROCESSOR_TIMER_COUNT) {
       return (halSetError(E_HAL_INVALID_CHANNEL));
    }
    if ((TimerData[Index].Flags & HAL_OPEN_RW) == 0) {
        register volatile 
            TIMER_REGISTERS_t *Register = &TimerRegs[Index];
        
        Register->Period    = 0;
        Register->HPreScale = (Resolution >> 8) & 0xFF;
        Register->LPreScale = (Resolution) & 0xFF;   
        Register->Control   = TIMER_RELOAD_BIT | TIMER_STOP_BIT;    
            
        TimerData[Index].Flags = HAL_OPEN_RW;
        
        return (CHANNEL_CLASS_TIMER +  Index);
    }
    return (halSetError(E_HAL_DEVICE_IN_USE));
}


/*****************************************************************************/
/*! 
 *  \brief   Close an open timer
 *
 *      Closes an open timer, stops it and disables timer interrupts.
 * 
 *  \iparam  Handle = Handle of open analog channel
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halTimerClose (HANDLE_t Handle) {

    const Int32 Index = halGetTimerIndex(Handle);

    if (Index >= 0) {
        TimerRegs[Index].Control = TIMER_STOP_BIT;
        TimerData[Index].Flags = 0;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Read timer counter register
 *
 *      Read the timer counter registers of the timer associated with Handle
 *      and copies it to the variable pointed to by Counter.
 *  
 *      If the channel is not open, an error is retuned.
 * 
 *  \iparam  Handle  = Handle of a timer
 *  \oparam  Counter = Pointer to a buffer to store the result
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halTimerRead (HANDLE_t Handle, UInt32 *Counter) {
    
    const Int32 Index = halGetTimerIndex(Handle);

    if (Index >= 0) {
        if (!isPtrNull(Counter)) {
            *Counter = TimerRegs[Index].Counter;
        }
        return (NO_ERROR);
    }
    return (halSetError(Index));
}


/*****************************************************************************/
/*! 
 *  \brief   Writes the timer counter
 *
 *      Writes a new count value into the timer period register. The content
 *      of the period is automatically transfered into the timer counter,
 *      whenever the running counter reaches zero. To force an immediate
 *      load of the new time count value into the counter register, the
 *      ControlID TIMER_CTRL_RELOAD can be used. Other control IDs are
 *      ignored (use halTimerControl instead). The timer interrupt flag
 *      is automatically reset and the timer started.
 *  
 *      If the channel is not open, an error is returned.
 * 
 *  \iparam  Handle  = Handle of open timer
 *  \iparam  Value   = New counter period value
 *  \iparam  Control = Control ID (see text)
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halTimerWrite (HANDLE_t Handle, UInt32 Value, UInt16 Control) {

    const Int32 Index = halGetTimerIndex(Handle);

    if (Index >= 0) {
        register volatile 
            TIMER_REGISTERS_t *Register = &TimerRegs[Index];

        Register->Period = Value;

        if (Control & TIMER_CTRL_RELOAD) {
            Register->Control |= TIMER_RELOAD_BIT;
        }
        Register->Control |= TIMER_INTERRUPT_FLAG;
        Register->Control &= ~TIMER_STOP_BIT;
        
        return (NO_ERROR);
    }
    return (halSetError(Index));
}


/*****************************************************************************/
/*! 
 *  \brief   Controls an open timer
 *
 *      Controls the timer associated with Handle in several ways. Depending
 *      on ControlID the following actions can be initiated:
 * 
 *      - Start/stop timer
 *      - Reload timer
 *      - Enable/disable timer interrupts
 *      - Clear interrupt flag
 * 
 *  \iparam  Handle = Handle of open analog channel
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halTimerControl (HANDLE_t Handle, UInt16 ControlID) {

    const Int32 Index = halGetTimerIndex(Handle);

    if (Index >= 0) {         
        register volatile 
            TIMER_REGISTERS_t *Register = &TimerRegs[Index];
        
        if (ControlID & TIMER_CTRL_STOP) {
           Register->Control |= TIMER_STOP_BIT;
        }
        if (ControlID & TIMER_CTRL_RELOAD) {
           Register->Control |= TIMER_RELOAD_BIT;
        }    
        if (ControlID & TIMER_CTRL_START) {
           Register->Control &= ~TIMER_STOP_BIT;
        }
        if (ControlID & TIMER_INTR_CLEAR) {    
           Register->Control |= TIMER_INTERRUPT_FLAG;
        }
        if (ControlID & TIMER_INTR_ENABLE) {    
           Register->Control |= TIMER_INTERRUPT_ENABLE;
        }
        else if (ControlID & TIMER_INTR_DISABLE) {    
           Register->Control &= ~TIMER_INTERRUPT_ENABLE;
        }    
        return (NO_ERROR);
    }
    return (halSetError(Index));
}


/*****************************************************************************/
/*! 
 *  \brief   Get status of a timer
 *
 *      Returns the status of the timer associated with Handle. Which status
 *      to return depends on the supplied StatusID. Following informations
 *      can be requested:
 *  
 *      - Counter running status (yes, no)
 *      - Counter period register content
 *      - Interrupt enable status (yes, no)
 *      - Interrupt flaged status (yes, no)
 * 
 *      If a unsupported StatusID is supplied, a 0 is returned.
 * 
 *  \iparam  Handle   = Handle of open analog channel
 *  \iparam  StatusID = Type of status
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halTimerStatus (HANDLE_t Handle, UInt16 StatusID) {

    const Int32 Index = halGetTimerIndex(Handle);

    if (Index >= 0) {         
        register volatile 
            TIMER_REGISTERS_t *Register = &TimerRegs[Index];

        switch (StatusID) {
            case TIMER_STAT_PERIOD:
                return (Register->Period);
                
            case TIMER_STAT_IsRUNNING:
                return (~Register->Control & TIMER_STOP_BIT);

            case TIMER_INTR_IsENABLED:
                return (Register->Control & TIMER_INTERRUPT_ENABLE);
                
            case TIMER_INTR_IsFLAGED:
                return (Register->Control & TIMER_INTERRUPT_FLAG);
        }
        return (0);
    }    
    return (halSetError(Index));
}


/*****************************************************************************/
/*! 
 *  \brief   Get index of a timer
 *
 *      Returns the index of a timer associated with Handle. The index is 
 *      used to address an entry in the TimerData table, which contains
 *      all variables of a timer.
 * 
 *  \iparam  Handle = Handle of open analog channel
 * 
 *  \return  Timer index or (negative) error code
 *
 ****************************************************************************/

static ERROR_t halGetTimerIndex (HANDLE_t Handle) 
{
    register const Int32 Index = Handle ^ CHANNEL_CLASS_TIMER;

    if (Index < PROCESSOR_TIMER_COUNT && Index >= 0) {

        if (!(TimerData[Index].Flags & HAL_OPEN_RW)) {
            return (E_HAL_DEVICE_NOT_OPEN);
        }                        
        return (Index);
    }
    return (E_INVALID_HANDLE);
}


/*****************************************************************************/
/*! 
 *  \brief   Waits a short time
 *
 *      This function waits for the given number of microseconds and then
 *      returns. It uses no hardware resources, but executes a defined
 *      number of processor cycles. 
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halShortDelay (UInt32 DelayInMicroSeconds) {
    
    register UInt32 Counter = DelayInMicroSeconds;
    
    while (Counter--) {
        asm (" RPT #100 || NOP");
    }
    return (NO_ERROR);   
}


/*****************************************************************************/
/*! 
 *  \brief   Initialize timer module
 *
 *      Initializes the processor internal system timers by setting up the
 *      appropriate peripheral registers. 
 * 
 *      This function is called only once during startup.
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halInitializeTimers (void) {

    int i;
    
    for (i=0; i < PROCESSOR_TIMER_COUNT; i++) { 
           
       halPeripheralClockEnable (PERIPHERAL_TIMER0 + i, ON);        
       TimerRegs[i].Control = TIMER_STOP_BIT;
       TimerData[i].Flags = 0;       
    }
    return (NO_ERROR);   
}

//****************************************************************************/
