/****************************************************************************/
/*! \file Initial.cpp
 *
 *  \brief Implementation file for class Initial.
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
#include <ExternalProcessController/Include/ExternalProcessStates/Initial.h>
#include <Global/Include/Utils.h>

namespace ExternalProcessControl {

/****************************************************************************/
/*!
*  \brief    Constructor.
*
*  \param[in]    wObj = pointer to working object / parent
*
****************************************************************************/
Initial::Initial(ExternalProcessController *wObj) :
        State::State("Initial", wObj),
        m_myController(wObj)
{
}

/****************************************************************************/
/*!
*  \brief    Destructor.
*
****************************************************************************/
Initial::~Initial()
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
bool Initial::OnEntry(StateMachines::StateEvent et)
{
    Q_UNUSED(et)
    qDebug() << "InitialState entered for process" << this->m_myController->GetProcessName();

    if (m_myController == NULL) {
        /// \todo log error
        if (!State::DispatchEvent(Global::AsInt(EP_NULL_CTRL_POINTER))) {
            /// \todo log error
        }
        return false;
    }

    if (!m_myController->m_myDevice->StartDevice()) {
        /// \todo log error
        qDebug() << "InitialState: Cannot start ExternalProcessDevice!";
        if (!State::DispatchEvent(Global::AsInt(EP_CANNOT_START_DEVICE))) {
            /// \todo log error
        }
        return false;
    }

    if (!m_myController->m_myProcess->StartProcess(m_myController->m_myCommand)) {
        qDebug() << (QString)("InitialState: Cannot start the External Process !");
        /// \todo log error
        if (!State::DispatchEvent(Global::AsInt(EP_CANNOT_START_EXTPROCESS))) {
            /// \todo log error
        }
        return false;
    }

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
bool Initial::OnExit(StateMachines::StateEvent et)
{
    Q_UNUSED(et)
    qDebug() << "InitialState exited for process" << this->m_myController->GetProcessName();
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
bool Initial::HandleEvent(StateMachines::StateEvent et)
{
    qDebug() << "InitialState handles internal event " + QString::number(et.GetIndex(), 10);

    if (m_myController == NULL) {
        /// \todo log error
        return false;
    }

    switch (et.GetIndex()) {
    case EP_EXTPROCESS_LOGIN_TIMEOUT:
        // login timedout -> try to kill and restart the process
        if (!m_myController->m_myProcess->KillProcess()) {
            qDebug() << (QString)("InitialState: Cannot kill the External Process !");
            /// \todo log error
            if (!State::DispatchEvent(Global::AsInt(EP_CANNOT_KILL_EXTPROCESS))) {
                /// \todo log error
            }
            return false; ///< \todo it shall probably be true here, not false. false means "cannot process or internal error"
        }
        break;
    default:
        break;
    }
    return true;
}

} // end namespace ExternalProcessControl
