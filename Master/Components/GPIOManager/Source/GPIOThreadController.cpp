/****************************************************************************/
/*! \file GPIOThreadController.cpp
 *
 *  \brief SoftSwitchManager Implementation
 *
 *   $Version: $ 0.1
 *   $Date:    $ 15.11.2012
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
//QT Headers
#include <QMetaType>
#include <QDebug>
//Std lib headers
#include <unistd.h>
//Project Headers
#include <GPIOManager/Include/GPIOThreadController.h>
#include <GPIOManager/Include/GpioPoller.h>
#include <Global/Include/Utils.h>
#include <Global/Include/Commands/CmdSoftSwitchPressed.h>
#include <GPIOManager/Include/GPIO.h>
#include <EventHandler/Include/StateHandler.h>
#include <GPIOManager/Include/GPIOManagerEventCodes.h>
//commands
#include <NetCommands/Include/CmdCriticalActionCheck.h>
#include <Global/Include/Commands/CmdShutDown.h>


namespace GPIOManager {

static const unsigned long THREAD_WAIT_TIME         = 1500;             ///< Time to wait when stopping thread [ms].
static const quint32 POWER_FAIL_WAIT_TIMER          = 1000;             ///< Time to wait before sending stage two powerfail cmd [ms]
static const quint32 POWER_GOOD_TEST_TIMER_INTERVAL = 100;              ///< Power good sample time [ms]
static const quint32 POWER_GOOD_GPIO                = 51;               ///< GPIO2_19 of the processor (i.Mx35 EBox)


/****************************************************************************/
/*!
 *  \brief    Constructor for the GPIOThreadController
 *  \iparam ThreadID = Unique Thread ID
 *  \iparam ServiceMode = true if the device is in service mode
 *
 *  \return
 *
 ****************************************************************************/
GPIOThreadController::GPIOThreadController(quint32 ThreadID, bool ServiceMode)
    : Threads::ThreadController(ThreadID, "GPIOManager")
    , m_SoftSwitchManager(this, ServiceMode)
    , m_LedManager(this)
    , mp_PollingThread(NULL)
    , mp_GpioPoller(NULL)
    , m_SkipSoftSwitchAtBoot(false)
    , m_PowerFailCommandSent(false)
    , m_PowerGoodGPIO(POWER_GOOD_GPIO, DIRECTION_IS_INPUT)
    , m_PowerBadCount(0)
    , m_ServiceMode(ServiceMode)
{
    qRegisterMetaType<NetCommands::CriticalActionStatus_t>("NetCommands::CritcalActionStatus_t");
    m_PowerGoodSignalTestTimer.setInterval(POWER_GOOD_TEST_TIMER_INTERVAL);
    CONNECTSIGNALSLOT(&m_PowerGoodSignalTestTimer, timeout(), this, TestPowerGoodSignal());
}


/****************************************************************************/
/*!
 *  \brief    Destructor for the GPIOThreadController
 *
 *  \return
 */
 /****************************************************************************/
GPIOThreadController::~GPIOThreadController()
{
    try {
        delete mp_PollingThread;
        delete mp_GpioPoller;
    }
    CATCHALL_DTOR();
}

/****************************************************************************/
/*!
 *  \brief  Create and initialize objects of the thread controller
 *
 */
/****************************************************************************/
void GPIOThreadController::CreateAndInitializeObjects()
{
    // now register commands
    RegisterCommands();
}

/****************************************************************************/
/*!
 *  \brief Destroy all used objects and prepare for shutdown.
 *
 *         See detailed description in the Base Class's header
 *
 *  \return
 */
/****************************************************************************/
void GPIOThreadController::CleanupAndDestroyObjects()
{
    delete mp_PollingThread;
    mp_PollingThread = NULL;
    delete mp_GpioPoller;
    mp_GpioPoller = NULL;
}

/****************************************************************************/
/*!
 *  \brief Register commands handled by thread Controller
 *
 */
