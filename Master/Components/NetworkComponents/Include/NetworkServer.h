/****************************************************************************/
/*! \file NetworkServer.h
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

#ifndef NETWORKBASE_NETWORKSERVER_H
#define NETWORKBASE_NETWORKSERVER_H

#include <QtNetwork>
#include <QtXml>

#include <DataLogging/Include/DayEventEntry.h>

namespace NetworkBase {

class ConnectionManager;
class NetworkDevice;

const QString NSS_TYPE_AXEDA      = "axeda";         ///< server for communication with Axeda client
const QString NSS_TYPE_GUI        = "gui";           ///< server for communication with normal GUI
const QString NSS_TYPE_SERVICE    = "service";       ///< server for communication with service SW
const QString NSS_TYPE_SEPIA      = "sepia";         ///< server for communication with sepia
const QString NSS_TYPE_EXPORT     = "export";        ///< server for communication with Export
const QString NSS_TYPE_BLG        = "BLG";           ///< server for communication with BLG
const QString NSS_TYPE_INVALID    = "invalid";       ///< wrong server type

const QString NO_CLIENT           = "NO_CLIENT";     ///< parameter to destroy connection at the setup stage

const quint32 NS_MAX_CONNECTIONS     =  0xFFFFFFF0;  ///< Maximum number of client connections
const quint32 NS_NO_FREE_CONNECTIONS =  0xFFFFFFFF;  ///< This number identifies the case with no free connections left

/// Existing Server Types
typedef enum {
    NSE_TYPE_UNKNOWN = 1,         ///< uninitialized server
    NSE_TYPE_AXEDA,               ///< server for communication with axeda client
    NSE_TYPE_NORMAL_GUI,          ///< server for communication with normal device gui
    NSE_TYPE_SERVICE,             ///< server for communication with service (gui) client
    NSE_TYPE_SEPIA,               ///< server for communication with sepia client
    NSE_TYPE_EXPORT,              ///< server for communication with Export component
    NSE_TYPE_BLG,                ///< server for communication with BLG component
    NSE_TYPE_INVALID              ///< wrong server type
} NetworkServerType_t;

/// Possible error codes of the Server
typedef enum {
    NS_ALL_OK = 1,                ///< self explaining
    NS_LOAD_SETTINGS_FAILED,      ///< self explaining
    NS_UNKNOWN_APP_REQUEST,       ///< unknown server type requested
    NS_LISTEN_FAILED,             ///< Server's "listen" call failed
    NS_EVTEMITTER_NOT_CONNECTED   ///< event emitting signal of the server is not connected
} NetworkServerErrorType_t;

/// Reference counter rollover status values
typedef enum {
    NS_YES = 1,                   ///< reference counter rolled over
    NS_NO                         ///< reference counter did not roll over
} ConnectionCounterType_t;

/// Possible disconnection reasons
typedef enum {
    UNDEFINED_NO_INIT = 0,        ///< initial "undefined" value
    SOCKET_ERROR,                 ///< error in socket initialization
    SOCKET_DISCONNECT,            ///< socket connections disconnected by the client
    AUTHENTICATION_TIMEOUT,       ///< self explaining
    AUTHENTICATION_FAILED,        ///< self explaining
    NULL_POINTER_IN_HASH,         ///< hash contents are broken
    NO_AVAILABLE_CONNECTIONS,     ///< all connections of certain type are taken
    CONNECT_SIGNAL_FAIL,          ///< connection of SIGNAL to SLOT failed
    NO_FREE_REFERENCES,           ///< all reference numbers are taken
    CANNOT_ALLOCATE_MEMORY,       ///< the "new" call failed
    NO_CONNECTION_IN_HASH,        ///< connection manager reference is missing in the hash
    UNKNOWN_CLIENT_DISCONNECTED,  ///< client disconnected, but its type is unknown (not in the hash)
    UNKNOWN_ERROR,                ///< self explaining
    USER_REQUEST                  ///< user requested to shut down the server
} DisconnectType_t;


/****************************************************************************/
/**
 * \brief  This class implements a Network TCP/IP Server.
 *
 * This Server listens to a defined IP/Port, handles incoming connections
 * and maintains a record of all connected clients. New ConnectionManager
 * is created for every new incoming connection. If connection is closed,
 * corresponding ConnectionManager will be deleted.
 *
 * On creation, this Server loads a list of allowed client Types and allowed
 * number of simultaneously existing connections for each client type. Thus
 * it is possible to filter out unwanted incoming connection attempts.
 *
 * \warning Error Handling:
 * All connection errors are reported via "connectionError" signal.
 * Also, the return code of the "startServer" function must be checked
 * to evaluate Server startup errors.
 */
