/****************************************************************************/
/*! \file AgentController.cpp
 *
 *  \brief Implementation file for class AgentController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 04.07.2011
 *  $Author:    $ Y.Novak
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

#include <QMetaType>
#include <QSharedMemory>
#include <Global/Include/SystemPaths.h>
#include <RemoteCareAgent/Include/AxedaAgent.h>
#include <RemoteCareAgent/Include/AgentController.h>

// master commands
#include <RemoteCareAgent/Include/Commands/CmdAxedaAlarm.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaEvent.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaDataItem.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaUpload.h>
//#include <RemoteCareAgent/Include/Commands/CmdPowerDown.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaRemoteSessionStatus.h>

// agent commands
#include <RemoteCareAgent/Include/Commands/CmdAxedaSetTag.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaRemoteSessionRequest.h>

// global commands
#include <Global/Include/Commands/AckOKNOK.h>

/// \todo: remove this test includes -------------------
#include <Global/Include/Commands/CmdDateAndTime.h>
/// ----------------------------------------------------

namespace RCAgentNamespace {

const CreatorFunctorShPtr_t     NullCreatorFunctor(NULL);    ///< NULL creator functor.
const DispatcherFunctorShPtr_t  NullDispatcherFunctor(NULL); ///< NULL dispatcher functor.

/****************************************************************************/
/*!
*  \brief    Default Constructor.
*
****************************************************************************/
AgentController::AgentController() :
        m_myAxedaAgent(NULL),
        m_myMasterLinkDevice(NULL),
        m_WaitLimit(RCAGENT_EXIT_DEFAULT_WAIT),
        m_InitStatus(RCAGENT_INIT_FAILED)
{
    // register command relevant metatypes
    qRegisterMetaType<Global::tRefType>("Global::tRefType");
    qRegisterMetaType<Global::CommandShPtr_t>("Global::CommandShPtr_t");
    qRegisterMetaType<AxedaDataItemType_t>("AxedaDataItemType_t");
    qRegisterMetaType<AxedaDataItemQuality_t>("AxedaDataItemQuality_t");
}

/****************************************************************************/
/*!
*  \brief    Destructor.
*
****************************************************************************/
AgentController::~AgentController()
{
    try {
        if (m_myAxedaAgent != NULL) {
            delete m_myAxedaAgent;
            m_myAxedaAgent = NULL;
        }
        if (m_myMasterLinkDevice != NULL) {
            delete m_myMasterLinkDevice;
            m_myMasterLinkDevice = NULL;
        }
    }
    catch(...) {
        // to please Lint
    }
}

/****************************************************************************/
/*!
*  \brief   Initialize Agent Component.
*
*  \param[in]    NofArgs = number of input arguments
*  \param[in]    args = input arguments
*
*  \return  true if successfully initialized, false otherwise
*
****************************************************************************/
RCAgentInitCodes_t AgentController::Initialize(int NofArgs, char *args[])
{
    QString ip = "";
    QString port = "";
    m_InitStatus = RCAGENT_INITIALIZED_OK;

    Stop();
    Cleanup();

    if (NofArgs == 3) {
        ip = (QString)args[1];
        port = (QString)args[2];
    }

    try {
        if (!InitializeAgent()) {
            // the AxedaAgent cannot run, but we can still connect to Master and report error
            qDebug() << "AgentController ERROR: InitializeAgent failed !";
            m_InitStatus = static_cast<RCAgentInitCodes_t>(m_InitStatus | RCAGENT_AXEDA_INIT_FAILED);
        }

        if (!InitializeMasterLink(ip, port)) {
            // RCAgent cannot connect to Master, but maybe it can run AxedaAgent and report error to RC-EServer
            qDebug() << "AgentController ERROR: InitializeMasterLink failed !";
            m_InitStatus = static_cast<RCAgentInitCodes_t>(m_InitStatus | RCAGENT_MASTERLINK_INIT_FAILED);
        }

        // register incoming Agent commands
        RegisterAgentMessage<RCAgentNamespace::CmdAxedaRemoteSessionRequest, AgentController>(&AgentController::OnAgentRemoteSessionRequest, this);

        // register incoming Master commands
        RegisterMasterMessage<RCAgentNamespace::CmdAxedaAlarm, AgentController>(&AgentController::OnCmdAxedaAlarm, this);
        RegisterMasterMessage<RCAgentNamespace::CmdAxedaEvent, AgentController>(&AgentController::OnCmdAxedaEvent, this);
        RegisterMasterMessage<RCAgentNamespace::CmdAxedaDataItem, AgentController>(&AgentController::OnCmdAxedaDataItem, this);
        RegisterMasterMessage<RCAgentNamespace::CmdAxedaUpload, AgentController>(&AgentController::OnCmdAxedaUpload, this);
        RegisterMasterMessage<RCAgentNamespace::CmdAxedaRemoteSessionStatus, AgentController>(&AgentController::OnCmdAxedaRemoteSessionStatus, this);
//        RegisterMasterMessage<RCAgentNamespace::CmdPowerDown, AgentController>(&AgentController::OnCmdPowerDown, this);
        RegisterMasterMessage<Global::AckOKNOK, AgentController>(&AgentController::OnCmdAck, this);

#if ((!defined(QT_NO_DEBUG)) && (!defined(QT_NO_DEBUG_OUTPUT)))
        /// initialize the shared memory debug monitor for debug binary
        InitializeShutDownMonitor();
#endif
    }
    catch(...) {
        // does not matter what it is, the agent controller failed to initialize.
        return RCAGENT_INIT_FAILED;
    }

    return m_InitStatus;
}

