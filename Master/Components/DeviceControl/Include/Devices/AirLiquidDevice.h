#ifndef AIRLIQUIDDEVICE_H
#define AIRLIQUIDDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"


namespace DeviceControl
{
class CPressureControl;
class CTemperatureControl;
class CDigitalOutput;

#define RELEASE_PRESSURE_MAX_TIME (20*1000)
#define RELEASE_PRESSURE_PROCEDURE_INTERRUPT (-1)
#define VALVE_1_INDEX (0)
#define VALVE_2_INDEX (1)
#define VALVE_STATE_OPEN (1)
#define VALVE_STATE_CLOSE (0)
#define PRESSURE_STATIC_DIFFERENCE     (2)
#define PRESSURE_MAX_SETUP_TIME        (120*1000)  //Tp_Rrr
#define PRESSURE_HOLD_TIME             (5*1000)    //Tp_Delay
#define PRESSURE_POLLING_TIME          (500)       //Polling time
#define PRESSURE_PID_STEADY_NUM        (10)

#define DRAINGING_TARGET_THRESHOLD_PRESSURE  (15)
#define DRAINGING_TARGET_FINISHED_PRESSURE   (10)
#define DRAINGING_SETUP_WARNING_TIME    (120*1000) //Td_Err
#define DRAINGING_MAX_SETUP_TIME        (240*1000) //Td_Err
#define DRAINGING_POLLING_TIME          (1000)
#define DRAINGING_RET_OK                (1)
#define DRAINGING_RET_GENERAL_ERR       (-1)
#define DRAINGING_RET_TIMEOUT           (-2)

#define SUCKING_TARGET_PRESSURE       (-30)
#define SUCKING_MAX_DELAY_TIME        (5000)
#define SUCKING_POOLING_TIME          (400)
#define SUCKING_SETUP_WARNING_TIME    (120*1000)
#define SUCKING_MAX_SETUP_TIME        (240*1000)
#define SUCKING_OVERFLOW_SAMPLE_SIZE    (8)
#define SUCKING_OVERFLOW_TOLERANCE      (2)
#define SUCKING_OVERFLOW_WINDOW_SIZE    (3)
#define SUCKING_OVERFLOW_THRESHOLD      (10)
#define SUCKING_RET_OK                  (1)
#define SUCKING_RET_GENERAL_FAIL        (-1)
#define SUCKING_RET_TIMEOUT             (-2)
#define SUCKING_RET_OVERFLOW            (-3)

#define VACCUM_STATIC_DIFFERENCE     (2)
#define VACCUM_PID_STEADY_NUM        (10)
#define VACCUM_MAX_SETUP_TIME        (120*1000)  //Tv_Rrr
#define VACCUM_HOLD_TIME             (5*1000)  //Tv_Delay
#define VACCUM_POLLING_TIME          (500)  //Polling time
class CAirLiquidDevice : public CBaseDevice
{
    Q_OBJECT
public:

    //! constructor
    CAirLiquidDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    //! destructor
    virtual ~CAirLiquidDevice();

    //! general task handling function
    void HandleTasks();
    //! Air Liquid device function
    ReturnCode_t SetPressureCtrlON();
    ReturnCode_t SetPressureCtrlOFF();
    ReturnCode_t ReleasePressure(void);
    ReturnCode_t Pressure();
    ReturnCode_t Vaccum();
    ReturnCode_t Draining(quint32 DelayTime);
    ReturnCode_t Filling(quint32 DelayTime);
    qreal GetRecentPressure(quint8 Index);


