/****************************************************************************/
/*! \file TestNetworkClient.cpp
 *
 *  \brief Implementation file for class TestNetworkClient.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 10.02.2011
 *   $Author:  $ Y.Novak
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

#include <QTest>
#include <NetworkComponents/Test/TestNetworkClient.h>
#include <Global/Include/SystemPaths.h>

namespace NetworkBase {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestNetworkClient::TestNetworkClient() :
            m_myClient(NULL),
            m_myNetworkDevice(NULL),
            m_FlagSocketError(false),
            m_FlagConnectionEstablished(false),
            m_FlagConnectionLost(false),
            m_FlagConnectionFailed(false),
            m_FlagForwardToMH1(false),
            m_FlagForwardToMH2(false),
            m_SocketError(QAbstractSocket::UnknownSocketError),
            m_String(""),
            m_ConnectError(NC_ALL_OK)
{
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestNetworkClient::~TestNetworkClient()
{
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkClient::init()
{
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkClient::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkClient::initTestCase()
{
    // create client
    m_myClient = new NetworkClient(NCE_TYPE_COLORADO_GUI, UT_TEST_IP, UT_TEST_PORT, this);
    QCOMPARE(m_myClient->m_myName, (QString)"");
    QCOMPARE(m_myClient->m_myType, (NetworkClientType_t)NCE_TYPE_COLORADO_GUI);
    QCOMPARE(m_myClient->m_msgHdlr, (NetworkDevice*)NULL);
    QCOMPARE(m_myClient->m_myVersion, (QString)"");
    QCOMPARE(m_myClient->m_Ip, UT_TEST_IP);
    QCOMPARE(m_myClient->m_port, UT_TEST_PORT);
    QCOMPARE(m_myClient->m_authStage, (ClientConnectionStateType_t)NC_INIT);

    // initialize client:
    QCOMPARE(m_myClient->Initialize(), true);
    QCOMPARE(m_myClient->m_myName, (QString)"Colorado Device GUI");
    QCOMPARE(m_myClient->m_myVersion, (QString)"1.0");
    QCOMPARE(m_myClient->m_Ip, UT_TEST_IP);
    QCOMPARE(m_myClient->m_port, UT_TEST_PORT);

    // connect test slots:
    if (!QObject::connect(m_myClient, SIGNAL(ConnectionEstablished(const QString &)),
                          this, SLOT(TestConnectionEstablished(const QString &)))) {
        QFAIL("TestNetworkClient: cannot connect ConnectionEstablished signal !");
    }
    if (!QObject::connect(m_myClient, SIGNAL(ConnectionLost(const QString &)),
                          this, SLOT(TestConnectionLost(const QString &)))) {
        QFAIL("TestNetworkClient: cannot connect ConnectionLost signal !");
    }
    if (!QObject::connect(m_myClient, SIGNAL(ConnectionFailed(NetworkBase::NetworkClientErrorType_t)),
                          this, SLOT(TestConnectionFailed(NetworkBase::NetworkClientErrorType_t)))) {
        QFAIL("TestNetworkClient: cannot connect ConnectionFailed signal !");
    }
    if (!QObject::connect(m_myClient, SIGNAL(ForwardToMessageHandler(const QString &)),
                          this, SLOT(TestForwardToMessageHandler(const QString &)))) {
        QFAIL("TestNetworkClient: cannot connect ForwardToMessageHandler signal !");
    }
    if (!QObject::connect(m_myClient, SIGNAL(ForwardToMessageHandler(quint8, QByteArray &)),
                          this, SLOT(TestForwardToMessageHandler(quint8, QByteArray &)))) {
        QFAIL("TestNetworkClient: cannot connect ForwardToMessageHandler signal !");
    }
    if (!QObject::connect(&(m_myClient->m_tcpSocket), SIGNAL(error(QAbstractSocket::SocketError)),
                    this, SLOT(TestHandleSocketError(QAbstractSocket::SocketError)))) {
        QFAIL("TestNetworkClient: cannot connect error signal !");
    }

    // create message handler:
    //QString path =  Global::SystemPaths::Instance().GetSettingsPath() + "/Communication/gui";
    QString path = "SomeDummyPath"; // (schould) not (be) relevant for Client
    m_myNetworkDevice = new NetworkClientDevice(NCE_TYPE_COLORADO_GUI, UT_TEST_IP, UT_TEST_PORT, path, this);
    QCOMPARE(m_myNetworkDevice->InitializeDevice(), true);

    // test message handler registration:
    QCOMPARE(m_myClient->RegisterMessageHandler(m_myNetworkDevice), true);
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkClient::cleanupTestCase()
{
    // cleanup:
    delete m_myClient;
    delete m_myNetworkDevice;
}

/****************************************************************************/
/**
 * \brief Internal check for NULL pointer.
 */
