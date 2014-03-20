/****************************************************************************/
/*! \file AnalogOutput.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CAnalogOutput
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

#include "DeviceControl/Include/SlaveModules/AnalogOutput.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief    Constructor for the CAnalogOutput
 *
 *  \param    p_MessageConfiguration = Message configuration
 *  \param    pCANCommunicator = pointer to communication class
 *  \param    pParentNode = pointer to base module, where this module is assigned to
 *
 ****************************************************************************/
CAnalogOutput::CAnalogOutput(const CANMessageConfiguration* p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                             CBaseModule* pParentNode) :
    CFunctionModule(CModuleConfig::CAN_OBJ_TYPE_ANALOG_OUT_PORT , p_MessageConfiguration, pCANCommunicator, pParentNode),
    m_unCanIDAnaOutputConfigOutput(0), m_unCanIDAnaOutputStateSet(0), m_unCanIDAnaOutputStateReq(0),
    m_unCanIDAnaOutputState(0), m_unCanIDAnaOutputLifeTimeReq(0), m_unCanIDAnaOutputLifeTime(0)
{
    m_mainState = FM_MAIN_STATE_BOOTUP;
    m_SubStateConfig = FM_AOUTP_SUB_STATE_CONFIG_INIT;

    //module command array initialisation
    for(quint8 idx = 0; idx < MAX_AOUTP_CMD_IDX; idx++)
    {
        m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_FREE;
    }
}

/****************************************************************************/
/*!
 *  \brief    Destructor of CANAnalogOutput
 *
 ****************************************************************************/
