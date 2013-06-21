/****************************************************************************/
/*! \file TestNetworkClientDevice.cpp
 *
 *  \brief Implementation file for class TestNetworkClientDevice.
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
#include <NetworkComponents/Test/TestNetworkClientDevice.h>
#include <NetworkComponents/Include/ProtocolTxCommand.h>
#include <NetworkComponents/Include/ProtocolRxCommand.h>
#include <NetworkComponents/Include/ProtocolTxCommands/HeartBeat.h>
#include <Global/Include/SystemPaths.h>

namespace NetworkBase {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestNetworkClientDevice::TestNetworkClientDevice() :
            m_myDevice(NULL)
{
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestNetworkClientDevice::~TestNetworkClientDevice()
{
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkClientDevice::init()
{
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkClientDevice::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkClientDevice::initTestCase()
{
    // initialize settings path:
    Global::SystemPaths::Instance().SetSettingsPath("../../../../../../Himalaya/Master/Components/Main/Build/Settings");

    // get path to settings:
    QString path = Global::SystemPaths::Instance().GetSettingsPath() + "/Communication/gui";

    // create device
    m_myDevice = new NetworkClientDevice(NCE_TYPE_HIMALAYA_GUI, UT_CLIENT_IP, UT_CLIENT_PORT, path, (QString)"gui_messages", this);
    QCOMPARE(m_myDevice->m_myMessageChecker, (MessageChecker*)NULL);
    QCOMPARE(m_myDevice->m_myNetworkClient, (NetworkClient*)NULL);
    QCOMPARE(m_myDevice->m_myClientType, NCE_TYPE_HIMALAYA_GUI);
    QCOMPARE(m_myDevice->m_myIp, UT_CLIENT_IP);
    QCOMPARE(m_myDevice->m_myPort, UT_CLIENT_PORT);
    QCOMPARE(m_myDevice->m_myPath, path);
    QCOMPARE(m_myDevice->m_myDocType, (QString)"gui_messages");
    QCOMPARE(m_myDevice->m_HBDelay, HEARTBEAT_CLIENT_DELAY);
    QCOMPARE(m_myDevice->RunningCommands.isEmpty(), true);

    // init the device:
    QCOMPARE(m_myDevice->InitializeDevice(), true);
    QVERIFY(m_myDevice->m_myMessageChecker != (MessageChecker*)NULL);
    QVERIFY(m_myDevice->m_myNetworkClient != (NetworkClient*)NULL);
    // check if repeated init will fail:
    QCOMPARE(m_myDevice->InitializeDevice(), true);
    QVERIFY(m_myDevice->m_myMessageChecker != (MessageChecker*)NULL);
    QVERIFY(m_myDevice->m_myNetworkClient != (NetworkClient*)NULL);
    // check that all base commands are initialized:
    QCOMPARE(BaseRxPCmd::CheckClassInstance((QString)"Ack"), true);
    QCOMPARE(m_myDevice->CheckClassRegistration<ProtocolTxCommand>((QString)"HeartBeat"), true);
    QCOMPARE(m_myDevice->CheckClassRegistration<ProtocolTxCommand>((QString)"SomeWrongCommand"), false);

    // set wrong server type and watch initialization fail:
    m_myDevice->m_myNetworkClient->m_myType = NCE_TYPE_UNKNOWN;
    QCOMPARE(m_myDevice->InitializeNetwork(), false);
    QVERIFY(m_myDevice->m_myNetworkClient == (NetworkClient*)NULL);
    // re-init the network:
    QCOMPARE(m_myDevice->InitializeDevice(), true);
    QVERIFY(m_myDevice->m_myMessageChecker != (MessageChecker*)NULL);
    QVERIFY(m_myDevice->m_myNetworkClient != (NetworkClient*)NULL);

    // connect test slots:
    if (!QObject::connect(m_myDevice, SIGNAL(SigPeerConnected(const QString &)),
                          this, SLOT(TestSigPeerConnected(const QString &)))) {
        QFAIL("TestNetworkClientDevice: cannot connect SigPeerConnected signal !");
        return;
    }
    if (!QObject::connect(m_myDevice, SIGNAL(SigPeerDisconnected(const QString &)),
                          this, SLOT(TestSigPeerDisconnected(const QString &)))) {
        QFAIL("TestNetworkClientDevice: cannot connect SigPeerDisconnected signal !");
        return;
    }
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkClientDevice::cleanupTestCase()
{
    // cleanup:
    delete m_myDevice;
}

/****************************************************************************/
/**
 * \brief Internal check for NULL pointer.
 */
/****************************************************************************/
void TestNetworkClientDevice::CheckForNull()
{
    if ((m_myDevice == NULL) ) {
        QFAIL("TestNetworkClientDevice: one of the internal pointers is NULL !");
    }
}

/****************************************************************************/
/**
 * \brief Test server operation.
 */
