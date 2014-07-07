/****************************************************************************/
/*! \file RemoteCareController/Source/RemoteCareThreadController.cpp
 *
 *  \brief Implementation file for class RemoteCareController.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2014-03-13
 *  $Author:    $ Ramya GJ
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

#include <RemoteCareController/Include/RemoteCareThreadController.h>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/Utils.h>

#include <Global/Include/Commands/AckOKNOK.h>

#include <NetCommands/Include/CmdRCNotifyDataItem.h>
#include <NetCommands/Include/CmdRCNotifyShutdown.h>
#include <NetCommands/Include/CmdRCNotifyReconnection.h>
#include <NetCommands/Include/CmdRCNotifyAssetComplete.h>
#include <NetCommands/Include/CmdRCReportEvent.h>
#include <NetCommands/Include/CmdRCSetLogEvent.h>
#include <NetCommands/Include/CmdRCSetTag.h>

namespace RemoteCare {

/****************************************************************************/
/**
 * \brief Constructor.
 *
 * \iparam   ThreadID    Thread ID.
 */
/****************************************************************************/
RemoteCareController::RemoteCareController(const quint32 ThreadID, const QString networkProcessName, NetworkBase::NetworkServerType_t serverType)
    : ExternalProcessController::ExternalProcessController(networkProcessName, ThreadID)
    , mp_RemoteCareDevice(NULL)
    , m_networkProcessName(networkProcessName)
    , m_networkServerType(serverType)   //NetworkBase::NSE_TYPE_AXEDA
{
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
RemoteCareController::~RemoteCareController() {
    try {
        if (mp_RemoteCareDevice != NULL) {
            delete mp_RemoteCareDevice;
        }
        mp_RemoteCareDevice = NULL;
    }
    CATCHALL_DTOR();
}

/****************************************************************************/
/**
 * \brief Create and configure needed objects.
 *
 * If something goes wrong, the already created objects must be deleted.
 */
/****************************************************************************/
void RemoteCareController::CreateAndInitializeObjects() {
    Global::EventObject::Instance().RaiseEvent(Global::EVENT_THREAD_CREATE_AND_INITIALIZE, Global::FmtArgs() << "Remotecare controller");
    qDebug() << "RemoteCareController: initializing objects...";

    QString path = Global::SystemPaths::Instance().GetSettingsPath() + NetworkBase::CMH_PATH_TO_SETTINGS;
    mp_RemoteCareDevice = new ExternalProcessControl::ExternalProcessDevice(m_networkServerType, m_networkProcessName, path, this, this);

    // register remote care device with Base Class
    ExternalProcessController::RegisterExternalProcessDevice(mp_RemoteCareDevice);

    // register acknowledge processing and timeout functors
    RegisterThreadAcksAndTimeouts();

    // continue with initialization
    ExternalProcessController::CreateAndInitializeObjects();
}

/****************************************************************************/
/**
 * \brief Cleanup and destroy the created objects.
 */
/****************************************************************************/
void RemoteCareController::CleanupAndDestroyObjects() {
    // destroy all objects
    qDebug() << (QString)("RemoteCareController: CleanupAndDestroyObjects called.");
    ExternalProcessController::CleanupAndDestroyObjects();
}

/****************************************************************************/
/**
 * \brief This method is called when the base class received the \ref Go signal.
 *
 * This means that everything is fine and normal operation started.
 * We are running in our own thread now.\n
 */
/****************************************************************************/
void RemoteCareController::OnGoReceived() {
    Global::EventObject::Instance().RaiseEvent(Global::EVENT_THREAD_ON_GO_RECEIVED, Global::FmtArgs() << "Remote care controller");
    qDebug() << (QString)("RemoteCareController: OnGo received");
    ExternalProcessController::OnGoReceived();
}

/****************************************************************************/
/**
 * \brief This method is called when RCAgent is connected and ready to work.
 *
 *    This means we were called from the "Working" state.
 */
/****************************************************************************/
void RemoteCareController::OnReadyToWork() {
    qDebug() << "RemoteCareController: ready to work called.";

    CHECKPTR(mp_RemoteCareDevice);

    // start sending DateAndTime (use default period)
    if (!mp_RemoteCareDevice->StartDateTimeSync(0)) {
        /// \todo: handle error.
    }

    emit RCAConnected();
}

/****************************************************************************/
/**
 * \brief This method is called when the base class received the \ref Stop signal.
 *
 * This means that normal operation will stop after processing this signal.
 * We are still running in our own thread.\n
 */
/****************************************************************************/
void RemoteCareController::OnStopReceived() {
    qDebug() << (QString)("RemoteCareController: OnStop received.");
    ExternalProcessController::OnStopReceived();
}

/****************************************************************************/
/**
 * \brief Power will fail shortly.
 *
 * Power will fail shortly.
 *
 *  \iparam PowerFailStage
 */
/****************************************************************************/
void RemoteCareController::OnPowerFail(const Global::PowerFailStages PowerFailStage)
{
    Q_UNUSED(PowerFailStage)
}

/****************************************************************************/
/**
 * \brief This function registers all command acknowledges
 *        used by this thread controller
 */
/****************************************************************************/
void RemoteCareController::RegisterThreadAcksAndTimeouts()
{
    // Outgoing remotecare commands
    RegisterCommandForProcessing<NetCommands::CmdRCNotifyDataItem, RemoteCare::RemoteCareController>
            (&RemoteCareController::SendCmdToExternalProcess<NetCommands::CmdRCNotifyDataItem>, this);
    RegisterCommandForProcessing<NetCommands::CmdRCNotifyShutdown, RemoteCare::RemoteCareController>
            (&RemoteCareController::SendCmdToExternalProcess<NetCommands::CmdRCNotifyShutdown>, this);
    RegisterCommandForProcessing<NetCommands::CmdRCNotifyReconnection, RemoteCare::RemoteCareController>
            (&RemoteCareController::SendCmdToExternalProcess<NetCommands::CmdRCNotifyReconnection>, this);
    RegisterCommandForProcessing<NetCommands::CmdRCReportEvent, RemoteCare::RemoteCareController>
            (&RemoteCareController::SendCmdToExternalProcess<NetCommands::CmdRCReportEvent>, this);
    RegisterCommandForProcessing<NetCommands::CmdRCNotifyAssetComplete, RemoteCare::RemoteCareController>
            (&RemoteCareController::SendCmdToExternalProcess<NetCommands::CmdRCNotifyAssetComplete>, this);

    RegisterAcknowledgeForProcessing<Global::AckOKNOK, RemoteCare::RemoteCareController>
            (&RemoteCareController::SendAckToExternalProcess<Global::AckOKNOK>, this);

    // Incoming remotecare commands
    RegisterExternalMessage<NetCommands::CmdRCSetLogEvent, RemoteCare::RemoteCareController>
                (&RemoteCareController::ForwardCmdFromExternalProcess<NetCommands::CmdRCSetLogEvent>, this);
    RegisterExternalMessage<NetCommands::CmdRCSetTag, RemoteCare::RemoteCareController>
                (&RemoteCareController::ForwardCmdFromExternalProcess<NetCommands::CmdRCSetTag>, this);

    // register acknowledges for receiving from external process
    RegisterExternalMessage<Global::AckOKNOK, RemoteCare::RemoteCareController>
            (&RemoteCareController::ForwardAckFromExternalProcess<Global::AckOKNOK>, this);

}

/****************************************************************************/
/**
 * \brief Handler for command timeouts.
 *
 * \iparam       Ref         The command reference.
 * \iparam       CmdName     Name of command.
 */
/****************************************************************************/
void RemoteCareController::OnCmdTimeout(const Global::tRefType Ref, const QString &CmdName)
{
    Q_UNUSED(Ref)
    Q_UNUSED(CmdName)
}

/****************************************************************************/
/*!
 *  \brief  This method handles internal command acknowledge.
 *
 *  \iparam Ref - reference of internal message
 *  \iparam Ack - the acknowledge
 *
 *  \return  true if success, false otherwise
 */
/****************************************************************************/
void RemoteCareController::OnCommandAckReceived(const Global::tRefType Ref, const Global::Acknowledge &Ack)
{
    Q_UNUSED(Ref)
    Q_UNUSED(Ack)
}

/****************************************************************************/
/**
 * \brief  Informs Master Thread controller that RCA process has stopped
 *         working.
 * \iparam StopForEver = true indicates to stop working for ever, since no
 *                       more retries would be carried out. When this happens
 *                      RCA work flow needs to be stopped
 */
/****************************************************************************/
void RemoteCareController::OnStopWorking(bool StopForEver)
{
    if(StopForEver) {
        emit ProcessStoppedForEver();
    }
}

} // end namespace Axeda
