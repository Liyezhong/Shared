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
const QString SW_UPDATE_OPTION_CLEAN                = "-clean"; //!< Clean tmp files
const qint32  SW_UPDATE_CHECK_SUCCESS               = 0; //!< SW return value for success

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
                mp_SWUpdateStarter->StartProcess(QString("./%1 %2 %3 %4 %5").arg(SW_UPDATE_STARTER_SCRIPT_NAME).
                                                 arg(Option).
                                                 arg(UpdateFrom).
                                                 arg(QString::number(EVENT_GROUP_PLATFORM_SW_UPDATE)).
                                                 arg(m_MasterThreadControllerRef.m_InstrumentName.simplified().remove(" ")));
                #else
                //Need "sudo" on Desktop
                mp_SWUpdateStarter->StartProcess(QString("sudo ./%1 %2 %3 %4 %5").arg(SW_UPDATE_STARTER_SCRIPT_NAME).
                                                arg(Option).
                                                arg(UpdateFrom).
                                                arg(QString::number(EVENT_GROUP_PLATFORM_SW_UPDATE)).
                                                 arg(m_MasterThreadControllerRef.m_InstrumentName.simplified().remove(" ")));
                #endif
            }
            else if (Option == SW_UPDATE_OPTION_UPDATEROLLBACK || Option == SW_UPDATE_OPTION_CLEAN) {
                #if defined(__arm__)
                mp_SWUpdateStarter->StartProcess(QString("./%1 %2 %3").arg(SW_UPDATE_STARTER_SCRIPT_NAME).
                                                arg(Option).
                                                arg(QString::number(EVENT_GROUP_PLATFORM_SW_UPDATE)));
                mp_SWUpdateStarter->WaitForFinished();
                #else
                mp_SWUpdateStarter->StartProcess(QString("sudo ./%1 %2 %3").arg(SW_UPDATE_STARTER_SCRIPT_NAME).
                                                arg(Option).
                                                arg(QString::number(EVENT_GROUP_PLATFORM_SW_UPDATE)));
                mp_SWUpdateStarter->WaitForFinished();
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
 *  \brief  Halts software update script on power fail & updates BootConfig.txt
 *
 */
/****************************************************************************/
void SWUpdateManager::PowerFailed()
{
    if (m_ScriptExited == false) { //this means sw update is in progress
        mp_SWUpdateStarter->KillProcess();
        mp_SWUpdateStarter->deleteLater();
        // lint -esym(423,SWUpdate::SWUpdateManager::mp_SWUpdateStarter)
        mp_SWUpdateStarter = NULL;
        m_ScriptExited = true;
        emit WaitDialog(false, Global::SOFTWARE_UPDATE_TEXT);
        emit SWUpdateStatus(false);
        UpdateRebootFile("NA", "NA");
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

    if (m_UpdateOption == SW_UPDATE_OPTION_CHECK ) {
        if (ExitCode == SW_UPDATE_CHECK_SUCCESS) {
            //Since check is success, we can start update now.
            UpdateRebootFile("NA", "SW_Update");
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
            if (m_UpdateOption == SW_UPDATE_OPTION_UPDATEROLLBACK) {
                //We don't log much details since SW update script has its own logging
                //mechanism and details are logged by it.
                Global::EventObject::Instance().RaiseEvent(EVENT_SW_UPDATE_ROLLBACK_UPDATE_FAILED);
            }
            else {
                Global::EventObject::Instance().RaiseEvent(EVENT_SW_UPDATE_STARTER_FAILED);
            }
            EventHandler::StateHandler::Instance().setInitStageProgress(1, false); // Initialization failed
            m_MasterThreadControllerRef.SetSWUpdateStatus(false);
            m_MasterThreadControllerRef.m_UpdatingRollback = false;
        }
        else {
            //Inform user that software update is success
            Global::EventObject::Instance().RaiseEvent(EVENT_SW_UPDATE_SUCCESS);
            m_MasterThreadControllerRef.SetSWUpdateStatus(true);
            UpdateRebootFile("NA", "NA");
            m_MasterThreadControllerRef.m_UpdatingRollback = false;
            emit RollBackComplete();
        }
    }
    mp_SWUpdateStarter->blockSignals(true);
    mp_SWUpdateStarter->deleteLater();
    // lint -esym(423,SWUpdate::SWUpdateManager::mp_SWUpdateStarter)
    mp_SWUpdateStarter = NULL;
    m_ScriptExited = true;
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
    QString ProcessName = mp_SWUpdateStarter->GetProcessName();
    if (ProcessName.contains(QString("%1").arg(SW_UPDATE_OPTION_UPDATEROLLBACK))) {
        //We don't log much details since SW update script has its own logging
        //mechanism and details are logged by it.
        Global::EventObject::Instance().RaiseEvent(EVENT_SW_UPDATE_ROLLBACK_UPDATE_FAILED);
    }
    else {
        Global::EventObject::Instance().RaiseEvent(EVENT_SW_UPDATE_STARTER_FAILED);
    }
    mp_SWUpdateStarter->blockSignals(true);
    mp_SWUpdateStarter->deleteLater();
    mp_SWUpdateStarter = NULL;

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
 *  \brief  Sets SW  Update parameters in BootConfig.txt
 *  \iparam UpdateStatus = "NA" or "Success" or "Failure"
 *  \iparam StartProcess = "SW_Update"
 */
/****************************************************************************/
void SWUpdateManager::UpdateRebootFile(const QString UpdateStatus,
                                       const QString StartProcess)
{
    m_MasterThreadControllerRef.m_BootConfigFileContent.insert("Software_Update_Status", UpdateStatus);
    m_MasterThreadControllerRef.m_BootConfigFileContent.insert("Start_Process", StartProcess);
    Global::UpdateRebootFile(m_MasterThreadControllerRef.m_BootConfigFileContent);
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
