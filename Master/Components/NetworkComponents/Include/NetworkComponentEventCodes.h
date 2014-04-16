/****************************************************************************/
/*! \file NetworkComponentEventCodes.h
 *
 *  \brief All event codes used by network components.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-09-2
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

#ifndef NETWORKBASE_NETWORKCOMPONENTEVENTCODES_H
#define NETWORKBASE_NETWORKCOMPONENTEVENTCODES_H

#include <../Include/PlatformEventCodes.h>

namespace NetworkBase {

const quint32 EVENT_CM_ERROR_MSG_INCOMPLETE                 = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0001; ///< ConnectionManager: Client %1. message not yet complete.
const quint32 EVENT_CM_ERROR_XML_PARSING                    = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0002; ///< ConnectionManager: Client %1. parse incoming XMl doc failed at %2.
const quint32 EVENT_CM_CLIENT_CONNECTED                     = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0003; ///< ConnectionManager: Client %1 with version connected.
const quint32 EVENT_CM_CLIENT_AUTHENTICATION_FAILED         = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0004; ///< ConnectionManager: Authentication of Client %1 with version failed.
const quint32 EVENT_CM_OUTGOINGMSG_CONVERT_ERROR            = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0005; ///< ConnectionManager: Client %1. cannot convert outgoing QString message to QDomDocument! Error:%2
const quint32 EVENT_CM_OUTGOINGMSG_EMPTY                    = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0006; ///< ConnectionManager: Client %1. outgoing QByteArray is empty.
const quint32 EVENT_CM_OUTGOINGMSG_WRITE_FAILED             = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0007; ///< ConnectionManager: Client %1. writing outgoing data to Socket failed !.

const quint32 EVENT_MLC_INIT_FAILED                         = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0008; ///< MasterLinkController: IP %1. Cannot create/initialise MasterClient at %2.
const quint32 EVENT_MLC_CONNECT_FAILED                      = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0009; ///< MasterLinkController: IP %1. Connecting LinkDevice'signal "%2" failed.
const quint32 EVENT_MLC_SIGNAL_NOT_CONNECTED                = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x000A; ///< MasterLinkController: IP %1. message emitting signal is not connected.

const quint32 EVENT_MLD_CONNECT_FAILED                      = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x000B; ///< MasterLinkDevice: Connecting LinkDevice'signal "%1" failed.
const quint32 EVENT_MLD_NETOBJECT_INIT_FAILED               = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x000C; ///< MasterLinkDevice: Cannot initialise network object.

const quint32 EVENT_MC_ERROR_LOADING_MSG                    = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x000D; ///< MessageChecker: cannot load messages at %1. Error %2.
const quint32 EVENT_MC_NULL_VALIDATOR                       = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x000E; ///< MessageChecker ERROR: validator is NULL.
const quint32 EVENT_MC_INVALID_MSG                          = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x000F; ///< MessageChecker ERROR: incoming MSG document is invalid.

const quint32 EVENT_ML_ERROR_FILE_OPEN                      = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0010; ///< MessageLoader: Opening of file %1 failed.
const quint32 EVENT_ML_ERROR_FILE_PARSING                   = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0011; ///< MessageLoader: Parsing of file %1 failed.
const quint32 EVENT_ML_NO_VALID_MSGS                        = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0012; ///< MessageLoader: File %1, element tag mismatch with tag %2.
const quint32 EVENT_ML_ERROR_ELEMENT_TAG_MISMATCH           = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0013; ///< MessageLoader::ParseCommands:No valid messages.

const quint32 EVENT_NST_ERROR_FILE_OPEN                     = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0014; ///< NetSettings: Opening of file %1 failed.
const quint32 EVENT_NST_CONFIG_FILE_PARSING_FAILED          = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0015; ///< NetSettings: Parsing of file with error %1.
const quint32 EVENT_NST_CONFIG_ELEMENT_TAG_MISMATCH         = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0016; ///< NetSettings: Element tag %1 mismatch with tag %2.
const quint32 EVENT_NST_CONFIG_FILE_VERSION_MISMATCH        = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0017; ///< NetSettings: Version mismatch with %1.
const quint32 EVENT_NST_NULL_POINTER                        = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0018; ///< NetSettings: Pointer null at %1.
const quint32 EVENT_NST_CONNECTION_SETTINGS_EMPTY           = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0019; ///< NetSettings: Connection settings empty.
const quint32 EVENT_NST_NO_VALID_CLIENT_CONNECTIONS         = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x001A; ///< NetSettings: Novalid client connections.
const quint32 EVENT_NST_NO_AUTHENTICATION_VALUES            = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x001B; ///< NetSettings: No authentication values.

const quint32 EVENT_NC_SOCKET_ERROR                         = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x001C; ///< NetworkClient: Client %1. Socket error %2.
const quint32 EVENT_NC_CONNECTION_LOST                      = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x001D; ///< NetworkClient: Client %1. Working conenction lost.
const quint32 EVENT_NC_AUTHENTICATION_FAILED                = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x001E; ///< NetworkClient: Client %1. Authentication failed.
const quint32 EVENT_NC_WRONG_SERVER_MESSAGE                 = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x001F; ///< NetworkClient: Client %1. Connection failed due to wrong server message.
const quint32 EVENT_NC_MSGHANDLER_POINTER_NULL              = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0020; ///< NetworkClient: Client %1. Msg Handler Pointer null at %2.
const quint32 EVENT_NC_OUTGOINGMSG_CONVERT_ERROR            = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0021; ///< NetworkClient: Client %1. cannot convert outgoing QString message to QDomDocument! Error:%2
const quint32 EVENT_NC_OUTGOING_MSG_EMPTY                   = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0022; ///< NetworkClient: Client %1. outgoing QByteArray is empty.
const quint32 EVENT_NC_SOCKET_WRITE_ERROR                   = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0023; ///< NetworkClient: Client %1. Writing to socket failed.
const quint32 EVENT_NC_CONFIG_PARASET_FAILED                = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0024; ///< NetworkClient: Client %1. setting config parameters failed.
const quint32 EVENT_NC_CONFIG_PARA_EMPTY                    = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0025; ///< NetworkClient: Client %1. config parameter(s) missing.


const quint32 EVENT_NCD_INIT_FAILED                         = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0026; ///< NetworkClientDevice: IP %1. initializing NetworkDevice/Client failed.
const quint32 EVENT_NCD_MSGHANDLER_REGISTRATION_FAILED      = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0027; ///< NetworkClientDevice: IP %1. Message handler registration failed.

const quint32 EVENT_NSD_INIT_FAILED                         = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0028; ///< NetworkServerDevice: initializing NetworkDevice/Server failed.

const quint32 EVENT_NL_NULL_POINTER                         = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0029; ///< NetworkLayer: IP/client %1. Null pointer error at %2.
const quint32 EVENT_NCD_NETWORK_ERROR                       = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x002A; ///< NetworkClientDevice: IP %1. Network error %2 .
const quint32 EVENT_NCD_MISSING_INPUT                       = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x002B; ///< NetworkClientDevice: IP %1. Missing input.

const quint32 EVENT_ND_WRONG_DEVICE_TYPE                    = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x002C; ///< NetworkDevice: cannot initialize wrong type of device %1.
const quint32 EVENT_ND_ERROR_LOADING_MSGS                   = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x002D; ///< NetworkDevice: cannot load messages.

const quint32 EVENT_ND_COMMAND_ALREADY_EXISTS               = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x002F; ///< NetworkDevice: command with REF %1 is already in hash.
const quint32 EVENT_ND_COMMAND_CREATE_ERROR                 = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0030; ///< NetworkDevice: creating %1 command failed .
const quint32 EVENT_ND_COMMAND_EXECUTION_ERROR              = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0031; ///< NetworkDevice: %1 command execution failed.
const quint32 EVENT_ND_UNKNOWN_MSG_TYPE                     = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0032; ///< NetworkDevice: unknown message type %1.
const quint32 EVENT_ND_MSG_PARSING_ERROR                    = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0033; ///< NetworkDevice: cannot parse message! Error %1.
const quint32 EVENT_ND_CMD_ELEMENT_DOESNOT_EXISTS           = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0034; ///< NetworkDevice: command element %1 not found.
const quint32 EVENT_ND_CMD_ELEMENT_EMPTY                    = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0035; ///< NetworkDevice: command element %1 is empty.
const quint32 EVENT_ND_COMMAND_INIT_FAILED                  = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0036; ///< NetworkDevice: command %1 cannot be initialized.

const quint32 EVENT_NS_NO_AVAILABLE_CONNECTIONS             = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0037; ///< NETSERVER: IP %1: PORT %2: Connection attempt failed: no free slots.
const quint32 EVENT_NS_DESTROY_MANAGEROBJ_FAILED            = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0038; ///< NETSERVER: IP %1: Failed to destroy Manager Object %2.
const quint32 EVENT_NS_SERVER_DISCONNECTED                  = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0039; ///< NETSERVER: IP %1: Server Disconnect reason %2.
const quint32 EVENT_NS_LOAD_SETTINGS_FAILED                 = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x003A; ///< NETSERVER: IP %1: Loading settings failed reason: %2.
const quint32 EVENT_NS_LISTEN_FAILED                        = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x003B; ///< NETSERVER: IP %1: PORT %2: Listening failed.

const quint32 EVENT_NSD_NETWORK_ERROR                       = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x003C; ///< NetworkServerDevice: Client %1: NetworkServer reported problem %2.
const quint32 EVENT_NSD_ERROR_SERVER_INT                    = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x003D; ///< NetworkServerDevice: Client %1: cannot initialize Server.
const quint32 EVENT_NSD_ERROR_SERVER_REGISTRATION           = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x003E; ///< NetworkServerDevice: Client %1: cannot register protocol handler.
const quint32 EVENT_NSD_ERROR_DATETIME_SYNC                 = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x003F; ///< NetworkServerDevice: Client %1: DateTimeSync cannot be started.

const quint32 EVENT_NL_ATTR_EMPTY                           = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0040; ///< NetworkLayer: attribute %1 is empty at %2.
const quint32 EVENT_NL_CMH_MSG_SENDING_FAILED               = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0041; ///< NetworkLayer: Message sending failed wiht status %1.

const quint32 EVENT_NL_COMMAND_NOT_COMPLETE                 = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0042; ///< NetworkLayer: command %1 is empty at %2.
const quint32 EVENT_NL_COMMAND_NOT_RUNNING                  = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0043; ///< NetworkLayer: command %1 is not a running command at %2.
const quint32 EVENT_NL_COMMAND_INVALID_REFERENCE            = EVENT_GROUP_PLATFORM_NETWORKCOMPONENT + 0x0044; ///< NetworkLayer: command %1 is corresponds to invalid reference at %2.

} // end namespace NetworkBase

#endif // NETWORKBASE_NETWORKCOMPONENTEVENTCODES_H
