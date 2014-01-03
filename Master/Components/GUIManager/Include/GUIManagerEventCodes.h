/****************************************************************************/
/*! \file GUIManagerEventCodes.h
 *
 *  \brief Definition of event groups for event codes.
 *
 *  Version:    0.1
 *  Date:       2010-07-12
 *  Author:     Raju
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef GUIMANAGEREVENTCODES_H
#define GUIMANAGEREVENTCODES_H

#include <QtGlobal>
#include <../Include/PlatformEventCodes.h>


const quint32 EVENT_PASSWORD_INCORRECT              = EVENT_GROUP_PLATFORM_GUI_MANAGER + 0x0001; ///< Incorrect password
const quint32 EVENT_PASSWORD_NOT_MATCHING           = EVENT_GROUP_PLATFORM_GUI_MANAGER + 0x0002; ///< New Password and confirmation password are not matching
const quint32 EVENT_PASSWORD_CHANGED_SUCCESSFULLY   = EVENT_GROUP_PLATFORM_GUI_MANAGER + 0x0003; ///< Password changed successfully

// event codes for date time
const quint32 EVENT_DATE_TIME_CANNOT_BE_MORE_THAN_24_HOURS       = EVENT_GROUP_PLATFORM_GUI_MANAGER + 0x0004; ///< Can not change the date and time less/more than 24 hours of the current system date time is %1.
const quint32 EVENT_PASSWORD_FILE_CORRUPTED                      = EVENT_GROUP_PLATFORM_GUI_MANAGER + 0x0005; ///< Password file is corrupted.
const quint32 EVENT_ERROR_CANNOT_START_SERVICE                   = EVENT_GROUP_PLATFORM_GUI_MANAGER + 0x0006; ///< Error starting Service Software. Service Software not found


#endif // GUIMANAGEREVENTCODES_H
