/****************************************************************************/
/*! \file GUIManager.cpp
 *
 *  \brief Implementation file for class GUIManager.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2013-11-07
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

#include "GUIManager/Include/GUIManager.h"
#include <Global/Include/SystemPaths.h>
#include <Global/Include/Utils.h>
#include <Global/Include/Commands/AckOKNOK.h>
#include <GUIManager/Include/GUIManagerEventCodes.h>

namespace GUIManager {

/****************************************************************************/
/**
 * \brief Constructor.
 *
 * \iparam   MasterThreadControllerRef
 * \iparam  LanguageFileName
 */
/****************************************************************************/
CGUIManager::CGUIManager(Threads::MasterThreadController &MasterThreadControllerRef,                       
                       QString LanguageFileName)
    : m_MasterThreadControllerRef(MasterThreadControllerRef),
      mp_DeviceConfigInterface(NULL),
      mp_SettingsInterface(NULL),
      m_LanguageFileName(LanguageFileName),
      m_CommandChannelGui(&MasterThreadControllerRef, "Gui", Global::EVENTSOURCE_NONE)

{
}

/****************************************************************************/
void CGUIManager::Initialize()
{
    RegisterCommonCommands();
    try {
        try {
            // read password file
            DataManager::XmlConfigFilePasswords PwdFile(m_MasterThreadControllerRef.GetSerialNumber());
            PwdFile.ReadPasswords(Global::SystemPaths::Instance().GetSettingsPath() + "/Password.xml", m_PasswordManager);
        }
        CATCHALL_RETHROW()
    }
    catch(...) {
        Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORD_FILE_CORRUPTED);
    }
}

/****************************************************************************/
void CGUIManager::RegisterCommonCommands()
{
    // so far the only registration with real function:
    m_MasterThreadControllerRef.RegisterCommandForProcessing<Global::CmdDateAndTime, CGUIManager>(&CGUIManager::SetDateTime, this);

//    m_MasterThreadControllerRef.RegisterCommandForProcessing<NetCommands::CmdExportDayRunLogReply, GuiManager>
//            (&GUIManager::ExportDayRunLogHandler, this);

    //User level functions
    m_MasterThreadControllerRef.RegisterCommandForProcessing<NetCommands::CmdChangeAdminPassword, CGUIManager>
            (&CGUIManager::ChangeAdminPasswordHandler, this);
    m_MasterThreadControllerRef.RegisterCommandForProcessing<NetCommands::CmdChangeUserLevel, CGUIManager>
            (&CGUIManager::ChangeUserLevelHandler, this);

    // to receive gui events
    m_MasterThreadControllerRef.RegisterCommandForProcessing<NetCommands::CmdGuiEventReport, CGUIManager>
            (&CGUIManager::GuiEventReportHandler, this);

    //Service SW
    m_MasterThreadControllerRef.RegisterCommandForProcessing<NetCommands::CmdStartServiceSW, CGUIManager>
            (&CGUIManager::CmdStartServiceSWHandler, this);

}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
CGUIManager::~CGUIManager() {
    /// Don't delete any of the ponters or reference arguments
    /// It will be automatically freed by Main class
}


