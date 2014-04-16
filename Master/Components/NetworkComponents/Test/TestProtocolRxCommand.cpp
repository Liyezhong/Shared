/****************************************************************************/
/*! \file TestProtocolRxCommand.cpp
 *
 *  \brief Implementation file for class TestProtocolRxCommand.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 31.03.2011
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
#include <NetworkComponents/Test/TestProtocolRxCommand.h>
#include <Global/Include/SystemPaths.h>

namespace NetworkBase {


/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestProtocolRxCommand::TestProtocolRxCommand() :
        m_RxCommand(NULL),
        m_myDevice(NULL)
{
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestProtocolRxCommand::~TestProtocolRxCommand()
{
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestProtocolRxCommand::init()
{
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestProtocolRxCommand::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestProtocolRxCommand::initTestCase()
{
    // initialize settings path:
    Global::SystemPaths::Instance().SetSettingsPath("../../../../../../Colorado/Master/Components/Main/Build/Settings");
    // get path to settings:
    QString path = Global::SystemPaths::Instance().GetSettingsPath() + "/Communication/gui";

    // create device
    m_myDevice = new NetworkServerDevice(NSE_TYPE_NORMAL_GUI, (QString)"Colorado Device GUI", path,/* (QString)"gui_messages",*/ this);
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestProtocolRxCommand::cleanupTestCase()
{
    // cleanup:
    delete m_myDevice;
}

/****************************************************************************/
/**
 * \brief Internal check for NULL pointer.
 */
/****************************************************************************/
void TestProtocolRxCommand::CheckForNull()
{
    if (m_myDevice == NULL) {
        QFAIL("TestProtocolRxCommand: one of the internal pointers is NULL !");
    }
}

/****************************************************************************/
/**
 * \brief Test cmd operation.
 */
