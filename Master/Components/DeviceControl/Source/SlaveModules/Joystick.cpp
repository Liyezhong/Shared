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
 *  \brief    Constructor for the CANJoystick
 *
 *  \param    mp_MessageConfiguration = Message configuration
 *  \param    pCANCommunicator = pointer to communication class
 *  \param    pParentNode = pointer to CANNode, where this module is assigned to
 *
 ****************************************************************************/
CJoystick::CJoystick(const CANMessageConfiguration *mp_MessageConfiguration, CANCommunicator* pCANCommunicator,
                     CBaseModule* pParentNode) :  //lint !e578
    CFunctionModule(CModuleConfig::CAN_OBJ_TYPE_JOYSTICK, mp_MessageConfiguration, pCANCommunicator, pParentNode),
    m_unCanIDConfig(0), m_unCanIDMechOffsetSet(0),
    m_unCanIDDisplacementReq(0), m_unCanIDDisplacement(0),
    m_unCanIDMechOffsetReq(0), m_unCanIDMechOffset(0),
    m_aktionTimespan(0), m_actInputValue(0), m_contactLimit(0)
{
    m_mainState = FM_MAIN_STATE_BOOTUP;
    m_subStateConfig = FM_JSTCK_SUB_STATE_CONFIG_INIT;
    m_taskState = MODULE_CMD_STATE_FREE;
}

/****************************************************************************/
/*!
 *  \brief    Destructor of CANJoystick
 *
 ****************************************************************************/
CJoystick::~CJoystick()
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
 *  \brief    Initialize the module's CAN message IDs
 *
 *   The CAN-IDs are read from the CAN-Message configuration class.
 *   The CAN-ID is composed by the message key, the function
 *   module's channel and the node id of the CANNode this fct-module is assigned to.
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
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
    FILE_LOG_L(laINIT, llDEBUG) << "   JoystickMechOffsetSet   : 0x" << std::hex << m_unCanIDMechOffsetSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   JoystickDisplacementReq : 0x" << std::hex << m_unCanIDDisplacementReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   JoystickDisplacement    : 0x" << std::hex << m_unCanIDDisplacement;
    FILE_LOG_L(laINIT, llDEBUG) << "   JoystickMechOffsetReq   : 0x" << std::hex << m_unCanIDMechOffsetReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   JoystickMechOffset      : 0x" << std::hex << m_unCanIDMechOffset;

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
 *  \brief    Handles the function module's state machine
 *
 *   Depending on the active main state, the appropriate task function will be called.
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
            RetVal = SendCANMsgConfig();
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
                m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_FAILED;
                m_subStateConfig = FM_JSTCK_SUB_STATE_CONFIG_ERROR;
                m_mainState = FM_MAIN_STATE_ERROR;
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
void CJoystick::HandleIdleState()
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL/*FM_MODULE_TASKID_REQ_ACTINP*/) //!< \todo
    {
        HandleDisplacmentRequestTask();
    }
    else if(m_TaskID == MODULE_TASKID_COMMAND_HDL/*FM_MODULE_TASKID_SET_CONTACT_LIMIT*/) //!< \todo
    {
        HandleContactLimitSetTask();
    }
}

void CJoystick::HandleDisplacmentRequestTask()
{
    if(m_taskState == MODULE_CMD_STATE_REQ)
    {
        ReturnCode_t retval;

        FILE_LOG_L(laFCT, llINFO) << "   CANJoystick:: HandleDisplacmentRequestTask: send request.";
        retval = SendCANMsgDisplacementReq();
        if(retval == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_aktionTimespan = CAN_JOYSTICK_TIMEOUT_READ;
            m_timeAction.Trigger();
            m_taskState = MODULE_CMD_STATE_REQ_SEND;
        }
        else
        {
            ResetTask();
            emit ReportActInputValue(GetModuleHandle(), retval, 0);
        }
    }
    else if(m_taskState == MODULE_CMD_STATE_REQ_SEND)
    {
        //check timeout
        if(m_timeAction.Elapsed() > m_aktionTimespan)
        {
            FILE_LOG_L(laFCT, llERROR) << "  CANJoystick:: InputDataRequest: timeout";
            ResetTask();
            emit ReportActInputValue(GetModuleHandle(), DCL_ERR_TIMEOUT, 0);
        }
    }
    else
    {
        FILE_LOG_L(laFCT, llERROR) << "   CANJoystick: HandleInputDataRequestTask, invalid task, id: " << (int) m_TaskID << ", state: " << (int) m_taskState;
    }
}

