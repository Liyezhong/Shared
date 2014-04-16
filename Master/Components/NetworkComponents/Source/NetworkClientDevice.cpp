/****************************************************************************/
/*! \file NetworkClientDevice.cpp
 *
 *  \brief NetworkClientDevice implementation.
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

#include <NetworkComponents/Include/NetworkClientDevice.h>
#include <NetworkComponents/Include/ProtocolRxCommands/Ack.h>
#include <Global/Include/TranslatableString.h>
#include <Global/Include/GlobalEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>
#include <NetworkComponents/Include/NetworkComponentEventCodes.h>
#include <Global/Include/EventObject.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief    Constructor
 *
 *  \iparam ctype = type of client to create
 *  \iparam ip = server ip to connect to
 *  \iparam port = server port to connect to
 *  \iparam path = path to settings folder
 *  \iparam pParent = pointer to parent
 *
 ****************************************************************************/
NetworkClientDevice::NetworkClientDevice(NetworkClientType_t ctype, const QString &ip, const QString &port, const QString &path, QObject *pParent) :
        NetworkDevice::NetworkDevice(CML_TYPE_CLIENT, path, pParent),
        m_myNetworkClient(NULL),
        m_myClientType(ctype),
        m_myIp(ip),
        m_myPort(port)
{
    this->setParent(pParent);
}

/****************************************************************************/
/*!
 *  \brief    Destructor
 *
 *
 ****************************************************************************/
NetworkClientDevice::~NetworkClientDevice()
{
    try {
        if (m_myNetworkClient != NULL) {
            delete m_myNetworkClient;
        }
        m_myNetworkClient = NULL;
    }
    CATCHALL_DTOR();
}

/****************************************************************************/
/*!
 *  \brief    API function to call before NetworkClient Device can be used
 *
 *      This function does everything to initialize the NetworkClient Device.
 *      It shall be called before NetworkClient Device can be used.
 *
 *  \return   true if success, false if error
 *
 ****************************************************************************/
bool NetworkClientDevice::InitializeDevice()
{
    qDebug() << "NetworkClientDevice: initializing...";

    try {
        if (!NetworkDevice::InitializeDevice()) {
            qDebug() << "NetworkClientDevice: cannot initialize my NetworkDevice !";
            /// \todo: handle error
            Global::EventObject::Instance().RaiseEvent(EVENT_NCD_INIT_FAILED,
                                                       Global::tTranslatableStringList() << m_myIp);
            return false;
        }

        if (!InitializeNetwork()) {
             qDebug() << "NetworkClientDevice: cannot initialize NetworkClient !";
            /// \todo: handle error
             Global::EventObject::Instance().RaiseEvent(EVENT_NCD_INIT_FAILED,
                                                        Global::tTranslatableStringList() << m_myIp);
             return false;
        }

        // connect connection signals
        CONNECTSIGNALSLOT(m_myNetworkClient, ConnectionEstablished(QString), this, PeerConnected(QString));
        // connect disconnection signals
        CONNECTSIGNALSLOT(m_myNetworkClient, ConnectionLost(QString), this, PeerDisconnected(QString));
        // connect error signal
        CONNECTSIGNALSLOT(m_myNetworkClient, ConnectionFailed(NetworkBase::NetworkClientErrorType_t),
                          this, HandleNetworkError(NetworkBase::NetworkClientErrorType_t));
        CONNECTSIGNALSIGNAL(m_myNetworkClient, StartConnectionLostTimer(),
                              this, StartConnectionLostTimer())

        return true;
    }
    CATCHALL();
    /// \todo: handle error

    return false;
}

/****************************************************************************/
/*!
 *  \brief    Initialize the TCPIP Client for communication with peer.
 *
 *
 *  \return   true if success, false otherwise
 */
/****************************************************************************/
bool NetworkClientDevice::InitializeNetwork()
{
    try {
        if (m_myNetworkClient == NULL) {
            m_myNetworkClient = new NetworkClient(m_myClientType, m_myIp, m_myPort, this);
        }
        // tell client which message handler to use:
        if (!m_myNetworkClient->RegisterMessageHandler(this)) {
            qDebug() << "NetworkClientDevice: cannot register message handler !";
            delete m_myNetworkClient;
            m_myNetworkClient = NULL;
            Global::EventObject::Instance().RaiseEvent(EVENT_NCD_MSGHANDLER_REGISTRATION_FAILED,
                                                       Global::tTranslatableStringList() << m_myIp);
            return false;
        }
        if (!m_myNetworkClient->Initialize()) {
            qDebug() << "NetworkClientDevice: NetClient init failed !";
            delete m_myNetworkClient;
            m_myNetworkClient = NULL;
            Global::EventObject::Instance().RaiseEvent(EVENT_NCD_INIT_FAILED,
                                                       Global::tTranslatableStringList() << m_myIp);
            return false;
        }
    }
    CATCHALL_RETURN(false)

    qDebug() << "NetworkClientDevice: Client initialized.";
    return true;
}

/****************************************************************************/
/*!
 *  \brief  This method starts NetworkClient Device operation.
 *
 *          This method shall be called after the successful
 *          InitializeDevice call.
 *
 *  \return  true if success, false otherwise
 */
