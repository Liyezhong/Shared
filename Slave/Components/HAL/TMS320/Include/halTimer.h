/****************************************************************************/
/*! \file halTimer.h
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

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

//! Control IDs for halTimerControl()
typedef enum {
    TIMER_CTRL_START   = BIT(0),   //!< Start timer   
    TIMER_CTRL_STOP    = BIT(1),   //!< Stop timer
    TIMER_CTRL_RELOAD  = BIT(2),   //!< Reload timer           
    TIMER_INTR_ENABLE  = BIT(3),   //!< Enable timer interrupt
    TIMER_INTR_DISABLE = BIT(4),   //!< Disable timer interrupt
    TIMER_INTR_CLEAR   = BIT(5)    //!< Clear interrupt flag
} TIMER_CONTROL_t;

//! Status IDs for requesting the state of a timer
typedef enum {
    TIMER_STAT_PERIOD,             //!< Request actual timer period
    TIMER_STAT_IsRUNNING,          //!< Check if timer is running
    TIMER_INTR_IsENABLED,          //!< Check if timer interrupt is enabled
    TIMER_INTR_IsFLAGED            //!< Check if timer interrupt is flaged
} TIMER_STATUS_t;

#define TIMER_MAXVALUE  0xFFFFFFFF   //!< Maximum for the timer counter


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

HANDLE_t halTimerOpen    (CHANNEL_t Channel, UInt16 Resolution);
ERROR_t  halTimerRead    (HANDLE_t Handle, UInt32 *Count);
ERROR_t  halTimerWrite   (HANDLE_t Handle, UInt32 Count, UInt16 ControlID);
ERROR_t  halTimerControl (HANDLE_t Handle, UInt16 ControlID);
ERROR_t  halTimerStatus  (HANDLE_t Handle, UInt16 StatusID);
ERROR_t  halTimerClose   (HANDLE_t Handle);
ERROR_t  halShortDelay   (UInt32 MicroSeconds);

ERROR_t  halInitializeTimers (void);

//****************************************************************************/

#endif /*HAL_TIMER_H*/
