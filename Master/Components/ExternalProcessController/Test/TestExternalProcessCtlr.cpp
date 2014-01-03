/****************************************************************************/
/*! \file TestExternalProcessCtlr.cpp
 *
 *  \brief Implementation file for class TestExternalProcessCtlr.
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
#include <ExternalProcessController/Test/TestExternalProcessCtlr.h>
#include <DataLogging/Include/DayEventEntry.h>
#include <Global/Include/SystemPaths.h>

#include <ExternalProcessController/Include/ExternalProcessStates/Initial.h>
#include <ExternalProcessController/Include/ExternalProcessStates/ExtProcessStartRetry.h>
#include <ExternalProcessController/Include/ExternalProcessStates/FatalError.h>
#include <ExternalProcessController/Include/ExternalProcessStates/Working.h>
#include <ExternalProcessController/Include/ExternalProcessStates/CommunicationRetry.h>
#include <ExternalProcessController/Include/ExternalProcessStates/Final.h>

namespace ExternalProcessControl {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestExternalProcessCtlr::TestExternalProcessCtlr() :
            m_myExtProcessCtlr(NULL),
            m_myExtDevice(NULL),
            m_myExtProcessPID(0),
            m_myProcessExitedString(""),
            m_myProcessExitedCode(-5),
            m_myProcessStartedString(""),
            m_myProcessErrorCode(-5)
{
    qRegisterMetaType<DataLogging::DayEventEntry>("DataLogging::EventEntry");
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestExternalProcessCtlr::~TestExternalProcessCtlr()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcessCtlr::initTestCase()
{
    // use AdjustedTime once, otherwise linker will complain:
    Global::AdjustedTime::Instance().SetOffsetSeconds((int)1000);
    // initialize settings path:
    Global::SystemPaths::Instance().SetSettingsPath("../Settings");

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
    m_myExtDevice = new ExternalProcessDevice(NetworkBase::NSE_TYPE_NORMAL_GUI, EPC_TEST_CLIENT, path, m_myExtProcessCtlr, this);
    // check device registration:
    try {
        m_myExtProcessCtlr->RegisterExternalProcessDevice(m_myExtDevice);
    } catch(...) {
        QFAIL("You should never get here!");
    }

    // run initialization:
    try {
        m_myExtProcessCtlr->CreateAndInitializeObjects();
    }catch(...) {
        QFAIL("You should never get here!");
    }
    // check pointers for not being NULL:
    QVERIFY(m_myExtProcessCtlr->m_myDevice != NULL);
    //QVERIFY(m_myExtProcessCtlr->m_myProcess != NULL);
    QVERIFY(m_myExtProcessCtlr->m_myStateManager != NULL);
    QVERIFY(m_myExtProcessCtlr->m_InitialState != NULL);
    QVERIFY(m_myExtProcessCtlr->m_ExtProcessStartRetryState != NULL);
    QVERIFY(m_myExtProcessCtlr->m_FatalErrorState != NULL);
    QVERIFY(m_myExtProcessCtlr->m_WorkingState != NULL);
    QVERIFY(m_myExtProcessCtlr->m_CommunicationRetryState != NULL);
    QVERIFY(m_myExtProcessCtlr->m_FinalState != NULL);
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcessCtlr::init()
{
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcessCtlr::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestExternalProcessCtlr::cleanupTestCase()
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
void TestExternalProcessCtlr::CheckForNull()
{
    if ((m_myExtProcessCtlr == NULL) ||
        (m_myExtDevice == NULL)) {
        QFAIL("TestExternalProcessCtlr: one of the internal pointers is NULL !");
    }
}

/****************************************************************************/
/**
 * \brief Test the few working functions.
 */
