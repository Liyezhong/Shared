#ifndef MOCKDEVICECONTROL_H
#define MOCKDEVICECONTROL_H

//#include <QTest>
#include <QHistoryState>
//#include <QSignalSpy>
#include <QDebug>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QDateTime>
#include "DeviceControl/Include/Interface/IDeviceControl.h"
#include <QString>

using::testing::Return;
using::testing::AtLeast;
using::testing::_;
using::testing::Lt;



namespace DeviceControl {

class MockDeviceControl : public QObject, public IDeviceControl {
    Q_OBJECT
 public:
    MockDeviceControl(){}
//    MockIDeviceControl(const MockIDeviceControl&){}
    virtual ~MockDeviceControl(){}

  MOCK_METHOD0(EmergencyStop,
      void());
  MOCK_METHOD0(Standby,
      void());
  MOCK_METHOD0(Destroy,
      void());
  MOCK_METHOD0(StartConfigurationService,
      ReturnCode_t());
  MOCK_METHOD0(RestartConfigurationService,
      ReturnCode_t());
  MOCK_METHOD0(StartDiagnosticService,
      ReturnCode_t());
  MOCK_METHOD0(CloseDiagnosticService,
      ReturnCode_t());
  MOCK_METHOD0(StartAdjustmentService,
      ReturnCode_t());
  MOCK_METHOD1(GetDevice,
      CBaseDevice*(quint32 InstanceID));
  MOCK_METHOD1(GetNode,
      CBaseModule*(bool First));
  MOCK_METHOD0(ALSetPressureCtrlON,
      ReturnCode_t());
  MOCK_METHOD0(ALSetPressureCtrlOFF,
      ReturnCode_t());
  MOCK_METHOD1(ALReleasePressure,
      ReturnCode_t(const QString&));
  MOCK_METHOD1(ALPressure,
      ReturnCode_t(float));
  MOCK_METHOD1(ALVaccum,
      ReturnCode_t(float));
  MOCK_METHOD3(ALDraining,
      ReturnCode_t(quint32, float, bool));
  MOCK_METHOD3(IDForceDraining,
      ReturnCode_t(quint32, float, const QString&));
  MOCK_METHOD3(ALFilling,
      ReturnCode_t(quint32 DelayTime, bool EnableInsufficientCheck, bool SafeReagent4Paraffin));
  MOCK_METHOD1(ALStopCmdExec,
      ReturnCode_t(quint8 CmdType));
  MOCK_METHOD2(ALFillingForService,
      ReturnCode_t(quint32 DelayTime, bool EnableInsufficientCheck));
  MOCK_METHOD0(ALGetRecentPressure,
      qreal());
  MOCK_METHOD1(ALSetTempCtrlON,
      ReturnCode_t(ALTempCtrlType_t Type));
  MOCK_METHOD1(ALSetTempCtrlOFF,
      ReturnCode_t(ALTempCtrlType_t type));
  MOCK_METHOD5(ALSetTemperaturePid,
      ReturnCode_t(ALTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime));
  MOCK_METHOD3(ALStartTemperatureControl,
      ReturnCode_t(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange));
  MOCK_METHOD2(ALGetRecentTemperature,
      qreal(ALTempCtrlType_t Type, quint8 Index));
  MOCK_METHOD1(ALGetTemperatureControlState,
      TempCtrlState_t(ALTempCtrlType_t Type));
  MOCK_METHOD0(ALTurnOnFan,
      ReturnCode_t());
  MOCK_METHOD0(ALTurnOffFan,
      ReturnCode_t());
  MOCK_METHOD0(ALAllStop,
      ReturnCode_t());
  MOCK_METHOD0(ALBreakAllOperation,
      ReturnCode_t());
  MOCK_METHOD1(ALSetPressureDrift,
      ReturnCode_t(qreal pressureDrift));
  MOCK_METHOD7(ALStartTemperatureControlWithPID,
      ReturnCode_t(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime));
  MOCK_METHOD2(ALControlValve,
      ReturnCode_t(quint8 ValveIndex, quint8 ValveState));
  MOCK_METHOD1(ALGetHeatingStatus,
      bool(ALTempCtrlType_t Type));
  MOCK_METHOD0(RVSetTempCtrlON,
      ReturnCode_t());
  MOCK_METHOD0(RVSetTempCtrlOFF,
      ReturnCode_t());
  MOCK_METHOD4(RVSetTemperaturePid,
      ReturnCode_t(quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime));
  MOCK_METHOD2(RVStartTemperatureControl,
      ReturnCode_t(qreal NominalTemperature, quint8 SlopeTempChange));
  MOCK_METHOD1(RVGetRecentTemperature,
      qreal(quint32 Index));
  MOCK_METHOD0(RVGetTemperatureControlState,
      TempCtrlState_t());
  MOCK_METHOD1(RVReqMoveToInitialPosition,
      ReturnCode_t(RVPosition_t));
  MOCK_METHOD1(RVReqMoveToRVPosition,
      ReturnCode_t(RVPosition_t RVPosition));
  MOCK_METHOD0(RVReqActRVPosition,
      RVPosition_t());
  MOCK_METHOD1(RVReqMoveToCurrentTubePosition,
      ReturnCode_t(RVPosition_t CurrentRVPosition));
  MOCK_METHOD0(GetCurrentLowerLimit,
      quint32());
  MOCK_METHOD6(RVStartTemperatureControlWithPID,
      ReturnCode_t(qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime));
  MOCK_METHOD2(RVSetTemperatureSwitchState,
      ReturnCode_t(qint8 HeaterVoltage, qint8 AutoType));
  MOCK_METHOD1(OvenSetTempCtrlON,
      ReturnCode_t(OVENTempCtrlType_t Type));
  MOCK_METHOD1(OvenSetTempCtrlOFF,
      ReturnCode_t(OVENTempCtrlType_t type));
  MOCK_METHOD5(OvenSetTemperaturePid,
      ReturnCode_t(OVENTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime));
  MOCK_METHOD3(OvenStartTemperatureControl,
      ReturnCode_t(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange));
  MOCK_METHOD2(OvenGetRecentTemperature,
      qreal(OVENTempCtrlType_t Type, quint8 Index));
  MOCK_METHOD1(OvenGetHeatingStatus,
      bool(OVENTempCtrlType_t Type));
  MOCK_METHOD0(OvenGetRecentLidStatus,
      quint16());
  MOCK_METHOD1(OvenGetTemperatureControlState,
      TempCtrlState_t(OVENTempCtrlType_t Type));
  MOCK_METHOD7(OvenStartTemperatureControlWithPID,
      ReturnCode_t(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime));
  MOCK_METHOD1(RTSetTempCtrlON,
      ReturnCode_t(RTTempCtrlType_t Type));
  MOCK_METHOD1(RTSetTempCtrlOFF,
      ReturnCode_t(RTTempCtrlType_t Type));
  MOCK_METHOD5(RTSetTemperaturePid,
      ReturnCode_t(RTTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime));
  MOCK_METHOD3(RTStartTemperatureControl,
      ReturnCode_t(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange));
  MOCK_METHOD7(RTStartTemperatureControlWithPID,
      ReturnCode_t(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime));
  MOCK_METHOD2(RTGetRecentTemperature,
      qreal(RTTempCtrlType_t Type, quint8 Index));
  MOCK_METHOD1(RTGetTemperatureControlState,
      TempCtrlState_t(RTTempCtrlType_t Type));
  MOCK_METHOD0(RTUnlock,
      ReturnCode_t());
  MOCK_METHOD0(RTLock,
      ReturnCode_t());
  MOCK_METHOD3(RTSetTemperatureSwitchState,
      ReturnCode_t(RTTempCtrlType_t Type, qint8 HeaterVoltage, qint8 AutoType));
  MOCK_METHOD0(RTGetRecentLockStatus,
      quint16());
  MOCK_METHOD0(PerTurnOffMainRelay,
      ReturnCode_t());
  MOCK_METHOD0(PerTurnOnMainRelay,
      ReturnCode_t());
  MOCK_METHOD2(PerControlAlarm,
      ReturnCode_t(bool On, bool Remote));
  MOCK_METHOD1(PerGetRecentAlarmStatus,
      quint16(qint8 type));
  MOCK_METHOD2(IDBottleCheck,
      ReturnCode_t(QString ReagentGrpID, RVPosition_t TubePos));
  MOCK_METHOD2(IDSealingCheck,
      ReturnCode_t(qreal ThresholdPressure, RVPosition_t SealPosition));
  MOCK_METHOD2(GetFunctionModuleRef,
      CFunctionModule*(quint32 InstanceID, const QString &Key));
  MOCK_METHOD1(IDGetSlaveCurrent,
      quint16(HimSlaveType_t Type));
  MOCK_METHOD1(IDGetSlaveVoltage,
      quint16(HimSlaveType_t Type));
  MOCK_METHOD1(GetBaseModule,
      CBaseModule*(HimSlaveType_t Type));
  MOCK_METHOD0(IDGetRemoteAlarmStatus,
      quint16());
  MOCK_METHOD0(IDGetLocalAlarmStatus,
      quint16());
  MOCK_METHOD1(IDSetAlarm,
      ReturnCode_t(qint8 opcode));
  MOCK_METHOD0(InitArchiveServiceInforState,
      void());
  MOCK_METHOD0(ArchiveServiceInfor,
      void());
  MOCK_METHOD1(NotifySavedServiceInfor,
      void(const QString& deviceType));
  MOCK_METHOD1(ResetActiveCarbonFilterLifeTime,
      void(quint32 setVal));
  MOCK_METHOD0(GetCurrentVoltageStatus,
      bool());
  MOCK_METHOD3(GetSlaveModuleReportError,
      ReportError_t(quint8 errorCode, const QString& devName, quint32 sensorName));
  MOCK_METHOD2(GetSensorCurrent,
      quint16(const QString& DevName, quint8 Index));
  MOCK_METHOD1(GetHeaterSwitchType,
      quint8(const QString& DevName));
  MOCK_METHOD1(DelaySomeTime,
      int(int DelayTime));
//  MOCK_METHOD2(ReportInitializationFinished,
//      void(quint32, ReturnCode_t));
//  MOCK_METHOD2(ReportConfigurationFinished,
//      void(quint32, ReturnCode_t));
//  MOCK_METHOD2(ReportStartNormalOperationMode,
//      void(quint32, ReturnCode_t));
//  MOCK_METHOD5(ReportError,
//      void(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData, QDateTime timeStamp));
//  MOCK_METHOD6(ReportErrorWithInfo,
//      void(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo));
//  MOCK_METHOD0(ReportDestroyFinished,
//      void());
//  MOCK_METHOD0(DeviceShutdown,
//      void());
//  MOCK_METHOD0(ReportLevelSensorStatus1,
//      void());
//  MOCK_METHOD0(ReportFillingTimeOut2Min,
//      void());
//  MOCK_METHOD0(ReportDrainingTimeOut2Min,
//      void());
//  MOCK_METHOD3(ReportGetServiceInfo,
//      void(ReturnCode_t ReturnCode, const DataManager::CModule &ModuleInfo, const QString&));
//  MOCK_METHOD1(ReportSavedServiceInfor,
//      void(const QString& deviceType));
  MOCK_METHOD5(OnError,
      void(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData, QDateTime timeStamp));
  MOCK_METHOD1(OnInitializationFinished,
      void(ReturnCode_t));
  MOCK_METHOD1(OnConfigurationFinished,
      void(ReturnCode_t));
  MOCK_METHOD1(OnStartNormalOperationMode,
      void(ReturnCode_t));
  MOCK_METHOD6(OnErrorWithInfo,
      void(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo));
  MOCK_METHOD1(OnDiagnosticServiceClosed,
      void(qint16 DiagnosticResult));
  MOCK_METHOD0(OnDestroyFinished,
      void());
  MOCK_METHOD0(OnTimeOutSaveServiceInfor,
      void());
  MOCK_METHOD0(OnTimeOutSaveLifeCycleRecord,
      void());
signals:
    //! Forward the 'intitialisation finished' notification
    void ReportInitializationFinished(quint32, ReturnCode_t);
    //! Forward the 'configuration finished' notification
    void ReportConfigurationFinished(quint32, ReturnCode_t);
    //! Forward the 'normal operation mode started' notification
    void ReportStartNormalOperationMode(quint32, ReturnCode_t);

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
    void ReportError(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID,
                     quint16 usErrorData, QDateTime timeStamp);
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
    void ReportErrorWithInfo(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID,
                             quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo);

