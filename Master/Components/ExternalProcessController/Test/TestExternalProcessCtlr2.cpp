/****************************************************************************/
/*! \file TestExternalProcessCtlr2.cpp
 *
 *  \brief Implementation file for class TestExternalProcessCtlr2.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 21.02.2011
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
#include <ExternalProcessController/Test/TestExternalProcessCtlr2.h>
#include <DataLogging/Include/EventEntry.h>
#include <Global/Include/SystemPaths.h>

#include <ExternalProcessController/Include/ExternalProcessStates/WaitState.h>
#include <ExternalProcessController/Include/ExternalProcessStates/FatalError.h>
#include <ExternalProcessController/Include/ExternalProcessStates/Working.h>
#include <ExternalProcessController/Include/ExternalProcessStates/Final.h>

namespace ExternalProcessControl {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestExternalProcessCtlr2::TestExternalProcessCtlr2() :
            m_myExtProcessCtlr(NULL),
            m_myExtDevice(NULL),
            m_myExtProcessPID(0),
            m_myProcessExitedString(""),
            m_myProcessExitedCode(-5),
            m_myProcessStartedString(""),
            m_myProcessErrorCode(-5)
{
    qRegisterMetaType<DataLogging::EventEntry>("DataLogging::EventEntry");
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestExternalProcessCtlr2::~TestExternalProcessCtlr2()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcessCtlr2::initTestCase()
{
    // use AdjustedTime once, otherwise linker will complain:
    Global::AdjustedTime::Instance().SetOffsetSeconds((int)1000);
    // initialize settings path:
    // point to settings with remote login enabled:
    Global::SystemPaths::Instance().SetSettingsPath("../Settings/Settings2");

    // create test controller :
    m_myExtProcessCtlr = new DerivedController((Global::gSourceType)1224);
    QVERIFY(m_myExtProcessCtlr->m_myDevice == NULL);
    QVERIFY(m_myExtProcessCtlr->m_myProcess == NULL);
    QVERIFY(m_myExtProcessCtlr->m_myStateManager == NULL);
    QVERIFY(m_myExtProcessCtlr->m_InitialState == NULL);
    QVERIFY(m_myExtProcessCtlr->m_ExtProcessStartRetryState == NULL);
    QVERIFY(m_myExtProcessCtlr->m_FatalErrorState == NULL);
    QVERIFY(m_myExtProcessCtlr->m_WorkingState == NULL);
    QVERIFY(m_myExtProcessCtlr->m_CommunicationRetryState == NULL);
    QVERIFY(m_myExtProcessCtlr->m_FinalState == NULL);

    // create test device :
    QString path = Global::SystemPaths::Instance().GetSettingsPath() + EPC_PATH_ADDITION;
    m_myExtDevice = new ExternalProcessDevice(NetworkBase::NSE_TYPE_NORMAL_GUI, EPC_TEST_CLIENT, path, EPC_CONFIGFILE_TYPE, this);
    // check device registration:
    try {
        m_myExtProcessCtlr->RegisterExternalProcessDevice(m_myExtDevice);
    } catch(...) {
        QFAIL("You should never get here!");
    }
    // run initialization:
    try {
        m_myExtProcessCtlr->CreateAndInitializeObjects();
    }catch(...){
        QFAIL("You should never get here!");
    }
    // check pointers for not being NULL:
    QVERIFY(m_myExtProcessCtlr->m_myDevice != NULL);
    QVERIFY(m_myExtProcessCtlr->m_myStateManager != NULL);
    QVERIFY(m_myExtProcessCtlr->m_WaitState != NULL);
    QVERIFY(m_myExtProcessCtlr->m_FatalErrorState != NULL);
    QVERIFY(m_myExtProcessCtlr->m_WorkingState != NULL);
    QVERIFY(m_myExtProcessCtlr->m_FinalState != NULL);
    // this one is not created when remote login is enabled in settings:
    QVERIFY(m_myExtProcessCtlr->m_myProcess == NULL);
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcessCtlr2::init()
{
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcessCtlr2::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcessCtlr2::cleanupTestCase()
{
    // cleanup:
    delete m_myExtProcessCtlr;
    delete m_myExtDevice;
}

/****************************************************************************/
/**
 * \brief Internal check for NULL pointer.
 */
