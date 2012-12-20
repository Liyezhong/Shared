/****************************************************************************/
/*! \file NetworkDevice.cpp
 *
 *  \brief NetworkDevice implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16.11.2010
 *   $Author:  $ Y.Novak
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

#include <NetworkComponents/Include/ProtocolTxCommand.h>
#include <NetworkComponents/Include/ProtocolRxCommand.h>
#include <NetworkComponents/Include/NetworkDevice.h>
#include <Global/Include/TranslatableString.h>
#include <Global/Include/GlobalEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>

/****************************************************************************/
/**
 * \brief Namespace containing all TCP-IP networking base classes.
 */
/****************************************************************************/
namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief  Constructor
 *
 *  \param[in]  ptype - type of message loader
 *  \param[in]  path - path to settings
 *  \param[in]  pParent - pointer to the parent object
 */
/****************************************************************************/
NetworkDevice::NetworkDevice(MessageLoaderType_t ptype, const QString &path, QObject *pParent) :
        QObject(pParent),
        m_myMessageChecker(NULL),
        m_myType(ptype),
        m_myPath(path),
        m_cmdRef(0)
{
    RunningCommands.clear();
    switch (ptype) {
    case CML_TYPE_SERVER:
        m_HBDelay = HEARTBEAT_SERVER_DELAY;
        break;
    case CML_TYPE_CLIENT:
        m_HBDelay = HEARTBEAT_CLIENT_DELAY;
        break;
    default:
        m_HBDelay = HEARTBEAT_SERVER_DELAY;
        break;
    }
}

/****************************************************************************/
/*!
 *  \brief    Destructor
 *
 ****************************************************************************/
NetworkDevice::~NetworkDevice()
{
    try {
        if(m_myMessageChecker != NULL) {
            delete m_myMessageChecker;
        }
        m_myMessageChecker = NULL;

        // delete all still running commands
        QHash<quint32, ProtocolTxCommand*>::iterator itr;
        for (itr = RunningCommands.begin(); itr != RunningCommands.end(); ++itr) {
            if (itr.value() != NULL) {
                delete itr.value();
            }
        }
    } catch (...) {
        // to please PCLint...
    }
}

/****************************************************************************/
/*!
 *  \brief    API function to call before NetworkDevice can be used
 *
 *      This function does everything to initialize the NetworkDevice.
 *      It shall be called before NetworkDevice can be used.
 *
 *  \return   true if success, false if error
 *
 ****************************************************************************/
bool NetworkDevice::InitializeDevice()
{
    if (!InitializeMessaging()) {
        /// \todo: handle error
        qDebug() << "NetworkDevice: cannot initialize Messaging !";
        return false;
    }

    try {
        if (m_myType == CML_TYPE_SERVER) {
            // for server we need to connect to HeartBeat, not to HeartBeatTimeout:
            CONNECTSIGNALSLOT(&m_HeartBeatTimer, timeout(), this, HandleHeartBeat());
        } else if (m_myType == CML_TYPE_CLIENT)  {
            CONNECTSIGNALSLOT(&m_HeartBeatTimer, timeout(), this, HandleHeartBeatTimeout());
        } else {
            qDebug() << "NetworkDevice: cannot initialize wrong type of device : " << static_cast<int>(m_myType);
            return false;
        }
    } catch (...) {
        /// \todo: handle error
        return false;
    }

    // configure static members of abstract factory commands
    BaseTxPCmd::Initialize();
    BaseRxPCmd::Initialize();

    return true;
}

/****************************************************************************/
/*!
 *  \brief Initialize the ProtocolHandler for communication with local client.
 *
 *
 *  \return     true if success, false otherwise
 */