/****************************************************************************/
void GPIOThreadController::RegisterCommands()
{
    RegisterAcknowledgeForProcessing<Global::AckOKNOK, GPIOThreadController>
            (&GPIOThreadController::OnAcknowledge, this);
    RegisterCommandForProcessing<NetCommands::CmdCriticalActionStatus>
            (&GPIOThreadController::OnCmdCriticalActionStatus, this);
    RegisterCommandForProcessing<Global::CmdLCDPowerControl>
            (&GPIOThreadController::OnCmdLCDPowerControl, this);
    RegisterCommandForProcessing<GPIOManager::CmdSoftSwitchLEDControl>
            (&GPIOThreadController::OnLEDControl, this);
    RegisterCommandForProcessing<NetCommands::CmdUserAction>
            (&GPIOThreadController::OnCmdUserAction, this);
}

/****************************************************************************/
/*!
 *  \brief This method is called when the base class receives Go signal.
 *
 *         See detailed description in the Base Class's header
 *
 *  \return
 */
/****************************************************************************/
void GPIOThreadController::OnGoReceived()
{
    m_SoftSwitchManager.ConnectSignals();
    CONNECTSIGNALSLOT(&m_SoftSwitchManager, SendSoftSwitchPressedCmd(), this , SendSoftSwitchPressedCmd());
    CONNECTSIGNALSLOT(&m_SoftSwitchManager, SendCricitalActionCheck(), this , SendCriticalActionCheckCmd());
    CONNECTSIGNALSLOT(&m_SoftSwitchManager, SendShutDownCommand(), this, SendShutDownCommand());
    CONNECTSIGNALSLOT(this, CritcalActionStatus(NetCommands::CriticalActionStatus_t), &m_SoftSwitchManager, CriticalActionStatusCheck(NetCommands::CriticalActionStatus_t));

    EventHandler::StateHandler *p_StateHandler = &EventHandler::StateHandler::Instance();
    CONNECTSIGNALSLOT(p_StateHandler, enteredSoftSwitchMonitorState(), this, StartGPIOPolling());
    CONNECTSIGNALSLOT(p_StateHandler, enteredSoftSwitchMonitorState(), &m_LedManager, StartStateMachine());
    CONNECTSIGNALSIGNAL(p_StateHandler, initComplete(), &m_LedManager, InitComplete());
    Global::EventObject::Instance().RaiseEvent(EVENT_SOFTSWITCH_MONITOR_START);
    EventHandler::StateHandler::Instance().setStateToSoftSwitchMonitorState();
}

/****************************************************************************/
/*!
 *  \brief This method is called when the base class receives Stop signal.
 *
 *         See detailed description in the Base Class's header
 *
 *  \return
 */
/****************************************************************************/
void GPIOThreadController::OnStopReceived()
{

    if (mp_PollingThread) {
        mp_GpioPoller->disconnect();
        mp_PollingThread->quit();
        if (!mp_PollingThread->wait(THREAD_WAIT_TIME)) {
            Global::EventObject::Instance().RaiseEvent(EVENT_GPIO_ERROR,
                                                       Global::FmtArgs() << QString("Gpio Poller thread did not quit in %1 ms").arg( THREAD_WAIT_TIME));
        }
    }
}

/****************************************************************************/
/**
 * \brief Power will fail shortly.
 * \iparam PowerFailStage = Stage1/2 or revert
 */
/****************************************************************************/
void GPIOThreadController::OnPowerFail(const Global::PowerFailStages PowerFailStage)
{
    Q_UNUSED(PowerFailStage)
}

/****************************************************************************/
/**
 * \brief  Method to send commands to other thread controllers
 * \iparam Ref = unique command reference
 * \iparam Cmd = Commad to be sent
 */
/****************************************************************************/
void GPIOThreadController::SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd)
{
    Threads::ThreadController::SendCommand(Ref, Cmd);
}

/****************************************************************************/
/**
 * \brief  Generates new command reference
 * \return Command reference
 */
/****************************************************************************/
Global::tRefType GPIOThreadController::GetNewCommandRef()
{
    return Threads::ThreadController::GetNewCommandRef();
}

