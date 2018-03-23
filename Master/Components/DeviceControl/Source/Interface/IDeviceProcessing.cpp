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
#include <QCoreApplication>

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
#include "DeviceControl/Include/SlaveModules/PressureControl.h"
#include "Global/Include/AdjustedTime.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include <DeviceControl/Include/Devices/DeviceState.h>
#include <DeviceControl/Include/Devices/ArchiveServiceInforState.h>
#include <unistd.h>
#include <QFinalState>

namespace DeviceControl
{
//2* 60 * 60 * 1000;
const int INTERVAL_SAVE_SERVICE_LIFE_CYCLE = 2 * 60 * 60 * 1000;  //!< 2 hours
const int INTERVAL_SAVE_LIFE_CYCLE_RECORD =  10 * 60 * 1000;  //!< 10 mintues
/****************************************************************************/
/*!
 *  \brief  Constructor of the class IDeviceProcessing
 *  \param DevProcTimerInterval  timer interval by ms
 */
/****************************************************************************/
IDeviceProcessing::IDeviceProcessing(int DevProcTimerInterval) :
        m_reqTaskID(DeviceProcTask::TASK_ID_DP_UNDEF), m_reqTaskPriority(DeviceProcTask::TASK_PRIO_LOW),
        m_reqTaskParameter1(0), m_reqTaskParameter2(0), m_machine(this), m_TimerSaveServiceInfor(this),
        m_DevProcTimerInterval(DevProcTimerInterval),
        m_SaveLifeCycleRecordTimer(this)
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
    CONNECTSIGNALSLOT(mp_DevProc, ReportError(quint32, quint16, quint16, quint16, QDateTime),
                      this, OnError(quint32, quint16, quint16, quint16, QDateTime));
    CONNECTSIGNALSLOT(mp_DevProc, ReportErrorWithInfo(quint32, quint16, quint16, quint16, QDateTime, QString),
                      this, OnErrorWithInfo(quint32, quint16, quint16, quint16, QDateTime, QString));
    CONNECTSIGNALSLOT(mp_DevProc, ReportDiagnosticServiceClosed(qint16), this, OnDiagnosticServiceClosed(qint16));
    CONNECTSIGNALSLOT(mp_DevProc, ReportDestroyFinished(), this, OnDestroyFinished());
    CONNECTSIGNALSLOT(&m_TimerSaveServiceInfor, timeout(),this, OnTimeOutSaveServiceInfor());
    m_TimerSaveServiceInfor.setInterval(INTERVAL_SAVE_SERVICE_LIFE_CYCLE);

    CONNECTSIGNALSLOT(&m_SaveLifeCycleRecordTimer, timeout(),this, OnTimeOutSaveLifeCycleRecord());
    m_SaveLifeCycleRecordTimer.setInterval(INTERVAL_SAVE_LIFE_CYCLE_RECORD);
    m_SaveLifeCycleRecordTimer.setSingleShot(true);

    // Shutdown signal to device threads
    CONNECTSIGNALSIGNAL(this, DeviceShutdown(), mp_DevProc, DeviceShutdown());
    CONNECTSIGNALSIGNAL(mp_DevProc, ReportLevelSensorStatus1(), this, ReportLevelSensorStatus1());
    CONNECTSIGNALSIGNAL(mp_DevProc, ReportFillingTimeOut2Min(), this, ReportFillingTimeOut2Min());
    CONNECTSIGNALSIGNAL(mp_DevProc, ReportDrainingTimeOut2Min(), this, ReportDrainingTimeOut2Min());
    CONNECTSIGNALSIGNAL(mp_DevProc, ReportGetServiceInfo(ReturnCode_t, const DataManager::CModule&, const QString&),
                     this, ReportGetServiceInfo(ReturnCode_t, const DataManager::CModule&, const QString&));
    m_ParentThreadID = QThread::currentThreadId();

    m_deviceList <<  DEVICE_INSTANCE_ID_ROTARY_VALVE
                  << DEVICE_INSTANCE_ID_AIR_LIQUID
                  << DEVICE_INSTANCE_ID_OVEN
                  << DEVICE_INSTANCE_ID_RETORT
                  << DEVICE_INSTANCE_ID_MAIN_CONTROL
                  << DEVICE_INSTANCE_ID_OTHER_DEVICE;

    m_EnableLowerPressure = Global::Workaroundchecking("LOWER_PRESSURE");
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
        (void)mp_DevProcThread->wait();
        delete mp_DevProcTimer;
        delete mp_DevProcThread;
        delete mp_DevProc;
    }
    catch (...)
    {
    }
}

/****************************************************************************/
/*!
 *  \brief  slot for local thread started
 *
 *  This slot is connected to the signal started
 *
 */
