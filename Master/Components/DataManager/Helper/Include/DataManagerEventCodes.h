/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Helper/Include/DataManagerEventCodes.h
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

const quint32 EVENT_DM_ERROR_READING_XML_STARTELEMENT                = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0001;    ///< Error reading xml start element '%1' in file '%2'.
const quint32 EVENT_DM_ERROR_UNEXPECTED_XML_STARTELEMENT             = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0002;    ///< Unexpected xml start element '%1'. Expected: '%2' in file '%3'
const quint32 EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND                 = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0003;    ///< xml attribute not found: '%1' in file '%2'.
const quint32 EVENT_DM_ERROR_NO_VALID_NUMBER                         = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0004;    ///< '%1' is not a valid number.
const quint32 EVENT_DM_ERROR_NOT_SUPPORTED_LANGUAGE                  = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0005;    ///< '%1' is not a supported language.
const quint32 EVENT_DM_INVALID_RMS_ONOFFSTATE                        = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0006;    ///< '%1' is not a valid on/off state.
const quint32 EVENT_DM_ERROR_UNSUPPORTED_VERSION                     = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0007;    ///< Unsupported version '%1' in file '%2'.
const quint32 EVENT_DM_ERROR_LANG_NOT_FOUND                          = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0008;    ///< Translations for language '%1' not found.
const quint32 EVENT_DM_ERROR_PASSWORD_FORMAT                         = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0009;    ///< Password does not fit requirements.
const quint32 EVENT_DM_ERROR_PASSWORD_SERIAL_NUMBER                  = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x000A;    ///< Serial number does not fit.
const quint32 EVENT_DM_INVALID_DATEFORMAT                            = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x000B;    ///< '%1' is not a valid date format.
const quint32 EVENT_DM_INVALID_TIMEFORMAT                            = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x000C;    ///< '%1' is not a valid time format.
const quint32 EVENT_DM_INVALID_TEMPFORMAT                            = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x000D;    ///< '%1' is not a valid temperature format .
const quint32 EVENT_DM_INVALID_OVENSTARTMODE                         = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x000E;    ///<  '%1' is not a valid oven start mode.
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
const quint32 EVENT_DM_GV_FAILED                                     = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x001B;    ///< Group verification is failed
const quint32 EVENT_DM_NULL_PTR                                      = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x001C;    ///< Event for null pointers
const quint32 EVENT_DM_CHECK_USER_LOG_FOR_MORE_INFO                  = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x001D;    ///< Used when more than 5 warnings/errors occur in containers
const quint32 EVENT_DM_SHUTDOWN_XML_WRITE_FAILED                     = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x001E;    ///< Writing the XML %1 failed during shutdwn.

const quint32 EVENT_DM_STATION_ROWS_COUNT_DOESNOT_MACTH_MAX          = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x020; ///< Station Matrix Number of rows %1 does not match with maximum limit.
const quint32 EVENT_DM_STATION_COL_COUNT_DOESNOT_MACTH_MAX           = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x021; ///< Station Matrix Number of columns %1 does not match with maximum limit.
const quint32 EVENT_DM_STATION_INVALID_ZONE                          = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x022; ///< Station Matrix Station %1 zone is invalid.
const quint32 EVENT_DM_STATION_INVALID_ITEM_TYPE                     = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x023; ///< Station Matrix Station %1 type is invalid.
const quint32 EVENT_DM_STATIONGRID_XML_READ_FAILED                   = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x024; ///< Reading StationGrid.xml failed.
const quint32 EVENT_DM_STATIONGRID_VERIFICATION_FAILED               = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x025; ///< Verifying StationGrid.xml failed.
const quint32 EVENT_DM_RCCONFIG_XML_READ_FAILED                      = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x026;    ///< RC Config XML file Read failed
const quint32 EVENT_DM_RCCONFIG_VERIFICATION_FAILED                  = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x027;    ///< RCConfig Verification Failed
const quint32 EVENT_DM_REAGENT_XML_READ_FAILED                       = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x028;  ///< Reagents XML file read failed
const quint32 EVENT_DM_REAGENT_VERIFICATION_FAILED                   = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x029;  ///< Reagents Verification failed
const quint32 EVENT_DM_STATIONS_XML_READ_FAILED                      = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x02A;  ///< Stations XML read Failed
const quint32 EVENT_DM_STATION_VERIFICATION_FAILED                   = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x02B;  ///< Stations Verification Failed
const quint32 EVENT_DM_STATION_INVALID_STATION_TYPE                  = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x02C; ///< Station Matrix Station %1 type is invalid.
const quint32 EVENT_DM_MODULE_CONF_READ_FAILED                       = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x02D; ///< Instrument history XML read failed.


