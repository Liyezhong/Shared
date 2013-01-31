/****************************************************************************/
/*! \file Rfid11785.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CRfid11785
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

#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor for the CRfid11785
 *
 *  \iparam p_MessageConfiguration = Message configuration
 *  \iparam pCANCommunicator = pointer to communication class
 *  \iparam pParentNode = Base module this module is assigned to
 */
/****************************************************************************/
CRfid11785::CRfid11785(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                       CBaseModule* pParentNode) :
    CFunctionModule(CModuleConfig::CAN_OBJ_TYPE_RFID11785, p_MessageConfiguration, pCANCommunicator, pParentNode),
    m_unCanIDConfig(0), m_unCanIDConfigAckn(0),
    m_unCanIDLogin(0), m_unCanIDLoginAckn(0),
    m_unCanIDWriteUserData(0), m_unCanIDWritePassword(0), m_unCanIDWriteConfig(0),
    m_unCanIDUserDataReq(0), m_unCanIDUserData(0), m_unCanIDUIDReq(0), m_unCanIDUID(0)
{
    m_mainState = FM_MAIN_STATE_BOOTUP;

    m_ConfigurationState.Enabled = 0;
    // For now the data rate will always be RF/32
    m_ConfigurationState.DataRate = 1;
    m_ConfigurationState.Antenna = 0;
}

