#ifndef DEVICEROTARYVALVE_H
#define DEVICEROTARYVALVE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"
#include <QEventLoop>

namespace DeviceControl
{
class CTemperatureControl;
class CStepperMotor;


//#define RV_DEV_TASK_INITPOSITION          0
//#define RV_DEV_TASK_POS_SET   1 //!<
//#define RV_DEV_TASK_POS_REQ   2 //!<
//#define RV_DEV_TASK_TEMP_SET        3 //!<
//#define RV_DEV_TASK_TEMP_REQ        4 //!<
//#define RV_DEV_TASK_TEMP_STATUS_SET 5 //!<
//#define RV_DEV_TASK_TEMP_STATUS_REQ 6 //!<
//#define RV_DEVICE_TASK_COUNT        7 //!<






class CRotaryValveDevice : public CBaseDevice
{
    //friend class :: WrapperDeviceRotaryValve;

    Q_OBJECT

public:

    //! constructor
    CRotaryValveDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    //! destructor
    virtual ~CRotaryValveDevice();

    //! general task handling function
    void HandleTasks();


    ReturnCode_t SetTempCtrlON();
    ReturnCode_t SetTempCtrlOFF();
    ReturnCode_t SetTemperaturePid(quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    ReturnCode_t StartTemperatureControl(qreal NominalTemperature, quint8 SlopeTempChange);
    qreal GetRecentTemperature(quint32 Index);
    TempCtrlState_t GetTemperatureControlState();
    //! Execute the move to intial position of the RV
    ReturnCode_t ReqMoveToInitialPosition();
    //! Position the oven cover
    ReturnCode_t ReqMoveToRVPosition( RVPosition_t RVPosition);
    //! Request actual oven cover position
    RVPosition_t ReqActRVPosition();

    //! Request actual oven temperature
    //! Set oven nominal temperature
    //    ReturnCode_t SetTemperature(qint16 Temperatur);
    //! Request the status of the oven temperature control

    ReturnCode_t ClearErrorState();
signals:
    void ReportActTempeature(quint32 SensorIndex, qreal Temperature);
private slots:
    qreal GetTemperature(quint32 Index);
    bool SetTemperatureControlStatus(TempCtrlStatus_t TempCtrlStatus);
    void OnTempControlStatus(quint32 InstanceID, ReturnCode_t ReturnCode, TempCtrlStatus_t TempCtrlStatus, TempCtrlMainsVoltage_t MainsVoltage);
    bool SetTemperature(qreal NominalTemperature, quint8 SlopeTempChange);

    void CheckSensorsData();
    bool GetTemperatureAsync(quint8 Index);
    //! Oven cover motor error notification
    void StepperMotorError(quint32, quint16, quint16, quint16, QDateTime);
    //! motor set state acknowledge notification
    //void StepperMotorSetStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo);
    //! motor reference run acknowledge notification
    //void StepperMotorMoveToInitialPositionAckn(quint32 instanceID, ReturnCode_t hdlInfo, RVPosition_t rvPosition);
    //! motor movement acknowledge notification
    //void StepperMotorMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, RVPosition_t rvPosition, qint16 speed);
    //! motor position notification
    //void StepperMotorActRVPosition(quint32 instanceID, ReturnCode_t hdlInfo, RVPosition_t rvPosition);

    //! Temperature control error notification
    void TempCtrlError(quint32, quint16, quint16, quint16, QDateTime);
    //! Actual temperature notification
    //void TempCtrlActTemperature(quint32, ReturnCode_t, quint8 Index, qreal OvenTemperature);
    //! Set temperature acknowledge notification
    //void ReportRefTemperature(quint32, ReturnCode_t, qreal);
    //! Actual temperature control status notification
    //void TempCtrlActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t);
    //! Set temperature control status acknowledge notification
    //void TempCtrlSetStatusAckn(quint32, ReturnCode_t, TempCtrlStatus_t);
    //! Actual temperature control operating mode notification
    //void TempCtrlActOperatingMode(quint32, ReturnCode_t, TempCtrlOperatingMode_t);
    //! Set temperature control operating mode acknowledge notification
    void TempCtrlSetOperatingModeAckn(quint32, ReturnCode_t, TempCtrlOperatingMode_t);

    bool IsInsideRange();
    bool IsOutsideRange();
    bool IsTemperatureControlOn();
    bool IsTemperatureControlOff();
    //! Handle substate initialisation
    ReturnCode_t HandleInitializationState();
    //! Handle substate configuration
    ReturnCode_t HandleConfigurationState();
    //! Handle substate motor configuration
    //ReturnCode_t ConfigureDeviceTasks();
    bool DoReferenceRun();
    // bool SetState(bool flag);
    void OnReferenceRun(quint32 InstanceID, ReturnCode_t ReturnCode, qint32 Position);
    void OnSetMotorState(quint32 /*InstanceID*/, ReturnCode_t ReturnCode);
    void OnGetPosition(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qint32 Position, qint8 PosCode);
    void OnSetConfiguration(quint32 /*InstanceID*/, ReturnCode_t ReturnCode);
    void OnGetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint8 Index, qreal Temp);
    void OnSetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Temperature);
    void OnSetTempPid(quint32, ReturnCode_t ReturnCode, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);