/****************************************************************************/
void TestNetworkClient::CheckForNull()
{
    if ((m_myClient == NULL) ||
        (m_myNetworkDevice == NULL)) {
        QFAIL("TestNetworkClient: one of the internal pointers is NULL !");
    }
}

/****************************************************************************/
/**
 * \brief Test client operation.
 */
/****************************************************************************/
void TestNetworkClient::utTestConfigParametersSetting()
{
    CheckForNull();

    m_myClient->m_myName = (QString)"";
    m_myClient->m_myVersion = (QString)"";

    m_myClient->m_myType = (NetworkClientType_t)NCE_TYPE_UNKNOWN;
    m_myClient->m_Ip = (QString)"";
    m_myClient->m_port = (QString)"";
    QCOMPARE(m_myClient->SetConfigParameters(), (NetworkClientType_t)NCE_TYPE_INVALID);
    QCOMPARE(m_myClient->m_myName, (QString)"");
    QCOMPARE(m_myClient->m_myVersion, (QString)"");
    QCOMPARE(m_myClient->m_Ip, (QString)"");
    QCOMPARE(m_myClient->m_port, (QString)"");

    m_myClient->m_myType = (NetworkClientType_t)NCE_TYPE_INVALID;
    m_myClient->m_Ip = (QString)"";
    m_myClient->m_port = (QString)"";
    QCOMPARE(m_myClient->SetConfigParameters(), (NetworkClientType_t)NCE_TYPE_INVALID);
    QCOMPARE(m_myClient->m_myName, (QString)"");
    QCOMPARE(m_myClient->m_myVersion, (QString)"");
    QCOMPARE(m_myClient->m_Ip, (QString)"");
    QCOMPARE(m_myClient->m_port, (QString)"");

    m_myClient->m_myType = (NetworkClientType_t)NCE_TYPE_AXEDA;
    m_myClient->m_Ip = (QString)"";
    m_myClient->m_port = (QString)"";
    QCOMPARE(m_myClient->SetConfigParameters(), (NetworkClientType_t)NCE_TYPE_AXEDA);
    QCOMPARE(m_myClient->m_myName, (QString)"Axeda Client");
    QCOMPARE(m_myClient->m_myVersion, (QString)"1.0");
    QCOMPARE(m_myClient->m_Ip, (QString)"127.0.0.15");
    QCOMPARE(m_myClient->m_port, (QString)"8801");

    m_myClient->m_myType = (NetworkClientType_t)NCE_TYPE_SEPIA;
    m_myClient->m_Ip = (QString)"";
    m_myClient->m_port = (QString)"";
    QCOMPARE(m_myClient->SetConfigParameters(), (NetworkClientType_t)NCE_TYPE_SEPIA);
    QCOMPARE(m_myClient->m_myName, (QString)"Sepia Client");
    QCOMPARE(m_myClient->m_myVersion, (QString)"1.0");
    QCOMPARE(m_myClient->m_Ip, (QString)"127.0.0.20");
    QCOMPARE(m_myClient->m_port, (QString)"8801");


    // set working parameters back again:
    m_myClient->m_myType = (NetworkClientType_t)NCE_TYPE_COLORADO_GUI;
    m_myClient->m_Ip = UT_TEST_IP;
    m_myClient->m_port = UT_TEST_PORT;
    m_myClient->m_authStage = (ClientConnectionStateType_t)NC_INIT;
    m_myClient->m_myName = (QString)"Colorado Device GUI";
    m_myClient->m_myVersion = (QString)"1.0";
}