/****************************************************************************/
/*!
*  \brief   Initialize Network Layer for connection to Master.
*
*  \param[in]    ip = server ip to connect to
*  \param[in]    port = server port
*
*  \return  true if link is successfully initialized, false otherwise
*
****************************************************************************/
bool AgentController::InitializeMasterLink(const QString &ip, const QString &port)
{
    try {
        // give NULL as parent to be able to move m_myMasterLinkDevice to thread
        m_myMasterLinkDevice = new NetworkBase::NetworkClientDevice(NetworkBase::NCE_TYPE_AXEDA, ip, port, "", NULL);

        if (m_myMasterLinkDevice == NULL) {
            return false;
        }
        if (!m_myMasterLinkDevice->InitializeDevice()) {
            return false;
        }
        if (!QObject::connect(&m_myNetLayerThread, SIGNAL(started()), m_myMasterLinkDevice, SLOT(StartDevice()))) {
            qDebug() << "AgentController ERROR: Cannot connect NetLayer and Thread !";
            return false;
        }
        if (!QObject::connect(&m_myNetLayerThread, SIGNAL(finished()), m_myMasterLinkDevice, SLOT(DestroyDevice()))) {
            qDebug() << "AgentController ERROR: Cannot connect NetLayer and Thread !";
            return false;
        }
        if (!QObject::connect(&m_myNetLayerThread, SIGNAL(terminated()), m_myMasterLinkDevice, SLOT(DestroyDevice()))) {
            qDebug() << "AgentController ERROR: Cannot connect NetLayer and Thread !";
            return false;
        }
        if (!QObject::connect(m_myMasterLinkDevice, SIGNAL(SigPeerConnected(const QString &)),
                              this, SLOT(ConnectedToMaster(const QString &)))) {
            return false;
        }
        if (!QObject::connect(m_myMasterLinkDevice, SIGNAL(SigPeerDisconnected(const QString &)),
                              this, SLOT(MasterLinkProblem(const QString &)))) {
            return false;
        }
        // Signal which delivers an incoming from Master message
        if (!QObject::connect(m_myMasterLinkDevice, SIGNAL(ForwardMessageToUpperLayer(const QString &, const QByteArray &)),
                              this, SLOT(ProcessMasterMessage(const QString &, const QByteArray &)))) {
            qDebug() << "AgentController: Cannot connect ACK signal-slot pair !\n";
            return false;
        }
        // Signal which sends message to Master
        if (!QObject::connect(this, SIGNAL(SigSendMessageToMaster(const QString &, const QByteArray &, Global::tRefType)),
                              m_myMasterLinkDevice, SLOT(SendOutgoingCommand(const QString &, const QByteArray &, Global::tRefType)))) {
            qDebug() << "AgentController: Cannot connect message sending signal-slot pair !\n";
            return false;
        }
        // Message sending status signal
        if (!QObject::connect(m_myMasterLinkDevice, SIGNAL(SigMsgSendingResult(Global::tRefType, const QString &)),
                              this, SLOT(OnMsgSendingResult(Global::tRefType, const QString &)))) {
            qDebug() << "AgentController: Cannot connect message sending result signal-slot pair !\n";
            return false;
        }

        // move the NetDevice to its execution thread
        m_myMasterLinkDevice->moveToThread(&m_myNetLayerThread);
    }
    catch (const std::bad_alloc &) {
        ///< \todo: log error and inform Master
        return false;
    }
    return true;
}

