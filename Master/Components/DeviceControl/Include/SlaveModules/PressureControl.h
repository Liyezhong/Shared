#ifndef PRESSURECONTROL_H
#define PRESSURECONTROL_H



#include "DeviceControl/Include/SlaveModules/FunctionModule.h"
#include "Global/Include/MonotonicTime.h"

namespace DeviceControl
{

class CANCommunicator;

#define MAX_PRESSURE_MODULE_CMD_IDX (4)   ///< up to 4 module commands can be handled simultaneously

/****************************************************************************/
/*!
 *  \brief This class implements the functionality to configure and control a
 *         slave's 'pressure control' function module
 */
/****************************************************************************/

class CPressureControl : public CFunctionModule
{
    Q_OBJECT

 public:
    CPressureControl(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator, CBaseModule* pParentNode);
    ~CPressureControl();

    ReturnCode_t Initialize();  ///< Initialize CAN-messages

    void HandleTasks(); ///< Task handling function
    void HandleCanMessage(can_frame* pCANframe);    ///< CAN-message handling function

    //! Set reference pressure
    ReturnCode_t SetPressure(quint8 flag, float Pressure);
    //! Request actual pressure
    ReturnCode_t ReqActPressure(quint8 Index);
    //! Set pressure ctrl. status
    ReturnCode_t SetStatus(PressureCtrlStatus_t PressureCtrlState);
    //! Request pressure ctrl. status
    ReturnCode_t ReqStatus();
    //! Set pressure ctrl. operating mode
    ReturnCode_t SetOperatingMode(PressureCtrlOperatingMode_t PressureCtrlOpMode);
    //! Request pressure ctrl. operating mode
    ReturnCode_t ReqOperatingMode();
    //! Reset the pump operating time
    ReturnCode_t ResetPumpOperatingTime(quint8 Index);
    //! Get the pump operating time in seconds
    ReturnCode_t GetPumpOperatingTime(quint8 Index);
    //! Get the fan speed in revelations per minute
    ReturnCode_t GetFanSpeed(quint8 Index);
    //! Get the hardware status
    ReturnCode_t GetHardwareStatus();
    //! Set valve status
    ReturnCode_t SetValve(quint8 ValveIndex, quint8 ValveState);
    //! Enable/Disable calibration function
    ReturnCode_t SetCalibration(bool Enable);
    //! Set PWM parameters
    ReturnCode_t SetPWMParams(quint16 maxActuatingValue, quint16 minActuatingValue, quint8 maxPwmDuty, quint8 minPwmDuty);
signals:
    /****************************************************************************/
    /*!
     *  \brief  This signal reports the actual pressre
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Index = Index of the pressure sensor
     *  \iparam Pressure = Actual pressure
     *
     ****************************************************************************/
    void ReportActPressure(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index, float Pressure);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the actual reference pressure
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Pressure = Reference pressure
     */
    /****************************************************************************/
    void ReportRefPressure(quint32 InstanceID, ReturnCode_t HdlInfo, float Pressure);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the actual pressure ctrl. status
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam PressureCtrlStatus = Pressure ctrl. status
     *  \iparam Voltage = Voltage mode
     */
    /****************************************************************************/
    void ReportActStatus(quint32 InstanceID, ReturnCode_t HdlInfo, PressureCtrlStatus_t Status, PressureCtrlMainsVoltage_t Voltage);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the acknowledge for status setting
     *          Report Acknowlege for SetStatus(..) - request
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Status = pressure ctrl. status
     */
    /****************************************************************************/
    void ReportSetStatusAckn(quint32 InstanceID, ReturnCode_t HdlInfo, PressureCtrlStatus_t Status);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the actual operation mode
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam OperatingMode = Operation mode
     */
    /****************************************************************************/
    void ReportActOperatingMode(quint32 InstanceID, ReturnCode_t HdlInfo, PressureCtrlOperatingMode_t OperatingMode);

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
    void ReportSetOperatingModeAckn(quint32 InstanceID, ReturnCode_t HdlInfo, PressureCtrlOperatingMode_t OperatingMode);

