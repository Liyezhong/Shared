#ifndef PERIPHERY_H
#define PERIPHERY_H
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"


namespace DeviceControl
{
class CDigitalOutput;
class CDigitalInput;

/****************************************************************************/
/*!
*   \brief This class implements the functionality to configure and control a
*          'Periphery' device
*/
/****************************************************************************/
class CPeripheryDevice : public CBaseDevice
{
    Q_OBJECT
public:

    //! constructor
    CPeripheryDevice(DeviceProcessing* pDeviceProcessing,
                     QString& Type,
                     const DeviceModuleList_t &ModuleList,
                     quint32 InstanceID);
    //! destructor
    virtual ~CPeripheryDevice();

    //! type for digital output fucntiom modules in this device
    typedef enum {
        PER_MAIN_RELAY = 0,
        PER_REMOTE_ALARM_CTRL  = 1,
        PER_LOCAL_ALARM_CTRL  = 2,
        //PER_REMOTE_ALARM_SET   = 2,
        //PER_REMOTE_ALARM_CLEAR = 3,
        PER_DO_TOTAL_NUM = 3
    } PerDOType_t;

    //! type for digital input fucntiom modules in this device
    typedef enum {
        PER_LOCAL_ALARM_STATUS   = 0,
        PER_REMOTE_ALARM_STATUS  = 1,
        PER_DI_TOTAL_NUM = 2
    } PerDIType_t;

    //! general task handling function
    void HandleTasks();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function TurnOffMainRelay
     *
     *  \return from TurnOffMainRelay
     */
    /****************************************************************************/
    ReturnCode_t TurnOffMainRelay();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function TurnOnMainRelay
     *
     *  \return from TurnOnMainRelay
     */
    /****************************************************************************/
    ReturnCode_t TurnOnMainRelay();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function TurnOffLocalAlarm
     *
     *  \return from TurnOffLocalAlarm
     */
    /****************************************************************************/
    ReturnCode_t TurnOffLocalAlarm();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function TurnOnLocalAlarm
     *
     *  \return from TurnOnLocalAlarm
     */
    /****************************************************************************/
    ReturnCode_t TurnOnLocalAlarm();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function TurnOffRemoteAlarm
     *
     *  \return from TurnOffRemoteAlarm
     */
    /****************************************************************************/
    ReturnCode_t TurnOffRemoteAlarm();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function TurnOnRemoteAlarm
     *
     *  \return from TurnOnRemoteAlarm
     */
    /****************************************************************************/
    ReturnCode_t TurnOnRemoteAlarm();


    /****************************************************************************/
    /*!
     *  \brief  Get Remote Alarm status asynchronously.
     *
     *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
     */
    /****************************************************************************/
    ReturnCode_t GetRemoteAlarmStatusAsync();

    /****************************************************************************/
    /*!
     *  \brief  Get Local Alarm status asynchronously.
     *
     *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
     */
    /****************************************************************************/
    ReturnCode_t GetLocalAlarmStatusAsync();

    /****************************************************************************/
    /*!
     *  \brief  Get Local Alarm Connection status.
     *
     *  \return  1: Conencted, 0: Not connected
     */
    /****************************************************************************/
    quint16 GetLocalAlarmStatus();

    /****************************************************************************/
    /*!
     *  \brief  Get Remote Alarm Connection status.
     *
     *  \return  1: Conencted, 0: Not connected
     */
    /****************************************************************************/
    quint16 GetRemoteAlarmStatus();

    /****************************************************************************/
    /*!
     *  \brief   Get the Local Alarm status captured in last 500 milliseconds.
     *
     *  \return  Actual connection status, UNDEFINED if failed.
     */
    /****************************************************************************/
    quint16 GetRecentLocalAlarmStatus();

    /****************************************************************************/
    /*!
     *  \brief   Get the Remote Alarm status captured in last 500 milliseconds.
     *
     *  \return  Actual connection status, UNDEFINED if failed.
     */
    /****************************************************************************/
    quint16 GetRecentRemoteAlarmStatus();

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
     *  \brief  Definition/Declaration of slot SetDOValue
     */
    /****************************************************************************/
    ReturnCode_t SetDOValue(PerDOType_t Type, quint16 OutputValue, quint16 Duration, quint16 Delay);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnSetDOOutputValue
     */
    /****************************************************************************/
    void OnSetDOOutputValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 OutputValue);

    /****************************************************************************/
    /*!
     *  \brief   slot associated with get digital input value.
     */
    /****************************************************************************/
    void OnGetDIValue(quint32 InstanceID, ReturnCode_t ReturnCode, quint16 InputValue);

    /****************************************************************************/
    /*!
     *  \brief   Read device's sensors data asynchronizely
     */
    /****************************************************************************/
    void CheckSensorsData();
    //! command handling task
    //  void HandleCommandRequestTask();
    //  void HandleDeviceTaskActions();

    //! Return the oven cover position type from motor position
    //  RVPosition_t GetRVPositionFromEncoderDiskPos(qint32 EDPosition);
    //! Return the motor position from oven cover position type
    // qint32 GetEDPosFromRVPos(RVPosition_t RVPosition);

private:
    //Function modules
    CDigitalOutput* m_pDigitalOutputs[PER_DO_TOTAL_NUM]; //!< Digital outputs FMs used in this device
    CDigitalInput* m_pDigitalInputs[PER_DI_TOTAL_NUM]; //!< Digital inputs FMs used in this device
    QMap<quint32, PerDOType_t> m_InstDOTypeMap;        //!< Map between instance ID and digital output function modules
    qint16 m_TargetDOOutputValues[PER_DO_TOTAL_NUM];   //!< Target output value; for verification of action result
    qint16 m_TargetDIInputValues[PER_DI_TOTAL_NUM];    //!< Target input value; for verification of action result
    QMap<quint32, PerDIType_t> m_InstDITypeMap;        //!< Map between instance ID and digital input function modules
    qint64 m_LastGetLocalAlarmStatusTime;              //!< Last get local alarm status time
    qint64 m_LastGetRemoteAlarmStatusTime;             //!< Last get remote alarm status time

    /*! error task state definitiosn */
    typedef enum {
        PER_DEV_ERRTASK_STATE_FREE           = 0x00,   ///< task state free, no error
        PER_DEV_ERRTASK_STATE_REPORT_IFACE   = 0x01,   ///< reports the error to the interface class
        PER_DEV_ERRTASK_STATE_REPORT_DEVPROC = 0x02,   ///< reports the error to device processing
        PER_DEV_ERRTASK_STATE_IDLE           = 0x03,   ///< error idle state, do nothing, block anything
        PER_DEV_ERRTASK_STATE_RESET          = 0x04    ///< reset error state, change to initial state
    } PERDevErrTaskState_t;

    PERDevErrTaskState_t m_ErrorTaskState;  //!< error task state
    QMutex m_Mutex; //!< Protects the task handling thread from request functions

};
}

#endif // PERIPHERY_H
