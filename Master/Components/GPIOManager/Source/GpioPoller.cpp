/****************************************************************************/
/*! \file GpioPoller.cpp
 *
 *  \brief GpioPoller Implementation
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.02.2013
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
#include <GPIOManager/Include/GpioPoller.h>
#include <Global/Include/EventObject.h>
#include <GPIOManager/Include/GPIOManagerEventCodes.h>
#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <sys/syscall.h> //for syscall()

namespace GPIOManager {

const quint32 POLL_TIMER_INTERVAL = 100; //!< timer value in ms

/********************************************************************************/
/*!
 *  \brief    Constructor
 *  \iparam SoftSwitchFileDescriptor  = File descriptor of the opened GPIO file
 *                                      in sysfs of Linux file system
 *  \iparam PowerGoodFileDescriptor = File descriptor of the opened GPIO file
 *                                    in sysfs of Linux file system
 *  \iparam SkipSoftSwitchAtBoot     = True - skip softswitch polling at bootup.
 *                                    Set to true for reboot, software update,
 *                                    boot after power failure.
 */
/********************************************************************************/
GpioPoller::GpioPoller(const qint32 SoftSwitchFileDescriptor, const qint32 PowerGoodFileDescriptor,
                       const bool SkipSoftSwitchAtBoot)
    : m_NumbOfFileDesc(2) // 1)Softswitch 2) PowerGood
    , m_Buf('1')
    , m_SoftSwitchFd(SoftSwitchFileDescriptor)
    , m_PowerGoofFd(PowerGoodFileDescriptor)
    , m_SkipSoftSwitchAtBoot(SkipSoftSwitchAtBoot)
    , mp_PollTimer(NULL)
    , m_DontPollPowerGood(true)
    , m_QuitPolling(false)
{
    memset((void*)Fdset, 0, sizeof(Fdset));


    //Calling Poll without read used to result in unblocking of poll() immediately with
    //event set to POLLERR -> Strange behavior.
    //Looks like reading of gpio files before poll resets or clears pending events /irqs
    lseek(m_SoftSwitchFd, 0, SEEK_SET); /* Start of file */
    m_Buf = 0;
    (void)read(m_SoftSwitchFd, &m_Buf, 1);
    lseek(m_PowerGoofFd, 0, SEEK_SET); /* Start of file */
    (void)read(m_PowerGoofFd, &m_Buf, 1);
    //We start polling of gpio only on the target
#if !defined(__arm__)

    //ON desktop we simulate polling. We poll text files which simulate
    // sysfs gpio files.
    m_SoftSwitchFile.setFileName("SoftSwitch.txt");
    if (!m_SoftSwitchFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
        qDebug("Softswitch file open failed");
    }
    m_PowerGoodFile.setFileName("PowerGood.txt");
    if (!m_PowerGoodFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
        qDebug("PowerGood file open failed");
    }
    //Set default value high.
    char Value = '1';
    (void)m_SoftSwitchFile.write(&Value);
    (void)m_PowerGoodFile.write(&Value);
#endif

}

/****************************************************************************/
/*!
 *  \brief  This method is called when GPIO polling thread is started.
 *
 */
/****************************************************************************/
void GpioPoller::Run() {
    Global::EventObject::Instance().LogThreadId("GpioPoller" , syscall(SYS_gettid));
    mp_PollTimer = new QTimer(this);
    (void)connect(mp_PollTimer, SIGNAL(timeout()), this, SLOT(Poll()));
    mp_PollTimer->start(POLL_TIMER_INTERVAL);
#if !defined(__arm__)
    //we simulate softswitch press ,when not running on target.
    if (m_SkipSoftSwitchAtBoot == false) {
        emit SoftSwitchPressed();
    }
#endif
    // We'll operate on the currently running thread.
    pthread_t this_thread = pthread_self();

    // struct sched_param is used to store the scheduling priority
    struct sched_param params;
    //For schedule idle, priority must be set to 0, else
    //setting priority will fail.
    params.__sched_priority = 0;

    // Attempt to set thread real-time priority to the SCHED_IDLE policy
    pthread_setschedparam(this_thread, SCHED_IDLE, &params);

    // Reboot/Software update/previous power fail, directly boot the software
    if (m_SkipSoftSwitchAtBoot) {
        emit SoftSwitchPressed();
    }
}