/****************************************************************************/
void IDeviceProcessing::ThreadStarted()
{
    mp_DevProcTimer = new QTimer(this);
    CONNECTSIGNALSLOT(mp_DevProcTimer, timeout(), this, HandleTasks());
    mp_DevProcTimer->start(m_DevProcTimerInterval);
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
void IDeviceProcessing::OnError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorID, quint16 ErrorData, QDateTime TimeStamp)
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
void IDeviceProcessing::OnErrorWithInfo(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorID, quint16 ErrorData, QDateTime TimeStamp, QString ErrorInfo)
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
//bool IDeviceProcessing::GetSerialNumber(QString& SerialNo)
//{
//    return DeviceProcessing::GetSerialNumber(SerialNo);
//}

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
                             m_reqTaskID, errorTimeStamp); //lint !e641
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
        quint32 id;
        foreach (id, m_deviceList)
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
                case DEVICE_INSTANCE_ID_MAIN_CONTROL:
                    Name = "Main control";
                    m_pPeriphery = (CPeripheryDevice *)pDevice;
                    break;
                default:
                    LOG() << "unknown device detected";
                    break;
                }
            }

        }
    }
    emit ReportConfigurationFinished(m_instanceID, HdlInfo);
    if (DCL_ERR_FCT_CALL_SUCCESS == HdlInfo)
    {
        InitArchiveServiceInforState();
    }
}


void IDeviceProcessing::InitArchiveServiceInforState()
{
    CState *p_Active = new CState("Active", &m_machine);
    QFinalState *p_Final = new QFinalState(&m_machine);
    m_machine.setInitialState(p_Active);
    CState *p_LastState = NULL;
    quint32 id;
    foreach (id, m_deviceList)
    {
        CBaseDevice *pDevice = mp_DevProc->GetDevice(id);
        if (!pDevice)
          continue;

        CState *p_NewState = new CArchiveServiceInforState(pDevice, p_Active);

        if (p_LastState == NULL)
        {
            p_Active->setInitialState(p_NewState);
        }
        else
        {
            p_LastState->addTransition(p_LastState, SIGNAL(finished()), p_NewState);
        }
        p_LastState = p_NewState;

        CONNECTSIGNALSLOT(this, ReportSavedServiceInfor(const QString&), pDevice, OnReportSavedServiceInfor(const QString&));

    }
    p_LastState->addTransition(p_LastState, SIGNAL(finished()), p_Final);
    m_TimerSaveServiceInfor.start();
    m_machine.start();
}

void IDeviceProcessing::ArchiveServiceInfor()
{
    m_machine.stop();
    m_machine.start();
}

void IDeviceProcessing::NotifySavedServiceInfor(const QString& deviceType)
{
    emit ReportSavedServiceInfor(deviceType);
}

void IDeviceProcessing::ResetActiveCarbonFilterLifeTime(quint32 setVal)
{
    if(m_pAirLiquid)
    {
        m_pAirLiquid->ResetActiveCarbonFilterLifeTime(setVal);
    }
    ArchiveServiceInfor();
    m_SaveLifeCycleRecordTimer.start();//Note: It will not work in debug mode
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
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
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
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
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
CBaseDevice* IDeviceProcessing::GetDevice(quint32 InstanceID)
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
    if(mp_DevProcTimer->isActive()) {
        m_taskID = IDEVPROC_TASKID_REQ_TASK;
        m_taskState = IDEVPROC_TASK_STATE_REQ;

        m_reqTaskID = DeviceProcTask::TASK_ID_DP_DESTROY;
        m_reqTaskPriority = DeviceProcTask::TASK_PRIO_MIDDLE;
        m_reqTaskParameter1 = 0;
        m_reqTaskParameter2 = 0;
        emit DeviceShutdown();
    } else {
        emit ReportDestroyFinished();
    }
}

/****************************************************************************/
/**
 *  \brief  slot associate with Destroy
 *
 *  This slot is connected to the signal, ReportDestroyFinished
 *
 *
 */
/****************************************************************************/
void IDeviceProcessing::OnDestroyFinished()
{
    mp_DevProcTimer->stop();
    emit ReportDestroyFinished();
}

void IDeviceProcessing::OnTimeOutSaveServiceInfor()
{
    ArchiveServiceInfor();
    m_SaveLifeCycleRecordTimer.start();
}

void IDeviceProcessing::OnTimeOutSaveLifeCycleRecord()
{
    if (mp_DevProc)
    {
        mp_DevProc->WriteDeviceLifeCycle();
    }
}

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::ALReleasePressure()
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

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *  \param   targetPressure target Pressure
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::ALPressure(float targetPressure)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->Pressure(targetPressure);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *  \param  targetPressure target Pressure
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::ALVaccum(float targetPressure)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->Vaccum(targetPressure);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *  \iparam  DelayTime = Delay time before stop pump.
 *  \iparam  targetPressure = Target Pressure.
 *  \iparam  IgnorePressure = Ignore pressure check.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::ALDraining(quint32 DelayTime, float targetPressure, bool IgnorePressure)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->Draining(DelayTime, targetPressure, IgnorePressure);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

