/****************************************************************************/
/*! \file RCManagerEventCodes.h
 *
 *  \brief All event codes used by external process controller.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2014-03-13
 *  $Author:    $ Ramya GJ
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

const quint32 EVENT_RCMANAGER_RECEIVED_COMMAND              = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0001; //!< RCManager.Received command %1 with data %n from RCA.
const quint32 EVENT_RCMANAGER_REMOTESESSION_DENIED          = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0002; //!< RCManager.Remote Session request denied. %1.
const quint32 EVENT_RCMANAGER_REMOTESESSION_ACCEPTED        = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0003; //!< RCManager.Remote Session request accepted by user.
const quint32 EVENT_RCMANAGER_SENT_COMMAND                  = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0004; //!< RCManager.Sent command %1 with data %n to RCA.
const quint32 EVENT_RCMANAGER_RCA_STOPPED_FOREVER           = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0005; //!< RCManager.RCA stopped working forever.
const quint32 EVENT_RCMANAGER_DATAITEM_INAVLID              = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0006; //!< RCManager.Data item name %1 is invalid.
const quint32 EVENT_RCMANAGER_DATAITEM_VALUE_INVALID        = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0007; //!< RCManager.Value %1 of data item %2 is invalid.
const quint32 EVENT_RCMANAGER_DATAITEM_TYPE_INVALID         = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0008; //!< RCManager.Type %1 of data item %2 is invalid.
const quint32 EVENT_RCMANAGER_INVALID_REQUEST_SUBSCRIPTION_NOTSET    = EVENT_GROUP_PLATFORM_RC_MANAGER + 0x0009; //!< RCManager.RemoteCare subscription is not set.

} // end namespace RemoteCare

#endif // PLATFORM_RCMANAGEREVENTCODES_H
