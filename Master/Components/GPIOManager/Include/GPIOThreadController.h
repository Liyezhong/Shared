/****************************************************************************/
/*! \file Include/GPIOThreadController.h
 *
 *  \brief  Definition of class SoftSwitchManager
*           This Class shall be responsible to manage the softswitch related
 *          events.They include:
 *              1) Manage LEDS.
 *              2) Inform master thread to start initialization.
 *              3) Initiate the shutdown flow and inform master thread
 *                 to shutdown.
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
#ifndef GPIOTHREADCONTROLLER_H
#define GPIOTHREADCONTROLLER_H
//QT Headers
#include <QObject>

//Project Headers
#include <Global/Include/GlobalDefines.h>
#include <Threads/Include/ThreadController.h>
#include <Global/Include/Commands/AckOKNOK.h>
#include <GPIOManager/Include/SoftSwitchManager.h>
#include <GPIOManager/Include/LEDManager.h>

//Commands
#include <NetCommands/Include/CmdCriticalActionStatus.h>
#include <NetCommands/Include/CmdUserAction.h>
#include <Global/Include/Commands/CmdLCDPowerControl.h>
#include <GPIOManager/Commands/Include/CmdSoftSwitchLEDControl.h>

namespace GPIOManager {

class GpioPoller;

/****************************************************************************/
/**
 * \brief This Class shall be responsible to manage the softswitch related
 *        events.They include
 *        1) Manage LEDS
 *        2) Inform master thread to start initialization
 *        3) Initiate the shutdown flow.
 *        4) Initiate Power fail flow.
 */
/****************************************************************************/
class GPIOThreadController : public Threads::ThreadController {
    Q_OBJECT
    friend class TestGPIOThreadController; //!< Unit test class
public:
    GPIOThreadController(quint32 ThreadID, bool ServiceMode = false);
    virtual ~GPIOThreadController();
    virtual void CreateAndInitializeObjects();
    virtual void CleanupAndDestroyObjects();
    void SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd);
    Global::tRefType GetNewCommandRef();
    /****************************************************************************/
    /**
     * \brief  Setting the flag skips softswitch press from user, instead boots
     *         main software directly.
     *
     *
     * \iparam SkipSoftSwitch = true indicates skip softswitch at boot up
     *                                & start GUI .
     */
    /****************************************************************************/
    void SkipSoftSwitchAtBoot(const bool SkipSoftSwitch) { m_SkipSoftSwitchAtBoot = SkipSoftSwitch;}

protected:
    void OnGoReceived();
    void OnStopReceived();
    virtual void OnPowerFail(const Global::PowerFailStages PowerFailStage);

private:
    SoftSwitchMgr m_SoftSwitchManager;      //!< The softswitch Manager
    LedManager m_LedManager;                //!< The LED Manager
    QThread *mp_PollingThread;              //!< Thread polling the gpio lines
    GpioPoller *mp_GpioPoller;              //!< Class managing the polling thread
    bool m_SkipSoftSwitchAtBoot;            //!< Flag used to inform GPIO poller to skip softswitch since reboot occured.
    bool m_PowerFailCommandSent;            //!< Flag indicating Power fail cmd with STAGE 1 is sent
    QMutex m_Mutex;                         //!< Mutex used during destruction.
    GPIOPin m_PowerGoodGPIO;                //!< GPIO for power good signal
    QTimer m_PowerGoodSignalTestTimer;      //!< Timer which checks fluctuations in power good sinal.
    quint32 m_PowerBadCount;                //!< Count indicating the number of times power good signal was zero
    bool    m_ServiceMode;                  //!< True if device is running with service software.
    /****************************************************************************/
    /**
     * \brief  Disable copy and assignment operator
     */
    /****************************************************************************/
    Q_DISABLE_COPY(GPIOThreadController)
    void RegisterCommands();

    //Command handler
    void OnAcknowledge(Global::tRefType Ref, const Global::AckOKNOK & Ack);
    void OnCmdCriticalActionStatus(Global::tRefType, const NetCommands::CmdCriticalActionStatus &Cmd);
    void OnCmdUserAction(Global::tRefType Ref, const NetCommands::CmdUserAction &Cmd);
    void OnCmdLCDPowerControl(Global::tRefType Ref, const Global::CmdLCDPowerControl &Cmd);
    void OnLEDControl(Global::tRefType Ref, const GPIOManager::CmdSoftSwitchLEDControl &Cmd);

private slots:
    void SendSoftSwitchPressedCmd();
    void StartGPIOPolling();
    void SendCriticalActionCheckCmd();
    void SendShutDownCommand();
    void SendPowerFailCommand();
    void SendPowerFailStageTwoCmd();
    void TestPowerGoodSignal();

signals:
    /****************************************************************************/
    /**
     * \brief  Signals critical action status to clients- for now only Softswitch
     *         manager is connected.
     * \iparam CriticalActionStatusType = critical action -complete, in progress,
     *                                    no crictical action
     */
    /****************************************************************************/
    void CritcalActionStatus(NetCommands::CriticalActionStatus_t CriticalActionStatusType);
};

} // end namespace EventHandler
#endif // GPIOTHREADCONTROLLER_H
