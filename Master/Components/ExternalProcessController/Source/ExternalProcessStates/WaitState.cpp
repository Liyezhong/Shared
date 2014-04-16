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
    if (m_myController == NULL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_NULL_POINTER,
                                                   Global::tTranslatableStringList() << ""
                                                    << FILE_LINE);
        if (!State::DispatchEvent(Global::AsInt(EP_NULL_CTRL_POINTER))) {
            Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_DISPATCH_EVENT,
                                                       Global::tTranslatableStringList() << ""
                                                                                    << "EP_NULL_CTRL_POINTER"
                                                                                     << FILE_LINE);
        }
        return false;
    }

    qDebug() << "WaitState entered for process" << this->m_myController->GetProcessName();

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
    if(m_myController) {
        qDebug() << "WaitState exited for process" << this->m_myController->GetProcessName();
    }
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
        Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_NULL_POINTER,
                                                   Global::tTranslatableStringList() << ""
                                                    << FILE_LINE);
        return false;
    }

    switch (et.GetIndex())
    {
    case EP_START_OPERATION:
        if (m_myController->DoesExternalProcessUseNetCommunication() && !m_myController->m_myDevice->StartDevice()) {
            Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_START_DEVICE_PROCESS,
                                                       Global::tTranslatableStringList() << m_myController->GetProcessName()
                                                                                        << FILE_LINE);
            qDebug() << "WaitState: Cannot start ExternalProcessDevice!";
            if (!State::DispatchEvent(Global::AsInt(EP_CANNOT_START_DEVICE))) {
                Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_DISPATCH_EVENT,
                                                           Global::tTranslatableStringList() << m_myController->GetProcessName()
                                                                                            << "EP_CANNOT_START_DEVICE"
                                                                                             << FILE_LINE);
            }
            return false;
        }
        break;
    case EP_EXTPROCESS_DISCONNECTED:
        if (m_myController->DoesExternalProcessUseNetCommunication()) {
            m_myController->m_myDevice->StartLoginGuard();
            if (m_myTimer.isActive()) {
                if (m_NofDisconnects > EPC_NOF_ALLOWED_DISCONNECTS) {
                    if (!State::DispatchEvent(Global::AsInt(EP_TOO_MANY_RESTARTS))) {
                        Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_DISPATCH_EVENT,
                                                                   Global::tTranslatableStringList() << m_myController->GetProcessName()
                                                                                                    << "EP_TOO_MANY_RESTARTS"
                                                                                                     << FILE_LINE);
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
        }
        break;
    default:
        break;
    }
    return true;
}

} // end namespace ExternalProcessControl
