/****************************************************************************/
/*! \file ConnectionManager.cpp
 *
 *  \brief Module which manages an incoming network connection.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 29.06.2010
 *   $Author:  $ Y.Novak
 *
 *  \b Description:
 *
 *       This Module authenticates and manages an incoming network
 *       connection. If authentication is successful, it will be registered
 *       with the Network Server. If authentication fails, the connection
 *       will be destroyed. Upon successful connection Network Server will
 *       connect this object to the client's protocol handler.
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

#include <NetworkComponents/Include/ConnectionManager.h>
#include <NetworkComponents/Include/NetworkDevice.h>
#include <Global/Include/Utils.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief    This is the ConnectionManager's constructor.
 *
 *  \param    hash    = list of all allowed clients with their type strings
 *  \param    number  = object's unique reference for TcpServer
 *  \param    req     = authentication request string
 *  \param    conf    = authentication confirmation string
 *  \param    d       = authentication response XML element (structure)
 *  \param    pParent = pointer to the object's parent
 *
 *
 ****************************************************************************/
ConnectionManager::ConnectionManager(QHash<QString, QString> hash,
                                     quint32 number, const QString & req, const QString & conf, const QDomElement & d,
                                     QObject *pParent) :
                                     QObject(pParent),
                                     m_myNumber(number),
                                     m_myClient(""),
                                     m_authenticationStringsList(hash),
                                     m_timer(this),
                                     m_authRequest(req),
                                     m_authConfirm(conf),
                                     m_replySection(d),
                                     m_connectionState(CM_NOT_AUTHENTICATED)

{
}

/****************************************************************************/
/*!
 *  \brief    This is the ConnectionManager's destructor.
 *
 *
 ****************************************************************************/
ConnectionManager::~ConnectionManager()
{
}

/****************************************************************************/
/*!
 *  \brief    This SLOT handles new incoming connection.
 *
 *      This function shall be called to authenticate a new incoming
 *      connection.
 *
 *  \param    socketDescriptor = unique socket identifier
 *
 ****************************************************************************/
void ConnectionManager::HandleNewConnection(int socketDescriptor)
{
    qDebug() << "ConnectionManager::HandleNewConnection";

    // set the correct Descriptor:
    if (!m_TcpSocket.setSocketDescriptor(socketDescriptor)) {
        // error logging is done by the Server
        DestroyConnection(SOCKET_ERROR);
        return;
    }

    try {
        CONNECTSIGNALSLOT(&m_timer, timeout(), this, HandleTimeout());

        // set "no Nagle alg" and "no-delay option" (which speeds up
        // Network response):
        m_TcpSocket.setSocketOption(QAbstractSocket::LowDelayOption, 1);
        // turn keepalive on:
        m_TcpSocket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);

        CONNECTSIGNALSLOT(&m_TcpSocket, disconnected(), this, HandleSocketDisconnect());
        CONNECTSIGNALSLOT(&m_TcpSocket, readyRead(), this, ReadSocket());
    } catch (...) {
        // error logging is done by the Server
        DestroyConnection(CONNECT_SIGNAL_FAIL);
        return;
    }

    // initiate authentication:
    m_authRequest = CMH_AUTHENTICATION_REQ;
    ForwardMessage(m_authRequest);

    // start timer to check for authentication timeout
    m_timer.start(CM_AUTHENTICATION_TIMEOUT);
}

/****************************************************************************/
/*!
 *  \brief    This SLOT handles error signal from TCP socket.
 *
 *
 ****************************************************************************/
void ConnectionManager::HandleSocketDisconnect()
{
    DestroyConnection(SOCKET_DISCONNECT);
}

/****************************************************************************/
/*!
 *  \brief    This SLOT handles authentication timeout.
 *
 *
 ****************************************************************************/
void ConnectionManager::HandleTimeout()
{
    qDebug() << "xxxxx ConnectionManager::HandleTimeout";
    DestroyConnection(AUTHENTICATION_TIMEOUT);
}

/****************************************************************************/
/*!
 *  \brief    This SLOT is called when Socket received new data.
 *
 *      This function reads the authentication response from client and
 *      checks if it is correct. If not, connection will be destroyed.
 *
 ****************************************************************************/
