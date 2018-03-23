#ifndef IDEVICECONTROL_H
#define IDEVICECONTROL_H

#include <QObject>
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"


namespace DeviceControl
{
#define UNDEFINED_VALUE (999)  //!< undefine value. used for invlid sensor's data
/****************************************************************************/
/*!
 *  \brief  This is the interface class of the device control layer.
 *
 *      The class provides methods to operate with the device control layer's
 *      functionality.
 */
/****************************************************************************/
class IDeviceControl
{
public:
    virtual ~IDeviceControl(){}
    //! Returns the serial number from config file
    inline static bool GetSerialNumber(QString& SerialNo){
        return DeviceProcessing::GetSerialNumber(SerialNo);}
    inline virtual IDeviceControl* WithSender(const QString& sender){m_Sender = sender; return this;}
    //! Emergency stop
    virtual void EmergencyStop() = 0;   // should be called if the device's cover was opened by the user
    //! Switch to standby mode
    virtual void Standby() = 0;         // should be called to change to standby mode
    //! Clean up the environment
    virtual void Destroy() = 0;         // should be called to finish all the activities

    //! Start device control layer configuration
    virtual ReturnCode_t StartConfigurationService() = 0;
    //! Restart device control layer configuration
    virtual ReturnCode_t RestartConfigurationService() = 0;
    //! Start device control layer diagnstic service
    virtual ReturnCode_t StartDiagnosticService() = 0;
    //! Finisch device control layer diagnostic service
   virtual  ReturnCode_t CloseDiagnosticService() = 0;

    //! Start adjustment service
    virtual ReturnCode_t StartAdjustmentService() = 0;

