/****************************************************************************/
/*! \file TestNetworkServerDevice.cpp
 *
 *  \brief Implementation file for class TestNetworkServerDevice.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 11.02.2011
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
#include <NetworkComponents/Test/TestNetworkServerDevice.h>
#include <NetworkComponents/Include/ProtocolTxCommand.h>
#include <NetworkComponents/Include/ProtocolRxCommand.h>
#include <NetworkComponents/Include/ProtocolTxCommands/HeartBeat.h>
#include <Global/Include/SystemPaths.h>
#include <NetworkComponents/Test/DerivedRxCommand.h>

namespace NetworkBase {

/// Static registration for command classes:
DEFINE_REG_CLASS(ProtocolRxCommand, DerivedRxCommand);    ///< Static registration for DerivedRxCommand
DEFINE_REG_CLASS(ProtocolRxCommand, DerivedRxCommandOne); ///< Static registration for DerivedRxCommandOne
DEFINE_REG_CLASS(ProtocolRxCommand, DerivedRxCommandTwo); ///< Static registration for DerivedRxCommandTwo

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestNetworkServerDevice::TestNetworkServerDevice() :
            m_myDevice(NULL),
            m_FlagPeerConnected(false),
            m_FlagPeerDisconnected(false)
{
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestNetworkServerDevice::~TestNetworkServerDevice()
{
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkServerDevice::init()
{
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkServerDevice::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkServerDevice::initTestCase()
{
    // initialize settings path:
    Global::SystemPaths::Instance().SetSettingsPath("../Settings");

    // get path to settings:
    QString path = Global::SystemPaths::Instance().GetSettingsPath() + "/Communication";

    // create device
    m_myDevice = new DerivedNetworkServerDevice(NSE_TYPE_NORMAL_GUI, (QString)"Colorado Device GUI", path, /*(QString)"netlayer_messages",*/ this);
    QCOMPARE(m_myDevice->m_myMessageChecker, (MessageChecker*)NULL);
    QCOMPARE(m_myDevice->m_myServer, (NetworkServer*)NULL);
    QCOMPARE(m_myDevice->m_myServerType, NSE_TYPE_NORMAL_GUI);
    QCOMPARE(m_myDevice->m_myClient, (QString)"Colorado Device GUI");
    QCOMPARE(m_myDevice->m_myType, CML_TYPE_SERVER);
    QCOMPARE(m_myDevice->m_myPath, path);
    //QCOMPARE(m_myDevice->m_myDocType, (QString)"netlayer_messages");
    QCOMPARE(m_myDevice->m_cmdRef, (quint32)0);
    QCOMPARE(m_myDevice->m_HBDelay, HEARTBEAT_SERVER_DELAY);
    QCOMPARE(m_myDevice->RunningCommands.isEmpty(), true);

    // init the device:
    QCOMPARE(m_myDevice->InitializeDevice(), true);
    QVERIFY(m_myDevice->m_myMessageChecker != (MessageChecker*)NULL);
    QVERIFY(m_myDevice->m_myServer != (NetworkServer*)NULL);
    // check if repeated init will fail:
    QCOMPARE(m_myDevice->InitializeDevice(), true);
    QVERIFY(m_myDevice->m_myMessageChecker != (MessageChecker*)NULL);
    QVERIFY(m_myDevice->m_myServer != (NetworkServer*)NULL);
    // check that all base commands are initialized:
    QCOMPARE(BaseRxPCmd::CheckClassInstance((QString)"Ack"), true);
    QCOMPARE(m_myDevice->CheckClassRegistration<ProtocolTxCommand>((QString)"HeartBeat"), true);
    QCOMPARE(m_myDevice->CheckClassRegistration<ProtocolTxCommand>((QString)"SomeWrongTXCmd"), false);
    QCOMPARE(m_myDevice->CheckClassRegistration<ProtocolRxCommand>((QString)"Ack"), true);
    QCOMPARE(m_myDevice->CheckClassRegistration<ProtocolRxCommand>((QString)"SomeWrongCommand"), false);

    // set wrong server type and watch initialization fail:
    m_myDevice->m_myServer->m_myType = NSE_TYPE_UNKNOWN;
    QCOMPARE(m_myDevice->InitializeNetwork(), false);
    QVERIFY(m_myDevice->m_myServer == (NetworkServer*)NULL);
    // re-init the network:
    QCOMPARE(m_myDevice->InitializeDevice(), true);
    QVERIFY(m_myDevice->m_myMessageChecker != (MessageChecker*)NULL);
    QVERIFY(m_myDevice->m_myServer != (NetworkServer*)NULL);

    // connect test slots:
    if (!QObject::connect(m_myDevice, SIGNAL(SigPeerConnected(const QString &)),
                          this, SLOT(TestSigPeerConnected(const QString &)))) {
        QFAIL("TestNetworkServerDevice: cannot connect SigPeerConnected signal !");
        return;
    }
    if (!QObject::connect(m_myDevice, SIGNAL(SigPeerDisconnected(const QString &)),
                          this, SLOT(TestSigPeerDisconnected(const QString &)))) {
        QFAIL("TestNetworkServerDevice: cannot connect SigPeerDisconnected signal !");
        return;
    }
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestNetworkServerDevice::cleanupTestCase()
{
    // cleanup:
    delete m_myDevice;
}

