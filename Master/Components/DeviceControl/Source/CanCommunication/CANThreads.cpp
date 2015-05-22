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
#include <linux/can/error.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

#include "DeviceControl/Include/CanCommunication/CANThreads.h"
#include "DeviceControl/Include/CanCommunication/CANInterface.h"
#include "DeviceControl/Include/CanCommunication/CANCommunicator.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor for the CANReceiveThread class
 *
 *  \iparam pCANCommunicator = Class for receive queue access
 *  \iparam rCANInterface = Class for can interface
 */
/****************************************************************************/
CANReceiveThread::CANReceiveThread(CANCommunicator* pCANCommunicator, CANInterface & rCANInterface)
    : QThread(pCANCommunicator)
    , m_pCANCommunicator(pCANCommunicator)
    , m_rCANInterface(rCANInterface)
    , m_bBreak(false)
{
    m_lastErrorFrame.can_id = 0;
}

/****************************************************************************/
/*!
 *  \brief  The receive thread's execution function
 *
 *      The function calls a blocking read and waits until new CAN message
 *      data were received. The new CAN message will be added to the receive
 *      queue and the blocking read will be called again.
 */
/****************************************************************************/
void CANReceiveThread::run()
{
    forever
    {
        if(m_pCANCommunicator != NULL) {
            int ReturnValue = m_rCANInterface.Ready();
            if (ReturnValue < 0) {
                m_pCANCommunicator->ReportCANError();
            }
            else {
                // new msg available
                if (ReturnValue > 0) {
                    // read message from CAN bus (blocking call to socket)
                    struct can_frame frame;
                    int nReadResult = m_rCANInterface.Read(&frame);
                    if (nReadResult != sizeof(frame)) {
                        m_pCANCommunicator->ReportCANError();
                    }
                    else {
#if defined(__arm__) //Target
                        if (0 == (frame.can_id & 0x01)) {   // process only slave messages
#endif
                            //m_pCANCommunicator->DispatchMessage(frame);
                            if (0 == (frame.can_id & CAN_ERR_FLAG)) {
                                // only support EFF
                                if (CAN_EFF_FLAG == (frame.can_id & (CAN_EFF_FLAG | CAN_RTR_FLAG) )) {
                                    // forward message to module
                                    frame.can_id = frame.can_id & CAN_EFF_MASK;
                                    m_pCANCommunicator->DispatchMessage(frame);
                                    if (m_lastErrorFrame.can_id & CAN_ERR_MASK) {
                                        frame.can_id = 0;               // log as resolved
                                        m_lastErrorFrame.can_id = 0;    // reset error frame
                                    }
                                }
                                else {
                                    m_pCANCommunicator->ReportCANError();
                                }
                            }
                            else {
                                // error frame received
                                FILE_LOG_L(laCAN, llWARNING) << " Error-CanID: " << std::hex << (frame.can_id & CAN_ERR_MASK);
                                m_lastErrorFrame = frame;
                            }
#if defined(__arm__) //Target
                        }
#endif
                    }
                }
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
 *  \iparam pCANCommunicator = Class for transmit queue access
 *  \iparam rCANInterface = Class for can interface
 *
 */
/****************************************************************************/
CANTransmitThread::CANTransmitThread(CANCommunicator* pCANCommunicator, CANInterface & rCANInterface)
    : QThread(pCANCommunicator)
    , m_pCANCommunicator(pCANCommunicator)
    , m_rCANInterface(rCANInterface)
    , m_WakeUp(0)
    , m_bBreak(false)
    , m_lastErrno(0)
{
}

/****************************************************************************/
/**
 *  \brief  The transmit thread's execution function
 *
 *      The function checks for pending CAN messages to be sent. If any, the
 *      CAN message will be taken from the send queue and sent via CAN bus.
 *
 *      If no further CAN messages are left for sending, the thread will block
 *      until the wait condition wakes them up again.
 *
 *      The wait condition will be set by CANCommunicator::SendCOB, if a new
 *      CAN-message was put into the send queue.
 */
/****************************************************************************/
void CANTransmitThread::run()
{
    struct can_frame canframeToSend;
    int nWriteResult;
    forever
    {
        // wait until get waked up
        m_WakeUp.acquire();
        while (m_WakeUp.tryAcquire());  // clear sepmaphore

        // check the thread's break condition
        m_BreakLock.lock();
        if(m_bBreak == true)
        {
            m_BreakLock.unlock();
            FILE_LOG_L(laCONFIG, llDEBUG) << "  CANTransmitThread::run() -> break";
            break;
        }
        m_BreakLock.unlock();

        // transmit data from send buffer
        while (m_pCANCommunicator->IsOutMessagePending())
        {
            // pop message from send queue
            canframeToSend = m_pCANCommunicator->PopPendingOutMessage();

            // write it to the CAN bus socket layer
            nWriteResult = m_rCANInterface.Write(canframeToSend);
            if (nWriteResult != sizeof(canframeToSend)) {
                m_pCANCommunicator->ReportCANError();
            }
//                if (m_lastErrno != errno) {
//                    m_pCANCommunicator->ReportCANError();
//                    m_lastErrno = errno;
//                }
//            }
//            else {
//                if (m_lastErrno != errno) {
//                    m_pCANCommunicator->ReportCANError();
//                    m_lastErrno = errno;
//                }
//            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Wake up the transmit thread
 */
/****************************************************************************/
void CANTransmitThread::WakeUp()
{
    m_WakeUp.release();
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
