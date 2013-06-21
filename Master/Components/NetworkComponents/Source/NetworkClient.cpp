/****************************************************************************/
/*! \file NetworkClient.cpp
 *
 *  \brief Module which creates and authenticates an outgoing
 *         network connection.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 29.06.2010
 *   $Author:  $ Y.Novak
 *
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

#include <QtNetwork>
#include <NetworkComponents/Include/NetworkClient.h>
#include <Global/Include/Utils.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief    Constructor
 *
 *      In case of error the connection will be destroyed and an error
 *      signal will be emitted.
 *
 *  \param    type = type of client to create
 *  \param    ip = server IP if different from the default one
 *  \param    port = server port if different from the default one
 *  \param    pParent = pointer to parent
 *
 ****************************************************************************/
NetworkClient::NetworkClient(NetworkClientType_t type, const QString &ip, const QString &port, QObject *pParent) :
      QObject(pParent),
      m_myName(""),
      m_myType(type),
      m_msgHdlr(NULL),
      m_myVersion(""),
      m_Ip(ip),
      m_port(port),
      m_authStage(NC_INIT),
      m_BlockSize(0)
{
}


/****************************************************************************/
/*!
 *  \brief    This function initiates new outgoing connection.
 *
 *
 ****************************************************************************/
void NetworkClient::ConnectToServer()
{
    m_tcpSocket.abort();
    m_tcpSocket.connectToHost(m_Ip, m_port.toUInt());
    qDebug() << "NetworkClient: connecting to server " << m_Ip << "/" << m_port;

    /// Note: no need to start timer here because connectToHost will emit
    ///       an error signal, which we receive and process.
}

/****************************************************************************/
/*!
 *  \brief    This SLOT handles TCP Socket errors
 *
 *      In case of error the connection will be destroyed and an error
 *      signal will be emitted.
 *
 *  \param    err = socket error
 *
 ****************************************************************************/
void NetworkClient::HandleSocketError(QAbstractSocket::SocketError err)
{
//    qDebug() << "xxxxx NetworkClient::HandleSocketError";
    /*
     * if these errors happen, we shall continue pinging:
             QAbstractSocket::ConnectionRefusedError
             QAbstractSocket::SocketTimeoutError
     *
     * in all other cases we go down.
     */

    if (m_authStage == NC_INIT) {
        // if connection was not yet established, client shall try
        // to ping server for connection.
        if ((err == QAbstractSocket::ConnectionRefusedError) ||
            (err == QAbstractSocket::SocketTimeoutError)) {
            //qDebug() << "NETCLIENT: pinging server again !";
            QTimer::singleShot(5000, this, SLOT(ConnectToServer()));
            return;
        }
    }

    // if connection was established and fails, close socket and
    // inform upper layer.
    DisconnectFromServer();
    // working connection failed --> we have to report that "peer disconnected"
    emit ConnectionLost(m_myName);
}

/****************************************************************************/
/*!
 *  \brief    This SLOT handles a timeout during connection authentication.
 *
 *      In case of timeout the connection will be destroyed and an error
 *      signal will be emitted.
 *
 ****************************************************************************/
void NetworkClient::HandleAuthTimeout()
{
    qDebug() << "NetworkClient::HandleAuthTimeout";
    // stop the authentication timer:
    static_cast<void>(
            // cannot use return value here anyway
            m_timer.blockSignals(true)
    );
    m_timer.stop();
    // destroy connection:
    DisconnectFromServer();
    EmitConnectionFailed(NC_AUTHENTICATION_FAILED);
}

/****************************************************************************/
/*!
 *  \brief    This SLOT is called during Socket authentication.
 *
 *      This function reads incoming TCP Socket data during
 *      authentication process.
 *
 ****************************************************************************/
void NetworkClient::ReadRawSocket()
{
 //  Incoming data TCP Socket stream
  QDataStream in;
  in.setDevice(&m_tcpSocket);
  in.setVersion(static_cast<int>(QDataStream::Qt_4_0));
  quint8 MarkerByte = 0;

  // The TcpSocket's readyRead signal will only be emitted one time if new
  // data is available. This means if we are too slow to fetch data, any
  // new incoming message will not generate the readyRead signal. Therefore
  // we have to keep fetching data from Socket till we get it all. Otherwise
  // the data will wait in the queue till next message arrives and generates
  // the next readyRead signal:
  do { // while() -->
    if (m_BlockSize == 0) {
        if (m_tcpSocket.bytesAvailable() < (int)sizeof(quint32)) {
            return;
        }
        in >> m_BlockSize;
    }
    // make sure message came with marker byte (therefore BlockSize + 1):
    if (m_tcpSocket.bytesAvailable() < (m_BlockSize + 1)) {
        qDebug()<<"\n\n Marker byte not available";
        return;
    }
    in >> MarkerByte;
    QByteArray response = m_tcpSocket.read(m_BlockSize);
    switch (m_authStage) {
    case NC_INIT:
        HandleInitAction(response);
        break;
    case NC_NOT_AUTHENTICATED:
        HandleAuthAction(response);
        break;
    case NC_AUTHENTICATED:
        // Connection is established at this point --> forward msg to the MessageHandler
        emit ForwardToMessageHandler(MarkerByte, response);
        break;
    default:
        /// \todo: what here?
        break;
    }

    MarkerByte = 0;
    m_BlockSize = 0;

  } while (m_tcpSocket.bytesAvailable());
}

