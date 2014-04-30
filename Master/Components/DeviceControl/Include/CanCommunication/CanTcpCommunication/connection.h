/****************************************************************************/
/*! \file connection.h
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 03.11.2010
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class Connection
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
#ifndef CONNECTION_H
#define CONNECTION_H

#include "Global/Include/MonotonicTime.h"

#include <QHostAddress>
#include <QString>
#include <QTcpSocket>
#include <QTimer>
#include <QDebug>

//can_frame
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>
#include <linux/can.h>

namespace DeviceControl
{

static const int MaxBufferSize = 1024000;  //!< maximum buffer size

/****************************************************************************/
/*! \brief This class implements the Tcp connection functionality used by
 *         the simulation
 *
*/
/****************************************************************************/
class Connection : public QTcpSocket
{
    Q_OBJECT

public:
    /*! Connection state definitions */
    enum ConnectionState {
        WaitingForGreeting,   //!< start up
        ReadingGreeting,      //!< establishing connection
        ReadyForUse           //!< ready for use, connection established
    };

    /*! Data type definitions */
    enum DataType {
        PlainText,            //!< just Text
        CanMessage,           //!< CAN message
        Ping,                 //!< Ping message
        Pong,                 //!< Pong
        Greeting,             //!< greeting message, start connection
        Undefined             //!< undefined
    };

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Connection
     *
     *  \param pParent = QObject type parameter
     *
     *  \return from Connection
     */
    /****************************************************************************/
    Connection(QObject *pParent = 0);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function name
     *
     *  \return from name
     */
    /****************************************************************************/
    QString name() const;

    void SetGreetingMessage(const QString &message);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendCANMessage
     *
     *  \param pCANframe = can_frame type parameter
     *
     *  \return from SendCANMessage
     */
    /****************************************************************************/
    bool SendCANMessage(can_frame* pCANframe);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Initialize
     *
     *  \param ip = QString type parameter
     *  \param port =  QString type parameter
     *
     *  \return from Initialize
     */
    /****************************************************************************/
    bool Initialize(QString ip, QString port);

signals:
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when the connection is ready
     *
     ****************************************************************************/
    void readyForUse();
    //void newMessage(const QString &from, const QString &message);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to report a received CAN message
     *
     *  \param   ID = CAN message ID
     *  \param   data0    = Date byte 0
     *  \param   data1   =  Date byte 1
     *  \param   data2   =  Date byte 2
     *  \param   data3   =  Date byte 3
     *  \param   data4   =  Date byte 4
     *  \param   data5   =  Date byte 5
     *  \param   data6   =  Date byte 6
     *  \param   data7   =  Date byte 7
     *  \param   dlc
     *
     ****************************************************************************/
    void newCANMessage(quint32 ID, quint8 data0, quint8 data1, quint8 data2, quint8 data3, quint8 data4, quint8 data5, quint8 data6, quint8 data7, quint8 dlc);

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot connectToServer
     */
    /****************************************************************************/
    void connectToServer();

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot processReadyRead
     */
    /****************************************************************************/
    void processReadyRead();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot sendPing
     */
    /****************************************************************************/
    void sendPing();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot sendGreetingMessage
     */
    /****************************************************************************/
    void sendGreetingMessage();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot handleSocketError
     */
    /****************************************************************************/
    void handleSocketError(QAbstractSocket::SocketError);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot disconnectFromServer
     */
    /****************************************************************************/
    void disconnectFromServer();

private:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function readDataIntoBuffer
     *
     *  \param maxSize = int type parameter
     *
     *  \return from readDataIntoBuffer
     */
    /****************************************************************************/
    int readDataIntoBuffer(int maxSize = MaxBufferSize);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function dataLengthForCurrentDataType
     *
     *  \return from dataLengthForCurrentDataType
     */
    /****************************************************************************/
    int dataLengthForCurrentDataType();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function readProtocolHeader
     *
     *  \return from readProtocolHeader
     */
    /****************************************************************************/
    bool readProtocolHeader();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function hasEnoughData
     *
     *  \return from hasEnoughData
     */
    /****************************************************************************/
    bool hasEnoughData();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function processData
     *
     *  \return from processData
     */
    /****************************************************************************/
    void processData();

    QString m_GreetingMessage;              //!< greeting message for connection establishing
    QString m_ConnectionName;                 //!< connection name
    QTimer m_PingTimer;                       //!< ping pong timer
    Global::MonotonicTime m_PongTime;         //!< ping pong time delay
    QByteArray m_Buffer;                      //!< buffer for receive data
    ConnectionState m_ConnectState;           //!< connection state
    DataType m_CurrentDataType;             //!< current data type
    int      m_NumBytesForCurrentDataType;  //!< number of byte to receive for current data type
    int transferTimerId;                    //!< Timer for the timeout of the current message
    bool isGreetingMessageSent;             //!< Indicates if the greeting message was sent

    QString m_myIp;                         //!< IP-address
    QString m_myPort;                       //!< Communication port
};

} //namespace

#endif // CONNECTION_H
