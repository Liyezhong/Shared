/****************************************************************************/
/*! \file AxedaController/Source/AxedaThreadController.cpp
 *
 *  \brief Implementation file for class AxedaThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-08-30
 *  $Author:    $ J.Bugariu
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

#include <AxedaController/Include/AxedaThreadController.h>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/Utils.h>

#include <Global/Include/Commands/AckOKNOK.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaAlarm.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaEvent.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaDataItem.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaUpload.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaRemoteSessionRequest.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaRemoteSessionStatus.h>

namespace Axeda {

/****************************************************************************/
/**
 * \brief Constructor.
 *
 * \param[in]   TheHeartBeatSource    Logging source to be used.
 */
/****************************************************************************/
AxedaThreadController::AxedaThreadController(Global::gSourceType TheHeartBeatSource) :
        ExternalProcessController::ExternalProcessController(AXEDA_PROCESS_NAME, TheHeartBeatSource),
        m_pAxedaDevice(NULL)
{
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
AxedaThreadController::~AxedaThreadController() {
    try {
        if (m_pAxedaDevice != NULL) {
            delete m_pAxedaDevice;
        }
        m_pAxedaDevice = NULL;
    } catch(...) {
        // to please PCLint...
    }
}

/****************************************************************************/
/**
 * \brief Create and configure needed objects.
 *
 * If something goes wrong, the already created objects must be deleted.
 */
/****************************************************************************/
void AxedaThreadController::CreateAndInitializeObjects() {

    qDebug() << "AxedaThreadController: initializing objects...";

    QString path = Global::SystemPaths::Instance().GetSettingsPath() + NetworkBase::CMH_PATH_TO_SETTINGS;
    m_pAxedaDevice = new ExternalProcessControl::ExternalProcessDevice(NetworkBase::NSE_TYPE_AXEDA, AXEDA_PROCESS_NAME, path, this, this);

    // register GuiDevice with Base Class
    ExternalProcessController::RegisterExternalProcessDevice(m_pAxedaDevice);

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
void AxedaThreadController::CleanupAndDestroyObjects() {
    // destroy all objects
    /// \todo implement
    qDebug() << (QString)("AxedaThreadController: CleanupAndDestroyObjects called.");
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
void AxedaThreadController::OnGoReceived() {
    qDebug() << (QString)("AxedaThreadController: OnGo received");
    ExternalProcessController::OnGoReceived();
}

/****************************************************************************/
/**
 * \brief This method is called when RCAgent is connected and ready to work.
 *
 *    This means we were called from the "Working" state.
 */
/****************************************************************************/
void AxedaThreadController::OnReadyToWork() {
    qDebug() << "AxedaThreadController: ready to work called.";

    CHECKPTR(m_pAxedaDevice);

    // start sending DateAndTime (use default period)
    if (!m_pAxedaDevice->StartDateTimeSync(0)) {
        /// \todo: handle error.
    }

    /// \todo: remove ths temporary test code ! ========================
    // alarm
    RCAgentNamespace::CmdAxedaAlarm Alarm(10000, "MasterAlarmTest", "Test Message N42", "777", "Unconditional", Global::AdjustedTime::Instance().GetCurrentDateTime().toString(NetworkBase::DATEANDTIME_FORMAT));
    SendCmdToExternalProcess<RCAgentNamespace::CmdAxedaAlarm>((Global::tRefType)1, Alarm);
    // event
    RCAgentNamespace::CmdAxedaEvent Event(10000, "MasterEventTest", "Test Message N11", "Ooops", Global::AdjustedTime::Instance().GetCurrentDateTime().toString(NetworkBase::DATEANDTIME_FORMAT));
    SendCmdToExternalProcess<RCAgentNamespace::CmdAxedaEvent>((Global::tRefType)2, Event);
    // data item
    RCAgentNamespace::CmdAxedaDataItem DataItem(10000, "RemoteSessionRequest", (RCAgentNamespace::AxedaDataItemType_t)1, \
                                                (RCAgentNamespace::AxedaDataItemQuality_t)0, "0", Global::AdjustedTime::Instance().GetCurrentDateTime().toString(NetworkBase::DATEANDTIME_FORMAT));
    SendCmdToExternalProcess<RCAgentNamespace::CmdAxedaDataItem>((Global::tRefType)3, DataItem);
    // upload
    RCAgentNamespace::CmdAxedaUpload Upload(10000, "TestUpload.txt");
    SendCmdToExternalProcess<RCAgentNamespace::CmdAxedaUpload>((Global::tRefType)4, Upload);
    /// ====================================================================
}

/****************************************************************************/
/**
 * \brief This method is called when the base class received the \ref Stop signal.
 *
 * This means that normal operation will stop after processing this signal.
 * We are still running in our own thread.\n
 */
/****************************************************************************/
void AxedaThreadController::OnStopReceived() {
    qDebug() << (QString)("AxedaThreadController: OnStop received.");
    ExternalProcessController::OnStopReceived();
}

/****************************************************************************/
/**
 * \brief Power will fail shortly.
 *
 * Power will fail shortly.
 */
/****************************************************************************/
void AxedaThreadController::OnPowerFail() {
    /// \todo implement
}

/****************************************************************************/
/**
 * \brief This function registers all command acknowledges
 *        used by this thread controller
 */
/****************************************************************************/
void AxedaThreadController::RegisterThreadAcksAndTimeouts()
{
    // Outgoing Axeda commands
    RegisterCommandForProcessing<RCAgentNamespace::CmdAxedaAlarm, Axeda::AxedaThreadController>
            (&AxedaThreadController::SendCmdToExternalProcess<RCAgentNamespace::CmdAxedaAlarm>, this);
    RegisterCommandForProcessing<RCAgentNamespace::CmdAxedaEvent, Axeda::AxedaThreadController>
            (&AxedaThreadController::SendCmdToExternalProcess<RCAgentNamespace::CmdAxedaEvent>, this);
    RegisterCommandForProcessing<RCAgentNamespace::CmdAxedaDataItem, Axeda::AxedaThreadController>
            (&AxedaThreadController::SendCmdToExternalProcess<RCAgentNamespace::CmdAxedaDataItem>, this);
    RegisterCommandForProcessing<RCAgentNamespace::CmdAxedaUpload, Axeda::AxedaThreadController>
            (&AxedaThreadController::SendCmdToExternalProcess<RCAgentNamespace::CmdAxedaUpload>, this);

    // Incoming Axeda commands
    RegisterExternalMessage<RCAgentNamespace::CmdAxedaRemoteSessionRequest, Axeda::AxedaThreadController>
            (&AxedaThreadController::OnCmdAxedaRemoteSessionRequest, this);  ///< dummy for tests. remove later!

    // general commands
    /// \todo JB: implement ack and acknowledge handling (see gui for details)
    RegisterAxedaDeviceAck<Global::AckOKNOK>();
}

/****************************************************************************/
/**
 * \brief Dummy test processor for the CmdAxedaRemoteSessionRequest command.
 *
 * \param[in]       Ref         The command reference.
 * \param[in]       Cmd         The command.
 */
/****************************************************************************/
void  AxedaThreadController::OnCmdAxedaRemoteSessionRequest(Global::tRefType Ref, RCAgentNamespace::CmdAxedaRemoteSessionRequest *Cmd)
{
    Q_UNUSED(Ref)
    if (Cmd != NULL) {
        qDebug() << "CmdAxedaRemoteSessionRequest  - request value = " << Global::AsInt(Cmd->GetRequestValue());
        qDebug() << "CmdAxedaRemoteSessionRequest  - request type = " << Global::AsInt(Cmd->GetTagType());
        delete Cmd;
    }
    RCAgentNamespace::CmdAxedaRemoteSessionStatus status(10000, true);
    SendCmdToExternalProcess<RCAgentNamespace::CmdAxedaRemoteSessionStatus>((Global::tRefType)77, status);
}

/****************************************************************************/
/**
 * \brief Handler for command timeouts.
 *
 * \param[in]       Ref         The command reference.
 * \param[in]       CmdName     Name of command.
 */
/****************************************************************************/
void AxedaThreadController::OnCmdTimeout(Global::tRefType Ref, const QString &CmdName)
{
    Q_UNUSED(Ref)
    Q_UNUSED(CmdName)
    /// \todo: implement
}

/****************************************************************************/
/*!
 *  \brief  This method handles internal command acknowledge.
 *
 *  \param[in] Ref - reference of internal message
 *  \param[in] Ack - the acknowledge
 *
 *  \return  true if success, false otherwise
 */
/****************************************************************************/
void AxedaThreadController::OnCommandAckReceived(Global::tRefType Ref, const Global::Acknowledge &Ack)
{
    Q_UNUSED(Ref)
    Q_UNUSED(Ack)
    /// \todo: implement
}

} // end namespace Axeda
