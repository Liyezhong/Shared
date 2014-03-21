/****************************************************************************/
/*! \file halWatchdog.c
 *
 *  \brief  Watchdog Control Functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to control the watchdog timer. It
 *      uses the processor-internal independent watchdog, which has its
 *      own internal osscillator. The watchdog interval time must be
 *      configured in the HAL configuration module.
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
#include "halMain.h"
#include "halWatchdog.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define IWDG_CLK_RATE       40000    //!< Watchdog input clock rate
#define IWDG_RLR_MASK       0xFFF    //!< Watchdog reload value bitmask

#define IWDG_KEY_START      0xCCCC   //!< Key register start pattern
#define IWDG_KEY_TRIGGER    0xAAAA   //!< Key register trigger pattern
#define IWDG_KEY_UNPROTECT  0x5555   //!< Key register unprotect pattern
#define IWDG_KEY_PROTECT    0x0000   //!< Key register protect pattern

#define IWDG_SR_RVU         0x0002   //!< Counter reload register update
#define IWDG_SR_PVU         0x0001   //!< Prescaler register update


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Layout of processor internal watchdog control registers
typedef volatile struct {
    UInt32 KR;          //!< Watchdog Key register
    UInt32 PR;          //!< Watchdog Prescaler register
    UInt32 RLR;         //!< Watchdog Reload register
    UInt32 SR;          //!< Watchdog Status register
} WdgRegFile_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Hardware registers to control independent watchdog
static WdgRegFile_t *WDG = (WdgRegFile_t*) 0x40003000;



/*****************************************************************************/
/*!
 *  \brief   Trigger watchdog timer
 *
 *      Triggers and resets the watchdog timer. This function must be called
 *      in a regular time interval to prevent the watchdog from initiating a
 *      hardware reset. The maximal trigger interval is specified in the HAL
 *      configuration file.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halWatchdogTrigger (void) {

    WDG->KR = IWDG_KEY_TRIGGER;
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Enable watchdog timer
 *
 *      Enables and starts the watchdog timer.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halWatchdogEnable (void) {

    if (halWatchdogInterval) {                                  //lint !e506
        WDG->KR = IWDG_KEY_UNPROTECT;
        WDG->KR = IWDG_KEY_START;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Disable watchdog timer
 *
 *      Disables the watchdog timer. Disabling the watchdog is possible only
 *      in debug mode and only if the hardware supports this feature.
 *
 *      The STM32 doesn't support this, therefor an error is returned.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halWatchdogDisable (void) {

    return (E_UNSUPPORTED_FUNCTION);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize watchdog
 *
 *      Initializes the watchdog by setting up the prescaler and reload
 *      registers. The watchdog uses it's own internal 40kHz oscillator,
 *      which is independent of the main clocks. It should be kept in
 *      mind, that the internal oscillator is not very accurate and can
 *      vary between 30 and 60 kHz. If halWatchdogInterval is set to 0,
 *      the watchdog stays disabled.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halWatchdogInit (void) {

    UInt32 PreScaler = 4;
    UInt32 Index = 0;

    if (halGetInitState() == INIT_IN_PROGRESS) {
        if (halWatchdogInterval) {                           //lint !e506
            
            UInt32 Counter = IWDG_CLK_RATE * halWatchdogInterval / 1000;
        
            while (Counter/PreScaler > IWDG_RLR_MASK) {
                if (PreScaler > 256) {
                    return (E_WATCHDOG_INTERVAL);
                }
                PreScaler <<= 1;
                Index++;
            }
            WDG->KR = IWDG_KEY_UNPROTECT;
            while (WDG->SR & IWDG_SR_PVU) {}
            WDG->PR = Index;
    
            while (WDG->SR & IWDG_SR_RVU) {}
            WDG->RLR = (Counter / PreScaler) & IWDG_RLR_MASK;
            WDG->KR = IWDG_KEY_PROTECT;
        }
    }
    return (NO_ERROR);
}

//****************************************************************************/