const quint32 EVENT_DM_PROCESS_SETTINGS_CONF_READ_FAILED             = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0030; ///< Reading ProcessSettings.xml failed.
const quint32 EVENT_DM_PROCESS_SETTINGS_VERIFICATION_FAILED          = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0031; ///< Verifying ProcessSettings.xml failed.

const quint32 EVENT_DM_RACK_INSERTED                   = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0032; ///< Rack %1 has been inserted into drawer %2
const quint32 EVENT_DM_RACK_REMOVED                    = EVENT_GROUP_PLATFORM_DATAMANAGER + 0x0033; ///< Rack %1 has been removed from instrument

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
const quint32 EVENT_DM_ERROR_INVALID_REMOTECARE_ONOFFSTATE           = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0X000B; ///< '%1' is not a valid RemoteCare on/off state.
const quint32 EVENT_DM_ERROR_INVALID_DIRECT_CONNECTION_ONOFFSTATE    = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0X000C; ///< '%1' is not a valid DirectConnection on/off state.
const quint32 EVENT_DM_INVALID_PROXY_USERNAME_CHAR_COUNT             = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0X000D; ///< '%1' is not a valid Proxy UserName.
const quint32 EVENT_DM_INVALID_PROXY_PASSWORD_CHAR_COUNT             = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0X000E; ///< '%1' is not a valid Proxy Password.
const quint32 EVENT_DM_ERROR_INVALID_PROXY_IP_ADDRESS                = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0X000F; ///< '%1' is not a valid Proxy IP Address.
const quint32 EVENT_DM_ERROR_INVALID_PROXY_IP_PORT                   = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0X0010; ///< '%1' is not a valid Proxy IP Port.
const quint32 EVENT_DM_CANCEL_PLAY_ALARM_TEST_TONE                   = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0X0011; ///< Cancel the Test Alarm Tone.
const quint32 EVENT_DM_SETTINGS_UPDATE_FAILED                        = EVENT_GROUP_PLATFORM_DM_USERSETTINGS + 0X0012; ///< Generic failure message

const quint32 EVENT_DM_INVALID_DEVICE_NAME_CHAR_COUNT                = EVENT_GROUP_PLATFORM_DM_DEVICECONFIG + 0x0001; ///< Device name character count is Invalid.
const quint32 EVENT_DM_INVALID_DEVICE_NUMBER_CHAR_COUNT              = EVENT_GROUP_PLATFORM_DM_DEVICECONFIG + 0x0002; ///< Device Number character count is Invalid.
const quint32 EVENT_DM_INVALID_COVERSLIPPER_NAME_WIDTH               = EVENT_GROUP_PLATFORM_DM_DEVICECONFIG + 0x0003; ///< Cover Slipper device name character count is Invalid.
const quint32 EVENT_DM_INVALID_WSMODE_DATA                           = EVENT_GROUP_PLATFORM_DM_DEVICECONFIG + 0x0004; ///< Work Station mode data is Invalid.
const quint32 EVENT_DM_INVALID_HEATED_CUEVETTES_DATA                 = EVENT_GROUP_PLATFORM_DM_DEVICECONFIG + 0x0005; ///< Heated Cuevettes mode data is Invalid.
const quint32 EVENT_DM_INVALID_CAMERA_SLIDEID_DATA                   = EVENT_GROUP_PLATFORM_DM_DEVICECONFIG + 0x0006; ///< Camera Slide Id data is Invalid.
const quint32 EVENT_DM_INVALID_STAINER_DEV_NAME_WIDTH                = EVENT_GROUP_PLATFORM_DM_DEVICECONFIG + 0x0007; ///< Stainer Device name character count is Invalid.
const quint32 EVENT_DM_ERROR_INVALID_SETTINGS_XML_DATA_FOUND         = EVENT_GROUP_PLATFORM_DM_DEVICECONFIG + 0x0008; ///< Invalid Settings XML data found

