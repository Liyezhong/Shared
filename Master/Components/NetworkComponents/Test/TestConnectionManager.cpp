/****************************************************************************/
/*! \file TestConnectionManager.cpp
 *
 *  \brief Implementation file for class TestConnectionManager.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 12.02.2011
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
#include <NetworkComponents/Test/TestConnectionManager.h>
#include <Global/Include/SystemPaths.h>

namespace NetworkBase {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestConnectionManager::TestConnectionManager() :
            m_myConnectionManager(NULL),
            m_FlagDestroyMe(false),
            m_FlagClientConnected(false),
            m_FlagMsgReceived(false),
            m_Dt((DisconnectType_t)UNDEFINED_NO_INIT),
            m_String(""),
            m_Num(0),
            m_ServerSocketDesciptor(0)
{
    m_myConnections.empty();
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestConnectionManager::~TestConnectionManager()
{
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestConnectionManager::init()
{
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestConnectionManager::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestConnectionManager::initTestCase()
{
    // initialize settings path:
    Global::SystemPaths::Instance().SetSettingsPath("../../../../../../Himalaya/Master/Components/Main/Build/Settings");

    // make a list of some available connections (Name + Version):
    m_myConnections.insert((QString)"Client-1", (QString)"1.0");
    m_myConnections.insert((QString)"Client-2", (QString)"2.0");
    m_myConnections.insert((QString)"Client-3", (QString)"3.0");
    QString reply = "<reply><cmd>Authentication</cmd><name></name><version></version></reply>";
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    if (!domD.setContent(reply, true, &err, &errL, &errC)) {
        QFAIL("TestConnectionManager: cannot convert reply section to QDomDocument !");
        return;
    }
    QDomElement domEl = domD.documentElement();
    // create connection manager:
    m_myConnectionManager = new ConnectionManager(m_myConnections, (quint32)7, (QString)"Authenticate", (QString)"Welcome", domEl, this);
    QCOMPARE(m_myConnectionManager->m_authConfirm, (QString)"Welcome");
    QCOMPARE(m_myConnectionManager->m_authRequest, (QString)"Authenticate");
    QCOMPARE(m_myConnectionManager->m_connectionState, CM_NOT_AUTHENTICATED);
    QCOMPARE(m_myConnectionManager->GetClientName(), (QString)"");
    QCOMPARE(m_myConnectionManager->GetRefNumber(), (quint32)7);
    QCOMPARE(m_myConnectionManager->m_authenticationStringsList.size(), (int)3);

    // connect test slots:
    if (!QObject::connect(m_myConnectionManager, SIGNAL(DestroyMe(quint32, const QString &, NetworkBase::DisconnectType_t)),
                          this, SLOT(TestDestroyMe(quint32, const QString &, NetworkBase::DisconnectType_t)))) {
        QFAIL("TestConnectionManager: cannot connect DestroyMe signal !");
    }
    if (!QObject::connect(m_myConnectionManager, SIGNAL(ClientConnected(quint32, const QString &)),
                          this, SLOT(TestClientConnected(quint32, const QString &)))) {
        QFAIL("TestConnectionManager: cannot connect ClientConnected signal !");
    }
    if (!QObject::connect(m_myConnectionManager, SIGNAL(MsgReceived(quint8, QByteArray &)),
                          this, SLOT(TestMsgReceived(quint8, QByteArray &)))) {
        QFAIL("TestConnectionManager: cannot connect MsgReceived signal !");
    }

    // start the test server:
    if (!m_LocalServer.listen(QHostAddress("127.0.0.7"), (quint16)7774)) {
        QFAIL("ConnectionManager: m_LocalServer -> listen failed ! ");
        return;
    }
    QCOMPARE(m_LocalServer.hasPendingConnections(), false);
    // connect local socket:
    m_LocalClientSocket.connectToHost(QHostAddress("127.0.0.7"), (quint16)7774, QIODevice::ReadWrite);
    // give server some time to react:
    QTest::qWait(200);
    // check local socket is connected:
    QCOMPARE(m_LocalClientSocket.state(), QTcpSocket::ConnectedState);
    // give server some time to react (otherwise hasPendingConnections returns "false"):
    QTest::qWait(200);
    // get socket descriptor for server:
    QCOMPARE(m_LocalServer.hasPendingConnections(), true);
    QTcpSocket *tempSock = m_LocalServer.nextPendingConnection();
    m_ServerSocketDesciptor = tempSock->socketDescriptor();
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestConnectionManager::cleanupTestCase()
{
    // cleanup:
    delete m_myConnectionManager;
}

/****************************************************************************/
/**
 * \brief Internal check for NULL pointer.
 */
