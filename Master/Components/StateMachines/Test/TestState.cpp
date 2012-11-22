/****************************************************************************/
/*! \file TestState.cpp
 *
 *  \brief Implementation file for class TestState.
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
#include <QDebug>
#include <Global/Include/Utils.h>
#include <Global/Include/GlobalEventCodes.h>
#include <Global/Include/Exception.h>
#include <StateMachines/Include/StateMachinesEventCodes.h>
#include <StateMachines/Test/TestState.h>

namespace StateMachines {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestState::TestState() :
        m_ObjA(NULL),
        m_ObjB(NULL),
        m_ObjC(NULL),
        m_StateA(NULL),
        m_StateB(NULL),
        m_StateC(NULL),
        m_myState("")
{
    qRegisterMetaType<StateMachines::StateNameType_t>("StateMachines::StateNameType_t");
    qRegisterMetaType<StateMachines::StateEvent>("StateMachines::StateEvent");
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestState::~TestState()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestState::initTestCase()
{
    try {
        m_ObjA = new QObject(NULL);
        m_StateA = new DerivedState("m_StateA", m_ObjA);
        m_ObjB = new QObject(NULL);
        m_StateB = new DerivedState("m_StateB", m_ObjB);
        m_ObjC = new QObject(NULL);
        m_StateC = new DerivedState("m_StateC", m_ObjC);

        // Check names
        QCOMPARE(m_StateA->GetName(), (StateNameType_t)"m_StateA");
        QCOMPARE(m_StateB->GetName(), (StateNameType_t)"m_StateB");
        QCOMPARE(m_StateC->GetName(), (StateNameType_t)"m_StateC");

        // connect the state changing signal
        CONNECTSIGNALSLOT(m_StateA, SigSetNewState(const StateMachines::StateNameType_t &, StateMachines::StateEvent),
                          this, SetState(const StateMachines::StateNameType_t &, StateMachines::StateEvent));
        CONNECTSIGNALSLOT(m_StateB, SigSetNewState(const StateMachines::StateNameType_t &, StateMachines::StateEvent),
                          this, SetState(const StateMachines::StateNameType_t &, StateMachines::StateEvent));
        CONNECTSIGNALSLOT(m_StateC, SigSetNewState(const StateMachines::StateNameType_t &, StateMachines::StateEvent),
                          this, SetState(const StateMachines::StateNameType_t &, StateMachines::StateEvent));

        // verify that there are no existing trasnsitions
        QVERIFY(m_StateA->m_myTransitions.isEmpty());
        QVERIFY(m_StateB->m_myTransitions.isEmpty());
        QVERIFY(m_StateC->m_myTransitions.isEmpty());

        // insert transitions
        // for state A
        m_StateA->AddTransition((StateNameType_t)"m_StateB", (StateEventIndexType_t)10);
        // for state B
        m_StateB->AddTransition((StateNameType_t)"m_StateC", (StateEventIndexType_t)20);
        m_StateB->AddTransition((StateNameType_t)"m_StateA", (StateEventIndexType_t)21);
        m_StateB->AddTransition((StateNameType_t)"m_StateC", (StateEventIndexType_t)22);
        // for state C
        m_StateC->AddTransition((StateNameType_t)"m_StateA", (StateEventIndexType_t)30);
        m_StateC->AddTransition((StateNameType_t)"m_StateB", (StateEventIndexType_t)31);
        m_StateC->AddTransition((StateNameType_t)"m_StateA", (StateEventIndexType_t)32);

        // verify that trasnsitions list is not empty anymore
        QVERIFY(!m_StateA->m_myTransitions.isEmpty());
        QVERIFY(!m_StateB->m_myTransitions.isEmpty());
        QVERIFY(!m_StateC->m_myTransitions.isEmpty());
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
void TestState::init()
{
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestState::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestState::cleanupTestCase()
{
    // cleanup:
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
void TestState::CheckForNull()
{
    if ((m_StateA == NULL) ||
        (m_StateB == NULL) ||
        (m_StateC == NULL)) {
        QFAIL("TestState: one of the test state pointers is NULL !");
    }
}

/****************************************************************************/
/**
 * \brief Test how States process events.
 */
