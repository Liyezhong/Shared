/****************************************************************************/
/*! \file   DeviceState.cpp
 *
 *  \brief  Implementation file for class CState. Base class of all states
 *
 *  \version  0.1
 *  \date     2012-08-26
 *  \author   Torsten Scheck
 *
 *  \b Description:
 *
 *   N/A
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/
#include "DeviceControl/Include/Devices/DeviceState.h"
#include <QStateMachine>
#include <QDebug>

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief      Constructor of class CState.
 *
 *  \iparam     strName = name of the state
 *  \iparam     p_Parent = parent of the state
 */
/****************************************************************************/
CState::CState( const QString& strName, QState* p_Parent )
    : QState( p_Parent ),
      m_name( strName ),
      m_prefix(),
      m_DebugOutput(true)
{
}

/****************************************************************************/
/*!
 *  \brief      Constructor of class CState.
 *
 *  \iparam     strName = name of the state
 *  \iparam     strPrefix = CState with prefix
 *  \iparam     p_Parent = parent of the state
 */
/****************************************************************************/
CState::CState(const QString& strName, const QString& strPrefix, QState* p_Parent)
    : QState(p_Parent),
      m_name(strName),
      m_prefix(strPrefix),
      m_DebugOutput(true)
{
}

/****************************************************************************/
/*!
 *  \brief      Invoked on entring the state.
*               Prints state name on entry.
 *
 *  \iparam     e = unused
 */
/****************************************************************************/
void CState::onEntry( QEvent* e )
{
    Q_UNUSED( e );
 
    // Print out the state we are entering and its parents
    QString state = m_name;
    CState* p_Parent = dynamic_cast<CState*>( parentState() );
    while ( p_Parent != 0 )
    {
        state = p_Parent->name() + "->" + state;
        p_Parent = dynamic_cast<CState*>( p_Parent->parentState() );
    }
#ifdef DEVICE_STATE_DEBUG
    if (m_DebugOutput) {
        qDebug() << m_prefix << "Entering state:" << state;
    }
#endif
}
 
/****************************************************************************/
/*!
 *  \brief      Invoked on exiting the state.
*               Prints state name on exit.
 *
 *  \iparam     e = unused
 */
/****************************************************************************/
void CState::onExit( QEvent* e )
{
    Q_UNUSED( e );
 
    // Print out the state we are exiting and its parents
    QString state = m_name;
    CState* p_Parent = dynamic_cast<CState*>( parentState() );
    while ( p_Parent != 0 )
    {
        state = p_Parent->name() + "->" + state;
        p_Parent = dynamic_cast<CState*>( p_Parent->parentState() );
    }
#ifdef DEVICE_STATE_DEBUG
    if (m_DebugOutput) {
        qDebug() << m_prefix << "Exiting state:" << state;
    }
#endif
}

/****************************************************************************/
/*!
 *  \brief      To get pointer of top most state
 *
 *  \return     QState pointer
 */
/****************************************************************************/
QState *CState::getTopState()
{
    QState *s = this;
    while ( s->parentState() != s->machine() ) {
         s = s->parentState();
    }
    return s;
}

} //namespace
