/****************************************************************************/
/*! \file Include/SoftSwitchManagerThreadController.h
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
#ifndef SOFTSWITCHMANAGERTHREADCONTROLLER_H
#define SOFTSWITCHMANAGERTHREADCONTROLLER_H
//QT Headers
#include <QObject>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
//Project Headers
#include <Global/Include/GlobalDefines.h>
#include <Threads/Include/ThreadController.h>
#include <Global/Include/Commands/AckOKNOK.h>
#include <SoftSwitchManager/Include/SoftSwitchManager.h>



namespace SoftSwitchManager {
class GpioPoller;
/****************************************************************************/
/**
 * \brief This Class shall be responsible to manage the softswitch related
 *        events.They include
 *        1) Manage LEDS
 *        2) Inform master thread to start initialization
 *        3) Initiate the shutdown flow and inform master thread to shutdown
 */
/****************************************************************************/
class SoftSwitchManagerThreadController : public Threads::ThreadController {
    Q_OBJECT

public:
    SoftSwitchManagerThreadController(Global::gSourceType TheHeartBeatSource);
    virtual ~SoftSwitchManagerThreadController();
    virtual void CreateAndInitializeObjects();
    virtual void CleanupAndDestroyObjects();

    /****************************************************************************/
    /**
      * \brief Send command to external process.
      *
      * Send a command to the external process. Reference mapping does not
      * have to be done since the acknowledge we receive from the external
      * process corresponds with Ref.
      *
      * \param[in]   Ref                 Command reference.
      * \param[in]   Cmd                 Command to send.
      */
    /****************************************************************************/
    void SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd);
    Global::tRefType GetNewCommandRef();

protected:
    void OnGoReceived();
    void OnStopReceived();
    virtual void OnPowerFail();

private:
    QThread *mp_PollingThread;
    SoftSwitchMgr m_SoftSwitchManager;
    GpioPoller *mp_GpioPoller;
    Q_DISABLE_COPY(SoftSwitchManagerThreadController) //!< Disable copy and assignment
    void RegisterCommands();
    void OnAcknowledge(Global::tRefType, const Global::AckOKNOK &);
private slots:
    void SendSoftSwitchPressedCmd();
    void TempInitComplete();
    void StartGPIOPolling();
};


class GpioPoller:public QObject
{
    Q_OBJECT
public:
    GpioPoller(qint32 Fd) {
        NumbOfFileDesc = 2;
        m_SoftSwitchFd = Fd;
    }
private:
    struct pollfd fdset[2];
    int NumbOfFileDesc;
    char Buf[100];
    qint32 m_SoftSwitchFd;

private slots:
    void Run() {
         #if !defined(__arm__) || !defined(__TARGET_ARCH_ARM) || !defined(_M_ARM)
            emit OnSoftSwitchPressed();
         #endif
        while (1) {
            memset((void*)fdset, 0, sizeof(fdset));
            fdset[0].fd = STDIN_FILENO; //Standard input
            fdset[0].events = POLLIN;
            fdset[1].fd = m_SoftSwitchFd; //Soft Switch
            fdset[1].events = POLLPRI;

            int TimeOut = -1; // Infinite Timeout
            int  PollReturn = poll(fdset, NumbOfFileDesc, TimeOut);

            if (PollReturn < 0) {
                qDebug("\npoll() failed!\n");
                return;
            }

            if (PollReturn == 0) {
                qDebug(".");
            }
            //SoftSwitch GPIO
            if (fdset[1].revents & POLLPRI) {
                int len = read(fdset[1].fd, &Buf, 100);
                qDebug()<<"\npoll() GPIO interrupt occurred\n";
                emit OnSoftSwitchPressed();
            }

            if (fdset[0].revents & POLLIN) {
                (void)read(fdset[0].fd, &Buf, 1);
                qDebug()<<"\npoll() stdin read:"<< Buf[0];
                emit OnSoftSwitchPressed();
            }

            fflush(stdout);
        }
    }
signals:
    void OnSoftSwitchPressed();

};

} // end namespace EventHandler
#endif // SOFTSWITCHMANAGERTHREADCONTROLLER_H