const quint32 EVENT_SWVERSION_RELEASE_DATE_VERION_EMPTY             = EVENT_GROUP_PLATFORM_DM_SWVERSION + 0x0001; ///< The release date/version is empty in SW_Version.xml.
const quint32 EVENT_SWVERSION_SW_DATE_VERION_EMPTY                  = EVENT_GROUP_PLATFORM_DM_SWVERSION + 0x0002; ///< The release date/version of component %1 is empty in SW_Version.xml.
const quint32 EVENT_SWVERSION_SW_NAME_EMPTY                         = EVENT_GROUP_PLATFORM_DM_SWVERSION + 0x0003; ///< The SW component name is empty in SW_Version.xml.
const quint32 EVENT_SWVERSION_SW_COMPONENT_NOT_UNIQUE               = EVENT_GROUP_PLATFORM_DM_SWVERSION + 0x0004; ///< The SW Component %1 already exists in the SW_Version.xml.
const quint32 EVENT_SWVERSION_SW_COMPONENT_DOESNOT_EXISTS           = EVENT_GROUP_PLATFORM_DM_SWVERSION + 0x0005; ///< The SW Component %1 already exists in SW_Version.xml.


const quint32 EVENT_EXPORTCONFIG_EMPTY_FILE_LIST                 = EVENT_GROUP_PLATFORM_DM_EXPORT_CONFIG + 0x0001; ///< ExportConfig:Empty file name found in file list of %1 configuration.
const quint32 EVENT_EXPORTCONFIG_EMPTY_GROUP_FILE                = EVENT_GROUP_PLATFORM_DM_EXPORT_CONFIG + 0x0002; ///< ExportConfig:Empty file name found in group file list of %1 configuration.
const quint32 EVENT_EXPORTCONFIG_INVALID_PACKAGETYPE             = EVENT_GROUP_PLATFORM_DM_EXPORT_CONFIG + 0x0003; ///< ExportConfig:The package type of %1 configuration is invalid, it should be native or nonnative.
const quint32 EVENT_EXPORTCONFIG_EMPTY_SOURCEDIR                 = EVENT_GROUP_PLATFORM_DM_EXPORT_CONFIG + 0x0004; ///< ExportConfig:Empty Source directory name.
const quint32 EVENT_EXPORTCONFIG_EMPTY_TARGETFILE                = EVENT_GROUP_PLATFORM_DM_EXPORT_CONFIG + 0x0005; ///< ExportConfig:Empty target file name.


