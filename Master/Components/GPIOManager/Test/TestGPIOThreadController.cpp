/****************************************************************************/
/*! \file TestGPIOThreadController.cpp
 *
 *  \brief Unit Test Class for GPIO Component
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-02-22
 *  $Author:    $ N.Kamath
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
#include <QtTest/QTest>
#include <Threads/Include/PlatformThreadIDs.h>
#include <EventHandler/Include/StateHandler.h>
#include <GPIOManager/Include/GPIOThreadController.h>
#include <GPIOManager/Include/GpioPoller.h>
#include <GPIOManager/Include/LEDManager.h>
#include <GPIOManager/Include/GPIO.h>
#include <GPIOManager/Commands/Include/CmdSoftSwitchLEDControl.h>
#include <QSignalSpy>
#include <QVariant>

namespace GPIOManager {

/****************************************************************************/
/**
 * \brief Test class for GPIOThreadController.
 */
/****************************************************************************/
class TestGPIOThreadController : public QObject {
    Q_OBJECT

private:
    GPIOThreadController *mp_GPIOThreadController; //!< GPIO threadController

private slots:

    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
     */
    /****************************************************************************/
    void init();
    /****************************************************************************/
    /**
     * \brief Test GPIO Thread Controller
     */
    /****************************************************************************/
    void TestGPIO();
    /****************************************************************************/
    /**
     * \brief Test SoftSwitch
     */
    /****************************************************************************/
    void TestSofSwitch();
    /****************************************************************************/
    /**
     * \brief Test LED Manager
     */
    /****************************************************************************/
    void TestLED();
    /****************************************************************************/
    /**
     * \brief Test GPIO Pin
     */
    /****************************************************************************/
    void GPIOPin();

    /****************************************************************************/
    /**
     * \brief Test GPIO Commands
     */
    /****************************************************************************/
    void GPIOCommand();
    /****************************************************************************/
    /**
     * \brief Called after each testfunction was executed.
     */
    /****************************************************************************/
    void cleanup();
    /****************************************************************************/
    /**
     * \brief Called after last testfunction was executed.
     */
    /****************************************************************************/
    void cleanupTestCase();

signals:
    /****************************************************************************/
    /**
     * \brief Start GPIO Processing
     */
    /****************************************************************************/
    void StartProcessing();
};

void TestGPIOThreadController::TestGPIO()
{
    mp_GPIOThreadController = new GPIOThreadController(10, false);
    QCOMPARE(false, mp_GPIOThreadController->m_ServiceMode);
    mp_GPIOThreadController->SkipSoftSwitchAtBoot(true);
    QCOMPARE(mp_GPIOThreadController->m_SkipSoftSwitchAtBoot, true);
    connect(this, SIGNAL(StartProcessing()), mp_GPIOThreadController, SLOT(StartGPIOPolling()));
    LedManager *p_LEDManager = &(mp_GPIOThreadController->m_LedManager);
    connect(this, SIGNAL(StartProcessing()), p_LEDManager, SLOT(StartStateMachine()));
    emit StartProcessing();
    QCOMPARE(mp_GPIOThreadController->m_PowerFailCommandSent, false);
    mp_GPIOThreadController->SendPowerFailCommand();
    QCOMPARE(mp_GPIOThreadController->m_PowerFailCommandSent, true);
    GPIOManager::CmdSoftSwitchLEDControl Cmd(GREEN_LED_ON);
    mp_GPIOThreadController->OnLEDControl(100, Cmd);
    NetCommands::CmdUserAction CmdUserAction;
    CmdUserAction.m_UserActionCompleted = false;
    CmdUserAction.m_UserActionID = 1;
    mp_GPIOThreadController->OnCmdUserAction(101, CmdUserAction);
    Global::CmdLCDPowerControl CmdLCD(false);
    mp_GPIOThreadController->OnCmdLCDPowerControl(102, CmdLCD);
    qRegisterMetaType<NetCommands::CriticalActionStatus_t>("NetCommands::CriticalActionStatus_t");
    QSignalSpy Spy(mp_GPIOThreadController, SIGNAL(CritcalActionStatus(NetCommands::CriticalActionStatus_t)));
    NetCommands::CmdCriticalActionStatus CmdCriticalAction;
    CmdCriticalAction.m_CriticalActionStatus = NetCommands::CRITICAL_ACTION_COMPLETE;
    CmdCriticalAction.m_EvaluateCommand = true;
    mp_GPIOThreadController->OnCmdCriticalActionStatus(103, CmdCriticalAction);
    QCOMPARE(Spy.count(), 1);
    mp_GPIOThreadController->OnPowerFail(Global::POWER_FAIL_REVERT);
    Global::AckOKNOK Ack(true);
    mp_GPIOThreadController->SendSoftSwitchPressedCmd();
    mp_GPIOThreadController->SendShutDownCommand();
    mp_GPIOThreadController->SendCriticalActionCheckCmd();
    mp_GPIOThreadController->OnAcknowledge(104, Ack);
    mp_GPIOThreadController->mp_GpioPoller->QuitPolling();
    mp_GPIOThreadController->OnStopReceived();
    mp_GPIOThreadController->CleanupAndDestroyObjects();
}