/****************************************************************************/
/*!
 *  \brief  Polls the GPIOs and emits appropriate signals
 *
 */
/****************************************************************************/
void GpioPoller::Poll()
{
#if defined(__arm__) //Target
    if (m_QuitPolling && mp_PollTimer) {
        mp_PollTimer->stop();
        return;
    }
    memset((void*)Fdset, 0, sizeof(Fdset));

    Fdset[0].fd = m_SoftSwitchFd; //Soft Switch
    Fdset[0].events = POLLPRI | POLLNVAL; //On target gpio will have priority data

    Fdset[1].fd = m_PowerGoofFd; //Power good
    Fdset[1].events = POLLPRI | POLLNVAL;

    int TimeOut = 1000; // poll times out every second.
    int  PollReturn = poll(Fdset, m_NumbOfFileDesc, TimeOut);


    if (PollReturn < 0) {
        Global::EventObject::Instance().RaiseEvent(EVENT_GPIO_ERROR, Global::FmtArgs() <<"gpio poll exited with negative value");
    }

    if ((Fdset[0].revents) & POLLNVAL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_GPIO_ERROR, Global::FmtArgs() <<"Invalid polling request for softswitch");
        return;
    }

    if ((Fdset[1].revents) & POLLNVAL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_GPIO_ERROR, Global::FmtArgs() <<"Invalid polling request for PowerFail");
    }

    //SoftSwitch GPIO
    if (Fdset[0].revents & POLLPRI) {
        Global::EventObject::Instance().RaiseEvent(EVENT_GPIO_INFO, Global::FmtArgs() <<"Softswitch gpio signal changed. ");
        lseek(Fdset[0].fd, 0, SEEK_SET); /* Start of file */
        m_Buf = 0;
        (void)read(Fdset[0].fd, &m_Buf, 1);
        if (m_Buf == '0') {
            Global::EventObject::Instance().RaiseEvent(EVENT_GPIO_INFO, Global::FmtArgs() <<"Softswitch pressed");
            emit SoftSwitchPressed();
        }
    }
    if (!m_DontPollPowerGood) {
        //Power failure GPIO
        if (Fdset[1].revents & POLLPRI) {
            lseek(Fdset[1].fd, 0, SEEK_SET); /* Start of file */
            m_Buf = 0;
            (void)read(Fdset[1].fd, &m_Buf, 1);
            if (m_Buf == '0') {
                emit PowerFailed();
            }
        }
    }
#else // Desktop
    if (m_QuitPolling && mp_PollTimer) {
        mp_PollTimer->stop();
        return;
    }
    m_SoftSwitchFile.seek(0);
    if ((m_SoftSwitchFile.read(&m_Buf, 1) != -1) && m_Buf == '0' ) {
        char Value = '1';
        (void)m_SoftSwitchFile.seek(0);
        (void)m_SoftSwitchFile.write(&Value);
        fsync(m_SoftSwitchFile.handle());
        emit SoftSwitchPressed();
    }
    //reset buffer
    m_Buf = '2';
    m_PowerGoodFile.seek(0);
    if ((m_PowerGoodFile.read(&m_Buf, 1) != -1) && m_Buf == '0' ) {
        emit PowerFailed();
    }
    //reset buffer
    m_Buf = '2';
#endif
}

/****************************************************************************/
/*!
 *  \brief  Sets the flag to enable powergood signal polling.
 *
 */
/****************************************************************************/
void GpioPoller::PollPowerGood()
{
    m_DontPollPowerGood = false;
}

} // End of namespace GPIOManager