/****************************************************************************/
/**
 * \brief Internal check for NULL pointer.
 */
/****************************************************************************/
void TestNetworkServerDevice::CheckForNull()
{
    if ((m_myDevice == NULL) ) {
        QFAIL("TestNetworkServerDevice: one of the internal pointers is NULL !");
    }
}

/****************************************************************************/
/**
 * \brief Test server operation.
 */
/****************************************************************************/
void TestNetworkServerDevice::utTestCommandsManagementInterface()
{
    // check that reference cannot be Zero:
    m_myDevice->m_cmdRef = 0xFFFFFFFF;
    // reference gets incremented and would have been Zero here:
    QCOMPARE(m_myDevice->GetNewCmdReference(), (quint32)1);

    // check that there is no currently running commands:
    QCOMPARE(m_myDevice->RunningCommands.isEmpty(), true);
    // register some dummy commands:
    m_myDevice->RegisterRunningCommand((quint32)23, (ProtocolTxCommand*)NULL);
    m_myDevice->RegisterRunningCommand((quint32)351, (ProtocolTxCommand*)NULL);
    m_myDevice->RegisterRunningCommand((quint32)6142, (ProtocolTxCommand*)NULL);
    // and one real command:
    //QDomDocument domD;
    ProtocolTxCommand *pC = m_myDevice->CreateNewOutgoingCommand<ProtocolTxCommand, BaseTxPCmd>((QString)"HeartBeat");
    if (pC == NULL) {
        QFAIL("TestNetworkServerDevice: command creation failed !");
        return;
    }
    m_myDevice->RegisterRunningCommand((quint32)45, pC);
    // check that NofCommands in the list is four:
    QCOMPARE(m_myDevice->RunningCommands.size(), (int)4);
    // try to insert commands with already existing referenses:
    m_myDevice->RegisterRunningCommand((quint32)351, pC);
    m_myDevice->RegisterRunningCommand((quint32)6142, pC);
    // check that NofCommands in the list is four:
    QCOMPARE(m_myDevice->RunningCommands.size(), (int)4);
    // check that dummy commands are still associated with NULL pointer:
    QCOMPARE(m_myDevice->FetchRunningCommand(351), (ProtocolTxCommand*)NULL);
    QCOMPARE(m_myDevice->FetchRunningCommand(6142), (ProtocolTxCommand*)NULL);
    // fetch real command:
    QCOMPARE(m_myDevice->FetchRunningCommand(45), pC);
    // deregister the real command (it will be deleted):
    m_myDevice->DeregisterRunningCommand((quint32)45, pC);
    // check that NofCommands in the list is two:
    QCOMPARE(m_myDevice->RunningCommands.size(), (int)3);
    // try to fetch the deregistered command (we get NULL since command is not there anymore):
    QCOMPARE(m_myDevice->FetchRunningCommand(45), (ProtocolTxCommand*)NULL);
    // fetching dummy command shall return NULL:
    QCOMPARE(m_myDevice->FetchRunningCommand(23), (ProtocolTxCommand*)NULL);
    // deregister the dummy commands (device shall not try to "delete" them):
    m_myDevice->DeregisterRunningCommand((quint32)23, (ProtocolTxCommand*)NULL);
    m_myDevice->DeregisterRunningCommand((quint32)351, (ProtocolTxCommand*)NULL);
    // check size of running commands list, it shall be one:
    QCOMPARE(m_myDevice->RunningCommands.size(), (int)1);
    m_myDevice->DeregisterRunningCommand((quint32)6142, (ProtocolTxCommand*)NULL);
    // check size of running commands list, it shall be Zero:
    QCOMPARE(m_myDevice->RunningCommands.size(), (int)0);
    delete pC;
}

