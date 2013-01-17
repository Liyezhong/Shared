/****************************************************************************/
/*! \file Uart.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 22.12.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CANUART
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

#include "DeviceControl/Include/SlaveModules/Uart.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief    Constructor for the CANUART
 *
 *  \param    p_MessageConfiguration = Message configuration
 *  \param    pCANCommunicator = pointer to communication class
 *  \param    pParentNode = pointer to CANNode, where this module is assigned to
 *
 ****************************************************************************/
CUart::CUart(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
             CBaseModule* pParentNode) :
    CFunctionModule(CModuleConfig::CAN_OBJ_TYPE_UART, p_MessageConfiguration, pCANCommunicator, pParentNode),
    m_unCanIDError(0), m_unCanIDErrorReq(0),
    m_unCanIDUARTConfig(0), m_unCanIDUARTDataSet(0), m_unCanIDUARTDataAckn(0),
    m_unCanIDUARTDataReq(0), m_unCanIDUARTData(0),
    m_aktionTimespan(0)
{
    m_mainState = FM_MAIN_STATE_BOOTUP;
    m_subStateConfig = FM_UART_SUB_STATE_CONFIG_INIT;

    m_taskState = MODULE_CMD_STATE_FREE;
}

/****************************************************************************/
/*!
 *  \brief    Destructor of CANUART
 *
 ****************************************************************************/
CUart::~CUart()
{
    /// \todo Auto-generated destructor stub
}

/****************************************************************************/
/*!
 *  \brief    Initialize this function module
 *
 *   The CAN-IDs are read from the CAN-Message configuration class, and the CAN-ID are composed
 *   Receiveable CAN-message are registered to the communication layer
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CUart::Initialize()
{
    ReturnCode_t RetVal;

    RetVal = InitializeCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = RegisterCANMessages();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_mainState = FM_MAIN_STATE_INIT;
            FILE_LOG_L(laINIT, llDEBUG) << " function module initialized: " << GetName().toStdString();
        }
    }

    return RetVal;

}

/****************************************************************************/
/*!
 *  \brief    Initialize the module's CAN message IDs
 *
 *   The CAN-IDs are read from the CAN-Message configuration class.
 *   The CAN-ID is composed by the message key, the function
 *   module's channel and the node id of the CANNode this fct-module is assigned to.
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CUart::InitializeCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 bChannel;
    const quint8 ModuleID = MODULE_ID_UART;

    if(m_pCANObjectConfig == 0)
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    bChannel = m_pCANObjectConfig->m_sChannel;

    RetVal = InitializeEventCANMessages(ModuleID);

    m_unCanIDUARTConfig   = mp_MessageConfiguration->GetCANMessageID(ModuleID, "UARTConfig", bChannel, m_pParent->GetNodeID());
    m_unCanIDUARTDataSet  = mp_MessageConfiguration->GetCANMessageID(ModuleID, "UARTDataSet", bChannel, m_pParent->GetNodeID());
    m_unCanIDUARTDataAckn = mp_MessageConfiguration->GetCANMessageID(ModuleID, "UARTDataAckn", bChannel, m_pParent->GetNodeID());
    m_unCanIDUARTDataReq  = mp_MessageConfiguration->GetCANMessageID(ModuleID, "UARTDataReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDUARTData     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "UARTData", bChannel, m_pParent->GetNodeID());

    FILE_LOG_L(laINIT, llDEBUG) << " CAN-messages for fct-module:" << GetName().toStdString() << ",node id:" << std::hex << m_pParent->GetNodeID();
    FILE_LOG_L(laINIT, llDEBUG) << "   EventInfo      : 0x" << std::hex << m_unCanIDEventInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventWarning   : 0x" << std::hex << m_unCanIDEventWarning;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventError     : 0x" << std::hex << m_unCanIDEventError;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventFatalError: 0x" << std::hex << m_unCanIDEventFatalError;
    FILE_LOG_L(laINIT, llDEBUG) << "   Config   : 0x" << std::hex << m_unCanIDUARTConfig;
    FILE_LOG_L(laINIT, llDEBUG) << "   DataSet  : 0x" << std::hex << m_unCanIDUARTDataSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   DataAckn : 0x" << std::hex << m_unCanIDUARTDataAckn;
    FILE_LOG_L(laINIT, llDEBUG) << "   DataReq  : 0x" << std::hex << m_unCanIDUARTDataReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   Data     : 0x" << std::hex << m_unCanIDUARTData;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Register the receive CAN-messages to communication layer
 *
 *   Each receiveable CAN-message must be registered to the communication layer.
 *   This enables the communication layer to call the 'HandelCANMessage(..)' function of this
 *   instance after receiption of the message.
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CUart::RegisterCANMessages()
{
    ReturnCode_t RetVal;

    RetVal = RegisterEventCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDError, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDUARTDataAckn, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDUARTData, this);
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Handles the function module's state machine
 *
 *   Depending on the active main state, the appropriate task function will be called.
 *
 *  \return   void
 */