/****************************************************************************/
/*!
 *  \brief    This function initiates authentication answer
 *
 *      The very first message Client expects from Server is the
 *      authentication request. If something else is received - it is an
 *      error.
 *
 *  \param    msg = Server message
 *
 ****************************************************************************/
void NetworkClient::HandleInitAction(const QByteArray &msg)
{
    QString in(msg);

    if (in == CMH_AUTHENTICATION_REQ) {
        AckConnection();
        m_authStage = NC_NOT_AUTHENTICATED;
        // start authentication timer
        m_timer.start(NC_AUTHENTICATION_TIMEOUT);
    }
    else {
        DisconnectFromServer();
        EmitConnectionFailed(NC_WRONG_SERVER_MESSAGE);
    }
}

/****************************************************************************/
/*!
 *  \brief    This function handles Server authentication confirmation
 *
 *      If Client's authentication was correct, Server shall confirm
 *      it. If expected confirmation comes, Client instantiates a Message
 *      Handler and normal operation begins. If confirmation fails, the
 *      connection will be destroyed.
 *
 *  \param    msg = Server message
 *
 ****************************************************************************/
void NetworkClient::HandleAuthAction(const QByteArray &msg)
{
    qDebug() << "xxxxx NetworkClient::HandleAuthAction";
    // stop the authentication timer:
    static_cast<void>(
            // cannot use return value here anyway
            m_timer.blockSignals(true)
    );
    m_timer.stop();

    QString in(msg);

    if (in == CMH_AUTHENTICATION_CONF) {

        m_authStage = NC_AUTHENTICATED;
        if (m_msgHdlr == 0) {
            DisconnectFromServer();
            EmitConnectionFailed(NC_MSGHANDLER_POINTER_NULL);
            return;
        }

        try {
            CONNECTSIGNALSLOT(this, ForwardToMessageHandler(quint8, QByteArray &),
                              m_msgHdlr, GetIncomingMsg(quint8, QByteArray &));
            CONNECTSIGNALSLOT(m_msgHdlr, SendMessage(quint8, const QByteArray &),
                              this, SendMessage(quint8, const QByteArray &));
        } catch (...) {
            DisconnectFromServer();
            EmitConnectionFailed(NC_SIGNAL_CONNECT_FAILED);
            return;
        }
        // inform users about successful connection
        emit ConnectionEstablished(m_myName);
    }
    else {
        DisconnectFromServer();
        EmitConnectionFailed(NC_WRONG_SERVER_MESSAGE);
    }

}

/****************************************************************************/
/*!
 *  \brief    Register a Message Handler for certain Client type.
 *
 *      This function updates internal Hash of client protocol handlers.
 *      Each outgoing connection shall have a corresponding protocol
 *      handler in the Hash.
 *
 *  \param    pH = pointer to Message Handler object
 *
 *  \return   true if success, false if error
 *
 ****************************************************************************/
bool NetworkClient::RegisterMessageHandler(NetworkDevice *pH)
{
    // check pointer
    if (pH == NULL) {
        qDebug() << (QString)("NetworkClient: my MessageHandler is NULL !");
        return false;
    }

    m_msgHdlr = pH;

    return true;
}

/****************************************************************************/
/*!
 *  \brief    This function sends an authentication reply to Server
 *
 *      XML message is constructed and sent over TCP Socket.
 *
 ****************************************************************************/
void NetworkClient::AckConnection()
{
    QString str = NC_MSG_ON_TAG + "<cmd>Authentication</cmd><name>" + m_myName +
                                  "</name><version>" + m_myVersion +
                                  "</version>" + NC_MSG_OFF_TAG;

    //<msg><cmd>CONTENTS</cmd><name>CONTENTS</name><version>CONTENTS</version></msg>

    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    if (!domD.setContent(str, true, &err, &errL, &errC)) {
        /// \todo: log error?
        qDebug() << (QString)("NetworkClient: cannot convert outgoing QString message to QDomDocument! Error: " + err);
        return;
    }
    QByteArray ba = domD.toByteArray(-1);
    if (ba.isEmpty()) {
        /// \todo: log error?
        qDebug() << (QString)("NetworkDevice: outgoing QByteArray is empty !");
        return;
    }

    SendMessage(static_cast<quint8>(NET_NETLAYER_MESSAGE), ba);
}

