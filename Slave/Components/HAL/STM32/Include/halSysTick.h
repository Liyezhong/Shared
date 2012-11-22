/****************************************************************************/
/*! \file halSysTick.h
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

#ifndef HAL_SYSTICK_H
#define HAL_SYSTICK_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

UInt32  halGetFastTick (void);
Error_t halShortDelay  (UInt16 MicroSeconds);
UInt32  halTimeExpired (UInt32 SinceTime);
UInt32  halSysTickRead (void);
Error_t halSysTickInit (void);

//****************************************************************************/

#endif /*HAL_SYSTICK_H*/
