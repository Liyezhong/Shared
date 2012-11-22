/****************************************************************************/
/*! \file DataManagementEventCodes.h
 *
 *  \brief All event codes used by data management.
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

#ifndef DATAMANAGEMENT_DATAMANAGEMENTEVENTCODES_H
#define DATAMANAGEMENT_DATAMANAGEMENTEVENTCODES_H

#include <../Include/PlatformEventCodes.h>

/****************************************************************************/
/**
 * \brief Namespace for data management stuff.
 */
/****************************************************************************/
namespace DataManager {

const quint32 EVENT_DATAMANAGER_ERROR_READING_XML_STARTELEMENT       = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0001;    ///< Error reading xml start element '%1'.
const quint32 EVENT_DATAMANAGER_ERROR_UNEXPECTED_XML_STARTELEMENT    = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0002;    ///< Unexpected xml start element '%1'. Expected: '%2'
const quint32 EVENT_DATAMANAGER_ERROR_XML_ATTRIBUTE_NOT_FOUND        = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0003;    ///< xml attribute not found: '%1'.
const quint32 EVENT_DATAMANAGER_ERROR_NO_VALID_NUMBER                = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0004;    ///< '%1' is not a valid number.
const quint32 EVENT_DATAMANAGER_ERROR_NOT_SUPPORTED_LANGUAGE         = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0005;    ///< '%1' is not a supported language.
const quint32 EVENT_DATAMANAGER_ERROR_NO_VALID_ONOFFSTATE            = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0006;    ///< '%1' is not a valid on/off state.
const quint32 EVENT_DATAMANAGER_ERROR_UNSUPPORTED_VERSION            = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0007;    ///< Unsupported version '%1'.
const quint32 EVENT_DATAMANAGER_ERROR_LANG_NOT_FOUND                 = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0008;    ///< Translations for language '%1' not found.
const quint32 EVENT_DATAMANAGER_ERROR_PASSWORD_FORMAT                = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0009;    ///< Password does not fit requirements.
const quint32 EVENT_DATAMANAGER_ERROR_PASSWORD_SERIAL_NUMBER         = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x000A;    ///< Serial number does not fit.
const quint32 EVENT_DATAMANAGER_ERROR_NO_VALID_DATEFORMAT            = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x000B;    ///< '%1' is not a valid date format.
const quint32 EVENT_DATAMANAGER_ERROR_NO_VALID_TIMEFORMAT            = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x000C;    ///< '%1' is not a valid time format.
const quint32 EVENT_DATAMANAGER_ERROR_NO_VALID_TEMPFORMAT            = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x000D;    ///< '%1' is not a valid temperature format .
const quint32 EVENT_DATAMANAGER_ERROR_NO_VALID_OVENSTARTMODE         = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x000E;    ///<  '%1' is not a valid oven start mode.

} // end namespace DataManager

#endif // DATAMANAGEMENT_DATAMANAGEMENTEVENTCODES_H
