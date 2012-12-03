/****************************************************************************/
/*! \file client.h
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 03.11.2010
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class Client
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
#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QMultiHash>

//can_frame
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>
#include <linux/can.h>

#include "DeviceControl/Include/CanCommunication/CanTcpCommunication/connection.h"


namespace DeviceControl
{


/****************************************************************************/
/*! \brief This is implements the conversion of CAN-Messages to TCP pakets used for
 *         the simulation
 *
*/
/****************************************************************************/
class Can2TcpClient : public QObject
{
    Q_OBJECT

public:
    Can2TcpClient();
    ~Can2TcpClient();

    bool HasConnection(QHostAddress senderIp, int senderPort = -1);
    void SendMessage(can_frame* pCANframe);
    bool Initialize();

signals:
    /****************************************************************************/
    /**
     * \brief This signal emits a new message's information
     *
     * \param  From = Sender of the message
     * \param  Message = Message string
     */
    /****************************************************************************/
    void newMessage(const QString &From, const QString &Message);
    /****************************************************************************/
    /**
     * \brief This signal emits a new CAN message's information
     *
     * \param  CANID = CAN message ID
     * \param  DB0 = Data byte 0
     * \param  DB1 = Data byte 1
     * \param  DB2 = Data byte 2
     * \param  DB3 = Data byte 3
     * \param  DB4 = Data byte 4
     * \param  DB5 = Data byte 5
     * \param  DB6 = Data byte 6
     * \param  DB7 = Data byte 7
     * \param  DLC = Data code length of the message
     */
    /****************************************************************************/
    void newCANMessage(quint32 CANID, quint8 DB0, quint8 DB1, quint8 DB2, quint8 DB3, quint8 DB4, quint8 DB5, quint8 DB6, quint8 DB7, quint8 DLC);

    /****************************************************************************/
    /**
     * \brief This signal is emitted when a new connection is established
     *
     * \param  nick = Connection name
     */
    /****************************************************************************/
    void newParticipant(const QString &nick);

private slots:
    void connectionError(QAbstractSocket::SocketError socketError);
    void disconnected();
    void readyForUse();

private:
    //!<  Read the simulation network settings file
    bool ReadNetworkConfig(QString *ip, QString *port);

private:
    //Can2TcpServer m_Server;   //!< Communication server instance
    QMultiHash<QHostAddress, Connection *> m_Peers;  //!< Host addresses table
    Connection *m_pConnection;        //!< Connection object
};

} //namespace

#endif // CLIENT_H
