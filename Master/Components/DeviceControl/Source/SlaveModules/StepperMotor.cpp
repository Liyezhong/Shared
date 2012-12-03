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


// CAN-IDs for Error/Warning/Info Messages
#define MSG_EVENT_INFO              BUILD_CAN_ID(CMD_CLASS_ERROR,   0, 0)
#define MSG_EVENT_WARNING           BUILD_CAN_ID(CMD_CLASS_ERROR,   1, 0)
#define MSG_EVENT_ERROR             BUILD_CAN_ID(CMD_CLASS_ERROR,   2, 0)
#define MSG_EVENT_FATAL             BUILD_CAN_ID(CMD_CLASS_ERROR,   3, 0)

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
    m_unCanIDConfig(0),
    m_unCanIDStateSet(0), m_unCanIDStateSetAck(0),
    m_unCanIDReferenceMovement(0), m_unCanIDReferenceMovementReqAckn(0),  m_unCanIDReferenceMovementAckn(0),
    m_unCanIDTargetPos(0), m_unCanIDTargetPosCmdAckn(0),
    m_unCanIDTargetSpeed(0), m_unCanIDTargetSpeedCmdAckn(0),
    m_unCanIDMovementAckn(0),
    m_unCanIDActPositionReq(0), m_unCanIDActPositionResp(0),
    m_unCanIDActSpeedReq(0), m_unCanIDActSpeed(0),
    m_unCanIDOpTimeDataReq(0), m_unCanIDOpTimeData(0),
    m_unCanIDRevCountDataReq(0), m_unCanIDRevCountData(0),
    m_RevolutionCount(0), m_TripCounter(0), m_OperationTime(0)
{
    // main state
    m_mainState = FM_MAIN_STATE_BOOTUP;
    // configuration state
    m_subStateConfig  = FM_SM_SUB_STATE_CONFIG_INIT;
    m_subIndex.type.profileData = false;
    m_subIndex.param.index      = LS1;

    // init event string list
    m_eventString[MSG_EVENT_ERROR | 1]  = "E_SMOT_INVALID_STATE: a command was received while the motor's state is not able to execute it";
    m_eventString[MSG_EVENT_ERROR | 2]  = "E_SMOT_CONFIG_INCOMPLETE: received configuration data is not complete";

    m_eventString[MSG_EVENT_ERROR | 3]  = "E_SMOT_CONFIG_LIMITSWITCH: position code configuration doesn't match limit switch configuration";
    m_eventString[MSG_EVENT_ERROR | 4]  = "E_SMOT_CONFIG_INVALID_DRIVER: configuration data for unknown stepper driver received";
    m_eventString[MSG_EVENT_ERROR | 5]  = "E_SMOT_CONFIG_REFRUN: invalid reference run configuration";
    m_eventString[MSG_EVENT_ERROR | 6]  = "E_SMOT_CONFIG_PROFILES: invalid amount of profiles";
    m_eventString[MSG_EVENT_ERROR | 7]  = "E_SMOT_CONFIG_INVALID_PROFILE: profile data for given vMax invalid";
    m_eventString[MSG_EVENT_ERROR | 8]  = "E_SMOT_CONFIG_INVALID_PROFILE_VMIN: profile data for vMin/microsteps not compatible to timer reolution";
    m_eventString[MSG_EVENT_ERROR | 9]  = "E_SMOT_CONFIG_INVALID_PROFILE_VMAX: profile data for vMax/microsteps not compatible to interrupt timing";

    m_eventString[MSG_EVENT_ERROR | 10] = "E_SMOT_REFRUN_TIMEOUT: during a reference run a timeout occured";
    m_eventString[MSG_EVENT_ERROR | 11] = "E_SMOT_REFRUN_MAXDISTANCE: during a reference run the max distance was exceeded";
    m_eventString[MSG_EVENT_ERROR | 12] = "E_SMOT_REFRUN_ERROR: during a reference run the motor stopped unexpectedly";
    m_eventString[MSG_EVENT_ERROR | 13] = "E_SMOT_TIMER_OVERLOAD: all stepper operations stopped because of timer overload - BOARD RESTART NEEDED";

    m_eventString[MSG_EVENT_ERROR | 20] = "E_SMOT_INVALID_POSCODE: position code from limitswitches is invalid";
    m_eventString[MSG_EVENT_ERROR | 21] = "E_SMOT_INVALID_POSCODE_POSITION: position code from limitswitches does not match with actual position";
    m_eventString[MSG_EVENT_ERROR | 22] = "E_SMOT_INVALID_POSCODE_DIR: movement rotation direction from this position code not allowed";
    m_eventString[MSG_EVENT_ERROR | 23] = "E_SMOT_STOP_BY_POSCODE: during a movement the motor stopped at an position code signal";
    m_eventString[MSG_EVENT_ERROR | 24] = "E_SMOT_STOP_BY_OFFLIMIT: during a movement the motor stopped because actual position get off-limit";
    m_eventString[MSG_EVENT_ERROR | 25] = "E_SMOT_STOP_BY_STEPDIFF: during a movement the motor stopped because step difference between encoder and counter get's too big";
    m_eventString[MSG_EVENT_ERROR | 26] = "E_SMOT_STOP_BY_STEPLOSS: during a movement the motor stopped because of too much lost steps";
    m_eventString[MSG_EVENT_ERROR | 27] = "E_SMOT_STOP_BY_SHUTDOWN: during a movement the motor stopped because of shutdown request from master";
    m_eventString[MSG_EVENT_ERROR | 28] = "E_SMOT_STOP_BY_EMERGENCYSTOP: during a movement the motor stopped by emergency stop request";

    m_eventString[MSG_EVENT_ERROR | 30] = "E_SMOT_MOTION_PROFILE_UNFIT: requested speed exceeds vMin/vMax of selected profile";

    m_eventString[MSG_EVENT_WARNING | 1] = "W_SMOT_STEPLOSS: during a movement step loss was detected";
    m_eventString[MSG_EVENT_WARNING | 2] = "W_SMOT_STEPDIFF: during a movement step difference was detected";
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
    const quint8 ModuleID = MODULE_ID_STEPPER;

    Q_ASSERT(m_pCANObjectConfig);

    RetVal = InitializeEventCANMessages(ModuleID);

    quint32 nodeId  = (m_pParent->GetNodeID() & 0x00000F00);    // get node index
    nodeId |= ((m_pParent->GetNodeID() & 0x000000FF) << 1);     // add node type
    nodeId |= ((m_pCANObjectConfig->m_sChannel) << 12);         // and channel

    m_unCanIDConfig             = MSG_SMOT_CONFIG | nodeId;

    m_unCanIDStateSet           = MSG_SMOT_SET_ENABLE | nodeId;
    m_unCanIDStateSetAck        = MSG_SMOT_SET_ENABLE_ACK | nodeId;

    m_unCanIDReferenceMovement          = MSG_SMOT_REQ_REF_RUN | nodeId;
    m_unCanIDReferenceMovementReqAckn   = MSG_SMOT_REQ_REF_RUN_ACK | nodeId;
    m_unCanIDReferenceMovementAckn      = MSG_SMOT_REFERENCE_RUN_ACK | nodeId;

    m_unCanIDTargetPos          = MSG_SMOT_TARGET_POS | nodeId;
    m_unCanIDTargetPosCmdAckn   = MSG_SMOT_TARGET_POS_ACK | nodeId;
    m_unCanIDTargetSpeed        = MSG_SMOT_TARGET_SPEED | nodeId;
    m_unCanIDTargetSpeedCmdAckn = MSG_SMOT_TARGET_SPEED_ACK | nodeId;
    m_unCanIDMovementAckn       = MSG_SMOT_MOVEMENT_ACK | nodeId;

    m_unCanIDActPositionReq     = MSG_SMOT_ACT_POS_REQ | nodeId;
    m_unCanIDActPositionResp    = MSG_SMOT_ACT_POS | nodeId;

    m_unCanIDActSpeedReq        = MSG_SMOT_ACT_SPEED_REQ | nodeId;
    m_unCanIDActSpeed           = MSG_SMOT_ACT_SPEED | nodeId;

    m_unCanIDOpTimeDataReq      = MSG_SMOT_OPTIME_REQ | nodeId;
    m_unCanIDOpTimeData         = MSG_SMOT_OPTIME | nodeId;

    m_unCanIDRevCountDataReq    = MSG_SMOT_REVCOUNT_REQ | nodeId;
    m_unCanIDRevCountData       = MSG_SMOT_REVCOUNT | nodeId;



    FILE_LOG_L(laINIT, llDEBUG) << "  CAN-messages for fct-module:" << GetName().toStdString() << ",node id:" << std::hex << m_pParent->GetNodeID();
    FILE_LOG_L(laINIT, llDEBUG) << "   EventInfo             : 0x" << std::hex << m_unCanIDEventInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventWarning          : 0x" << std::hex << m_unCanIDEventWarning;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventError            : 0x" << std::hex << m_unCanIDEventError;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventFatalError       : 0x" << std::hex << m_unCanIDEventFatalError;
    FILE_LOG_L(laINIT, llDEBUG) << "   ConfigParameter       : 0x" << std::hex << m_unCanIDConfig;
    FILE_LOG_L(laINIT, llDEBUG) << "   StateSet              : 0x" << std::hex << m_unCanIDStateSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   StateSetAck           : 0x" << std::hex << m_unCanIDStateSetAck;
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
    FILE_LOG_L(laINIT, llDEBUG) << "   OperationTimeReq      : 0x" << std::hex << m_unCanIDOpTimeDataReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   OperationData         : 0x" << std::hex << m_unCanIDOpTimeData;
    FILE_LOG_L(laINIT, llDEBUG) << "   RevCountReq           : 0x" << std::hex << m_unCanIDRevCountDataReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   RevCountData          : 0x" << std::hex << m_unCanIDRevCountData;

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
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDOpTimeData, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDRevCountData, this);
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
        if(m_timeAction.Elapsed() > 50)
        {
            HandleConfigurationState();
            m_timeAction.Trigger();
        }
    }
    else if(m_mainState == FM_MAIN_STATE_ERROR)
    {
    }
}


