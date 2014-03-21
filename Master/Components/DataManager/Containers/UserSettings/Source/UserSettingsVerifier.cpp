/****************************************************************************/
/*! \file  Components/DataManager/Containers/UserSettings/Source/UserSettingsVerifier.cpp
 *
 *  \brief Implementation file for class UserSettingsVerifier.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2012-04-23
 *  $Author:    $ Raju123
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

#include <QDebug>
#include <QFile>

#include "DataManager/Containers/UserSettings/Include/UserSettingsVerifier.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/EventObject.h"
//lint -e641

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CUserSettingsVerifier::CUserSettingsVerifier() : mp_USettingsInterface(NULL)
{

}

/****************************************************************************/
/*!
 *  \brief  Verifies user settings
 *
 *  \iparam p_UserSettingsInterface = user settings interface class object
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CUserSettingsVerifier::VerifyData(CDataContainerBase* p_UserSettingsInterface)
{    
    // by default make the verification flag to true
    bool VerifiedData = true;
    try {
        CHECKPTR(p_UserSettingsInterface)
        // to store the error description
        QString ErrorDescription;


        // assign pointer to member variable
        mp_USettingsInterface = static_cast<CUserSettingsInterface*>(p_UserSettingsInterface);
        CHECKPTR(mp_USettingsInterface)
        // check content of user settings
        CUserSettings* p_UserSettings = mp_USettingsInterface->GetUserSettings();
        CHECKPTR(p_UserSettings)
        // check the language
        switch (p_UserSettings->GetLanguage()) {
        case QLocale::C:
            qDebug() << "Unsupported language is detected";
            m_ErrorHash.insert(EVENT_DM_ERROR_NOT_SUPPORTED_LANGUAGE, Global::tTranslatableStringList() << p_UserSettings->GetLanguage());
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_NOT_SUPPORTED_LANGUAGE, Global::tTranslatableStringList() <<p_UserSettings->GetLanguage(), true);
            VerifiedData = false;
            break;
        default:
            // nothing to do
            break;
        }


        // check the Date format
        switch (p_UserSettings->GetDateFormat()) {
        case Global::DATE_INTERNATIONAL:
        case Global::DATE_ISO:
        case Global::DATE_US:
            break;
        default:
            qDebug() << "Date format is not valid";
            m_ErrorHash.insert(EVENT_DM_INVALID_DATEFORMAT, Global::tTranslatableStringList() << p_UserSettings->GetDateFormat());
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_DATEFORMAT, Global::tTranslatableStringList() << p_UserSettings->GetDateFormat(), true);
            VerifiedData = false;
            break;
        }

        // check the Time format
        switch (p_UserSettings->GetTimeFormat()) {
        case Global::TIME_24:
        case Global::TIME_12:
            break;
        default:
            qDebug() << "Time format is not valid";
            m_ErrorHash.insert(EVENT_DM_INVALID_TIMEFORMAT, Global::tTranslatableStringList() << p_UserSettings->GetTimeFormat());
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_TIMEFORMAT, Global::tTranslatableStringList() <<p_UserSettings->GetTimeFormat(), true);
            VerifiedData = false;
            break;
        }

        // check the Temperature format
        switch (p_UserSettings->GetTemperatureFormat()) {
        case Global::TEMP_FORMAT_CELSIUS:
        case Global::TEMP_FORMAT_FAHRENHEIT:
            break;
        default:
            qDebug() << "Tempeature format is not valid";
            m_ErrorHash.insert(EVENT_DM_INVALID_TEMPFORMAT, Global::tTranslatableStringList() << p_UserSettings->GetTemperatureFormat());
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_TEMPFORMAT, Global::tTranslatableStringList() <<p_UserSettings->GetTemperatureFormat(), true);
            VerifiedData = false;
            break;
        }

        // check the sound level warnings and errors
        CheckSoundLevelWarnings(p_UserSettings, VerifiedData);

        // check network settings parameters
        CheckNetWorkSettings(p_UserSettings, VerifiedData);
    }
    catch (Global::Exception &E) {
        Global::EventObject::Instance().RaiseEvent(E.GetErrorCode(),E.GetAdditionalData(),true);
        VerifiedData = false;
    }
    catch (...) {
        VerifiedData = false;
    }
    return VerifiedData;

}

/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorHash_t& CUserSettingsVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorHash;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CUserSettingsVerifier::ResetLastErrors()
{
    m_ErrorHash.clear();
}

/****************************************************************************/
/*!
 *  \brief  Returns whether verifier is a local verifier or not
 *
 *  \return bool - On successful (True) otherwise (False)
 */
/****************************************************************************/
bool CUserSettingsVerifier::IsLocalVerifier()
{
    return true;
}


/****************************************************************************/
/*!
 *  \brief  check the sould level warnings
 *
 *  \iparam p_UserSettings - Pointer to CUserSettings
 *  \iparam VerifiedData = verifier flag value
 *
 */
