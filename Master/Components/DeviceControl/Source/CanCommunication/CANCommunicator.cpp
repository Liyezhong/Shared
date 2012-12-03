/****************************************************************************/
/*! \file CANCommunicator.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CANCommunicator
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

#include <QString>

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "DeviceControl/Include/CanCommunication/CANThreads.h"
#include "DeviceControl/Include/CanCommunication/CANInterface.h"
#include "DeviceControl/Include/CanCommunication/CANCommunicator.h"
#include "DeviceControl/Include/CanCommunication/CanTcpCommunication/client.h"
#include "DeviceControl/Include/SlaveModules/Module.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

//! Number of instances of the class CANCommunicator
static int m_nInstanceCnt_s = 0;

#define ERR_COMM_BUS_OFF -1   //!< error 'Bus Off'
#define ERR_COMM_NONE     0   //!< no communication error

/****************************************************************************/
/*!
 *  \brief  Constructor for the CANCommunicator
 */
/****************************************************************************/
CANCommunicator::CANCommunicator() :
    m_pCANReceiveThread(0), m_pCANTransmitThread(0), m_pCANInterface(0), m_pClient(0), m_bUseTcp(false),
    m_nErrorState(0), m_nErrorAdditionalInfo(0)
{
    // initialize and increment instance ids
    m_nInstanceNo = m_nInstanceCnt_s++;
    m_nErrorCode = ERR_COMM_NONE;
    errorUntransmitted = 0;

    m_pCANInterface = new CANInterface();
}

/****************************************************************************/
/*!
 *  \brief  Destructor of the CANCommunicator class
 */
