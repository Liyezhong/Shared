/****************************************************************************/
/*! \file Global/Include/GlobalExitCodes.h
 *
 *  \brief All event codes used by all.
 *
 *  Version:    0.1
 *  Date:       2012-10-23
 *  Author:     Raju
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

#ifndef GLOBAL_GLOBALEXITCODES_H
#define GLOBAL_GLOBALEXITCODES_H

#include <QtGlobal>

namespace Global {
// string IDs for Export
// Exit codes for the Export process
const qint32  EXIT_CODE_EXPORT_SUCCESS                                         = 0x000; ///< Export is success
const qint32  EXIT_CODE_EXPORT_FAILED                                          = 0x001; ///< Export failed
const qint32  EXIT_CODE_EXPORT_UNABLE_TO_READ_FILE_TEMP_EXPORTCONFIGURATION    = 0x002; ///< unable to read the configuration file
const qint32  EXIT_CODE_EXPORT_INIT_CONTAINER_FAILED                           = 0x003; ///< initialization of the container failed
const qint32  EXIT_CODE_EXPORT_VERIFICATION_CONTAINER_FAILED                   = 0x004; ///< verification of the container failed
const qint32  EXIT_CODE_EXPORT_UNABLE_ARCHIVE_FILES                            = 0x005; ///< unable to archive files
const qint32  EXIT_CODE_EXPORT_SOURCE_DIRECTORY_NOT_EXISTS                     = 0x006; ///< source directory does not exists
const qint32  EXIT_CODE_EXPORT_TARGET_DIRECTORY_NOT_EXISTS                     = 0x007; ///< target directory does not exist
const qint32  EXIT_CODE_EXPORT_LOG_DIRECTORY_NOT_EXISTS                        = 0x008; ///< log directory does not exist
const qint32  EXIT_CODE_EXPORT_TARGET_FILE_FORMAT_IS_WRONG                     = 0x009; ///< target file format is wrong
const qint32  EXIT_CODE_EXPORT_INVALID_EXPORT                                  = 0x00a; ///< invalid type of Export
const qint32  EXIT_CODE_EXPORT_CRYTOSERVICE_RUNNING                            = 0x00b; ///< trying to run more than one Cryptoservice
const qint32  EXIT_CODE_EXPORT_CANNOT_OPEN_FILE_FOR_READ                       = 0x00c; ///< cannot open the file for reading
const qint32  EXIT_CODE_EXPORT_CANNOT_OPEN_FILE_FOR_WRITE                      = 0x00d; ///< cannot open the file for writing
const qint32  EXIT_CODE_EXPORT_ERROR_TO_READ                                   = 0x00e; ///< error in reading file
const qint32  EXIT_CODE_EXPORT_ERROR_TO_WRITE                                  = 0x00f; ///< error in writing file
const qint32  EXIT_CODE_EXPORT_INDEX_IS_MATCHING                               = 0x010; ///< hash chain index and USB device indexes are matching
const qint32  EXIT_CODE_EXPORT_KEY_SIZE_LESS                                   = 0x011; ///< key size is more by comparing with hash block size
const qint32  EXIT_CODE_EXPORT_KEYDATA_SIZE_IS_NOT_MATCHING                    = 0x012; ///< key file size is not matching with the in-built key file size
const qint32  EXIT_CODE_EXPORT_HMAC_NOT_INITIALIZED                            = 0x013; ///< HMAC is not initialized
const qint32  EXIT_CODE_EXPORT_AES_NOT_INITIALIZED                             = 0x014; ///< AES is not initialized
const qint32  EXIT_CODE_EXPORT_INTEGER_SIZE_IS_MORE                            = 0x015; ///< size of the array shall be either 4 or 2 or 1
const qint32  EXIT_CODE_EXPORT_MSB_BIT_IS_NOT_SET                              = 0x016; ///< Most significat bit not set for calculations
const qint32  EXIT_CODE_EXPORT_INVALID_FILE_MODE                               = 0x017; ///< file mode is not valid
const qint32  EXIT_CODE_EXPORT_HMAC_COMPUTATION_STARTED                        = 0x018; ///< HMAC computation is already started
const qint32  EXIT_CODE_EXPORT_ZIP_ERROR                                       = 0x019; ///< Unable to zip the files
const qint32  EXIT_CODE_EXPORT_ZIP_COMMAND_NOT_FOUND                           = 0x01a; ///< Zip command not found in the system
const qint32  EXIT_CODE_EXPORT_ZIP_IS_TAKING_LONGTIME                          = 0x01b; ///< zip is taking long time to complete the request
const qint32  EXIT_CODE_EXPORT_NO_ENOUGH_SPACE_ON_USB                          = 0X01C; ///< No enouth space on USB storage!

} // end namespace Global

#endif // GLOBAL_GLOBALEVENTCODES_H
