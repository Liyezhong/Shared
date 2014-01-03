/****************************************************************************/
/*! \file PasswordManagerEventCodes.h
 *
 *  \brief All event codes used by PasswordManager.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-04
 *  $Author:    $ Raju
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

#ifndef PASSWORDMANAGER_PASSWORDMANAGEREVENTCODES_H
#define PASSWORDMANAGER_PASSWORDMANAGEREVENTCODES_H

#include <../Include/PlatformEventCodes.h>

/****************************************************************************/
/**
 * \brief Namespace for Password Management stuff.
 */
/****************************************************************************/
namespace PasswordManager {

const quint32 EVENT_PASSWORDMANAGER_NO_USB_DEVICE_EXISTS        = EVENT_GROUP_PLATFORM_PASSWORDMANAGER + 0x0001;    ///< No USB device exits in the system
const quint32 EVENT_PASSWORDMANAGER_KEY_FILE_NOT_FOUND          = EVENT_GROUP_PLATFORM_PASSWORDMANAGER + 0x0002;    ///< "LBSaccess.key file does not exist
const quint32 EVENT_PASSWORDMANAGER_HASH_NOT_MATCHED            = EVENT_GROUP_PLATFORM_PASSWORDMANAGER + 0x0003;    ///< Hash value is not matched
const quint32 EVENT_PASSWORDMANAGER_CHECKSUM_NOT_MATCHED        = EVENT_GROUP_PLATFORM_PASSWORDMANAGER + 0x0004;    ///< check-sum is not matched
const quint32 EVENT_PASSWORDMANAGER_DATE_IS_EXPIRED             = EVENT_GROUP_PLATFORM_PASSWORDMANAGER + 0x0005;    ///< date is expired for the validation
const quint32 EVENT_PASSWORDMANAGER_DEVICENAME_NOT_EXISTS       = EVENT_GROUP_PLATFORM_PASSWORDMANAGER + 0x0006;    ///< device name does not exist in the key file
const quint32 EVENT_PASSWORDMANAGER_BASIC_TAG_VALUE_IS_WRONG    = EVENT_GROUP_PLATFORM_PASSWORDMANAGER + 0x0007;    ///< device basic tag value is wrong
const quint32 EVENT_PASSWORDMANAGER_LOG_SERVICE_ID              = EVENT_GROUP_PLATFORM_PASSWORDMANAGER + 0x0008;    ///< log the service ID
} // end namespace PASSWORDMANAGER

#endif // PASSWORDMANAGER_PASSWORDMANAGEREVENTCODES_H

