/****************************************************************************/
/*! \file TestState.h
 *
 *  \brief Definition file for class TestState.
 *
 *  $Version: $ 0.1
 *  $Date:    $ 05.02.2011
 *  $Author:  $ Y.Novak
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

#ifndef STATEMACHINES_TESTSTATE_H
#define STATEMACHINES_TESTSTATE_H

#include <StateMachines/Include/State.h>
#include <StateMachines/Test/DerivedState.h>

namespace StateMachines {

/****************************************************************************/
/**
 * \brief Test class for State class.
 */
/****************************************************************************/
class TestState: public QObject
{
  Q_OBJECT

public:

    TestState();
    ~TestState();

public slots:

    void SetState(const StateMachines::StateNameType_t &, StateMachines::StateEvent);

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    /// \warning: Each test case builds on the output of the previous one.
    /// All tests shall be run exactly in the order as they are listed here:
    void utTestEventProcessing();      ///< Test N1
    void utTestTransitionAdd();        ///< Test N2
    void utTestOneTransitionRemoval(); ///< Test N3
    void utTestAllTransitionRemoval(); ///< Test N4
    void utTestBadInputHandling();     ///< Test N5

private:

    void CheckForNull();

private:

    QObject *m_ObjA;  ///< Test state A parent
    QObject *m_ObjB;  ///< Test state B parent
    QObject *m_ObjC;  ///< Test state C parent

    DerivedState *m_StateA; ///< Test state A pointer
    DerivedState *m_StateB; ///< Test state B pointer
    DerivedState *m_StateC; ///< Test state C pointer

    StateNameType_t m_myState; ///< test state name
    StateEvent m_myEvent;      ///< test state event
};

} // end namespace StateMachines

#endif // STATEMACHINES_TESTSTATE_H