/****************************************************************************/
/**
 * \brief Test client operation.
 */
/****************************************************************************/
void TestNetworkClient::utTestInitializationAction()
{
    // check if wrong server message during authentication can be reported:
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    QString msg = "<msg>SomeWrongContents</msg>";
    if (!domD.setContent(msg, true, &err, &errL, &errC)) {
        QFAIL("TestNetworkClient: cannot set QDomDoc contents !");
        return;
    }
    QByteArray ba = domD.toByteArray(-1);
    if (ba.isEmpty()) {
        QFAIL("TestNetworkClient: cannot convert QDomDoc to QByteArray !");
        return;
    }
    m_myClient->HandleInitAction(ba);
    QCOMPARE(m_FlagConnectionFailed, true);
    QCOMPARE(m_ConnectError, (NetworkClientErrorType_t)NC_WRONG_SERVER_MESSAGE);
    m_FlagConnectionFailed = false;
    m_ConnectError = (NetworkClientErrorType_t)NC_ALL_OK;

    // check action for correct server message:
    msg = CMH_AUTHENTICATION_REQ;
    //CMH_AUTHENTICATION_CONF
    if (!domD.setContent(msg, true, &err, &errL, &errC)) {
        QFAIL("TestNetworkClient: cannot set QDomDoc contents !");
        return;
    }
    ba = domD.toByteArray(-1);
    if (ba.isEmpty()) {
        QFAIL("TestNetworkClient: cannot convert QDomDoc to QByteArray !");
        return;
    }
    m_myClient->HandleInitAction(ba);
    QCOMPARE(m_myClient->m_authStage, NC_NOT_AUTHENTICATED);
    QVERIFY(m_myClient->m_timer.isActive() == true);
}

/****************************************************************************/
/**
 * \brief Test client operation.
 */
/****************************************************************************/
void TestNetworkClient::utTestAuthenticationAction()
{
    // check if wrong server message during authentication can be reported:
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    QString msg = "<msg>SomeWrongContents</msg>";
    if (!domD.setContent(msg, true, &err, &errL, &errC)) {
        QFAIL("TestNetworkClient: cannot set QDomDoc contents !");
        return;
    }
    QByteArray ba = domD.toByteArray(-1);
    if (ba.isEmpty()) {
        QFAIL("TestNetworkClient: cannot convert QDomDoc to QByteArray !");
        return;
    }
    m_myClient->HandleAuthAction(ba);
    QCOMPARE(m_FlagConnectionFailed, true);
    QCOMPARE(m_ConnectError, (NetworkClientErrorType_t)NC_WRONG_SERVER_MESSAGE);
    m_FlagConnectionFailed = false;
    m_ConnectError = (NetworkClientErrorType_t)NC_ALL_OK;

    // check action for correct server message:
    msg = CMH_AUTHENTICATION_CONF;
    if (!domD.setContent(msg, true, &err, &errL, &errC)) {
        QFAIL("TestNetworkClient: cannot set QDomDoc contents !");
        return;
    }
    ba = domD.toByteArray(-1);
    if (ba.isEmpty()) {
        QFAIL("TestNetworkClient: cannot convert QDomDoc to QByteArray !");
        return;
    }
    m_myClient->HandleAuthAction(ba);
    // check parameters change:
    QCOMPARE(m_myClient->m_authStage, NC_AUTHENTICATED);
    QVERIFY(m_myClient->m_timer.isActive() == false);
    QCOMPARE(m_FlagConnectionEstablished, true);
    QCOMPARE(m_String, m_myClient->m_myName);
    // reset flags:
    m_FlagConnectionEstablished = false;
    m_String = (QString)"";

    // check that connection failes when MessageHandler==NULL
    m_myClient->m_msgHdlr = NULL;
    m_myClient->HandleAuthAction(ba);
    QCOMPARE(m_myClient->m_authStage, NC_INIT);
    QCOMPARE(m_FlagConnectionFailed, true);
    QCOMPARE(m_ConnectError, (NetworkClientErrorType_t)NC_MSGHANDLER_POINTER_NULL);
    m_FlagConnectionFailed = false;
    m_ConnectError = (NetworkClientErrorType_t)NC_ALL_OK;

    // assign correct MessageHandler pointer before running next test:
    m_myClient->m_msgHdlr = m_myNetworkDevice;
}

