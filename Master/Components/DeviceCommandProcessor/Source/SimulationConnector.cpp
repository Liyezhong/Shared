/****************************************************************************/
/*! \file SimulationConnector.cpp
 *
 *  \brief Implementation file for class SimulationConnector.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2012-11-16
 *  $Author:    $ Michael Thiel
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

#include "DeviceCommandProcessor/Include/SimulationConnector.h"

//#include "NetworkComponents/Include/NetworkServerDevice.h"
#include "Global/Include/SystemPaths.h"
#include "Global/Include/GlobalDefines.h"
#include "Global/Include/Commands/Command.h"
//#include <QtGui/QApplication>
#include <QPair>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <sys/socket.h>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <cxxabi.h>


namespace DeviceCommandProcessor {

bool SimulationConnector::m_pipeBroken = false;
//const NetworkBase::NetworkServerType_t MY_CLIENT_TYPE3 = NetworkBase::NSE_TYPE_SEPIA;

SimulationConnector::SimulationConnector(quint16 port)
    : m_clientConnection(0)
    , m_pCommfile(new QFile("SimulationServer.txt"))
    , m_outStream(m_pCommfile)
    , m_verbose(true)
{
    __sighandler_t oldHandler = signal(SIGPIPE, SimulationConnector::handleBrokenPipe);   // ignore broken pipe errors
    if (SIG_ERR == oldHandler) {
        return;
    }
    m_pReadTimer = NULL;

    m_tcpServer = new QTcpServer(this);
    if (!m_tcpServer->listen(QHostAddress::Any, port))
        qDebug() << m_tcpServer->errorString();

    qDebug() << m_tcpServer->serverAddress();
    qDebug() << m_tcpServer->serverPort();
    qDebug() << m_tcpServer->errorString();

    CONNECTSIGNALSLOT(m_tcpServer, newConnection(), this, registerConnection());

    if (!m_pCommfile->open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    if (m_verbose)
//lint --e{534} suppress "Ignoring return value of function..." in entire braced block
    {
        m_outStream << QDateTime::currentDateTime().time().toString() << " Starting\n";
        m_outStream.flush();
        m_pCommfile->flush();
    }
//    startSimulationGui();
}

SimulationConnector::~SimulationConnector()
{
    try {
        delete m_tcpServer;
        m_tcpServer = NULL;

        m_clientConnection = NULL;
        //stopSimulationGui();

        if (m_pCommfile) {
            m_pCommfile->close();
            delete m_pCommfile;
            m_pCommfile = NULL;
        }
    }
    CATCHALL_DTOR();
}

void SimulationConnector::handleBrokenPipe(int sig)
{
    Q_UNUSED(sig)
    m_pipeBroken = true;
}

void SimulationConnector::startSimulationGui(QString fileName)
{
    QProcess *process = new QProcess(this);
    QString program = "./" + fileName;
    process->start(program);
}

void SimulationConnector::stopSimulationGui(QString processName)
{
    QProcess *process = new QProcess(this);
    QString program = "pkill " + processName;
    process->start(program);
}

void SimulationConnector::registerConnection()
{
    m_outStream << "SimulationConnector::registerConnection\n";
    m_outStream.flush();
    m_clientConnection = m_tcpServer->nextPendingConnection();
    if (m_pReadTimer) {
        delete m_pReadTimer;
        m_pReadTimer = NULL;
    }
//    connect(m_clientConnection, SIGNAL(readyRead()), this, SLOT(readMessageFromClient()));
    m_pReadTimer = new QTimer();
    m_pReadTimer->setInterval(300);
    CONNECTSIGNALSLOT(m_pReadTimer, timeout(), this, readMessageFromClient());

    sendMessageToClient("Connected to Simulation Viewer");

//    int set = 1;
//    setsockopt(sd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
    m_pipeBroken = false;

    foreach (QString message, m_pendingMessages)
    {
        sendMessageToClient(message);
    }
    m_pendingMessages.clear();
    m_pReadTimer->start();
}

void SimulationConnector::sendMessageToClient(QString message)
{
    if ((!m_clientConnection) || (m_pipeBroken)) {
        m_pendingMessages.append(message);
        if ((m_pipeBroken) && (m_clientConnection)) {
            m_clientConnection->close();
            delete m_clientConnection;
            m_clientConnection = 0;
            stopSimulationGui("SimulationView");
        }
        return;
    }

    //qDebug() << "SimulationConnector::sendMessage" << message;

    try
//lint --e{534} suppress "Ignoring return value of function..." in entire braced block
    {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion((int)QDataStream::Qt_4_0);
        out << message << "\r";
        if (out.device()->reset()) {
            if (out.device()->isOpen()) {
                if (m_clientConnection->isOpen())
                {
//                m_clientConnection->flush();
                    m_pipeBroken = false;
                    m_clientConnection->write(block);
                    //QTBUG-14464
//                    if (!m_pipeBroken) {
//                        m_clientConnection->flush();
//                    }
                }
            }
        }
        if (m_verbose)
        {
            if (!message.contains("CurrentTime"))
            {
                m_outStream << QDateTime::currentDateTime().toTime_t() << " SimulationConnector Send: " << message << "\n";
                m_outStream.flush();
                m_pCommfile->flush();
            }
        }

        return;
    }
    CATCHALL();
    qDebug() << "!!!!!!! SimulationConnector::sendMessageToClient, Exception caught" << message;
}

QDomDocument SimulationConnector::ReadXmlFromFile(QString filename)
{
    QDomDocument doc("CommandEvents");
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly))
    {
//lint -e{534} suppress "Ignoring return value of function..." in next statement
        doc.setContent(&file);
    }

    file.close();
    return doc;
}

void SimulationConnector::readMessageFromClient()
{
    if (!m_clientConnection) {
        return;
    }

    QString input = "";
    int bytesAvail = 0;
//    if (m_tcpSocket->waitForReadyRead(10)) {
        bytesAvail = m_clientConnection->bytesAvailable();
//    }
    if (bytesAvail > 0) {
        int cnt = 0;
        bool endOfLine = false;
        bool endOfStream = false;
        while (cnt < bytesAvail && (!endOfLine) && (!endOfStream)) {
            char ch;
            int bytesRead = m_clientConnection->read(&ch, sizeof(ch));
            if (bytesRead == sizeof(ch)) {
                cnt++;
                if ((int)ch > 33) {
                    input.append(ch);
                }
                //input = input.trimmed();
                if (ch == '\r') {
                    endOfLine = true;
                }
            }
            else {
                endOfStream = true;
            }
        }
        input = input.replace(QChar('"'), QChar(' '));
        input = input.replace(" ", "");
    }

    if (input.length() < 2) {
        return;
    }

    if (m_verbose)
    {
        if (!input.contains("CurrentTime"))
//lint --e{534} suppress "Ignoring return value of function..." in entire braced block
        {
            m_outStream << QDateTime::currentDateTime().toTime_t() << " SimulationConnector Read: " << input << "\n";
            m_outStream.flush();
            m_pCommfile->flush();
        }
    }
    emit messageFromClient(input);
}

}
