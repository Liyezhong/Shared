/****************************************************************************/
/*! \file StepperMotor.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CANStepperMotor
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

#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

#include <QtDebug>
#include <cmath>

//@{**************************************************************************
///! CAN message command classes
//****************************************************************************/
typedef enum {
    CMD_CLASS_SYSTEM   = 0,     //!< System commands
    CMD_CLASS_TRIGGER  = 1,     //!< Trigger commands
    CMD_CLASS_ERROR    = 2,     //!< Error/warning commands
    CMD_CLASS_UNUSED   = 3,     //!< Not used
    CMD_CLASS_FUNCTION = 4,     //!< Function commands
    CMD_CLASS_PROCESS  = 5,     //!< Process commands/data
    CMD_CLASS_SERVICE  = 6,     //!< Service commands
    CMD_CLASS_DIAGNOSE = 7      //!< Diagnosis mode
} CanCmdClass_t;

// Bitshift counts for subfields in CAN ID
#define CANiD_SHIFT_MASTER      0            //!< Master/slave bitmask
#define CANiD_SHIFT_NODETYPE    1            //!< Node type bitmask
#define CANiD_SHIFT_NODEINDEX   8            //!< Node index bitmask
#define CANiD_SHIFT_CHANNEL     12           //!< Channel bitshift count
#define CANiD_SHIFT_CMDCODE     19           //!< Command code bitmask
#define CANiD_SHIFT_CMDCLASS    26           //!< Command class bitmask
#define CANiD_SHIFT_BROADCAST   29           //!< Broadcast bit

#define BUILD_CAN_ID(Class,Code,MS) \
    (((quint32)(Class) << CANiD_SHIFT_CMDCLASS) | ((quint32)(Code) << CANiD_SHIFT_CMDCODE) | (MS))

