/****************************************************************************/
/*! \file SoftSwitchManager.cpp
 *
 *  \brief SoftSwitchManager Implementation
 *
 *   $Version: $ 0.1
 *   $Date:    $ 07.01.2013
 *   $Author:  $ N.Kamath
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
#include <SoftSwitchManager/Include/SoftSwitchManager.h>
#include <EventHandler/Include/StateHandler.h>
#include <SoftSwitchManager/Include/GPIOManagerEventCodes.h>

#include <unistd.h> //Provides STDIN_FILENO
#include <QTextStream>

namespace SoftSwitchManager {

const quint32 SOFT_SWITCH_GPIO_NUMBER   = 0;    //!< GPIO1_0 of the processor (i.Mx35 EBox)
const quint32 DIRECTION_IS_INPUT        = 0;    //!< Value for input direction of GPIO
const quint32 TIMER_INTERVAL            = 7000; //!< Timer times out after 7 seconds

const QString DefaultState                      =   "DefaultState"; //!< Default State string
const QString PressedAtSoftSwitchMonitorState   =   "PressedAtBasicInitState";
const QString PressedAtIdleState                =   "PressedAtIdleState";
const QString PressedAtBusyState                =   "PressedAtBusyState";
const QString CriticalActionCheckState          =   "CriticalActionStateCheckState";
const QString CriticalActionState               =   "CriticalActionState";
const QString ShutdownState                     =   "ShutdownState";

SoftSwitchMgr::SoftSwitchMgr(QObject *p_Parent)
    :m_SoftSwitchGPIO(SOFT_SWITCH_GPIO_NUMBER, DIRECTION_IS_INPUT)
    ,mp_DefaultToInitTransition(NULL)
    ,mp_SoftSwitchPressedNotifier(NULL)
{
    //!< This is required so that our object is on the right thread
    setParent(p_Parent);
    Init();
}

void SoftSwitchMgr::Init()
{
    QFile *File = new QFile(this);
    File->open(stdin,QIODevice::ReadOnly);
    mp_SoftSwitchPressedNotifier = new QSocketNotifier(File->handle(), QSocketNotifier::Read);
    mp_SoftSwitchPressedNotifier->setParent(this);
    //!< Create Seven Second timer
    mp_Timer = new QTimer(this);
    mp_Timer->setInterval(TIMER_INTERVAL);
    CONNECTSIGNALSLOT(mp_Timer, timeout(), this, ResetStateMachine());

    //!< create poll timer
    mp_PollTimer = new QTimer(this);
    mp_PollTimer->setInterval(2000);
    CONNECTSIGNALSLOT(mp_PollTimer, timeout(), this, ActivatePolling());

    //!< Create StateMachine
    mp_SoftSwitchStateMachine = new QStateMachine();
    /*! Parent is set so that the statemachine and its states are in the right thread object.
     *  \warning : Set Parent to objects created in heap in this function (or before this
     *  Object is moved to the Thread)
     */
    mp_SoftSwitchStateMachine->setParent(this);

    //!< Create States
    mp_DefaultState = new GenericStateTemplate(*this, &SoftSwitchMgr::OnDefaultStateEntered);
    mp_DefaultState->setParent(this);
    mp_DefaultState->setObjectName(DefaultState);

    mp_PressedAtSoftSwitchMonitorState = new GenericStateTemplate(*this, &SoftSwitchMgr::OnPressedAtInitStateEntered);
    mp_PressedAtSoftSwitchMonitorState->setParent(this);
    mp_PressedAtSoftSwitchMonitorState->setObjectName(PressedAtSoftSwitchMonitorState);

    mp_PressedAtBusyState = new GenericStateTemplate(*this, &SoftSwitchMgr::OnPressedAtBusyStateEntered);
    mp_PressedAtBusyState->setParent(this);
    mp_PressedAtBusyState->setObjectName(PressedAtBusyState);

    mp_PressedAtIdleState = new GenericStateTemplate(*this, &SoftSwitchMgr::OnPressedAtIdleStateEntered);
    mp_PressedAtIdleState->setParent(this);
    mp_PressedAtIdleState->setObjectName(PressedAtIdleState);

    mp_CriticalActionCheckState = new GenericStateTemplate(*this, &SoftSwitchMgr::OnCriticalActionCheckStateEntered);
    mp_CriticalActionCheckState->setParent(this);
    mp_CriticalActionCheckState->setObjectName(CriticalActionCheckState);

    mp_CriticalActionState = new GenericStateTemplate(*this, &SoftSwitchMgr::OnCriticalActionStateEntered);
    mp_CriticalActionState->setParent(this);
    mp_CriticalActionState->setObjectName(CriticalActionState);

    mp_ShutDownState = new GenericStateTemplate(*this, &SoftSwitchMgr::OnShutDownStateEntered);
    mp_ShutDownState->setParent(this);
    mp_ShutDownState->setObjectName(ShutdownState);

    //! Build the state machine
    mp_SoftSwitchStateMachine->addState(mp_DefaultState);
    mp_SoftSwitchStateMachine->addState(mp_PressedAtSoftSwitchMonitorState);
    mp_SoftSwitchStateMachine->addState(mp_PressedAtIdleState);
    mp_SoftSwitchStateMachine->addState(mp_PressedAtBusyState);
    mp_SoftSwitchStateMachine->addState(mp_CriticalActionState);
    mp_SoftSwitchStateMachine->addState(mp_ShutDownState);

    mp_SoftSwitchStateMachine->setInitialState(mp_DefaultState);
    //!< Add Transition between states of the state machine
    mp_DefaultToInitTransition = new SoftSwitchStateTransition(this, SIGNAL(SoftSwitchPressed()),
                                                               *this, &SoftSwitchMgr::IsSystemStateSoftSwitchMonitor,
                                                               mp_PressedAtSoftSwitchMonitorState);
    mp_DefaultToInitTransition->setParent(this);
    mp_DefaultState->addTransition(mp_DefaultToInitTransition);

    mp_PressedAtSoftSwitchMonitorState->addTransition(this, SIGNAL(SystemInitComplete()), mp_DefaultState);

    mp_DefaultToIdleTransition = new SoftSwitchStateTransition(this, SIGNAL(SoftSwitchPressed()),
                                                               *this, &SoftSwitchMgr::CheckIfDeviceIsIdle,
                                                               mp_PressedAtIdleState);
    mp_DefaultToIdleTransition->setParent(this);
    mp_DefaultState->addTransition(mp_DefaultToIdleTransition);

    mp_DefaultToBusyTransition = new SoftSwitchStateTransition(this, SIGNAL(SoftSwitchPressed()),
                                                               *this, &SoftSwitchMgr::CheckIfDeviceIsBusy,
                                                               mp_PressedAtBusyState);
    mp_DefaultToBusyTransition->setParent(this);
    mp_DefaultState->addTransition(mp_DefaultToBusyTransition);

    mp_PressedAtIdleState->addTransition(this, SIGNAL(SoftSwitchPressed()), mp_ShutDownState);
    mp_PressedAtBusyState->addTransition(this, SIGNAL(SoftSwitchPressed()), mp_CriticalActionCheckState);
    mp_CriticalActionCheckState->addTransition(this, SIGNAL(CriticalActionNotInProgress()), mp_ShutDownState);
    mp_CriticalActionCheckState->addTransition(this, SIGNAL(CriticalActionInProgress()), mp_CriticalActionState);
    mp_CriticalActionState->addTransition(this, SIGNAL(CriticalActionComplete()), mp_ShutDownState);

    /*! Set State to default state if the timer times out. Usually this happens when
     *  User does not press the softswitch for the second time within 7 seconds; Rarely
     *  this can happen if critical action details are not available.
     */
    mp_PressedAtIdleState->addTransition(this, SIGNAL(TimerTimeOut()), mp_DefaultState);
    mp_PressedAtBusyState->addTransition(this, SIGNAL(TimerTimeOut()), mp_DefaultState);
    mp_CriticalActionState->addTransition(this, SIGNAL(TimerTimeOut()), mp_DefaultState);
    mp_CriticalActionCheckState->addTransition(this, SIGNAL(TimerTimeOut()), mp_DefaultState);

    //!< Finally! start the StateMachine
    mp_SoftSwitchStateMachine->start();
    qDebug()<< "State Machine error " <<mp_SoftSwitchStateMachine->error();
    qDebug()<<"Running state" <<mp_SoftSwitchStateMachine->isRunning();
    qDebug()<<"SoftSwitchMgr::Init()" << mp_SoftSwitchStateMachine->thread();
}