/****************************************************************************/
/**
 * \brief Test server operation.
 */
/****************************************************************************/
void TestNetworkServerDevice::utTestCommandsCreationInterface()
{
    QDomDocument domD;
    ProtocolRxCommand *pRC = NULL;
    ProtocolTxCommand *pTC = NULL;

    // construct test data:
    QString testMsg = "<message><cmd name=\"Ack\" ref=\"123\" /><dataitems cmd=\"TestCommand\" status=\"ok\" /></message>";
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }

    // ================ creation of incoming commands:

    // create real command:
    pRC = m_myDevice->CreateNewIncomingCommand<ProtocolRxCommand, BaseRxPCmd>((QString)"Ack", domD);
    QVERIFY(pRC != (ProtocolRxCommand *)NULL);
    QCOMPARE(pRC->GetName(), (QString)"Ack");
    delete pRC;
    // try to create non-existing command:
    pRC = m_myDevice->CreateNewIncomingCommand<ProtocolRxCommand, BaseRxPCmd>((QString)"SomeWrongCommand1", domD);
    QVERIFY(pRC == (ProtocolRxCommand *)NULL);

    // create real command:
    QByteArray bArr = QString("<message><cmd name=\"DerivedRxCommand\" ref=\"123\" /></message> and Some Test Text").toUtf8();
    pRC = m_myDevice->CreateNewIncomingCommand<ProtocolRxCommand, BaseRxPCmd>((QString)"DerivedRxCommand", (QString)"125", bArr);
    QVERIFY(pRC != (ProtocolRxCommand *)NULL);
    QCOMPARE(pRC->GetName(), (QString)"DerivedRxCommand");
    delete pRC;
    // try to create non-existing command:
    pRC = m_myDevice->CreateNewIncomingCommand<ProtocolRxCommand, BaseRxPCmd>((QString)"SomeWrongCommand3", (QString)"126", bArr);
    QVERIFY(pRC == (ProtocolRxCommand *)NULL);

    // ================ creation of outgoing commands:

    // create real command:
    pTC = m_myDevice->CreateNewOutgoingCommand<ProtocolTxCommand, BaseTxPCmd>((QString)"HeartBeat");
    QVERIFY(pTC != (ProtocolTxCommand *)NULL);
    QCOMPARE(pTC->GetName(), (QString)"HeartBeat");
    delete pTC;
    // try to create non-existing command:
    pTC = m_myDevice->CreateNewOutgoingCommand<ProtocolTxCommand, BaseTxPCmd>((QString)"SomeWrongCommand6");
    QVERIFY(pTC == (ProtocolTxCommand *)NULL);

}

/****************************************************************************/
/**
 *  \brief Convert text message to QDomDocument.
 *
 *  \param[in]  message = text message
 *  \param[out] ddoc = corresponding QDomDocument
 *
 *  \return  true if conversion is ok, false otherwise
 */
/****************************************************************************/
bool TestNetworkServerDevice::GetMsgAsDomDoc(const QString &message, QDomDocument *ddoc)
{
    QString err = "";
    int errL = 0;
    int errC = 0;
    if (!ddoc->setContent(message, true, &err, &errL, &errC)) {
        return false;
    }
    return true;
}

/****************************************************************************/
/**
 *  \brief Convert text message to QByteArray.
 *
 *  \param[in]  message = text message
 *  \param[out] barr = corresponding QByteArray
 *
 *  \return  true if conversion is ok, false otherwise
 */
/****************************************************************************/
bool TestNetworkServerDevice::GetMsgAsByteArray(const QString &message, QByteArray *barr)
{
    QDomDocument domD;
    bool result = GetMsgAsDomDoc(message, &domD);
    *barr = domD.toByteArray();
    if (barr->isEmpty()) {
        result = false;
    }
    return result;
}

/****************************************************************************/
/**
 * \brief Test server operation.
 */