namespace DeviceControl
{

///< definitions for configuration message's subindex
#define MSG_CONFIG_SUB_INDEX_LS1_1   1   ///< sub index configuration message 'limit switch 1 block 1'
#define MSG_CONFIG_SUB_INDEX_LS1_2   2   ///< sub index configuration message 'limit switch 1 block 2'
#define MSG_CONFIG_SUB_INDEX_LS2_1   3   ///< sub index configuration message 'limit switch 2 block 1'
#define MSG_CONFIG_SUB_INDEX_LS2_2   4   ///< sub index configuration message 'limit switch 2 block 2'
#define MSG_CONFIG_SUB_INDEX_ENC     5   ///< sub index configuration message 'encoder'
#define MSG_CONFIG_SUB_INDEX_MOTOR1  6   ///< sub index configuration message 'motor data 1'
#define MSG_CONFIG_SUB_INDEX_MOTOR2  7   ///< sub index configuration message 'motor data 2'
#define MSG_CONFIG_SUB_INDEX_MOTOR3  8   ///< sub index configuration message 'motor data 3'
#define MSG_CONFIG_SUB_INDEX_MOTOR4  9   ///< sub index configuration message 'motor data 4'
#define MSG_CONFIG_SUB_INDEX_MOTOR5 10   ///< sub index configuration message 'motor data 5'
#define MSG_CONFIG_SUB_INDEX_MOTOR6 11   ///< sub index configuration message 'motor data 6'
#define MSG_CONFIG_SUB_INDEX_MOTOR7 12   ///< sub index configuration message 'motor data 7'

#define SM_STATE_BITPOS_ENABLED    0     ///< Bit position 'MotorState Enabled'
#define SM_STATE_BITPOS_SKIPREFRUN 1     ///< Bit position 'MotorState SkipRefRun'

QMap<quint32, std::string> CStepperMotor::m_eventString;  // static string list with info strings for CAN events

/****************************************************************************/
/*!
 *  \brief    Constructor for the CANStepperMotor
 *
 *  \param    p_MessageConfiguration = Message configuration
 *  \param    pCANCommunicator  = pointer to communication interface
 *  \param    pParentNode       = pointer to CANNode, where this motor is assigned to
 *
 ****************************************************************************/
CStepperMotor::CStepperMotor(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                             CBaseModule* pParentNode) :
    CFunctionModule(CModuleConfig::CAN_OBJ_TYPE_STEPPERMOTOR, p_MessageConfiguration, pCANCommunicator, pParentNode),
    m_unCanIDError(0), m_unCanIDErrorReq(0),
    m_unCanIDStateSet(0), m_unCanIDStateSetAck(0), m_unCanIDStateReq(0), m_unCanIDState(0),
    m_unCanIDReferenceMovement(0), m_unCanIDReferenceMovementReqAckn(0),  m_unCanIDReferenceMovementAckn(0),
    m_unCanIDTargetPos(0), m_unCanIDTargetPosCmdAckn(0),
    m_unCanIDTargetSpeed(0), m_unCanIDTargetSpeedCmdAckn(0),
    m_unCanIDMovementAckn(0),
    m_unCanIDActPositionReq(0), m_unCanIDActPositionResp(0), m_unCanIDActSpeed(0), m_unCanIDActSpeedReq(0),
    m_unCanIDConfig(0), m_unCanIDMotionProfile(0),
    m_unCanIDLiveCycleDataReq(0), m_unCanIDLiveCycleData(0),
    m_unCanIDDiagSoftwareReq(0), m_unCanIDDiagSoftware(0), m_unCanIDDiagHardwareReq(0), m_unCanIDDiagHardware(0),
    m_unCanIDDebug(0), m_unCanIDDebug2(0),
    m_ReqTargetPosition(0), m_ReqTargetSpeed(0),
    m_ReqMovementProfile(0), m_ReqSubCommandID(0), m_ReqSubCommandData(0),
    m_aktionTimespan(0), MotionProfileIndex(0), MotionProfileSubIndex(0), m_MotorState(false),
    m_StepCounter(0), m_TripCounter(0), m_OperationTime(0),
    m_MinPosition(0), m_MaxPosition(0), m_MaxSpeed(0),
    m_bTestOutput(0), m_nCounter(0)
{
    // main state
    m_mainState = FM_MAIN_STATE_BOOTUP;
    // configuration state
    m_subStateConfig  = FM_SM_SUB_STATE_CONFIG_INIT;
    m_subIndex.type.profileData = false;
    m_subIndex.param.index      = LS1;

    //motor command  array initialisation
    for(quint8 idx = 0; idx < MAX_SM_MODULE_CMD_IDX; idx++)
    {
        m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
    }

    //quick and easy testing
    m_bTestOutput = false;
    m_nCounter = 0;

    // init event string list
    m_eventString[1]  = "E_SMOT_INVALID_STATE: a command was received while the motor's state is not able to execute it";
    m_eventString[2]  = "E_SMOT_CONFIG_INCOMPLETE: received configuration data is not complete";

    m_eventString[3]  = "E_SMOT_CONFIG_LIMITSWITCH: position code configuration doesn't match limit switch configuration";
    m_eventString[4]  = "E_SMOT_CONFIG_INVALID_DRIVER: configuration data for unknown stepper driver received";
    m_eventString[5]  = "E_SMOT_CONFIG_REFRUN: invalid reference run configuration";
    m_eventString[6]  = "E_SMOT_CONFIG_PROFILES: invalid amount of profiles";
    m_eventString[7]  = "E_SMOT_CONFIG_INVALID_PROFILE: profile data for given vMax invalid";
    m_eventString[8]  = "E_SMOT_CONFIG_INVALID_PROFILE_VMIN: profile data for vMin/microsteps not compatible to timer reolution";
    m_eventString[9]  = "E_SMOT_CONFIG_INVALID_PROFILE_VMAX: profile data for vMax/microsteps not compatible to interrupt timing";

    m_eventString[10] = "E_SMOT_REFRUN_TIMEOUT: during a reference run a timeout occured";
    m_eventString[11] = "E_SMOT_REFRUN_MAXDISTANCE: during a reference run the max distance was exceeded";
    m_eventString[12] = "E_SMOT_REFRUN_INVALID_LS: during a reference run the motor stopped at an unexpected limitswitch position";

    m_eventString[20] = "E_SMOT_INVALID_POSCODE: position code from limitswitches is invalid";
    m_eventString[21] = "E_SMOT_INVALID_POSCODE_POSITION: position code from limitswitches does not match with actual position";
    m_eventString[22] = "E_SMOT_INVALID_POSCODE_DIR: movement rotation direction from this position code not allowed";
    m_eventString[23] = "E_SMOT_STOP_BY_POSCODE: during a movement the motor stopped at an position code signal";
    m_eventString[24] = "E_SMOT_STOP_BY_OFFLIMIT: during a movement the motor stopped because actual position get off-limit";

    m_eventString[30] = "E_SMOT_MOTION_PROFILE_UNFIT: requested speed exceeds vMin/vMax of selected profile";
}


/****************************************************************************/
/*!
 *  \brief    Destructor of CANStepperMotor
 *
 ****************************************************************************/
CStepperMotor::~CStepperMotor()
{
}


/****************************************************************************/
/*!
 *  \brief    Initialize this function module
 *
 *   The CAN-IDs are read from the CAN-Message configuration class, and the CAN-IDs are composed
 *   Receiveable CAN-message are registered to the communication layer
 *   This function should be called from device control next after the instance was created
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::Initialize()
{
    ReturnCode_t RetVal;

    //read the CAN-messages assigned to the stepper motor
    RetVal = InitializeCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        //register the CAN-messages to communication layer
        RetVal = RegisterCANMessages();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_mainState = FM_MAIN_STATE_INIT;
            m_subStateConfig  = FM_SM_SUB_STATE_CONFIG_START;

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
ReturnCode_t CStepperMotor::InitializeCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 bIfaceID;
    const quint8 ModuleID = MODULE_ID_STEPPER;

    bIfaceID = m_pCANObjectConfig->m_sChannel;

    RetVal = InitializeEventCANMessages(ModuleID);

    quint32 nodeId  = (m_pParent->GetNodeID() & 0x00000F00);    // get node index
    nodeId |= ((m_pParent->GetNodeID() & 0x000000FF) << 1);     // add node type
    nodeId |= ((m_pCANObjectConfig->m_sChannel) << 12);         // and channel

    m_unCanIDConfig             = MSG_SMOT_CONFIG | nodeId;

    m_unCanIDMotionProfile      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "StepperMotorMotionProfile", bIfaceID, m_pParent->GetNodeID());

    m_unCanIDStateSet           = MSG_SMOT_SET_ENABLE | nodeId;
    m_unCanIDStateSetAck        = MSG_SMOT_SET_ENABLE_ACK | nodeId;

    m_unCanIDStateReq           = mp_MessageConfiguration->GetCANMessageID(ModuleID, "StepperMotorStateReq", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDState              = mp_MessageConfiguration->GetCANMessageID(ModuleID, "StepperMotorState", bIfaceID, m_pParent->GetNodeID());

    m_unCanIDReferenceMovement          = MSG_SMOT_REQ_REF_RUN | nodeId;
    m_unCanIDReferenceMovementReqAckn   = MSG_SMOT_REQ_REF_RUN_ACK | nodeId;
    m_unCanIDReferenceMovementAckn      = MSG_SMOT_REFERENCE_RUN_ACK | nodeId;

//    m_unCanIDTargetPos          = mp_MessageConfiguration->GetCANMessageID(ModuleID, "StepperMotorTargetPosition", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDTargetPos          = MSG_SMOT_TARGET_POS | nodeId;
    m_unCanIDTargetPosCmdAckn   = MSG_SMOT_TARGET_POS_ACK | nodeId;
    m_unCanIDTargetSpeed        = MSG_SMOT_TARGET_SPEED | nodeId;
    m_unCanIDTargetSpeedCmdAckn = MSG_SMOT_TARGET_SPEED_ACK | nodeId;
    m_unCanIDMovementAckn       = MSG_SMOT_MOVEMENT_ACK | nodeId;

    m_unCanIDActPositionReq    = MSG_SMOT_ACT_POS_REQ | nodeId;
    m_unCanIDActPositionResp   = MSG_SMOT_ACT_POS | nodeId;

    m_unCanIDActSpeedReq        = MSG_SMOT_ACT_SPEED_REQ | nodeId;
    m_unCanIDActSpeed           = MSG_SMOT_ACT_SPEED | nodeId;

    m_unCanIDLiveCycleDataReq   = mp_MessageConfiguration->GetCANMessageID(ModuleID, "StepperMotorLiveCycleDataReq", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDLiveCycleData      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "StepperMotorLiveCycleData", bIfaceID, m_pParent->GetNodeID());

    m_unCanIDDiagSoftwareReq   = mp_MessageConfiguration->GetCANMessageID(ModuleID, "StepperMotorDiagSoftwareReq", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDiagSoftware      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "StepperMotorDiagSoftware", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDiagHardwareReq   = mp_MessageConfiguration->GetCANMessageID(ModuleID, "StepperMotorDiagHardwareReq", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDiagHardware      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "StepperMotorDiagHardware", bIfaceID, m_pParent->GetNodeID());

    m_unCanIDDebug              = mp_MessageConfiguration->GetCANMessageID(ModuleID, "StepperMotorDebug", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDDebug2             = mp_MessageConfiguration->GetCANMessageID(ModuleID, "StepperMotorDebug2", bIfaceID, m_pParent->GetNodeID());

    FILE_LOG_L(laINIT, llDEBUG) << "  CAN-messages for fct-module:" << GetName().toStdString() << ",node id:" << std::hex << m_pParent->GetNodeID();
    FILE_LOG_L(laINIT, llDEBUG) << "   EventInfo             : 0x" << std::hex << m_unCanIDEventInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventWarning          : 0x" << std::hex << m_unCanIDEventWarning;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventError            : 0x" << std::hex << m_unCanIDEventError;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventFatalError       : 0x" << std::hex << m_unCanIDEventFatalError;
    FILE_LOG_L(laINIT, llDEBUG) << "   ConfigParameter       : 0x" << std::hex << m_unCanIDConfig;
    FILE_LOG_L(laINIT, llDEBUG) << "   DriveParameter        : 0x" << std::hex << m_unCanIDMotionProfile;
    FILE_LOG_L(laINIT, llDEBUG) << "   StateSet              : 0x" << std::hex << m_unCanIDStateSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   StateSetAck           : 0x" << std::hex << m_unCanIDStateSetAck;
    FILE_LOG_L(laINIT, llDEBUG) << "   StateReq              : 0x" << std::hex << m_unCanIDStateReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   State                 : 0x" << std::hex << m_unCanIDState;
    FILE_LOG_L(laINIT, llDEBUG) << "   ReferenceMovementReq  : 0x" << std::hex << m_unCanIDReferenceMovement;
    FILE_LOG_L(laINIT, llDEBUG) << "   ReferenceMovementReqAck  : 0x" << std::hex << m_unCanIDReferenceMovementReqAckn;
    FILE_LOG_L(laINIT, llDEBUG) << "   ReferenceMovementAckn : 0x" << std::hex << m_unCanIDReferenceMovementAckn;
    FILE_LOG_L(laINIT, llDEBUG) << "   TargetPosition        : 0x" << std::hex << m_unCanIDTargetPos;
    FILE_LOG_L(laINIT, llDEBUG) << "   TargetPositionCmdAckn : 0x" << std::hex << m_unCanIDTargetPosCmdAckn;
    FILE_LOG_L(laINIT, llDEBUG) << "   TargetSpeed           : 0x" << std::hex << m_unCanIDTargetSpeed;
    FILE_LOG_L(laINIT, llDEBUG) << "   TargetSpeedCmdAckn    : 0x" << std::hex << m_unCanIDTargetSpeedCmdAckn;
    FILE_LOG_L(laINIT, llDEBUG) << "   MovementAckn          : 0x" << std::hex << m_unCanIDMovementAckn;
    FILE_LOG_L(laINIT, llDEBUG) << "   ActPositionResp       : 0x" << std::hex << m_unCanIDActPositionResp;
    FILE_LOG_L(laINIT, llDEBUG) << "   ActPositionReq        : 0x" << std::hex << m_unCanIDActPositionReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   ActSpeed              : 0x" << std::hex << m_unCanIDActSpeed;
    FILE_LOG_L(laINIT, llDEBUG) << "   ActSpeedReq           : 0x" << std::hex << m_unCanIDActSpeedReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   LiveCycleDataReq      : 0x" << std::hex << m_unCanIDLiveCycleDataReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   LiveCycleData         : 0x" << std::hex << m_unCanIDLiveCycleData;
    FILE_LOG_L(laINIT, llDEBUG) << "   DiagSoftwareReq       : 0x" << std::hex << m_unCanIDDiagSoftwareReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   DiagSoftware          : 0x" << std::hex << m_unCanIDDiagSoftware;
    FILE_LOG_L(laINIT, llDEBUG) << "   DiagHardwareReq       : 0x" << std::hex << m_unCanIDDiagHardwareReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   DiagHardware          : 0x" << std::hex << m_unCanIDDiagHardware;
    FILE_LOG_L(laINIT, llDEBUG) << "   Debug                 : 0x" << std::hex << m_unCanIDDebug;
    FILE_LOG_L(laINIT, llDEBUG) << "   Debug2                : 0x" << std::hex << m_unCanIDDebug2;


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
ReturnCode_t CStepperMotor::RegisterCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    RetVal = RegisterEventCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDError, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDState, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDStateSetAck, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDReferenceMovementReqAckn, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDReferenceMovementAckn, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDTargetPosCmdAckn, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDTargetSpeedCmdAckn, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDMovementAckn, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDActPositionResp, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDActSpeed, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDLiveCycleData, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDDiagSoftware, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDDiagHardware, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDDebug, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDDebug2, this);
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
void CStepperMotor::HandleTasks()
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
        m_timeAction.Trigger();
    }
    else if(m_mainState == FM_MAIN_STATE_CONFIG)
    {
        // 50ms delay between each call of HandleConfigurationState()
        // ensures a proper CAN-message intervall
#if 1
        if(m_timeAction.Elapsed() > 5)
#else
        if(m_timeAction.Elapsed() > 50)
#endif
        {
            HandleConfigurationState();
            m_timeAction.Trigger();
        }
    }
    else if(m_mainState == FM_MAIN_STATE_ERROR)
    {
    }
}

/******************************************************************/
/*! \brief      Handles the Motors configuration state
 *
 *      During the configuration procedure, the stepper motors configuration data, subdivided to several CAN messages,
 *      will be send via CAN bus.
 *      This function fills the CAN messages date area with the configuration options for the stepper motor.

 *      The following parameters are added to the CAN messages:
 *      - DB[0]  = sub index, the information transmitted with this can message ID is subdivided into
 *                 several CAN messages. The first byte DB[0] is the sub index of the CAN message
 *                 Data bytes 2..7 will be evaluated as described below:
 *       SubIndex == MSG_CONFIG_SUB_INDEX_LS1_1/2_1: configuration limit switch 1/2
 *          DB[1]   = [0/1] 1 - Limit switch 1/2 exists physically
 *                          0 - Limit switch 1/2 doesn't exists physically
 *          DB[2]   = [0/1] The limit switch's orientation
 *                        0 - CW motor axis rotation will move the axis closer to limit switch 1/2
 *                        1 - CCW motor axis rotation will move the axis closer to limit switch 1/2
 *          DB[3]   = The polarity of limit switch 1/2
 *          DB[4]   = Sample rate if the assigned digital input port
 *          DB[5]   = Debounce counter of the assigned digital input port
 *          DB[6,7] = reserved
 *
 *       SubIndex == MSG_CONFIG_SUB_INDEX_LS1_2/2_2: configuration limit switch 1/2, datablock 2
 *          DB[1,2] = limit switch position at rotatorically axis
 *          DB[3,4] = limit switch active with at rotatorically axis
 *          DB[5,6] = Revision halfstep count
 *          DB[7]   = reserved
 *
 *       SubIndex == MSG_CONFIG_SUB_INDEX_ENC: configuration encoder
 *          DB[1]    = [0/1] 1 - Encoder exists physically
 *                           0 - Encoder doesn't exists physically
 *          DB[2,3]  = The encoder's resolution, the number of tick of a revolution
 *          DB[4]    = The encoder's orientation
 *                        0 - CW motor axis rotation will increase the encoder ticks
 *                        1 - CW motor axis rotation will decrease the encoder ticks
 *          DB[5..7] = reserved
 *
 *       SubIndex == MSG_CONFIG_SUB_INDEX_MOTOR1: motor configuration data block 1
 *          DB[1,2] = The motor's step resolution, the number of steps per revolution
 *          DB[3] = Direction of rotation
 *                        0 - Increasing step counter results in CW motor axis rotation
 *                        1 - Increasing step counter results in CCW motor axis rotation
 *          DB[4..5] = Motor minimum speed in half-steps/sec (Int16)
 *          DB[6] = rotation type (linear axis, rotatorically axis)
 *          DB[7] = reserved
 *
 *       SubIndex == MSG_CONFIG_SUB_INDEX_MOTOR2: motor configuration data block 2
 *          DB[1..4] = Motor minimum position in half steps (Int32)
 *          DB[5,6]  = number of steps for one revolution of rotator. axis (incl. mechanic ratio)
 *          DB[7]    = reserved
 *       SubIndex == MSG_CONFIG_SUB_INDEX_MOTOR3: motor configuration data block 3
 *          DB[1..4] = Motor maximum position in half steps (Int32)
 *          DB[5]    = Direction of rotation at reference run
 *                        0 - CW motor axis rotation at reference run
 *                        1 - CCW motor axis rotation at reference run
 *          DB[6,7] = Motor slow speed at reference run
 *
 *       SubIndex == MSG_CONFIG_SUB_INDEX_MOTOR4: motor configuration data block 4
 *          DB[1..4] = Maximum movement of half steps at reference run (Int32)
 *          DB[5,6]  = Motor high speed at reference run
 *                        0 - CW motor axis rotation at reference run
 *                        1 - CCW motor axis rotation at reference run
 *          DB[7] = reserved
 */
 /******************************************************************/

StepperMotorRotDir_t CStepperMotor::SetupRotationDir (CANFctModuleStepperMotor::RotationDir_t dir)
{
    StepperMotorRotDir_t rotDir = SMOT_ROTDIR_CW;

    switch (dir)
    {
    case CANFctModuleStepperMotor::ROTATION_DIR_CW:
        rotDir = SMOT_ROTDIR_CW;
        break;
    case CANFctModuleStepperMotor::ROTATION_DIR_CCW:
        rotDir = SMOT_ROTDIR_CCW;
        break;
    default:
        Q_ASSERT(false);
    }
    return rotDir;
}

/****************************************************************************/
/*!
 *  \brief    Configure limit switch data.
 *
 *  \iparam   LimitSwitch = Data to configure the limit switch
 *  \iparam   ls = configuration data

 *  \return   void
 */
/****************************************************************************/
void CStepperMotor::SetupLimitSwitchConfigData (CANFctModuleLimitSwitch &LimitSwitch, ConfigData_LS_t &ls)
{
    ls.flag.exist = LimitSwitch.bExists;
    ls.flag.polarity = LimitSwitch.bPolarity;   //limit switch logic level
    ls.flag.reserved = 0;
    ls.sampleRate = LimitSwitch.bSampleRate;
    ls.debounceCount = LimitSwitch.bDebounce;
}

/****************************************************************************/
/*!
 *  \brief    Configure position code.
 *
 *  \iparam   PosCode = Data to configure the position code
 *  \iparam   pc = position code data

 *  \return   void
 */
/****************************************************************************/
void CStepperMotor::SetupPosCodeConfigData (CANFctModulePosCode &PosCode, ConfigData_LSPOS_t &pc)
{
    pc.flag.valid   = PosCode.bValid;
    pc.flag.stop    = PosCode.bStop;
    pc.flag.stopDir = SetupRotationDir(CANFctModuleStepperMotor::RotationDir_t(PosCode.bStopDir));  // stop direction
#ifdef PRE_ALFA_TEST
    pc.flag.rotDirCheck = PosCode.bRotDirCheck;
    pc.flag.hitSkip     = PosCode.hitSkip;
#endif
    pc.position     = ValToDB4(PosCode.position);
    pc.width        = PosCode.width;
    pc.deviation    = PosCode.deviation;
}


void CStepperMotor::HandleConfigurationState()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    CANFctModuleStepperMotor* pCANObjConfMotor;
    pCANObjConfMotor = (CANFctModuleStepperMotor*) m_pCANObjectConfig;
    can_frame canmsg;
    ConfigData_Param_t &configData = *(ConfigData_Param_t*)canmsg.data;

