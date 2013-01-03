/****************************************************************************/
/*! \file Include/EventHandlerEventCodes.h
 *
 *  \brief Definition of event groups for event codes.
 *
 *  Version:    0.1
 *  Date:       2012-10-23
 *  Author:     Michael Thiel
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

#ifndef EVENTHANDLEREVENTCODES_H
#define EVENTHANDLEREVENTCODES_H

#include <QtGlobal>

const quint32 EVENT_ERROR_STATE                     = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0001;
const quint32 EVENT_NORMAL_STATE                    = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0002;
const quint32 EVENT_GUI_AVAILABLE                   = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0003;



#endif // EVENTHANDLEREVENTCODES_H
