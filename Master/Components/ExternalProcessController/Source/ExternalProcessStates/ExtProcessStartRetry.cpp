/****************************************************************************/
/*! \file ExtProcessStartRetry.cpp
 *
 *  \brief Implementation file for class ExtProcessStartRetry.
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
#include <ExternalProcessController/Include/ExternalProcessStates/ExtProcessStartRetry.h>
#include <Global/Include/Utils.h>

namespace ExternalProcessControl {

/****************************************************************************/
/*!
*  \brief    Constructor.
*
*  \param[in]    wObj = pointer to working object / parent
*
****************************************************************************/
ExtProcessStartRetry::ExtProcessStartRetry(ExternalProcessController *wObj) :
        State::State("ExtProcessStartRetry", wObj),
        m_myController(wObj)
{
}

/****************************************************************************/
/*!
*  \brief    Destructor.
*
****************************************************************************/
ExtProcessStartRetry::~ExtProcessStartRetry()
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
bool ExtProcessStartRetry::OnEntry(StateMachines::StateEvent et)
{
    qDebug() << "ExtProcessStartRetryState entered.";
    if (m_myController == NULL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_NULL_POINTER,
                                                   Global::tTranslatableStringList() << ""
                                                    << FILE_LINE);
        //StateMachines::StateEvent et1;
        //et1.SetIndex(EP_NULL_CTRL_POINTER);
        if (!State::DispatchEvent(Global::AsInt(EP_NULL_CTRL_POINTER))) {
            Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_DISPATCH_EVENT,
                                                       Global::tTranslatableStringList() << ""
                                                                                    << "EP_NULL_CTRL_POINTER"
                                                                                     << FILE_LINE);
        }
        return false;
    }
    m_myController->OnStopWorking();
    // stop device's powerup login timer if it is runnning
    m_myController->m_myDevice->StopLoginGuard();
    return HandleEvent(et);
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
bool ExtProcessStartRetry::OnExit(StateMachines::StateEvent et)
{
    Q_UNUSED(et)
    qDebug() << "ExtProcessStartRetryState exited.";

    // stop device's powerup login timer: if we exit this state,
    // this means we're either OK or dead. in both cases there is
    // no need for LoginGuard.
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
bool ExtProcessStartRetry::HandleEvent(StateMachines::StateEvent et)
{
    qDebug() << "ExtProcessStartRetryState handles internal event " + QString::number(et.GetIndex(), 10);

    if (m_myController == NULL) {
        return false;
    }

    switch (et.GetIndex()) {
    case EP_EXTPROCESS_LOGIN_TIMEOUT:
        m_myController->KillAndRestartProcess();
        break;
    case EP_CANNOT_START_EXTPROCESS:
        // WARNING: no break here, we go straight into next case -->
    case EP_EXTPROCESS_EXITED:
        m_myController->RestartProcess();
        break;
    default :
        break;
    }

    return true;
}

} // end namespace ExternalProcessControl