/****************************************************************************/
/**
 *  \brief  Device Force Draining function
 *
 *  \iparam  RVPos = RV position
 *  \iparam  targetPressure = target pressure
 *  \iparam  ReagentGrpID = reagent group ID
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::IDForceDraining(quint32 RVPos, float targetPressure, const QString& ReagentGrpID)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    QTime delayTime = QTime::currentTime();
    if((m_pRotaryValve)&&(m_pAirLiquid))
    {
        retCode = m_pAirLiquid->ReleasePressure();
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        // Move RV to sealing position
        retCode = m_pRotaryValve->ReqMoveToRVPosition((RVPosition_t)(RVPos + 1));
        if (DCL_ERR_DEV_RV_MOTOR_LOSTCURRENTPOSITION == retCode) // lost initial position
        {
           // Move to initial position and then move to sealing position again
           retCode = m_pRotaryValve->ReqMoveToInitialPosition((RVPosition_t)RVPos);
           if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
           {
               return retCode;
           }
           retCode = m_pRotaryValve->ReqMoveToRVPosition((RVPosition_t)(RVPos + 1));
        }
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }

        bool IsRightPos = false;
        delayTime = QTime::currentTime().addMSecs(30*1000);
        while (QTime::currentTime() < delayTime)
        {
            DelaySomeTime(100);
            RVPosition_t position = m_pRotaryValve->ReqActRVPosition();
            if (position == RVPos+1)
            {
                IsRightPos = true;
                break;
            }
        }
        if (false == IsRightPos)
        {
            return DCL_ERR_FCT_CALL_FAILED;
        }

        // Set positive pressure (40 kpa)
        retCode = m_pAirLiquid->Pressure(targetPressure);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            m_pAirLiquid->ReleasePressure();
            return retCode;
        }

        //Wait for 2 minutes to get 35kpa
        delayTime = QTime::currentTime().addMSecs(120*1000);
        qreal pressure = 0.0;
        bool IsGetTargetPressure = false;
        while (QTime::currentTime() < delayTime)
        {
            DelaySomeTime(100);
            pressure = m_pAirLiquid->GetPressure();
            if (qAbs(pressure - targetPressure) < 5.0)
            {
                IsGetTargetPressure = true;
                break;
            }
        }

        // Wait for 2 minutes to see if 20kpa has been reached. If not, report error
        if (false == IsGetTargetPressure)
        {
            pressure = m_pAirLiquid->GetPressure();
            if (pressure < 20.0)
            {
                m_pAirLiquid->ReleasePressure();
                return ERROR_DCL_FORCE_DRAINING_TIMEOUT_BULIDPRESSURE;
            }
        }

        // Move RV to tube position
        retCode = m_pRotaryValve->ReqMoveToRVPosition((RVPosition_t)(RVPos));
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            m_pAirLiquid->ReleasePressure();
            return retCode;
        }

        // Check if there is No reagent in the bottle
        qreal BasePressure = 1.33;
        if((ReagentGrpID == "RG1")||(ReagentGrpID == "RG2"))
        {
            BasePressure = 1.33;
        }
        else if((ReagentGrpID == "RG3")||(ReagentGrpID == "RG4")||(ReagentGrpID == "RG8"))
        {
            BasePressure = 1.05;
        }
        else if((ReagentGrpID == "RG5")||(ReagentGrpID == "RG7"))
        {
            BasePressure = 1.14;
        }
        else if((ReagentGrpID == "RG6"))
        {
            BasePressure = 0.53;
        }
#if !defined(__arm__)
    return DCL_ERR_FCT_CALL_SUCCESS;
#endif
        delayTime = QTime::currentTime().addMSecs(180*1000);
        while (QTime::currentTime() < delayTime)
        {
            DelaySomeTime(100);
            pressure = m_pAirLiquid->GetPressure();
            if (pressure < 3 * BasePressure)
            {
                m_pAirLiquid->ReleasePressure();
                return DCL_ERR_FCT_CALL_SUCCESS;
            }
        }

    }
    m_pAirLiquid->ReleasePressure();
    return DCL_ERR_FCT_CALL_FAILED;
}

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *  \iparam  DelayTime = Delay time before stop pump.
 *  \iparam  EnableInsufficientCheck = Flag to indicate if Insufficient check is needed.
 *  \iparam  SafeReagent4Paraffin = Flag to indicate if Filling is in safe reagent and for paraffin.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::ALFilling(quint32 DelayTime, bool EnableInsufficientCheck, bool SafeReagent4Paraffin)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->Filling(DelayTime, EnableInsufficientCheck, SafeReagent4Paraffin);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}


ReturnCode_t IDeviceProcessing::ALStopCmdExec(quint8 CmdType)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if (m_pAirLiquid)
    {
        m_pAirLiquid->StopCommandExec(CmdType);
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *  \iparam  DelayTime = Delay time before stop pump.
 *  \iparam  EnableInsufficientCheck = Flag to indicate if Insufficient check is needed.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::ALFillingForService(quint32 DelayTime, bool EnableInsufficientCheck)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->FillingForService(DelayTime, EnableInsufficientCheck);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *
 *  \return  Actual Pressure
 */