/****************************************************************************/
/**
 * \brief Test client operation.
 */
/****************************************************************************/
void TestNetworkClient::utTestWorkFunctions()
{
    CheckForNull();

    // try to connect to server:
    m_myClient->ConnectToServer();
    // now evaluate the timeout:
    //qDebug() << "\n ======== TestNetworkClient: waiting for socket error... ======== \n";
    QTest::qWait(1000);
    // check that connection failed and socket error signal was emitted:
    QCOMPARE(m_FlagSocketError, true);
    // check that client will try to ping server again:
    QVERIFY(((m_SocketError == QAbstractSocket::ConnectionRefusedError) ||
            (m_SocketError == QAbstractSocket::SocketTimeoutError)) && (m_myClient->m_authStage == NC_INIT));
    // if this test has passed, the client will try to ping the server again, which is correct.
    // reset flags:
    m_FlagSocketError = false;
    m_SocketError = QAbstractSocket::UnknownSocketError;

    // check if connection failure can be reported:
    m_myClient->EmitConnectionFailed((NetworkClientErrorType_t)NC_WRONG_SERVER_MESSAGE);
    QCOMPARE(m_FlagConnectionFailed, true);
    QCOMPARE(m_ConnectError, (NetworkClientErrorType_t)NC_WRONG_SERVER_MESSAGE);
    m_FlagConnectionFailed = false;
    m_ConnectError = (NetworkClientErrorType_t)NC_ALL_OK;

    // check if authentication timeout can be reported:
    m_myClient->m_authStage = (ClientConnectionStateType_t)NC_AUTHENTICATED;
    m_myClient->HandleAuthTimeout();
    QCOMPARE(m_myClient->m_authStage, (ClientConnectionStateType_t)NC_INIT);
    QCOMPARE(m_FlagConnectionFailed, true);
    QCOMPARE(m_ConnectError, (NetworkClientErrorType_t)NC_AUTHENTICATION_FAILED);
    m_FlagConnectionFailed = false;
    m_ConnectError = (NetworkClientErrorType_t)NC_ALL_OK;

    // run this function to make sure it does not blow anything up:
    ///< \todo this function shall probably return a value to upper layers
    QString msg = "<tag>TotallyWrongXMLFormat</tag>";

    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    if (!domD.setContent(msg, true, &err, &errL, &errC)) {
        QFAIL("TestNetworkClient: cannot construct QDomDoc !");
    }
    QByteArray ba = domD.toByteArray(-1);
    if (ba.isEmpty()) {
        QFAIL("TestNetworkClient: cannot convert QDomDoc to QByteArray !");
    }
    m_myClient->SendMessage(NetworkBase::NET_NETLAYER_MESSAGE, ba);

    msg = "<msg><cmd name=\"Test\" ref=\"4\" /><dataitems test=\"2\" /></msg>";
    if (!domD.setContent(msg, true, &err, &errL, &errC)) {
        QFAIL("TestNetworkClient: cannot construct QDomDoc !");
    }
    ba = domD.toByteArray(-1);
    if (ba.isEmpty()) {
        QFAIL("TestNetworkClient: cannot convert QDomDoc to QByteArray !");
    }
    m_myClient->SendMessage(NetworkBase::NET_NETLAYER_MESSAGE, ba);
}

/****************************************************************************/
/**
 * \brief Test client operation.
 */