    //the following parameters must be send to the motor:
    // limit switch: index, orientation, logic level, interval, debounce
    // encoder: type, resolution, direction of rotation
    // motor: direction of rotation, steps per revolution

    canmsg.can_id = m_unCanIDConfig;
    configData.index = m_subIndex;      // message sub index

    if (0 == m_subIndex.type.profileData)
    {
        switch (m_subIndex.param.index)
        {
        case LS1:
            canmsg.can_dlc = sizeof(ConfigData_LS_t) + sizeof(SubIndex_t);
            SetupLimitSwitchConfigData(pCANObjConfMotor->LimitSwitch1, configData.part.ls1);
            break;
        case LS2:
            canmsg.can_dlc = sizeof(ConfigData_LS_t) + sizeof(SubIndex_t);
            SetupLimitSwitchConfigData(pCANObjConfMotor->LimitSwitch2, configData.part.ls2);
            break;
        case POS1:
            canmsg.can_dlc = sizeof(ConfigData_LSPOS_t) + sizeof(SubIndex_t);
            SetupPosCodeConfigData(pCANObjConfMotor->PosCode1, configData.part.pos1);
            break;
        case POS2:
            canmsg.can_dlc = sizeof(ConfigData_LSPOS_t) + sizeof(SubIndex_t);
            SetupPosCodeConfigData(pCANObjConfMotor->PosCode2, configData.part.pos2);
            break;
        case POS3:
            canmsg.can_dlc = sizeof(ConfigData_LSPOS_t) + sizeof(SubIndex_t);
            SetupPosCodeConfigData(pCANObjConfMotor->PosCode3, configData.part.pos3);
            break;
        case ENC:
            {
                ConfigData_ENC_t &enc = configData.part.enc;
                canmsg.can_dlc = sizeof(ConfigData_ENC_t) + sizeof(SubIndex_t);

                enc.flag.exist = (0 != pCANObjConfMotor->bEncoderType);
                if (enc.flag.exist)
                {
                    enc.flag.rotDir = SetupRotationDir(pCANObjConfMotor->bEncoderDir);
                    enc.resolution = ValToDB2(pCANObjConfMotor->sEncoderResolution);
                }
                else
                {
                    enc.flag.rotDir = SMOT_ROTDIR_CW;
                    enc.resolution = ValToDB2(0);
                }
            }
            break;
        case MOT1:
            {
                ConfigData_MOT_P1_t &mot1 = configData.part.mot1;
                canmsg.can_dlc = sizeof(ConfigData_MOT_P1_t) + sizeof(SubIndex_t);

                mot1.resolution = ValToDB2(pCANObjConfMotor->sResolution);
                mot1.run_CurrentScale = pCANObjConfMotor->runCurrentScale;
                mot1.stop_CurrentScale = pCANObjConfMotor->stopCurrentScale;
                mot1.stopCurrent_Delay = ValToDB2(pCANObjConfMotor->stopCurrentDelay);

                switch (pCANObjConfMotor->driverType)
                {
                case CANFctModuleStepperMotor::DRIVER_TMC26X:
                    mot1.driverType = SMOT_DRIVER_TMC26X;
                    break;
                default:
                    mot1.driverType = SMOT_DRIVER_DEFAULT;
                    break;
                }
            }
            break;
        case MOT2:
            {
                ConfigData_MOT_P2_t &mot2 = configData.part.mot2;
                canmsg.can_dlc = sizeof(ConfigData_MOT_P2_t) + sizeof(SubIndex_t);

//                switch (pCANObjConfMotor->rotationType)
//                {
//                case CANFctModuleStepperMotor::ROTATION_TYPE_ROT:
//                    mot2.flag.rotType = ROTATION_TYPE_ROT;
//                    break;
//                case CANFctModuleStepperMotor::ROTATION_TYPE_LINEAR:
//                    mot2.flag.rotType = ROTATION_TYPE_LINEAR;
//                    break;
//                default:
//                    Q_ASSERT(false);
//                    //return ErrConfigData;
//                }

                mot2.flag.rotDir = SetupRotationDir(pCANObjConfMotor->bDirection);
                mot2.minPos = ValToDB4(pCANObjConfMotor->lMinPosition);
                mot2.resetPos = ValToDB2(pCANObjConfMotor->sResetPosition);

                quint8 motionProfCount = pCANObjConfMotor->listMotionProfiles.count();
                if ((motionProfCount < 1) || (motionProfCount > MAX_MOTION_PROFIL))
                    //return ErrConfigData;
                    Q_ASSERT(false);
                mot2.flag.motionProf_cnt = motionProfCount;

                m_MinPosition = pCANObjConfMotor->lMinPosition;
                m_MaxSpeed = pCANObjConfMotor->sMaxSpeed;
            }
            break;
        case MOT3:
            {
                ConfigData_MOT_P3_t &mot3 = configData.part.mot3;
                canmsg.can_dlc = sizeof(ConfigData_MOT_P3_t) + sizeof(SubIndex_t);

                mot3.maxPos = ValToDB4(pCANObjConfMotor->lMaxPosition);

                m_MaxPosition = pCANObjConfMotor->lMaxPosition;
            }
            break;

        case REFRUN1:
            {
                ConfigData_REFRUN_P1_t &refrun = configData.part.refRun1;
                canmsg.can_dlc = sizeof(ConfigData_REFRUN_P1_t) + sizeof(SubIndex_t);

                refrun.refRun_RefPos = pCANObjConfMotor->refRunRefPos;
                refrun.refRun_PosOffset = ValToDB4(pCANObjConfMotor->lRefPosOffset);
                refrun.refRun_Timeout = ValToDB2(pCANObjConfMotor->sRefRunTimeout);
            }
            break;
        case REFRUN2:
            {
                ConfigData_REFRUN_P2_t &refrun = configData.part.refRun2;
                canmsg.can_dlc = sizeof(ConfigData_REFRUN_P2_t) + sizeof(SubIndex_t);

                refrun.refRun_MaxDist = ValToDB4(pCANObjConfMotor->lRefRunMaxDistance);
                refrun.refRun_HighSpeed = ValToDB2(pCANObjConfMotor->sRefRunHighSpeed);
            }
            break;
        case REFRUN3:
            {
                ConfigData_REFRUN_P3_t &refrun = configData.part.refRun3;
                canmsg.can_dlc = sizeof(ConfigData_REFRUN_P3_t) + sizeof(SubIndex_t);

                refrun.refRun_ReverseDist = ValToDB4(pCANObjConfMotor->lRefRunReverseDistance);
                refrun.refRun_LowSpeed = ValToDB2(pCANObjConfMotor->sRefRunSlowSpeed);
            }
            break;
/*
        case MOT7:
            {
                ConfigData_MOT_P7_t &mot7 = configData.part.mot7;
                canmsg.can_dlc = sizeof(ConfigData_MOT_P7_t) + sizeof(SubIndex_t);

                mot7.stepLoss_WarnLimit = ValToDB2(pCANObjConfMotor->sStepLossWarnLimit);
                mot7.stepLoss_ErrLimit = ValToDB2(pCANObjConfMotor->sStepLossWarnLimit);
                mot7.currentLimit = ValToDB2(pCANObjConfMotor->sCurrentLimit);
            }
            break;
*/
        case TMC26x_DRVCONF:
            {
                ConfigData_DRV_TMC26X_t &tmc26x = configData.part.drvTMC26x;
                canmsg.can_dlc = sizeof(ConfigData_DRV_TMC26X_t) + sizeof(SubIndex_t);

                tmc26x.regVal = ValToDB3(pCANObjConfMotor->tmc26x.drvConf);
            }
            break;
        case TMC26x_SGCSCONF:
            {
                ConfigData_DRV_TMC26X_t &tmc26x = configData.part.drvTMC26x;
                canmsg.can_dlc = sizeof(ConfigData_DRV_TMC26X_t) + sizeof(SubIndex_t);

                tmc26x.regVal = ValToDB3(pCANObjConfMotor->tmc26x.sgcsConf);
            }
            break;
        case TMC26x_SMARTEN:
            {
                ConfigData_DRV_TMC26X_t &tmc26x = configData.part.drvTMC26x;
                canmsg.can_dlc = sizeof(ConfigData_DRV_TMC26X_t) + sizeof(SubIndex_t);

                tmc26x.regVal = ValToDB3(pCANObjConfMotor->tmc26x.smartEn);
            }
            break;
        case TMC26x_CHOPCONF:
            {
                ConfigData_DRV_TMC26X_t &tmc26x = configData.part.drvTMC26x;
                canmsg.can_dlc = sizeof(ConfigData_DRV_TMC26X_t) + sizeof(SubIndex_t);

                tmc26x.regVal = ValToDB3(pCANObjConfMotor->tmc26x.chopConf);
            }
            break;
        default:
            Q_ASSERT(false);
            break;
        }

        switch (pCANObjConfMotor->driverType)
        {
        case SMOT_DRIVER_TMC26X:
            switch (m_subIndex.param.index)
            {
            case REFRUN3:
                m_subIndex.param.index = TMC26x_DRVCONF;
                break;
            case TMC26x_CHOPCONF:
                m_subIndex.type.profileData = 1;
                m_subIndex.profile.index = P1;
                m_subIndex.profile.no = 0;
                break;
            default:
                m_subIndex.param.index++;
                break;
            }
            break;
        default:
            if (m_subIndex.param.index < REFRUN3)
                m_subIndex.param.index++;
            else
            {
                m_subIndex.type.profileData = 1;
                m_subIndex.profile.index = P1;
                m_subIndex.profile.no = 0;
            }
            break;
        }
    }

    else
    {
        QMap<short, CANFctModuleMotionProfile>::const_iterator iter;
        quint8 idx = 0;

        FILE_LOG_L(laFCT, llINFO) << "CANStepperMotor " << GetName().toStdString() << " : MotionProfile " << (int) MotionProfileIndex << " - " << (int) MotionProfileSubIndex;

        //position the iterator to the motion profile whichs data should be transmitted
        iter = pCANObjConfMotor->listMotionProfiles.constBegin();
        for(idx = 0; idx < m_subIndex.profile.no; idx++)
        {
            iter++;
        }

        if(iter == pCANObjConfMotor->listMotionProfiles.constEnd())
        {
            FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ":  " << (int) MotionProfileIndex << " motionprofils sent";
            m_subStateConfig = FM_SM_SUB_STATE_CONFIG_FINISHED;
        }
        else
        {
            const CANFctModuleMotionProfile &MotionProfile = iter.value();
            switch (m_subIndex.profile.index)
            {
            case P1:
                {
                    ProfileData_P1_t &prof1 = configData.part.prof1;
                    canmsg.can_dlc = sizeof(ProfileData_P1_t) + sizeof(SubIndex_t);

                    prof1.microsteps = MotionProfile.bMicroSteps;
//                    prof2.rampType = pMotionProfile->bRampType;
                    prof1.minSpeed = ValToDB2(MotionProfile.sSpeedMin);
                    prof1.targetSpeed  = ValToDB2(MotionProfile.sSpeedMax);
                }
                break;
            case P2:
                {
                    ProfileData_P2_t &prof2 = configData.part.prof2;
                    canmsg.can_dlc = sizeof(ProfileData_P2_t) + sizeof(SubIndex_t);

                    prof2.acceleration = ValToDB2(MotionProfile.sAcc);
                    prof2.accJerkTime = ValToDB2(MotionProfile.sAccTime);
                }
                break;
            case P3:
                {
                    ProfileData_P3_t &prof3 = configData.part.prof3;
                    canmsg.can_dlc = sizeof(ProfileData_P3_t) + sizeof(SubIndex_t);

                    prof3.deceleration = ValToDB2(MotionProfile.sDec);
                    prof3.decJerkTime = ValToDB2(MotionProfile.sDecTime);
                }
                break;
            default:
                Q_ASSERT(false);
            }

            //prepare for next profile
            if (m_subIndex.profile.index < P3)
                m_subIndex.profile.index++;
            else
            {
                m_subIndex.profile.index = P1;
                m_subIndex.profile.no++;
            }
        }
    }


