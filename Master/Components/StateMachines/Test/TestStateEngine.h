/****************************************************************************/
/*! \file TestStateEngine.h
 *
 *  \brief Definition file for class TestStateEngine.
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

#ifndef STATEMACHINES_TESTSTATEENGINE_H
#define STATEMACHINES_TESTSTATEENGINE_H

#include <StateMachines/Include/StateMachineEngine.h>
#include <StateMachines/Test/DerivedState.h>

namespace StateMachines {

/****************************************************************************/
/**
 * \brief Test class for StateEngine class.
 */
/****************************************************************************/
class TestStateEngine: public QObject
{
  Q_OBJECT

public:

    TestStateEngine();
    ~TestStateEngine();

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    /// \warning: Each test case builds on the output of the previous one.
    /// All tests shall be run exactly in the order as they are listed here:
    void utTestTransitions();        ///< Test N1
    void utTestStateRemoval();       ///< Test N2
    void utTestStateInsertion();     ///< Test N3
    void utTestTransitionRemoval();  ///< Test N4
    void utTestBadInputHandling();   ///< Test N5

private:

    void CheckForNull();

private:

    QObject *m_ObjA;  ///< Test state A parent
    QObject *m_ObjB;  ///< Test state B parent
    QObject *m_ObjC;  ///< Test state C parent

    DerivedState *m_StateA; ///< Test state A pointer
    DerivedState *m_StateB; ///< Test state B pointer
    DerivedState *m_StateC; ///< Test state C pointer

    StateMachineEngine *m_myStateEngine; ///< the state engine to test
};

} // end namespace StateMachines

#endif // STATEMACHINES_TESTSTATEENGINE_H