/****************************************************************************/
void TestExternalProcessCtlr2::CheckForNull()
{
    if ((m_myExtProcessCtlr == NULL) ||
        (m_myExtDevice == NULL)) {
        QFAIL("TestExternalProcessCtlr2: one of the internal pointers is NULL !");
    }
}

/****************************************************************************/
/**
 * \brief Test the few working functions.
 */
/****************************************************************************/
void TestExternalProcessCtlr2::utTestWorkingFunctions()
{
    CheckForNull();

    // simulate connected process before state machine is even started:
    m_myExtProcessCtlr->ExternalProcessConnected(EPC_TEST_CLIENT);
    // check that we stay in NULL state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)NULL);
    // set state and try to process events before state machine is started:
    m_myExtProcessCtlr->m_myStateManager->m_myCurrentState = m_myExtProcessCtlr->m_WaitState;
    m_myExtProcessCtlr->ExternalProcessConnected(EPC_TEST_CLIENT);
    // check that we stay in m_WaitState:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WaitState);

    // start operation:
    m_myExtProcessCtlr->OnGoReceived();
    // check: current state shall be Wait:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WaitState);

    // simulate connected process:
    m_myExtProcessCtlr->ExternalProcessConnected(EPC_TEST_CLIENT);
    // check if we switched to Working State:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WorkingState);

    // simulate disconnected process:
    m_myExtProcessCtlr->ExternalProcessDisconnected(EPC_TEST_CLIENT);
    // check if we switched to m_WaitState:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WaitState);
    // retry guard timer shall be started here:
    QCOMPARE(m_myExtProcessCtlr->m_WaitState->m_myTimer.isActive(), true);
    // simulate connected process again:
    m_myExtProcessCtlr->ExternalProcessConnected(EPC_TEST_CLIENT);
    // check that we got back to the working state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WorkingState);

    // simulate disconnected process:
    m_myExtProcessCtlr->ExternalProcessDisconnected(EPC_TEST_CLIENT);
    // check if we switched to m_WaitState:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WaitState);
    // check that retry guard timer still runs:
    QCOMPARE(m_myExtProcessCtlr->m_WaitState->m_myTimer.isActive(), true);

    // simulate connected process again:
    m_myExtProcessCtlr->ExternalProcessConnected(EPC_TEST_CLIENT);
    // check that we got back to the working state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WorkingState);

    // simulate disconnected process:
    m_myExtProcessCtlr->ExternalProcessDisconnected(EPC_TEST_CLIENT);
    // check that we switched to fatal error state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FatalErrorState);
    // check that retry guard timer is still running:
    QCOMPARE(m_myExtProcessCtlr->m_WaitState->m_myTimer.isActive(), true);

    // simulate connected process again:
    m_myExtProcessCtlr->ExternalProcessConnected(EPC_TEST_CLIENT);
    // check that we stay in fatal error state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FatalErrorState);
    // simulate disconnected process again:
    m_myExtProcessCtlr->ExternalProcessDisconnected(EPC_TEST_CLIENT);
    // check that we stay in fatal error state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FatalErrorState);

    // set waiting state:
    m_myExtProcessCtlr->m_myStateManager->m_myCurrentState = m_myExtProcessCtlr->m_WaitState;
    // check:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WaitState);
    // trigger timeout:
    m_myExtProcessCtlr->LoginTimeout();
    // check that we switched to fatal error state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FatalErrorState);

    // set working state:
    m_myExtProcessCtlr->m_myStateManager->m_myCurrentState = m_myExtProcessCtlr->m_WorkingState;
    // check:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WorkingState);
    // simulate power down event:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->DispatchEvent((StateMachines::StateEventIndexType_t)EP_POWERDOWN), true);
    // check that we switched to final state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FinalState);

    // set waiting state:
    m_myExtProcessCtlr->m_myStateManager->m_myCurrentState = m_myExtProcessCtlr->m_WaitState;
    // check:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WaitState);
    // simulate power down event:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->DispatchEvent((StateMachines::StateEventIndexType_t)EP_POWERDOWN), true);
    // check that we switched to final state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FinalState);

    // simulate connected process:
    m_myExtProcessCtlr->ExternalProcessConnected(EPC_TEST_CLIENT);
    // check that we stay in final state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FinalState);
    // simulate disconnected process again:
    m_myExtProcessCtlr->ExternalProcessDisconnected(EPC_TEST_CLIENT);
    // check that we stay in final state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FinalState);

    // check that state engine is not stopped:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->m_EngineStopped, false);
    // power down:
    m_myExtProcessCtlr->OnStopReceived();
    // check that state machine is stopped:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->m_EngineStopped, true);
    // check that server is deleted:
    QCOMPARE(m_myExtProcessCtlr->m_myDevice->m_myServer, (NetworkBase::NetworkServer*)NULL);
}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestExternalProcessCtlr2::utTestBadInputHandling()
{
    // delete the device:
    delete m_myExtDevice;
    // set wrong settings path:
    Global::SystemPaths::Instance().SetSettingsPath("../WrongFolderPath123");
    // create device with the wrong path
    QString path = Global::SystemPaths::Instance().GetSettingsPath();
    m_myExtDevice = new ExternalProcessDevice(NetworkBase::NSE_TYPE_NORMAL_GUI, EPC_TEST_CLIENT, path, EPC_CONFIGFILE_TYPE, this);
    // check device registration:
    try {
        m_myExtProcessCtlr->RegisterExternalProcessDevice(m_myExtDevice);
    } catch(...) {
        QFAIL("You should never get here!");
    }
    // run and fail initialization:
    try {
        m_myExtProcessCtlr->CreateAndInitializeObjects();
        QFAIL("You should never get here!");
    }catch(...){
    }
    // try to start and fail due to failed init:
    m_myExtProcessCtlr->OnGoReceived();
    // give process time to get to fatalerror state:
    QTest::qWait(100);
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FatalErrorState);

    // NULL all state controller pointers:
    m_myExtProcessCtlr->m_WaitState->m_myController = NULL;

    // run these state functions just to make sure they do not blow anything up:
    m_myExtProcessCtlr->m_WaitState->OnEntry((ExternalProcessStateEvents_t)255);
    m_myExtProcessCtlr->m_WaitState->OnExit((ExternalProcessStateEvents_t)255);
    m_myExtProcessCtlr->m_WaitState->HandleEvent((ExternalProcessStateEvents_t)255);

    // make sure that all member pointers == NULL:
    delete m_myExtProcessCtlr;
    m_myExtProcessCtlr = new DerivedController((Global::gSourceType)1224);
    QVERIFY(m_myExtProcessCtlr->m_myDevice == NULL);
    QVERIFY(m_myExtProcessCtlr->m_myProcess == NULL);
    QVERIFY(m_myExtProcessCtlr->m_myStateManager == NULL);

    // try all kind of inits which should fail if member pointers == NULL:
    try {
        m_myExtProcessCtlr->RegisterExternalProcessDevice(NULL);
        QFAIL("You should never get here!");
    } catch(...) {
    }
    //QCOMPARE(m_myExtProcessCtlr->InitializeExternalProcessDevice(), false);
    try{
        m_myExtProcessCtlr->CreateAndInitializeObjects();
        QFAIL("You should never get here!");
    }catch(...){
    }
    DataLogging::EventEntry ee;
    QCOMPARE(m_myExtProcessCtlr->LocalProcessErrorEvent(ee), false);
    // just make sure these functions do not blow anything up:
    m_myExtProcessCtlr->ExternalProcessExited((QString)"Dummy", (int)1);
    m_myExtProcessCtlr->ExternalProcessStarted((QString)"Dummy");
    m_myExtProcessCtlr->ExternalProcessError((int)2);
    m_myExtProcessCtlr->ExternalProcessConnected((QString)"Dummy");
    m_myExtProcessCtlr->ExternalProcessDisconnected((QString)"Dummy");
    m_myExtProcessCtlr->LoginTimeout();
    m_myExtProcessCtlr->OnGoReceived();
    m_myExtProcessCtlr->OnStopReceived();
    m_myExtProcessCtlr->CleanupAndDestroyObjects();
}

} // end namespace ExternalProcessControl

QTEST_MAIN(ExternalProcessControl::TestExternalProcessCtlr2)