/****************************************************************************/
void TestConnectionManager::CheckForNull()
{
    if (m_myConnectionManager == NULL) {
        QFAIL("TestConnectionManager: one of the internal pointers is NULL !");
    }
}

/****************************************************************************/
/**
 * \brief Test server operation.
 */
/****************************************************************************/
void TestConnectionManager::utTestAuthenticationFunction()
{
    CheckForNull();

    // construct correct authentication message:
    QString authMsg = "<reply><cmd>Authentication</cmd><name>Client-2</name><version>2.0</version></reply>";
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    if (!domD.setContent(authMsg, true, &err, &errL, &errC)) {
        QFAIL("TestConnectionManager: cannot convert QString message to QDomDocument !");
        return;
    }
    QByteArray bArr = domD.toByteArray();
    // check that we are not authenticated:
    QCOMPARE(m_myConnectionManager->m_connectionState, CM_NOT_AUTHENTICATED);
    // check authentication function:
    QCOMPARE(m_myConnectionManager->CheckAuthenticationResponse(&bArr), true);
    // check state change:
    QCOMPARE(m_myConnectionManager->m_connectionState, CM_AUTHENTICATED);
    // check that connection signal was emitted:
    QCOMPARE(m_FlagClientConnected, true);
    QCOMPARE(m_Num, m_myConnectionManager->m_myNumber);
    QCOMPARE(m_String, m_myConnectionManager->m_myClient);
    m_FlagClientConnected = false;
    m_Num = 0;
    m_String = "";
    m_myConnectionManager->DisconnectMe();
    // check that state has changed:
    QCOMPARE(m_myConnectionManager->m_connectionState, CM_NOT_AUTHENTICATED);

    // construct wrong authentication message N1:
    authMsg = "<reply><cmd>WrongText</cmd><name>Client-2</name><version>2.0</version></reply>";
    if (!domD.setContent(authMsg, true, &err, &errL, &errC)) {
        QFAIL("TestConnectionManager: cannot convert QString message to QDomDocument !");
        return;
    }
    bArr = domD.toByteArray();
    // check authentication function:
    QCOMPARE(m_myConnectionManager->CheckAuthenticationResponse(&bArr), false);

    // construct wrong authentication message N2:
    authMsg = "<reply><cmd>Authentication</cmd><name>SomeWrong Device GUI</name><version>2.0</version></reply>";
    if (!domD.setContent(authMsg, true, &err, &errL, &errC)) {
        QFAIL("TestConnectionManager: cannot convert QString message to QDomDocument !");
        return;
    }
    bArr = domD.toByteArray();
    // check authentication function:
    QCOMPARE(m_myConnectionManager->CheckAuthenticationResponse(&bArr), false);

    // construct wrong authentication message N3:
    authMsg = "<reply><cmd>Authentication</cmd><name>Client-2</name><version>5.2</version></reply>";
    if (!domD.setContent(authMsg, true, &err, &errL, &errC)) {
        QFAIL("TestConnectionManager: cannot convert QString message to QDomDocument !");
        return;
    }
    bArr = domD.toByteArray();
    // check authentication function:
    QCOMPARE(m_myConnectionManager->CheckAuthenticationResponse(&bArr), false);

    // construct wrong authentication message N4:
    authMsg = "<reply><cmd>Authentication</cmd><version>2.0</version></reply>";
    if (!domD.setContent(authMsg, true, &err, &errL, &errC)) {
        QFAIL("TestConnectionManager: cannot convert QString message to QDomDocument !");
        return;
    }
    bArr = domD.toByteArray();
    // check authentication function:
    QCOMPARE(m_myConnectionManager->CheckAuthenticationResponse(&bArr), false);

    // construct wrong authentication message N5:
    authMsg = "<reply><name>Client-2</name><version>2.0</version></reply>";
    if (!domD.setContent(authMsg, true, &err, &errL, &errC)) {
        QFAIL("TestConnectionManager: cannot convert QString message to QDomDocument !");
        return;
    }
    bArr = domD.toByteArray();
    // check authentication function:
    QCOMPARE(m_myConnectionManager->CheckAuthenticationResponse(&bArr), false);

    // construct wrong authentication message N6:
    authMsg = "<reply><cmd>Authentication</cmd><name>Client-2</name></reply>";
    if (!domD.setContent(authMsg, true, &err, &errL, &errC)) {
        QFAIL("TestConnectionManager: cannot convert QString message to QDomDocument !");
        return;
    }
    bArr = domD.toByteArray();
    // check authentication function:
    QCOMPARE(m_myConnectionManager->CheckAuthenticationResponse(&bArr), false);
}

