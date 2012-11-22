/****************************************************************************/
/*! \file DataLogging/Source/EventFilterNetworkServer.cpp
 *
 *  \brief Implementation file for class EventFilterNetworkServer.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
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

#include <DataLogging/Include/EventFilterNetworkServer.h>
#include <DataLogging/Include/EventFilter.h>
#include <Global/Include/TranslatableString.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>

#include <QStringList>

namespace DataLogging {

/****************************************************************************/
EventFilterNetworkServer::EventFilterNetworkServer(QObject *pParent, quint16 Port) :
    QObject(pParent),
    m_pServer(NULL),
    m_pSocket(NULL)
{
    // may throw an exeption, but it does not matter
    m_pServer = new QTcpServer(this);
    // set max pending connections to only one
    m_pServer->setMaxPendingConnections(1);
    // now connect signals of server to our slots
    try {
        CONNECTSIGNALSLOT(m_pServer, newConnection(), this, AcceptNewConnection());
    } catch (...) {
        Global::ToConsole("QObject::connect(m_pServer, SIGNAL(newConnection()), this, SLOT(AcceptNewConnection())) failed");
    }
    // listen
    if(!m_pServer->listen(QHostAddress::Any, Port)) {
        // listen failed.
        Global::ToConsole("m_pServer->listen(QHostAddress::Any, Port) failed: " + m_pServer->errorString());
    }
}

/****************************************************************************/
EventFilterNetworkServer::~EventFilterNetworkServer() {
    try {
        delete m_pSocket;
        m_pSocket = NULL;
        delete m_pServer;
        m_pServer = NULL;
    } catch(...) {
    }
}

/****************************************************************************/
void EventFilterNetworkServer::AcceptNewConnection() {
    Q_ASSERT(m_pSocket == NULL);
    Q_ASSERT(m_pServer != NULL);
    if(m_pServer != NULL) {
        m_pSocket = m_pServer->nextPendingConnection();
        if(m_pSocket == NULL) {
            // something went wrong
            // exit
            return;
        }
        m_pSocket->setTextModeEnabled(true);
        try {
            // connect new connections signals to our slots
            CONNECTSIGNALSLOT(m_pSocket, disconnected(), this, ConnectionDisconnected());
        } catch (...) {
            Global::ToConsole("QObject::connect(m_pSocket, SIGNAL(disconnected()), this, SLOT(ConnectionDisconnected())) failed");
        }
    }
}

/****************************************************************************/
void EventFilterNetworkServer::ConnectionDisconnected() {
    if(m_pSocket == NULL) {
        return;
    }
    // read and interpret complete data
    ReadData();
    // delete when returning to event loop
    m_pSocket->deleteLater();
    // and set pointer to NULL
    m_pSocket = NULL;
}

/****************************************************************************/
void EventFilterNetworkServer::ProcessProtocolVersion_1(QStringList &rLines) const {
    if(rLines.size() < 1) {
        return;
    }
    QString EndLine = rLines.takeLast();
    if(EndLine != "END") {
        // missing end marker
        return;
    }
    for(QStringList::iterator it = rLines.begin(); it != rLines.end(); ++it) {
        QString Line = *it;
        QStringList List = Line.split(';', QString::KeepEmptyParts);
        if(List.size() == 3) {
            QString FileName = List[0];
            int LineStart = List[1].toInt();
            int LineStop = List[2].toInt();
            // add line range to event filter
            EventFilter::Instance().AddLineRange(FileName, LineStart, LineStop);
        }
    }
}

/****************************************************************************/
void EventFilterNetworkServer::ReadData() {
    Q_ASSERT(m_pSocket != NULL);
    if(m_pSocket == NULL) {
        return;
    }
    // read and interpret all pending data
    // first of all clear EventFilter list.
    EventFilter::Instance().Clear();
    // now read one line at a time
    QStringList Lines;
    char buf[1024];
    for(;;) {
        qint64 LineLength = m_pSocket->readLine(buf, sizeof(buf));
        if(-1 == LineLength) {
            // exit loop
            break;
        }
        Lines << QString(buf).trimmed();
    }
    // check data
    if(Lines.size() < 2) {
        // not enaugh data received
        return;
    }
    // check if first line is protocol
    QString ProtocolString = Lines.takeFirst();
    if(!ProtocolString.startsWith("Protocol:")) {
        // not a protocol information
        return;
    }
    // extract protocol version
    int ProtocolVersion = ProtocolString.split(":").value(1).toInt();
    if(ProtocolVersion == 1) {
        ProcessProtocolVersion_1(Lines);
    } else {
        // unknown protocol version
    }
}

} // end namespace DataLogging
