/******************************************************************/
/*! \file IDeviceProcessing.cpp
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 08.08.2012
 *   Author:  $ Norbert Wiedmann / Stalin
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class IDeviceProcessing
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
/******************************************************************/

#include <QMutex>

//#include "DataLogging/Include/LoggingObject.h"
#include "DeviceControl/Include/Configuration/HardwareConfiguration.h"
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/SlaveModules/AnalogOutput.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/Rfid15693.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#ifdef PRE_ALFA_TEST
#include "DeviceControl/Include/SlaveModules/PressureControl.h"
#endif
#include "Global/Include/AdjustedTime.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor of the class IDeviceProcessing
 */
/****************************************************************************/
IDeviceProcessing::IDeviceProcessing() :
        m_reqTaskID(DeviceProcTask::TASK_ID_DP_UNDEF), m_reqTaskPriority(DeviceProcTask::TASK_PRIO_LOW),
        m_reqTaskParameter1(0), m_reqTaskParameter2(0)
{
    m_taskID = IDEVPROC_TASKID_FREE;
    m_taskState = IDEVPROC_TASK_STATE_FREE;
    m_instanceID = DEVICE_INSTANCE_ID_DEVPROC;
    m_pRotaryValve = NULL;
    m_pAirLiquid = NULL;
    m_pRetort = NULL;
    m_pOven = NULL;
    m_pPeriphery = NULL;
    /* activate the logging */
    FILE* pFile = fopen("device_control.log", "w");
    Output2FILE::Stream() = pFile;

    mp_DevProc = new DeviceProcessing(this);
    mp_DevProcThread = new QThread(); //!< Device processing thread

    this->moveToThread(mp_DevProcThread);
    CONNECTSIGNALSLOT(mp_DevProcThread, started(), this, ThreadStarted());

    //start the DeviceProcessing thread
    mp_DevProcThread->start();

    CONNECTSIGNALSLOT(mp_DevProc, ReportInitializationFinished(ReturnCode_t), this, OnInitializationFinished(ReturnCode_t));
    CONNECTSIGNALSLOT(mp_DevProc, ReportConfigurationFinished(ReturnCode_t), this, OnConfigurationFinished(ReturnCode_t));
    CONNECTSIGNALSLOT(mp_DevProc, ReportStartNormalOperationMode(ReturnCode_t), this, OnStartNormalOperationMode(ReturnCode_t));
    CONNECTSIGNALSLOT(mp_DevProc, ReportError(DevInstanceID_t, quint16, quint16, quint16, QDateTime),
                      this, OnError(DevInstanceID_t, quint16, quint16, quint16, QDateTime));
    CONNECTSIGNALSLOT(mp_DevProc, ReportErrorWithInfo(DevInstanceID_t, quint16, quint16, quint16, QDateTime, QString),
                      this, OnErrorWithInfo(DevInstanceID_t, quint16, quint16, quint16, QDateTime, QString));
    CONNECTSIGNALSLOT(mp_DevProc, ReportDiagnosticServiceClosed(qint16), this, OnDiagnosticServiceClosed(qint16));
    CONNECTSIGNALSLOT(mp_DevProc, ReportDestroyFinished(), this, OnDestroyFinished());
    m_ParentThreadID = QThread::currentThreadId();
#if 1
    qDebug()<< "IDeviceProcess's parent thread id is: "<<m_ParentThreadID;
#endif
}

/****************************************************************************/
/*!
 *  \brief  Destructor of the class IDeviceProcessing
 */
/****************************************************************************/
IDeviceProcessing::~IDeviceProcessing()
{
    try
    {
        mp_DevProcThread->quit();
        mp_DevProcThread->wait();
    delete mp_DevProcTimer;
    delete mp_DevProcThread;
        delete mp_DevProc;
    }
    catch (...)
    {
    }
}
void IDeviceProcessing::ThreadStarted()
{
    mp_DevProcTimer = new QTimer(this);
    CONNECTSIGNALSLOT(mp_DevProcTimer, timeout(), this, HandleTasks());
    mp_DevProcTimer->start(10);
}
/****************************************************************************/
/*!
 *  \brief  Forward error information via a signal
 *
 *  \iparam InstanceID = the error reporting object
 *  \iparam ErrorGroup = Error group ID
 *  \iparam ErrorID    = Error group ID
 *  \iparam ErrorData  = Optional error data
 *  \iparam TimeStamp  = Error time stamp
 */
