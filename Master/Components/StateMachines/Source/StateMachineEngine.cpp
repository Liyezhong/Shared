/****************************************************************************/
/*! \file StateMachineEngine.cpp
 *
 *  \brief Implementation file for class StateMachineEngine.
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

#include <StateMachines/Include/StateMachineEngine.h>
#include <StateMachines/Include/StateMachinesEventCodes.h>
#include <Global/Include/Utils.h>
#include <Global/Include/Exception.h>
#include <Global/Include/EventObject.h>

#include <QDebug>
#include <QMetaType>

namespace StateMachines {

/****************************************************************************/
/*!
*  \brief    Constructor.
*
*  \param[in]    pParent = pointer to parent
*
****************************************************************************/
StateMachineEngine::StateMachineEngine(QObject *pParent) :
        QObject(pParent),
        m_myCurrentState(NULL),
        m_EngineStopped(true)
{
    // register new Qt metatypes for Signal/Slot to work
    // no need for Q_DECLARE_METATYPE ?
    qRegisterMetaType<StateNameType_t>("StateMachines::StateNameType_t");
    qRegisterMetaType<StateEvent>("StateMachines::StateEvent");

    // clear list of registered states
    StatesList.clear();
}

/****************************************************************************/
/*!
*  \brief    Destructor.
*
****************************************************************************/
StateMachineEngine::~StateMachineEngine()
{
    try {
        // clear list of registered states
        StatesList.clear();
    }
    catch(...) {
        // to please Lint
    }
}

/****************************************************************************/
/*!
*  \brief    Adds state to the state machine.
*
*  \param[in]    st = pointer to state object
*
****************************************************************************/
void StateMachineEngine::AddState(State *st)
{
    CHECKPTR(st);
    StatesList.insert(st->GetName(), st);
    // check for name
    if (StatesList.value(st->GetName()) != st) {
        LOGANDTHROWARG(EVENT_STATEMACHINES_ERROR_ADD_STATE_FAILED, st->GetName());
    }
    // connect signals
    CONNECTSIGNALSLOT(st,   SigSetNewState(const StateMachines::StateNameType_t &, StateMachines::StateEvent),
                      this, SetState(const StateMachines::StateNameType_t &, StateMachines::StateEvent));
}

/****************************************************************************/
/*!
*  \brief    Removes state from the state machine.
*
*
*  \param[in]    st = pointer to state object
*
*  \warning  This function will throw an exception if it is attempted to
*            remove the current state. Current state cannot be removed.
*
****************************************************************************/
void StateMachineEngine::RemoveState(State *st)
{
    // check for NULL pointer
    CHECKPTR(st);

    QString stname = st->GetName();

    // check if current state
    if (m_myCurrentState != NULL) {
        if (m_myCurrentState->GetName() == stname) {
            // cannot remove current state!
            LOGANDTHROWARG(EVENT_STATEMACHINES_ERROR_REMOVE_CURRENT_STATE, stname);
        }
    }

    // remove
    StatesList.remove(stname);
    // check if removed
    if(StatesList.contains(stname)) {
        // state not removed correctly
        LOGANDTHROWARG(EVENT_STATEMACHINES_ERROR_REMOVE_STATE_FAILED, stname);
    }

    // notify all states which have transitions to the "st" that this state is dead
    QList<State*> slist = StatesList.values();
    for (int i = 0; i < StatesList.size(); i++) {
        /// \todo if an exception is thrown, what about the remaining states from list?
        (slist[i])->RemoveAllTransitions(stname);
    }

    // disconnect state signals from all receivers
    //if (!QObject::disconnect(st, SIGNAL(SigSetNewState(const StateMachines::StateNameType_t &, StateMachines::StateEvent)), this, SLOT(SetState(const StateMachines::StateNameType_t &, StateMachines::StateEvent)))) {
    if (!st->disconnect()) {
        //todo: log error
    }
}

