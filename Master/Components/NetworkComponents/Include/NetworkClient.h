/****************************************************************************/
/*! \file NetworkClient.h
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

#ifndef NETWORKBASE_NETWORKCLIENT_H
#define NETWORKBASE_NETWORKCLIENT_H

#include <QtXml>
#include <QTcpSocket>
#include <NetworkComponents/Include/NetworkDevice.h>

namespace NetworkBase {

/// Message opening tag
const QString NC_MSG_ON_TAG   = "<message>";
/// Message closing tag
const QString NC_MSG_OFF_TAG  = "</message>";

/// timeout for server response
#define NC_AUTHENTICATION_TIMEOUT  10000

/*! \warning
     This array contains default configuration parameters for the corresponding clients.
     Do NOT change them unless you change the server network configuration XML file !!! */
const QString ClientConfiguration[][4] = {
    {"Axeda Client", "1.0", "127.0.0.15", "8801"},         ///< client runs in Axeda Process
    {"Skyline Device GUI", "1.0", "127.0.0.16", "8801"},  ///< client runs in Colorado Normal GUI
    {"Sepia Device GUI", "1.0", "127.0.0.17", "8801"},     ///< client runs in Sepia Normal GUI
    {"Himalaya Serice SW", "1.0", "127.0.0.18", "8801"},   ///< client runs in Colorado Service GUI
    {"Sepia Serice SW", "1.0", "127.0.0.19", "8801"},      ///< client runs in Sepia Service GUI
    {"Sepia Client", "1.0", "127.0.0.20", "8801"},         ///< client runs in Sepia (Colorado is Master in WSMode)
    {"Export", "1.0", "127.0.0.21", "8801"},                ///< client runs in Export process
    {"BLG", "1.0", "127.0.0.22", "8801"}                ///< client runs in BLG process
};

/// Possible Network Client errors
typedef enum {
    NC_ALL_OK = 1,
    NC_MSGHANDLER_POINTER_NULL,    ///< no valid pointer to the message handler
    NC_SIGNAL_CONNECT_FAILED,      ///< cannot connect Qt's signal to slot
    NC_WRONG_SERVER_MESSAGE,       ///< unexpected server message during authentication
    NC_AUTHENTICATION_FAILED       ///< authentication timed out
} NetworkClientErrorType_t;

/// Possible Types of network clients
typedef enum {
    NCE_TYPE_UNKNOWN = 1,            ///< uninitialized client
    NCE_TYPE_AXEDA,                  ///< client for communication with axeda server
    NCE_TYPE_HIMALAYA_GUI,           ///< client for communication with himalaya gui server
    NCE_TYPE_SEPIA_GUI,              ///< client for communication with sepia gui server
    NCE_TYPE_HIMALAYA_SERVICE,       ///< client for communication with himalaya service server
    NCE_TYPE_SEPIA_SERVICE,          ///< client for communication with sepia service server
    NCE_TYPE_SEPIA,                  ///< client for communication with colorado device
    NCE_TYPE_INVALID                 ///< wrong client type
} NetworkClientType_t;

/// States of network client
typedef enum {
    NC_INIT = 1,                 ///< created
    NC_NOT_AUTHENTICATED,        ///< authentication request from server received
    NC_AUTHENTICATED,            ///< authenticated
    NC_CONNECT_FAILED            ///< \todo tbd
} ClientConnectionStateType_t;


/****************************************************************************/
/**
 * \brief  This class implements a Network TCP/IP Client.
 *
 *  This Module handles creation and authentication of an outgoing
 *  network connection.
 *
 * \warning Error Handling:
 * All connection errors are reported via "connectionFailed" signal.
 */
/****************************************************************************/
class NetworkClient : public QObject
{
    Q_OBJECT

    /// test classes are all friends:
    friend class TestNetworkClient;
    friend class TestNetworkClientDevice;

public:

    NetworkClient(NetworkClientType_t type, const QString &ip, const QString &port, QObject *pParent = 0);
    void DisconnectFromServer();
    bool Initialize();
    bool RegisterMessageHandler(NetworkDevice *pH);

signals:
   /****************************************************************************/
   /*!
    *  \brief    This signal is emitted when connection with Server has been
    *            successfully  established.
    *
    *  \param    name = client name
    *
    ****************************************************************************/
    void ConnectionEstablished(const QString &name);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when working connection with Server
     *            fails.
     *
     *  \param    name = client name
     *
     ****************************************************************************/
     void ConnectionLost(const QString &name);

   /****************************************************************************/
   /*!
    *  \brief    This signal is emitted when Connection has failed and
    *            shall be destroyed.
    *
    *  \param    error = error code
    *
    ****************************************************************************/
    void ConnectionFailed(NetworkBase::NetworkClientErrorType_t error);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when Connection is authenticated
     *            and messages shall be forwarded to MessageHandler.
     *
     *  \param    msg = message as string
     *
     ****************************************************************************/
    void ForwardToMessageHandler(const QString &msg);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when Connection is authenticated
     *            and messages shall be forwarded to MessageHandler.
     *
     *  \param    type = type of message
     *  \param    msg = message as byte array
     *
     ****************************************************************************/
    void ForwardToMessageHandler(quint8 type, QByteArray &msg);
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to start timer , when working
     *            connection with Server fails
     *
     *
     ****************************************************************************/
     void StartConnectionLostTimer();

public slots:
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of slot UpdateReagentList
      */
     /****************************************************************************/
    void ReadRawSocket();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void HandleSocketError(QAbstractSocket::SocketError);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void HandleAuthTimeout();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void SendMessage(quint8, const QByteArray &ba);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void ConnectToServer();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void DisconnectConnection();

private:

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(NetworkClient)
   void AckConnection();
   void HandleInitAction(const QByteArray &msg);
   void HandleAuthAction(const QByteArray &msg);
   NetworkClientType_t SetConfigParameters();
   void EmitConnectionFailed(NetworkClientErrorType_t err);

private:

    /*!  Client's TCP Socket */
    QTcpSocket m_tcpSocket;
    /*!  Client name (will be read from file according to type) */
    QString m_myName;
    /*!  Client type */
    NetworkClientType_t m_myType;
    /*! protocol message handler */
    NetworkDevice *m_msgHdlr;
    /*!  Client version */
    QString m_myVersion;
    /*!  IP to connect to */
    QString m_Ip;
    /*!  Port to connect to */
    QString m_port;
    /*!  Connection state variable */
    ClientConnectionStateType_t m_authStage;
    /*!  Internal timer */
    QTimer m_timer;
    /*! Block size of incomming mesg */
    qint32 m_BlockSize;
    /*!  Internal timer for Connection establishment*/
    QTimer *mp_Connectiontimer;
};

} // end namespace NetworkBase

#endif // NETWORKBASE_NETWORKCLIENT_H
