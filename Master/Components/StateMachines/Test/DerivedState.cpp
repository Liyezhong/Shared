/****************************************************************************/
/*! \file DerivedState.cpp
 *
 *  \brief Implementation file for class DerivedState.
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
#include <StateMachines/Test/DerivedState.h>
#include <Global/Include/LoggingSource.h>

namespace StateMachines {


/****************************************************************************/
/**
 *  \brief Constructor.
 *
 *  \param[in] name = name of the state
 *  \param[in] pParent = object's parent
 */
/****************************************************************************/
DerivedState::DerivedState(const StateNameType_t &name, QObject *pParent) :
        State::State(name, pParent),
        m_myEntryFlag(false),
        m_myExitFlag(false),
        m_myHandleEventFlag(false)
{
    m_myEntryEvent.SetIndex(0);
    m_myExitEvent.SetIndex(0);
    m_myHandledEvent.SetIndex(0);
}

/****************************************************************************/
/**
 *  \brief Destructor.
 */
/****************************************************************************/
DerivedState::~DerivedState()
{
}

/****************************************************************************/
/**
 *  \brief Implementing State's pure virtual "OnEntry" function
 *
 *  \param[in] et = event to handle
 */
/****************************************************************************/
bool DerivedState::OnEntry(StateEvent et)
{
    m_myEntryEvent = et;
    m_myEntryFlag = true;
    return true;
}

/****************************************************************************/
/**
 *  \brief Implementing State's pure virtual "OnExit" function
 *
 *  \param[in] et = event to handle
 */
/****************************************************************************/
bool DerivedState::OnExit(StateEvent et)
{
    m_myExitEvent = et;
    m_myExitFlag = true;
    return true;
}

/****************************************************************************/
/**
 *  \brief Implementing State's pure virtual "HandleEvent" function
 *
 *  \param[in] et = event to handle
 */
/****************************************************************************/
bool DerivedState::HandleEvent(StateEvent et)
{
    m_myHandledEvent = et;
    m_myHandleEventFlag = true;
    return true;
}

} // end namespace StateMachines
