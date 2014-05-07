/****************************************************************************/
/*! \file IDeviceProcessing.h
 *
 *  \brief Definition file for class IDeviceProcessing
 *
 *   Version: $ 0.1
 *   Date:    $ 08.08.2012
 *   Author:  $ Norbert Wiedmann / Stalin
 *
 *
 *  \b Description:
 *
 *       This module contains the interface to the IDeviceProcessing class
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef IDEVICEPROCESSING_H
#define IDEVICEPROCESSING_H

#include <QHash>
#include <QObject>

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/SlaveModules/AnalogOutput.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/Rfid15693.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/Devices/RotaryValveDevice.h"
#include "DeviceControl/Include/Devices/AirLiquidDevice.h"
#include "DeviceControl/Include/Devices/RetortDevice.h"
#include "DeviceControl/Include/Devices/OvenDevice.h"
#include "DeviceControl/Include/Devices/PeripheryDevice.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

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
class IDeviceProcessing : public QObject
{
    Q_OBJECT

public:
    IDeviceProcessing();
    ~IDeviceProcessing();

    //! Returns the serial number from config file
    static bool GetSerialNumber(QString& SerialNo);

    //! Emergency stop
    void EmergencyStop();   // should be called if the device's cover was opened by the user
    //! Shutdown the device
    void Shutdown();        // should be called if the device's power failed shortly
    //! Switch to standby mode
    void Standby();         // should be called to change to standby mode
    //! Clean up the environment
    void Destroy();         // should be called to finish all the activities

    //! Start device control layer configuration
    ReturnCode_t StartConfigurationService();
    //! Restart device control layer configuration
    ReturnCode_t RestartConfigurationService();
    //! Start device control layer diagnstic service
    ReturnCode_t StartDiagnosticService();
    //! Finisch device control layer diagnostic service
    ReturnCode_t CloseDiagnosticService();

    //! Start adjustment service
    ReturnCode_t StartAdjustmentService();

    //! Returns Device derived class pointer specified by instanceID
    CBaseDevice* GetDevice(quint32 InstanceID);
    //! Return the pointer to the CBaseModule which is next in list
    CBaseModule* GetNode(bool First);
    //Air liquid device funcs
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetPressureCtrlON
     *
     *  \return from ALSetPressureCtrlON
     */
    /****************************************************************************/
    ReturnCode_t ALSetPressureCtrlON();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetPressureCtrlOFF
     *
     *  \return from ALSetPressureCtrlOFF
     */
    /****************************************************************************/
    ReturnCode_t ALSetPressureCtrlOFF();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALReleasePressure
     *
     *  \return from ALReleasePressure
     */
    /****************************************************************************/
    ReturnCode_t ALReleasePressure(void);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALPressure
     *
     *  \return from ALPressure
     */
    /****************************************************************************/
    ReturnCode_t ALPressure();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALVaccum
     *
     *  \return from ALVaccum
     */
    /****************************************************************************/
    ReturnCode_t ALVaccum();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALDraining
     *
     *  \param DelayTime = quint32 type parameter
     *
     *  \return from ALDraining
     */
    /****************************************************************************/
    ReturnCode_t ALDraining(quint32 DelayTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALFilling
     *
     *  \param DelayTime = quint32 type parameter
     *  \param EnableInsufficientCheck = flag to indicate if Insufficient check is needed
     *
     *  \return from ALFilling
     */
    /****************************************************************************/
    ReturnCode_t ALFilling(quint32 DelayTime, bool EnableInsufficientCheck);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALGetRecentPressure
     *
     *  \return from ALGetRecentPressure
     */
    /****************************************************************************/
    qreal ALGetRecentPressure();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetTempCtrlON
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *
     *  \return from ALSetTempCtrlON
     */
    /****************************************************************************/
    ReturnCode_t ALSetTempCtrlON(ALTempCtrlType_t Type);
    ReturnCode_t ALSetTempCtrlOFF(ALTempCtrlType_t type);
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
    ReturnCode_t ALSetTemperaturePid(ALTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
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
    ReturnCode_t ALStartTemperatureControl(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
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
    qreal ALGetRecentTemperature(ALTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALGetTemperatureControlState
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *
     *  \return from ALGetTemperatureControlState
     */
    /****************************************************************************/
    TempCtrlState_t ALGetTemperatureControlState(ALTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALTurnOnFan
     *
     *  \return from ALTurnOnFan
     */
    /****************************************************************************/
    ReturnCode_t ALTurnOnFan();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALTurnOffFan
     *
     *  \return from ALTurnOffFan
     */
    /****************************************************************************/
    ReturnCode_t ALTurnOffFan();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALAllStop
     *
     *  \return from ALAllStop
     */
    /****************************************************************************/
    ReturnCode_t ALAllStop();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALBreakAllOperation
     *
     *  \return from ALBreakAllOperation
     */
    /****************************************************************************/
    ReturnCode_t ALBreakAllOperation();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetPressureDrift
     *
     *  \param pressureDrift = qreal type parameter
     *
     *  \return from ALSetPressureDrift
     */
    /****************************************************************************/
    ReturnCode_t ALSetPressureDrift(qreal pressureDrift);
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
    ReturnCode_t ALStartTemperatureControlWithPID(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    //Rotary Valve device func
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVSetTempCtrlON
     *
     *  \return from RVSetTempCtrlON
     */
    /****************************************************************************/
    ReturnCode_t RVSetTempCtrlON();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVSetTempCtrlOFF
     *
     *  \return from RVSetTempCtrlOFF
     */
    /****************************************************************************/
    ReturnCode_t RVSetTempCtrlOFF();
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
    ReturnCode_t RVSetTemperaturePid(quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
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
    ReturnCode_t RVStartTemperatureControl(qreal NominalTemperature, quint8 SlopeTempChange);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVGetRecentTemperature
     *
     *  \param Index = quint32 type parameter
     *
     *  \return from RVGetRecentTemperature
     */
    /****************************************************************************/
    qreal RVGetRecentTemperature(quint32 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVGetTemperatureControlState
     *
     *  \return from RVGetTemperatureControlState
     */
    /****************************************************************************/
    TempCtrlState_t RVGetTemperatureControlState();
    //! Execute the move to intial position of the RV
    ReturnCode_t RVReqMoveToInitialPosition();
    //! Position the oven cover
    ReturnCode_t RVReqMoveToRVPosition( RVPosition_t RVPosition);
    //! Request actual oven cover position
    RVPosition_t RVReqActRVPosition();
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
    ReturnCode_t RVStartTemperatureControlWithPID(qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    //Oven device func
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenSetTempCtrlON
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *
     *  \return from OvenSetTempCtrlON
     */
    /****************************************************************************/
    ReturnCode_t OvenSetTempCtrlON(OVENTempCtrlType_t Type);
    ReturnCode_t OvenSetTempCtrlOFF(OVENTempCtrlType_t type);
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
    ReturnCode_t OvenSetTemperaturePid(OVENTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
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
    ReturnCode_t OvenStartTemperatureControl(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
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
    qreal OvenGetRecentTemperature(OVENTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenGetRecentLidStatus
     *
     *  \return from OvenGetRecentLidStatus
     */
    /****************************************************************************/
    quint16 OvenGetRecentLidStatus();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenGetTemperatureControlState
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *
     *  \return from OvenGetTemperatureControlState
     */
    /****************************************************************************/
    TempCtrlState_t OvenGetTemperatureControlState(OVENTempCtrlType_t Type);
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
    ReturnCode_t OvenStartTemperatureControlWithPID(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
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
    ReturnCode_t RTSetTempCtrlON(RTTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTSetTempCtrlOFF
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *
     *  \return from RTSetTempCtrlOFF
     */
    /****************************************************************************/
    ReturnCode_t RTSetTempCtrlOFF(RTTempCtrlType_t Type);
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
    ReturnCode_t RTSetTemperaturePid(RTTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
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
    ReturnCode_t RTStartTemperatureControl(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
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
    ReturnCode_t RTStartTemperatureControlWithPID(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
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
    qreal RTGetRecentTemperature(RTTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTGetTemperatureControlState
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *
     *  \return from RTGetTemperatureControlState
     */
    /****************************************************************************/
    TempCtrlState_t RTGetTemperatureControlState(RTTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTUnlock
     *
     *  \return from RTUnlock
     */
    /****************************************************************************/
    ReturnCode_t RTUnlock();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTLock
     *
     *  \return from RTLock
     */
    /****************************************************************************/
    ReturnCode_t RTLock();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTGetRecentLockStatus
     *
     *  \return from RTGetRecentLockStatus
     */
    /****************************************************************************/
    quint16 RTGetRecentLockStatus();
    //Periphery device func
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function PerTurnOffMainRelay
     *
     *  \return from PerTurnOffMainRelay
     */
    /****************************************************************************/
    ReturnCode_t PerTurnOffMainRelay();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function PerTurnOnMainRelay
     *
     *  \return from PerTurnOnMainRelay
     */
    /****************************************************************************/
    ReturnCode_t PerTurnOnMainRelay();

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
    ReturnCode_t IDBottleCheck(QString ReagentGrpID, RVPosition_t TubePos);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDSealingCheck
     *
     *  \param ThresholdPressure = qreal type parameter
     *
     *  \return from IDSealingCheck
     */
    /****************************************************************************/
    ReturnCode_t IDSealingCheck(qreal ThresholdPressure);
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
    CFunctionModule* GetFunctionModuleRef(quint32 InstanceID, const QString &Key);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDGetSlaveCurrent
     *
     *  \param Type = Slave module type
     *
     *  \return Slave current
     */
    /****************************************************************************/
    quint16 IDGetSlaveCurrent(HimSlaveType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDGetSlaveVoltage
     *
     *  \param Type = Slave module type
     *
     *  \return Slave voltage
     */
    /****************************************************************************/
    quint16 IDGetSlaveVoltage(HimSlaveType_t Type);
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
public slots:
    //! Get error information from DeviceProcessing
    void OnError(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID,
                 quint16 usErrorData, QDateTime timeStamp);
private slots:
    //! Task handling
    void HandleTasks();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot ThreadStarted
     */
    /****************************************************************************/
    void ThreadStarted();

    //! Get the 'intitialisation finished' notification
    void OnInitializationFinished(ReturnCode_t);
    //! Get the 'configuration finished' notification
    void OnConfigurationFinished(ReturnCode_t);
    //! Forward the 'normal operation mode started' notification
    void OnStartNormalOperationMode(ReturnCode_t);
    //! Get error information from DeviceProcessing
    void OnErrorWithInfo(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID,
                         quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo);

    //! Device control layer diagnostic service acknwoledge
    void OnDiagnosticServiceClosed(qint16 DiagnosticResult);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnDestroyFinished
     */
    /****************************************************************************/
    void OnDestroyFinished();

private:
    //! Handle the state 'Task request pending'
    void HandleTaskRequestState();

    DeviceProcessing *mp_DevProc;     //!< Device processing instance
    QThread *mp_DevProcThread;        //!< Device processing thread
    QTimer *mp_DevProcTimer;          //!< Device processing timer
    Qt::HANDLE m_ParentThreadID;      //!< Parent thread ID

    //! Device processing task ID
    typedef enum {
        IDEVPROC_TASKID_INIT     = 0x00,    //!< Initialisation
        IDEVPROC_TASKID_FREE     = 0x01,    //!< Task free, nothing to do
        IDEVPROC_TASKID_ERROR    = 0x02,    //!< Error state
        IDEVPROC_TASKID_REQ_TASK = 0x03     //!< A reqest is active
    } IDeviceProcessingTaskID_t;

    //! Device processing task state
    typedef enum {
        IDEVPROC_TASK_STATE_FREE     = 0x00,    //!< Task state free, ready for action request
        IDEVPROC_TASK_STATE_REQ      = 0x01,    //!< An action was requested, next step will be to forward the command
        IDEVPROC_TASK_STATE_REQ_SEND = 0x02,    //!< Command was forwarded, wait for acknowledge, check timeout
        IDEVPROC_TASK_STATE_ERROR    = 0x03     //!< Error, e.g. timeout while waiting for acknowledge, or error CAN-message received
    } IDeviceProcessingTaskState_t;

    IDeviceProcessingTaskID_t m_taskID;         //!< Task identifier
    IDeviceProcessingTaskState_t m_taskState;   //!< Task state

    DeviceProcTask::TaskID_t m_reqTaskID;           //!< Task identification
    DeviceProcTask::TaskPrio_t m_reqTaskPriority;   //!< Task priority
    quint16 m_reqTaskParameter1;                    //!< Task parameter 1
    quint16 m_reqTaskParameter2;                    //!< Task parameter 2

    quint32 m_instanceID;                           //!< Instance identification
    CRotaryValveDevice *m_pRotaryValve;             //!< Rotary Valve device
    CAirLiquidDevice *m_pAirLiquid;                 //!< Air-liquid device
    CRetortDevice *m_pRetort;                       //!< Retort device
    COvenDevice *m_pOven;                           //!< Oven device
    CPeripheryDevice *m_pPeriphery;                 //!< Periphery device

    QMutex m_IMutex;    //!< Handles thread safety of IDeviceProcessing
    QMutex m_Mutex;     //!< Handles thread safety of DeviceProcessing
};

} //namespace

#endif /* IDEVICEPROCESSING_H */
