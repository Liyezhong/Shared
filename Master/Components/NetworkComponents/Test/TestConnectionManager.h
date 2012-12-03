/****************************************************************************/
/*! \file TestConnectionManager.h
 *
 *  \brief Definition file for class TestConnectionManager.
 *
 *  $Version: $ 0.1
 *  $Date:    $ 12.02.2011
 *  $Author:  $ Y.Novak
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

#ifndef NETWORKBASE_TESTCONNECTIONMANAGER_H
#define NETWORKBASE_TESTCONNECTIONMANAGER_H

#include <NetworkComponents/Include/ConnectionManager.h>
#include <NetworkComponents/Include/NetworkServer.h>

namespace NetworkBase {

/****************************************************************************/
/**
 * \brief Test class for ConnectionManager class.
 */
/****************************************************************************/
class TestConnectionManager: public QObject
{
  Q_OBJECT

public:

    TestConnectionManager();
    ~TestConnectionManager();

public slots:

    void TestDestroyMe(quint32, const QString &, NetworkBase::DisconnectType_t);
    void TestClientConnected(quint32, const QString &);
    void TestMsgReceived(quint8, QByteArray &);

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    /// \warning: Each test case builds on the output of the previous one.
    /// All tests shall be run exactly in the order as they are listed here:
    void utTestAuthenticationFunction();     ///< Test N1
    void utTestConnectDisconnect();          ///< Test N2
    void utTestSocketHandling();             ///< Test N3
    void utTestBadInputHandling();           ///< Test N4

private:

    void CheckForNull();
    void SendTestMessage(quint8, const QByteArray &);

private:

    ConnectionManager *m_myConnectionManager;  ///< the class to test
    QHash<QString, QString > m_myConnections;  ///< list of test connections

    bool m_FlagDestroyMe;                      ///< flag to indicate the class has requested its destruction
    bool m_FlagClientConnected;                ///< flag to indicate that client has connected
    bool m_FlagMsgReceived;                    ///< flag to indicate that message has received

    NetworkBase::DisconnectType_t m_Dt;        ///< test disconnect reason
    QString m_String;                          ///< test name string
    quint32 m_Num;                             ///< test reference number

    QTcpSocket   m_LocalClientSocket;          ///< local client socket to emulate network connections
    QTcpServer   m_LocalServer;                ///< local server to emulate network connections
    quintptr     m_ServerSocketDesciptor;      ///< Qt parameter for socket management
};

} // end namespace

#endif // NETWORKBASE_TESTCONNECTIONMANAGER_H
