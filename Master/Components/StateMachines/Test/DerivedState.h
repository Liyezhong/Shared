/****************************************************************************/
/*! \file DerivedState.h
 *
 *  \brief Definition file for class DerivedState.
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

#ifndef STATEMACHINES_DERIVEDSTATE_H
#define STATEMACHINES_DERIVEDSTATE_H

#include <StateMachines/Include/State.h>

namespace StateMachines {

/****************************************************************************/
/**
 * \brief Test implementation of State class.
 */
/****************************************************************************/
class DerivedState: public State
{
  Q_OBJECT

private:
    /// test classes are all friends
    friend class TestState;
    friend class TestStateEngine;

public:

    DerivedState(const StateNameType_t &name, QObject *pParent);
    ~DerivedState();

    virtual bool OnEntry(StateEvent et);
    virtual bool OnExit(StateEvent et);

private:

    virtual bool HandleEvent(StateEvent et);

private:

    bool m_myEntryFlag;           ///< shows if state was entered
    bool m_myExitFlag;            ///< shows if state was exited
    bool m_myHandleEventFlag;     ///< shows if event was handled
    StateEvent m_myEntryEvent;    ///< event for state entry
    StateEvent m_myExitEvent;     ///< event for state exit
    StateEvent m_myHandledEvent;  ///< event for in-state handling
};

} // end namespace StateMachines

#endif // STATEMACHINES_DERIVEDSTATE_H
