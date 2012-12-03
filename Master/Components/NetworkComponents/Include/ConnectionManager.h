/****************************************************************************/
/*! \file ConnectionManager.h
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

#ifndef NETWORKBASE_CONNECTIONMANAGER_H
#define NETWORKBASE_CONNECTIONMANAGER_H

#include <QtNetwork>
#include <QtXml>

#include <NetworkComponents/Include/NetworkServer.h>

namespace NetworkBase {

/// Timeout to wait for authentication response
const quint16 CM_AUTHENTICATION_TIMEOUT = 3000;
/// Enclosing message tag
const QString CM_CMD_TAG_NAME = "cmd";

/// Connection states
typedef enum {
    CM_NOT_AUTHENTICATED = 1,   ///< connection is not authenticated yet
    CM_AUTHENTICATED            ///< connection is already authenticated
} ServerConnectionStateType_t;


/****************************************************************************/
/**
 * \brief  Class which manages an incoming network connection.
 *
 * This Module authenticates and manages an incoming network
 * connection. If authentication is successful, it will be registered
 * with the Network Server. If authentication fails, the connection
 * will be destroyed. Upon successful connection Network Server will
 * connect this object to the client's protocol handler.
 *
 * For each new incoming connection a new ConnectionManager will be created.
 *
 */
/****************************************************************************/
class ConnectionManager : public QObject
{
    Q_OBJECT

    /// all test classes are friends:
    friend class TestConnectionManager;

public:
    ConnectionManager(QHash<QString, QString> hash, quint32 number, const QString &req, const QString &conf, const QDomElement &d, QObject *pParent = 0);
    ~ConnectionManager();
    void DisconnectMe();
    quint32 GetRefNumber();
    QString GetClientName();
    void ForwardMessage(const QString &str);

signals:

   /****************************************************************************/
   /*!
    *  \brief    This signal is emitted when Connection has failed or was
    *            disconnected and shall be destroyed.
    *
    *  \param    number = unique number of the client
    *  \param    name = unique name of the client
    *  \param    dtype = why this object wants to disconnect
    *
    ****************************************************************************/
    void DestroyMe(quint32 number, const QString &name, NetworkBase::DisconnectType_t dtype);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when Client has successfully connected.
     *
     *  \param    number =  unique number of the client
     *  \param    name =  unique name of the client
     *
     ****************************************************************************/
    void ClientConnected(quint32 number, const QString &name);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when Data is received and
     *            the connection is already authenticated.
     *
     *  \param    type = type of message
     *  \param    msg = received message
     *
     ****************************************************************************/
    void MsgReceived(quint8 type, QByteArray &msg);


public slots:
    void HandleNewConnection(int socketDescriptor);
    void ReadSocket();
    void DestroyConnection(NetworkBase::DisconnectType_t dtype);
    void ForwardMessage(quint8 type, const QByteArray &ba);
    void HandleSocketDisconnect();
    void HandleTimeout();

private:
    bool CheckAuthenticationResponse(QByteArray *ba);
    bool ParseAuthenticationResponse(QDomDocument *d);

private:
    /*! TCP-IP Socket */
    QTcpSocket m_TcpSocket;
    /*! Unique reference number assigned by TcpServer */
    quint32 m_myNumber;
    /*! Name of the connecting Client */
    QString m_myClient;
    /*! list of all possible clients with their type strings
        Format: <ClientName, typeString > */
    QHash<QString, QString> m_authenticationStringsList;
    /*! Connection timer */
    QTimer  m_timer;
    /*! Authentication request string */
    QString m_authRequest;
    /*! Authentication confirmation string */
    QString m_authConfirm;
    /*! Authentication reply message */
    QDomElement m_replySection;
    /*! State of connection: authenticated or not */
    ServerConnectionStateType_t m_connectionState;
};

} // end namespace NetworkBase

#endif // NETWORKBASE_CONNECTIONMANAGER_H
