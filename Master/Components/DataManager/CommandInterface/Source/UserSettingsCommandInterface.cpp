/****************************************************************************/
/*! \file Components/DataManager/CommandInterface/Source/UserSettingsCommandInterface.cpp
 *
 *  \brief UserSettings Command Interface  Implementation
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-30
 *  $Author:    $ N.Kamath
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

#include <QDebug>

namespace DataManager {
/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_DataManager = Datamanager object
 *  \iparam p_HimalayaMasterThreadController = Himalaya Master thread Cntrl
 *  \iparam p_DataContainer = DataContainer pointer
 */
/****************************************************************************/
CUserSettingsCommandInterface::CUserSettingsCommandInterface(CDataManagerBase *p_DataManager,
                                                             Threads::MasterThreadController *p_MasterThreadController,
                                                             CDataContainerCollectionBase *p_DataContainer):
    CCommandInterfaceBase(p_DataManager, p_MasterThreadController, p_DataContainer)
{
    RegisterCommands();
}

/****************************************************************************/
/**
 * \brief Register Commands related to Settings Container
 */
/****************************************************************************/
void CUserSettingsCommandInterface::RegisterCommands() {
    mp_MasterThreadController->RegisterCommandForProcessing<MsgClasses::CmdChangeUserSettings, DataManager::CUserSettingsCommandInterface>
            (&CUserSettingsCommandInterface::SettingsUpdateHandler, this);
    mp_MasterThreadController->RegisterCommandForProcessing<MsgClasses::CmdAlarmToneTest, DataManager::CUserSettingsCommandInterface>
            (&CUserSettingsCommandInterface::AlarmTestToneHandler, this);
}

/****************************************************************************/
/**
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
        CUserSettings Settings;
        SettingsDataStream >> Settings;

        // compare the previous settings with the present settings and log the events
        bool LanguageChanged = false;
        const CUserSettings TempSettings(*mp_DataContainer->SettingsInterface->GetUserSettings());

        bool Result = true;
        Result = mp_DataContainer->SettingsInterface->UpdateUserSettings(&Settings);
        if (!Result) {
            // If error occurs , get errors and send errors to GUI
            ListOfErrors_t &ErrorList = mp_DataContainer->SettingsInterface->GetErrorList();
            // For now we send only default message , since user cant exceed any of the constraints
            // in the verifier , from GUI.s
            while(!ErrorList.isEmpty()) {
                ErrorHash_t *p_ErrorHash = ErrorList.first();
                p_ErrorHash->clear();
                ErrorList.removeFirst();
            }
            SendNackToGUI(Ref,AckCommandChannel, ErrorList, EVENT_DM_SETTINGS_UPDATE_FAILED);
            return;
        }
        else {

            // Set volume values for UserSettings
            Global::AlarmHandler *p_AlarmHandler = mp_MasterThreadController->GetAlarmHandler();
            if (p_AlarmHandler) {
                p_AlarmHandler->setVolume(Global::ALARM_WARNING, Settings.GetSoundLevelWarning());
                p_AlarmHandler->setSoundNumber(Global::ALARM_WARNING, Settings.GetSoundNumberWarning());
                p_AlarmHandler->setVolume(Global::ALARM_ERROR, Settings.GetSoundLevelError());
                p_AlarmHandler->setSoundNumber(Global::ALARM_ERROR, Settings.GetSoundNumberError());
            }

            // raise the event if the language is changed
            if (TempSettings.GetLanguage() != Settings.GetLanguage()) {
                Global::EventObject::Instance().RaiseEvent(EVENT_GLOBAL_USER_ACTIVITY_US_LANGUAGE_CHANGED,
                                                           Global::FmtArgs() << QLocale::languageToString(Settings.GetLanguage()));
                LanguageChanged = true;
            }
            // raise the event if the network settings is changed
            if (TempSettings.GetProxyIPAddress() != Settings.GetProxyIPAddress() ||
                    TempSettings.GetProxyIPPort() != Settings.GetProxyIPPort() ||
                    TempSettings.GetProxyUserName() != Settings.GetProxyUserName() ||
                    TempSettings.GetProxyPassword() != Settings.GetProxyPassword() ||
                    TempSettings.GetRemoteCare() != Settings.GetRemoteCare() ||
                    TempSettings.GetDirectConnection() != Settings.GetDirectConnection()) {
                Global::EventObject::Instance().RaiseEvent(EVENT_GLOBAL_USER_ACTIVITY_US_NETWORK_SETTINGS_ADDED);
            }

            if (!mp_DataContainer->SettingsInterface->VerifyData(true)) {
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_FAILED);
                SendNackToGUIWithDefaultMsg(Ref, AckCommandChannel, EVENT_DM_SETTINGS_UPDATE_FAILED);
                return;
            }

            if (!mp_DataContainer->SettingsInterface->Write()) {
                SendNackToGUIWithDefaultMsg(Ref, AckCommandChannel, EVENT_DM_SETTINGS_UPDATE_FAILED);
                return;
            }
            emit UserSettingsChanged(LanguageChanged);

            SettingsDataStream.device()->reset();
            SendAckAndUpdateGUI(Ref, AckCommandChannel, Global::CommandShPtr_t(new MsgClasses::CmdChangeUserSettings(5000, SettingsDataStream)));
            qDebug()<<"\n\n User Settings Update Success";
        }
    }
    catch (Global::Exception &E) {
        Global::EventObject::Instance().RaiseEvent(E.GetErrorCode(),E.GetAdditionalData(),true);
        SendNackToGUIWithDefaultMsg(Ref, AckCommandChannel, EVENT_DM_SETTINGS_UPDATE_FAILED);
    }
    catch (...) {
        SendNackToGUIWithDefaultMsg(Ref, AckCommandChannel, EVENT_DM_SETTINGS_UPDATE_FAILED);
    }
}
/****************************************************************************/
/**
 * \brief Function which handles CmdAlarmToneTest
 *  \iparam Ref = Command reference
 *  \iparam Cmd = Command
 *  \iparam AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void CUserSettingsCommandInterface::AlarmTestToneHandler(Global::tRefType Ref, const MsgClasses::CmdAlarmToneTest &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    bool Result = false;
    qDebug()<<"Alarm Test- Type:"<<Cmd.GetAlarmType() <<"Sound:"<< Cmd.GetSound()<<"Volume:" << Cmd.GetVolume();
    if (mp_MasterThreadController) {
        mp_MasterThreadController->SendAcknowledgeOK(Ref, AckCommandChannel);
        Global::AlarmHandler *p_AlarmHandler = mp_MasterThreadController->GetAlarmHandler();
        if (p_AlarmHandler) {
            Result = p_AlarmHandler->playTestTone(Cmd.GetAlarmType(), Cmd.GetVolume(), Cmd.GetSound());
            qDebug()<<Result;
        }
        if (!Result) {
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_CANCEL_PLAY_ALARM_TEST_TONE);

        }
    }
}

}// End of Namespace DataManager
