#ifndef PRESSURECONTROL_H
#define PRESSURECONTROL_H



#include "DeviceControl/Include/SlaveModules/FunctionModule.h"
#include "Global/Include/MonotonicTime.h"

#ifdef PRE_ALFA_TEST
namespace DeviceControl
{

class CANCommunicator;

#define MAX_PRESSURE_MODULE_CMD_IDX (4)   ///< up to 4 module commands can be handled simultaneously

///****************************************************************************/
///*!
// *  \brief This class implements the functionality to configure and control a
// *         slave's 'pressure control' function module
// */
///****************************************************************************/

class CPressureControl : public CFunctionModule
{
    Q_OBJECT

 public:
    CPressureControl(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator, CBaseModule* pParentNode);
    ~CPressureControl();

    ReturnCode_t Initialize();  ///< Initialize CAN-messages

    void HandleTasks(); ///< Task handling function
    void HandleCanMessage(can_frame* pCANframe);    ///< CAN-message handling function

//    //! Set reference temperature
    ReturnCode_t SetPressure(quint8 flag, qint8 Pressure);
//    //! Request actual temperature
    ReturnCode_t ReqActPressure(quint8 Index);
//    //! Set temperature ctrl. status
    ReturnCode_t SetStatus(PressureCtrlStatus_t PressureCtrlState);
//    //! Request temperature ctrl. status
    ReturnCode_t ReqStatus();
//    //! Set temperature ctrl. operating mode
    ReturnCode_t SetOperatingMode(PressureCtrlOperatingMode_t PressureCtrlOpMode);
//    //! Request temperature ctrl. operating mode
    ReturnCode_t ReqOperatingMode();
//    //! Reset the heater operating time
    ReturnCode_t ResetHeaterOperatingTime(quint8 Index);
//    //! Get the heater operating time in seconds
    ReturnCode_t GetHeaterOperatingTime(quint8 Index);
//    //! Get the fan speed in revelations per minute
    ReturnCode_t GetFanSpeed(quint8 Index);
//    //! Get the hardware status
    ReturnCode_t GetHardwareStatus();

    ReturnCode_t SetValve(quint8 ValveIndex, quint8 ValveState);
    ReturnCode_t SetCalibration(bool Enable);
signals:
//    /****************************************************************************/
//    /*!
//     *  \brief  This signal reports the actual temperature
//     *
//     *  \iparam InstanceID = Instance identifier of this function module instance
//     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
//     *  \iparam Index = Index of the temperature sensor
//     *  \iparam Temperature = Actual temperature
//     *
//     ****************************************************************************/
    void ReportActPressure(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index, qreal Pressure);

//    /****************************************************************************/
//    /*!
//     *  \brief  This signal reports the actual reference temperature
//     *
//     *  \iparam InstanceID = Instance identifier of this function module instance
//     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
//     *  \iparam Temperature = Reference temperature
//     */
//    /****************************************************************************/
    void ReportRefPressure(quint32 InstanceID, ReturnCode_t HdlInfo, qreal Pressure);

//    /****************************************************************************/
//    /*!
//     *  \brief  This signal reports the actual temperature ctrl. status
//     *
//     *  \iparam InstanceID = Instance identifier of this function module instance
//     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
//     *  \iparam TempCtrlStatus = Temperature ctrl. status
//     */
//    /****************************************************************************/
    void ReportActStatus(quint32 InstanceID, ReturnCode_t HdlInfo, PressureCtrlStatus_t Status, PressureCtrlMainsVoltage_t Voltage);

//    /****************************************************************************/
//    /*!
//     *  \brief  This signal reports the acknowledge for status setting
//     *          Report Acknowlege for SetStatus(..) - request
//     *
//     *  \iparam InstanceID = Instance identifier of this function module instance
//     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
//     *  \iparam OperatingMode = Temperature ctrl. status
//     */
//    /****************************************************************************/
    void ReportSetStatusAckn(quint32 InstanceID, ReturnCode_t HdlInfo, PressureCtrlStatus_t Status);

//    /****************************************************************************/
//    /*!
//     *  \brief  This signal reports the actual operation mode
//     *
//     *  \iparam InstanceID = Instance identifier of this function module instance
//     *  \iparam HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
//     *  \iparam OperatingMode = Operation mode
//     */
//    /****************************************************************************/
    void ReportActOperatingMode(quint32 InstanceID, ReturnCode_t HdlInfo, PressureCtrlOperatingMode_t OperatingMode);

//    /****************************************************************************/
//    /*!
//     *  \brief  This signal reports the acknwoledge for operation mode setting
//     *          Report Acknowlege for SetOperatingMode(..) - request
//     *
//     *  \iparam InstanceID = Instance identifier of this function module instance
//     *  \iparam HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
//     *  \iparam OperatingMode = Operation mode
//     */
//    /****************************************************************************/
    void ReportSetOperatingModeAckn(quint32 InstanceID, ReturnCode_t HdlInfo, PressureCtrlOperatingMode_t OperatingMode);

//    /****************************************************************************/
//    /*!
//     *  \brief  Acknwoledgement for reseting the heater operating time
//     *
//     *  \iparam InstanceID = Instance identifier of this function module instance
//     *  \iparam HdlInfo    = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
//     */
//    /****************************************************************************/
    void ReportResetHeaterOperatingTime(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index);

//    /****************************************************************************/
//    /*!
//     *  \brief  This signal reports the operating time of a heating elmenent
//     *
//     *  \iparam InstanceID = Instance identifier of this function module instance
//     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
//     *  \iparam Index = Index of the heating element
//     *  \iparam OperatingTime = Heater operating time in seconds
//     */
//    /****************************************************************************/
    void ReportHeaterOperatingTime(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index, quint32 OperatingTime);

//    /****************************************************************************/
//    /*!
//     *  \brief  This signal reports the operating time of a heating elmenent
//     *
//     *  \iparam InstanceID = Instance identifier of this function module instance
//     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
//     *  \iparam Index = Index of the ventilation fan
//     *  \iparam FanSpeed = Fan speed in revelations per minute
//     */
//    /****************************************************************************/
    void ReportFanSpeed(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index, quint16 FanSpeed);

//    /****************************************************************************/
//    /*!
//     *  \brief  This signal reports the operating time of a heating elmenent
//     *
//     *  \iparam InstanceID = Instance identifier of this function module instance
//     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
//     *  \iparam Sensors = Number of temperature sensors connected to the board
//     *  \iparam Fans = Number of ventilation fans connected to the board
//     *  \iparam Heaters = Number of heating elements connected to the board
//     *  \iparam Pids = Number of PID controllers in the control loop
//     *  \iparam Current = Current through the heatinf circuit in milliamperes
//     */
//    /****************************************************************************/
    void ReportHardwareStatus(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Sensors,
                              quint8 Fans, quint8 Heaters, quint8 Pids, quint16 Current);

//    /****************************************************************************/
//    /*!
//     *  \brief  This signal reports the operating time of a heating elmenent
//     *
//     *  \iparam InstanceID = Instance identifier of this function module instance
//     *  \iparam HdlInfo = Return code, DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
//     *  \iparam InRange = Indicates if the temperature is in range or not
//     *  \iparam Temperature = Temperature in °C
//     */
//    /****************************************************************************/
    void ReportPressureRange(quint32 InstanceID, ReturnCode_t HdlInfo, bool InRange, qint8 Pressure);

