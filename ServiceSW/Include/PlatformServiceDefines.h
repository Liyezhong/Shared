/****************************************************************************/
/*! \file Include/PlatformServiceDefines.h
 *
 *  \brief Some Service Software definitions.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ Srivathsa HH
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

#ifndef PLATFORMSERVICE_H
#define PLATFORMSERVICE_H

namespace PlatformService {

/****************************************************************************/
/**
 * \brief Enum containing Service or Manufacturing Mode.
 */
/****************************************************************************/
typedef enum SoftwareModeType {
    SERVICE_MODE,                 ///< Service Mode.
    MANUFACTURING_MODE,           ///< Manufacturing Mode.
    INVALID_MODE                  ///< Invalid Mode
} SoftwareModeType_t;             ///< Enum variable

/****************************************************************************/
/**
 * \brief Enum for Network settings
 */
/****************************************************************************/
typedef enum NetworkSettings {
    HOST_REACHABLE,             ///< Host reachable or not
    SERVICE_AVAILABLE,          ///< Service available or not
    ACCESS_RIGHTS,              ///< Access rights
    DOWNLOAD_FIRMWARE           ///< Download firmware
} NetworkSettings_t;            ///< Enum variable

/****************************************************************************/
/**
 * \brief Languages available for S.A.M software
 */
/****************************************************************************/
typedef enum Languages {
    CHINESE,                    ///< Chinese language
    DEUTSCH,                    ///< Deutsch language
    US_ENGLISH                  ///< English language
} Languages_t;                  ///< Enum variable

}


#endif //PLATFORMSERVICE_H