/****************************************************************************/
/*!
 *  \brief  Initialize this function module
 *
 *      The CAN-IDs are read from the CAN-Message configuration class, and
 *      the CAN-ID are composed. Receiveable CAN-message are registered to
 *      the communication layer.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CRfid11785::Initialize()
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
 *      The CAN-IDs are read from the CAN-Message configuration class. The
 *      CAN-ID is composed by the message key, the function module's channel
 *      and the node id of the CANNode this fct-module is assigned to.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CRfid11785::InitializeCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 bChannel;
    const quint8 ModuleID = MODULE_ID_RFID11785;

    if (m_pCANObjectConfig == NULL) {
        return DCL_ERR_NULL_PTR_ACCESS;
    }
    bChannel = m_pCANObjectConfig->m_sChannel;

    RetVal = InitializeEventCANMessages(ModuleID);

    m_unCanIDConfig            = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785Config", bChannel, m_pParent->GetNodeID());
    m_unCanIDConfigAckn        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785ConfigAckn", bChannel, m_pParent->GetNodeID());
    m_unCanIDLogin             = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785Login", bChannel, m_pParent->GetNodeID());
    m_unCanIDLoginAckn         = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785LoginAckn", bChannel, m_pParent->GetNodeID());
    m_unCanIDWriteUserData     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785WriteUserData", bChannel, m_pParent->GetNodeID());
    m_unCanIDWriteUserDataAckn = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785WriteUserDataAckn", bChannel, m_pParent->GetNodeID());
    m_unCanIDWritePassword     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785WritePassword", bChannel, m_pParent->GetNodeID());
    m_unCanIDWritePasswordAckn = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785WritePasswordAckn", bChannel, m_pParent->GetNodeID());
    m_unCanIDWriteConfig       = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785WriteConfig", bChannel, m_pParent->GetNodeID());
    m_unCanIDWriteConfigAckn   = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785WriteConfigAckn", bChannel, m_pParent->GetNodeID());
    m_unCanIDUserDataReq       = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785UserDataReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDUserData          = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785UserData", bChannel, m_pParent->GetNodeID());
    m_unCanIDUIDReq            = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785UIDNumberReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDUID               = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RFID11785UIDNumber", bChannel, m_pParent->GetNodeID());

    FILE_LOG_L(laINIT, llDEBUG) << " CAN-messages for fct-module:" << GetName().toStdString() << ",node id:" << std::hex << m_pParent->GetNodeID();
    FILE_LOG_L(laINIT, llDEBUG) << "   EventInfo         : 0x" << std::hex << m_unCanIDEventInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventWarning      : 0x" << std::hex << m_unCanIDEventWarning;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventError        : 0x" << std::hex << m_unCanIDEventError;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventFatalError   : 0x" << std::hex << m_unCanIDEventFatalError;
    FILE_LOG_L(laINIT, llDEBUG) << "   Config            : 0x" << std::hex << m_unCanIDConfig;
    FILE_LOG_L(laINIT, llDEBUG) << "   ConfigAckn        : 0x" << std::hex << m_unCanIDConfigAckn;
    FILE_LOG_L(laINIT, llDEBUG) << "   Login             : 0x" << std::hex << m_unCanIDLogin;
    FILE_LOG_L(laINIT, llDEBUG) << "   LoginAckn         : 0x" << std::hex << m_unCanIDLoginAckn;
    FILE_LOG_L(laINIT, llDEBUG) << "   WriteUserData     : 0x" << std::hex << m_unCanIDWriteUserData;
    FILE_LOG_L(laINIT, llDEBUG) << "   WriteUserDataAckn : 0x" << std::hex << m_unCanIDWriteUserDataAckn;
    FILE_LOG_L(laINIT, llDEBUG) << "   WritePassword     : 0x" << std::hex << m_unCanIDWritePassword;
    FILE_LOG_L(laINIT, llDEBUG) << "   WritePasswordAckn : 0x" << std::hex << m_unCanIDWritePasswordAckn;
    FILE_LOG_L(laINIT, llDEBUG) << "   WriteConfig       : 0x" << std::hex << m_unCanIDWriteConfig;
    FILE_LOG_L(laINIT, llDEBUG) << "   WriteConfigAckn   : 0x" << std::hex << m_unCanIDWriteConfigAckn;
    FILE_LOG_L(laINIT, llDEBUG) << "   UserDataReq       : 0x" << std::hex << m_unCanIDUserDataReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   UserData          : 0x" << std::hex << m_unCanIDUserData;
    FILE_LOG_L(laINIT, llDEBUG) << "   UIDNumberReq      : 0x" << std::hex << m_unCanIDUIDReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   UIDNumber         : 0x" << std::hex << m_unCanIDUID;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Register the receive CAN-messages to communication layer
 *
 *      Each receiveable CAN-message must be registered to the communication
 *      layer. This enables the communication layer to call the
 *      'HandelCANMessage(..)' function of this instance after receiption of
 *      the message.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CRfid11785::RegisterCANMessages()
{
    ReturnCode_t RetVal;

    RetVal = RegisterEventCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDConfigAckn, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDLoginAckn, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDUserData, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDUID, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDWriteUserDataAckn, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDWritePasswordAckn, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDWriteConfigAckn, this);
    }

    return RetVal;
}


/****************************************************************************/
/*!
 *  \brief  Handles the function module's state machine
 *
 *      Depending on the active main state, the appropriate task function
 *      will be called.
 */
/****************************************************************************/
void CRfid11785::HandleTasks()
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
        CANFctModuleRFID11785* pCANObjConfRFID;
        pCANObjConfRFID = (CANFctModuleRFID11785*) m_pCANObjectConfig;

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
 *      Depending on the active task ID, the appropriate task function will
 *      be called.
 */