/****************************************************************************/
bool NetworkDevice::InitializeMessaging()
{
    qDebug() << "NetworkDevice: initializing ProtocolHandler..." << m_myPath;

    try {
        if (m_myMessageChecker == NULL) {
            m_myMessageChecker = new MessageChecker(m_myType, CMH_CONFIGFILE_TYPE, m_myPath, this);
        }
        // load protocol configuration
        if (!m_myMessageChecker->Initialize()) {
            /// \todo: handle error
            qDebug() << "NetworkDevice: cannot load messages";
            delete m_myMessageChecker;
            m_myMessageChecker = NULL;
            return false;
        }
    }
    catch (const std::bad_alloc &) {
        /// \todo: handle error
        qDebug() << "NetworkDevice: Cannot allocate memory!";
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief   Fetch new reference number to create a command.
 *
 *  \warning Generated Reference shall never be ZERO.
 *           ZERO is a special case which means "no reference exists", e.g.
 *           the command is not an Ack.
 *
 *  \return  unique command reference
 */
/****************************************************************************/
quint32 NetworkDevice::GetNewCmdReference()
{
    ++m_cmdRef;
    if (m_cmdRef == 0) {
        ++m_cmdRef;
    }
    return m_cmdRef;
}

/****************************************************************************/
/*!
 *  \brief  This method inserts command to the list of running commands.
 *
 *          Any created command, which wants to live longer than for the
 *          duration of the "Execute" call, shall insert itself into this
 *          list. E.g. if command sends request and waits for answer, etc.
 *
 *  \param[in]  ref - unique command reference
 *  \param[in]  cmd - pointer to the command object
 */
/****************************************************************************/
void NetworkDevice::RegisterRunningCommand(quint32 ref, ProtocolTxCommand *cmd)
{
    //qDebug() << (QString)("NetworkDevice: Command registered with network reference: " + QString::number(ref, 10));
    if (RunningCommands.contains(ref)) {
        /// \todo: handle error
        // Command with this reference is already running, this shall never happen!
        qDebug() << "NetworkDevice: command with this REF is already in hash !";
        return;
    }
    RunningCommands.insert(ref, cmd);
}

/****************************************************************************/
/*!
 *  \brief  This method delets command from the list of running commands.
 *
 *          As soon as command has finished, it shall call this function to
 *          remove itself from the list of running commands.
 *
 *  \param[in]  ref - unique command reference
 *  \param[in]  cmd - pointer to the command object
 */
/****************************************************************************/
void NetworkDevice::DeregisterRunningCommand(quint32 ref, ProtocolTxCommand *cmd)
{
    //qDebug() << (QString)("NetworkDevice: Command de-registered with network reference: " + QString::number(ref, 10));
    RunningCommands.remove(ref);
    if (cmd != NULL) {
        cmd->deleteLater();
    }
}

/****************************************************************************/
/*!
 *  \brief  This method returns a pointer to the running command.
 *
 *          When reply from peer is received, this function shall be used
 *          to fetch a command which shall process this reply.
 *
 *  \param[in]  ref - unique command reference
 *
 *  \return pointer to ProtocolCommand or NULL if no such element in Hash
 */
/****************************************************************************/
ProtocolTxCommand* NetworkDevice::FetchRunningCommand(quint32 ref)
{
    if(RunningCommands.contains(ref)) {
        return RunningCommands.value(ref);
    }
    return NULL;
}

/****************************************************************************/
/*!
 *  \brief  This member reports missing HeartBeats to upper layers
 *
 *  \param[in] problem = error description
 */
/****************************************************************************/
void NetworkDevice::ReportHeartBeatProblem(const QString &problem)
{
    emit SigHeartBeatError(problem);
}

/****************************************************************************/
/*!
 *  \brief  This member reports problem with DateTime sync process
 *
 *  \param[in] problem = error description
 */
/****************************************************************************/
void NetworkDevice::ReportFailedDateTimeSync(const QString &problem)
{
    emit SigDateTimeSyncError(problem);
}

/****************************************************************************/
/*!
 *  \brief  This method sends HeartBeat to check peer's sanity.
 *
 */
/****************************************************************************/
void NetworkDevice::HandleHeartBeat()
{
    m_HeartBeatTimer.stop();
    m_HeartBeatTimer.start(m_HBDelay);
    qDebug() << "NetworkDevice: HandleHeartBeat() called, HeartBeat Timer started.";

    // create command processing object
    ProtocolTxCommand *pC = CreateNewOutgoingCommand<ProtocolTxCommand, BaseTxPCmd>((QString)"HeartBeat");
    if (pC == NULL) {
        qDebug() << "NetworkDevice: creating HeartBeat command failed !";
        ReportHeartBeatProblem(CMH_MSG_SENDING_FAILED);
        return;
    }

    // run created command:
    if (!pC->Execute()) {
        qDebug() << "NetworkDevice: command execution failed :     HeartBeat";
        ReportHeartBeatProblem(CMH_MSG_SENDING_FAILED);
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief  This method restarts HeartBeat timeout.
 *
 *          HeartBeat incoming command shall restart timer if correct
 *          HeartBeat is received before timeout.
 */
/****************************************************************************/
void NetworkDevice::RestartHeartBeatTimer()
{
    // restart the timer
    m_HeartBeatTimer.stop();
    m_HeartBeatTimer.start();

    qDebug() << "NetworkDevice: Heartbeat timer restarted.";
}

/****************************************************************************/
/*!
 *  \brief  This method acts on HeartBeat timeout.
 *
 */
/****************************************************************************/
void NetworkDevice::HandleHeartBeatTimeout()
{
    // stop the timer
    m_HeartBeatTimer.stop();
    qDebug() << "NetworkDevice: Heartbeat TIMEDOUT !!!";
    // act on missing heartbeat
    ReportHeartBeatProblem(CMH_MSG_SENDING_TIMEOUT);
}

/****************************************************************************/
/*!
 *  \brief  This method processes peer connection event.
 *
 *          All periodic peer activities shall be started till peer
 *          disconnects again.
 */
/****************************************************************************/
void NetworkDevice::PeerConnected(const QString &name)
{
    // start periodic heartbeat timer:
    m_HeartBeatTimer.start(m_HBDelay);
    qDebug() << "NetworkDevice: HeartBeat Timer started.";
    // inform whoever is interested in this event
    emit SigPeerConnected(name);
}

/****************************************************************************/
/*!
 *  \brief  This method processes peer disconnection event.
 *
 *          All periodic perr activities shall be stopped till peer
 *          connects again.
 */
/****************************************************************************/
void NetworkDevice::PeerDisconnected(const QString &name)
{
    // stop periodic heartbeat timer:

    m_HeartBeatTimer.stop();
    qDebug() << "NetworkDevice: HeartBeat Timer stopped.";

    // inform whoever is interested in this event
    emit SigPeerDisconnected(name);
}

/****************************************************************************/
/*!
 *  \brief    This function acts on user requested NetworkDevice
 *            disconnect from peer
 *
 ****************************************************************************/
void NetworkDevice::DisconnectPeer()
{
    // stop periodic heartbeat timer:
    m_HeartBeatTimer.stop();
}

/****************************************************************************/
/*!
 *  \brief    This SLOT receives an incoming Message from Server
 *
 *      Implement this function to process incoming from Server messages.
 *      Server shall become a pointer to object of type ProtocolHandler.
 *      Signal/Slot connections/disconnections of this object are handled
 *      by the Server.
 *
 *  \param    type = type of incoming message
 *  \param    ba = incoming XML message as byte array
 *
 *  \warning  ba is consciously non-const because this function will change it
 *
 ****************************************************************************/
void NetworkDevice::GetIncomingMsg(quint8 type, QByteArray &ba)
{
    qDebug() << "xxxxxxx NetworkDevice::GetIncomingMsg, path="
             << this->m_myPath
             << ", myType=" << this->m_myType
             << ", msgType=" << type
             << ", data=" << ba.toUInt();
    switch (static_cast<NetMessageType_t>(type)) {
    case NET_NETLAYER_MESSAGE:
        ParseNetLayerMessage(ba);
        break;
    case NET_APPLICATION_MESSAGE:
        ParseApplicationMessage(ba);
        break;
    default:
        /// \todo: handle error?
        // unknown message type: reply with error?
        break;
    }

    /// \todo: if reception fails at this stage, shall an error message be sent here?
}

/****************************************************************************/
/*!
 *  \brief    This function parses an incoming XML message
 *
 *            This function parses an incoming XML message and forwards
 *            it to the corresponding message processor
 *
 *  \param    ba = the incoming XML message
 *
 ****************************************************************************/
void NetworkDevice::ParseNetLayerMessage(const QByteArray &ba)
{
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument msg;
    if (!msg.setContent(ba, true, &err, &errL, &errC)) {
        /// \todo: handle error(?)
        // emit error signal
        emit MessageParsingFailed();
        qDebug() << (QString)("NetworkDevice: cannot parse incoming QByteArray's message! Error: " + err);
        return;
    }

    // get the name of the command and check schema
    QString cmdname = "";
    if (!CheckIncomingMsgSchema(&msg, &cmdname)) {
        // emit error signal
        /// \todo: handle error(?)
        emit MessageParsingFailed();
        return;
    }

    // call corresponding message handler
    if (!ProcessIncomingMessage(cmdname, msg)) {
        /// \todo: handle error(?)
        emit MessageParsingFailed();
    }
}

/****************************************************************************/
/*!
 *  \brief    This function parses an incoming message with byte array payload
 *
 *            This function parses an incoming file message and forwards
 *            it to the corresponding message processor
 *
 *  \param    ba = incoming message's payload
 *
 *  \warning  ba is consciously non-const
 *
 ****************************************************************************/
void NetworkDevice::ParseApplicationMessage(QByteArray &ba)
{
    QDomDocument msg;
    if (!ExtractProtocolMessage(&ba, &msg)) {
        // emit error signal
        /// \todo: handle error(?)
        emit MessageParsingFailed();
        return;
    }
    // get the name of the command
    QString cmdname = ExtractCommandName(&msg);
    QString ref = ExtractCommandReference(&msg);
    // call corresponding message handler (in the derived class)
    static_cast<void> (
        // no use for the return value here: application shall handle errors in this case.
        ProcessIncomingMessage(cmdname, ref, ba)
    );
}

/****************************************************************************/
/*!
 *  \brief  This method processes an incomig from peer device message.
 *
 *          This method is called by the ProtocolHandler.
 *
 *  \param[in]  cmdname - name of the command
 *  \param[in]  domD - message as XML document
 *
 *  \return true if success, false otherwise
 */
/****************************************************************************/
bool NetworkDevice::ProcessIncomingMessage(const QString &cmdname, const QDomDocument &domD)
{
    //qDebug() << "NetworkDevice::ProcessIncomingMessage :  command " << cmdname << " received !";
    // create command processing object
    ProtocolRxCommand *pC = CreateNewIncomingCommand<ProtocolRxCommand, BaseRxPCmd>(cmdname, domD);
    if (pC == NULL) {
        /// \todo: handle error
        qDebug() << "NetworkDevice: command creation failed :" << cmdname;
        return false;
    }
    // run created command:
    bool result = pC->Execute();
    // done, delete the command:
    delete pC;

    return result;
}

/****************************************************************************/
/*!
 *  \brief  This method processes an incomig from peer device message.
 *
 *          This method is called by the ProtocolHandler.
 *
 *  \param[in]  cmdname - name of the command
 *  \param[in]  ref - net message reference
 *  \param[in]  bA  - message payload as byte array
 *
 *  \return true if success, false otherwise
 */
/****************************************************************************/
bool NetworkDevice::ProcessIncomingMessage(const QString &cmdname, const QString &ref, const QByteArray &bA)
{
    Q_UNUSED(cmdname)
    Q_UNUSED(ref)
    Q_UNUSED(bA)
    // Base class is not (at least for now) supposed to handle this command type,
    // therefore return false:
    return false;
}

/****************************************************************************/
/*!
 *  \brief    This function extracts cmd name and checks XML schema
 *
 *
 *  \param[in]    msg = pointer to the XML message
 *  \param[out]   CmdName = pointer to command name
 *
 *  \return   true if schema check was successfull,
 *            false otherwise
 *
 ****************************************************************************/
bool NetworkDevice::CheckIncomingMsgSchema(QDomDocument *msg, QString *CmdName)
{
    // get the name of the command
    *CmdName = ExtractCommandName(msg);

    // Fetch the XML schema here and check the message format.
    // Thus commands do not need to check schemas. It is done before command is instantiated.
    if (m_myMessageChecker != NULL) {
        if (!m_myMessageChecker->CheckMsgSchema(*CmdName, msg)) {
            /// \todo: handle error
            qDebug() << ((QString)"NetworkDevice: msg parsing failed -> schema check failed !");
            return false;
        }
    }
    else {
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief    This function extracts command name from incoming message
 *
 *
 *  \param    domD   = pointer to the incoming XML message
 *
 *  \return   Command Name if success, Empty String if failure
 *
 ****************************************************************************/
QString NetworkDevice::ExtractCommandName(QDomDocument *domD)
{
    QDomElement root = domD->documentElement();

    QDomElement elmt = root.firstChildElement(CMH_CMD_TAG_NAME);
    if (elmt.isNull()) {
        qDebug() << "NetworkDevice: no cmd element !";
        // calling function shall send an error reply
        return (QString)"";
    }
    QString cmdname = elmt.attribute(CMH_CMDNAME);
    if (cmdname == "") {
        // calling function shall send an error reply
        qDebug() << ((QString)"NetworkDevice: msg parsing failed -> cmd element empty !");
        return (QString)"";
    }

    return cmdname;
}

/****************************************************************************/
/*!
 *  \brief    This function extracts command reference from incoming message
 *
 *
 *  \param    domD   = pointer to the incoming XML message
 *
 *  \return   Command Name if success, Empty String if failure
 *
 ****************************************************************************/
QString NetworkDevice::ExtractCommandReference(QDomDocument *domD)
{
    QDomElement root = domD->documentElement();

    QDomElement elmt = root.firstChildElement(CMH_CMD_TAG_NAME);
    if (elmt.isNull()) {
        qDebug() << "NetworkDevice: no cmd element !";
        // calling function shall send an error reply
        return (QString)"";
    }
    QString ref = elmt.attribute(CMH_REFERENCE);
    if (ref == "") {
        // calling function shall send an error reply
        qDebug() << ((QString)"NetworkDevice: msg parsing failed -> cmd element empty !");
        return (QString)"";
    }

    return ref;
}

/****************************************************************************/
/*!
 *  \brief    This function extracts a Protocol Message from incoming File Message
 *
 *
 *  \param[in,out] ba = incoming XML message as byte array
 *  \param[out]    message = extracted protocol message
 *
 *  \return true if extraction succeeded, false otherwise
 *
 ****************************************************************************/
bool NetworkDevice::ExtractProtocolMessage(QByteArray *ba, QDomDocument *message)
{
    QString checkstring = CMH_MSG_OFF_TAG;
    int Index = ba->indexOf(checkstring);
    if (Index == -1) {
        /// \todo: handle error
        qDebug() << "NetworkDevice: incoming QByteArray message is broken !";
        return false;
    }
    // read protocol message to string
    Index = Index + checkstring.size();
    QByteArray batemp(ba->data(), Index);
    QString msg(batemp);

    // remove the protocol message from array
    ba->remove(0, Index);

    // convert message to QDomDocument
    QString err = "";
    int errL = 0;
    int errC = 0;
    if (!message->setContent(msg, true, &err, &errL, &errC)) {
        // emit error signal
        emit MessageParsingFailed();
        qDebug() << (QString)("NetworkDevice: cannot parse incoming QByteArray's message! Error: " + err);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief    This function sends a message to peer over TCP Socket
 *
 *
 *  \param    cmd = outgoing message
 *
 *  \return   True if sending command succeeds, false otherwise
 *
 ****************************************************************************/
bool NetworkDevice::SendCommand(const QString &cmd)
{
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    if (!domD.setContent(cmd, true, &err, &errL, &errC)) {
        qDebug() << (QString)("NetworkDevice: cannot convert outgoing QString message to QDomDocument! Error: " + err);
        return false;
    }
    QByteArray ba = domD.toByteArray(-1);
    if (ba.isEmpty()) {
        qDebug() << (QString)("NetworkDevice: outgoing QByteArray is empty !");
        return false;
    }
    // if input is just a string, it is automatically NET_NETLAYER_MESSAGE type
    return SendCommand(NET_NETLAYER_MESSAGE, ba);
}

/****************************************************************************/
/*!
 *  \brief    This function sends a message over TCP Socket
 *
 *
 *  \param    cmdtype = type of command
 *  \param    ba = outgoing byte array
 *
 *  \return   True if signal is connected and was emitted, False otherwise
 *
 ****************************************************************************/
bool NetworkDevice::SendCommand(NetMessageType_t cmdtype, const QByteArray &ba)
{
    qDebug() << "xxxxx NetworkDevice::SendCommand";
    // Check for valid "connection" to SLOT. If signal is not
    // connected, report error.
    if (receivers(SIGNAL(SendMessage(quint8, const QByteArray &))) == 0) {
        /// \todo: how do we report it to sender?
        return false;
    }
    // there is a valid connection, send message:
    emit SendMessage(static_cast<quint8>(cmdtype), ba);
    return true;
}

/****************************************************************************/
/*!
 *  \brief  This method creates and sends a new outgoing protocol command.
 *
 *  \param[in]  cmdname - name of the command
 *  \param[in]  bArray - payload as byte array
 *  \param[in]  Ref - reference of the application command
 *
 */
/****************************************************************************/
void NetworkDevice::SendOutgoingCommand(const QString &cmdname, const QByteArray &bArray, Global::tRefType Ref)
{
    QString status = CMH_MSG_SENDING_FAILED;
    try {
        // create command processing object
        ProtocolTxCommand *pC = new ProtocolTxCommand();
        qDebug() << (QString)("ProtocolTxCommand created : " + cmdname);
        // generate reference:
        quint32 netRef = GetNewCmdReference();
        // init the outgoing command
        if (!pC->Initialize(cmdname, netRef, Ref, bArray, this)) {
            // command init failed: inform upper layer
            qDebug() << (QString)("ProtocolTxCommand " + cmdname + " cannot be initialized !");
            delete pC;
            MessageSendingResult(Ref, status);
            return;
        }
        // init ok, now send the command
        if (!pC->Execute()) {
            // get rid of the command since it did not send anything
            pC->DismissCommand(); // this funciton will call "deleteLater" on the command
            // command sending failed: inform upper layer
            MessageSendingResult(Ref, status);
            return;
        }
        // all OK. do not delete the command here, it will delete itself after ACK/NACK/Timeout !
        // set the sending status to OK though
        status = CMH_MSG_SENDING_OK;
    }
    catch (const std::bad_alloc &) {
        /// \todo: log error?
        qDebug() << (QString)("ProtocolTxCommand " + cmdname + " cannot be initialized !");
    }
    // operation failed: inform upper layer
    MessageSendingResult(Ref, status);
}

/****************************************************************************/
/*!
 *  \brief  This method informs upper layer of command execution result
 *
 *  \param[in]  ref - application command reference
 *  \param[in]  status - command execution status (Ack/Nack/Timeout/CannotBeSent)
 */
/****************************************************************************/
void NetworkDevice::MessageSendingResult(Global::tRefType ref, const QString &status)
{
    if (receivers(SIGNAL(SigMsgSendingResult(Global::tRefType, const QString &))) > 0) {
        emit SigMsgSendingResult(ref, status);
    }
    else {
        /// \todo: is it an error?
        //qDebug() << "\nNetworkDevice::MessageSendingResult: signal SigMsgSendingResult is not connected !";
        //qDebug() << "NetworkDevice::MessageSendingResult: Ref = " << ref << ", Status = " << status << "\n";
    }
}


} // end namespace NetworkBase