    if(m_subStateConfig != FM_SM_SUB_STATE_CONFIG_FINISHED)
        retval = m_pCANCommunicator->SendCOB(canmsg);
    else
    {
        m_subIndex.type.profileData = false;
        m_subIndex.param.index      = LS1;
        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_INIT; // prepare state and index to run configuration again

        emit ReportConfigureDone(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS);

        m_mainState = FM_MAIN_STATE_IDLE;
        m_TaskID = MODULE_TASKID_FREE;
        FILE_LOG_L(laCONFIG, llDEBUG) << " Module " << GetName().toStdString() << ": change to FM_MAIN_STATE_IDLE";
    }

    if(retval != DCL_ERR_FCT_CALL_SUCCESS)
    {
        FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCOB returns" << (int) retval;
        m_mainState = FM_MAIN_STATE_ERROR;

        emit ReportConfigureDone(GetModuleHandle(), retval);
    }
}




//void CStepperMotor::HandleConfigurationState()
//{
//    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
//    CANFctModuleStepperMotor* pCANObjConfMotor;
//    pCANObjConfMotor = (CANFctModuleStepperMotor*) m_pCANObjectConfig;
//    can_frame canmsg;

//    //the following parameters must be send to the motor:
//    // limit switch: index, orientation, logic level, interval, debounce
//    // encoder: type, resolution, direction of rotation
//    // motor: direction of rotation, steps per revolution
//    if(FM_SM_SUB_STATE_CONFIG_START == m_subStateConfig)
//    {
//        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_LS1_1;
//        StartTimeDelay();
//    }

//    if(FM_SM_SUB_STATE_CONFIG_LS1_1 == m_subStateConfig)
//    {
//        FILE_LOG_L(laCONFIG, llDEBUG) << "  " << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDConfig << " limit switch 1 block 1";
//        canmsg.can_id = m_unCanIDConfig;
//        canmsg.data[0] = MSG_CONFIG_SUB_INDEX_LS1_1; // message sub index: data limit switch 0
//        if(pCANObjConfMotor->pLimitSwitch1 != 0)
//        {
//            canmsg.data[1] = pCANObjConfMotor->pLimitSwitch1->bExists;
//            canmsg.data[2] = pCANObjConfMotor->pLimitSwitch1->bOrientation; //limit switch orientation
//            canmsg.data[3] = pCANObjConfMotor->pLimitSwitch1->bPolarity ; //limit switch logic level
//            canmsg.data[4] = pCANObjConfMotor->pLimitSwitch1->bSampleRate;
//            canmsg.data[5] = pCANObjConfMotor->pLimitSwitch1->bDebounce;
//            canmsg.data[6] = 0;
//            canmsg.data[7] = 0;
//        }
//        else
//        {
//            canmsg.data[1] = 0; // code: no limit switch
//        }
//        canmsg.can_dlc = 8;
//        retval = m_pCANCommunicator->SendCOB(canmsg);

//        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_LS1_2;
//    }
//    else if(FM_SM_SUB_STATE_CONFIG_LS1_2 == m_subStateConfig)
//    {
//        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDConfig << " limit switch 1 block 2";;
//        canmsg.can_id = m_unCanIDConfig;
//        canmsg.data[0] = MSG_CONFIG_SUB_INDEX_LS1_2; // message sub index: data block 2 limit switch 1
//        if(pCANObjConfMotor->pLimitSwitch1 != 0)
//        {
//            SetCANMsgDataS16(&canmsg, pCANObjConfMotor->pLimitSwitch1->Position, 1);  //the limit switch position when driving forward (in 0.05 degrees)
//            SetCANMsgDataS16(&canmsg, pCANObjConfMotor->pLimitSwitch1->PositionWidth, 3);  //effectiv active lentgh (in 0.05 degrees)
//            canmsg.data[5] = 0;
//            canmsg.data[6] = 0;
//            canmsg.data[7] = 0;
//        }
//        else
//        {
//            canmsg.data[1] = 0; // code: no limit switch
//        }
//        canmsg.can_dlc = 8;
//        retval = m_pCANCommunicator->SendCOB(canmsg);

//        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_LS2_1;
//    }
//    else if(FM_SM_SUB_STATE_CONFIG_LS2_1 == m_subStateConfig)
//    {
//        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDConfig << " limit switch 2 block 1";
//        canmsg.can_id = m_unCanIDConfig;
//        canmsg.data[0] = MSG_CONFIG_SUB_INDEX_LS2_1; // message sub index: data limit switch 1
//        if(pCANObjConfMotor->pLimitSwitch2 != 0)
//        {
//            canmsg.data[1] = pCANObjConfMotor->pLimitSwitch2->bExists;
//            canmsg.data[2] = pCANObjConfMotor->pLimitSwitch2->bOrientation; //limit switch orientation
//            canmsg.data[3] = pCANObjConfMotor->pLimitSwitch2->bPolarity ; //limit switch logic level
//            canmsg.data[4] = pCANObjConfMotor->pLimitSwitch2->bSampleRate;
//            canmsg.data[5] = pCANObjConfMotor->pLimitSwitch2->bDebounce;
//            canmsg.data[6] = 0;
//            canmsg.data[7] = 0;
//        }
//        else
//        {
//            canmsg.data[1] = 0; // code: no limit switch
//        }
//        canmsg.can_dlc = 8;
//        retval = m_pCANCommunicator->SendCOB(canmsg);

//        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_LS2_2;
//    }
//    else if(FM_SM_SUB_STATE_CONFIG_LS2_2 == m_subStateConfig)
//    {
//        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDConfig << " limit switch 2 block 2";
//        canmsg.can_id = m_unCanIDConfig;
//        canmsg.data[0] = MSG_CONFIG_SUB_INDEX_LS2_2; // message sub index: data block 2 limit switch 2
//        if(pCANObjConfMotor->pLimitSwitch2 != 0)
//        {
//            SetCANMsgDataS16(&canmsg, pCANObjConfMotor->pLimitSwitch2->Position, 1);  //the limit switch position when driving forward (in 0.05 degrees)
//            SetCANMsgDataS16(&canmsg, pCANObjConfMotor->pLimitSwitch2->PositionWidth, 3);  //effectiv active lentgh (in 0.05 degrees)
//            canmsg.data[5] = 0;
//            canmsg.data[6] = 0;
//            canmsg.data[7] = 0;
//        }
//        else
//        {
//            canmsg.data[1] = 0; // code: no limit switch
//        }
//        canmsg.can_dlc = 8;
//        retval = m_pCANCommunicator->SendCOB(canmsg);

//        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_ENC;
//    }
//    else if(FM_SM_SUB_STATE_CONFIG_ENC == m_subStateConfig)
//    {
//        FILE_LOG_L(laCONFIG, llDEBUG)  << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDConfig;
//        canmsg.can_id = m_unCanIDConfig;
//        canmsg.data[0] = MSG_CONFIG_SUB_INDEX_ENC; // message sub index: encoder data
//        if(pCANObjConfMotor->bEncoderType == 1) /// \todo
//        {
//            canmsg.data[1] = pCANObjConfMotor->bEncoderType;
//            SetCANMsgDataU16(&canmsg, pCANObjConfMotor->sEncoderResolution, 2);
//            canmsg.data[4] = (quint8) pCANObjConfMotor->bEncoderDir; //
//        }
//        else
//        {
//            canmsg.data[1] = 0; // no encoder
//            canmsg.data[2] = 0;
//            canmsg.data[3] = 0;
//            canmsg.data[4] = 0;
//        }
//        canmsg.data[5] = 0;
//        canmsg.data[6] = 0;
//        canmsg.data[7] = 0;
//        canmsg.can_dlc = 8;
//        retval = m_pCANCommunicator->SendCOB(canmsg);

//        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_MOTOR_1;
//    }
//    else if(m_subStateConfig == FM_SM_SUB_STATE_CONFIG_MOTOR_1)
//    {
//        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDConfig;
//        canmsg.can_id = m_unCanIDConfig;
//        canmsg.data[0] = MSG_CONFIG_SUB_INDEX_MOTOR1; // message sub index: motor data
//        SetCANMsgDataU16(&canmsg, pCANObjConfMotor->sResolution, 1);
//        canmsg.data[3] = (quint8) pCANObjConfMotor->bDirection; //rotation direction
//        SetCANMsgDataS16(&canmsg, pCANObjConfMotor->sMinSpeed, 4);  // min speed
//        if(pCANObjConfMotor->rotationType == CANFctModuleStepperMotor::ROTATION_TYPE_LINEAR)
//        {
//            canmsg.data[6] = 1;
//        }
//        else if(pCANObjConfMotor->rotationType == CANFctModuleStepperMotor::ROTATION_TYPE_ROT)
//        {
//            canmsg.data[6] = 2;
//        }
//        else
//        {
//            canmsg.data[6] = 0;
//        }
//        canmsg.data[7] = 0;
//        canmsg.can_dlc = 8;

//        FILE_LOG_L(laFCT, llDEBUG4) << "CANStepperMotor " << GetName().toStdString() << " :sendCfg min: " << pCANObjConfMotor->lMinPosition << " data:" <<
//                (int) canmsg.data[4] << "," << (int) canmsg.data[5] << "," <<
//                (int) canmsg.data[6] << "," << (int) canmsg.data[7] << " time: " << m_timeAction.Elapsed();

//        retval = m_pCANCommunicator->SendCOB(canmsg);

//        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_MOTOR_2;
//    }
//    else if(FM_SM_SUB_STATE_CONFIG_MOTOR_2 == m_subStateConfig)
//    {
//        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDConfig;
//        canmsg.can_id = m_unCanIDConfig;
//        canmsg.data[0] = MSG_CONFIG_SUB_INDEX_MOTOR2; // message sub index: motor data
//        SetCANMsgDataS32(&canmsg, pCANObjConfMotor->lMinPosition, 1);
//        m_MinPosition = pCANObjConfMotor->lMinPosition;

//        //SetCANMsgDataS16(&canmsg, pCANObjConfMotor->sMaxSpeed, 5);
//        SetCANMsgDataS16(&canmsg, pCANObjConfMotor->sRevHStepCount, 5);
//        m_MaxSpeed = pCANObjConfMotor->sMaxSpeed;
//        canmsg.data[7] = 0;
//        canmsg.can_dlc = 8;

//        FILE_LOG_L(laFCT, llINFO) << "CANStepperMotor " << GetName().toStdString() << " : sendCfg min: " << pCANObjConfMotor->lMinPosition << " data:" <<
//                (int) canmsg.data[1] << "," << (int) canmsg.data[2] << "," <<
//                (int) canmsg.data[3] << "," << (int) canmsg.data[4] << " time: " << m_timeAction.Elapsed();

//        retval = m_pCANCommunicator->SendCOB(canmsg);

//        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_MOTOR_3;
//    }
//    else if(FM_SM_SUB_STATE_CONFIG_MOTOR_3 == m_subStateConfig)
//    {
//        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDConfig;
//        canmsg.can_id = m_unCanIDConfig;
//        canmsg.data[0] = MSG_CONFIG_SUB_INDEX_MOTOR3; // message sub index: motor data

//        SetCANMsgDataS32(&canmsg, pCANObjConfMotor->lMaxPosition, 1); // maximum position
//        m_MaxPosition = pCANObjConfMotor->lMaxPosition;