/****************************************************************************/
/*!
*  \brief   Initialize AxedaAgent for connection to Enterprise Server.
*
*
*  \return  true if link is successfully initialized, false otherwise
*
****************************************************************************/
bool AgentController::InitializeAgent()
{
    try {
        m_myAxedaAgent = new AxedaAgent();
        if (m_myAxedaAgent == NULL) {
            return false;
        }
        if (!m_myAxedaAgent->Initialize()) {
            qDebug() << "AgentController ERROR: Cannot initialize Agent !";
            return false;
        }
        // connect Agent's thread start signal to Agent's start function
        if (!QObject::connect(&m_myAgentThread, SIGNAL(started()), m_myAxedaAgent, SLOT(Work()))) {
            qDebug() << "AgentController ERROR: Cannot connect Agent and Thread !";
            return false;
        }
        // connect Agent's main loop exit signal to quit slot of the local event loop
        if (!QObject::connect(m_myAxedaAgent, SIGNAL(SigAgentStopped()), &m_EventLoop, SLOT(quit()))) {
            qDebug() << "AgentController ERROR: Cannot connect Agent and Event Loop !";
            return false;
        }
        // connect Agent's Stop slot with Controller's exit request signal
        if (!QObject::connect(this, SIGNAL(SigStopAxedaAgent()), m_myAxedaAgent, SLOT(Stop()))) {
            qDebug() << "AgentController ERROR: Cannot connect SigStopAxedaAgent signal !";
            return false;
        }

        /// \todo: ---------- exit signals:  sort them out for all cases ! -----------------
        ///        --- (also see comment about state machines in the class description) ----
        // connect Agent's main loop exit signal to Agent's thread stop slot
        if (!QObject::connect(m_myAxedaAgent, SIGNAL(SigAgentStopped()), &m_myAgentThread, SLOT(quit()))) {
            qDebug() << "AgentController ERROR: Cannot connect Agent and Thread !";
            return false;
        }
        // connect Agent's thread quit signal to AgentController exit function
        if (!QObject::connect(&m_myAgentThread, SIGNAL(finished()), this, SLOT(ExitNow()))) {
            qDebug() << "AgentController ERROR: Cannot connect Agent's Thread and Controller !";
            return false;
        }
        // connect Agent's thread terminated signal to AgentController exit function
        if (!QObject::connect(&m_myAgentThread, SIGNAL(terminated()), this, SLOT(ExitNow()))) {
            qDebug() << "AgentController ERROR: Cannot connect Agent's Thread and Controller !";
            return false;
        }
        /// ---------- exit signals -------------


        // channel to send Alarms
        if (!QObject::connect(this, SIGNAL(SigAlarmRequest(quint64, QByteArray, QByteArray, const QString &, QByteArray, const QString &)),
                              m_myAxedaAgent, SLOT(SubmitAlarmRequest(quint64, QByteArray, QByteArray, const QString &, QByteArray, const QString &)))) {
            qDebug() << "AgentController ERROR: Cannot connect SigAlarmRequest signal !";
            return false;
        }
        // channel to send Events
        if (!QObject::connect(this, SIGNAL(SigEventRequest(quint64, QByteArray, QByteArray, const QString &, const QString &)),
                              m_myAxedaAgent, SLOT(SubmitEventRequest(quint64, QByteArray, QByteArray, const QString &, const QString &)))) {
            qDebug() << "AgentController ERROR: Cannot connect SigEventRequest signal !";
            return false;
        }
        // channel to send DataItems
        if (!QObject::connect(this, SIGNAL(SigDataItemRequest(quint64, QByteArray, AxedaDataItemType_t, AxedaDataItemQuality_t, QByteArray, const QString &)),
                              m_myAxedaAgent, SLOT(SubmitDataItemRequest(quint64, QByteArray, AxedaDataItemType_t, AxedaDataItemQuality_t, QByteArray, const QString &)))) {
            qDebug() << "AgentController ERROR: Cannot connect SigDataItemRequest signal !";
            return false;
        }
        // channel to send Upload requests
        if (!QObject::connect(this, SIGNAL(SigUploadRequest(quint64, QByteArray)),
                              m_myAxedaAgent, SLOT(SubmitUploadRequest(quint64, QByteArray)))) {
            qDebug() << "AgentController ERROR: Cannot connect SigUploadRequest signal !";
            return false;
        }  ;
        // channel to send Upload requests
        if (!QObject::connect(this, SIGNAL(SigSetRemoteSessionStatus(bool)),
                              m_myAxedaAgent, SLOT(SetRemoteSessionStatus(bool)))) {
            qDebug() << "AgentController ERROR: Cannot connect SigSetRemoteSessionStatus signal !";
            return false;
        }
        // request status signal
        if (!QObject::connect(m_myAxedaAgent, SIGNAL(SigRequestStatus(quint64, const QString &)),
                              this, SLOT(OnRequestStatus(quint64, const QString &)))) {
            qDebug() << "AgentController ERROR: Cannot connect Agent's SigRequestStatus signal !";
            return false;
        }
        // Agent's SetTag command
        if (!QObject::connect(m_myAxedaAgent, SIGNAL(SigAgentIncomingCommand(const Global::CommandShPtr_t &)),
                              this, SLOT(OnAgentIncomingCommand(const Global::CommandShPtr_t &)))) {
            qDebug() << "AgentController ERROR: Cannot connect Agent's SigAgentIncomingCommand signal !";
            return false;
        }

        /**   ===  Calculate thread exit waiting time (in ms)  ===
           This is a timeout for RemoteCareAgent powerdown. To be able to power
           down gracefully the Controller needs to wait till AxedaAgent exits its
           working loop. Otherwise the RC-EServer might be left with some
           pending requests.

           The waiting period depends on the ExecTime of the AxedaAgent.
        */
        m_WaitLimit = (unsigned long)((m_myAxedaAgent->GetExecTime() + 1) * 1000);
        qDebug() << "AgentController: the m_WaitLimit is set to " << m_WaitLimit;

        // move Agent to its execution thread:
        m_myAxedaAgent->moveToThread(&m_myAgentThread);
    }
    catch (const std::bad_alloc &) {
        ///< \todo: log error and inform Master
        return false;
    }
    return true;
}

