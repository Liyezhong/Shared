/****************************************************************************/
/*! \file bmBlink.h
 *
 *  \brief Controls the Onboard Status LED
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all function to controls the status LED.
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

#ifndef BM_BLINK_H
#define BM_BLINK_H


//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//! IDs to specify how the status LED has to blink
typedef enum {
    LED_OFF         = 0,   //!< LED switched off
    LED_BLINK_1HZ   = 1,   //!< Blink with a rate of 1Hz
    LED_BLINK_2HZ   = 2,   //!< Blink with a rate of 2Hz
    LED_BLINK_4HZ   = 3,   //!< Blink with a rate of 4Hz
    LED_FLASH_1HZ   = 4,   //!< Flashing with a rate of 1Hz
    LED_FLASH_2HZ   = 5,   //!< Flashing with a rate of 1Hz
    LED_FLASH_SLOW  = 6,   //!< Blink very slow (standby)
    LED_BLINK_CODE2 = 7,   //!< Blink code (2 blinks + pause)
    LED_BLINK_CODE3 = 8,   //!< Blink code (3 blinks + pause)
    LED_BLINK_CODE4 = 9,   //!< Blink code (4 blinks + pause)
    LED_BLINK_CODE5 = 10,  //!< Blink code (5 blinks + pause)
    LED_BLINK_CODE6 = 11,  //!< Blink code (6 blinks + pause)
    LED_BLINK_CODE7 = 12,  //!< Blink code (7 blinks + pause)
    LED_BLINK_CODE8 = 13,  //!< Blink code (8 blinks + pause)
    LED_STATIC_ON   = 14,  //!< LED permanent on (no blinking)
    NUMBER_OF_BLINKCODES   //!< Number of blink codes
} bmBlinkCode_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t bmSignalErrorLed  (bmBlinkCode_t BlinkCode, Bool State);
Error_t bmSignalStatusLed (bmBlinkCode_t BlinkCode);
Error_t bmStatusLedTask   (void);
Error_t bmInitStatusLeds  (void);

//****************************************************************************/

#endif /*BM_BLINK_H*/
