/****************************************************************************/
/*! \file NetworkServer.cpp
 *
 *  \brief Network TCP/IP Server Module.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 29.06.2010
 *   $Author:  $ Y.Novak
 *
 *  \b Description:
 *
 *       This Server listens to a defined IP/Port, handles incoming
 *       connections and maintains a record of all connected clients.
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


#include <NetworkComponents/Include/NetworkServer.h>
#include <NetworkComponents/Include/ConnectionManager.h>
#include <NetworkComponents/Include/NetSettings.h>
#include <NetworkComponents/Include/NetworkDevice.h>
#include <Global/Include/TranslatableString.h>
#include <Global/Include/GlobalEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>
#include <NetworkComponents/Include/NetworkComponentEventCodes.h>
#include <Global/Include/EventObject.h>


namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief    Constructor for the NetworkServer
 *
 *      Depending on type, either local (listening to 127.0.0.1) network
 *      server or global network server will be instantiated.
 *
 *  \param    type    = Server type: axeda, gui, etc.
 *  \param    pParent = pointer to the parent object
 *
 ****************************************************************************/
NetworkServer::NetworkServer(NetworkServerType_t type, QObject *pParent)
                    : QTcpServer(pParent),
                    m_connectionCounter(0),
                    m_connectionCounterRolledOver(NS_NO),
                    m_myServer(NULL),
                    m_myIp(""),
                    m_myPort(""),
                    m_myType(type),
                    m_authReq(""),
                    m_authConf("")
//                    m_started(false)
{
    m_connectionsList.clear();
    m_registeredHandlersList.clear();
    m_availableConnections.clear();
    m_takenConnections.clear();
}

/****************************************************************************/
/*!
 *  \brief    Destructor for the NetworkServer
 *
 ****************************************************************************/
NetworkServer::~NetworkServer()
{
    try {
        qDebug() << "NETSERVER: NetworkServer destroyed!";
        // clear list of registered handlers:
        m_registeredHandlersList.clear();
        // delete all instantiated connection managers:
        QList<ConnectionManager*> cmList = m_connectionsList.values();
        for (int i = 0; i < cmList.size(); i++) {
            if (cmList[i] != NULL) {
                static_cast<void>(
                        // cannot use return value here anyway
                        cmList[i]->disconnect()
                );
                delete cmList[i];
            }
        }

        if (m_myServer != NULL) {
            delete m_myServer;
        }
        m_myServer = NULL;
    }
    /*lint -e1551 */
    CATCHALL();
}

/****************************************************************************/
/*!
 *  \brief    Returns the type of this Server object
 *
 *  \return   server type
 *
 ****************************************************************************/
NetworkServerType_t NetworkServer::GetServerType()
{
    return m_myType;
}

/****************************************************************************/
/*!
 *  \brief    This SLOT handles any new incoming connection.
 *
 *      This function is called every time there is an incoming connection
 *      attempt to this Server/Port. It checks if Server still has free
 *      connection slots. If not, connection will be refused. If yes, a new
 *      ConnectionManager will be created to authenticate and further handle
 *      the connection.
 *
 *  \param    sktDescriptor = socket parameter given by the QTcpServer
 *
 ****************************************************************************/