const quint32 EVENT_DM_RC_QUEUE_SIZE_OUTOF_RANGE                = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0001; ///< RCCOnfig: Queue size %1 is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_INVALID_HTTPCON_PERSISTENCE_ONOFFSTATE = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0002; ///< RCCOnfig: '%1' is not a valid HTTP connection persistence on/off state.
const quint32 EVENT_DM_RC_INVALID_DEBUG_ONOFFSTATE              = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0003; ///< RCCOnfig: '%1' is not a valid Debug on/off state.
const quint32 EVENT_DM_RC_INVALID_HTTPSECURE_CON_ONOFFSTATE     = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0004; ///< RCCOnfig: '%1' is not a valid HTTP secure connection on/off state.
const quint32 EVENT_DM_RC_ENCRYPTION_OUTOF_RANGE                = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0005; ///< RCCOnfig: Encryption level size %1 is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_INVALID_AUTHENTICATION_ONOFFSTATE     = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0006; ///< RCCOnfig: '%1' is not a valid Authentication on/off state.
const quint32 EVENT_DM_RC_CERT_FILENAME_LENGHT_OUTOF_RANGE      = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0007; ///< RCCOnfig: Certificate FileName %1 lenght is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_EXEC_TIME_OUTOF_RANGE                 = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0008; ///< RCCOnfig: Exec Time is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_SERVERTYPE_OUTOF_RANGE                = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0009; ///< RCCOnfig: Server type %1 is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_TARGET_DATABASE_LENGHT_OUTOF_RANGE    = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x000A; ///< RCCOnfig: Target Database %1 lenght is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_SERVER_URL_LENGHT_OUTOF_RANGE         = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x000B; ///< RCCOnfig: Server URL %1 lenght is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_SERVER_ID_OUTOF_RANGE                 = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x000C; ///< RCCOnfig: Server Id %1 is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_SERIAL_NUMBER_LENGHT_OUTOF_RANGE      = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x000D; ///< RCCOnfig: Serial Number %1 lenght is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_MODEL_NUMBER_LENGHT_OUTOF_RANGE       = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x000E; ///< RCCOnfig: Model Number %1 lenght is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_DEVICE_ID_OUTOF_RANGE                 = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x000F; ///< RCCOnfig: Device Id %1 is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_DEVICETYPE_OUTOF_RANGE                = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0010; ///< RCCOnfig: Device type %1 is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_INVALID_USEPROXY_ONOFFSTATE           = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0011; ///< RCCOnfig: '%1' is not a valid UseProxy on/off state.
const quint32 EVENT_DM_RC_PROXY_PROTOCOL_OUTOF_RANGE            = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0012; ///< RCCOnfig: Proxy protocol %1 is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_INVALID_PROXY_PROTOCOL                = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0013; ///< RCCOnfig: '%1' is not a valid proxy protocol.
const quint32 EVENT_DM_RC_PROXY_USERNAME_LENGHT_OUTOF_RANGE     = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0014; ///< RCCOnfig: Proxy user name %1 lenght is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_PROXY_PASSWORD_LENGHT_OUTOF_RANGE     = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0015; ///< RCCOnfig: Proxy password %1 lenght is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_INVALID_PROXY_IP_ADDRESS              = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0016; ///< RCCOnfig: '%1' is not a valid proxy IP address.
const quint32 EVENT_DM_RC_PROXY_PORT_OUTOF_RANGE                = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0017; ///< RCCOnfig: Proxy port is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_INVALID_ALLOWREMOTESESSION_ONOFFSTATE = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0018; ///< RCCOnfig: '%1' is not a valid Allow remote session on/off state.
const quint32 EVENT_DM_RC_INVALID_REMOTESESSIONSETUP_ONOFFSTATE = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0019; ///< RCCOnfig: '%1' is not a valid remote session setup on/off state.
const quint32 EVENT_DM_RC_REMOTESESSION_NAME_LENGHT_OUTOF_RANGE = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x001A; ///< RCCOnfig: remote session name %1 lenght is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_REMOTESESSION_TYPE_LENGHT_OUTOF_RANGE = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x001B; ///< RCCOnfig: remote session type %1 lenght is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_INVALID_REMOTESESSION_IP_ADDRESS      = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x001C; ///< RCCOnfig: '%1' is not a valid remote session IP address.
const quint32 EVENT_DM_RC_REMOTESESSION_PORT_OUTOF_RANGE        = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x001D; ///< RCCOnfig: remote session port is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_MAXCHUNKSIZE_OUTOF_RANGE              = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x001E; ///< RCCOnfig: MaxChunkSize %1 is out of range, min %2, max %3.
const quint32 EVENT_DM_RC_INVALID_COMPRESSION_ONOFFSTATE        = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x001F; ///< RCCOnfig: '%1' is not a valid Compression on/off state.
const quint32 EVENT_DM_RC_DATAITEM_TYPE_INVALID                 = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0020; ///< RCCOnfig: '%1' dataitem has invlaid type.
const quint32 EVENT_DM_RC_DATAITEM_QUALITY_INVALID              = EVENT_GROUP_PLATFORM_DM_RC_CONFIG + 0x0021; ///< RCCOnfig: '%1' dataitem has invlaid quality.

