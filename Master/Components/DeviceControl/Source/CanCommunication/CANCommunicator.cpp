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
#include "DeviceControl/Include/CanCommunication/CANCommunicator.h"
#include "DeviceControl/Include/SlaveModules/Module.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

#include "Global/Include/Utils.h"
//lint -sem(DeviceControl::CANCommunicator::StopComm,cleanup)

namespace DeviceControl
{

#define ERR_COMM_BUS_OFF -1   //!< error 'Bus Off'
#define ERR_COMM_NONE     0   //!< no communication error

/****************************************************************************/
/*!
 *  \brief  Constructor for the CANCommunicator
 *  \iparam pParent is QObject
 */
/****************************************************************************/
CANCommunicator::CANCommunicator(QObject * pParent)
    : QObject(pParent)
    , m_pCANReceiveThread(0)
    , m_pCANTransmitThread(0)
{
    // initialize and increment instance ids
    m_nErrorCode = ERR_COMM_NONE;
    m_errorUntransmitted = 0;
}

/****************************************************************************/
/*!
 *  \brief  Destructor of the CANCommunicator class
 */
/****************************************************************************/
CANCommunicator::~CANCommunicator()
{
    try {
        StopComm();
    }
    CATCHALL_DTOR()
}

/****************************************************************************/
/*!
 *  \brief Start the can communication
 *
 *      The function creates and starts the receive and transmit thread
 *
 *  \iparam Interface = CAN network interface
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CANCommunicator::StartComm(const char *Interface)
{
    FILE_LOG_L(laINIT, llDEBUG) << " enter function";
    FILE_LOG_L(laDEVPROC, llDEBUG) << " CANCommunicator::CANCommunicator Thread: " << QThread::currentThreadId();

    if(m_CANInterface.Open(Interface) < 0) {
        //emit ReportEvent(EVENT_DEVICECONTROL_ERROR_CANBUS_OPEN, 0);
        return DCL_ERR_FCT_CALL_FAILED;
    }

    FILE_LOG_L(laINIT, llDEBUG) << " create transmit thread";
    m_pCANTransmitThread = new CANTransmitThread(this, m_CANInterface);
    if (!m_pCANTransmitThread) {
        StopComm();
        return DCL_ERR_FCT_CALL_FAILED;
    }

    CONNECTSIGNALSLOT(m_pCANTransmitThread, ReportEvent(quint32, quint16),
                      this, OnReportEvent(quint32, quint16));

    FILE_LOG_L(laINIT, llDEBUG) << " create receive thread";
    m_pCANReceiveThread = new CANReceiveThread(this, m_CANInterface);
    if (!m_pCANReceiveThread) {
        StopComm();
        return DCL_ERR_FCT_CALL_FAILED;
    }

    CONNECTSIGNALSLOT(m_pCANReceiveThread, ReportEvent(quint32, quint16),
                      this, OnReportEvent(quint32, quint16));

    FILE_LOG_L(laINIT, llDEBUG) << " start receive thread";
    m_pCANReceiveThread->start();

    FILE_LOG_L(laINIT, llDEBUG) << " start transmit thread: " << m_pCANTransmitThread;
    m_pCANTransmitThread->start();

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief  Hanlde and forward error information
 *
 *  \iparam EventCode = Event code
 *  \iparam EventData = Error data
 */
/****************************************************************************/
void CANCommunicator::OnReportEvent(quint32 EventCode, quint16 EventData)
{
//    if (EVENT_DEVICECONTROL_ERROR_CANBUS_DOWN == EventCode) {
//        StopComm();
//    }
    emit ReportEvent(EventCode, EventData);
}

/****************************************************************************/
/*!
 *  \brief  Stop the can bus communication
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
void CANCommunicator::StopComm()
{
    FILE_LOG_L(laINIT, llDEBUG) << " StopComm";
    if(m_pCANTransmitThread) {

        FILE_LOG_L(laINIT, llDEBUG) << " -- Break transmit thread";
        m_pCANTransmitThread->SetBreak();
        m_pCANTransmitThread->WakeUp();

        FILE_LOG_L(laINIT, llDEBUG) << " -- wait for transmit thread";
        if(m_pCANTransmitThread->wait() == false) {
            FILE_LOG_L(laINIT, llDEBUG) << " -- wait finished for transmit thread";
        }

        FILE_LOG_L(laINIT, llDEBUG) << " -- delete transmit thread and interface";
        delete m_pCANTransmitThread;
        m_pCANTransmitThread = NULL;
    }

    if((m_pCANReceiveThread)) {

        FILE_LOG_L(laINIT, llDEBUG) << " -- receive break set";
        m_pCANReceiveThread->SetBreak();

        FILE_LOG_L(laINIT, llDEBUG) << " -- nw wait until finished for receive thread";
        if(m_pCANReceiveThread->wait() == false) {
            FILE_LOG_L(laINIT, llDEBUG) << " -- wait finished for receive thread";
        }

        FILE_LOG_L(laINIT, llDEBUG) << " -- delete receive thread and interface";
        delete m_pCANReceiveThread;
        m_pCANReceiveThread = NULL;
    }

    FILE_LOG_L(laINIT, llDEBUG) << " -- close can interface";
    m_CANInterface.Close();

    // Purge queues of pending messages
    m_mutexCOB.lock();
    m_SendQueue.clear();
    m_mutexCOB.unlock();
}

void CANCommunicator::ReportCANError(quint32 ErrorID)
{
    Q_UNUSED(ErrorID)
    qDebug()<< "********* error happen";
    emit ReportError(DEVICE_INSTANCE_ID_CAN_COMMUTOR, EVENT_GRP_DCL_CANINTERFACE, (quint16)DCL_ERR_CANCOMMUTOR_COMM_FAILED,
                     (quint16)DCL_ERR_CANCOMMUTOR_COMM_FAILED,Global::AdjustedTime::Instance().GetCurrentDateTime());
}


/****************************************************************************/
/*!
 *  \brief  Returns the CAN interface object
 *
 *  \return CAN interface object
 */
/****************************************************************************/
const CANInterface &CANCommunicator::GetCANInterface()
{
    return m_CANInterface;
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
 *  \iparam canmsg = The CAN message to be sent
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful copied to transmit queue (but still not sent!)
 *          DCL_ERR_CANBUS_NOT_READY it the CAN bus is not ready
 *          DCL_ERR_CANBUS_ERROR if an error exists
 */
/****************************************************************************/
ReturnCode_t CANCommunicator::SendCOB(can_frame& canmsg)
{
    if (!m_pCANTransmitThread) {
        return DCL_ERR_CANBUS_NOT_READY;
    }

    FILE_LOG_L(laCOMM, llDEBUG3)  << "wake up transmit thread for CAN-ID: " << std::hex << canmsg.can_id;

    m_mutexCOB.lock();
    m_SendQueue.push_back(canmsg);      // copy the message to the send queue
    m_mutexCOB.unlock();

    m_pCANTransmitThread->WakeUp();

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief  Dispatches a receiceived CAN message
 *
 *      The HandleCanMessage(..) function of the object which registered  the
 *      CAN message will be called
 *
 *  \iparam canmsg = The CAN message to be sent
 */
/****************************************************************************/
void CANCommunicator::DispatchMessage(can_frame& canmsg)
{
    CModule *pCANObjectBase;
    std::map<__u32, CModule*>::iterator iterCOBAssign;

    iterCOBAssign = m_cobMap.find(canmsg.can_id);
    if(iterCOBAssign != m_cobMap.end())
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
 *  \brief Checks whether a CAN-message is pending to be sent.
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

} //namespace
