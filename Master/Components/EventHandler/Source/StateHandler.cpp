/****************************************************************************/
/*! \file EventHandler/Source/StateHandler.cpp
 *
 *  \brief Implementation file for class StateHandler.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2012-07-17
 *  $Author:    $ Michael Thiel
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <EventHandler/Include/StateHandler.h>
#include "Global/Include/Commands/CmdEventUpdate.h"
#include <EventHandler/Include/EventHandlerEventCodes.h>

#include <QAbstractTransition>
#include <QDebug>

namespace EventHandler {

StateHandler StateHandler::m_StateHandlerInstance; //!< The instance

/****************************************************************************/
StateHandler::StateHandler()
{
    m_DefaultState = new QState();
    m_DefaultState->setObjectName("DefaultState");
    m_SoftSwitchMonitorState = new QState();
    m_SoftSwitchMonitorState->setObjectName("SoftSwitchMonitorState");
    m_InitState = new QState();
    m_InitState->setObjectName("InitState");
    m_idleState = new QState();
    m_idleState->setObjectName("IdleState");
    m_busyState = new QState();
    m_busyState->setObjectName("BusyState");

    m_errorState = new QState();
    m_errorState->setObjectName("ErrorState");
    m_normalState = new QState();
    m_normalState->setObjectName("NormalState");

    m_operationMachine.addState(m_DefaultState);
    m_operationMachine.addState(m_SoftSwitchMonitorState);
    m_operationMachine.addState(m_InitState);
    m_operationMachine.addState(m_idleState);
    m_operationMachine.addState(m_busyState);

    m_availabilityMachine.addState(m_errorState);
    m_availabilityMachine.addState(m_normalState);

//    QAbstractTransition *transition = m_idleState->addTransition(m_errorState);
    m_DefaultState->addTransition(this, SIGNAL(softSwitchMonitorStart()), m_SoftSwitchMonitorState);
    m_SoftSwitchMonitorState->addTransition(this, SIGNAL(softSwitchPressed()), m_InitState);
    m_InitState->addTransition(this, SIGNAL(initComplete()), m_idleState);
    m_busyState->addTransition(this, SIGNAL(enterIdleState()), m_idleState);
    m_idleState->addTransition(this, SIGNAL(enterBusyState()), m_busyState);

    m_errorState->addTransition(this, SIGNAL(enterNormalState()), m_normalState);
    m_normalState->addTransition(this, SIGNAL(enterErrorState()), m_errorState);

    m_operationMachine.setInitialState(m_DefaultState);
    m_availabilityMachine.setInitialState(m_normalState);

//    connect(m_idleState, SIGNAL(stateChanged()), this, SLOT(onStateChanged()));
//    connect(m_busyState, SIGNAL(stateChanged()), this, SLOT(onStateChanged()));

    connect(m_idleState, SIGNAL(entered()), this, SLOT(onOperationStateChanged()));
    connect(m_busyState, SIGNAL(entered()), this, SLOT(onOperationStateChanged()));
    connect(m_errorState, SIGNAL(entered()), this, SLOT(onAvailabilityStateChanged()));
    connect(m_normalState, SIGNAL(entered()), this, SLOT(onAvailabilityStateChanged()));

//    connect(m_errorState, SIGNAL(entered()), this, SLOT(onErrorState()));
//    connect(m_normalState, SIGNAL(entered()), this, SLOT(onNormalState()));

    m_operationMachine.start();
    m_availabilityMachine.start();
}

/****************************************************************************/
StateHandler::~StateHandler()
{
}

bool StateHandler::isAllowed(Global::CommandShPtr_t command)
{
    bool retVal = true;

    foreach (QAbstractState* state, m_operationMachine.configuration())
    {
        qDebug()<<"Current State" << state->objectName();
        if (!command.GetPointerToUserData()->isStateAllowed(state->objectName()))
            retVal = false;
    }

    foreach (QAbstractState* state, m_availabilityMachine.configuration())
    {
        if (!command.GetPointerToUserData()->isStateAllowed(state->objectName()))
            retVal = false;
    }

    return retVal;
}

QString StateHandler::getCurrentOperationState()
{
    if (!m_operationMachine.configuration().empty())
    {
        QAbstractState *state = m_operationMachine.configuration().toList().at(0);
        return state->objectName();
    }
    return "";
}

