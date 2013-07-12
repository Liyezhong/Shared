/****************************************************************************/
/*! \file halTimer.h
 *
 *  \brief  Timer access function
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage hardware timers.
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

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include "halInterrupt.h"

//****************************************************************************/
// Public Constants
//****************************************************************************/

#define TIMER_MAX_VALUE        0xFFFF  //!< Maximum for the timer counter

// Timer clock mode defines
#define TIM_MODE_INTERVAL      0x00    //!< Set timer to interval mode
#define TIM_MODE_ONE_SHOT      0x01    //!< Set timer to one-shot mode

// Timer count direction defines
#define TIM_MODE_COUNT_UP      0x00    //!< Count timer up
#define TIM_MODE_COUNT_DOWN    0x01    //!< Count timer down
#define TIM_MODE_CENTER_UP     0x02    //!< Count timer up/down
#define TIM_MODE_CENTER_DOWN   0x03    //!< Count timer down/up

// Timer clock source defines
#define TIM_MODE_INTERNAL      0x00    //!< Select internal clock
#define TIM_MODE_EXTERNAL      0x01    //!< Select external clock

// Timer clock mode defines
#define TIM_MODE_INTERNAL      0x00    //!< Internal mode
#define TIM_MODE_ENCODER_1     0x01    //!< Encoder mode 1
#define TIM_MODE_ENCODER_2     0x02    //!< Encoder mode 2
#define TIM_MODE_ENCODER_3     0x03    //!< Encoder mode 3
#define TIM_MODE_RESET         0x04    //!< Reset counter by external trigger
#define TIM_MODE_GATED         0x05    //!< Gate counter by external trigger
#define TIM_MODE_TRIGGER       0x06    //!< Start counter by external trigger

// Timer interrupt flag bit masks
#define TIM_IFLAG_UPDATE       0x01    //!< Timer counter update interrupt
#define TIM_IFLAG_CAPCOM1      0x02    //!< Capture/compare unit 1 interrupt
#define TIM_IFLAG_CAPCOM2      0x04    //!< Capture/compare unit 2 interrupt
#define TIM_IFLAG_CAPCOM3      0x08    //!< Capture/compare unit 3 interrupt
#define TIM_IFLAG_CAPCOM4      0x10    //!< Capture/compare unit 4 interrupt
#define TIM_IFLAG_TRIGGER      0x40    //!< Timer trigger interrupt

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Control IDs for controlling timer operation (see halTimerControl)
typedef enum {
    TIM_CTRL_START,       //!< Start timer
    TIM_CTRL_STOP,        //!< Stop timer
    TIM_CTRL_RELOAD,      //!< Reload timer
    TIM_CTRL_IOP_RESET,   //!< Input polarity non-inverted / output polarity active high
    TIM_CTRL_IOP_SET,     //!< Input polarity inverted / output polarity active low
    TIM_INTR_ENABLE,      //!< Enable timer interrupt
    TIM_INTR_DISABLE,     //!< Disable timer interrupt
    TIM_INTR_CLEAR        //!< Clear interrupt flag
} TimCtrlID_t;

//! Status IDs for requesting the state of a timer (see halTimerStatus)
typedef enum {
    TIM_STAT_MAXCOUNT,    //!< Get maximum counter value
    TIM_STAT_COUNTRATE,   //!< Get count rate
    TIM_STAT_CLOCKRATE,   //!< Get timer input clock rate
    TIM_STAT_DIRECTION,   //!< Get count direction
    TIM_STAT_OVERRUN,     //!< Capture/compare register overrun
    TIM_STAT_PENDING,     //!< Get pending interrupts bitmask
    TIM_STAT_UNITMODE,    //!< Configured mode of unit
    TIM_STAT_UNITNO,      //!< Physical unit number
    TIM_STAT_UNITS,       //!< Number of capture/compare units
    TIM_STAT_MATCH        //!< Capture/compare match flag
} TimStatID_t;

//! Register select IDs used by halTimerRead/Write
typedef enum {
    TIM_REG_COUNTER,      //!< Select counter register
    TIM_REG_RELOAD,       //!< Select reload register
    TIM_REG_PRESCALER     //!< Select prescaler register
} TimRegsID_t;

//! Structure to setup the mode of a timer (see halTimerMode)
typedef struct {
    UInt8 Direction :2;   //!< Count up, down, up/down, down/up
    UInt8 OneShot   :1;   //!< Continuous vs. one-shot mode
    UInt8 ClkSource :1;   //!< Internal vs. external clock
    UInt8 ClkMode   :3;   //!< Normal, Reset, Gated, Trigger, Encode 1..3
} TimerMode_t;

//! Default timer mode (can be used for halTimerOpen/halTimerMode)
extern const TimerMode_t halTimerDefaultMode;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halTimerOpen     (Device_t Device, UInt32 UserTag, halIntrHandler_t Handler);
Error_t halTimerSetup    (Handle_t Handle, const TimerMode_t *Mode, UInt32 Prescale);
Error_t halTimerClose    (Handle_t Handle);
Error_t halTimerInit     (void);

Error_t halTimerRead     (Handle_t Handle, TimRegsID_t RegID, UInt32 *Value);
Error_t halTimerWrite    (Handle_t Handle, TimRegsID_t RegID, UInt32 Value);
Error_t halTimerStatus   (Handle_t Handle, TimStatID_t StatusID);
Error_t halTimerControl  (Handle_t Handle, TimCtrlID_t ControlID);

Error_t halCapComRead    (Handle_t Handle, UInt16 UnitNo, UInt32 *Value);
Error_t halCapComWrite   (Handle_t Handle, UInt16 UnitNo, UInt32 Value);
Error_t halCapComStatus  (Handle_t Handle, UInt16 UnitNo, TimStatID_t StatusID);
Error_t halCapComControl (Handle_t Handle, UInt16 UnitNo, TimCtrlID_t ControlID);

void    halCapComWriteTimer1_Simplified (UInt16 UnitNo, UInt32 Value);

//****************************************************************************/

#endif /*HAL_TIMER_H*/