void TestGPIOThreadController::TestSofSwitch()
{
    SoftSwitchMgr ObjSoftSwitchMgr(NULL, false);
    ObjSoftSwitchMgr.ConnectSignals();
    ObjSoftSwitchMgr.IsSystemStateInit(NULL);
    ObjSoftSwitchMgr.IsSystemStateSoftSwitchMonitor(NULL);
    ObjSoftSwitchMgr.CheckIfDeviceInitFailed(NULL);
    ObjSoftSwitchMgr.CheckIfDeviceIsBusy(NULL);
    ObjSoftSwitchMgr.CheckIfDeviceIsIdle(NULL);
    QSignalSpy Spy(&ObjSoftSwitchMgr, SIGNAL(CriticalActionComplete()));
    ObjSoftSwitchMgr.m_CurrentState = "CriticalActionState";
    ObjSoftSwitchMgr.CriticalActionStatusCheck(NetCommands::CRITICAL_ACTION_COMPLETE);
    QCOMPARE(Spy.count(), 1);
    ObjSoftSwitchMgr.OnSoftSwitchPressed();
    ObjSoftSwitchMgr.OnDefaultStateEntered();
    ObjSoftSwitchMgr.OnPressedAtInitStateEntered();
    ObjSoftSwitchMgr.OnPressedAtSoftSwitchMonitorStateEntered();
    ObjSoftSwitchMgr.OnPressedAtIdleStateEntered();
    ObjSoftSwitchMgr.OnPressedAtBusyStateEntered();
    ObjSoftSwitchMgr.OnCriticalActionCheckStateEntered();
    ObjSoftSwitchMgr.OnCriticalActionStateEntered();
    ObjSoftSwitchMgr.OnShutDownStateEntered();
    QCOMPARE(ObjSoftSwitchMgr.GetSoftSwitchFd(), -1);
}

void TestGPIOThreadController::TestLED()
{
    LedManager ObjLedManager(NULL, false);
    ObjLedManager.OnEntryInitCompleteState();
    ObjLedManager.ServiceModeInit();
}

void TestGPIOThreadController::GPIOPin()
{
    GPIOManager::GPIOPin TestPin(10, DIRECTION_IS_OUTPUT, NULL);
    QCOMPARE(TestPin.m_Direction, DIRECTION_IS_OUTPUT);
    QCOMPARE(TestPin.m_PinNumber, (quint32)10);
    TestPin.ExportGPIOPin();
    QCOMPARE(TestPin.GetGpioFd(), (int)-1);
    QCOMPARE(TestPin.SetEdge("falling"),(int) -1);
    QCOMPARE(TestPin.GetValue(), (int)-1);
    QCOMPARE(TestPin.SetValue(1), (int)-1);
    TestPin.SetDirection(DIRECTION_IS_OUTPUT);
}

void TestGPIOThreadController::GPIOCommand()
{
    CmdSoftSwitchLEDControl Cmd1(GREEN_LED_OFF);
    CmdSoftSwitchLEDControl Cmd2(Cmd1);
    CmdSoftSwitchLEDControl Cmd3(GREEN_LED_ON);

    QVERIFY(Cmd1.GetName() == "Global::CmdSoftSwitchLEDControl");

    Cmd3 = Cmd2;
}

void TestGPIOThreadController::cleanup()
{
}

void TestGPIOThreadController::cleanupTestCase()
{
    delete mp_GPIOThreadController;
}

void TestGPIOThreadController::initTestCase()
{

}

void TestGPIOThreadController::init()
{

}

} // End of namespace GPIOManager

QTEST_MAIN(GPIOManager::TestGPIOThreadController)

#include "TestGPIOThreadController.moc"