const quint32 EVENT_DM_PARAMSETS_COUNT_EXCEEDS_LIMIT            = EVENT_GROUP_PLATFORM_DM_PARAM_SETS + 0x0001; ///< Too many ParameterSets (max 10 allowed).
const quint32 EVENT_DM_PARAMSETS_INVALID_RESULT                 = EVENT_GROUP_PLATFORM_DM_PARAM_SETS + 0x0002; ///< ParamSets Result Can't have more than 5 Error code, Result can be either success or failure, not both:
const quint32 EVENT_DM_PARAMSETS_INVALID_RESULT_CODE            = EVENT_GROUP_PLATFORM_DM_PARAM_SETS + 0x0003; ///< ParamSets : '%1' : Invalid result code
const quint32 EVENT_DM_INVALID_PARAMSETS_ID                     = EVENT_GROUP_PLATFORM_DM_PARAM_SETS + 0x0004; ///<  '%1' is not a valid ParamSets ID.
const quint32 EVENT_DM_UKNOWN_PARAM_COLOUR                      = EVENT_GROUP_PLATFORM_DM_PARAM_SETS + 0x0005; ///<  ParamSets: '%1' is not a valid Color.
const quint32 EVENT_DM_UKNOWN_PARAM_MOUNTANT_NAME               = EVENT_GROUP_PLATFORM_DM_PARAM_SETS + 0x0006; ///<  ParamSets: '%1' is not a valid MountantName.
const quint32 EVENT_DM_UKNOWN_PARAM_MOUNTANT_VOLUME             = EVENT_GROUP_PLATFORM_DM_PARAM_SETS + 0x0007; ///<  ParamSets: '%1' is not a valid MountantVolume.
const quint32 EVENT_DM_INVALID_PARAM_STARTCAPABLE_RESULT        = EVENT_GROUP_PLATFORM_DM_PARAM_SETS + 0x0008; ///<  ParamSets: '%1' is not a valid StartCapable Result.
const quint32 EVENT_DM_UKNOWN_PARAM_COVERGLASS_LENGTH           = EVENT_GROUP_PLATFORM_DM_PARAM_SETS + 0x0009; ///<  ParamSets: '%1' is not a valid CoverglassLength.

const quint32 EVENT_DM_STATION_ID_NOT_UNIQUE                    = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x0001; ///< Station ID %1 already in use. Please choose different ID.
const quint32 EVENT_DM_STATION_ID_NOT_FOUND                     = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x0002; ///< Station ID %1 does not exists in the list.
const quint32 EVENT_DM_STATION_ID_SIZE_EXCEEDS_LIMIT            = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x0003; ///< Station ID %1 size is more than 3 characters.
const quint32 EVENT_DM_STATION_ID_INVALID_TYPE                  = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x0004; ///< Station %1 is not dry/heated/load/oven/park/reagent/slide/transfer/unload/water station.
const quint32 EVENT_DM_STATION_INVALID_ID                       = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x0005; ///< Station ID %1 doesn't have numbers.
const quint32 EVENT_DM_STATION_INVALID_EXPIRYDATE               = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x0006; ///< Station %1 Expiry Date %2 not valid.
const quint32 EVENT_DM_STATION_INVALID_BESTBEFOREDATE           = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x0007; ///< Station %1 Best Before Date %2 not valid.
const quint32 EVENT_DM_STATION_BATHLAYOUT_CHANGE_FAILED         = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x0008; ///< Changing the bathlayout failed.
const quint32 EVENT_DM_STATION_ID_MISSING                       = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x0009; ///< Station ID %1 attribute is missing.
//const quint32 EVENT_DM_STATION_INVALID_DATA                     = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x000A; ///< Station  %1 attribute is Invalid.
const quint32 EVENT_DM_STATION_INVALID_FIRST_USAGEDATE          = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x000B; ///< Station %1 Usage Date %2 not valid.
const quint32 EVENT_DM_STATION_UKNOWN_COLORADO_XML_DATA_FOUND   = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x000C; ///< Station  %1 XML data Invalid.
const quint32 EVENT_DM_STATION_RESET_STATION                    = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x000D; ///< User reset the Station %1.
const quint32 EVENT_DM_STATION_CHANGE_STATION                   = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x000e; ///< User change the Station %1 to reagent %2.
const quint32 EVENT_DM_STATION_SET_STATION_FULL                 = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x000f; ///< User set the Station %1 to full.
const quint32 EVENT_DM_STATION_SET_STATION_EMPTY                = EVENT_GROUP_PLATFORM_DM_STATIONS + 0x0010; ///< User set the Station %1 to empty.

