/****************************************************************************/
/*! \file GpioPoller.h
 *
 *  \brief  Definition of class GpioPoller.
 *          This class provides functionality to poll GPIO's on EBox.
 *          Currently  the following GPIO's are polled :
 *          1) GPIO1_0  for softswitch
 *          2) GPIO2_19 for Power fail
 *          On Desktop, SoftSwitch.txt and PowerGood.txt is polled.
 *          '0' in Softswitch.txt would simulate softswitch press.
 *          '0' in PowerGood.txt would simulate PowerGood.txt.
 *
 *  Version:    0.1
 *  Date:       2013-02-08
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
#ifndef GPIOPOLLER_H
#define GPIOPOLLER_H

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <QObject>
#include <QTimer>
#include <QFile>
namespace GPIOManager {

/****************************************************************************/
/**
 * \brief  This class provides functionality to poll GPIO's on EBox.
 *         Currently  the following GPIO's are polled :
 *         1) GPIO1_0  for softswitch
 *         2) GPIO2_19 for Power fail
 */
/****************************************************************************/
class GpioPoller:public QObject
{
    Q_OBJECT
    friend class TestGPIOThreadController;
public:
    GpioPoller(const qint32 SoftSwitchFileDescriptor, const qint32 PowerGoodFileDescriptor,const bool SkipSoftSwitchAtBoot);

    /****************************************************************************/
    /**
     * \brief  Informs poller to quit polling.
     */
    /****************************************************************************/
    void QuitPolling() { m_QuitPolling = true; }

private:
    Q_DISABLE_COPY(GpioPoller)

    struct pollfd Fdset[2]; //!< file descriptor array to be polled.
    int m_NumbOfFileDesc; //!< Number of file desc to be polled
    char m_Buf; //!< Buffer to store read value from GPIO file
    qint32 m_SoftSwitchFd; //!< File desc of softswitch GPIO
    qint32 m_PowerGoofFd; //!< File desc of power good GPIO
    bool m_SkipSoftSwitchAtBoot; //!< Flag indicating not to consider softswitch for booting up, directly send softswitch pressed signal
    QTimer *mp_PollTimer; //!< Periodic timer to poll GPIOs.
    bool m_DontPollPowerGood; //!< Flag indicating  not to poll power good signal.
    bool m_QuitPolling; //!< If set to true, Poll() function will quit.
#if !defined(__arm__)
    QFile m_SoftSwitchFile; //!< SoftSwitch.txt
    QFile m_PowerGoodFile;  //!< PowerGood.txt
#endif

private slots:
    void Run();
    void Poll();
    void PollPowerGood();

signals:
    /****************************************************************************/
    /**
     * \brief  Signals softswitch press
     */
    /****************************************************************************/
    void SoftSwitchPressed();
    /****************************************************************************/
    /**
     * \brief  Signals Power failure
     */
    /****************************************************************************/
    void PowerFailed();
};
}
#endif // GPIOPOLLER_H