/****************************************************************************/
void TestNetworkServerDevice::utTestHeartBeatFunctions()
{
    // try to create HeartBeat command:
    QCOMPARE(m_myDevice->RunningCommands.size(), (int)0);
    QCOMPARE(m_myDevice->m_HeartBeatTimer.isActive(), false);
    m_myDevice->HandleHeartBeat();
    // check size of running commands list, it shall be one:
    QCOMPARE(m_myDevice->RunningCommands.size(), (int)1);
    // delete the instantiated HeartBeat command:
    ProtocolTxCommand *pC = m_myDevice->FetchRunningCommand(m_myDevice->m_cmdRef);
    QVERIFY(pC != (ProtocolTxCommand*)NULL);
    quint32 hbRef = pC->m_myRef;
    m_myDevice->DeregisterRunningCommand(hbRef, pC);
    // check that the timer is running now:
    QCOMPARE(m_myDevice->m_HeartBeatTimer.isActive(), true);
    // stop the HeartBeat timer:
    m_myDevice->HandleHeartBeatTimeout();
    QCOMPARE(m_myDevice->m_HeartBeatTimer.isActive(), false);
    // restart the HeartBeat timer:
    m_myDevice->RestartHeartBeatTimer();
    QCOMPARE(m_myDevice->m_HeartBeatTimer.isActive(), true);
    // emulate peer disconnect:
    m_myDevice->PeerDisconnected((QString)"DummyPeer1");
    // check consequences:
    QCOMPARE(m_myDevice->m_HeartBeatTimer.isActive(), false);
    QCOMPARE(m_FlagPeerDisconnected, true);
    QCOMPARE(m_String, (QString)"DummyPeer1");
    // reset flags:
    m_String = "";
    m_FlagPeerDisconnected = false;
    // emulate peer connect:
    m_myDevice->PeerConnected((QString)"DummyPeer2");
    // check consequences:
    QCOMPARE(m_myDevice->m_HeartBeatTimer.isActive(), true);
    QCOMPARE(m_FlagPeerConnected, true);
    QCOMPARE(m_String, (QString)"DummyPeer2");
    // reset flags:
    m_String = "";
    m_FlagPeerConnected = false;
    // stop the HeartBeat timer:
    m_myDevice->DisconnectPeer();
    QCOMPARE(m_myDevice->m_HeartBeatTimer.isActive(), false);
    // destroy running HeartBeats:
    QList<ProtocolTxCommand*> CmdList = m_myDevice->RunningCommands.values();
    for (int i = 0; i < CmdList.size(); i++) {
        if (CmdList[i] != NULL) {
            delete CmdList[i];
        }
    }

    // start device operation:
    QCOMPARE(m_myDevice->StartDevice(), true);
    // start again, this shall fail since server already listens to the IP/PORT:
//    QCOMPARE(m_myDevice->StartDevice(), false);

    // this is a dummy function so far, just run it to make sure it
    // does not blow anything up:
    m_myDevice->HandleNetworkError((DisconnectType_t)UNKNOWN_ERROR, (QString)"Test");

    // socket is not connected so sending commands will fail.
    // but we try to send commands anyway just to make sure nothing goes BOOOM!
    QByteArray bArr;
    QCOMPARE(m_myDevice->SendCommand(NET_APPLICATION_MESSAGE, bArr), false);
    QCOMPARE(m_myDevice->SendCommand((QString)"Test"), false);

    // ===============  Simulate HeartBeat ping-pong:
    // check that running commands list is empty:
    QCOMPARE(m_myDevice->RunningCommands.size(), (int)0);
    // create outgoing HeartBeat:
    m_myDevice->HandleHeartBeat();
    // check that HeartBeat is in the running commands list:
    QCOMPARE(m_myDevice->RunningCommands.size(), (int)1);
    // get the HeartBeat number and reference:
    pC = m_myDevice->FetchRunningCommand(m_myDevice->m_cmdRef);
    QVERIFY(pC != (ProtocolTxCommand*)NULL);
    quint16 num = ((HeartBeat*)pC)->GetSentNumber() + 1;
    hbRef = pC->m_myRef;
    QString hbMsg = "<message><cmd name=\"HeartBeat\" ref=\"" + QString::number(hbRef, 10) +
                    "\" /><dataitems nr=\"" + QString::number(num, 10) + "\" /></message>";
    // create incoming HeartBeat:
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    if (!domD.setContent(hbMsg, true, &err, &errL, &errC)) {
        QFAIL("TestNetworkServerDevice: cannot convert outgoing QString message to QDomDocument !");
        return;
    }
    // "send" the incoming HeartBeat to itself:
    QCOMPARE(m_myDevice->ProcessIncomingMessage((QString)"HeartBeatClient", domD), true);
    // check that HeartBeat is removed from running commands list:
    QCOMPARE(m_myDevice->RunningCommands.size(), (int)0);

    // ===============  Use HeartBeat to check Ack command:
    // create outgoing HeartBeat:
    m_myDevice->HandleHeartBeat();
    // check that HeartBeat is in the running commands list:
    QCOMPARE(m_myDevice->RunningCommands.size(), (int)1);
    // get the HeartBeat reference:
    pC = m_myDevice->FetchRunningCommand(m_myDevice->m_cmdRef);
    QVERIFY(pC != (ProtocolTxCommand*)NULL);
    hbRef = pC->m_myRef;
    QString ackMsg = "<message><cmd name=\"Ack\" ref=\"" + QString::number(hbRef, 10) +
                    "\" /><dataitems cmd=\"HeartBeat\" status=\"ok\" /></message>";
    // create incoming Ack:
    if (!domD.setContent(ackMsg, true, &err, &errL, &errC)) {
        QFAIL("TestNetworkServerDevice: cannot convert outgoing QString message to QDomDocument !");
        return;
    }
    // "send" the incoming Ack to itself:
    QCOMPARE(m_myDevice->ProcessIncomingMessage((QString)"Ack", domD), true);
    // check that HeartBeat is removed from running commands list:
    QCOMPARE(m_myDevice->RunningCommands.size(), (int)0);
}

