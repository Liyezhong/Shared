/****************************************************************************/
/*! \file Working.cpp
 *
 *  \brief Implementation file for class Working.
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
#include <ExternalProcessController/Include/ExternalProcessStates/Working.h>
#include <Global/Include/Utils.h>

namespace ExternalProcessControl {

/****************************************************************************/
/*!
*  \brief    Constructor.
*
*  \param[in]    wObj = pointer to working object / parent
*
****************************************************************************/
Working::Working(ExternalProcessController *wObj) :
        State::State("Working", wObj),
        m_myController(wObj)
{
}

/****************************************************************************/
/*!
*  \brief    Destructor.
*
****************************************************************************/
Working::~Working()
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
bool Working::OnEntry(StateMachines::StateEvent et)
{
    Q_UNUSED(et)
    if(m_myController) {
        qDebug() << "WorkingState of process " << m_myController->m_ProcessName << " entered";
    }

    if (m_myController == NULL)
    {
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
    if (m_myController->DoesExternalProcessUseNetCommunication()) {
        m_myController->m_myDevice->StopLoginGuard();
    }
    m_myController->OnReadyToWork();

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
bool Working::OnExit(StateMachines::StateEvent et)
{
    Q_UNUSED(et)
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
bool Working::HandleEvent(StateMachines::StateEvent et)
{
    Q_UNUSED(et)
    qDebug() << "WorkingState handles internal event " + QString::number(et.GetIndex(), 10);
    return true;
}

} // end namespace ExternalProcessControl
