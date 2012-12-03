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

typedef quint8 UInt8;
#include "FunctionModules/defStepperMotor.h"

inline Msg_DB2_t ValToDB2 (quint16 val)
{
    Msg_DB2_t db;
    db.hi = val >> 8;
    db.lo = val & 0xff;
    return db;
}

inline quint16 DB2ToVal (Msg_DB2_t db)
{
    return (db.hi << 8 | db.lo);
}

inline Msg_DB3_t ValToDB3 (quint32 val)
{
    Msg_DB3_t data;
    data.db_2 = (val >> 16) & 0xff;
    data.db_1 = (val >> 8) & 0xff;
    data.db_0 = val & 0xff;
    return data;
}

inline Msg_DB4_t ValToDB4 (quint32 val)
{
    Msg_DB4_t data;
    data.db_3 = (val >> 24) & 0xff;
    data.db_2 = (val >> 16) & 0xff;
    data.db_1 = (val >> 8) & 0xff;
    data.db_0 = val & 0xff;
    return data;
}

inline quint32 DB4ToVal (Msg_DB4_t data)
{
    return (data.db_3 << 24 | data.db_2 << 16 | data.db_1 << 8 | data.db_0);
}


namespace DeviceControl
{

class CANCommunicator;

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

    //! Request motor operation time
    ReturnCode_t RequestOperationTimeData();

    //! Request motor revolution count
    ReturnCode_t RequestRevolutionCountData();

    //! motor positioning command
    ReturnCode_t DriveToPosition(quint32 TargetPosition,
                                 quint8  MovementProfile);

    //! motor speed command
    ReturnCode_t DriveSpeed(qint16  TargetSpeed,
                            quint8  MovementProfile);

    //! reference run command
    ReturnCode_t ExecReferenceMovement();

    //! Request data reset
    ReturnCode_t ReqDataReset();

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
     *  \param   Speed      = Actual motor speed [half steps / sec]
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
     *  \param   InstanceID = Instance identifier of this function module instance
     *  \param   HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \param   Speed      = Actual motor speed [half steps / sec]
     *
     ****************************************************************************/
    void ReportSpeed(quint32 InstanCANStepperMotorceID, ReturnCode_t HdlInfo, qint16 Speed);

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
                                          quint8 MotionProfile);
    //! sends the can message 'TargetSpeed'
    ReturnCode_t SendCANMsgTargetSpeed(Position_t TargetSpeed,
                                       quint8 MotionProfileIdx);
    //! sends the can message 'ActPositionRequest'
    ReturnCode_t SendCANMsgActPositionReq();
    //! sends the can message 'ActSpeedRequest'
    ReturnCode_t SendCANMsgActSpeedReq();
    //! sends the can message 'operation time request'
    ReturnCode_t SendCANMsgOperationTimeDataReq();
    //! sends the can message 'revolution count request'
    ReturnCode_t SendCANMsgRevolutionCountDataReq();

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
    //! handles the receipt of can message 'ActSpeed'
    void HandleCANMsgActSpeedResp(can_frame* pCANframe);
    //! handles the receipt of can message 'OperationTimeData'
    void HandleCANMsgOperationTimeData(can_frame* pCANframe);
    //! handles the receipt of can message 'RevolutionsData'
    void HandleCANMsgRevolutionsData(can_frame* pCANframe);

    //! command handling function
    void HandleCommandRequestTask();

    /*! configuration state definitions */
    typedef enum {
        FM_SM_SUB_STATE_CONFIG_INIT      = 0x00, ///< Initialisation state
        FM_SM_SUB_STATE_CONFIG_START     = 0x01, ///< Configuration start state
        FM_SM_SUB_STATE_CONFIG_FINISHED  = 0x0f  ///< Finish state
    } CANStepperMotorSubStateConfig_t;

    CANStepperMotorSubStateConfig_t m_subStateConfig;   ///< Motor configuration state machine control
    SubIndex_t                      m_subIndex;         ///< Motor configuration state machine control

    quint32  m_unCanIDConfig;                 ///< CAN message 'Configuration data'
    quint32  m_unCanIDStateSet;               ///< CAN message 'Set motor state'
    quint32  m_unCanIDStateSetAck;            ///< CAN message 'Set motor state acknowledge'
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
    quint32  m_unCanIDActSpeedReq;            ///< CAN message 'Actual speed request'
    quint32  m_unCanIDActSpeed;               ///< CAN message 'Actual speed'
    quint32  m_unCanIDOpTimeDataReq;          ///< CAN message 'Motor operation time request'
    quint32  m_unCanIDOpTimeData;             ///< CAN message 'Motor operation time data'
    quint32  m_unCanIDRevCountDataReq;        ///< CAN message 'Motor revolution count request'
    quint32  m_unCanIDRevCountData;           ///< CAN message 'Motor revolution count data'

    /*! motor movement command type */
    typedef enum {
        FM_SM_CMD_TYPE_UNDEF               = 0x00,  //!< undefined movement type
        FM_SM_CMD_TYPE_STATE               = 0x01,  //!< set motor state
        FM_SM_CMD_TYPE_REFRUN              = 0x02,  //!< reference run request
        FM_SM_CMD_TYPE_POS                 = 0x03,  //!< movement to target position
        FM_SM_CMD_TYPE_SPEED               = 0x04,  //!< movement with target speed
        FM_SM_CMD_TYPE_ACTPOS_REQ          = 0x05,  //!< actual position request
        FM_SM_CMD_TYPE_ACTSPEED_REQ        = 0x06,  //!< actual speed request
        FM_SM_CMD_TYPE_OPTIME_DATA_REQ     = 0x07,  //!< operation time data request
        FM_SM_CMD_TYPE_REVCOUNT_DATA_REQ   = 0x08,  //!< revolution count data request
        FM_SM_CMD_TYPE_REQ_DATA_RESET      = 0x09   //!< data reset
    } CANStepperMotorMotionCmdType_t;

    /*! motor command data, used for internal data transfer */
    typedef struct {
        CANStepperMotorMotionCmdType_t Type;                //!< command type
        ModuleCmdState_t               State;               //!< command state
        bool                           MotorState;          //!< motor state
        Position_t                     TargetPos;           //!< position [half-steps]
        qint16                         TargetSpeed;         //!< target speed [half-steps/sec]
        quint8                         MotionProfileIdx;    //!< index of movement profile
        Global::MonotonicTime          ReqSendTime;         //!< time the command was executed
        qint32                         Timeout;             //!< timeout in ms
    } ModuleCommand_t;

    QList<ModuleCommand_t *> m_ModuleCommand;   //!< Queue of module commands for simultaneous execution

    //! Adds the module command type to the transmit queue
    ModuleCommand_t *SetModuleTask(CANStepperMotorMotionCmdType_t CommandType);
    //! Clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANStepperMotorMotionCmdType_t CommandType);

    Global::MonotonicTime m_timeAction; ///< Action start time, for timeout detection

    //life cycle data
    quint32 m_RevolutionCount;      //!< count of motor revolutions during life cycle
    quint32 m_TripCounter;          //!< count of changes in direction during life cycle
    quint32 m_OperationTime;        //!< motor operation time in hours during life cycle


    static QMap<quint32, std::string> m_eventString;    //!< list with info strings for CAN events
};


std::ostream& operator<< (std::ostream& s, const SM_AckState_t &ackState);     //!< convert SM_AckState_t to string

} //namespace

#endif /* DEVICECONTROL_STEPPERMOTORPRIVATE_H */
