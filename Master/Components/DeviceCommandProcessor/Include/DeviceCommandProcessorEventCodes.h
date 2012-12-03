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

const quint32 EVENT_DEVICECOMMANDPROCESSOR_INFO_COMMAND_STARTED     = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0001; ///< Started command execution: '%1'.
const quint32 EVENT_DEVICECOMMANDPROCESSOR_INFO_COMMAND_FINISHED    = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0002; ///< Finished command execution: '%1'.
const quint32 EVENT_DEVICECOMMANDPROCESSOR_ERROR_COMMAND_FAILED     = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0003; ///< Command execution failed: '%1'.
const quint32 EVENT_DEVICECOMMANDPROCESSOR_ERROR_DEVCTRLLAYER       = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0004; ///< Error at DeviceControlLayer: '%1'.

const quint32 EVENT_DEVICECOMMANDPROCESSOR_STRING_INITIALIZATION    = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0101; ///< Initialization
const quint32 EVENT_DEVICECOMMANDPROCESSOR_STRING_CONFIGURATION     = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0102; ///< Configuration
const quint32 EVENT_DEVICECOMMANDPROCESSOR_STRING_CLEANUP           = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0103; ///< Cleanup
const quint32 EVENT_DEVICECOMMANDPROCESSOR_STRING_GRAPPLER_REFRUN   = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0104; ///< Reference run grappler %1

//// when adding grappler don't forget to adjust DeviceCommandProcessorThreadController::GrapplerNameByInstanceID
//const quint32 EVENT_DEVICECOMMANDPROCESSOR_STRING_GRAPPLER_UNKNOWN  = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0201; ///< unknown
//const quint32 EVENT_DEVICECOMMANDPROCESSOR_STRING_GRAPPLER_1        = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0202; ///< 1 (left)
//const quint32 EVENT_DEVICECOMMANDPROCESSOR_STRING_GRAPPLER_2        = EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR + 0x0203; ///< 2 (right)


} // end namespace DeviceCommandProcessor

#endif // DEVICECOMMANDPROCESSOR_DEVICECOMMANDPROCESSOREVENTCODES_H
