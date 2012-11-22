/****************************************************************************/
/*! \file TestNetworkClient.h
 *
 *  \brief Definition file for class TestNetworkClient.
 *
 *  $Version: $ 0.1
 *  $Date:    $ 10.02.2011
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

#ifndef NETWORKBASE_TESTNETWORKCLIENT_H
#define NETWORKBASE_TESTNETWORKCLIENT_H

#include <NetworkComponents/Include/NetworkClient.h>
#include <NetworkComponents/Include/NetworkClientDevice.h>

namespace NetworkBase {

const QString UT_TEST_IP = "127.0.0.14";  ///< test ip to connect to
const QString UT_TEST_PORT = "7772";      ///< test port to connect to

/****************************************************************************/
/**
 * \brief Test class for NetworkClient class.
 */
/****************************************************************************/
class TestNetworkClient: public QObject
{
  Q_OBJECT

public:

    TestNetworkClient();
    ~TestNetworkClient();

public slots:

    void TestHandleSocketError(QAbstractSocket::SocketError);
    void TestConnectionEstablished(const QString &);
    void TestConnectionLost(const QString &);
    void TestConnectionFailed(NetworkBase::NetworkClientErrorType_t);
    void TestForwardToMessageHandler(const QString &);
    void TestForwardToMessageHandler(quint8, QByteArray &);

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    ///< \warning: Each test case builds on the output of the previous one.
    ///< All tests shall be run exactly in the order as they are listed here:
    void utTestConfigParametersSetting();    ///< Test N1
    void utTestInitializationAction();       ///< Test N2
    void utTestAuthenticationAction();       ///< Test N3
    void utTestWorkFunctions();              ///< Test N4
    void utTestSocketHandling();             ///< Test N5
    void utTestBadInputHandling();           ///< Test N6

private:

    void CheckForNull();
    void SendTestMessage(quint8, const QByteArray &);

private:

    NetworkClient       *m_myClient;        ///< the class to test
    NetworkClientDevice *m_myNetworkDevice; ///< the derived class

    bool m_FlagSocketError;              ///< flag to indicate socket error
    bool m_FlagConnectionEstablished;    ///< flag to indicate established connection
    bool m_FlagConnectionLost;           ///< flag to indicate lost connection
    bool m_FlagConnectionFailed;         ///< flag to indicate failed connection
    bool m_FlagForwardToMH1;             ///< flag to indicate forwarded message
    bool m_FlagForwardToMH2;             ///< flag to indicate forwarded message

    QAbstractSocket::SocketError m_SocketError; ///< variable to hold socker error
    QString  m_String;                          ///< variable to hold client name
    NetworkClientErrorType_t m_ConnectError;    ///< variable to hold connection error

    QTcpSocket   m_LocalServerSocket;          ///< local server socket to emulate net connection
    QTcpServer   m_LocalServer;                ///< local server to emulate net connection
    quintptr     m_ServerSocketDesciptor;      ///< Qt net socket management parameter
};

} // end namespace

#endif // NETWORKBASE_TESTNETWORKCLIENT_H