/****************************************************************************/
/*!
 *  \brief    This function sends a message to Server.
 *
 *  \param    type = type of message
 *  \param    ba = outgoing byte array
 *
 *  \todo this function shall probably return a value(or throw exception) to upper layers
 *
 ****************************************************************************/
void NetworkClient::SendMessage(quint8 type, const QByteArray &ba)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(static_cast<int>(QDataStream::Qt_4_0));

    qint32 bsize = ba.size();
    out << bsize;
    // set the Marker Byte:
    out << type;
    // put our byte array into buffer
    int nofbytes = out.writeRawData(ba.data(), bsize);
    // send the buffer
    if ((m_tcpSocket.write(block) < (bsize + 1 + sizeof(qint32))) || nofbytes != bsize) {
        /// \todo: log error
    }
}

/****************************************************************************/
/*!
 *  \brief    This function disconnects Client from Server
 *
 *      If Message Handler was instantiated, it will be destroyed.
 *      Connection will be closed.
 *
 ****************************************************************************/
void NetworkClient::DisconnectFromServer()
{
    qDebug() << "xxxxx NetworkClient::DisconnectFromServer";
    if (m_msgHdlr != 0) {
        static_cast<void>(
                // cannot use return value here anyway
        QObject::disconnect(this, SIGNAL(ForwardToMessageHandler(quint8, QByteArray &)),
                                                m_msgHdlr, SLOT(GetIncomingMsg(quint8, QByteArray &)))
        );
        static_cast<void>(
                // cannot use return value here anyway
        QObject::disconnect(m_msgHdlr, SIGNAL(SendMessage(quint8, const QByteArray &)),
                                                this, SLOT(SendMessage(quint8, const QByteArray &)))
        );
    }

    m_authStage = NC_INIT;
    m_tcpSocket.disconnectFromHost();
    m_tcpSocket.close();
}

/****************************************************************************/
/*!
 *  \brief    This function initializes network Client
 *
 *      This function must be called first before Client can be
 *      productively used.
 *
 *  \return true if initialization was successful, false otherwise
 *
 ****************************************************************************/
bool NetworkClient::Initialize()
{
    //set parent of the socket, otherwise it will not be moved to thread
    m_tcpSocket.setParent(this);

    // no Nagle alg + no-delay option:
    m_tcpSocket.setSocketOption(QAbstractSocket::LowDelayOption, 1);
    // keepalive on:
    m_tcpSocket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    // fetch configuration strings
    if (SetConfigParameters() == NCE_TYPE_INVALID) {
        /// \todo: handle error
        qDebug() << (QString)("NETCLIENT: setting config parameters failed !");
        return false;
    }
    // check all configuration parameters
    if ((m_Ip.isEmpty())||(m_port.isEmpty())||(m_myName.isEmpty())||(m_myVersion.isEmpty())) {
        /// \todo: handle error
        qDebug() << (QString)("NETCLIENT: config parameter(s) missing !");
        return false;
    }

    try {
        CONNECTSIGNALSLOT(&m_tcpSocket, readyRead(), this, ReadRawSocket());
        CONNECTSIGNALSLOT(&m_tcpSocket, error(QAbstractSocket::SocketError), this, HandleSocketError(QAbstractSocket::SocketError));
        CONNECTSIGNALSLOT(&m_timer, timeout(), this, HandleAuthTimeout());
    } catch(...) {
        /// \todo: handle error
        return false;
    }

    qDebug() << "NetworkClient: my network settings --> ";
    qDebug() << "IP: " + m_Ip;
    qDebug() << "Port: " + m_port;

    return true;
}

/****************************************************************************/
/*!
 *  \brief    Sets client configuration according to its type
 *
 *
 *  \return    NCS_TYPE_INVALID if conversion failed, otherwise valid string
 *
 ****************************************************************************/
NetworkClientType_t NetworkClient::SetConfigParameters()
{
    if ((m_myType > NCE_TYPE_UNKNOWN) && (m_myType < NCE_TYPE_INVALID)) {

        m_myName = ClientConfiguration[Global::AsInt(m_myType) - 2][0];
        m_myVersion = ClientConfiguration[Global::AsInt(m_myType) - 2][1];
        if ((m_Ip.isEmpty()) || (m_port.isEmpty())) {
            m_Ip = ClientConfiguration[Global::AsInt(m_myType) - 2][2];
            m_port = ClientConfiguration[Global::AsInt(m_myType) - 2][3];
        }

        return m_myType;
    }

    qDebug() << "NetworkClient: config parameters are wrong, cannot use them !";
    return NCE_TYPE_INVALID;
}

/****************************************************************************/
/*!
 *  \brief    This function emits the disconnection signal.
 *
 *  \param    err = error code
 *
 ****************************************************************************/
void NetworkClient::EmitConnectionFailed(NetworkClientErrorType_t err)
{
    emit ConnectionFailed(err);
}

} // end namespace NetworkBase