/****************************************************************************/
void CRfid11785::HandleIdleState()
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
 *   The function is called from HandleIdleState() if m_taskID == FM_RFID_TASKID_COMMAND_HDL.
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
void CRfid11785::HandleCommandRequestTask()
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

            if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_SET_CONFIG)
            {
                //send the configuration data to the slave, this command will be acknowledged by reception
                FILE_LOG_L(laFCT, llINFO) << " CANRFID11785: 'Set configuration' send";
                RetVal = SendCANMessageConfiguration();

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_RFID_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportStateAckn(GetModuleHandle(), RetVal);
                }
            }
            else if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_LOGIN)
            {
                //send the value request to the slave, this command will be acknowledged by reception
                FILE_LOG_L(laFCT, llINFO) << " CANRFID11785: Login send";
                RetVal = SendCANMsgLogin(p_ModuleCommand->Password);

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_RFID_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportLoginAckn(GetModuleHandle(), RetVal);
                }
            }
            else if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_USER_DATA_REQ)
            {
                //send the value request to the slave, this command will be acknowledged by reception
                // of the m_unCanIDUserData CAN-message
                FILE_LOG_L(laFCT, llINFO) << " CANRFID11785: Read user data: send request.";
                RetVal = SendCANMsgReadUserData(p_ModuleCommand->UserDataAddress);

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_RFID_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportUserData(GetModuleHandle(), RetVal, 0, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_USER_DATA_WRITE)
            {
                //send the value request to the slave, this command will be acknowledged by reception
                // of the m_unCanIDWriteData CAN-message
                FILE_LOG_L(laFCT, llINFO) << " CANRFID11785: Write user data: send request.";
                RetVal = SendCANMsgWriteUserData(p_ModuleCommand->UserDataAddress, p_ModuleCommand->UserData);

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_RFID_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportWriteData(GetModuleHandle(), RetVal, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_UID_REQ)
            {
                //send the value request to the slave, this command will be acknowledged by the reception
                // of the m_unCanIDUID CAN-message
                FILE_LOG_L(laFCT, llINFO) << " CANRFID11785: Read UID: send request.";
                RetVal = SendCANMsgReadUID();

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_RFID_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportUID(GetModuleHandle(), RetVal, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_PASSWORD_WRITE)
            {
                //send the value request to the slave, this command will be acknowledged by reception
                // of the m_unCanIDWritePassword CAN-message
                FILE_LOG_L(laFCT, llINFO) << " CANRFID11785: Write password: send request.";
                RetVal = SendCANMsgWritePassword(p_ModuleCommand->Password);

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_RFID_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportWritePassword(GetModuleHandle(), RetVal);
                }
            }
            else if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_CONGIG_WRITE)
            {
                //send the value request to the slave, this command will be acknowledged by receiption
                // of the m_unCanIDWriteConfig CAN-message
                FILE_LOG_L(laFCT, llINFO) << " CANRFID11785: Write password: send request.";
                RetVal = SendCANMsgWriteConfig(p_ModuleCommand->ReadLogin, p_ModuleCommand->WriteLogin,
                                               p_ModuleCommand->ReaderTalkFirst);

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_RFID_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportWriteConfiguration(GetModuleHandle(), RetVal);
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

                if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_SET_CONFIG)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID11785:: '" << GetKey().toStdString() << "': set config req. timeout";
                    emit ReportStateAckn(GetModuleHandle(), DCL_ERR_TIMEOUT);
                }
                else if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_LOGIN)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID11785:: '" << GetKey().toStdString() << "': login req. timeout";
                    emit ReportLoginAckn(GetModuleHandle(), DCL_ERR_TIMEOUT);
                }
                else if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_USER_DATA_REQ)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID11785:: '" << GetKey().toStdString() << "': user data req. timeout";
                    emit ReportUserData(GetModuleHandle(), DCL_ERR_TIMEOUT, 0, 0);
                }
                else if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_USER_DATA_WRITE)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID11785:: '" << GetKey().toStdString() << "': write data req. timeout";
                    emit ReportWriteData(GetModuleHandle(), DCL_ERR_TIMEOUT, 0);
                }
                else if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_UID_REQ)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID11785:: '" << GetKey().toStdString() << "': UID req. timeout";
                    emit ReportUID(GetModuleHandle(), DCL_ERR_TIMEOUT, 0);
                }
                else if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_PASSWORD_WRITE)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID11785:: '" << GetKey().toStdString() << "': write password timeout";
                    emit ReportWritePassword(GetModuleHandle(), DCL_ERR_TIMEOUT);
                }
                else if(p_ModuleCommand->Type == FM_RFID_CMD_TYPE_CONGIG_WRITE)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID11785:: '" << GetKey().toStdString() << "': write config timeout";
                    emit ReportWriteConfiguration(GetModuleHandle(), DCL_ERR_TIMEOUT);
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
void CRfid11785::HandleCanMessage(can_frame* pCANframe)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
    CANRFIDModuleCmdType_t ModuleCommandType = FM_RFID_CMD_TYPE_UNDEF;

    FILE_LOG_L(laFCT, llDEBUG) << "  CANRFID11785::HandleCanMessage 0x" << std::hex << pCANframe->can_id;

    if((pCANframe->can_id == m_unCanIDEventInfo) ||
       (pCANframe->can_id == m_unCanIDEventWarning) ||
       (pCANframe->can_id == m_unCanIDEventError) ||
       (pCANframe->can_id == m_unCanIDEventFatalError))
    {
        quint32 EventCode = HandleCANMsgEvent(pCANframe);
        hdlInfo = DCL_ERR_EXTERNAL_ERROR;

        for(qint32 i = 0; i < m_ModuleCommand.size(); i++)
        {
            if(m_ModuleCommand[i]->State == MODULE_CMD_STATE_REQ_SEND)
            {
                ModuleCommandType = m_ModuleCommand[i]->Type;
                break;
            }
        }

        if (ModuleCommandType == FM_RFID_CMD_TYPE_UNDEF &&
             (pCANframe->can_id == m_unCanIDEventError || pCANframe->can_id == m_unCanIDEventFatalError)) {
            emit ReportEvent(EventCode, m_lastEventData, m_lastEventTime);
        }
    }

    if(m_unCanIDConfigAckn == pCANframe->can_id || FM_RFID_CMD_TYPE_SET_CONFIG == ModuleCommandType)
    {
        HandleCANMsgConfigAckn(pCANframe, hdlInfo);
    }
    else if(m_unCanIDLoginAckn == pCANframe->can_id || FM_RFID_CMD_TYPE_LOGIN == ModuleCommandType)
    {
        HandleCANMsgLoginAckn(pCANframe, hdlInfo);
    }
    else if(m_unCanIDUserData == pCANframe->can_id || FM_RFID_CMD_TYPE_USER_DATA_REQ == ModuleCommandType)
    {
        HandleCANMsgUserData(pCANframe, hdlInfo);
    }
    else if(m_unCanIDWriteUserDataAckn == pCANframe->can_id || FM_RFID_CMD_TYPE_USER_DATA_WRITE == ModuleCommandType)
    {
        HandleCANMsgWriteData(pCANframe, hdlInfo);
    }
    else if(m_unCanIDUID == pCANframe->can_id || FM_RFID_CMD_TYPE_UID_REQ == ModuleCommandType)
    {
        HandleCANMsgUID(pCANframe, hdlInfo);
    }
    else if(m_unCanIDWritePasswordAckn == pCANframe->can_id || FM_RFID_CMD_TYPE_PASSWORD_WRITE == ModuleCommandType)
    {
        HandleCANMsgWritePassword(pCANframe, hdlInfo);
    }
    else if(m_unCanIDWriteConfigAckn == pCANframe->can_id || FM_RFID_CMD_TYPE_CONGIG_WRITE == ModuleCommandType)
    {
        HandleCANMsgWriteConfig(pCANframe, hdlInfo);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'SetConfiguration' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *  \iparam hdlInfo = Indicates if the message was received successfully
 */
/****************************************************************************/
void CRfid11785::HandleCANMsgConfigAckn(can_frame* pCANframe, ReturnCode_t hdlInfo)
{
    FILE_LOG_L(laFCT, llINFO) << " CANRFID";

    if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        if(pCANframe->can_dlc != 0)
        {
            hdlInfo = DCL_ERR_CANMSG_INVALID;
        }
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_RFID_CMD_TYPE_SET_CONFIG);
    }
    emit ReportStateAckn(GetModuleHandle(), hdlInfo);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'Login' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *  \iparam hdlInfo = Indicates if the message was received successfully
 */