/****************************************************************************/
void TestState::utTestEventProcessing()
{
    CheckForNull();

    // test internal event handling function:
    QVERIFY(m_StateA->m_myHandleEventFlag == false);
    QVERIFY(m_StateA->m_myExitFlag == false);
    QCOMPARE(m_StateA->DispatchEvent((StateEventIndexType_t)111), true); // go!
    QVERIFY(m_StateA->m_myHandleEventFlag == true); // internal event handling function shall be called
    QVERIFY(m_StateA->m_myExitFlag == false); // state shall NOT be exited
    QCOMPARE(m_StateA->m_myHandledEvent.GetIndex(), (StateEventIndexType_t)111); // event index shall match
    // reset the changed members:
    m_StateA->m_myHandleEventFlag = false;
    m_StateA->m_myHandledEvent.SetIndex(0);

    // test state exit condition:
    QCOMPARE(m_myState, (StateNameType_t)"");
    QCOMPARE(m_myEvent.GetIndex(), (StateEventIndexType_t)0);
    QVERIFY(m_StateB->m_myHandleEventFlag == false);
    QVERIFY(m_StateB->m_myExitFlag == false);
    QCOMPARE(m_StateB->DispatchEvent((StateEventIndexType_t)21), true); // go!
    QVERIFY(m_StateB->m_myHandleEventFlag == false); // internal event handling function shall NOT be called
    QVERIFY(m_StateB->m_myExitFlag == true); // state shall be exited
    QCOMPARE(m_StateB->m_myExitEvent.GetIndex(), (StateEventIndexType_t)21); // event index shall match
    // reset the changed members
    m_StateB->m_myExitFlag = false;
    m_StateB->m_myExitEvent.SetIndex(0);
    // check if state change signal was emitted successfully
    QCOMPARE(m_myState, (StateNameType_t)"m_StateA");
    QCOMPARE(m_myEvent.GetIndex(), (StateEventIndexType_t)21);

    // test state exit condition:
    QVERIFY(m_StateC->m_myHandleEventFlag == false);
    QVERIFY(m_StateC->m_myExitFlag == false);
    QCOMPARE(m_StateC->DispatchEvent((StateEventIndexType_t)31), true); // go!
    QVERIFY(m_StateC->m_myHandleEventFlag == false); // internal event handling function shall NOT be called
    QVERIFY(m_StateC->m_myExitFlag == true); // state shall be exited
    QCOMPARE(m_StateC->m_myExitEvent.GetIndex(), (StateEventIndexType_t)31); // event index shall match
    // reset the changed members
    m_StateC->m_myExitFlag = false;
    m_StateC->m_myExitEvent.SetIndex(0);
    // check if state change signal was emitted successfully
    QCOMPARE(m_myState, (StateNameType_t)"m_StateB");
    QCOMPARE(m_myEvent.GetIndex(), (StateEventIndexType_t)31);
}

/****************************************************************************/
/**
 * \brief Test how States handle addition of transitions.
 */
/****************************************************************************/
void TestState::utTestTransitionAdd()
{
    CheckForNull();
    try {
        // add not existing state transition. Should succeed.
        m_StateA->AddTransition((StateNameType_t)"m_StateB", (StateEventIndexType_t)999);
        // add already existing state transition. Should fail.
        try {
            m_StateA->AddTransition((StateNameType_t)"m_StateB", (StateEventIndexType_t)999);
            QFAIL("You should never get here...");
        } catch(const Global::Exception &E) {
            QCOMPARE(E.GetErrorCode(), EVENT_STATEMACHINES_ERROR_TRANSITION_EXISTS);
        } catch(...) {
            QFAIL("You should never get here...");
        }
        // remove state transition again. Should succeed.
        m_StateA->RemoveTransition((StateNameType_t)"m_StateB", (StateEventIndexType_t)999);
    } catch(...) {
        QFAIL("You should never get here...");
    }
}


/****************************************************************************/
/**
 * \brief Test how States handle removal of one transition.
 */
/****************************************************************************/
void TestState::utTestOneTransitionRemoval()
{
    CheckForNull();

    try {
        // test one transition removal:
        QVERIFY(m_StateB->m_myHandleEventFlag == false);
        QVERIFY(m_StateB->m_myExitFlag == false);
        QVERIFY(m_StateB->m_myTransitions.isEmpty() == false);
        m_StateB->RemoveTransition((StateNameType_t)"m_StateA", (StateEventIndexType_t)21);

        QVERIFY(m_StateB->m_myTransitions.isEmpty() == false);
        QCOMPARE(m_StateB->DispatchEvent((StateEventIndexType_t)21), true); // go!
        QVERIFY(m_StateB->m_myHandleEventFlag == true); // internal event handling function shall be called
        QVERIFY(m_StateB->m_myExitFlag == false); // state shall NOT be exited
        QCOMPARE(m_StateB->m_myHandledEvent.GetIndex(), (StateEventIndexType_t)21); // event index shall match
        // reset the changed members
        m_StateB->m_myHandleEventFlag = false;
        m_StateB->m_myHandledEvent.SetIndex(0);
        // now check if we still can use an existing transition:
        QCOMPARE(m_StateB->DispatchEvent((StateEventIndexType_t)22), true); // go!
        QVERIFY(m_StateB->m_myHandleEventFlag == false); // internal event handling function shall NOT be called
        QVERIFY(m_StateB->m_myExitFlag == true); // state shall be exited
        QCOMPARE(m_StateB->m_myExitEvent.GetIndex(), (StateEventIndexType_t)22); // event index shall match
        // reset the changed members
        m_StateB->m_myExitFlag = false;
        m_StateB->m_myExitEvent.SetIndex(0);
        // check if state change signal was emitted successfully
        QCOMPARE(m_myState, (StateNameType_t)"m_StateC");
        QCOMPARE(m_myEvent.GetIndex(), (StateEventIndexType_t)22);
    } catch(...) {
        QFAIL("You should never get here...");
    }
}

