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





/****************************************************************************/
/*!
*   \brief This class implements the functionality to configure and control a
*          'Rotary valve' device
*/
/****************************************************************************/

class CRotaryValveDevice : public CBaseDevice
{
    Q_OBJECT

public:

    //! constructor
    CRotaryValveDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    //! destructor
    virtual ~CRotaryValveDevice();

    //! general task handling function
    void HandleTasks();


    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetTempCtrlON
     *
     *  \return from SetTempCtrlON
     */
    /****************************************************************************/
    ReturnCode_t SetTempCtrlON();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetTempCtrlOFF
     *
     *  \return from SetTempCtrlOFF
     */
    /****************************************************************************/
    ReturnCode_t SetTempCtrlOFF();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetTemperaturePid
     *
     *  \param MaxTemperature = quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from SetTemperaturePid
     */
    /****************************************************************************/
    ReturnCode_t SetTemperaturePid(quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StartTemperatureControl
     *
     *  \param NominalTemperature = qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *
     *  \return from StartTemperatureControl
     */
    /****************************************************************************/
    ReturnCode_t StartTemperatureControl(qreal NominalTemperature, quint8 SlopeTempChange);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StartTemperatureControlWithPID
     *
     *  \param NominalTemperature = qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from StartTemperatureControlWithPID
     */
    /****************************************************************************/
    ReturnCode_t StartTemperatureControlWithPID(qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetRecentTemperature
     *
     *  \param Index = quint32 type parameter
     *
     *  \return from GetRecentTemperature
     */
    /****************************************************************************/
    qreal GetRecentTemperature(quint32 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetTemperatureControlState
     *
     *  \return from GetTemperatureControlState
     */
    /****************************************************************************/
    TempCtrlState_t GetTemperatureControlState();
    //! Execute the move to intial position of the RV
    ReturnCode_t ReqMoveToInitialPosition();
    //! Position the oven cover
    ReturnCode_t ReqMoveToRVPosition( RVPosition_t RVPosition);
    //! Request actual oven cover position
    RVPosition_t ReqActRVPosition();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ReqAdjacentPosition
     *
     *  \param position = RVPosition_t type parameter
     *
     *  \return from ReqAdjacentPosition
     */
    /****************************************************************************/
    RVPosition_t ReqAdjacentPosition(RVPosition_t position);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsSealPosition
     *
     *  \param position = RVPosition_t type parameter
     *
     *  \return from IsSealPosition
     */
    /****************************************************************************/
    bool IsSealPosition(RVPosition_t position);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetHeaterCurrent
     *
     *  \return from GetHeaterCurrent
     */
    /****************************************************************************/
    quint16 GetHeaterCurrent();
    //! Request actual oven temperature
    //! Set oven nominal temperature
    //    ReturnCode_t SetTemperature(qint16 Temperatur);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ClearErrorState
     *          Request the status of the oven temperature control
     *
     *  \return from ClearErrorState
     */
    /****************************************************************************/
    ReturnCode_t ClearErrorState();

signals:
    /****************************************************************************/
    /*!
     *  \brief  Signal for report actual temperature
     *
     *  \param SensorIndex = Sensor index
     *  \param Temperature = Temperature
     *
     */
    /****************************************************************************/
    void ReportActTempeature(quint32 SensorIndex, qreal Temperature);

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot sendPing
     */
    /****************************************************************************/
    qreal GetTemperature(quint32 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot sendPing
     */
    /****************************************************************************/
    ReturnCode_t SetTemperatureControlStatus(TempCtrlStatus_t TempCtrlStatus);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot sendPing
     */
    /****************************************************************************/
    void OnTempControlStatus(quint32 InstanceID, ReturnCode_t ReturnCode, TempCtrlStatus_t TempCtrlStatus, TempCtrlMainsVoltage_t MainsVoltage);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot sendPing
     */
    /****************************************************************************/
    ReturnCode_t SetTemperature(qreal NominalTemperature, quint8 SlopeTempChange);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot sendPing
     */
    /****************************************************************************/
    void CheckSensorsData();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot sendPing
     */
    /****************************************************************************/
    ReturnCode_t GetTemperatureAsync(quint8 Index);
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
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot sendPing
     */
    /****************************************************************************/
    bool IsInsideRange();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot sendPing
     */
    /****************************************************************************/
    bool IsOutsideRange();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot sendPing
     */
    /****************************************************************************/
    bool IsTemperatureControlOn();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot sendPing
     */
    /****************************************************************************/
    bool IsTemperatureControlOff();
    //! Handle substate initialisation
    ReturnCode_t HandleInitializationState();
    //! Handle substate configuration
    ReturnCode_t HandleConfigurationState();
    //! Handle substate motor configuration
    //ReturnCode_t ConfigureDeviceTasks();
    ReturnCode_t DoReferenceRun();
    // bool SetState(bool flag);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnReferenceRun
     */
    /****************************************************************************/
    void OnReferenceRun(quint32 InstanceID, ReturnCode_t ReturnCode, qint32 Position);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnSetMotorState
     */
    /****************************************************************************/
    void OnSetMotorState(quint32 /*InstanceID*/, ReturnCode_t ReturnCode);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnGetPosition
     */
    /****************************************************************************/
    void OnGetPosition(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qint32 Position, qint8 PosCode);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnSetConfiguration
     */
    /****************************************************************************/
    void OnSetConfiguration(quint32 /*InstanceID*/, ReturnCode_t ReturnCode);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnGetTemp
     */
    /****************************************************************************/
    void OnGetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint8 Index, qreal Temp);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnSetTemp
     */
    /****************************************************************************/
    void OnSetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Temperature);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnSetTempPid
     */
    /****************************************************************************/
    void OnSetTempPid(quint32, ReturnCode_t ReturnCode, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnTCGetHardwareStatus
     */
    /****************************************************************************/
    void OnTCGetHardwareStatus(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Sensors, quint8 Fans,
                                               quint8 Heaters, quint8 Pids, quint16 Current, quint8 HeaterSwitchType);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot MoveToNextPort
     */
    /****************************************************************************/
    ReturnCode_t MoveToNextPort(bool changeParameter, quint32 LowerLimit, quint32 UpperLimit);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot MoveToNextPortCW
     */
    /****************************************************************************/
    ReturnCode_t MoveToNextPortCW();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot MoveToNextPortCCW
     */
    /****************************************************************************/
    ReturnCode_t MoveToNextPortCCW();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetRotationDirection
     */
    /****************************************************************************/
    quint8 GetRotationDirection();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetRotationDirection
     */
    /****************************************************************************/
    void SetRotationDirection(quint8 direction);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot TranslateFromEDPosition
     */
    /****************************************************************************/
    QString TranslateFromEDPosition(quint32 EDPosition);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetUpperLimit
     */
    /****************************************************************************/
    quint32 GetUpperLimit(quint32 CurrentEDPosition, DeviceControl::CANFctModuleStepperMotor::RotationDir_t Direction, bool ChangeDirection);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetLowerLimit
     */
    /****************************************************************************/
    quint32 GetLowerLimit(quint32 CurrentEDPosition, DeviceControl::CANFctModuleStepperMotor::RotationDir_t Direction, bool ChangeDirection);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetConfigLS2Exists
     */
    /****************************************************************************/
    void SetConfigLS2Exists(quint8 bExists);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetConfigLS2Exists
     */
    /****************************************************************************/
    quint8 GetConfigLS2Exists();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetConfigPos2Value
     */
    /****************************************************************************/
    void SetConfigPos2Value(quint8 bValid);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetConfigPos2Value
     */
    /****************************************************************************/
    quint8 GetConfigPos2Value();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetConfigPos3Value
     */
    /****************************************************************************/
    void SetConfigPos3Value(quint8 bValid);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetConfigPos3Value
     */
    /****************************************************************************/
    quint8 GetConfigPos3Value();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetConfigRefRunPosition
     */
    /****************************************************************************/
    qint8 GetConfigRefRunPosition();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetConfigRefRunPosition
     */
    /****************************************************************************/
    void SetConfigRefRunPosition(qint8 refRunRefPos);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetConfigPos1Stop
     */
    /****************************************************************************/
    void SetConfigPos1Stop(quint8 bStop);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetConfigPos2Stop
     */
    /****************************************************************************/
    void SetConfigPos2Stop(quint8 bStop);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetConfigPos2Stop
     */
    /****************************************************************************/
    quint8 GetConfigPos2Stop();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetConfigPos3Stop
     */
    /****************************************************************************/
    void SetConfigPos3Stop(quint8 bStop);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetConfigPos3Stop
     */
    /****************************************************************************/
    quint8 GetConfigPos3Stop();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetConfigPos1Stop
     */
    /****************************************************************************/
    quint8 GetConfigPos1Stop();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetLimitSwitchCode
     */
    /****************************************************************************/
    QString GetLimitSwitchCode();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetEDPosition
     */
    /****************************************************************************/
    RVPosition_t GetEDPosition();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetEDPosition
     */
    /****************************************************************************/
    void SetEDPosition(RVPosition_t position);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetPrevEDPosition
     */
    /****************************************************************************/
    RVPosition_t GetPrevEDPosition();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetPrevEDPosition
     */
    /****************************************************************************/
    void SetPrevEDPosition(RVPosition_t position);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot InitEDPosition
     */
    /****************************************************************************/
    void InitEDPosition();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot GetPosition
     */
    /****************************************************************************/
    QString GetPosition();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot RestoreLastValidParameterSet
     */
    /****************************************************************************/
    void RestoreLastValidParameterSet();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot Reset
     */
    /****************************************************************************/
    void Reset();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot DoReferenceRunWithStepCheck
     */
    /****************************************************************************/
    ReturnCode_t DoReferenceRunWithStepCheck(quint32 LowerLimit, quint32 UpperLimit);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetMotorState
     */
    /****************************************************************************/
    ReturnCode_t SetMotorState(bool flag);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot ApplyNewParameterSet
     */
    /****************************************************************************/
    ReturnCode_t ApplyNewParameterSet();

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
    CTemperatureControl* m_pTempCtrl;         //!< Temperature control
    CANFctModuleStepperMotor m_Config;        //!< Copy of configuration.
    CStepperMotor* m_pMotorRV;                //!< Oven cover motor

    /*! Device command type definitions */
    typedef enum {
        RVDEV_CMD_TYPE_UNDEF       = 0x00,    //!< undefined command type
        RVDEV_CMD_INITPOS          = 0x01,    //!< cover reference run
        RVDEV_CMD_REQ_RV_MOVE      = 0x02,    //!< move cover
        RVDEV_CMD_REQ_RV_ACTPOS    = 0x03,    //!< request actual cover position
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

    RVDevErrTaskState_t m_ErrorTaskState;           //!< error task state

    // Variables from process settings
    qint32 m_CurrentLimitSwitchCode;                //!< Current limit switchs' code
    RVPosition_t m_RVCurrentPosition;               //!< Current rotary valve postion
    RVPosition_t m_RVPrevPosition;                  //!< Privious rotary valve postion
    qint32 m_CurrentPosition;                       //!< Current position (stored by asynchronous call)
    qreal m_CurrentTemperature;                     //!< Current temperature
    qreal m_TargetTemperature;                      //!< Current temperature
    TempCtrlStatus_t m_TargetTempCtrlStatus;        //!< Target temperature control status; for verification of action result.
    TempCtrlStatus_t m_CurrentTempCtrlStatus;       //!< Current temperature control status
    qint64 m_LastGetTempTime[50];                   //!< Last get temperature time
    TempCtrlMainsVoltage_t m_MainsVoltageStatus;    //!< Mains voltage state of the heaters
    QMutex m_Mutex;                                 //!< Protects the task handling thread from request functions
    qint64 m_LastGetTCCurrentTime;       ///<  Definition/Declaration of variable m_LastGetTCCurrentTime
    TempCtrlHardwareStatus_t m_TCHardwareStatus;       ///<  Definition/Declaration of variable m_TCHardwareStatus
};


}  //namespace

#endif /* DEVICEROTARYVALVE_H */