void NetworkServer::incomingConnection(qintptr sktDescriptor)
{
    qDebug() << "NETSERVER: Entered incomingConnection..." << this->m_myIp << this->m_myPort;

    // check if there are any free connections left:
    if (m_availableConnections.isEmpty()) {
        // no available connections, so drop this one
        DisconnectTheSocket(NO_AVAILABLE_CONNECTIONS, sktDescriptor);
        qDebug() << "NETSERVER: Connection attempt failed: no free slots!";
        Global::EventObject::Instance().RaiseEvent(EVENT_NS_NO_AVAILABLE_CONNECTIONS,
                                                   Global::tTranslatableStringList() << m_myIp << m_myPort);
        return;
    }

    // generate key for currently created connection:
    if (!GenerateReference()) {
        // oops, we failed.
        // disconnect the socket:
        DisconnectTheSocket(NO_FREE_REFERENCES, sktDescriptor);
        return;
    }

    try {
        try {
            // create connection manager:
            ConnectionManager* cManager = new ConnectionManager(m_availableConnections, m_connectionCounter, m_authReq, m_authConf, m_authReply, this);

            // connect all needed signals/slots:
            try {
                try {
                    CONNECTSIGNALSLOT(cManager, ClientConnected(quint32, const QString &),
                                      this, RegisterConnection(quint32, const QString &));
                    CONNECTSIGNALSLOT(cManager, DestroyMe(quint32, const QString &, NetworkBase::DisconnectType_t),
                                      this, DestroyManager(quint32, const QString &, NetworkBase::DisconnectType_t));
                    CONNECTSIGNALSLOT(this, DestroyConnection(NetworkBase::DisconnectType_t),
                                      cManager, DestroyConnection(NetworkBase::DisconnectType_t));
                }
                CATCHALL_RETHROW()
            }
            catch(...) {
                DisconnectTheSocket(CONNECT_SIGNAL_FAIL, sktDescriptor);
                delete cManager;
                return;
            }

            // store the reference:
            m_connectionsList.insert(m_connectionCounter, cManager);
            // tell the Manager which circuit to use:
            cManager->HandleNewConnection(sktDescriptor);
        }
        CATCHALL_RETHROW()
    }
    catch (...) {
        // disconnect the socket:
        DisconnectTheSocket(CANNOT_ALLOCATE_MEMORY, sktDescriptor);
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief    Destroy incoming Socket connection
 *
 *      This function tells the TcpServer to drop incoming connection
 *      because it cannot be internally processed.
 *
 *  \param    dtype = disconnection reason
 *  \param    sktDescriptor = socket parameter given by the QTcpServer
 *
 ****************************************************************************/
void NetworkServer::DisconnectTheSocket(DisconnectType_t dtype, qintptr sktDescriptor)
{
    QTcpSocket socket;
    if (!socket.setSocketDescriptor(sktDescriptor)) {
        /// \todo: do anything here?
    }
    static_cast<void> (
    // cannot use return value here anyway
            socket.blockSignals(true)
    );
    socket.close();

    // take action according to the type of disconnect
    HandleDisconnectReason(dtype, NO_CLIENT);
}

/****************************************************************************/
/*!
 *  \brief    Generate a unique connection identification number.
 *
 *      Each connection gets a unique reference number so the Server
 *      will be able to find this connection in the hash.
 *
 *  \return  true if success, false if error
 *
 ****************************************************************************/
bool NetworkServer::GenerateReference()
{
    // generate key for currently created connection:
    if ((m_connectionCounter == NS_MAX_CONNECTIONS) ||
                        (m_connectionCounterRolledOver == NS_YES)) {
        // remember this condition
        m_connectionCounterRolledOver = NS_YES;
        // parse current connections and get the free number
        m_connectionCounter = GetFreeConnectionNumber();
        // check if there are still numbers available
        if (m_connectionCounter == NS_NO_FREE_CONNECTIONS) {
            // all connection references are consumed
            // its an error, this shall never happen!
            Global::EventObject::Instance().RaiseEvent(EVENT_NS_NO_AVAILABLE_CONNECTIONS,
                                                       Global::tTranslatableStringList() << m_myIp << m_myPort);
            return false;
        }
    }
    else {
        m_connectionCounter++;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief    Find a free connection identification number.
 *
 *      The variable m_connectionCounter is up-counting to generate
 *      unique connection references. If it is about to overflow (which
 *      probably will never happen), Server has to parse existing
 *      connections to find out which number was given back and is
 *      free now. There will always be one because Server will never
 *      handle NS_MAX_CONNECTIONS simultaneously.
 *
 *  \return  free connection number or ERROR: NS_NO_FREE_CONNECTIONS
 *
 ****************************************************************************/
quint32 NetworkServer::GetFreeConnectionNumber()
{
    // Parse the m_connectionsList to find free numbers
    for (quint32 i = 0; i <= NS_MAX_CONNECTIONS; i++) {
        if (!m_connectionsList.contains(i)) {
            // this connection number is free, get it:
            return i;
        }
    }

    // if we get here, something went horribly wrong because Server
    // seem to have consumed all NS_MAX_CONNECTIONS

    // return error:
    return NS_NO_FREE_CONNECTIONS;
}

/****************************************************************************/
/*!
 *  \brief    Register a Message Handler for certain Client type.
 *
 *      This function updates internal Hash of client protocol handlers.
 *      Each incoming connection shall have a corresponding protocol
 *      handler in the Hash.
 *
 *  \param    pH = pointer to Protocol Handler object
 *  \param    client = type of the registering client
 *
 *  \return   true if success, false if error
 *
 ****************************************************************************/
bool NetworkServer::RegisterMessageHandler(NetworkDevice *pH, const QString & client)
{
    // check pointer
    if (pH == NULL) {
        qDebug() << "NETSRVR: NetworkDevice is NULL : not registered !";
        Global::EventObject::Instance().RaiseEvent(EVENT_NL_NULL_POINTER,
                                                   Global::tTranslatableStringList() << m_myIp << FILE_LINE);
        return false;
    }
    // update the list of registered protocol handlers
    m_registeredHandlersList.insert(client, pH);

    return true;
}

/****************************************************************************/
/*!
 *  \brief    This SLOT handles successfully authenticated connection.
 *
 *      This function shall be called if new incoming connection
 *      was successfully authenticated. The Server will check then if slots
 *      are still available for this type of connection. If not, the
 *      connection will be destroyed (for example we cannot have two GUIs
 *      connected at the same time, so if second GUI tries to connect, Server
 *      has to refuse the connection).
 *
 *  \param    number = unique connection identifier
 *  \param    client = type of the connected client
 *
 ****************************************************************************/
void NetworkServer::RegisterConnection(quint32 number, const QString &client)
{
    qDebug() << "NetworkServer::RegisterConnection" << this->m_myIp << this->m_myPort << client ;
    // check if this type of connection is still available
    if (m_availableConnections.contains(client)) {
        //connect Connection Manager with Protocol Handler
        ConnectionManager *cManager = NULL;
        cManager = m_connectionsList.value(number);
        NetworkDevice *pHandler = NULL; //MessageHandler
        pHandler = m_registeredHandlersList.value(client);
        if ((cManager == NULL) || (pHandler == NULL)) {
            // one of the needed pointers is invalid!
            // connection cannot be established, destroy it
            DestroyManager(number, client, NULL_POINTER_IN_HASH);
            Global::EventObject::Instance().RaiseEvent(EVENT_NL_NULL_POINTER,
                                                       Global::tTranslatableStringList() << m_myIp << FILE_LINE);
            return;
        }

        try {
            try {
                CONNECTSIGNALSLOT(cManager, MsgReceived(quint8, QByteArray &),
                                  pHandler, GetIncomingMsg(quint8, QByteArray &));
                CONNECTSIGNALSLOT(pHandler, SendMessage(quint8, const QByteArray &),
                                  cManager, ForwardMessage(quint8, const QByteArray &));
            }
            CATCHALL_RETHROW();
        }
        catch (...) {
            // connection cannot be established, destroy it
            DestroyManager(number, client, CONNECT_SIGNAL_FAIL);
            return;
        }

        // update connections lists
        m_takenConnections.insert(client, m_availableConnections.value(client));
        m_availableConnections.remove(client);

        // confirm client's connection
        /// \todo: do we need flexibility with this?
        m_authConf = CMH_AUTHENTICATION_CONF;
        cManager->ForwardMessage(m_authConf);

        // inform controlling entity about new client connection
        emit ClientConnected(client);
    }
    else {
        // no, the connection is already taken: destroy this
        // connection manager
        DestroyManager(number, client, NO_AVAILABLE_CONNECTIONS);
        Global::EventObject::Instance().RaiseEvent(EVENT_NS_NO_AVAILABLE_CONNECTIONS,
                                                   Global::tTranslatableStringList() << m_myIp << m_myPort);
    }
}

/****************************************************************************/
/*!
 *  \brief    This function destroys all network connections.
 *
 *      This function shall be called if all existing connection needs to
 *      be destroyed.
 *
 ****************************************************************************/
void NetworkServer::DestroyAllConnections()
{
    // check if signal is connected. if ConnectionManager was destroyed
    // before, the call will land in nirvana.
    if (QObject::receivers(SIGNAL(DestroyConnection(NetworkBase::DisconnectType_t))) != 0) {
        emit DestroyConnection(USER_REQUEST);
    }
}

/****************************************************************************/
/*!
 *  \brief    This SLOT destroys a network connection.
 *
 *      This function shall be called if any existing connection needs to
 *      be destroyed. It deletes all relevant objects and removes
 *      connection's record from the current connection list.
 *
 *  \param    number = unique connection identifier
 *  \param    client = type of the connected client
 *  \param    dtype  = reason for connection destruction
 *
 ****************************************************************************/
void NetworkServer::DestroyManager(quint32 number, const QString &client, DisconnectType_t dtype)
{
    qDebug() << "NETSERVER: destroyManager called !" << client;
    // fetch the pointer according to the supplied number...
    // ...and kill the object
    if (m_connectionsList.contains(number)) {
        ConnectionManager* cManager = NULL;
        cManager = m_connectionsList.value(number);
        if (cManager != NULL) {
               // disconnect everything connected to this object's signals
               static_cast<void>(
                    // cannot use return value here anyway
                       QObject::disconnect(cManager, 0, 0, 0)
               );
               // disconnect all cManager slots from server's signals
               static_cast<void>(
                    // cannot use return value here anyway
                       QObject::disconnect(this, 0, cManager, 0)
               );
               // kill the Socket connection:
               cManager->DisconnectMe();
            // WARNING: do not delete the manager here because this
            // might be a function (signal/slot) call from the exact same cManager
            // (which is decided by compiler). Use "deleteLater" !
               cManager->deleteLater();
        }
        // destroy hash reference to the object:
        m_connectionsList.remove(number);
        // renew connections lists:
        if (m_takenConnections.contains(client)) {
            m_availableConnections.insert(client, m_takenConnections.value(client));
            m_takenConnections.remove(client);
        }
        else {
            // no such element in the hash of authenticated connections.
            // this might happen if client authentication fails.
            if ((dtype != AUTHENTICATION_TIMEOUT) &&
                (dtype != AUTHENTICATION_FAILED)) {
                // this connection was active and it is NOT in the hash.
                // something might have gone wrong --> report error
                HandleDisconnectReason(UNKNOWN_CLIENT_DISCONNECTED, client);
            }
        }
    }
    else {
        // no such element in the hash of existing connections.
        // its an ERROR, this shall not happen.
        HandleDisconnectReason(NO_CONNECTION_IN_HASH, client);
        qDebug() << ((QString)"NETSERVER: Failed to destroy Manager Object N" + QString::number(number));
        Global::EventObject::Instance().RaiseEvent(EVENT_NS_DESTROY_MANAGEROBJ_FAILED,
                                                   Global::tTranslatableStringList() << m_myIp << QString::number(number));
    }

    // take action according to the type of disconnect
    HandleDisconnectReason(dtype, client);
}

/****************************************************************************/
/*!
 *  \brief    Take action according to connection destruction reason
 *
 *      Different actions shall be triggered according to the reason
 *      why the connection needs to be killed. It might be just a client
 *      disconnect or internal error, authentication failure, etc.
 *
 *  \param    dtype  = reason for disconnect
 *  \param    client = client type
 *
 ****************************************************************************/
void NetworkServer::HandleDisconnectReason(DisconnectType_t dtype, const QString &client)
{
    qDebug() << (QString)("NETSERVER: Disconnect reason: " + QString::number(static_cast<int>(dtype), 10));

    Global::EventObject::Instance().RaiseEvent(EVENT_NS_SERVER_DISCONNECTED,
                                               Global::tTranslatableStringList() << m_myIp << QString::number((int)dtype));

    switch (dtype) {
    case SOCKET_DISCONNECT:
    case USER_REQUEST:
        // the only two cases so far where we have to notify user about client
        // disconnect. all other cases are errors during connection setup
        emit ClientDisconnected(client);
        /// \todo: do we need to log this?
        break;
    default:
        emit ConnectionError(dtype, client);
        break;
    }
}

/****************************************************************************/
/*!
 *  \brief    Initialize Network Server and prepare it for start.
 *
 *      This function shall be called to prepare Network Server operation.
 *      Check the return code: if initialization fails, Server will not be
 *      started.
 *
 *  \return   NS_ALL_OK if success, otherwise error
 *
 ****************************************************************************/
NetworkServerErrorType_t NetworkServer::InitializeServer()
{
    QString stype = FetchServerTypeString();

    if (stype == NSS_TYPE_INVALID) {
        return NS_UNKNOWN_APP_REQUEST;
    }

    // read configuration from disk
    NetSettings nstg(stype, this);
    NetSettingsErrorType_t err = nstg.ReadSettings();
    if (err != NST_ALL_OK) {
        qDebug() << (QString)("NETSERVER: Loading settings failed !");
        Global::EventObject::Instance().RaiseEvent(EVENT_NS_LOAD_SETTINGS_FAILED,
                                                   Global::tTranslatableStringList() << m_myIp << QString::number((int)err));
        return NS_LOAD_SETTINGS_FAILED;
    }
    // set all configuration parameters
    nstg.SetSettings(&m_myIp, &m_myPort, &m_authReq, &m_authReply, &m_authConf, &m_availableConnections);

//    m_started = false;

    return NS_ALL_OK;
}

/****************************************************************************/
/*!
 *  \brief    Initialize and start a Network Server.
 *
 *      This function shall be called to start the Network Server operation.
 *      Check the return code: if initialization fails, Server will not be
 *      started.
 *
 *  \return   NS_ALL_OK if success, otherwise error
 *
 ****************************************************************************/
NetworkServerErrorType_t NetworkServer::StartServer()
{
//    if (m_started)
//        return NS_ALL_OK;

    // try to listen to the specified IP/PORT
    if (!listen(QHostAddress(m_myIp), m_myPort.toInt())) {
        qDebug() << (QString)("NETSERVER: Listen to IP/PORT:  " + m_myIp + "/" + m_myPort + " failed !");
        Global::EventObject::Instance().RaiseEvent(EVENT_NS_LISTEN_FAILED,
                                                   Global::tTranslatableStringList() << m_myIp << m_myPort);
        return NS_LISTEN_FAILED;
    }

    qDebug() << (QString)("NETSERVER: Server listens to IP/PORT:  " + m_myIp + "/" + m_myPort);

//    m_started = true;

    return NS_ALL_OK;
}

/****************************************************************************/
/*!
 *  \brief    Translate server type to string
 *
 *      This function translates server's type from internal type to string,
 *      which would be used for parsing of XML configuration files.
 *
 *  \return    NS_TYPE_INVALID if conversion failed, otherwise valid string
 *
 ****************************************************************************/
QString NetworkServer::FetchServerTypeString()
{
    QString type = NSS_TYPE_INVALID;
    switch (m_myType) {
    case NSE_TYPE_AXEDA:
        type = NSS_TYPE_AXEDA;
        break;
    case NSE_TYPE_NORMAL_GUI:
        type = NSS_TYPE_GUI;
        break;
    case NSE_TYPE_SERVICE:
        type = NSS_TYPE_SERVICE;
        break;
    case NSE_TYPE_SEPIA:
        type = NSS_TYPE_SEPIA;
        break;
    case NSE_TYPE_EXPORT:
        type = NSS_TYPE_EXPORT;
        break;
    case NSE_TYPE_BLG:
        type = NSS_TYPE_BLG;
        break;
    case NSE_TYPE_SEPIA_GUI:
        type = NSS_TYPE_SEPIA_GUI;
        break;
    default:
        break;
    }

    return type;
}

} // end namespace NetworkBase
