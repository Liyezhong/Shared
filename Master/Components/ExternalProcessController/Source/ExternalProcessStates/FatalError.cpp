/****************************************************************************/
/*! \file FatalError.cpp
 *
 *  \brief Implementation file for class FatalError.
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
#include <ExternalProcessController/Include/ExternalProcessStates/FatalError.h>
#include <Global/Include/Utils.h>

namespace ExternalProcessControl {

/****************************************************************************/
/*!
*  \brief    Constructor.
*
*  \param[in]    wObj = pointer to working object / parent
*
****************************************************************************/
FatalError::FatalError(ExternalProcessController *wObj) :
        State::State("FatalError", wObj),
        m_myController(wObj)
{
}

/****************************************************************************/
/*!
*  \brief    Destructor.
*
*
****************************************************************************/
FatalError::~FatalError()
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
bool FatalError::OnEntry(StateMachines::StateEvent et)
{
    if (m_myController == NULL || m_myController->m_myProcess == NULL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_NULL_POINTER,
                                                   Global::tTranslatableStringList() << ""
                                                    << FILE_LINE);
        /// \todo we cannot switch further, do something here :)
        return false;
    }


    if (this->m_myController->GetProcessName().contains("Sepia"))
    {
        qDebug() << "Sepia FatalError";
    }

    if (m_myController->DoesExternalProcessUseNetCommunication()) {
        // stop device's powerup login timer if it is runnning
        m_myController->m_myDevice->StopLoginGuard();
    }
    if (et.GetIndex() == EP_TOO_MANY_RESTARTS) {
        m_myController->OnStopWorking(true);
    }

    //Kill if process is running
    m_myController->m_myProcess->KillProcess();
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
bool FatalError::OnExit(StateMachines::StateEvent et)
{
    Q_UNUSED(et)
    if(m_myController) {
        qDebug() << "FatalErrorState exited for process" << this->m_myController->GetProcessName();
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
bool FatalError::HandleEvent(StateMachines::StateEvent et)
{
    qDebug() << "FatalErrorState handles internal event " + QString::number(et.GetIndex(), 10);

    if (m_myController == NULL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_NULL_POINTER,
                                                   Global::tTranslatableStringList() << ""
                                                    << FILE_LINE);
        return false;
    }

    switch (et.GetIndex())
    {
    case EP_EXTPROCESS_CONNECTED:
        if (m_myController->DoesExternalProcessUseNetCommunication()) {
            m_myController->m_myDevice->DisconnectPeer();
        }
        break;
    default:
        break;
    }
    return true;
}

} // end namespace ExternalProcessControl
