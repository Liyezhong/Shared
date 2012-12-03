/****************************************************************************/
/*! \file TestStateEngine.cpp
 *
 *  \brief Implementation file for class TestStateEngine.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 05.02.2011
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
#include <StateMachines/Test/TestStateEngine.h>
#include <StateMachines/Include/StateMachinesEventCodes.h>
#include <Global/Include/Exception.h>

namespace StateMachines {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestStateEngine::TestStateEngine() :
            m_ObjA(NULL),
            m_ObjB(NULL),
            m_ObjC(NULL),
            m_StateA(NULL),
            m_StateB(NULL),
            m_StateC(NULL),
            m_myStateEngine(NULL)
{
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestStateEngine::~TestStateEngine()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestStateEngine::initTestCase()
{
    try {
        // create states:
        m_ObjA = new QObject(NULL);
        m_StateA = new DerivedState((StateNameType_t)"m_StateA", m_ObjA);
        m_ObjB = new QObject(NULL);
        m_StateB = new DerivedState((StateNameType_t)"m_StateB", m_ObjB);
        m_ObjC = new QObject(NULL);
        m_StateC = new DerivedState((StateNameType_t)"m_StateC", m_ObjC);
        // create engine:
        m_myStateEngine = new StateMachineEngine(NULL);

        // verify that current state is not selected:
        QVERIFY(m_myStateEngine->m_myCurrentState == NULL);
        // check that engine is stopped per default:
        QCOMPARE(m_myStateEngine->m_EngineStopped, true);
        // verify that there are no existing states:
        QVERIFY(m_myStateEngine->StatesList.isEmpty() == true);
        // insert some states:
        m_myStateEngine->AddState(m_StateA);
        m_myStateEngine->AddState(m_StateB);
        m_myStateEngine->AddState(m_StateC);

        // verify that states list is not empty anymore:
        QVERIFY(m_myStateEngine->StatesList.isEmpty() == false);

        // verify that there are no existing trasnsitions
        QVERIFY(m_StateA->m_myTransitions.isEmpty() == true);
        QVERIFY(m_StateB->m_myTransitions.isEmpty() == true);
        QVERIFY(m_StateC->m_myTransitions.isEmpty() == true);

        // add transitions between states
        m_myStateEngine->AddTransition(m_StateA, m_StateB, (StateEventIndexType_t)10);
        m_myStateEngine->AddTransition(m_StateA, m_StateB, (StateEventIndexType_t)11);
        m_myStateEngine->AddTransition(m_StateA, m_StateC, (StateEventIndexType_t)12);
        m_myStateEngine->AddTransition(m_StateB, m_StateC, (StateEventIndexType_t)20);
        m_myStateEngine->AddTransition(m_StateB, m_StateC, (StateEventIndexType_t)21);
        m_myStateEngine->AddTransition(m_StateB, m_StateA, (StateEventIndexType_t)22);
        m_myStateEngine->AddTransition(m_StateC, m_StateA, (StateEventIndexType_t)30);
        m_myStateEngine->AddTransition(m_StateC, m_StateA, (StateEventIndexType_t)31);
        m_myStateEngine->AddTransition(m_StateC, m_StateB, (StateEventIndexType_t)32);
        m_myStateEngine->AddTransition(NULL, m_StateC, (StateEventIndexType_t)777); // all-to-m_StateC transition

        // verify that there are some existing trasnsitions
        QVERIFY(m_StateA->m_myTransitions.isEmpty() == false);
        QVERIFY(m_StateB->m_myTransitions.isEmpty() == false);
        QVERIFY(m_StateC->m_myTransitions.isEmpty() == false);
    } catch(...) {
        QFAIL("You should never get here...");
    }
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestStateEngine::init()
{
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestStateEngine::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestStateEngine::cleanupTestCase()
{
    // cleanup:
    delete m_myStateEngine;
    delete m_StateA;
    delete m_StateB;
    delete m_StateC;
    delete m_ObjA;
    delete m_ObjB;
    delete m_ObjC;
}

/****************************************************************************/
/**
 * \brief Internal check for NULL pointer.
 */