/****************************************************************************/
void CRfid11785::HandleCANMsgLoginAckn(can_frame* pCANframe, ReturnCode_t hdlInfo)
{
    FILE_LOG_L(laFCT, llINFO) << " CANRFID";

    if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        if(pCANframe->can_dlc != 0)
        {
            hdlInfo = DCL_ERR_CANMSG_INVALID;
        }
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_RFID_CMD_TYPE_LOGIN);
    }
    emit ReportLoginAckn(GetModuleHandle(), hdlInfo);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'UserData' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *  \iparam hdlInfo = Indicates if the message was received successfully
 */
/****************************************************************************/
void CRfid11785::HandleCANMsgUserData(can_frame* pCANframe, ReturnCode_t hdlInfo)
{
    FILE_LOG_L(laFCT, llINFO) << " CANRFID";
    quint32 RFIDData = 0;
    quint8 RFIDAddress = 0;

    if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        if(pCANframe->can_dlc == 5)
        {
            RFIDAddress = pCANframe->data[0];
            RFIDData = GetCANMsgDataU32(pCANframe, 1);
        }
        else
        {
            hdlInfo = DCL_ERR_CANMSG_INVALID;
        }
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_RFID_CMD_TYPE_USER_DATA_REQ);
    }
    emit ReportUserData(GetModuleHandle(), hdlInfo, RFIDAddress, RFIDData);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'WriteDataAckn' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *  \iparam hdlInfo = Indicates if the message was received successfully
 */
