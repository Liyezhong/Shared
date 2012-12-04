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

const quint32 EVENT_GLOBAL_UNDEFINED                        = EVENT_GROUP_PLATFORM_GLOBAL + 0xFFFF; ///< undefined. Used for initialisations
const quint32 EVENT_GLOBAL_UNKNOWN_STRING_ID                = EVENT_GROUP_PLATFORM_GLOBAL + 0x0001; ///< Unknown string id: %1.
const quint32 EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION          = EVENT_GROUP_PLATFORM_GLOBAL + 0x0002; ///< Unknown exception caught: file: %1 line: %2.
const quint32 EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION          = EVENT_GROUP_PLATFORM_GLOBAL + 0x0003; ///< Memory allocation error: file: %1 line: %2.
const quint32 EVENT_GLOBAL_ERROR_NULL_POINTER               = EVENT_GROUP_PLATFORM_GLOBAL + 0x0004; ///< Pointer is NULL: %1 file: %2 line: %3.
const quint32 EVENT_GLOBAL_ERROR_NOT_NULL_POINTER           = EVENT_GROUP_PLATFORM_GLOBAL + 0x0005; ///< Pointer is not NULL: %1 file: %2 line: %3.
const quint32 EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED       = EVENT_GROUP_PLATFORM_GLOBAL + 0x0006; ///< Signal not connected: %1.
const quint32 EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT        = EVENT_GROUP_PLATFORM_GLOBAL + 0x0007; ///< Error connecting signal %1::%2 to slot %3::%4 in file: %5 line: %6.
const quint32 EVENT_GLOBAL_ERROR_SIGNAL_SIGNAL_CONNECT      = EVENT_GROUP_PLATFORM_GLOBAL + 0x0008; ///< Error connecting signal %1::%2 to signal %3::%4 in file: %5 line: %6.
const quint32 EVENT_GLOBAL_ERROR_INVOKEMETHOD               = EVENT_GROUP_PLATFORM_GLOBAL + 0x0009; ///< Call of invokeMethod for method '%1' in object '%2' failed.
const quint32 EVENT_GLOBAL_ERROR_READ_CONFIG_FILE           = EVENT_GROUP_PLATFORM_GLOBAL + 0x000A; ///< Error reading config file %1.
const quint32 EVENT_GLOBAL_ERROR_MONOTONIC_CLOCK            = EVENT_GROUP_PLATFORM_GLOBAL + 0x000B; ///< Error reading monotonic clock.
const quint32 EVENT_GLOBAL_ERROR_FALLBACK_LANGUAGE          = EVENT_GROUP_PLATFORM_GLOBAL + 0x000C; ///< Could not set fallback language to '%1'.
const quint32 EVENT_GLOBAL_ERROR_FILE_CREATE                = EVENT_GROUP_PLATFORM_GLOBAL + 0x000D; ///< Error creating file %1.
const quint32 EVENT_GLOBAL_ERROR_FILE_OPEN                  = EVENT_GROUP_PLATFORM_GLOBAL + 0x000E; ///< Error opening file %1.
const quint32 EVENT_GLOBAL_ERROR_FILE_REMOVE                = EVENT_GROUP_PLATFORM_GLOBAL + 0x000F; ///< Error deleting file %1.
const quint32 EVENT_GLOBAL_ERROR_FILE_WRITE                 = EVENT_GROUP_PLATFORM_GLOBAL + 0x0010; ///< Error writing data: File %1. %2 bytes of %3 written.
const quint32 EVENT_GLOBAL_ERROR_FILE_FLUSH                 = EVENT_GROUP_PLATFORM_GLOBAL + 0x0011; ///< Error flushing file %1.
const quint32 EVENT_GLOBAL_ERROR_FILE_RENAME                = EVENT_GROUP_PLATFORM_GLOBAL + 0x0012; ///< Error could not rename file %1 to %2.
const quint32 EVENT_GLOBAL_ERROR_FILE_SEEK                  = EVENT_GROUP_PLATFORM_GLOBAL + 0x0013; ///< Error seeking in file %1.
const quint32 EVENT_GLOBAL_ERROR_TIME_OFFSET_TOO_LARGE      = EVENT_GROUP_PLATFORM_GLOBAL + 0x0014; ///< Time offset is too large '%1' seconds. Allowed: '%2' seconds.
const quint32 EVENT_GLOBAL_ERROR_READING_SERIAL_NUMBER      = EVENT_GROUP_PLATFORM_GLOBAL + 0x0015; ///< Error reading serial number.
const quint32 EVENT_GLOBAL_ERROR_COMPUTING_NEW_REF          = EVENT_GROUP_PLATFORM_GLOBAL + 0x0016; ///< Unable to compute new command ref.
const quint32 EVENT_GLOBAL_ERROR_SIGNAL_RECEIVED            = EVENT_GROUP_PLATFORM_GLOBAL + 0x0017; ///< We have an unexpected signal (possibly crash)
const quint32 EVENT_GLOBAL_DUMMY_ERROR                      = EVENT_GROUP_PLATFORM_GLOBAL + 0xFFFE; ///< Dummy error. Should be replaced by correct error. File: %1 line: %2.

// now some string IDs
const quint32 EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE          = EVENT_GROUP_PLATFORM_GLOBAL + 0x1000; ///< Debug message: '%1'
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_UNDEFINED      = EVENT_GROUP_PLATFORM_GLOBAL + 0x1001; ///< Undefined Event type
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_FATAL_ERROR    = EVENT_GROUP_PLATFORM_GLOBAL + 0x1002; ///< Fatal error
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_ERROR          = EVENT_GROUP_PLATFORM_GLOBAL + 0x1003; ///< Error
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_WARNING        = EVENT_GROUP_PLATFORM_GLOBAL + 0x1004; ///< Warning
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_INFO           = EVENT_GROUP_PLATFORM_GLOBAL + 0x1005; ///< Info
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_DEBUG          = EVENT_GROUP_PLATFORM_GLOBAL + 0x1006; ///< Debug
const quint32 EVENT_GLOBAL_STRING_ID_EVTSTAT_ON             = EVENT_GROUP_PLATFORM_GLOBAL + 0x1007; ///< On
const quint32 EVENT_GLOBAL_STRING_ID_EVTSTAT_OFF            = EVENT_GROUP_PLATFORM_GLOBAL + 0x1008; ///< Off
const quint32 EVENT_GLOBAL_STRING_TERMINATING               = EVENT_GROUP_PLATFORM_GLOBAL + 0x1009; ///< Terminating software.
const quint32 EVENT_GLOBAL_SET_DATE_TIME                    = EVENT_GROUP_PLATFORM_GLOBAL + 0x100A; ///< Setting date / time to '%1'.
const quint32 EVENT_GLOBAL_CURRENT_TIME_OFFSET              = EVENT_GROUP_PLATFORM_GLOBAL + 0x100B; ///< Current time offset is '%1' seconds.
const quint32 EVENT_GLOBAL_POWER_FAIL                       = EVENT_GROUP_PLATFORM_GLOBAL + 0x100C; ///< Power fail.

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


} // end namespace Global

#endif // GLOBAL_GLOBALEVENTCODES_H
