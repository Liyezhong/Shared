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
#define UNDEFINED_VALUE (999)
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
    CBaseDevice* GetDevice(DevInstanceID_t InstanceID);
    //! Return the pointer to the CBaseModule which is next in list
    CBaseModule* GetNode(bool First);
    //Air liquid device funcs
    ReturnCode_t ALSetPressureCtrlON();
    ReturnCode_t ALSetPressureCtrlOFF();
    ReturnCode_t ALReleasePressure(void);
    ReturnCode_t ALPressure();
    ReturnCode_t ALVaccum();
    ReturnCode_t ALDraining(quint32 DelayTime);
    ReturnCode_t ALFilling(quint32 DelayTime);
    qreal ALGetRecentPressure(quint8 Index);
    ReturnCode_t ALSetTempCtrlON(ALTempCtrlType_t Type);
    ReturnCode_t ALSetTempCtrlOFF(ALTempCtrlType_t type);
    ReturnCode_t ALSetTemperaturePid(ALTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    ReturnCode_t ALStartTemperatureControl(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    qreal ALGetRecentTemperature(ALTempCtrlType_t Type, quint8 Index);
    TempCtrlState_t ALGetTemperatureControlState(ALTempCtrlType_t Type);
    ReturnCode_t ALTurnOnFan();
    ReturnCode_t ALTurnOffFan();
    ReturnCode_t ALAllStop();
    ReturnCode_t ALBreakAllOperation();
    ReturnCode_t ALSetPressureDrift(qreal pressureDrift);
    ReturnCode_t ALStartTemperatureControlWithPID(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    //Rotary Valve device func
    ReturnCode_t RVSetTempCtrlON();
    ReturnCode_t RVSetTempCtrlOFF();
    ReturnCode_t RVSetTemperaturePid(quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    ReturnCode_t RVStartTemperatureControl(qreal NominalTemperature, quint8 SlopeTempChange);
    qreal RVGetRecentTemperature(quint32 Index);
    TempCtrlState_t RVGetTemperatureControlState();
    //! Execute the move to intial position of the RV
    ReturnCode_t RVReqMoveToInitialPosition();
    //! Position the oven cover
    ReturnCode_t RVReqMoveToRVPosition( RVPosition_t RVPosition);
    //! Request actual oven cover position
    RVPosition_t RVReqActRVPosition();
    ReturnCode_t RVStartTemperatureControlWithPID(qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    //Oven device func
    ReturnCode_t OvenSetTempCtrlON(OVENTempCtrlType_t Type);
    ReturnCode_t OvenSetTempCtrlOFF(OVENTempCtrlType_t type);
    ReturnCode_t OvenSetTemperaturePid(OVENTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    ReturnCode_t OvenStartTemperatureControl(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    qreal OvenGetRecentTemperature(OVENTempCtrlType_t Type, quint8 Index);
    quint16 OvenGetRecentLidStatus();
    TempCtrlState_t OvenGetTemperatureControlState(OVENTempCtrlType_t Type);
    ReturnCode_t OvenStartTemperatureControlWithPID(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    //Retort device func
    ReturnCode_t RTSetTempCtrlON(RTTempCtrlType_t Type);
    ReturnCode_t RTSetTempCtrlOFF(RTTempCtrlType_t Type);
    ReturnCode_t RTSetTemperaturePid(RTTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    ReturnCode_t RTStartTemperatureControl(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    ReturnCode_t RTStartTemperatureControlWithPID(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    qreal RTGetRecentTemperature(RTTempCtrlType_t Type, quint8 Index);
    TempCtrlState_t RTGetTemperatureControlState(RTTempCtrlType_t Type);
    ReturnCode_t RTUnlock();
    ReturnCode_t RTLock();
    quint16 RTGetRecentLockStatus();
    //Periphery device func
    ReturnCode_t PerTurnOffMainRelay();
    ReturnCode_t PerTurnOnMainRelay();

    ReturnCode_t IDBottleCheck(QString ReagentGrpID, RVPosition_t TubePos);
signals:
    //! Forward the 'intitialisation finished' notification
    void ReportInitializationFinished(DevInstanceID_t, ReturnCode_t);
    //! Forward the 'configuration finished' notification
    void ReportConfigurationFinished(DevInstanceID_t, ReturnCode_t);
    //! Forward the 'normal operation mode started' notification
    void ReportStartNormalOperationMode(DevInstanceID_t, ReturnCode_t);

    //! Forward error information
    void ReportError(DevInstanceID_t instanceID, quint16 usErrorGroup, quint16 usErrorID,
                     quint16 usErrorData, QDateTime timeStamp);
    //! Forward error information
    void ReportErrorWithInfo(DevInstanceID_t instanceID, quint16 usErrorGroup, quint16 usErrorID,
                             quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo);

    void ReportDestroyFinished();

private slots:
    //! Task handling
    void HandleTasks();
    void ThreadStarted();

    //! Get the 'intitialisation finished' notification
    void OnInitializationFinished(ReturnCode_t);
    //! Get the 'configuration finished' notification
    void OnConfigurationFinished(ReturnCode_t);
    //! Forward the 'normal operation mode started' notification
    void OnStartNormalOperationMode(ReturnCode_t);

    //! Get error information from DeviceProcessing
    void OnError(DevInstanceID_t instanceID, quint16 usErrorGroup, quint16 usErrorID,
                 quint16 usErrorData, QDateTime timeStamp);
    //! Get error information from DeviceProcessing
    void OnErrorWithInfo(DevInstanceID_t instanceID, quint16 usErrorGroup, quint16 usErrorID,
                         quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo);

    //! Device control layer diagnostic service acknwoledge
    void OnDiagnosticServiceClosed(qint16 DiagnosticResult);

    void OnDestroyFinished();

private:
    //! Handle the state 'Task request pending'
    void HandleTaskRequestState();

    DeviceProcessing *mp_DevProc;   //!< Device processing instance
    QThread *mp_DevProcThread;        //!< Device processing thread
    QTimer *mp_DevProcTimer;          //!< Device processing timer
    Qt::HANDLE m_ParentThreadID;
    typedef enum {
        IDEVPROC_TASKID_INIT     = 0x00,    //!< Initialisation
        IDEVPROC_TASKID_FREE     = 0x01,    //!< Task free, nothing to do
        IDEVPROC_TASKID_ERROR    = 0x02,    //!< Error state
        IDEVPROC_TASKID_REQ_TASK = 0x03     //!< A reqest is active
    } IDeviceProcessingTaskID_t;

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
    quint16 m_reqTaskParameter1;    //!< Task parameter 1
    quint16 m_reqTaskParameter2;    //!< Task parameter 2

    DevInstanceID_t m_instanceID;   //!< Instance identification
    CRotaryValveDevice *m_pRotaryValve;
    CAirLiquidDevice *m_pAirLiquid;
    CRetortDevice *m_pRetort;
    COvenDevice *m_pOven;
    CPeripheryDevice *m_pPeriphery;

    QMutex m_IMutex;    //!< Handles thread safety of IDeviceProcessing
    QMutex m_Mutex;     //!< Handles thread safety of DeviceProcessing
};

} //namespace

#endif /* IDEVICEPROCESSING_H */
