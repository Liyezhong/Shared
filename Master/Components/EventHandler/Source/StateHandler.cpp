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


namespace EventHandler {

StateHandler StateHandler::m_StateHandlerInstance; //!< The instance

/****************************************************************************/
StateHandler::StateHandler()
    : m_SimulationOn(false)
    , m_swInitFailed(false)
    , m_Lock(QMutex::Recursive)
{
    mp_DefaultState = new QState();
    mp_DefaultState->setObjectName("DefaultState");
    mp_SoftSwitchMonitorState = new QState();
    mp_SoftSwitchMonitorState->setObjectName("SoftSwitchMonitorState");
    mp_InitState = new QState();
    mp_InitState->setObjectName("InitState");


    mp_idleState = new QState();
    mp_idleState->setObjectName("IdleState");

    mp_busyState = new QState();
    mp_busyState->setObjectName("BusyState");

    mp_InitFailedState = new QState();
    mp_InitFailedState->setObjectName("InitFailedState");

    mp_errorState = new QState();
    mp_errorState->setObjectName("ErrorState");
    mp_normalState = new QState();
    mp_normalState->setObjectName("NormalState");

    m_operationMachine.addState(mp_DefaultState);
    m_operationMachine.addState(mp_SoftSwitchMonitorState);
    m_operationMachine.addState(mp_InitState);
    m_operationMachine.addState(mp_idleState);
    m_operationMachine.addState(mp_busyState);
    m_operationMachine.addState(mp_InitFailedState);

    m_availabilityMachine.addState(mp_errorState);
    m_availabilityMachine.addState(mp_normalState);

    mp_DefaultState->addTransition(this, SIGNAL(softSwitchMonitorStart()), mp_SoftSwitchMonitorState);
    mp_SoftSwitchMonitorState->addTransition(this, SIGNAL(softSwitchPressed()), mp_InitState);
    mp_InitState->addTransition(this, SIGNAL(initComplete()), mp_idleState);
    mp_InitState->addTransition(this, SIGNAL(initFailed()), mp_InitFailedState);
    mp_busyState->addTransition(this, SIGNAL(enterIdleState()), mp_idleState);
    mp_idleState->addTransition(this, SIGNAL(enterBusyState()), mp_busyState);
    mp_idleState->addTransition(this, SIGNAL(enterInitState()), mp_InitState);

    mp_errorState->addTransition(this, SIGNAL(enterNormalState()), mp_normalState);
    mp_normalState->addTransition(this, SIGNAL(enterErrorState()), mp_errorState);

    m_operationMachine.setInitialState(mp_DefaultState);
    m_availabilityMachine.setInitialState(mp_normalState);

    connect(mp_SoftSwitchMonitorState, SIGNAL(entered()), this, SLOT(onSoftSwitchMonitorStateEntered()));
    connect(mp_InitState, SIGNAL(entered()), this, SLOT(onInitStateEntered()));
    connect(mp_idleState, SIGNAL(entered()), this, SLOT(onOperationStateChanged()));
    connect(mp_busyState, SIGNAL(entered()), this, SLOT(onOperationStateChanged()));
    connect(mp_errorState, SIGNAL(entered()), this, SLOT(onAvailabilityStateChanged()));
    connect(mp_normalState, SIGNAL(entered()), this, SLOT(onAvailabilityStateChanged()));

    m_operationMachine.start();
    m_availabilityMachine.start();
}

/****************************************************************************/
StateHandler::~StateHandler()
{
}

bool StateHandler::isAllowed(Global::CommandShPtr_t command)
{
    QMutexLocker Lock(&m_Lock);
    bool retVal = true;

    foreach (QAbstractState* state, m_operationMachine.configuration())
    {
        if (!command.GetPointerToUserData()->isStateAllowed(state->objectName())) {
            retVal = false;
        }
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
    QMutexLocker Lock(&m_Lock);
    if (!m_operationMachine.configuration().empty())
    {
        QAbstractState *state = m_operationMachine.configuration().toList().at(0);
        return state->objectName();
    }
    return "";
}

QString StateHandler::getCurrentAvailabilityState()
{
    QMutexLocker Lock(&m_Lock);
    if (!m_availabilityMachine.configuration().empty())
    {
        QAbstractState *state = m_availabilityMachine.configuration().toList().at(0);
        return state->objectName();
    }
    return "";
}

void StateHandler::setActivityUpdate(bool active, quint32 activityId)
{
    QMutexLocker Lock(&m_Lock);
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
    QMutexLocker Lock(&m_Lock);
    if (getCurrentOperationState() == "SoftSwitchMonitorState") {
        emit softSwitchPressed();
    }
    qDebug()<<"Current state" << getCurrentOperationState();
}

void StateHandler::setIdleState()
{
    QMutexLocker Lock(&m_Lock);
    if (getCurrentOperationState() == "InitState") {
        emit initComplete();
    }
}

quint8 StateHandler::getInitStageProgress()
{
    QMutexLocker Lock(&m_Lock);
    return m_initStage.count();
}

void StateHandler::setInitStageProgress(quint8 stage, bool success)
{
    QMutexLocker Lock(&m_Lock);
    if (success)
    {
        if (getCurrentOperationState() != "InitState") {
            emit enterInitState();
        }
        if ((stage != 1) || (!m_swInitFailed)) {
            m_initStage.insert(stage, success);
            if (m_initStage.count() == 3)
            {
                m_initStage.clear();
                Global::EventObject::Instance().RaiseEvent(EVENT_INIT_SUCCESS);
                setIdleState();
            }
        }
    }
    else
    {
        if (stage == 1) {
            m_swInitFailed = true;
        }
        m_initStage.remove(stage);
        setAvailability(true, EVENT_INIT_FAILED);
    }
}


void StateHandler::setStateToSoftSwitchMonitorState()
{
    QMutexLocker Lock(&m_Lock);
    if (getCurrentOperationState() == "DefaultState") {
        emit softSwitchMonitorStart();
    }
}

void StateHandler::setAvailability(bool active, quint32 eventId)
{
    QMutexLocker Lock(&m_Lock);
    if (active)
    {
        m_errorList.append(eventId);
        if (m_errorList.count() == 1)
        {
            Global::EventObject::Instance().RaiseEvent(EVENT_ERROR_STATE, Global::FmtArgs() << eventId);
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
    QMutexLocker Lock(&m_Lock);
    QString state = getCurrentAvailabilityState();
    emit stateChanged(state);
}

void StateHandler::onOperationStateChanged()
{
    QMutexLocker Lock(&m_Lock);
    QString state = getCurrentOperationState();
    emit stateChanged(state);
}

void StateHandler::onInitStateEntered()
{
    QMutexLocker Lock(&m_Lock);
    emit enteredInitState();
}

void StateHandler::onSoftSwitchMonitorStateEntered()
{
    QMutexLocker Lock(&m_Lock);
    emit enteredSoftSwitchMonitorState();
}

void StateHandler::SetInitializationFailed()
{
    QMutexLocker Lock(&m_Lock);
    emit initFailed();
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

//HimalayaStartupState::HimalayaStartupState(HimalayaThreadController *threadController)
//    : QState()
//    , m_threadController(threadController)
//{

//}

//void HimalayaStartupState::onEntry(QEvent* event)
//{
//    // doing reference runs

//    qDebug() << "HimalayaStartupState::onEntry" << this->objectName();
////    emit SIGNAL(stateChanged());
//}

//void HimalayaStartupState::onExit(QEvent* event)
//{
//    qDebug() << "HimalayaState::onExit" << this->objectName();
////    emit SIGNAL(stateChanged());
//}

} // end namespace Himalaya
