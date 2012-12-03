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
    CTemperatureControl(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                        CBaseModule* pParentNode);
    ~CTemperatureControl();

    ReturnCode_t Initialize();  ///< Initialize CAN-messages

    void HandleTasks(); ///< Task handling function
    void HandleCanMessage(can_frame* pCANframe);    ///< CAN-message handling function

    //! Request actual temperature
    ReturnCode_t ReqActTemperature(quint8 Index);
    //! Set temperature ctrl. status and set point temperature
    ReturnCode_t SetStatus(TempCtrlStatus_t TempCtrlState, qreal Temperature);
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

signals:
    /****************************************************************************/
    /*!
     *  \brief  This signal reports the actual temperature
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Index = Index of the temperature sensor
     *  \iparam Temperature = Actual temperature
     */
    /****************************************************************************/
    void ReportActTemperature(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index, qreal Temperature);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the actual temperature ctrl. status
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Status = Temperature control status
     *  \iparam Voltage = Mains voltage domain (220V, 110V)
     */
    /****************************************************************************/
    void ReportActStatus(quint32 InstanceID, ReturnCode_t HdlInfo, TempCtrlStatus_t Status, TempCtrlMainsVoltage_t Voltage);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the acknowledge for status setting
     *          Report Acknowlege for SetStatus(..) - request
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Status = Temperature ctrl. status
     *  \iparam Temperature = Set point temperature in °C
     */
    /****************************************************************************/
    void ReportSetStatusAckn(quint32 InstanceID, ReturnCode_t HdlInfo, TempCtrlStatus_t Status, qreal Temperature);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the actual operation mode
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
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
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam OperatingMode = Operation mode
     */
    /****************************************************************************/
    void ReportSetOperatingModeAckn(quint32 InstanceID, ReturnCode_t HdlInfo, TempCtrlOperatingMode_t OperatingMode);

    /****************************************************************************/
    /*!
     *  \brief  Acknwoledgement for reseting the heater operating time
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Index = Index of the heating element
     */
    /****************************************************************************/
    void ReportResetHeaterOperatingTime(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the operating time of a heating elmenent
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
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
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
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
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Sensors = Number of temperature sensors connected to the board
     *  \iparam Fans = Number of ventilation fans connected to the board
     *  \iparam Heaters = Number of heating elements connected to the board
     *  \iparam Pids = Number of PID controllers in the control loop
     *  \iparam Current = Current through the heatinf circuit in milliamperes
     */
    /****************************************************************************/
    void ReportHardwareStatus(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Sensors,
                              quint8 Fans, quint8 Heaters, quint8 Pids, quint16 Current);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the operating time of a heating elmenent
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam InRange = Indicates if the temperature is in range or not
     *  \iparam Temperature = Temperature in °C
     */
    /****************************************************************************/
    void ReportTemperatureRange(quint32 InstanceID, ReturnCode_t HdlInfo, bool InRange, qreal Temperature);

 private:
    ReturnCode_t InitializeCANMessages();   //!< can message ID initialization
    ReturnCode_t RegisterCANMessages();     //!< registers the can messages to communication layer

    void HandleIdleState(); //!< idle task handling function

    //! sends the can message 'FanWatchdog'
    ReturnCode_t SendCANMsgFanWatchdogSet();
    //! sends the can message 'CurrentWatchdog'
    ReturnCode_t SendCANMsgCurrentWatchdogSet();
    //! sends the can message 'PidParameters'
    ReturnCode_t SendCANMsgPidParametersSet(quint8 Index);
    //! sends the can set message 'Temperature'
    ReturnCode_t SendCANMsgSetTemperature();
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
        FM_TEMP_CMD_TYPE_REQ_ACTTEMP   = 1, //!< request actual temperature
        FM_TEMP_CMD_TYPE_SET_OPMODE    = 2, //!< set operating mode
        FM_TEMP_CMD_TYPE_REQ_ACTOPMODE = 3, //!< request operating mode
        FM_TEMP_CMD_TYPE_SET_STATUS    = 4, //!< set status (on/off)
        FM_TEMP_CMD_TYPE_REQ_ACTSTATUS = 5, //!< request status
        FM_TEMP_CMD_TYPE_RESET_OPTIME  = 6, //!< reset heater operating time
        FM_TEMP_CMD_TYPE_REQ_OPTIME    = 7, //!< request operating time
        FM_TEMP_CMD_TYPE_REQ_FANSPEED  = 8, //!< request fan speed
        FM_TEMP_CMD_TYPE_REQ_HARDWARE  = 9  //!< request hardware status
    } CANTempCtrlCmdType_t;

    /*! motor command data, used for internal data transfer*/
    typedef struct {
        CANTempCtrlCmdType_t Type;              //!< command type
        ModuleCmdState_t State;                 //!< command state
        Global::MonotonicTime ReqSendTime;      //!< time the command was executed
        qint32 Timeout;                         //!< timeout in ms
        quint8 Index;                           //!< sensor, heater or fan index
    } ModuleCommand_t;

    qreal m_Temperature;                        //!< temperature
    TempCtrlStatus_t m_TempCtrlState;           //!< temperature control state
    TempCtrlOperatingMode_t m_TempCtrlOpMode;   //!< temperature control operating mode

    QList<ModuleCommand_t *> m_ModuleCommand;   //!< Queue of module commands for simultaneous execution

    //! Adds the module command type to the transmit queue
    ModuleCommand_t *SetModuleTask(CANTempCtrlCmdType_t CommandType);
    //! Clears all entrys with the specified module command type to free
    bool ResetModuleCommand(CANTempCtrlCmdType_t CommandType);

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

    Global::MonotonicTime m_timeAction; ///< Action start time, for timeout detection
    qint16 m_aktionTimespan;            ///< Delay im ms, for timeout detection
};

} //namespace

#endif /* DEVICECONTROL_TEMPERATURECONTROLPRIVATE_H */
