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

    qDebug() << "InitialState entered for process" << this->m_myController->GetProcessName();

    if (m_myController->DoesExternalProcessUseNetCommunication() && !m_myController->m_myDevice->StartDevice()) {
        Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_START_DEVICE_PROCESS,
                                                   Global::tTranslatableStringList() << m_myController->GetProcessName()
                                                                                    << FILE_LINE);
        qDebug() << "InitialState: Cannot start ExternalProcessDevice!";
        if (!State::DispatchEvent(Global::AsInt(EP_CANNOT_START_DEVICE))) {
            Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_DISPATCH_EVENT,
                                                       Global::tTranslatableStringList() << m_myController->GetProcessName()
                                                                                        << "EP_CANNOT_START_DEVICE"
                                                                                         << FILE_LINE);
        }
        return false;
    }

    if (!m_myController->m_myProcess->StartProcess(m_myController->m_myCommand)) {
        qDebug() << (QString)("InitialState: Cannot start the External Process !");
        Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_START_DEVICE_PROCESS,
                                                   Global::tTranslatableStringList() << m_myController->GetProcessName()
                                                                                    << FILE_LINE);
        if (!State::DispatchEvent(Global::AsInt(EP_CANNOT_START_EXTPROCESS))) {
            Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_DISPATCH_EVENT,
                                                       Global::tTranslatableStringList() << m_myController->GetProcessName()
                                                                                        << "EP_CANNOT_START_EXTPROCESS"
                                                                                         << FILE_LINE);
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
    if(m_myController) {
        qDebug() << "InitialState exited for process" << this->m_myController->GetProcessName();
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
bool Initial::HandleEvent(StateMachines::StateEvent et)
{
    qDebug() << "InitialState handles internal event " + QString::number(et.GetIndex(), 10);

    if (m_myController == NULL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_EPC_ERROR_NULL_POINTER,
                                                   Global::tTranslatableStringList() << ""
                                                   << FILE_LINE);
        return false;
    }
    return true;
}

} // end namespace ExternalProcessControl
