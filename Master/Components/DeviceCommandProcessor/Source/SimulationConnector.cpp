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
#include <QtGui/QApplication>
#include <QPair>
#include <QDir>
#include <QDebug>
#include <QDateTime>



namespace DeviceCommandProcessor {

//const NetworkBase::NetworkServerType_t MY_CLIENT_TYPE3 = NetworkBase::NSE_TYPE_SEPIA;

SimulationConnector::SimulationConnector()
    : m_clientConnection(0)
    , m_commfile(new QFile("simulationInput.txt"))
    , m_outStream(m_commfile)
    , m_verbose(true)
{

    m_tcpServer = new QTcpServer(this);
    if (!m_tcpServer->listen(QHostAddress::Any, 8889))
        qDebug() << m_tcpServer->errorString();

    qDebug() << m_tcpServer->serverAddress();
    qDebug() << m_tcpServer->serverPort();
    qDebug() << m_tcpServer->errorString();

    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(registerConnection()));

    if (!m_commfile->open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    if (m_verbose)
    {
        m_outStream << QDateTime::currentDateTime().time().toString() << " Starting\n";
        m_outStream.flush();
        m_commfile->flush();
    }
//    startSimulationGui();
}

void SimulationConnector::startSimulationGui()
{
    QProcess *process = new QProcess(this);
    QString program = "./SchedulerSimulation";
    process->start(program);
}

void SimulationConnector::registerConnection()
{
    qDebug() << "SimulationConnector::registerConnection";
    m_clientConnection = m_tcpServer->nextPendingConnection();
    connect(m_clientConnection, SIGNAL(readyRead()), this, SLOT(readMessageFromClient()));
    sendMessageToClient("Connected to Colorado simulation");
}

void SimulationConnector::sendMessageToClient(QString message)
{
    if (!m_clientConnection)
        return;

//    qDebug() << "SimulationConnector::sendMessage" << message;

    try
    {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << message;
        if (m_clientConnection->isOpen())
        {
            m_clientConnection->write(block);
            m_clientConnection->flush();
        }

        if (m_verbose)
        {
            if (!message.contains("CurrentTime"))
            {
                m_outStream << QDateTime::currentDateTime().toTime_t() << " SimulationConnector Send: " << message << "\n";
                m_outStream.flush();
                m_commfile->flush();
            }
        }
    }
    catch (...)
    {
        qDebug() << "!!!!!!! SimulationConnector::sendMessageToClient, Exception caught" << message;
    }
}

QDomDocument SimulationConnector::ReadXmlFromFile(QString filename)
{
    QDomDocument doc("CommandEvents");
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly))
    {
        doc.setContent(&file);
    }

    file.close();
    return doc;
}

void SimulationConnector::readMessageFromClient()
{
    QDataStream in(m_clientConnection);
    in.setVersion(QDataStream::Qt_4_0);
    QString input;
    in >> input;

    if (m_verbose)
    {
        if (!input.contains("CurrentTime"))
        {
            m_outStream << QDateTime::currentDateTime().toTime_t() << " SimulationCommandThreadController Read: " << input << "\n";
            m_outStream.flush();
            m_commfile->flush();
        }
    }
    emit messageFromClient(input);
}

}