/****************************************************************************/
void CRfid11785::HandleCANMsgWriteData(can_frame* pCANframe, ReturnCode_t hdlInfo)
{
    FILE_LOG_L(laFCT, llINFO) << " CANRFID";
    quint8 RFIDAddress = 0;

    if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        if(pCANframe->can_dlc == 1)
        {
            RFIDAddress = pCANframe->data[0];
        }
        else
        {
            hdlInfo = DCL_ERR_CANMSG_INVALID;
        }
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_RFID_CMD_TYPE_USER_DATA_WRITE);
    }
    emit ReportWriteData(GetModuleHandle(), hdlInfo, RFIDAddress);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'UID' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *  \iparam hdlInfo = Indicates if the message was received successfully
 */
/****************************************************************************/
void CRfid11785::HandleCANMsgUID(can_frame* pCANframe, ReturnCode_t hdlInfo)
{
    FILE_LOG_L(laFCT, llINFO) << " CANRFID";
    quint32 RFIDUID = 0;

    if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        if(pCANframe->can_dlc == 4)
        {
            RFIDUID = GetCANMsgDataU32(pCANframe, 0);
        }
        else
        {
            hdlInfo = DCL_ERR_CANMSG_INVALID;
        }
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_RFID_CMD_TYPE_UID_REQ);
    }
    emit ReportUID(GetModuleHandle(), hdlInfo, RFIDUID);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'WritePasswordAckn' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *  \iparam hdlInfo = Indicates if the message was received successfully
 */
/****************************************************************************/
void CRfid11785::HandleCANMsgWritePassword(can_frame* pCANframe, ReturnCode_t hdlInfo)
{
    FILE_LOG_L(laFCT, llINFO) << " CANRFID";

    if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        if(pCANframe->can_dlc != 0)
        {
            hdlInfo = DCL_ERR_CANMSG_INVALID;
        }
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_RFID_CMD_TYPE_PASSWORD_WRITE);
    }
    emit ReportWritePassword(GetModuleHandle(), hdlInfo);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of the 'WriteConfigAckn' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *  \iparam hdlInfo = Indicates if the message was received successfully
 */