/****************************************************************************/
/*!
*  \brief   Start Axeda Component operation.
*           This funciton checks initialization results and acts accordingly
*
*  \return  true if started successfully, false otherwise
*
****************************************************************************/
bool AgentController::Start()
{
    bool result = true;
    switch (m_InitStatus) {
    case RCAGENT_INITIALIZED_OK:
    case RCAGENT_AXEDA_INIT_FAILED:
        StartNetwork();
        break;
    case RCAGENT_MASTERLINK_INIT_FAILED:
        StartAgent();
        break;
    default:
        // this condition is not allowed --> fail
        result = false;
        break;
    }
    return result;
}

/****************************************************************************/
/*!
*  \brief   This slot starts Axeda Agent operation.
*
****************************************************************************/
void AgentController::StartNetwork()
{
    if (m_myMasterLinkDevice == NULL) {
        ///< \todo: error. inform Enterprise Server (if we can connect...)
        return;
    }
    m_myNetLayerThread.start();
}

/****************************************************************************/
/*!
*  \brief   This slot starts Axeda Agent operation.
*
****************************************************************************/
void AgentController::StartAgent()
{
    if (m_myAxedaAgent == NULL) {
        ///< \todo: error. inform Master (if we are connected...)
        return;
    }
    // start Agent's thread (we better start event loop before emitting signal)
    m_myAgentThread.start();
    // check the init status and act on MasterLink init fail
    if (m_InitStatus == RCAGENT_MASTERLINK_INIT_FAILED) {
        // set error condition for the AxedaAgent
        QByteArray name = QString("MasterLinkFail").toUtf8();
        QByteArray message = QString("No connection to Master!").toUtf8();
        QString severity("777");
        QByteArray condition = QString("Highest").toUtf8();
        QString timestamp(QDateTime::currentDateTime().toString(RCAgentNamespace::DATEANDTIME_FORMAT));
        emit SigAlarmRequest((quint64)1, name, message, severity, condition, timestamp);
    }
}

/****************************************************************************/
/*!
*  \brief   Stop Axeda Component operation.
*
****************************************************************************/
void AgentController::Stop()
{
    // if Agent is running, tell it to exit the working loop
    if (m_myAxedaAgent != NULL) {
        if (m_myAxedaAgent->GetState() == AGENT_IS_RUNNING) {
            /// \todo: sort Stop() out with sig/slot !
            //m_myAxedaAgent->Stop();
            emit SigStopAxedaAgent();
            // now turn asynchronous event to synchronous one:
            // wait till Agent exits its loop or quit on timeout.
            QTimer::singleShot((int)m_WaitLimit, &m_EventLoop, SLOT(quit()));
            qDebug() << "\nEntering event loop at: " << QDateTime::currentDateTime().toString("hh:mm:ss");
            static_cast<void>(m_EventLoop.exec()); // don't care about return value here
            qDebug() << "Exiting event loop at: " << QDateTime::currentDateTime().toString("hh:mm:ss") << "\n";
        }
    }
    // drop connection to Master
    if (m_myMasterLinkDevice != NULL) {
        m_myMasterLinkDevice->DisconnectPeer();
    }

    // stop both threads (if they are not running, "quit()" just does nothing)
    m_myNetLayerThread.quit();
    m_myAgentThread.quit();
}

/****************************************************************************/
/*!
*  \brief   Destroy Axeda Component.
*
****************************************************************************/
void AgentController::Cleanup()
{
    // destroy all created objects
    if (m_myAxedaAgent != NULL) {
        m_myAxedaAgent->deleteLater();
    }
    if (m_myMasterLinkDevice != NULL) {
        m_myMasterLinkDevice->deleteLater();
    }
}

