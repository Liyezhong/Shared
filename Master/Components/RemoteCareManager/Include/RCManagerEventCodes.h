/****************************************************************************/
/*! \file RCManagerEventCodes.h
 *
 *  \brief All event codes used by external process controller.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-06-15
 *  $Author:    $ J.Bugariu
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

#ifndef REMOTECARE_RCMANAGEREVENTCODES_H
#define REMOTECARE_RCMANAGEREVENTCODES_H

#include <../Include/PlatformEventCodes.h>

namespace RemoteCare {

const quint32 EVENT_RCMANAGER_RECEIVED_COMMAND              = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0001; ///< RCManager.Received command %1 with data %n from RCA.
const quint32 EVENT_RCMANAGER_REMOTESESSION_DENIED          = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0002; ///< RCManager.Remote Session request denied. %1.
const quint32 EVENT_RCMANAGER_REMOTESESSION_ACCEPTED        = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0003; ///< RCManager.Remote Session request accepted by user.
const quint32 EVENT_RCMANAGER_SENT_COMMAND                  = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0004; ///< RCManager.Sent command %1 with data %n to RCA.
const quint32 EVENT_RCMANAGER_RCA_STOPPED_FOREVER           = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0005; ///< RCManager.RCA stopped working forever.

} // end namespace RemoteCare

#endif // PLATFORM_RCMANAGEREVENTCODES_H
