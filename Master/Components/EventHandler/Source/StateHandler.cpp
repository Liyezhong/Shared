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
#include <QReadLocker>
#include <QWriteLocker>

namespace EventHandler {


/****************************************************************************/
StateHandler::StateHandler()
    : m_SimulationOn(false)
    , m_swInitFailed(false)
    , m_powerFailStage(Global::POWER_FAIL_NONE)
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
    mp_idleState->addTransition(this, SIGNAL(enterErrorState()), mp_busyState);

    mp_errorState->addTransition(this, SIGNAL(enterNormalState()), mp_normalState);
    mp_normalState->addTransition(this, SIGNAL(enterErrorState()), mp_errorState);
    mp_errorState->addTransition(this, SIGNAL(enterBusyState()), mp_normalState);

    m_operationMachine.setInitialState(mp_DefaultState);
    m_availabilityMachine.setInitialState(mp_normalState);

    connect(mp_SoftSwitchMonitorState, SIGNAL(entered()), this, SLOT(onSoftSwitchMonitorStateEntered()));
    connect(mp_InitState, SIGNAL(entered()), this, SLOT(onInitStateEntered()));
    connect(mp_InitState, SIGNAL(entered()), this, SLOT(onOperationStateChanged()));
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
    QMutexLocker locker(&m_Mutex);
    bool retVal = true;
    const QSet<QAbstractState*>& operationStateSet = m_operationMachine.configuration();
    foreach (QAbstractState* state, operationStateSet)
    {
       if (!command.GetPointerToUserData()->isStateAllowed(state->objectName())) {
           retVal = false;
       }
    }

    const QSet<QAbstractState*>& availabilityStateSet = m_availabilityMachine.configuration();
    foreach (QAbstractState* state, availabilityStateSet)
    {
       if (!command.GetPointerToUserData()->isStateAllowed(state->objectName()))
           retVal = false;
    }
    return retVal;
}

QString StateHandler::getCurrentOperationState()
{
    QMutexLocker locker(&m_Mutex);
    if (!m_operationMachine.configuration().empty())
    {
        QAbstractState *state = m_operationMachine.configuration().toList().at(0);
        return state->objectName();
    }
    return "";
}

QString StateHandler::getCurrentAvailabilityState()
{
    QMutexLocker locker(&m_Mutex);
    if (!m_availabilityMachine.configuration().empty())
    {
        QAbstractState *state = m_availabilityMachine.configuration().toList().at(0);
        return state->objectName();
    }
    return "";
}

void StateHandler::setActivityUpdate(bool active, quint32 activityId)
{
    Q_UNUSED(activityId)
    if (active)
    {
        emit enterBusyState();
    }
    else
    {
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
    if (getCurrentOperationState() == "InitState") {
        emit initComplete();
    }
}

quint8 StateHandler::getInitStageProgress()
{
    return m_initStage.count();
}

void StateHandler::setInitStageProgress(quint8 stage, bool success, bool restricted)
{
    if (success)
    {
        // do we have restrictions limiting the later usage of the instrument ?
        if (restricted) {
            m_stageRestrictions.insert(stage, restricted);
        }

        if (restricted) {
            Global::EventObject::Instance().RaiseEvent(EVENT_INIT_RESTRICTED_STAGE_SUCCESS, Global::FmtArgs() << stage);
        }
        else {
            Global::EventObject::Instance().RaiseEvent(EVENT_INIT_STAGE_SUCCESS, Global::FmtArgs() << stage);
        }
        if (getCurrentOperationState() != "InitState") {
            emit enterInitState();
        }
        if ((stage != 1) || (!m_swInitFailed)) {
            m_initStage.insert(stage, success);
            if (m_initStage.count() == 3)
            {
                if ((m_stageRestrictions.value(1, false)) || (m_stageRestrictions.value(2, false)) || (m_stageRestrictions.value(3, false))) {
                    Global::EventObject::Instance().RaiseEvent(EVENT_INIT_RESTRICTED_SUCCESS);
                }
                else {
                    Global::EventObject::Instance().RaiseEvent(EVENT_INIT_SUCCESS);
                }
                m_initStage.clear();
                m_stageRestrictions.clear();
                setIdleState();
            }
        }
    }
    else
    {
        Global::EventObject::Instance().RaiseEvent(EVENT_INIT_STAGE_FAILURE, Global::FmtArgs() << stage);
        if (stage == 1) {
            m_swInitFailed = true;
        }
        m_initStage.remove(stage);
        setAvailability(true, EVENT_INIT_FAILED);
        SetInitializationFailed();
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
            Global::EventObject::Instance().RaiseEvent(EVENT_ERROR_STATE, Global::FmtArgs() << eventId);
            emit enterErrorState();
        }
    }
    else
    {
        m_errorList.removeOne(eventId);
        m_errorList.clear();
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

void StateHandler::onInitStateEntered()
{
    emit enteredInitState();
}

void StateHandler::onSoftSwitchMonitorStateEntered()
{
    emit enteredSoftSwitchMonitorState();
}

void StateHandler::SetInitializationFailed()
{
    emit initFailed();
}

} // end namespace Colorado
