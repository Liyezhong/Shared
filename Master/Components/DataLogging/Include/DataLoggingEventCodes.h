/****************************************************************************/
/*! \file DataLogging/Include/DataLoggingEventCodes.h
 *
 *  \brief All event codes used by data logging.
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

#ifndef DATALOGGING_DATALOGGINGEVENTCODES_H
#define DATALOGGING_DATALOGGINGEVENTCODES_H

#include <../Include/PlatformEventCodes.h>

/****************************************************************************/
/**
 * \brief Namespace containing all classes for logging.
 */
/****************************************************************************/
namespace DataLogging {

const quint32 EVENT_DATALOGGING_ERROR_FILE_NOT_OPEN         = EVENT_GROUP_PLATFORM_DATALOGGING + 0x0001;    ///< Error file not open.
const quint32 EVENT_DATALOGGING_ERROR_FILE_NOT_EXISTS       = EVENT_GROUP_PLATFORM_DATALOGGING + 0x0002;    ///< Error file does not exist: %1.
const quint32 EVENT_DATALOGGING_ERROR_FILE_WRONG_SUFFIX     = EVENT_GROUP_PLATFORM_DATALOGGING + 0x0003;    ///< Error file has wrong suffix: %1. Expected: %2.
const quint32 EVENT_DATALOGGING_ERROR_EVENT_ID_NOT_EXISTS   = EVENT_GROUP_PLATFORM_DATALOGGING + 0x0004;    ///< Event Id %1 not exists in the event string xml file
const quint32 EVENT_DATALOGGING_ERROR_FILE_CREATE           = EVENT_GROUP_PLATFORM_DATALOGGING + 0x0005;    ///< Error file not open.
const quint32 EVENT_DATALOGGING_ERROR_FILE_REMOVE           = EVENT_GROUP_PLATFORM_DATALOGGING + 0x0006;    ///< Error file not open.
const quint32 EVENT_DATALOGGING_ERROR_FILE_WRITE            = EVENT_GROUP_PLATFORM_DATALOGGING + 0x0007;    ///< Error file not open.
const quint32 EVENT_DATALOGGING_ERROR_FILE_FLUSH            = EVENT_GROUP_PLATFORM_DATALOGGING + 0x0008;    ///< Error file not open.

// now some string IDs
const quint32 EVENT_DATALOGGING_INFO_FILE_CREATE            = EVENT_GROUP_PLATFORM_DATALOGGING + 0x1001;    ///< Created file %1.
const quint32 EVENT_DATALOGGING_INFO_FILE_OPEN              = EVENT_GROUP_PLATFORM_DATALOGGING + 0x1002;    ///< Opened file %1.
const quint32 EVENT_DATALOGGING_INFO_FILE_REMOVE            = EVENT_GROUP_PLATFORM_DATALOGGING + 0x1003;    ///< Removed file %1.

} // end namespace DataLogging

#endif // DATALOGGING_DATALOGGINGEVENTCODES_H