    /****************************************************************************/
    /*!
     *  \brief  Acknwoledgement for reseting the pump operating time
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Index      = Index of the pumps
     */
    /****************************************************************************/
    void ReportResetPumpOperatingTime(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the operating time of a pump
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Index = Index of the pump
     *  \iparam OperatingTime = Pump operating time in seconds
     */
    /****************************************************************************/
    void ReportPumpOperatingTime(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index, quint32 OperatingTime);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the operating time of a pump
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
     *  \brief  This signal reports the operating time of a pump
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Sensors = Number of pressure sensors connected to the board
     *  \iparam Fans = Number of ventilation fans connected to the board
     *  \iparam Pumps = Number of pump connected to the board
     *  \iparam Pids = Number of PID controllers in the control loop
     *  \iparam Current = Current through the pump circuit in milliamperes
     */
    /****************************************************************************/
    void ReportHardwareStatus(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Sensors,
                              quint8 Fans, quint8 Pumps, quint8 Pids, quint16 Current);

    /****************************************************************************/
    /*!
     *  \brief  This signal reports the operating time of a pump
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam InRange = Indicates if the pressure is in range or not
     *  \iparam Pressure = Pressure in kpa
     */
    /****************************************************************************/
    void ReportPressureRange(quint32 InstanceID, ReturnCode_t HdlInfo, bool InRange, float Pressure);
    /****************************************************************************/
    /*!
     *  \brief  This signal reports the state of the valve
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam ValveIndex = The index of the valve.
     *  \iparam ValveState = valve's state
     */
    /****************************************************************************/
    void ReportRefValveState(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 ValveIndex, quint8 ValveState);
  
private:
    ReturnCode_t InitializeCANMessages();   //!< can message ID initialization
    ReturnCode_t RegisterCANMessages();     //!< registers the can messages to communication layer

    //!< configuration task handling function
    void SendConfiguration();
    //!< idle task handling function
    void HandleIdleState();
    //! sends the can message 'FanWatchdog'
    ReturnCode_t SendCANMsgFanWatchdogSet();
    //! sends the can message 'CurrentWatchdog'
    ReturnCode_t SendCANMsgCurrentWatchdogSet();
    //! sends the can message 'PidParameters'
    ReturnCode_t SendCANMsgPidParametersSet(quint8 Index);
    //! sends the can set message 'Pressure'
    ReturnCode_t SendCANMsgSetPressure(quint8 flag, float Pressure, quint8 Tolerance, quint16 SamplingTime, quint16 DurationTime);
    //! sends the can request message 'Pressure'
    ReturnCode_t SendCANMsgPressureRequest();
    //! sends the can request message 'ServiceSensor'
    ReturnCode_t SendCANMsgServiceSensorRequest(quint8 Index);
    //! sends the can request message 'PumpTimeSet'
    ReturnCode_t SendCANMsgPumpTimeSet(quint8 Index);
    //! sends the can request message 'PumpTimeReq'
    ReturnCode_t SendCANMsgPumpTimeReq(quint8 Index);
    //! sends the can request message 'ServiceFanReq'
    ReturnCode_t SendCANMsgServiceFanReq(quint8 Index);
    //! sends the can request message 'HardwareReq'
    ReturnCode_t SendCANMsgHardwareReq();
    //! sends the can request message 'SetValve'
    ReturnCode_t SendCANMsgSetValve(quint8 ValveIndex, quint8 flags);
    //! sends the can request message 'Calibration'
    ReturnCode_t SendCANMsgCalibration(bool Enable);
    ReturnCode_t SendCANMsgSetPWMParam(quint16 MaxActuatingValue, quint16 MinActuatingValue, quint8 MaxPwmDuty, quint8 MinPwmDuty);


    //! handles the receipt of can response message 'ServiceSensor'
    void HandleCANMsgServiceSensor(can_frame* pCANframe);
    //! handles the receipt of can response message 'Temperature'
    void HandleCANMsgPressure(can_frame* pCANframe);
    //! handles the receipt of can response message 'PumpTime'
    void HandleCANMsgPumpTime(can_frame* pCANframe);
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
        FM_PRESSURE_SUB_STATE_CONFIG_INIT     = 0,  ///< Initialisation state
        FM_PRESSURE_SUB_STATE_CONFIG_START    = 1,  ///< Configuration start state
        FM_PRESSURE_SUB_STATE_CONFIG_SEND     = 2,  ///< Configuration send state
        FM_PRESSURE_SUB_STATE_CONFIG_FINISHED = 3   ///< Configuration finished state
    } CANPressureCtrlSubStateConfig_t;

