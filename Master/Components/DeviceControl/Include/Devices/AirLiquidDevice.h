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
#define DRAINGING_PRESSURE_BUILD_TIME   (120*1000) //Max time for setup pressure
#define DRAINGING_SETUP_WARNING_TIME    (120*1000) //Td_Err
#define DRAINGING_MAX_SETUP_TIME        (240*1000) //Td_Err
#define DRAINGING_POLLING_TIME          (1000)
#define DRAINGING_RET_OK                (1)
#define DRAINGING_RET_GENERAL_ERR       (-1)
#define DRAINGING_RET_TIMEOUT           (-2)

#define SUCKING_TARGET_PRESSURE       (-30)
#define SUCKING_OVERFLOW_PRESSURE         (-25)
#define SUCKING_OVERFLOW_4SAMPLE_DELTASUM (-2.5)
#define SUCKING_INSUFFICIENT_PRESSURE     (-20)
#define SUCKING_INSUFFICIENT_4SAMPLE_DELTASUM (1)

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

/****************************************************************************/
/*!
*   \brief This class implements the functionality to configure and control a
*          'Air-Liquid' device
*/
/****************************************************************************/
class CAirLiquidDevice : public CBaseDevice
{
    Q_OBJECT
public:

    //! constructor
    CAirLiquidDevice(DeviceProcessing* pDeviceProcessing, QString& Type,
                     const DeviceModuleList_t &ModuleList,
                     quint32 InstanceID);
    //! destructor
    virtual ~CAirLiquidDevice();

