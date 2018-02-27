/****************************************************************************/
/*! \file RemoteCareEventCodes.h
 *
 *  \brief  Error codes of the RemoteCare Agent
 *
 *   Version: 0.1
 *   Date:    26.08.2013
 *   Author:  Boris Stach
 *
 *  \b Description:
 *
 *       This module contains the error and event defines of the device
 *       control layer.
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

#ifndef RCAGENTNAMESPACE_EVENTCODES_H
#define RCAGENTNAMESPACE_EVENTCODES_H

#include <Shared/Master/Include/PlatformEventCodes.h>

namespace RCAgentNamespace
{

//! AgentController events
const quint32 EVENT_REMOTECARE_ERROR_CONNECTION_TO_MASTER  = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0001; //! Self explaining

//! MasterConnector events
const quint32 EVENT_REMOTECARE_INFO_CONNECTED_TO_MASTER    = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0101; //! Self explaining

//! REServerConnector errors
const quint32 EVENT_REMOTECARE_ERROR_CONFIGURATION_READ                 = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0201; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_WEB_ACCESS                         = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0202; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_READ_SETTINGS                      = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0203; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_SET_SETTINGS                       = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0204; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_POST_DATAITEMS                     = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0205; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_BAD_ALLOC                          = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0206; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_NOT_INITIALIZED                    = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0207; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_SERVER_STATUS                      = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0208; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_UPLOAD_FILE                        = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0209; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_SET_TAG                            = EVENT_GROUP_PLATFORM_REMOTECARE + 0x020A; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_SUBMITEVENT_POST                   = EVENT_GROUP_PLATFORM_REMOTECARE + 0x020B; //! Self explaining

const quint32 EVENT_REMOTECARE_INFO_DEVICE_REGISTERED                   = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0251; //! Self explaining
const quint32 EVENT_REMOTECARE_INFO_SET_TAG_RECEIVED                    = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0252; //! Self explaining
const quint32 EVENT_REMOTECARE_INFO_SUBMITEVENT_SEVERITY_CONVERSION     = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0253; //! Self explaining
const quint32 EVENT_REMOTECARE_INFO_START_REMOTESESSION                 = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0254; //! Self explaining
const quint32 EVENT_REMOTECARE_INFO_END_REMOTESESSION                   = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0255; //! Self explaining
const quint32 EVENT_REMOTECARE_INFO_START_DOWNLOAD                      = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0256; //! Self explaining
const quint32 EVENT_REMOTECARE_INFO_END_DOWNLOAD                        = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0257; //! Self explaining
const quint32 EVENT_REMOTECARE_INFO_CONNECTED_ENTERPRISE_SERVER         = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0258; //! Self explaining

//! RC configuration events
const quint32 EVENT_REMOTECARE_ERROR_READ_SETTINGS_DEVICE           = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0301; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_READ_SETTINGS_USER             = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0302; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_READ_SETTINGS_RC               = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0303; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_SET_SETTING_AE_INIT            = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0304; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_SET_SETTINGS_MODELNR_EMPTY     = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0305; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_SET_SETTINGS_ADD_DEVICE        = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0306; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_SET_SETTINGS_ADD_SERVER        = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0307; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_SET_SETTINGS_SSL               = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0308; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_SET_SETTINGS_REMOTE_SESSION    = EVENT_GROUP_PLATFORM_REMOTECARE + 0x0309; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_SET_SETTINGS_PROXY             = EVENT_GROUP_PLATFORM_REMOTECARE + 0x030A; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_POSTITEM_DA_CONVERSION         = EVENT_GROUP_PLATFORM_REMOTECARE + 0x030B; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_POSTITEM_DD_CONVERSION         = EVENT_GROUP_PLATFORM_REMOTECARE + 0x030C; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_POSTITEM_NOT_KNOWN             = EVENT_GROUP_PLATFORM_REMOTECARE + 0x030D; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_POSTITEM                       = EVENT_GROUP_PLATFORM_REMOTECARE + 0x030E; //! Self explaining
const quint32 EVENT_REMOTECARE_ERROR_SET_SETTINGS_WEB_PERSIST       = EVENT_GROUP_PLATFORM_REMOTECARE + 0x030F; //! Self explaining

} //namespace

#endif /* REMOTECARE_EVENTCODES_H */
