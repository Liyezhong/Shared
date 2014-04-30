#ifndef PERIPHERY_H
#define PERIPHERY_H
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"


namespace DeviceControl
{
class CDigitalOutput;

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
    CPeripheryDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    //! destructor
    virtual ~CPeripheryDevice();

    //! type for digital output fucntiom modules in this device
    typedef enum {
        PER_MAIN_RELAY = 0,
        PER_REMOTE_ALARM_CTRL  = 1,
        PER_LOCAL_ALARM_CTRL  = 2,
        //PER_REMOTE_ALARM_SET   = 2,
        //PER_REMOTE_ALARM_CLEAR = 3,
        PER_TOTAL_NUM = 3
    } PerDOType_t;

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

    //! command handling task
    //  void HandleCommandRequestTask();
    //  void HandleDeviceTaskActions();

    //! Return the oven cover position type from motor position
    //  RVPosition_t GetRVPositionFromEncoderDiskPos(qint32 EDPosition);
    //! Return the motor position from oven cover position type
    // qint32 GetEDPosFromRVPos(RVPosition_t RVPosition);

private:
    //Function modules
    CDigitalOutput* m_pDigitalOutputs[PER_TOTAL_NUM]; //!< Digital outputs FMs used in this device
    QMap<quint32, PerDOType_t> m_InstDOTypeMap;       //!< Map between instance ID and digital output function modules
    qint16 m_TargetDOOutputValues[PER_TOTAL_NUM];     //!< Target output value; for verification of action result

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