    //! Forward the 'Destroy finished' to IDeviceProcessing
    void ReportDestroyFinished();
    /****************************************************************************/
    /*!
     *  \brief  Emitted when all devices should shut down
     */
    /****************************************************************************/
    void DeviceShutdown();

    /****************************************************************************/
    /*!
     *  \brief  Forward the 'level sensor change to 1' to scheduler module
     */
    /****************************************************************************/
    void ReportLevelSensorStatus1();

    /****************************************************************************/
    /*!
     *  \brief  Forward Filling TimeOut 2Min to scheduler module
     */
    /****************************************************************************/
    void ReportFillingTimeOut2Min();

    /****************************************************************************/
    /*!
     *  \brief  Forward Draining TimeOut 2Min to scheduler module
     */
    /****************************************************************************/
    void ReportDrainingTimeOut2Min();

    /*!
     *  \brief  function ReportGetServiceInfo
     * \param  ReturnCode return code
     * \param  ModuleInfo module info
     */
    void ReportGetServiceInfo(ReturnCode_t ReturnCode, const DataManager::CModule &ModuleInfo, const QString&);
    /****************************************************************************/
    /*!
     *  \brief  function ReportSavedServiceInfor
     *
     *  \iparam deviceType = device type
     */
    /****************************************************************************/
    void ReportSavedServiceInfor(const QString& deviceType);

};

}  // namespace DeviceControl

#endif // MOCKDEVICECONTROL_H
