/****************************************************************************/
/*! \file CommunicationRetry.cpp
 *
 *  \brief Implementation file for class CommunicationRetry.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 07.12.2010
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
#include <ExternalProcessController/Include/ExternalProcessStates/CommunicationRetry.h>
#include <Global/Include/Utils.h>

namespace ExternalProcessControl {

/****************************************************************************/
/*!
*  \brief    Constructor.
*
*  \param[in]    wObj = pointer to working object / parent
*
****************************************************************************/
CommunicationRetry::CommunicationRetry(ExternalProcessController *wObj) :
        State::State("CommunicationRetry", wObj),
        m_myController(wObj)
{
}

/****************************************************************************/
/*!
*  \brief    Destructor.
*
****************************************************************************/
CommunicationRetry::~CommunicationRetry()
{
    m_myController = NULL;
}

/****************************************************************************/
/*!
*  \brief    This function is called when the State is entered.
*
*  \param[in]    et = the event to process
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool CommunicationRetry::OnEntry(StateMachines::StateEvent et)
{
    Q_UNUSED(et)
    qDebug() << "CommunicationRetryState entered.";

    if (m_myController == NULL) {
        /// \todo log error
        if (!State::DispatchEvent(Global::AsInt(EP_NULL_CTRL_POINTER))) {
            /// \todo log error
        }
        return false;
    }

    // restart device's powerup login timer
    m_myController->m_myDevice->StartLoginGuard();

    return true;
}

/****************************************************************************/
/*!
*  \brief    This function is called when the State is exited.
*
*  \param[in]    et = the event to process
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool CommunicationRetry::OnExit(StateMachines::StateEvent et)
{
    Q_UNUSED(et)
    qDebug() << "CommunicationRetryState exited.";

    // restart device's powerup login timer
    if (m_myController != NULL) {
        m_myController->m_myDevice->StopLoginGuard();
    }
    else {
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
*  \brief    This function processes state's internal event.
*
*  \param[in]    et = the event to process
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool CommunicationRetry::HandleEvent(StateMachines::StateEvent et)
{
    qDebug() << "CommunicationRetryState handles internal event " + QString::number(et.GetIndex(), 10);

    if (m_myController == NULL) {
        return false;
    }

    switch (et.GetIndex()) {
    case EP_EXTPROCESS_LOGIN_TIMEOUT:
        // login timedout -> try to kill and restart the process
        if (!m_myController->m_myProcess->KillProcess()) {
            qDebug() << (QString)("CommunicationRetryState: Cannot kill the External Process !");
            /// \todo log error
            if (!State::DispatchEvent(Global::AsInt(EP_CANNOT_KILL_EXTPROCESS))) {
                /// \todo log error
            }
            return false;
        }
        break;
    default:
        break;
    }
    return true;
}

} // end namespace ExternalProcessControl