/****************************************************************************/
void TestNetworkClient::utTestSocketHandling()
{
    // start the test server:
    bool ok = true;
    if (!m_LocalServer.listen(QHostAddress(UT_TEST_IP), (quint16)UT_TEST_PORT.toUInt(&ok, 10))) {
        QFAIL("ConnectionManager: m_LocalServer -> listen failed ! ");
        return;
    }
    QCOMPARE(m_LocalServer.hasPendingConnections(), false);
    // try to connect to server:
    m_myClient->ConnectToServer();
    // give server some time to react:
    QTest::qWait(200);
    // check local socket is connected:
    QCOMPARE(m_myClient->m_tcpSocket.state(), QTcpSocket::ConnectedState);
    // give server some time to react (otherwise hasPendingConnections returns "false"):
    QTest::qWait(200);
    // get socket descriptor for server:
    QCOMPARE(m_LocalServer.hasPendingConnections(), true);
    QTcpSocket *tempSock = m_LocalServer.nextPendingConnection();
    m_ServerSocketDesciptor = tempSock->socketDescriptor();
    // configure TX socket:
    m_LocalServerSocket.setSocketDescriptor(m_ServerSocketDesciptor);

    // create test authentication message:
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    QString msg = CMH_AUTHENTICATION_REQ;
    if (!domD.setContent(msg, true, &err, &errL, &errC)) {
        QFAIL("TestNetworkClient: cannot set QDomDoc contents !");
        return;
    }
    QByteArray ba = domD.toByteArray(-1);
    if (ba.isEmpty()) {
        QFAIL("TestNetworkClient: cannot convert QDomDoc to QByteArray !");
        return;
    }
    // send message:
    SendTestMessage((quint8)0xFF, ba);
    // wait for data:
    QCOMPARE(m_myClient->m_tcpSocket.waitForReadyRead(500), true);
    // check on incoming socket:
    m_myClient->ReadRawSocket();
    QCOMPARE(m_myClient->m_authStage, NC_NOT_AUTHENTICATED);
    QVERIFY(m_myClient->m_timer.isActive() == true);

    // test action after server confirmation:
    msg = CMH_AUTHENTICATION_CONF;
    if (!domD.setContent(msg, true, &err, &errL, &errC)) {
        QFAIL("TestNetworkClient: cannot set QDomDoc contents !");
        return;
    }
    ba = domD.toByteArray(-1);
    if (ba.isEmpty()) {
        QFAIL("TestNetworkClient: cannot convert QDomDoc to QByteArray !");
        return;
    }
    // send message:
    SendTestMessage((quint8)0xFF, ba);
    // wait for data:
    QCOMPARE(m_myClient->m_tcpSocket.waitForReadyRead(500), true);
    // check on incoming socket:
    m_myClient->ReadRawSocket();
    // check parameters change:
    QCOMPARE(m_myClient->m_authStage, NC_AUTHENTICATED);
    QVERIFY(m_myClient->m_timer.isActive() == false);
    QCOMPARE(m_FlagConnectionEstablished, true);
    QCOMPARE(m_String, m_myClient->m_myName);
    // reset flags:
    m_FlagConnectionEstablished = false;
    m_String = (QString)"";

    // disconnect socket:
    m_myClient->DisconnectFromServer();
    // check that socket is down:
    QVERIFY(m_myClient->m_tcpSocket.state() != QTcpSocket::ConnectedState);
    // close local server socket:
    m_LocalServerSocket.close();
}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestNetworkClient::utTestBadInputHandling()
{
    // do some tests before killing the client:
    // set dummy authentication and try to trigger socker error:
    m_myClient->m_authStage = (ClientConnectionStateType_t)NC_AUTHENTICATED;
    m_myClient->HandleSocketError(QAbstractSocket::NetworkError);
    // give process some time:
//    QTest::qWait(500);
    // check that error signal was sent:
    QCOMPARE(m_FlagConnectionLost, true);
    QCOMPARE(m_String, (QString)"Colorado Device GUI");
    // reset flags:
    m_FlagConnectionLost = false;
    m_String = "";

    // delete client:
    delete m_myClient;

    // create new client with dummy parameters:
    m_myClient = new NetworkClient(NCE_TYPE_UNKNOWN, (QString)"", (QString)"7771", this);
    QCOMPARE(m_myClient->m_myName, (QString)"");
    QCOMPARE(m_myClient->m_myType, (NetworkClientType_t)NCE_TYPE_UNKNOWN);
    QCOMPARE(m_myClient->m_msgHdlr, (NetworkDevice*)NULL);
    QCOMPARE(m_myClient->m_myVersion, (QString)"");
    QCOMPARE(m_myClient->m_Ip, (QString)"");
    QCOMPARE(m_myClient->m_port, (QString)"7771");
    QCOMPARE(m_myClient->m_authStage, (ClientConnectionStateType_t)NC_INIT);

    // initialize client:
    QCOMPARE(m_myClient->Initialize(), false);
    QCOMPARE(m_myClient->m_myName, (QString)"");
    QCOMPARE(m_myClient->m_myVersion, (QString)"");

    // try to register a NULL MessageHandler:
    QCOMPARE(m_myClient->RegisterMessageHandler(NULL), false);
    // make sure this function does not blow anything up:
    m_myClient->m_authStage = (ClientConnectionStateType_t)NC_AUTHENTICATED;
    m_myClient->DisconnectFromServer();
    QCOMPARE(m_myClient->m_authStage, (ClientConnectionStateType_t)NC_INIT);
}