/****************************************************************************/
void TestStateEngine::CheckForNull()
{
    if ((m_StateA == NULL) ||
        (m_StateB == NULL) ||
        (m_StateC == NULL) ||
        (m_myStateEngine == NULL)) {
        QFAIL("TestStateEngine: one of the internal pointers is NULL !");
    }
}

/****************************************************************************/
/**
 * \brief Test transitions from state to state.
 */
/****************************************************************************/
void TestStateEngine::utTestTransitions()
{
    CheckForNull();

    // state machine ready: go!
    QVERIFY(m_StateA->m_myEntryFlag == false);
    QVERIFY(m_StateA->m_myEntryEvent.GetIndex() == 0);
    QCOMPARE(m_myStateEngine->Start((StateNameType_t)"m_StateA", (StateEventIndexType_t)158), true);
    // check all consequences:
    QCOMPARE(m_myStateEngine->m_myCurrentState->GetName(), (StateNameType_t)"m_StateA");
    QCOMPARE(m_myStateEngine->GetCurrentState(), m_StateA);
    QCOMPARE(m_myStateEngine->m_EngineStopped, false);
    QVERIFY(m_StateA->m_myEntryFlag == true);
    QCOMPARE(m_StateA->m_myEntryEvent.GetIndex(), (StateEventIndexType_t)158);
    // reset changed members:
    m_StateA->m_myEntryFlag = false;
    m_StateA->m_myEntryEvent.SetIndex(0);

    // test transition to B:
    QVERIFY(m_StateB->m_myEntryFlag == false);
    QVERIFY(m_StateB->m_myEntryEvent.GetIndex() == 0);
    QCOMPARE(m_myStateEngine->DispatchEvent((StateEventIndexType_t)10), true);
    QCOMPARE(m_myStateEngine->m_myCurrentState->GetName(), (StateNameType_t)"m_StateB");
    QCOMPARE(m_myStateEngine->GetCurrentState(), m_StateB);
    QVERIFY(m_StateB->m_myEntryFlag == true);
    QCOMPARE(m_StateB->m_myEntryEvent.GetIndex(), (StateEventIndexType_t)10);
    // reset changed members:
    m_StateB->m_myEntryFlag = false;
    m_StateB->m_myEntryEvent.SetIndex(0);

    // test all-to-m_StateC transition:
    QVERIFY(m_StateC->m_myEntryFlag == false);
    QVERIFY(m_StateC->m_myEntryEvent.GetIndex() == 0);
    QCOMPARE(m_myStateEngine->DispatchEvent((StateEventIndexType_t)777), true);
    QCOMPARE(m_myStateEngine->m_myCurrentState->GetName(), (StateNameType_t)"m_StateC");
    QVERIFY(m_StateC->m_myEntryFlag == true);
    QCOMPARE(m_StateC->m_myEntryEvent.GetIndex(), (StateEventIndexType_t)777);
    // reset changed members:
    m_StateC->m_myEntryFlag = false;
    m_StateC->m_myEntryEvent.SetIndex(0);

    // test transition back to A:
    QVERIFY(m_StateA->m_myEntryFlag == false);
    QVERIFY(m_StateA->m_myEntryEvent.GetIndex() == 0);
    QCOMPARE(m_myStateEngine->DispatchEvent((StateEventIndexType_t)31), true);
    QCOMPARE(m_myStateEngine->m_myCurrentState->GetName(), (StateNameType_t)"m_StateA");
    QVERIFY(m_StateA->m_myEntryFlag == true);
    QCOMPARE(m_StateA->m_myEntryEvent.GetIndex(), (StateEventIndexType_t)31);
    // reset changed members:
    m_StateA->m_myEntryFlag = false;
    m_StateA->m_myEntryEvent.SetIndex(0);

    // stop engine:
    QCOMPARE(m_myStateEngine->m_EngineStopped, false);
    m_myStateEngine->Stop();
    // check that engine got stopped:
    QCOMPARE(m_myStateEngine->m_EngineStopped, true);
    // check that we are still in state A:
    QCOMPARE(m_myStateEngine->m_myCurrentState->GetName(), (StateNameType_t)"m_StateA");
    // try to switch from m_StateA to m_StateB:
    QCOMPARE(m_myStateEngine->DispatchEvent((StateEventIndexType_t)10), false);
    // engine is stopped, so we shall stay in state A:
    QCOMPARE(m_myStateEngine->m_myCurrentState->GetName(), (StateNameType_t)"m_StateA");
    // return engine to "running" state:
    QCOMPARE(m_myStateEngine->Start((StateNameType_t)"m_StateA", (StateEventIndexType_t)158), true);
    QCOMPARE(m_myStateEngine->m_EngineStopped, false);
}

