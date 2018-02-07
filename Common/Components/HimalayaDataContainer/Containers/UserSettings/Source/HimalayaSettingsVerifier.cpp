/****************************************************************************/
/*! \file HimalayaSettingsVerifier.cpp
 *
 *  \brief impelmentation file for class HimalayaSettingVerifier.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-03-25
 *  $Author:    $ R. Wu
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2010 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaSettingsVerifier.h"

#include "DataManager/Helper/Include/DataManagerEventCodes.h"

#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaUserSettings.h"
#include "HimalayaDataContainer/Helper/Include/HimalayaDataManagerEventCodes.h"

namespace DataManager {

CHimalayaSettingsVerifier::CHimalayaSettingsVerifier()
    : CUserSettingsVerifier()
{
}

/****************************************************************************/
/*!
 *  \brief Verifies the data against constraints
 *  \iparam p_UserSettingsInterface = Settings container
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CHimalayaSettingsVerifier::VerifyData(CDataContainerBase *p_UserSettingsInterface)
{
    bool VerifiedData = false;
    try {
        //Verify base class
        VerifiedData = CUserSettingsVerifier::VerifyData(p_UserSettingsInterface);

        CHECKPTR(mp_USettingsInterface);
        // check content of user settings
        CUserSettings* p_UserSettings = mp_USettingsInterface->GetUserSettings();
        CHECKPTR(p_UserSettings);
        CHimalayaUserSettings HiUserSettings(*p_UserSettings);

        // check the language
        switch (HiUserSettings.GetLanguage()) {
        case QLocale::English:
        case QLocale::German:
        case QLocale::Spanish:
        case QLocale::Portuguese:
        case QLocale::Chinese:
        case QLocale::Japanese:
        case QLocale::Korean: //ko
        case QLocale::Czech:   //cs
        case QLocale::French: //fr
        case QLocale::Hungarian: //hu
        case QLocale::Dutch: //nl
        case QLocale::Polish: //pl
        case QLocale::Turkish: //tr
        case QLocale::Russian: //tr
        case QLocale::Italian: //tr

            break;
        default:
            qDebug() << "Unsupported language is detected";
            m_ErrorMap.insert(EVENT_DM_ERROR_NOT_SUPPORTED_LANGUAGE,
                               Global::tTranslatableStringList()
                               << (int)HiUserSettings.GetLanguage());
            Global::EventObject::Instance().RaiseEvent
                    (EVENT_DM_ERROR_NOT_SUPPORTED_LANGUAGE,
                     Global::tTranslatableStringList()
                     << (int)HiUserSettings.GetLanguage(), true);
            VerifiedData = false;
            break;
        }

        // check sound settings
        int SoundLevelError = HiUserSettings.GetSoundLevelError();
        if (!((MIN_SOUND_LEVEL_ERROR <= SoundLevelError) &&
              (SoundLevelError <= MAX_SOUND_LEVEL)))
        {
            qDebug() << "Unknown error volume level is detected";
            m_ErrorMap.insert(EVENT_DM_ERROR_SOUND_LEVEL_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "");
            Global::EventObject::Instance().
                    RaiseEvent(EVENT_DM_ERROR_SOUND_LEVEL_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "", true);
            VerifiedData = false;
        }

        int SoundPeriodicTimeError = HiUserSettings.GetSoundPeriodicTimeError();
        if (!((MIN_PERIODIC_TIME <= SoundPeriodicTimeError) &&
              (SoundPeriodicTimeError <= MAX_PERIODIC_TIME)))
        {
            qDebug() << "Unknown error periodic time is detected";
            m_ErrorMap.insert(EVENT_DM_INFO_SOUND_PERIODIC_TIME_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "");
            Global::EventObject::Instance().
                    RaiseEvent(EVENT_DM_INFO_SOUND_PERIODIC_TIME_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "", true);
            VerifiedData = false;
        }

        int SoundPeriodicTimeWarning =
                HiUserSettings.GetSoundPeriodicTimeWarning();
        if (!((MIN_PERIODIC_TIME <= SoundPeriodicTimeWarning) &&
              (SoundPeriodicTimeWarning <= MAX_PERIODIC_TIME)))
        {
            qDebug() << "Unknown warning periodic time is detected";
            m_ErrorMap.insert(EVENT_DM_INFO_SOUND_PERIODIC_TIME_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "");
            Global::EventObject::Instance().
                    RaiseEvent(EVENT_DM_INFO_SOUND_PERIODIC_TIME_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "", true);
            VerifiedData = false;
        }

        int SoundNumberInformation = HiUserSettings.GetSoundNumberInformation();
        if (!((MIN_SOUND_NUMBER <= SoundNumberInformation) &&
              (SoundNumberInformation <= MAX_SOUND_NUMBER)))
        {
            qDebug() << "Unknown information tone is detected for the sound";
            m_ErrorMap.insert(EVENT_DM_INFO_SOUND_NUMBER_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "");
            Global::EventObject::Instance().
                    RaiseEvent(EVENT_DM_INFO_SOUND_NUMBER_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "", true);
            VerifiedData = false;
        }

        int SoundLevelInformation = HiUserSettings.GetSoundLevelInformation();
        if (!((MIN_SOUND_LEVEL <= SoundLevelInformation) &&
              (SoundLevelInformation <= MAX_SOUND_LEVEL)))
        {
            qDebug() << "Unknown information level is detected";
            m_ErrorMap.insert(EVENT_DM_INFO_SOUND_LEVEL_OUT_OF_RANGE,
                               Global::tTranslatableStringList() <<"");
            Global::EventObject::Instance().
                    RaiseEvent(EVENT_DM_INFO_SOUND_LEVEL_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "", true);
            VerifiedData = false;
        }

        int SoundPeriodicTimeInformation =
                HiUserSettings.GetSoundPeriodicTimeInformation();
        if (!((MIN_PERIODIC_TIME <= SoundPeriodicTimeInformation) &&
              (SoundPeriodicTimeInformation <= MAX_PERIODIC_TIME)))
        {
            qDebug() << "Unknown information periodic time is detected";
            m_ErrorMap.insert(EVENT_DM_INFO_SOUND_PERIODIC_TIME_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "");
            Global::EventObject::Instance().
                    RaiseEvent(EVENT_DM_INFO_SOUND_PERIODIC_TIME_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "", true);
            VerifiedData = false;
        }

        // check system setup
        // check the error tones for the sound
        int TemperatureParaffinBath =
                HiUserSettings.GetTemperatureParaffinBath();
        if (!((MIN_PARAFFIN_TEMP <= TemperatureParaffinBath) &&
              (TemperatureParaffinBath <= MAX_PARAFFIN_TEMP)))
        {
            qDebug() << "Unknown paraffin bath temperature is detected";
            m_ErrorMap.insert(EVENT_DM_PARAFFIN_TEMP_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "");
            Global::EventObject::Instance().
                    RaiseEvent(EVENT_DM_PARAFFIN_TEMP_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "", true);
            VerifiedData = false;
        }

        switch (HiUserSettings.GetModeRMSProcessing())
        {
        case Global::RMS_OFF:
        case Global::RMS_CASSETTES:
        case Global::RMS_CYCLES:
        case Global::RMS_DAYS:
            break;
        default:
        {
            qDebug() << "Unknown RMS mode for procesisng reagent is detected";
            m_ErrorMap.insert(EVENT_DM_RMS_PROCESSING_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "");
            Global::EventObject::Instance().
                    RaiseEvent(EVENT_DM_RMS_PROCESSING_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "", true);
            VerifiedData = false;
        }
            break;
        }

        switch (HiUserSettings.GetModeRMSCleaning())
        {
        case Global::RMS_OFF:
        case Global::RMS_CYCLES:
        case Global::RMS_DAYS:
            break;
        default:
        {
            qDebug() << "Unknown RMS mode for cleaning reagent is detected";
            m_ErrorMap.insert(EVENT_DM_RMS_CLEANING_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "");
            Global::EventObject::Instance().
                    RaiseEvent(EVENT_DM_RMS_CLEANING_OUT_OF_RANGE,
                               Global::tTranslatableStringList() << "", true);
            VerifiedData = false;
        }
            break;
        }
    }
    catch (Global::Exception &E) {
        Global::EventObject::Instance().RaiseEvent
                (E.GetErrorCode(), E.GetAdditionalData(),true);
        VerifiedData = false;
    }
    catch (...) {
        VerifiedData = false;
    }
    return VerifiedData;
}

} //End of namespace datamanager
