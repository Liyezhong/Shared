/****************************************************************************/
/*! \file halPwm.c
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
 *      software to other hardware plattforms (which might have dedicated 
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

#include "Global.h"
#include "halError.h"
#include "halClocks.h"
#include "halCommon.h"
#include "halTimer.h"
#include "halPwm.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define PWM_REF_CLOCKRATE   100000000   //!< Reference clock is 100MHz
#define PWM_MAX_CHANNELS    4           //!< Maximal number of PWM channels


/*****************************************************************************/
/*! 
 *  \brief   Open a PWM controller
 *
 *      Opens a PWM controller and returns a handle. DeviceID must be the 
 *      numerical "name" of a PWM device. The parameters Mode and Period 
 *      have the same meaning as halPwmSetup (see there). If Period time
 *      is 0, an error is returned.
 * 
 *      Handler must be a pointer to an interrupt handler function to be 
 *      called when the device signals an interrupt. It can be NULL if no
 *      interrupts are used. UserTag is a user defined number to be passed 
 *      to the handler function, e.g. as index into a data table in case 
 *      of multiple instances. 
 *
 *  \iparam  DeviceID = Logical device ID
 *  \iparam  UserTag  = User tag to pass thru to interrupt handler
 *  \iparam  Handler  = Interrupt handler function
 * 
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

Error_t halPwmOpen (Device_t DeviceID, UInt32 UserTag, halIntrHandler_t Handler) {

    const PwmMode_t DefaultMode = { 
        TIM_MODE_COUNT_UP, PWM_MODE_INTERVAL, PWM_MODE_INTERNAL, 0};
    Handle_t Handle;

    if ((Handle = halTimerOpen (DeviceID, UserTag, Handler)) > 0) {
                                                           
        Error_t Status = halPwmSetup (Handle, DefaultMode, MAX_UINT32);

        if (Status == NO_ERROR) {
            Status = halTimerControl(Handle, TIM_CTRL_START);
        }
        if (Status != NO_ERROR) {
            halTimerClose(Handle);
            return (Status);
        }
    }
    return (Handle);
}


/*****************************************************************************/
/*! 
 *  \brief   Close PWM controller
 *
 *      Closes an open PWM controller  
 * 
 *  \iparam  Handle = Handle of PWM controller
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halPwmClose (Handle_t Handle) {

    return (halTimerClose(Handle));
}


/*****************************************************************************/
/*!
 *  \brief   Setup PWM controller's mode
 *
 *      Sets or changes the operational mode of a PWM controller according
 *      to the given parameters. The following mode parameters can be set:
 *
 *      - Count direction (up, down, center aligned)
 *      - Repetition mode (continuous, one-shot)
 *      - Clock source (internal, external)
 *      - External control (trigger, reset, gate, none)
 *
 *      Period defines the period time of the PWM signal in 10ns steps
 *      in internal clock mode or in number of external clocks in 
 *      external clock mode. Period time should be at least 10 times
 *      higher than the input clock of the PWM controller (internal
 *      or external clock). 
 *
 *      If Period is 0, that particular parameter is not evaluated and  
 *      the related setting is left unchanged. 
 *
 *  \iparam  Handle = Handle of PWM controller
 *  \iparam  Mode   = Mode parameters
 *  \iparam  Period = PWM period time
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halPwmSetup (Handle_t Handle, PwmMode_t Mode, UInt32 Period) {

    UInt16 Width[PWM_MAX_CHANNELS];
    UInt16 Prescale = 0;
    Error_t Status;

    if (Period) {
        UInt16 Units = halTimerStatus(Handle, TIM_STAT_UNITS);
        UInt16 UnitNo;

        if (Units > PWM_MAX_CHANNELS) {
            Units = PWM_MAX_CHANNELS;
        }
        // Save actual pulse widths for later reconstruction 
        for (UnitNo=0; UnitNo < Units; UnitNo++) {
            halPwmRead (Handle, UnitNo, &Width[UnitNo]);
        }
        // Normalize period time to reference clock (internal mode only)
        if (Mode.ClkSource == PWM_MODE_INTERNAL) {
            Period = (UInt64) Period * 
                halTimerStatus(Handle, TIM_STAT_CLOCKRATE) / PWM_REF_CLOCKRATE;
        }
        if (Mode.Direction & PWM_MODE_COUNT_UPDN) {
            Period /= 2;
        }
        Prescale = (Period / MAX_UINT16) + 1;

        // Setup period (prescale) register
        Status = halTimerWrite (Handle, TIM_REG_RELOAD, Period / Prescale - 1);
        if (Status < 0) {
            return (Status);
        }
        // Reconstruct pulse width of all PWM channels
        for (UnitNo=0; UnitNo < Units; UnitNo++) {
            halPwmWrite (Handle, UnitNo, Width[UnitNo]);
        }
    }
    return (halTimerSetup (Handle, &Mode, Prescale));
}


/*****************************************************************************/
/*! 
 *  \brief   Read PWM pulse width
 *
 *      Reads the actual pulse width value from the PWM device associated 
 *      with Handle and copies it to the variable pointed to by Value. 
 *      The value is scaled to 16 bit: 0xFFFF indicates 100% pulse width.
 *
 *      UnitNo selects the channel of a multi-channel PWM device. The 
 *      number of channels a device supports can be requested using
 *      halPwmStatus().
 *
 *  \iparam  Handle = Handle of PWM controller
 *  \iparam  UnitNo = PWM channel number
 *  \oparam  Width  = Pointer to a variable to return result
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halPwmRead (Handle_t Handle, UInt16 UnitNo, UInt16 *Width) {

    Error_t Status;
    UInt32 Period;
    UInt32 Value;

    Status = halTimerRead (Handle, TIM_REG_RELOAD, &Period);
    if (Status == NO_ERROR) {
        Status = halCapComRead (Handle, UnitNo, &Value);

        if (Width != NULL) {
            *Width = Value * MAX_UINT16 / (Period + 1);
        }
    }
    return (Status);
}


/*****************************************************************************/
/*! 
 *  \brief   Write PWM pulse width
 *
 *      Writes a new pulse width value to the PWM device associated with
 *      Handle. Value must be scaled to 16 bit: 0xFFFF indicates 100% 
 *      pulse width. UnitNo selects the channel of a multi-channel PWM 
 *      device. The number of channels a device supports can be requested 
 *      using halPwmStatus().
 *
 *  \iparam  Handle = Handle of PWM controller
 *  \iparam  UnitNo = PWM channel number
 *  \iparam  Width  = New PWM pulse width
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halPwmWrite (Handle_t Handle, UInt16 UnitNo, UInt16 Width) {

    Error_t Status;
    UInt32 Period;

    Status = halTimerRead (Handle, TIM_REG_RELOAD, &Period);
    if (Status == NO_ERROR) {
        UInt32 Value = (UInt32) (Period + 1) * Width / MAX_UINT16;

        return (halCapComWrite (Handle, UnitNo, Value));    
    }
    return (Status);
}

 
/*****************************************************************************/
/*!
 *  \brief   Controls a PWM output
 *
 *      Controls the PWM channel defined by UnitNo of the PWM controller 
 *      associated with Handle in several ways. Depending on ControlID  
 *      the following actions can be initiated:
 *
 *      - Start/stop PWM generation
 *      - Enable/disable PWM interrupt
 *      - Clear PWM interrupt flag
 *
 *      UnitNo selects the channel of a multi-channel PWM controller.
 *      If interrupt is enabled, it will be asserted when the inner
 *      edge of the PWM signal changes state.
 *
 *  \iparam  Handle    = Handle of PWM controller
 *  \iparam  UnitNo    = PWM channel number
 *  \iparam  ControlID = Type of control
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halPwmControl (
            Handle_t Handle, UInt16 UnitNo, PwmCtrlID_t ControlID) {

    TimCtrlID_t TimerControl;

    switch (ControlID) {    
        case PWM_CTRL_START:
            TimerControl = TIM_CTRL_START; break;

        case PWM_CTRL_STOP:
            TimerControl = TIM_CTRL_STOP; break;

        case PWM_INTR_ENABLE:
            TimerControl = TIM_INTR_ENABLE; break;

        case PWM_INTR_DISABLE:
            TimerControl = TIM_INTR_DISABLE; break;

        case PWM_INTR_CLEAR:
            TimerControl = TIM_INTR_CLEAR; break;

        default:
            return (E_UNKNOWN_CONTROL_ID);
    }
    return (halCapComControl(Handle, UnitNo, TimerControl));
}


/*****************************************************************************/
/*!
 *  \brief   Get status of a PWM output
 *
 *      Returns the status of the PWM channel defined by UnitNo of the PWM 
 *      device associated with Handle. Which status to return depends on  
 *      the supplied StatID. Following informations can be requested:
 *
 *      - Pending interrupt status
 *      - Number of PWM channels
 *      - Hardware clock rate
 *      - Resolution of PWM signal (number of steps)
 *
 *      Changing the period time of the PWM signal also changes resolution 
 *      of the PWM signal.
 *
 *      UnitNo selects the channel of the multi-channel PWM device.
 *      If requesting the number of channel, UnitNo is don't care.
 *
 *  \iparam  Handle   = Handle of PWM controller
 *  \iparam  UnitNo   = PWM channel number
 *  \iparam  StatusID = Status identifier
 *
 *  \return  Status of PWM output or (negative) error code
 *
 ****************************************************************************/

Error_t halPwmStatus (Handle_t Handle, UInt16 UnitNo, PwmStatID_t StatusID) {

    Error_t Status;
    UInt32 Period;

    switch (StatusID) {
        case PWM_STAT_PENDING:
            return (halCapComStatus(Handle, UnitNo, TIM_STAT_PENDING));

        case PWM_STAT_CHANNELS:
            return (halTimerStatus(Handle, TIM_STAT_UNITS));

        case PWM_STAT_CLOCKRATE: 
            return (halTimerStatus(Handle, TIM_STAT_CLOCKRATE));
            
        case PWM_STAT_RESOLUTION:
            Status = halTimerRead (Handle, TIM_REG_RELOAD, &Period);
            if (Status != NO_ERROR) {
                return (Status);
            }
            return (Period + 1);
    }
    return (E_UNKNOWN_STATUS_ID);
}


/*****************************************************************************/
/*! 
 *  \brief   Initialize PWM module
 *
 *      Initializes this module.
 * 
 *      This function is called only once during startup.
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halPwmInit (void) {

    return (NO_ERROR);
}

//****************************************************************************/