//        canmsg.data[5] = (quint8) pCANObjConfMotor->bRefRunDirection;
//        SetCANMsgDataS16(&canmsg, pCANObjConfMotor->sRefRunSlowSpeed, 6);  // min speed
//        canmsg.can_dlc = 8;

//        FILE_LOG_L(laFCT, llINFO) << "CANStepperMotor " << GetName().toStdString() << " : sendCfg max: " << pCANObjConfMotor->lMaxPosition << " data:" <<
//                (int) canmsg.data[1] << "," << (int) canmsg.data[2] << "," <<
//                (int) canmsg.data[3] << "," << (int) canmsg.data[4] << " time: " << m_timeAction.Elapsed();

//        retval = m_pCANCommunicator->SendCOB(canmsg);

//        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_MOTOR_4;
//    }
//    else if(FM_SM_SUB_STATE_CONFIG_MOTOR_4 == m_subStateConfig)
//    {
//        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDConfig;
//        canmsg.can_id = m_unCanIDConfig;
//        canmsg.data[0] = MSG_CONFIG_SUB_INDEX_MOTOR4; // message sub index: motor data

//        SetCANMsgDataS32(&canmsg, pCANObjConfMotor->lRefRunMaxDistance, 1);
//        SetCANMsgDataS16(&canmsg, pCANObjConfMotor->sRefRunHighSpeed, 5);

//        canmsg.data[7] = 0;
//        canmsg.can_dlc = 8;

//        FILE_LOG_L(laFCT, llINFO) << "CANStepperMotor " << GetName().toStdString() << " : sendCfg refdist: " << pCANObjConfMotor->lRefRunMaxDistance << " data:" <<
//                (int) canmsg.data[1] << "," << (int) canmsg.data[2] << "," <<
//                (int) canmsg.data[3] << "," << (int) canmsg.data[4] << " time: " << m_timeAction.Elapsed();

//        retval = m_pCANCommunicator->SendCOB(canmsg);

//        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_MOTOR_5;
//    }
//    else if(FM_SM_SUB_STATE_CONFIG_MOTOR_5 == m_subStateConfig)
//    {
//        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDConfig;
//        canmsg.can_id = m_unCanIDConfig;
//        canmsg.data[0] = MSG_CONFIG_SUB_INDEX_MOTOR5; // message sub index: motor data

//        SetCANMsgDataS32(&canmsg, pCANObjConfMotor->lRefRunReverseDistance, 1);
//        canmsg.data[5] = 0;
//        canmsg.data[6] = 0;
//        canmsg.data[7] = 0;
//        canmsg.can_dlc = 8;

//        FILE_LOG_L(laFCT, llINFO) << "CANStepperMotor " << GetName().toStdString() << " : sendCfg refReverseDist: " << pCANObjConfMotor->lRefRunReverseDistance << " data:" <<
//                (int) canmsg.data[1] << "," << (int) canmsg.data[2] << "," <<
//                (int) canmsg.data[3] << "," << (int) canmsg.data[4] <<
//                " motionProfiles: " << (int) canmsg.data[5] << " time: " << m_timeAction.Elapsed();

//        retval = m_pCANCommunicator->SendCOB(canmsg);

//        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_MOTOR_6;
//    }
//    else if(FM_SM_SUB_STATE_CONFIG_MOTOR_6 == m_subStateConfig)
//    {
//        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDConfig;
//        canmsg.can_id = m_unCanIDConfig;
//        canmsg.data[0] = MSG_CONFIG_SUB_INDEX_MOTOR6; // message sub index: motor data

//        SetCANMsgDataS32(&canmsg, pCANObjConfMotor->lRefPosOffset, 1);
//        canmsg.data[5] = (quint8) pCANObjConfMotor->listMotionProfiles.count();
//        canmsg.data[6] = 0;
//        canmsg.data[7] = 0;
//        canmsg.can_dlc = 8;

//        FILE_LOG_L(laFCT, llINFO) << "CANStepperMotor " << GetName().toStdString() << " : sendCfg refOffset: " << pCANObjConfMotor->lRefPosOffset << " data:" <<
//                (int) canmsg.data[1] << "," << (int) canmsg.data[2] << "," <<
//                (int) canmsg.data[3] << "," << (int) canmsg.data[4] << " time: " << m_timeAction.Elapsed();

//        retval = m_pCANCommunicator->SendCOB(canmsg);

//        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_MOTOR_7;
//    }
//    else if(FM_SM_SUB_STATE_CONFIG_MOTOR_7 == m_subStateConfig)
//    {
//        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDConfig;
//        canmsg.can_id = m_unCanIDConfig;
//        canmsg.data[0] = MSG_CONFIG_SUB_INDEX_MOTOR7; // message sub index: motor data
//        SetCANMsgDataU16(&canmsg, pCANObjConfMotor->sStepLossWarnLimit, 1);
//        SetCANMsgDataU16(&canmsg, pCANObjConfMotor->sStepLossErrorLimit, 3);
//        SetCANMsgDataU16(&canmsg, pCANObjConfMotor->sCurrentLimit, 5);
//        canmsg.data[7] = 1;  //Hardware simulation flag
//        canmsg.can_dlc = 8;

//        FILE_LOG_L(laFCT, llINFO) << "CANStepperMotor " << GetName().toStdString() << " : sendCfg stepLoss: " << pCANObjConfMotor->sStepLossWarnLimit << " data:" <<
//                (int) canmsg.data[1] << "," << (int) canmsg.data[2] << "," <<
//                (int) canmsg.data[3] << "," << (int) canmsg.data[4] << " time: " << m_timeAction.Elapsed();

//        retval = m_pCANCommunicator->SendCOB(canmsg);

//        m_subStateConfig = FM_SM_SUB_STATE_CONFIG_MOTOR_PROFILE;
//        MotionProfileIndex = 0;
//        MotionProfileSubIndex = 1;  //starts with 1!

//        //activate test output at oven motor
//        //if(GetModuleHandle() == 0x00010003)
//        //    m_bTestOutput = true;

//    }
//    else if(FM_SM_SUB_STATE_CONFIG_MOTOR_PROFILE == m_subStateConfig)
//    {
//        QMap<short, CANFctModuleMotionProfile*>::const_iterator iter;
//        CANFctModuleMotionProfile* pMotionProfile = 0;
//        quint8 idx = 0;

//        FILE_LOG_L(laFCT, llINFO) << "CANStepperMotor " << GetName().toStdString() << " : MotionProfile " << (int) MotionProfileIndex << " - " << (int) MotionProfileSubIndex;

//        //position the iterator to the motion profile whichs data should be transmitted
//        iter = pCANObjConfMotor->listMotionProfiles.constBegin();
//        for(idx = 0; idx < MotionProfileIndex; idx++)
//        {
//            iter++;
//        }

//        if(iter == pCANObjConfMotor->listMotionProfiles.constEnd())
//        {
//            FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ":  " << (int) MotionProfileIndex << " motionprofils sent";
//            m_subStateConfig = FM_SM_SUB_STATE_CONFIG_FINISHED;
//        }
//        else
//        {
//            pMotionProfile = iter.value();

//            canmsg.can_id = m_unCanIDMotionProfile; // CAN message ID for motion profile data transmission
//            canmsg.data[0] = MotionProfileIndex;    // movement profile index
//            canmsg.data[1] = MotionProfileSubIndex; // movement profile sub index
//            if(MotionProfileSubIndex == 1)
//            {
//                SetCANMsgDataU16(&canmsg, pMotionProfile->sAcc, 2);
//                SetCANMsgDataU16(&canmsg, pMotionProfile->sSpeedMax, 4);
//                SetCANMsgDataU16(&canmsg, pMotionProfile->sDec, 6);
//                canmsg.can_dlc = 8;
//                MotionProfileSubIndex++;
//            }
//            else if(MotionProfileSubIndex == 2)
//            {
//                canmsg.data[2] = pMotionProfile->bMicroSteps;
//                canmsg.data[3] = pMotionProfile->bRampType;
//                SetCANMsgDataU16(&canmsg, pMotionProfile->sAccTime, 4);
//                SetCANMsgDataU16(&canmsg, pMotionProfile->sDecTime, 6);
//                canmsg.can_dlc = 8;
//                MotionProfileSubIndex++;
//            }
//            else if(MotionProfileSubIndex == 3)
//            {
//                SetCANMsgDataU16(&canmsg, pMotionProfile->sSpeedMin, 2);
//                SetCANMsgDataU16(&canmsg, (quint16) 0, 4);
//                SetCANMsgDataU16(&canmsg, (quint16) 0, 6);
//                canmsg.can_dlc = 8;

//                //prepare for next profile
//                MotionProfileSubIndex = 1;
//                MotionProfileIndex++;
//            }

//            retval = m_pCANCommunicator->SendCOB(canmsg);
//        }
//    }
//    else if(m_subStateConfig == FM_SM_SUB_STATE_CONFIG_FINISHED)
//    {
//        m_mainState = FM_MAIN_STATE_IDLE;
//        m_TaskID = MODULE_TASKID_FREE;
//        FILE_LOG_L(laCONFIG, llDEBUG) << " Module " << GetName().toStdString() << ": change to FM_MAIN_STATE_IDLE";
//    }

//    if(retval != DCL_ERR_FCT_CALL_SUCCESS)
//    {
//        FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCOB returns" << (int) retval;
//        m_mainState = FM_MAIN_STATE_ERROR;
//    }
//}

/****************************************************************************/
/*!
 *  \brief    Handles the function module's state machine in idle main state
 *
 *   Depending on the active task ID, the appropriate task function will be called.
 *
 *  \return   void
 */
/****************************************************************************/
void CStepperMotor::HandleIdleState()
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        HandleCommandRequestTask();
    }
}


/****************************************************************************/
/*!
 *  \brief    Handle the task to execute the motor commands
 *
 *   The function is called from HandleIdleState() if m_taskID == FM_SM_TASKID_COMMAND_HDL.
 *   The function loops thru the m_ModuleCommand array and controls the motor commands stored there
 *
 *    - Motor command with state FM_SM_MOTION_CMD_STATE_REQ will be forwarded to the motor function module on slave side by sending
 *      the corresponding CAN-messge
 *
 *    - Motor command with state FM_SM_MOTION_CMD_STATE_REQ_SEND will be checked for timeout
 *
 *    Usually, a motor command will be confirmed and closed by receiving the expected CAN-message (e.g. movement acknowledge)
 *    This is done at the HandleCANMessage... function
 *
 *
 *
 *  \return   void
 */