/****************************************************************************/
void CUserSettingsVerifier::CheckSoundLevelWarnings(CUserSettings* p_UserSettings, bool& VerifiedData)
{
    // check the error tones for the sound
    if (!((p_UserSettings->GetSoundNumberError() >= MIN_SOUND_NUMBER) && (p_UserSettings->GetSoundNumberError() <= MAX_SOUND_NUMBER))) {
        qDebug() << "Unknown error tone is detected for the sound";
        m_ErrorHash.insert(EVENT_DM_ERROR_SOUND_NUMBER_OUT_OF_RANGE, Global::tTranslatableStringList() << "");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_SOUND_NUMBER_OUT_OF_RANGE, Global::tTranslatableStringList() <<"", true);
        VerifiedData = false;
    }

    if (!((p_UserSettings->GetSoundLevelError() >= MIN_SOUND_LEVEL) && (p_UserSettings->GetSoundLevelError() <= MAX_SOUND_LEVEL))) {
        qDebug() << "Unknown sound number error volume tone is detected";
        m_ErrorHash.insert(EVENT_DM_ERROR_SOUND_LEVEL_OUT_OF_RANGE, Global::tTranslatableStringList() << "");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_SOUND_LEVEL_OUT_OF_RANGE, Global::tTranslatableStringList() <<"", true);
        VerifiedData = false;
    }

    if (!((p_UserSettings->GetSoundNumberWarning() >= MIN_SOUND_NUMBER) && (p_UserSettings->GetSoundNumberWarning() <= MAX_SOUND_NUMBER))) {
        qDebug() << "Unknown warning warning tone is detected for the sound";
        m_ErrorHash.insert(EVENT_DM_WARN_SOUND_NUMBER_OUT_OF_RANGE, Global::tTranslatableStringList() << "");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_WARN_SOUND_NUMBER_OUT_OF_RANGE, Global::tTranslatableStringList() <<"", true);
        VerifiedData = false;
    }

    if (!((p_UserSettings->GetSoundLevelWarning() >= MIN_SOUND_LEVEL_FOR_WARNING) && (p_UserSettings->GetSoundLevelWarning() <= MAX_SOUND_LEVEL_FOR_WARNING))) {
        qDebug() << "Unknown sound number warning volume tone is detected";
        m_ErrorHash.insert(EVENT_DM_WARN_SOUND_LEVEL_OUT_OF_RANGE, Global::tTranslatableStringList() <<"");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_WARN_SOUND_LEVEL_OUT_OF_RANGE, Global::tTranslatableStringList() <<"", true);
        VerifiedData = false;
    }

}

/****************************************************************************/
/*!
 *  \brief  Check Network Settings.
 *
 *  \iparam p_UserSettings - Pointer to CUserSettings
 *  \iparam VerifiedData = verifier flag value
 *
 */