void CJoystick::HandleContactLimitSetTask()
{
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
        HandleCANMsgError(pCANframe);
        if ((pCANframe->can_id == m_unCanIDEventError) || (pCANframe->can_id == m_unCanIDEventFatalError)) {
            emit ReportError(GetModuleHandle(), m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
    }
    else if(pCANframe->can_id == m_unCanIDDisplacement)
    {
        HandleCANMsgInputValueResp(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDMechOffset)
    {
        ;
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the reception of the CAN message 'Displacement'
 *
 *  \iparam   pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CJoystick::HandleCANMsgInputValueResp(can_frame* pCANframe)
{
    if((m_TaskID == MODULE_TASKID_COMMAND_HDL) && (m_taskState == MODULE_CMD_STATE_REQ_SEND)) //!< \todo
    {
        ResetTask();
    }

    if(pCANframe->can_dlc == 8)
    {
        m_actInputValue = ((qint16) pCANframe->data[0]) << 8;
        m_actInputValue |= ((qint16) pCANframe->data[1]);
        FILE_LOG_L(laFCT, llINFO) << "  CANJoystick: Input value received: " << std::hex << m_actInputValue;

        emit ReportActInputValue(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, m_actInputValue);
    }
    else
    {
        emit ReportActInputValue(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief    Send the 'Configuration' CAN-Message
 *
 *   The function module's configuration parameter will be sent via CAN-Bus to the slave.
 *   The function module on slave side needs the following parameters:
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CJoystick::SendCANMsgConfig()
{
    CANFctModuleJoystick* pCANObjConfJoystick;
    pCANObjConfJoystick = (CANFctModuleJoystick*) m_pCANObjectConfig;
    can_frame canmsg;
    ReturnCode_t RetVal;

    bool SetCenter = false;
    bool CalibMode = false;

    if(pCANObjConfJoystick)
    {
        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration : 0x" << std::hex << m_unCanIDConfig;
        canmsg.can_id = m_unCanIDConfig;
        canmsg.can_dlc = 7;

        canmsg.data[0] = 0;
        if(pCANObjConfJoystick->m_bCommModeThresHold)
        {
            canmsg.data[0] |= 0x01;
        }
        if(SetCenter)
        {
            canmsg.data[0] |= 0x02;
        }
        if(CalibMode)
        {
            canmsg.data[0] |= 0x04;
        }
        SetCANMsgDataU16(&canmsg, pCANObjConfJoystick->m_sSampleRate, 1);
        SetCANMsgDataU16(&canmsg, pCANObjConfJoystick->m_sUpperThreshold, 3);
        SetCANMsgDataU16(&canmsg, pCANObjConfJoystick->m_sLowerThreshold, 5);
        canmsg.data[7] = 0;

        canmsg.can_dlc = 7;

        RetVal = m_pCANCommunicator->SendCOB(canmsg);
    }
    else
    {
        FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": configuration not available";
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the 'MechanicalOffsetRequest' CAN-Message
 */
/****************************************************************************/
ReturnCode_t CJoystick::SendCANMsgMechanicalOffsetReq()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDMechOffsetReq;
    canmsg.data[0] = 0; //
    canmsg.data[1] = 0;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the 'DisplacementRequest' CAN-Message
 */
/****************************************************************************/
ReturnCode_t CJoystick::SendCANMsgDisplacementReq()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDDisplacementReq;
    canmsg.data[0] = 0; //
    canmsg.data[1] = 0;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

ReturnCode_t CJoystick::ReqActInputValue()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if((m_TaskID == MODULE_TASKID_FREE) && (m_taskState == MODULE_CMD_STATE_FREE))
    {
        m_TaskID = MODULE_TASKID_COMMAND_HDL/*FM_MODULE_TASKID_REQ_ACTINP*/; //!< \todo
        m_taskState = MODULE_CMD_STATE_REQ;
        m_timeAction.Trigger();
        FILE_LOG_L(laFCT, llINFO) << "   CANJoystick::ReqInputValue ";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << "   CANJoystick::ReqActInputValue invalid state: " << (int) m_TaskID << ", " << (int) m_taskState;
    }

    return RetVal;
}

ReturnCode_t CJoystick::SetContactLimitValue(quint16 contactLimit)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if((m_TaskID == MODULE_TASKID_FREE) && (m_taskState == MODULE_CMD_STATE_FREE))
    {
        m_TaskID = MODULE_TASKID_COMMAND_HDL/*FM_MODULE_TASKID_SET_CONTACT_LIMIT*/; //!< \todo
        m_taskState = MODULE_CMD_STATE_REQ;
        m_contactLimit = contactLimit;
        m_timeAction.Trigger();
        FILE_LOG_L(laFCT, llINFO) << "   CANJoystick::SetContactLimitValue ";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << "   CANJoystick::SetContactLimitValue invalid state: " << (int) m_TaskID << ", " << (int) m_taskState;
    }

    return RetVal;

}

ReturnCode_t CJoystick::GetActInputValue(quint16& inputValue)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    inputValue = m_actInputValue;

    return RetVal;
}

void CJoystick::ResetTask()
{
    m_TaskID = MODULE_TASKID_FREE;
    m_taskState = MODULE_CMD_STATE_FREE;
}

} //namespace