/****************************************************************************/
/*!
*  \brief   Axeda Component operation finished --> Exit.
*
****************************************************************************/
void AgentController::ExitNow()
{
    qDebug() << "AgentController: exiting now...";
    emit SigOperationStopped();
}

/****************************************************************************/
/*!
*  \brief   This slot is called when connection to Master SW is established
*
*           If AxedaAgent did not init properly, this function will send
*           an error message to Master.
*
*  \param[in] name = name of the client
*
****************************************************************************/
void AgentController::ConnectedToMaster(const QString &name)
{
    Q_UNUSED(name)
    qDebug() << "AgentController: ConnectedToMaster slot is called.";
    if (m_InitStatus != RCAGENT_AXEDA_INIT_FAILED) {
        // all OK, just start the AxedaAgent and return
        StartAgent();

        /// \todo: remove this test block !! --------
        // send some test stuff to Master
        Global::CmdDateAndTime cmd1(10000, QDateTime::currentDateTime());
        Global::tRefType Ref = SendCmdToMaster<Global::CmdDateAndTime>(cmd1);
        Q_UNUSED(Ref);
        /// -----------------------------------------

        return;
    }
    // AxedaAgent initialization failed: report that to Master
    ///< \todo: do not start the Agent and report problem to Master!
}

/****************************************************************************/
/*!
*  \brief   This slot is called when RCAgent loses the connection to
*           the Master SW.
*
*  \param[in] name = name of the client
*
****************************************************************************/
void AgentController::MasterLinkProblem(const QString &name)
{
    Q_UNUSED(name)
    ///< \todo: implement. At the moment AgentController just shuts down:
    Stop();
}

/****************************************************************************/
/*!
*  \brief   Process the incoming from Master message
*
*  \param[in]    name = message name
*  \param[in]    payload = serialized command
*
****************************************************************************/
void AgentController::ProcessMasterMessage(const QString &name, const QByteArray &payload)
{
    Q_UNUSED(name)
    // deserialize data
    QDataStream DS(payload);
    DS.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    // read class name
    QString ClassName = "";
    DS >> ClassName;
    // read Ref
    Global::tRefType Ref = 0;
    DS >> Ref;
    qDebug() << "AgentController::ProcessMasterMessage: incoming ClassName " << ClassName << " with reference " << Ref;
    // OK, now get functor and execute
    CreatorFunctorShPtr_t Functor = GetCreatorFunctor(ClassName);
    if(Functor == NullCreatorFunctor) {
        /// \todo: report error to Master?
        qDebug() << "AgentController::ProcessMasterMessage:  No creator functor found for " << ClassName;
        return;
    }
    // execute
    Functor.GetPointerToUserData()->CreateAndDeserialize(Ref, DS);
}

/****************************************************************************/
/*!
*  \brief   This slot is called when Agent receives a command from the
*           Remote Care Server.
*
*  \param   ptr = shared pointer to a command
*
*****************************************************************************/
void AgentController::OnAgentIncomingCommand(const Global::CommandShPtr_t &ptr)
{
    QString cmdname = ptr.GetPointerToUserData()->GetName();
    qDebug () << "AgentController::OnAgentIncomingCommand: name = " << cmdname;

    // get functor and execute
    DispatcherFunctorShPtr_t Functor = GetDispatcherFunctor(cmdname);
    if(Functor == NullDispatcherFunctor) {
        /// \todo: report error to Master?
        qDebug() << "AgentController::OnAgentIncomingCommand:  No dispatcher functor found for " << cmdname;
        return;
    }
    // execute
    Functor.GetPointerToUserData()->Dispatch(ptr);
}

/****************************************************************************/
/*!
*  \brief   This slot is called when Server requests to enable Remote
*           Sessions on device.
*
*  \note    We (so far) do not care about timeout for this command.
*           It involves user action via GUI on Master side. Besides, we
*           cannot (so far) report anything useful to Axeda Server anyway.
*
*  \param   ptr = shared pointer to the Agent's RemoteSessionRequest command
*
*****************************************************************************/
void AgentController::OnAgentRemoteSessionRequest(const Global::CommandShPtr_t &ptr)
{
    Q_UNUSED(ptr)
    qDebug() << "AgentController::OnAgentRemoteSessionRequest called.";
    qDebug() << "*** m_ReqValue = " << ((CmdAxedaRemoteSessionRequest*)(ptr.GetPointerToUserData()))->GetRequestValue();
    // the pointer type is checked by DispatcherFunctor, no need to check it again.
    CmdAxedaRemoteSessionRequest cmd = *((CmdAxedaRemoteSessionRequest*)(ptr.GetPointerToUserData()));
    // Forward command to Master.
    static_cast<void> (  // we do not need the returned reference
       SendCmdToMaster<RCAgentNamespace::CmdAxedaRemoteSessionRequest>(cmd)
    );
}

