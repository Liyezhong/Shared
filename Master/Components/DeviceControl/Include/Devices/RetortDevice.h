#ifndef RETORTDEVICE_H
#define RETORTDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"


namespace DeviceControl
{
class CTemperatureControl;
class CDigitalOutput;
class CDigitalInput;

/****************************************************************************/
/*!
*   \brief This class implements the functionality to configure and control a
*          'Retort' device
*/
/****************************************************************************/
class CRetortDevice : public CBaseDevice
{
    Q_OBJECT
public:

    //! constructor
    CRetortDevice(DeviceProcessing* pDeviceProcessing, QString& Type,
                  const DeviceModuleList_t &ModuleList,
                  quint32 InstanceID);
    //! destructor
    virtual ~CRetortDevice();

    //! general task handling function
    void HandleTasks();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetTempCtrlON
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *
     *  \return from SetTempCtrlON
     */
    /****************************************************************************/
    ReturnCode_t SetTempCtrlON(RTTempCtrlType_t Type);
    ReturnCode_t SetTempCtrlOFF(RTTempCtrlType_t type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetTemperaturePid
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from SetTemperaturePid
     */
    /****************************************************************************/
    ReturnCode_t SetTemperaturePid(RTTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StartTemperatureControl
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *
     *  \return from StartTemperatureControl
     */
    /****************************************************************************/
    ReturnCode_t StartTemperatureControl(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StartTemperatureControlWithPID
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from StartTemperatureControlWithPID
     */
    /****************************************************************************/
    ReturnCode_t StartTemperatureControlWithPID(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetRecentTemperature
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *  \param Index =  quint8 type parameter
     *
     *  \return from GetRecentTemperature
     */
    /****************************************************************************/
    qreal GetRecentTemperature(RTTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetTemperatureControlState
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *
     *  \return from GetTemperatureControlState
     */
    /****************************************************************************/
    TempCtrlState_t GetTemperatureControlState(RTTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetHardwareStatus
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *
     *  \return from GetHardwareStatus
     */
    /****************************************************************************/
    TempCtrlHardwareStatus_t GetHardwareStatus(RTTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetLidStatus
     *
     *  \return from GetLidStatus
     */
    /****************************************************************************/
    quint16 GetLidStatus();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetRecentRetortLockStatus
     *
     *  \return from GetRecentRetortLockStatus
     */
    /****************************************************************************/
    quint16 GetRecentRetortLockStatus();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Unlock
     *
     *  \return from Unlock
     */
    /****************************************************************************/
    ReturnCode_t Unlock();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Lock
     *
     *  \return from Lock
     */
    /****************************************************************************/
    ReturnCode_t Lock();

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnError
     */
    /****************************************************************************/
    void Reset();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnError
     */
    /****************************************************************************/
    ReturnCode_t HandleInitializationState();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnError
     */
    /****************************************************************************/
    ReturnCode_t HandleConfigurationState();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnError
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
     *  \brief  Definition/Declaration of slot SetTemperatureControlStatus
     */
    /****************************************************************************/
    ReturnCode_t SetTemperatureControlStatus(RTTempCtrlType_t Type, TempCtrlStatus_t TempCtrlStatus);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetTemperature
     */
    /****************************************************************************/
    ReturnCode_t SetTemperature(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetTemperature
     */
    /****************************************************************************/
    qreal GetTemperature(RTTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetTemperatureAsync
     */
    /****************************************************************************/
    ReturnCode_t GetTemperatureAsync(RTTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetLockStatusAsync
     */
    /****************************************************************************/
    ReturnCode_t GetLockStatusAsync();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetDOValue
     */
    /****************************************************************************/
    ReturnCode_t SetDOValue(quint16 OutputValue, quint16 Duration, quint16 Delay);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot IsInsideRange
     */
    /****************************************************************************/
    bool IsInsideRange(RTTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot IsOutsideRange
     */
    /****************************************************************************/
    bool IsOutsideRange(RTTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot IsTemperatureControlOn
     */
    /****************************************************************************/
    bool IsTemperatureControlOn(RTTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot IsTemperatureControlOff
     */
    /****************************************************************************/
    bool IsTemperatureControlOff(RTTempCtrlType_t Type);


    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot CheckSensorsData
     */
    /****************************************************************************/
    void CheckSensorsData();

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
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnSetDOOutputValue
     */
    /****************************************************************************/
    void OnSetDOOutputValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 OutputValue);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnTempControlStatus
     */
    /****************************************************************************/
    void OnTempControlStatus(quint32 /*InstanceID*/, ReturnCode_t ReturnCode,TempCtrlStatus_t TempCtrlStatus, TempCtrlMainsVoltage_t MainsVoltage);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnGetDIValue
     */
    /****************************************************************************/
    void OnGetDIValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 InputValue);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnGetHardwareStatus
     */
    /****************************************************************************/
    void OnGetHardwareStatus(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Sensors, quint8 Fans, quint8 Heaters, quint8 Pids, quint16 Current, quint8 HeaterSwitchType);
    //! command handling task
    //  void HandleCommandRequestTask();
    //  void HandleDeviceTaskActions();

    //! Return the oven cover position type from motor position
    //  RVPosition_t GetRVPositionFromEncoderDiskPos(qint32 EDPosition);
    //! Return the motor position from oven cover position type
    // qint32 GetEDPosFromRVPos(RVPosition_t RVPosition);

private:
    //Function modules
    CTemperatureControl* m_pTempCtrls[RT_TEMP_CTRL_NUM];              //!< Temperature control function modules used in this device
    CDigitalOutput* m_pLockDigitalOutput;                             //!< Digital output for lock
    CDigitalInput* m_pLockDigitalInput;                               //!< Digital input for lock
    TempCtrlHardwareStatus_t m_HardwareStatus[RT_TEMP_CTRL_NUM];      //!< Hardware status for temperature control modules

    qreal m_CurrentTemperatures[RT_TEMP_CTRL_NUM][MAX_SENSOR_PER_TEMP_CTRL];                  //!< Current temperature
    qreal m_TargetTemperatures[RT_TEMP_CTRL_NUM];                     //!< Current temperature
    TempCtrlStatus_t m_TargetTempCtrlStatus[RT_TEMP_CTRL_NUM];        //!< Target temperature control status; for verification of action result.
    TempCtrlStatus_t m_CurrentTempCtrlStatus[RT_TEMP_CTRL_NUM];       //!< Current temperature control status
    TempCtrlMainsVoltage_t m_MainsVoltageStatus[RT_TEMP_CTRL_NUM];    //!< Mains voltage state of the heaters
    qint64 m_LastGetTempTime[RT_TEMP_CTRL_NUM][MAX_SENSOR_PER_TEMP_CTRL];                    //!< Last get temperature time
    QMap<quint32, RTTempCtrlType_t> m_InstTCTypeMap;                  //!< Map between instance ID and temperature control FMs

    qint64 m_LastGetLockStatusTime;                                   //!< Last get lock status time
    qint16 m_TargetDOOutputValue;                                     //!< Target output value; for verification of action result
    qint16 m_LockStatus;                                              //!< Target output value; for verification of action result


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