void ConnectionManager::ReadSocket()
{
    QDataStream in;
    in.setDevice(&m_TcpSocket);
    in.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    quint8 MarkerByte = 0;
    quint32 BlockSize = 0;

    // The TcpSocket's readyRead signal will only be emitted one time if new
    // data is available. This means if we are too slow to fetch data, any
    // new incoming message will not generate the readyRead signal. Therefore
    // we have to keep fetching data from Socket till we get it all. Otherwise
    // the data will wait in the queue till next message arrives and generates
    // the next readyRead signal:
    do {  // while -->

        if (BlockSize == 0) {
            if (m_TcpSocket.bytesAvailable() < (int)sizeof(quint32)) {
                return;
            }
            in >> BlockSize;
        }

        // make sure message came with marker byte (therefore "+ 1"):
        if (m_TcpSocket.bytesAvailable() < (BlockSize + 1)) {
            //qDebug() << "CMANAGER: message not yet complete...";
            return;
        }

        // get the MarkerByte value
        in >> MarkerByte;
        // read payload data
        QByteArray incoming_data = m_TcpSocket.read(BlockSize);

        // if the connection is already authenticated, data shall be
        // forwarded to the Protocol Handler:
        if (m_connectionState == CM_AUTHENTICATED) {
            //qDebug() << "CMANAGER: about to emit SIGNAL msgReceived() to ProtocolHandler !";
            emit MsgReceived(MarkerByte, incoming_data);
        }
        else {
        // The only Client data this object should process is the authentication response.
        // Any other response is an error. There will be no message parsing here, it will be
        // done by the MessageHandler.
            if (!CheckAuthenticationResponse(&incoming_data)) {
                // error logging is done by the Server
                DestroyConnection(AUTHENTICATION_FAILED);
            }
        }
        MarkerByte = 0;
        BlockSize = 0;
    } while (m_TcpSocket.bytesAvailable());
}
/****************************************************************************/
/*!
 *  \brief    This function checks incoming authentication response.
 *
 *  \param    ba = pointer to the incoming data
 *
 *  \return   true if authentication is correct, otherwise false
 *
 ****************************************************************************/
bool ConnectionManager::CheckAuthenticationResponse(QByteArray *ba)
{
    qDebug() << "xxxxxxx ConnectionManager::CheckAuthenticationResponse";
    // stop the authentication timer:
    static_cast<void>(
            // do not need return value here
            m_timer.blockSignals(true)
    );
    m_timer.stop();

    QString str(*ba);

    qDebug() << ".... content:" << str;

    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domDoc;
    if (!domDoc.setContent(str, true, &err, &errL, &errC)) {
        qDebug() << ((QString)"Parsing incoming XML Doc failed: " + err);
        return false;
    }

    if (!ParseAuthenticationResponse(&domDoc)) {
        qDebug() << "Parsing incoming XML Message failed!";
        return false;
    }

    // inform TcpServer about successful connection:
    emit ClientConnected(m_myNumber, m_myClient);

    return true;
}

/****************************************************************************/
/*!
 *  \brief    This function parses a valid incoming XML document.
 *
 *      The incoming message shall be a valid authentication response
 *      in XML document format. If authentication is correct, a
 *      ProtocolHandler is instantiated according to Client's type and an
 *      authentication confirmation is sent to Client.
 *
 *  \param    d = pointer to a valid XML document
 *
 *  \return   true if authentication is correct, otherwise false
 *
 ****************************************************************************/