/****************************************************************************/
bool CGUIManager::SendLanguageFileToGUI(QString FileName) {

    QString TranslationPath = Global::SystemPaths::Instance().GetTranslationsPath()
            + QDir::separator() + FileName;
    QFile File(TranslationPath);
    // check the existence of the file
    if (File.exists()) {
        // open the file
        if (File.open(QIODevice::ReadWrite)) {
            if (mp_SettingsInterface) {
                const DataManager::CUserSettings *p_Settings = mp_SettingsInterface->GetUserSettings();
                if (p_Settings) {
                    QLocale::Language CurrentLanguage =  p_Settings->GetLanguage();
                    QDataStream LangDataStream(&File);
                    LangDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
                    // send the command to GUI
                    (void)m_MasterThreadControllerRef.
                            SendCommand(Global::CommandShPtr_t(new NetCommands::CmdLanguageFile
                                                               (GUI_COMMANDS_TIMEOUT, LangDataStream, CurrentLanguage)),
                                        m_CommandChannelGui);
                }
            }

            // change the UI translator default language also
            // get locale extracted by filename
            QString Locale;
            Locale = FileName;                  // For e.g. "Colorado_de.qm"
            Locale.truncate(Locale.lastIndexOf('.'));   // For e.g. "Colorado_de"
            (void)Locale.remove(0, Locale.indexOf('_') + 1);   // "de"
            Global::UITranslator::TranslatorInstance().SetDefaultLanguage(QLocale(Locale).language());

        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
    return true;
}

/****************************************************************************/
bool CGUIManager::UpdateSupportedGUILanguages() {

    QDir TheDir(Global::SystemPaths::Instance().GetTranslationsPath());
    QStringList FileNames = TheDir.entryList(QStringList(m_LanguageFileName));
    if (mp_DeviceConfigInterface) {
        DataManager::CDeviceConfiguration *p_DeviceConfiguration = mp_DeviceConfigInterface->GetDeviceConfiguration();
        if (p_DeviceConfiguration) {
            QStringList LanguageList;
            for (int i = 0; i < FileNames.size(); ++i)
            {
                // get locale extracted by filename
                QString Locale;
                Locale = FileNames[i];                  // For e.g. "Colorado_de.qm"
                Locale.truncate(Locale.lastIndexOf('.'));   // For e.g. "Colorado_de"
                (void)Locale.remove(0, Locale.indexOf('_') + 1);   // "de"
                LanguageList << Global::LanguageToString(QLocale(Locale).language());
            }
            p_DeviceConfiguration->SetLanguageList(LanguageList);
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}

/************************************************************************************/
/*!
 *  \brief This slot is called to make GUI to stabilize and shoots a timer
 *           to send Configuration files to GUI
 */
/************************************************************************************/
void CGUIManager::WaitForGUIToStabilize()
{
    QTimer::singleShot(CONFIG_FILE_TIMER_INTERVAL, this, SLOT(InitiateSendingConfigurationFiles()));
}

/************************************************************************************/
/*!
 *  \brief This slot is called to initiate the sending configuration files
 */
/************************************************************************************/
void CGUIManager::InitiateSendingConfigurationFiles()
{
    emit StartSendingConfigurationFiles();
}

/****************************************************************************/
void CGUIManager::GuiEventReportHandler(Global::tRefType Ref,
                                       const NetCommands::CmdGuiEventReport &Cmd,
                                       Threads::CommandChannel &AckCommandChannel) {
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);
    NetCommands::GuiEventReportStruct GuiEventData;
    QByteArray EventData(const_cast<QByteArray &>(Cmd.GetEventData()));
    QDataStream EventDataStream(&EventData, QIODevice::ReadWrite);
    EventDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    // stream the data
    EventDataStream >> GuiEventData.EventCode;
    EventDataStream >> GuiEventData.EventStringList;
    // stream the alternative string usage
    qint32 AltStringUsage;
    EventDataStream >> AltStringUsage;
    GuiEventData.AltStringUsage = (Global::AlternateEventStringUsage) AltStringUsage;
    // check the string list count
    if (GuiEventData.EventStringList.count() > 0) {
        Global::tTranslatableStringList TrStringList;
        // update the translatable string list
        for (qint32 Counter = 0; Counter < GuiEventData.EventStringList.count();
             Counter++) {
            TrStringList << GuiEventData.EventStringList.value(Counter);
        }
        // raise exception
        Global::EventObject::Instance().RaiseEvent(GuiEventData.EventCode, TrStringList, true, GuiEventData.AltStringUsage);
        return;
    }

    if (GuiEventData.AltStringUsage != Global::NOT_APPLICABLE) {
        // raise exception
        Global::EventObject::Instance().RaiseEvent(GuiEventData.EventCode, GuiEventData.AltStringUsage);
        return;
    }

    Global::EventObject::Instance().RaiseEvent(GuiEventData.EventCode);

}
/****************************************************************************/
void CGUIManager::SetDateTime(Global::tRefType Ref, const Global::CmdDateAndTime &Cmd,
                             Threads::CommandChannel &AckCommandChannel) {

    if(!m_MasterThreadControllerRef.SetAdjustedDateTimeOffset(Cmd.GetDateTime())) {

        // remove the offset seconds from the Adjusted time
        QString DateTime = Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs
                (-Global::AdjustedTime::Instance().GetOffsetSeconds()).toString();
        // raise the event
        Global::EventObject::Instance().RaiseEvent(EVENT_DATE_TIME_CANNOT_BE_MORE_THAN_24_HOURS,
                                                   Global::FmtArgs() << DateTime, true);

        QString TranslatedString = Global::UITranslator::TranslatorInstance().Translate
                (Global::TranslatableString(EVENT_DATE_TIME_CANNOT_BE_MORE_THAN_24_HOURS,
                                            Global::tTranslatableStringList()
                                            << DateTime));
        // send NACK
        m_MasterThreadControllerRef.SendAcknowledgeNOK(Ref, AckCommandChannel, TranslatedString,
                                                               Global::GUIMSGTYPE_ERROR);
    }
    else {

        // log the activity - DateTime
        Global::EventObject::Instance().
                RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_US_DATE_TIME_CHANGED,
                           Global::FmtArgs() << Cmd.GetDateTime().toString());
        // send ACK
        m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);
        /// this signal tells the dependent components to update the time
        /// For example for colorado it updates the RMS
        emit TimeUpdated();
    }

}


/****************************************************************************/
void CGUIManager::ChangeAdminPasswordHandler(Global::tRefType Ref,
                                            const NetCommands::CmdChangeAdminPassword &Cmd,
                                            Threads::CommandChannel &AckCommandChannel) {

    QString CommandType("Failed");
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);
    if (Cmd.GetCommandType().compare("Old") == 0) {
        if (m_PasswordManager.CheckPassword("Administrator", Cmd.GetPassword())) {
            CommandType = "New";
        }
        else {
            Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORD_INCORRECT);
        }
    }
    else if (Cmd.GetCommandType().compare("New") == 0) {
        // save the password value
        m_PasswordManager.SetNewPassword(Cmd.GetPassword());
        CommandType = "Confirm";
    }
    else if(Cmd.GetCommandType().compare("Confirm") == 0) {
        // compare the old pass word with new password
        if (m_PasswordManager.GetNewPassword().compare(Cmd.GetPassword()) == 0) {
            // set the new password
            m_PasswordManager.SetPassword("Administrator", m_PasswordManager.GetNewPassword());
            // success
            CommandType = "Success";
            Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORD_CHANGED_SUCCESSFULLY);
            // write the changed password in the xml file
            DataManager::XmlConfigFilePasswords WritePasswordFile(m_MasterThreadControllerRef.GetSerialNumber());
            WritePasswordFile.WritePasswords(Global::SystemPaths::Instance().GetSettingsPath() + "/Password.xml",
                                             m_PasswordManager,
                                             m_MasterThreadControllerRef.GetSerialNumber());
        }
        else {
            Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORD_NOT_MATCHING);
            CommandType = "New";
        }
    }

    (void)m_MasterThreadControllerRef.
            SendCommand(Global::CommandShPtr_t(new NetCommands::CmdChangeAdminPasswordReply(GUI_COMMANDS_TIMEOUT, CommandType)),
                        m_CommandChannelGui);

}