/****************************************************************************/
void CRfid11785::HandleCANMsgWriteConfig(can_frame* pCANframe, ReturnCode_t hdlInfo)
{
    FILE_LOG_L(laFCT, llINFO) << " CANRFID";

    if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
    {
        if(pCANframe->can_dlc != 0)
        {
            hdlInfo = DCL_ERR_CANMSG_INVALID;
        }
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_RFID_CMD_TYPE_CONGIG_WRITE);
    }
    emit ReportWriteConfiguration(GetModuleHandle(), hdlInfo);
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN-message 'SetConfiguration'
 *
 *      The function module's configuration parameter will be sent via
 *      CAN-Bus to the slave. This message contains the module enable/disable
 *      parameter.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CRfid11785::SendCANMessageConfiguration()
{
    can_frame canmsg;
    ReturnCode_t RetVal;

    canmsg.can_id = m_unCanIDConfig;
    canmsg.data[0] = m_ConfigurationState.Enabled;
    canmsg.data[0] |= ((quint8)(m_ConfigurationState.Antenna << 2));
    canmsg.data[0] |= ((quint8)(m_ConfigurationState.DataRate << 6));
    canmsg.can_dlc = 1;

    FILE_LOG_L(laCONFIG, llDEBUG) << " " << GetName().toStdString() << ": CANRFID11785: data[0]: 0x" <<
                                     std::hex << (int) canmsg.data[0];

    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN-message 'Login'
 *
 *      The RFID login password will be sent via CAN-Bus to the slave.
 *
 *  \iparam Password = Password to unlock the RFID tag
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CRfid11785::SendCANMsgLogin(quint32 Password)
{
    ReturnCode_t RetVal;
    can_frame CanMsg;

    CanMsg.can_id = m_unCanIDLogin;
    CanMsg.can_dlc = 4;
    SetCANMsgDataU32(&CanMsg, Password, 0);
    RetVal = m_pCANCommunicator->SendCOB(CanMsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN-message 'ReadUserData'
 *
 *      A user data read request will be sent via CAN-Bus to the slave.
 *
 *  \iparam Address = RFID tag register address
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CRfid11785::SendCANMsgReadUserData(quint8 Address)
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDUserDataReq;
    canmsg.can_dlc = 1;
    canmsg.data[0] = Address;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llINFO) << " CANRFID: " << (int) RetVal;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN-message 'WriteUserData'
 *
 *      A user data write request will be sent via CAN-Bus to the slave.
 *
 *  \iparam Address = RFID tag register address
 *  \iparam Data = RFID tag register address
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CRfid11785::SendCANMsgWriteUserData(quint8 Address, quint32 Data)
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDWriteUserData;
    canmsg.can_dlc = 5;
    canmsg.data[0] = Address;
    SetCANMsgDataU32(&canmsg, Data, 1);
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llINFO) << " CANRFID: " << (int) RetVal;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN-message 'ReadUID'
 *
 *      A UID read request will be sent via CAN-Bus to the slave.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CRfid11785::SendCANMsgReadUID()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDUIDReq;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llINFO) << " CANRFID: " << (int) RetVal;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN-message 'WritePassword'
 *
 *      A password write request will be sent via CAN-Bus to the slave.
 *
 *  \iparam Password = Password to be written
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CRfid11785::SendCANMsgWritePassword(quint32 Password)
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDWritePassword;
    canmsg.can_dlc = 4;
    SetCANMsgDataU32(&canmsg, Password, 0);
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llINFO) << " CANRFID: " << (int) RetVal;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN-message 'WriteConfig'
 *
 *      A configuration write request will be sent via CAN-Bus to the slave.
 *
 *  \iparam ReadLogin = Needs a login before a read access
 *  \iparam WriteLogin = Needs a login before a write access
 *  \iparam ReaderTalkFirst = Tag only sends data when asked by the reader
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code from SendCOB
 */
