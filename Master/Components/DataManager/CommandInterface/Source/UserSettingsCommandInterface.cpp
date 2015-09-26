/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/CommandInterface/Source/UserSettingsCommandInterface.cpp
 *
 *  \brief UserSettings Command Interface  Implementation
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-30
 *  $Author:    $ N.Kamath, Ramya GJ
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
#include "DataManager/CommandInterface/Include/UserSettingsCommandInterface.h"
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "NetCommands/Include/CmdRCNotifyReconnection.h"
#include "Global/Include/AlarmPlayer.h"
#include "RemoteCareManager/Include/RemoteCareManager.h"
#include <QDebug>

namespace DataManager {
/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_DataManager = Datamanager object
 *  \iparam p_MasterThreadController = Himalaya Master thread Cntrl
 *  \iparam p_DataContainer = DataContainer pointer
 */
/****************************************************************************/
CUserSettingsCommandInterface::CUserSettingsCommandInterface(CDataManagerBase *p_DataManager,
                                                             Threads::MasterThreadController *p_MasterThreadController,
                                                             CDataContainerCollectionBase *p_DataContainer):
    CCommandInterfaceBase(p_DataManager, p_MasterThreadController, p_DataContainer), m_ConnToOtherProcess(false)

{
    RegisterCommands();
    (void)connect(this, SIGNAL(PlayTestTone(bool, quint8, quint8)),
            &Global::AlarmPlayer::Instance(), SLOT(playTestTone(bool, quint8, quint8)));
}

/****************************************************************************/
/*!
 * \brief Register Commands related to Settings Container
 */
/****************************************************************************/
void CUserSettingsCommandInterface::RegisterCommands() {
    if(mp_MasterThreadController) {
        mp_MasterThreadController->RegisterCommandForProcessing<MsgClasses::CmdChangeUserSettings, DataManager::CUserSettingsCommandInterface>
                (&CUserSettingsCommandInterface::SettingsUpdateHandler, this);
        mp_MasterThreadController->RegisterCommandForProcessing<MsgClasses::CmdAlarmToneTest, DataManager::CUserSettingsCommandInterface>
                (&CUserSettingsCommandInterface::AlarmTestToneHandler, this);
    }
}

/****************************************************************************/
/*!
 * \brief Function which handles CmdChangeUserSettings
 *  \iparam Ref = Command reference
 *  \iparam Cmd = Command
 *  \iparam AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void CUserSettingsCommandInterface::SettingsUpdateHandler(Global::tRefType Ref, const MsgClasses::CmdChangeUserSettings &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    try {
        CHECKPTR(mp_MasterThreadController);
        CHECKPTR(mp_DataContainer);
        CHECKPTR(mp_DataContainer->SettingsInterface);
        QByteArray SettingsData(const_cast<QByteArray &>(Cmd.GetUserSettings()));
        QDataStream SettingsDataStream(&SettingsData, QIODevice::ReadWrite);
        SettingsDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        CHimalayaUserSettings Settings;
        SettingsDataStream >> Settings;
        bool ProxySettingsChanged = false;

        // compare the previous settings with the present settings and log the events
        bool LanguageChanged = false;
        const CHimalayaUserSettings TempSettings(*mp_DataContainer->SettingsInterface->GetUserSettings());

        bool Result = true;
        Result = mp_DataContainer->SettingsInterface->UpdateUserSettings(&Settings);
        if (!Result) {
            // If error occurs , get errors and send errors to GUI
            ListOfErrors_t &ErrorList = mp_DataContainer->SettingsInterface->GetErrorList();
            // For now we send only default message , since user cant exceed any of the constraints
            // in the verifier , from GUI.s
            while(!ErrorList.isEmpty()) {
                ErrorMap_t *p_ErrorMap = ErrorList.first();
                p_ErrorMap->clear();
                ErrorList.removeFirst();
            }
            SendNackToGUI(Ref,AckCommandChannel, ErrorList, EVENT_DM_SETTINGS_UPDATE_FAILED);
            return;
        }
        else {
            // Set volume values for UserSettings
            Global::AlarmPlayer::Instance().setVolume(Global::ALARM_WARNING, Settings.GetSoundLevelWarning());
            Global::AlarmPlayer::Instance().setSoundNumber(Global::ALARM_WARNING, Settings.GetSoundNumberWarning());
            Global::AlarmPlayer::Instance().setVolume(Global::ALARM_ERROR, Settings.GetSoundLevelError());
            Global::AlarmPlayer::Instance().setSoundNumber(Global::ALARM_ERROR, Settings.GetSoundNumberError());

            Global::AlarmHandler::Instance().setWarnPeriod(Settings.GetSoundPeriodicWarning());
            Global::AlarmHandler::Instance().setWarnPeriodInterval(Settings.GetSoundPeriodicTimeWarning());

            // raise the event if the language is changed
            if (TempSettings.GetLanguage() != Settings.GetLanguage()) {
                Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_US_LANGUAGE_CHANGED,
                                                           Global::FmtArgs() << QLocale::languageToString(Settings.GetLanguage()));
                LanguageChanged = true;
            }

            QString str = Settings.GetActiveCarbonLastResetDate();
            QDateTime lastResetDateTime = QDateTime::fromString(str);
            QDateTime currentDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime();
            int diff = lastResetDateTime.secsTo(currentDateTime);
            if (diff < 3600)
            {
                emit ResetActiveCarbonFilterLifeTime();
            }

            // raise the event if the network settings is changed
            if (TempSettings.GetProxyIPAddress() != Settings.GetProxyIPAddress() ||
                    TempSettings.GetProxyIPPort() != Settings.GetProxyIPPort() ||
                    TempSettings.GetProxyUserName() != Settings.GetProxyUserName() ||
                    TempSettings.GetProxyPassword() != Settings.GetProxyPassword() ||
                    TempSettings.GetRemoteCare() != Settings.GetRemoteCare() ||
                    TempSettings.GetDirectConnection() != Settings.GetDirectConnection()) {
                Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_US_NETWORK_SETTINGS_ADDED);
                ProxySettingsChanged = true;
            }

            if (TempSettings.GetRemoteCare() != Settings.GetRemoteCare())
                (const_cast<RemoteCare::RemoteCareManager*>(mp_MasterThreadController->GetRemoteCareManager()))->SetRemoteCareStatus(Settings.GetRemoteCare() == Global::ONOFFSTATE_ON ? true : false);

            // raise the event if the RMS state is changed
            if (TempSettings.GetValue("RMS_PROCESSINGMODE") != Settings.GetValue("RMS_PROCESSINGMODE")) {
                QString Value = Settings.GetValue("RMS_PROCESSINGMODE");
                quint32 RMSStringID = 0;
                if (Value.compare("DAYS", Qt::CaseInsensitive) == 0) {
                    Value = Global::UITranslator::TranslatorInstance().Translate
                            (Global::EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_DAYS);
                    RMSStringID = Global::EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_DAYS;
                }
                else if (Value.compare("CYCLES", Qt::CaseInsensitive) == 0) {
                    Value = Global::UITranslator::TranslatorInstance().Translate
                            (Global::EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_CYCLES);
                    RMSStringID = Global::EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_CYCLES;
                }
                else if (Value.compare("CASSETTES", Qt::CaseInsensitive) == 0) {
                    Value = Global::UITranslator::TranslatorInstance().Translate
                            (Global::EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_CASSETTES);
                    RMSStringID = Global::EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_CASSETTES;
                }
                else {
                    Value = Global::UITranslator::TranslatorInstance().Translate
                            (Global::EVENT_GLOBAL_USER_ACTIVITY_STATE_CHANGED_OFF);
                    RMSStringID = Global::EVENT_GLOBAL_USER_ACTIVITY_STATE_CHANGED_OFF;
                }
                Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_CHANGED,
                                                           //Global::FmtArgs() << Value);
                                                           Global::tTranslatableStringList()<< Global::TranslatableString(RMSStringID));
            }
            if (TempSettings.GetValue("RMS_CLEANINGMODE") != Settings.GetValue("RMS_CLEANINGMODE")) {
                QString Value = Settings.GetValue("RMS_CLEANINGMODE");
                quint32 RMSStringID = 0;
                if (Value.compare("DAYS", Qt::CaseInsensitive) == 0) {
                    Value = Global::UITranslator::TranslatorInstance().Translate
                            (Global::EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_DAYS);
                    RMSStringID = Global::EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_DAYS;
                }
                else if (Value.compare("CYCLES", Qt::CaseInsensitive) == 0) {
                    Value = Global::UITranslator::TranslatorInstance().Translate
                            (Global::EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_CYCLES);
                    RMSStringID = Global::EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_CYCLES;
                }
                else {
                    Value = Global::UITranslator::TranslatorInstance().Translate
                            (Global::EVENT_GLOBAL_USER_ACTIVITY_STATE_CHANGED_OFF);
                    RMSStringID = Global::EVENT_GLOBAL_USER_ACTIVITY_STATE_CHANGED_OFF;
                }
                Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_US_CLEANING_RMS_STATE_CHANGED,
                                                           //Global::FmtArgs() << Value);
                                                           Global::tTranslatableStringList()<< Global::TranslatableString(RMSStringID));
            }

            // raise the event if the temprature format is changed
            if (TempSettings.GetTemperatureFormat() != Settings.GetTemperatureFormat()) {
                Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_US_TEMPERATURE_FORMAT_CHANGED,
                                                           Global::FmtArgs() << Global::TemperatureFormatToString
                                                           (Settings.GetTemperatureFormat()));
            }

            // raise the event if the time format is changed
            if (TempSettings.GetTimeFormat() != Settings.GetTimeFormat()) {
                Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_US_TIME_FORMAT_CHANGED,
                                                           Global::FmtArgs() << Global::TimeFormatToString
                                                           (Settings.GetTimeFormat()) + "h");
            }

            // raise the event if the date format is changed
            if (TempSettings.GetDateFormat() != Settings.GetDateFormat()) {
                Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_US_DATE_FORMAT_CHANGED,
                                                           Global::FmtArgs() << Global::DateFormatToString
                                                           (Settings.GetDateFormat()));
            }

            // raise the event if the error sound note is changed
            if (TempSettings.GetSoundNumberError() != Settings.GetSoundNumberError()) {
                Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_US_ALARAM_ERROR_SOUND_NOTE_CHANGED);
            }

            // raise the event if the error volume tone is changed
            if (TempSettings.GetSoundLevelError() != Settings.GetSoundLevelError()) {
                Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_US_ALARAM_ERROR_VOLUME_NOTE_CHANGED);
            }

            // raise the event if the warning sound tone is changed
            if (TempSettings.GetSoundNumberWarning() != Settings.GetSoundNumberWarning()) {
                Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_US_ALARAM_WARN_SOUND_NOTE_CHANGED);
            }

            // raise the event if the warning volume tone is changed
            if (TempSettings.GetSoundLevelWarning() != Settings.GetSoundLevelWarning()) {
                Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_US_ALARAM_WARN_VOLUME_NOTE_CHANGED);
            }

            if (!mp_DataContainer->SettingsInterface->VerifyData(true)) {
                Global::EventObject::Instance().RaiseEvent(DataManager::EVENT_DM_GV_FAILED);
                SendNackToGUIWithDefaultMsg(Ref, AckCommandChannel, EVENT_DM_SETTINGS_UPDATE_FAILED);
                return;
            }

            if (!mp_DataContainer->SettingsInterface->Write()) {
                SendNackToGUIWithDefaultMsg(Ref, AckCommandChannel, EVENT_DM_SETTINGS_UPDATE_FAILED);
                return;
            }
            emit UserSettingsChanged(LanguageChanged);

            (void)SettingsDataStream.device()->reset();
            SendAckAndUpdateGUI(Ref, AckCommandChannel, Global::CommandShPtr_t(new MsgClasses::CmdChangeUserSettings(30000, SettingsDataStream)));
            // if the another process is availble other than colorado (other process means it is Sepia)
            if (m_ConnToOtherProcess) {
                //send usersettings changed command to sepia
                (void)mp_MasterThreadController->SendCommand(
                            Global::CommandShPtr_t(new MsgClasses::CmdChangeUserSettings(30000, SettingsDataStream)),
                            mp_MasterThreadController->GetCommandChannel(5));
            }

            //if the proxy settings has been changed and the data is valid, send the command to Remote Care
            if(ProxySettingsChanged) {
                (const_cast<RemoteCare::RemoteCareManager*>(mp_MasterThreadController->GetRemoteCareManager()))->SendNotifyReconnectToRemoteCare();
            }
        }

        return;
    }
    CATCHALL();

    SendNackToGUIWithDefaultMsg(Ref, AckCommandChannel, EVENT_DM_SETTINGS_UPDATE_FAILED);
}

/****************************************************************************/
/*!
 * \brief Function which handles CmdAlarmToneTest
 *  \iparam Ref = Command reference
 *  \iparam Cmd = Command
 *  \iparam AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void CUserSettingsCommandInterface::AlarmTestToneHandler(Global::tRefType Ref, const MsgClasses::CmdAlarmToneTest &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    if (mp_MasterThreadController) {
        mp_MasterThreadController->SendAcknowledgeOK(Ref, AckCommandChannel);
        emit PlayTestTone(Cmd.GetAlarmType(), Cmd.GetVolume(), Cmd.GetSound());
    }
}

/****************************************************************************/
/**
 * \brief This slot is used if any other process is connected means it is Sepia
 * \iparam Connected = true if sepia is connected
 */
/****************************************************************************/
void CUserSettingsCommandInterface::ConnectedToOtherProcess(bool Connected)
{
    m_ConnToOtherProcess = Connected;
}

}// End of Namespace DataManager