/****************************************************************************/
void IDeviceProcessing::OnError(DevInstanceID_t InstanceID, quint16 ErrorGroup, quint16 ErrorID, quint16 ErrorData, QDateTime TimeStamp)
{
    FILE_LOG_L(laDEVPROC, llERROR) << " IDeviceProcessing::ThrowError (" << std::hex << (int) InstanceID << ", " <<
                                      std::hex << ErrorGroup << ", " << std::hex << ErrorID << ", " << std::hex << ErrorData << ")";
    emit ReportError(InstanceID, ErrorGroup, ErrorID, ErrorData, TimeStamp);
}

/****************************************************************************/
/*!
 *  \brief  Forward error information via a signal
 *
 *  \iparam InstanceID = the error reporting object
 *  \iparam ErrorGroup = Error group ID
 *  \iparam ErrorID    = Error group ID
 *  \iparam ErrorData  = Eptional error data
 *  \iparam TimeStamp  = Error time stamp
 *  \iparam ErrorInfo  = Additional error information
 */
/****************************************************************************/
void IDeviceProcessing::OnErrorWithInfo(DevInstanceID_t InstanceID, quint16 ErrorGroup, quint16 ErrorID, quint16 ErrorData, QDateTime TimeStamp, QString ErrorInfo)
{
    FILE_LOG_L(laDEVPROC, llERROR) << " IDeviceProcessing: emit event: " << std::hex << ErrorGroup << ", " <<
                                      std::hex << ErrorID << ", " <<
                                      ErrorData << ", '" << ErrorInfo.toStdString() << "'!";
    emit ReportErrorWithInfo(InstanceID, ErrorGroup, ErrorID, ErrorData, TimeStamp, ErrorInfo);
}

/****************************************************************************/
/*!
 *  \brief  Forward the serial number reading to implementation class
 *
 *  \iparam SerialNo = reference to a QString object, will be filled with the serial number
 *
 *  \return TRUE if the serial number has been filled, otherwise FALSE
 */
/****************************************************************************/
bool IDeviceProcessing::GetSerialNumber(QString& SerialNo)
{
    return DeviceProcessing::GetSerialNumber(SerialNo);
}

/****************************************************************************/
/*!
 *  \brief  Handle the internal state machine for this class
 */
/****************************************************************************/
void IDeviceProcessing::HandleTasks()
{
    if(m_taskID == IDEVPROC_TASKID_REQ_TASK)
    {
        HandleTaskRequestState();
    }

    m_Mutex.lock();
    mp_DevProc->HandleTasks();
    m_Mutex.unlock();
}

/****************************************************************************/
/*!
 *  \brief  Handle the state 'Task request pending'
 *
 *      A task was requested and must be forwarded to the implementation class
 */