/****************************************************************************/
ReturnCode_t CRfid11785::SendCANMsgWriteConfig(bool ReadLogin, bool WriteLogin, bool ReaderTalkFirst)
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDWriteConfig;
    canmsg.can_dlc = 1;
    canmsg.data[0] = 0x10;

    if(ReadLogin) {
        canmsg.data[0] |= 0x01;
    }
    if(WriteLogin) {
        canmsg.data[0] |= 0x02;
    }
    if(ReaderTalkFirst) {
        canmsg.data[0] |= 0x04;
    }

    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llINFO) << " CANRFID: " << (int) RetVal;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Set the function module's configuration state (enabled/disabled)
 *
 *      The task will be acknowledged by sending the signal ReportStateAckn
 *
 *  \iparam Enabled = Flag for module on/off
 *  \iparam Antenna = Antenna selection
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRfid11785::SetState(bool Enabled, quint8 Antenna)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if (Antenna >= 16) {
        return DCL_ERR_INVALID_PARAM;
    }

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_RFID_CMD_TYPE_SET_CONFIG);
    if(p_ModuleCommand != NULL)
    {
        m_ConfigurationState.Enabled = Enabled;
        m_ConfigurationState.Antenna = Antenna;
        FILE_LOG_L(laDEV, llINFO) << " CANRFID11785, Enabled/Antenna: " << (int) Enabled << " / " << (int)Antenna;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANRFID11785 invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Set the RFID tags login password
 *
 *      The task will be acknowledged by sending the signal ReportLoginAckn
 *
 *  \iparam Password = login password
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRfid11785::SetLogin(quint32 Password)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_RFID_CMD_TYPE_LOGIN);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->Password = Password;
        FILE_LOG_L(laDEV, llINFO) << " CANRFID11785";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANRFID11785 invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the unique ID of the RFID tag
 *
 *      The task will be acknowledged by sending the signal ReportUID
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRfid11785::ReqUID()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_RFID_CMD_TYPE_UID_REQ);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANRFID11785 invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the RFID user data of the RFID tag
 *
 *      The task will be acknowledged by sending the signal ReportUserData.
 *
 *  \iparam Address = Address of the RFID tag register
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRfid11785::ReqUserData(quint8 Address)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_RFID_CMD_TYPE_USER_DATA_REQ);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->UserDataAddress = Address;
        FILE_LOG_L(laDEV, llDEBUG) << " CANRFID11785";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANRFID11785 invalid state: " << (quint32)m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Write RFID user data to the RFID tag
 *
 *      The task will be acknowledged by sending the signal ReportWriteData.
 *
 *  \iparam Address = Address of the RFID tag register
 *  \iparam Data = Data word to be written
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRfid11785::WriteUserData(quint8 Address, quint32 Data)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_RFID_CMD_TYPE_USER_DATA_WRITE);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->UserDataAddress = Address;
        p_ModuleCommand->UserData = Data;
        FILE_LOG_L(laDEV, llDEBUG) << " CANRFID11785";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANRFID11785 invalid state: " << (quint32)m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Write the password to the RFID tag
 *
 *      The task will be acknowledged by sending the signal
 *      ReportWritePassword.
 *
 *  \iparam Password = Password to be written
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRfid11785::WritePassword(quint32 Password)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_RFID_CMD_TYPE_PASSWORD_WRITE);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->Password = Password;
        FILE_LOG_L(laDEV, llDEBUG) << " CANRFID11785";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANRFID11785 invalid state: " << (quint32)m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Write the configuraton register of the RFID tag
 *
 *      The task will be acknowledged by sending the signal
 *      ReportWriteConfig.
 *
 *  \iparam ReadLogin = Needs a login before a read access
 *  \iparam WriteLogin = Needs a login before a write access
 *  \iparam ReaderTalkFirst = Tag only sends data when asked by the reader
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRfid11785::WriteConfiguration(bool ReadLogin, bool WriteLogin, bool ReaderTalkFirst)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_RFID_CMD_TYPE_CONGIG_WRITE);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->ReadLogin = ReadLogin;
        p_ModuleCommand->WriteLogin = WriteLogin;
        p_ModuleCommand->ReaderTalkFirst = ReaderTalkFirst;
        FILE_LOG_L(laDEV, llDEBUG) << " CANRFID11785";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANRFID11785 invalid state: " << (quint32)m_TaskID;
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
CRfid11785::ModuleCommand_t *CRfid11785::SetModuleTask(CANRFIDModuleCmdType_t CommandType)
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
void CRfid11785::ResetModuleCommand(CANRFIDModuleCmdType_t CommandType)
{
    for(qint32 i = 0; i < m_ModuleCommand.size(); i++) {
        if (m_ModuleCommand[i]->Type == CommandType) {
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