    //! general task handling function
    void HandleTasks();
    //! Air Liquid device function
    ReturnCode_t SetPressureCtrlON(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetPressureCtrlOFF
     *
     *  \return from SetPressureCtrlOFF
     */
    /****************************************************************************/
    ReturnCode_t SetPressureCtrlOFF(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ReleasePressure
     *
     *  \return from ReleasePressure
     */
    /****************************************************************************/
    ReturnCode_t ReleasePressure(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Pressure
     *
     *  \return from Pressure
     */
    /****************************************************************************/
    ReturnCode_t Pressure(float targetPressure = AL_TARGET_PRESSURE_POSITIVE);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SealingCheckPressure
     *
     *  \return from SealingCheckPressure
     */
    /****************************************************************************/
    ReturnCode_t SealingCheckPressure();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Vaccum
     *
     *  \return from Vaccum
     */
    /****************************************************************************/
    ReturnCode_t Vaccum(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Draining
     *
     *  \param DelayTime = quint32 type parameter
     *
     *  \return from Draining
     */
    /****************************************************************************/
    ReturnCode_t Draining(quint32 DelayTime, float targetPressure = AL_TARGET_PRESSURE_POSITIVE);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Filling
     *
     *  \param DelayTime = quint32 type parameter
     *  \iparam  EnableInsufficientCheck = Enable Insufficient Check
     *
     *  \return from Filling
     */
    /****************************************************************************/
    ReturnCode_t Filling(quint32 DelayTime, bool EnableInsufficientCheck);

    /****************************************************************************/
    /*!
     *  \brief   Stop command execution
     *
     *  \iparam  void
     *
     *  \return  void
     */
    /****************************************************************************/
    void StopCommandExec(quint8 CmdType);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Filling4Service
     *
     *  \param DelayTime = quint32 type parameter
     *  \iparam  EnableInsufficientCheck = Enable Insufficient Check
     *
     *  \return from Filling
     */
    /****************************************************************************/
    ReturnCode_t FillingForService(quint32 DelayTime, bool EnableInsufficientCheck);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetRecentPressure
     *
     *  \return from GetRecentPressure
     */
    /****************************************************************************/
    qreal GetRecentPressure(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function PressureForBottoleCheck
     *
     *  \return from PressureForBottoleCheck
     */
    /****************************************************************************/
    ReturnCode_t PressureForBottoleCheck(void);


    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetTempCtrlON
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *
     *  \return from SetTempCtrlON
     */
    /****************************************************************************/
    ReturnCode_t SetTempCtrlON(ALTempCtrlType_t Type);

    ReturnCode_t SetTempCtrlOFF(ALTempCtrlType_t type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetTemperaturePid
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from SetTemperaturePid
     */
    /****************************************************************************/
    ReturnCode_t SetTemperaturePid(ALTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StartTemperatureControl
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *
     *  \return from StartTemperatureControl
     */
    /****************************************************************************/
    ReturnCode_t StartTemperatureControl(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StartTemperatureControlWithPID
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *  \param MaxTemperature =  quint1SigStopCommandExec6 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from StartTemperatureControlWithPID
     */
    /****************************************************************************/
    ReturnCode_t StartTemperatureControlWithPID(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetRecentTemperature
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *  \param Index =  quint8 type parameter
     *
     *  \return from GetRecentTemperature
     */
    /****************************************************************************/
    qreal GetRecentTemperature(ALTempCtrlType_t Type, quint8 Index);

    /****************************************************************************/
    /*!
     *  \brief   Get recent current of sensor captured in last 500 milliseconds.
     *
     *  \iparam  Type = The target temperature contorl module to control.
     *
     *  \return  Actual current, UNDEFINED if failed.
     */
    /****************************************************************************/
    quint32 GetRecentCurrent(ALTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetTemperatureControlState
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *
     *  \return from GetTemperatureControlState
     */
    /****************************************************************************/
    TempCtrlState_t GetTemperatureControlState(ALTempCtrlType_t Type);


    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function TurnOnFan
     *
     *  \return from TurnOnFan
     */
    /****************************************************************************/
    ReturnCode_t TurnOnFan();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function TurnOffFan
     *
     *  \return from TurnOffFan
     */
    /****************************************************************************/
    ReturnCode_t TurnOffFan();
    //qint32 GetFanOperationTime();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function BreakAllOperation
     *
     *  \return from BreakAllOperation
     */
    /****************************************************************************/
    ReturnCode_t BreakAllOperation(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function AllStop
     *
     *  \return from AllStop
     */
    /****************************************************************************/
    ReturnCode_t AllStop(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ControlValve
     *
     *  \return from ControlValve
     */
    /****************************************************************************/
    ReturnCode_t ControlValve(quint8 ValveIndex, quint8 ValveState);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetPressureDrift
     *
     *  \param pressureDrift = float type parameter
     *
     *  \return from SetPressureDrift
     */
    /****************************************************************************/
    ReturnCode_t SetPressureDrift(float pressureDrift);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetHeaterCurrent
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *
     *  \return from GetHeaterCurrent
     */
    /****************************************************************************/
    quint16 GetHeaterCurrent(ALTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot StopCompressor
     */
    /****************************************************************************/
    void StopCompressor(void);
    /****************************************************************************/
    /*!
     *  \brief  Reset the life time of ActiveCarbonFilter
     */
    /****************************************************************************/
    void ResetActiveCarbonFilterLifeTime();
private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot Reset
     */
    /****************************************************************************/
    void Reset();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot HandleInitializationState
     */
    /****************************************************************************/
    ReturnCode_t HandleInitializationState();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot HandleConfigurationState
     */
    /****************************************************************************/
    ReturnCode_t HandleConfigurationState();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot ConfigureDeviceTasks
     */
    /****************************************************************************/
    ReturnCode_t ConfigureDeviceTasks();
    //! idle state handling task
    void HandleIdleState();
    //! error handling task
    void HandleErrorState();
    // void HandleErrorState();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetPressure
     */
    /****************************************************************************/
    ReturnCode_t SetPressure(quint8 flag, float NominalPressure);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetPressure
     */
    /****************************************************************************/
    qreal GetPressure(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetTargetPressure
     */
    /****************************************************************************/
    ReturnCode_t SetTargetPressure(quint8 flag, float pressure);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetValve
     */
    /****************************************************************************/
    ReturnCode_t SetValve(quint8 ValveIndex, quint8 ValveState);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot etPressureControlStatus
     */
    /****************************************************************************/
    ReturnCode_t SetPressureControlStatus(PressureCtrlStatus_t PressureCtrlStatus);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetTemperatureControlStatus
     */
    /****************************************************************************/
    ReturnCode_t SetTemperatureControlStatus(ALTempCtrlType_t Type, TempCtrlStatus_t TempCtrlStatus);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetTemperature
     */
    /****************************************************************************/
    ReturnCode_t SetTemperature(ALTempCtrlType_t Type, float NominalTemperature, quint8 SlopeTempChange);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetTemperature
     */
    /****************************************************************************/
    qreal GetTemperature(ALTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetTemperatureAsync
     */
    /****************************************************************************/
    ReturnCode_t GetTemperatureAsync(ALTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetPressureAsync
     */
    /****************************************************************************/
    ReturnCode_t GetPressureAsync(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetHeaterCurrentAsync
     */
    /****************************************************************************/
    ReturnCode_t GetHeaterCurrentAsync(ALTempCtrlType_t Type);


    //ReturnCode_t SetDOValue(quint16 OutputValue, quint16 Duration, quint16 Delay);


    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot IsInsideRange
     */
    /****************************************************************************/
    bool IsInsideRange(ALTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot IsOutsideRange
     */
    /****************************************************************************/
    bool IsOutsideRange(ALTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot IsTemperatureControlOn
     */
    /****************************************************************************/
    bool IsTemperatureControlOn(ALTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot IsTemperatureControlOf
     */
    /****************************************************************************/
    bool IsTemperatureControlOff(ALTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot IsPIDDataSteady
     */
    /****************************************************************************/
    bool IsPIDDataSteady(qreal TargetValue, qreal CurrentValue, qreal Tolerance, qint32 Num, bool Init);


    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot  PressureTimerCB
     */
    /****************************************************************************/
    void PressureTimerCB(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot VaccumTimerCB
     */
    /****************************************************************************/
    void VaccumTimerCB(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot DrainingTimerCB
     */
    /****************************************************************************/
    void DrainingTimerCB(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot ReleasePressureTimerCB
     */
    /****************************************************************************/
    void ReleasePressureTimerCB(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SuckingTimerCB
     */
    /****************************************************************************/
    void SuckingTimerCB(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot CheckSensorsData
     */
    /****************************************************************************/
    void CheckSensorsData();


    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnSetPressure
     */
    /****************************************************************************/
    void OnSetPressure(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, float TargetPressure);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnSetValve
     */
    /****************************************************************************/
    void OnSetValve(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint8 ValveIndex, quint8 ValveState);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnGetPressure
     */
    /****************************************************************************/
    void OnGetPressure(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint8 Index, float ActPressure);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnSetTemp
     */
    /****************************************************************************/
    void OnSetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Temperature);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnGetTemp
     */
    /****************************************************************************/
    void OnGetTemp(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Index, qreal Temp);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnSetTempPid
     */
    /****************************************************************************/
    void OnSetTempPid(quint32, ReturnCode_t ReturnCode, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    //void OnSetDOOutputValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 OutputValue);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnLevelSensorState
     */
    /****************************************************************************/
    void OnLevelSensorState(quint32, ReturnCode_t ReturnCode, quint8 State);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnTempControlStatus
     */
    /****************************************************************************/
    void OnTempControlStatus(quint32 /*InstanceID*/, ReturnCode_t ReturnCode,TempCtrlStatus_t TempCtrlStatus, TempCtrlMainsVoltage_t MainsVoltage);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnGetLifeTime
     */
    /****************************************************************************/
    void OnGetLifeTime(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint32 LifeTime, quint32 LifeCycles);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnSetFanStatus
     */
    /****************************************************************************/
    void OnSetFanStatus(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 FanStatus);
    //void OnFunctionModuleError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnTCGetHardwareStatus
     */
    /****************************************************************************/
    void OnTCGetHardwareStatus(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Sensors, quint8 Fans,
                                quint8 Heaters, quint8 Pids, quint16 Current, quint8 HeaterSwitchType);
    //! command handling task
    //  void HandleCommandRequestTask();
    //  void HandleDeviceTaskActions();

    //! Return the oven cover position type from motor position
    //  RVPosition_t GetRVPositionFromEncoderDiskPos(qint32 EDPosition);
    //! Return the motor position from oven cover position type
    // qint32 GetEDPosFromRVPos(RVPosition_t RVPosition);
signals:
    /****************************************************************************/
    /*!
     *  \brief  Signal for Stopping command execution
     */
    /****************************************************************************/
    void SigStopCommandExec(quint8 CmdType);

private:
    //Function modules
    CPressureControl* m_pPressureCtrl;                   //!< Pressure control FM
    CTemperatureControl* m_pTempCtrls[AL_TEMP_CTRL_NUM]; //!< Temperature controls FMs for the heaters
    //CDigitalOutput* m_pFanDigitalOutput;                 //!< Digital output FM for the fan

    float m_TargetPressure;                              //!< Target pressure; for verification of action result.
    float m_CurrentPressure;                             //!< Current pressure
    float m_PressureDrift;                               //!< Current pressure drift
    qint32 m_WorkingPressurePositive;                    //!< Positive working pressure
    qint32 m_WorkingPressureNegative;                    //!< Negative working pressure
    PressureCtrlStatus_t m_TargetPressureCtrlStatus;     //!< Target pressure control status; for verification of action result.
    PressureCtrlStatus_t m_CurrentPressureCtrlStatus;    //!< Current pressure control status
    qint64 m_LastGetPressureTime;                        //!< Last time of getting pressure
    QList<qreal> m_PIDDataList;                          //!< PID parameters list

    qreal m_CurrentTemperatures[AL_TEMP_CTRL_NUM][MAX_SENSOR_PER_TEMP_CTRL];   //!< Current temperature
    qreal m_TargetTemperatures[AL_TEMP_CTRL_NUM];                     //!< Current temperature
    TempCtrlStatus_t m_TargetTempCtrlStatus[AL_TEMP_CTRL_NUM];        //!< Target temperature control status; for verification of action result.
    TempCtrlStatus_t m_CurrentTempCtrlStatus[AL_TEMP_CTRL_NUM];       //!< Current temperature control status
    TempCtrlMainsVoltage_t m_MainsVoltageStatus[AL_TEMP_CTRL_NUM];    //!< Mains voltage state of the heaters
    TempCtrlHardwareStatus_t m_TCHardwareStatus[AL_TEMP_CTRL_NUM];    //!< Hardware status of the heaters
    quint32 m_SuckingTime[16];                                        //!< in mec, idx 1-13 is port 1-13, idx 0 is unused
    qint64 m_LastGetTempTime[AL_TEMP_CTRL_NUM][5];                    //!< Last time of getting temperature
    qint64 m_LastGetTCCurrentTime[AL_TEMP_CTRL_NUM];                  //!< Last time of getting current
    QMap<quint32, ALTempCtrlType_t> m_InstTCTypeMap;                  //!< Map between instance ID and temperature control

    qint16 m_TargetDOOutputValue;     //!< Target output value; for verification of action result
    quint32 m_DOLifeTime;             //!< Digital output life time
    quint32 m_DOLifeCycles;           //!< Digital output life cycles

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