/****************************************************************************/
void CGUIManager::ChangeUserLevelHandler(Global::tRefType Ref,
                                        const NetCommands::CmdChangeUserLevel &Cmd,
                                        Threads::CommandChannel &AckCommandChannel) {

    Global::GuiUserLevel AuthenticatedLevel = Global::OPERATOR;
    QString DeviceName("");
    switch(Cmd.GetUserLevel()) {
    case Global::ADMIN:
        if (m_PasswordManager.CheckPassword("Administrator", Cmd.GetPassword())) {
            AuthenticatedLevel = Global::ADMIN;
            Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_ADMIN_LOGIN);
        }
        break;
    case Global::SERVICE:

        if (mp_DeviceConfigInterface != NULL) {

            DataManager::CDeviceConfiguration *p_DeviceConfiguration = mp_DeviceConfigInterface->GetDeviceConfiguration(false);

            if (p_DeviceConfiguration != NULL) {
                DeviceName = p_DeviceConfiguration->GetValue("DeviceName");
            }
        }

        if (m_PasswordManager.ServiceAuthentication(Cmd.GetPassword(), DeviceName)) {
            AuthenticatedLevel = Global::SERVICE;
        }
        break;
    case Global::OPERATOR:
        /// Don't do anything. This won't happen. To avoid compiler warning it is added
        break;
    }

    // send ack to GUI
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);

    // check whether user level is operator, if it is operator then authentication failed
    if (AuthenticatedLevel == Global::OPERATOR && Cmd.GetUserLevel() == Global::ADMIN) {
        Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORD_INCORRECT);
    }
    // send the authenticated command to GUI
    (void)m_MasterThreadControllerRef.SendCommand(Global::CommandShPtr_t(new NetCommands::CmdChangeUserLevelReply
                                                                         (GUI_COMMANDS_TIMEOUT, AuthenticatedLevel)), m_CommandChannelGui);
    // check the whether fallback password is validated for the successful login
    if (m_PasswordManager.GetFallbackPasswordFlag()) {
        // User entered the fallback password so ask him to change the password
        (void)m_MasterThreadControllerRef.SendCommand(Global::CommandShPtr_t(new NetCommands::CmdChangeAdminPasswordReply
                                                                             (GUI_COMMANDS_TIMEOUT, "New")), m_CommandChannelGui);
    }
}