/****************************************************************************/
/*!
*  \brief  Remove command record from a list of pending Acks.
*
*  \param[in] ref = reference of the "acked" command.
*
*****************************************************************************/
void AgentController::RemoveFromPendingAcks(Global::tRefType ref)
{
    if(m_PendingAcks.contains(ref)) {
        // remove from list of pending commands
        m_PendingAcks.remove(ref);
        // mark reference as reusable
        m_RefManager.UnblockReference(ref);
        qDebug() << "AgentController: Ref = " << ref << " removed from pending list.";
        return;
    }
    // unknown command reference. maybe already processed?.
    qDebug() << "AgentController ERROR: Ref = " << ref << " is not found in the pending list !";
    /// \todo: what now?
}

/****************************************************************************/
/*!
*  \brief  An acknowledge for a sent Master command is received.
*
*  \param[in]   ref = ack command reference.
*  \param[in]   ack = the acknowledge.
*
*****************************************************************************/
void AgentController::OnCmdAck(Global::tRefType ref, const Global::AckOKNOK &ack)
{
    qDebug() << "AgentController::OnCmdAck: Ref = " << ref << " Ack = " << ack.GetName();
    // remove command record from the list of pending acks
    RemoveFromPendingAcks(ref);
    /// \todo: do whatever needs to be done on this event
}

/****************************************************************************/
/*!
*  \brief  Sent Master command has timed out
*
*  \param[in]   ref = reference of the timed out command.
*  \param[in]   cmdname = the command name.
*
*****************************************************************************/
void AgentController::OnCmdTimeout(Global::tRefType ref, const QString &cmdname)
{
    qDebug() << "AgentController::OnCmdTimeout: Ref = " << ref << " Name = " << cmdname;
    // remove command record from the list of pending acks
    RemoveFromPendingAcks(ref);
    /// \todo: do whatever needs to be done on this event
}

/****************************************************************************/
/*!
*  \brief   This slot is called when Agent reports Controller's request status
*
*  \param[in]    ref = reference of the request
*  \param[in]    status = status description
*
****************************************************************************/
void AgentController::OnRequestStatus(quint64 ref, const QString &status)
{
    //qDebug() << "AgentController: request " << ref << " to AxedaAgent reported status : " << status;
    // check if master link was initialized correctly. if not - exit.
    if ((status != AGENT_REQUEST_OK) && (m_InitStatus == RCAGENT_MASTERLINK_INIT_FAILED)) {
        // Nothing works! Go down.
        Stop();
        ExitNow();
    }
    // set data, construct and send the Ack to master
    bool CmdStatus = false;
    QString str("Command failed");
    if (status == AGENT_REQUEST_OK) {
        CmdStatus = true;
        str = "Command succeeded";
    }
    Global::AckOKNOK ack(CmdStatus, str, Global::GUIMSGTYPE_UNDEFINED); ///< \todo: type of AckOKNOK shall be (not only for GUI but) universal
    SendAckToMaster<Global::AckOKNOK>(ref, ack);
}

/****************************************************************************/
/*!
*  \brief   This slot is called when MasterLinkDevice reports Controller's
*           request status
*
*  \param[in]    ref = reference of the request
*  \param[in]    status = status description
*
****************************************************************************/
void AgentController::OnMsgSendingResult(Global::tRefType ref, const QString &status)
{
    Q_UNUSED(ref)
    Q_UNUSED(status)
    //qDebug() << "AgentController: request " << ref << " to MasterLinkDevice reported status : " << status;
    /// \todo: check the status and act on Nack/Fail/Timeout
}

/****************************************************************************/
/*!
*  \brief  Create an instantiation hash entry for a particular command
*
*          Associate a template functor with a Command class name to be
*          able to create command objects and fill them with data
*          at runtime.
*
*  \param[in]  cmdname = the name of the command served by this functor
*  \param[in]  functor = the functor for this command
*
*****************************************************************************/
void AgentController::RegisterCreatorFunctor(const QString &cmdname, const CreatorFunctorShPtr_t &functor)
{
    // check if already registered
    if (m_CreatorFunctors.contains(cmdname)) {
        /// \todo: inform Master?
        qDebug() << "AgentController: creator functor for " << cmdname << " already registered";
        return;
    }
    // everything OK
    static_cast<void> (
        // we DO NOT NEED the return value of insert
        m_CreatorFunctors.insert(cmdname, functor)
    );
}

