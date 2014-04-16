/****************************************************************************/
/*! \file DeviceCommandProcessorEventCodes.h
 *
 *  \brief All event codes used by device command processor.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-10-07
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

#ifndef DEVICECOMMANDPROCESSOR_DEVICECOMMANDPROCESSOREVENTCODES_H
#define DEVICECOMMANDPROCESSOR_DEVICECOMMANDPROCESSOREVENTCODES_H

#include <../Include/PlatformEventCodes.h>

/****************************************************************************/
/**
 * \brief Namespace containing all classes for device command processor.
 */
/****************************************************************************/
namespace DeviceCommandProcessor {

/// Loader open
const quint32 EVENT_DCP_INFO_LOADER_DEV_OPEN                = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0201;
/// Loader closed
const quint32 EVENT_DCP_INFO_LOADER_DEV_CLOSED              = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0202;
/// Loader blocked
const quint32 EVENT_DCP_INFO_LOADER_DEV_LOCKED              = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0203;
/// Loader unblocked
const quint32 EVENT_DCP_INFO_LOADER_DEV_UNLOCKED            = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0204;

/// Unloader open
const quint32 EVENT_DCP_INFO_UNLOADER_DEV_OPEN              = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0241;
/// Unloader closed
const quint32 EVENT_DCP_INFO_UNLOADER_DEV_CLOSED            = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0242;
/// Unloader blocked
const quint32 EVENT_DCP_INFO_UNLOADER_DEV_LOCKED            = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0243;
/// Unloader unblocked
const quint32 EVENT_DCP_INFO_UNLOADER_DEV_UNLOCKED          = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0244;

/// exhaust fan switched on
const quint32 EVENT_DCP_EXHAUST_FAN_ON                      = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x701;
/// exhaust fan switched off
const quint32 EVENT_DCP_EXHAUST_FAN_OFF                     = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x702;
/// exhaust airflow normal
const quint32 EVENT_DCP_EXHAUST_AIRFLOW_GOOD                = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x703;
/// exhaust airflow too low
const quint32 EVENT_DCP_EXHAUST_AIRFLOW_BAD                 = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x704;

const quint32 EVENT_DCP_INFO_HOOD_DEV_OPEN                  = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0B11; //!< Hood has been opened
const quint32 EVENT_DCP_INFO_HOOD_DEV_CLOSED                = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0B12; //!< Hood has been closed
const quint32 EVENT_DCP_INFO_DRAWER_RFID_READ               = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0B13; //!< Drawer RFID has been read

} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_DEVICECOMMANDPROCESSOREVENTCODES_H
