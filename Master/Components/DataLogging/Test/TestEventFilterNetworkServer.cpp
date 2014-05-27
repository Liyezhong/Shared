/****************************************************************************/
/*! \file TestEventFilter.cpp
 *
 *  \brief Implementation file for class TestEventFilter.
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
/********************TestEventFilterNetworkServer************/

#include <QTest>
#include <QTcpSocket>
#include <QDebug>
#include <DataLogging/Include/EventFilterNetworkServer.h>
#include <unistd.h>
#include <QTimer>
#include <QSignalSpy>

// Run exec for a maximum of TIMEOUT msecs
#define QCOREAPPLICATION_EXEC(TIMEOUT) \
{ \
    QTimer timer; \
    timer.setSingleShot(true); \
    timer.setInterval(TIMEOUT); \
    timer.start(); \
    connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit())); \
    QCoreApplication::exec(); \
}

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Test class for EventFilter class.
 */
/****************************************************************************/
class TestEventFilterNetworkServer : public QObject {
    Q_OBJECT

private:
     EventFilterNetworkServer  *m_pEventFilterNetworkServer;
     QTcpSocket   *sock;
     QSignalSpy  *spyServer;
public slots:
     void readMsg();

private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     *
     * Here is the only place where we can test the default constructor.
     */
    /****************************************************************************/
    void initTestCase();

    void connectCase();
    /****************************************************************************/
    /**
     * \brief Called after last testfunction was executed.
     */
    /****************************************************************************/
    void cleanupTestCase();
    /****************************************************************************/

}; // end class TestEventFilter

/****************************************************************************/
void TestEventFilterNetworkServer::initTestCase()
{
    m_pEventFilterNetworkServer = new EventFilterNetworkServer(this, 9876);
    QVERIFY(m_pEventFilterNetworkServer != NULL);
    spyServer = new QSignalSpy(m_pEventFilterNetworkServer->m_pServer, SIGNAL(newConnection()));
    QVERIFY(spyServer != NULL);
}

/****************************************************************************/
void TestEventFilterNetworkServer::connectCase()
{
      if (!fork()) {
              delete m_pEventFilterNetworkServer;
              sock = new QTcpSocket(this);
              sock->connectToHost("127.0.0.1", 9876);
              connect(sock, SIGNAL(readyRead()), this, SLOT(readMsg()));
              sock->write("Protocol:1");
              QCOREAPPLICATION_EXEC(500);
              sock->close();
              delete sock;
              exit(0);
      }

      QCOREAPPLICATION_EXEC(2000);
}

/****************************************************************************/
void TestEventFilterNetworkServer::readMsg()
{
      qDebug() << sock->readAll();

}

/****************************************************************************/
void TestEventFilterNetworkServer::cleanupTestCase()
{
      QCOMPARE(spyServer->count(), 1);
      delete m_pEventFilterNetworkServer;
}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestEventFilterNetworkServer)

#include "TestEventFilterNetworkServer.moc"
