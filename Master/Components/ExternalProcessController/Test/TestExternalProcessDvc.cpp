/****************************************************************************/
/*! \file TestExternalProcessDvc.cpp
 *
 *  \brief Implementation file for class TestExternalProcessDvc.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 07.02.2011
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
#include <ExternalProcessController/Test/TestExternalProcessDvc.h>
#include <Global/Include/SystemPaths.h>

namespace ExternalProcessControl {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestExternalProcessDvc::TestExternalProcessDvc() :
            m_myExtDevice(NULL),
            m_myLoginTimeoutFlag(false)
{
    qRegisterMetaType<DataLogging::DayEventEntry>("DataLogging::EventEntry");
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestExternalProcessDvc::~TestExternalProcessDvc()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcessDvc::initTestCase()
{
    // use AdjustedTime once, otherwise linker will complain:
    Global::AdjustedTime::Instance().SetOffsetSeconds((int)1000);
    // initialize settings path:
    Global::SystemPaths::Instance().SetSettingsPath("../Settings");

    // create test device :
    QString path = Global::SystemPaths::Instance().GetSettingsPath() + EPD_PATH_ADDITION;
    m_myExtDevice = new ExternalProcessDevice(NetworkBase::NSE_TYPE_NORMAL_GUI, EPD_TEST_CLIENT, path, NULL, this);

    if (!QObject::connect(m_myExtDevice, SIGNAL(SigLoginTimeout()), this, SLOT(SlotLoginTimeout()))) {
        QFAIL("TestExternalProcessDvc: cannot connect SigLoginTimeout signal !");
        return;
    }
    QVERIFY(m_myExtDevice->m_myServer == NULL);
    QVERIFY(m_myExtDevice->m_myMessageChecker == NULL);
    // initialize device:
    QCOMPARE(m_myExtDevice->InitializeDeviceWithParameters((QString)"No", (int)0), false);
    // check that internal objects got created:
    QVERIFY(m_myExtDevice->m_myServer == NULL);
    QVERIFY(m_myExtDevice->m_myMessageChecker == NULL);
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcessDvc::init()
{
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcessDvc::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcessDvc::cleanupTestCase()
{
    // cleanup:
    delete m_myExtDevice;
}

/****************************************************************************/
/**
 * \brief Internal check for NULL pointer.
 */
/****************************************************************************/
void TestExternalProcessDvc::CheckForNull()
{
    if (m_myExtDevice == NULL) {
        QFAIL("TestExternalProcessDvc: one of the internal pointers is NULL !");
    }
}

/****************************************************************************/
/**
 * \brief Test the few working functions.
 */
/****************************************************************************/
void TestExternalProcessDvc::utTestWorkingFunctions()
{
    CheckForNull();

    // check timer functions:
    m_myExtDevice->StartLoginGuard();
    QVERIFY(m_myExtDevice->m_LoginTimer.isActive() == true);
    m_myExtDevice->StopLoginGuard();
    QVERIFY(m_myExtDevice->m_LoginTimer.isActive() == false);

    // check timeout signal:
    m_myExtDevice->StartLoginGuard();
    QVERIFY(m_myExtDevice->m_LoginTimer.isActive() == true);
    m_myExtDevice->LoginTimedout();
    QVERIFY(m_myExtDevice->m_LoginTimer.isActive() == false);
    // give process time to get to retry state:
    //QTest::qWait(100);
    QVERIFY(m_myLoginTimeoutFlag == true);
    // reset changed flag:
    m_myLoginTimeoutFlag = false;

    // start device operation:
    QCOMPARE(m_myExtDevice->StartDevice(), false);
    QVERIFY(m_myExtDevice->m_LoginTimer.isActive() == false);
    m_myExtDevice->PeerConnected((QString)"Dummy");
    // peer connected -> timer shall be stopped:
    QVERIFY(m_myExtDevice->m_LoginTimer.isActive() == false);
    QString msg = "<msg><cmd name=\"Test\" ref=\"123456\" ><dataitems test=\"2\" /></msg>";
    QVERIFY(m_myExtDevice->SendCommand(msg) == false); // no real connection -> no message sending
    m_myExtDevice->HandleNetworkError((NetworkBase::DisconnectType_t)10, (QString)"DummyClient");
    m_myExtDevice->DisconnectPeer();
    m_myExtDevice->DestroyDevice();
    // check that we killed the server:
    QVERIFY(m_myExtDevice->m_myServer == NULL);
}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestExternalProcessDvc::utTestBadInputHandling()
{
    // destroy the device:
    delete m_myExtDevice;

    // create test device with wrong path:
    QString path = "Some Bad Path";
    m_myExtDevice = new ExternalProcessDevice(NetworkBase::NSE_TYPE_NORMAL_GUI, EPD_TEST_CLIENT, path, NULL, this);
    if (!QObject::connect(m_myExtDevice, SIGNAL(SigLoginTimeout()), this, SLOT(SlotLoginTimeout()))) {
        QFAIL("TestExternalProcessDvc: cannot connect SigLoginTimeout signal !");
        return;
    }
    // initialize device:
    QCOMPARE(m_myExtDevice->InitializeDeviceWithParameters((QString)"No", (int)0), false);

    // run all these functions just to make sure they do not blow anything up:
    QCOMPARE(m_myExtDevice->StartDevice(), false);
    QCOMPARE(m_myExtDevice->SendCommand((QString)"BadCommand"), false);
    QString msg = "<msg><cmd name=\"Test\" ref=\"123456\" ><dataitems test=\"2\" /></msg>";
    QVERIFY(m_myExtDevice->SendCommand(msg) == false);
    m_myExtDevice->StartLoginGuard();
    m_myExtDevice->StopLoginGuard();
    m_myExtDevice->HandleNetworkError((NetworkBase::DisconnectType_t)10, (QString)"DummyClient");
    m_myExtDevice->LoginTimedout();
    m_myExtDevice->PeerConnected((QString)"Dummy");
    m_myExtDevice->HandleHeartBeat();
    m_myExtDevice->DisconnectPeer();
    m_myExtDevice->PeerDisconnected((QString)"Dummy");
    m_myExtDevice->DestroyDevice();
}

/****************************************************************************/
/**
 * \brief Test slot.
 */
/****************************************************************************/
void TestExternalProcessDvc::SlotLoginTimeout()
{
    m_myLoginTimeoutFlag = true;
}

} // end namespace ExternalProcessControl

QTEST_MAIN(ExternalProcessControl::TestExternalProcessDvc)
