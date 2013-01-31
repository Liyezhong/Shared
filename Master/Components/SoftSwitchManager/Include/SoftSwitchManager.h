/****************************************************************************/
/*! \file SoftSwitchManager.h
 *
 *  \brief  Definition of class SoftSwitchManager
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
#include <QSocketNotifier>
#include <QFile>
#include <QFileSystemWatcher>
//Project includes
#include <SoftSwitchManager/Include/GPIO.h>
#include <SoftSwitchManager/Include/GenericState.h>
#include <Global/Include/SigTransition.h>

namespace SoftSwitchManager {

class SoftSwitchMgr : public QObject
{
    Q_OBJECT
    typedef Global::CSignalTransition<SoftSwitchMgr> SoftSwitchStateTransition;
    typedef GenericState<SoftSwitchMgr> GenericStateTemplate;
public:
    SoftSwitchMgr(QObject *p_Parent = 0);
    ~SoftSwitchMgr();
    Q_DISABLE_COPY(SoftSwitchMgr) //Disable copy and assignment
    void Init();
    void ConnectSignals();
    qint32 GetSoftSwitchFd() {return m_Fd;}

private:
    GPIOPin m_SoftSwitchGPIO;       //!< GPIO connected to the softswitch on EBox

    // State machine attributes
    QStateMachine *mp_SoftSwitchStateMachine; //!< StateMachine managing SoftSwitch workflow
    GenericStateTemplate *mp_DefaultState;          //!< Default state for the statemachine.
    GenericStateTemplate *mp_PressedAtSoftSwitchMonitorState;    //!< State when SoftSwitch is pressed during Main Init (or system init).
    GenericStateTemplate *mp_PressedAtIdleState;    //!< State when SoftSwitch is pressed at Idle state of system.
    GenericStateTemplate *mp_PressedAtBusyState;    //!< State when SoftSwitch is pressed at busy state of system.
    GenericStateTemplate *mp_ShutDownState;         //!< State to initiate shutdown procedure
    GenericStateTemplate *mp_CriticalActionState;   //!< State when a critical action is going on in the system
    GenericStateTemplate *mp_CriticalActionCheckState; //!< State which checks if there is a critical action going on.
    SoftSwitchStateTransition *mp_DefaultToInitTransition; //!< Transition for Default->Init
    SoftSwitchStateTransition *mp_InitToDefaultTransition; //!< Transition for Init->Default
    SoftSwitchStateTransition *mp_DefaultToIdleTransition; //!< Transition for Init->Idle
    SoftSwitchStateTransition *mp_DefaultToBusyTransition; //!< Transition for Init->Busy
    SoftSwitchStateTransition *mp_IdleToShutDownTransition; //!< Transition for Idle->Shutdown
    SoftSwitchStateTransition *mp_CriticalCheckToShutDownTransition; //!< Transition for CriticalAction->Shutdown

    QTimer *mp_Timer;   //!< Seven second timer. We reset to default state in statemachine on timeout.
    QString m_CurrentState; //!< Current state of the StateMachine
    QFile *mp_File;
    qint32 m_Fd;
    bool CheckIfDeviceIsIdle(QEvent *p_Event);
    bool CheckIfDeviceIsBusy(QEvent *p_Event);
    bool IsSystemStateSoftSwitchMonitor(QEvent *p_Event);
    bool CheckShutDownTransition(QEvent *p_Event);
    void OnDefaultStateEntered();
    void OnPressedAtInitStateEntered();
    void OnPressedAtIdleStateEntered();
    void OnPressedAtBusyStateEntered();
    void OnCriticalActionCheckStateEntered();
    void OnCriticalActionStateEntered();
    void OnShutDownStateEntered();

private slots:
    void OnSoftSwitchPressed();
    void ResetStateMachine();

signals:
    void SoftSwitchPressed();
    void SendSoftSwitchPressedCmd();
    void SystemInitComplete();
    void TimerTimeOut();
    void CriticalActionNotInProgress();
    void CriticalActionInProgress();
    void CriticalActionComplete();
};
}
#endif // SOFTSWITCHMANAGER_H
