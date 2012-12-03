/****************************************************************************/
/*! \file connection.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 03.11.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class Connection
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

#include "DeviceControl/Include/CanCommunication/CanTcpCommunication/connection.h"
#include "DeviceControl/Include/Global/DeviceControlError.h"
#include "DeviceControl/Include/Global/dcl_log.h"

#include <QtNetwork>

namespace DeviceControl
{

static const int TransferTimeout = 30 * 1000;  ///< timeout for data transfer
static const int PongTimeout = 60 * 1000;      ///< timeout for pong acknowledge
static const int PingInterval = 5 * 1000;      ///< ping interval
static const char SeparatorToken = ' ';        ///< separator

/****************************************************************************/
/*!
 *  \brief    Constructor of the Connection class
 *
 *  \param    pParent = Parent as passed to the QObject constructor
 ****************************************************************************/
Connection::Connection(QObject *pParent)
    : QTcpSocket(pParent)
{
    m_GreetingMessage = tr("undefined");
    m_ConnectionName = tr("unknown");
    m_ConnectState = WaitingForGreeting;
    m_CurrentDataType = Undefined;
    m_NumBytesForCurrentDataType = -1;
    transferTimerId = 0;
    isGreetingMessageSent = false;
    m_PingTimer.setInterval(PingInterval);
    m_myIp = "";
    m_myPort = "";
}

/****************************************************************************/
/*!
 *  \brief    Returns the name property of the connection
 *
 *  \return   Name property
 ****************************************************************************/
QString Connection::name() const
{
    return m_ConnectionName;
}

/****************************************************************************/
/*!
 *  \brief    Set the greetingMessage property
 *
 *  \param    Message = gretingMessage content to set
 ****************************************************************************/
void Connection::SetGreetingMessage(const QString &Message)
{
    m_GreetingMessage = Message;
}

/****************************************************************************/
/*!
 *  \brief    Sends a CAN message via TCP/IP
 *
 *  \param    pCANframe = CAN message to send
 *
 *  \return   True if CAN message was send, otherwise false
 ****************************************************************************/
bool Connection::SendCANMessage(can_frame* pCANframe)
{
    quint16 dataSize;
    QByteArray canData;
    QByteArray outData;
    QDataStream outStream(&canData, QIODevice::WriteOnly);

    FILE_LOG_L(laSIM, llDEBUG4) << " CAN-> TCP: 0x" << std::hex << pCANframe->can_id  << " : " <<
            std::hex << (int)pCANframe->data[0] << "-" << std::hex << (int)pCANframe->data[1] << "-" <<
            std::hex << (int)pCANframe->data[2] << "-" << std::hex << (int)pCANframe->data[3] <<
            std::hex << (int)pCANframe->data[4] << "-" << std::hex << (int)pCANframe->data[5] << "-" <<
            std::hex << (int)pCANframe->data[6] << "-" << std::hex << (int)pCANframe->data[7] << "-" << (int)pCANframe->can_dlc;

    outStream.setVersion((int)QDataStream::Qt_4_0);
    dataSize = 15;
    outStream << (quint16) 0;
    outStream << (quint32) pCANframe->can_id;
    outStream << (quint8)  pCANframe->data[0];
    outStream << (quint8)  pCANframe->data[1];
    outStream << (quint8)  pCANframe->data[2];
    outStream << (quint8)  pCANframe->data[3];
    outStream << (quint8)  pCANframe->data[4];
    outStream << (quint8)  pCANframe->data[5];
    outStream << (quint8)  pCANframe->data[6];
    outStream << (quint8)  pCANframe->data[7];
    outStream << (quint8)  pCANframe->can_dlc;
    if(outStream.device()->seek(0))
    {
        outStream << (quint16) dataSize;   //data length

        outData = "CANMESSAGE " + QByteArray::number(dataSize) + " " + canData;

        if(write(outData) >= 0)
        {
            return true;
        }
    }

    return false;
}

/****************************************************************************/
/*!
 *  \brief    Initialise the connection
 *
 *  \param     ip = IP address
 *  \param     port = communication port
 *  \return    True if initialisation was done, otherwise false
 ****************************************************************************/
