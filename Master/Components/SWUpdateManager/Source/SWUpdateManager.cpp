/****************************************************************************/
/*! \file SWUpdateManager.cpp
 *
 *  \brief Implementation file for class SWUpdateManager
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-05-02
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

#include <stdlib.h> //For "system()"
#include <unistd.h> //For fsync()
#include <SWUpdateManager/Include/SWUpdateManager.h>
#include <Global/Include/Utils.h>
#include <ExternalProcessController/Include/ExternalProcess.h>
#include <QTimer>
#include <Global/Include/EventObject.h>
#include <EventHandler/Include/StateHandler.h>
#include <Threads/Include/MasterThreadController.h>
#include <NetCommands/Include/CmdSWUpdate.h>
#include <DataManager/Containers/SWVersions/Include/SWVersionList.h>
#include <NetCommands/Include/CmdExecutionStateChanged.h>

namespace SWUpdate {

const QString SW_UPDATE_STARTER_SCRIPT_NAME         = "MasterSWupdateStarter.sh"; //!< Update script
const QString SW_UPDATE_OPTION_CHECK                = "-check";  //!< SW update option
const QString SW_UPDATE_OPTION_UPDATEROLLBACK       = "-updateRollback"; //!< SW update option
const QString SW_UPDATE_OPTION_UPDATE               = "-update"; //!< SW update option
const QString SW_UPDATE_OPTION_CLEAN                = "-Clean"; //!< Clean tmp files
const qint32  SW_UPDATE_CHECK_SUCCESS               = 0; //!< SW return value for success

//! Software Update script shall not not hang on infinitely , if there is no response withing 60 seconds , we signal error
//! in SW update
const quint32 TIMEOUT_FOR_SW_UPDATE_STARTER = 100000; //!< 10 [s] .

/****************************************************************************/
/*!
 *  \brief    Constructor
 *  \iparam     MasterThreadRef
 */
/****************************************************************************/
SWUpdateManager::SWUpdateManager(Threads::MasterThreadController &MasterThreadRef)
    :mp_SWUpdateStarter(NULL)
    ,m_ScriptExited(true)
    ,m_MasterThreadControllerRef(MasterThreadRef)
{
    this->setParent(&m_MasterThreadControllerRef);
    m_MasterThreadControllerRef.RegisterCommandForProcessing<NetCommands::CmdSWUpdate, SWUpdateManager>
            (&SWUpdateManager::SWUpdateHandler, this);
    CONNECTSIGNALSLOT(&m_SWUpdateCheckTimer, timeout(), this, OnNoResponseFromSWUpdate());
    m_SWUpdateCheckTimer.setSingleShot(true);
}

/****************************************************************************/
/*!
 *  \brief    Destructor
 */
/****************************************************************************/
SWUpdateManager::~SWUpdateManager()
{

}

/****************************************************************************/
/*!
 *  \brief   Interface for updating software
 *  \iparam Option = "-check" or "-updateRollback"
 *  \iparam UpdateFrom = "USB" or "RemoteCare"
 */
/****************************************************************************/
void SWUpdateManager::UpdateSoftware(const QString &Option, const QString &UpdateFrom)
{
    try {
        if (m_ScriptExited) { // Make sure that sw update script is not running already.
            m_ScriptExited = false;
            m_UpdateOption = Option;
            m_SWUpdateCheckTimer.start(TIMEOUT_FOR_SW_UPDATE_STARTER);
            mp_SWUpdateStarter = new ExternalProcessControl::ExternalProcess("SWUpdate", this);
            mp_SWUpdateStarter->Initialize();
            // connect ProcessManager's start/exit/error signals:
            CONNECTSIGNALSLOT(mp_SWUpdateStarter, ProcessExited(const QString &, int), this, SWUpdateProcessExited(const QString &, int));
            CONNECTSIGNALSLOT(mp_SWUpdateStarter, ProcessStarted(const QString &), this, SWUpdateStarted(const QString &));
            CONNECTSIGNALSLOT(mp_SWUpdateStarter, ProcessError(int), this, SWUpdateError(int));
            if (Option == SW_UPDATE_OPTION_CHECK) {
                emit SWUpdateStatus(true);
                //Display wait dialog in GUI
                emit WaitDialog(true, Global::SOFTWARE_UPDATE_TEXT);
                #if defined(__arm__)
                //On Target we execute as root. sudo is not needed
                mp_SWUpdateStarter->StartProcess(QString("./%1 %2 %3 %4").arg(SW_UPDATE_STARTER_SCRIPT_NAME).
                                                 arg(Option).
                                                 arg(UpdateFrom).
                                                 arg(QString::number(EVENT_GROUP_PLATFORM_SW_UPDATE)));
                                                 //arg("ST8200"));
                #else
                //Need "sudo" on Desktop
                mp_SWUpdateStarter->StartProcess(QString("sudo ./%1 %2 %3 %4").arg(SW_UPDATE_STARTER_SCRIPT_NAME).
                                                arg(Option).
                                                arg(UpdateFrom).
                                                arg(QString::number(EVENT_GROUP_PLATFORM_SW_UPDATE)));
                                                // arg("ST8200"));
                #endif
            }
            else if (Option == SW_UPDATE_OPTION_UPDATEROLLBACK || Option == SW_UPDATE_OPTION_CLEAN) {
                #if defined(__arm__)
                mp_SWUpdateStarter->StartProcess(QString("./%1 %2 %3").arg(SW_UPDATE_STARTER_SCRIPT_NAME).
                                                arg(Option).
                                                arg(QString::number(EVENT_GROUP_PLATFORM_SW_UPDATE)));
                #else
                mp_SWUpdateStarter->StartProcess(QString("sudo ./%1 %2 %3").arg(SW_UPDATE_STARTER_SCRIPT_NAME).
                                                arg(Option).
                                                arg(QString::number(EVENT_GROUP_PLATFORM_SW_UPDATE)));
                #endif
            }
            else {
                emit SWUpdateStatus(false);
            }
        }
        else {
            Global::EventObject::Instance().RaiseEvent(EVENT_SW_UPDATE_FAILED);
            emit SWUpdateStatus(false);
        }

        return;
    }
    CATCHALL();

    emit WaitDialog(false, Global::SOFTWARE_UPDATE_TEXT);
    Global::EventObject::Instance().RaiseEvent(EVENT_SW_UPDATE_FAILED);
    emit SWUpdateStatus(false);
}

