/****************************************************************************/
/*! \file State.h
 *
 *  \brief Definition file for class State.
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

#ifndef STATEMACHINES_STATE_H
#define STATEMACHINES_STATE_H

#include <StateMachines/Include/StateEvent.h>

#include <QHash>

namespace StateMachines {

/// Typedef for a State Name type
typedef QString StateNameType_t;

/****************************************************************************/
/**
 * \brief  This is a virtual base class for a generic State.
 */
/****************************************************************************/
class State : public QObject
{
    Q_OBJECT

    /// Unit test classes are all friends:
    friend class TestState;
    friend class TestStateEngine;

public:

    State(const StateNameType_t &name, QObject *pParent);
    virtual ~State();

public:

    bool DispatchEvent(StateEvent et);
    /****************************************************************************/
    /*!
    *  \brief    Get function for the state name
    *
    *  \return   name of the state
    *
    ****************************************************************************/
    inline StateNameType_t GetName() const {
        return m_myName;
    }
    void AddTransition(const StateNameType_t &name, StateEventIndexType_t index);
    void RemoveTransition(const StateNameType_t &name, StateEventIndexType_t index);
    void RemoveAllTransitions(const StateNameType_t &name);

    /****************************************************************************/
    /*!
    *  \brief    This function is called when the State is entered.
    *
    *  \param[in]    et = the event
    *
    *  \return   TRUE if executed successfully, FALSE otherwise
    *
    ****************************************************************************/
    virtual bool OnEntry(StateEvent et) = 0;
    /****************************************************************************/
    /*!
    *  \brief    This function is called when the State is exited.
    *
    *  \param[in]    et = the event
    *
    *  \return   TRUE if executed successfully, FALSE otherwise
    *
    ****************************************************************************/
    virtual bool OnExit(StateEvent et) = 0;

signals:

   /****************************************************************************/
   /*!
    *  \brief    This signal is emitted when state transition shall happen
    *
    *  \param    stname = name of the state to switch to
    *  \param    stevent = the event which caused transition
    *
    ****************************************************************************/
    void SigSetNewState(const StateMachines::StateNameType_t &stname, StateMachines::StateEvent stevent);

private:

    State();                                    ///< Not implemented.
    State(const State &);                       ///< Not implemented.
    const State & operator = (const State &);   ///< Not implemented.


    /****************************************************************************/
    /*!
    *  \brief    This function processes state's internal event.
    *
    *  \param[in]    et = the event
    *
    *  \return   TRUE if executed successfully, FALSE otherwise
    *
    ****************************************************************************/
    virtual bool HandleEvent(StateEvent et) = 0;

private:

    /// name of the state
    StateNameType_t   m_myName;
    /// list of all transitions from this state to other states
    QHash<StateEventIndexType_t, StateNameType_t>  m_myTransitions;
};

} // end namespace StateMachines

#endif // STATEMACHINES_STATE_H
