/****************************************************************************/
/*! \file halMain.c
 *
 *  \brief  Hardware abstraction layer main module
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to initialize the hardware abstraction
 *      layer. This is done by calling the initialization functions in the
 *      various HAL layer modules.
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
#include "halCommon.h"
#include "halError.h"
#include "halConsole.h"
#include "halInterrupt.h"
#include "halExtIntr.h"
#include "halUtilities.h"
#include "halStorage.h"
#include "halPorts.h"
#include "halI2cBus.h"
#include "halSpiBus.h"
#include "halAD779x.h"
#include "halAnalog.h"
#include "halSysTick.h"
#include "halRealTime.h"
#include "halStepper.h"
#include "halWatchdog.h"
#include "halTimer.h"
#include "halSerial.h"
#include "halClocks.h"
#include "halEncoder.h"
#include "halPwm.h"
#include "halCan.h"
#include "halMain.h"


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

typedef Error_t (*halInitFunction_t)(void); //!< Pointer to HAL init functions

//****************************************************************************/
// Private Variables
//****************************************************************************/

static halInitState_t halInitState = INIT_NOT_DONE;  //!< Initialization state


/*****************************************************************************/
/*!
 *  \brief   Initialize HAL layer
 *
 *      Initializes the complete hardware abstraction layer by calling all
 *      initialization functions in the other HAL layer modules. Interrupts
 *      are disabled and access to the protected hardware registers are
 *      allowed until initialization is done.
 *
 *      If initialization fails the function is immediately aborted and
 *      an error code is returned. The hardware is in an undefined state
 *      in this case and the HAL layer function shouldn't be used.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halInitializeLayer (void) {

    const halInitFunction_t halInitFunctions[] = {
        halClocksInit,
        halConsoleInit,
        halInterruptInit,
        halSysTickInit,
        halExtIntInit,
        halI2cInit,
        halPortInit,
        halSpiInit,
        halCanInit,
        halTimerInit,
        halPwmInit,
        halAnalogInit,
        halEncoderInit,
        halStepperInit,
        halSerialInit,
        halRtcInit,
        halStorageInit,
        halUtilitiesInit,
        halWatchdogInit
    };
    static Error_t Status = NO_ERROR;
    UInt32 i;

    if (halInitState == INIT_NOT_DONE) {

        halGlobalInterruptDisable();
        halInitState = INIT_IN_PROGRESS;

        for (i=0; i < ELEMENTS(halInitFunctions); i++) {
            if ((Status = halInitFunctions[i]()) != NO_ERROR) {
                halInitState = INIT_FAILED;
                return (Status);
            }
        }
        halInitState = INIT_SUCCEEDED;
        halGlobalInterruptEnable();
    }
    return (Status);
}


/*****************************************************************************/
/*!
 *  \brief   Get initialization state
 *
 *      Returns the state of initialization, which can be:
 *
 *      - Initialization not done
 *      - Initialization in progress
 *      - Initialization failed
 *      - Initialization successful done
 *
 *  \return  Initialization state
 *
 ****************************************************************************/

halInitState_t halGetInitState (void) {

    return (halInitState);
}


/*****************************************************************************/
/*!
 *  \brief   Set initialization state
 *
 *      Sets the state of initialization, which can be:
 *
 *      - Initialization not done
 *      - Initialization in progress
 *      - Initialization failed
 *      - Initialization successful done
 *
 *  \iparam  InitState = Initialization state
 *
 ****************************************************************************/

void halSetInitState (halInitState_t InitState) {

    halInitState = InitState;
}


/*****************************************************************************/
/*!
 *  \brief   HAL control task
 *
 *      This function should be called regulary. It handles asynchronous
 *      events in the HAL layer.
 *
 *      Actually, only the real time clock is updated.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halControlTask (void) {

    // call AD779x external ADC scan task
    if (halAd779xDescriptorCount) {
        halAd779xTask();
    }
    return (NO_ERROR);
}

//****************************************************************************/