    //! Returns Device derived class pointer specified by instanceID
    virtual CBaseDevice* GetDevice(quint32 InstanceID) = 0;
    //! Return the pointer to the CBaseModule which is next in list
    virtual CBaseModule* GetNode(bool First) = 0;
    //Air liquid device funcs
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetPressureCtrlON
     *
     *  \return from ALSetPressureCtrlON
     */
    /****************************************************************************/
    virtual ReturnCode_t ALSetPressureCtrlON() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetPressureCtrlOFF
     *
     *  \return from ALSetPressureCtrlOFF
     */
    /****************************************************************************/
    virtual ReturnCode_t ALSetPressureCtrlOFF() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALReleasePressure
     *
     *  \return from ALReleasePressure
     */
    /****************************************************************************/
    virtual ReturnCode_t ALReleasePressure() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALPressure
     *
     *  \return from ALPressure
     */
    /****************************************************************************/
    virtual ReturnCode_t ALPressure(float targetPressure = AL_TARGET_PRESSURE_POSITIVE) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALVaccum
     *  \param  targetPressure - AL_TARGET_PRESSURE_NEGATIVE
     *  \return from ALVaccum
     */
    /****************************************************************************/
    virtual ReturnCode_t ALVaccum(float targetPressure = AL_TARGET_PRESSURE_NEGATIVE) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALDraining
     *
     *  \param DelayTime = quint32 type parameter
     *
     *  \return from ALDraining
     */
    /****************************************************************************/
    virtual ReturnCode_t ALDraining(quint32 DelayTime, float targetPressure = AL_TARGET_PRESSURE_POSITIVE, bool IgnorePressure = false) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDForceDraining
     *
     *  \param RVPos = rv pos
     *  \param targetPressure =  target pressure
     *  \param ReagentGrpID = reagent group ID
     *  \return from ALDraining
     */
    /****************************************************************************/
    virtual ReturnCode_t IDForceDraining(quint32 RVPos, float targetPressure = AL_FORCEDRAIN_PRESSURE, const QString& ReagentGrpID = "RG2") = 0;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALFilling
     *
     *  \param DelayTime = quint32 type parameter
     *  \param EnableInsufficientCheck = flag to indicate if Insufficient check is needed
     *  \param SafeReagent4Paraffin = Flag to indicate if Filling is in safe reagent and for paraffin
     *
     *  \return from ALFilling
     */
    /****************************************************************************/
    virtual ReturnCode_t ALFilling(quint32 DelayTime, bool EnableInsufficientCheck, bool SafeReagent4Paraffin) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALStopCmdExec
     *
     *  \param CmdType = quint8 type parameter
     *
     *  \return from ALStopCmdExec
     */
    /****************************************************************************/
    virtual ReturnCode_t ALStopCmdExec(quint8 CmdType) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALFillingForService
     *
     *  \param DelayTime = quint32 type parameter
     *  \param EnableInsufficientCheck = flag to indicate if Insufficient check is needed
     *
     *  \return from ALFilling
     */
    /****************************************************************************/
    virtual ReturnCode_t ALFillingForService(quint32 DelayTime, bool EnableInsufficientCheck) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALGetRecentPressure
     *
     *  \return from ALGetRecentPressure
     */
    /****************************************************************************/
    virtual qreal ALGetRecentPressure() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetTempCtrlON
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *
     *  \return from ALSetTempCtrlON
     */
    /****************************************************************************/
    virtual ReturnCode_t ALSetTempCtrlON(ALTempCtrlType_t Type) = 0;
    virtual ReturnCode_t ALSetTempCtrlOFF(ALTempCtrlType_t type) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetTemperaturePid
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from ALSetTemperaturePid
     */
    /****************************************************************************/
    virtual ReturnCode_t ALSetTemperaturePid(ALTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALStartTemperatureControl
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *
     *  \return from ALStartTemperatureControl
     */
    /****************************************************************************/
    virtual ReturnCode_t ALStartTemperatureControl(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALGetRecentTemperature
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *  \param Index =  quint8 type parameter
     *
     *  \return from ALGetRecentTemperature
     */
    /****************************************************************************/
    virtual qreal ALGetRecentTemperature(ALTempCtrlType_t Type, quint8 Index) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALGetTemperatureControlState
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *
     *  \return from ALGetTemperatureControlState
     */
    /****************************************************************************/
    virtual TempCtrlState_t ALGetTemperatureControlState(ALTempCtrlType_t Type) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALTurnOnFan
     *
     *  \return from ALTurnOnFan
     */
    /****************************************************************************/
    virtual ReturnCode_t ALTurnOnFan() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALTurnOffFan
     *
     *  \return from ALTurnOffFan
     */
    /****************************************************************************/
    virtual ReturnCode_t ALTurnOffFan() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALAllStop
     *
     *  \return from ALAllStop
     */
    /****************************************************************************/
    virtual ReturnCode_t ALAllStop() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALBreakAllOperation
     *
     *  \return from ALBreakAllOperation
     */
    /****************************************************************************/
    virtual ReturnCode_t ALBreakAllOperation() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetPressureDrift
     *
     *  \param pressureDrift = qreal type parameter
     *
     *  \return from ALSetPressureDrift
     */
    /****************************************************************************/
    virtual ReturnCode_t ALSetPressureDrift(qreal pressureDrift) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALStartTemperatureControlWithPID
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from ALStartTemperatureControlWithPID
     */
    /****************************************************************************/
    virtual ReturnCode_t ALStartTemperatureControlWithPID(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALControlValve
     *
     *  \param ValveIndex = quint type parameter
     *  \param ValveState = quint type parameter
     *
     *  \return from ALControlValve
     */
    /****************************************************************************/
    virtual ReturnCode_t ALControlValve(quint8 ValveIndex, quint8 ValveState) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Get LA SensorHeating status
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *
     *  \return true - on, false -off
     */
    /****************************************************************************/
    virtual bool ALGetHeatingStatus(ALTempCtrlType_t Type) = 0;

    //Rotary Valve device func
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVSetTempCtrlON
     *
     *  \return from RVSetTempCtrlON
     */
    /****************************************************************************/
    virtual ReturnCode_t RVSetTempCtrlON() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVSetTempCtrlOFF
     *
     *  \return from RVSetTempCtrlOFF
     */
    /****************************************************************************/
    virtual ReturnCode_t RVSetTempCtrlOFF() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVSetTemperaturePid
     *
     *  \param MaxTemperature = quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from RVSetTemperaturePid
     */
    /****************************************************************************/
    virtual ReturnCode_t RVSetTemperaturePid(quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVStartTemperatureControl
     *
     *  \param NominalTemperature = qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *
     *  \return from RVStartTemperatureControl
     */
    /****************************************************************************/
    virtual ReturnCode_t RVStartTemperatureControl(qreal NominalTemperature, quint8 SlopeTempChange) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVGetRecentTemperature
     *
     *  \param Index = quint32 type parameter
     *
     *  \return from RVGetRecentTemperature
     */
    /****************************************************************************/
    virtual qreal RVGetRecentTemperature(quint32 Index) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVGetTemperatureControlState
     *
     *  \return from RVGetTemperatureControlState
     */
    /****************************************************************************/
    virtual TempCtrlState_t RVGetTemperatureControlState() = 0;
    //! Execute the move to intial position of the RV
    virtual ReturnCode_t RVReqMoveToInitialPosition(RVPosition_t RVPosition = RV_UNDEF) = 0;
    //! Position the oven cover
    virtual ReturnCode_t RVReqMoveToRVPosition( RVPosition_t RVPosition) = 0;
    //! Request actual oven cover position
    virtual RVPosition_t RVReqActRVPosition() = 0;

    /****************************************************************************/
    /*!
     *  \brief   Request the rotary valve to move to current tube position.
     *  \iparam  CurrentRVPosition = Current rotary valve encoder disk's position.
     *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
     */
    /****************************************************************************/
    virtual ReturnCode_t RVReqMoveToCurrentTubePosition(RVPosition_t CurrentRVPosition) = 0;

    /****************************************************************************/
    /*!
     *  \brief  get the lower limit
     *  \return quint32
     */
    /****************************************************************************/
    virtual quint32 GetCurrentLowerLimit() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVStartTemperatureControlWithPID
     *
     *  \param NominalTemperature = qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from RVStartTemperatureControlWithPID
     */
    /****************************************************************************/
    virtual ReturnCode_t RVStartTemperatureControlWithPID(qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVSetTemperatureSwitchState
     *  \param  HeaterVoltage heater voltage
     *  \param  AutoType  auto type
     *  \return from RVSetTemperatureSwitchState
     */
    /****************************************************************************/
    virtual ReturnCode_t RVSetTemperatureSwitchState(qint8 HeaterVoltage, qint8 AutoType) = 0;

    //Oven device func
    /****************************************************************************/
    /*!
     *  \brief  Enable Oven temperature control
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *
     *  \return DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
     */
    /****************************************************************************/
    virtual ReturnCode_t OvenSetTempCtrlON(OVENTempCtrlType_t Type) = 0;
    /****************************************************************************/
    /*!
     *  \brief Disable Oven temperature control
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *
     *  \return DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
     */
    /****************************************************************************/
    virtual ReturnCode_t OvenSetTempCtrlOFF(OVENTempCtrlType_t type) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenSetTemperaturePid
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from OvenSetTemperaturePid
     */
    /****************************************************************************/
    virtual ReturnCode_t OvenSetTemperaturePid(OVENTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenStartTemperatureControl
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *
     *  \return from OvenStartTemperatureControl
     */
    /****************************************************************************/
    virtual ReturnCode_t OvenStartTemperatureControl(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenGetRecentTemperature
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *  \param Index =  quint8 type parameter
     *
     *  \return from OvenGetRecentTemperature
     */
    /****************************************************************************/
    virtual qreal OvenGetRecentTemperature(OVENTempCtrlType_t Type, quint8 Index) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Get Heating status
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *
     *  \return true - on, false -off
     */
    /****************************************************************************/
    virtual bool OvenGetHeatingStatus(OVENTempCtrlType_t Type) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenGetRecentLidStatus
     *
     *  \return from OvenGetRecentLidStatus
     */
    /****************************************************************************/
    virtual quint16 OvenGetRecentLidStatus() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenGetTemperatureControlState
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *
     *  \return from OvenGetTemperatureControlState
     */
    /****************************************************************************/
    virtual TempCtrlState_t OvenGetTemperatureControlState(OVENTempCtrlType_t Type) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenStartTemperatureControlWithPID
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from OvenStartTemperatureControlWithPID
     */
    /****************************************************************************/
    virtual ReturnCode_t OvenStartTemperatureControlWithPID(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) = 0;
    //Retort device func
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTSetTempCtrlON
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *
     *  \return from RTSetTempCtrlON
     */
    /****************************************************************************/
    virtual ReturnCode_t RTSetTempCtrlON(RTTempCtrlType_t Type) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTSetTempCtrlOFF
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *
     *  \return from RTSetTempCtrlOFF
     */
    /****************************************************************************/
    virtual ReturnCode_t RTSetTempCtrlOFF(RTTempCtrlType_t Type) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTSetTemperaturePid
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from RTSetTemperaturePid
     */
    /****************************************************************************/
    virtual ReturnCode_t RTSetTemperaturePid(RTTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTStartTemperatureControl
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *
     *  \return from RTStartTemperatureControl
     */
    /****************************************************************************/
    virtual ReturnCode_t RTStartTemperatureControl(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTStartTemperatureControlWithPID
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from RTStartTemperatureControlWithPID
     */
    /****************************************************************************/
    virtual ReturnCode_t RTStartTemperatureControlWithPID(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTGetRecentTemperature
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *  \param Index =  quint8 type parameter
     *
     *  \return from RTGetRecentTemperature
     */
    /****************************************************************************/
    virtual qreal RTGetRecentTemperature(RTTempCtrlType_t Type, quint8 Index) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTGetTemperatureControlState
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *
     *  \return from RTGetTemperatureControlState
     */
    /****************************************************************************/
    virtual TempCtrlState_t RTGetTemperatureControlState(RTTempCtrlType_t Type) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTUnlock
     *
     *  \return from RTUnlock
     */
    /****************************************************************************/
    virtual ReturnCode_t RTUnlock() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTLock
     *
     *  \return from RTLock
     */
    /****************************************************************************/
    virtual ReturnCode_t RTLock() = 0;
    /****************************************************************************/
    /*!
    *  \brief  Definition/Declaration of function RTSetTemperatureSwitchState
    *  \param  Type  type
    *  \param  HeaterVoltage  voltage
    *  \param  AutoType  auto type
    *  \return from RTSetTemperatureSwitchState
     */
    /****************************************************************************/
    virtual ReturnCode_t RTSetTemperatureSwitchState(RTTempCtrlType_t Type, qint8 HeaterVoltage, qint8 AutoType) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTGetRecentLockStatus
     *
     *  \return from RTGetRecentLockStatus
     */
    /****************************************************************************/
    virtual quint16 RTGetRecentLockStatus() = 0;
    //Periphery device func
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function PerTurnOffMainRelay
     *
     *  \return from PerTurnOffMainRelay
     */
    /****************************************************************************/
    virtual ReturnCode_t PerTurnOffMainRelay() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function PerTurnOnMainRelay
     *
     *  \return from PerTurnOnMainRelay
     */
    /****************************************************************************/
    virtual ReturnCode_t PerTurnOnMainRelay() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function PerControlAlarm
     *  \param On = true: On, false: Off
     *  \param Remote =  true: Remote, false:Local
     *
     *  \return from PerTurnOnMainRelay
     */
    /****************************************************************************/
    virtual ReturnCode_t PerControlAlarm(bool On, bool Remote) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Get local or remote Alarm status in last 500 milliseconds.
     *
     *  \param  type - qint8 0 - local alarm, 1 - remote alarm
     *  \return 1 - connect, 0 - not connect, UNDEFINED_VALUE if failed
     */
    /****************************************************************************/
    virtual quint16 PerGetRecentAlarmStatus(qint8 type) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDBottleCheck
     *
     *  \param ReagentGrpID = QString type parameter
     *  \param TubePos =  RVPosition_t type parameter
     *
     *  \return from IDBottleCheck
     */
    /****************************************************************************/
    virtual ReturnCode_t IDBottleCheck(QString ReagentGrpID, RVPosition_t TubePos) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDSealingCheck
     *
     *  \param  ThresholdPressure = qreal type parameter
     *
     *  \param  SealPosition = seal position
     *  \return from IDSealingCheck
     */
    /****************************************************************************/
    virtual ReturnCode_t IDSealingCheck(qreal ThresholdPressure, RVPosition_t SealPosition) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetFunctionModuleRef
     *
     *  \param InstanceID = quint32 type parameter
     *  \param Key =  QString type parameter
     *
     *  \return from GetFunctionModuleRef
     */
    /****************************************************************************/
    virtual CFunctionModule* GetFunctionModuleRef(quint32 InstanceID, const QString &Key) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDGetSlaveCurrent
     *
     *  \param Type = Slave module type
     *
     *  \return Slave current
     */
    /****************************************************************************/
    virtual quint16 IDGetSlaveCurrent(HimSlaveType_t Type) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDGetSlaveVoltage
     *
     *  \param Type = Slave module type
     *
     *  \return Slave voltage
     */
    /****************************************************************************/
    virtual quint16 IDGetSlaveVoltage(HimSlaveType_t Type) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetBaseModule
     *
     *  \param Type = Slave module type
     *
     *  \return BaseModule
     */
    /****************************************************************************/
    virtual CBaseModule* GetBaseModule(HimSlaveType_t Type) = 0;

    /****************************************************************************/
    /*!
     *  \brief  get remote alarm connect status
     *
     *  \return 1: connected, 0: not connected
     */
    virtual quint16 IDGetRemoteAlarmStatus() = 0;

    /****************************************************************************/
    /*!
     *  \brief  get local alarm connect status
     *
     *  \return 1: connected, 0: not connected
     */
    virtual quint16 IDGetLocalAlarmStatus() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDSetAlarm
     *
     *  \param opcode = operate code to turn on/off local/remote alarm
     *
     *  \return operate result
     */
    virtual ReturnCode_t IDSetAlarm(qint8 opcode) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Initialize the state machine for archieve service information
     *
     */
    /****************************************************************************/
    virtual void InitArchiveServiceInforState() = 0;
    /****************************************************************************/
    /*!
     *  \brief  start to archieve service information
     *
     */
    /****************************************************************************/
    virtual void ArchiveServiceInfor() = 0;
    /****************************************************************************/
    /*!
     *  \brief  report last service information is saved
     *
     */
    /****************************************************************************/
    /*!
     *  \brief  function NotifySavedServiceInfor
     *  \param  deviceType device type
     *
     */
    virtual void NotifySavedServiceInfor(const QString& deviceType) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Reset the life time of ActiveCarbonFilter
     *  \param  setVal -- lifetime for carbon filter
     *
     */
    /****************************************************************************/
    virtual void ResetActiveCarbonFilterLifeTime(quint32 setVal) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Get Current and Voltage status of all the slave devices
     *
     *  \return bool
     */
    /****************************************************************************/
    virtual bool GetCurrentVoltageStatus() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Get report error for the specific slave module
     *
     *  \param  errorCode - Error Code of temperature module
     *  \param  devName - Device name
     *  \param  sensorName sensor name
     *
     *  \return ReportError_t
     */
    /****************************************************************************/
    virtual ReportError_t GetSlaveModuleReportError(quint8 errorCode, const QString& devName, quint32 sensorName) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Get current of the specific sensor
     *
     *  \param  DevName - Device name
     *  \param  Index index
     *
     *  \return sensor's current
     */
    /****************************************************************************/
    virtual quint16 GetSensorCurrent(const QString& DevName, quint8 Index) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Get heater switch type of the specific sensor
     *  \param  DevName - Device name
     *  \return sensor's switch type
     */
    /****************************************************************************/
    virtual quint8 GetHeaterSwitchType(const QString& DevName) = 0;

    /****************************************************************************/
    /*!
     *  \brief  delay some time
     *  \param  DelayTime - time value
     *  \return return the exec value
     */
    /****************************************************************************/
    virtual int DelaySomeTime(int DelayTime) = 0;

signals:
    //! Forward the 'intitialisation finished' notification
    virtual void ReportInitializationFinished(quint32, ReturnCode_t) = 0;
    //! Forward the 'configuration finished' notification
    virtual void ReportConfigurationFinished(quint32, ReturnCode_t) = 0;
    //! Forward the 'normal operation mode started' notification
    virtual void ReportStartNormalOperationMode(quint32, ReturnCode_t) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Forward error information via a signal
     *
     *  \iparam instanceID = the error reporting object
     *  \iparam usErrorGroup = Error group ID
     *  \iparam usErrorID    = Error group ID
     *  \iparam usErrorData  = Optional error data
     *  \iparam timeStamp  = Error time stamp
     */
    /****************************************************************************/
    virtual void ReportError(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID,
                     quint16 usErrorData, QDateTime timeStamp) = 0;
    /****************************************************************************/
    /*!
     *  \brief  Forward error information via a signal
     *
     *  \iparam instanceID = the error reporting object
     *  \iparam usErrorGroup = Error group ID
     *  \iparam usErrorID    = Error group ID
     *  \iparam usErrorData  = Optional error data
     *  \iparam timeStamp  = Error time stamp
     *  \iparam strErrorInfo  = Error information
     */
    /****************************************************************************/
    virtual void ReportErrorWithInfo(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID,
                             quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo) = 0;

    //! Forward the 'Destroy finished' to IDeviceProcessing
    virtual void ReportDestroyFinished() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Emitted when all devices should shut down
     */
    /****************************************************************************/
    virtual void DeviceShutdown() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Forward the 'level sensor change to 1' to scheduler module
     */
    /****************************************************************************/
    virtual void ReportLevelSensorStatus1() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Forward Filling TimeOut 2Min to scheduler module
     */
    /****************************************************************************/
    virtual void ReportFillingTimeOut2Min() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Forward Draining TimeOut 2Min to scheduler module
     */
    /****************************************************************************/
    virtual void ReportDrainingTimeOut2Min() = 0;

    /*!
     *  \brief  function ReportGetServiceInfo
     * \param  ReturnCode return code
     * \param  ModuleInfo module info
     */
    virtual void ReportGetServiceInfo(ReturnCode_t ReturnCode, const DataManager::CModule &ModuleInfo, const QString&) = 0;
    /****************************************************************************/
    /*!
     *  \brief  function ReportSavedServiceInfor
     *
     *  \iparam deviceType = device type
     */
    /****************************************************************************/
    virtual void ReportSavedServiceInfor(const QString& deviceType) = 0;

//public slots:
    //! Get error information from DeviceProcessing
    virtual void OnError(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID,
                 quint16 usErrorData, QDateTime timeStamp) = 0;

//private slots:
    //! Task handling


    //! Get the 'intitialisation finished' notification
    virtual void OnInitializationFinished(ReturnCode_t) = 0;
    //! Get the 'configuration finished' notification
    virtual void OnConfigurationFinished(ReturnCode_t) = 0;
    //! Forward the 'normal operation mode started' notification
    virtual void OnStartNormalOperationMode(ReturnCode_t) = 0;
    //! Get error information from DeviceProcessing
    virtual void OnErrorWithInfo(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID,
                         quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo) = 0;

    //! Device control layer diagnostic servi    QString m_Sender;ce acknwoledge
    virtual void OnDiagnosticServiceClosed(qint16 DiagnosticResult) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnDestroyFinished
     */
    /****************************************************************************/
    virtual void OnDestroyFinished() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnTimeOutSaveServiceInfor
     */
    /****************************************************************************/
    virtual void OnTimeOutSaveServiceInfor() = 0;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnTimeOutSaveLifeCycleRecord
     */
    /****************************************************************************/
    virtual void OnTimeOutSaveLifeCycleRecord() = 0;

protected:
    QString m_Sender;

};

} //namespace


#endif // IDEVICECONTROL_H
