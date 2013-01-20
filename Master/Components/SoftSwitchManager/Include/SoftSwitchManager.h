#ifndef SOFTSWITCHMANAGER_H
#define SOFTSWITCHMANAGER_H
//Qt includes
#include <QObject>
#include <QStateMachine>

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

private:
    GPIOPin m_SoftSwitchGPIO;       //!< GPIO connected to the softswitch on EBox
    QStateMachine *mp_SoftSwitchStateMachine; //!< StateMachine managing SoftSwitch workflow
    GenericStateTemplate *mp_DefaultState;          //!< Default state for the statemachine.
    GenericStateTemplate *mp_PressedAtInitState;    //!< State when SoftSwitch is pressed during Main Init (or system init).
    GenericStateTemplate *mp_PressedAtIdleState;    //!< State when SoftSwitch is pressed at Idle state of system.
    GenericStateTemplate *mp_PressedAtBusyState;    //!< State when SoftSwitch is pressed at busy state of system.
    GenericStateTemplate *mp_ShutDownState;         //!< State to initiate shutdown procedure
    GenericStateTemplate *mp_CriticalActionState;   //!< State when a critical action is going on in the system
    SoftSwitchStateTransition *mp_DefaultToInitTransition; //!< Transition for Default->Init
    SoftSwitchStateTransition *mp_InitToDefaultTransition; //!< Transition for Init->Default
    SoftSwitchStateTransition *mp_DefaultToIdleTransition; //!< Transition for Init->Idle
    SoftSwitchStateTransition *mp_DefaultToBusyTransition; //!< Transition for Init->Busy

    bool CheckIfDeviceIsIdle(QEvent *p_Event);
    bool CheckIfDeviceIsBusy(QEvent *p_Event);
    bool IsSystemStateSoftSwitchMonitor(QEvent *p_Event);
    void OnDefaultStateEntered();
    void OnPressedAtInitStateEntered();
    void OnPressedAtIdleStateEntered();
    void OnPressedAtBusyStateEntered();
    void OnCriticalActionStateEntered();
    void OnShutDownStateEntered();

private slots:
    void TempInitComplete();

signals:
    void OnSoftSwitchPressed();
    void SendSoftSwitchPressedCmd();
    void SystemInitComplete();
};
}
#endif // SOFTSWITCHMANAGER_H
