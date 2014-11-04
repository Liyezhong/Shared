/****************************************************************************/
/*! \file client.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 03.11.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class Client
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

#include "DeviceControl/Include/CanCommunication/CanTcpCommunication/client.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/SystemPaths.h"

#include <QDomDocument>
#include <QtNetwork>

namespace DeviceControl
{

Can2TcpClient::Can2TcpClient() :
    m_pConnection(0)
{
}

Can2TcpClient::~Can2TcpClient()
{
    try
    {
        Connection *connection;
        QList<Connection *> connections = m_Peers.values();
        QListIterator<Connection*> iterConnection(connections);

        iterConnection.toFront();
        while (iterConnection.hasNext())
        {
            connection = iterConnection.next();
            delete connection;
            connection = 0;
        }

        /*foreach (Connection *connection, connections)
        {
            delete connection;
        }*/
        m_Peers.clear();

        if(m_pConnection)
        {
            m_pConnection = 0;
        }
    }
    catch (...)
    {
        // and exit
        return;
    }

}

/****************************************************************************/
/*!
 *  \brief    Initialisation function
 *
 *  \return   true if initialisation has been done, otherwise false
 *
 ****************************************************************************/
bool Can2TcpClient::Initialize()
{
    QString ip = "";
    QString port = "";

    // read XML config
    if (!ReadNetworkConfig(&ip, &port)) {
        LOG() << "CAN-TCP-CLIENT: cannot read network settings !";
        return false;
    }

    if (ip.isEmpty() || port.isEmpty()) {
        LOG() << "CAN-TCP-CLIENT: network settings are empty !";
        return false;
    }

    try {
        m_pConnection = new Connection(this);
    }
    catch (const std::bad_alloc &) {
        LOG() << "CAN-TCP-CLIENT: cannot create 'Connection' !";
        return false;
    }

    if (!m_pConnection->Initialize(ip, port)) {
        LOG() << "CAN-TCP-CLIENT: cannot initialize 'Connection' !";
        return false;
    }

    m_pConnection->SetGreetingMessage("TCP_User");

    if (!QObject::connect(m_pConnection, SIGNAL(readyForUse()), this, SLOT(readyForUse()))) {
        LOG() << "CAN-TCP-CLIENT: cannot connect Connection's readyForUse signal !";
        return false;
    }

    // start pinging the server:
    m_pConnection->connectToServer();

    return true;
}

/****************************************************************************/
/*!
 *  \brief    Read the simulation network settings file
 *
 *  \param    ip = IP-address of the simulation
 *  \param    port = TCP/IP port used for simulation
 *  \return   true if all settings have been read, otherwise false
 *
 ****************************************************************************/
bool Can2TcpClient::ReadNetworkConfig(QString *ip, QString *port)
{
    QString filename = Global::SystemPaths::Instance().GetSettingsPath() + "/network_config.xml";

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        LOG() << "CAN-TCP-CLIENT: cannot open file " + filename;
        return false;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;
    QDomDocument domDocument;
    if (!domDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
        LOG() << "CAN-TCP-CLIENT: cannot parse XML file " + filename;
        return false;
    }

    QDomElement root = domDocument.documentElement();

    QDomElement child = root.firstChildElement("servertype");
    QString type = child.firstChildElement().text();
    if (type.isEmpty()) {
        LOG() << "CAN-TCP-CLIENT: no server type defined !";
        return false;
    }

    child = root.firstChildElement("servers");
    child = child.firstChildElement(type);
    child = child.firstChildElement("server");
    if (child.isNull()) {
        LOG() << "CAN-TCP-CLIENT: no servers configured for type --> " + type;
        return false;
    }

    QString myip = child.firstChildElement().text();
    QString myport = child.lastChildElement().text();

    if (myip.isEmpty() || myport.isEmpty()) {
        LOG() << "CAN-TCP-CLIENT: ip and port config parameters are empty !";
        return false;
    }

    *ip = myip;
    *port = myport;

    return true;
}

