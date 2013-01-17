/****************************************************************************/
/*! \file PlatformEventCodes.h
 *
 *  \brief Definition of event groups for event codes.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
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

#ifndef PLATFORMEVENTCODES_H
#define PLATFORMEVENTCODES_H

#include <QtGlobal>

const quint32 EVENT_GROUP_SOURCENAME    = 0x0000;   ///< Event group for logging source translations.

/*! Note : Add event groups for your platform components here */
const quint32 EVENT_GROUP_PLATFORM      = 0x01;             ///< Event group for platform.
const quint32 EVENT_GROUP_PLATFORM_GLOBAL                   = ((EVENT_GROUP_PLATFORM << 8) + 0x01) << 16;   ///< Event group for global stuff.
const quint32 EVENT_GROUP_PLATFORM_THREADS                  = ((EVENT_GROUP_PLATFORM << 8) + 0x02) << 16;   ///< Event group for thread stuff.
const quint32 EVENT_GROUP_PLATFORM_DATALOGGING              = ((EVENT_GROUP_PLATFORM << 8) + 0x03) << 16;   ///< Event group for data logging.
const quint32 EVENT_GROUP_PLATFORM_EVENTHANDLER             = ((EVENT_GROUP_PLATFORM << 8) + 0x04) << 16;   ///< Event group for event handler.
const quint32 EVENT_GROUP_PLATFORM_NETWORKCLIENT            = ((EVENT_GROUP_PLATFORM << 8) + 0x05) << 16;   ///< Event group for network client.
const quint32 EVENT_GROUP_PLATFORM_NETWORKSERVER            = ((EVENT_GROUP_PLATFORM << 8) + 0x06) << 16;   ///< Event group for network server.
const quint32 EVENT_GROUP_PLATFORM_DATAMANAGER              = ((EVENT_GROUP_PLATFORM << 8) + 0x07) << 16;   ///< Event group for data manager.
const quint32 EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR   = ((EVENT_GROUP_PLATFORM << 8) + 0x08) << 16;   ///< Event group for device ommand processor.
const quint32 EVENT_GROUP_PLATFORM_STATEMACHINES            = ((EVENT_GROUP_PLATFORM << 8) + 0x09) << 16;   ///< Event group for state machines.
const quint32 EVENT_GROUP_PLATFORM_XML_PARSER               = ((EVENT_GROUP_PLATFORM << 8) + 0x0A) << 16;   ///< Event group for XML parser.
const quint32 EVENT_GROUP_PLATFORM_EXTERNALPROCESSCONTROL   = ((EVENT_GROUP_PLATFORM << 8) + 0x0B) << 16;   ///< Event group for external process control.
const quint32 EVENT_GROUP_PLATFORM_DEVICE_CONTROL_LAYER     = ((EVENT_GROUP_PLATFORM << 8) + 0x0F) << 16;   ///< Event group for device control layer
const quint32 EVENT_GROUP_PLATFORM_EXPORT                   = ((EVENT_GROUP_PLATFORM << 8) + 0x10) << 16;   ///< Event group for Export
const quint32 EVENT_GROUP_PLATFORM_PASSWORDMANAGER          = ((EVENT_GROUP_PLATFORM << 8) + 0x11) << 16;   ///< Event group for Password manager
const quint32 EVENT_GROUP_PLATFORM_DM_USERSETTINGS          = ((EVENT_GROUP_PLATFORM << 8) + 0x12) << 16;   ///< Event group for Usersettings Data container
const quint32 EVENT_GROUP_PLATFORM_GPIO_MANAGER             = ((EVENT_GROUP_PLATFORM << 8) + 0x13) << 16;   ///< Event group for GPIO manager
const quint32 EVENT_GROUP_PLATFORM_GENERIC_EVENTS           = ((EVENT_GROUP_PLATFORM << 8) + 0x14) << 16;   ///< Event group for generic events
const quint32 EVENT_GROUP_PLATFORM_DM_DEVICECONFIG          = ((EVENT_GROUP_PLATFORM << 8) + 0x15) << 16;

//!< Generic events
const quint32 EVENT_SOFTSWITCH_MONITOR_START                = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x001; //!< Raised when basic system init is complete
const quint32 EVENT_SOFTSWITCH_PRESSED_FIRST_TIME           = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x002; //!< Raised when basic system init is complete
const quint32 EVENT_SYSTEM_INIT_COMPLETE                    = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x002; //!< Raised when basic system init is complete
#endif // namespace PLATFORMEVENTCODES_H
