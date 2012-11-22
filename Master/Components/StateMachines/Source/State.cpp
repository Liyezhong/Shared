/****************************************************************************/
/*! \file State.cpp
 *
 *  \brief Implementation file for class State.
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

#include <QDebug>
#include <StateMachines/Include/State.h>
#include <StateMachines/Include/StateMachinesEventCodes.h>
#include <Global/Include/Exception.h>

namespace StateMachines {

/****************************************************************************/
/*!
*  \brief    Constructor.
*
*  \param[in]    name = name of the state
*  \param[in]    pParent = pointer to parent
*
****************************************************************************/
State::State(const StateNameType_t &name, QObject *pParent) :
        QObject(pParent),
        m_myName(name)
{
    m_myTransitions.clear();
}

/****************************************************************************/
/*!
*  \brief    Destructor.
*
****************************************************************************/
State::~State()
{
}

/****************************************************************************/
/*!
*  \brief    This function processes an event.
*
*       First it checks if state has to be changed. If not, it is a local
*       event and it will be processed internally.
*
*  \param[in]    et = the event
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool State::DispatchEvent(StateEvent et)
{
    /// \todo check if only one transition exists for this event
    qDebug() << "State " + (QString)m_myName + " dispatches event " + QString::number(et.GetIndex(), 10);
    if (m_myTransitions.contains(et.GetIndex())) {
        if (!OnExit(et)) {
            /// \todo: log error
        }
        StateNameType_t name = m_myTransitions.value(et.GetIndex());
        emit SigSetNewState(name, et);
        return true;
    }

    return HandleEvent(et);
}

/****************************************************************************/
/*!
*  \brief    This function adds transition to state's list of transitions.
*
*  \param[in]    name = name of the state to switch to
*  \param[in]    Index = the event index to cause the transition
*
****************************************************************************/
void State::AddTransition(const StateNameType_t &name, StateEventIndexType_t Index)
{
    // check if transition already exists
    if(m_myTransitions.contains(Index)) {
        THROWARGS(EVENT_STATEMACHINES_ERROR_TRANSITION_EXISTS, Global::tTranslatableStringList() << m_myName << name <<
                  QString::number(Index, 10));
    }
    // insert transition
    m_myTransitions.insert(Index, name);
    // check if was successfully added
    if(!m_myTransitions.contains(Index)) {
        THROWARGS(EVENT_STATEMACHINES_ERROR_ADD_TRANSITION_FAILED, Global::tTranslatableStringList() << m_myName << name <<
                                                                   QString::number(Index, 10));
    }
}

/****************************************************************************/
/*!
*  \brief    This function removes transition from state's list of transitions.
*
*  \param[in]    name = name of the state
*  \param[in]    Index = the event index
*
****************************************************************************/
void State::RemoveTransition(const StateNameType_t &name, StateEventIndexType_t Index)
{
    // succeed if transition is not contained
    if (!m_myTransitions.contains(Index)) {
        return;
    }
    // remove
    m_myTransitions.remove(Index);
    // check if was successfully removed
    if(m_myTransitions.contains(Index)) {
        THROWARGS(EVENT_STATEMACHINES_ERROR_REMOVE_TRANSITION_FAILED, Global::tTranslatableStringList() << m_myName << name <<
                                                                      QString::number(Index, 10));
    }
}

/****************************************************************************/
/*!
*  \brief    This function removes all transitions for a given state name.
*
*       If this state has any transitions to the state with the name "sname",
*       all corresponding transitions will be removed from state's list of
*       transitions.
*
*  \param[in]    sname = name of the state
*
****************************************************************************/
void State::RemoveAllTransitions(const StateNameType_t &sname)
{
    // succeed if no transitions
    QList<StateEventIndexType_t> klist = m_myTransitions.keys(sname);
    if (klist.isEmpty()) {
        return;
    }

    // iterate through transitions and remove them
    for (int i = 0; i < klist.size(); i++) {
        m_myTransitions.remove(klist[i]);
    }

    // check if all transitions were deleted
    if(m_myTransitions.keys(sname).size() != 0) {
        THROWARGS(EVENT_STATEMACHINES_ERROR_REMOVE_ALL_TRANSITIONS_FAILED, Global::tTranslatableStringList() << m_myName <<  sname);
    }
}

} // end namespace StateMachines