/****************************************************************************/
void CStepperMotor::HandleCommandRequestTask()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_NOT_FOUND;
    bool ActiveCommandFound = false;

    for(quint8 idx = 0; idx < MAX_SM_MODULE_CMD_IDX; idx++)
    {
        if(m_ModuleCommand[idx].State == MODULE_CMD_STATE_REQ)
        {
            // forward the motor command to the motor function module on slave side by sending
            // the corresponding CAN-message
            ActiveCommandFound = true;

            if(m_ModuleCommand[idx].Type == FM_SM_CMD_TYPE_STATE)
            {
                //send the state set command to the module, this command will be acknowledged by the receiption
                // of the m_unCanIDStateSetAck CAN-message
                FILE_LOG_L(laFCT, llINFO) << "   CANStepperMotor set motor state";
                RetVal = SendCANMsgSetState(m_ModuleCommand[idx].m_MotorState);
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_STEPPERMOTOR_TIMEOUT_SET_STATE;
                }
                else
                {
                    emit ReportSetStateAckn(GetModuleHandle(), RetVal);
                }
            }
            else if(m_ModuleCommand[idx].Type == FM_SM_CMD_TYPE_REFRUN)
            {
                //send the reference run request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDReferenceMovementAckn CAN-message
                FILE_LOG_L(laFCT, llINFO) << "   CStepperMotor::HandleReferenceMovement: send reference movement request.";
                RetVal = SendCANMsgReferenceMovement();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_STEPPERMOTOR_TIMEOUT_REFMOVE_REQ;
                }
                else
                {
                    emit ReportReferenceMovementAckn(GetModuleHandle(), RetVal, 0);
                }
            }
            else if(m_ModuleCommand[idx].Type == FM_SM_CMD_TYPE_POS)
            {
                //send the target position to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDMovementAckn CAN-message
                RetVal = SendCANMsgTargetPosition(m_ModuleCommand[idx].TargetPos,
                                                  m_ModuleCommand[idx].MotionProfileIdx,
                                                  m_ModuleCommand[idx].SubCommandID,
                                                  m_ModuleCommand[idx].SubCommandData);
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_STEPPERMOTOR_TIMEOUT_POSMOVE_REQ;
                }
                else
                {
                    emit ReportMovementAckn(GetModuleHandle(), RetVal, 0, 0);
                }
            }
            else if(m_ModuleCommand[idx].Type == FM_SM_CMD_TYPE_SPEED)
            {
                //send the target position to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDMovementAckn CAN-message
                RetVal = SendCANMsgTargetSpeed(m_ModuleCommand[idx].TargetSpeed,
                                               m_ModuleCommand[idx].MotionProfileIdx,
                                               m_ModuleCommand[idx].SubCommandID,
                                               m_ModuleCommand[idx].SubCommandData);
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_STEPPERMOTOR_TIMEOUT_SPEEDMOVE_REQ;
                }
                else
                {
                    emit ReportMovementAckn(GetModuleHandle(), RetVal, 0, 0);
                }
            }
            else if(m_ModuleCommand[idx].Type == FM_SM_CMD_TYPE_ACTPOS_REQ)
            {
                //send the actual position request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDActPositionResp CAN-message
                RetVal = SendCANMsgActPositionReq();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_STEPPERMOTOR_TIMEOUT_ACTPOS_REQ;
                }
                else
                {
                    emit ReportPosition(GetModuleHandle(), RetVal, 0, 0);
                }
            }
            else if(m_ModuleCommand[idx].Type == FM_SM_CMD_TYPE_ACTSPEED_REQ)
            {
                //send the actual speed request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDActSpeed CAN-message
                RetVal = SendCANMsgActSpeedReq();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_STEPPERMOTOR_TIMEOUT_ACTSPD_REQ;
                }
                else
                {
                    emit ReportSpeed(GetModuleHandle(), RetVal, 0);
                }
            }
            else if(m_ModuleCommand[idx].Type == FM_SM_CMD_TYPE_LIFE_CYCLE_DATA_REQ)
            {
                //send the actual speed request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDActSpeed CAN-message.
                RetVal = SendCANMsgLifeCycleDataReq();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].m_Timeout = CAN_STEPPERMOTOR_TIMEOUT_LIFECYCLEDATA_REQ;
                }
                else
                {
                    emit ReportLifeCycleData(GetModuleHandle(), RetVal, 0, 0, 0);
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
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
            }
        }
        else if(m_ModuleCommand[idx].State == MODULE_CMD_STATE_REQ_SEND)
        {
            // check avtive motor commands for timeout
            ActiveCommandFound = true;
            if(m_ModuleCommand[idx].m_ReqSendTime.Elapsed() > m_ModuleCommand[idx].m_Timeout)
            {
                m_lastErrorHdlInfo = DCL_ERR_TIMEOUT;
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
                if(m_ModuleCommand[idx].Type == FM_SM_CMD_TYPE_STATE)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANStepperMotor '" << GetKey().toStdString() << "': SetState timeout error.";
                    emit ReportSetStateAckn(GetModuleHandle(), m_lastErrorHdlInfo);
                }
                else if(m_ModuleCommand[idx].Type == FM_SM_CMD_TYPE_REFRUN)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANStepperMotor '" << GetKey().toStdString() << "': reference run request timeout error.";
                    emit ReportReferenceMovementAckn(GetModuleHandle(), m_lastErrorHdlInfo, 0);
                }
                else if(m_ModuleCommand[idx].Type == FM_SM_CMD_TYPE_POS)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANStepperMotor '" << GetKey().toStdString() << "': positioning timeout error.";
                    emit ReportMovementAckn(GetModuleHandle(), m_lastErrorHdlInfo, 0, 0);
                }
                else if(m_ModuleCommand[idx].Type == FM_SM_CMD_TYPE_SPEED)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANStepperMotor '" << GetKey().toStdString() << "': speed timeout error.";
                    emit ReportMovementAckn(GetModuleHandle(), m_lastErrorHdlInfo, 0, 0);
                }
                else if(m_ModuleCommand[idx].Type == FM_SM_CMD_TYPE_ACTPOS_REQ)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANStepperMotor '" << GetKey().toStdString() << "': act pos req. timeout error.";
                    emit ReportPosition(GetModuleHandle(), m_lastErrorHdlInfo, 0, 0);
                }
                else if(m_ModuleCommand[idx].Type == FM_SM_CMD_TYPE_ACTSPEED_REQ)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANStepperMotor '" << GetKey().toStdString() << "': act speed req. timeout error.";
                    emit ReportSpeed(GetModuleHandle(), m_lastErrorHdlInfo, 0);                    
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
 *   was registered by this class instance, was received.
 *   The CAN message's data will be forwarded to the specialized handle function.
 *
 *  \iparam   pCANframe = struct contains the data of the receipt CAN message
 *
 *  \return   void
 */