/****************************************************************************/
/**
 * \brief Test server operation.
 */
/****************************************************************************/
void TestNetworkServerDevice::utTestWorkFunctions()
{

//    CMH_TEXT_MESSAGE
//    CMH_COMBINED_MESSAGE

    QByteArray baMsg;
    QString testMsg = (QString)"<message>" +
              (QString)"<cmd name=\"DerivedRxCommand\" ref=\"123\" />" +
              (QString)"</message>";
    if (!GetMsgAsByteArray(testMsg, &baMsg)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    QCOMPARE(m_myDevice->m_FlagCommandExecuted, false);
    m_myDevice->GetIncomingMsg(NET_NETLAYER_MESSAGE, baMsg);
    QCOMPARE(m_myDevice->m_FlagCommandExecuted, true);
    // reset the flag:
    m_myDevice->m_FlagCommandExecuted = false;

    testMsg = (QString)"<message>" +
              (QString)"<cmd name=\"DerivedRxCommandOne\" ref=\"123\" />" +
              (QString)"<dataitems att1=\"TEST1\" att2=\"TEST2\" att3=\"TEST5\" >" +
              (QString)"</dataitems>" +
              (QString)"</message>";
    if (!GetMsgAsByteArray(testMsg, &baMsg)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    QCOMPARE(m_myDevice->m_FlagCommandExecuted, false);
    m_myDevice->GetIncomingMsg(NET_NETLAYER_MESSAGE, baMsg);
    QCOMPARE(m_myDevice->m_FlagCommandExecuted, true);
    // reset the flag:
    m_myDevice->m_FlagCommandExecuted = false;

    testMsg = (QString)"<message>" +
                  (QString)"<cmd name=\"DerivedRxCommandTwo\" ref=\"123\" nofitems=\"2\" />" +
                  (QString)"<dataitems att1=\"TEST1\" att2=\"TEST2\" att3=\"TEST5\" nofitems=\"2\">" +
                    (QString)"<subitem satt1=\"sub1\" satt2=\"sub2\" satt3=\"sub1\" satt4=\"sub2\" />" +
                    (QString)"<subitem satt1=\"sub11\" satt2=\"sub22\" satt3=\"sub33\" satt4=\"sub44\" />" +
                  (QString)"</dataitems>" +
                  (QString)"<dataitems att1=\"TEST3\" att2=\"TEST4\" att3=\"TEST6\" nofitems=\"2\">" +
                    (QString)"<subitem satt1=\"sub3\" satt2=\"sub4\" satt3=\"sub3\" satt4=\"sub4\" />" +
                    (QString)"<subitem satt1=\"sub33\" satt2=\"sub44\" satt3=\"sub55\" satt4=\"sub66\" />" +
                  (QString)"</dataitems>" +
                  (QString)"</message>";
    if (!GetMsgAsByteArray(testMsg, &baMsg)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    QCOMPARE(m_myDevice->m_FlagCommandExecuted, false);
    m_myDevice->GetIncomingMsg(NET_NETLAYER_MESSAGE, baMsg);
    QCOMPARE(m_myDevice->m_FlagCommandExecuted, true);
    // reset the flag:
    m_myDevice->m_FlagCommandExecuted = false;


    testMsg = (QString)"<message>" +
              (QString)"<cmd name=\"DerivedRxCommandOne\" ref=\"123\" />" +
              (QString)"<dataitems att1=\"TEST1\" att2=\"TEST2\" att3=\"TEST5\" >" +
              (QString)"</dataitems>" +
              (QString)"</message>";
    baMsg.clear();
    baMsg = ((QString)"SomeTest String to Emulate a file").toUtf8();
    baMsg.prepend(testMsg.toUtf8());
    QCOMPARE(m_myDevice->m_FlagCommandExecuted, false);
    m_myDevice->GetIncomingMsg(NET_APPLICATION_MESSAGE, baMsg);
    // this shall fail since base calss is not supposed to handle this type of command
    QCOMPARE(m_myDevice->m_FlagCommandExecuted, false);

    baMsg.clear();
    // simulate XML file:
    baMsg = ((QString)"<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE netlayer_messages><netlayer_messages version=\"1.0\">" +
             (QString)"<server_messages><message id=\"\" ><cmd name=\"Ack\" /><dataitems cmd=\"\" /></message>" +
             (QString)"</server_messages></netlayer_messages>").toUtf8();
    baMsg.prepend(testMsg.toUtf8());
    QCOMPARE(m_myDevice->m_FlagCommandExecuted, false);
    m_myDevice->GetIncomingMsg(NET_APPLICATION_MESSAGE, baMsg);
    // this shall fail since base calss is not supposed to handle this type of command
    QCOMPARE(m_myDevice->m_FlagCommandExecuted, false);
}

/****************************************************************************/
/**
 * \brief Test bad input.
 */
/****************************************************************************/
void TestNetworkServerDevice::utTestBadInputHandling()
{
    // kill existing device:
    m_myDevice->DestroyDevice();
    QCOMPARE(m_myDevice->m_myServer, (NetworkServer*)NULL);
    delete m_myDevice;

    // create broken device
    QString path = "./DummyFolder";
    m_myDevice = new DerivedNetworkServerDevice(NSE_TYPE_UNKNOWN, (QString)"Some Dummy Client", path, /*(QString)"TEST_messages",*/ this);
    QCOMPARE(m_myDevice->m_myMessageChecker, (MessageChecker*)NULL);
    QCOMPARE(m_myDevice->m_myServer, (NetworkServer*)NULL);
    // try to process non-existent incoming message:
    QDomDocument domD;
    QCOMPARE(m_myDevice->ProcessIncomingMessage((QString)"SomeWrongMessage", domD), false);
    // try to send commands with non-existent MessageChecker:
    QByteArray bArr;
    QCOMPARE(m_myDevice->SendCommand(NET_APPLICATION_MESSAGE, bArr), false);
    QCOMPARE(m_myDevice->SendCommand((QString)"Dummy"), false);
    // check that all possible initializations fail:
    QCOMPARE(m_myDevice->InitializeNetwork(), false);
    QCOMPARE(m_myDevice->InitializeMessaging(), false);
    QCOMPARE(m_myDevice->InitializeDevice(), false);
    QVERIFY(m_myDevice->m_myMessageChecker == (MessageChecker*)NULL);
    QVERIFY(m_myDevice->m_myServer == (NetworkServer*)NULL);
    // try to start the broken device:
    QCOMPARE(m_myDevice->StartDevice(), false);
    // check message processing functions, which shall only be called in derived class objects:
    QString strtemp;
    QByteArray arrtemp;
    QCOMPARE(m_myDevice->ProcessIncomingMessage(strtemp, strtemp, arrtemp), false);
    // this is a dummy function so far, just run it to make sure it
    // does not blow anything up:
    m_myDevice->HandleNetworkError((DisconnectType_t)UNKNOWN_ERROR, (QString)"Test");
}

/****************************************************************************/
/**
 *  \brief Test slot for client conneciton.
 *
 *  \param  str1 = client name
 */
/****************************************************************************/
void TestNetworkServerDevice::TestSigPeerConnected(const QString &str1)
{
    m_String = str1;
    m_FlagPeerConnected = true;
}

/****************************************************************************/
/**
 *  \brief Test slot for client disconneciton.
 *
 *  \param  str1 = client name
 */
/****************************************************************************/
void TestNetworkServerDevice::TestSigPeerDisconnected(const QString &str1)
{
    m_String = str1;
    m_FlagPeerDisconnected = true;
}

} // end namespace

QTEST_MAIN(NetworkBase::TestNetworkServerDevice)