/****************************************************************************/
void TestExternalProcessCtlr::utTestWorkingFunctions()
{
    CheckForNull();
    // simulate connected process before state machine is even started:
    m_myExtProcessCtlr->ExternalProcessConnected(EPC_TEST_CLIENT);
    // check that we stay in NULL state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)NULL);
    // set state and try to process events before state machine is started:
    m_myExtProcessCtlr->m_myStateManager->m_myCurrentState = m_myExtProcessCtlr->m_InitialState;
    m_myExtProcessCtlr->ExternalProcessConnected(EPC_TEST_CLIENT);
    // check that we stay in m_InitialState:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_InitialState);
    // get back to NULL state:
    m_myExtProcessCtlr->m_myStateManager->m_myCurrentState = (StateMachines::State*)NULL;

    // start operation:
    m_myExtProcessCtlr->OnGoReceived();
    // check: current state shall be Initial:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_InitialState);
    // give external process time to start:
    QTest::qWait(2000);

    // simulate connected process:
    m_myExtProcessCtlr->ExternalProcessConnected(EPC_TEST_CLIENT);
    // check if we switched to Working State:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WorkingState);

    // simulate disconnected process:
    m_myExtProcessCtlr->ExternalProcessDisconnected(EPC_TEST_CLIENT);
    QTest::qWait(1500);
    // check if we switched to CommRetry State:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WorkingState);
    // simulate login timeout:
    m_myExtProcessCtlr->LoginTimeout();
    // CommunicationRetryState will kill extprocess now, give process time to die:
    QTest::qWait(1500);
    // check if we switched to start retry State:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_CommunicationRetryState);
    // retry state time shall be started here:
    //QCOMPARE(m_myExtProcessCtlr->m_ExtProcessStartRetryState>m_myTimer.isActive(), true);

    // simulate connected process again:
    m_myExtProcessCtlr->ExternalProcessConnected(EPC_TEST_CLIENT);
    // check that we got back to the working state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WorkingState);

    // simulate disconnected process again:
    m_myExtProcessCtlr->ExternalProcessDisconnected(EPC_TEST_CLIENT);
    // check if we switched to CommRetry State:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_WorkingState);
    m_myExtProcessCtlr->ExternalProcessExited(EPC_TEST_CLIENT, (int)2);
    // give process time to get to fatal error state:
    QTest::qWait(200);
    // check if we switched to fatal error State:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_ExtProcessStartRetryState);
    // retry state time shall be stopped here:
    //QCOMPARE(m_myExtProcessCtlr->m_ExtProcessStartRetryState->m_myTimer.isActive(), false);

    // set current state to m_ExtProcessStartRetryState and check that this states handles internal events correctly:
    m_myExtProcessCtlr->m_myStateManager->SetState(m_myExtProcessCtlr->m_ExtProcessStartRetryState->GetName(), \
                                                   (StateMachines::StateEventIndexType_t)EP_EXTPROCESS_DISCONNECTED);
    // check if we switched to retry State:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_ExtProcessStartRetryState);
    // pass EP_EXTPROCESS_LOGIN_TIMEOUT event to m_ExtProcessStartRetryState:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->DispatchEvent((StateMachines::StateEventIndexType_t)EP_EXTPROCESS_LOGIN_TIMEOUT), true);
    // check that stay in m_ExtProcessStartRetryState:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FatalErrorState);
    // pass EP_EXTPROCESS_EXITED event to m_ExtProcessStartRetryState state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->DispatchEvent((StateMachines::StateEventIndexType_t)EP_EXTPROCESS_EXITED), true);
    // retry state time shall be started here:
    //QCOMPARE(m_myExtProcessCtlr->m_ExtProcessStartRetryState->m_myTimer.isActive(), true);
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->DispatchEvent((StateMachines::StateEventIndexType_t)EP_EXTPROCESS_EXITED), true);
    // give process time to get to fatal error state:
    QTest::qWait(200);
    // check if we switched to fatal error State:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FatalErrorState);
    // retry state time shall be stopped here:
    //QCOMPARE(m_myExtProcessCtlr->m_ExtProcessStartRetryState->m_myTimer.isActive(), false);

    // set current state to m_ExtProcessStartRetryState and check that this states handles internal events correctly:
    m_myExtProcessCtlr->m_myStateManager->SetState(m_myExtProcessCtlr->m_ExtProcessStartRetryState->GetName(), \
                                                   (StateMachines::StateEventIndexType_t)EP_EXTPROCESS_DISCONNECTED);
    // check if we switched to retry State:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_ExtProcessStartRetryState);
    // pass EP_CANNOT_START_EXTPROCESS event to m_ExtProcessStartRetryState state:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->DispatchEvent((StateMachines::StateEventIndexType_t)EP_CANNOT_START_EXTPROCESS), true);
    // give process time to get to fatal error state:
    QTest::qWait(200);
    // check if we switched to fatal error State:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FatalErrorState);

    // check how m_InitialState handles EP_EXTPROCESS_LOGIN_TIMEOUT event:
    m_myExtProcessCtlr->m_InitialState->HandleEvent(((StateMachines::StateEventIndexType_t)EP_EXTPROCESS_LOGIN_TIMEOUT));
    // check if we switched to fatal error State:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FatalErrorState);

    // check that state engine is not stopped:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->m_EngineStopped, false);
    // power down:
    m_myExtProcessCtlr->OnStopReceived();
    // check that state machine is stopped:
    QCOMPARE(m_myExtProcessCtlr->m_myStateManager->m_EngineStopped, false);
    // check that server is deleted:
    QCOMPARE(m_myExtProcessCtlr->m_myDevice->m_myServer, (NetworkBase::NetworkServer*)NULL);
}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestExternalProcessCtlr::utTestBadInputHandling()
{
    try {
        // delete the device:
        delete m_myExtDevice;
        // set wrong settings path:
        Global::SystemPaths::Instance().SetSettingsPath("../WrongFolderPath123");
        // create device with the wrong path
        QString path = Global::SystemPaths::Instance().GetSettingsPath();
        m_myExtDevice = new ExternalProcessDevice(NetworkBase::NSE_TYPE_NORMAL_GUI, EPC_TEST_CLIENT, path, m_myExtProcessCtlr, this);
        // check device registration:
        try {
            m_myExtProcessCtlr->RegisterExternalProcessDevice(m_myExtDevice);
        } catch(...) {
            QFAIL("You should never get here!");
        }
        // run and fail initialization:
        try {
            m_myExtProcessCtlr->CreateAndInitializeObjects();
        }catch(...){
            //QFAIL("You should never get here!");
        }

        // try to start and fail due to failed init:
        m_myExtProcessCtlr->OnGoReceived();
        // give process time to get to fatalerror state:
        QTest::qWait(100);
        QCOMPARE(m_myExtProcessCtlr->m_myStateManager->GetCurrentState(), (StateMachines::State*)m_myExtProcessCtlr->m_FatalErrorState);

        // NULL all state controller pointers:
        m_myExtProcessCtlr->m_CommunicationRetryState->m_myController = NULL;
        m_myExtProcessCtlr->m_ExtProcessStartRetryState->m_myController = NULL;
        m_myExtProcessCtlr->m_FatalErrorState->m_myController = NULL;
        m_myExtProcessCtlr->m_FinalState->m_myController = NULL;
        m_myExtProcessCtlr->m_InitialState->m_myController = NULL;
        m_myExtProcessCtlr->m_WorkingState->m_myController = NULL;
        // run these state functions just to make sure they do not blow anything up:
        m_myExtProcessCtlr->m_CommunicationRetryState->OnEntry((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_CommunicationRetryState->OnExit((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_CommunicationRetryState->HandleEvent((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_ExtProcessStartRetryState->OnEntry((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_ExtProcessStartRetryState->OnExit((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_ExtProcessStartRetryState->HandleEvent((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_FatalErrorState->OnEntry((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_FatalErrorState->OnExit((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_FatalErrorState->HandleEvent((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_FinalState->OnEntry((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_FinalState->OnExit((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_FinalState->HandleEvent((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_InitialState->OnEntry((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_InitialState->OnExit((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_InitialState->HandleEvent((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_WorkingState->OnEntry((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_WorkingState->OnExit((ExternalProcessStateEvents_t)255);
        m_myExtProcessCtlr->m_WorkingState->HandleEvent((ExternalProcessStateEvents_t)255);

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
        try {
            m_myExtProcessCtlr->CreateAndInitializeObjects();
        }catch(...){
            //QFAIL("You should never get here!");
        }
        DataLogging::DayEventEntry ee;
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
    catch(...) {

    }
}

} // end namespace ExternalProcessControl

QTEST_MAIN(ExternalProcessControl::TestExternalProcessCtlr)
