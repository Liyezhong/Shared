#include <SoftSwitchManager/Include/SoftSwitchManager.h>
#include <EventHandler/Include/StateHandler.h>

namespace SoftSwitchManager {


const quint32 SOFT_SWITCH_GPIO_NUMBER = 0; //!< : GPIO1_0 of the processor (i.Mx35 EBox)
const quint32 DIRECTION_IS_INPUT = 0;   //!< Value for input direction of GPIO

SoftSwitchMgr::SoftSwitchMgr(QObject *p_Parent)
    :m_SoftSwitchGPIO(SOFT_SWITCH_GPIO_NUMBER, DIRECTION_IS_INPUT)
    ,mp_SoftSwitchPressedInitTransition(NULL)
{
    setParent(p_Parent);
    Init();
}

void SoftSwitchMgr::Init()
{
    mp_SoftSwitchStateMachine = new QStateMachine();
    mp_SoftSwitchStateMachine->setParent(this);
    //mp_SoftSwitchStateMachine->moveToThread(this->thread());
    //!< Create States
    mp_DefaultState = new GenericStateTemplate(*this, &SoftSwitchMgr::OnDefaultStateEntered);
    mp_DefaultState->setParent(this);
    //mp_DefaultState->moveToThread(this->thread());
    mp_PressedAtInitState = new GenericStateTemplate(*this, &SoftSwitchMgr::OnPressedAtInitStateEntered);
    mp_PressedAtInitState->setParent(this);
    //mp_PressedAtInitState->moveToThread(this->thread());
    mp_PressedAtBusyState = new GenericStateTemplate(*this, &SoftSwitchMgr::OnPressedAtBusyStateEntered);
    mp_PressedAtBusyState->setParent(this);
    //mp_PressedAtBusyState->moveToThread(this->thread());
    mp_PressedAtIdleState = new GenericStateTemplate(*this, &SoftSwitchMgr::OnPressedAtIdleStateEntered);
    mp_PressedAtIdleState->setParent(this);
    //mp_PressedAtIdleState->moveToThread(this->thread());
    mp_CriticalActionState = new GenericStateTemplate(*this, &SoftSwitchMgr::OnCriticalActionStateEntered);
    mp_CriticalActionState->setParent(this);
    //mp_CriticalActionState->moveToThread(this->thread());
    mp_ShutDownState = new GenericStateTemplate(*this, &SoftSwitchMgr::OnShutDownStateEntered);
    mp_ShutDownState->setParent(this);
    //mp_ShutDownState->moveToThread(this->thread());


    //! Build the state machine
    mp_SoftSwitchStateMachine->addState(mp_DefaultState);
    mp_SoftSwitchStateMachine->addState(mp_PressedAtInitState);
    mp_SoftSwitchStateMachine->addState(mp_PressedAtIdleState);
    mp_SoftSwitchStateMachine->addState(mp_PressedAtBusyState);
    mp_SoftSwitchStateMachine->addState(mp_CriticalActionState);
    mp_SoftSwitchStateMachine->addState(mp_ShutDownState);

    mp_SoftSwitchStateMachine->setInitialState(mp_DefaultState);
    //!< Add Transition between states of the state machine
    mp_SoftSwitchPressedInitTransition = new SoftSwitchStateTransition(this, SIGNAL(OnSoftSwitchPressed()),
                                                                   *this, &SoftSwitchMgr::CheckIfDeviceIsInInit,
                                                                   mp_PressedAtInitState);
    mp_SoftSwitchPressedInitTransition->setParent(this);
    //mp_SoftSwitchPressedInitTransition->moveToThread(this->thread());
    mp_DefaultState->addTransition(mp_SoftSwitchPressedInitTransition);
    mp_SoftSwitchStateMachine->start();
    qDebug()<< "State Machine error " <<mp_SoftSwitchStateMachine->error();
    qDebug()<<"Running state" <<mp_SoftSwitchStateMachine->isRunning();
    qDebug()<<"SoftSwitchMgr::Init()" << mp_SoftSwitchStateMachine->thread();

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
    return false;
}

bool SoftSwitchMgr::CheckIfDeviceIsIdle(QEvent *p_Event) {
    return false;
}

bool SoftSwitchMgr::CheckIfDeviceIsInInit(QEvent *p_Event) {
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
    qDebug()<<" Default State Entered \n \n";
}

void SoftSwitchMgr::OnPressedAtInitStateEntered()
{
    qDebug()<<" SoftSwitch PressedAt Init State Entered \n \n";
    qDebug()<<"State thread" << mp_PressedAtInitState->thread();
    EventHandler::StateHandler::Instance().setInitState();
    emit SendSoftSwitchPressedCmd();
}

void SoftSwitchMgr::OnPressedAtIdleStateEntered()
{
}

void SoftSwitchMgr::OnPressedAtBusyStateEntered()
{
}

void SoftSwitchMgr::OnCriticalActionStateEntered()
{
}

void SoftSwitchMgr::OnShutDownStateEntered()
{
}

}// End of namespace SoftSwitchManager