/****************************************************************************/
void CUserSettingsVerifier::CheckNetWorkSettings(CUserSettings* p_UserSettings, bool& VerifiedData)
{
    if (!((p_UserSettings->GetRemoteCare() == Global::ONOFFSTATE_ON || p_UserSettings->GetRemoteCare() == Global::ONOFFSTATE_OFF))) {
        qDebug() << "NETWORK SETTINGS REMOTE CONNECTION IS NOT VALID";
        m_ErrorHash.insert(EVENT_DM_ERROR_INVALID_REMOTECARE_ONOFFSTATE, Global::tTranslatableStringList() << p_UserSettings->GetRemoteCare());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_INVALID_REMOTECARE_ONOFFSTATE, Global::tTranslatableStringList() << p_UserSettings->GetRemoteCare(), true);
        VerifiedData = false;
    }

    if (!((p_UserSettings->GetDirectConnection() == Global::ONOFFSTATE_ON || p_UserSettings->GetDirectConnection() == Global::ONOFFSTATE_OFF))) {
        qDebug() << "NETWORK SETTINGS DIRECT CONNECTION IS NOT VALID";
        m_ErrorHash.insert(EVENT_DM_ERROR_INVALID_DIRECT_CONNECTION_ONOFFSTATE, Global::tTranslatableStringList() << p_UserSettings->GetDirectConnection());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_INVALID_DIRECT_CONNECTION_ONOFFSTATE, Global::tTranslatableStringList() << p_UserSettings->GetDirectConnection(), true);
        VerifiedData = false;
    }

    if (!((p_UserSettings->GetProxyUserName().isEmpty() != true  && p_UserSettings->GetProxyUserName().length() >= MIN_PROXY_USERNAME_LENGTH &&
          p_UserSettings->GetProxyUserName().length() <= MAX_PROXY_USERNAME_LENGTH))) {
        qDebug() << "PROXY USERNAME IS NOT VALID";
        m_ErrorHash.insert(EVENT_DM_INVALID_PROXY_USERNAME_CHAR_COUNT, Global::tTranslatableStringList() << p_UserSettings->GetProxyUserName());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_PROXY_USERNAME_CHAR_COUNT, Global::tTranslatableStringList() << p_UserSettings->GetProxyUserName(), true);
        VerifiedData = false;
    }

    if (!((p_UserSettings->GetProxyPassword().isEmpty() != true && p_UserSettings->GetProxyPassword().length() >= MIN_PROXY_PASSWORD_LENGTH &&
           p_UserSettings->GetProxyPassword().length() <= MAX_PROXY_PASSWORD_LENGTH))) {
        qDebug() << "PROXY PASSWORD IS NOT VALID";
        m_ErrorHash.insert(EVENT_DM_INVALID_PROXY_PASSWORD_CHAR_COUNT, Global::tTranslatableStringList() << p_UserSettings->GetProxyPassword());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_PROXY_PASSWORD_CHAR_COUNT, Global::tTranslatableStringList() << p_UserSettings->GetProxyPassword(), true);
        VerifiedData = false;
    }

    if (!(CheckProxyIPAddress(p_UserSettings))) {
        qDebug() << "PROXY IP ADDRESS NOT IS NOT VALID";
        m_ErrorHash.insert(EVENT_DM_ERROR_INVALID_PROXY_IP_ADDRESS, Global::tTranslatableStringList() << p_UserSettings->GetProxyIPAddress());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_INVALID_PROXY_IP_ADDRESS, Global::tTranslatableStringList() <<  p_UserSettings->GetProxyIPAddress(), true);
        VerifiedData = false;
    }

    if (!(CheckProxyIPPort(p_UserSettings))) {
        qDebug() << "PROXY IP PORT NUMBER NOT IN RANGE";
        m_ErrorHash.insert(EVENT_DM_ERROR_INVALID_PROXY_IP_PORT, Global::tTranslatableStringList() << QString::number(p_UserSettings->GetProxyIPPort()));
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_INVALID_PROXY_IP_PORT, Global::tTranslatableStringList() << QString::number(p_UserSettings->GetProxyIPPort()), true);
        VerifiedData = false;
    }

}

/****************************************************************************/
/*!
 *  \brief Check Proxy IP Address of Network Settings.
 *
 *  \iparam p_UserSettings - Pointer to CUserSettings
 *  \return True - If IP address is valid else False
 */
/****************************************************************************/
bool CUserSettingsVerifier::CheckProxyIPAddress(CUserSettings *p_UserSettings)
{
    QString ProxyIPAddress = p_UserSettings->GetProxyIPAddress();
    qDebug()<<"\n\n Count= "<< ProxyIPAddress.split(".").count();
    bool VerifiedIPAddress = false;
    if (ProxyIPAddress.split(".").count() == 4) {
        for (int SplitCount = 0; SplitCount < ProxyIPAddress.split(".").count(); SplitCount++)
        {
            QString AddressNumberString = (ProxyIPAddress.split(".").value(SplitCount));
            if (AddressNumberString.length() <= 3) {
                bool Result = false;
                int AddressNumber = AddressNumberString.toInt(&Result, 10);
                if (Result) {
                    qDebug()<<"\n\n AddressNumber ="<< AddressNumber;
                    if (AddressNumber < MIN_IP_ADDRESS_NUMBER || AddressNumber > MAX_IP_ADDRESS_NUMBER || AddressNumberString == "") {
                        return false;
                    }
                    else if (SplitCount == 3) {
                        if (AddressNumber == 0) {
                            return false;
                        }
                        else {
                            VerifiedIPAddress = true;
                        }
                    }
                    else {
                        VerifiedIPAddress = true;
                    }
                }
                else {
                    VerifiedIPAddress = false;
                    return VerifiedIPAddress;
                }
            }
            else {
                VerifiedIPAddress = false;
                return VerifiedIPAddress;
            }
        }
    }
    else {
        VerifiedIPAddress = false;
    }
    return VerifiedIPAddress;
}

/****************************************************************************/
/*!
 *  \brief Check Proxy IP Port of Network Settings.
 *
 *  \iparam p_UserSettings - Pointer to CUserSettings
 *  \return True - If IP address is valid else False
 */
/****************************************************************************/
bool CUserSettingsVerifier::CheckProxyIPPort(CUserSettings *p_UserSettings)
{
    bool Result = false;
    QString IPPort = QString::number(p_UserSettings->GetProxyIPPort());
    int PortNumber = IPPort.toInt(&Result, 10);
    if (Result) {
        if (PortNumber < MIN_PROXY_IP_PORT || PortNumber > MAX_PROXY_IP_PORT) {
            return false;
        }
        else {
            return true;
        }
    }
    else {
        return false;
    }
}
}  // namespace DataManager


