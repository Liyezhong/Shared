/******************************************************************/
/*! \file TemperatureControl.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CTemperatureControl
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

#ifndef DEVICECONTROL_TEMPERATURECONTROLPRIVATE_H
#define DEVICECONTROL_TEMPERATURECONTROLPRIVATE_H

#include "DeviceControl/Include/SlaveModules/FunctionModule.h"
#include "Global/Include/MonotonicTime.h"

namespace DeviceControl
{

class CANCommunicator;

#define MAX_TEMP_MODULE_CMD_IDX 4   ///< up to 4 module commands can be handled simultaneously

/****************************************************************************/
/*!
 *  \brief This class implements the functionality to configure and control a
 *         slave's 'temperature control' function module
 */
/****************************************************************************/
class CTemperatureControl : public CFunctionModule
{
    Q_OBJECT

 public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CTemperatureControl
     *
     *  \param  p_MessageConfiguration
     *  \param  pCANCommunicator
     *  \param  pParentNode
     *
     *  \return from CTemperatureControl
     */
    /****************************************************************************/
    CTemperatureControl(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                        CBaseModule* pParentNode);
    ~CTemperatureControl();

    ReturnCode_t Initialize();  ///< Initialize CAN-messages

    void HandleTasks(); ///< Task handling function
    void HandleCanMessage(can_frame* pCANframe);    ///< CAN-message handling function

