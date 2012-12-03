/****************************************************************************/
/*! \file halWatchdog.h
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

#ifndef HAL_WATCHDOG_H
#define HAL_WATCHDOG_H


//****************************************************************************/
// Public Constants
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halWatchdogTrigger (void);
Error_t halWatchdogDisable (void);
Error_t halWatchdogEnable  (void);
Error_t halWatchdogInit    (void);

//****************************************************************************/

#endif
