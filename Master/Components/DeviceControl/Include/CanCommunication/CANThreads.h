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

#include <QMutex>
#include <QThread>
#include <QWaitCondition>

namespace DeviceControl
{

/* Forward declarations */
class CANCommunicator;
class Can2TcpClient;

/****************************************************************************/
/*!
 *  \brief  This class implements the CAN receive thread
 */
/****************************************************************************/
class CANReceiveThread : public QThread
{
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CANReceiveThread
     *
     *  \param pCANCommunicator = CANCommunicator type parameter
     *
     *  \return from CANReceiveThread
     */
    /****************************************************************************/
    CANReceiveThread(CANCommunicator* pCANCommunicator);

    /****************************************************************************/
    /*!
     *  \brief  Set Simulation flag
     *
     *  \iparam TcpSimmulation = Usage of simulation flag
     */
    /****************************************************************************/
    void SetTcpSimmulation(bool TcpSimmulation) { m_bTcpSimulation = TcpSimmulation; }

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function run
     *
     *  \return from run
     */
    /****************************************************************************/
    void run();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetBreak
     *
     *  \return from SetBreak
     */
    /****************************************************************************/
    void SetBreak();

private:
    CANCommunicator* m_pCANCommunicator;    //!< Communicator object
    QMutex m_BreakLock;     //!< Break synchronization
    bool m_bTcpSimulation;  //!< TCP simulation state flag (use/ don't use)
    bool m_bBreak;          //!< break condition
};


/****************************************************************************/
/*!
 *  \brief  This class implements the CAN transmit thread
 */
/****************************************************************************/
class CANTransmitThread : public QThread
{
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CANTransmitThread
     *
     *  \param pCANCommunicator = CANCommunicator type parameter
     *
     *  \return from CANTransmitThread
     */
    /****************************************************************************/
    CANTransmitThread(CANCommunicator* pCANCommunicator);

    /****************************************************************************/
    /*!
     *  \brief  Set the Can2TcpClient instance for simulator connection
     *
     *  \iparam pClient = Pointer to Can2TcpClient
     */
    /****************************************************************************/
    void SetTcpClient(Can2TcpClient* pClient) { m_pClient = pClient; }

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function run
     *
     *  \return from run
     */
    /****************************************************************************/
    void run();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetBreak
     *
     *  \return from SetBreak
     */
    /****************************************************************************/
    void SetBreak();

    QWaitCondition m_waitCondCANTransmit;   //!< Condition variable for write synchronizing

private:
    CANCommunicator* m_pCANCommunicator;    //!< CAN communicator object
    QMutex m_lockCANTransmit;   //!< Write synchronization
    QMutex m_BreakLock;         //!< Break synchronization
    Can2TcpClient* m_pClient;   //!< CAN 2 TCP converter
    bool m_bBreak;              //!< Break condition for exit
};

} //namespace

#endif /* CANTHREADS_H */