/****************************************************************************/
/**
 * \brief Test how state engine handles removal of a state.
 */
/****************************************************************************/
void TestStateEngine::utTestStateRemoval()
{
    CheckForNull();

    // try to remove current state. it shall fail:
    try {
        m_myStateEngine->RemoveState(m_StateA);
        QFAIL("You should never get here...");
    } catch(const Global::Exception &E) {
        QCOMPARE(E.GetErrorCode(), EVENT_STATEMACHINES_ERROR_REMOVE_CURRENT_STATE);
    } catch(...) {
        QFAIL("You should never get here...");
    }
    // m_StateA still shall be the current state:
    QCOMPARE(m_myStateEngine->m_myCurrentState->GetName(), (StateNameType_t)"m_StateA");
    // remove m_StateC and then try to dispatch its events:
    try {
        m_myStateEngine->RemoveState(m_StateC);
    } catch(...) {
        QFAIL("You should never get here...");
    }
    QCOMPARE(m_myStateEngine->DispatchEvent((StateEventIndexType_t)777), true);
    // we shall stay in m_StateA:
    QCOMPARE(m_myStateEngine->m_myCurrentState->GetName(), (StateNameType_t)"m_StateA");
    QVERIFY(m_StateA->m_myHandleEventFlag == true);
    QCOMPARE(m_StateA->m_myHandledEvent.GetIndex(), (StateEventIndexType_t)777);
    m_StateA->m_myHandleEventFlag = false;
    m_StateA->m_myHandledEvent.SetIndex(0);
}

/****************************************************************************/
/**
 * \brief Test how state engine handles insertion of a state.
 */
/****************************************************************************/
void TestStateEngine::utTestStateInsertion()
{
    CheckForNull();

    try {
        m_myStateEngine->AddState(m_StateC);
        m_myStateEngine->AddTransition(m_StateC, m_StateB, (StateEventIndexType_t)88);
        m_myStateEngine->AddTransition(NULL, m_StateC, (StateEventIndexType_t)777); // all-to-m_StateC transition

        QCOMPARE(m_myStateEngine->DispatchEvent((StateEventIndexType_t)777), true);
        // we shall get to m_StateC:
        QCOMPARE(m_myStateEngine->m_myCurrentState->GetName(), (StateNameType_t)"m_StateC");
        QVERIFY(m_StateC->m_myEntryFlag == true);
        QVERIFY(m_StateC->m_myEntryEvent.GetIndex() == 777);
        m_StateC->m_myEntryFlag = false;
        m_StateC->m_myEntryEvent.SetIndex(0);
        // move from m_StateC to m_StateB:
        QCOMPARE(m_myStateEngine->DispatchEvent((StateEventIndexType_t)88), true);
        QCOMPARE(m_myStateEngine->m_myCurrentState->GetName(), (StateNameType_t)"m_StateB");
        QVERIFY(m_StateB->m_myEntryFlag == true);
        QCOMPARE(m_StateB->m_myEntryEvent.GetIndex(), (StateEventIndexType_t)88);
        m_StateB->m_myEntryFlag = false;
        m_StateB->m_myEntryEvent.SetIndex(0);
    } catch(...) {
        QFAIL("You should never get here...");
    }
}