/****************************************************************************/
/**
 * \brief Informs GUI to display/close wait dialog
 *
 * \iparam Display - true indicate display dialog, false indicates close dialog
 * \iparam WaitDialogText = Wait dialog text
 */
/****************************************************************************/
void CGUIManager::ShowWaitDialog(bool Display, Global::WaitDialogText_t WaitDialogText) {
    NetCommands::CmdExecutionStateChanged *p_Command = new NetCommands::CmdExecutionStateChanged(GUI_COMMANDS_TIMEOUT);
    p_Command->m_Stop = false;
    p_Command->m_WaitDialogFlag = Display;
    p_Command->m_WaitDialogText = WaitDialogText;
    (void)m_MasterThreadControllerRef.SendCommand(Global::CommandShPtr_t(p_Command),
                                                    m_CommandChannelGui);
}

/****************************************************************************/
/**
 * \brief Informs GUI to display/close wait dialog. Used when GUI is connected
 *        to Main Software
 */
/****************************************************************************/
void CGUIManager::ShowWaitDialog()
{
    ShowWaitDialog(true, Global::INITIALIZING_TEXT);
}

/****************************************************************************/
/**
 * \brief Hides wait dialog
 */
/****************************************************************************/
void CGUIManager::HideWaitDialog()
{
    ShowWaitDialog(false, Global::INITIALIZING_TEXT);
}

/****************************************************************************/
void CGUIManager::CmdStartServiceSWHandler(Global::tRefType Ref, const NetCommands::CmdStartServiceSW &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    Q_UNUSED(Cmd)
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);
    if (QFile::exists(qApp->applicationDirPath() + "/StartService.sh") && QFile::exists(qApp->applicationDirPath() + "/colorado_service")) {
        (void)system("./StartService.sh Service &");
        m_MasterThreadControllerRef.InitiateShutdown();
    }
    else {
        Global::EventObject::Instance().RaiseEvent(EVENT_ERROR_CANNOT_START_SERVICE);
    }
}

}
