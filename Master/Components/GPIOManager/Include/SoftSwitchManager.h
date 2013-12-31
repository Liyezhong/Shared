/****************************************************************************/
/*! \file SoftSwitchManager.h
 *
 *  \brief  Definition of class SoftSwitchManager.
 *          StateMachine approach is used to handle softswitch related
 *          workflow. Each state in the statemachine decides the action to be
 *          taken on softsiwtch press. Actions to be taken are implemented
 *          as Events which inform/Warn user OR as commands which inform the
 *          MasterThread to initiate further actions.
 *
 *
 *
 *  Version:    0.1
 *  Date:       2012-11-15
 *  Author:     N.Kamath
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
#ifndef SOFTSWITCHMANAGER_H
#define SOFTSWITCHMANAGER_H
//Qt includes
#include <QObject>
#include <QStateMachine>
#include <QTimer>
#include <QFile>
//Project includes
#include <GPIOManager/Include/GPIO.h>
#include <GPIOManager/Include/GenericState.h>
#include <Global/Include/SigTransition.h>
#include <NetCommands/Include/CmdCriticalActionStatus.h>

namespace GPIOManager {
/****************************************************************************/
/**
 * \brief  This class shall be used to Manage SoftSwitch flow.
 *
 */
/****************************************************************************/
class SoftSwitchMgr : public QObject
{
    Q_OBJECT
    friend class TestGPIOThreadController; //!<  Unit test class
    typedef Global::CSignalTransition<SoftSwitchMgr> SoftSwitchStateTransition_t;//!< typedef for Signal transition class
    typedef GenericState<SoftSwitchMgr> GenericState_t; //!< typdef for Softswitch manager generic state.
public:
    SoftSwitchMgr(QObject *p_Parent = 0, bool ServiceMode = false);
    ~SoftSwitchMgr();
    void ConnectSignals();
    /****************************************************************************/
    /**
     * \brief  Returns gpio file handle of softswitch.
     * \return File handle
     */
    /****************************************************************************/
    qint32 GetSoftSwitchFd() {return m_Fd;}
    /****************************************************************************/
    /**
     * \brief  Sets user action flag.
     * \iparam UserActionID = Event ID of the user action.
     */
    /****************************************************************************/
    void UserActionStarted(const quint32 UserActionID) {
        m_UserActionID = UserActionID;
        m_UserActionStarted = true;
    }

    /****************************************************************************/
    /**
     * \brief  Resets User action flag.
     */
    /****************************************************************************/
    void UserActionComplete() { m_UserActionStarted = false; }


private:
    GPIOPin m_SoftSwitchGPIO;       //!< GPIO connected to the softswitch on EBox

    // State machine attributes
    QStateMachine *mp_SoftSwitchStateMachine; //!< StateMachine managing SoftSwitch workflow
    GenericState_t *mp_DefaultState;          //!< Default state for the statemachine.
    GenericState_t *mp_PressedAtSoftSwitchMonitorState;    //!< State when SoftSwitch is pressed during Main Init (or system init).
    GenericState_t *mp_PressedAtInitState;    //!< State when SoftSwitch is pressed at InitState(e.g reference runs);
    GenericState_t *mp_PressedAtIdleState;    //!< State when SoftSwitch is pressed at Idle state of system.
    GenericState_t *mp_PressedAtBusyState;    //!< State when SoftSwitch is pressed at busy state of system.
    GenericState_t *mp_ShutDownState;         //!< State to initiate shutdown procedure
    GenericState_t *mp_CriticalActionState;   //!< State when a critical action is going on in the system
    GenericState_t *mp_CriticalActionCheckState; //!< State which checks if there is a critical action going on.
    SoftSwitchStateTransition_t *mp_DefaultToSoftSwitchMonitorTransition; //!< Transition for Default->softSwitch Monitor
    SoftSwitchStateTransition_t *mp_DefaultToInitTransition; //!< Transition from Default to Init Transiton
    SoftSwitchStateTransition_t *mp_SoftSwitchMonitorToInitTransition;//!< Transition for SoftSwitchMonitor ->Init
    SoftSwitchStateTransition_t *mp_InitToDefaultTransition; //!< Transition for SoftSwitchMonitor->Default
    SoftSwitchStateTransition_t *mp_InitSelfTransition;   //!< Self transition for init
    SoftSwitchStateTransition_t *mp_DefaultToIdleTransition; //!< Transition for Default->Idle
    SoftSwitchStateTransition_t *mp_AlternativeDefaultToIdleTransition; //!< Transition for Default -> Idle (used when init fails)
    SoftSwitchStateTransition_t *mp_DefaultToBusyTransition; //!< Transition for Default->Busy
    SoftSwitchStateTransition_t *mp_IdleToShutDownTransition; //!< Transition for Idle->Shutdown
    SoftSwitchStateTransition_t *mp_CriticalCheckToShutDownTransition; //!< Transition for CriticalAction->Shutdown