StepperMotorRotDir_t CStepperMotor::SetupRotationDir (CANFctModuleStepperMotor::RotationDir_t dir)
{
    StepperMotorRotDir_t rotDir;

    Q_ASSERT ((CANFctModuleStepperMotor::ROTATION_DIR_CW == dir) || (CANFctModuleStepperMotor::ROTATION_DIR_CCW == dir));

    if  (CANFctModuleStepperMotor::ROTATION_DIR_CW == dir)
        rotDir = SMOT_ROTDIR_CW;
    else
        rotDir = SMOT_ROTDIR_CCW;

    return rotDir;
}


void CStepperMotor::SetupLimitSwitchConfigData (CANFctModuleLimitSwitch &LimitSwitch, ConfigData_LS_t &ls)
{
    ls.flag.exist = LimitSwitch.bExists;
    ls.flag.polarity = LimitSwitch.bPolarity;   //limit switch logic level
    ls.flag.reserved = 0;
    ls.sampleRate = LimitSwitch.bSampleRate;
    ls.debounceCount = LimitSwitch.bDebounce;
}


void CStepperMotor::SetupPosCodeConfigData (CANFctModulePosCode &PosCode, ConfigData_LSPOS_t &pc)
{
    pc.flag.valid   = PosCode.bValid;
    pc.flag.stop    = PosCode.bStop;
    pc.flag.stopDir = SetupRotationDir(CANFctModuleStepperMotor::RotationDir_t(PosCode.bStopDir));  // stop direction
    pc.position     = ValToDB4(PosCode.position);
    pc.width        = PosCode.width;
    pc.deviation    = PosCode.deviation;
}