/****************************************************************************/
void IDeviceProcessing::HandleTaskRequestState()
{
    // we are running in the mutexIDeviceProc locked state!
    ReturnCode_t retCode;

    QMutexLocker lock(&m_IMutex);

    FILE_LOG_L(laDEVPROC, llINFO) << " IDeviceProcessing::HandleTaskRequestState";

    if(m_taskState == IDEVPROC_TASK_STATE_REQ)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << " HandleTaskRequestState - AddTask";
        m_Mutex.lock();
        retCode = mp_DevProc->ActivateTask(m_reqTaskID, m_reqTaskParameter1, m_reqTaskParameter2);
        m_Mutex.unlock();
        if(retCode == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_taskState = IDEVPROC_TASK_STATE_FREE;
            m_taskID = IDEVPROC_TASKID_FREE;
        }
        else
        {
            QDateTime errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
            emit ReportError(m_instanceID, EVENT_GRP_DCL_DEVCTRL, ERROR_DCL_DEVCTRL_ACTIVATE_TASK_FAILED,
                             m_reqTaskID, errorTimeStamp);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Starts the device's configuration
 *
 *      After the creation and the connection to the IDeviceProcessing
 *      signal, this function starts the device's configuration and the
 *      configuration of the nodes as defined in hw_specification.xml.
 *      All errors during configuration will be forwarded by the
 *      i_errorDeviceProc signal
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS - configuration start done
 *          otherwise the error code, refer to ReturnCode_t definition
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::StartConfigurationService()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    FILE_LOG_L(laCONFIG, llDEBUG) << " IDeviceProcessing::StartConfigurationService";
    m_Mutex.lock();
    DeviceProcessing::DeviceProcessingMainState_t State = mp_DevProc->GetState();
    m_Mutex.unlock();
    if(State < DeviceProcessing::DP_MAIN_STATE_WAIT_FOR_CONFIG)
    {
        retval = DCL_ERR_INVALID_STATE;
    }
    else
    {
        m_IMutex.lock();
        m_taskID = IDEVPROC_TASKID_REQ_TASK;
        m_taskState = IDEVPROC_TASK_STATE_REQ;

        m_reqTaskID = DeviceProcTask::TASK_ID_DP_CONFIG;
        m_reqTaskPriority = DeviceProcTask::TASK_PRIO_MIDDLE;
        m_reqTaskParameter1 = 0;
        m_reqTaskParameter2 = 0;
        m_IMutex.unlock();
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Restarts the device's configuration
 *
 *      This function should be called to reinitialize the device
 *      and the node configuration during runtime. This can be
 *      necessary whenever a node is connected to or disconnected
 *      from the bus.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS - reconfiguration start done
 *          otherwise the error code, refer to ReturnCode_t definition
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::RestartConfigurationService()
{
    m_Mutex.lock();
    mp_DevProc->Initialize();
    m_Mutex.unlock();
    return StartConfigurationService();
}

/****************************************************************************/
/*!
 *  \brief  Forwards the 'initialisation finished' notification
 *
 *      Will be called from inside the dcl, the function forwards the
 *      'initialisation finished' notification by emitting a signal.
 *
 *  \iparam HdlInfo = The initialisation result code
 */
/****************************************************************************/
void IDeviceProcessing::OnInitializationFinished(ReturnCode_t HdlInfo)
{
    FILE_LOG_L(laDEVPROC, llINFO) << "  IDeviceProcessing::RouteInitializationFinished:" << (int) HdlInfo;
    emit ReportInitializationFinished(m_instanceID, HdlInfo);
}

/****************************************************************************/
/*!
 *  \brief  Forwards the 'configuration finished' notification
 *
 *      Will be called from inside the dcl, the function forwards the
 *      'configuration finished' notification by emitting a signal.
 *
 *  \iparam  HdlInfo = The configuration result code
 */
/****************************************************************************/
void  IDeviceProcessing::OnConfigurationFinished(ReturnCode_t HdlInfo)
{
    FILE_LOG_L(laDEVPROC, llINFO) << "  IDeviceProcessing::RouteConfigurationFinished: " << (int) HdlInfo;
    if((HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)||(HdlInfo == DCL_ERR_TIMEOUT))
    {
        QList<DevInstanceID_t> list;
        list <<  DEVICE_INSTANCE_ID_ROTARY_VALVE
              << DEVICE_INSTANCE_ID_AIR_LIQUID
              << DEVICE_INSTANCE_ID_OVEN
              << DEVICE_INSTANCE_ID_RETORT
              << DEVICE_INSTANCE_ID_PERIPHERY;

        DevInstanceID_t id;
        foreach (id, list)
        {
            CBaseDevice *pDevice = mp_DevProc->GetDevice(id);
            if (pDevice && pDevice->GetMainState() == CBaseDevice::DEVICE_MAIN_STATE_IDLE)
            {
                QString Name = "";
                switch(id)
                {
                case  DEVICE_INSTANCE_ID_ROTARY_VALVE:
                    Name = "RotaryValve";
                    m_pRotaryValve = (CRotaryValveDevice*)pDevice;
                    break;
                case DEVICE_INSTANCE_ID_AIR_LIQUID:
                    Name = "AirLiquid";
                    m_pAirLiquid = (CAirLiquidDevice*)pDevice;
                    break;
                case DEVICE_INSTANCE_ID_OVEN:
                    Name = "Oven";
                    m_pOven = (COvenDevice *)pDevice;
                    break;
                case DEVICE_INSTANCE_ID_RETORT:
                    Name = "Retort";
                    m_pRetort =  (CRetortDevice *)pDevice;
                    break;
                case DEVICE_INSTANCE_ID_PERIPHERY:
                    Name = "Periphery";
                    m_pPeriphery = (CPeripheryDevice *)pDevice;
                    break;
                default:
                    qDebug() << "unknown device detected";
                    break;
                }
            }

        }
    }

 emit ReportConfigurationFinished(m_instanceID, HdlInfo);

}

/****************************************************************************/
/*!
 *  \brief Forwards the 'normal operation mode starts' notification
 *
 *      Will be called from inside the dcl, the function forwards the
 *      'normal operation mode starts' notification by emitting a signal.
 *
 *  \iparam HdlInfo = The configuration result code
 */
/****************************************************************************/
void IDeviceProcessing::OnStartNormalOperationMode(ReturnCode_t HdlInfo)
{
    emit ReportStartNormalOperationMode(m_instanceID, HdlInfo);
}

/****************************************************************************/
/*!
 *  \brief  Start diagnostic service
 *
 *      By calling this function the diagnostic service will start.
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::StartDiagnosticService()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    QMutexLocker locker(&m_IMutex);

    m_taskID = IDEVPROC_TASKID_REQ_TASK;
    m_taskState = IDEVPROC_TASK_STATE_REQ;

    m_reqTaskID = DeviceProcTask::TASK_ID_DP_DIAGNOSTIC;
    m_reqTaskPriority = DeviceProcTask::TASK_PRIO_MIDDLE;
    m_reqTaskParameter1 = 0;
    m_reqTaskParameter2 = 0;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Close the diagnostic service
 *
 *      By calling this function the diagnostic service will finish
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS - reconfiguration start done
 *          otherwise the error code, refer to ReturnCode_t definition
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::CloseDiagnosticService()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    FILE_LOG_L(laDEVPROC, llINFO) << "  IDeviceProcessing: close diagnostic service.";
    //m_devProc.SetTaskState(DeviceProcTask::TASK_ID_DP_DIAGNOSTIC, DeviceProcTask::TASK_STATE_ACTIVE_BRK);

    return retval;
}

/****************************************************************************/
/**
 *  \brief  Close the diagnostic service
 *
 *      By calling this function the diagnostic service will finish
 *
 *  \iparam DiagnosticResult = Error code, in any
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS - reconfiguration start done
 *          otherwise the error code, refer to ReturnCode_t definition
 */
/****************************************************************************/
void IDeviceProcessing::OnDiagnosticServiceClosed(qint16 DiagnosticResult)
{
    QDateTime errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();

    FILE_LOG_L(laDEVPROC, llINFO) << "  IDeviceProcessing: emit i_diagnosticFinished";

    emit ReportError(m_instanceID, EVENT_GRP_DCL_CONFIGURATION, ERROR_DCL_DIAG_FINISHED,
                     DiagnosticResult, errorTimeStamp);
}

/****************************************************************************/
/*!
 *  \brief  Start adjustement service
 *
 *      By calling this function the adjustement service will start.
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::StartAdjustmentService()
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    QMutexLocker locker(&m_IMutex);

    m_taskID = IDEVPROC_TASKID_REQ_TASK;
    m_taskState = IDEVPROC_TASK_STATE_REQ;

    m_reqTaskID = DeviceProcTask::TASK_ID_DP_ADJUST;
    m_reqTaskPriority = DeviceProcTask::TASK_PRIO_MIDDLE;
    m_reqTaskParameter1 = 0;
    m_reqTaskParameter2 = 0;

    return retCode;
}

/****************************************************************************/
/*!
 *  \brief  Returns a pointer to Device derivided class specified by instanceID
 *
 *  \iparam InstanceID = instance Id of the required Device
 *
 *  \return The pointer to the specified device, if any
 */
/****************************************************************************/
CBaseDevice* IDeviceProcessing::GetDevice(DevInstanceID_t InstanceID)
{
    QMutexLocker locker(&m_Mutex);
    return mp_DevProc->GetDevice(InstanceID);
}

/****************************************************************************/
/*!
 *  \brief  Returns a base module from the object tree
 *
 *      The functions returns pointer the base modules from the object tree
 *      in the order they are stored at the list.
 *
 *  \iparam First = The first base module should be returned
 *
 *  \return Next base module in object tree
 */
/****************************************************************************/
CBaseModule* IDeviceProcessing::GetNode(bool First)
{
    QMutexLocker locker(&m_Mutex);
    return mp_DevProc->GetCANNodeFromObjectTree(First);
}

/****************************************************************************/
/*!
 *  \brief  Send an emergency stop message to all nodes
 *
 *      The function sends an emergency stop message to all nodes
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was acceppted,
 *          otherwise the error code
 */
/****************************************************************************/
void IDeviceProcessing::EmergencyStop()
{
    QMutexLocker locker(&m_IMutex);

    m_taskID = IDEVPROC_TASKID_REQ_TASK;
    m_taskState = IDEVPROC_TASK_STATE_REQ;

    m_reqTaskID = DeviceProcTask::TASK_ID_DP_CONFIG;
    m_reqTaskPriority = DeviceProcTask::TASK_PRIO_MIDDLE;
    m_reqTaskParameter1 = 0;
    m_reqTaskParameter2 = 0;
}

/****************************************************************************/
/*!
 *  \brief  Forward the system's shutdown to the device control layer
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was acceppted,
 *          otherwise the error code
 */
/****************************************************************************/
void IDeviceProcessing::Shutdown()
{
    QMutexLocker locker(&m_IMutex);

    m_taskID = IDEVPROC_TASKID_REQ_TASK;
    m_taskState = IDEVPROC_TASK_STATE_REQ;

    m_reqTaskID = DeviceProcTask::TASK_ID_DP_DESTROY;
    m_reqTaskPriority = DeviceProcTask::TASK_PRIO_MIDDLE;
    m_reqTaskParameter1 = 0;
    m_reqTaskParameter2 = 0;
}

/****************************************************************************/
/*!
 *  \brief  Forward the system's standby command to the device control layer
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was acceppted,
 *          otherwise the error code
 */
/****************************************************************************/
void IDeviceProcessing::Standby()
{
    QMutexLocker locker(&m_IMutex);

    m_taskID = IDEVPROC_TASKID_REQ_TASK;
    m_taskState = IDEVPROC_TASK_STATE_REQ;

    m_reqTaskID = DeviceProcTask::TASK_ID_DP_CONFIG;
    m_reqTaskPriority = DeviceProcTask::TASK_PRIO_MIDDLE;
    m_reqTaskParameter1 = 0;
    m_reqTaskParameter2 = 0;
}

/****************************************************************************/
/*!
 *  \brief  Destroy the Device processing.
 *
 *      A call to this function will cause the closing procedure of the
 *      device processing. All threads will be finished, all objects will be
 *      deleted. The functions blocks as long as all the work is done. When
 *      the function returns, the only thing left is to delete this
 *      IDeviceProcessing instance.
 */
/****************************************************************************/
void IDeviceProcessing::Destroy()
{
    QMutexLocker locker(&m_IMutex);

    m_taskID = IDEVPROC_TASKID_REQ_TASK;
    m_taskState = IDEVPROC_TASK_STATE_REQ;

    m_reqTaskID = DeviceProcTask::TASK_ID_DP_DESTROY;
    m_reqTaskPriority = DeviceProcTask::TASK_PRIO_MIDDLE;
    m_reqTaskParameter1 = 0;
    m_reqTaskParameter2 = 0;
}

void IDeviceProcessing::OnDestroyFinished()
{
    mp_DevProcTimer->stop();
    emit ReportDestroyFinished();
}
ReturnCode_t IDeviceProcessing::ALSetPressureCtrlON()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->SetPressureCtrlON();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::ALSetPressureCtrlOFF()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->SetPressureCtrlOFF();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::ALReleasePressure(void)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->ReleasePressure();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::ALPressure()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->Pressure();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::ALVaccum()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->Vaccum();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::ALDraining(quint32 DelayTime)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->Draining(DelayTime);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::ALFilling(quint32 DelayTime)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->Filling(DelayTime);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
qreal IDeviceProcessing::ALGetRecentPressure(quint8 Index)
{
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->GetRecentPressure(Index);
    }
    else
    {
        return UNDEFINED_VALUE;
    }

}

ReturnCode_t IDeviceProcessing::ALSetTempCtrlON(ALTempCtrlType_t Type)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->SetTempCtrlON(Type);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::ALSetTempCtrlOFF(ALTempCtrlType_t Type)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
   if(m_pAirLiquid)
    {
        return m_pAirLiquid->SetTempCtrlOFF(Type);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::ALSetTemperaturePid(ALTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
   if(m_pAirLiquid)
    {
        return m_pAirLiquid->SetTemperaturePid(Type, MaxTemperature, ControllerGain, ResetTime, DerivativeTime);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::ALStartTemperatureControl(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
   if(m_pAirLiquid)
    {
        return m_pAirLiquid->StartTemperatureControl(Type, NominalTemperature, SlopeTempChange);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }

}

ReturnCode_t IDeviceProcessing::ALStartTemperatureControlWithPID(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->StartTemperatureControlWithPID(Type, NominalTemperature, SlopeTempChange, MaxTemperature, ControllerGain,  ResetTime,  DerivativeTime);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }

}

qreal IDeviceProcessing::ALGetRecentTemperature(ALTempCtrlType_t Type, quint8 Index)
{
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->GetRecentTemperature(Type, Index);
    }
    else
    {
        return UNDEFINED_VALUE;
    }
}
TempCtrlState_t IDeviceProcessing::ALGetTemperatureControlState(ALTempCtrlType_t Type)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return TEMPCTRL_STATE_ERROR;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->GetTemperatureControlState(Type);
    }
    else
    {
        return TEMPCTRL_STATE_ERROR;
    }
}

ReturnCode_t IDeviceProcessing::ALTurnOnFan()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
   if(m_pAirLiquid)
    {
        return m_pAirLiquid->TurnOnFan();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::ALTurnOffFan()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
   if(m_pAirLiquid)
    {
        return m_pAirLiquid->TurnOffFan();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

ReturnCode_t IDeviceProcessing::ALAllStop()
{
   if(m_pAirLiquid)
    {
        return m_pAirLiquid->BreakAllOperation();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

ReturnCode_t IDeviceProcessing::RVSetTempCtrlON()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRotaryValve)
    {
        return m_pRotaryValve->SetTempCtrlON();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::RVSetTempCtrlOFF()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRotaryValve)
    {
        return m_pRotaryValve->SetTempCtrlOFF();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::RVSetTemperaturePid(quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRotaryValve)
    {
        return m_pRotaryValve->SetTemperaturePid(MaxTemperature,ControllerGain,ResetTime,DerivativeTime);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::RVStartTemperatureControl(qreal NominalTemperature, quint8 SlopeTempChange)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRotaryValve)
    {
        return m_pRotaryValve->StartTemperatureControl(NominalTemperature,SlopeTempChange);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }

}

ReturnCode_t IDeviceProcessing::RVStartTemperatureControlWithPID(qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
   if(m_pRotaryValve)
    {
        return m_pRotaryValve->StartTemperatureControlWithPID(NominalTemperature, SlopeTempChange, MaxTemperature, ControllerGain,  ResetTime,  DerivativeTime);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }

}


qreal IDeviceProcessing::RVGetRecentTemperature(quint32 Index)
{
    if(m_pAirLiquid)
    {
        return m_pRotaryValve->GetRecentTemperature(Index);
    }
    else
    {
        return UNDEFINED_VALUE;
    }
}
TempCtrlState_t IDeviceProcessing::RVGetTemperatureControlState()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return TEMPCTRL_STATE_ERROR;
    }
    if(m_pRotaryValve)
    {
        return m_pRotaryValve->GetTemperatureControlState();
    }
    else
    {
        return TEMPCTRL_STATE_ERROR;
    }
}
//! Execute the move to intial position of the RV
ReturnCode_t IDeviceProcessing::RVReqMoveToInitialPosition()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRotaryValve)
    {
        return m_pRotaryValve->ReqMoveToInitialPosition();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
//! Position the oven cover
ReturnCode_t IDeviceProcessing::RVReqMoveToRVPosition( RVPosition_t RVPosition)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRotaryValve)
    {
        return m_pRotaryValve->ReqMoveToRVPosition(RVPosition);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

//! Request actual oven cover position
RVPosition_t IDeviceProcessing::RVReqActRVPosition()
{
    if(m_pRotaryValve)
    {
        return m_pRotaryValve->ReqActRVPosition();
    }
    else
    {
        return RV_UNDEF;
    }
}
ReturnCode_t IDeviceProcessing::OvenSetTempCtrlON(OVENTempCtrlType_t Type)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pOven)
    {
        return m_pOven->SetTempCtrlON(Type);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::OvenSetTempCtrlOFF(OVENTempCtrlType_t Type)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pOven)
    {
        return m_pOven->SetTempCtrlOFF(Type);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::OvenSetTemperaturePid(OVENTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pOven)
    {
        return m_pOven->SetTemperaturePid(Type, MaxTemperature, ControllerGain, ResetTime, DerivativeTime);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

ReturnCode_t IDeviceProcessing::OvenStartTemperatureControl(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
   if(m_pOven)
    {
        return m_pOven->StartTemperatureControl(Type, NominalTemperature, SlopeTempChange);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
   }
}

ReturnCode_t IDeviceProcessing::OvenStartTemperatureControlWithPID(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
   if(m_pOven)
    {
        return m_pOven->StartTemperatureControlWithPID(Type, NominalTemperature, SlopeTempChange, MaxTemperature, ControllerGain,  ResetTime,  DerivativeTime);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }

}

qreal IDeviceProcessing::OvenGetRecentTemperature(OVENTempCtrlType_t Type, quint8 Index)
{
    if(m_pOven)
    {
        return m_pOven->GetRecentTemperature(Type, Index);
    }
    else
    {
        return UNDEFINED_VALUE;
    }
}
TempCtrlState_t IDeviceProcessing::OvenGetTemperatureControlState(OVENTempCtrlType_t Type)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return TEMPCTRL_STATE_ERROR;
    }
    if(m_pOven)
    {
        return m_pOven->GetTemperatureControlState(Type);
    }
    else
    {
        return TEMPCTRL_STATE_ERROR;
    }
}
ReturnCode_t IDeviceProcessing::RTSetTempCtrlON(RTTempCtrlType_t Type)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRetort)
    {
        return m_pRetort->SetTempCtrlON(Type);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::RTSetTempCtrlOFF(RTTempCtrlType_t Type)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRetort)
    {
        return m_pRetort->SetTempCtrlOFF(Type);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::RTSetTemperaturePid(RTTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRetort)
    {
        return m_pRetort->SetTemperaturePid(Type, MaxTemperature, ControllerGain, ResetTime, DerivativeTime);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::RTStartTemperatureControl(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRetort)
    {
        return m_pRetort->StartTemperatureControl(Type, NominalTemperature, SlopeTempChange);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::RTStartTemperatureControlWithPID(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
   if(m_pRetort)
    {
        return m_pRetort->StartTemperatureControlWithPID(Type, NominalTemperature, SlopeTempChange, MaxTemperature, ControllerGain,  ResetTime,  DerivativeTime);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }

}
qreal IDeviceProcessing::RTGetRecentTemperature(RTTempCtrlType_t Type, quint8 Index)
{
        if(m_pRetort)
        {
            return m_pRetort->GetRecentTemperature(Type, Index);
        }
        else
        {
            return UNDEFINED_VALUE;
        }
}
TempCtrlState_t IDeviceProcessing::RTGetTemperatureControlState(RTTempCtrlType_t Type)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return TEMPCTRL_STATE_ERROR;
    }
    if(m_pRetort)
    {
        return m_pRetort->GetTemperatureControlState(Type);
    }
    else
    {
        return TEMPCTRL_STATE_ERROR;
    }
}
ReturnCode_t IDeviceProcessing::RTUnlock()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRetort)
    {
        return m_pRetort->Unlock();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}
