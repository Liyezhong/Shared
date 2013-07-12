/****************************************************************************/
/*! \file bmBlink.c
 *
 *  \brief Controls the Onboard Status LED
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions to control the onboard status
 *       and/or error LED. Blink codes are selected using an enumeration
 *       that serves as an index into a fixed table of blink patterns.
 *       Blink patterns can describe a constant blink rate in different
 *       frequencies, but can also describe a blink sequence, i.e. "n"
 *       blinks in a row followed by a longer pause.
 *
 *       Constant blink rates are intended to be used to signal the node
 *       state on the status LED, while blink sequences are intended to
 *       be used to signal important errors (the number of blinks in a
 *       row is the error code). However, each blink code can be used
 *       as status or error code. The calling of the right signalling
 *       function determines, if the code is an error or status code.
 *
 *       This module supports separate LEDs for status and errors, but
 *       also a single shared LED. In case of a shared LED, errors have
 *       precedence over status, i.e. as long as an error is signalled,
 *       status blinks will be suppressed (but not forgotten).
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
#include "halLayer.h"
#include "bmError.h"
#include "bmTime.h"
#include "bmDebug.h"
#include "bmBlink.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define NUMBER_OF_LEDS   2  //!< Number of LEDs (1 or 2)

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! LED control task states used in bmStatusLedTask()
typedef enum {
    STATE_LED_START,    //!< Start new blink sequence
    STATE_LED_ON,       //!< LED-on phase
    STATE_LED_OFF,      //!< LED-off phase
    STATE_LED_PAUSE     //!< Pause between blink sequences
} bmLedTaskState_t;

//! Blink pattern - defines a blink sequence for the LED
typedef struct {
    UInt16 TimeOn;      //!< Duration of LED-on phase [ms]
    UInt16 TimeOff;     //!< Duration of LED-off phase [ms]
    UInt16 TimePause;   //!< Time between blink sequences [ms]
    UInt8  Count;       //!< Number of blinks per sequence
    UInt8  PortOn;      //!< Port value to switch LED on
    UInt8  PortOff;     //!< Port value to switch LED off
} bmBlinkPattern_t;

//! State machine status - contains state for one LED
typedef struct {
    UInt16 State;       //!< State of blink state machine
    UInt16 Code;        //!< Actual blink code
    UInt32 Time;        //!< Blink phase start time
    UInt16 Delay;       //!< Blink phase duration
    UInt16 Count;       //!< Number of blinks
    UInt16 Value;       //!< Actual LED on/off value
    Handle_t Handle;    //!< Handle to access digital output
} bmBlinkState_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

//! This table contains the parameters for the different blink codes
static const bmBlinkPattern_t BlinkPatterns[] = {
    { 500,  500,   0,  1,  0,  0 },  //!< LED off
    { 500,  500,   0,  1,  1,  0 },  //!< LED_BLINK_1HZ
    { 250,  250,   0,  1,  1,  0 },  //!< LED_BLINK_2HZ
    { 125,  125,   0,  1,  1,  0 },  //!< LED_BLINK_4HZ
    { 100,  900,   0,  1,  1,  0 },  //!< LED_FLASH_1HZ
    { 100,  400,   0,  1,  1,  0 },  //!< LED_FLASH_2HZ
    { 100, 9900,   0,  1,  1,  0 },  //!< LED_FLASH_SLOW (Standby)
    { 100,  400, 900,  2,  1,  0 },  //!< LED_BLINK_CODE2
    { 100,  400, 900,  3,  1,  0 },  //!< LED_BLINK_CODE3
    { 100,  400, 900,  4,  1,  0 },  //!< LED_BLINK_CODE4
    { 100,  400, 900,  5,  1,  0 },  //!< LED_BLINK_CODE5
    { 100,  400, 900,  6,  1,  0 },  //!< LED_BLINK_CODE6
    { 100,  400, 900,  7,  1,  0 },  //!< LED_BLINK_CODE7
    { 100,  400, 900,  8,  1,  0 },  //!< LED_BLINK_CODE8
    { 500,  500,   0,  1,  1,  1 }   //!< LED permanent on
};

static bmBlinkState_t BlinkState[NUMBER_OF_LEDS]; //!< State of all LEDs

static UInt16 BlinkErrorCode  = 0;  //!< Actual active error LED blink code
static UInt16 BlinkStateCode  = 0;  //!< Actual active status LED blink code

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static UInt16 bmGetNextBlinkCode (UInt16 LedNr);



/*****************************************************************************/
/*!
 *  \brief    Signal blink code on error LED
 *
 *      Controls the blinking on the error LED. BlinkCode specifies the
 *      blinking code, State specifies if the code have to be started (TRUE)
 *      or stopped (FALSE). Blink codes are prioritized: higher values
 *      override lower values. If the highest priority blink code gets
 *      stopped, the next lower priority blink code will become active.
 *      If a blink code is activated, while a higher priority blink code
 *      is already active, this blink code is not forgotten, but becomes
 *      active, when the higher priority blink code is stopped.
 *
 *      Blink code LED_OFF can be used to reset ALL scheduled blink codes,
 *      i,e. to stop blinking without the need to stop each single code.
 *
 *  \iparam   BlinkCode = blink code
 *  \iparam   State     = start (TRUE) or stop (FALSE) blink code
 *
 *  \return   NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmSignalErrorLed (bmBlinkCode_t BlinkCode, Bool State) {

    static UInt32 PriorityBitmask = 0;
    UInt32 Bitmask;
    UInt16 i;

    // if blink code is 0 (led off), reset all errors
    if (!BlinkCode && State) {
        PriorityBitmask = 0;
    }
    // determine highest priority blink code
    if (BlinkCode < NUMBER_OF_BLINKCODES) {
        if (State) {
            PriorityBitmask |=  BIT(BlinkCode);
        }
        else {
            PriorityBitmask &= ~BIT(BlinkCode);
        }
        Bitmask = PriorityBitmask;

        for (i=0; i < NUMBER_OF_BLINKCODES; i++) {
            if (Bitmask) {
                Bitmask >>= 1;
            }
            if (!Bitmask) {
                BlinkErrorCode = i;
                return (NO_ERROR);
            }
        }
    }
    return (E_PARAMETER_OUT_OF_RANGE);
}


/*****************************************************************************/
/*!
 *  \brief    Signal blink code on status LED
 *
 *      Controls the blink code on the status LED. Parameter BlinkCode
 *      specifies the blink sequence. Other than on the error LED, new
 *      blink codes overrides existing ones; there is no priorization.
 *      If status and error codes share a single LED, error codes have
 *      precedence over status code. The status code is displayed not
 *      before the last error code is removed.
 *
 *  \iparam   BlinkCode = blink code
 *
 *  \return   NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmSignalStatusLed (bmBlinkCode_t BlinkCode) {

    if (BlinkCode < NUMBER_OF_BLINKCODES) {

        if (BlinkCode != BlinkStateCode) {
            dbgPrint ("Status Blink Code %d", BlinkCode);
        }
        BlinkStateCode = BlinkCode;
        return (NO_ERROR);
    }
    return (E_PARAMETER_OUT_OF_RANGE);
}


/*****************************************************************************/
/*!
 *  \brief   LED control task
 *
 *         This is the task function to control blinking on the status and
 *         error LED. Depending on the fact, how many LEDs are present,
 *         status and error codes are displayed on two different LEDs or
 *         share a common LED. If a common LED is used, errors have
 *         precedence over status, i.e. status codes are not displayed
 *         before the last error code is removed.
 *
 *         Once started, a blink code is output until finished. A new blink
 *         sequence can not be started, before the previous one is done.
 *         This is independent of priority.
 *
 *         This function must be called on a regular base by the scheduler.
 *
 *  \return   NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmStatusLedTask (void) {

    bmBlinkState_t *Blink = BlinkState;
    UInt16 Led;

    for (Led=0; Led < NUMBER_OF_LEDS; Led++) {

        if (bmTimeExpired(Blink->Time) >= Blink->Delay) {

            const bmBlinkPattern_t *Pattern = &BlinkPatterns[Blink->Code];
            const UInt16 curLedValue = Blink->Value;

            switch (Blink->State) {

                case STATE_LED_START:
                    Blink->Code  = bmGetNextBlinkCode(Led);
                    Blink->Count = Blink->Delay = 0;
                    Blink->State = STATE_LED_ON;
                    break;

                case STATE_LED_ON:
                    Blink->Delay = Pattern->TimeOn;
                    Blink->Value = Pattern->PortOn;
                    Blink->State = STATE_LED_OFF;
                    break;

                case STATE_LED_OFF:
                    Blink->State =
                        (++Blink->Count < Pattern->Count) ? STATE_LED_ON : STATE_LED_PAUSE;
                    Blink->Delay = Pattern->TimeOff;
                    Blink->Value = Pattern->PortOff;
                    break;

                case STATE_LED_PAUSE:
                    Blink->Delay = Pattern->TimePause;
                    Blink->Value = Pattern->PortOff;
                    Blink->State = STATE_LED_START;
                    break;
            }
            if (Blink->Value != curLedValue) {
                halPortWrite (Blink->Handle, Blink->Value);
            }
            Blink->Time = bmGetTime();
        }
        Blink++;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Get next blink code
 *
 *         Returns the next blink code for the given LED. If two LEDs are
 *         present, the 1st one is used to signal errors, the 2nd one
 *         to signal the actual node state. If only one LED is present,
 *         errors and states are signaled on the same LED. Errors have
 *         precedence over state signalling, i.e. states are signalled
 *         only if there is no error to signal.
 *
 *  \iparam  LedNr = LED number
 *
 *  \return  Blink code (0 = no blinking)
 *
 ****************************************************************************/

static UInt16 bmGetNextBlinkCode (UInt16 LedNr) {

    if (NUMBER_OF_LEDS > 1) {                                   //lint !e506
        return (LedNr ? BlinkErrorCode : BlinkStateCode);
    }
    else if (BlinkErrorCode) {
        return (BlinkErrorCode);
    }
    return (BlinkStateCode);
}


/*****************************************************************************/
/*!
 *  \brief   Status-LED Initialization
 *
 *         This function initializes this module. It must be called once
 *         before using any of the functions in this module. It opens the
 *         LED output port(s) and initializes the state machine.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmInitStatusLeds (void) {

    Error_t errStatus = NO_ERROR;
    Handle_t Handle;
    UInt16 i;

    for (i=0; i < NUMBER_OF_LEDS; i++) {

        Handle = halPortOpen (HAL_STATUS_LED1 + i, HAL_OPEN_WRITE);
        if (Handle < 0) {
            errStatus = Handle;
        }
        BlinkState[i].Handle = Handle;
        BlinkState[i].State  = STATE_LED_START;
        BlinkState[i].Time   = 0;
        BlinkState[i].Delay  = 0;
     }
    return (errStatus);
}

//****************************************************************************/

