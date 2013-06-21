/****************************************************************************/
/*! \file UserSettingsCommandInterface.cpp
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
 *  \AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void CUserSettingsCommandInterface::SettingsUpdateHandler(Global::tRefType Ref, const MsgClasses::CmdChangeUserSettings &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    QByteArray SettingsData(const_cast<QByteArray &>(Cmd.GetUserSettings()));
    QDataStream SettingsDataStream(&SettingsData, QIODevice::ReadWrite);
    SettingsDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    CUserSettings Settings;
    SettingsDataStream >> Settings;

    bool Result = true;
    Result = mp_DataContainer->SettingsInterface->UpdateUserSettings(&Settings);
    if (!Result) {
        // If error occurs , get errors and send errors to GUI
        ListOfErrors_t &ErrorList = mp_DataContainer->SettingsInterface->GetErrorList();
        QString ErrorString;
        DataManager::Helper::ErrorIDToString(ErrorList, ErrorString);
        // If error occurs , get errors and send errors to GUI
        qDebug()<<"\n\n\nSettings Update Failed";
        mp_MasterThreadController->SendAcknowledgeNOK(Ref, AckCommandChannel, ErrorString, Global::GUIMSGTYPE_INFO);
    }
    else {
        mp_MasterThreadController->SendAcknowledgeOK(Ref, AckCommandChannel);
        SettingsDataStream.device()->reset();
        mp_MasterThreadController->BroadcastCommand(Global::CommandShPtr_t(new MsgClasses::CmdChangeUserSettings(5000, SettingsDataStream)));
        // Set volume values for UserSettings
        Global::AlarmHandler *p_AlarmHandler = mp_MasterThreadController->GetAlarmHandler();
        if (p_AlarmHandler) {
            p_AlarmHandler->setVolume(Global::ALARM_WARNING, Settings.GetSoundLevelWarning());
            p_AlarmHandler->setSoundNumber(Global::ALARM_WARNING, Settings.GetSoundNumberWarning());
            p_AlarmHandler->setVolume(Global::ALARM_ERROR, Settings.GetSoundLevelError());
            p_AlarmHandler->setSoundNumber(Global::ALARM_ERROR, Settings.GetSoundNumberError());
        }
        emit UserSettingsChanged();
        qDebug()<<"\n\n User Settings Update Success";
        mp_DataContainer->SettingsInterface->Write();
        if (mp_DataContainer->SettingsInterface->VerifyData(true)) {
            Global::EventObject::Instance().RaiseEvent(DataManager::EVENT_DM_GV_FAILED);
        }
    }
}
/****************************************************************************/
/**
 * \brief Function which handles CmdAlarmToneTest
 *  \iparam Ref = Command reference
 *  \iparam Cmd = Command
 *  \AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void CUserSettingsCommandInterface::AlarmTestToneHandler(Global::tRefType Ref, const MsgClasses::CmdAlarmToneTest &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    //CUserSettings Settings;
    bool Result;
    qDebug()<<"Test tone handler"<<endl;
    qDebug()<<"Alarm Test- Type:"<<Cmd.GetAlarmType() <<"Sound:"<< Cmd.GetSound()<<"Volume:" << Cmd.GetVolume();
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

}// End of Namespace DataManager