/****************************************************************************/
/**
 * \brief Test server operation.
 */
/****************************************************************************/
void TestConnectionManager::utTestConnectDisconnect()
{
    CheckForNull();

    // emulate socket error:
    m_myConnectionManager->HandleSocketDisconnect();
    // check that we got the signal:
    QCOMPARE(m_FlagDestroyMe, true);
    QCOMPARE(m_Num, m_myConnectionManager->m_myNumber);
    QCOMPARE(m_String, m_myConnectionManager->m_myClient);
    QCOMPARE(m_Dt, SOCKET_DISCONNECT);
    m_FlagDestroyMe = false;
    m_Num = 0;
    m_String = "";
    m_Dt = UNDEFINED_NO_INIT;

    // emulate authentication timeout:
    m_myConnectionManager->HandleTimeout();
    // check that we got the signal:
    QCOMPARE(m_FlagDestroyMe, true);
    QCOMPARE(m_Num, m_myConnectionManager->m_myNumber);
    QCOMPARE(m_String, m_myConnectionManager->m_myClient);
    QCOMPARE(m_Dt, AUTHENTICATION_TIMEOUT);
    m_FlagDestroyMe = false;
    m_Num = 0;
    m_String = "";
    m_Dt = UNDEFINED_NO_INIT;
}

/****************************************************************************/
/**
 * \brief Test server operation.
 */
