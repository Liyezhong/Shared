#include <SoftSwitchManager/Include/SoftSwitchManager.h>

namespace SoftSwitchManager {

typedef Global::CSignalTransition<SoftSwitchManager> CReadRackRfidTransition;
const quint32 SOFT_SWITCH_GPIO_NUMBER = 0; //!< : GPIO1_0 of the processor (i.Mx35 EBox)
const quint32 DIRECTION_IS_INPUT = 0;

SoftSwitchManager::SoftSwitchManager()
    :m_SoftSwitchGPIO(SOFT_SWITCH_GPIO_NUMBER, DIRECTION_IS_INPUT)
{
    //! Build the state machine
    m_SoftSwitchStateMachine.addState(&m_DefaultState);
    m_SoftSwitchStateMachine.addState(&m_PressedAtInitState);
    m_SoftSwitchStateMachine.addState(&m_PressedAtIdleState);
    m_SoftSwitchStateMachine.addState(&m_PressedAtBusyState);
    m_SoftSwitchStateMachine.addState(&m_CriticalActionState);
    m_SoftSwitchStateMachine.addState(&m_ShutDownState);

    m_SoftSwitchStateMachine.setInitialState(&m_DefaultState);

    //m_DefaultState.addTransition();
}

SoftSwitchManager::~SoftSwitchManager()
{

}

}// End of namespace SoftSwitchManager
