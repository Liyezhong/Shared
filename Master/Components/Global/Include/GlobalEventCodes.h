/****************************************************************************/
/*! \file Global/Include/GlobalEventCodes.h
 *
 *  \brief All event codes used by all.
 *
 *  Version:    0.1
 *  Date:       2010-07-12
 *  Author:     J.Bugariu
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

#ifndef GLOBAL_GLOBALEVENTCODES_H
#define GLOBAL_GLOBALEVENTCODES_H

#include <../Include/PlatformEventCodes.h>

namespace Global {

const quint32 EVENT_GLOBAL_UNKNOWN_STRING_ID                = EVENT_GROUP_PLATFORM_GLOBAL + 0x0001; ///< Unknown string id: %1.
const quint32 EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION          = EVENT_GROUP_PLATFORM_GLOBAL + 0x0002; ///< Unknown exception caught: file: %1 line: %2.
const quint32 EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION          = EVENT_GROUP_PLATFORM_GLOBAL + 0x0003; ///< Memory allocation error: file: %1 line: %2.
const quint32 EVENT_GLOBAL_ERROR_NULL_POINTER               = EVENT_GROUP_PLATFORM_GLOBAL + 0x0004; ///< Pointer is NULL: %1 file: %2 line: %3.
const quint32 EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED       = EVENT_GROUP_PLATFORM_GLOBAL + 0x0006; ///< Signal not connected: %1.
const quint32 EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT        = EVENT_GROUP_PLATFORM_GLOBAL + 0x0007; ///< Error connecting signal %1::%2 to slot %3::%4 in file: %5 line: %6.
const quint32 EVENT_GLOBAL_ERROR_SIGNAL_SIGNAL_CONNECT      = EVENT_GROUP_PLATFORM_GLOBAL + 0x0008; ///< Error connecting signal %1::%2 to signal %3::%4 in file: %5 line: %6.
const quint32 EVENT_GLOBAL_ERROR_MONOTONIC_CLOCK            = EVENT_GROUP_PLATFORM_GLOBAL + 0x000B; ///< Error reading monotonic clock.
const quint32 EVENT_GLOBAL_ERROR_FILE_CREATE                = EVENT_GROUP_PLATFORM_GLOBAL + 0x000D; ///< Error creating file %1.
const quint32 EVENT_GLOBAL_ERROR_FILE_OPEN                  = EVENT_GROUP_PLATFORM_GLOBAL + 0x000E; ///< Error opening file %1.
const quint32 EVENT_GLOBAL_ERROR_COMPUTING_NEW_REF          = EVENT_GROUP_PLATFORM_GLOBAL + 0x0016; ///< Unable to compute new command ref.
const quint32 EVENT_GLOBAL_ERROR_SIGNAL_RECEIVED            = EVENT_GROUP_PLATFORM_GLOBAL + 0x0017; ///< We have an unexpected signal (possibly crash)
const quint32 EVENT_GLOBAL_UNCAUGHT_EXCEPTION               = EVENT_GROUP_PLATFORM_GLOBAL + 0x0018; ///< We have catched an unexpected exception
const quint32 EVENT_GLOBAL_EXCEPTION                        = EVENT_GROUP_PLATFORM_GLOBAL + 0x0019; ///< We have catched an application exception
const quint32 EVENT_GLOBAL_STD_EXCEPTION                    = EVENT_GROUP_PLATFORM_GLOBAL + 0x001A; ///< We have catched an std exception


// log user activities of the data manager starts from 0x0200
/// don't start the number for events(EVENT_GROUP_PLATFORM_GLOBAL) from 0x0200
const quint32 EVENT_GLOBAL_USER_ACTIVITY_PROGRAM_ADDED                  = EVENT_GROUP_PLATFORM_GLOBAL + 0x0200; ///< Program %1 added
const quint32 EVENT_GLOBAL_USER_ACTIVITY_PROGRAM_DELETED                = EVENT_GROUP_PLATFORM_GLOBAL + 0x0201; ///< Program %1 deleted
const quint32 EVENT_GLOBAL_USER_ACTIVITY_PROGRAM_UPDATE                 = EVENT_GROUP_PLATFORM_GLOBAL + 0x0202; ///< Program %1 updated
const quint32 EVENT_GLOBAL_USER_ACTIVITY_PROGRAM_MODIFY_LEICA           = EVENT_GROUP_PLATFORM_GLOBAL + 0x0203; ///< Leica Program %1 updated
const quint32 EVENT_GLOBAL_USER_ACTIVITY_PROGRAM_COLOR                  = EVENT_GROUP_PLATFORM_GLOBAL + 0x0204; ///< Color for program %1 changed
const quint32 EVENT_GLOBAL_USER_ACTIVITY_REAGENT_ADDED                  = EVENT_GROUP_PLATFORM_GLOBAL + 0x0205; ///< Reagent %1 added
const quint32 EVENT_GLOBAL_USER_ACTIVITY_REAGENT_DELETED                = EVENT_GROUP_PLATFORM_GLOBAL + 0x0206; ///< Reagent %1 deleted
const quint32 EVENT_GLOBAL_USER_ACTIVITY_REAGENT_UPDATED                = EVENT_GROUP_PLATFORM_GLOBAL + 0x0207; ///< Reagent %1 updated
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_LANGUAGE_CHANGED            = EVENT_GROUP_PLATFORM_GLOBAL + 0x0208; ///< Language set to %1
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_DATE_TIME_CHANGED           = EVENT_GROUP_PLATFORM_GLOBAL + 0x0209; ///< Date/ Time changed: %1
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_OVEN_TEMP_CHANGED           = EVENT_GROUP_PLATFORM_GLOBAL + 0x020A; ///< Oventemperature changed to %1
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_NETWORK_SETTINGS_ADDED      = EVENT_GROUP_PLATFORM_GLOBAL + 0x020B; ///< Network Settings changed
const quint32 EVENT_GLOBAL_USER_ACTIVITY_WHITE_RACK_PROGRAM_ASSOCIATED  = EVENT_GROUP_PLATFORM_GLOBAL + 0x020C; ///< White rack assigned to program %1
const quint32 EVENT_GLOBAL_USER_ACTIVITY_ADMIN_LOGIN                    = EVENT_GROUP_PLATFORM_GLOBAL + 0x020D; ///< Administrator Login
const quint32 EVENT_GLOBAL_USER_ACTIVITY_OPERATOR_LOGIN                 = EVENT_GROUP_PLATFORM_GLOBAL + 0x020E; ///< Operator Login
const quint32 EVENT_GLOBAL_RMS_ACTIVITY_STATION_DC_UPDATED              = EVENT_GROUP_PLATFORM_GLOBAL + 0x0210; ///< Station DC updated by RMS
const quint32 EVENT_GLOBAL_CMS_ACTIVITY_STATION_DC_UPDATED              = EVENT_GROUP_PLATFORM_GLOBAL + 0x0211; ///< Station DC updated by CMS
const quint32 EVENT_GLOBAL_USER_ACTIVITY_BATHLAYOUT_CHANGE_FAILED       = EVENT_GROUP_PLATFORM_GLOBAL + 0x0212; ///< Bathlayout changed by user
const quint32 EVENT_GLOBAL_USER_ACTIVITY_BATHLAYOUT_CHANGED             = EVENT_GROUP_PLATFORM_GLOBAL + 0x0213; ///< Bathlayout change by user failed

const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_WATER_TYPE_CHANGED          = EVENT_GROUP_PLATFORM_GLOBAL + 0x0214; ///< Water type is changed to %1
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_OVEN_START_MODE_CHANGED     = EVENT_GROUP_PLATFORM_GLOBAL + 0x0215; ///< Oven start mode changed to %1
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_TEMPERATURE_FORMAT_CHANGED  = EVENT_GROUP_PLATFORM_GLOBAL + 0x0216; ///< Temperature format changed to %1
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_TIME_FORMAT_CHANGED         = EVENT_GROUP_PLATFORM_GLOBAL + 0x0217; ///< Time format changed to %1
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_DATE_FORMAT_CHANGED         = EVENT_GROUP_PLATFORM_GLOBAL + 0x0218; ///< Date format changed to %1
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_AGITATION_SPEED_CHANGED     = EVENT_GROUP_PLATFORM_GLOBAL + 0x0219; ///< Agitation speed changed to %1
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_LOADER_REAGENT_CHANGED      = EVENT_GROUP_PLATFORM_GLOBAL + 0x021A; ///< Loader reagent %1 changed to %2
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_ALARAM_WARN_SOUND_NOTE_CHANGED    = EVENT_GROUP_PLATFORM_GLOBAL + 0x021B; ///< Alaram warning sound note changed
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_ALARAM_ERROR_SOUND_NOTE_CHANGED   = EVENT_GROUP_PLATFORM_GLOBAL + 0x021C; ///< Alaram error sound note changed
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_CHANGED                 = EVENT_GROUP_PLATFORM_GLOBAL + 0x021D; ///< RMS state changed to %1
/// these are dummy events which required for translations
const quint32 EVENT_GLOBAL_USER_ACTIVITY_STATE_CHANGED_ON              = EVENT_GROUP_PLATFORM_GLOBAL + 0x021E; ///< ON
const quint32 EVENT_GLOBAL_USER_ACTIVITY_STATE_CHANGED_OFF             = EVENT_GROUP_PLATFORM_GLOBAL + 0x021F; ///< OFF
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_WATER_TYPE_CHANGED_TAP            = EVENT_GROUP_PLATFORM_GLOBAL + 0x0220; ///< Tap
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_WATER_TYPE_CHANGED_DISTILLED      = EVENT_GROUP_PLATFORM_GLOBAL + 0x0221; ///< Distiled
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_OVEN_START_MODE_CHANGED_PERMANENT = EVENT_GROUP_PLATFORM_GLOBAL + 0x0222; ///< Permannet
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_OVEN_START_MODE_CHANGED_PROGSTART = EVENT_GROUP_PLATFORM_GLOBAL + 0x0223; ///< Program Start

const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_ALARAM_WARN_VOLUME_NOTE_CHANGED    = EVENT_GROUP_PLATFORM_GLOBAL + 0x0224; ///< Alaram warning volume note changed
const quint32 EVENT_GLOBAL_USER_ACTIVITY_US_ALARAM_ERROR_VOLUME_NOTE_CHANGED   = EVENT_GROUP_PLATFORM_GLOBAL + 0x0225; ///< Alaram error volume note changed

const quint32 EVENT_GLOBAL_USER_ACTIVITY_REAGENT_HC_MODE                = EVENT_GROUP_PLATFORM_GLOBAL + 0x0226; ///< Heated cuvette start mode is changed to %1 for the Reagent %2
const quint32 EVENT_GLOBAL_USER_ACTIVITY_REAGENT_HC_TEMPERATURE         = EVENT_GROUP_PLATFORM_GLOBAL + 0x0227; ///< Heated cuvette temperature is changed to %1 for the Reagent %2
const quint32 EVENT_GLOBAL_USER_ACTIVITY_REAGENT_HC                     = EVENT_GROUP_PLATFORM_GLOBAL + 0x0228; ///< Heated cuvette is %1 for the Reagent %2 and temperature is %3


// AlarmHandler
const quint32 EVENT_ALARM_SOUND_MISSING                     = EVENT_GROUP_PLATFORM_GLOBAL + 0x0300; ///< alarm sound missing

// now some string IDs
const quint32 EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE          = EVENT_GROUP_PLATFORM_GLOBAL + 0x1000; ///< Debug message: '%1'
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_UNDEFINED      = EVENT_GROUP_PLATFORM_GLOBAL + 0x1001; ///< Undefined Event type
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_FATAL_ERROR    = EVENT_GROUP_PLATFORM_GLOBAL + 0x1002; ///< Fatal error
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_ERROR          = EVENT_GROUP_PLATFORM_GLOBAL + 0x1003; ///< Error
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_WARNING        = EVENT_GROUP_PLATFORM_GLOBAL + 0x1004; ///< Warning
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_INFO           = EVENT_GROUP_PLATFORM_GLOBAL + 0x1005; ///< Info
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_DEBUG          = EVENT_GROUP_PLATFORM_GLOBAL + 0x1006; ///< Debug
const quint32 EVENT_GLOBAL_STRING_TERMINATING               = EVENT_GROUP_PLATFORM_GLOBAL + 0x1009; ///< Terminating software.
const quint32 EVENT_GLOBAL_CURRENT_TIME_OFFSET              = EVENT_GROUP_PLATFORM_GLOBAL + 0x100B; ///< Current time offset is '%1' seconds.
const quint32 EVENT_GLOBAL_STRING_ID_RESOLVED               = EVENT_GROUP_PLATFORM_GLOBAL + 0x100D; ///< Resolved:
const quint32 EVENT_GLOBAL_STRING_ID_ACKNOWLEDGED           = EVENT_GROUP_PLATFORM_GLOBAL + 0x100E; ///< Acknowledged by user:

const quint32 EVENT_SHUTDOWN_REQUEST_TOUSER                 = EVENT_GROUP_PLATFORM_GLOBAL + 0x100F; ///< ask user to shutdown
const quint32 EVENT_SHUTDOWN_ON_SYSTEM_ERROR                = EVENT_GROUP_PLATFORM_GLOBAL + 0x1010; ///< shutdown the system


// string IDs for Export
// Event codes for the Export process
const quint32 EVENT_EXPORT_DIRECTORY_CREATION_FAILED                        = EVENT_GROUP_PLATFORM_EXPORT + 0x0001; ///< Directory creation is failed
const quint32 EVENT_EXPORT_FILES_NOT_COPIED                                 = EVENT_GROUP_PLATFORM_EXPORT + 0x0002; ///< Files are copied
const quint32 EVENT_EXPORT_UNABLE_TO_CREATE_FILE_RMS_STATUS                 = EVENT_GROUP_PLATFORM_EXPORT + 0x0003; ///< Unable to create the RMS_Status.csv file
const quint32 EVENT_EXPORT_UNABLE_TO_CREATE_FILE_TEMP_EXPORTCONFIGURATION   = EVENT_GROUP_PLATFORM_EXPORT + 0x0004; ///< Unable to create the temporary export configuration xml file
const quint32 EVENT_EXPORT_UNABLE_TO_READ_FILE_TEMP_EXPORTCONFIGURATION     = EVENT_GROUP_PLATFORM_EXPORT + 0x0005; ///< unable to read the configuration file
const quint32 EVENT_EXPORT_INIT_CONTAINER_FAILED                            = EVENT_GROUP_PLATFORM_EXPORT + 0x0006; ///< initialization of the container failed
const quint32 EVENT_EXPORT_VERIFICATION_CONTAINER_FAILED                    = EVENT_GROUP_PLATFORM_EXPORT + 0x0007; ///< verification of the container failed
const quint32 EVENT_EXPORT_UNABLE_TO_ARCHIVE_FILES                          = EVENT_GROUP_PLATFORM_EXPORT + 0x0008; ///< unable to write files on the device
const quint32 EVENT_EXPORT_SUCCESS                                          = EVENT_GROUP_PLATFORM_EXPORT + 0x0009; ///< Export is success
const quint32 EVENT_EXPORT_FAILED                                           = EVENT_GROUP_PLATFORM_EXPORT + 0x000A; ///< Export failed
const quint32 EVENT_EXPORT_SOURCE_DIRECTORY_NOT_EXISTS                      = EVENT_GROUP_PLATFORM_EXPORT + 0x000B; ///< source directory does not exist
const quint32 EVENT_EXPORT_TARGET_DIRECTORY_NOT_EXISTS                      = EVENT_GROUP_PLATFORM_EXPORT + 0x000C; ///< target directory does not exist
const quint32 EVENT_EXPORT_LOG_DIRECTORY_NOT_EXISTS                         = EVENT_GROUP_PLATFORM_EXPORT + 0x000D; ///< log directory does not exist
const quint32 EVENT_EXPORT_TARGET_FILE_FORMAT_IS_WRONG                      = EVENT_GROUP_PLATFORM_EXPORT + 0x000E; ///< target file format is wrong
const quint32 EVENT_EXPORT_INVALID_EXPORT                                   = EVENT_GROUP_PLATFORM_EXPORT + 0x000F; ///< invalid type of Import
const quint32 EVENT_EXPORT_CRYTOSERVICE_RUNNING                             = EVENT_GROUP_PLATFORM_EXPORT + 0x0010; ///< trying to run more than one Cryptoservice
const quint32 EVENT_EXPORT_CANNOT_OPEN_FILE_FOR_READ                        = EVENT_GROUP_PLATFORM_EXPORT + 0x0011; ///< cannot open the file in read mode
const quint32 EVENT_EXPORT_CANNOT_OPEN_FILE_FOR_WRITE                       = EVENT_GROUP_PLATFORM_EXPORT + 0x0012; ///< cannot open the file in write mode
const quint32 EVENT_EXPORT_ERROR_TO_READ                                   = EVENT_GROUP_PLATFORM_EXPORT + 0x0013;  ///< error in reading the file
const quint32 EVENT_EXPORT_ERROR_TO_WRITE                                  = EVENT_GROUP_PLATFORM_EXPORT + 0x0014;  ///< error in writing the file
const quint32 EVENT_EXPORT_INDEX_IS_MATCHING                                = EVENT_GROUP_PLATFORM_EXPORT + 0x0015; ///< hash chain index and USB device indexes are matching
const quint32 EVENT_EXPORT_KEY_SIZE_LESS                                    = EVENT_GROUP_PLATFORM_EXPORT + 0x0016; ///< key size is more by comparing with hash block size
const quint32 EVENT_EXPORT_KEYDATA_SIZE_IS_NOT_MATCHING                     = EVENT_GROUP_PLATFORM_EXPORT + 0x0017; ///< key file size is not matching with the in-built key file size
const quint32 EVENT_EXPORT_HMAC_NOT_INITIALIZED                             = EVENT_GROUP_PLATFORM_EXPORT + 0x0018; ///< HMAC is not initialized
const quint32 EVENT_EXPORT_AES_NOT_INITIALIZED                              = EVENT_GROUP_PLATFORM_EXPORT + 0x0019; ///< AES is not initialized
const quint32 EVENT_EXPORT_INTEGER_SIZE_IS_MORE                             = EVENT_GROUP_PLATFORM_EXPORT + 0x001A; ///< size of the array shall be either 4 or 2 or 1
const quint32 EVENT_EXPORT_MSB_BIT_IS_NOT_SET                               = EVENT_GROUP_PLATFORM_EXPORT + 0x001B; ///< Most significat bit not set for calculations
const quint32 EVENT_EXPORT_INVALID_FILE_MODE                                = EVENT_GROUP_PLATFORM_EXPORT + 0x001C; ///< file mode is not valid
const quint32 EVENT_EXPORT_HMAC_COMPUTATION_STARTED                         = EVENT_GROUP_PLATFORM_EXPORT + 0x001D; ///< HMAC computation already started
const quint32 EVENT_EXPORT_ZIP_ERROR                                        = EVENT_GROUP_PLATFORM_EXPORT + 0x001E; ///< Unable to zip the files
const quint32 EVENT_EXPORT_ZIP_COMMAND_NOT_FOUND                            = EVENT_GROUP_PLATFORM_EXPORT + 0x001F; ///< Zip command not found in the system
const quint32 EVENT_EXPORT_ZIP_IS_TAKING_LONGTIME                           = EVENT_GROUP_PLATFORM_EXPORT + 0x0020; ///< zip is taking long time to complete the request
const quint32 EVENT_EXPORT_STOPPED_FOREVER                                  = EVENT_GROUP_PLATFORM_EXPORT + 0x0021; ///< Export process stopped working forever
const quint32 EVENT_EXPORT_UNABLE_TO_CREATE_FILE_CMS_STATUS                 = EVENT_GROUP_PLATFORM_EXPORT + 0x0022; ///< Unable to create the CMS_Status.csv file


const quint32 EVENT_COVERGLASS_REFILL                                       = EVENT_GROUP_SHARED_EVENTS + 0x0001; ///< Fill coverglass and/or mountant
const quint32 EVENT_STOP_ACCEPT_NEW_RACK                                    = EVENT_GROUP_SHARED_EVENTS + 0x0002; ///< Stop accepting new racks/data
const quint32 EVENT_THREAD_ON_GO_RECEIVED                                   = EVENT_GROUP_SHARED_EVENTS + 0x0003; ///< Debug message: %1 thread on go received
const quint32 EVENT_THREAD_CREATE_AND_INITIALIZE                            = EVENT_GROUP_SHARED_EVENTS + 0x0004; ///< Debug message: %1 thread create and initialize is called
const quint32 EVENT_THREAD_CONTROL_STARTED                                  = EVENT_GROUP_SHARED_EVENTS + 0x0005; ///< Debug message: Specific thread controller %1 is started
                                                                                                                  ///< (For Device command processor-0x0101,Gui-0x0102,Scheduler-0x0103,ImportExport Thread-0x0104,Export-0x0105,Sepia-0x0106,BLG-0x0107)




} // end namespace Global

#endif // GLOBAL_GLOBALEVENTCODES_H