CAnalogOutput::~CAnalogOutput()
{

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
ReturnCode_t CAnalogOutput::Initialize()
{
    ReturnCode_t retval;

    retval = InitializeCANMessages();
    if(retval == DCL_ERR_FCT_CALL_SUCCESS)
    {
        retval = RegisterCANMessages();
        if(retval == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_mainState = FM_MAIN_STATE_INIT;
            FILE_LOG_L(laINIT, llDEBUG) << " function module initialized: " << GetName().toStdString();
        }
    }

    return retval;
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
ReturnCode_t CAnalogOutput::InitializeCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 bIfaceID;
    const quint8 ModuleID = MODULE_ID_ANALOG_OUT;

    bIfaceID = m_pCANObjectConfig->m_sChannel;

    RetVal = InitializeEventCANMessages(ModuleID);

    m_unCanIDAnaOutputConfigOutput = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AnalogOutputConfigOutput", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDAnaOutputStateSet     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AnalogOutputStateSet", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDAnaOutputStateReq     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AnalogOutputStateReq", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDAnaOutputState        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AnalogOutputState", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDAnaOutputLifeTimeReq  = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AnalogOutputLifetimeReq", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDAnaOutputLifeTime     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AnalogOutputLifetime", bIfaceID, m_pParent->GetNodeID());

    FILE_LOG_L(laINIT, llDEBUG) << "  CAN-messages for fct-module: " << GetName().toStdString() << ",node id:" << std::hex << m_pParent->GetNodeID();
    FILE_LOG_L(laINIT, llDEBUG) << "   EventInfo               : 0x" << std::hex << m_unCanIDEventInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventWarning            : 0x" << std::hex << m_unCanIDEventWarning;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventError              : 0x" << std::hex << m_unCanIDEventError;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventFatalError         : 0x" << std::hex << m_unCanIDEventFatalError;
    FILE_LOG_L(laINIT, llDEBUG) << "   AnalogOutputConfigOutput: 0x" << std::hex << m_unCanIDAnaOutputConfigOutput;
    FILE_LOG_L(laINIT, llDEBUG) << "   AnalogOutputStateSet    : 0x" << std::hex << m_unCanIDAnaOutputStateSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   AnalogOutputStateReq    : 0x" << std::hex << m_unCanIDAnaOutputStateReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   AnalogOutputState       : 0x" << std::hex << m_unCanIDAnaOutputState;
    FILE_LOG_L(laINIT, llDEBUG) << "   AnalogOutputLifetimeReq : 0x" << std::hex << m_unCanIDAnaOutputLifeTimeReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   AnalogOutputLifetime    : 0x" << std::hex << m_unCanIDAnaOutputLifeTime;

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
ReturnCode_t CAnalogOutput::RegisterCANMessages()
{
    ReturnCode_t RetVal;

    RetVal = RegisterEventCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDAnaOutputState, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDAnaOutputLifeTime, this);
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
void CAnalogOutput::HandleTasks()
{
    QMutexLocker Locker(&m_Mutex);

    if(FM_MAIN_STATE_IDLE == m_mainState)
    {
        HandleIdleState();
    }
    else if(FM_MAIN_STATE_CONFIRMED == m_mainState)
    {
        //fall through
        m_mainState = FM_MAIN_STATE_CONFIG;
        m_SubStateConfig = FM_AOUTP_SUB_STATE_CONFIG_START;
    }
    else if(FM_MAIN_STATE_CONFIG == m_mainState)
    {
        if(FM_AOUTP_SUB_STATE_CONFIG_START == m_SubStateConfig)
        {
            ReturnCode_t RetVal;

            RetVal = SendCANMessageConfiguration();
            if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_mainState = FM_MAIN_STATE_IDLE;
                m_SubStateConfig = FM_AOUTP_SUB_STATE_CONFIG_FINISHED;
                m_TaskID = MODULE_TASKID_FREE;
                FILE_LOG_L(laCONFIG, llDEBUG) << "   Module " << GetName().toStdString() << ": change to FM_MAIN_STATE_IDLE";
            }
            else
            {
                m_mainState = FM_MAIN_STATE_ERROR;
                m_SubStateConfig = FM_AOUTP_SUB_STATE_CONFIG_ERROR;

                FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCOB returns" << (int) RetVal;
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the function module's state machine in idle main state
 *
 *   Depending on the active task ID, the appropriate task function will be called.
 *
 *  \return   void
 */
/****************************************************************************/
void CAnalogOutput::HandleIdleState()
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        HandleCommandRequestTask();
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the task to execute the module commands
 *
 *   The function is called from HandleIdleState() if m_taskID == FM_DOUTP_TASKID_COMMAND_HDL.
 *   The function loops thru the m_ModuleCommand array and controls the commands pending for execution there
 *
 *    - Module command with state MODULE_CMD_STATE_REQ will be forwarded to the function module on slave side by sending
 *      the corresponding CAN-message. After sending the command's state will be set to MODULE_CMD_STATE_REQ_SEND
 *
 *    - Module command with state MODULE_CMD_STATE_REQ_SEND will be checked for timeout.
 *      Usually, a module command will be confirmed and closed by receiving the expected CAN-message (e.g. acknowledge, or data)
 *      The state will be exited when the expected CAN-message was received. this is done at the HandleCANMessage... - function
 *
 *    - Module command with state MODULE_CMD_STATE_ERR will be set to MODULE_CMD_STATE_FREE, its error informations are forwarded
 *      to the device control layer by calling whose ThrowError(...)-function
 *
 */
/****************************************************************************/
void CAnalogOutput::HandleCommandRequestTask()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_NOT_FOUND;
    bool ActiveCommandFound = false;

    for(quint8 idx = 0; idx < MAX_AOUTP_CMD_IDX; idx++)
    {
        if(m_ModuleCommand[idx].m_State == MODULE_CMD_STATE_REQ)
        {
            // forward the module command to the function module on slave side by sending
            // the corresponding CAN-message
            ActiveCommandFound = true;
            if(m_ModuleCommand[idx].m_Type == FM_AO_CMD_TYPE_SET_OUTP)
            {
                //send the requested output value to the slave by sending the CAN message
                FILE_LOG_L(laFCT, llINFO) << " CANAnalogOutput: Set output state.";
                RetVal = SendCANMsgAnalogOutputStateSet(m_ModuleCommand[idx].m_OutputValue,
                                                        m_ModuleCommand[idx].m_Duration,
                                                        m_ModuleCommand[idx].m_Delay);

                m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_FREE;
                emit ReportOutputValueAckn(GetModuleHandle(), RetVal, m_ModuleCommand[idx].m_OutputValue);
            }
            else if(m_ModuleCommand[idx].m_Type == FM_AO_CMD_TYPE_REQ_ACTVALUE)
            {
                //send the value request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDAnaInputState CAN-message
                FILE_LOG_L(laFCT, llINFO) << " CANAnalogOutput: output value req.: send request.";
                RetVal = SendCANMsgAnalogOutputStateReq();

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_AOUTP_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportActOutputValue(GetModuleHandle(), RetVal, 0);
                }
            }
            else if(m_ModuleCommand[idx].m_Type == FM_AO_CMD_TYPE_REQ_LIFECYCLE)
            {
                //send the value request to the slave, this command will be acknowledged by the receiption
                // of the LivetimeData CAN-message
                FILE_LOG_L(laFCT, llINFO) << " CANAnalogOutput: live data req.: send request.";
                RetVal = SendCANMsgLifeTimeDataReq();

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_AOUTP_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportLifeTimeData(GetModuleHandle(), RetVal, 0, 0);
                }
            }

            //check for success
            if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
            {
                //trigger timeout supervision
                m_ModuleCommand[idx].m_ReqSendTime.Trigger();
            }
            else
            {
                m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_FREE;
            }
        }
        else if(m_ModuleCommand[idx].m_State == MODULE_CMD_STATE_REQ_SEND)
        {
            // check avtive module commands for timeout
            ActiveCommandFound = true;
            if(m_ModuleCommand[idx].m_ReqSendTime.Elapsed() > m_ModuleCommand[idx].m_Timeout)
            {
                m_lastErrorHdlInfo = DCL_ERR_TIMEOUT;
                m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_FREE;

                if(m_ModuleCommand[idx].m_Type == FM_AO_CMD_TYPE_REQ_ACTVALUE)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANAnalogOutput:: '" << GetKey().toStdString() << "': output value req. timeout";
                    emit ReportActOutputValue(GetModuleHandle(), m_lastErrorHdlInfo, 0);
                }
                else if(m_ModuleCommand[idx].m_Type == FM_AO_CMD_TYPE_REQ_LIFECYCLE)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANAnalogOutput:: '" << GetKey().toStdString() << "': life time data req. timeout";
                    emit ReportLifeTimeData(GetModuleHandle(), m_lastErrorHdlInfo, 0, 0);
                }
            }
        }
    }

    if(ActiveCommandFound == false)
    {
        m_TaskID = MODULE_TASKID_FREE;
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the reception of a CAN message
 *
 *   The function is called from communication layer if a CAN message, which
 *   was registered to this class instance, was received.
 *   The message will be forwarded to the specialized function.
 *
 *  \iparam   pCANframe = struct contains the data of the receipt CAN message
 *
 *  \return   void
 */
/****************************************************************************/
void CAnalogOutput::HandleCanMessage(can_frame* pCANframe)
{
    QMutexLocker Locker(&m_Mutex);

    if((pCANframe->can_id == m_unCanIDEventInfo) ||
       (pCANframe->can_id == m_unCanIDEventWarning) ||
       (pCANframe->can_id == m_unCanIDEventError) ||
       (pCANframe->can_id == m_unCanIDEventFatalError))
    {
        HandleCANMsgError(pCANframe);
        if ((pCANframe->can_id == m_unCanIDEventError) || (pCANframe->can_id == m_unCanIDEventFatalError)) {
            emit ReportError(GetModuleHandle(), m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
    }
    else if(pCANframe->can_id == m_unCanIDAnaOutputState)
    {
        HandleCANMsgAnalogOutputState(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDAnaOutputLifeTime)
    {
        HandleCANMsgAnalogOutputLifetime(pCANframe);
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the reception of the 'OutputState' CAN message
 *
 *
 *  \iparam   pCANframe = struct contains the data of the receipt CAN message
 *
 *  \return   void
 */
/****************************************************************************/
void CAnalogOutput::HandleCANMsgAnalogOutputState(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_AO_CMD_TYPE_REQ_ACTVALUE);
    }

    if(pCANframe->can_dlc == 8)
    {
        quint16 OutputValue;

        OutputValue = (quint16) pCANframe->data[0] << 8;
        OutputValue |= (quint16) pCANframe->data[1];
        FILE_LOG_L(laFCT, llINFO) << "  CANAnalogOutput: act. output state: " << (int) OutputValue;

        emit ReportActOutputValue(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, OutputValue);
    }
    else
    {
        emit ReportActOutputValue(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the reception of the 'Lifetime' CAN message
 *
 *  \iparam   pCANframe = struct contains the data of the receipt CAN message
 *
 *  \return   void
 */
/****************************************************************************/
void CAnalogOutput::HandleCANMsgAnalogOutputLifetime(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_AO_CMD_TYPE_REQ_LIFECYCLE);
    }

    if(pCANframe->can_dlc == 8)
    {
        quint32 LifeTime, LifeCycles;

        LifeTime = (((quint32) pCANframe->data[0]) << 24);
        LifeTime |= (((quint32) pCANframe->data[1]) << 16);
        LifeTime |= (((quint32) pCANframe->data[2]) << 8);
        LifeTime |= ((quint32) pCANframe->data[3]);

        LifeCycles = (((quint32) pCANframe->data[0]) << 24);
        LifeCycles |= (((quint32) pCANframe->data[1]) << 16);
        LifeCycles |= (((quint32) pCANframe->data[2]) << 8);
        LifeCycles |= ((quint32) pCANframe->data[3]);

        FILE_LOG_L(laFCT, llDEBUG) << " CANAnalogOutput: " << (int) LifeTime << ", " << (int) LifeCycles;

        emit ReportLifeTimeData(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, LifeTime, LifeCycles);
    }
    else
    {
        emit ReportLifeTimeData(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief    Send the 'Configuration' CAN-Message
 *
 *   The function module's configuration parameter will be sent via CAN-Bus to the slave.
 *   The function module on slave side needs the following parameters
 *    -
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CAnalogOutput::SendCANMessageConfiguration()
{
    CANFctModuleAnalogOutput* pCANObjConfAnaOutPort;
    pCANObjConfAnaOutPort = (CANFctModuleAnalogOutput*) m_pCANObjectConfig;
    can_frame canmsg;
    ReturnCode_t RetVal;

    FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration (I/O-dir): 0x" << std::hex << m_unCanIDAnaOutputConfigOutput;
    canmsg.can_id = m_unCanIDAnaOutputConfigOutput;
    canmsg.can_dlc = 6;

    canmsg.data[0] = 0;
    if(pCANObjConfAnaOutPort->m_bEnabled)
    {
        canmsg.data[0] = 0x80;
    }
    if(pCANObjConfAnaOutPort->m_bInaktivAtShutdown)
    {
        canmsg.data[0] |= 0x40;
    }
    if(pCANObjConfAnaOutPort->m_bInaktivAtEmgyStop)
    {
        canmsg.data[0] |= 0x20;
    }

	canmsg.data[1] = pCANObjConfAnaOutPort->m_sBitCount;
    SetCANMsgDataU16(&canmsg, pCANObjConfAnaOutPort->m_sOutvalInactiv, 2);
    SetCANMsgDataU16(&canmsg, pCANObjConfAnaOutPort->m_sLivetimeLimit, 4);


    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Send the CAN-message 'OutputStateSet' to set output state
 *
 *  \iparam   OutputValue = Output value of the analog output FM
 *  \iparam   Duration = Duration of the value
 *  \iparam   Delay = Time delay
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CAnalogOutput::SendCANMsgAnalogOutputStateSet(quint16 OutputValue, quint16 Duration, quint16 Delay)
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDAnaOutputStateSet;

    canmsg.data[0] = 0;
    SetCANMsgDataU16(&canmsg, OutputValue, 1);
    SetCANMsgDataU16(&canmsg, Duration, 3);
    SetCANMsgDataU16(&canmsg, Delay, 5);

    canmsg.can_dlc = 7;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llINFO) << "   CANAnalogOutPort::SendCANMsgAnalogOutValueSet canID: 0x" << std::hex << m_unCanIDAnaOutputStateSet;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Send the CAN-message 'OutputStateReq' to request act. output state
 *
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CAnalogOutput::SendCANMsgAnalogOutputStateReq()
{
    ReturnCode_t retval;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDAnaOutputStateReq;
    canmsg.can_dlc = 0;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    return retval;
}

/****************************************************************************/
/*!
 *  \brief    Send the CAN-message 'LifeTimeDataReq'
 *
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CAnalogOutput::SendCANMsgLifeTimeDataReq()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDAnaOutputLifeTimeReq;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);
    FILE_LOG_L(laFCT, llINFO) << " CANAnalogOutput";

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Set the analog output value
 *
 *            The task will be acknowledged by sending the signal ReportActOutputValue
 *
 *  \iparam   OutputValue = Output value of the analog output FM
 *  \iparam   Duration = Duration of the value
 *  \iparam   Delay = Time delay
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CAnalogOutput::SetOutputValue(quint16 OutputValue, quint16 Duration, quint16 Delay)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_AO_CMD_TYPE_SET_OUTP, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].m_OutputValue = OutputValue;
        m_ModuleCommand[CmdIndex].m_Duration = Duration;
        m_ModuleCommand[CmdIndex].m_Delay = Delay;
        FILE_LOG_L(laDEV, llINFO) << " CAnalogOutput, Value: " << (int) OutputValue;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CAnalogOutput invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Request the actual analog output value
 *            The request will be acknowledged by sending the signal ReportActOutputValue
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CAnalogOutput::ReqOutputValue()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(SetModuleTask(FM_AO_CMD_TYPE_REQ_ACTVALUE))
    {
        FILE_LOG_L(laDEV, llDEBUG) << " CANAnalogOutput";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANAnalogOutput invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Request the analog output's life cycle data
 *            The request will be acknowledged by sending the signal ReportLifeTimeData
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CAnalogOutput::ReqLifeTimeData()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(SetModuleTask(FM_AO_CMD_TYPE_REQ_LIFECYCLE))
    {
        FILE_LOG_L(laDEV, llDEBUG) << " CANAnalogOutput";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANAnalogOutput invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Helper function, sets a free module command to the given command type
 *
 *  \iparam CommandType = command type to set
 *  \iparam pCmdIndex = pointer to index within the command array the command is set to (optional parameter, default 0)
 *
 *  \return true, if the command type can be placed, otherwise false
 */
/****************************************************************************/
bool CAnalogOutput::SetModuleTask(CANAnalogOutputModuleCmdType_t CommandType, quint8* pCmdIndex)
{
    bool CommandAdded = false;

    if((m_TaskID == MODULE_TASKID_FREE) || (m_TaskID == MODULE_TASKID_COMMAND_HDL))
    {
        for(quint8 idx = 0; idx < MAX_AOUTP_CMD_IDX; idx++)
        {
            if(m_ModuleCommand[idx].m_State == MODULE_CMD_STATE_FREE)
            {
                m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_REQ;
                m_ModuleCommand[idx].m_Type = CommandType;

                m_TaskID = MODULE_TASKID_COMMAND_HDL;
                CommandAdded  = true;
                if(pCmdIndex)
                {
                    *pCmdIndex = idx;
                }

                FILE_LOG_L(laFCT, llINFO) << " CANAnalogOutput:  task " << (int) idx << " request.";
                break;
            }
        }
    }

    return CommandAdded;
}

/****************************************************************************/
/*!
 *  \brief  Set the ModuleCommands with the specified command type to 'FREE'
 *
 *  \iparam ModuleCommandType = ModuleCommands having this command type will be set to free
 */
/****************************************************************************/
void CAnalogOutput::ResetModuleCommand(CANAnalogOutputModuleCmdType_t ModuleCommandType)
{
    bool ActiveCommandFound = false;

    for(quint8 idx = 0; idx < MAX_AOUTP_CMD_IDX; idx++)
    {
        if((m_ModuleCommand[idx].m_Type == ModuleCommandType) &&
           (m_ModuleCommand[idx].m_State == MODULE_CMD_STATE_REQ_SEND))
        {
            m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_FREE;
        }

        if(m_ModuleCommand[idx].m_State != MODULE_CMD_STATE_FREE)
        {
            ActiveCommandFound = true;
        }
    }

    if(ActiveCommandFound == false)
    {
        m_TaskID = MODULE_TASKID_FREE;
    }
}

} // namespace