    QTimer *mp_Timer;   //!< Seven second timer. We reset to default state in statemachine on timeout.
    QTimer  m_StabilityTimer; //!< Timer that makes sure softswitch timer is stable
    QString m_CurrentState; //!< Current state of the StateMachine
    qint32 m_Fd;            //!< File descriptor of the GPIO file
    bool m_SoftSwitchPressed; //!< true if softswitch is pressed, else false.
    bool m_UserActionStarted; //!< true if user action is started (e.g Import, export), else false
    bool m_ServiceMode; //!< true if device is running with running service software
    quint32 m_UserActionID; //!< ID (Event ID ) of the user action in progress

    /****************************************************************************/
    /**
     * \brief  Disable copy and assignment operator
     */
    /****************************************************************************/
    Q_DISABLE_COPY(SoftSwitchMgr)

    void Init();
    bool CheckIfDeviceIsIdle(QEvent *p_Event);
    bool CheckIfDeviceIsBusy(QEvent *p_Event);
    bool CheckIfDeviceInitFailed(QEvent *p_Event);
    bool IsSystemStateSoftSwitchMonitor(QEvent *p_Event);
    bool IsSystemStateInit(QEvent *p_Event);

    void OnDefaultStateEntered();
    void OnPressedAtSoftSwitchMonitorStateEntered();
    void OnPressedAtInitStateEntered();
    void OnPressedAtIdleStateEntered();
    void OnPressedAtBusyStateEntered();
    void OnCriticalActionCheckStateEntered();
    void OnCriticalActionStateEntered();
    void OnShutDownStateEntered();

private slots:
    void OnSoftSwitchPressed();
    void ResetStateMachine();
    void CriticalActionStatusCheck(NetCommands::CriticalActionStatus_t CriticalActionStatus);

signals:
    /****************************************************************************/
    /**
     * \brief  Informs state machine that softswitch is pressed.
     */
    /****************************************************************************/
    void SoftSwitchPressed();
    /****************************************************************************/
    /**
     * \brief  Informs Master thread that softswitch is pressed.
     */
    /****************************************************************************/
    void SendSoftSwitchPressedCmd();
    /****************************************************************************/
    /**
     * \brief  Informs state machine that system initialization is complete.
     */
    /****************************************************************************/
    void SystemInitComplete();
    /****************************************************************************/
    /**
     * \brief  Signals timeout
     */
    /****************************************************************************/
    void TimerTimeOut();
    /****************************************************************************/
    /**
     * \brief  Informs state machine that critical action is not in progress
     */
    /****************************************************************************/
    void CriticalActionNotInProgress();
    /****************************************************************************/
    /**
     * \brief  Informs state machine that critical action is in progress.
     */
    /****************************************************************************/
    void CriticalActionInProgress();
    /****************************************************************************/
    /**
     * \brief  Informs state machine that critical action is complete.
     */
    /****************************************************************************/
    void CriticalActionComplete();
    /****************************************************************************/
    /**
     * \brief  Requests main to check if any critical action is in progress.
     */
    /****************************************************************************/
    void SendCricitalActionCheck();
    /****************************************************************************/
    /**
     * \brief  Initiates shutdown flow.
     */
    /****************************************************************************/
    void SendShutDownCommand();
    /****************************************************************************/
    /**
     * \brief  Informs state machine to enter init state.
     */
    /****************************************************************************/
    void EnterInitState();
    /****************************************************************************/
    /**
     * \brief  Informs state machine to enter default state.
     */
    /****************************************************************************/
    void EnterDefaultState();
};
}
#endif // SOFTSWITCHMANAGER_H
