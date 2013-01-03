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
const quint32 EVENT_DM_SETTINGS_XML_READ_FAILED                      = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x000F;    ///< UserSettings XML File Read failed
const quint32 EVENT_DM_DEVICE_CONFIG_XML_READ_FAILED                 = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0010;    ///< Device Config XML file Read failed
const quint32 EVENT_DM_SETTINGS_VERIFICATION_FAILED                  = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0011;    ///< UserSettings Verification Failed
const quint32 EVENT_DM_DEVICE_CONFIG_VERIFICATION_FAILED             = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0012;    ///< DeviceConfiguration Verification Failed
const quint32 EVENT_DM_INITIALIZATION_ERROR                          = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0013;    ///< '%1' Initialization Error
const quint32 EVENT_DM_FILE_OPEN_FAILED                              = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0014;    ///< '%1' file open operation failed
const quint32 EVENT_DM_FILE_WRITE_FAILED                             = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0015;    ///< '%1' file write operation failed
const quint32 EVENT_DM_STREAMIN_FAILED                               = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0016;    ///< '%1' Stream In Operation failed
const quint32 EVENT_DM_STREAMOUT_FAILED                              = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0017;    ///< '%1' Stream Out Operation failed
const quint32 EVENT_DM_XML_FILE_NOT_EXISTS                           = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0018;    ///< '%1' XML file not exists
const quint32 EVENT_DM_XML_SERIALIZE_FAILED                          = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0019;    ///< '%1' XML Serialization failed
const quint32 EVENT_DM_XML_DESERIALIZE_FAILED                        = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x001A;    ///< '%1' XML De Serialization failed
const quint32 EVENT_DM_ERROR_INVALID_AGITAION_SPEED                  = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0x0001; ///< '%1' is not a valid Agitation Speed
const quint32 EVENT_DM_ERROR_INVALID_OVENTEMP                        = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0x0002; ///< '%1' is not a valid Oven temperature
const quint32 EVENT_DM_ERROR_INVALID_WATERTYPE                       = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0x0003; ///< '%1' is not a valid Watertype
const quint32 EVENT_DM_INVALID_LEICA_AGITAION_SPEED                  = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0x0004; ///< '%1' is not a valid Leica Agitation Speed
const quint32 EVENT_DM_INVALID_LEICA_OVENTEMP                        = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0x0005; ///< '%1' is not a valid Leica Oven temperature
const quint32 EVENT_DM_INVALID_LOADER_REAGENTID                      = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0x0006; ///< Invalid Reagent Id in Loader station '%1'
const quint32 EVENT_DM_ERROR_SOUND_NUMBER_OUT_OF_RANGE               = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0x0007; ///< Error Sound Number values are Out Of Range
const quint32 EVENT_DM_ERROR_SOUND_LEVEL_OUT_OF_RANGE                = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0x0008; ///< Error Sound Level values are Out Of Range
const quint32 EVENT_DM_WARN_SOUND_NUMBER_OUT_OF_RANGE                = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0x0009; ///< Warning Sound Number values are Out Of Range
const quint32 EVENT_DM_WARN_SOUND_LEVEL_OUT_OF_RANGE                 = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0x000A; ///< Warning Sound Level values are Out Of Range
} // end namespace DataManager

#endif // DATAMANAGEMENT_DATAMANAGEMENTEVENTCODES_H
