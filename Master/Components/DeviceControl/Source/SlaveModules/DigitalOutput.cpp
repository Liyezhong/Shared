/****************************************************************************/
/*! \file DigitalOutput.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CANDigitalOutput
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

#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

//<! timeout for data read request commands
//#define CAN_DOUTP_TIMEOUT_READ_REQ  1000

/****************************************************************************/
/*!
 *  \brief  Constructor for the CDigitalOutput
 *
 *  \iparam p_MessageConfiguration = Message configuration
 *  \iparam pCANCommunicator = pointer to communication class
 *  \iparam pParentNode = pointer to base module, where this module is assigned to
 */
/****************************************************************************/
CDigitalOutput::CDigitalOutput(const CANMessageConfiguration* p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                               CBaseModule* pParentNode) :
    CFunctionModule(CModuleConfig::CAN_OBJ_TYPE_DIGITAL_OUT_PORT, p_MessageConfiguration, pCANCommunicator, pParentNode),
    m_unCanIDDigOutputConfigOutput(0), m_unCanIDDigOutputConfigBlinking(0),
    m_unCanIDDigOutputStateSet(0), m_unCanIDDigOutputStateReq(0), m_unCanIDDigOutputState(0),
    m_unCanIDDigOutputLifetimeReq(0), m_unCanIDDigOutputLifetime(0)
{
    m_mainState = FM_MAIN_STATE_BOOTUP;
    m_SubStateConfig = FM_DOUTP_SUB_STATE_CONFIG_INIT;
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CDigitalOutput
 */
/****************************************************************************/
CDigitalOutput::~CDigitalOutput()
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
ReturnCode_t CDigitalOutput::Initialize()
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

    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
    {
        FILE_LOG_L(laINIT, llERROR) << " function module initialisation failed: " << (int) RetVal;
        m_mainState = FM_MAIN_STATE_ERROR;
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
ReturnCode_t CDigitalOutput::InitializeCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 bIfaceID;
    const quint8 ModuleID = MODULE_ID_DIGITAL_OUT;

    if (m_pCANObjectConfig == NULL) {
        return DCL_ERR_NULL_PTR_ACCESS;
    }
    bIfaceID = m_pCANObjectConfig->m_sChannel;

    RetVal = InitializeEventCANMessages(ModuleID);

    m_unCanIDDigOutputConfigOutput   = mp_MessageConfiguration->GetCANMessageID(ModuleID, "DigitalOutputConfigOutput", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDigOutputConfigBlinking = mp_MessageConfiguration->GetCANMessageID(ModuleID, "DigitalOutputConfigBlinking", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDigOutputStateSet       = mp_MessageConfiguration->GetCANMessageID(ModuleID, "DigitalOutputStateSet", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDigOutputStateReq       = mp_MessageConfiguration->GetCANMessageID(ModuleID, "DigitalOutputStateReq", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDigOutputState          = mp_MessageConfiguration->GetCANMessageID(ModuleID, "DigitalOutputState", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDigOutputLifetimeReq    = mp_MessageConfiguration->GetCANMessageID(ModuleID, "DigitalOutputLifetimeReq", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDigOutputLifetime       = mp_MessageConfiguration->GetCANMessageID(ModuleID, "DigitalOutputLifetime", bIfaceID, m_pParent->GetNodeID());

    FILE_LOG_L(laINIT, llDEBUG) << "  CAN-messages for fct-module:" << GetName().toStdString() << ",node id:" << std::hex << m_pParent->GetNodeID() << std::endl;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventInfo                 : 0x" << std::hex << m_unCanIDEventInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventWarning               : 0x" << std::hex << m_unCanIDEventWarning;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventError                 : 0x" << std::hex << m_unCanIDEventError;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventFatalError            : 0x" << std::hex << m_unCanIDEventFatalError;
    FILE_LOG_L(laINIT, llDEBUG) << "   DigitalOutputConfigOutput  : 0x" << std::hex << m_unCanIDDigOutputConfigOutput;
    FILE_LOG_L(laINIT, llDEBUG) << "   DigitalOutputConfigBlinking: 0x" << std::hex << m_unCanIDDigOutputConfigBlinking;
    FILE_LOG_L(laINIT, llDEBUG) << "   DigitalOutputStateSet      : 0x" << std::hex << m_unCanIDDigOutputStateSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   DigitalOutputStateReq      : 0x" << std::hex << m_unCanIDDigOutputStateReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   DigitalOutputState         : 0x" << std::hex << m_unCanIDDigOutputState;
    FILE_LOG_L(laINIT, llDEBUG) << "   DigitalOutputLifetimeReq   : 0x" << std::hex << m_unCanIDDigOutputLifetimeReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   DigitalOutputLifetime      : 0x" << std::hex << m_unCanIDDigOutputLifetime;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Register the receive CAN-messages to communication layer
 *
 *   Each receiveable CAN-message must be registered to the communication layer.
 *   This enables the communication layer to call the 'HandelCANMessage(..)' function of this
 *   instance after the receiption of the message.
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CDigitalOutput::RegisterCANMessages()
{
    ReturnCode_t RetVal;

    RetVal = RegisterEventCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDDigOutputState, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDDigOutputLifetime, this);
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
void CDigitalOutput::HandleTasks()
{
    QMutexLocker Locker(&m_Mutex);

    if(m_mainState == FM_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(m_mainState == FM_MAIN_STATE_CONFIRMED)
    {
        //fall through
        m_SubStateConfig = FM_DOUTP_SUB_STATE_CONFIG_START;
        m_mainState = FM_MAIN_STATE_CONFIG;
    }
    else if(m_mainState == FM_MAIN_STATE_CONFIG)
    {
        if(m_SubStateConfig == FM_DOUTP_SUB_STATE_CONFIG_START)
        {
            ReturnCode_t RetVal;

            RetVal = SendCANMessageConfiguration();
            if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_SubStateConfig = FM_DOUTP_SUB_STATE_CONFIG_FINISH;
                m_mainState = FM_MAIN_STATE_IDLE;
                m_TaskID = MODULE_TASKID_FREE;

                FILE_LOG_L(laCONFIG, llDEBUG) << " Module " << GetName().toStdString() << ": change to FM_MAIN_STATE_IDLE";
            }
            else
            {
                m_SubStateConfig = FM_DOUTP_SUB_STATE_CONFIG_ERROR;
                m_mainState = FM_MAIN_STATE_ERROR;

                FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCANMessageConfiguration returns" << (int) RetVal;
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Send the 'Configuration' CAN-Message
 *
 *   The function module's configuration parameter will be sent via CAN-Bus to the slave.
 *   The function module on slave side needs the following parameters
 *          DB[0]   = Operating mode
 *                    Bit 7: Enable(1) / disable(0)
 *                    Bit 6: set inactive at shutdown (1)
 *                    Bit 5: set inactive at standby (1)
 *                    Bit 4: set inactive at emergency stop (1)
 *          DB[1,2] = Polarity mask
 *          DB[3,4] = Inactive state output value
 *          DB[5,6] = level for operating counter
 *          DB[7]   = reserved
 *
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CDigitalOutput::SendCANMessageConfiguration()
{
    ReturnCode_t RetVal;
    CANFctModuleDigitOutput* pCANObjConfDigOutPort;
    pCANObjConfDigOutPort = (CANFctModuleDigitOutput*) m_pCANObjectConfig;

    if (pCANObjConfDigOutPort == NULL) {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    can_frame canmsg;

    FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration (I/O-dir): 0x" << std::hex << m_unCanIDDigOutputConfigOutput;

    canmsg.can_id = m_unCanIDDigOutputConfigOutput;
    canmsg.can_dlc = 7;

    canmsg.data[0] = 0;
    if(pCANObjConfDigOutPort->m_bEnabled)
    {
        canmsg.data[0] = 0x80;
    }
    if(pCANObjConfDigOutPort->m_bInaktivAtShutdown)
    {
        canmsg.data[0] |= 0x40;
    }
    if(pCANObjConfDigOutPort->m_bInaktivAtEmgyStop)
    {
        canmsg.data[0] |= 0x20;
    }

    SetCANMsgDataU16(&canmsg, pCANObjConfDigOutPort->m_sPolarity, 1);
    SetCANMsgDataU16(&canmsg, pCANObjConfDigOutPort->m_sOutvalInactiv, 3);
    SetCANMsgDataU16(&canmsg, pCANObjConfDigOutPort->m_sLivetimeLimit, 5);

    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
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
void CDigitalOutput::HandleIdleState()
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
 *      the corresponding CAN-message. After it the command's state will be set to MODULE_CMD_STATE_REQ_SEND
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
void CDigitalOutput::HandleCommandRequestTask()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_NOT_FOUND;

    QMutableListIterator<ModuleCommand_t *> Iterator(m_ModuleCommand);
    while(Iterator.hasNext())
    {
        ModuleCommand_t *p_ModuleCommand = Iterator.next();
        bool RemoveCommand = false;

        if(p_ModuleCommand->State == MODULE_CMD_STATE_REQ)
        {
            // forward the module command to the function module on slave side by sending
            // the corresponding CAN-message
            p_ModuleCommand->State = MODULE_CMD_STATE_REQ_SEND;
            p_ModuleCommand->ReqSendTime.Trigger();

            if(p_ModuleCommand->Type == FM_DO_CMD_TYPE_SET_OUTP)
            {
                //send the requested output value the slave by sending the CAN message
                FILE_LOG_L(laFCT, llINFO) << " CANDigitalOutput: output data set.";
                RetVal = SendCANMsgDigOutputDataSet(p_ModuleCommand->OutputValue,
                                                    p_ModuleCommand->Duration,
                                                    p_ModuleCommand->Delay);

                RemoveCommand = true;
                emit ReportOutputValueAckn(GetModuleHandle(), RetVal, p_ModuleCommand->OutputValue);
            }
            else  if(p_ModuleCommand->Type == FM_DO_CMD_TYPE_REQ_ACTVALUE)
            {
                //send the value request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDAnaInputState CAN-message
                FILE_LOG_L(laFCT, llINFO) << " CANDigitalOutput: output value req.: send request.";
                RetVal = SendCANMsgDigOutputDataReq();

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_DOUTP_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportActOutputValue(GetModuleHandle(), RetVal, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_DO_CMD_TYPE_REQ_LIFECYCLE)
            {
                //send the value request to the slave, this command will be acknowledged by the receiption
                // of the LivetimeData CAN-message
                FILE_LOG_L(laFCT, llINFO) << " CANDigitalOutput: life time data req.: send request.";
                RetVal = SendCANMsgLifeTimeDataReq();

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_DOUTP_TIMEOUT_LIFECYCLE_REQ;
                }
                else
                {
                    emit ReportLifeTimeData(GetModuleHandle(), RetVal, 0, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_DO_CMD_TYPE_REQ_DATA_RESET)
            {
                RetVal = SendCANMsgReqDataReset();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_DOUTP_TIMEOUT_LIFECYCLE_REQ;
                }
                else {
                    emit ReportDataResetAckn(GetModuleHandle(), RetVal);
                }
            }

            // Check for success
            if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
            {
                RemoveCommand = true;
            }
        }
        else if(p_ModuleCommand->State == MODULE_CMD_STATE_REQ_SEND)
        {
            // check active module commands for timeout
            if(p_ModuleCommand->ReqSendTime.Elapsed() > p_ModuleCommand->Timeout)
            {
                RemoveCommand = true;

                if(p_ModuleCommand->Type == FM_DO_CMD_TYPE_REQ_ACTVALUE)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANDigitalOutput:: '" << GetKey().toStdString() << "': output value req. timeout";
                    emit ReportActOutputValue(GetModuleHandle(), DCL_ERR_TIMEOUT, 0);
                }
                else if(p_ModuleCommand->Type == FM_DO_CMD_TYPE_REQ_LIFECYCLE)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANDigitalOutput:: '" << GetKey().toStdString() << "': life time data req. timeout";
                    emit ReportLifeTimeData(GetModuleHandle(), DCL_ERR_TIMEOUT, 0, 0);
                }
                else if(p_ModuleCommand->Type == FM_DO_CMD_TYPE_REQ_DATA_RESET)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANDigitalOutput:: '" << GetKey().toStdString() << "': ReqDataReset timeout error.";
                    emit ReportDataResetAckn(GetModuleHandle(), DCL_ERR_TIMEOUT);
                }
            }
        }

        if (RemoveCommand == true)
        {
            delete p_ModuleCommand;
            Iterator.remove();
        }
    }

    if(m_ModuleCommand.isEmpty())
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
void CDigitalOutput::HandleCanMessage(can_frame* pCANframe)
{
    QMutexLocker Locker(&m_Mutex);

    if((pCANframe->can_id == m_unCanIDEventInfo) ||
       (pCANframe->can_id == m_unCanIDEventWarning) ||
       (pCANframe->can_id == m_unCanIDEventError) ||
       (pCANframe->can_id == m_unCanIDEventFatalError))
    {
        HandleCANMsgEvent(pCANframe);
        if ((pCANframe->can_id == m_unCanIDEventError) || (pCANframe->can_id == m_unCanIDEventFatalError)) {
            emit ReportEvent(BuildEventCode(m_lastEventGroup, m_lastEventCode), m_lastEventData, m_lastEventTime);
        }
    }
    else if(pCANframe->can_id == m_unCanIDAcknDataReset)
    {
        ResetModuleCommand(FM_DO_CMD_TYPE_REQ_DATA_RESET);
        HandleCANMsgAcknDataReset(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDDigOutputState)
    {
        HandleCANMsgOutputState(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDDigOutputLifetime)
    {
        HandleCANMsgLifeTimeData(pCANframe);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'OutputState' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CDigitalOutput::HandleCANMsgOutputState(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_DO_CMD_TYPE_REQ_ACTVALUE);
    }

    if(pCANframe->can_dlc == 7)
    {
        quint16 OutputValue;

        OutputValue = (((quint16) pCANframe->data[1]) << 8);
        OutputValue |= ((quint16) pCANframe->data[2]);
        FILE_LOG_L(laFCT, llINFO) << "    CANDigitalOutput: HandleCANMsgDigOutputDataResp: " << (int) OutputValue;
        FILE_LOG_L(laFCT, llINFO) << "   CAN-ID: 0x" << std::hex << pCANframe->can_id;
        FILE_LOG_L(laFCT, llINFO) << "     data  0x" << std::hex << (int) pCANframe->data[0] << ", " << (int) pCANframe->data[1];

        emit ReportActOutputValue(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, OutputValue);
    }
    else
    {
        emit ReportActOutputValue(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'Lifetime' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CDigitalOutput::HandleCANMsgLifeTimeData(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_DO_CMD_TYPE_REQ_LIFECYCLE);
    }

    if(pCANframe->can_dlc == 8)
    {
        quint32 LifeTime;
        quint32 LifeCycles;

        LifeTime = GetCANMsgDataU32(pCANframe, 0);
        LifeCycles = GetCANMsgDataU32(pCANframe, 4);

        FILE_LOG_L(laFCT, llDEBUG) << " CANDigitalOutput: " << (int) LifeTime << ", " << (int) LifeCycles;

        emit ReportLifeTimeData(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, LifeTime, LifeCycles);
    }
    else
    {
        emit ReportLifeTimeData(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN-message 'OutputStateSet' to set output state
 *
 *  \iparam OutputValue = Target value of the digital output
 *  \iparam Duration = Duration the value is active in milliseconds
 *  \iparam Delay = Time after that the value gets active in milliseconds
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CDigitalOutput::SendCANMsgDigOutputDataSet(quint16 OutputValue, quint16 Duration, quint16 Delay)
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDDigOutputStateSet;

    canmsg.data[0] = 0;
    SetCANMsgDataU16(&canmsg, OutputValue, 1);
    SetCANMsgDataU16(&canmsg, Duration, 3);
    SetCANMsgDataU16(&canmsg, Delay, 5);

    canmsg.can_dlc = 7;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);
    FILE_LOG_L(laFCT, llINFO) << "    CANDigitalOutput::SendCANMsgDigOutputDataSet() 0x" << std::hex << m_unCanIDDigOutputStateSet <<
            (int) canmsg.data[1] << " - " << (int)canmsg.data[2];

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
ReturnCode_t CDigitalOutput::SendCANMsgDigOutputDataReq()
{
    ReturnCode_t retval;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDDigOutputStateReq;
    canmsg.data[0] = 0; //
    canmsg.data[1] = 0;
    canmsg.can_dlc = 0;
    retval = m_pCANCommunicator->SendCOB(canmsg);
    FILE_LOG_L(laFCT, llINFO) << "    CANDigitalOutput::SendCANMsgDigOutputDataReq() 0x" << std::hex << m_unCanIDDigOutputStateReq << ", " <<
            (int) canmsg.data[1] << " - " << (int)canmsg.data[2];

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
ReturnCode_t CDigitalOutput::SendCANMsgLifeTimeDataReq()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDDigOutputLifetimeReq;
    canmsg.data[0] = 0; //
    canmsg.data[1] = 0;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);
    FILE_LOG_L(laFCT, llINFO) << " CANDigitalOutput";

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Set the digital output value
 *
 *      The task will be acknowledged by sending the signal
 *      ReportActOutputValue.
 *
 *  \iparam OutputValue = Target value of the digital output
 *  \iparam Duration = Duration the value is active in milliseconds
 *  \iparam Delay = Time after that the value gets active in milliseconds
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CDigitalOutput::SetOutputValue(quint16 OutputValue, quint16 Duration, quint16 Delay)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_DO_CMD_TYPE_SET_OUTP);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->OutputValue = OutputValue;
        p_ModuleCommand->Duration = Duration;
        p_ModuleCommand->Delay = Delay;
        FILE_LOG_L(laDEV, llINFO) << " CANDigitalOutput, Value: " << (int) OutputValue;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANDigitalOutput invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Request the actual digital output
 *
 *            The task will be acknowledged by sending the signal ReportActOutputValue
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CDigitalOutput::ReqOutputValue()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_DO_CMD_TYPE_REQ_ACTVALUE);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANDigitalOutput invalid state: " << (int) m_TaskID;
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
ReturnCode_t CDigitalOutput::ReqLifeTimeData()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_DO_CMD_TYPE_REQ_LIFECYCLE);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANDigitalOutput invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request a data reset
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CDigitalOutput::ReqDataReset()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_DO_CMD_TYPE_REQ_DATA_RESET);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CDigitalOutput '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Adds a new command to the transmit queue
 *
 *  \iparam CommandType = Command type to set
 *
 *  \return Module command, if the command type can be placed, otherwise NULL
 */
/****************************************************************************/
CDigitalOutput::ModuleCommand_t *CDigitalOutput::SetModuleTask(CANDigitalOutputModuleCmdType_t CommandType)
{
    if((m_TaskID == MODULE_TASKID_FREE) || (m_TaskID == MODULE_TASKID_COMMAND_HDL)) {
        for(qint32 i = 0; i < m_ModuleCommand.size(); i++) {
            if (m_ModuleCommand[i]->Type == CommandType) {
                return NULL;
            }
        }

        ModuleCommand_t *p_ModuleCommand = new ModuleCommand_t;
        p_ModuleCommand->Type = CommandType;
        p_ModuleCommand->State = MODULE_CMD_STATE_REQ;
        m_ModuleCommand.append(p_ModuleCommand);

        m_TaskID = MODULE_TASKID_COMMAND_HDL;

        return p_ModuleCommand;
    }

    return NULL;
}

/****************************************************************************/
/*!
 *  \brief  Removes an existing command from the transmit queue
 *
 *  \iparam CommandType = Command of that type will be set to free
 */
/****************************************************************************/
void CDigitalOutput::ResetModuleCommand(CANDigitalOutputModuleCmdType_t CommandType)
{
    for(qint32 i = 0; i < m_ModuleCommand.size(); i++) {
        if (m_ModuleCommand[i]->Type == CommandType && m_ModuleCommand[i]->State == MODULE_CMD_STATE_REQ_SEND) {
            delete m_ModuleCommand.takeAt(i);
            break;
        }
    }

    if(m_ModuleCommand.isEmpty())
    {
        m_TaskID = MODULE_TASKID_FREE;
    }
}

} //namespace