/****************************************************************************/
/*!
*  \brief  Get access to a previously created command creating functor
*
*  \param[in]  cmdname = the name of the command to fetch a functor for
*
*  \return  command functor access pointer on success, NULL-Functor on failure
*
*****************************************************************************/
CreatorFunctorShPtr_t AgentController::GetCreatorFunctor(const QString &cmdname) const
{
    CreatorFunctorHash_t::const_iterator it = m_CreatorFunctors.find(cmdname);
    if (it == m_CreatorFunctors.constEnd()) {
        // functor not found
        // return NULL functor
        return NullCreatorFunctor;
    }
    // return functor
    return it.value();
}

/****************************************************************************/
/*!
*  \brief  Create a dispatching hash entry for a particular command
*
*          Associate a template functor with a Command class name to be
*          able to dispatch command objects at runtime.
*
*  \param[in]  cmdname = the name of the command served by this functor
*  \param[in]  functor = the functor for this command
*
*****************************************************************************/
void AgentController::RegisterDispatcherFunctor(const QString &cmdname, const DispatcherFunctorShPtr_t &functor)
{
    // check if already registered
    if (m_DispatcherFunctors.contains(cmdname)) {
        /// \todo: inform Master?
        qDebug() << "AgentController: Dispatcher functor for " << cmdname << " already registered";
        return;
    }
    // everything OK
    static_cast<void> (
        // we DO NOT NEED the return value of insert
        m_DispatcherFunctors.insert(cmdname, functor)
    );
}

/****************************************************************************/
/*!
*  \brief  Get access to a previously created command dispatching functor
*
*  \param[in]  cmdname = the name of the command to fetch a functor for
*
*  \return  command functor access pointer on success, NULL-Functor on failure
*
*****************************************************************************/
DispatcherFunctorShPtr_t AgentController::GetDispatcherFunctor(const QString &cmdname) const
{
    DispatcherFunctorHash_t::const_iterator it = m_DispatcherFunctors.find(cmdname);
    if (it == m_DispatcherFunctors.constEnd()) {
        // functor not found
        // return NULL functor
        return NullDispatcherFunctor;
    }
    // return functor
    return it.value();
}

/****************************************************************************/
/*!
*  \brief  Convert data and forward alarm to Agent
*
*  \param[in]    ref = reference of the request
*  \param[in]    cmd = incoming Alarm command
*
*****************************************************************************/
void AgentController::OnCmdAxedaAlarm(Global::tRefType ref, const CmdAxedaAlarm &cmd)
{
    // get all formats right
    QByteArray bAname = cmd.GetAlarmName().toUtf8();
    QByteArray bAmessage = cmd.GetAlarmMessage().toUtf8();
    QByteArray bAcondition = cmd.GetAlarmCondition().toUtf8();
    QString timestamp = cmd.GetAlarmTimestamp();

    /// \todo: any data check here?

    // try to forward the alarm
    if (QObject::receivers(SIGNAL(SigAlarmRequest(quint64, QByteArray, QByteArray, const QString &, QByteArray, const QString &))) <= 0) {
        // we are not connected --> epic fail.
        qDebug() << "AgentController::OnCmdAxedaAlarm:  epic fail. We are not connected!";
        /// \todo:  Nack handling
        return;
    }
    // good. we are connected
    emit SigAlarmRequest((quint64)ref, bAname, bAmessage, cmd.GetAlarmSeverity(), bAcondition, timestamp);
    /// \todo:  timeout & ack handling TBD
}

/****************************************************************************/
/*!
*  \brief  Convert data and forward event to Agent
*
*  \param[in]    ref = reference of the request
*  \param[in]    cmd = incoming Event command
*
*****************************************************************************/
void AgentController::OnCmdAxedaEvent(Global::tRefType ref, const CmdAxedaEvent &cmd)
{
    // get all formats right
    QByteArray bAname = cmd.GetEventName().toUtf8();
    QByteArray bAmessage = cmd.GetEventMessage().toUtf8();
    QString timestamp = cmd.GetEventTimestamp();

    /// \todo: any data check here?

    // try to forward the event
    if (QObject::receivers(SIGNAL(SigEventRequest(quint64, QByteArray, QByteArray, const QString &, const QString &))) <= 0) {
        // we are not connected --> epic fail.
        qDebug() << "AgentController::OnCmdAxedaEvent:  epic fail. We are not connected!";
        /// \todo:  Nack handling
        return;
    }
    // good. we are connected
    emit SigEventRequest((quint64)ref, bAname, bAmessage, cmd.GetEventSeverity(), timestamp);
    /// \todo:  timeout & ack handling TBD
}