/******************************************************************/
/*! \brief      Handles the Motors configuration state
 *
 *      During the configuration procedure, the stepper motors configuration data, subdivided to several CAN messages,
 *      will be send via CAN bus.
 *      This function fills the CAN messages date area with the configuration options for the stepper motor.
 */
 /******************************************************************/
void CStepperMotor::HandleConfigurationState()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    CANFctModuleStepperMotor* pCANObjConfMotor;

    Q_ASSERT(m_pCANObjectConfig);
    pCANObjConfMotor = (CANFctModuleStepperMotor*) m_pCANObjectConfig;

    can_frame canmsg;
    ConfigData_Param_t *configData = (ConfigData_Param_t*)canmsg.data;

    //the following parameters must be send to the motor:
    // limit switch: index, orientation, logic level, interval, debounce
    // encoder: type, resolution, direction of rotation
    // motor: direction of rotation, steps per revolution

    canmsg.can_id = m_unCanIDConfig;
    configData->index = m_subIndex;      // message sub index

    if (0 == m_subIndex.type.profileData)
    {
        switch (m_subIndex.param.index)
        {
        case LS1:
            canmsg.can_dlc = sizeof(ConfigData_LS_t) + sizeof(SubIndex_t);
            SetupLimitSwitchConfigData(pCANObjConfMotor->LimitSwitch1, configData->part.ls1);
            break;
        case LS2:
            canmsg.can_dlc = sizeof(ConfigData_LS_t) + sizeof(SubIndex_t);
            SetupLimitSwitchConfigData(pCANObjConfMotor->LimitSwitch2, configData->part.ls2);
            break;
        case POS1:
            canmsg.can_dlc = sizeof(ConfigData_LSPOS_t) + sizeof(SubIndex_t);
            SetupPosCodeConfigData(pCANObjConfMotor->PosCode1, configData->part.pos1);
            break;
        case POS2:
            canmsg.can_dlc = sizeof(ConfigData_LSPOS_t) + sizeof(SubIndex_t);
            SetupPosCodeConfigData(pCANObjConfMotor->PosCode2, configData->part.pos2);
            break;
        case POS3:
            canmsg.can_dlc = sizeof(ConfigData_LSPOS_t) + sizeof(SubIndex_t);
            SetupPosCodeConfigData(pCANObjConfMotor->PosCode3, configData->part.pos3);
            break;
        case ENC:
            {
                ConfigData_ENC_t &enc = configData->part.enc;
                canmsg.can_dlc = sizeof(ConfigData_ENC_t) + sizeof(SubIndex_t);

                enc.flag.exist = (0 != pCANObjConfMotor->bEncoderType);
                if (enc.flag.exist)
                {
                    enc.resolution = ValToDB2(pCANObjConfMotor->sEncoderResolution);
                    enc.stepLoss_WarnLimit = ValToDB2(pCANObjConfMotor->sStepLossWarnLimit);
                    enc.stepLoss_ErrLimit = ValToDB2(pCANObjConfMotor->sStepLossErrorLimit);
                }
                else
                {
                    enc.resolution = ValToDB2(0);
                    enc.stepLoss_WarnLimit = ValToDB2(0);
                    enc.stepLoss_ErrLimit = ValToDB2(0);
                }
            }
            break;
        case MOT1:
            {
                ConfigData_MOT_P1_t &mot1 = configData->part.mot1;
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
                ConfigData_MOT_P2_t &mot2 = configData->part.mot2;
                canmsg.can_dlc = sizeof(ConfigData_MOT_P2_t) + sizeof(SubIndex_t);

                mot2.flag.rotDir = SetupRotationDir(pCANObjConfMotor->bDirection);
                mot2.minPos = ValToDB4(pCANObjConfMotor->lMinPosition);
                mot2.resetPos = ValToDB2(pCANObjConfMotor->sResetPosition);

                quint8 motionProfCount = pCANObjConfMotor->listMotionProfiles.count();
                if ((motionProfCount < 1) || (motionProfCount > MAX_MOTION_PROFIL))
                    //return ErrConfigData;
                    Q_ASSERT(false);
                mot2.flag.motionProf_cnt = motionProfCount;
            }
            break;
        case MOT3:
            {
                ConfigData_MOT_P3_t &mot3 = configData->part.mot3;
                canmsg.can_dlc = sizeof(ConfigData_MOT_P3_t) + sizeof(SubIndex_t);

                mot3.maxPos = ValToDB4(pCANObjConfMotor->lMaxPosition);
            }
            break;

        case REFRUN1:
            {
                ConfigData_REFRUN_P1_t &refrun = configData->part.refRun1;
                canmsg.can_dlc = sizeof(ConfigData_REFRUN_P1_t) + sizeof(SubIndex_t);

                refrun.refRun_RefPos = pCANObjConfMotor->refRunRefPos;
                refrun.refRun_PosOffset = ValToDB4(pCANObjConfMotor->lRefPosOffset);
                refrun.refRun_Timeout = ValToDB2(pCANObjConfMotor->sRefRunTimeout);
            }
            break;
        case REFRUN2:
            {
                ConfigData_REFRUN_P2_t &refrun = configData->part.refRun2;
                canmsg.can_dlc = sizeof(ConfigData_REFRUN_P2_t) + sizeof(SubIndex_t);

                refrun.refRun_MaxDist = ValToDB4(pCANObjConfMotor->lRefRunMaxDistance);
                refrun.refRun_HighSpeed = ValToDB2(pCANObjConfMotor->sRefRunHighSpeed);
            }
            break;
        case REFRUN3:
            {
                ConfigData_REFRUN_P3_t &refrun = configData->part.refRun3;
                canmsg.can_dlc = sizeof(ConfigData_REFRUN_P3_t) + sizeof(SubIndex_t);

                refrun.refRun_ReverseDist = ValToDB4(pCANObjConfMotor->lRefRunReverseDistance);
                refrun.refRun_LowSpeed = ValToDB2(pCANObjConfMotor->sRefRunSlowSpeed);
            }
            break;
        case TMC26x_DRVCONF:
            {
                ConfigData_DRV_TMC26X_t &tmc26x = configData->part.drvTMC26x;
                canmsg.can_dlc = sizeof(ConfigData_DRV_TMC26X_t) + sizeof(SubIndex_t);

                tmc26x.regVal = ValToDB3(pCANObjConfMotor->tmc26x.drvConf);
            }
            break;
        case TMC26x_SGCSCONF:
            {
                ConfigData_DRV_TMC26X_t &tmc26x = configData->part.drvTMC26x;
                canmsg.can_dlc = sizeof(ConfigData_DRV_TMC26X_t) + sizeof(SubIndex_t);

                tmc26x.regVal = ValToDB3(pCANObjConfMotor->tmc26x.sgcsConf);
            }
            break;
        case TMC26x_SMARTEN:
            {
                ConfigData_DRV_TMC26X_t &tmc26x = configData->part.drvTMC26x;
                canmsg.can_dlc = sizeof(ConfigData_DRV_TMC26X_t) + sizeof(SubIndex_t);

                tmc26x.regVal = ValToDB3(pCANObjConfMotor->tmc26x.smartEn);
            }
            break;
        case TMC26x_CHOPCONF:
            {
                ConfigData_DRV_TMC26X_t &tmc26x = configData->part.drvTMC26x;
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
        case CANFctModuleStepperMotor::DRIVER_TMC26X:
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
            if (int(m_subIndex.param.index) < int(REFRUN3))
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

        //position the iterator to the motion profile whichs data should be transmitted
        iter = pCANObjConfMotor->listMotionProfiles.constBegin();
        for(idx = 0; idx < m_subIndex.profile.no; idx++)
        {
            iter++;
        }

        if(iter == pCANObjConfMotor->listMotionProfiles.constEnd())
        {
            m_subStateConfig = FM_SM_SUB_STATE_CONFIG_FINISHED;
        }
        else
        {
            const CANFctModuleMotionProfile &MotionProfile = iter.value();
            switch (m_subIndex.profile.index)
            {
            case P1:
                {
                    ProfileData_P1_t &prof1 = configData->part.prof1;
                    canmsg.can_dlc = sizeof(ProfileData_P1_t) + sizeof(SubIndex_t);

                    prof1.microsteps = MotionProfile.bMicroSteps;
//                    prof2.rampType = pMotionProfile->bRampType;
                    prof1.minSpeed = ValToDB2(MotionProfile.sSpeedMin);
                    prof1.targetSpeed  = ValToDB2(MotionProfile.sSpeedMax);
                }
                break;
            case P2:
                {
                    ProfileData_P2_t &prof2 = configData->part.prof2;
                    canmsg.can_dlc = sizeof(ProfileData_P2_t) + sizeof(SubIndex_t);

                    prof2.acceleration = ValToDB2(MotionProfile.sAcc);
                    prof2.accJerkTime = ValToDB2(MotionProfile.sAccTime);
                }
                break;
            case P3:
                {
                    ProfileData_P3_t &prof3 = configData->part.prof3;
                    canmsg.can_dlc = sizeof(ProfileData_P3_t) + sizeof(SubIndex_t);

                    prof3.deceleration = ValToDB2(MotionProfile.sDec);
                    prof3.decJerkTime = ValToDB2(MotionProfile.sDecTime);
                }
                break;
            default:
                Q_ASSERT(false);
            }

            //prepare for next profile
            if (int(m_subIndex.profile.index) < int(P3))
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

    QMutableListIterator<ModuleCommand_t *> Iterator(m_ModuleCommand);
    while(Iterator.hasNext())
    {
        ModuleCommand_t *p_ModuleCommand = Iterator.next();
        bool RemoveCommand = false;

        if(p_ModuleCommand->State == MODULE_CMD_STATE_REQ)
        {
            // forward the motor command to the motor function module on slave side by sending
            // the corresponding CAN-message
            p_ModuleCommand->State = MODULE_CMD_STATE_REQ_SEND;
            p_ModuleCommand->ReqSendTime.Trigger();

            if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_STATE)
            {
                //send the state set command to the module, this command will be acknowledged by the receiption
                // of the m_unCanIDStateSetAck CAN-message
                FILE_LOG_L(laFCT, llINFO) << "   CANStepperMotor set motor state";
                RetVal = SendCANMsgSetState(p_ModuleCommand->MotorState);
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_STEPPERMOTOR_TIMEOUT_SET_STATE;
                }
                else
                {
                    emit ReportSetStateAckn(GetModuleHandle(), RetVal);
                }
            }
            else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_REFRUN)
            {
                //send the reference run request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDReferenceMovementAckn CAN-message
                FILE_LOG_L(laFCT, llINFO) << "   CStepperMotor::HandleReferenceMovement: send reference movement request.";
                RetVal = SendCANMsgReferenceMovement();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_STEPPERMOTOR_TIMEOUT_REFMOVE_REQ;
                }
                else
                {
                    emit ReportReferenceMovementAckn(GetModuleHandle(), RetVal, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_POS)
            {
                //send the target position to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDMovementAckn CAN-message
                RetVal = SendCANMsgTargetPosition(p_ModuleCommand->TargetPos,
                                                  p_ModuleCommand->MotionProfileIdx);
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_STEPPERMOTOR_TIMEOUT_POSMOVE_REQ;
                }
                else
                {
                    emit ReportMovementAckn(GetModuleHandle(), RetVal, 0, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_SPEED)
            {
                //send the target position to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDMovementAckn CAN-message
                RetVal = SendCANMsgTargetSpeed(p_ModuleCommand->TargetSpeed,
                                               p_ModuleCommand->MotionProfileIdx);
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_STEPPERMOTOR_TIMEOUT_SPEEDMOVE_REQ;
                }
                else
                {
                    emit ReportMovementAckn(GetModuleHandle(), RetVal, 0, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_ACTPOS_REQ)
            {
                //send the actual position request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDActPositionResp CAN-message
                RetVal = SendCANMsgActPositionReq();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_STEPPERMOTOR_TIMEOUT_ACTPOS_REQ;
                }
                else
                {
                    emit ReportPosition(GetModuleHandle(), RetVal, 0, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_ACTSPEED_REQ)
            {
                //send the actual speed request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDActSpeed CAN-message
                RetVal = SendCANMsgActSpeedReq();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_STEPPERMOTOR_TIMEOUT_ACTSPD_REQ;
                }
                else
                {
                    emit ReportSpeed(GetModuleHandle(), RetVal, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_OPTIME_DATA_REQ)
            {
                //send the actual speed request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDActSpeed CAN-message.
                RetVal = SendCANMsgOperationTimeDataReq();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_STEPPERMOTOR_TIMEOUT_LIFECYCLEDATA_REQ;
                }
                else
                {
                    emit ReportLifeCycleData(GetModuleHandle(), RetVal, 0, 0, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_REVCOUNT_DATA_REQ)
            {
                //send the actual speed request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDActSpeed CAN-message.
                RetVal = SendCANMsgRevolutionCountDataReq();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_STEPPERMOTOR_TIMEOUT_LIFECYCLEDATA_REQ;
                }
                else
                {
                    emit ReportLifeCycleData(GetModuleHandle(), RetVal, 0, 0, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_REQ_DATA_RESET)
            {
                RetVal = SendCANMsgReqDataReset();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_STEPPERMOTOR_TIMEOUT_LIFECYCLEDATA_REQ;
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
            // check active motor commands for timeout
            if(p_ModuleCommand->ReqSendTime.Elapsed() > p_ModuleCommand->Timeout)
            {
                RemoveCommand = true;

                if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_STATE)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANStepperMotor '" << GetKey().toStdString() << "': SetState timeout error.";
                    emit ReportSetStateAckn(GetModuleHandle(), DCL_ERR_TIMEOUT);
                }
                else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_REFRUN)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANStepperMotor '" << GetKey().toStdString() << "': reference run request timeout error.";
                    emit ReportReferenceMovementAckn(GetModuleHandle(), DCL_ERR_TIMEOUT, 0);
                }
                else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_POS)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANStepperMotor '" << GetKey().toStdString() << "': positioning timeout error.";
                    emit ReportMovementAckn(GetModuleHandle(), DCL_ERR_TIMEOUT, 0, 0);
                }
                else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_SPEED)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANStepperMotor '" << GetKey().toStdString() << "': speed timeout error.";
                    emit ReportMovementAckn(GetModuleHandle(), DCL_ERR_TIMEOUT, 0, 0);
                }
                else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_ACTPOS_REQ)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANStepperMotor '" << GetKey().toStdString() << "': act pos req. timeout error.";
                    emit ReportPosition(GetModuleHandle(), DCL_ERR_TIMEOUT, 0, 0);
                }
                else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_ACTSPEED_REQ)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANStepperMotor '" << GetKey().toStdString() << "': act speed req. timeout error.";
                    emit ReportSpeed(GetModuleHandle(), DCL_ERR_TIMEOUT, 0);
                }
                else if(p_ModuleCommand->Type == FM_SM_CMD_TYPE_REQ_DATA_RESET)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANStepperMotor '" << GetKey().toStdString() << "': ReqDataReset timeout error.";
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
        quint32 eventClass = (pCANframe->can_id & 0x1ff80000);  // unmask command class and code
        std::string eventString;
        if (quint16(GetType()) == m_lastErrorGroup)
            eventString = m_eventString[eventClass | m_lastErrorCode];
        else
            eventString = m_pParent->m_eventString[m_lastErrorCode];
        if ("" == eventString)
            eventString = "unknown";
        FILE_LOG_L(laFCT, llERROR) << " " << eventString;

        if ((pCANframe->can_id == m_unCanIDEventError) || (pCANframe->can_id == m_unCanIDEventFatalError)) {
            emit ReportError(GetModuleHandle(), m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
    }
    else if(m_unCanIDAcknDataReset == pCANframe->can_id)
    {
        ResetModuleCommand(FM_SM_CMD_TYPE_REQ_DATA_RESET);
        HandleCANMsgAcknDataReset(pCANframe);
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
    else if(m_unCanIDOpTimeData == pCANframe->can_id)
    {
        HandleCANMsgOperationTimeData(pCANframe);
    }
    else if(m_unCanIDRevCountData == pCANframe->can_id)
    {
        HandleCANMsgRevolutionsData(pCANframe);
    }
    else if(m_unCanIDActSpeed == pCANframe->can_id)
    {
        HandleCANMsgActSpeedResp(pCANframe);
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
        Msg_RefRunReqAckData_t *ackData = (Msg_RefRunReqAckData_t*)pCANframe->data;

        SM_AckState_t ack = ackData->ack;

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
    if(MSG_SMOT_REFERENCE_RUN_ACK_DLC == pCANframe->can_dlc)
    {
        Msg_RefRunAckData_t *ackData = (Msg_RefRunAckData_t*)pCANframe->data;

        Position_t position = DB4ToVal(ackData->pos);
        quint8 posCode = ackData->posCode;
        SM_AckState_t ack = ackData->ack;

        FILE_LOG_L(laFCT, llDEBUG1) << "  CStepperMotor::reference movement done: '" << GetKey().toStdString() << "',  Position:" << position << "' LimitSwitches:" << (int)posCode << "' Ack:" << ack;
        Q_ASSERT(m_pCANObjectConfig);
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
        Msg_TargetPosAckData_t *ackData = (Msg_TargetPosAckData_t*)pCANframe->data;

        SM_AckState_t ack = ackData->ack;

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
        Msg_TargetSpeedAckData_t *ackData = (Msg_TargetSpeedAckData_t*)pCANframe->data;

        SM_AckState_t ack = ackData->ack;

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
        Msg_MovementAckData_t *ackData = (Msg_MovementAckData_t*)pCANframe->data;

        SM_AckState_t ack   = ackData->ack;
        Position_t Position = DB4ToVal(ackData->pos);
        qint16 Speed        = DB2ToVal(ackData->speed);

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
        Msg_PositionData_t *posData = (Msg_PositionData_t*)pCANframe->data;

        SM_AckState_t ack   = posData->ack;
        Position_t position = DB4ToVal(posData->pos);
        qint8 posCode = posData->posCode;

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
        Msg_SpeedData_t *speedData = (Msg_SpeedData_t*)pCANframe->data;

        SM_AckState_t ack   = speedData->ack;
        Speed_t Speed = DB2ToVal(speedData->speed);

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
 *  \brief  Handles the reception of the CAN message 'Operation time'
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CStepperMotor::HandleCANMsgOperationTimeData(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_SM_CMD_TYPE_OPTIME_DATA_REQ);
    }

    if(MSG_SMOT_OPTIME_DLC == pCANframe->can_dlc)
    {
        Msg_OperationTimeData_t *OpTimeData = (Msg_OperationTimeData_t*)pCANframe->data;

        SM_AckState_t ack   = OpTimeData->ack;
        m_OperationTime = DB4ToVal(OpTimeData->hours);

        FILE_LOG_L(laFCT, llDEBUG1) << " CANStepperMotor-LiveCyleData: Time: " << m_RevolutionCount <<  ", " << m_TripCounter << ", " << m_OperationTime;

        emit ReportLifeCycleData(GetModuleHandle(), (SM_ACK==ack ? DCL_ERR_FCT_CALL_SUCCESS : DCL_ERR_EXTERNAL_ERROR), m_RevolutionCount, m_TripCounter, m_OperationTime);
    }
    else
    {
        emit ReportLifeCycleData(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0, 0);
    }
}


/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'revolution count'
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CStepperMotor::HandleCANMsgRevolutionsData(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_SM_CMD_TYPE_REVCOUNT_DATA_REQ);
    }

    if(MSG_SMOT_REVCOUNT_DLC == pCANframe->can_dlc)
    {
        Msg_RevolutionsData_t *RevolutionsData = (Msg_RevolutionsData_t*)pCANframe->data;

        SM_AckState_t ack   = RevolutionsData->ack;
        m_RevolutionCount = DB4ToVal(RevolutionsData->count);

        FILE_LOG_L(laFCT, llDEBUG1) << " CANStepperMotor-LiveCyleData: Time: " << m_RevolutionCount <<  ", " << m_TripCounter << ", " << m_OperationTime;

        emit ReportLifeCycleData(GetModuleHandle(), (SM_ACK==ack ? DCL_ERR_FCT_CALL_SUCCESS : DCL_ERR_EXTERNAL_ERROR), m_RevolutionCount, m_TripCounter, m_OperationTime);
    }
    else
    {
        emit ReportLifeCycleData(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0, 0);
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

    Msg_EnableData_t *stateData = (Msg_EnableData_t*)canmsg.data;

    canmsg.can_id = m_unCanIDStateSet;
    canmsg.can_dlc = MSG_SMOT_SET_ENABLE_DLC;

    if(MotorState)
    {
        stateData->enable = true;
        // TODO: for reconfiguration testing: no need for ReferenceRun when enabling FM
        stateData->dbg_skipRefRun = true;
    }
    else
    {
        stateData->enable = false;
        stateData->dbg_skipRefRun = false;
    }
    stateData->reserved = 0;

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

    Msg_RefRunData_t *refRunData = (Msg_RefRunData_t*)canmsg.data;

    canmsg.can_id = m_unCanIDReferenceMovement;
    canmsg.can_dlc = MSG_SMOT_REQ_REF_RUN_DLC;

    refRunData->profile = 0; // use profile 0 for reference run

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
 *
 *  \param    TargetPos = Position where the motor shall drive to
 *  \param    MotionProfile = Motion profile for the movement
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *            otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::SendCANMsgTargetPosition(Position_t TargetPos,
                                                     quint8 MotionProfile)
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDTargetPos;
    canmsg.can_dlc = MSG_SMOT_TARGET_POS_DLC;

    Msg_TargetPositionData_t *posData = (Msg_TargetPositionData_t*)canmsg.data;

    posData->pos = ValToDB4(TargetPos);
    posData->profile = MotionProfile;

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
 *
 *  \param    TargetSpeed    = Position where the motor shall drive to
 *  \param    MotionProfile  = Motion profile for the movement
 *
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *            otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::SendCANMsgTargetSpeed(Position_t TargetSpeed,
                                                  quint8 MotionProfileIdx)
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    Msg_TargetSpeedData_t *speedData = (Msg_TargetSpeedData_t*)canmsg.data;

    canmsg.can_id = m_unCanIDTargetSpeed;
    canmsg.can_dlc = MSG_SMOT_TARGET_SPEED_DLC;

    // unit for speed is half-steps
    speedData->speed = ValToDB2(TargetSpeed);
    speedData->profile = MotionProfileIdx;

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
 *  \brief    Send the CAN message to request the motor operation time data
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transim queue
 *            otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::SendCANMsgOperationTimeDataReq()
{
    ReturnCode_t retval;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDOpTimeDataReq;
    canmsg.can_dlc = MSG_SMOT_OPTIME_REQ_DLC;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG2) << "   CStepperMotor::SendCANMsgOperationTimeDataReq canID: 0x" << std::hex << m_unCanIDOpTimeDataReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief    Send the CAN message to request the motor revolution count data
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transim queue
 *            otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::SendCANMsgRevolutionCountDataReq()
{
    ReturnCode_t retval;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDRevCountDataReq;
    canmsg.can_dlc = MSG_SMOT_REVCOUNT_REQ_DLC;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG2) << "   CStepperMotor::SendCANMsgRevolutionCountDataReq canID: 0x" << std::hex << m_unCanIDRevCountDataReq;

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

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_SM_CMD_TYPE_STATE);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->MotorState = Enabled;
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

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_SM_CMD_TYPE_REFRUN);
    if(p_ModuleCommand == NULL)
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
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::DriveToPosition(quint32 TargetPosition, quint8 MotionProfileIdx)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_SM_CMD_TYPE_POS);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->TargetPos = TargetPosition;
        p_ModuleCommand->TargetSpeed = 0;
        p_ModuleCommand->MotionProfileIdx = MotionProfileIdx;
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
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::DriveSpeed(qint16 TargetSpeed, quint8 MotionProfileIdx)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_SM_CMD_TYPE_SPEED);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->TargetPos = 0;
        p_ModuleCommand->TargetSpeed = TargetSpeed;
        p_ModuleCommand->MotionProfileIdx = MotionProfileIdx;
        FILE_LOG_L(laDEV, llINFO) << " CANStepperMotor, Sp: " << (int) TargetSpeed;
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

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_SM_CMD_TYPE_ACTPOS_REQ);
    if(p_ModuleCommand == NULL)
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

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_SM_CMD_TYPE_ACTSPEED_REQ);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANStepperMotor invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Request the motor operation time
 *            The request will be acknowledged by sending the signal ReportLifeCycleData
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::RequestOperationTimeData()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_SM_CMD_TYPE_OPTIME_DATA_REQ);
    if(p_ModuleCommand != NULL)
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
 *  \brief    Request the motor revolution count
 *            The request will be acknowledged by sending the signal ReportLifeCycleData
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *            otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::RequestRevolutionCountData()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_SM_CMD_TYPE_REVCOUNT_DATA_REQ);
    if(p_ModuleCommand != NULL)
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
 *  \brief  Request a data reset
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CStepperMotor::ReqDataReset()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_SM_CMD_TYPE_REQ_DATA_RESET);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CStepperMotor '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
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
CStepperMotor::ModuleCommand_t *CStepperMotor::SetModuleTask(CANStepperMotorMotionCmdType_t CommandType)
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
 *  \iparam ModuleCommandType = Command of that type will be set to free
 */
/****************************************************************************/
void CStepperMotor::ResetModuleCommand(CANStepperMotorMotionCmdType_t CommandType)
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

    Q_ASSERT(m_pCANObjectConfig);
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


std::ostream& operator<< (std::ostream& s, const SM_AckState_t &ackState)
{
    switch (ackState)
    {
    case SM_ACK:
        s << "SM_ACK(success)";
        break;
    case SM_NACK:
        s << "SM_NACK(failed)";
        break;
    default:
        s << "unknown value for SM_AckState_t";
        break;
    }
    return s;
}


} //namespace