    void ReportRefValveState(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 ValveIndex, quint8 ValveState);
  
private:
    ReturnCode_t InitializeCANMessages();   //!< can message ID initialization
    ReturnCode_t RegisterCANMessages();     //!< registers the can messages to communication layer

//    void SendConfiguration();   //!< configuration task handling function
    void HandleIdleState();     //!< idle task handling function

//    //! sends the can message 'FanWatchdog'
    ReturnCode_t SendCANMsgFanWatchdogSet();
//    //! sends the can message 'CurrentWatchdog'
    ReturnCode_t SendCANMsgCurrentWatchdogSet();
//    //! sends the can message 'PidParameters'
    ReturnCode_t SendCANMsgPidParametersSet(quint8 Index);
//    //! sends the can set message 'Temperature'
#if 0
    ReturnCode_t SendCANMsgSetPressure(qreal Pressure, PressureCtrlOperatingMode_t OperatingMode, PressureCtrlStatus_t Status);
#else
 ReturnCode_t SendCANMsgSetPressure(quint8 flag, qint8 Pressure, quint8 Tolerance, quint16 SamplingTime, quint16 DurationTime);
#endif
//    //! sends the can request message 'Temperature'
    ReturnCode_t SendCANMsgPressureRequest();
//    //! sends the can request message 'ServiceSensor'
    ReturnCode_t SendCANMsgServiceSensorRequest(quint8 Index);
//    //! sends the can request message 'HeaterTimeSet'
    ReturnCode_t SendCANMsgHeaterTimeSet(quint8 Index);
//    //! sends the can request message 'HeaterTimeReq'
    ReturnCode_t SendCANMsgHeaterTimeReq(quint8 Index);
//    //! sends the can request message 'ServiceFanReq'
    ReturnCode_t SendCANMsgServiceFanReq(quint8 Index);
//    //! sends the can request message 'HardwareReq'
    ReturnCode_t SendCANMsgHardwareReq();



    ReturnCode_t SendCANMsgSetValve(quint8 NumberValve, quint8 flags);

