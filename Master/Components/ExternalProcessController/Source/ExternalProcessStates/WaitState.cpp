/****************************************************************************/
/*! \file WaitState.cpp
 *
 *  \brief Implementation file for class WaitState.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.02.2011
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

#include <QDebug>
#include <ExternalProcessController/Include/ExternalProcessStates/WaitState.h>
#include <Global/Include/Utils.h>

namespace ExternalProcessControl {

/****************************************************************************/
/*!
*  \brief    Constructor.
*
*  \param[in]    wObj = pointer to working object / parent
*
****************************************************************************/
WaitState::WaitState(ExternalProcessController *wObj) :
        State::State("WaitState", wObj),
        m_myController(wObj),
        m_NofDisconnects(0)
{
}

/****************************************************************************/
/*!
*  \brief    Destructor.
*
****************************************************************************/
WaitState::~WaitState()
{
    m_myController = NULL;
}

/****************************************************************************/
/*!
*  \brief    This function is called when the State is entered.
*
*  \param[in]    et = the event
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool WaitState::OnEntry(StateMachines::StateEvent et)
{
    qDebug() << "WaitState entered for process" << this->m_myController->GetProcessName();

    if (m_myController == NULL) {
        /// \todo log error
        if (!State::DispatchEvent(Global::AsInt(EP_NULL_CTRL_POINTER))) {
            /// \todo log error
        }
        return false;
    }

    return HandleEvent(et);
}

/****************************************************************************/
/*!
*  \brief    This function is called when the State is exited.
*
*  \param[in]    et = the event
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool WaitState::OnExit(StateMachines::StateEvent et)
{
    Q_UNUSED(et)
    qDebug() << "WaitState exited for process" << this->m_myController->GetProcessName();
    return true;
}

/****************************************************************************/
/*!
*  \brief    This function processes state's internal event.
*
*  \param[in]    et = the event
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool WaitState::HandleEvent(StateMachines::StateEvent et)
{
    qDebug() << "WaitState handles internal event " + QString::number(et.GetIndex(), 10);

    if (m_myController == NULL) {
        /// \todo log error
        return false;
    }

    switch (et.GetIndex())
    {
    case EP_START_OPERATION:
        if (!m_myController->m_myDevice->StartDevice()) {
            /// \todo log error
            qDebug() << "WaitState: Cannot start ExternalProcessDevice!";
            if (!State::DispatchEvent(Global::AsInt(EP_CANNOT_START_DEVICE))) {
                /// \todo log error
            }
            return false;
        }
        break;
    case EP_EXTPROCESS_DISCONNECTED:
        m_myController->m_myDevice->StartLoginGuard();
        if (m_myTimer.isActive()) {
            if (m_NofDisconnects > EPC_NOF_ALLOWED_DISCONNECTS) {
                if (!State::DispatchEvent(Global::AsInt(EP_TOO_MANY_RESTARTS))) {
                    /// \todo log error
                }
            }
            else {
                m_NofDisconnects++;
            }
        }
        else {
            // configure and start the "restart window" timer:
            m_myTimer.setSingleShot(true);
            m_myTimer.setInterval(CONNECT_GUARD_WINDOW);
            m_myTimer.start();
            m_NofDisconnects++;
        }
        break;
    default:
        break;
    }
    return true;
}

} // end namespace ExternalProcessControl