/****************************************************************************/
bool NetworkClientDevice::StartDevice()
{
    if (m_myNetworkClient == NULL) {
        qDebug() << (QString)("NetworkClientDevice: my Client is NULL !");
        Global::EventObject::Instance().RaiseEvent(EVENT_NL_NULL_POINTER,
                                                   Global::tTranslatableStringList() << m_myIp << FILE_LINE);
        return false;
    }
    // The client will automatically ping Server till connection is established.
    m_myNetworkClient->ConnectToServer();
    qDebug() << (QString)("NetworkClientDevice: sent connect request to peer... ");

    return true;
}

/****************************************************************************/
/*!
 *  \brief  This method processes an incomig from peer device message.
 *
 *
 *  \iparam  cmdname - name of the command
 *  \iparam  ref - message reference
 *  \iparam  bArr - message payload
 *
 *  \warning   if other message delivery method is required, just re-implement
 *             this virtual function in the derived class.
 *
 *  \return True if message was successfully processed, false otherwise
 */
/****************************************************************************/
bool NetworkClientDevice::ProcessIncomingMessage(const QString &cmdname, const QString &ref, const QByteArray &bArr)
{
    //qDebug() << "\nNetworkClientDevice: " << cmdname << " with network reference: " << ref << " received !\n";
    bool result = false;
    QString status = NetworkBase::CMH_MSG_SENDING_NACK;

    if (cmdname.isEmpty() || ref.isEmpty()) {
        qDebug() << "\nNetworkClientDevice: oops. missing input !\n";
        static_cast<void> (NetworkBase::Ack::SendAcknowledge(this, ref, cmdname, status));
        Global::EventObject::Instance().RaiseEvent(EVENT_NCD_MISSING_INPUT,
                                                   Global::tTranslatableStringList() << m_myIp);
        return result;
    }
    // check if signal is connected and if yes --> forward message
    if (receivers(SIGNAL(ForwardMessageToUpperLayer(const QString &, const QByteArray &))) > 0) {
        /// \todo if ACK from upper layer is needed (e.g. to make sure the upper layer could
        ///       decode the data) we'd need to supply NetRef in this signal and implement
        ///       another Signal-Slot pair for return ACK.
        emit ForwardMessageToUpperLayer(cmdname, bArr);
        // mission accomplished (?). send Ack and return.
        static_cast<void> (NetworkBase::Ack::SendAcknowledge(this, ref, cmdname, NetworkBase::CMH_MSG_SENDING_ACK));
        return true;
    }

    // if we got here, it means the signal was not sent --> report error
    static_cast<void> (NetworkBase::Ack::SendAcknowledge(this, ref, cmdname, status));
    return result;
}

/****************************************************************************/
/*!
 *  \brief     Destroy NetworkClient Device.
 *
 */
/****************************************************************************/
void NetworkClientDevice::DestroyDevice()
{
    // disconnect from server
    DisconnectPeer();
    // delete all instantiated objects:
    if (m_myNetworkClient != NULL) {
        delete m_myNetworkClient;
    }
    m_myNetworkClient = NULL;
    qDebug() << (QString)("NetworkClientDevice: Device destroyed.");
}

/****************************************************************************/
/*!
 *  \brief    This slot handles TCP Client network errors
 *
 *  \param    error = client's error code
 *
 ****************************************************************************/
void NetworkClientDevice::HandleNetworkError(NetworkClientErrorType_t error)
{
    qDebug() << "NetworkClientDevice: Network error happend: " + QString::number(static_cast<int>(error), 10);
    Global::EventObject::Instance().RaiseEvent(EVENT_NCD_NETWORK_ERROR,
                                               Global::tTranslatableStringList() << m_myIp
                                               << QString::number((int)error));

    switch (error) {
    case NC_SIGNAL_CONNECT_FAILED:
        // fatal errors, client cannot work
        /// \todo: log fatal error
        break;
    case NC_WRONG_SERVER_MESSAGE:
    case NC_AUTHENTICATION_FAILED:
    case NC_MSGHANDLER_POINTER_NULL:
        /// \todo: this is fatal, but Client re-initialization might help
        ///        re-start client here?
        break;
    default:
        /// \todo: all others just need to be logged
        break;
    } //switch
}

/****************************************************************************/
/*!
 *  \brief    This function disconnects Network Client Device from peer
 *
 *
 ****************************************************************************/
void NetworkClientDevice::DisconnectPeer()
{
    // stop periodic heartbeat timer:
    NetworkDevice::DisconnectPeer();
    qDebug() << "NetworkClientDevice: peer disconnected.";

    // disconnect the client
    if (m_myNetworkClient != NULL) {
        m_myNetworkClient->DisconnectFromServer();
    }
}

/****************************************************************************/
/*!
 *  \brief   Forward incoming Date and Time to the application.
 *
 *  \iparam  ref = incoming network layer message reference
 *  \iparam  qdt = the date and time
 *
 *  \warning    This function is virtual. Re-implement it in the derived
 *              class if needed.
 *
 *  \return     true if successfully forwarded, false otherwise
 */
/****************************************************************************/
bool NetworkClientDevice::SetDateAndTime(const QString &ref, const QDateTime &qdt)
{
    Q_UNUSED(ref)
    if (receivers(SIGNAL(SigDateAndTime(const QDateTime &))) > 0) {
        emit SigDateAndTime(qdt);
        return true;
    }
    return false;
}

} // end namespace NetworkBase
