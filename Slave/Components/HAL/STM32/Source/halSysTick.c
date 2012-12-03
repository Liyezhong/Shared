/****************************************************************************/
/*! \file halSysTick.c
 *
 *  \brief  System tick implementation
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access the system tick timer,
 *      the fast tick timer, and the real time clock.
 *
 *      The tick timer and fast tick timer uses the cortex system tick
 *      counter which is expanded to 56 bit resolution by software using
 *      the counter overflow interrupt. The tick timer returns the ticks
 *      in milliseconds, the fast tick timer in microseconds.
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
#include "halCommon.h"
#include "halError.h"
#include "halClocks.h"
#include "halMain.h"
#include "halInterrupt.h"
#include "halSysTick.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define SYS_TICK_INTERVAL    1           //!< systick interval in ms

#define STK_COUNTER_MAXVAL   0x00FFFFFF  //!< Max value of counter
#define STK_COUNTER_BITCNT   24          //!< Number of counter bits

#define STK_CTRL_ENABLE      0x00000001  //!< SysTick counter enable
#define STK_CTRL_TICKINT     0x00000002  //!< SysTick exception enable
#define STK_CTRL_CLKSOURCE   0x00000004  //!< Clock source selection
#define STK_CTRL_COUNTFLAG   0x00000100  //!< Counter overflow flag

#define STK_CALIB_TENMS      0x00FFFFFF  //!< Count value for 10ms
#define STK_CALIB_SKEW       0x40000000  //!< Indicates inexact calibration
#define STK_CALIB_NOREF      0x80000000  //!< Indicates "no reference clock"


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Register layout of Cortex internal system tick timer
typedef volatile struct {
    UInt32 CTRL;            //!< SysTick control and status register
    UInt32 LOAD;            //!< SysTick reload value register
    UInt32 VALUE;           //!< SysTick current value register
    UInt32 CALIB;           //!< SysTick calibration value register
} TickRegFile_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Pointer to system tick timer registers
static TickRegFile_t *STK = (TickRegFile_t*) 0xE000E010;

static volatile UInt32 SysTickCounter;  //!< System tick counter

static UInt32 SysTickCalibrate;         //!< System tick interval time



/*****************************************************************************/
/*!
 *  \brief   Wait for a short time
 *
 *      This function returns after the specified number of microseconds
 *      has expired. Delay is restricted to the duration of 1/2 system
 *      tick. If a longer duration is specified, an error is returned.
 *
 *      This function is intended for HAL internal usage to fullfill
 *      timing requirements when accessing hardware registers.
 *
 *  \iparam  MicroSeconds = Delay time [us]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halShortDelay (UInt16 MicroSeconds) {

    UInt32 Delay = MicroSeconds * SysTickCalibrate / 1000;
    UInt32 Start = STK->VALUE;
    UInt32 Expired;

    if (Delay <= STK->LOAD / 2) {
        do {
            Expired = (Start - STK->VALUE) % STK->LOAD;
        } while (Expired < Delay);

        return (NO_ERROR);
    }
    return (E_TIMER_DELAY_LIMIT);
}


/*****************************************************************************/
/*!
 *  \brief   Calculate time difference
 *
 *      Returns the number of milliceconds expired since the time given by
 *      SinceTime. SinceTime must be a valid system tick count, as returned
 *      by bmGetTime().
 *
 *  \iparam  SinceTime = Time to calculate the differnece to
 *
 *  \return  Time expired since SinceTime [ms]
 *
 ****************************************************************************/

UInt32 halTimeExpired (UInt32 SinceTime) {

    return (halSysTickRead() - SinceTime);
}


/*****************************************************************************/
/*!
 *  \brief   Get actual system tick
 *
 *      Returns the actual system tick, i.e. the number of milliseconds
 *      expired since startup. Uses the system tick timer of the Cortex.
 *      Since the system tick counter is 32 bit wide it wraps around
 *      after each 49 days (approximately).
 *
 *  \return  Timer index or (negative) error code
 *
 ****************************************************************************/

UInt32 halSysTickRead (void) {

    return (SysTickCounter);
}


/*****************************************************************************/
/*!
 *  \brief   Tick timer overflow interrupt handler
 *
 *      This function is called by the interrupt controller, whenever the
 *      system tick timer has counted down to zero. It increments the
 *      system tick counter variable. 
 *
 *  \return  Nothing
 *
 ****************************************************************************/

interrupt void halSysTickInterrupt (void) {

    SysTickCounter += SYS_TICK_INTERVAL;
    STK->CTRL &= ~STK_CTRL_COUNTFLAG;
}


/*****************************************************************************/
/*!
 *  \brief   Get fast tick number
 *
 *      Returns the fast system tick, i.e. the number of microseconds
 *      expired since startup. Uses the system tick timer of the Cortex
 *      core. Since the fast tick counter is 32 bit wide it wraps around
 *      after approx. every 71 minutes.
 *
 *  \return  Number of microseconds since startup
 *
 ****************************************************************************/

UInt32 halGetFastTick (void) {

    UInt32 Ticks;
    UInt32 Value;
    UInt32 Count;

    do {
        Ticks = SysTickCounter;
        Value = STK->VALUE;

        if (STK->CTRL & STK_CTRL_COUNTFLAG) {
            if (Value) {
                Ticks++;
            }
        }
    } while (Ticks != SysTickCounter);

    Count = (STK->LOAD - Value) * 1000 / SysTickCalibrate;

    return (Ticks * 1000 + Count);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize system tick counter
 *
 *      Initializes the processor internal system tick timer by setting up
 *      the appropriate peripheral registers.
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halSysTickInit (void) {

    Error_t Status;

    if (halGetInitState() == INIT_IN_PROGRESS) {

        SysTickCalibrate = halGetProcessorClockRate() / 8000;
        SysTickCounter = 0;
    
        STK->LOAD  = SYS_TICK_INTERVAL * SysTickCalibrate;
        STK->VALUE = STK->LOAD;
        STK->CTRL  = STK_CTRL_ENABLE | STK_CTRL_TICKINT;

        Status = halInterruptEnable (EXCEPTION_SYSTICK, IRQ_PRIO_HIGHEST);
        if (Status != NO_ERROR) {
            return (Status);
        }
    }
    return (NO_ERROR);
}

//****************************************************************************/
