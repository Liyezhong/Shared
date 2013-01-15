#ifndef SOFTSWITCHMANAGER_H
#define SOFTSWITCHMANAGER_H
//Qt includes
#include <QObject>
#include <QStateMachine>

//Project includes
#include <SoftSwitchManager/Include/GPIO.h>
#include <Global/Include/SigTransition.h>

namespace SoftSwitchManager {

class SoftSwitchManager : public QObject
{
public:
    SoftSwitchManager();
    ~SoftSwitchManager();
    Q_DISABLE_COPY(SoftSwitchManager) //Disable copy and assignment

private:
    GPIOPin m_SoftSwitchGPIO; //!< GPIO connected to the softswitch on EBox
    QStateMachine m_SoftSwitchStateMachine; //!< StateMachine managing Softswitch workflow
    QState m_DefaultState; //!< Default state for the statemachine.
    QState m_PressedAtInitState; //!< State when SoftSwitch is pressed during Main Init (or system init).
    QState m_PressedAtIdleState; //!< State when SoftSwitch is pressed at Idle state of system.
    QState m_PressedAtBusyState; //!< State when SoftSwitch is pressed at busy state of system.
    QState m_ShutDownState; //!< State to initiate shutdown procedure
    QState m_CriticalActionState; //!< State when a critical action is going on in the system
    bool CheckIfDeviceIsIdle();
    bool CheckIfDeviceIsBusy();
};
}
#endif // SOFTSWITCHMANAGER_H