/****************************************************************************/
/**
 * \brief  Acknowledgement handler
 * \iparam Ref = unique  command reference
 * \iparam Ack = Acknowledge command
 */
/****************************************************************************/
void GPIOThreadController::OnAcknowledge(Global::tRefType Ref, const Global::AckOKNOK &Ack)
{
    Q_UNUSED(Ref)
    Q_UNUSED(Ack)
    //For now , there is no need to handle acknowledgement.
}

/****************************************************************************/
/**
 * \brief  Critical action command handler. Informs SoftSwitch  Manager using
 *         signal.
 *
 * \iparam Ref = Cmd reference
 * \iparam Cmd = Critical action command.
 *
 */
/****************************************************************************/
void GPIOThreadController::OnCmdCriticalActionStatus(Global::tRefType Ref, const NetCommands::CmdCriticalActionStatus &Cmd)
{
    SendAcknowledgeOK(Ref);
    if (Cmd.m_EvaluateCommand) {
        emit CritcalActionStatus(Cmd.m_CriticalActionStatus);
    }
}

/****************************************************************************/
/**
 * \brief  User action cmd handler. Informs SoftSwitch  Manager using
 *         signal. Examples of user action -> SW update, Import, Export.
 *
 * \iparam Ref = Cmd reference
 * \iparam Cmd = User action command.
 *
 */
/****************************************************************************/
void GPIOThreadController::OnCmdUserAction(Global::tRefType Ref, const NetCommands::CmdUserAction &Cmd)
{
    SendAcknowledgeOK(Ref);
    if (Cmd.m_UserActionCompleted) {
        m_SoftSwitchManager.UserActionComplete();
    }
    else {
        m_SoftSwitchManager.UserActionStarted(Cmd.m_UserActionID);
    }

}

/****************************************************************************/
/**
 * \brief  Turn ON/OFF LCD
 *
 * \iparam Ref = Cmd reference
 * \iparam Cmd = Critical action command.
 *
 */
/****************************************************************************/
void GPIOThreadController::OnCmdLCDPowerControl(Global::tRefType Ref, const Global::CmdLCDPowerControl &Cmd)
{
    SendAcknowledgeOK(Ref);
#if defined(__arm__)
    if (Cmd.m_LCDOn) {
       (void)system("lcd on");
    }
    else {
       (void)system("lcd off");
    }
#else
    Q_UNUSED(Cmd)
#endif
}

/****************************************************************************/
/**
 * \brief  Turn ON/OFF SoftSwitch LED
 *
 * \iparam Ref = Cmd reference
 * \iparam Cmd = LED control command.
 *
 */
/****************************************************************************/
void GPIOThreadController::OnLEDControl(Global::tRefType Ref, const CmdSoftSwitchLEDControl &Cmd)
{
    SendAcknowledgeOK(Ref);
    m_LedManager.ControlLED(Cmd.m_LEDControl);
}

/****************************************************************************/
/**
 * \brief  Send SoftSwitch Pressed command to Master Thread
 *
 */
/****************************************************************************/
void GPIOThreadController::SendSoftSwitchPressedCmd()
{
    Global::EventObject::Instance().RaiseEvent(EVENT_SOFTSWITCH_PRESSED_FIRST_TIME);
    Global::CmdSoftSwitchPressed *p_SoftSwitchPressedCommand = new Global::CmdSoftSwitchPressed();
    p_SoftSwitchPressedCommand->PressedAtStartUp = true;
    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(p_SoftSwitchPressedCommand));
}


/****************************************************************************/
/**
 * \brief  Create Polling thread and start GPIO polling
 *
 */