void SoftSwitchMgr::ConnectSignals()
{
    EventHandler::StateHandler *p_StateHandler = &EventHandler::StateHandler::Instance();
    connect(p_StateHandler, SIGNAL(initComplete()), this, SIGNAL(SystemInitComplete()));
}

SoftSwitchMgr::~SoftSwitchMgr()
{
    /*! State machine objects are not deleted
     *  Why?
     *  we have set Parent as "this" object and when parent goes out
     *  of scope qt's dynamic object management will take care of deleting
     *  child objects :)
     */
}

bool SoftSwitchMgr::CheckIfDeviceIsBusy(QEvent *p_Event) {
    qDebug()<<"Testing If Device Is Busy";
    QString CurrentSytemState = EventHandler::StateHandler::Instance().getCurrentOperationState();
    qDebug()<<"Device State" << CurrentSytemState;
    if (QString::compare(CurrentSytemState, "BusyState") == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool SoftSwitchMgr::CheckIfDeviceIsIdle(QEvent *p_Event) {
    qDebug()<<"Testing If Device Is Idle";
    QString CurrentSytemState = EventHandler::StateHandler::Instance().getCurrentOperationState();
    qDebug()<<"Device State" << CurrentSytemState;
    if (QString::compare(CurrentSytemState, "IdleState") == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool SoftSwitchMgr::IsSystemStateSoftSwitchMonitor(QEvent *p_Event) {
    qDebug()<<"Testing If Device Is In Init";
    QString CurrentSytemState = EventHandler::StateHandler::Instance().getCurrentOperationState();
    qDebug()<<"Device State" << CurrentSytemState;
    if (QString::compare(CurrentSytemState, "SoftSwitchMonitorState") == 0) {
        return true;
    }
    else {
        return false;
    }
}

void SoftSwitchMgr::OnDefaultStateEntered()
{
    /*!< When we enter Default state , first check we came here as a result of timeout, if so
     *  we have to close any msg boxes open in GUI.
     */
    if (m_CurrentState == PressedAtIdleState) {
        mp_Timer->stop();
        Global::EventObject::Instance().RaiseEvent(EVENT_PRESS_SOFTSWITCH_TO_SHUTDOWN, false);
    }
    else if (m_CurrentState == PressedAtBusyState) {
        mp_Timer->stop();
        Global::EventObject::Instance().RaiseEvent(EVENT_PRESS_SOFTSWITCH_TO_SHUTDOWN_WITH_WARNING, false);
    }

    m_CurrentState = DefaultState;
    qDebug()<<" Default State Entered \n \n";
}

void SoftSwitchMgr::OnPressedAtInitStateEntered()
{
    m_CurrentState = PressedAtSoftSwitchMonitorState;
    qDebug()<<" SoftSwitch PressedAt Init State Entered \n \n";
    qDebug()<<"State thread" << mp_PressedAtSoftSwitchMonitorState->thread();
    EventHandler::StateHandler::Instance().setInitState();
    emit SendSoftSwitchPressedCmd();
}

void SoftSwitchMgr::OnPressedAtIdleStateEntered()
{
    //! Start timer. Will internally check if timer is already running,if so will stop and restarts the timer.
    mp_Timer->start();
    m_CurrentState = PressedAtIdleState;
    Global::EventObject::Instance().RaiseEvent(EVENT_PRESS_SOFTSWITCH_TO_SHUTDOWN);
    qDebug()<<" SoftSwitch PressedAt Idle State Entered \n \n";
}

void SoftSwitchMgr::OnPressedAtBusyStateEntered()
{
    //! Start timer. Will internally check if timer is already running,if so will stop and restarts the timer.
    mp_Timer->start();
    m_CurrentState = PressedAtBusyState;
    Global::EventObject::Instance().RaiseEvent(EVENT_PRESS_SOFTSWITCH_TO_SHUTDOWN_WITH_WARNING);
    qDebug()<<" SoftSwitch PressedAt Busy State Entered \n \n";
}

void SoftSwitchMgr::OnCriticalActionCheckStateEntered()
{
    //! Start timer. Will internally check if timer is already running,if so will stop and restarts the timer.
    mp_Timer->start();
    m_CurrentState = CriticalActionCheckState;
    qDebug()<<"CriticalActionCheckState Entered \n\n";
}

void SoftSwitchMgr::OnCriticalActionStateEntered()
{
    //! Start timer. Will internally check if timer is already running,if so will stop and restarts the timer.
    //mp_Timer->start();
    m_CurrentState = CriticalActionState;
    Global::EventObject::Instance().RaiseEvent(EVENT_CRITICAL_ACTION_WARNING_MESG);
    qDebug()<<" SoftSwitch PressedAt Critical State Entered \n \n";
}

void SoftSwitchMgr::OnShutDownStateEntered()
{
    m_CurrentState = ShutdownState;
    qDebug()<<" SoftSwitch PressedAt Shutdown State Entered \n \n";
}

void SoftSwitchMgr::OnSoftSwitchPressed(int GpioFd)
{
    if (mp_SoftSwitchPressedNotifier) {
        mp_SoftSwitchPressedNotifier->setEnabled(false);
        mp_PollTimer->start();
    }
    QTextStream qin(stdin);
    qin.readLine();
    qDebug()<<"Soft Switch Pressed";
    emit SoftSwitchPressed();
}

void SoftSwitchMgr::ResetStateMachine()
{
    //!< This would put the system to default state.
    emit TimerTimeOut();
}

void SoftSwitchMgr::ActivatePolling()
{
    mp_PollTimer->stop();
    if (mp_SoftSwitchPressedNotifier) {
        mp_SoftSwitchPressedNotifier->setEnabled(true);
    }
}

}// End of namespace SoftSwitchManager
