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

#include "Global/Include/EventObject.h"
#include "DeviceControl/Include/Configuration/HardwareConfiguration.h"
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/SlaveModules/AnalogOutput.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/Rfid15693.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/Devices/DeviceBase.h"
#include "Global/Include/AdjustedTime.h"
#include "Global/Include/EventObject.h"
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
    m_reqTaskParameter1(0), m_reqTaskParameter2(0),
    mp_DevProc(NULL)
{
    m_taskID = IDEVPROC_TASKID_FREE;
    m_taskState = IDEVPROC_TASK_STATE_FREE;

    moveToThread(&m_DevProcThread);

    //start the DeviceProcessing thread
    connect(&m_DevProcThread, SIGNAL(started()), this, SLOT(ThreadStarted()));
    m_DevProcThread.start();
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
        m_DevProcThread.quit();

        if (!m_DevProcThread.wait()) {
            LOGANDTHROW(EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR);
        }

        delete mp_DevProc;
    }
    catch (...)
    {
    }
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
void IDeviceProcessing::OnEvent(quint32 EventCode, quint16 EventData, QDateTime TimeStamp)
{
    FILE_LOG_L(laDEVPROC, llERROR) << " IDeviceProcessing::OnEvent (" << EventCode << ", " << EventData << ", "
                                   << TimeStamp.toString().constData() << ")";
    emit ReportEvent(EventCode, EventData, TimeStamp);
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
void IDeviceProcessing::OnEventWithInfo(quint32 EventCode, quint16 EventData, QDateTime TimeStamp, QString EventInfo)
{
    FILE_LOG_L(laDEVPROC, llERROR) << " IDeviceProcessing::OnEventWithInfo (" << EventCode << ", " << EventData << ", "
                                   << TimeStamp.toString().constData() << ", " << EventInfo.constData() << ")";
    emit ReportEventWithInfo(EventCode, EventData, TimeStamp, EventInfo);
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
            Global::EventObject::Instance().RaiseEvent(EVENT_DEVICECONTROL_ERROR_ACTIVATE_TASK,
                                                       Global::FmtArgs() << (quint16)m_reqTaskID);
            emit ReportEvent(EVENT_DEVICECONTROL_ERROR_ACTIVATE_TASK, (quint16)m_reqTaskID, errorTimeStamp);
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
    emit ReportInitializationFinished(HdlInfo);
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
    emit ReportConfigurationFinished(HdlInfo);
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
    emit ReportStartNormalOperationMode(HdlInfo);
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
void IDeviceProcessing::OnDiagnosticServiceClosed(ReturnCode_t DiagnosticResult)
{
    QDateTime errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();

    FILE_LOG_L(laDEVPROC, llINFO) << "  IDeviceProcessing: emit i_diagnosticFinished";

    emit ReportEvent(EVENT_DEVICECONTROL_ERROR_START_DIAG, (quint16)DiagnosticResult, errorTimeStamp);
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
CDeviceBase* IDeviceProcessing::GetDevice(DevInstanceID_t InstanceID)
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

void IDeviceProcessing::SetAdjustmentList(DataManager::CAdjustment AdjustmentList)
{
    if (mp_DevProc != NULL)
    {
        mp_DevProc->SetAdjustmentList(AdjustmentList);
    }
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

    emit DeviceShutdown();
}

/****************************************************************************/
/*!
 *  \brief  Received when device processing is stopped
 *
 *      This function stops the task loop of the device control layer.
 */
/****************************************************************************/
void IDeviceProcessing::OnDestroyFinished()
{
    m_DevProcTimer.stop();
    emit ReportDestroyFinished();
}

void IDeviceProcessing::ThreadStarted()
{
    /* activate the logging */
    FILE* pFile = fopen("device_control.log", "w");
    Output2FILE::Stream() = pFile;

    mp_DevProc = new DeviceProcessing(this);

    CONNECTSIGNALSLOT(mp_DevProc, ReportInitializationFinished(ReturnCode_t),
                      this, OnInitializationFinished(ReturnCode_t));
    CONNECTSIGNALSLOT(mp_DevProc, ReportConfigurationFinished(ReturnCode_t),
                      this, OnConfigurationFinished(ReturnCode_t));
    CONNECTSIGNALSLOT(mp_DevProc, ReportStartNormalOperationMode(ReturnCode_t),
                      this, OnStartNormalOperationMode(ReturnCode_t));
    CONNECTSIGNALSLOT(mp_DevProc, ReportEvent(quint32, quint16, QDateTime),
                      this, OnEvent(quint32, quint16, QDateTime));
    CONNECTSIGNALSLOT(mp_DevProc, ReportEventWithInfo(quint32, quint16, QDateTime, QString),
                      this, OnEventWithInfo(quint32, quint16, QDateTime, QString));
    CONNECTSIGNALSLOT(mp_DevProc, ReportDiagnosticServiceClosed(ReturnCode_t),
                      this, OnDiagnosticServiceClosed(ReturnCode_t));
    CONNECTSIGNALSLOT(mp_DevProc, ReportDestroyFinished(),
                      this, OnDestroyFinished());

    // Shutdown signal to device threads
    CONNECTSIGNALSIGNAL(this, DeviceShutdown(), mp_DevProc, DeviceShutdown());

    CONNECTSIGNALSLOT(&m_DevProcTimer, timeout(), this, HandleTasks());
    m_DevProcTimer.start(10);

    qDebug() << "IDeviceProcessing: " << thread() << m_DevProcThread.thread();
}

} // namespace
