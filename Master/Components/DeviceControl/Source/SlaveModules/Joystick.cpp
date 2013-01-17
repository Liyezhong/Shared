/****************************************************************************/
/*! \file Joystick.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the CANJoystick class
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

#include "DeviceControl/Include/SlaveModules/Joystick.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor for the CANJoystick
 *
 *  \iparam p_MessageConfiguration = Message configuration
 *  \iparam pCANCommunicator = pointer to communication class
 *  \iparam pParentNode = pointer to CANNode, where this module is assigned to
 */
/****************************************************************************/
CJoystick::CJoystick(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                     CBaseModule* pParentNode) :
    CFunctionModule(CModuleConfig::CAN_OBJ_TYPE_JOYSTICK, p_MessageConfiguration, pCANCommunicator, pParentNode),
    m_unCanIDConfig(0), m_unCanIDVoltageRangeSet(0), m_unCanIDMechOffsetSet(0),
    m_unCanIDDisplacementReq(0), m_unCanIDDisplacement(0),
    m_unCanIDMechOffsetReq(0), m_unCanIDMechOffset(0)
{
    m_mainState = FM_MAIN_STATE_BOOTUP;
    m_subStateConfig = FM_JSTCK_SUB_STATE_CONFIG_INIT;
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
ReturnCode_t CJoystick::Initialize()
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
 *  \brief  Initialize the module's CAN message IDs
 *
 *      The CAN-IDs are read from the CAN-Message configuration class. The
 *      CAN-ID is composed by the message key, the function module's channel
 *      and the node id of the CANNode this fct-module is assigned to.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CJoystick::InitializeCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 bIfaceID;
    const quint8 ModuleID = MODULE_ID_JOYSTICK;

    if(m_pCANObjectConfig == 0)
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    bIfaceID = m_pCANObjectConfig->m_sChannel;

    RetVal = InitializeEventCANMessages(ModuleID);

    m_unCanIDConfig          = mp_MessageConfiguration->GetCANMessageID(ModuleID, "JoystickConfig", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDVoltageRangeSet = mp_MessageConfiguration->GetCANMessageID(ModuleID, "JoystickVoltageRangeSet", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDMechOffsetSet   = mp_MessageConfiguration->GetCANMessageID(ModuleID, "JoystickMechOffsetSet", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDisplacementReq = mp_MessageConfiguration->GetCANMessageID(ModuleID, "JoystickDisplacementReq", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDisplacement    = mp_MessageConfiguration->GetCANMessageID(ModuleID, "JoystickDisplacement", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDMechOffsetReq   = mp_MessageConfiguration->GetCANMessageID(ModuleID, "JoystickMechOffsetReq", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDMechOffset      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "JoystickMechOffset", bIfaceID, m_pParent->GetNodeID());

    FILE_LOG_L(laINIT, llDEBUG) << " CAN-messages for fct-module: " << GetName().toStdString() << ",node id:" << std::hex << m_pParent->GetNodeID();
    FILE_LOG_L(laINIT, llDEBUG) << "   EventInfo               : 0x" << std::hex << m_unCanIDEventInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventWarning            : 0x" << std::hex << m_unCanIDEventWarning;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventError              : 0x" << std::hex << m_unCanIDEventError;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventFatalError         : 0x" << std::hex << m_unCanIDEventFatalError;
    FILE_LOG_L(laINIT, llDEBUG) << "   JoystickConfig          : 0x" << std::hex << m_unCanIDConfig;
    FILE_LOG_L(laINIT, llDEBUG) << "   JoystickVoltageRangeSet : 0x" << std::hex << m_unCanIDVoltageRangeSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   JoystickMechOffsetSet   : 0x" << std::hex << m_unCanIDMechOffsetSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   JoystickDisplacementReq : 0x" << std::hex << m_unCanIDDisplacementReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   JoystickDisplacement    : 0x" << std::hex << m_unCanIDDisplacement;
    FILE_LOG_L(laINIT, llDEBUG) << "   JoystickMechOffsetReq   : 0x" << std::hex << m_unCanIDMechOffsetReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   JoystickMechOffset      : 0x" << std::hex << m_unCanIDMechOffset;

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
ReturnCode_t CJoystick::RegisterCANMessages()
{
    ReturnCode_t RetVal;

    RetVal = RegisterEventCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDDisplacement, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDMechOffset, this);
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
void CJoystick::HandleTasks()
{
    QMutexLocker Locker(&m_Mutex);

    if(FM_MAIN_STATE_IDLE == m_mainState)
    {
        HandleIdleState();
    }
    else if(FM_MAIN_STATE_CONFIRMED == m_mainState)
    {
        //fall through
        m_subStateConfig = FM_JSTCK_SUB_STATE_CONFIG_START;
        m_mainState = FM_MAIN_STATE_CONFIG;
    }
    else if(FM_MAIN_STATE_CONFIG == m_mainState)
    {
        ReturnCode_t RetVal;

        if(m_subStateConfig == FM_JSTCK_SUB_STATE_CONFIG_START)
        {
            RetVal = SendCANMsgSetConfig(false, false, false);
            if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_subStateConfig = FM_JSTCK_SUB_STATE_CONFIG_FINISH;
                m_TaskID = MODULE_TASKID_FREE;
                m_mainState = FM_MAIN_STATE_IDLE;

                FILE_LOG_L(laCONFIG, llDEBUG) << " Module " << GetName().toStdString() << ": change to FM_MAIN_STATE_IDLE";
            }
            else
            {
                FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCOB returns" << (int) RetVal;
                m_lastEventHdlInfo = DCL_ERR_FCT_CALL_FAILED;
                m_subStateConfig = FM_JSTCK_SUB_STATE_CONFIG_ERROR;
                m_mainState = FM_MAIN_STATE_ERROR;
            }
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
void CJoystick::HandleIdleState()
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
 *   The function is called from HandleIdleState() if m_TaskID == MODULE_TASKID_COMMAND_HDL.
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
void CJoystick::HandleCommandRequestTask()
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

            if(p_ModuleCommand->Type == FM_JSTCK_CMD_TYPE_SET_CONFIG)
            {
                //send the configuration data to the slave, this command will not be acknowledged by reception
                FILE_LOG_L(laFCT, llINFO) << " CJoystick: 'Set config' sent";
                RetVal = SendCANMsgSetConfig(p_ModuleCommand->Active, p_ModuleCommand->Scanning,
                                             p_ModuleCommand->SetCenter);

                RemoveCommand = true;
                emit ReportSetState(GetModuleHandle(), RetVal);
            }
            else if(p_ModuleCommand->Type == FM_JSTCK_CMD_TYPE_SET_VOLTAGE_RANGE)
            {
                //send the voltage range to the slave, this command will not be acknowledged by reception
                FILE_LOG_L(laFCT, llINFO) << " CJoystick: 'Set voltage range' sent";
                RetVal = SendCANMsgSetVoltageRange(p_ModuleCommand->MinimumX, p_ModuleCommand->MaximumX,
                                                   p_ModuleCommand->MinimumY, p_ModuleCommand->MaximumY);

                RemoveCommand = true;
                emit ReportSetVoltageRange(GetModuleHandle(), RetVal);
            }
            else if(p_ModuleCommand->Type == FM_JSTCK_CMD_TYPE_SET_MECH_OFFSET)
            {
                //send the mechanical offset to the slave, this command will not be acknowledged by reception
                FILE_LOG_L(laFCT, llINFO) << " CJoystick: 'Set mechanical offset' sent";
                RetVal = SendCANMsgSetMechanicalOffset(p_ModuleCommand->MechanicalOffset);

                RemoveCommand = true;
                emit ReportSetMechanicalOffset(GetModuleHandle(), RetVal);
            }
            else if(p_ModuleCommand->Type == FM_JSTCK_CMD_TYPE_GET_DISPLACEMENT)
            {
                //send the value request to the slave, this command will be acknowledged by reception
                FILE_LOG_L(laFCT, llINFO) << " CJoystick: 'Get displacement' sent";
                RetVal = SendCANMsgGetDisplacement();

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = 100;
                }
                else
                {
                    emit ReportGetDisplacement(GetModuleHandle(), RetVal, 0, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_JSTCK_CMD_TYPE_GET_MECH_OFFSET)
            {
                //send the value request to the slave, this command will be acknowledged by the reception
                FILE_LOG_L(laFCT, llINFO) << " CJoystick: 'Get mechanical offset' sent";
                RetVal = SendCANMsgGetMechanicalOffset();

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = 100;
                }
                else
                {
                    emit ReportGetMechanicalOffset(GetModuleHandle(), RetVal, 0);
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
            // check active commands for timeout
            if(p_ModuleCommand->ReqSendTime.Elapsed() > p_ModuleCommand->Timeout)
            {
                RemoveCommand = true;

                if(p_ModuleCommand->Type == FM_JSTCK_CMD_TYPE_SET_CONFIG)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID11785:: '" << GetKey().toStdString() << "': set configuration timeout";
                    emit ReportSetState(GetModuleHandle(), DCL_ERR_TIMEOUT);
                }
                else if(p_ModuleCommand->Type == FM_JSTCK_CMD_TYPE_SET_VOLTAGE_RANGE)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID11785:: '" << GetKey().toStdString() << "': set voltage range timeout";
                    emit ReportSetVoltageRange(GetModuleHandle(), DCL_ERR_TIMEOUT);
                }
                else if(p_ModuleCommand->Type == FM_JSTCK_CMD_TYPE_SET_MECH_OFFSET)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID11785:: '" << GetKey().toStdString() << "': set mechanical offset timeout";
                    emit ReportSetMechanicalOffset(GetModuleHandle(), DCL_ERR_TIMEOUT);
                }
                else if(p_ModuleCommand->Type == FM_JSTCK_CMD_TYPE_GET_DISPLACEMENT)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID11785:: '" << GetKey().toStdString() << "': get displacement timeout";
                    emit ReportGetDisplacement(GetModuleHandle(), DCL_ERR_TIMEOUT, 0, 0);
                }
                else if(p_ModuleCommand->Type == FM_JSTCK_CMD_TYPE_GET_MECH_OFFSET)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANRFID11785:: '" << GetKey().toStdString() << "': get mechanical offset timeout";
                    emit ReportGetMechanicalOffset(GetModuleHandle(), DCL_ERR_TIMEOUT, 0);
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
 *  \brief  Handle the reception of a CAN message
 *
 *      The function is called from communication layer if a CAN message,
 *      which was registered to this class instance, was received. The
 *      message will be forwarded to the specialized function.
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CJoystick::HandleCanMessage(can_frame* pCANframe)
{
    QMutexLocker Locker(&m_Mutex);

    FILE_LOG_L(laFCT, llDEBUG) << "  CANJoystick::HandleCanMessage: 0x" << std::hex << pCANframe->can_id;
    FILE_LOG_L(laFCT, llDEBUG) << "                  dlc:" << (int) pCANframe->can_dlc << " Task:" << (int) m_TaskID;

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
    else if(pCANframe->can_id == m_unCanIDDisplacement)
    {
        HandleCANMsgGetDisplacement(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDMechOffset)
    {
        HandleCANMsgGetMechanicalOffset(pCANframe);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'Displacement'
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CJoystick::HandleCANMsgGetDisplacement(can_frame* pCANframe)
{
    FILE_LOG_L(laFCT, llINFO) << " CJoystick";
    ReturnCode_t HdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
    quint16 DisplacementX = 0;
    quint16 DisplacementY = 0;

    if(pCANframe->can_dlc == 4)
    {
        DisplacementX = GetCANMsgDataU16(pCANframe, 0);
        DisplacementY = GetCANMsgDataU16(pCANframe, 2);
    }
    else
    {
        HdlInfo = DCL_ERR_CANMSG_INVALID;
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_JSTCK_CMD_TYPE_GET_DISPLACEMENT);
    }
    emit ReportGetDisplacement(GetModuleHandle(), HdlInfo, DisplacementX, DisplacementY);
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'Mechanical offset'
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CJoystick::HandleCANMsgGetMechanicalOffset(can_frame* pCANframe)
{
    FILE_LOG_L(laFCT, llINFO) << " CJoystick";
    ReturnCode_t HdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
    quint32 Offset = 0;

    if(pCANframe->can_dlc == 4)
    {
        Offset = GetCANMsgDataU32(pCANframe, 0);
    }
    else
    {
        HdlInfo = DCL_ERR_CANMSG_INVALID;
    }

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_JSTCK_CMD_TYPE_GET_MECH_OFFSET);
    }
    emit ReportGetMechanicalOffset(GetModuleHandle(), HdlInfo, Offset);
}

/****************************************************************************/
/*!
 *  \brief  Send the 'Configuration' CAN-Message
 *
 *      The function module's configuration parameter will be sent via
 *      CAN-Bus to the slave. The function module on slave side needs the
 *      following parameters:
 *
 *  \iparam Active = Enables/disables the module
 *  \iparam Scanning = Permanent (true) or threshold (false) mode
 *  \iparam SetCenter = Calibrates the center position
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CJoystick::SendCANMsgSetConfig(bool Active, bool Scanning, bool SetCenter)
{
    CANFctModuleJoystick* pCANObjConfJoystick;
    pCANObjConfJoystick = (CANFctModuleJoystick*) m_pCANObjectConfig;
    can_frame canmsg;
    ReturnCode_t RetVal;

    if(pCANObjConfJoystick)
    {
        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration : 0x" << std::hex << m_unCanIDConfig;
        canmsg.can_id = m_unCanIDConfig;
        canmsg.can_dlc = 7;

        canmsg.data[0] = 0;
        if(Active)
        {
            canmsg.data[0] |= 0x01;
        }
        if(Scanning)
        {
            canmsg.data[0] |= 0x02;
        }
        if(SetCenter)
        {
            canmsg.data[0] |= 0x04;
        }
        SetCANMsgDataU16(&canmsg, pCANObjConfJoystick->m_SamplingRate, 1);
        SetCANMsgDataU16(&canmsg, pCANObjConfJoystick->m_UpperThreshold, 3);
        SetCANMsgDataU16(&canmsg, pCANObjConfJoystick->m_LowerThreshold, 5);

        canmsg.can_dlc = 7;

        RetVal = m_pCANCommunicator->SendCOB(canmsg);
    }
    else
    {
        FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": configuration not available";
        m_lastEventHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the 'VoltageRangeSet' CAN-Message
 *
 *  \iparam MinX = Minimum voltage of the X-axis in tenth of mV
 *  \iparam MaxX = Maximum voltage of the X-axis in tenth of mV
 *  \iparam MinY = Minimum voltage of the Y-axis in tenth of mV
 *  \iparam MaxY = Maximum voltage of the Y-axis in tenth of mV
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CJoystick::SendCANMsgSetVoltageRange(qint16 MinX, qint16 MaxX, qint16 MinY, qint16 MaxY)
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDVoltageRangeSet;
    SetCANMsgDataS16(&canmsg, MinX, 0);
    SetCANMsgDataS16(&canmsg, MaxX, 2);
    SetCANMsgDataS16(&canmsg, MinY, 4);
    SetCANMsgDataS16(&canmsg, MaxY, 6);
    canmsg.can_dlc = 8;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the 'MechanicalOffsetSet' CAN-Message
 *
 *  \iparam Offset = Mechanical offset data
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CJoystick::SendCANMsgSetMechanicalOffset(quint32 Offset)
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDMechOffsetSet;
    SetCANMsgDataU32(&canmsg, Offset, 0);
    canmsg.can_dlc = 4;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the 'MechanicalOffsetRequest' CAN-Message
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CJoystick::SendCANMsgGetMechanicalOffset()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDMechOffsetReq;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the 'DisplacementRequest' CAN-Message
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CJoystick::SendCANMsgGetDisplacement()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDDisplacementReq;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Sets the state of the joystick module
 *
 *  \iparam Active = Enables/disables the module
 *  \iparam Scanning = Permanent (true) or threshold (false) mode
 *  \iparam SetCenter = Calibrates the center position
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CJoystick::SetState(bool Active, bool Scanning, bool SetCenter)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_JSTCK_CMD_TYPE_SET_CONFIG);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->Active = Active;
        p_ModuleCommand->Scanning = Scanning;
        p_ModuleCommand->SetCenter = SetCenter;
        FILE_LOG_L(laFCT, llINFO) << "   CANJoystick::SetState";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << "   CANJoystick::SetState invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Sets the voltage range for the joystick axes
 *
 *  \iparam MinX = Minimum voltage of the X-axis in tenth of mV
 *  \iparam MaxX = Maximum voltage of the X-axis in tenth of mV
 *  \iparam MinY = Minimum voltage of the Y-axis in tenth of mV
 *  \iparam MaxY = Maximum voltage of the Y-axis in tenth of mV
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CJoystick::SetVoltageRange(qint16 MinX, qint16 MaxX, qint16 MinY, qint16 MaxY)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_JSTCK_CMD_TYPE_SET_VOLTAGE_RANGE);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->MinimumX = MinX;
        p_ModuleCommand->MaximumX = MaxX;
        p_ModuleCommand->MinimumY = MinY;
        p_ModuleCommand->MaximumY = MaxY;
        FILE_LOG_L(laFCT, llINFO) << "   CANJoystick::SetVoltageRange";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << "   CANJoystick::SetVoltageRange invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Sets the mechanical offset stored on the board
 *
 *  \iparam Offset = Mechanical offset data
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CJoystick::SetMechanicalOffset(quint32 Offset)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_JSTCK_CMD_TYPE_SET_MECH_OFFSET);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->MechanicalOffset = Offset;
        FILE_LOG_L(laFCT, llINFO) << "   CANJoystick::SetMechanicalOffset";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << "   CANJoystick::SetMechanicalOffset invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Requests the actual displacement value
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CJoystick::GetDisplacement()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_JSTCK_CMD_TYPE_GET_DISPLACEMENT);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << "   CANJoystick::GetDisplacement invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Requests the mechanical offset stored on the board
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CJoystick::GetMechanicalOffset()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_JSTCK_CMD_TYPE_GET_MECH_OFFSET);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << "   CANJoystick::GetMechanicalOffset invalid state: " << (int) m_TaskID;
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
CJoystick::ModuleCommand_t *CJoystick::SetModuleTask(CANJoystickModuleCmdType_t CommandType)
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
void CJoystick::ResetModuleCommand(CANJoystickModuleCmdType_t CommandType)
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