const quint32 EVENT_DM_REAGENT_NAME_NOT_UNIQUE                   =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0001; ///< Reagent name '%1' already in use. Please choose different reagent name.
const quint32 EVENT_DM_REAGENT_SHORT_NAME_NOT_UNIQUE             =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0002; ///< Reagent Short name '%1' already in use. Please choose different Reagent Short name.
const quint32 EVENT_DM_REAGENT_ID_NOT_UNIQUE                     =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0003; ///< Reagent ID '%1' already in use. Please choose different Reagent ID.
const quint32 EVENT_DM_REAGENT_ID_DOESTNOT_EXISTS                =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0004; ///< Reagent ID '%1' does not exists in the list.
const quint32 EVENT_DM_REAGENT_EXCEEDS_LIMIT                     =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0005; ///< Too many reagents (max 100 allowed):
const quint32 EVENT_DM_REAGENT_INVALID_PREFIX                    =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0006; ///< Reagent '%1' Reagent ID doesn't start with S,L,U:
const quint32 EVENT_DM_REAGENT_SHORTNAME_LENGTH_CHECK_FAILED     =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0007; ///< Reagent '%1' Short Name is too long (max 10 Characters)
const quint32 EVENT_DM_REAGENT_LONGNAME_LENGTH_CHECK_FAILED      =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0008; ///< Reagent '%1' Long Name is too long (max 32 Characters)
const quint32 EVENT_DM_REAGENT_MAXSLIDES_EXCEEDS_LIMIT           =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0009; ///< Reagent '%1' The value for Slides max must be equal or less than 3000.
const quint32 EVENT_DM_REAGENT_USE_DAYS_EXCEEDS_LIMIT            =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x000A; ///< Reagent '%1' The value for Days max must be equal or less than 99.
const quint32 EVENT_DM_REAGENT_TEMP_NOTIN_LIMIT                  =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x000B; ///< Reagent '%1' Temperature not in limit (between 37 and 75)
const quint32 EVENT_DM_REAGENT_HEATING_MODE_UNDEF                =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x000C; ///< Reagent '%1' Heating Start mode Undefined
const quint32 EVENT_DM_REAGENT_UPDATE_FAILED_INTERNAL_ERR        =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x000D; ///< ReagentList writing failed '%1' reagents
const quint32 EVENT_DM_REAGENT_ADD_FAILED_INTERNAL_ERR           =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x000E; ///< ReagentList reading failed '%1' reagents
const quint32 EVENT_DM_REAGENT_OPERATION_FAILED                  =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x000F; ///< Reagent : '%1' operation failed
const quint32 EVENT_DM_REAGENT_DEL_FAILED_INTERNAL_ERR           =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0010; ///< Reagent deletion failed.
const quint32 EVENT_DM_REAGENT_INVALID_COLORADO_XML_DATA_FOUND   =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0011; ///< Invalid Colorado Xml data with Reagent '%1' found.
const quint32 EVENT_DM_REAGENT_ADD_REAGENT   =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0012; ///< User add reagent %1.
const quint32 EVENT_DM_REAGENT_DEL_REAGENT   =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0013; ///< User delete reagent %1.
const quint32 EVENT_DM_REAGENT_UPDATE_REAGENT   =   EVENT_GROUP_PLATFORM_DM_REAGENTS + 0x0014; ///< User update reagent %1.


}

#endif // DATAMANAGEMENT_DATAMANAGEMENTEVENTCODES_H