ReturnCode_t IDeviceProcessing::RTLock()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRetort)
    {
        return m_pRetort->Lock();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

ReturnCode_t IDeviceProcessing::PerTurnOffMainRelay()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pPeriphery)
    {
        return m_pPeriphery->TurnOffMainRelay();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

ReturnCode_t IDeviceProcessing::PerTurnOnMainRelay()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pPeriphery)
    {
        return m_pPeriphery->TurnOnMainRelay();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

ReturnCode_t IDeviceProcessing::IDBottleCheck(QString ReagentGrpID, RVPosition_t TubePos)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_FAILED;
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if((m_pRotaryValve)&&(m_pAirLiquid))
    {
        qreal density = 1;
        qreal basePressure = 1.4;

        retCode = m_pRotaryValve->ReqMoveToRVPosition((RVPosition_t)(TubePos + 1));
        if(DCL_ERR_DEV_RV_MOVE_OK != retCode)
        {
            return retCode;
        }

        retCode = m_pAirLiquid->PressureForBottoleCheck();
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }

        if((ReagentGrpID == "RG1")||(ReagentGrpID == "RG2"))
        {
            density = 1;
        }
        else if((ReagentGrpID == "RG3")||(ReagentGrpID == "RG4")||(ReagentGrpID == "RG8"))
        {
            density = 0.8;
        }
        else if((ReagentGrpID == "RG5")||(ReagentGrpID == "RG7"))
        {
            density = 0.88;
        }
        else if((ReagentGrpID == "RG6"))
        {
            density = 1;
            basePressure = 0.6;
        }
        retCode = m_pRotaryValve->ReqMoveToRVPosition(TubePos);
        if(DCL_ERR_DEV_RV_MOVE_OK != retCode)
        {
            return retCode;
        }
        usleep(3000*1000);
        qreal pressure = m_pAirLiquid->GetRecentPressure(0);

        qDebug()<<"Bottle Check pressure: " << pressure;

        if(pressure < (0.4 * density * basePressure))
        {
            retCode = DCL_ERR_DEV_BOTTLE_CHECK_NOT_FULL;
            qDebug()<<"Bottle Check: Not full";
        }
        else if(pressure < (0.7 * density * basePressure))
        {
            retCode = DCL_ERR_DEV_BOTTLE_CHECK_LEAKAGE;
            qDebug()<<"Bottle Check: leakage";
        }
        else if(pressure < (2 * density * basePressure))
        {
            retCode = DCL_ERR_DEV_BOTTLE_CHECK_OK;
            qDebug()<<"Bottle Check: OK";
        }
        else
        {
            retCode = DCL_ERR_DEV_BOTTLE_CHECK_BLOCKAGE;
            qDebug()<<"Bottle Check: blockage";
        }

        m_pRotaryValve->ReqMoveToRVPosition((RVPosition_t)(TubePos + 1));
        return retCode;
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

} // namespace