/****************************************************************************/
void CUart::HandleTasks()
{
    QMutexLocker Locker(&m_Mutex);

    if(m_mainState == FM_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(FM_MAIN_STATE_CONFIRMED == m_mainState)
    {
        //fall through
        m_subStateConfig = FM_UART_SUB_STATE_CONFIG_START;
        m_mainState = FM_MAIN_STATE_CONFIG;
    }
    else if(FM_MAIN_STATE_CONFIG == m_mainState)
    {
        if(FM_UART_SUB_STATE_CONFIG_START == m_subStateConfig)
        {
            m_subStateConfig = FM_UART_SUB_STATE_CONFIG_SEND;
            StartTimeDelay();
        }
        else if(FM_UART_SUB_STATE_CONFIG_SEND == m_subStateConfig)
        {
            ReturnCode_t RetVal;

            RetVal = SendCANMessageConfiguration();
            if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_mainState = FM_MAIN_STATE_IDLE;
                m_subStateConfig = FM_UART_SUB_STATE_CONFIG_FINISH;
                m_TaskID = MODULE_TASKID_FREE;
                FILE_LOG_L(laCONFIG, llDEBUG) << " Module " << GetName().toStdString() << ": change to FM_MAIN_STATE_IDLE";
            }
            else
            {
                FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCOB returns" << (int) RetVal;
                m_lastEventHdlInfo = DCL_ERR_FCT_CALL_FAILED;
                m_subStateConfig = FM_UART_SUB_STATE_CONFIG_ERROR;
                m_mainState = FM_MAIN_STATE_ERROR;
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Send the CAN-Message 'Config'
 *
 *   The function module's configuration parameter will be sent via CAN-Bus to the slave.
 *   The function module on slave side needs the following parameters
 *     - Module enable/disable
 *     - read communiction mode ( 0- send on request / 1 - send independent )
 *     - number of stop bits (0 - one / 1 - two)
 *     - parity bit enable
 *     - parity (0 - odd / 1 - even)
 *
 *  \return   void
 */
/****************************************************************************/
ReturnCode_t CUart::SendCANMessageConfiguration()
{
    CANFctModuleUART* pCANObjConfUART;
    can_frame canmsg;
    ReturnCode_t RetVal;

    pCANObjConfUART = (CANFctModuleUART*) m_pCANObjectConfig;

    if(pCANObjConfUART)
    {
        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDUARTConfig;
        canmsg.can_id = m_unCanIDUARTConfig;
        canmsg.data[0] = pCANObjConfUART->m_bEnabled;
        canmsg.data[1] = pCANObjConfUART->m_bReadCommunicationMode;
        canmsg.data[2] = pCANObjConfUART->m_bParityEnabled;
        canmsg.data[3] = pCANObjConfUART->m_bParity;
        SetCANMsgDataU16(&canmsg, pCANObjConfUART->m_sBaudrate, 4);
        canmsg.data[6] = 0;
        canmsg.data[7] = 0;
        canmsg.can_dlc = 8;

        RetVal = m_pCANCommunicator->SendCOB(canmsg);
    }
    else
    {
        FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": configuration not available";
        m_lastEventHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        RetVal = DCL_ERR_NULL_PTR_ACCESS;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Handles the function module's state machine in idle main state
 *
 *   Depending on the active task ID, the appropriate task function will be called.
 */
/****************************************************************************/
void CUart::HandleIdleState()
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL/*FM_UART_TASKID_SEND_DATA*/) //!< \todo
    {
        HandleSendDataTask();
    }
    else if(m_TaskID == MODULE_TASKID_COMMAND_HDL/*FM_UART_TASKID_REQ_REC_DATA*/) //!< \todo
    {
        HandleReqReceiveDataTask();
    }
}

/****************************************************************************/
/*!
 *  \brief   Handle the task to send data via CAN to fct-module on slave
 */
/****************************************************************************/
void CUart::HandleSendDataTask()
{
    if(m_taskState == MODULE_CMD_STATE_REQ)
    {
        ReturnCode_t retval;

        retval = SendCANMsgSetSendData();
        if(retval == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_aktionTimespan = CAN_UART_TIMEOUT_READ_REQ;
            m_timeAction.Trigger();
            m_taskState = MODULE_CMD_STATE_REQ_SEND;
        }
        else
        {
            ResetTask();
            emit ReportReadData(GetModuleHandle(), retval, 0, 0);
        }
    }
    else if(m_taskState == MODULE_CMD_STATE_REQ_SEND)
    {
        //check timeout
        if(m_timeAction.Elapsed() > m_aktionTimespan)
        {
            FILE_LOG_L(laFCT, llERROR) << "   CANRFID:: task FM_UART_TASKID_SEND_DATA timeout";
            ResetTask();
            emit ReportReadData(GetModuleHandle(), DCL_ERR_TIMEOUT, 0, 0);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief   Handle the task to receive data via CAN fromfct-module on slave
 *
 *           If 'read communication mode' is set to 'send on request', a "receive data request'
 *           CAN-message must be send. Than the state which receipt the data is handled here.
 */
/****************************************************************************/
void CUart::HandleReqReceiveDataTask()
{
    if(m_taskState == MODULE_CMD_STATE_REQ)
    {
        ReturnCode_t retval;

        retval = SendCANMsgReceiveDataRequest();
        if(retval == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_aktionTimespan = CAN_UART_TIMEOUT_REQ_REC_DATA;
            m_timeAction.Trigger();
            m_taskState = MODULE_CMD_STATE_REQ;
        }
        else
        {
            ResetTask();
            emit ReportWriteAckn(GetModuleHandle(), retval);
        }
    }
    else if(m_taskState == MODULE_CMD_STATE_REQ)
    {
        //check timeout
        if(m_timeAction.Elapsed() > m_aktionTimespan)
        {
            FILE_LOG_L(laFCT, llERROR) << "   CANRFID:: task FM_UART_TASKID_REQ_REC_DATA timeout";
            ResetTask();
            emit ReportWriteAckn(GetModuleHandle(), DCL_ERR_TIMEOUT);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of a CAN message
 *
 *   The function is called from communication layer if a CAN message, which
 *   was registered to this class instance, was received.
 *   The message will be forwarded to the specialized function.
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CUart::HandleCanMessage(can_frame* pCANframe)
{
    QMutexLocker Locker(&m_Mutex);

    FILE_LOG_L(laFCT, llDEBUG) << "  CANUART::HandleCanMessage 0x" << std::hex << pCANframe->can_id;
    if(pCANframe->can_id == m_unCanIDError)
    {
        HandleCANMsgEvent(pCANframe);
        if ((pCANframe->can_id == m_unCanIDEventError) || (pCANframe->can_id == m_unCanIDEventFatalError)) {
            emit ReportEvent(BuildEventCode(m_lastEventGroup, m_lastEventCode), m_lastEventData, m_lastEventTime);
        }
    }
    else if(m_unCanIDUARTDataAckn == pCANframe->can_id)
    {
        HandleCANMsgSendDataAckn(pCANframe);
    }
    else if(m_unCanIDUARTData == pCANframe->can_id)
    {
        HandleCANMsgReceiveData(pCANframe);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the receipt of the 'send data ackn' CAN-message
 *
 *  \iparam pCANframe = Received CAN message
 */
/****************************************************************************/
void CUart::HandleCANMsgSendDataAckn(can_frame* pCANframe)
{
    if(pCANframe->can_dlc == 8)
    {
        /*ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        quint8 writeResult;

        writeResult = (quint8) pCANframe->data[0];
        FILE_LOG_L(laFCT, llINFO) << "  CANUART: SendDataAckn received";
        /// \todo hdlInfo = (qint16) writeResult;

        if((m_taskID == FM_RFID_TASKID_WRITE_DATA) && (m_taskState == FM_RFID_TASK_STATE_REQ_SEND))
        {
            ResetTask();
        }

        emit ReportWriteAckn(GetModuleHandle(), hdlInfo);*/
    }
    else
    {
        emit ReportWriteAckn(GetModuleHandle(), DCL_ERR_CANMSG_INVALID);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the receipt of the 'received data' CAN-message
 *
 *  \iparam pCANframe = Received CAN message
 */
/****************************************************************************/
void CUart::HandleCANMsgReceiveData(can_frame* pCANframe)
{
    if(pCANframe->can_dlc == 8)
    {
        /*ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;

        m_RFIDData1 = ((quint32) pCANframe->data[0]) << 24;
        m_RFIDData1 |= ((quint32) pCANframe->data[1]) << 16;
        m_RFIDData1 |= ((quint32) pCANframe->data[2]) << 8;
        m_RFIDData1 |= ((quint32) pCANframe->data[3]);
        m_RFIDData2 = ((quint32) pCANframe->data[4]) << 24;
        m_RFIDData2 |= ((quint32) pCANframe->data[5]) << 16;
        m_RFIDData2 |= ((quint32) pCANframe->data[6]) << 8;
        m_RFIDData2 |= ((quint32) pCANframe->data[7]);
        FILE_LOG_L(laFCT, llINFO) << "   CANRFID::HandleCANMsgReadRFIDDataResp: 0x" << std::hex << m_RFIDData1 <<
                                                        ", 0x" << std::hex << m_RFIDData2 << " received.";

        FILE_LOG_L(laFCT, llINFO) << "   CANRFID::HandleCANMsgReadRFIDDataResp: 0x" << std::hex << m_pCANObjectConfig->m_sCANNodeIndex << "  0x" << std::hex << GetModuleHandle();

        if((m_taskID == FM_RFID_TASKID_READ_DATA) && (m_taskState == FM_RFID_TASK_STATE_REQ_SEND))
        {
            ResetTask();
        }

        emit ReportReadData(GetModuleHandle(), hdlInfo, m_RFIDData1, m_RFIDData2);*/
    }
    else
    {
        emit ReportReadData(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Send CAN-message 'error request'
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CUart::SendCANMsgErrorReqest()
{
    ReturnCode_t retval;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDErrorReq;
    canmsg.data[0] = 0; //
    canmsg.data[1] = 0;
    canmsg.can_dlc = 0;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send CAN-message 'set send data'
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CUart::SendCANMsgSetSendData()
{
    ReturnCode_t retval;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDUARTDataSet;
    canmsg.data[0] = 0;
    /*canmsg.data[0] = (quint8) ((m_rfidWriteDataMSB & 0xFF000000) >> 24);
    canmsg.data[1] = (quint8) ((m_rfidWriteDataMSB & 0x00FF0000) >> 16);
    canmsg.data[2] = (quint8) ((m_rfidWriteDataMSB & 0x0000FF00) >> 8);
    canmsg.data[3] = (quint8) (m_rfidWriteDataMSB & 0x000000FF);
    canmsg.data[4] = (quint8) ((m_rfidWriteDataLSB & 0xFF000000) >> 24);
    canmsg.data[5] = (quint8) ((m_rfidWriteDataLSB & 0x00FF0000) >> 16);
    canmsg.data[6] = (quint8) ((m_rfidWriteDataLSB & 0x0000FF00) >> 8);
    canmsg.data[7] = (quint8) (m_rfidWriteDataLSB & 0x000000FF);*/
    canmsg.can_dlc = 8;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send CAN-message 'request received data'
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CUart::SendCANMsgReceiveDataRequest()
{
    ReturnCode_t retval;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDUARTDataReq;
    canmsg.data[0] = 0;
    /*canmsg.data[0] = (quint8) ((m_rfidWriteDataMSB & 0xFF000000) >> 24);
    canmsg.data[1] = (quint8) ((m_rfidWriteDataMSB & 0x00FF0000) >> 16);
    canmsg.data[2] = (quint8) ((m_rfidWriteDataMSB & 0x0000FF00) >> 8);
    canmsg.data[3] = (quint8) (m_rfidWriteDataMSB & 0x000000FF);
    canmsg.data[4] = (quint8) ((m_rfidWriteDataLSB & 0xFF000000) >> 24);
    canmsg.data[5] = (quint8) ((m_rfidWriteDataLSB & 0x00FF0000) >> 16);
    canmsg.data[6] = (quint8) ((m_rfidWriteDataLSB & 0x0000FF00) >> 8);
    canmsg.data[7] = (quint8) (m_rfidWriteDataLSB & 0x000000FF);*/
    canmsg.can_dlc = 8;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    return retval;
}

/****************************************************************************/
/*!
 *  \brief   The passing datas will be send via CAN-Bus to the function module
 *
 *      This function is called to set the datas to be sent by the UART.
 *      The passed data willbe transmitted via CAN-Bus to the slave's UART-function module
 *      and send by this functionmodule via UAR
 *
 *  \iparam   OutData = Data array containing data to be send
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CUart::SendData(QByteArray OutData)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    Q_UNUSED(OutData);

    if((m_TaskID == MODULE_TASKID_FREE) && (m_taskState == MODULE_CMD_STATE_FREE))
    {
        m_TaskID = MODULE_TASKID_COMMAND_HDL/*FM_UART_TASKID_SEND_DATA*/; //!< \todo
        m_taskState = MODULE_CMD_STATE_REQ;

        /*m_rfidWriteDataMSB = rfidDataMSB;
        m_rfidWriteDataLSB = rfidDataLSB;*/

        FILE_LOG_L(laFCT, llINFO) << " CANUART::WriteData()";
    }
    else
    {
        retval = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << "   CANUART::task FM_UART_TASKID_SEND_DATA, invalid state: " << (int) m_TaskID << ", " << (int) m_taskState;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief   Request the datas received by the slaves's function module
 *
 *      This function is called to request the datas received by the UART.
 *      The passed data will be transmitted via CAN-Bus from the slave's UART-function module
 *      to this class and forwarded by emitting the "receivedData" signal.
 *
 *      The function should exclusively be called if the module is configured to
 *      'read communication mode' = 'send on request'.
 *      Otherwise DCL_ERR_INVALID_STATE will be returned
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CUart::ReqReceivedData()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    if((m_TaskID == MODULE_TASKID_FREE) && (m_taskState == MODULE_CMD_STATE_FREE))
    {
        m_TaskID = MODULE_TASKID_COMMAND_HDL/*FM_UART_TASKID_REQ_REC_DATA*/; //!< \todo
        m_taskState = MODULE_CMD_STATE_REQ;

        FILE_LOG_L(laFCT, llINFO) << "   CANUART::ReqReceivedData()";
    }
    else
    {
        retval = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << "   CANUART::task FM_UART_TASKID_REQ_REC_DATA, invalid state: " << (int) m_TaskID << ", " << (int) m_taskState;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief   Request the task state to 'free'
 *
 *  \return   void
 */
/****************************************************************************/
void CUart::ResetTask()
{
    m_TaskID = MODULE_TASKID_FREE;
    m_taskState = MODULE_CMD_STATE_FREE;
}

}//namespace