bool Connection::Initialize(QString ip, QString port)
{
    m_myIp = ip;
    m_myPort = port;

    if (!QObject::connect(this, SIGNAL(readyRead()), this, SLOT(processReadyRead()))) {
        qDebug() << "Connection: cannot connect 'readyRead' signal !";
        return false;
    }

    if (!QObject::connect(&m_PingTimer, SIGNAL(timeout()), this, SLOT(sendPing()))) {
        qDebug() << "Connection: cannot connect 'timeout' signal !";
        return false;
    }
    if (!QObject::connect(this, SIGNAL(connected()), this, SLOT(sendGreetingMessage()))) {
        qDebug() << "Connection: cannot connect 'connected' signal !";
        return false;
    }
    if (!QObject::connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
                          this, SLOT(handleSocketError(QAbstractSocket::SocketError)))) {
        qDebug() << (QString)("Connection: cannot connect 'error' signal !");
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief    Try to establish a connection to host
 *
 ****************************************************************************/
void Connection::connectToServer()
{
    abort();
    if (m_myIp.isEmpty() || m_myPort.isEmpty()) {
        qDebug() << (QString)("Connection: cannot connect to IP/PORT --> " + m_myIp + "/" + m_myPort);
        return;
    }
    qDebug() << "Connection: trying to connect to HOST ---> " + m_myIp + "/" + m_myPort;
    connectToHost(QHostAddress(m_myIp), m_myPort.toUInt());
//Note: no need to start timer here because connectToHost will emit
//      an error signal, which we receive and process in handleSocketError.
}

/****************************************************************************/
/*!
 *  \brief    handle a socket error by reconneting and reporting
 *
 *  \param    err = Socket error
 *
 ****************************************************************************/
void Connection::handleSocketError(QAbstractSocket::SocketError err)
{
    /*
     * if these errors happen, we shall continue pinging.
     * in all other cases we go down:
       QAbstractSocket::ConnectionRefusedError
       QAbstractSocket::SocketTimeoutError
     */

    if (m_ConnectState == WaitingForGreeting) {
        // if connection was not yet established, client shall try
        // to ping server for connection.
        if ((err == QAbstractSocket::ConnectionRefusedError) ||
            (err == QAbstractSocket::SocketTimeoutError)) {
            QTimer::singleShot(5000, this, SLOT(connectToServer()));
            return;
        }
    }

    qDebug() << "Connection: SOCKET ERROR ---> " + QString::number((int)err, 10);

    // if connection was established and fails, close socket and set initial state
    disconnectFromServer();
    // working connection failed --> try to re-establish it
    QTimer::singleShot(5000, this, SLOT(connectToServer()));
}

/****************************************************************************/
/*!
 *  \brief    Disconnect from host and close the connection
 *
 *
 ****************************************************************************/
void Connection::disconnectFromServer()
{
    m_PingTimer.stop();
    disconnectFromHost();
    close();

    // reset all parameters
    m_ConnectionName = tr("unknown");
    m_ConnectState = WaitingForGreeting;
    m_CurrentDataType = Undefined;
    m_NumBytesForCurrentDataType = -1;
    transferTimerId = 0;
    isGreetingMessageSent = false;

    qDebug() << "Connection: disconnected from Server !";
}

/****************************************************************************/
/*!
 *  \brief    Process received data
 *            This slot is connected to the QTcpSocket's readyRead signal,
 *            which is emitted once every time new data is available for reading from the device.
 *
 ****************************************************************************/
void Connection::processReadyRead()
{
    if (m_ConnectState == WaitingForGreeting) {
        if (!readProtocolHeader())
            return;
        if (m_CurrentDataType != Greeting) {
            abort();
            return;
        }
        m_ConnectState = ReadingGreeting;
    }

    if (m_ConnectState == ReadingGreeting) {
        if (!hasEnoughData())
            return;

        m_Buffer = read(m_NumBytesForCurrentDataType);
        if (m_Buffer.size() != m_NumBytesForCurrentDataType) {
            abort();
            return;
        }

        m_ConnectionName = QString(m_Buffer) + "@" + peerAddress().toString() + ":"
                   + QString::number(peerPort());
        m_CurrentDataType = Undefined;
        m_NumBytesForCurrentDataType = 0;
        m_Buffer.clear();

        if (!isValid()) {
            abort();
            return;
        }

        if (!isGreetingMessageSent)
            sendGreetingMessage();

        m_PingTimer.start();
        m_PongTime.Trigger();
        m_ConnectState = ReadyForUse;
        emit readyForUse();
    }

    do {
        if (m_CurrentDataType == Undefined) {
            if (!readProtocolHeader())
                return;
        }
        if (!hasEnoughData())
            return;
        processData();
    } while (bytesAvailable() > 0);
}

/****************************************************************************/
/*!
 *  \brief    Send the "Ping" message
 *
 ****************************************************************************/
void Connection::sendPing()
{
    if (m_PongTime.Elapsed() > PongTimeout) {
        abort();
        return;
    }

    if(write("PING 1 p") == -1)
    {
        qDebug() << "Connection: error  while writing 'PING'";
    }
}

/******************************************************************/
/**
 *  \brief     Write the grettingMessage content to the socket
 *
 */
 /******************************************************************/
void Connection::sendGreetingMessage()
{
    QByteArray greeting = m_GreetingMessage.toUtf8();
    QByteArray data = "GREETING " + QByteArray::number(greeting.size()) + " " + greeting;
    if (write(data) == data.size())
        isGreetingMessageSent = true;
}

/******************************************************************/
/**
 *  \brief     Read data from socket and copy it to m_Buffer
 *
 *  \param     maxSize = Maximum number of bytes read to m_Buffer
 *
 *  \return    Number of bytes read
 */
 /******************************************************************/
int Connection::readDataIntoBuffer(int maxSize)
{
    if (maxSize > MaxBufferSize)
        return 0;

    int numBytesBeforeRead = m_Buffer.size();
    if (numBytesBeforeRead == MaxBufferSize) {
        abort();
        return 0;
    }

    while (bytesAvailable() > 0 && m_Buffer.size() < maxSize) {
        m_Buffer.append(read(1));
        if (m_Buffer.endsWith(SeparatorToken))
            break;
    }
    return m_Buffer.size() - numBytesBeforeRead;
}

/******************************************************************/
/**
 *  \brief     Returns the data type specific lenght
 *
 *  \return    Number of byte to read for this data type
 */
 /******************************************************************/
int Connection::dataLengthForCurrentDataType()
{
    qint64 byteAvailable = bytesAvailable();
    int readBytes = readDataIntoBuffer();
    bool fBufferEndsWithToken = m_Buffer.endsWith(SeparatorToken);
    //qDebug() << "    DataLength:" << byteAvailable << ", " << readBytes << ", " << fBufferEndsWithToken;

    if((byteAvailable <= 0) || (readBytes <= 0) || !fBufferEndsWithToken)
        return 0;

    //if (bytesAvailable() <= 0 || readDataIntoBuffer() <= 0
    //        || !buffer.endsWith(SeparatorToken))
    //    return 0;

    m_Buffer.chop(1);
    int number = m_Buffer.toInt();
    //qDebug() << "  DataTypeLength:" << number;
    m_Buffer.clear();
    return number;
}

/******************************************************************/
/**
 *  \brief     Read the protocoll header and assign the current data type
 *
 *  \return    True of data type assignment was done, otherwise false
 */
 /******************************************************************/
bool Connection::readProtocolHeader()
{
    if (transferTimerId) {
        killTimer(transferTimerId);
        transferTimerId = 0;
    }

    if (readDataIntoBuffer() <= 0) {
        transferTimerId = startTimer(TransferTimeout);
        return false;
    }

    if (m_Buffer == "PING ") {
        m_CurrentDataType = Ping;
    }
    else if (m_Buffer == "PONG ") {
        m_CurrentDataType = Pong;
    }
    else if (m_Buffer == "CANMESSAGE ") {
        m_CurrentDataType = CanMessage;
    }
    else if (m_Buffer == "GREETING ") {
        m_CurrentDataType = Greeting;
    }
    else {
        m_CurrentDataType = Undefined;
        abort();
        return false;
    }

    m_Buffer.clear();
    m_NumBytesForCurrentDataType = dataLengthForCurrentDataType();

    return true;
}

/******************************************************************/
/**
 *  \brief     Checks wether enought bytes have been received
 *
 *  \return    True of enough data are available, otherwise false
 */
 /******************************************************************/
bool Connection::hasEnoughData()
{
    if (transferTimerId) {
        QObject::killTimer(transferTimerId);
        transferTimerId = 0;
    }

    if (m_NumBytesForCurrentDataType <= 0)
        m_NumBytesForCurrentDataType = dataLengthForCurrentDataType();

    if (bytesAvailable() < m_NumBytesForCurrentDataType
            || m_NumBytesForCurrentDataType <= 0) {
        transferTimerId = startTimer(TransferTimeout);
        return false;
    }

    return true;
}

/******************************************************************/
/**
 *  \brief     Process the received data
 *
 */
 /******************************************************************/
void Connection::processData()
{
    m_Buffer = read(m_NumBytesForCurrentDataType);
    if (m_Buffer.size() != m_NumBytesForCurrentDataType) {
        abort();
        return;
    }

    switch (m_CurrentDataType) {
    case CanMessage:
        {
            quint32 ID;
            quint8 data0, data1, data2, data3, data4, data5, data6, data7, dlc;
            can_frame CANframe;

            ID = m_Buffer.data()[2] << 24;
            ID += m_Buffer.data()[3] << 16;
            ID += m_Buffer.data()[4] << 8;
            ID += m_Buffer.data()[5];
            data0 = m_Buffer.data()[6];
            data1 = m_Buffer.data()[7];
            data2 = m_Buffer.data()[8];
            data3 = m_Buffer.data()[9];
            data4 = m_Buffer.data()[10];
            data5 = m_Buffer.data()[11];
            data6 = m_Buffer.data()[12];
            data7 = m_Buffer.data()[13];
            dlc = m_Buffer.data()[14];

            CANframe.can_id  = m_Buffer.data()[2] << 24;
            CANframe.can_id  += m_Buffer.data()[3] << 16;
            CANframe.can_id  += m_Buffer.data()[4] << 8;
            CANframe.can_id  += m_Buffer.data()[5];
            CANframe.data[0] = m_Buffer.data()[6];
            CANframe.data[1] = m_Buffer.data()[7];
            CANframe.data[2] = m_Buffer.data()[8];
            CANframe.data[3] = m_Buffer.data()[9];
            CANframe.data[4] = m_Buffer.data()[10];
            CANframe.data[5] = m_Buffer.data()[11];
            CANframe.data[6] = m_Buffer.data()[12];
            CANframe.data[7] = m_Buffer.data()[13];
            CANframe.can_dlc  = m_Buffer.data()[14];

            QString strCANMessage;
            strCANMessage = QString("0x%1: %2 %3 %4 %5 %6 %7 %8 %9").arg(CANframe.can_id, 8, 16, QChar('0'))
                                                                    .arg(data0, 2, 16, QChar('0'))
                                                                    .arg(data1, 2, 16, QChar('0'))
                                                                    .arg(data2, 2, 16, QChar('0'))
                                                                    .arg(data3, 2, 16, QChar('0'))
                                                                    .arg(data4, 2, 16, QChar('0'))
                                                                    .arg(data5, 2, 16, QChar('0'))
                                                                    .arg(data6, 2, 16, QChar('0'))
                                                                    .arg(data7, 2, 16, QChar('0'));
            dlc = CANframe.can_dlc;

            if(ID == 0x1037a10c)
            {
                ID = 0x1038a10c;
            }

            emit newCANMessage(ID, data0, data1, data2, data3, data4, data5, data6, data7, dlc);
        }
        break;

    case Ping:
        if(write("PONG 1 p") == -1)
        {
            qDebug() << "Connection: error  while writing 'PING'";
        }
        break;
    case Pong:
        m_PongTime.Trigger();
        break;
    default:
        break;
    }

    m_CurrentDataType = Undefined;
    m_NumBytesForCurrentDataType = 0;
    m_Buffer.clear();
}

} //namespace
