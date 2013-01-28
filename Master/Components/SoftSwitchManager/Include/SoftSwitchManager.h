#ifndef SOFTSWITCHMANAGER_H
#define SOFTSWITCHMANAGER_H
//Qt includes
#include <QObject>
#include <QStateMachine>
#include <QTimer>
#include <QSocketNotifier>
//Project includes
#include <SoftSwitchManager/Include/GPIO.h>
#include <SoftSwitchManager/Include/GenericState.h>
#include <Global/Include/SigTransition.h>

namespace SoftSwitchManager {

class SoftSwitchMgr : public QObject
{
    typedef Global::CSignalTransition<SoftSwitchMgr> SoftSwitchStateTransition;
    typedef GenericState<SoftSwitchMgr> GenericStateTemplate;
    Q_OBJECT
public:
    SoftSwitchMgr(QObject *p_Parent = 0);
    ~SoftSwitchMgr();
    void Init();
    void ConnectSignals();
    Q_DISABLE_COPY(SoftSwitchMgr) //Disable copy and assignment
    QSocketNotifier* getSoftSwitchPressedNotifier() { return mp_SoftSwitchPressedNotifier;}

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

    QTimer *mp_Timer;   //!< Seven second timer. We reset to default state in statemachine on timeout.
    QTimer *mp_PollTimer; //!< This timer when timed out would activate the polling of SoftSwitch GPIO.
    QString m_CurrentState; //!< Current state of the StateMachine
    QSocketNotifier *mp_SoftSwitchPressedNotifier;
    bool CheckIfDeviceIsIdle(QEvent *p_Event);
    bool CheckIfDeviceIsBusy(QEvent *p_Event);
    bool IsSystemStateSoftSwitchMonitor(QEvent *p_Event);
    void OnDefaultStateEntered();
    void OnPressedAtInitStateEntered();
    void OnPressedAtIdleStateEntered();
    void OnPressedAtBusyStateEntered();
    void OnCriticalActionCheckStateEntered();
    void OnCriticalActionStateEntered();
    void OnShutDownStateEntered();

private slots:
    void OnSoftSwitchPressed(int GpioFd);
    void ResetStateMachine();
    void ActivatePolling();

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