/****************************************************************************/
/**
 * \brief Test how States handle removal of all transitions.
 */
/****************************************************************************/
void TestState::utTestAllTransitionRemoval()
{
    CheckForNull();

    try {
        // test ALL transition removal:
        m_myState = ""; // reset transition state
        m_myEvent.SetIndex(0); // reset transition event
        QVERIFY(m_StateC->m_myHandleEventFlag == false);
        QVERIFY(m_StateC->m_myExitFlag == false);
        QVERIFY(m_StateC->m_myTransitions.isEmpty() == false);
        m_StateC->RemoveAllTransitions((StateNameType_t)"m_StateA");
        m_StateC->RemoveAllTransitions((StateNameType_t)"m_StateB");
        QVERIFY(m_StateC->m_myTransitions.isEmpty() == true);
        QCOMPARE(m_StateC->DispatchEvent((StateEventIndexType_t)30), true); // go!
        QCOMPARE(m_StateC->m_myHandledEvent.GetIndex(), (StateEventIndexType_t)30); // event index shall match
        QCOMPARE(m_StateC->DispatchEvent((StateEventIndexType_t)31), true); // go!
        QCOMPARE(m_StateC->m_myHandledEvent.GetIndex(), (StateEventIndexType_t)31); // event index shall match
        QCOMPARE(m_StateC->DispatchEvent((StateEventIndexType_t)32), true); // go!
        QCOMPARE(m_StateC->m_myHandledEvent.GetIndex(), (StateEventIndexType_t)32); // event index shall match
        QVERIFY(m_StateC->m_myExitFlag == false); // state shall NOT be exited
        // reset the changed members
        m_StateC->m_myHandleEventFlag = false;
        m_StateC->m_myHandledEvent.SetIndex(0);
        // check if state change signal was emitted (it shall NOT be emitted here!)
        QCOMPARE(m_myState, (StateNameType_t)"");
        QCOMPARE(m_myEvent.GetIndex(), (StateEventIndexType_t)0);
    } catch(...) {
        QFAIL("You should never get here...");
    }
}

/****************************************************************************/
/**
 * \brief Test how States handle bad input.
 */
/****************************************************************************/
void TestState::utTestBadInputHandling()
{
    CheckForNull();

    try {
        // removing non-existent transition. it shall succeed:
        m_StateB->RemoveTransition((StateNameType_t)"m_StateA", (StateEventIndexType_t)9999);
        // removing all transitions for non-existent state. it shall succeed:
        m_StateB->RemoveAllTransitions((StateNameType_t)"m_StateABCDEG");
        // faking corrupted state transition table:
        m_StateB->m_myTransitions.insertMulti((StateEventIndexType_t)51, (StateNameType_t)"TestState1");
        m_StateB->m_myTransitions.insertMulti((StateEventIndexType_t)51, (StateNameType_t)"TestState2");
        m_StateB->m_myTransitions.insertMulti((StateEventIndexType_t)51, (StateNameType_t)"TestState3");
        // removing transition for event 51. it shall not fail (multiple transitions for one event):
        m_StateB->RemoveTransition((StateNameType_t)"TestState2", (StateEventIndexType_t)51);
        // faking corrupted state transition table again:
        m_StateB->m_myTransitions.insertMulti((StateEventIndexType_t)52, (StateNameType_t)"TestState1");
        m_StateB->m_myTransitions.insertMulti((StateEventIndexType_t)52, (StateNameType_t)"TestState2");
        m_StateB->m_myTransitions.insertMulti((StateEventIndexType_t)52, (StateNameType_t)"TestState3");
        // removing all transitions for TestState2. it shall not fail (multiple transitions for one event):
        m_StateB->RemoveAllTransitions((StateNameType_t)"TestState2");
    } catch(...) {
        QFAIL("You should never get here...");
    }
}

/****************************************************************************/
/**
 *  \brief   Test slot.
 *
 *  \param   stt = state to set
 *  \param   evt = event to handle
 */
/****************************************************************************/
void TestState::SetState(const StateMachines::StateNameType_t &stt, StateMachines::StateEvent evt)
{
    m_myState = stt;
    m_myEvent = evt;
}

} // end namespace StateMachines

QTEST_MAIN(StateMachines::TestState)