/****************************************************************************/
/*!
*  \brief    This function adds state transition to the state machine.
*
*       Transit from state s1 to state s2 upon event with "index"
*
*  \param[in]    s1 = pointer to state object to exit. If it is NULL, all states (excluding s2) will get the transition
*  \param[in]    s2 = pointer to state object to enter
*  \param[in]    Index = event index to cause the transition
*
****************************************************************************/
void StateMachineEngine::AddTransition(State *s1, State *s2, StateEventIndexType_t Index)
{
    CHECKPTR(s2);
    if (s1 != NULL) {
        // only one state gets the transition
        s1->AddTransition(s2->GetName(), Index);
    } else {
        // all states except s2 get the transition
        for(QHash<StateNameType_t, State*>::iterator it = StatesList.begin(); it != StatesList.end(); ++it) {
            State *pStartState = *it;
            if(pStartState != s2) {
                // try to attach transition
                pStartState->AddTransition(s2->GetName(), Index);
            }
        }
    }
}

/****************************************************************************/
/*!
*  \brief    This function removes state transition from the state machine.
*
*
*  \param[in]    s1 = pointer to state object to exit
*  \param[in]    s2 = pointer to state object to enter
*  \param[in]    Index = event index to cause the transition
*
****************************************************************************/
void StateMachineEngine::RemoveTransition(State *s1, State *s2, StateEventIndexType_t Index)
{
    CHECKPTR(s1);
    CHECKPTR(s2);
    // now remove transition
    s1->RemoveTransition(s2->GetName(), Index);
}

/****************************************************************************/
/*!
*  \brief    This function starts state machine operation.
*
*
*  \param[in]    name = name of state to start from
*  \param[in]    e = initial event
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool StateMachineEngine::Start(const StateNameType_t &name, StateEvent e)
{
    if (!StatesList.contains(name)) {
        // throw exception
        LOGANDTHROWARG(EVENT_STATEMACHINES_ERROR_UNKNOWN_STATE, name);
    }
    m_myCurrentState = StatesList.value(name);
    CHECKPTR(m_myCurrentState);
    // set working flag and GO!:
    m_EngineStopped = false;
    if (!m_myCurrentState->OnEntry(e)) {
        /// \todo: log error
    }
    return true;
}

/****************************************************************************/
/*!
*  \brief    This function starts state machine operation.
*
*
*  \param[in]    name = name of state to start from
*  \param[in]    ei = initial event index
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool StateMachineEngine::Start(const StateNameType_t &name, StateEventIndexType_t ei) {
    StateEvent Evt(ei);
    return Start(name, Evt);
}

/****************************************************************************/
/*!
*  \brief    This function stops state machine operation.
*
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
void StateMachineEngine::Stop()
{
    m_EngineStopped = true;
}

/****************************************************************************/
/*!
*  \brief    This function forwards external event to the current state.
*
*
*  \param[in]   et = the event to dispatch
*
*  \return   TRUE if Engine is started and executed successfully, FALSE otherwise
*
****************************************************************************/
bool StateMachineEngine::DispatchEvent(StateEvent et)
{
    if (m_EngineStopped == true) {
        return false;
    }

    CHECKPTR(m_myCurrentState);
    return m_myCurrentState->DispatchEvent(et);
}

/****************************************************************************/
/*!
*  \brief    This function changes current state machine's state.
*
*
*  \param[in]    name = name of state to switch to
*  \param[in]    et = the event which caused the transition
*
****************************************************************************/
void StateMachineEngine::SetState(const StateNameType_t &name, StateEvent et)
{
    if (!StatesList.contains(name)) {
        // throw exception
        LOGANDTHROWARG(EVENT_STATEMACHINES_ERROR_UNKNOWN_STATE, name);
    }
    m_myCurrentState = StatesList.value(name);
    CHECKPTR(m_myCurrentState);
    if (!m_myCurrentState->OnEntry(et)) {
        /// \todo: handle error
    }
}

/****************************************************************************/
/*!
*  \brief    Get pointer to the current state.
*
*  \return   current state pointer
*
****************************************************************************/
State* StateMachineEngine::GetCurrentState()
{
    return m_myCurrentState;
}

} // end namespace StateMachines