/****************************************************************************/
CANCommunicator::~CANCommunicator()
{
    try
    {
        delete m_pCANInterface;
        delete m_pClient;
    }
    catch (...)
    {
        // and exit
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief Start the can communication
 *
 *      The function creates and starts the receive and transmit thread
 *
 *  \iparam ifaceCAN = CAN network interface
 *
 *  \return 0 on success, otherwise error code
 */
/****************************************************************************/
qint16 CANCommunicator::StartComm(const char* ifaceCAN)
{
    qint16 sReturn = 0;

    FILE_LOG_L(laINIT, llDEBUG) << " enter function";
    FILE_LOG_L(laDEVPROC, llDEBUG) << " CANCommunicator::CANCommunicator Thread: " << QThread::currentThreadId();


    if(!m_bUseTcp)
    {
        sReturn = OpenCAN(ifaceCAN);

        if(sReturn >= 0)
        {
            FILE_LOG_L(laINIT, llDEBUG) << " create transmit thread";
            m_pCANTransmitThread = new CANTransmitThread(this);

            FILE_LOG_L(laINIT, llDEBUG) << " create receive thread";
            m_pCANReceiveThread = new CANReceiveThread(this);

            FILE_LOG_L(laINIT, llDEBUG) << " start receive thread";
            m_pCANReceiveThread->start();

            FILE_LOG_L(laINIT, llDEBUG) << " start transmit thread: " << m_pCANTransmitThread;
            m_pCANTransmitThread->start();
        }
    }
    else
    {
        FILE_LOG_L(laINIT, llDEBUG) << "  start tcp connection, create Client... ";
        m_pClient = new Can2TcpClient();

        if (!m_pClient->Initialize()) {
            FILE_LOG_L(laINIT, llDEBUG) << "  CANCommunicator ERROR: cannot initialize TCP-Client... ";
            sReturn = -1;
        }
        else
        {
            FILE_LOG_L(laINIT, llDEBUG) << "create transmit thread";
            m_pCANTransmitThread = new CANTransmitThread(this);

            FILE_LOG_L(laINIT, llDEBUG) << "  tcp connection installed.";
            m_pCANTransmitThread->SetTcpClient(m_pClient);

            FILE_LOG_L(laINIT, llDEBUG) << "start transmit thread: " << m_pCANTransmitThread;
            m_pCANTransmitThread->start();

        }
    }

    FILE_LOG_L(laINIT, llDEBUG) << "exit function:" << sReturn;

    return sReturn;
}

/****************************************************************************/
/*!
 *  \brief  Stop the can bus communication
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CANCommunicator::StopComm()
{
    ReturnCode_t RetCode = DCL_ERR_FCT_CALL_SUCCESS;

    if(m_pCANTransmitThread == 0)
    {
        RetCode = DCL_ERR_NULL_PTR_ACCESS;
        FILE_LOG_L(laINIT, llDEBUG) << " StopComm";
        return RetCode;
    }

    FILE_LOG_L(laINIT, llDEBUG) << " StopComm";
    FILE_LOG_L(laINIT, llDEBUG) << " -- Break transmit thread";
    m_pCANTransmitThread->SetBreak();
    m_pCANTransmitThread->m_waitCondCANTransmit.wakeOne();

    FILE_LOG_L(laINIT, llDEBUG) << " -- wait for transmit thread";
    if(m_pCANTransmitThread->wait() == false)
    {
        FILE_LOG_L(laINIT, llDEBUG) << " -- wait finished for transmit thread";
    }

    if(!m_bUseTcp)
    {
        if((m_pCANReceiveThread == 0))
        {
            RetCode = DCL_ERR_NULL_PTR_ACCESS;
            return RetCode;
        }
        else
        {
            FILE_LOG_L(laINIT, llDEBUG) << " -- receive break set";
            m_pCANReceiveThread->SetBreak();

            FILE_LOG_L(laINIT, llDEBUG) << " -- nw wait until finished for receive thread";
            if(m_pCANReceiveThread->wait() == false)
            {
                FILE_LOG_L(laINIT, llDEBUG) << " -- wait finished for receive thread";
            }

            FILE_LOG_L(laINIT, llDEBUG) << " -- close can interface";
            m_pCANInterface->Close();

            FILE_LOG_L(laINIT, llDEBUG) << " --  delete threads and interface";
            delete m_pCANReceiveThread;
            m_pCANReceiveThread = NULL;
            delete m_pCANTransmitThread;
            m_pCANTransmitThread = NULL;
        }
    }
    else
    {
        FILE_LOG_L(laINIT, llDEBUG) << " --  tcp delete client";
        delete m_pClient;
        FILE_LOG_L(laINIT, llDEBUG) << " --  delete transmit thread ";
        delete m_pCANTransmitThread;
    }

    return RetCode;
}

/****************************************************************************/
/*!
 *  \brief  Open the can bus network interface
 *
 *  \iparam ifaceCAN = CAN network interface
 *
 *  \return 0 if successfully opened, otherwise negative error code
 */
/****************************************************************************/
qint16 CANCommunicator::OpenCAN(const char* ifaceCAN)
{
    qint16 sRetval = m_pCANInterface->Open(ifaceCAN);

    FILE_LOG_L(laINIT, llDEBUG) << " can open returns " <<  sRetval;

    return sRetval;
}

/****************************************************************************/
/*!
 *  \brief  Set a communication error
 *
 *  \iparam nError = Error code
 *  \iparam nErrorAdditionalInfo = additional error info
 */
/****************************************************************************/
void CANCommunicator::SetCommunicationError(qint16 nError, qint16 nErrorAdditionalInfo)
{
    //pthread_mutex_lock( &mutexCOB );
    if(nError != m_nErrorCode)
    {
        m_nErrorCode = nError;
        m_nErrorAdditionalInfo = nErrorAdditionalInfo;
        errorUntransmitted = 1;
        FILE_LOG_L(laCAN, llERROR) << "SetCommErr.: " << m_nErrorCode;
    }
    //pthread_mutex_unlock( &mutexCOB );
}

/****************************************************************************/
/*!
 *  \brief  Returns a communication error
 *
 *  \oparam nErrorAdditionalInfo = additional error info
 *
 *  \return Error code
 */
/****************************************************************************/
qint16 CANCommunicator::GetCommunicationError(qint16& nErrorAdditionalInfo)
{
    qint16 errorToTransmit = 0;

    //pthread_mutex_lock( &mutexCOB );
    if(errorUntransmitted)
    {
        errorUntransmitted = 0;
        nErrorAdditionalInfo = m_nErrorAdditionalInfo;
        errorToTransmit = m_nErrorCode;
    }
    //pthread_mutex_unlock( &mutexCOB );
    return errorToTransmit;
}

/****************************************************************************/
/*!
 *  \brief  Returns the communication status
 *
 *  \return Error code
 */
/****************************************************************************/
int CANCommunicator::GetCommunicationStatus()
{
    static int nOldCanStatus = 0;
    int nCanStatus = 0;

    if(m_pCANInterface)
    {
        nCanStatus = m_pCANInterface->GetStatus();
        if(nOldCanStatus != nCanStatus)
        {
            nOldCanStatus = nCanStatus;
        }
    }

    return nCanStatus;

    /*int nError;
    //pthread_mutex_lock( &mutexCOB );
    nError = m_nError;
    //pthread_mutex_unlock( &mutexCOB );
    return m_nError;

    IOC_GET_STATUS*/
}

/****************************************************************************/
/*!
 *  \brief  Returns address of the CAN interface class
 *
 *  \return Address if the CAN interface
 */
/****************************************************************************/
CANInterface* CANCommunicator::GetCANInterface()
{
    return m_pCANInterface;
}

/****************************************************************************/
/*!
 *  \brief  Register a CAN message ID
 *
 *  \iparam unCanID = CAN-ID to regiser
 *  \iparam pCallbackModule = pointer to the regitrating CANObjectBase-instance
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successfully registered, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CANCommunicator::RegisterCOB(unsigned int unCanID, CModule* pCallbackModule)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    FILE_LOG_L(laINIT, llDEBUG)  << "CanID: " << std::hex << unCanID;

    m_cobMap[unCanID] = pCallbackModule;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Copy a CAN message to the transmit queue
 *
 *  \iparam canmsg = the CAN message to send
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful copied to transmit queue (but still not send!)
 *          DCL_ERR_CANBUS_NOT_READY it the CAN bus is not ready
 *          DCL_ERR_CANBUS_ERROR if an error exists
 */
/****************************************************************************/
ReturnCode_t CANCommunicator::SendCOB(can_frame& canmsg)
{

    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    if(m_pCANTransmitThread == 0)
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    if(m_nErrorCode == ERR_COMM_NONE)
    {
        /*if(GetCommunicationStatus() & S_BOff)
            SetCommunicationError(ERR_COMM_BUS_OFF);
        else*/
          //SetCommunicationError(ERR_COMM_NONE);
        FILE_LOG_L(laCOMM, llDEBUG3)  << "wake up transmit thread for CAN-ID: " << std::hex << canmsg.can_id;

        m_mutexCOB.lock();
        m_SendQueue.push_back(canmsg);      // copy the message to the send queue
        m_mutexCOB.unlock();

        qint16 nErrorAddInfo;
        if(GetCommunicationError(nErrorAddInfo) == ERR_COMM_NONE)
        {
            //wake up the transmit threads blocked by the specified condition variable.
            m_pCANTransmitThread->m_waitCondCANTransmit.wakeOne();
        }
        else
        {
            //ReOpenCanBus();
            FILE_LOG_L(laCAN, llERROR) << "GetCommunicationError() returns an error!";
            retval = DCL_ERR_CANBUS_NOT_READY;
        }
    }
    else
    {
        retval = DCL_ERR_CANBUS_ERROR; // in case of an existing error, the message will not be send
    }

    return retval;

}

/****************************************************************************/
/*!
 *  \brief  Dispatches a receiceived CAN message
 *
 *      The HandleCanMessage(..) function of the object which registered  the
 *      CAN message will be called
 *
 *  \iparam canmsg = the CAN message to send
 */
/****************************************************************************/
void CANCommunicator::DispatchMessage(can_frame& canmsg)
{
    CModule *pCANObjectBase;
    std::map<__u32, CModule*>::iterator iterCOBAssign;

    iterCOBAssign = m_cobMap.find(canmsg.can_id);
    if( iterCOBAssign != m_cobMap.end() )
    {
        pCANObjectBase = iterCOBAssign->second;
        FILE_LOG_L(laCAN, llDEBUG2) << " HandleMsg " << std::hex << canmsg.can_id << std::hex << (int)canmsg.data[0] <<
                                                                           " "  << std::hex << (int)canmsg.data[1] <<
                                                                           " "  << std::hex << (int)canmsg.data[2] <<
                                                                           " "  << std::hex << (int)canmsg.data[3] <<
                                                                           " "  << std::hex << (int)canmsg.data[4] <<
                                                                           " "  << std::hex << (int)canmsg.data[5] <<
                                                                           " "  << std::hex << (int)canmsg.data[6] <<
                                                                           " "  << std::hex << (int)canmsg.data[7];
        pCANObjectBase->HandleCanMessage(&canmsg);
    }
    else
    {
        FILE_LOG_L(laCAN, llWARNING) << " not assigned CAN-Msg ID: 0x" << std::hex << canmsg.can_id;
    }
}

/****************************************************************************/
/*!
 *  \brief  Gets a pending CAN-message from receive queue and dispatch it to DCL
 *
 *      The function should be called cyclically to forward received CAN messages
 */
/****************************************************************************/
void CANCommunicator::DispatchPendingInMessage()
{

    while(IsInMessagePending())
    {
        can_frame canmsg = PopPendingInMessage();
        DispatchMessage(canmsg);
    }
}

/****************************************************************************/
/*!
 *  \brief  Append a CAN message to receive queue
 *
 *  \iparam canmsg = The CAN message to insert
 */
/****************************************************************************/
void CANCommunicator::AppendToReceiveQueue(can_frame& canmsg)
{
    m_mutexCOB.lock();
    m_RecvQueue.push_back(canmsg);
    m_mutexCOB.unlock();
}

/****************************************************************************/
/*!
 *  \brief Checks whether a CAN-message is pending to be send.
 *
 *  \return True if a CAN message is pending, otherwise false
 */
/****************************************************************************/
bool CANCommunicator::IsOutMessagePending()
{
    int nSize = 0;

    m_mutexCOB.lock();
    nSize = m_SendQueue.size();
    m_mutexCOB.unlock();

    return (nSize > 0);
}

/****************************************************************************/
/*!
 *  \brief  Checks whether a CAN-message was received and is pending to handled
 *
 *  \return True if a CAN message is pending, otherwise false
 */
/****************************************************************************/
bool CANCommunicator::IsInMessagePending()
{
    int nSize = 0;

    m_mutexCOB.lock();
    nSize = m_RecvQueue.size();
    m_mutexCOB.unlock();

    return (nSize > 0);
}

/****************************************************************************/
/*!
 *  \brief  Pop a pending CAN message from send queue.
 *
 *  \return The popped CAN message.
 */
/****************************************************************************/
can_frame CANCommunicator::PopPendingOutMessage()
{
    can_frame canmsg;

    m_mutexCOB.lock();

    std::deque<can_frame>::iterator iterSendQueue;
    iterSendQueue = m_SendQueue.begin();
    canmsg = *iterSendQueue;
    m_SendQueue.pop_front();

    m_mutexCOB.unlock();

    return canmsg;
}

/****************************************************************************/
/*!
 *  \brief  Pops a pending CAN message from receive queue.
 *
 *  \return The popped CAN message.
 */
/****************************************************************************/
can_frame CANCommunicator::PopPendingInMessage()
{
    can_frame canmsg;

    m_mutexCOB.lock();

    std::deque <can_frame> :: iterator iterRecvQueue;
    iterRecvQueue = m_RecvQueue.begin();  // get the first msg in queue
    canmsg = *iterRecvQueue;         // copy it
    m_RecvQueue.pop_front();              // and delete it from queue

    m_mutexCOB.unlock();

    return canmsg;
}

} //namespace