    ReturnCode_t SetTempCtrlON(ALTempCtrlType_t Type);
    ReturnCode_t SetTempCtrlOFF(ALTempCtrlType_t type);
    ReturnCode_t SetTemperaturePid(ALTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    ReturnCode_t StartTemperatureControl(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    ReturnCode_t StartTemperatureControlWithPID(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    qreal GetRecentTemperature(ALTempCtrlType_t Type, quint8 Index);
    TempCtrlState_t GetTemperatureControlState(ALTempCtrlType_t Type);


    ReturnCode_t TurnOnFan();
    ReturnCode_t TurnOffFan();
    ReturnCode_t BreakAllOperation(void);

private slots:
    void Reset();
    ReturnCode_t HandleInitializationState();
    ReturnCode_t HandleConfigurationState();
    ReturnCode_t ConfigureDeviceTasks();
    //! idle state handling task
    void HandleIdleState();
    //! error handling task
    void HandleErrorState();
    // void HandleErrorState();
    bool SetPressure(quint8 flag, qreal NominalPressure);
    qreal GetPressure(quint8 Index);
    bool SetTargetPressure(qreal pressure);
    bool SetValve(quint8 ValveIndex, quint8 ValveState);
    bool SetPressureControlStatus(PressureCtrlStatus_t PressureCtrlStatus);
    void StopCompressor(void);


    bool SetTemperatureControlStatus(ALTempCtrlType_t Type, TempCtrlStatus_t TempCtrlStatus);
    bool SetTemperature(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    qreal GetTemperature(ALTempCtrlType_t Type, quint8 Index);
    bool GetTemperatureAsync(ALTempCtrlType_t Type, quint8 Index);
    bool GetPressureAsync(quint8 Index);


    bool SetDOValue(quint16 OutputValue, quint16 Duration, quint16 Delay);


    bool IsInsideRange(ALTempCtrlType_t Type);
    bool IsOutsideRange(ALTempCtrlType_t Type);
    bool IsTemperatureControlOn(ALTempCtrlType_t Type);
    bool IsTemperatureControlOff(ALTempCtrlType_t Type);
    bool IsPIDDataSteady(qreal TargetValue, qreal CurrentValue, qreal Tolerance, qint32 Num, bool Init);


    void PressureTimerCB(void);
    void VaccumTimerCB(void);
    void DrainingTimerCB(void);
    void ReleasePressureTimerCB(void);
    void SuckingTimerCB(void);
    void CheckSensorsData();


    void OnSetPressure(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Pressure);
    void OnSetValve(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint8 ValveIndex, quint8 ValveState);
    void OnGetPressure(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint8 Index, qreal Pressure);
    void OnSetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Temperature);
    void OnGetTemp(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Index, qreal Temp);
    void OnSetTempPid(quint32, ReturnCode_t ReturnCode, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    void OnSetDOOutputValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 OutputValue);
    void OnLevelSensorState(quint32, ReturnCode_t ReturnCode, quint8 State);
    void OnTempControlStatus(quint32 /*InstanceID*/, ReturnCode_t ReturnCode,TempCtrlStatus_t TempCtrlStatus, TempCtrlMainsVoltage_t MainsVoltage);

    //! command handling task
    //  void HandleCommandRequestTask();
    //  void HandleDeviceTaskActions();

    //! Return the oven cover position type from motor position
    //  RVPosition_t GetRVPositionFromEncoderDiskPos(qint32 EDPosition);
    //! Return the motor position from oven cover position type
    // qint32 GetEDPosFromRVPos(RVPosition_t RVPosition);

private:
    //Function modules
    CPressureControl* m_pPressureCtrl;
    CTemperatureControl* m_pTempCtrls[AL_TEMP_CTRL_NUM];
    CDigitalOutput* m_pFanDigitalOutput;

    qreal m_TargetPressure;                              //!< Target pressure; for verification of action result.
    qreal m_CurrentPressure;                             //!< Current pressure
    qreal m_PressureDrift;
    qint32 m_WorkingPressurePositive;
    qint32 m_WorkingPressureNegative;
    PressureCtrlStatus_t m_TargetPressureCtrlStatus;     //!< Target pressure control status; for verification of action result.
    PressureCtrlStatus_t m_CurrentPressureCtrlStatus;    //!< Current pressure control status
    qint64 m_LastGetPressureTime[5];
    QList<qreal> m_PIDDataList;

    qreal m_CurrentTemperatures[AL_TEMP_CTRL_NUM];                     //!< Current temperature
    qreal m_TargetTemperatures[AL_TEMP_CTRL_NUM];                     //!< Current temperature
    TempCtrlStatus_t m_TargetTempCtrlStatus[AL_TEMP_CTRL_NUM];        //!< Target temperature control status; for verification of action result.
    TempCtrlStatus_t m_CurrentTempCtrlStatus[AL_TEMP_CTRL_NUM];       //!< Current temperature control status
    TempCtrlMainsVoltage_t m_MainsVoltageStatus[AL_TEMP_CTRL_NUM];    //!< Mains voltage state of the heaters
    quint32 m_SuckingTime[50];  // in mec, idx 1-13 is port 1-13, idx 0 is unused
    qint64 m_LastGetTempTime[AL_TEMP_CTRL_NUM][5];
    QMap<quint32, ALTempCtrlType_t> m_InstTCTypeMap;

    qint16 m_TargetDOOutputValue;     //!< Target output value; for verification of action result

    QEventLoop m_LoopGetPressure;
    QEventLoop m_LoopPressureTimer;
    QEventLoop m_LoopDrainingTimer;
    QEventLoop m_LoopSuckingTimer;
    QEventLoop m_LoopSuckingLevelSensor;
    QEventLoop m_LoopReleasePressureTimer;

    /*! error task state definitiosn */
    typedef enum {
        AL_DEV_ERRTASK_STATE_FREE           = 0x00,   ///< task state free, no error
        AL_DEV_ERRTASK_STATE_REPORT_IFACE   = 0x01,   ///< reports the error to the interface class
        AL_DEV_ERRTASK_STATE_REPORT_DEVPROC = 0x02,   ///< reports the error to device processing
        AL_DEV_ERRTASK_STATE_IDLE           = 0x03,   ///< error idle state, do nothing, block anything
        AL_DEV_ERRTASK_STATE_RESET          = 0x04    ///< reset error state, change to initial state
    } ALDevErrTaskState_t;

    ALDevErrTaskState_t m_ErrorTaskState;  //!< error task state
    QMutex m_Mutex; //!< Protects the task handling thread from request functions

};
}

#endif // AIRLIQUIDDEVICE_H