/****************************************************************************/
void CStepperMotor::HandleCanMessage(can_frame* pCANframe)
{
    QMutexLocker Locker(&m_Mutex);

    FILE_LOG_L(laFCT, llDEBUG1) << "  CANStepperMotor: 0x" << std::hex << pCANframe->can_id;
    FILE_LOG_L(laFCT, llDEBUG2) << "                  dlc:" << (int) pCANframe->can_dlc << " Task:" << (int) m_TaskID;

    if((pCANframe->can_id == m_unCanIDEventInfo) ||
       (pCANframe->can_id == m_unCanIDEventWarning) ||
       (pCANframe->can_id == m_unCanIDEventError) ||
       (pCANframe->can_id == m_unCanIDEventFatalError))
    {
        HandleCANMsgError(pCANframe);

        // log event info string
        std::string eventString;
        if (GetType() == m_lastErrorGroup)
            eventString = m_eventString[m_lastErrorCode];
        else
            eventString = m_pParent->m_eventString[m_lastErrorCode];
        if ("" == eventString)
            eventString = "unknown";
        FILE_LOG_L(laFCT, llERROR) << " " << eventString;

        if ((pCANframe->can_id == m_unCanIDEventError) || (pCANframe->can_id == m_unCanIDEventFatalError)) {
            emit ReportError(GetModuleHandle(), m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
    }
    else if(m_unCanIDTargetPosCmdAckn == pCANframe->can_id)
    {
        HandleCANMsgTargetPosCmdAckn(pCANframe);
    }
    else if(m_unCanIDTargetSpeedCmdAckn == pCANframe->can_id)
    {
        HandleCANMsgTargetSpeedCmdAckn(pCANframe);
    }
    else if(m_unCanIDMovementAckn == pCANframe->can_id)
    {
        HandleCANMsgMovementAckn(pCANframe);
    }
    else if(m_unCanIDActPositionResp == pCANframe->can_id)
    {
        HandleCANMsgActPositionResp(pCANframe);
    }
    else if(m_unCanIDStateSetAck == pCANframe->can_id)
    {
        HandleCANMsgSetStateAck(pCANframe);
    }
    else if(m_unCanIDReferenceMovementReqAckn == pCANframe->can_id)
    {
        HandleCANMsgReferenceMovementReqAckn(pCANframe);
    }
    else if(m_unCanIDReferenceMovementAckn == pCANframe->can_id)
    {
        HandleCANMsgReferenceMovementAckn(pCANframe);
    }
    else if(m_unCanIDDebug == pCANframe->can_id)
    {
        HandleCANMsgDebug(pCANframe);
    }
    else if(m_unCanIDState == pCANframe->can_id)
    {
        HandleCANMsgState(pCANframe);
    }
    else if(m_unCanIDLiveCycleData == pCANframe->can_id)
    {
        HandleCANMsgLiveCycleData(pCANframe);
    }
    else if(m_unCanIDActSpeed == pCANframe->can_id)
    {
        HandleCANMsgActSpeedResp(pCANframe);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'State'
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CStepperMotor::HandleCANMsgState(can_frame* pCANframe)
{

    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_SM_CMD_TYPE_STATE);
    }

    if(pCANframe->can_dlc == 3)
    {
        FILE_LOG_L(laFCT, llINFO) << "  CANStepperMotor New state '" << GetKey().toStdString() << "', New state " << (int) m_MotorState;

        m_MotorState = pCANframe->data[0];

        emit ReportState(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, m_MotorState);
    }
    else
    {
        emit ReportState(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'Set State Ackn'
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CStepperMotor::HandleCANMsgSetStateAck(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_SM_CMD_TYPE_STATE);
    }

    if(MSG_SMOT_SET_ENABLE_ACK_DLC == pCANframe->can_dlc)
    {
        FILE_LOG_L(laFCT, llINFO) << "  CANStepperMotor set state ack'" << GetKey().toStdString();

        emit ReportSetStateAckn(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS);
    }
    else
    {
        emit ReportSetStateAckn(GetModuleHandle(), DCL_ERR_CANMSG_INVALID);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'reference run request acknowledge'
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CStepperMotor::HandleCANMsgReferenceMovementReqAckn(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_SM_CMD_TYPE_REFRUN);
    }

    if(MSG_SMOT_REQ_REF_RUN_ACK_DLC == pCANframe->can_dlc)
    {
        Msg_RefRunReqAckData_t &ackData = *(Msg_RefRunReqAckData_t*)pCANframe->data;

        SM_AckState_t ack = ackData.ack;

        FILE_LOG_L(laFCT, llDEBUG1) << "  CStepperMotor::reference movement request acknowledge: '" << GetKey().toStdString() <<  "' Ack=" << ack;

        if (SM_ACK != ack)
            emit ReportReferenceMovementAckn(GetModuleHandle(), DCL_ERR_EXTERNAL_ERROR, 0);
    }
    else
    {
        emit ReportReferenceMovementAckn(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'reference run acknwoledge'
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CStepperMotor::HandleCANMsgReferenceMovementAckn(can_frame* pCANframe)
{
//    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
//    {
//        ResetModuleCommand(FM_SM_CMD_TYPE_REFRUN);
//    }

    if(MSG_SMOT_REFERENCE_RUN_ACK_DLC == pCANframe->can_dlc)
    {
        Msg_RefRunAckData_t &ackData = *(Msg_RefRunAckData_t*)pCANframe->data;

        Position_t position = DB4ToVal(ackData.pos);
        quint8 posCode = ackData.posCode;
        SM_AckState_t ack = ackData.ack;

        FILE_LOG_L(laFCT, llDEBUG1) << "  CStepperMotor::reference movement done: '" << GetKey().toStdString() << "',  Position:" << position << "' LimitSwitches:" << (int)posCode << "' Ack:" << ack;
        FILE_LOG_L(laFCT, llDEBUG1) << "  CStepperMotor::reference movement: 0x" << std::hex << m_pCANObjectConfig->m_sCANNodeIndex << "  0x" << std::hex << GetModuleHandle();

        emit ReportReferenceMovementAckn(GetModuleHandle(), (SM_ACK==ack ? DCL_ERR_FCT_CALL_SUCCESS : DCL_ERR_EXTERNAL_ERROR), position);
    }
    else
    {
        emit ReportReferenceMovementAckn(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'Target position command acknwoledge'
 *
 *      By sending this CAN message on slave side, it acknowledges the receiption of the message,
 *      but not the execution of the movement.
 *      This is not the acknowledge of the movement!
 *      The movement execution will be acknowledged with m_unCanIDMovementAckn, both for speed and position movements.
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CStepperMotor::HandleCANMsgTargetPosCmdAckn(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_SM_CMD_TYPE_POS);
    }

    if (MSG_SMOT_TARGET_POS_ACK_DLC == pCANframe->can_dlc)
    {
        Msg_TargetPosAckData_t &ackData = *(Msg_TargetPosAckData_t*)pCANframe->data;

        SM_AckState_t ack = ackData.ack;

        FILE_LOG_L(laFCT, llDEBUG1) << "  CStepperMotor::target position request acknowledge: '" << GetKey().toStdString() <<  "' Ack=" << ack;

        if (SM_ACK != ack)
            emit ReportMovementAckn(GetModuleHandle(), DCL_ERR_EXTERNAL_ERROR, 0, 0);
    }
    else
    {
        emit ReportMovementAckn(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'Target speed command acknwoledge'
 *
 *      By sending this CAN message on slave side, it acknowledges the receiption of the message,
 *      but not the execution of the movement.
 *      The execution will be acknowledged with m_unCanIDMovementAckn, both for speed and position movement.
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CStepperMotor::HandleCANMsgTargetSpeedCmdAckn(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_SM_CMD_TYPE_SPEED);
    }

    if(MSG_SMOT_TARGET_SPEED_ACK_DLC == pCANframe->can_dlc)
    {
        Msg_TargetSpeedAckData_t &ackData = *(Msg_TargetSpeedAckData_t*)pCANframe->data;

        SM_AckState_t ack = ackData.ack;

        FILE_LOG_L(laFCT, llDEBUG1) << "  CStepperMotor::target speed request acknowledge: '" << GetKey().toStdString() <<  "' Ack=" << ack;

        if (SM_ACK != ack)
            emit ReportMovementAckn(GetModuleHandle(), DCL_ERR_EXTERNAL_ERROR, 0, 0);
    }
    else
    {
        emit ReportMovementAckn(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'Movement acknowledge'
 *
 *      The CAN message is sent by the motor whenever a movement command was executed
 *       - the movement to a target position was finished
 *       - a speed command was executed (the target speed was reached)
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CStepperMotor::HandleCANMsgMovementAckn(can_frame* pCANframe)
{
    if(MSG_SMOT_MOVEMENT_ACK_DLC == pCANframe->can_dlc)
    {
        Msg_MovementAckData_t &ackData = *(Msg_MovementAckData_t*)pCANframe->data;

        SM_AckState_t ack   = ackData.ack;
        Position_t Position = DB4ToVal(ackData.pos);
        qint16 Speed        = DB2ToVal(ackData.speed);

        FILE_LOG_L(laFCT, llDEBUG1) << "  CANStepperMotor: movement ackn:  '" << GetKey().toStdString() << "' Pos:" << Position << ", Speed:" << Speed <<  "' Ack=" << ack;

        emit ReportMovementAckn(GetModuleHandle(), (SM_ACK==ack ? DCL_ERR_FCT_CALL_SUCCESS : DCL_ERR_EXTERNAL_ERROR), Position, Speed);
    }
    else
    {
        emit ReportMovementAckn(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'Actual position'
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CStepperMotor::HandleCANMsgActPositionResp(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_SM_CMD_TYPE_ACTPOS_REQ);
    }

    if(MSG_SMOT_ACT_POS_DLC == pCANframe->can_dlc)
    {
        Msg_PositionData_t &posData = *(Msg_PositionData_t*)pCANframe->data;

        SM_AckState_t ack   = posData.ack;
        Position_t position = DB4ToVal(posData.pos);
        qint8 posCode = posData.posCode;

        FILE_LOG_L(laFCT, llDEBUG1) << "  CANStepperMotor: act. position: '" << GetKey().toStdString() << "' Pos:" << position << "' LimitSwitches:" << (int)posCode <<  "' Ack=" << ack;

        emit ReportPosition(GetModuleHandle(), (SM_ACK==ack ? DCL_ERR_FCT_CALL_SUCCESS : DCL_ERR_EXTERNAL_ERROR), position, posCode);
    }
    else
    {
        emit ReportPosition(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'Actual speed'
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CStepperMotor::HandleCANMsgActSpeedResp(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_SM_CMD_TYPE_ACTSPEED_REQ);
    }

    if(MSG_SMOT_ACT_SPEED_DLC == pCANframe->can_dlc)
    {
        Msg_SpeedData_t &speedData = *(Msg_SpeedData_t*)pCANframe->data;

        SM_AckState_t ack   = speedData.ack;
        Speed_t Speed = DB2ToVal(speedData.speed);

        FILE_LOG_L(laFCT, llDEBUG1) << "  CANStepperMotor: act. speed: '" << GetKey().toStdString() << "' Speed:" << Speed <<  "' Ack=" << ack;

        emit ReportSpeed(GetModuleHandle(), (SM_ACK==ack ? DCL_ERR_FCT_CALL_SUCCESS : DCL_ERR_EXTERNAL_ERROR), Speed);
    }
    else
    {
        emit ReportSpeed(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0);
    }
}


/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'LiveCycleData'
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CStepperMotor::HandleCANMsgLiveCycleData(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_SM_CMD_TYPE_LIFE_CYCLE_DATA_REQ);
    }

    if(pCANframe->can_dlc == 5)
    {
        quint8 SubIndex;

        SubIndex = pCANframe->data[0];
        if(SubIndex == 0)
        {
            m_StepCounter = ((quint32) pCANframe->data[1]) << 24;
            m_StepCounter |= ((quint32) pCANframe->data[2]) << 16;
            m_StepCounter |= ((quint32) pCANframe->data[3]) << 8;
            m_StepCounter |= ((quint32) pCANframe->data[4]);
        }
        else if(SubIndex == 1)
        {
            m_TripCounter = ((quint32) pCANframe->data[1]) << 24;
            m_TripCounter |= ((quint32) pCANframe->data[2]) << 16;
            m_TripCounter |= ((quint32) pCANframe->data[3]) << 8;
            m_TripCounter |= ((quint32) pCANframe->data[4]);
        }
        else if(SubIndex == 2)
        {
            m_OperationTime = ((quint32) pCANframe->data[1]) << 24;
            m_OperationTime |= ((quint32) pCANframe->data[2]) << 16;
            m_OperationTime |= ((quint32) pCANframe->data[3]) << 8;
            m_OperationTime |= ((quint32) pCANframe->data[4]);
        }

        FILE_LOG_L(laFCT, llDEBUG1) << " CANStepperMotor-LiveCyleData: Steps, trips, time: " << m_StepCounter <<  ", " << m_TripCounter << ", " << m_OperationTime;
        emit ReportLifeCycleData(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, m_StepCounter, m_TripCounter, m_OperationTime);
    }
    else
    {
        emit ReportLifeCycleData(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'Debug'
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CStepperMotor::HandleCANMsgDebug(can_frame* pCANframe)
{
    if(pCANframe->can_dlc == 8)
    {
        Position_t Position;
        qint16     Speed;
        quint16    Acc;

        Position = ((Position_t) pCANframe->data[0]) << 8;
        Position |= ((Position_t) pCANframe->data[1]);
        Acc = ((qint16) pCANframe->data[2]) << 8;
        Acc |= ((qint16) pCANframe->data[3]);
        Speed = ((qint16) pCANframe->data[4]) << 8;
        Speed= ((qint16) pCANframe->data[5]);

        FILE_LOG_L(laFCT, llDEBUG1) << " Debug: pos/speed/acc: " << Position << ", " << Speed << ", " << Acc;
    }
}

/****************************************************************************/
/*!
 *  \brief    Send the CAN message to set the motor state
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *            otherwise the return code as returned from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::SendCANMsgSetState(bool MotorState)
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    Msg_EnableData_t &stateData = *(Msg_EnableData_t*)canmsg.data;

    canmsg.can_id = m_unCanIDStateSet;
    canmsg.can_dlc = MSG_SMOT_SET_ENABLE_DLC;

    if(MotorState)
    {
        stateData.enable = true;
        // TODO: for reconfiguration testing: no need for ReferenceRun when enabling FM
        stateData.dbg_skipRefRun = true;
    }
    else
    {
        stateData.enable = false;
        stateData.dbg_skipRefRun = false;
    }
    stateData.dbg_sendMovementData = false;
    stateData.reserved = 0;

//    if(MotorState)
//    {
//        MotorStateTmp |= 1 << SM_STATE_BITPOS_ENABLED;
//        // TODO: for reconfiguration testing: no need for ReferenceRun when enabling FM
//        MotorStateTmp |= 1 << SM_STATE_BITPOS_SKIPREFRUN;
//    }
//    canmsg.data[0] = MotorStateTmp;
//    canmsg.data[1] = 0;
//    canmsg.data[2] = 0;
//    canmsg.data[3] = 0;
//    canmsg.data[4] = 0;
//    canmsg.data[5] = 0;
//    canmsg.data[6] = 0;
//    canmsg.data[7] = 0;

    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG2) << "   CStepperMotor::SendCANMsgSetState canID: 0x" << std::hex << m_unCanIDReferenceMovement;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Send the CAN message to start the reference run
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit
 *            queue otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::SendCANMsgReferenceMovement()
{
    ReturnCode_t retval;
    can_frame canmsg;

    Msg_RefRunData_t &refRunData = *(Msg_RefRunData_t*)canmsg.data;

    canmsg.can_id = m_unCanIDReferenceMovement;
    canmsg.can_dlc = MSG_SMOT_REQ_REF_RUN_DLC;

    refRunData.profile = 0; // use profile 0 for reference run

    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG2) << "   CStepperMotor::SendCANMsgReferenceMovement canID: 0x" << std::hex << m_unCanIDReferenceMovement;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief    Send the CAN message to start a motor's positioning
 *
 *            The can message is composed by
 *             - Byte 0..3 : Target position
 *             - Byte 4    : Movement profile
 *             - Byte 5    : Sub command index
 *             - Byte 6..7 : Sub command data
 *
 *  \param    TargetPos = Position where the motor shall drive to
 *  \param    MotionProfile = Motion profile for the movement
 *  \param    SubCommandID = SubCommand identifier
 *  \param    SubCommandData = Sub command data
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *            otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::SendCANMsgTargetPosition(Position_t TargetPos,
                                                     quint8 MotionProfile,
                                                     StepperMotorSubCommandMotion_t SubCommandID,
                                                     quint16 SubCommandData)
{
    Q_UNUSED(SubCommandID)
    Q_UNUSED(SubCommandData)

    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDTargetPos;
    canmsg.can_dlc = MSG_SMOT_TARGET_POS_DLC;

    Msg_TargetPositionData_t &posData = *(Msg_TargetPositionData_t*)canmsg.data;

    posData.pos = ValToDB4(TargetPos);
    posData.profile = MotionProfile;

    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG2) << "   CStepperMotor::sendCANMsgTargetPosition CanID: 0x" << std::hex << m_unCanIDTargetPos;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Send the CAN message to start a motor's movement with target speed
 *
 *            The can message is composed by
 *             - target speed
 *             - movement profile
 *             -
 *            The can message is composed by
 *             - Byte 0..3 : Target speed
 *             - Byte 4    : Movement profile
 *             - Byte 5    : Sub command index
 *             - Byte 6..7 : Sub command data
 *
 *  \param    TargetSpeed    = Position where the motor shall drive to
 *  \param    MotionProfile  = Motion profile for the movement
 *  \param    SubCommandID   = SubCommand identifier
 *  \param    SubCommandData = Sub command data
 *
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *            otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::SendCANMsgTargetSpeed(Position_t TargetSpeed,
                                                  quint8 MotionProfileIdx,
                                                  StepperMotorSubCommandMotion_t SubCommandID,
                                                  quint16 SubCommandData)
{
    Q_UNUSED(SubCommandID)
    Q_UNUSED(SubCommandData)

    ReturnCode_t RetVal;
    can_frame canmsg;

    Msg_TargetSpeedData_t &speedData = *(Msg_TargetSpeedData_t*)canmsg.data;

    canmsg.can_id = m_unCanIDTargetSpeed;
    canmsg.can_dlc = MSG_SMOT_TARGET_SPEED_DLC;

    // unit for speed is half-steps
    speedData.speed = ValToDB2(TargetSpeed);
    speedData.profile = MotionProfileIdx;

    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG2) << "   CStepperMotor::SendCANMsgTargetSpeed CanID: 0x" << std::hex << m_unCanIDTargetSpeed;

    return RetVal;

}

/****************************************************************************/
/*!
 *  \brief    Send the CAN message to request the actual motor position
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *            otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::SendCANMsgActPositionReq()
{
    ReturnCode_t retval;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDActPositionReq;
    canmsg.can_dlc = MSG_SMOT_ACT_POS_REQ_DLC;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG2) << "   CStepperMotor::SendCANMsgActPositionReq canID: 0x" << std::hex << m_unCanIDActPositionReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief    Send the CAN message to request the actual motor speed
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transim queue
 *            otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::SendCANMsgActSpeedReq()
{
   //ReturnCode_t retval = DCL_ERR_FCT_NOT_IMPLEMENTED;
   //return retval;
    ReturnCode_t retval;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDActSpeedReq;
    canmsg.can_dlc = MSG_SMOT_ACT_SPEED_REQ_DLC;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG2) << "   CStepperMotor::SendCANMsgActPositionReq canID: 0x" << std::hex << m_unCanIDActSpeedReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief    Send the CAN message to request the live cycle data
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transim queue
 *            otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::SendCANMsgLifeCycleDataReq()
{
    ReturnCode_t retval;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDLiveCycleDataReq;
    canmsg.data[0] = 0;
    canmsg.data[1] = 0;
    canmsg.data[2] = 0;
    canmsg.data[3] = 0;
    canmsg.data[4] = 0;
    canmsg.data[5] = 0;
    canmsg.data[6] = 0;
    canmsg.data[7] = 0;
    canmsg.can_dlc = 0;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG2) << "   CStepperMotor::SendCANMsgLifeCycleDataReq canID: 0x" << std::hex << m_unCanIDLiveCycleDataReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief    Set the motor state regarding enabled/disabled
 *            The request will be acknowledged by sending the signal ReportSetStateAckn,
 *
 *  \param    Enabled = Motor state
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::SetMotorState(bool Enabled)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8  CmdIndex;

    if(SetModuleTask(FM_SM_CMD_TYPE_STATE, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].m_MotorState = Enabled;
        FILE_LOG_L(laDEV, llINFO) << " CANStepperMotor : " << (int) Enabled;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANStepperMotor invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Request the motor's reference run
 *            The request will be acknowledged by sending the signal ReportReferenceMovementAckn
 *            as soon as the reference run is confirmed by the slave's function module CAN message
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::ExecReferenceMovement()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(SetModuleTask(FM_SM_CMD_TYPE_REFRUN))
    {
        FILE_LOG_L(laDEV, llDEBUG) << " CANStepperMotor";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANStepperMotor invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Request a movement to a target position
 *            The request will be acknowledged by sending the signal ReportPositionMovementAckn,
 *
 *  \param    TargetPosition   = Target position the motor should move to
 *  \param    MotionProfileIdx = Movement profile index of the profile used to execute the movement
 *  \iparam   SubCommandID     = Sub command identifier, passed to the motor
 *  \iparam   SubCommandData   = Sub command data, passed to the motor
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::DriveToPosition(quint32 TargetPosition, quint8 MotionProfileIdx, StepperMotorSubCommandMotion_t SubCommandID, quint16 SubCommandData)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    quint8  CmdIndex;

    if(GetModuleHandle() == 0x0010001)
    {
        if(SubCommandID == SM_SUBCMD_MOTION_TIME_DELAY)
        {
            CmdIndex = 0;
        }
    }

    if(SetModuleTask(FM_SM_CMD_TYPE_POS, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].TargetPos = TargetPosition;
        m_ModuleCommand[CmdIndex].TargetSpeed = 0;
        m_ModuleCommand[CmdIndex].MotionProfileIdx = MotionProfileIdx;
        m_ModuleCommand[CmdIndex].SubCommandID = SubCommandID;
        m_ModuleCommand[CmdIndex].SubCommandData = SubCommandData;
        FILE_LOG_L(laDEV, llINFO) << " CANStepperMotor, Position: " << (int) TargetPosition;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANStepperMotor invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Request a movement with a target speed
 *            The request will be acknowledged by sending the signal ReportSpeedMovementAckn
 *
 *  \iparam   TargetSpeed      = Motor movement target speed
 *  \iparam   MotionProfileIdx = Movement profile index of the profile used to execute the movement
 *  \iparam   SubCommandID     = Sub command identifier, passed to the motor
 *  \iparam   SubCommandData   = Sub command data, passed to the motor
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::DriveSpeed(qint16 TargetSpeed, quint8 MotionProfileIdx, StepperMotorSubCommandMotion_t SubCommandID, quint16 SubCommandData)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8  CmdIndex;

    if(SetModuleTask(FM_SM_CMD_TYPE_SPEED, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].TargetPos = 0;
        m_ModuleCommand[CmdIndex].TargetSpeed = TargetSpeed;
        m_ModuleCommand[CmdIndex].MotionProfileIdx = MotionProfileIdx;
        m_ModuleCommand[CmdIndex].SubCommandID = SubCommandID;
        m_ModuleCommand[CmdIndex].SubCommandData = SubCommandData;
        FILE_LOG_L(laDEV, llINFO) << " CANStepperMotor, Sp: " << (int) TargetSpeed << ", " << (int) SubCommandID << ", " << (int) SubCommandData;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANStepperMotor invalid state: " << (int) m_TaskID;
    }

    return RetVal;

}

/****************************************************************************/
/*!
 *  \brief    Request the actual motor position
 *            The request will be acknowledged by sending the signal ReportPosition
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::ReqActPosition()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(SetModuleTask(FM_SM_CMD_TYPE_ACTPOS_REQ))
    {
        FILE_LOG_L(laDEV, llDEBUG) << " CANStepperMotor";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANStepperMotor invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Request the actual motor speed
 *            The request will be acknowledged by sending the signal ReportPosition ??
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::ReqActSpeed()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(SetModuleTask(FM_SM_CMD_TYPE_ACTSPEED_REQ))
    {
        FILE_LOG_L(laDEV, llDEBUG) << " CANStepperMotor";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANStepperMotor invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Request the live cycle data
 *            The request will be acknowledged by sending the signal ReportLifeCycleData
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::RequestLifeCycleData()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8  CmdIndex;

    if(SetModuleTask(FM_SM_CMD_TYPE_LIFE_CYCLE_DATA_REQ, &CmdIndex))
    {
        FILE_LOG_L(laDEV, llDEBUG) << " CANStepperMotor";
        m_ModuleCommand[CmdIndex].SubCommandData = 0;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANStepperMotor invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Helper function, sets a free module command to the given command type
 *
 *  \iparam   CommandType = command type to set
 *  \iparam   pCmdIndex = pointer to index within the command array the command is set to (optional parameter, default 0)
 *
 *  \return  true, if the command type can be placed, otherwise false
 *
 ****************************************************************************/
bool CStepperMotor::SetModuleTask(CANStepperMotorMotionCmdType_t CommandType, quint8* pCmdIndex)
{
    bool CommandAdded = false;

    if((m_TaskID == MODULE_TASKID_FREE) || (m_TaskID == MODULE_TASKID_COMMAND_HDL))
    {
        for(quint8 idx = 0; idx < MAX_SM_MODULE_CMD_IDX; idx++)
        {
            if(m_ModuleCommand[idx].State == MODULE_CMD_STATE_FREE)
            {
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ;
                m_ModuleCommand[idx].Type = CommandType;

                m_TaskID = MODULE_TASKID_COMMAND_HDL;
                CommandAdded  = true;
                if(pCmdIndex)
                {
                    *pCmdIndex = idx;
                }

                FILE_LOG_L(laFCT, llINFO) << " CANStepperMotor:  task " << (int) idx << " request.";
                break;
            }
        }
    }

    return CommandAdded;
}

/****************************************************************************/
/**
 *  \brief      Set the ModuleCommands with the specified command type to 'FREE'
 *
 *  \param      ModuleCommandType = ModuleCommands having this command type will be set to free
 *
 *  \return     void
 */
/****************************************************************************/
void CStepperMotor::ResetModuleCommand(CANStepperMotorMotionCmdType_t ModuleCommandType)
{
    bool ActiveCommandFound = false;

    for(quint8 idx = 0; idx < MAX_SM_MODULE_CMD_IDX; idx++)
    {
        if((m_ModuleCommand[idx].Type == ModuleCommandType) &&
           (m_ModuleCommand[idx].State == MODULE_CMD_STATE_REQ_SEND))
        {
            m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
        }

        if(m_ModuleCommand[idx].State != MODULE_CMD_STATE_FREE)
        {
            ActiveCommandFound = true;
        }
    }

    if(ActiveCommandFound == false)
    {
        m_TaskID = MODULE_TASKID_FREE;
    }
}


/****************************************************************************/
/*!
 *  \brief    Return the speed the motor needs to be driven to move the specified distance
 *
 *            The function calculate the speed which must be send to the motor to
 *            cross the specified distance with the basic conditions taken from the specified
 *            motion profile
 *
 *  \iparam   Distance = The distance to postion
 *  \iparam   MotionProfileIdx = The motion profile's index used for the calculation
 *  \iparam   Period = Planned time consumption of the motion
 *
 *  \return   The speed need for this motion
 */
/****************************************************************************/
Speed_t CStepperMotor::GetSpeedFromPeriod(Position_t Distance, MotionProfileIdx_t MotionProfileIdx, quint16 Period)
{
    Speed_t Speed;
    CANFctModuleStepperMotor *pCANObjConfMotor = dynamic_cast<CANFctModuleStepperMotor *>(m_pCANObjectConfig);
    CANFctModuleMotionProfile &MotionProfile = pCANObjConfMotor->listMotionProfiles[MotionProfileIdx];
    qint32 Dummy;

    if(Distance < 0)
    {
        Distance = -Distance;
    }

    //keep it simple ;-)
    Dummy = Period;
    Speed = (Distance * 1000) / Dummy;

    //ensure that the speed not exceeds the limits
    if(Speed > (Speed_t) MotionProfile.sSpeedMax)
    {
        Speed = (Speed_t) MotionProfile.sSpeedMax;
    }
    if(Speed < (Speed_t) MotionProfile.sSpeedMin)
    {
        Speed = (Speed_t) MotionProfile.sSpeedMin;
    }

    return Speed;
}


/****************************************************************************/
/*!
 *  \brief    Initiates reconfiguration of parameter set used by the stepper module.
 *            The request will be acknowledged by sending the signal ReportConfigureDone.
 *            Reconfiguration is initiated by switching mainState back to FM_MAIN_STATE_CONFIRMED.
 *            The modified mainState is processed in HandleTasks.
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::DoReconfiguration(CANFctModuleStepperMotor *pConfig)
{
    QMutexLocker Locker(&m_Mutex);
    CANFctModuleStepperMotor* pCANObjConfMotor = (CANFctModuleStepperMotor*) m_pCANObjectConfig;

    if (!((FM_MAIN_STATE_IDLE == m_mainState) && (MODULE_TASKID_FREE == m_TaskID)))
    {
        return DCL_ERR_INVALID_STATE;
    }

    // Quick&Dirty: Profile and Limit switch configuration are directly changed
    //  => get rid of pointers or write assign op/copy constructor or even better:
    // \todo use new data container
    *pCANObjConfMotor = *pConfig;

//    m_TaskID = MODULE_TASKID_INIT;
    m_mainState = FM_MAIN_STATE_CONFIRMED;

    FILE_LOG_L(laFCT, llINFO) << " CANStepperMotor: reconfiguration request.";


    return DCL_ERR_FCT_CALL_SUCCESS;
}

void CStepperMotor::TestOutput(QString text)
{
    if(m_bTestOutput == true)
    {
        m_nCounter++;
        if(m_nCounter > 100)
        {
            FILE_LOG_L(laFCT, llINFO) << "   CANStepperMotor<" << GetKey().toStdString() << "> " << text.toStdString() << "  " << std::hex << this;
            m_nCounter = 0;
        }
    }
}
} //namespace
