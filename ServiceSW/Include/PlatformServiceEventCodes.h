/****************************************************************************/
/*! \file PlatformServiceEventCodes.h
 *
 *  \brief Definition of event groups for event codes.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2014-03-24
 *  $Author:    $ Soumya. D
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

#ifndef PLATFORMSERVICEEVENTCODES_H
#define PLATFORMSERVICEEVENTCODES_H

#include <QtGlobal>

// Note : Add event groups for platform service components here

/// Event group for platform service
const quint32 EVENT_GROUP_PLATFORM_SERVICE               = 0x41;                                                 ///< Event group for PlatformService.
const quint32 EVENT_GROUP_PLATFORMSERVICE_MAIN           = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x01) << 16;   ///< Event group for main program.

const quint32 EVENT_SERVICE_IMPORT_NO_FILES_TO_IMPORT                               = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0001;

const quint32 EVENT_SERVICE_IMPORT_FAILED                                           = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0002;

const quint32 EVENT_SERVICE_IMPORTEXPORT_IMPORT_NO_USB                              = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0003;

const quint32 EVENT_SERVICE_IMPORTEXPORT_EXPORT_NO_USB                              = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0004;

const quint32 EVENT_SERVICE_IMPORT_UPDATE_ROLLBACK_FAILED                           = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0005;

const quint32 EVENT_SERVICE_IMPORT_TAMPERED_ARCHIVE_FILE                            = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0006;

const quint32 EVENT_SERVICE_IMPORT_DEVICE_NAME_NOT_MATCHING                         = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0007;

const quint32 EVENT_SERVICE_IMPORT_ARCHIVE_FILE_FORMAT_NOT_PROPER                   = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0008;

const quint32 EVENT_SERVICE_IMPORT_TYPEOFIMPORTNOTVALID                             = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0009;

const quint32 EVENT_SERVICE_IMPORT_REQUIRED_FILES_NOT_AVAILABLE                     = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x000A;

const quint32 EVENT_SERVICE_IMPORT_UNABLE_TO_WRITE_FILES                            = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x000B;

const quint32 EVENT_SERVICE_IMPORT_ROLLBACK_FAILED                                  = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x000C;

const quint32 EVENT_SERVICE_IMPORT_SUCCESS                                          = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x000D;

const quint32 EVENT_SERVICE_EXPORT_SUCCESS                                          = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x000E;

const quint32 EVENT_SERVICE_EXPORT_FAILED                                           = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x000F;

const quint32 EVENT_SERVICE_EXPORT_DIRECTORY_CREATION_FAILED                        = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0010;

const quint32 EVENT_SERVICE_EXPORT_FILES_NOT_COPIED                                 = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0011;

const quint32 EVENT_SERVICE_EXPORT_UNABLE_TO_CREATE_FILE_TEMP_EXPORTCONFIGURATION   = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0012;

const quint32 EVENT_SERVICE_GUI_SETTINGS_DATAMANAGEMENT_IMPORT                      = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0013;

const quint32 EVENT_SERVICE_GUI_SETTINGS_DATAMANAGEMENT_EXPORT                      = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0014;

const quint32 EVENT_SERVICE_GUI_SETTINGS_FIRMWARE_INFO_REQUESTED                    = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0015;

const quint32 EVENT_SERVICE_GUI_SETTINGS_LANGUAGE_CHANGE                            = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0016;

const quint32 EVENT_SERVICE_NO_KEY_FILE                                             = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0017;

const quint32 EVENT_SERVICE_DEVICE_NAME_NOT_FOUND                                   = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0018;

const quint32 EVENT_SERVICE_BASIC_TAG_NOT_MATCHING                                  = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x0019;

const quint32 EVENT_SERVICE_KEY_DATE_EXPIRED                                        = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x001A;

const quint32 EVENT_SERVICE_INVALID_PIN                                             = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x001B;

const quint32 EVENT_SERVICE_IPADDRESS_UPDATED                                       = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x001C;

const quint32 EVENT_SERVICE_DATETIME_UPDATED                                        = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x001D;

const quint32 EVENT_SERVICE_IMPORT_CANCEL                                           = EVENT_GROUP_PLATFORMSERVICE_MAIN + 0x001E;


#endif // namespace PLATFORMSERVICEEVENTCODES_H