/****************************************************************************/
/*!
 *  \brief  Halts software update script on power fail & updates Reboot.txt
 *
 */
/****************************************************************************/
void SWUpdateManager::PowerFailed()
{
    if (m_ScriptExited == false) { //this means sw update is in progress
        mp_SWUpdateStarter->KillProcess();
        mp_SWUpdateStarter->deleteLater();
        m_ScriptExited = true;
        emit WaitDialog(false, Global::SOFTWARE_UPDATE_TEXT);
        emit SWUpdateStatus(false);
        UpdateRebootFile("NA", "Failure", "No");
    }
}

/****************************************************************************/
/*!
 *  \brief  Slot called when SWUpdate Script exits.Handles both error &
 *          normal exit cases
 *  \iparam ScriptName = name of the S/W update script
 *  \iparam ExitCode   = Exit code from the script
 */
/****************************************************************************/
void SWUpdateManager::SWUpdateProcessExited(const QString &ScriptName, int ExitCode)
{
    //Once script has exited we no longer need the external process , hence deleted.
    qDebug() << "\n SWUpdateManager: ExternalProcessExited called.\n" << ScriptName << ExitCode;
    m_SWUpdateCheckTimer.stop();
    mp_SWUpdateStarter->blockSignals(true);
    mp_SWUpdateStarter->deleteLater();
    mp_SWUpdateStarter = NULL;
    m_ScriptExited = true;
    if (m_UpdateOption == SW_UPDATE_OPTION_CHECK ) {
        if (ExitCode == SW_UPDATE_CHECK_SUCCESS) {
            emit WaitDialog(false, Global::SOFTWARE_UPDATE_TEXT);
            //Since check is success, we can start update now.
            #if defined(__arm__)
            system(QString("./%1 %2 %3 &").arg(SW_UPDATE_STARTER_SCRIPT_NAME).
                                             arg(SW_UPDATE_OPTION_UPDATE).
                                             arg(QString::number(EVENT_GROUP_PLATFORM_SW_UPDATE)).toLocal8Bit().data());
            #else
            system(QString("sudo ./%1 %2 %3 &").arg(SW_UPDATE_STARTER_SCRIPT_NAME).
                                             arg(SW_UPDATE_OPTION_UPDATE).
                                             arg(QString::number(EVENT_GROUP_PLATFORM_SW_UPDATE)).toLocal8Bit().data());
            #endif
            m_MasterThreadControllerRef.InitiateShutdown();
        }
        else {
            //Inform user
            emit WaitDialog(false, Global::SOFTWARE_UPDATE_TEXT);
            emit SWUpdateStatus(false);
            Global::EventObject::Instance().RaiseEvent(EVENT_SW_UPDATE_FAILED);
            m_MasterThreadControllerRef.SetSWUpdateStatus(false);
        }
    }
    else if (m_UpdateOption == SW_UPDATE_OPTION_UPDATEROLLBACK) {
        if (ExitCode != SW_UPDATE_CHECK_SUCCESS) {
            //Inform User
            Global::EventObject::Instance().RaiseEvent(EVENT_SW_UPDATE_FAILED);
            EventHandler::StateHandler::Instance().setInitStageProgress(1, false); // Initialization failed
            m_MasterThreadControllerRef.SetSWUpdateStatus(false);
            UpdateRebootFile("Yes");
            m_MasterThreadControllerRef.m_UpdatingRollback = false;
        }
        else {
            //Inform user that software update is success
            Global::EventObject::Instance().RaiseEvent(EVENT_SW_UPDATE_SUCCESS);
            qDebug() <<"\n\n  SW UPDATE SUCCESS \n\n";
            m_MasterThreadControllerRef.SetSWUpdateStatus(true);
            UpdateRebootFile("No");
            m_MasterThreadControllerRef.m_UpdatingRollback = false;
            emit RollBackComplete();
        }
    }
    //we don't check for "-update" because  Main S/W is shutdown after update is started.
}