/****************************************************************************/
/**
 *  \brief Test slot for socket errors.
 *
 *  \param    err = socket error reported by Qt class
 */
/****************************************************************************/
void TestNetworkClient::TestHandleSocketError(QAbstractSocket::SocketError err)
{
    m_FlagSocketError = true;
    m_SocketError = err;
}

/****************************************************************************/
/**
 *  \brief Test slot for client connection.
 *
 *  \param    str = client name
 */
/****************************************************************************/
void TestNetworkClient::TestConnectionEstablished(const QString &str)
{
    m_FlagConnectionEstablished = true;
    m_String = str;
}

/****************************************************************************/
/**
 *  \brief Test slot for client disconnection.
 *
 *  \param    str = client name
 */
/****************************************************************************/
void TestNetworkClient::TestConnectionLost(const QString &str)
{
    m_FlagConnectionLost = true;
    m_String = str;
}

/****************************************************************************/
/**
 *  \brief Test slot for connection failure.
 *
 *  \param    err = error description
 */
/****************************************************************************/
void TestNetworkClient::TestConnectionFailed(NetworkBase::NetworkClientErrorType_t err)
{
    m_FlagConnectionFailed = true;
    m_ConnectError = err;
}

/****************************************************************************/
/**
 *  \brief Test slot for message forwarding.
 *
 *  \param    str = outgoing message
 */
/****************************************************************************/
void TestNetworkClient::TestForwardToMessageHandler(const QString &str)
{
    m_FlagForwardToMH1 = true;
    m_String = str;
}

/****************************************************************************/
/**
 *  \brief Test slot for message forwarding.
 *
 *  \param    type = marker byte identifies a type of message
 *  \param    ba = outgoing byte array
 */
/****************************************************************************/
void TestNetworkClient::TestForwardToMessageHandler(quint8 type, QByteArray &ba)
{
    Q_UNUSED(type)
    Q_UNUSED(ba)
    m_FlagForwardToMH2 = true;
}

/****************************************************************************/
/*!
 *  \brief    This function sends a message to Tested class.
 *
 *  \param    mbyte = marker byte identifies a type of message
 *  \param    ba = outgoing byte array
 *
 ****************************************************************************/
void TestNetworkClient::SendTestMessage(quint8 mbyte, const QByteArray &ba)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    qint32 bsize = ba.size();
    out << bsize;
    // set the Marker Byte:
    out << mbyte;

    int nofbytes = out.writeRawData(ba.data(), bsize);

    // check if data was actually written: bsize + markerbyte + packet size
    if ((m_LocalServerSocket.write(block) < (bsize + 1 + sizeof(qint32))) || nofbytes != bsize) {
        QFAIL("TestNetworkClient: writing outgoing data to Socket failed !");
        return;
    }
    // send data immediately:
    if (!m_LocalServerSocket.flush()) {
        QFAIL("TestNetworkClient: flushing outgoing Socket failed !");
    }
}

} // end namespace

QTEST_MAIN(NetworkBase::TestNetworkClient)
