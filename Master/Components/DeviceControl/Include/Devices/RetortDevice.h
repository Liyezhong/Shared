#ifndef RETORTDEVICE_H
#define RETORTDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"


namespace DeviceControl
{
class CTemperatureControl;
class CDigitalOutput;

class CRetortDevice : public CBaseDevice
{
    Q_OBJECT
public:

    //! constructor
    CRetortDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    //! destructor
    virtual ~CRetortDevice();

    //! general task handling function
    void HandleTasks();

    ReturnCode_t SetTempCtrlON(RTTempCtrlType_t Type);
    ReturnCode_t SetTempCtrlOFF(RTTempCtrlType_t type);
    ReturnCode_t SetTemperaturePid(RTTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    ReturnCode_t StartTemperatureControl(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    ReturnCode_t StartTemperatureControlWithPID(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    qreal GetRecentTemperature(RTTempCtrlType_t Type, quint8 Index);
    TempCtrlState_t GetTemperatureControlState(RTTempCtrlType_t Type);


    ReturnCode_t Unlock();
    ReturnCode_t Lock();

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

    bool SetTemperatureControlStatus(RTTempCtrlType_t Type, TempCtrlStatus_t TempCtrlStatus);
    bool SetTemperature(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    qreal GetTemperature(RTTempCtrlType_t Type, quint8 Index);
    bool GetTemperatureAsync(RTTempCtrlType_t Type, quint8 Index);


    bool SetDOValue(quint16 OutputValue, quint16 Duration, quint16 Delay);


    bool IsInsideRange(RTTempCtrlType_t Type);
    bool IsOutsideRange(RTTempCtrlType_t Type);
    bool IsTemperatureControlOn(RTTempCtrlType_t Type);
    bool IsTemperatureControlOff(RTTempCtrlType_t Type);


    void CheckSensorsData();


    void OnSetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Temperature);
    void OnGetTemp(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Index, qreal Temp);
    void OnSetTempPid(quint32, ReturnCode_t ReturnCode, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    void OnSetDOOutputValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 OutputValue);
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
    CTemperatureControl* m_pTempCtrls[RT_TEMP_CTRL_NUM];
    CDigitalOutput* m_pLockDigitalOutput;


    qreal m_CurrentTemperatures[RT_TEMP_CTRL_NUM];                     //!< Current temperature
    qreal m_TargetTemperatures[RT_TEMP_CTRL_NUM];                     //!< Current temperature
    TempCtrlStatus_t m_TargetTempCtrlStatus[RT_TEMP_CTRL_NUM];        //!< Target temperature control status; for verification of action result.
    TempCtrlStatus_t m_CurrentTempCtrlStatus[RT_TEMP_CTRL_NUM];       //!< Current temperature control status
    TempCtrlMainsVoltage_t m_MainsVoltageStatus[RT_TEMP_CTRL_NUM];    //!< Mains voltage state of the heaters
    qint64 m_LastGetTempTime[RT_TEMP_CTRL_NUM][5];
    QMap<quint32, RTTempCtrlType_t> m_InstTCTypeMap;

    qint16 m_TargetDOOutputValue;     //!< Target output value; for verification of action result


    /*! error task state definitiosn */
    typedef enum {
        RT_DEV_ERRTASK_STATE_FREE           = 0x00,   ///< task state free, no error
        RT_DEV_ERRTASK_STATE_REPORT_IFACE   = 0x01,   ///< reports the error to the interface class
        RT_DEV_ERRTASK_STATE_REPORT_DEVPROC = 0x02,   ///< reports the error to device processing
        RT_DEV_ERRTASK_STATE_IDLE           = 0x03,   ///< error idle state, do nothing, block anything
        RT_DEV_ERRTASK_STATE_RESET          = 0x04    ///< reset error state, change to initial state
    } RTDevErrTaskState_t;

    RTDevErrTaskState_t m_ErrorTaskState;  //!< error task state
    QMutex m_Mutex; //!< Protects the task handling thread from request functions

};
}

#endif // RETORTDEVICE_H
