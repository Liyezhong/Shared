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
const quint32 EVENT_GROUP_PLATFORM_SERVICE                                  = 0x05;

const quint32 EVENT_IMPORT_NO_FILES_TO_IMPORT                               = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x01) << 16;

const quint32 EVENT_IMPORT_FAILED                                           = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x02) << 16;

const quint32 EVENT_IMPORTEXPORT_IMPORT_NO_USB                              = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x03) << 16;

const quint32 EVENT_IMPORTEXPORT_EXPORT_NO_USB                              = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x04) << 16;

const quint32 EVENT_IMPORT_UPDATE_ROLLBACK_FAILED                           = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x05) << 16;

const quint32 EVENT_IMPORT_TAMPERED_ARCHIVE_FILE                            = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x06) << 16;

const quint32 EVENT_IMPORT_DEVICE_NAME_NOT_MATCHING                         = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x07) << 16;

const quint32 EVENT_IMPORT_ARCHIVE_FILE_FORMAT_NOT_PROPER                   = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x08) << 16;

const quint32 EVENT_IMPORT_TYPEOFIMPORTNOTVALID                             = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x09) << 16;

const quint32 EVENT_IMPORT_REQUIRED_FILES_NOT_AVAILABLE                     = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x0A) << 16;

const quint32 EVENT_IMPORT_UNABLE_TO_WRITE_FILES                            = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x0B) << 16;

const quint32 EVENT_IMPORT_ROLLBACK_FAILED                                  = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x0C) << 16;

const quint32 EVENT_IMPORT_SUCCESS                                          = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x0D) << 16;

const quint32 EVENT_EXPORT_SUCCESS                                          = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x0E) << 16;

const quint32 EVENT_EXPORT_FAILED                                           = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x0F) << 16;

const quint32 DIRECTORY_MNT_STORAGE                                         = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x10) << 16;

const quint32 EVENT_EXPORT_DIRECTORY_CREATION_FAILED                        = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x11) << 16;

const quint32 EVENT_EXPORT_FILES_NOT_COPIED                                 = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x12) << 16;

const quint32 EVENT_EXPORT_UNABLE_TO_CREATE_FILE_TEMP_EXPORTCONFIGURATION   = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x13) << 16;

const quint32 EVENT_GUI_SETTINGS_DATAMANAGEMENT_IMPORT                      = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x14) << 16;

const quint32 EVENT_GUI_SETTINGS_DATAMANAGEMENT_EXPORT                      = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x15) << 16;

const quint32 EVENT_GUI_SETTINGS_FIRMWARE_INFO_REQUESTED                    = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x16) << 16;

const quint32 EVENT_GUI_SETTINGS_LANGUAGE_CHANGE                            = ((EVENT_GROUP_PLATFORM_SERVICE << 8) + 0x17) << 16;


#endif // namespace PLATFORMSERVICEEVENTCODES_H