/****************************************************************************/
/**
 * \brief Test how state engine handles transition removal.
 */
/****************************************************************************/
void TestStateEngine::utTestTransitionRemoval()
{
    CheckForNull();

// ====== test SetState and transition removal ===================

    try {
        // set m_StateA again:
        m_myStateEngine->SetState((StateNameType_t)"m_StateA", (StateEventIndexType_t)3456);
        // we shall move to m_StateA:
        QCOMPARE(m_myStateEngine->m_myCurrentState->GetName(), (StateNameType_t)"m_StateA");
        QVERIFY(m_StateA->m_myEntryFlag == true);
        QVERIFY(m_StateA->m_myEntryEvent.GetIndex() == 3456);
        m_StateA->m_myEntryFlag = false;
        m_StateA->m_myEntryEvent.SetIndex(0);
        // now remove the "777" transition:
        m_myStateEngine->RemoveTransition(m_StateA, m_StateC, (StateEventIndexType_t)777);
        // try to switch to m_StateC over event 777. it shall fail and we shall stay in m_StateA:
        QCOMPARE(m_myStateEngine->DispatchEvent((StateEventIndexType_t)777), true);
        QCOMPARE(m_myStateEngine->m_myCurrentState->GetName(), (StateNameType_t)"m_StateA");
        QVERIFY(m_StateA->m_myHandleEventFlag == true);
        QCOMPARE(m_StateA->m_myHandledEvent.GetIndex(), (StateEventIndexType_t)777);
        m_StateA->m_myHandleEventFlag = false;
        m_StateA->m_myHandledEvent.SetIndex(0);
    } catch(...) {
        QFAIL("You should never get here...");
    }

}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestStateEngine::utTestBadInputHandling()
{
    CheckForNull();
    DerivedState *dummy = NULL;

    // stop the engine:
    m_myStateEngine->Stop();
    // trying to add NULL state:
    try {
        m_myStateEngine->AddState(dummy);
        QFAIL("You should never get here...");
    } catch(const Global::Exception &E) {
        QCOMPARE(E.GetErrorCode(), Global::EVENT_GLOBAL_ERROR_NULL_POINTER);
    } catch(...) {
        QFAIL("You should never get here...");
    }

    // trying to remove NULL state:
    try {
        m_myStateEngine->RemoveState(dummy);
        QFAIL("You should never get here...");
    } catch(const Global::Exception &E) {
        QCOMPARE(E.GetErrorCode(), Global::EVENT_GLOBAL_ERROR_NULL_POINTER);
    } catch(...) {
        QFAIL("You should never get here...");
    }

    // trying to add transition to NULL state:
    try {
        m_myStateEngine->AddTransition(dummy, dummy, (StateEventIndexType_t)1234);
        QFAIL("You should never get here...");
    } catch(const Global::Exception &E) {
        QCOMPARE(E.GetErrorCode(), Global::EVENT_GLOBAL_ERROR_NULL_POINTER);
    } catch(...) {
        QFAIL("You should never get here...");
    }
    // trying to remove transition from NULL state:
    try {
        m_myStateEngine->RemoveTransition(dummy, m_StateA, (StateEventIndexType_t)10);
        QFAIL("You should never get here...");
    } catch(const Global::Exception &E) {
        QCOMPARE(E.GetErrorCode(), Global::EVENT_GLOBAL_ERROR_NULL_POINTER);
    } catch(...) {
        QFAIL("You should never get here...");
    }
    // trying to remove transition to NULL state:
    try {
        m_myStateEngine->RemoveTransition(m_StateB, dummy, (StateEventIndexType_t)10);
        QFAIL("You should never get here...");
    } catch(const Global::Exception &E) {
        QCOMPARE(E.GetErrorCode(), Global::EVENT_GLOBAL_ERROR_NULL_POINTER);
    } catch(...) {
        QFAIL("You should never get here...");
    }
    // trying to switch to non-existent state:
    try {
        m_myStateEngine->Start((StateNameType_t)"m_StateABCDEFG", (StateEventIndexType_t)10);
        QFAIL("You should never get here...");
    } catch(const Global::Exception &E) {
        QCOMPARE(E.GetErrorCode(), EVENT_STATEMACHINES_ERROR_UNKNOWN_STATE);
    } catch(...) {
        QFAIL("You should never get here...");
    }
    // get name of the current state:
    StateNameType_t StateName = m_myStateEngine->m_myCurrentState->GetName();
    // zero the current state's pointer in the Hash:
    m_myStateEngine->StatesList.remove(StateName);
    m_myStateEngine->StatesList.insert(StateName, NULL);
    // zero the current state:
    m_myStateEngine->m_myCurrentState = NULL;
    // try to Start with Zero current state and deleted pointer in Hash:
    try {
        m_myStateEngine->Start(StateName, (StateEventIndexType_t)10);
        QFAIL("You should never get here...");
    } catch(const Global::Exception &E) {
        QCOMPARE(E.GetErrorCode(), Global::EVENT_GLOBAL_ERROR_NULL_POINTER);
    } catch(...) {
        QFAIL("You should never get here...");
    }
    // check that engine is still stopped:
    QCOMPARE(m_myStateEngine->m_EngineStopped, true);
    // try to dispatch event with zero current state:
    QCOMPARE(m_myStateEngine->DispatchEvent((StateEventIndexType_t)10), false);
    // try to set the deleted state:
    try {
        m_myStateEngine->SetState(StateName, (StateEventIndexType_t)10);
        QFAIL("You should never get here...");
    } catch(const Global::Exception &E) {
        QCOMPARE(E.GetErrorCode(), Global::EVENT_GLOBAL_ERROR_NULL_POINTER);
    } catch(...) {
        QFAIL("You should never get here...");
    }
    QVERIFY(m_myStateEngine->m_myCurrentState == NULL);
    // try to set an unknown state:
    try {
        m_myStateEngine->SetState((StateNameType_t)"BlaBla88", (StateEventIndexType_t)10);
        QFAIL("You should never get here...");
    } catch(const Global::Exception &E) {
        QCOMPARE(E.GetErrorCode(), EVENT_STATEMACHINES_ERROR_UNKNOWN_STATE);
    } catch(...) {
        QFAIL("You should never get here...");
    }
    QVERIFY(m_myStateEngine->m_myCurrentState == NULL);
    // fake broken state transition list:
    m_StateB->m_myTransitions.insertMulti((StateEventIndexType_t)71, (StateNameType_t)"m_StateA");
    m_StateB->m_myTransitions.insertMulti((StateEventIndexType_t)71, (StateNameType_t)"m_StateC");
    // try to remove the broken transition. It should not fail:
    try {
        m_myStateEngine->RemoveTransition(m_StateB, m_StateC, (StateEventIndexType_t)71);
    } catch(...) {
        QFAIL("You should never get here...");
    }
    // fake broken state list:
    m_myStateEngine->StatesList.insertMulti((StateNameType_t)"m_StateA", m_StateA);
    m_myStateEngine->StatesList.insertMulti((StateNameType_t)"m_StateA", m_StateC);
    // trying to remove faked state. It should not fail:
    try {
        m_myStateEngine->RemoveState(m_StateA);
    } catch(...) {
        QFAIL("You should never get here...");
    }
    // empty the states list:
    m_myStateEngine->StatesList.clear();
    // try to remove state from an empty list. It should not fail:
    try {
        m_myStateEngine->RemoveState(m_StateA);
    } catch(...) {
        QFAIL("You should never get here...");
    }
}

} // end namespace StateMachines

QTEST_MAIN(StateMachines::TestStateEngine)
