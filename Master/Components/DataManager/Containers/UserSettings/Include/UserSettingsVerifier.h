/****************************************************************************/
/*! \file Components/DataManager/Containers/UserSettings/Include/UserSettingsVerifier.h
 *
 *  \brief UserSettingsVerifier definition.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 23.04.2012
 *   $Author:  $ Raju123, Ramya GJ
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
#ifndef DATAMANAGER_CUSERSETTINGSVERIFIER_H
#define DATAMANAGER_CUSERSETTINGSVERIFIER_H

#include <QString>
#include <QXmlStreamReader>

#include "DataManager/Containers/UserSettings/Include/UserSettingsInterface.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "Global/Include/EventObject.h"
//lint -e1540

namespace DataManager {

const int MIN_SOUND_NUMBER    = 1; ///< Minimum value for the sound
const int MIN_SOUND_LEVEL     = 2; ///< Minimum value for the sound level
const int MAX_SOUND_NUMBER    = 6; ///< Maximum value for the sound
const int MAX_SOUND_LEVEL     = 9; ///< Maximum value for the sound level
const int MIN_PROXY_IP_PORT   = 1; ///< Minimum value of IP Port
const int MAX_PROXY_IP_PORT   = 65535; ///< Maximum value of IP Port
const int MIN_IP_ADDRESS_NUMBER = 0;    ///< Minimum value of IP Address Number
const int MAX_IP_ADDRESS_NUMBER = 255;  ///< Maximum value of IP Address Number
const int MAX_PROXY_USERNAME_LENGTH = 16;///< Maximum length of Proxy UserName
const int MIN_PROXY_USERNAME_LENGTH = 1; ///< Minimum length of Proxy UserName
const int MAX_PROXY_PASSWORD_LENGTH = 16; ///< Maximum length of Proxy Password
const int MIN_PROXY_PASSWORD_LENGTH = 4; ///< Minimum length of Proxy Password
const int MIN_SOUND_LEVEL_FOR_WARNING = 0; ///< Minimum sound level for warning
const int MAX_SOUND_LEVEL_FOR_WARNING = 9; ///< Maximum sound level for warning

/****************************************************************************/
/*!
 *  \brief  This class implements a list of rack data verifiers.
 */
/****************************************************************************/
class CUserSettingsVerifier : public IVerifierInterface
{
public:
    CUserSettingsVerifier();

    bool VerifyData(CDataContainerBase* p_UserSettingsInterface);  // use concrete class for concrete verifier

    ErrorMap_t &GetErrors();

    void ResetErrors();
    bool IsLocalVerifier();

protected:
    CUserSettingsInterface* mp_USettingsInterface;   ///< Local pointer to a rack list
    ErrorMap_t m_ErrorMap;          //!< To store Error ID and any arguments associated
private:
    void CheckSoundLevelWarnings(CUserSettings* p_UserSettings, bool& VerifiedData);
    void CheckNetWorkSettings(CUserSettings* p_UserSettings, bool& VerifiedData);
    bool CheckProxyIPAddress(CUserSettings* p_UserSettings);
    bool CheckProxyIPPort(CUserSettings *p_UserSettings);

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CUserSettingsVerifier)
};

} // namespace DataManager
#endif // DATAMANAGER_CUSERSETTINGSVERIFIER_H
