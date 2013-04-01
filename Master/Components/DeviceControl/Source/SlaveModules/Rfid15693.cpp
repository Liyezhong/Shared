/****************************************************************************/
/*! \file Rfid15693.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CRfid15693
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

#include "DeviceControl/Include/SlaveModules/Rfid15693.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor for the CRfid15693
 *
 *  \iparam p_MessageConfiguration = Message configuration
 *  \iparam pCANCommunicator = pointer to communication class
 *  \iparam pParentNode = pointer to CANNode, where this module is assigned to
 */
/****************************************************************************/
CRfid15693::CRfid15693(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                       CBaseModule* pParentNode) :
    CFunctionModule(CModuleConfig::CAN_OBJ_TYPE_RFID15693, p_MessageConfiguration, pCANCommunicator, pParentNode),
    m_unCanIDSetConfig(0), m_unCanIDAcquireUid(0), m_unCanIDRespAcquireUid(0), m_unCanIDSetUid(0), m_unCanIDReqUid(0),
    m_unCanIDRespUid(0), m_unCanIDReadBlock(0), m_unCanIDRespReadBlock(0), m_unCanIDWriteBlock(0),
    m_unCanIDRespWriteBlock(0), m_unCanIDLockBlock(0), m_unCanIDRespLockBlock(0)
{
    m_mainState = FM_MAIN_STATE_BOOTUP;

    //module command array initialisation
    for(quint8 idx = 0; idx < MAX_RFID_CMD_IDX; idx++)
    {
        m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_FREE;
    }
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CRfid15693
 */
/****************************************************************************/
CRfid15693::~CRfid15693()
{
    /// \todo Auto-generated destructor stub
}


/****************************************************************************/
/*!
 *  \brief  Initialize this function module
 *
 *      The CAN-IDs are read from the CAN-Message configuration class, and the CAN-ID are composed
 *      Receiveable CAN-message are registered to the communication layer
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CRfid15693::Initialize()
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
 *  \brief  Initialize the module's CAN message IDs
 *
 *      The CAN-IDs are read from the CAN-Message configuration class.
 *      The CAN-ID is composed by the message key, the function
 *      module's channel and the node id of the CANNode this fct-module is assigned to.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CRfid15693::InitializeCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 bChannel;
    const quint8 ModuleID = MODULE_ID_RFID15693;

    if (m_pCANObjectConfig == NULL) {
        return DCL_ERR_NULL_PTR_ACCESS;
    }
    bChannel = m_pCANObjectConfig->m_sChannel;

    RetVal = InitializeEventCANMessages(ModuleID);

    m_unCanIDSetConfig = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID15693SetConfig", bChannel, m_pParent->GetNodeID());
    m_unCanIDAcquireUid = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID15693AcquireUid", bChannel, m_pParent->GetNodeID());
    m_unCanIDRespAcquireUid = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID15693RespAcquireUid", bChannel, m_pParent->GetNodeID());
    m_unCanIDSetUid = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID15693SetUid", bChannel, m_pParent->GetNodeID());
    m_unCanIDReqUid = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID15693ReqUid", bChannel, m_pParent->GetNodeID());
    m_unCanIDRespUid = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID15693RespUid", bChannel, m_pParent->GetNodeID());
    m_unCanIDReadBlock = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID15693ReadBlock", bChannel, m_pParent->GetNodeID());
    m_unCanIDRespReadBlock = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID15693RespReadBlock", bChannel, m_pParent->GetNodeID());
    m_unCanIDWriteBlock = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID15693WriteBlock", bChannel, m_pParent->GetNodeID());
    m_unCanIDRespWriteBlock = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID15693RespWriteBlock", bChannel, m_pParent->GetNodeID());
    m_unCanIDLockBlock = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID15693LockBlock", bChannel, m_pParent->GetNodeID());
    m_unCanIDRespLockBlock = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID15693RespLockBlock", bChannel, m_pParent->GetNodeID());

    FILE_LOG_L(laINIT, llDEBUG) << " CAN-messages for fct-module:" << GetName().toStdString() << ",node id:" << std::hex << m_pParent->GetNodeID();
    FILE_LOG_L(laINIT, llDEBUG) << "   EventInfo       : 0x" << std::hex << m_unCanIDEventInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventWarning    : 0x" << std::hex << m_unCanIDEventWarning;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventError      : 0x" << std::hex << m_unCanIDEventError;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventFatalError : 0x" << std::hex << m_unCanIDEventFatalError;
    FILE_LOG_L(laINIT, llDEBUG) << "   SetConfig       : 0x" << std::hex << m_unCanIDSetConfig;
    FILE_LOG_L(laINIT, llDEBUG) << "   AcquireUid      : 0x" << std::hex << m_unCanIDAcquireUid;
    FILE_LOG_L(laINIT, llDEBUG) << "   RespAcquireUid  : 0x" << std::hex << m_unCanIDRespAcquireUid;
    FILE_LOG_L(laINIT, llDEBUG) << "   SetUid          : 0x" << std::hex << m_unCanIDSetUid;
    FILE_LOG_L(laINIT, llDEBUG) << "   ReqUid          : 0x" << std::hex << m_unCanIDReqUid;
    FILE_LOG_L(laINIT, llDEBUG) << "   RespUid         : 0x" << std::hex << m_unCanIDRespUid;
    FILE_LOG_L(laINIT, llDEBUG) << "   ReadBlock       : 0x" << std::hex << m_unCanIDReadBlock;
    FILE_LOG_L(laINIT, llDEBUG) << "   RespReadBlock   : 0x" << std::hex << m_unCanIDRespReadBlock;
    FILE_LOG_L(laINIT, llDEBUG) << "   WriteBlock      : 0x" << std::hex << m_unCanIDWriteBlock;
    FILE_LOG_L(laINIT, llDEBUG) << "   RespWriteBlock  : 0x" << std::hex << m_unCanIDRespWriteBlock;
    FILE_LOG_L(laINIT, llDEBUG) << "   LockBlock       : 0x" << std::hex << m_unCanIDLockBlock;
    FILE_LOG_L(laINIT, llDEBUG) << "   RespLockBlock   : 0x" << std::hex << m_unCanIDRespLockBlock;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Register the receive CAN-messages to communication layer
 *
 *      Each receiveable CAN-message must be registered to the communication layer.
 *      This enables the communication layer to call the 'HandelCANMessage(..)' function of this
 *      instance after receiption of the message.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CRfid15693::RegisterCANMessages()
{
    ReturnCode_t RetVal;

    RetVal = RegisterEventCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDRespAcquireUid, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDRespUid, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDRespReadBlock, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDRespWriteBlock, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDRespLockBlock, this);
    }

    return RetVal;
}


/****************************************************************************/
/*!
 *  \brief  Handles the function module's state machine
 *
 *      Depending on the active main state, the appropriate task function will be called.
 */
/****************************************************************************/
void CRfid15693::HandleTasks()
{
    QMutexLocker Locker(&m_Mutex);

    if(m_mainState == FM_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(m_mainState == FM_MAIN_STATE_CONFIRMED)
    {
        //fall through
        m_mainState = FM_MAIN_STATE_CONFIG;
    }
    else if(FM_MAIN_STATE_CONFIG == m_mainState)
    {
        CANFctModuleRFID15693* pCANObjConfRFID;
        pCANObjConfRFID = (CANFctModuleRFID15693*) m_pCANObjectConfig;

        if(pCANObjConfRFID)
        {
            m_mainState = FM_MAIN_STATE_IDLE;
            m_TaskID = MODULE_TASKID_FREE;
        }
        else
        {
            m_mainState = FM_MAIN_STATE_ERROR;
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the function module's state machine in idle main state
 *
 *      Depending on the active task ID, the appropriate task function will be called.
 */
/****************************************************************************/
void CRfid15693::HandleIdleState()
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        HandleCommandRequestTask();
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the task to execute the module commands
 *
 *      The function is called from HandleIdleState() if m_taskID == FM_RFID_TASKID_COMMAND_HDL.
 *      The function loops thru the m_ModuleCommand array and controls the commands pending for execution there
 *
 *      - Module command with state MODULE_CMD_STATE_REQ will be forwarded to the function module on slave side by sending
 *        the corresponding CAN-message. After it the command's state will be set to MODULE_CMD_STATE_REQ_SEND
 *
 *      - Module command with state MODULE_CMD_STATE_REQ_SEND will be checked for timeout.
 *        Usually, a module command will be confirmed and closed by receiving the expected CAN-message (e.g. acknowledge, or data)
 *        The state will be exited when the expected CAN-message was received. this is done at the HandleCANMessage... - function
 *
 *      - Module command with state MODULE_CMD_STATE_ERR will be set to MODULE_CMD_STATE_FREE, its error informations are forwarded
 *        to the device control layer by calling whose ThrowError(...)-function
 */
/****************************************************************************/
void CRfid15693::HandleCommandRequestTask()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_NOT_FOUND;
    bool ActiveCommandFound = false;

    for(quint8 idx = 0; idx < MAX_RFID_CMD_IDX; idx++)
    {
        if(m_ModuleCommand[idx].m_State == MODULE_CMD_STATE_REQ)
        {
            ActiveCommandFound = true;
            if(m_ModuleCommand[idx].m_Type == FM_RFID_CMD_TYPE_SET_CONFIG)
            {
                // This command will be acknowledged on receiption
                FILE_LOG_L(laFCT, llINFO) << " CANRFID15693: Sending 'set config' message";
                RetVal = SendCANMsgSetConfig(m_ModuleCommand[idx].m_Enabled);

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_RFID_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportSetConfig(GetModuleHandle(), RetVal);
                }
            }
            else  if(m_ModuleCommand[idx].m_Type == FM_RFID_CMD_TYPE_ACQUIRE_UID)
            {
                // This command will be acknowledged on receiption
                FILE_LOG_L(laFCT, llINFO) << " CANRFID15693: Sending 'acquire UID' message";
                RetVal = SendCANMsgAcquireUid();

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_RFID_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportAcquireUid(GetModuleHandle(), RetVal, 0);
                }
            }
            else if(m_ModuleCommand[idx].m_Type == FM_RFID_CMD_TYPE_READ_BOCK)
            {
                // This command will be acknowledged on receiption
                FILE_LOG_L(laFCT, llINFO) << " CANRFID15693: Sending 'read block' message";
                RetVal = SendCANMsgReadBlock(m_ModuleCommand[idx].m_Address);

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_RFID_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportReadBlock(GetModuleHandle(), RetVal, 0);
                }
            }
            else if(m_ModuleCommand[idx].m_Type == FM_RFID_CMD_TYPE_WRITE_BOCK)
            {
                // This command will be acknowledged on receiption
                FILE_LOG_L(laFCT, llINFO) << " CANRFID15693: Sending 'write block' message";
                RetVal = SendCANMsgWriteBlock(m_ModuleCommand[idx].m_Address, m_ModuleCommand[idx].m_Data);

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_RFID_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportWriteBlock(GetModuleHandle(), RetVal);
                }
            }
            else if(m_ModuleCommand[idx].m_Type == FM_RFID_CMD_TYPE_LOCK_BOCK)
            {
                // This command will be acknowledged on receiption
                FILE_LOG_L(laFCT, llINFO) << " CANRFID15693: Sending 'lock block' message";
                RetVal = SendCANMsgLockBlock(m_ModuleCommand[idx].m_Address);

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_RFID_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportLockBlock(GetModuleHandle(), RetVal);
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
            // check avtive motor commands for timeout
            ActiveCommandFound = true;
            if(m_ModuleCommand[idx].m_ReqSendTime.Elapsed() > m_ModuleCommand[idx].m_Timeout)
            {
                m_lastErrorHdlInfo = DCL_ERR_TIMEOUT;
                m_ModuleCommand[idx].m_State = MODULE_CMD_STATE_FREE;

                if(m_ModuleCommand[idx].m_Type == FM_RFID_CMD_TYPE_SET_CONFIG)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID15693:: '" << GetKey().toStdString() << "': set config timeout";
                    emit ReportSetConfig(GetModuleHandle(), m_lastErrorHdlInfo);
                }
                else if(m_ModuleCommand[idx].m_Type == FM_RFID_CMD_TYPE_ACQUIRE_UID)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID15693:: '" << GetKey().toStdString() << "': acquire UID timeout";
                    emit ReportAcquireUid(GetModuleHandle(), m_lastErrorHdlInfo, 0);
                }
                else if(m_ModuleCommand[idx].m_Type == FM_RFID_CMD_TYPE_READ_BOCK)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID15693:: '" << GetKey().toStdString() << "': read block timeout";
                    emit ReportReadBlock(GetModuleHandle(), m_lastErrorHdlInfo, 0);
                }
                else if(m_ModuleCommand[idx].m_Type == FM_RFID_CMD_TYPE_WRITE_BOCK)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID15693:: '" << GetKey().toStdString() << "': write block timeout";
                    emit ReportWriteBlock(GetModuleHandle(), m_lastErrorHdlInfo);
                }
                else if(m_ModuleCommand[idx].m_Type == FM_RFID_CMD_TYPE_LOCK_BOCK)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID15693:: '" << GetKey().toStdString() << "': lock block timeout";
                    emit ReportLockBlock(GetModuleHandle(), m_lastErrorHdlInfo);
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
void CRfid15693::HandleCanMessage(can_frame* pCANframe)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
    CANRFIDModuleCmdType_t ModuleCommandType = FM_RFID_CMD_TYPE_UNDEF;

    FILE_LOG_L(laFCT, llDEBUG) << "  CANRFID15693::HandleCanMessage 0x" << std::hex << pCANframe->can_id;

    if((pCANframe->can_id == m_unCanIDEventInfo) ||
       (pCANframe->can_id == m_unCanIDEventWarning) ||
       (pCANframe->can_id == m_unCanIDEventError) ||
       (pCANframe->can_id == m_unCanIDEventFatalError))
    {
        HandleCANMsgError(pCANframe);

        if ((pCANframe->can_id == m_unCanIDEventError) ||  (pCANframe->can_id == m_unCanIDEventFatalError)) {
            hdlInfo = DCL_ERR_EXTERNAL_ERROR;
        }

        if(m_lastErrorGroup == MODULE_ID_RFID15693)
        {
            for(quint8 i = 0; i < MAX_RFID_CMD_IDX; i++)
            {
                if(m_ModuleCommand[i].m_State == MODULE_CMD_STATE_REQ_SEND)
                {
                    ModuleCommandType = m_ModuleCommand[i].m_Type;
                }
            }
        }
        else if ((pCANframe->can_id == m_unCanIDEventError) || (pCANframe->can_id == m_unCanIDEventFatalError)) {
            emit ReportError(GetModuleHandle(), m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
    }

    if(m_unCanIDEventInfo == pCANframe->can_id || FM_RFID_CMD_TYPE_SET_CONFIG == ModuleCommandType)
    {
        HandleCANMsgRespSetConfig(pCANframe, hdlInfo);
    }
    if(m_unCanIDRespAcquireUid == pCANframe->can_id || FM_RFID_CMD_TYPE_ACQUIRE_UID == ModuleCommandType)
    {
        HandleCANMsgRespAcquireUid(pCANframe, hdlInfo);
    }
    else if(m_unCanIDRespReadBlock == pCANframe->can_id || FM_RFID_CMD_TYPE_READ_BOCK == ModuleCommandType)
    {
        HandleCANMsgReadBlock(pCANframe, hdlInfo);
    }
    else if(m_unCanIDRespWriteBlock == pCANframe->can_id || FM_RFID_CMD_TYPE_WRITE_BOCK == ModuleCommandType)
    {
        HandleCANMsgWriteBlock(pCANframe, hdlInfo);
    }
    else if(m_unCanIDRespLockBlock == pCANframe->can_id || FM_RFID_CMD_TYPE_LOCK_BOCK == ModuleCommandType)
    {
        HandleCANMsgLockBlock(pCANframe, hdlInfo);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'SetConfig' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *  \iparam hdlInfo = Indicates if the message was received successfully
 */
/****************************************************************************/
void CRfid15693::HandleCANMsgRespSetConfig(can_frame* pCANframe, ReturnCode_t hdlInfo)
{
    FILE_LOG_L(laFCT, llINFO) << " CRfid15693";

    if(pCANframe->can_dlc != 6)
    {
        hdlInfo = DCL_ERR_CANMSG_INVALID;
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_RFID_CMD_TYPE_SET_CONFIG);
    }
    emit ReportSetConfig(GetModuleHandle(), hdlInfo);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'AcquireUid' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *  \iparam hdlInfo = Indicates if the message was received successfully
 */
/****************************************************************************/
void CRfid15693::HandleCANMsgRespAcquireUid(can_frame* pCANframe, ReturnCode_t hdlInfo)
{
    quint64 Uid = 0;

    FILE_LOG_L(laFCT, llINFO) << " CRfid15693";

    if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        if(pCANframe->can_dlc == 8)
        {
            Uid = GetCANMsgDataU64(pCANframe);
        }
        else
        {
            hdlInfo = DCL_ERR_CANMSG_INVALID;
        }
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_RFID_CMD_TYPE_ACQUIRE_UID);
    }
    emit ReportAcquireUid(GetModuleHandle(), hdlInfo, Uid);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'ReadBlock' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *  \iparam hdlInfo = Indicates if the message was received successfully
 */
/****************************************************************************/
void CRfid15693::HandleCANMsgReadBlock(can_frame* pCANframe, ReturnCode_t hdlInfo)
{
    quint32 Data = 0;

    FILE_LOG_L(laFCT, llINFO) << " CRfid15693";

    if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        if(pCANframe->can_dlc == 5)
        {
            Data = GetCANMsgDataU32(pCANframe, 1);
        }
        else
        {
            hdlInfo = DCL_ERR_CANMSG_INVALID;
        }
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_RFID_CMD_TYPE_READ_BOCK);
    }
    emit ReportReadBlock(GetModuleHandle(), hdlInfo, Data);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'WriteBlock' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *  \iparam hdlInfo = Indicates if the message was received successfully
 */
/****************************************************************************/
void CRfid15693::HandleCANMsgWriteBlock(can_frame* pCANframe, ReturnCode_t hdlInfo)
{
    FILE_LOG_L(laFCT, llINFO) << " CRfid15693";

    if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        if(pCANframe->can_dlc != 1)
        {
            hdlInfo = DCL_ERR_CANMSG_INVALID;
        }
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_RFID_CMD_TYPE_WRITE_BOCK);
    }
    emit ReportWriteBlock(GetModuleHandle(), hdlInfo);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'LockBlock' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *  \iparam hdlInfo = Indicates if the message was received successfully
 */
/****************************************************************************/
void CRfid15693::HandleCANMsgLockBlock(can_frame* pCANframe, ReturnCode_t hdlInfo)
{
    FILE_LOG_L(laFCT, llINFO) << " CRfid15693";

    if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        if(pCANframe->can_dlc != 1)
        {
            hdlInfo = DCL_ERR_CANMSG_INVALID;
        }
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_RFID_CMD_TYPE_LOCK_BOCK);
    }
    emit ReportLockBlock(GetModuleHandle(), hdlInfo);
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN-message 'SetConfiguration'
 *
 *      The function module's configuration parameter will be sent via
 *      CAN-Bus to the slave. This message contains the module enable/disable
 *      parameter.
 *
 *  \iparam Enabled = Enable/disable module
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CRfid15693::SendCANMsgSetConfig(bool Enabled)
{
    can_frame canmsg;

    canmsg.can_id = m_unCanIDSetConfig;
    canmsg.data[0] = Enabled;
    canmsg.can_dlc = 1;

    return m_pCANCommunicator->SendCOB(canmsg);
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN-message 'AcquireUid'
 *
 *      The acquire UID message will be sent via CAN-Bus to the slave.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CRfid15693::SendCANMsgAcquireUid()
{
    can_frame CanMsg;

    CanMsg.can_id = m_unCanIDAcquireUid;
    CanMsg.can_dlc = 0;

    return m_pCANCommunicator->SendCOB(CanMsg);
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN-message 'ReadBlock'
 *
 *      A read block request will be send via CAN-Bus to the slave.
 *
 *  \iparam Address = Block address
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CRfid15693::SendCANMsgReadBlock(quint8 Address)
{
    can_frame canmsg;

    canmsg.can_id = m_unCanIDReadBlock;
    canmsg.data[0] = Address;
    canmsg.can_dlc = 1;

    return m_pCANCommunicator->SendCOB(canmsg);
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN-message 'WriteBlock'
 *
 *      A write block request will be send via CAN-Bus to the slave.
 *
 *  \iparam Address = Block address
 *  \iparam Data = Data to be written
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CRfid15693::SendCANMsgWriteBlock(quint8 Address, quint32 Data)
{
    can_frame canmsg;

    canmsg.can_id = m_unCanIDWriteBlock;
    canmsg.data[0] = Address;
    SetCANMsgDataU32(&canmsg, Data, 1);
    canmsg.can_dlc = 5;

    return m_pCANCommunicator->SendCOB(canmsg);
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN-message 'LockBlock'
 *
 *      A lock block request will be send via CAN-Bus to the slave.
 *
 *  \iparam Address = Block address
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CRfid15693::SendCANMsgLockBlock(quint8 Address)
{
    can_frame canmsg;

    canmsg.can_id = m_unCanIDLockBlock;
    canmsg.data[0] = Address;
    canmsg.can_dlc = 1;

    return m_pCANCommunicator->SendCOB(canmsg);
}

/****************************************************************************/
/*!
 *  \brief  Set the function module's configuration state (enabled/disabled)
 *
 *      The task will be acknowledged by sending the signal ReportSetConfig
 *
 *  \iparam Enabled = Flag for module on/off
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRfid15693::SetConfig(bool Enabled)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_RFID_CMD_TYPE_SET_CONFIG, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].m_Enabled = Enabled;
        FILE_LOG_L(laDEV, llINFO) << " CANRFID15693, Enabled: " << (int) Enabled;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANRFID15693 invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Acquires the UID of an RFID tag
 *
 *      The task will be acknowledged by sending the signal ReportAcquireUid
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRfid15693::AcquireUid()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(SetModuleTask(FM_RFID_CMD_TYPE_ACQUIRE_UID))
    {
        FILE_LOG_L(laDEV, llINFO) << " CANRFID15693";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANRFID15693 invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Reads a data block from the RFID tag
 *
 *      The task will be acknowledged by sending the signal ReportReadBlock
 *
 *  \iparam Address = Block address
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRfid15693::ReadBlock(quint8 Address)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_RFID_CMD_TYPE_READ_BOCK, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].m_Address = Address;
        FILE_LOG_L(laDEV, llINFO) << " CANRFID15693, Address: " << (int) Address;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANRFID15693 invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Writes a data block to the RFID tag
 *
 *      The task will be acknowledged by sending the signal ReportWriteBlock
 *
 *  \iparam Address = Block address
 *  \iparam Data = Data to be written
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRfid15693::WriteBlock(quint8 Address, quint32 Data)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_RFID_CMD_TYPE_WRITE_BOCK, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].m_Address = Address;
        m_ModuleCommand[CmdIndex].m_Data = Data;
        FILE_LOG_L(laDEV, llINFO) << " CANRFID15693, Address: " << (int) Address << ", Data: " << Data;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANRFID15693 invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Locks a data block on the RFID tag
 *
 *      The task will be acknowledged by sending the signal ReportLockBlock
 *
 *  \iparam Address = Block address
 *  \iparam Data = Data to be written
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRfid15693::LockBlock(quint8 Address)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_RFID_CMD_TYPE_LOCK_BOCK, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].m_Address = Address;
        FILE_LOG_L(laDEV, llINFO) << " CANRFID15693, Address: " << (int) Address;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANRFID15693 invalid state: " << (int) m_TaskID;
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
bool CRfid15693::SetModuleTask(CANRFIDModuleCmdType_t CommandType, quint8* pCmdIndex)
{
    bool CommandAdded = false;

    if((m_TaskID == MODULE_TASKID_FREE) || (m_TaskID == MODULE_TASKID_COMMAND_HDL))
    {
        for(quint8 idx = 0; idx < MAX_RFID_CMD_IDX; idx++)
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

                FILE_LOG_L(laFCT, llINFO) << " CANRFID15693:  task " << (int) idx << " request.";
                break;
            }
        }
    }

    return CommandAdded;
}

/****************************************************************************/
/**
 *  \brief  Set the ModuleCommands with the specified command type to 'FREE'
 *
 *  \iparam ModuleCommandType = ModuleCommands having this command type will be set to free
 */
/****************************************************************************/
void CRfid15693::ResetModuleCommand(CANRFIDModuleCmdType_t ModuleCommandType)
{
    bool ActiveCommandFound = false;

    for(quint8 idx = 0; idx < MAX_RFID_CMD_IDX; idx++)
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

} //namespace
