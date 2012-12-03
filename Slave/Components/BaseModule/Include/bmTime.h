/****************************************************************************/
/*! \file bmTime.h
 *
 *  \brief Time and timer handling functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions to handle times. This includes
 *       the following:
 *
 *       - Local system ticks (local free running counter)
 *       - System clock (suppied by master)
 *       - Real time and date (supplied by master)
 *
 *       The tick counter is intended to measure time differences, the system
 *       clock can be used to synchronize actions on different slaves. The
 *       real time/date can be used, when absolute time informations are
 *       required. The real time must be send by the master via the CAN
 *       time/date message at least once a day.
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

#ifndef BM_TIME_H
#define BM_TIME_H

#include "halRealTime.h"

//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

UInt32  bmGetTime (void);
UInt32  bmTimeExpired (UInt32 SinceTime);

UInt16  bmGetSysClock (void);
UInt16  bmSysClockExpired (UInt16 ReferenceClock);
Error_t bmReadDateTime (DateTime_t *Date);
UInt32  bmGetSecondsOfTheDay (void);
Bool    bmIsLeapYear (UInt32 Year);

Error_t bmInitTimeModule (void);

//****************************************************************************/

#endif /*BM_TIME_H*/
