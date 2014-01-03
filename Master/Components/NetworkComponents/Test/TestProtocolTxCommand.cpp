/****************************************************************************/
/*! \file TestProtocolTxCommand.cpp
 *
 *  \brief Implementation file for class TestProtocolTxCommand.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 14.02.2011
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
#include <NetworkComponents/Test/TestProtocolTxCommand.h>
#include <Global/Include/SystemPaths.h>

namespace NetworkBase {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestProtocolTxCommand::TestProtocolTxCommand() :
        m_TxCommand(NULL),
        m_HeartBeat(NULL),
        m_myDevice(NULL),
        m_DateTime(NULL)
{
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestProtocolTxCommand::~TestProtocolTxCommand()
{
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestProtocolTxCommand::init()
{
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestProtocolTxCommand::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestProtocolTxCommand::initTestCase()
{
    // initialize settings path:
    Global::SystemPaths::Instance().SetSettingsPath("../../../../../../Colorado/Master/Components/Main/Build/Settings");
    // get path to settings:
    QString path = Global::SystemPaths::Instance().GetSettingsPath() + "/Communication/gui";

    // create device
    m_myDevice = new NetworkServerDevice(NSE_TYPE_NORMAL_GUI, (QString)"Colorado Device GUI", path, /*(QString)"gui_messages",*/ this);
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestProtocolTxCommand::cleanupTestCase()
{
    // cleanup:
    delete m_myDevice;
}

/****************************************************************************/
/**
 * \brief Internal check for NULL pointer.
 */
/****************************************************************************/
void TestProtocolTxCommand::CheckForNull()
{
    if (m_myDevice == NULL) {
        QFAIL("TestProtocolTxCommand: one of the internal pointers is NULL !");
    }
}

/****************************************************************************/
/**
 * \brief Test cmd operation.
 */
/****************************************************************************/
void TestProtocolTxCommand::utTestCommandFunctions()
{
    CheckForNull();

    // construct test data:
    QString testMsg = "<msg><cmd name=\"TestCommand\" ref=\"123\" /><dataitems test=\"TEST\" /></msg>";
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    if (!domD.setContent(testMsg, true, &err, &errL, &errC)) {
        QFAIL("TestProtocolTxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    QByteArray bArr = domD.toByteArray();

    // ProtocolCommand is virtual, so we have to use a derived command:
    m_TxCommand = new TxCommand();
    QCOMPARE(m_TxCommand->Initialize((QString)"TestCommand", (quint32)321, m_myDevice), true);
    // check members initialization:
    QCOMPARE(m_TxCommand->m_myDevice, m_myDevice);
    QCOMPARE(m_TxCommand->GetName(), (QString)"TestCommand");
    QCOMPARE(m_TxCommand->GetReference(), (QString)"321");
    delete m_TxCommand;

    // ProtocolCommand is virtual, so we have to use a derived command:
    m_TxCommand = new TxCommand();
    Global::tRefType rett = 777;
    QCOMPARE(m_TxCommand->Initialize((QString)"TestCommand", (quint32)321, rett, bArr, m_myDevice), true);
    // check members initialization:
    QCOMPARE(m_TxCommand->m_myDevice, m_myDevice);
    QCOMPARE(m_TxCommand->GetName(), (QString)"TestCommand");
    QCOMPARE(m_TxCommand->GetReference(), (QString)"321");
    QCOMPARE(m_TxCommand->m_myAppRef, (Global::tRefType)777);
    delete m_TxCommand;

    // check that all inits fail if device pointer is NULL:
    m_TxCommand = new TxCommand();
    QCOMPARE(m_TxCommand->Initialize((QString)"TestCommand", (quint32)321, NULL), false);
    // cleanup:
    delete m_TxCommand;

    m_TxCommand = new TxCommand();
    QCOMPARE(m_TxCommand->Initialize((QString)"TestCommand", (quint32)321, rett, bArr, m_myDevice), true);
    // check that there are no running commands:
    QCOMPARE(m_myDevice->RunningCommands.isEmpty(), true);
    // check that Ack timer is idle:
    QCOMPARE(m_TxCommand->m_myTimer.isActive(), false);
    // check command registration and dismissal:
    QCOMPARE(m_TxCommand->ExecuteWithParameters(NET_APPLICATION_MESSAGE, bArr), false); // execution fails since network device is not connected
    // check that command is registered now:
    QCOMPARE(m_myDevice->RunningCommands.value(321), m_TxCommand);
    // check that Ack timer is running:
    QCOMPARE(m_TxCommand->m_myTimer.isActive(), true);
    // dismiss the command:
    m_TxCommand->DismissCommand();
    // check that there are no running commands:
    QCOMPARE(m_myDevice->RunningCommands.isEmpty(), true);
    // check that Ack timer is idle:
    QCOMPARE(m_TxCommand->m_myTimer.isActive(), false);
    delete m_TxCommand;

    // check Ack timeout action
    m_TxCommand = new TxCommand();
    QCOMPARE(m_TxCommand->Initialize((QString)"TestCommand", (quint32)321, rett, bArr, m_myDevice), true);
    // check that there are no running commands:
    QCOMPARE(m_myDevice->RunningCommands.isEmpty(), true);
    // register command:
    QCOMPARE(m_TxCommand->ExecuteWithParameters(NET_APPLICATION_MESSAGE, bArr), false); // execution fails since network device is not connected
    // check that command is registered now:
    QCOMPARE(m_myDevice->RunningCommands.value(321), m_TxCommand);
    // check that Ack timer is running:
    QCOMPARE(m_TxCommand->m_myTimer.isActive(), true);
    // simulate Ack timeout:
    m_TxCommand->HandleAckTimeout();
    // check that there are no running commands:
    QCOMPARE(m_myDevice->RunningCommands.isEmpty(), true);
    delete m_TxCommand;
}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestProtocolTxCommand::utTestBadInputHandling()
{
    // create dummy object:
    QString reply = "<reply></reply>";
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    if (!domD.setContent(reply, true, &err, &errL, &errC)) {
        QFAIL("TestProtocolTxCommand: cannot convert reply section to QDomDocument !");
        return;
    }
    QDomElement domEl = domD.documentElement();
}

} // end namespace

QTEST_MAIN(NetworkBase::TestProtocolTxCommand)