/****************************************************************************/
void TestProtocolRxCommand::utTestCommandFunctions()
{
    CheckForNull();

    // ProtocolRxCommand is abstract, derived calss is used instead

    // construct test data:
    QString testMsg = "<msg><cmd name=\"TestCommand\" ref=\"123\" /><dataitems test1=\"TEST1\" test2=\"TEST2\" /></msg>";
    QDomDocument domD;
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    QStringList attrs = QStringList() << "test1" << "test2";

    // check the default constructor
    m_RxCommand = new RxCommand();
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), true);
    // message data tree shall be empty since no attributes were provided
    // base class members:
    QCOMPARE(m_RxCommand->m_myDevice, m_myDevice);
    QCOMPARE(m_RxCommand->GetName(), (QString)"TestCommand");
    QCOMPARE(m_RxCommand->m_myMessage, domD);
    // derived class members
    QCOMPARE(m_RxCommand->my_NOfDataitems, (qint16)1);
    QCOMPARE(m_RxCommand->m_myRef, (QString)"123");
    QCOMPARE(m_RxCommand->m_myAttributes.isEmpty(), true);
    QCOMPARE(m_RxCommand->m_myDataAttributes.isEmpty(), true);
    QCOMPARE(m_RxCommand->m_myMessageDataTree.isEmpty(), true);
    delete m_RxCommand;

    m_RxCommand = new RxCommand(attrs);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), true);
    // message data tree shall not be empty since attributes were provided
    // base class members:
    QCOMPARE(m_RxCommand->m_myDevice, m_myDevice);
    QCOMPARE(m_RxCommand->GetName(), (QString)"TestCommand");
    QCOMPARE(m_RxCommand->m_myMessage, domD);
    // derived class members
    QCOMPARE(m_RxCommand->my_NOfDataitems, (qint16)1);
    QCOMPARE(m_RxCommand->m_myRef, (QString)"123");
    QCOMPARE(m_RxCommand->m_myAttributes.isEmpty(), false);
    QCOMPARE(m_RxCommand->m_myDataAttributes.isEmpty(), true);
    QCOMPARE(m_RxCommand->m_myMessageDataTree.isEmpty(), false);
    QCOMPARE(m_RxCommand->m_myMessageDataTree[0].DataitemsValues.isEmpty(), false);
    QCOMPARE(m_RxCommand->m_myMessageDataTree[0].SubDataitemsValues.isEmpty(), true);
    delete m_RxCommand;

    testMsg = (QString)"<msg>" +
              (QString)"<cmd name=\"TestCommand\" ref=\"123\" nofitems=\"2\" />" +
              (QString)"<dataitems test1=\"TEST1\" test2=\"TEST2\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub1\" att2=\"sub2\" />" +
                (QString)"<subitem att1=\"sub11\" att2=\"sub22\" />" +
              (QString)"</dataitems>" +
              (QString)"<dataitems test1=\"TEST3\" test2=\"TEST4\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub3\" att2=\"sub4\" />" +
                (QString)"<subitem att1=\"sub33\" att2=\"sub44\" />" +
              (QString)"</dataitems>" +
              (QString)"</msg>";
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    attrs.clear();
    attrs << "test1" << "test2" << "nofitems";
    QStringList dattrs = QStringList() << "att1" << "att2";
    m_RxCommand = new RxCommand(attrs, dattrs);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), true);
    // message data tree shall not be empty since attributes were provided
    // base class members:
    QCOMPARE(m_RxCommand->m_myDevice, m_myDevice);
    QCOMPARE(m_RxCommand->GetName(), (QString)"TestCommand");
    QCOMPARE(m_RxCommand->m_myMessage, domD);
    // derived class members
    QCOMPARE(m_RxCommand->my_NOfDataitems, (qint16)2);
    QCOMPARE(m_RxCommand->m_myRef, (QString)"123");
    QCOMPARE(m_RxCommand->m_myAttributes.isEmpty(), false);
    QCOMPARE(m_RxCommand->m_myDataAttributes.isEmpty(), false);
    QCOMPARE(m_RxCommand->m_myMessageDataTree.isEmpty(), false);
    QCOMPARE(m_RxCommand->m_myMessageDataTree[0].DataitemsValues.isEmpty(), false);
    QCOMPARE(m_RxCommand->m_myMessageDataTree[0].DataitemsValues[0], (QString)"TEST1");
    QCOMPARE(m_RxCommand->m_myMessageDataTree[0].DataitemsValues[1], (QString)"TEST2");
    QCOMPARE(m_RxCommand->m_myMessageDataTree[1].DataitemsValues[0], (QString)"TEST3");
    QCOMPARE(m_RxCommand->m_myMessageDataTree[1].DataitemsValues[1], (QString)"TEST4");
    QCOMPARE(m_RxCommand->m_myMessageDataTree[0].SubDataitemsValues.isEmpty(), false);
    QCOMPARE(m_RxCommand->m_myMessageDataTree[0].SubDataitemsValues[0][0], (QString)"sub1");
    QCOMPARE(m_RxCommand->m_myMessageDataTree[0].SubDataitemsValues[0][1], (QString)"sub2");
    QCOMPARE(m_RxCommand->m_myMessageDataTree[0].SubDataitemsValues[1][0], (QString)"sub11");
    QCOMPARE(m_RxCommand->m_myMessageDataTree[0].SubDataitemsValues[1][1], (QString)"sub22");
    QCOMPARE(m_RxCommand->m_myMessageDataTree[1].SubDataitemsValues[0][0], (QString)"sub3");
    QCOMPARE(m_RxCommand->m_myMessageDataTree[1].SubDataitemsValues[0][1], (QString)"sub4");
    QCOMPARE(m_RxCommand->m_myMessageDataTree[1].SubDataitemsValues[1][0], (QString)"sub33");
    QCOMPARE(m_RxCommand->m_myMessageDataTree[1].SubDataitemsValues[1][1], (QString)"sub44");
    delete m_RxCommand;

    testMsg = "<msg><cmd name=\"TestCommand\" ref=\"123\" /><dataitems test1=\"TEST1\" test2=\"TEST2\" /></msg>";
    QByteArray bArr = testMsg.toUtf8();
    QDomDocument domDD;
    domDD.clear();

    m_RxCommand = new RxCommand();
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", QString("456"), bArr, m_myDevice), true);
    // message data tree shall be empty since no attributes were provided
    // base class members:
    QCOMPARE(m_RxCommand->m_myDevice, m_myDevice);
    QCOMPARE(m_RxCommand->GetName(), (QString)"TestCommand");
    QCOMPARE(m_RxCommand->m_myMessage, domDD);
    // derived class members
    QCOMPARE(m_RxCommand->my_NOfDataitems, (qint16)0);
    QCOMPARE(m_RxCommand->m_myRef, (QString)"456");
    QCOMPARE(m_RxCommand->m_myAttributes.isEmpty(), true);
    QCOMPARE(m_RxCommand->m_myDataAttributes.isEmpty(), true);
    QCOMPARE(m_RxCommand->m_myMessageDataTree.isEmpty(), true);
    QCOMPARE(m_RxCommand->m_myPayloadArray, bArr);
    // these functions will fail since network device is not connected,
    // but lets run them to make sure they do not blow anything up:
    QCOMPARE(m_RxCommand->CheckStatus(CMH_MSG_SENDING_FAILED), false);
    QCOMPARE(m_RxCommand->CheckStatus(CMH_MSG_SENDING_ACK), true);
    // run test function:
    m_RxCommand->PrintMessageDataTree();
    delete m_RxCommand;

    // check that all inits fail if device pointer is NULL:
    m_RxCommand = new RxCommand();
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, NULL), false);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", NULL), false);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", QString("456"), bArr, NULL), false);
    // cleanup:
    delete m_RxCommand;
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
bool TestProtocolRxCommand::GetMsgAsDomDoc(const QString &message, QDomDocument *ddoc)
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
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestProtocolRxCommand::utTestBadInputHandling()
{

    // create dummy object:
    QString reply = "<reply></reply>";
    QString err = "";
    int errL = 0;
    int errC = 0;
    QDomDocument domD;
    if (!domD.setContent(reply, true, &err, &errL, &errC)) {
        QFAIL("TestProtocolRxCommand: cannot convert reply section to QDomDocument !");
        return;
    }
    QDomElement domEl = domD.documentElement();

    CheckForNull();
    QStringList attrs = QStringList() << "test1" << "test2" << "nofitems";
    QStringList dattrs = QStringList() << "att1" << "att2";
    QString testMsg = (QString)"<msg>" +
              (QString)"<cmd name=\"TestCommand\" ref=\"123\" nofitems=\"2\" />" +
              (QString)"<dataitems test1=\"TEST1\" test2=\"TEST2\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub1\" att2=\"sub2\" />" +
                (QString)"<subitem att1=\"sub11\" att2=\"sub22\" />" +
              (QString)"</dataitems>" +
              (QString)"<dataitems test1=\"TEST3\" test2=\"TEST4\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub3\" att2=\"sub4\" />" +
                // one subitem is missing    (QString)"<subitem att1=\"sub33\" att2=\"sub44\" />" +
              (QString)"</dataitems>" +
              (QString)"</msg>";
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    m_RxCommand = new RxCommand(attrs, dattrs);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), false);
    delete m_RxCommand;

    testMsg = (QString)"<msg>" +
              (QString)"<cmd name=\"TestCommand\" ref=\"123\" nofitems=\"2\" />" +
              (QString)"<dataitems test1=\"TEST1\" test2=\"TEST2\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub1\" att2=\"sub2\" />" +
                // one subitem is missing    (QString)"<subitem att1=\"sub11\" att2=\"sub22\" />" +
              (QString)"</dataitems>" +
              (QString)"<dataitems test1=\"TEST3\" test2=\"TEST4\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub3\" att2=\"sub4\" />" +
                (QString)"<subitem att1=\"sub33\" att2=\"sub44\" />" +
              (QString)"</dataitems>" +
              (QString)"</msg>";
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    m_RxCommand = new RxCommand(attrs, dattrs);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), false);
    delete m_RxCommand;

    testMsg = (QString)"<msg>" +
              // CMD is missing now: (QString)"<cmd name=\"TestCommand\" ref=\"123\" nofitems=\"2\" />" +
              (QString)"<dataitems test1=\"TEST1\" test2=\"TEST2\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub1\" att2=\"sub2\" />" +
                (QString)"<subitem att1=\"sub11\" att2=\"sub22\" />" +
              (QString)"</dataitems>" +
              (QString)"<dataitems test1=\"TEST3\" test2=\"TEST4\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub3\" att2=\"sub4\" />" +
                (QString)"<subitem att1=\"sub33\" att2=\"sub44\" />" +
              (QString)"</dataitems>" +
              (QString)"</msg>";
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    m_RxCommand = new RxCommand(attrs, dattrs);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), false);
    delete m_RxCommand;

    testMsg = (QString)"<msg>" +
              (QString)"<cmd name=\"TestCommand\" ref=\"123\" nofitems=\"2\" />" +
              //nofitems is missing now: (QString)"<dataitems test1=\"TEST1\" test2=\"TEST2\" nofitems=\"2\">" +
              (QString)"<dataitems test1=\"TEST1\" test2=\"TEST2\" >" +
                (QString)"<subitem att1=\"sub1\" att2=\"sub2\" />" +
                (QString)"<subitem att1=\"sub11\" att2=\"sub22\" />" +
              (QString)"</dataitems>" +
              (QString)"<dataitems test1=\"TEST3\" test2=\"TEST4\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub3\" att2=\"sub4\" />" +
                (QString)"<subitem att1=\"sub33\" att2=\"sub44\" />" +
              (QString)"</dataitems>" +
              (QString)"</msg>";
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    m_RxCommand = new RxCommand(attrs, dattrs);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), false);
    delete m_RxCommand;

    testMsg = (QString)"<msg>" +
              (QString)"<cmd name=\"TestCommand\" ref=\"123\" nofitems=\"2\" />" +
              (QString)"<dataitems test1=\"TEST1\" test2=\"TEST2\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub1\" att2=\"sub2\" />" +
                (QString)"<subitem att1=\"sub11\" att2=\"sub22\" />" +
              (QString)"</dataitems>" +
              //nofitems is missing now: (QString)"<dataitems test1=\"TEST3\" test2=\"TEST4\" nofitems=\"2\">" +
              (QString)"<dataitems test1=\"TEST3\" test2=\"TEST4\" >" +
                (QString)"<subitem att1=\"sub3\" att2=\"sub4\" />" +
                (QString)"<subitem att1=\"sub33\" att2=\"sub44\" />" +
              (QString)"</dataitems>" +
              (QString)"</msg>";
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    m_RxCommand = new RxCommand(attrs, dattrs);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), false);
    delete m_RxCommand;

    testMsg = (QString)"<msg>" +
              // ref is empty now: (QString)"<cmd name=\"TestCommand\" ref=\"123\" nofitems=\"2\" />" +
              (QString)"<cmd name=\"TestCommand\" ref=\"\" nofitems=\"2\" />" +
              (QString)"<dataitems test1=\"TEST1\" test2=\"TEST2\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub1\" att2=\"sub2\" />" +
                (QString)"<subitem att1=\"sub11\" att2=\"sub22\" />" +
              (QString)"</dataitems>" +
              (QString)"<dataitems test1=\"TEST3\" test2=\"TEST4\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub3\" att2=\"sub4\" />" +
                (QString)"<subitem att1=\"sub33\" att2=\"sub44\" />" +
              (QString)"</dataitems>" +
              (QString)"</msg>";
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    m_RxCommand = new RxCommand(attrs, dattrs);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), false);
    delete m_RxCommand;

    testMsg = (QString)"<msg>" +
              (QString)"<cmd name=\"TestCommand\" ref=\"123\" nofitems=\"2\" />" +
              (QString)"<dataitems test1=\"TEST1\" test2=\"TEST2\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub1\" att2=\"sub2\" />" +
                // attribute is missing now: (QString)"<subitem att1=\"sub11\" att2=\"sub22\" />" +
                (QString)"<subitem att1=\"sub11\" />" +
              (QString)"</dataitems>" +
              (QString)"<dataitems test1=\"TEST3\" test2=\"TEST4\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub3\" att2=\"sub4\" />" +
                (QString)"<subitem att1=\"sub33\" att2=\"sub44\" />" +
              (QString)"</dataitems>" +
              (QString)"</msg>";
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    m_RxCommand = new RxCommand(attrs, dattrs);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), false);
    delete m_RxCommand;

    testMsg = (QString)"<msg>" +
              (QString)"<cmd name=\"TestCommand\" ref=\"123\" nofitems=\"2\" />" +
              (QString)"<dataitems test1=\"TEST1\" test2=\"TEST2\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub1\" att2=\"sub2\" />" +
                (QString)"<subitem att1=\"sub11\" att2=\"sub22\" />" +
              (QString)"</dataitems>" +
              (QString)"<dataitems test1=\"TEST3\" test2=\"TEST4\" nofitems=\"2\">" +
              // attribute is missing now: (QString)"<subitem att1=\"sub3\" att2=\"sub4\" />" +
                (QString)"<subitem att1=\"sub3\" />" +
                (QString)"<subitem att1=\"sub33\" att2=\"sub44\" />" +
              (QString)"</dataitems>" +
              (QString)"</msg>";
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    m_RxCommand = new RxCommand(attrs, dattrs);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), false);
    delete m_RxCommand;

    testMsg = (QString)"<msg>" +
              (QString)"<cmd name=\"TestCommand\" ref=\"123\" nofitems=\"2\" />" +
              (QString)"<dataitems test1=\"TEST1\" test2=\"TEST2\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub1\" att2=\"sub2\" />" +
                // different subitem types: (QString)"<subitem att1=\"sub11\" att2=\"sub22\" />" +
                (QString)"<subitem123 att1=\"sub11\" att2=\"sub22\" />" +
              (QString)"</dataitems>" +
              (QString)"<dataitems test1=\"TEST3\" test2=\"TEST4\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub3\" att2=\"sub4\" />" +
                (QString)"<subitem att1=\"sub33\" att2=\"sub44\" />" +
              (QString)"</dataitems>" +
              (QString)"</msg>";
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    m_RxCommand = new RxCommand(attrs, dattrs);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), false);
    delete m_RxCommand;

    testMsg = (QString)"<msg>" +
              (QString)"<cmd name=\"TestCommand\" ref=\"123\" nofitems=\"2\" />" +
              (QString)"<dataitems test1=\"TEST1\" test2=\"TEST2\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub1\" att2=\"sub2\" />" +
                (QString)"<subitem att1=\"sub11\" att2=\"sub22\" />" +
              (QString)"</dataitems>" +
              (QString)"<dataitems test1=\"TEST3\" test2=\"TEST4\" nofitems=\"2\">" +
              // different subitem types: (QString)"<subitem att1=\"sub3\" att2=\"sub4\" />" +
                (QString)"<subitemABF att1=\"sub3\" att2=\"sub4\" />" +
                (QString)"<subitem att1=\"sub33\" att2=\"sub44\" />" +
              (QString)"</dataitems>" +
              (QString)"</msg>";
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    m_RxCommand = new RxCommand(attrs, dattrs);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), false);
    delete m_RxCommand;

    // clear attribute lists:
    attrs.clear();
    dattrs.clear();
    // construct wrong attribute lists:
    attrs = QStringList() << "test1" << "nofitems";
    dattrs = QStringList() << "att1";
    testMsg = (QString)"<msg>" +
              (QString)"<cmd name=\"TestCommand\" ref=\"123\" nofitems=\"2\" />" +
              (QString)"<dataitems test1=\"TEST1\" test2=\"TEST2\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub1\" att2=\"sub2\" />" +
                (QString)"<subitem att1=\"sub11\" att2=\"sub22\" />" +
              (QString)"</dataitems>" +
              (QString)"<dataitems test1=\"TEST3\" test2=\"TEST4\" nofitems=\"2\">" +
                (QString)"<subitem att1=\"sub3\" att2=\"sub4\" />" +
                (QString)"<subitem att1=\"sub33\" att2=\"sub44\" />" +
              (QString)"</dataitems>" +
              (QString)"</msg>";
    if (!GetMsgAsDomDoc(testMsg, &domD)) {
        QFAIL("TestProtocolRxCommand: cannot convert QString message to QDomDocument !");
        return;
    }
    m_RxCommand = new RxCommand(attrs, dattrs);
    QCOMPARE(m_RxCommand->Initialize((QString)"TestCommand", domD, m_myDevice), false);
    delete m_RxCommand;

}

} // end namespace

QTEST_MAIN(NetworkBase::TestProtocolRxCommand)
