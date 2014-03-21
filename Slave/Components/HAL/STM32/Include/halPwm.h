/****************************************************************************/
/*! \file halPwm.h
 * 
 *  \brief  Pulse Width Modulation
 *
 *  $Version: $ 0.1
 *  $Date:    $ 29.03.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage pulse width 
 *      modulated outputs. Since the STM32 has no dedicated PWM hardware, 
 *      the timers are used for this purpose. To be able to port the 
 *      software to other hardware platforms (which might have dedicated
 *      PWM units), a dedicated software interface is provided, that hides 
 *      the fact that standard timers are used on STM32 hardware.
 *
 *      One PWM controller can have one or multiple PWM output channels,
 *      that all share the same time base hardware. Therefore, they all
 *      run at the same PWM frequency. Of course, the pulse width of each
 *      channel can be set individually.
 *
 *      The timers used for PWM must be configured properly in the HAL
 *      configuration file to be usable as PWM controllers.
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

#ifndef HAL_PWM_H
#define HAL_PWM_H

#include "halTimer.h"


//****************************************************************************/
// Public Constants
//****************************************************************************/

// PWM count direction defines
#define PWM_MODE_COUNT_UP    0x00    //!< Count period timer up
#define PWM_MODE_COUNT_DOWN  0x01    //!< Count period timer down
#define PWM_MODE_COUNT_UPDN  0x02    //!< Count period timer up/down
#define PWM_MODE_COUNT_DNUP  0x03    //!< Count period timer down/up

// PWM repetition mode defines
#define PWM_MODE_INTERVAL    0x00    //!< Select interval (continuous) mode
#define PWM_MODE_ONE_SHOT    0x01    //!< Select one-shot mode

// PWM clock source defines
#define PWM_MODE_INTERNAL    0x00    //!< Select internal clock
#define PWM_MODE_EXTERNAL    0x01    //!< Select external clock

// PWM clock mode defines
#define PWM_MODE_NORMAL      0x00    //!< Internal clock mode
#define PWM_MODE_RESET       0x04    //!< Reset by external trigger
#define PWM_MODE_GATED       0x05    //!< Gate with external input
#define PWM_MODE_TRIGGER     0x06    //!< Start by external trigger

// PWM controller interrupt flag bit masks
#define PWM_IFLAG_UNIT1      0x02    //!< PWM unit 1 interrupt
#define PWM_IFLAG_UNIT2      0x04    //!< PWM unit 2 interrupt
#define PWM_IFLAG_UNIT3      0x08    //!< PWM unit 3 interrupt
#define PWM_IFLAG_UNIT4      0x10    //!< PWM unit 4 interrupt


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Control IDs for controlling timer operation (see halTimerControl)
typedef enum {
    PWM_CTRL_START,       //!< Start PWM output
    PWM_CTRL_STOP,        //!< Stop PWM output
    PWM_INTR_ENABLE,      //!< Enable timer interrupt
    PWM_INTR_DISABLE,     //!< Disable timer interrupt
    PWM_INTR_CLEAR        //!< Clear interrupt flag
} PwmCtrlID_t;

//! Status IDs for requesting the state of a timer (see halTimerStatus)
typedef enum {
    PWM_STAT_CLOCKRATE,   //!< Request physical clock rate
    PWM_STAT_RESOLUTION,  //!< Request PWM output resolution
    PWM_STAT_PENDING,     //!< Get pending interrupts bitmask
    PWM_STAT_CHANNELS     //!< Number of capture/compare units
} PwmStatID_t;

//! Structure to define mode of operation (same as for timers)
typedef TimerMode_t PwmMode_t;  


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halPwmOpen    (Device_t Device, UInt32 UserTag, halIntrHandler_t Handler);
Error_t halPwmSetup   (Handle_t Handle, PwmMode_t Mode, UInt32 Period);
Error_t halPwmRead    (Handle_t Handle, UInt16 UnitNo, UInt16 *Width);
Error_t halPwmWrite   (Handle_t Handle, UInt16 UnitNo, UInt16 Width);
Error_t halPwmStatus  (Handle_t Handle, UInt16 UnitNo, PwmStatID_t StatusID);
Error_t halPwmControl (Handle_t Handle, UInt16 UnitNo, PwmCtrlID_t ControlID);
Error_t halPwmClose   (Handle_t Handle);
Error_t halPwmInit    (void);

//****************************************************************************/

#endif /*HAL_PWM_H*/