bool ConnectionManager::ParseAuthenticationResponse(QDomDocument *d)
{
    qDebug() << "xxxxxxx ConnectionManager::ParseAuthenticationResponse";

    QDomElement root = d->documentElement();

    QDomElement elmt = root.firstChildElement(CM_CMD_TAG_NAME);
    if (elmt.isNull()) {
        qDebug() << (QString)("ConnectionManager: cannot parse incoming XML doc !");
        return false;
    }
    if (elmt.text() != m_replySection.firstChildElement().text()) {
        qDebug() << (QString)("ConnectionManager: cannot parse incoming XMl doc !");
        return false;
    }

    elmt = elmt.nextSiblingElement();
    QString name = elmt.text();
    elmt = elmt.nextSiblingElement();
    QString version = elmt.text();

    // instantiate new ProtocolHandler if correct authentication string is found.
    if (m_authenticationStringsList.contains(name) &&
           (m_authenticationStringsList.value(name) == version)) {
        // fetch the name of the connected client
        m_myClient = name;
        // change state to AUTHENTICATED
        m_connectionState = CM_AUTHENTICATED;
        // inform TcpServer about successful connection:
        qDebug() << (QString)("Client connected! \nClient Name: " + name + "\nClient Version: " + version);
    }
    else {
        // authentication failed
        qDebug() << (QString)("Authentication failed! \nClient Name: " + name + "\nClient Version: " + version);

        qDebug() << "xxxxx available connection strings:";

        QHash<QString, QString>::iterator i;
        for (i = m_authenticationStringsList.begin(); i != m_authenticationStringsList.end(); ++i)
        {
            qDebug() << i.key() << ": " << i.value() << endl;
        }

        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief    This function sends a message to Client.
 *
 *  \param    str = outgoing message string
 *
 ****************************************************************************/
void ConnectionManager::ForwardMessage(const QString &str)
{
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    if (!domD.setContent(str, true, &err, &errL, &errC)) {
        /// \todo: how do we inform the sender?
        qDebug() << (QString)("ConnectionManager: cannot convert outgoing QString message to QDomDocument! Error: " + err);
        return;
    }
    QByteArray ba = domD.toByteArray(-1);
    if (ba.isEmpty()) {
        /// \todo: how do we inform the sender?
        qDebug() << (QString)("ConnectionManager: outgoing QByteArray is empty !");
        return;
    }
    ForwardMessage(static_cast<quint8>(NET_NETLAYER_MESSAGE), ba);
}

/****************************************************************************/
/*!
 *  \brief    This SLOT forwards message to connected Client.
 *
 *  \param    mbyte = type of message
 *  \param    ba = reference to outgoing byte array
 *
 ****************************************************************************/
void ConnectionManager::ForwardMessage(quint8 mbyte, const QByteArray & ba)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(static_cast<int>(QDataStream::Qt_4_0));

    qint32 bsize = ba.size();
    out << bsize;
    // set the Marker Byte:
    out << mbyte;

    int nofbytes = out.writeRawData(ba.data(), bsize);
    qDebug() << "\n\n Bytes Written \t"<<bsize;
    // check if data was actually written: bsize + markerbyte + packet size
    if ((m_TcpSocket.write(block) < (bsize + 1 + sizeof(qint32))) || nofbytes != bsize) {
        /// \todo: how do we inform the sender?
        qDebug() << (QString)("ConnectionManager: writing outgoing data to Socket failed !");
    }
    m_TcpSocket.flush();
}

/****************************************************************************/
/*!
 *  \brief    This function asks Server to destroy the connection.
 *
 *  \param    dtype = disconnect reason
 *
 ****************************************************************************/
void ConnectionManager::DestroyConnection(DisconnectType_t dtype)
{
    // Connection is down. Life does not make sense anymore. :(
    // Tell the TcpServer to delete this Manager Object and make this
    // connection type available again.
    emit DestroyMe(m_myNumber, m_myClient, dtype);
}

/****************************************************************************/
/*!
 *  \brief    This function is called by the Server to destroy
 *            the connection.
 *
 ****************************************************************************/
void ConnectionManager::DisconnectMe()
{
    // stop the authentication timer (we might get here from timer timeout):
    static_cast<void>(
            // cannot use return value here anyway
            m_timer.blockSignals(true)
    );
    m_timer.stop();

    // shut down the socket
    static_cast<void>(
            // cannot use return value here anyway
            m_TcpSocket.blockSignals(true)
    );
    m_TcpSocket.close();

    // change the state
    m_connectionState = CM_NOT_AUTHENTICATED;
}

/****************************************************************************/
/*!
 *  \brief    This function returns ConnectionManager's reference.
 *
 *  \return   reference number assigned by NetworkServer
 *
 ****************************************************************************/
quint32 ConnectionManager::GetRefNumber()
{
    return m_myNumber;
}

/****************************************************************************/
/*!
 *  \brief    This function returns ConnectionManager's client name.
 *
 *  \return   currently connected client's name
 *
 ****************************************************************************/
QString ConnectionManager::GetClientName()
{
    return m_myClient;
}

} // end namespace NetworkBase