    CANPressureCtrlSubStateConfig_t m_subStateConfig; ///< configuration state machine control

    /*! module command type definitions */
    typedef enum {
        FM_PRESSURE_CMD_TYPE_UNDEF             = 0, //!< undefined module cmd type
        FM_PRESSURE_CMD_TYPE_SET_PRESSURE      = 1, //!< set reference pressure
        FM_PRESSURE_CMD_TYPE_REQ_ACTPRESSURE   = 2, //!< request actual pressure
        FM_PRESSURE_CMD_TYPE_SET_OPMODE        = 3, //!< set operating mode
        FM_PRESSURE_CMD_TYPE_REQ_ACTOPMODE     = 4, //!< request operating mode
        FM_PRESSURE_CMD_TYPE_SET_STATUS        = 5, //!< set status (on/off)
        FM_PRESSURE_CMD_TYPE_REQ_ACTSTATUS     = 6, //!< request status
        FM_PRESSURE_CMD_TYPE_RESET_OPTIME      = 7, //!< reset pump operating time
        FM_PRESSURE_CMD_TYPE_REQ_OPTIME        = 8, //!< request operating time
        FM_PRESSURE_CMD_TYPE_REQ_FANSPEED      = 9, //!< request fan speed
        FM_PRESSURE_CMD_TYPE_REQ_HARDWARE      = 10, //!< request hardware status
        FM_PRESSURE_CMD_TYPE_SET_VALVE         = 11,
        FM_PRESSURE_CMD_TYPE_CALIBRATION       = 12,
        FM_PRESSURE_CMD_TYPE_SET_PWM           = 13
    } CANPressureCtrlCmdType_t;

    /*! motor command data, used for internal data transfer*/
    typedef struct {
        CANPressureCtrlCmdType_t Type;                  //!< command type
        ModuleCmdState_t State;                         //!< command state
        quint8 flag;                                    //!< flag of pressre control
        float Pressure;                                 //!< pressure
        PressureCtrlStatus_t PressureCtrlState;         //!< pressure control state
        PressureCtrlOperatingMode_t PressureCtrlOpMode; //!< pressure control operating mode
        Global::MonotonicTime ReqSendTime;              //!< time the command was executed
        qint32 Timeout;                                 //!< timeout in ms
        quint8 Index;                                   //!< sensor, pump or fan index
        quint8 ValveIndex;                              //!< Valve index
        quint8 ValveState;                              //!< Valve State. 0: off, 1: on
        bool EnableCalibration;                         //!< Enable or disable calibration
        quint16 MaxActuatingValue;
        quint16 MinActuatingValue;
        quint8 MaxPwmDuty;
        quint8 MinPwmDuty;
    } PressureCtrlCommand_t;

    PressureCtrlCommand_t m_ModuleCommand[MAX_PRESSURE_MODULE_CMD_IDX]; //!< module command array for simultaneously command execution

    //! set the module command type to free entry within array
    bool SetModuleTask(CANPressureCtrlCmdType_t CommandType, quint8* pCmdIndex = 0);
    //! clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANPressureCtrlCmdType_t);

    quint32 m_unCanIDError;                 //!< CAN message 'Error'
    quint32 m_unCanIDErrorReq;              //!< CAN message 'Request error'
    quint32 m_unCanIDPressureSet;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDFanWatchdogSet;        //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDCurrentWatchdogSet;    //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPIDParamSet;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPumpTimeSet;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPressureReq;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPressure;              //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPIDParamReq;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPIDParam;              //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPumpTimeReq;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPumpTime;              //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDServiceSensorReq;      //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDServiceSensor;         //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDServiceFanReq;         //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDServiceFan;            //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDHardwareReq;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDHardware;              //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDNotiAutoTune;          //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDNotiInRange;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDNotiOutOfRange;        //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDValveSet;               //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDCalibration;
    quint32 m_unCanIDPWMParamSet;
    Global::MonotonicTime m_timeAction; ///< Action start time, for timeout detection
    qint16 m_aktionTimespan;            ///< Delay im ms, for timeout detection
};

} //namespace
#endif // PRESSURECONTROL_H