/****************************************************************************/
void TestConnectionManager::utTestSocketHandling()
{
    CheckForNull();

    // check local socket is connected:
    QVERIFY(m_LocalClientSocket.state() == QTcpSocket::ConnectedState);
    QVERIFY(m_ServerSocketDesciptor != 0);
    // timer shall not yet run:
    QCOMPARE(m_myConnectionManager->m_timer.isActive(), false);
    // check that server socket descriptor:
    QCOMPARE(m_myConnectionManager->m_TcpSocket.socketDescriptor(), -1);
    // emulate incoming connection:
    m_myConnectionManager->HandleNewConnection(m_ServerSocketDesciptor);
    // server socket shall be initialized:
    QVERIFY(m_myConnectionManager->m_TcpSocket.socketDescriptor() != -1);
    // check that timeout timer has been started:
    QCOMPARE(m_myConnectionManager->m_timer.isActive(), true);

    // "send" valid authentication reply:
    QString authMsg = "<reply><cmd>Authentication</cmd><name>Client-3</name><version>3.0</version></reply>";
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    if (!domD.setContent(authMsg, true, &err, &errL, &errC)) {
        QFAIL("TestConnectionManager: cannot convert QString message to QDomDocument !");
        return;
    }
    QByteArray bArr = domD.toByteArray();
    // check that we are not authenticated:
    QCOMPARE(m_myConnectionManager->m_connectionState, CM_NOT_AUTHENTICATED);
    // "send the message":
    SendTestMessage((quint8)0xFF, bArr);
    // wait for data:
    QCOMPARE(m_myConnectionManager->m_TcpSocket.waitForReadyRead(500), true);
    // check on incoming socket:
    m_myConnectionManager->ReadSocket();
    // check state change:
    QCOMPARE(m_myConnectionManager->m_connectionState, CM_AUTHENTICATED);
    // timer shall be stopped:
    QVERIFY(m_myConnectionManager->m_timer.isActive() == false);
    // check that connection signal was emitted:
    QCOMPARE(m_FlagClientConnected, true);
    QCOMPARE(m_Num, m_myConnectionManager->m_myNumber);
    QCOMPARE(m_String, m_myConnectionManager->m_myClient);
    m_FlagClientConnected = false;
    m_Num = 0;
    m_String = "";

    // "send the message" again. check that MsgReceived signal is emitted:
    QCOMPARE(m_FlagMsgReceived, false);
    SendTestMessage((quint8)0xFF, bArr);
    // wait for data:
    QCOMPARE(m_myConnectionManager->m_TcpSocket.waitForReadyRead(500), true);
    // check on incoming socket:
    m_myConnectionManager->ReadSocket();
    // check that we got the message:
    QCOMPARE(m_FlagMsgReceived, true);
    QCOMPARE(m_Num, (quint32)0xFF);
    m_FlagMsgReceived = false;
    m_Num = 0;

    // kill connection:
    m_myConnectionManager->DisconnectMe();
    // check socket status:
    QVERIFY(m_myConnectionManager->m_TcpSocket.state() != QTcpSocket::ConnectedState);
    // close local socket:
    m_LocalClientSocket.close();
}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestConnectionManager::utTestBadInputHandling()
{
    // kill the good object:
    delete m_myConnectionManager;

    // create dummy object:
    QString reply = "<reply></reply>";
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    if (!domD.setContent(reply, true, &err, &errL, &errC)) {
        QFAIL("TestConnectionManager: cannot convert reply section to QDomDocument !");
        return;
    }
    QDomElement domEl = domD.documentElement();
    // create connection manager:
    m_myConnectionManager = new ConnectionManager(m_myConnections, (quint32)2, (QString)"Dummy1", (QString)"Dummy2", domEl, this);

    // check that "send" functions do not blow anything up:
    QByteArray bArr = domD.toByteArray();
    m_myConnectionManager->ForwardMessage((quint8)0x22, bArr);
    m_myConnectionManager->ForwardMessage(reply);

    // try to init the socket with some dummy descriptor:
    m_myConnectionManager->HandleNewConnection((int)34);
    // timer shall not be started:
    QVERIFY(m_myConnectionManager->m_timer.isActive() == false);
}


/****************************************************************************/
/**
 * \brief Test slot for self-destruction.
 *
 *  \param    Num = test reference number
 *  \param    Str = client name
 *  \param    Dt = disconnect reason
 */
/****************************************************************************/
void TestConnectionManager::TestDestroyMe(quint32 Num, const QString &Str, NetworkBase::DisconnectType_t Dt)
{
    m_FlagDestroyMe = true;
    m_Num = Num;
    m_String = Str;
    m_Dt = Dt;
}

/****************************************************************************/
/**
 * \brief Test slot for client connection.
 *
 *  \param    Num = test reference number
 *  \param    Str = client name
 */
/****************************************************************************/
void TestConnectionManager::TestClientConnected(quint32 Num, const QString &Str)
{
    m_FlagClientConnected = true;
    m_Num = Num;
    m_String = Str;
}

/****************************************************************************/
/**
 * \brief Test slot for message reception.
 *
 *  \param    Num = test reference number
 *  \param    ba = payload
 */
/****************************************************************************/
void TestConnectionManager::TestMsgReceived(quint8 Num, QByteArray &ba)
{
    Q_UNUSED(ba)
    m_FlagMsgReceived = true;
    m_Num = (quint32)Num;
}

/****************************************************************************/
/*!
 *  \brief    This function sends a message to Tested class.
 *
 *  \param    mbyte = marker byte identifies a type of message
 *  \param    ba = pointer to an outgoing byte array
 *
 ****************************************************************************/
void TestConnectionManager::SendTestMessage(quint8 mbyte, const QByteArray &ba)
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
    if ((m_LocalClientSocket.write(block) < (bsize + 1 + sizeof(qint32))) || nofbytes != bsize) {
        QFAIL("TestConnectionManager: writing outgoing data to Socket failed !");
        return;
    }
    // send data immediately:
    if (!m_LocalClientSocket.flush()) {
        QFAIL("TestConnectionManager: flushing outgoing Socket failed !");
    }
}

} // end namespace

QTEST_MAIN(NetworkBase::TestConnectionManager)