    ReturnCode_t SendCANMsgCalibration(bool Enable);


//    //! handles the receipt of can response message 'ServiceSensor'
    void HandleCANMsgServiceSensor(can_frame* pCANframe);
//    //! handles the receipt of can response message 'Temperature'
    void HandleCANMsgPressure(can_frame* pCANframe);
//    //! handles the receipt of can response message 'HeaterTime'
    void HandleCANMsgHeaterTime(can_frame* pCANframe);
//    //! handles the receipt of can response message 'ServiceFan'
    void HandleCANMsgServiceFan(can_frame* pCANframe);
//    //! handles the receipt of can response message 'Hardware'
    void HandleCANMsgHardware(can_frame* pCANframe);
//    //! handles the receipt of can response message 'NotiInRange'
    void HandleCANMsgNotiRange(can_frame* pCANframe, bool InRange);

//    //! command handling function
    void HandleCommandRequestTask();

    /*! configuration state definitions */
    typedef enum {
        FM_PRESSURE_SUB_STATE_CONFIG_INIT     = 0,  ///< Initialisation state
        FM_PRESSURE_SUB_STATE_CONFIG_START    = 1,  ///< Configuration start state
        FM_PRESSURE_SUB_STATE_CONFIG_SEND     = 2,  ///< Configuration send state
        FM_PRESSURE_SUB_STATE_CONFIG_FINISHED = 3   ///< Configuration finished state
    } CANPressureCtrlSubStateConfig_t;

    CANPressureCtrlSubStateConfig_t m_subStateConfig; ///< configuration state machine control

//    /*! module command type definitions */
    typedef enum {
        FM_PRESSURE_CMD_TYPE_UNDEF         = 0, //!< undefined module cmd type
        FM_PRESSURE_CMD_TYPE_SET_PRESSURE      = 1, //!< set reference temperature
        FM_PRESSURE_CMD_TYPE_REQ_ACTPRESSURE   = 2, //!< request actual temperature
        FM_PRESSURE_CMD_TYPE_SET_OPMODE    = 3, //!< set operating mode
        FM_PRESSURE_CMD_TYPE_REQ_ACTOPMODE = 4, //!< request operating mode
        FM_PRESSURE_CMD_TYPE_SET_STATUS    = 5, //!< set status (on/off)
        FM_PRESSURE_CMD_TYPE_REQ_ACTSTATUS = 6, //!< request status
        FM_PRESSURE_CMD_TYPE_RESET_OPTIME  = 7, //!< reset heater operating time
        FM_PRESSURE_CMD_TYPE_REQ_OPTIME    = 8, //!< request operating time
        FM_PRESSURE_CMD_TYPE_REQ_FANSPEED  = 9, //!< request fan speed
        FM_PRESSURE_CMD_TYPE_REQ_HARDWARE  = 10, //!< request hardware status
        FM_PRESSURE_CMD_TYPE_SET_VALVE  = 11,
        FM_PRESSURE_CMD_TYPE_CALIBRATION  = 12
    } CANPressureCtrlCmdType_t;

//    /*! motor command data, used for internal data transfer*/
    typedef struct {
        CANPressureCtrlCmdType_t Type;              //!< command type
        ModuleCmdState_t State;                 //!< command state
        quint8 flag;
        qint8 Pressure;                      //!< temperature
        PressureCtrlStatus_t PressureCtrlState;         //!< temperature control state
        PressureCtrlOperatingMode_t PressureCtrlOpMode; //!< temperature control operating mode
        Global::MonotonicTime ReqSendTime;      //!< time the command was executed
        qint32 Timeout;                         //!< timeout in ms
        quint8 Index;                           //!< sensor, heater or fan index
        quint8 ValveIndex;                      // Valve index
        quint8 ValveState;                      // Valve State. 0: off, 1: on
        bool EnableCalibration;
    } PressureCtrlCommand_t;

    PressureCtrlCommand_t m_ModuleCommand[MAX_PRESSURE_MODULE_CMD_IDX]; //!< module command array for simultaneously command execution

//    //! set the module command type to free entry within array
    bool SetModuleTask(CANPressureCtrlCmdType_t CommandType, quint8* pCmdIndex = 0);
//    //! clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANPressureCtrlCmdType_t);

    quint32 m_unCanIDError;                 //!< CAN message 'Error'
    quint32 m_unCanIDErrorReq;              //!< CAN message 'Request error'
    quint32 m_unCanIDPressureSet;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDFanWatchdogSet;        //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDCurrentWatchdogSet;    //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPIDParamSet;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDHeaterTimeSet;         //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPressureReq;           //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDPressure;              //!< CAN-message id of 'TBD' message
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
    quint32 m_unCanIDValveSet;               //!< CAN-message id of 'TBD' message
    quint32 m_unCanIDCalibration;
    Global::MonotonicTime m_timeAction; ///< Action start time, for timeout detection
    qint16 m_aktionTimespan;            ///< Delay im ms, for timeout detection
};

} //namespace
#endif //PRE_ALFA_TEST
#endif // PRESSURECONTROL_H