/****************************************************************************/
void GPIOThreadController::StartGPIOPolling()
{
    mp_PollingThread = new QThread();
#if defined(__arm__)
    m_PowerGoodGPIO.SetEdge("falling");
#endif
    mp_GpioPoller = new GpioPoller(m_SoftSwitchManager.GetSoftSwitchFd(), m_PowerGoodGPIO.GetGpioFd(), m_SkipSoftSwitchAtBoot);
    mp_GpioPoller->moveToThread(mp_PollingThread);
    CONNECTSIGNALSLOT(mp_GpioPoller, SoftSwitchPressed(), &m_SoftSwitchManager, OnSoftSwitchPressed());
    CONNECTSIGNALSLOT(mp_GpioPoller, PowerFailed(), this, SendPowerFailCommand());
    CONNECTSIGNALSLOT(&EventHandler::StateHandler::Instance(), initComplete(), mp_GpioPoller, PollPowerGood());
    CONNECTSIGNALSLOT(mp_PollingThread, started(), mp_GpioPoller, Run());
    mp_PollingThread->start();
}

/****************************************************************************/
/**
 * \brief  Send critical action check command to Master Thread
 *
 */
/****************************************************************************/
void GPIOThreadController::SendCriticalActionCheckCmd()
{
    qDebug()<<"Sending Critical Action Check to Scheduler";
    NetCommands::CmdCriticalActionCheck *p_CriticalActionCheck = new NetCommands::CmdCriticalActionCheck();
    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(p_CriticalActionCheck));
}

/****************************************************************************/
/**
 * \brief  Send shutdown command to Master thread
 *
 */
/****************************************************************************/
void GPIOThreadController::SendShutDownCommand()
{
    mp_GpioPoller->QuitPolling();
    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(new Global::CmdShutDown()));
}

/****************************************************************************/
/**
 * \brief  Send power fail stage 1 command to Master thread
 *
 */
/****************************************************************************/
void GPIOThreadController::SendPowerFailCommand()
{
    if (!m_PowerFailCommandSent) {
        SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(new Global::CmdPowerFail(Global::POWER_FAIL_STAGE_1)));
        QTimer::singleShot(POWER_FAIL_WAIT_TIMER, this, SLOT(SendPowerFailStageTwoCmd()));
        Global::EventObject::Instance().RaiseEvent(GPIOManager::EVENT_POWER_FAIL_STAGE1);
        m_PowerFailCommandSent = true;
    }
    else {
        // Test fluctuations of PowerGood GPIO signal. If there are too many fluctuations, we consider it as power fail
        // irrespective of what the value of PowerGood signal is at recheck time
        if (!m_PowerGoodSignalTestTimer.isActive()) {
             m_PowerGoodSignalTestTimer.start();
        }
    }
}

/****************************************************************************/
/**
 * \brief  Send power fail stage 2 command to Master thread
 *
 */
/****************************************************************************/
void GPIOThreadController::SendPowerFailStageTwoCmd()
{
    m_PowerGoodSignalTestTimer.stop();
    const qint32 PowerGoodReadValue = m_PowerGoodGPIO.GetValue();
    Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, Global::FmtArgs() << QString("Power Good Value %1").arg(PowerGoodReadValue));
    //Check if power was bad for more than or equal to half of the wait time or the current value is zero
    if (m_PowerBadCount >= (POWER_FAIL_WAIT_TIMER/POWER_GOOD_TEST_TIMER_INTERVAL) / 2 || PowerGoodReadValue == 0) {
        mp_GpioPoller->QuitPolling();
        Global::EventObject::Instance().RaiseEvent(GPIOManager::EVENT_POWER_FAIL_STAGE2);
        SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(new Global::CmdPowerFail(Global::POWER_FAIL_STAGE_2)));
    }
    else {
        SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(new Global::CmdPowerFail(Global::POWER_FAIL_REVERT)));
        m_PowerFailCommandSent = false;
        Global::EventObject::Instance().RaiseEvent(GPIOManager::EVENT_POWER_FAIL_REVERT);
    }
}

/****************************************************************************/
/**
 * \brief  Samples power good signal and counts the number of times it remained
 *         low.
 */
/****************************************************************************/
void GPIOThreadController::TestPowerGoodSignal()
{
#if !defined(__arm__)
    //simulate power is always bad on desktop
    m_PowerBadCount++;
#else
    const qint32 PowerGoodReadValue = m_PowerGoodGPIO.GetValue();
    if (PowerGoodReadValue == 0) {
        m_PowerBadCount++;
    }
#endif
}

} //End Of namespace GPIOManager