/****************************************************************************/
/*!
 *  \brief    chacks wether there is a stablished connection matches the passed parameters
 *
 *  \param    SenderIp = Requested IP-address
 *  \param    SenderPort = Requested TCP/IP port
 *  \return   true if connection was found, otherwise false
 *
 ****************************************************************************/
bool Can2TcpClient::HasConnection(QHostAddress SenderIp, int SenderPort)
{
    if (SenderPort == -1)
/*lint -save -e1025 */
        return m_Peers.contains((QHostAddress)SenderIp);
/*lint -restore */

/*lint -save -e1025 */
    if (!m_Peers.contains((QHostAddress)SenderIp))
/*lint -restore */
        return false;

/*lint -save -e1025 */
    QList<Connection *> connections = m_Peers.values(SenderIp);
/*lint -restore */

    Connection* connection;
    QListIterator<Connection*> iterConnection(connections);

    iterConnection.toFront();
    while (iterConnection.hasNext())
    {
        connection = iterConnection.next();
        if (connection->peerPort() == SenderPort)
            return true;
    }

    return false;
}

/****************************************************************************/
/*!
 *  \brief    This slot will be called when the 'readyForUse' signal was emitted by the connection
 *            It signals a established connection
 *
 ****************************************************************************/
void Can2TcpClient::readyForUse()
{
    Connection *connection = qobject_cast<Connection *>(sender());
/*lint -save -e64 */
    if (!connection || HasConnection(connection->peerAddress(), connection->peerPort()))
/*lint -restore */
        return;

    /*if(!connect(connection, SIGNAL(newMessage(QString,QString)),
            this, SIGNAL(newMessage(QString,QString))))
    {
        LOG() << "Connection: cannot connect 'newMessage' signal !";
    }*/

    if(!connect(connection, SIGNAL(newCANMessage(quint32, quint8, quint8, quint8, quint8, quint8, quint8, quint8, quint8, quint8)),
            this, SIGNAL(newCANMessage(quint32, quint8, quint8, quint8, quint8, quint8, quint8, quint8, quint8, quint8))))
    {
        LOG() << "Connection: cannot connect 'newCANMessage' signal !";
    }

/*lint -save -e534 */
    m_Peers.insert(connection->peerAddress(), connection);
/*lint -restore */

    /*QHashIterator<QHostAddress, Connection*>::iterator iterHostAddress(m_Peers);
    iterHostAddress = m_Peers.insert(connection->peerAddress(), connection);
    Q_UNUSED(iterHostAddress);*/

    QString nick = connection->name();

    if (!nick.isEmpty())
    {
        emit newParticipant(nick);
    }
}

/****************************************************************************/
/*!
 *  \brief    Forward CAN message to connection object
 *
 *  \param    pCANframe = CAN message to send
 *
 ****************************************************************************/
void Can2TcpClient::SendMessage(can_frame* pCANframe)
{
    if (!pCANframe)
    {
        return;
    }

    QList<Connection *> connections = m_Peers.values();

    Connection* connection;
    QListIterator<Connection*> iterConnection(connections);

    iterConnection.toFront();
    while (iterConnection.hasNext())
    {
        connection = iterConnection.next();
        if(!connection->SendCANMessage(pCANframe))
        {
            LOG() << "Client: sendCANMessage failed !";
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot will be called when the 'disconnected' signal was emitted by the connection
 *
 ****************************************************************************/
void Can2TcpClient::disconnected()
{

}

/****************************************************************************/
/*!
 *  \brief    This slot will be called when the 'error' signal was emitted by the connection
 *
 *  \param    SocketError = Socket error description
 *
 ****************************************************************************/
void Can2TcpClient::connectionError(QAbstractSocket::SocketError SocketError)
{
    Q_UNUSED(SocketError);

}

} // namespace
