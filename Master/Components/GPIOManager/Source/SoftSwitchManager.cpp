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
#include <GPIOManager/Include/SoftSwitchManager.h>
#include <EventHandler/Include/StateHandler.h>
#include <GPIOManager/Include/GPIOManagerEventCodes.h>
#include <Global/Include/Utils.h>
#include <unistd.h> //Provides STDIN_FILENO
#include <Global/Include/UITranslator.h>

namespace GPIOManager {

const quint32 SOFT_SWITCH_GPIO_NUMBER   = 0;    //!< GPIO1_0 of the processor (i.Mx35 EBox)
const quint32 TIMER_INTERVAL            = 7000; //!< Timer times out after 7 seconds

const QString DefaultState                      =   "DefaultState"; //!< Default State string
const QString PressedAtSoftSwitchMonitorState   =   "PressedAtSoftSwitchMonitorState"; //!< SoftSwitch Monitor state string
const QString PressedAtInitState                =   "PressedAtInitState"; //!< Init state string
const QString PressedAtIdleState                =   "PressedAtIdleState"; //!< Idle state string
const QString PressedAtBusyState                =   "PressedAtBusyState"; //!< Busy state string
const QString CriticalActionCheckState          =   "CriticalActionStateCheckState"; //!< critical action state check string
const QString CriticalActionState               =   "CriticalActionState"; //!< Critical action state string
const QString ShutdownState                     =   "ShutdownState"; //!< shutdown state string

/*******************************************************************************/
/*!
 *  \brief    Constructor
 *  \iparam p_Parent    = Parent object
 *  \iparam ServiceMode = true indicates system is in service mode , else false.
 */
/********************************************************************************/
SoftSwitchMgr::SoftSwitchMgr(QObject *p_Parent, bool ServiceMode)
    : m_SoftSwitchGPIO(SOFT_SWITCH_GPIO_NUMBER, DIRECTION_IS_INPUT)
    , mp_SoftSwitchStateMachine(NULL)
    , mp_DefaultState(NULL)
    , mp_PressedAtSoftSwitchMonitorState(NULL)
    , mp_PressedAtInitState(NULL)
    , mp_PressedAtIdleState(NULL)
    , mp_PressedAtBusyState(NULL)
    , mp_ShutDownState(NULL)
    , mp_CriticalActionState(NULL)
    , mp_CriticalActionCheckState(NULL)
    , mp_DefaultToSoftSwitchMonitorTransition(NULL)
    , mp_DefaultToInitTransition(NULL)
    , mp_SoftSwitchMonitorToInitTransition(NULL)
    , mp_InitToDefaultTransition(NULL)
    , mp_InitSelfTransition(NULL)
    , mp_DefaultToIdleTransition(NULL)
    , mp_AlternativeDefaultToIdleTransition(NULL)
    , mp_DefaultToBusyTransition(NULL)
    , mp_IdleToShutDownTransition(NULL)
    , mp_CriticalCheckToShutDownTransition(NULL)
    , mp_Timer(NULL)
    , m_CurrentState("")
    , m_Fd(-1)
    , m_SoftSwitchPressed(false)
    , m_UserActionStarted(false)
    , m_ServiceMode(ServiceMode)
    , m_UserActionID(0)
{
    //! This is required so that our object is on the right thread, i.e
    //! GPIO thread
    setParent(p_Parent);
#if defined(__arm__) || defined(__TARGET_ARCH_ARM) || defined(_M_ARM)
    m_Fd = m_SoftSwitchGPIO.GetGpioFd();
    m_SoftSwitchGPIO.SetEdge("falling");
#endif
    if (!ServiceMode) {
        Init();
    }
}

/****************************************************************************/
/*!
 *  \brief  Initialize the Object. Creates, Initializes & Starts statemachine
 */
/****************************************************************************/
void SoftSwitchMgr::Init()
{
    //! Create Seven Second timer
    mp_Timer = new QTimer(this);
    mp_Timer->setInterval(TIMER_INTERVAL);

    //! Used to handle switch debounce
    m_StabilityTimer.setInterval(500);
    m_StabilityTimer.setSingleShot(true);
    CONNECTSIGNALSLOT(mp_Timer, timeout(), this, ResetStateMachine());

    //! Create StateMachine
    mp_SoftSwitchStateMachine = new QStateMachine();
    /*! Parent is set so that the statemachine and its states are in the right thread object.
     *  \warning : Set Parent to objects created in heap in this function (or before this
     *  Object is moved to the Thread)
     */
    mp_SoftSwitchStateMachine->setParent(this);

    //! Create States
    mp_DefaultState = new GenericState_t(*this, &SoftSwitchMgr::OnDefaultStateEntered);
    mp_DefaultState->setParent(this);
    mp_DefaultState->setObjectName(DefaultState);

    mp_PressedAtSoftSwitchMonitorState = new GenericState_t(*this, &SoftSwitchMgr::OnPressedAtSoftSwitchMonitorStateEntered);
    mp_PressedAtSoftSwitchMonitorState->setParent(this);
    mp_PressedAtSoftSwitchMonitorState->setObjectName(PressedAtSoftSwitchMonitorState);

    mp_PressedAtInitState = new GenericState_t(*this, &SoftSwitchMgr::OnPressedAtInitStateEntered);
    mp_PressedAtInitState->setParent(this);
    mp_PressedAtInitState->setObjectName(PressedAtInitState);

    mp_PressedAtBusyState = new GenericState_t(*this, &SoftSwitchMgr::OnPressedAtBusyStateEntered);
    mp_PressedAtBusyState->setParent(this);
    mp_PressedAtBusyState->setObjectName(PressedAtBusyState);

    mp_PressedAtIdleState = new GenericState_t(*this, &SoftSwitchMgr::OnPressedAtIdleStateEntered);
    mp_PressedAtIdleState->setParent(this);
    mp_PressedAtIdleState->setObjectName(PressedAtIdleState);

    mp_CriticalActionCheckState = new GenericState_t(*this, &SoftSwitchMgr::OnCriticalActionCheckStateEntered);
    mp_CriticalActionCheckState->setParent(this);
    mp_CriticalActionCheckState->setObjectName(CriticalActionCheckState);

    mp_CriticalActionState = new GenericState_t(*this, &SoftSwitchMgr::OnCriticalActionStateEntered);
    mp_CriticalActionState->setParent(this);
    mp_CriticalActionState->setObjectName(CriticalActionState);

    mp_ShutDownState = new GenericState_t(*this, &SoftSwitchMgr::OnShutDownStateEntered);
    mp_ShutDownState->setParent(this);
    mp_ShutDownState->setObjectName(ShutdownState);

    //! Build the state machine
    mp_SoftSwitchStateMachine->addState(mp_DefaultState);
    mp_SoftSwitchStateMachine->addState(mp_PressedAtSoftSwitchMonitorState);
    mp_SoftSwitchStateMachine->addState(mp_PressedAtInitState);
    mp_SoftSwitchStateMachine->addState(mp_PressedAtIdleState);
    mp_SoftSwitchStateMachine->addState(mp_PressedAtBusyState);
    mp_SoftSwitchStateMachine->addState(mp_CriticalActionCheckState);
    mp_SoftSwitchStateMachine->addState(mp_CriticalActionState);
    mp_SoftSwitchStateMachine->addState(mp_ShutDownState);

    mp_SoftSwitchStateMachine->setInitialState(mp_DefaultState);
    //! Add Transition between states of the state machine
    //Default state transitions
    mp_DefaultToSoftSwitchMonitorTransition = new SoftSwitchStateTransition_t(this, SIGNAL(SoftSwitchPressed()),
                                                               *this, &SoftSwitchMgr::IsSystemStateSoftSwitchMonitor,
                                                               mp_PressedAtSoftSwitchMonitorState);
    mp_DefaultToSoftSwitchMonitorTransition->setParent(this);
    mp_DefaultState->addTransition(mp_DefaultToSoftSwitchMonitorTransition);

    mp_DefaultToInitTransition = new SoftSwitchStateTransition_t(this, SIGNAL(SoftSwitchPressed()),
                                                               *this, &SoftSwitchMgr::IsSystemStateInit,
                                                               mp_PressedAtInitState);
    mp_DefaultToInitTransition->setParent(this);
    mp_DefaultState->addTransition(mp_DefaultToInitTransition);

    //SoftSwitch Monitor state transition
    mp_PressedAtSoftSwitchMonitorState->addTransition(this, SIGNAL(EnterDefaultState()), mp_DefaultState);

    //Init state transition
    mp_InitSelfTransition = new SoftSwitchStateTransition_t(this, SIGNAL(SoftSwitchPressed()),
                                                          *this, &SoftSwitchMgr::IsSystemStateInit,
                                                          mp_PressedAtInitState);
    mp_InitSelfTransition->setParent(this);
    mp_PressedAtInitState->addTransition(mp_InitSelfTransition);
    mp_PressedAtInitState->addTransition(this, SIGNAL(SystemInitComplete()), mp_DefaultState);

    mp_DefaultToIdleTransition = new SoftSwitchStateTransition_t(this, SIGNAL(SoftSwitchPressed()),
                                                               *this, &SoftSwitchMgr::CheckIfDeviceIsIdle,
                                                               mp_PressedAtIdleState);
    mp_DefaultToIdleTransition->setParent(this);
    mp_DefaultState->addTransition(mp_DefaultToIdleTransition);

    mp_AlternativeDefaultToIdleTransition = new SoftSwitchStateTransition_t(this, SIGNAL(SoftSwitchPressed()),
                                                               *this, &SoftSwitchMgr::CheckIfDeviceInitFailed,
                                                               mp_PressedAtIdleState);
    mp_AlternativeDefaultToIdleTransition->setParent(this);
    mp_DefaultState->addTransition(mp_AlternativeDefaultToIdleTransition);

    mp_DefaultToBusyTransition = new SoftSwitchStateTransition_t(this, SIGNAL(SoftSwitchPressed()),
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

    //! Finally! start the StateMachine
    mp_SoftSwitchStateMachine->start();
}


/****************************************************************************/
/*!
 *  \brief  Connect Signals required to manage statemachine
 */
/****************************************************************************/
void SoftSwitchMgr::ConnectSignals()
{
    EventHandler::StateHandler *p_StateHandler = &EventHandler::StateHandler::Instance();
    CONNECTSIGNALSIGNAL(p_StateHandler, initComplete(), this, SystemInitComplete());
    CONNECTSIGNALSIGNAL(p_StateHandler, initFailed(), this, SystemInitComplete());
}

/****************************************************************************/
/*!
 *  \brief  Destructor
 */
/****************************************************************************/
SoftSwitchMgr::~SoftSwitchMgr()
{
    /*! State machine objects are not deleted
     *  Why?
     *  we have set Parent as "this" object and when parent goes out
     *  of scope qt's dynamic object management will take care of deleting
     *  child objects :)
     */
}

/****************************************************************************/
/*!
 *  \brief  Check if device is in busy state
 *  \iparam p_Event = Transition Event
 *  \return true if in busy state, else false.
 */
/****************************************************************************/
bool SoftSwitchMgr::CheckIfDeviceIsBusy(QEvent *p_Event) {
    Q_UNUSED(p_Event)
    QString CurrentSytemState = EventHandler::StateHandler::Instance().getCurrentOperationState();
    if (QString::compare(CurrentSytemState, "BusyState") == 0) {
        return true;
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Default to Idle state transition check
 *  \iparam p_Event = Transition Event
 *  \return true if in idle state, else false
 */
/****************************************************************************/
bool SoftSwitchMgr::CheckIfDeviceIsIdle(QEvent *p_Event) {
    Q_UNUSED(p_Event)
    QString CurrentSytemState = EventHandler::StateHandler::Instance().getCurrentOperationState();
    if (QString::compare(CurrentSytemState, "IdleState") == 0) {
        return true;
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Check if device initialization failed
 *  \iparam p_Event = Transition Event
 *  \return true if initialization failed, else false.
 */
/****************************************************************************/
bool SoftSwitchMgr::CheckIfDeviceInitFailed(QEvent *p_Event) {
    Q_UNUSED(p_Event)
    QString CurrentSytemState = EventHandler::StateHandler::Instance().getCurrentOperationState();
    if (QString::compare(CurrentSytemState, "InitFailedState") == 0) {
        return true;
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Check if System State is softswitch monitor
 *  \iparam p_Event = Transition Event
 *  \return true if system state is softswitch monitor, else false.
 */
/****************************************************************************/
bool SoftSwitchMgr::IsSystemStateSoftSwitchMonitor(QEvent *p_Event) {
    Q_UNUSED(p_Event)
    QString CurrentSytemState = EventHandler::StateHandler::Instance().getCurrentOperationState();
    if (QString::compare(CurrentSytemState, "SoftSwitchMonitorState") == 0) {
        return true;
    }
    else {
        return false;
    }
}


/****************************************************************************/
/*!
 *  \brief  Check if System State is init
 *  \iparam p_Event = Transition Event
 *  \return true if system state is init, else false
 */
/****************************************************************************/
bool SoftSwitchMgr::IsSystemStateInit(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    QString CurrentSytemState = EventHandler::StateHandler::Instance().getCurrentOperationState();
    if (QString::compare(CurrentSytemState, "InitState") == 0) {
        return true;
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  OnEntry implmentation for Default state
 */
/****************************************************************************/
void SoftSwitchMgr::OnDefaultStateEntered()
{
    /*! When we enter Default state , first check we came here as a result of timeout, if so
     *  we have to close any msg boxes open in GUI.
     */
    qDebug()<<"Entered Default state , Previous state was" << m_CurrentState<<"\n\n\n";
    if (m_CurrentState == PressedAtIdleState) {
        mp_Timer->stop();
        Global::EventObject::Instance().RaiseEvent(EVENT_PRESS_SOFTSWITCH_TO_SHUTDOWN, false, Global::LOGGING);
    }
    else if (m_CurrentState == PressedAtBusyState) {
        mp_Timer->stop();
        Global::EventObject::Instance().RaiseEvent(EVENT_PRESS_SOFTSWITCH_TO_SHUTDOWN_WITH_WARNING, false, Global::LOGGING);
    }

    m_CurrentState = DefaultState;
    m_SoftSwitchPressed = false;
}

/****************************************************************************/
/*!
 *  \brief  OnEntry implementation for SoftSwitchMonitor State. This Object
 *          puts the System statemachine to Init State
 *
 */
/****************************************************************************/
void SoftSwitchMgr::OnPressedAtSoftSwitchMonitorStateEntered()
{
    qDebug()<<"Entered PressedAtSoftSwitchMonitorState , Previous state was" << m_CurrentState<<"\n\n\n";
    m_CurrentState = PressedAtSoftSwitchMonitorState;
    m_SoftSwitchPressed = false;
    EventHandler::StateHandler::Instance().setInitState();
    emit SendSoftSwitchPressedCmd();
    EnterDefaultState();
}

/****************************************************************************/
/*!
 *  \brief  OnEntry implementation for Init State. This method raises event
 *          to inform user to wait till initialization is complete
 *
 */
/****************************************************************************/
void SoftSwitchMgr::OnPressedAtInitStateEntered()
{
    m_CurrentState = PressedAtInitState;
    if (m_SoftSwitchPressed) {
        m_SoftSwitchPressed = false;
        //! Inform user to press softswitch after initialization
        Global::EventObject::Instance().RaiseEvent(EVENT_SOFTSWITCH_PRESSED_DURING_INIT_STATE);
    }
}

/****************************************************************************/
/*!
 *  \brief  OnEntry implementation for Idle State. This method raises event
 *          to inform user to press softswitch again to shutdown
 *
 */
/****************************************************************************/
void SoftSwitchMgr::OnPressedAtIdleStateEntered()
{
    //! Start timer. Will internally check if timer is already running,if so will stop and restarts the timer.
    mp_Timer->start();
    m_CurrentState = PressedAtIdleState;
    m_SoftSwitchPressed = false;
    Global::EventObject::Instance().RaiseEvent(EVENT_PRESS_SOFTSWITCH_TO_SHUTDOWN);
}


/****************************************************************************/
/*!
 *  \brief  OnEntry implementation for Busy State. This method raises event
 *          to warn user that device is busy and to press softswitch again
 *          to shutdown system
 *
 */
/****************************************************************************/
void SoftSwitchMgr::OnPressedAtBusyStateEntered()
{
    //! Start timer. Will internally check if timer is already running,if so will stop and restarts the timer.
    mp_Timer->start();
    m_CurrentState = PressedAtBusyState;
    m_SoftSwitchPressed = false;
    Global::EventObject::Instance().RaiseEvent(EVENT_PRESS_SOFTSWITCH_TO_SHUTDOWN_WITH_WARNING);
}

/****************************************************************************/
/**
 * \brief This function would send a request to Master Thread to check if
 *        any critical action check is in progress.
 *
 */
/****************************************************************************/
void SoftSwitchMgr::OnCriticalActionCheckStateEntered()
{
    mp_Timer->start();
    m_CurrentState = CriticalActionCheckState;
    m_SoftSwitchPressed = false;
    emit SendCricitalActionCheck();
}

/****************************************************************************/
/**
 * \brief This function is called when softswitch is pressed when critical
 *        action is going on. Waits for the critical action to complete and
 *        then informs statemachine to put system to shutdown state.
 */
/****************************************************************************/
void SoftSwitchMgr::OnCriticalActionStateEntered()
{
    //! Start timer. Will internally check if timer is already running,if so will stop and restarts the timer.
    mp_Timer->start();
    m_CurrentState = CriticalActionState;
    m_SoftSwitchPressed = false;
    Global::EventObject::Instance().RaiseEvent(EVENT_CRITICAL_ACTION_WARNING_MESG);
}

/****************************************************************************/
/**
 * \brief Initiates shutdown process of the Main software
 *
 */
/****************************************************************************/
void SoftSwitchMgr::OnShutDownStateEntered()
{
    Global::EventObject::Instance().RaiseEvent(EVENT_SHUTDOWN_IN_FEW_SECONDS);
    m_SoftSwitchPressed = false;
    m_CurrentState = ShutdownState;
    emit SendShutDownCommand();
    mp_SoftSwitchStateMachine->stop();
}

/****************************************************************************/
/**
 * \brief Method called on pressing of softswitch . Emits SoftSwitchPressed
 *        signal to update statemachine to appropriate state
 *
 */
/****************************************************************************/
void SoftSwitchMgr::OnSoftSwitchPressed()
{
    m_SoftSwitchPressed = true;
    if  (!m_ServiceMode) {
        if ((!m_StabilityTimer.isActive())) { // Switch debounce
                m_StabilityTimer.start();
                if (!m_UserActionStarted) {
                    emit SoftSwitchPressed();
                }
                else {
                    QString TranslatedUserActionString = Global::UITranslator::TranslatorInstance().
                            Translate(Global::TranslatableString(m_UserActionID));
                    Global::EventObject::Instance().RaiseEvent(EVENT_SOFTSWITCH_PRESSED_DURING_USER_ACTION,
                                                               Global::FmtArgs() << TranslatedUserActionString);
                }
        }
    }
    else {
        emit SendSoftSwitchPressedCmd();
    }
}

/****************************************************************************/
/*!
 *  \brief  This slot is called to put the statemachine into default state
 */
/****************************************************************************/
void SoftSwitchMgr::ResetStateMachine()
{
    //! This would put the system to default state.
    emit TimerTimeOut();
}

/****************************************************************************/
/*!
 *  \brief  Emits signals that modify softswitch statemachine state based on the
 *          critical action status.
 *  \iparam CriticalActionStatus = Complete/Progress/No critical action
 */
/****************************************************************************/
void SoftSwitchMgr::CriticalActionStatusCheck(NetCommands::CriticalActionStatus_t CriticalActionStatus)
{
    if (m_CurrentState == CriticalActionCheckState) {
        if (CriticalActionStatus == NetCommands::NO_CRITICAL_ACTION) {
            emit CriticalActionNotInProgress();
        }
        else if (CriticalActionStatus == NetCommands::CRITICAL_ACTION_IN_PROGRESS) {
            emit CriticalActionInProgress();
        }
    }
    else if (m_CurrentState == CriticalActionState &&
             CriticalActionStatus == NetCommands::CRITICAL_ACTION_COMPLETE) {
        emit CriticalActionComplete();
    }

}

}// End of namespace GPIOManager