QString StateHandler::getCurrentAvailabilityState()
{
    if (!m_availabilityMachine.configuration().empty())
    {
        QAbstractState *state = m_availabilityMachine.configuration().toList().at(0);
        return state->objectName();
    }
    return "";
}

void StateHandler::setActivityUpdate(bool active, quint32 activityId)
{
    if (active)
    {
        m_rackList.append(activityId);
        if (m_rackList.count() == 1)
            emit enterBusyState();
    }
    else
    {
        m_rackList.removeOne(activityId);
        if (m_rackList.count() == 0)
            emit enterIdleState();
    }
}


void StateHandler::setInitState()
{
    if (getCurrentOperationState() == "SoftSwitchMonitorState") {
        emit softSwitchPressed();
    }
    qDebug()<<"Current state" << getCurrentOperationState();
}

void StateHandler::setIdleState()
{
    qDebug()<<"Settin Idle state"<<getCurrentOperationState();
    if (getCurrentOperationState() == "InitState") {
        emit initComplete();
        qDebug()<<"INIT COMPLETE RECEIVERS: "<<receivers(SIGNAL(initComplete()));
        qDebug()<<"THREAD ID"<<this->thread();
    }
}

void StateHandler::setStateToSoftSwitchMonitorState()
{
    if (getCurrentOperationState() == "DefaultState") {
        emit softSwitchMonitorStart();
    }
}

void StateHandler::setAvailability(bool active, quint32 eventId)
{
    if (active)
    {
        m_errorList.append(eventId);
        if (m_errorList.count() == 1)
        {
            Global::EventObject::Instance().RaiseEvent(EVENT_ERROR_STATE);
            emit enterErrorState();
        }
    }
    else
    {
        m_errorList.removeOne(eventId);
        if (m_errorList.count() == 0)
        {
            Global::EventObject::Instance().RaiseEvent(EVENT_NORMAL_STATE);
            emit enterNormalState();
        }
    }
}

void StateHandler::onAvailabilityStateChanged()
{
    QString state = getCurrentAvailabilityState();
    emit stateChanged(state);
}

void StateHandler::onOperationStateChanged()
{
    QString state = getCurrentOperationState();
    emit stateChanged(state);
}

//void StateHandler::onNormalState()
//{
//    DeviceCommandProcessor::CmdExecutionStateChanged *p_CmdExecutionState = new DeviceCommandProcessor::CmdExecutionStateChanged(DeviceControl::DEVICE_INSTANCE_ID_GRAPPLER_1);
//    p_CmdExecutionState->m_Stop = false;
////    p_CmdExecutionState->m_eventId = Cmd.m_eventId;
//    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(p_CmdExecutionState));

//    NetCommands::CmdExecutionStateChanged *p_CmdExecutionStateGUI = new NetCommands::CmdExecutionStateChanged(1000);
//    p_CmdExecutionStateGUI->m_Stop = false;
//    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(p_CmdExecutionStateGUI));
//}

//void StateHandler::onErrorState()
//{
//    DeviceCommandProcessor::CmdExecutionStateChanged *p_CmdExecutionState = new DeviceCommandProcessor::CmdExecutionStateChanged(DeviceControl::DEVICE_INSTANCE_ID_GRAPPLER_1);
//    p_CmdExecutionState->m_Stop = true;
////    p_CmdExecutionState->m_eventId = Cmd.m_eventId;
//    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(p_CmdExecutionState));

//    NetCommands::CmdExecutionStateChanged *p_CmdExecutionStateGUI = new NetCommands::CmdExecutionStateChanged(1000);
//    p_CmdExecutionStateGUI->m_Stop = true;
//    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(p_CmdExecutionStateGUI));
//}

//ColoradoStartupState::ColoradoStartupState(ColoradoThreadController *threadController)
//    : QState()
//    , m_threadController(threadController)
//{

//}

//void ColoradoStartupState::onEntry(QEvent* event)
//{
//    // doing reference runs

//    qDebug() << "ColoradoStartupState::onEntry" << this->objectName();
////    emit SIGNAL(stateChanged());
//}

//void ColoradoStartupState::onExit(QEvent* event)
//{
//    qDebug() << "ColoradoState::onExit" << this->objectName();
////    emit SIGNAL(stateChanged());
//}

} // end namespace Colorado