/****************************************************************************/
void TestNetworkClientDevice::utTestWorkFunctions()
{
    CheckForNull();

    // start device operation:
    QCOMPARE(m_myDevice->StartDevice(), true);

    // this is a dummy function so far, just run it to make sure it
    // does not blow anything up:
    m_myDevice->HandleNetworkError((NetworkClientErrorType_t)NC_AUTHENTICATION_FAILED);

    // socket is not connected so sending commands will fail.
    // but we try to send commands anyway just to make sure nothing goes BOOOM!
    QByteArray bArr;
    QCOMPARE(m_myDevice->SendCommand(NET_NETLAYER_MESSAGE, bArr), false);
    QCOMPARE(m_myDevice->SendCommand((QString)"Test"), false);

    // ===============  Simulate incoming HeartBeat (act as client):
    // check that HBTimer is not active:
    QCOMPARE(m_myDevice->m_HeartBeatTimer.isActive(), false);
    QString hbMsg = "<msg><cmd name=\"HeartBeat\" ref=\"4\" /><dataitems nr=\"7\" /></msg>";
    // create incoming HeartBeat:
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    if (!domD.setContent(hbMsg, true, &err, &errL, &errC)) {
        QFAIL("TestNetworkClientDevice: cannot convert outgoing QString message to QDomDocument !");
        return;
    }
    // "send" the incoming HeartBeat to itself (it will fail since socket is not connected):
    QString cmdname("HeartBeatServer");
    QCOMPARE(((NetworkDevice*)m_myDevice)->ProcessIncomingMessage(cmdname, domD), false);
    // check that client HBTimer is active now:
    QCOMPARE(m_myDevice->m_HeartBeatTimer.isActive(), true);

    // run this function just to make sure it does not crash:
    m_myDevice->DisconnectPeer();
}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestNetworkClientDevice::utTestBadInputHandling()
{
    // run destructor test & kill existing device:
    m_myDevice->DestroyDevice();
    QCOMPARE(m_myDevice->m_myNetworkClient, (NetworkClient*)NULL);
    QCOMPARE(m_myDevice->InitializeNetwork(), true);
    QVERIFY(m_myDevice->m_myNetworkClient != (NetworkClient*)NULL);
    delete m_myDevice;

    // create broken device
    QString path = "./DummyFolder";
    m_myDevice = new NetworkClientDevice(NCE_TYPE_UNKNOWN, (QString)"", (QString)"", path, (QString)"TEST_messages", this);
    QCOMPARE(m_myDevice->m_myMessageChecker, (MessageChecker*)NULL);
    QCOMPARE(m_myDevice->m_myNetworkClient, (NetworkClient*)NULL);
    m_myDevice->DisconnectPeer();
    // try to process non-existent incoming message:
    QDomDocument domD;
    QString msg("SomeWrongMessage");
    QCOMPARE(((NetworkDevice*)m_myDevice)->ProcessIncomingMessage(msg, domD), false);
    // try to send commands with non-existent MessageChecker:
    QByteArray bArr;
    QCOMPARE(m_myDevice->SendCommand(NET_APPLICATION_MESSAGE, bArr), false);
    QCOMPARE(m_myDevice->SendCommand((QString)"Dummy"), false);
    // set wrong device type:
    m_myDevice->m_myType = (MessageLoaderType_t)23;
    // check that all possible initializations fail:
    QCOMPARE(m_myDevice->InitializeNetwork(), false);
    // Client ignores config files so far, therefore it will init OK:
    QCOMPARE(m_myDevice->InitializeMessaging(), false);
    QCOMPARE(m_myDevice->InitializeDevice(), false);
    QVERIFY(m_myDevice->m_myMessageChecker == (MessageChecker*)NULL);
    QVERIFY(m_myDevice->m_myNetworkClient == (NetworkClient*)NULL);
    // try to start the broken device:
    QCOMPARE(m_myDevice->StartDevice(), false);
    // check message processing functions, which shall only be called in derived class objects:
    QString strtemp;
    QByteArray arrtemp;
    QCOMPARE(m_myDevice->ProcessIncomingMessage(strtemp, strtemp, arrtemp), false);
    // this is a dummy function so far, just run it to make sure it
    // does not blow anything up:
    m_myDevice->HandleNetworkError((NetworkClientErrorType_t)NC_AUTHENTICATION_FAILED);
}

/****************************************************************************/
/**
 *  \brief Test slot for connected peer.
 *
 *  \param  str1 = client name
 */
/****************************************************************************/
void TestNetworkClientDevice::TestSigPeerConnected(const QString &str1)
{
    m_String = str1;
    m_FlagPeerConnected = true;
}

/****************************************************************************/
/**
 *  \brief Test slot for disconnected peer.
 *
 *  \param  str1 = client name
 */
/****************************************************************************/
void TestNetworkClientDevice::TestSigPeerDisconnected(const QString &str1)
{
    m_String = str1;
    m_FlagPeerDisconnected = true;
}

} // end namespace

QTEST_MAIN(NetworkBase::TestNetworkClientDevice)