    bool MoveToNextPort(bool changeParameter, quint32 LowerLimit, quint32 UpperLimit);
    bool MoveToNextPortCW();
    bool MoveToNextPortCCW();
    quint8 GetRotationDirection();
    bool SetRotationDirection(quint8 direction);
    QString TranslateFromEDPosition(quint32 EDPosition);
    quint32 GetUpperLimit(quint32 CurrentEDPosition, DeviceControl::CANFctModuleStepperMotor::RotationDir_t Direction, bool ChangeDirection);
    quint32 GetLowerLimit(quint32 CurrentEDPosition, DeviceControl::CANFctModuleStepperMotor::RotationDir_t Direction, bool ChangeDirection);
    bool SetConfigLS2Exists(quint8 bExists);
    quint8 GetConfigLS2Exists();
    bool SetConfigPos2Value(quint8 bValid);
    quint8 GetConfigPos2Value();
    bool SetConfigPos3Value(quint8 bValid);
    quint8 GetConfigPos3Value();
    qint8 GetConfigRefRunPosition();
    bool SetConfigRefRunPosition(qint8 refRunRefPos);
    bool SetConfigPos1Stop(quint8 bStop);
    bool SetConfigPos2Stop(quint8 bStop);
    quint8 GetConfigPos2Stop();
    bool SetConfigPos3Stop(quint8 bStop);
    quint8 GetConfigPos3Stop();
    quint8 GetConfigPos1Stop();
    QString GetLimitSwitchCode();
    RVPosition_t GetEDPosition();
    void SetEDPosition(RVPosition_t position);
    void InitEDPosition();
    QString GetPosition();
    void RestoreLastValidParameterSet();
    void Reset();
    bool DoReferenceRunWithStepCheck(quint32 LowerLimit, quint32 UpperLimit);
    bool SetMotorState(bool flag);
    bool ApplyNewParameterSet();

    //! idle state handling task
    void HandleIdleState();
    //! command handling task
    //  void HandleCommandRequestTask();
    //  void HandleDeviceTaskActions();
    //! error handling task
    void HandleErrorState();

    //! Return the oven cover position type from motor position
    //  RVPosition_t GetRVPositionFromEncoderDiskPos(qint32 EDPosition);
    //! Return the motor position from oven cover position type
    // qint32 GetEDPosFromRVPos(RVPosition_t RVPosition);
private:
    //Function modules
    CTemperatureControl* m_pTempCtrl;    //!< Temperature control
    CANFctModuleStepperMotor m_Config;      //!< Copy of configuration.
    CStepperMotor* m_pMotorRV;    //!< Oven cover motor

    /*! Device command type definitions */
    typedef enum {
        RVDEV_CMD_TYPE_UNDEF       = 0x00,    //!< undefined command type
        RVDEV_CMD_INITPOS     = 0x01,    //!< cover reference run
        RVDEV_CMD_REQ_RV_MOVE   = 0x02,    //!< move cover
        RVDEV_CMD_REQ_RV_ACTPOS = 0x03,    //!< request actual cover position
        RVDEV_CMD_SET_TEMP         = 0x04,    //!< set oven temperature
        RVDEV_CMD_REQ_ACT_TEMP     = 0x05,    //!< request actual oven temperature
        RVDEV_CMD_SET_STATUS       = 0x06,    //!< set temperature control state
        RVDEV_CMD_REQ_ACT_STATUS   = 0x07     //!< request actual temperature control state
    } OvenDevCmdType_t;

    /*! command state definitions */
    typedef enum {
        RVDEV_MODULE_CMD_STATE_FREE     = 0x00,   //!< undefined command state
        RVDEV_MODULE_CMD_STATE_REQ      = 0x01,   //!< command was reqested, waits for beeing send to slave
        RVDEV_MODULE_CMD_STATE_REQ_SEND = 0x02,   //!< command requests was sent to the slave
        RVDEV_MODULE_CMD_STATE_REQ_PROC = 0x03,   //!< command requests are processed by device tasks list
        RVDEV_MODULE_CMD_STATE_ERR      = 0x04    //!< command was finished with error
    } RVDevCmdState_t;


    /*! error task state definitiosn */
    typedef enum {
        RV_DEV_ERRTASK_STATE_FREE           = 0x00,   ///< task state free, no error
        RV_DEV_ERRTASK_STATE_REPORT_IFACE   = 0x01,   ///< reports the error to the interface class
        RV_DEV_ERRTASK_STATE_REPORT_DEVPROC = 0x02,   ///< reports the error to device processing
        RV_DEV_ERRTASK_STATE_IDLE           = 0x03,   ///< error idle state, do nothing, block anything
        RV_DEV_ERRTASK_STATE_RESET          = 0x04    ///< reset error state, change to initial state
    } RVDevErrTaskState_t;

    RVDevErrTaskState_t m_ErrorTaskState;  //!< error task state

    // Variables from process settings
    qint32 m_CurrentLimitSwitchCode;
    RVPosition_t m_RVCurrentPosition;
    qint32 m_CurrentPosition;  //!< Current position (stored by asynchronous call)
    qreal m_CurrentTemperature;
    qreal m_TargetTemperature;                     //!< Current temperature
    TempCtrlStatus_t m_TargetTempCtrlStatus;        //!< Target temperature control status; for verification of action result.
    TempCtrlStatus_t m_CurrentTempCtrlStatus;       //!< Current temperature control status
    qint64 m_LastGetTempTime[50];
    TempCtrlMainsVoltage_t m_MainsVoltageStatus;    //!< Mains voltage state of the heaters
    QMutex m_Mutex; //!< Protects the task handling thread from request functions
};


}  //namespace

#endif /* DEVICEROTARYVALVE_H */

