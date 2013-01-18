/****************************************************************************/
/*! \file DigitalInput.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CANDigitalInput
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

#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor for the CANDigitalInput
 *
 *  \iparam p_MessageConfiguration = Message configuration
 *  \iparam pCANCommunicator = pointer to communication class
 *  \iparam pParentNode = pointer to base module this module is assigned to
 */
/****************************************************************************/
CDigitalInput::CDigitalInput(const CANMessageConfiguration* p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                             CBaseModule* pParentNode) :
    CFunctionModule(CModuleConfig::CAN_OBJ_TYPE_DIGITAL_IN_PORT, p_MessageConfiguration, pCANCommunicator, pParentNode),
    m_unCanIDDigInputConfigInput(0), m_unCanIDDigInputConfigLimits(0), m_unCanIDDigInputStateReq(0), m_unCanIDDigInputState(0)
{
    m_mainState = FM_MAIN_STATE_BOOTUP;
    m_SubStateConfig = FM_DINP_SUB_STATE_CONFIG_INIT;
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CANDigitalInput
 */
/****************************************************************************/
CDigitalInput::~CDigitalInput()
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
ReturnCode_t CDigitalInput::Initialize()
{
    ReturnCode_t RetVal;

    FILE_LOG_L(laINIT, llDEBUG) << " CANDigitalInput::Initialize() " << GetName().toStdString();

    RetVal = InitializeCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = RegisterCANMessages();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_mainState = FM_MAIN_STATE_INIT;
            FILE_LOG_L(laINIT, llDEBUG) << " function module successfully initialized: " << GetName().toStdString();
            FILE_LOG_L(laINIT, llDEBUG) << "----------------------------------------- ";
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
ReturnCode_t CDigitalInput::InitializeCANMessages()
{
    ReturnCode_t RetVal;
    quint8 bIfaceID;
    const quint8 ModuleID = MODULE_ID_DIGITAL_IN;

    bIfaceID = m_pCANObjectConfig->m_sChannel;

    RetVal = InitializeEventCANMessages(ModuleID);

    m_unCanIDDigInputConfigInput  = mp_MessageConfiguration->GetCANMessageID(ModuleID, "DigitalInputConfigInput", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDigInputConfigLimits = mp_MessageConfiguration->GetCANMessageID(ModuleID, "DigitalInputConfigLimits", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDigInputStateReq     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "DigitalInputStateReq", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDigInputState        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "DigitalInputState", bIfaceID, m_pParent->GetNodeID());

    FILE_LOG_L(laINIT, llDEBUG) << "  CAN-messages for fct-module:" << GetName().toStdString() << ",node id:" << std::hex << m_pParent->GetNodeID();
    FILE_LOG_L(laINIT, llDEBUG) << "   EventInfo                 : 0x" << std::hex << m_unCanIDEventInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventWarning               : 0x" << std::hex << m_unCanIDEventWarning;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventError                 : 0x" << std::hex << m_unCanIDEventError;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventFatalError            : 0x" << std::hex << m_unCanIDEventFatalError;
    FILE_LOG_L(laINIT, llDEBUG) << "   DigitalInputConfigInput : 0x" << std::hex << m_unCanIDDigInputConfigInput;
    FILE_LOG_L(laINIT, llDEBUG) << "   DigitalInputConfigLimits: 0x" << std::hex << m_unCanIDDigInputConfigLimits;
    FILE_LOG_L(laINIT, llDEBUG) << "   DigitalInputStateReq    : 0x" << std::hex << m_unCanIDDigInputStateReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   DigitalInputState       : 0x" << std::hex << m_unCanIDDigInputState;

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
ReturnCode_t CDigitalInput::RegisterCANMessages()
{
    ReturnCode_t RetVal;

    RetVal = RegisterEventCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDDigInputState, this);
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Handles the function module's state machine
 *
 *   Depending on the active main state, the appropriate task function will be called.
 */
/****************************************************************************/
void CDigitalInput::HandleTasks()
{
    QMutexLocker Locker(&m_Mutex);

    if(m_mainState == FM_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(m_mainState == FM_MAIN_STATE_CONFIRMED)
    {
        //fall through
        m_SubStateConfig = FM_DINP_SUB_STATE_CONFIG_START;
        m_mainState = FM_MAIN_STATE_CONFIG;
    }
    else if(m_mainState == FM_MAIN_STATE_CONFIG)
    {
        if(m_SubStateConfig == FM_DINP_SUB_STATE_CONFIG_START)
        {
            ReturnCode_t RetVal;

            RetVal = SendCANMessageConfiguration();
            if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_SubStateConfig = FM_DINP_SUB_STATE_CONFIG_FINISH;
                m_mainState = FM_MAIN_STATE_IDLE;
                m_TaskID = MODULE_TASKID_FREE;
                FILE_LOG_L(laCONFIG, llDEBUG) << " Module " << GetName().toStdString() << ": change to FM_MAIN_STATE_IDLE";
            }
            else
            {
                m_SubStateConfig = FM_DINP_SUB_STATE_CONFIG_ERROR;
                m_mainState = FM_MAIN_STATE_ERROR;

                FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCANMessageConfiguration returns" << (int) RetVal;
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the function module's state machine in idle main state
 *
 *   Depending on the active task ID, the appropriate task function will be called.
 */
/****************************************************************************/
void CDigitalInput::HandleIdleState()
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
 *   The function is called from HandleIdleState() if m_taskID == FM_DINP_TASKID_COMMAND_HDL.
 *   The function loops thru the m_ModuleCommand array and controls the commands pending for execution there
 *
 *    - Module command with state MODULE_CMD_STATE_REQ will be forwarded to the function module on slave side by sending
 *      the corresponding CAN-message. After it the command's state will be set to MODULE_CMD_STATE_REQ_SEND
 *
 *    - Module command with state MODULE_CMD_STATE_REQ_SEND will be checked for timeout.
 *      Usually, a module command will be confirmed and closed by receiving the expected CAN-message (e.g. acknowledge, or data)
 *      The state will be exited when the expected CAN-message was received. this is done at the HandleCANMessage... - function
 *    - Module command with state MODULE_CMD_STATE_ERR will be set to MODULE_CMD_STATE_FREE, its error informations are forwarded
 *      to the device control layer by calling whose ThrowError(...)-function
 */
/****************************************************************************/
void CDigitalInput::HandleCommandRequestTask()
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

            if(p_ModuleCommand->Type == FM_DI_CMD_TYPE_ACTVALUE_REQ)
            {
                //send the value request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDAnaInputState CAN-message
                FILE_LOG_L(laFCT, llINFO) << " CANDigitalInput: input value req.: send request.";
                RetVal = SendCANMsgDigInputValueReq();

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_DINP_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportActInputValue(GetModuleHandle(), RetVal, 0);
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
            // check avtive motor commands for timeout
            if(p_ModuleCommand->ReqSendTime.Elapsed() > p_ModuleCommand->Timeout)
            {
                RemoveCommand = true;

                if(p_ModuleCommand->Type == FM_DI_CMD_TYPE_ACTVALUE_REQ)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANDigitalInput:: '" << GetKey().toStdString() << "': input value req. timeout";
                    emit ReportActInputValue(GetModuleHandle(), DCL_ERR_TIMEOUT, 0);
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
 */
/****************************************************************************/
void CDigitalInput::HandleCanMessage(can_frame* pCANframe)
{
    QMutexLocker Locker(&m_Mutex);

    if((pCANframe->can_id == m_unCanIDEventInfo) ||
       (pCANframe->can_id == m_unCanIDEventWarning) ||
       (pCANframe->can_id == m_unCanIDEventError) ||
       (pCANframe->can_id == m_unCanIDEventFatalError))
    {
        quint32 EventCode = HandleCANMsgEvent(pCANframe);
        if ((pCANframe->can_id == m_unCanIDEventError) || (pCANframe->can_id == m_unCanIDEventFatalError)) {
            emit ReportEvent(EventCode, m_lastEventData, m_lastEventTime);
        }
    }
    else if(pCANframe->can_id == m_unCanIDDigInputState)
    {
        HandleCANMsgDigInputState(pCANframe);
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the reception of the 'InputState' CAN message
 *
 *  \iparam   pCANframe = struct contains the data of the received CAN message
 */
/****************************************************************************/
void CDigitalInput::HandleCANMsgDigInputState(can_frame* pCANframe)
{
    FILE_LOG_L(laFCT, llINFO) << "  CANDigitalInput::HandleCANMsgDigInputState ";

    FILE_LOG_L(laFCT, llINFO) << "  CanID: 0x" << std::hex << pCANframe->can_id << ", " <<
                                         " "  << std::hex << (int)pCANframe->data[0] <<
                                         " "  << std::hex << (int)pCANframe->data[1] <<
                                         " "  << std::hex << (int)pCANframe->data[2] <<
                                         " "  << std::hex << (int)pCANframe->data[3] <<
                                         " "  << std::hex << (int)pCANframe->data[4] <<
                                         " "  << std::hex << (int)pCANframe->data[5] <<
                                         " "  << std::hex << (int)pCANframe->data[6] <<
                                         " "  << std::hex << (int)pCANframe->data[7] <<
                                         " dlc:"  << std::hex << (int)pCANframe->can_dlc;

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_DI_CMD_TYPE_ACTVALUE_REQ);
    }

    if(pCANframe->can_dlc == 2)
    {
        //CAN message without timestamp
        quint16 InputValue;

        InputValue = (((quint16) pCANframe->data[0]) << 8);
        InputValue = ((quint16) pCANframe->data[1]);

        FILE_LOG_L(laFCT, llINFO) << "  CANDigitalInput::HandleCANMsgDigInputState emit ReportActInputValue";

        emit ReportActInputValue(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, InputValue);
    }
    else if(pCANframe->can_dlc == 6)
    {
        //CAN message with timestamp
        /// \todo not implemented
    }
    else
    {
        emit ReportActInputValue(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief    Send the CAN-message to transmit the function module's configuration state
 *
 *   The function module's configuration parameter will be sent via CAN-Bus to the slave.
 *   The function module on slave side needs the following parameters
 *          DB[0]   = Operating mode
 *                    Bit 7: Enable(1) / disable(0)
 *                    Bit 6: Send timestamp when changed (1)
 *          DB[1,2] = Polarity mask
 *          DB[3,4] = Supervision, changes on input woll be send automatically, if set
 *          DB[5]   = Sample interval
 *          DB[6]   = Debounce interval
 *          DB[7]   = reserved
 *
 *  \return   ReturnCode_t
 */
/****************************************************************************/
ReturnCode_t CDigitalInput::SendCANMessageConfiguration()
{
    CANFctModuleDigitInput* pCANObjConfDigInpPort;

    pCANObjConfDigInpPort = (CANFctModuleDigitInput*) m_pCANObjectConfig;

    can_frame canmsg;
    ReturnCode_t RetVal;
    quint8 dataByte = 0;

    FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration (I/O-dir): 0x" << std::hex << m_unCanIDDigInputConfigInput;

    canmsg.can_id = m_unCanIDDigInputConfigInput;
    canmsg.can_dlc = 7;

    if(pCANObjConfDigInpPort->m_bEnabled)
    {
        dataByte = 0x80;
    }
    canmsg.data[0] = dataByte;

    SetCANMsgDataU16(&canmsg, pCANObjConfDigInpPort->m_sPolarity, 1);
    SetCANMsgDataU16(&canmsg, pCANObjConfDigInpPort->m_sSupervision, 3);
    canmsg.data[5] = pCANObjConfDigInpPort->m_bInterval;
    canmsg.data[6] = pCANObjConfDigInpPort->m_bDebounce;
    canmsg.data[7] = 0;

    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Send the CAN-message to request the function module's actual input state
 *
 *  \return   ReturnCode_t
 */
/****************************************************************************/
ReturnCode_t CDigitalInput::SendCANMsgDigInputValueReq()
{
    ReturnCode_t retval;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDDigInputStateReq;
    canmsg.data[0] = 0; //
    canmsg.data[1] = 0;
    canmsg.can_dlc = 0;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    return retval;
}

/****************************************************************************/
/*!
 *  \brief    Request the actual input value (bit-pattern)
 *
 *            The task will be acknowledged by sending the signal ReportActInputValue
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CDigitalInput::ReqActInputValue()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_DI_CMD_TYPE_ACTVALUE_REQ);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANDigitalInput invalid state: " << (int) m_TaskID;
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
CDigitalInput::ModuleCommand_t *CDigitalInput::SetModuleTask(CANDigitalInputModuleCmdType_t CommandType)
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
void CDigitalInput::ResetModuleCommand(CANDigitalInputModuleCmdType_t CommandType)
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