    //! Set reference temperature
    ReturnCode_t SetTemperature(qreal Temperature, quint8 SlopeTempChange=0);
    //! Request actual temperature
    ReturnCode_t ReqActTemperature(quint8 Index);
    //! Set temperature ctrl. status
    ReturnCode_t SetStatus(TempCtrlStatus_t TempCtrlState);
    //! Request temperature ctrl. status
    ReturnCode_t ReqStatus();
    //! Set temperature ctrl. operating mode
    ReturnCode_t SetOperatingMode(TempCtrlOperatingMode_t TempCtrlOpMode);
    //! Request temperature ctrl. operating mode
    ReturnCode_t ReqOperatingMode();
    //! Reset the heater operating time
    ReturnCode_t ResetHeaterOperatingTime(quint8 Index);
    //! Get the heater operating time in seconds
    ReturnCode_t GetHeaterOperatingTime(quint8 Index);
    //! Get the fan speed in revelations per minute
    ReturnCode_t GetFanSpeed(quint8 Index);
    //! Get the hardware status
    ReturnCode_t GetHardwareStatus();
    //! Set temperature ctrl. status
    ReturnCode_t SetTemperaturePid(quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetSwitchState
     *
     *  \param SwitchState = qint8 type parameter
     *  \param AutoSwitch =  qint8 type parameter
     *
     *  \return from SetSwitchState
     */
    /****************************************************************************/
    ReturnCode_t SetSwitchState(qint8 SwitchState, qint8 AutoSwitch);


signals:
    /****************************************************************************/
    /*!
     *  \brief  This signal reports the actual temperature
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Index = Index of the temperature sensor
     *  \iparam Temperature = Actual temperature
     *
     ****************************************************************************/
    void ReportActTemperature(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index, qreal Temperature);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the actual reference temperature
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Temperature = Reference temperature
     */
    /****************************************************************************/
    void ReportRefTemperature(quint32 InstanceID, ReturnCode_t HdlInfo, qreal Temperature);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the actual temperature ctrl. status
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Status = Temperature ctrl. status
     *  \iparam Voltage = Temperature ctrl. voltage
     */
    /****************************************************************************/
    void ReportActStatus(quint32 InstanceID, ReturnCode_t HdlInfo, TempCtrlStatus_t Status, TempCtrlMainsVoltage_t Voltage);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the acknowledge for status setting
     *          Report Acknowlege for SetStatus(..) - request
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Status = Temperature ctrl. status
     */
    /****************************************************************************/
    void ReportSetStatusAckn(quint32 InstanceID, ReturnCode_t HdlInfo, TempCtrlStatus_t Status);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the actual operation mode
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam OperatingMode = Operation mode
     */
    /****************************************************************************/
    void ReportActOperatingMode(quint32 InstanceID, ReturnCode_t HdlInfo, TempCtrlOperatingMode_t OperatingMode);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the acknwoledge for operation mode setting
     *          Report Acknowlege for SetOperatingMode(..) - request
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam OperatingMode = Operation mode
     */
    /****************************************************************************/
    void ReportSetOperatingModeAckn(quint32 InstanceID, ReturnCode_t HdlInfo, TempCtrlOperatingMode_t OperatingMode);

    /****************************************************************************/
    /*!
     *  \brief  Acknwoledgement for reseting the heater operating time
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Index = Index of the heating element
     */
    /****************************************************************************/
    void ReportResetHeaterOperatingTime(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the operating time of a heating elmenent
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Index = Index of the heating element
     *  \iparam OperatingTime = Heater operating time in seconds
     */
    /****************************************************************************/
    void ReportHeaterOperatingTime(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index, quint32 OperatingTime);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the operating time of a heating elmenent
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Index = Index of the ventilation fan
     *  \iparam FanSpeed = Fan speed in revelations per minute
     */
    /****************************************************************************/
    void ReportFanSpeed(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index, quint16 FanSpeed);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the operating time of a heating elmenent
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Sensors = Number of temperature sensors connected to the board
     *  \iparam Fans = Number of ventilation fans connected to the board
     *  \iparam Heaters = Number of heating elements connected to the board
     *  \iparam Pids = Number of PID controllers in the control loop
     *  \iparam Current = Current through the heatinf circuit in milliamperes
     *  \iparam HeaterSwitchState = Heater Switch state.
     */
    /****************************************************************************/
    void ReportHardwareStatus(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Sensors,
                              quint8 Fans, quint8 Heaters, quint8 Pids, quint16 Current, quint8 HeaterSwitchState);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the operating time of a heating elmenent
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam InRange = Indicates if the temperature is in range or not
     *  \iparam Temperature = Temperature in °C
     */
    /****************************************************************************/
    void ReportTemperatureRange(quint32 InstanceID, ReturnCode_t HdlInfo, bool InRange, qreal Temperature);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the state of the level sensor
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam State = The state of the level sensor
     */
    /****************************************************************************/
    void ReportLevelSensorState(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 State);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the state of the level sensor
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam MaxTemperature = Maximum temperature
     *  \iparam ControllerGain = Controller Gain
     *  \iparam ResetTime = Reset Time
     *  \iparam DerivativeTime = Derivative Time
     */
    /****************************************************************************/
    void ReportSetPidAckn(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ReportSetSwitchState
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam SwitchState = Switch State
     *  \iparam AutoSwitch = Auto Switch
     */
    /****************************************************************************/
    void ReportSetSwitchState(quint32 InstanceID, ReturnCode_t HdlInfo, qint8 SwitchState, qint8 AutoSwitch);


 private:
    ReturnCode_t InitializeCANMessages();   //!< can message ID initialization
    ReturnCode_t RegisterCANMessages();     //!< registers the can messages to communication layer

    void SendConfiguration();   //!< configuration task handling function
    void HandleIdleState();     //!< idle task handling function

    //! sends the can message 'FanWatchdog'
    ReturnCode_t SendCANMsgFanWatchdogSet();
    //! sends the can message 'CurrentWatchdog'
    ReturnCode_t SendCANMsgCurrentWatchdogSet();
    //! sends the can message 'PidParameters'
    ReturnCode_t SendCANMsgPidParametersSet(quint8 Index);
    //! sends the can message 'PidParameters'
    ReturnCode_t SendCANMsgPidParametersSet(quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendCANMsgAcCurrentWatchdogSet
     *
     *  \return from SendCANMsgAcCurrentWatchdogSet
     */
    /****************************************************************************/
    ReturnCode_t SendCANMsgAcCurrentWatchdogSet();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendCANMsgAcCurrentWatchdogSetExt
     *
     *  \return from SendCANMsgAcCurrentWatchdogSetExt
     */
    /****************************************************************************/
    ReturnCode_t SendCANMsgAcCurrentWatchdogSetExt();

    //! sends the can set message 'Temperature'
    ReturnCode_t SendCANMsgSetTemperature(qreal Temperature, TempCtrlOperatingMode_t OperatingMode, TempCtrlStatus_t Status, quint8 SlopeTempChange=0);
    //! sends the can request message 'Temperature'
    ReturnCode_t SendCANMsgTemperatureRequest();
    //! sends the can request message 'ServiceSensor'
    ReturnCode_t SendCANMsgServiceSensorRequest(quint8 Index);
    //! sends the can request message 'HeaterTimeSet'
    ReturnCode_t SendCANMsgHeaterTimeSet(quint8 Index);
    //! sends the can request message 'HeaterTimeReq'
    ReturnCode_t SendCANMsgHeaterTimeReq(quint8 Index);
    //! sends the can request message 'ServiceFanReq'
    ReturnCode_t SendCANMsgServiceFanReq(quint8 Index);
    //! sends the can request message 'HardwareReq'
    ReturnCode_t SendCANMsgHardwareReq();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendCANMsgSetSwitchState
     *
     *  \param SwitchState = qint8 type parameter
     *  \param AutoSwitch =  qint8 type parameter
     *
     *  \return from SendCANMsgSetSwitchState
     */
    /****************************************************************************/
    ReturnCode_t SendCANMsgSetSwitchState(qint8 SwitchState, qint8 AutoSwitch);

    //! handles the receipt of can response message 'ServiceSensor'
    void HandleCANMsgServiceSensor(can_frame* pCANframe);
    //! handles the receipt of can response message 'Temperature'
    void HandleCANMsgTemperature(can_frame* pCANframe);
    //! handles the receipt of can response message 'HeaterTime'
    void HandleCANMsgHeaterTime(can_frame* pCANframe);
    //! handles the receipt of can response message 'ServiceFan'
    void HandleCANMsgServiceFan(can_frame* pCANframe);
    //! handles the receipt of can response message 'Hardware'
    void HandleCANMsgHardware(can_frame* pCANframe);
    //! handles the receipt of can response message 'NotiInRange'
    void HandleCANMsgNotiRange(can_frame* pCANframe, bool InRange);
    //! handles the receipt of can response message 'LevelSensorState'
    void HandleCANMsgLevelSensorState(can_frame* pCANframe);
    //! command handling function
    void HandleCommandRequestTask();

    /*! configuration state definitions */
    typedef enum {
        FM_TEMP_SUB_STATE_CONFIG_INIT     = 0,  ///< Initialisation state
        FM_TEMP_SUB_STATE_CONFIG_START    = 1,  ///< Configuration start state
        FM_TEMP_SUB_STATE_CONFIG_SEND     = 2,  ///< Configuration send state
        FM_TEMP_SUB_STATE_CONFIG_FINISHED = 3   ///< Configuration finished state
    } CANTempCtrlSubStateConfig_t;

    CANTempCtrlSubStateConfig_t m_subStateConfig; ///< configuration state machine control

    /*! module command type definitions */
    typedef enum {
        FM_TEMP_CMD_TYPE_UNDEF         = 0, //!< undefined module cmd type
        FM_TEMP_CMD_TYPE_SET_TEMP      = 1, //!< set reference temperature
        FM_TEMP_CMD_TYPE_REQ_ACTTEMP   = 2, //!< request actual temperature
        FM_TEMP_CMD_TYPE_SET_OPMODE    = 3, //!< set operating mode
        FM_TEMP_CMD_TYPE_REQ_ACTOPMODE = 4, //!< request operating mode
        FM_TEMP_CMD_TYPE_SET_STATUS    = 5, //!< set status (on/off)
        FM_TEMP_CMD_TYPE_REQ_ACTSTATUS = 6, //!< request status
        FM_TEMP_CMD_TYPE_RESET_OPTIME  = 7, //!< reset heater operating time
        FM_TEMP_CMD_TYPE_REQ_OPTIME    = 8, //!< request operating time
        FM_TEMP_CMD_TYPE_REQ_FANSPEED  = 9, //!< request fan speed
        FM_TEMP_CMD_TYPE_REQ_HARDWARE  = 10,//!< request hardware status
        FM_TEMP_CMD_TYPE_SET_PID       = 11,//!< set PID parameters
        FM_TEMP_CMD_TYPE_SET_SWITCH_STATE = 12 //!< set PID parameters
    } CANTempCtrlCmdType_t;

    /*! motor command data, used for internal data transfer*/
    typedef struct {
        CANTempCtrlCmdType_t Type;              //!< command type
        ModuleCmdState_t State;                 //!< command state
        qreal Temperature;                      //!< temperature
        TempCtrlStatus_t TempCtrlState;         //!< temperature control state
        TempCtrlOperatingMode_t TempCtrlOpMode; //!< temperature control operating mode
        Global::MonotonicTime ReqSendTime;      //!< time the command was executed
        qint32 Timeout;                         //!< timeout in ms
        quint8 TimeoutRetry;                    //!< timeout retry time
        quint8 Index;                           //!< sensor, heater or fan index
        quint8 SlopeTimeInterval;       ///<  Definition/Declaration of variable SlopeTimeInterval
        quint8 SlopeTempChange;       ///<  Definition/Declaration of variable SlopeTempChange
        quint16 MaxTemperature;       ///<  Definition/Declaration of variable MaxTemperature
        quint16 ControllerGain;       ///<  Definition/Declaration of variable ControllerGain
        quint16 ResetTime;       ///<  Definition/Declaration of variable ResetTime
        quint16 DerivativeTime;       ///<  Definition/Declaration of variable DerivativeTime
        qint8 SwitchState;       ///<  Definition/Declaration of variable SwitchState
        qint8 AutoSwitch;       ///<  Definition/Declaration of variable AutoSwitch
    } TempCtrlCommand_t;

    TempCtrlCommand_t m_ModuleCommand[MAX_TEMP_MODULE_CMD_IDX]; //!< module command array for simultaneously command execution

    //! set the module command type to free entry within array
    bool SetModuleTask(CANTempCtrlCmdType_t CommandType, quint8* pCmdIndex = 0);
    //! clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANTempCtrlCmdType_t);

    quint32 m_unCanIDError;                 //!< CAN message 'Error'
    quint32 m_unCanIDErrorReq;              //!< CAN message 'Request error'
    quint32 m_unCanIDTemperatureSet;        //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDFanWatchdogSet;        //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDCurrentWatchdogSet;    //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPIDParamSet;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDHeaterTimeSet;         //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDTemperatureReq;        //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDTemperature;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPIDParamReq;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPIDParam;              //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDHeaterTimeReq;         //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDHeaterTime;            //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDServiceSensorReq;      //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDServiceSensor;         //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDServiceFanReq;         //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDServiceFan;            //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDHardwareReq;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDHardware;              //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDNotiAutoTune;          //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDNotiInRange;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDNotiOutOfRange;        //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDLevelSensorState;       ///<  Definition/Declaration of variable m_unCanIDLevelSensorState
    quint32 m_unCanIDSetSwitchState;       ///<  Definition/Declaration of variable m_unCanIDSetSwitchState
    quint32 m_unCanIDAcCurrentWatchdogSet;    //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDAcCurrentWatchdogSetExt; //!< CAN-message id of 'TBD' message
    Global::MonotonicTime m_timeAction; ///< Action start time, for timeout detection
    qint16 m_aktionTimespan;            ///< Delay im ms, for timeout detection
};

} //namespace

#endif /* DEVICECONTROL_TEMPERATURECONTROLPRIVATE_H */