/****************************************************************************/
class NetworkServer : public QTcpServer
{
    Q_OBJECT

    /// all test cases are friends:
    friend class TestNetworkServer;
    friend class TestNetworkServerDevice;

public:

    NetworkServer(NetworkServerType_t type, QObject *pParent = 0);
    ~NetworkServer();
    NetworkServerErrorType_t InitializeServer();
    NetworkServerErrorType_t StartServer();
    bool RegisterMessageHandler(NetworkDevice *pH, const QString &client);
    NetworkServerType_t GetServerType();
    void DestroyAllConnections();

protected:

    void incomingConnection(int sktDescriptor);

signals:

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when new client connects
     *
     *  \param    name = client name
     *
     ****************************************************************************/
     void ClientConnected(const QString &name);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when client disconnects
     *
     *  \param    name = client name
     *
     ****************************************************************************/
     void ClientDisconnected(const QString &name);

    /****************************************************************************/
    /*!
     * \brief     Emit connection setup error.
     *
     * \param     dtype = connection failure reason
     * \param     name = client name
     *
     ****************************************************************************/
     void ConnectionError(NetworkBase::DisconnectType_t dtype, const QString &name);

     /****************************************************************************/
     /*!
      * \brief     This signal is emitted when all existing connections shall be
      *            destroyed.
      *
      * \param     dtype = reason for disconnect
      *
      ****************************************************************************/
     void DestroyConnection(NetworkBase::DisconnectType_t dtype);

public slots:

    void DestroyManager(quint32 number, const QString &client, NetworkBase::DisconnectType_t dtype);
    void RegisterConnection(quint32 number, const QString &client);

private:

    quint32 GetFreeConnectionNumber();
    bool GenerateReference();
    void DisconnectTheSocket(DisconnectType_t dtype, int socketDescriptor);
    void HandleDisconnectReason(DisconnectType_t dtype, const QString &client);
    QString FetchServerTypeString();

private:

    NetworkServer();                                            ///< Not implemented.
    NetworkServer(const NetworkServer &);                       ///< Not implemented.
    const NetworkServer & operator = (const NetworkServer &);   ///< Not implemented.

    /*! list of all currently active connection managers
        Format: <ConnectionNumber, PointerToCManager >  */
    QHash<quint32, ConnectionManager* > m_connectionsList;
    /*! list of all registered protocol handlers
        Format: <ClientName, PointerToPHandler >  */
    QHash<QString, NetworkDevice* > m_registeredHandlersList;
    /*! counter for unique connection number generation */
    quint32 m_connectionCounter;
    /*! flag to log the connection counter roll-over condition */
    ConnectionCounterType_t m_connectionCounterRolledOver;
    /*! list of all possible clients with their type strings
        Format: <ClientName, typeString > */
    QHash<QString, QString > m_availableConnections;
    /*! list of all currently active clients with their type strings
        Format: <ClientName, typeString > */
    QHash<QString, QString > m_takenConnections;

    /*! Pointer to the actual TCP Server object */
    QTcpServer *m_myServer;
    /*!  IP to listen to */
    QString m_myIp;
    /*!  port to listen to */
    QString m_myPort;
    /*!  type of server: gui, axeda, service, etc */
    NetworkServerType_t m_myType;
    /*!  connection type attribute: name of the network (not used so far) */
    QString m_myAttribute;
    /*!  authentication request string */
    QString m_authReq;
    /*!  authentication confirmation string */
    QString m_authConf;
    /*!  authentication reply XML Element */
    QDomElement m_authReply;
};

} // end namespace NetworkBase

#endif // NETWORKBASE_NETWORKSERVER_H
