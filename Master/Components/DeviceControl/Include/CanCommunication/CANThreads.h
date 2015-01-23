/****************************************************************************/
/*! \file CANThreads.h
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 08.07.2010
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the CANReceiveThread class and
 *       the CANTransmitThread class
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

#ifndef CANTHREADS_H
#define CANTHREADS_H

#include <linux/can.h>

#include <QMutex>
#include <QSemaphore>
#include <QThread>
#include <QWaitCondition>

namespace DeviceControl
{

/* Forward declarations */
class CANCommunicator;
class CANInterface;

/****************************************************************************/
/*!
 *  \brief  This class implements the CAN receive thread
 */
/****************************************************************************/
class CANReceiveThread : public QThread
{
    Q_OBJECT

private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CANReceiveThread)

public:
    CANReceiveThread(CANCommunicator* pCANCommunicator, CANInterface & rCANInterface);

    void SetBreak();

signals:
    /****************************************************************************/
    /*!
     *  \brief  Forward error information
     *
     *  \iparam EventCode = Event code
     *  \iparam EventData = Event data
     */
    /****************************************************************************/
    void ReportEvent(quint32 EventCode, quint16 EventData);

private:
    void run();
    void LogErrorFrame(can_frame &Frame);

private:
    CANCommunicator*    m_pCANCommunicator;     //!< Communicator object
    CANInterface &      m_rCANInterface;        //!< reference to CAN interface
    QMutex              m_BreakLock;            //!< Break synchronization
    bool                m_bBreak;               //!< break condition
    can_frame           m_lastErrorFrame;       //!< last received error frame
};


/****************************************************************************/
/*!
 *  \brief  This class implements the CAN transmit thread
 */
/****************************************************************************/
class CANTransmitThread : public QThread
{
    Q_OBJECT

private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CANTransmitThread)

public:
    CANTransmitThread(CANCommunicator* pCANCommunicator, CANInterface & rCANInterface);

    void SetBreak();

    void WakeUp();

signals:
    /****************************************************************************/
    /*!
     *  \brief  Forward error information
     *
     *  \iparam EventCode = Event code
     *  \iparam EventData = Event data
     */
    /****************************************************************************/
    void ReportEvent(quint32 EventCode, quint16 EventData);

private:
    void run();

    CANCommunicator*    m_pCANCommunicator;     //!< CAN communicator object
    CANInterface &      m_rCANInterface;        //!< reference to CAN interface
    QSemaphore          m_WakeUp;               //!< Write synchronization
    QMutex              m_BreakLock;            //!< Break synchronization
    bool                m_bBreak;               //!< Break condition for exit
    int                 m_lastErrno;            //!< last result from writing to CAN bus

};

} //namespace

#endif /* CANTHREADS_H */
