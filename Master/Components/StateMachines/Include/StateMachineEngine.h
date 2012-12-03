/****************************************************************************/
/*! \file StateMachineEngine.h
 *
 *  \brief Definition file for class StateMachineEngine.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-12-02
 *  $Author:    $ Y.Novak
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

#ifndef STATEMACHINES_STATEMACHINEENGINE_H
#define STATEMACHINES_STATEMACHINEENGINE_H

#include <StateMachines/Include/State.h>

namespace ExternalProcessControl {
    class TestExternalProcessCtlr;
    class TestExternalProcessCtlr2;
}

namespace StateMachines {

/****************************************************************************/
/**
 * \brief  This is a base class for handling State transitions.
 *         It is a state machine container, which holds references to all
 *         existing in this state machine States.
 */
/****************************************************************************/
class StateMachineEngine : public QObject
{
    Q_OBJECT

    /// test classes are all friends:
    friend class TestStateEngine;
    friend class DerivedController;
    friend class ExternalProcessControl::TestExternalProcessCtlr;
    friend class ExternalProcessControl::TestExternalProcessCtlr2;

public:

    StateMachineEngine(QObject *pParent);
    virtual ~StateMachineEngine();

public:

    void AddState(State * st);
    void RemoveState(State * st);
    void AddTransition(State * s1, State * s2, StateEventIndexType_t index);
    void RemoveTransition(State * s1, State * s2, StateEventIndexType_t index);
    bool Start(const StateNameType_t &name, StateEvent e);
    bool Start(const StateNameType_t &name, StateEventIndexType_t ei);
    void Stop();
    bool DispatchEvent(StateEvent et);
    State* GetCurrentState();

public slots:

    void SetState(const StateMachines::StateNameType_t &name, StateMachines::StateEvent et);

private:
    StateMachineEngine();                                                   ///< Not implemented.
    StateMachineEngine(const StateMachineEngine&);                          ///< Not implemented.
    const StateMachineEngine & operator = (const StateMachineEngine &);     ///< Not implemented.

    /// list of all states in the state machine
    QHash<StateNameType_t, State*>       StatesList;
    /// current state pointer
    State*                             m_myCurrentState;
    /// flag to start/top engine operation. if engine is stopped, no event processing happens.
    bool m_EngineStopped;
};

} // end namespace StateMachines

#endif // STATEMACHINES_STATEMACHINEENGINE_H