/****************************************************************************/
qreal IDeviceProcessing::ALGetRecentPressure()
{
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->GetRecentPressure();
    }
    else
    {
        return UNDEFINED_VALUE;
    }

}


/****************************************************************************/
/*!
 *  \brief   Enable temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Disable temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Set PID parameters for temperature control module.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Start temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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


/****************************************************************************/
/*!
 *  \brief   Start temperature control with PID parameters.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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


/****************************************************************************/
/*!
 *  \brief   Get the temperature sensor data captured in last 500 milliseconds.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  Index = Actual temperature sensor index.
 *
 *  \return  Actual temperature, UNDEFINED if failed.
 */
/****************************************************************************/
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

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *  \iparam  Type = Temperature control module.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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


/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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


/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::ALAllStop()
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->AllStop();
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

ReturnCode_t IDeviceProcessing::ALControlValve(quint8 ValveIndex, quint8 ValveState)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pAirLiquid)
    {
        return m_pAirLiquid->ControlValve(ValveIndex, ValveState);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

bool IDeviceProcessing::ALGetHeatingStatus(ALTempCtrlType_t Type)
{
    bool ret = false;
    if (m_pAirLiquid)
    {
        ret = m_pAirLiquid->IsTemperatureControlOn(Type);
    }

    return ret;
}

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::ALBreakAllOperation()
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


/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *  \iparam  pressureDrift = Pressure drift value.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::ALSetPressureDrift(qreal pressureDrift)
{
   if(m_pAirLiquid)
    {
        return m_pAirLiquid->SetPressureDrift(pressureDrift);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}


/****************************************************************************/
/*!
 *  \brief   Enable temperature control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/**
 *  \brief  Device interface function.
 *
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Start temperature control with PID parameters.
 *
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Get the temperature sensor data captured in last 500 milliseconds.
 *
 *  \iparam  Index = Actual temperature sensor index.
 *
 *  \return  Actual temperature, UNDEFINED if failed.
 */
/****************************************************************************/
qreal IDeviceProcessing::RVGetRecentTemperature(quint32 Index)
{
    if(m_pRotaryValve)
    {
        return m_pRotaryValve->GetRecentTemperature(Index);
    }
    else
    {
        return UNDEFINED_VALUE;
    }
}

/****************************************************************************/
/*!
 *  \brief   Get temperature control module's status.
 *
 *  \return  Temperature control module status.
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Request the rotary valve to move to its initial position.
 *  \param   RVPosition  rv position
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::RVReqMoveToInitialPosition(RVPosition_t RVPosition)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRotaryValve)
    {
        return m_pRotaryValve->ReqMoveToInitialPosition(RVPosition);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
} 

/****************************************************************************/
/*!
 *  \brief   Request the rotary valve to move to certain encoder disk's position.
 *
 *  \iparam  RVPosition = Target rotary valve encoder disk's position.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

ReturnCode_t IDeviceProcessing::RVReqMoveToCurrentTubePosition(RVPosition_t CurrentRVPosition)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRotaryValve)
    {
        return m_pRotaryValve->ReqMoveToCurrentTubePosition(CurrentRVPosition);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

/****************************************************************************/
/*!
 *  \brief   Return rotary valve's encoder disk's position.
 *
 *  \return  Current encoder disk position.
 */
/****************************************************************************/
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

quint32 IDeviceProcessing::GetCurrentLowerLimit()
{
    if(m_pRotaryValve)
    {
        return m_pRotaryValve->GetCurrentLowerLimit();
    }
    else
    {
        return 0;
    }
}

ReturnCode_t IDeviceProcessing::RVSetTemperatureSwitchState(qint8 HeaterVoltage, qint8 AutoType)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRotaryValve)
    {
        return m_pRotaryValve->SetTemperatureSwitchState(HeaterVoltage, AutoType);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

/****************************************************************************/
/*!
 *  \brief   Enable temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Disable temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Set PID parameters for temperature control module.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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


/****************************************************************************/
/*!
 *  \brief   Start temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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


/****************************************************************************/
/*!
 *  \brief   Start temperature control with PID parameters.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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


/****************************************************************************/
/*!
 *  \brief   Get the temperature sensor data captured in last 500 milliseconds.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  Index = Actual temperature sensor index.
 *
 *  \return  Actual temperature, UNDEFINED if failed.
 */
/****************************************************************************/
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

bool IDeviceProcessing::OvenGetHeatingStatus(OVENTempCtrlType_t Type)
{
    if(m_pOven)
    {
        return m_pOven->GetRecentHeatingStatus(Type);
    }
    else
    {
        return false;
    }
}
/****************************************************************************/
/*!
 *  \brief   Get the Oven lid sensor data captured in last 500 milliseconds.
 *
 *  \return  Actual lid status, UNDEFINED if failed.
 */
/****************************************************************************/
quint16 IDeviceProcessing::OvenGetRecentLidStatus()
{
    if(m_pOven)
    {
        return m_pOven->GetRecentOvenLidStatus();
    }
    else
    {
        return UNDEFINED_VALUE;
    }
}


/****************************************************************************/
/*!
 *  \brief   Get temperature control module's status.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  Temperature control module status.
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Enable temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Disable temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Set PID parameters for temperature control module.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Start temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Start temperature control with PID parameters.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Get the temperature sensor data captured in last 500 milliseconds.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  Index = Actual temperature sensor index.
 *
 *  \return  Actual temperature, UNDEFINED if failed.
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief   Get temperature control module's status.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  Temperature control module status.
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief  Unlock the retort.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief  Lock the retort.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

ReturnCode_t IDeviceProcessing::RTSetTemperatureSwitchState(RTTempCtrlType_t Type, qint8 HeaterVoltage, qint8 AutoType)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pRetort)
    {
        return m_pRetort->SetTemperatureSwitchState(Type, HeaterVoltage, AutoType);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

/****************************************************************************/
/*!
 *  \brief   Get the Retort lid sensor data captured in last 500 milliseconds.
 *
 *  \return  Actual retort lock status, UNDEFINED if failed.
 */
/****************************************************************************/
quint16 IDeviceProcessing::RTGetRecentLockStatus()
{
    if(m_pRetort)
    {
        return m_pRetort->GetRecentRetortLockStatus();
    }
    else
    {
        return UNDEFINED_VALUE;
    }
}

/****************************************************************************/
/*!
 *  \brief  Turn off the main relay.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief  Turn on the main relay.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 *  \brief  Turn on/off the remote/local alarm
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::PerControlAlarm(bool On, bool Remote)
{
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(m_pPeriphery)
    {
        if (On) {
            if (Remote) {
                return m_pPeriphery->TurnOnRemoteAlarm();
            }
            else {
                return m_pPeriphery->TurnOnLocalAlarm();
            }

        }
        else {
            if (Remote) {
                return m_pPeriphery->TurnOffRemoteAlarm();
            }
            else {
                return m_pPeriphery->TurnOffLocalAlarm();
            }
        }
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

quint16 IDeviceProcessing::PerGetRecentAlarmStatus(qint8 type)
{
    if(m_pPeriphery)
    {
        if (0 == type)
        {
            return m_pPeriphery->GetRecentLocalAlarmStatus();
        }
        else if (1 == type)
        {
            return m_pPeriphery->GetRecentRemoteAlarmStatus();
        }
    }

    return UNDEFINED_VALUE;
}


/****************************************************************************/
/**
 *  \brief  Bottle check function.
 *
 *  \iparam  ReagentGrpID = Reagent Group ID
 *  \iparam  TubePos = Tube position.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t IDeviceProcessing::IDBottleCheck(QString ReagentGrpID, RVPosition_t TubePos)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;
    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if((m_pRotaryValve)&&(m_pAirLiquid))
    {
        retCode = m_pRotaryValve->ReqMoveToRVPosition((RVPosition_t)((quint32)TubePos + 1));
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }

        retCode = m_pAirLiquid->PressureForBottoleCheck();
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }

        // turn off pump
        m_pAirLiquid->StopCompressor();

        // dealy one second to make sure the command has been sent to HW
        DelaySomeTime(1000);

        // Move RV to tube position
        retCode = m_pRotaryValve->ReqMoveToRVPosition(TubePos);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        //make the stable threshold time 3 seconds
        DelaySomeTime(3000);

        // Retrive 3 values, and then get the average one
        qreal pressure = 0.0;
        qint8 count = 3;
        while (count>0)
        {
            pressure += m_pAirLiquid->GetPressure();
            count--;
        }
        pressure = pressure/3;

        qreal baseLine = 0;
        if((ReagentGrpID == "RG1")||(ReagentGrpID == "RG2"))
        {
            baseLine = 1.33;
        }
        else if((ReagentGrpID == "RG3")||(ReagentGrpID == "RG4")||(ReagentGrpID == "RG8"))
        {
             baseLine = 1.05;
        }
        else if((ReagentGrpID == "RG5")||(ReagentGrpID == "RG7"))
        {
            baseLine = 1.14;
        }
        else if((ReagentGrpID == "RG6"))
        {
            baseLine = 0.53;
        }

         m_pAirLiquid->LogDebug(QString("Current pressure in bootle check is: %1, and reagentGrpID is: %2").arg(pressure).arg(ReagentGrpID));

#ifdef __arm__
        if(pressure < (0.4 * baseLine))
        {
            retCode = DCL_ERR_DEV_LA_BOTTLECHECK_FAILED_EMPTY;
            LOG()<<"Bottle Check: Empty";
        }
#if 0
        else if(pressure < (0.7 * baseLine))
        {
            retCode = DCL_ERR_DEV_LA_BOTTLECHECK_FAILED_LEAKAGE;
            LOG()<<"Bottle Check: Leakage or Not Full";
        }
#endif
        else if(pressure < (2 * baseLine))
        {
            retCode = DCL_ERR_FCT_CALL_SUCCESS;
            LOG()<<"Bottle Check: OK";
        }
        else
        {
            retCode = DCL_ERR_DEV_LA_BOTTLECHECK_FAILED_BLOCKAGE;
            LOG()<<"Bottle Check: Blockage";
        }
#endif

        return retCode;
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

ReturnCode_t IDeviceProcessing::IDSealingCheck(qreal ThresholdPressure, RVPosition_t SealPosition)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_FAILED;

    if(QThread::currentThreadId() != m_ParentThreadID)
    {
        return retCode;
    }
    if((m_pRotaryValve)&&(m_pAirLiquid))
    {
        RVPosition_t currentPosition = m_pRotaryValve->ReqActRVPosition();
        if(SealPosition != currentPosition)
        {
            retCode = m_pRotaryValve->ReqMoveToRVPosition(SealPosition);
            if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
            {
                m_pAirLiquid->LogDebug("In IDSealingCheck, ReqMoveToRVPosition failed");
                return retCode;
            }
        }

        // Make sure to get the target pressure (30Kpa) in 30 seconds
        qreal targetPressure = 0.0;
        if(m_EnableLowerPressure)
        {
            targetPressure = 12.0;
            retCode = m_pAirLiquid->SealingCheckPressure();
        }
        else
        {
            targetPressure = 30.0;
            retCode = m_pAirLiquid->Pressure();
        }
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            m_pAirLiquid->LogDebug("In IDSealingCheck, failure in setting 30Kpa pressure in 30 seconds");
            return retCode;
        }
        QTime delayTime = QTime::currentTime().addMSecs(30000);
        bool targetPressureFlag = false;
        while (QTime::currentTime() < delayTime)
        {
            DelaySomeTime(100);
            if (std::abs(long(targetPressure - m_pAirLiquid->GetPressure())) < 5.0)
            {
                targetPressureFlag = true;
                break;
            }
        }
        if (false == targetPressureFlag)
        {
            m_pAirLiquid->LogDebug("In IDSealingCheck, DCL_ERR_DEV_LA_SEALING_FAILED_PRESSURE");
            return DCL_ERR_DEV_LA_SEALING_FAILED_PRESSURE;
        }
#ifdef __arm__
        //make the stable threshold time 5 seconds
        DelaySomeTime(5000);
#endif
        qreal previousPressure = m_pAirLiquid->GetPressure();

        // Turn off pump
        m_pAirLiquid->StopCompressor();

        // Due to the limitation of simulator, we just return success in PC.
#ifndef __arm__
        return DCL_ERR_FCT_CALL_SUCCESS;
#endif
        // Wait for 30 seconds to get current pressure
        delayTime = QTime::currentTime().addMSecs(30000);
        qint8 counter = 0;
        while (QTime::currentTime() < delayTime)
        {
            DelaySomeTime(100);
            if(previousPressure-(m_pAirLiquid->GetPressure()) > ThresholdPressure)
            {
                counter++;
                if (counter>=3)
                {
                    m_pAirLiquid->LogDebug("In IDSealingCheck, DCL_ERR_DEV_LA_SEALING_FAILED_PRESSURE(Wait for 30 seconds to get current pressure)");
                    (void)m_pAirLiquid->ReleasePressure();
                    return DCL_ERR_DEV_LA_SEALING_FAILED_PRESSURE;
                }
            }
            else
            {
                counter = 0;
            }
        }

        LOG()<<"Sealing test: Succeed.";
        (void)m_pAirLiquid->ReleasePressure();
        return DCL_ERR_FCT_CALL_SUCCESS;
   }
   else
   {
       return DCL_ERR_NOT_INITIALIZED;
   }

   return DCL_ERR_FCT_CALL_SUCCESS;
}
ReportError_t IDeviceProcessing::GetSlaveModuleReportError(quint8 errorCode, const QString& devName, quint32 sensorName)
{
    ReportError_t reportError;
    memset(&reportError, 0, sizeof(reportError));
    //FILE_LOG_L(laFCT, llERROR) << " Device name is: "<<devName.toStdString()<<" sensorname is: "<< sensorName;
    if ("Retort" == devName && NULL != m_pRetort)
    {
        if (RT_BOTTOM == sensorName)
        {
            //FILE_LOG_L(laFCT, llERROR) <<"Retort bottom current is: "<<m_pRetort->GetHardwareStatus(RT_BOTTOM).Current;
            reportError = m_pRetort->GetSlaveModuleError(errorCode,CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        }
        else if (RT_SIDE == sensorName)
        {
            //FILE_LOG_L(laFCT, llERROR) <<"Retort side current is: "<<m_pRetort->GetHardwareStatus(RT_SIDE).Current;
            reportError = m_pRetort->GetSlaveModuleError(errorCode,CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        }
    }
    else if ("Oven" == devName && NULL != m_pOven)
    {
        if (OVEN_BOTTOM == sensorName)
        {
            //FILE_LOG_L(laFCT, llERROR) <<"Oven Bottom current is: "<<m_pOven->GetHeaterCurrent(OVEN_BOTTOM);
            reportError = m_pOven->GetSlaveModuleError(errorCode,CANObjectKeyLUT::FCTMOD_OVEN_BOTTOMTEMPCTRL);
        }
        else if (OVEN_TOP == sensorName)
        {
            //FILE_LOG_L(laFCT, llERROR) <<"Oven top current is: "<<m_pOven->GetHeaterCurrent(OVEN_TOP);
            reportError = m_pOven->GetSlaveModuleError(errorCode,CANObjectKeyLUT::FCTMOD_OVEN_TOPTEMPCTRL);
        }
    }
    else if ("RV" == devName && NULL != m_pRotaryValve)
    {
        //FILE_LOG_L(laFCT, llERROR) <<"RV current is: "<<m_pRotaryValve->GetHeaterCurrent();
        reportError = m_pRotaryValve->GetSlaveModuleError(errorCode,CANObjectKeyLUT::FCTMOD_RV_TEMPCONTROL);
    }
    else if ("LA" == devName && NULL != m_pAirLiquid)
    {
        if (AL_LEVELSENSOR == sensorName)
        {
            //FILE_LOG_L(laFCT, llERROR) <<"LA LevelSensor current is: "<<m_pAirLiquid->GetHeaterCurrent(AL_LEVELSENSOR);
            reportError = m_pAirLiquid->GetSlaveModuleError(errorCode,CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL);
        }
        else if (AL_TUBE1 == sensorName)
        {
            //FILE_LOG_L(laFCT, llERROR) <<"LA Tube1 current is: "<<m_pAirLiquid->GetHeaterCurrent(AL_TUBE1);
            reportError = m_pAirLiquid->GetSlaveModuleError(errorCode,CANObjectKeyLUT::FCTMOD_AL_TUBE1TEMPCTRL);
        }
        else if (AL_TUBE2 == sensorName)
        {
            //FILE_LOG_L(laFCT, llERROR) <<"LA Tube2 current is: "<<m_pAirLiquid->GetHeaterCurrent(AL_TUBE2);
            reportError = m_pAirLiquid->GetSlaveModuleError(errorCode,CANObjectKeyLUT::FCTMOD_AL_TUBE2TEMPCTRL);
        }
        else if (AL_FAN == sensorName)
        {
            reportError = m_pAirLiquid->GetSlaveModuleError(errorCode,CANObjectKeyLUT::FCTMOD_AL_PRESSURECTRL);
        }
    }


    reportError.errorTime -= (Global::AdjustedTime::Instance().GetOffsetSeconds() * 1000);//DCR7175: switch to system time of the error
    return reportError;
}

quint8 IDeviceProcessing::GetHeaterSwitchType(const QString& DevName)
{
    quint8 switchType = 0;
    if ( ("Retort" == DevName && m_pRetort != NULL)  )
    {
        switchType = m_pRetort->GetRecentHeaterSwitchType();
    }
    else if ("Oven" == DevName && m_pOven != NULL)
    {

    }
    else if ("LA" == DevName && m_pAirLiquid != NULL)
    {

    }
    else if ("RV" == DevName && m_pRotaryValve != NULL)
    {
        switchType = m_pRotaryValve->GetRecentHeaterSwitchType();
    }

    return switchType;
}

quint16 IDeviceProcessing::GetSensorCurrent(const QString& DevName, quint8 Index)
{
    quint16 current = 0;
    if ("Retort" == DevName)
    {
        if (0 == Index && m_pRetort != NULL)
        {
            current = m_pRetort->GetRecentCurrent(RT_SIDE);
        }
        else if (1 == Index  && m_pRetort != NULL)
        {
            current = m_pRetort->GetRecentCurrent(RT_BOTTOM);
        }
    }
    else if ("Oven" == DevName)
    {
        if (0 == Index && m_pOven != NULL)
        {
            current = m_pOven->GetRecentCurrent(OVEN_TOP);
        }
        else if (1 == Index && m_pOven != NULL)
        {
            current = m_pOven->GetRecentCurrent(OVEN_BOTTOM);
        }
    }
    else if ("LA" == DevName)
    {
        if (0 == Index && m_pAirLiquid != NULL)
        {
            current = m_pAirLiquid->GetRecentCurrent(AL_LEVELSENSOR);
        }
        else if (1 == Index && m_pAirLiquid != NULL)
        {
            current = m_pAirLiquid->GetRecentCurrent(AL_TUBE1);
        }
        else if (2 == Index && m_pAirLiquid != NULL)
        {
            current = m_pAirLiquid->GetRecentCurrent(AL_TUBE2);
        }
    }
    else if ("RV" == DevName && m_pRotaryValve != NULL)
    {
        current = m_pRotaryValve->GetRecentCurrent();
    }

    return current;
}

CFunctionModule* IDeviceProcessing::GetFunctionModuleRef(quint32 InstanceID, const QString &Key)
{
    CBaseDevice* p_Device = GetDevice(InstanceID);

    if (p_Device) {
        return mp_DevProc->GetFunctionModule(p_Device->GetFctModInstanceFromKey(Key));
    }
    return NULL;
}

quint16 IDeviceProcessing::IDGetSlaveCurrent(HimSlaveType_t Type)
{
    quint16 current = UNDEFINED_2_BYTE;
    switch(Type)
    {
    case Slave_3:
        if(m_pRotaryValve)
        {
           // current = m_pRotaryValve->GetBaseModuleCurrent((quint16)Type);
            current = m_pRotaryValve->GetrecentBaseModuleCurrent();
        }
        break;
    case Slave_5:
        if(m_pOven)
        {
            //current = m_pOven->GetBaseModuleCurrent((quint16)Type);
            current = m_pOven->GetrecentBaseModuleCurrent();
        }
        break;
    case Slave_15:
        if(m_pAirLiquid)
        {
            //current = m_pAirLiquid->GetBaseModuleCurrent((quint16)Type);
            current = m_pAirLiquid->GetrecentBaseModuleCurrent();
        }
        break;
    }
    return current;
}

quint16 IDeviceProcessing::IDGetSlaveVoltage(HimSlaveType_t Type)
{
    quint16 voltage = UNDEFINED_2_BYTE;
    switch(Type)
    {
    case Slave_3:
        if(m_pRotaryValve)
        {
            //voltage = m_pRotaryValve->GetBaseModuleVoltage((quint16)Type);
            voltage = m_pRotaryValve->GetrecentBaseModuleVoltage();
        }
        break;
    case Slave_5:
        if(m_pOven)
        {
           // voltage = m_pOven->GetBaseModuleVoltage((quint16)Type);
            voltage = m_pOven->GetrecentBaseModuleVoltage();
        }
        break;
    case Slave_15:
        if(m_pAirLiquid)
        {
            //voltage = m_pAirLiquid->GetBaseModuleVoltage((quint16)Type);
            voltage = m_pAirLiquid->GetrecentBaseModuleVoltage();
        }
        break;
    }
    return voltage;
}

CBaseModule* IDeviceProcessing::GetBaseModule(HimSlaveType_t Type)
{
    return mp_DevProc->GetBaseModule(Type);
}

quint16 IDeviceProcessing::IDGetRemoteAlarmStatus()
{
    return m_pPeriphery->GetRemoteAlarmStatus();
}

quint16 IDeviceProcessing::IDGetLocalAlarmStatus()
{
    return m_pPeriphery->GetLocalAlarmStatus();
}

ReturnCode_t IDeviceProcessing::IDSetAlarm(qint8 opcode)
{
    if (m_pPeriphery == NULL)
        return DCL_ERR_FCT_CALL_FAILED;

    switch (opcode) {
    case 5:
        m_pPeriphery->TurnOnRemoteAlarm();
    case 3:
        m_pPeriphery->TurnOnLocalAlarm();
        break;

    case 4:
    case 2:
    case -1:
    default:
        m_pPeriphery->TurnOffLocalAlarm();
        m_pPeriphery->TurnOffRemoteAlarm();
        break;
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

int IDeviceProcessing::DelaySomeTime(int DelayTime)
{
    int ret = -1;
    if(DelayTime > 0)
    {
        QEventLoop event;
        QTimer::singleShot(DelayTime, &event, SLOT(quit()));
        ret = event.exec();
    }
    return ret;
}

} // namespace
