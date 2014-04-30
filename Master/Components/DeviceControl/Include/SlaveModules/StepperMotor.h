/******************************************************************/
/*! \file StepperMotor.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CStepperMotor
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
/******************************************************************/

#ifndef DEVICECONTROL_STEPPERMOTORPRIVATE_H
#define DEVICECONTROL_STEPPERMOTORPRIVATE_H

#include "DeviceControl/Include/SlaveModules/FunctionModule.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "Global/Include/MonotonicTime.h"

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of typedef UInt8
 */
/****************************************************************************/
typedef quint8 UInt8;
#include "FunctionModules/defStepperMotor.h"

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function ValToDB2
 *
 *  \param val = quint16 type parameter
 *
 *  \return from ValToDB2
 */
/****************************************************************************/
inline Msg_DB2_t ValToDB2 (quint16 val)
{
    Msg_DB2_t db;
    db.hi = val >> 8;
    db.lo = val & 0xff;
    return db;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function DB2ToVal
 *
 *  \param db = Msg_DB2_t type parameter
 *
 *  \return from DB2ToVal
 */
/****************************************************************************/
inline quint16 DB2ToVal (Msg_DB2_t db)
{
    return (db.hi << 8 | db.lo);
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function ValToDB3
 *
 *  \param val = quint32 type parameter
 *
 *  \return from ValToDB3
 */
/****************************************************************************/
inline Msg_DB3_t ValToDB3 (quint32 val)
{
    Msg_DB3_t data;
    data.db_2 = (val >> 16) & 0xff;
    data.db_1 = (val >> 8) & 0xff;
    data.db_0 = val & 0xff;
    return data;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function ValToDB4
 *
 *  \param val = quint32 type parameter
 *
 *  \return from ValToDB4
 */
/****************************************************************************/
inline Msg_DB4_t ValToDB4 (quint32 val)
{
    Msg_DB4_t data;
    data.db_3 = (val >> 24) & 0xff;
    data.db_2 = (val >> 16) & 0xff;
    data.db_1 = (val >> 8) & 0xff;
    data.db_0 = val & 0xff;
    return data;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function DB4ToVal
 *
 *  \param data = Msg_DB4_t type parameter
 *
 *  \return from DB4ToVal
 */
/****************************************************************************/
inline quint32 DB4ToVal (Msg_DB4_t data)
{
    return (data.db_3 << 24 | data.db_2 << 16 | data.db_1 << 8 | data.db_0);
}


namespace DeviceControl
{

class CANCommunicator;

#define MAX_SM_MODULE_CMD_IDX   4   ///< up to 4 motion commands can be handled simultaneously

/****************************************************************************/
/*!
 *  \brief  Stepper motor function module class
 *
 *      This class implements the functionality to configure and control a
 *      slave's 'stepper motor' function module
 */
/****************************************************************************/
class CStepperMotor : public CFunctionModule
{
    Q_OBJECT

public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CStepperMotor
     *
     *  \param p_MessageConfiguration
     *  \param pCANCommunicator
     *  \param pParentNode
     *
     *  \return from CStepperMotor
     */
    /****************************************************************************/
    CStepperMotor(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                  CBaseModule* pParentNode);
    ~CStepperMotor();

    ReturnCode_t Initialize();                    ///< initialise CAN-messages

    void HandleTasks();                           ///< Task handling function
    void HandleCanMessage(can_frame* pCANframe);  ///< CAN-message handling function

    ReturnCode_t DoReconfiguration(CANFctModuleStepperMotor *pConfig);  //!< Reconfigure the device

    /****************************************************************************/
    /*!
     *  \brief  Return pointer to current configuration
     *
     *  \return CANFctModuleStepperMotor*
     */
    /****************************************************************************/
    inline CANFctModuleStepperMotor* GetConfiguration()
    {
        return (CANFctModuleStepperMotor*) m_pCANObjectConfig;
    }

    //! Switch the motor state: on or off
    ReturnCode_t SetMotorState(bool Enabled);

    //! Request actual motor position
    ReturnCode_t ReqActPosition();

    //! Request actual motor speed
    ReturnCode_t ReqActSpeed();

    //! Request life cycle data
    ReturnCode_t RequestLifeCycleData();

    //! Request motor operation time
    ReturnCode_t RequestOperationTimeData();

    //! Request motor revolution count
    ReturnCode_t RequestRevolutionCountData();

    //! Request motor direction changes count
    ReturnCode_t RequestDirChangeCountData();

    //! motor positioning command
    ReturnCode_t DriveToPosition(quint32 TargetPosition,
                                 quint8  MovementProfile,
                                 StepperMotorSubCommandMotion_t SubCommandID = SM_SUBCMD_MOTION_NULL,
                                 quint16 SubCommandData = 0);
    //! motor speed command
    ReturnCode_t DriveSpeed(qint16  TargetSpeed,
                            quint8  MovementProfile,
                            StepperMotorSubCommandMotion_t SubCommandID = SM_SUBCMD_MOTION_NULL,
                            quint16 SubCommandData = 0);

    //! reference run command
    ReturnCode_t ExecReferenceMovement();

    //! calculate the target speed of a time period based movement
    Speed_t GetSpeedFromPeriod(Position_t Distance, MotionProfileIdx_t MotionProfileIdx, quint16 Period);

    /****************************************************************************/
    /*!
     *  \brief  Return the minimal position
     *
     *  \return The minimal position
     */
    /****************************************************************************/
    Position_t GetMinPosition() { return m_MinPosition; }

    /****************************************************************************/
    /*!
     *  \brief  Return the maximal position
     *
     *  \return The maximal position
     */
    /****************************************************************************/
    Position_t GetMaxPosition() { return m_MaxPosition; }

    /****************************************************************************/
    /*!
     *  \brief  Return the maximal speed
     *
     *  \return The maximal speed
     */
    /****************************************************************************/
    qint16 GetMaxSpeed() { return m_MaxSpeed; }

signals:
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to report the set motor state acknowledge
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *
     ****************************************************************************/
    void ReportSetStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to report the actual motor state
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   State      = Actual motor state
     *
     ****************************************************************************/
    void ReportState(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 State);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to report the reference movement acknowledge
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   Position   = Actual motor position [half steps]
     *
     ****************************************************************************/
    void ReportReferenceMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to acknowledge the reception of a position command on slave side
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *
     ****************************************************************************/
    void ReportPositionMovementCmdAckn(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to acknowledge the reception of a speed command on slave side
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *
     ****************************************************************************/
    void ReportSpeedMovementCmdAckn(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to acknowledge a completed movement
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   Position   = Actual motor position [half steps]
     *  \param   speed      = Actual motor speed [half steps / sec]
     *
     ****************************************************************************/
    void ReportMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position, qint16 speed);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to report the actual motor position
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   Position   = Actual motor position [half steps]
     *  \param   PosCode    = Actual limit switches position code
     *
     ****************************************************************************/
    void ReportPosition(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position, qint8 PosCode);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to report the actual motor speed
     *
     *  \param   InstanCANStepperMotorceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   Speed      = Actual motor speed [half steps / sec]
     *
     ****************************************************************************/
    void ReportSpeed(quint32 InstanCANStepperMotorceID, ReturnCode_t HdlInfo, qint16 Speed);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to report the limit switch state
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   LimitSwitchState = Limit switch state (Bit0 - LS1, Bit 1 - LS2, active (1), incative(0))
     *
     ****************************************************************************/
    void ReportLimitSwitchState(quint32 InstanceID, ReturnCode_t HdlInfo, qint8 LimitSwitchState);

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to report the motor's live cycle data
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   StepCounter = Number of revisions
     *  \param   TripCounter = Number of trips (cw-to-ccw and ccw-to-cw transitions)
     *  \param   OperationTime = Total on-times (minutes)
     *
     ****************************************************************************/
    void ReportLifeCycleData(quint32 InstanceID, ReturnCode_t HdlInfo, quint32 StepCounter, quint32 TripCounter, quint32 OperationTime);

    /****************************************************************************/
    /*!
     *  \brief   This signal is emitted to report accomplishment of configuration
     *
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *
     ****************************************************************************/
    void ReportConfigureDone(quint32 InstanceID, ReturnCode_t HdlInfo);

private:
    ReturnCode_t InitializeCANMessages();   //!< can message ID initialization
    ReturnCode_t RegisterCANMessages();     //!< registers the can messages to communication layer

    void HandleConfigurationState();        //!< configuration task handling function
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetupLimitSwitchConfigData
     *
     *  \param LimitSwitch = CANFctModuleLimitSwitch type parameter
     *  \param ls =  ConfigData_LS_t type parameter
     *
     *  \return from SetupLimitSwitchConfigData
     */
    /****************************************************************************/
    void SetupLimitSwitchConfigData (CANFctModuleLimitSwitch &LimitSwitch, ConfigData_LS_t &ls);    //! configure limit switch data
    void SetupPosCodeConfigData (CANFctModulePosCode &PosCode, ConfigData_LSPOS_t &pc);             //! configure position code data
    StepperMotorRotDir_t SetupRotationDir (CANFctModuleStepperMotor::RotationDir_t dir);            //! configure rotation dir

    void HandleIdleState();                 //!< idle task handling function

    //! sends the can message 'SetState'
    ReturnCode_t SendCANMsgSetState(bool MotorState);
    //! sends the can message 'ReferenceMovement'
    ReturnCode_t SendCANMsgReferenceMovement();
    //! sends the can message 'TargetPosition'
    ReturnCode_t SendCANMsgTargetPosition(Position_t TargetPos,
                                          quint8 MotionProfile,
                                          StepperMotorSubCommandMotion_t SubCommandID,
                                          quint16 SubCommandData);
    //! sends the can message 'TargetSpeed'
    ReturnCode_t SendCANMsgTargetSpeed(Position_t TargetSpeed,
                                       quint8 MotionProfileIdx,
                                       StepperMotorSubCommandMotion_t SubCommandID,
                                       quint16 SubCommandData);
    //! sends the can message 'ActPositionRequest'
    ReturnCode_t SendCANMsgActPositionReq();
    //! sends the can message 'ActSpeedRequest'
    ReturnCode_t SendCANMsgActSpeedReq();
    //! sends the can message 'operation time request'
    ReturnCode_t SendCANMsgOperationTimeDataReq();
    //! sends the can message 'revolution count request'
    ReturnCode_t SendCANMsgRevolutionCountDataReq();
    //! sends the can message 'direction changes count request'
    ReturnCode_t SendCANMsgDirChangesCountDataReq();

    //! handles the receipt of can message 'State'
    void HandleCANMsgState(can_frame* pCANframe);
    //! handles the receipt of can message 'SetStateAck'
    void HandleCANMsgSetStateAck(can_frame* pCANframe);
    //! handles the receipt of can message 'ReferenceMovementReqAckn'
    void HandleCANMsgReferenceMovementReqAckn(can_frame* pCANframe);
    //! handles the receipt of can message 'ReferenceMovementAckn'
    void HandleCANMsgReferenceMovementAckn(can_frame* pCANframe);
    //! handles the receipt of can message 'PositionCommandAckn'
    void HandleCANMsgTargetPosCmdAckn(can_frame* pCANframe);
    //! handles the receipt of can CANStepperMotormessage 'SpeedCommandAckn'
    void HandleCANMsgTargetSpeedCmdAckn(can_frame* pCANframe);
    //! handles the receipt of can message 'MovementAckn'
    void HandleCANMsgMovementAckn(can_frame* pCANframe);
    //! handles the receipt of can message 'ActPosition'
    void HandleCANMsgActPositionResp(can_frame* pCANframe);
    //! handles the receipt of can message 'Debug'
    void HandleCANMsgDebug(can_frame* pCANframe);
    //! handles the receipt of can message 'LiveCycleData'
    //void HandleCANMsgLiveCycleData(can_frame* pCANframe);
    //! handles the receipt of can message 'ActSpeed'
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HandleCANMsgActSpeedResp
     *
     *  \param pCANframe
     */
    /****************************************************************************/
    void HandleCANMsgActSpeedResp(can_frame* pCANframe);
    //! handles the receipt of can message 'OperationTimeData'
    void HandleCANMsgOperationTimeData(can_frame* pCANframe);
    //! handles the receipt of can message 'RevolutionsData'
    void HandleCANMsgRevolutionsData(can_frame* pCANframe);
    //! handles the receipt of can message 'DirectionChangesData'
    void HandleCANMsgDirChangesData(can_frame* pCANframe);
    //! command handling function
    void HandleCommandRequestTask();

    /*! configuration state definitions */
    typedef enum {
        FM_SM_SUB_STATE_CONFIG_INIT      = 0x00, ///< Initialisation state
        FM_SM_SUB_STATE_CONFIG_START     = 0x01, ///< Configuration start state
        FM_SM_SUB_STATE_CONFIG_LS1_1     = 0x02, ///< Limit switch 1 configuration data block 1
        FM_SM_SUB_STATE_CONFIG_LS1_2     = 0x03, ///< Limit switch 1 configuration data block 2
        FM_SM_SUB_STATE_CONFIG_LS2_1     = 0x04, ///< Limit switch 2 configuration data block 1
        FM_SM_SUB_STATE_CONFIG_LS2_2     = 0x05, ///< Limit switch 2 configuration data block 2
        FM_SM_SUB_STATE_CONFIG_ENC       = 0x06, ///< Encoder configuration data
        FM_SM_SUB_STATE_CONFIG_MOTOR_1   = 0x07, ///< Motor configuration data, part 1
        FM_SM_SUB_STATE_CONFIG_MOTOR_2   = 0x08, ///< Motor configuration data, part 2
        FM_SM_SUB_STATE_CONFIG_MOTOR_3   = 0x09, ///< Motor configuration data, part 3
        FM_SM_SUB_STATE_CONFIG_MOTOR_4   = 0x0a, ///< Motor configuration data, part 4
        FM_SM_SUB_STATE_CONFIG_MOTOR_5   = 0x0b, ///< Motor configuration data, part 5
        FM_SM_SUB_STATE_CONFIG_MOTOR_6   = 0x0c, ///< Motor configuration data, part 6
        FM_SM_SUB_STATE_CONFIG_MOTOR_7   = 0x0d, ///< Motor configuration data, part 7
        FM_SM_SUB_STATE_CONFIG_MOTOR_PROFILE = 0x0e, ///< Motor movement profile data,
        FM_SM_SUB_STATE_CONFIG_FINISHED  = 0x0f  ///< Finish state
    } CANStepperMotorSubStateConfig_t;

    CANStepperMotorSubStateConfig_t m_subStateConfig;   ///< Motor configuration state machine control
    SubIndex_t m_subIndex;  ///< Motor configuration state machine control

    quint32  m_unCanIDError;                  ///< CAN message 'Error'
    quint32  m_unCanIDErrorReq;               ///< CAN message 'Request error'
    quint32  m_unCanIDStateSet;               ///< CAN message 'Set motor state'
    quint32  m_unCanIDStateSetAck;            ///< CAN message 'Set motor state acknowledge'
    quint32  m_unCanIDStateReq;               ///< CAN message 'Request motor state'
    quint32  m_unCanIDState;                  ///< CAN  dmessage 'Motor state'
    quint32  m_unCanIDReferenceMovement;      ///< CAN message 'Reference run'
    quint32  m_unCanIDReferenceMovementReqAckn;  ///< CAN message 'Reference run request acknowledge'
    quint32  m_unCanIDReferenceMovementAckn;  ///< CAN message 'Reference run acknowledge'
    quint32  m_unCanIDTargetPos;              ///< CAN message 'Target position'
    quint32  m_unCanIDTargetPosCmdAckn;       ///< CAN message 'Target position command acknowledge'
    quint32  m_unCanIDTargetSpeed;            ///< CAN message 'Target speed'
    quint32  m_unCanIDTargetSpeedCmdAckn;     ///< CAN message 'Target speed command acknowledge'
    quint32  m_unCanIDMovementAckn;           ///< CAN message 'Position or speed movement acknowledge'
    quint32  m_unCanIDActPositionReq;         ///< CAN message 'Actual position request'
    quint32  m_unCanIDActPositionResp;        ///< CAN message 'Actual position'
    quint32  m_unCanIDActSpeed;               ///< CAN message 'Actual speed'
    quint32  m_unCanIDActSpeedReq;            ///< CAN message 'Actual speed request'
    quint32  m_unCanIDConfig;                 ///< CAN message 'Configuration data'
    quint32  m_unCanIDMotionProfile;          ///< CAN message 'Movement configuration data'
    quint32  m_unCanIDDiagSoftwareReq;        ///< CAN message 'Diagnostic Software request'
    quint32  m_unCanIDDiagSoftware;           ///< CAN message 'Diagnostic Software'
    quint32  m_unCanIDDiagHardwareReq;        ///< CAN message 'Diagnostic Hardware request'
    quint32  m_unCanIDDiagHardware;           ///< CAN message 'Diagnostic Hardware'
    quint32  m_unCanIDDebug;                  ///< CAN message 'Debug data'
    quint32  m_unCanIDDebug2;                 ///< CAN message 'Debug2 data'
    quint32  m_unCanIDOpTimeDataReq;          ///< CAN message 'Motor operation time request'
    quint32  m_unCanIDOpTimeData;             ///< CAN message 'Motor operation time data'
    quint32  m_unCanIDRevCountDataReq;        ///< CAN message 'Motor revolution count request'
    quint32  m_unCanIDRevCountData;           ///< CAN message 'Motor revolution count data'
    quint32  m_unCanIDDirCountDataReq;        ///< CAN message 'Motor direction changes count request'
    quint32  m_unCanIDDirCountData;           ///< CAN message 'Motor direction changes count data'

    /*! motor movement command type */
    typedef enum {
        FM_SM_CMD_TYPE_UNDEF               = 0x00,  //!< undefined movement type
        FM_SM_CMD_TYPE_STATE               = 0x01,  //!< set motor state
        FM_SM_CMD_TYPE_REFRUN              = 0x02,  //!< reference run request
        FM_SM_CMD_TYPE_POS                 = 0x03,  //!< movement to target position
        FM_SM_CMD_TYPE_SPEED               = 0x04,  //!< movement with target speed
        FM_SM_CMD_TYPE_ACTPOS_REQ          = 0x05,  //!< actual position request
        FM_SM_CMD_TYPE_ACTSPEED_REQ        = 0x06,  //!< actual speed request
        FM_SM_CMD_TYPE_LIFE_CYCLE_DATA_REQ = 0x07,  //!< life cycle data request
        FM_SM_CMD_TYPE_TECH_DATA1_REQ      = 0x08,  //!< technical data group 1 request
        FM_SM_CMD_TYPE_TECH_DATA2_REQ      = 0x09,  //!< technical data group 1 request
        FM_SM_CMD_TYPE_DEBUG_CMD_1         = 0x0a,  //!< debug request
        FM_SM_CMD_TYPE_OPTIME_DATA_REQ     = 0x0b,  //!< operation time data request
        FM_SM_CMD_TYPE_REVCOUNT_DATA_REQ   = 0x0c,  //!< revolution count data request
        FM_SM_CMD_TYPE_DIRCOUNT_DATA_REQ   = 0x0d  //!< direction change count data request
    } CANStepperMotorMotionCmdType_t;

    /*! motor command data, used for internal data transfer */
    typedef struct {
        CANStepperMotorMotionCmdType_t  Type;               //!< command type
        ModuleCmdState_t                State;              //!< command state
        bool                            m_MotorState;       //!< motor state
        Position_t                      TargetPos;          //!< position [half-steps]
        qint16                          TargetSpeed;        //!< target speed [half-steps/sec]
        quint8                          MotionProfileIdx;   //!< index of movement profile
        StepperMotorSubCommandMotion_t  SubCommandID;       //!< sub command id for further motion options
        quint16                         SubCommandData;     //!< additional data, depending on subcommand
        Global::MonotonicTime           m_ReqSendTime;      //!< time the command wasexecuted
        qint32                          m_Timeout;          //!< timeout in ms
        quint8                          m_TimeoutRetry;     //!< timeout retry time
    } ModuleCommand_t;

    //! module command array
    ModuleCommand_t m_ModuleCommand[MAX_SM_MODULE_CMD_IDX];

    //! set the module command type to free entry within array
    bool SetModuleTask(CANStepperMotorMotionCmdType_t CommandType, quint8* pCmdIndex = 0);
    //! clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANStepperMotorMotionCmdType_t CommandType);

    quint32 m_ReqTargetPosition;    ///< Requested target postion
    qint16 m_ReqTargetSpeed;        ///< Requested target speed
    quint8 m_ReqMovementProfile;    ///< Requested movement profile
    quint8 m_ReqSubCommandID;       ///< Requested sub command ID (e.g. time delay)
    qint16 m_ReqSubCommandData;     ///< Requested sun command data


    Global::MonotonicTime m_timeAction; ///< Action start time, for timeout detection
    qint16 m_aktionTimespan;            ///< Delay im ms, for timeout detection

    quint8 MotionProfileIndex;      ///< Motion profile index, used by configuration
    quint8 MotionProfileSubIndex;   ///< Motion profile sub index, used by configuration

    bool m_MotorState;  //!< motor state (enabled/disabled)
#if 0
    //life cycle data
    quint32 m_StepCounter;      //!< count of motor step during life cycle
    quint32 m_TripCounter;      //!< count of changes in direction during life cycle
    quint32 m_OperationTime;    //!< motor operation time in minutes during life cycle
#endif
    //configuration data
    Position_t m_MinPosition;   ///< minimal position
    Position_t m_MaxPosition;   ///< maximal position
    qint16 m_MaxSpeed;          ///< maximal speed

    //life cycle data
    quint32 m_RevolutionCount;      //!< count of motor revolutions during life cycle
    quint32 m_DirChangeCount;       //!< count of changes in direction during life cycle
    quint32 m_OperationTime;        //!< motor operation time in hours during life cycle

    static QMap<quint32, std::string> m_eventString;    //!< list with info strings for CAN events
};

} //namespace

#endif /* DEVICECONTROL_STEPPERMOTORPRIVATE_H */