/****************************************************************************/
/*!
*  \brief  Convert data and forward dataitem to Agent
*
*  \param[in]    ref = reference of the request
*  \param[in]    cmd = incoming DataItem command
*
*****************************************************************************/
void AgentController::OnCmdAxedaDataItem(Global::tRefType ref, const CmdAxedaDataItem &cmd)
{
    // get all formats right
    QByteArray bAname = cmd.GetDataItemName().toUtf8();
    AxedaDataItemType_t type = cmd.GetDataItemType();
    AxedaDataItemQuality_t quality = cmd.GetDataItemQuality();
    QByteArray bAvalue = cmd.GetDataItemValue().toUtf8();
    QString timestamp = cmd.GetDataItemTimestamp();

    /// \todo: any data check here?

    // try to forward the event
    if (QObject::receivers(SIGNAL(SigDataItemRequest(quint64, QByteArray, AxedaDataItemType_t, AxedaDataItemQuality_t, QByteArray, const QString &))) <= 0) {
        // we are not connected --> epic fail.
        qDebug() << "AgentController::OnCmdAxedaDataItem:  epic fail. We are not connected!";
        /// \todo:  Nack handling
        return;
    }
    // good. we are connected
    emit SigDataItemRequest((quint64)ref, bAname, type, quality, bAvalue, timestamp);
    /// \todo:  timeout & ack handling TBD
}

/****************************************************************************/
/*!
*  \brief  Convert data and forward upload request to Agent
*
*  \param[in]    ref = reference of the request
*  \param[in]    cmd = incoming Upload command
*
*****************************************************************************/
void AgentController::OnCmdAxedaUpload(Global::tRefType ref, const CmdAxedaUpload &cmd)
{
    // get all formats right
    QString filename = Global::SystemPaths::Instance().GetUploadsPath() + AGENT_SLASH_IN_PATH + cmd.GetUploadFileName();
    if (!QFile::exists(filename)) {
        // ooops. the file does not exist!
        qDebug() << "AgentController::OnCmdAxedaUpload: epic fail. The " << filename << " file does not exist!";
        /// \todo:  Nack handling
        return;
    }

    QByteArray bAname = cmd.GetUploadFileName().toUtf8();
    // try to forward the event
    if (QObject::receivers(SIGNAL(SigUploadRequest(quint64, QByteArray))) <= 0) {
        // we are not connected --> epic fail.
        qDebug() << "AgentController::OnCmdAxedaUpload:  epic fail. We are not connected!";
        /// \todo:  Nack handling
        return;
    }
    // good. we are connected
    emit SigUploadRequest((quint64)ref, bAname);
    /// \todo:  timeout & ack handling TBD
}

/****************************************************************************/
/*!
*  \brief  Enable/disable Remote Sessions on Agent
*
*  \param[in]    ref = reference of the request
*  \param[in]    cmd = incoming command
*
*****************************************************************************/
void AgentController::OnCmdAxedaRemoteSessionStatus(Global::tRefType ref, const CmdAxedaRemoteSessionStatus &cmd)
{
    Q_UNUSED(ref)
    emit SigSetRemoteSessionStatus(cmd.GetRemoteSessionStatus());
}

/****************************************************************************/
/*!
*  \brief   This debug function initializes the shared memory monitor.
*           In debug mode the AgentController can be shutdown with
*           the "SharedMemShutdown" (see Tools folder at the SVN trunk root).
*
****************************************************************************/
void AgentController::InitializeShutDownMonitor()
{
    if (!QObject::connect(&m_DebugMonitorTimer, SIGNAL(timeout()),
                          this, SLOT(RunShutDownMonitor()))) {
        qDebug() << "AgentController: cannot connect the debug timer signal !";
        return;
    }
    m_DebugMonitorTimer.start(950);
}

/****************************************************************************/
/*!
*  \brief   This debug function checks the shared memory and initiates
*           a shutdown if it was requested.
*
****************************************************************************/
void AgentController::RunShutDownMonitor()
{
    QSharedMemory ShMem(DEBUG_SHARED_MEM_KEY);

    // attach to shared memory which should have been created by external debug process
    if (!ShMem.attach()) {
        // well, no memory - no exit.
        return;
    }
    // lock ShMem and check its data
    if (ShMem.lock()) {
        unsigned char *pData = reinterpret_cast<unsigned char *>(ShMem.data());
        if (pData != NULL) {
            if (*pData == static_cast<unsigned char>(1)) {
                // Agent was requested to quit
                // first, reset the flag to default
                *pData = static_cast<unsigned char>(0);
                // now stop and exit
                Stop();
                /// \todo: not only stop, but exit here too.
            }
        }
        // do not forget to unlock ShMem
        static_cast<void>(ShMem.unlock());
    }
    static_cast<void>(ShMem.detach());
}

} // end namespace
