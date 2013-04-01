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

#ifndef PRE_ALFA_TEST
public slots:
bool DoReferenceRun();
bool DriveSpeed(qint16 Speed);
bool MoveToPosition(qint32 Position, qint32 Timeout = -1);
bool MoveFromPosition(qint32 Distance, qint32 Timeout = -1);
QString GetPosition();
QString GetLimitSwitchCode();
qint16 GetSpeed();
bool SetProfile(qint8 Profile);
qint8 GetProfile();
bool Wait(qint32 Timeout = -1);
void Reset();
bool Enable();
bool Disable();
bool ApplyNewParameterSet();
void RestoreLastValidParameterSet();
void ApplyNewParameterSetTimerCB();


/*
    bool SetConfigRefRunRefPosSkip(quint8 refRunRefPosSkip);
    quint8 GetConfigRefRunRefPosSkip();
    */
// bool SetConfigPos1HitSkip(quint8 hitSkip);
// quint8 GetConfigPos1HitSkip();
// bool SetConfigPos2HitSkip(quint8 hitSkip);
// quint8 GetConfigPos2HitSkip();
// bool SetConfigPos3HitSkip(quint8 hitSkip);
// quint8 GetConfigPos3HitSkip();
quint8 GetConfigPos2Stop();
bool SetConfigPos2Stop(quint8 bStop);
bool SetConfigPos3Stop(quint8 bStop);
quint8 GetConfigPos3Stop();
bool SetConfigPos3StopDir(quint8 bStopDir);
quint8 GetConfigPos3StopDir();

bool SetConfigPos2Value(quint8 bValid);
quint8 GetConfigPos2Value();
bool SetConfigPos3Value(quint8 bValid);
quint8 GetConfigPos3Value();

bool SetRotationDirection(quint8 direction);
quint8 GetRotationDirection();
bool MoveToInitialPosition();

bool MoveToTubePosition(quint32 Position);
bool MoveToSealPosition(quint32 Position);

bool MoveCW();
bool MoveCCW();

private slots:
void OnReferenceRun(quint32 InstanceID, ReturnCode_t ReturnCode, qint32 Position);
void OnSetPosition(quint32 InstanceID, ReturnCode_t ReturnCode, qint32 Position, qint16 Speed);
void OnGetPosition(quint32 InstanceID, ReturnCode_t ReturnCode, qint32 Position, qint8 PosCode);
void OnSetState(quint32 InstanceID, ReturnCode_t ReturnCode);
void OnSetConfiguration(quint32 InstanceID, ReturnCode_t ReturnCode);
void OnGetSpeed(quint32 InstanceID, ReturnCode_t ReturnCode, qint16 Speed);
void ReferenceRunTimerCB();
bool SetPosition(qint32 Position, qint32 Timeout);
bool SetState(bool flag);
qint32 ReferenceRunWithTimeout(quint32 LowerLimit, quint32 UpperLimit);
bool DoReferenceRunWithStepCheck(quint32 LowerLimit, quint32 UpperLimit);
bool MoveToNextPortCW();
bool MoveToNextPortCCW();
bool MoveToNextPort(bool changeParameter, quint32 LowerLimit, quint32 UpperLimit);
bool MoveToEDPosition(quint32 Position);
quint32 GetEDPosition();
void SetEDPosition(quint32 position);
void InitEDPosition();
QString TranslateFromEDPosition(quint32 EDPosition);
inline quint32 GetUpperLimit(quint32 CurrentEDPosition, DeviceControl::CANFctModuleStepperMotor::RotationDir_t Direction, bool ChangeDirection);
inline quint32 GetLowerLimit(quint32 CurrentEDPosition, DeviceControl::CANFctModuleStepperMotor::RotationDir_t Direction, bool ChangeDirection);

bool ReportStatus(ReturnCode_t ReturnCode);
void Log(const QString& msg);
float m_EncoderDiskPosition;
qint32 m_RefRunRetCode;
QEventLoop m_LoopApplyNewParameter;

CStepperMotor *mp_StepperMotor;  //!< Pointer to the StepperMotor FM
CANFctModuleStepperMotor m_Config;      //!< Copy of configuration.

QEventLoop m_LoopReferenceRun;   //!< Loop for blocking commands.
QEventLoop m_LoopSetPosition;    //!< Loop for blocking commands.
QEventLoop m_LoopRotation;       //!< Loop for blocking commands.
QEventLoop m_LoopGetPosition;    //!< Loop for blocking commands.
QEventLoop m_LoopSetState;       //!< Loop for blocking commands.
QEventLoop m_LoopSetConfiguration;  //!< Loop for blocking commands.
QEventLoop m_LoopWait;           //!< Loop for blocking commands.
QEventLoop m_LoopGetSpeed;           //!< Loop for blocking commands.

bool m_SetPositionActive;        //!< Flag for an active asynchronous action.
bool m_RotationOn;               //!< Flag for the rotation state.
bool m_CurrentPositionKnown;     //!< Ensure a known absolute Position for relative movement.

qint32 m_TargetPosition;   //!< Target position of linear movement; for verification of action result.
qint32 m_CurrentPosition;  //!< Current position (stored by asynchronous call)
qint32 m_CurrentLimitSwitchCode;  //!< Current limit switch code (0: none; 1: LS1 active; 2: LS2 active; 3: both active) (stored by asynchronous call)
qint16 m_TargetSpeed;      //!< Target speed of rotation; for verification of action result.
qint16 m_CurrentSpeed;     //!< Current speed (stored by asynchronous call)
qint16 m_Speed;            //!< Default speed.
qint8  m_Profile;          //!< Default profile.
};
#endif
