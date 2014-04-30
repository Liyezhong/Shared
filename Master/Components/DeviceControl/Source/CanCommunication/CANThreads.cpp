/****************************************************************************/
/*! \file CANThreads.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CANReceiveThread and CANTransmitThread
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
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>

#include "DeviceControl/Include/CanCommunication/CANThreads.h"
#include "DeviceControl/Include/CanCommunication/CANInterface.h"
#include "DeviceControl/Include/CanCommunication/CANCommunicator.h"
#include "DeviceControl/Include/CanCommunication/CanTcpCommunication/client.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor for the CANReceiveThread class
 *
 *  \iparam pCANCommunicator = CANCommunicator classed for receive queue access
 */
/****************************************************************************/
CANReceiveThread::CANReceiveThread(CANCommunicator* pCANCommunicator) : QThread(),
    m_pCANCommunicator(pCANCommunicator),
    m_bTcpSimulation(false), m_bBreak(false)
{
}

/****************************************************************************/
/*!
 *  \brief  The receive thread's execution function
 *
 *      The function calls a blocking read and waits until new CAN message data were received.
 *      The new CAN message will be add to the receive queue and the blocking read will be called again.
 */
/****************************************************************************/
void CANReceiveThread::run()
{
    struct can_frame frame;
    forever
    {
        if(m_pCANCommunicator != NULL && m_pCANCommunicator->GetCANInterface() != NULL)
        {
            // read message from CAN bus (blocking call to socket)
            int nReadResult = m_pCANCommunicator->GetCANInterface()->Read(&frame);

            // add message to receive queue
            if (nReadResult > 0)
            {
                m_pCANCommunicator->AppendToReceiveQueue(frame);
            }
            else
            {
                m_pCANCommunicator->SetCommunicationError(nReadResult, 0);
            }
        }

        // check the thread's break condition
        m_BreakLock.lock();
        if(m_bBreak == true)
        {
            m_BreakLock.unlock();
            FILE_LOG_L(laCONFIG, llDEBUG) << "  CANReceiveThread::run() -> break";
            break;
        }
        m_BreakLock.unlock();
    }
}

/****************************************************************************/
/*!
 *  \brief  Set break condition for exit
 */
/****************************************************************************/
void CANReceiveThread::SetBreak()
{
    m_BreakLock.lock();
    m_bBreak = true;
    m_BreakLock.unlock();
}

/****************************************************************************/
/*!
 *  \brief  Constructor for the CANTransmitThread class
 *
 *  \iparam pCANCommunicator = CANCommunicator classed for transmit queue access
 */
/****************************************************************************/
CANTransmitThread::CANTransmitThread(CANCommunicator* pCANCommunicator) : QThread()
{
    m_bBreak = false;
    m_pCANCommunicator = pCANCommunicator;
    m_pClient = 0;
}

/****************************************************************************/
/**
 *  \brief  The transmit thread's execution function
 *
 *      The function checks for pending CAN messages to send.
 *      If any, the CAN message will be taken from the send queue and send via CAN bus
 *      (If the simulation is active, the massage's data will be send via tcp-client)
 *
 *      If no further CAN messages left for sending, the thread blocks until the
 *      wait condition wakes them up again.
 *
 *      The wait condition will be set by CANCommunicator::SendCOB  if a new CAN-message
 *      was put into the send queue
 */
/****************************************************************************/
void CANTransmitThread::run()
{
    struct can_frame canframeToSend;
    int nWriteResult;

    forever
    {
        while (m_pCANCommunicator->IsOutMessagePending())
        {
            // pop message from send queue
            canframeToSend = m_pCANCommunicator->PopPendingOutMessage();

            if(m_pClient)
            {
                // running in simulation mode, use the tcp client to transmit the CAN-message data
                m_pClient->SendMessage(&canframeToSend);
            }
            else
            {
                // write it to the CAN bus socket layer
                nWriteResult = m_pCANCommunicator->GetCANInterface()->Write(&canframeToSend);
                if(nWriteResult < 0)
                {
                    m_pCANCommunicator->SetCommunicationError(nWriteResult, 0);
                }
            }
        }

        //initially lock the mutex
        m_lockCANTransmit.lock();
        // wait on the wait condition
        if(m_waitCondCANTransmit.wait(&m_lockCANTransmit) != false)
        {
            FILE_LOG_L(laCOMM, llDEBUG3) << " CANTransmitThread::wait() -> true";
        }
        m_lockCANTransmit.unlock();

        // check the thread's break condition
        m_BreakLock.lock();
        if(m_bBreak == true)
        {
            m_BreakLock.unlock();
            FILE_LOG_L(laCONFIG, llDEBUG) << "  CANTransmitThread::run() -> break";
            break;
        }
        m_BreakLock.unlock();
    }
}

/****************************************************************************/
/*!
 *  \brief  Set break condition for exit
 */
/****************************************************************************/
void CANTransmitThread::SetBreak()
{
    m_BreakLock.lock();
    m_bBreak = true;
    m_BreakLock.unlock();
}

} //namespace