/****************************************************************************/
/*!
 *  \brief  Slot called when SWUpdate Script exits with error
 *  \iparam ErrorCode   = Exit code from the script
 */
/****************************************************************************/
void SWUpdateManager::SWUpdateError(int ErrorCode)
{
    qDebug()<<"\n SWUpdate Process Error" << ErrorCode;
    m_ScriptExited = true;
    m_SWUpdateCheckTimer.stop();
    mp_SWUpdateStarter->blockSignals(true);
    mp_SWUpdateStarter->deleteLater();
    mp_SWUpdateStarter = NULL;
    //We don't log much details since SW update script has its own logging
    //mechanism and details are logged by it.
    Global::EventObject::Instance().RaiseEvent(EVENT_SW_UPDATE_FAILED); //Informs user
    m_MasterThreadControllerRef.SetSWUpdateStatus(false);
    m_MasterThreadControllerRef.m_UpdatingRollback = false;
    emit WaitDialog(false, Global::SOFTWARE_UPDATE_TEXT);
    emit SWUpdateStatus(false);
}

/****************************************************************************/
/*!
 *  \brief  Slot called when SW update script is started. (Just for debugging)
 *  \iparam Name = Script name
 */
/****************************************************************************/
void SWUpdateManager::SWUpdateStarted(const QString &Name)
{
    qDebug() << "\n SWUpdateManager Started:" << Name << "\n\n";
}

/****************************************************************************/
/*!
 *  \brief  Slot called when SWUpdate Script doesn't repond (Hang)
 */
/****************************************************************************/
void SWUpdateManager::OnNoResponseFromSWUpdate()
{
    mp_SWUpdateStarter->blockSignals(true);
    mp_SWUpdateStarter->deleteLater();
    mp_SWUpdateStarter = NULL;
    m_ScriptExited = true;
    Global::EventObject::Instance().RaiseEvent(EVENT_SW_UPDATE_FAILED);
    m_MasterThreadControllerRef.SetSWUpdateStatus(false);
    m_MasterThreadControllerRef.m_UpdatingRollback = false;
    emit WaitDialog(false, Global::SOFTWARE_UPDATE_TEXT);
    emit SWUpdateStatus(false);
}

/****************************************************************************/
/*!
 *  \brief   Function to handle SW Update
 *  \iparam Ref = Command reference
 *  \iparam Cmd = Command
 *  \iparam AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void SWUpdateManager::SWUpdateHandler(Global::tRefType Ref, const NetCommands::CmdSWUpdate &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);
    UpdateSoftware(SW_UPDATE_OPTION_CHECK, Cmd.m_USBUpdate ? "USB" : "RemoteCare");
}

/****************************************************************************/
/*!
 *  \brief  Sets SW  Update parameters in Reboot.txt
 *  \iparam Rollback = "Yes" or "No"
 *  \iparam UpdateStatus = "NA" or "Success" or "Failure"
 *  \iparam CheckStatus = "Success" or "Failure" or "NA"
 */
/****************************************************************************/
void SWUpdateManager::UpdateRebootFile(const QString UpdateStatus, const QString CheckStatus, const QString Rollback) {
    QString RebootPath = Global::SystemPaths::Instance().GetSettingsPath() + "/Reboot.txt";
    QFile RebootFile(RebootPath);
    if(!RebootFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
        //!< todo raise event.
        qDebug()<<"Reboot file open failed";
    }
    m_MasterThreadControllerRef.m_RebootFileContent.insert("Update_RollBack_Failed", Rollback);
    m_MasterThreadControllerRef.m_RebootFileContent.insert("Software_Update_Status", UpdateStatus);
    m_MasterThreadControllerRef.m_RebootFileContent.insert("Software_Update_Check_Status", CheckStatus);
    QTextStream RebootFileStream(&RebootFile);
    RebootFileStream.setFieldAlignment(QTextStream::AlignLeft);
    QMapIterator<QString, QString> RebootfileItr(m_MasterThreadControllerRef.m_RebootFileContent);
    while (RebootfileItr.hasNext()) {
        RebootfileItr.next();
        QString Key = RebootfileItr.key();
        QString Value = m_MasterThreadControllerRef.m_RebootFileContent.value(Key);
        RebootFileStream << Key << ":" << Value << "\n" << left;
    }
    RebootFile.close();
    (void)fsync(RebootFile.handle());
}

/****************************************************************************/
/*!
 *  \brief  Slot is called when new SW is downloaded from RCA
 */
/****************************************************************************/
void SWUpdateManager::OnSWUpdateFromRC()
{
    UpdateSoftware(SW_UPDATE_OPTION_CHECK, "RemoteCare");
}

} //End of namespace SWUpdate
