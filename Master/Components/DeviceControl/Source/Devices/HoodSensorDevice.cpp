/******************************************************************/
/*! \file HoodSensorDevice.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.08.2012
 *   $Author:  $ Stalin Govindarajan
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CHoodSensorDevice
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

#include "DeviceControl/Include/Devices/HoodSensorDevice.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{
/****************************************************************************/
/*!
 *  \brief  Constructor of CHoodSensorDevice
 *
 *  \iparam pDeviceProcessing = pointer to DeviceProcessing
 *  \iparam Type = Device type string
 *  \iparam InstanceID = identifier of the class instance
 */
/****************************************************************************/
CHoodSensorDevice::CHoodSensorDevice(DeviceProcessing* pDeviceProcessing, QString Type, DevInstanceID_t InstanceID)
        : CBaseDevice(pDeviceProcessing, Type), m_pHoodSensor(0)
{
    m_instanceID = InstanceID;
    Q_ASSERT(m_instanceID == DEVICE_INSTANCE_ID_HOOD);

    m_HoodState = HOOD_STATE_UNDEF;

    m_MainState    = DEVICE_MAIN_STATE_START;
    m_MainStateOld = m_MainState;
    m_TaskID       = HOODDEV_TASKID_INIT;
    m_ErrorTaskState = HOOD_DEV_ERRTASK_STATE_FREE;

    FILE_LOG_L(laDEV, llINFO) << " hood sensor device created :" << std::hex << (int) m_instanceID;
}

/****************************************************************************/
/*!
 *  \brief    Destructor of CHoodSensorDevice
 *
 *
 *  \return
 *
 ****************************************************************************/
CHoodSensorDevice::~CHoodSensorDevice()
{
    // Zero all the pointer variables
    m_pDevProc = 0;
    m_pHoodSensor = 0;
}


/****************************************************************************/
/*!
 *  \brief    Handles the internal state machine
 *
 *   This function should be called periodically, it handles the state machine of
 *   the class. Furthermore, the HandleTask-function of the interface class will be called.
 *   Each main state has a individual handling function, which will be called according to the
 *   currently active main state
 *
 *
 *  \return   void
 */
/****************************************************************************/
void CHoodSensorDevice::HandleTasks()
{
    ReturnCode_t RetVal;

    if(m_MainState == DEVICE_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(m_MainState == DEVICE_MAIN_STATE_START)
    {
        m_MainState = DEVICE_MAIN_STATE_INIT;
    }
    else if(m_MainState == DEVICE_MAIN_STATE_INIT)
    {
        RetVal = HandleInitializationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_CONFIG;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = HOOD_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_FCT_MOD_CFG;
            m_TaskID = HOODDEV_TASKID_FREE;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = HOOD_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_FCT_MOD_CFG)
    {
        RetVal = ConfigureDeviceTasks();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_IDLE;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = HOOD_DEV_ERRTASK_STATE_FREE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << " CHoodSensorDevice, id " << (int) m_instanceID << ": new state: " << (int) m_MainState;
        m_MainStateOld = m_MainState;
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules pointer variables
 *           m_pHoodSensor - hood sensor function module
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS or error return code
 *
 ****************************************************************************/
ReturnCode_t CHoodSensorDevice::HandleInitializationState()
{
    //quint32 InstanceID;

    FILE_LOG_L(laDEV, llINFO) << "  CHoodSensorDevice::HandleInitializationState() id:" << (int) m_instanceID;

    m_pHoodSensor = (CDigitalInput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_HoodSensorKey));
    if(m_pHoodSensor == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_HOOD_DEV, ERROR_DCL_HOOD_DEV_INIT_FCT_ALLOC_FAILED,
                          (quint16) CANObjectKeyLUT::FCTMOD_HOOD_SENSOR);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_HOOD_SENSOR not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes configuration state.
 *
 *           This function connects each function module's signals to the internal slots.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if configuration was successfully executed
 *           otherwise DCL_ERR_FCT_CALL_FAILED
 *
 ****************************************************************************/
ReturnCode_t CHoodSensorDevice::HandleConfigurationState()
{

    //Button
    if (!connect(m_pHoodSensor, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), this,
                 SLOT(HoodStatusChanged(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_HOOD_DEV, ERROR_DCL_HOOD_DEV_CONFIG_CONNECT_FAILED,
                          (quint16) CANObjectKeyLUT::FCTMOD_HOOD_SENSOR);
        FILE_LOG_L(laDEV, llERROR) << "   Connect dig. input signal 'ReportActInputValue'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if (!connect(m_pHoodSensor, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)),
                 this, SLOT(HoodStatusError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_HOOD_DEV, ERROR_DCL_HOOD_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_HOOD_SENSOR);
        FILE_LOG_L(laDEV, llERROR) << "   Connect dig. input signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   Create and configure the device tasks
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CHoodSensorDevice::ConfigureDeviceTasks()
{
    quint8  TaskIndex = 0;

    CANDigitalInputTask* pCANDigitalInputTask;

    //######################################
    // set drawer position
    // -> motor position command
    TaskIndex = 0;
    pCANDigitalInputTask = CBaseDevice::GetNewCANDigitalInputTask(CANDigitalInputTask::FCTMOD_DI_TASK_ID_REQ_VALUE);
    pCANDigitalInputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANDigitalInputTask->m_TaskIndex = TaskIndex;
    pCANDigitalInputTask->m_TriggerTaskIndex = TaskIndex;
    pCANDigitalInputTask->m_pCANObject = m_pHoodSensor;
    pCANDigitalInputTask->m_Timeout = 5000;
    m_DeviceTask[HOOD_DEV_TASK_REQ_SENSOR_STATUS].m_FctModTaskMap[TaskIndex] = pCANDigitalInputTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " CHoodSensorDevice: add task 'MovePos'";

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief    Internal function for idle state machine processing
 *
 *   The function handles the idle state, which is active if the class is 'ready for tasks'
 *   Depending on the pending task, which is stored in m_taskID, the task specific handling
 *   function will be called.
 *
 *  \return   void
 */
/****************************************************************************/
void CHoodSensorDevice::HandleIdleState()
{
    if(m_TaskID == HOODDEV_TASKID_FREE)
    {
        ; // there are no pending tasks
    }
    else if(m_TaskID == HOODDEV_TASKID_COMMAND_HDL)
    {
        HandleDeviceTaskActions();
    }
    else if(m_TaskID == HOODDEV_TASKID_ERROR)
    {
        //Just wait for 'ExitErrormode'
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << " Invalid taskID: " << (int) m_TaskID;

        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        SetErrorParameter(EVENT_GRP_DCL_HOOD_DEV, ERROR_DCL_HOOD_DEV_INVALID_STATE, (quint16)m_TaskID);

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = HOOD_DEV_ERRTASK_STATE_REPORT_IFACE;
    }
}


/****************************************************************************/
/*!
 *  \brief    Handle the task to execute the device tasks and their fct-module tasks
 *
 *   The function is called from HandleTasks() if m_TaskID == HOODDEV_TASKID_COMMAND_HDL.
 *   The function loops thru the device tasks and processes the tasks pending for execution.
 *
 */
/****************************************************************************/
void CHoodSensorDevice::HandleDeviceTaskActions()
{
    if((m_pDevProc == 0) || (m_pHoodSensor == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_HOOD_DEV, ERROR_DCL_HOOD_DEV_NULL_PTR_ACCESS, (quint16) m_TaskID);

        //forward the error information. The error relevant datas should already have been set.
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = HOOD_DEV_ERRTASK_STATE_REPORT_IFACE;

        return;
    }

    for (quint8 idx = 0; idx < HOOD_DEVICE_TASK_COUNT; idx++)
    {
        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_START)
        {
            m_DeviceTask[idx].m_TaskState = DeviceTask:: DEV_TASK_STATE_PROGRESS;
        }

        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            // execute the device task.
            FunctionModuleTask* pFunctionModuleTask = 0;
            bool bActionStateInProgress = false;
            bool bActionStateWithErrorDetect = false;

            //get the funtion module task to start, if any
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(m_DeviceTask[idx].m_FctModTaskMap,
                                                                                             bActionStateInProgress,
                                                                                             bActionStateWithErrorDetect);

            while(pFunctionModuleTask)
            {
                if(pFunctionModuleTask)
                {
                    ReturnCode_t RetVal;

                    FILE_LOG_L(laDEV, llINFO) << "  CHoodSensorDevice: task for startup found.";
                    RetVal = m_pDevProc->GetFunctionModuleTaskManager()->StartTask(pFunctionModuleTask);
                    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
                    {
                        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_FAILED;
                        m_TaskID = HOODDEV_TASKID_ERROR;
                        SetErrorParameter(EVENT_GRP_DCL_HOOD_DEV, ERROR_DCL_HOOD_DEV_REQ_STATUS_FCTCALL_FAILED, (quint16) m_TaskID);

                        FILE_LOG_L(laDEV, llERROR) << "  CHoodSensorDevice: failed to start task: " << (int) pFunctionModuleTask->m_TaskID;
                        return;
                    }
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(m_DeviceTask[idx].m_FctModTaskMap,
                                                                                                 bActionStateInProgress,
                                                                                                 bActionStateWithErrorDetect);
            }

            if(bActionStateWithErrorDetect == true)
            {
                //one of the function module tasks has error state
                FILE_LOG_L(laDEV, llERROR) << "  CHoodSensorDevice: Action state error.";
                m_TaskID = HOODDEV_TASKID_ERROR;
                m_DeviceTask[idx].m_TaskState = DeviceTask::DEV_TASK_STATE_ERROR;
                m_ErrorTaskState = HOOD_DEV_ERRTASK_STATE_REPORT_IFACE;
            }
            else if(bActionStateInProgress == false)
            {
                FILE_LOG_L(laDEV, llINFO) << "  CHoodSensorDevice: positions received, next start movement..";

                //reset the tasks to the 'unused' state
                //m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(m_DeviceTask[idx].m_FctModTaskMap);
                m_DeviceTask[idx].m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
            }
        }

        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            if(idx == HOOD_DEV_TASK_REQ_SENSOR_STATUS)
            {
                //m_pIHoodSensorDev->RouteReqHoodStatusAckn(m_instanceID, m_lastErrorHdlInfo, m_HoodState);
            }

            //reset the tasks to the 'unused' state
            m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(m_DeviceTask[idx].m_FctModTaskMap);
            m_DeviceTask[idx].m_TaskState = DeviceTask::DEV_TASK_STATE_UNUSED;
            m_TaskID = HOODDEV_TASKID_FREE;
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the error state of the class.
 *
 *   This task controls the error handling of the class.
 *   The function is called from HandleIdleState() if m_MainState == DEVICE_MAIN_STATE_ERROR..
 *   This task consists of the following states:
 *
 *     HOOD_DEV_ERRTASK_STATE_REPORT_IFACE:
 *      This state reports the error to the interface class. The interface class will change to error state
 *      and blocks future requests
 *
 *     HOOD_DEV_ERRTASK_STATE_REPORT_DEVPROC:
 *      This state reports the error to the device processing class.
 *
 *     HOOD_DEV_ERRTASK_STATE_IDLE:
 *      Hold the class in error state, do nothing
 *
 *    HOOD_DEV_ERRTASK_STATE_RESET
 *      Reset the error state, set all states to it's initialisation state
 *
 *  \return   void
 */
/****************************************************************************/
void CHoodSensorDevice::HandleErrorState()
{
    if(m_ErrorTaskState == HOOD_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_ErrorTaskState == HOOD_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        for (quint8 idx = 0; idx < HOOD_DEVICE_TASK_COUNT; idx++)
        {
            if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_ERROR)
            {
                FunctionModuleTask* pFunctionModuleTask;
                FunctionModuleTask* pFunctionModuleTaskError = 0;
                QMapIterator<quint8, FunctionModuleTask*> iterFctModDevTask(m_DeviceTask[idx].m_FctModTaskMap);

                while (iterFctModDevTask.hasNext())
                {
                    pFunctionModuleTask = iterFctModDevTask.next().value();
                    if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_ERROR)
                    {
                        pFunctionModuleTaskError = pFunctionModuleTask;
                        break;
                    }
                }

                if(idx == HOOD_DEV_TASK_REQ_SENSOR_STATUS)
                {
                    /*if(m_pIHoodSensorDev != NULL)
                    {
                        m_pIHoodSensorDev->RouteReqHoodStatusAckn(m_instanceID, m_lastErrorHdlInfo, HOOD_STATE_UNDEF);
                    }*/
                    if((pFunctionModuleTaskError) && (pFunctionModuleTaskError->m_ErrorHdlInfo == DCL_ERR_FCT_CALL_FAILED))
                    {
                        SetErrorParameter(EVENT_GRP_DCL_HOOD_DEV, ERROR_DCL_HOOD_DEV_REQ_STATUS_FCTCALL_FAILED, (quint16) m_TaskID);
                        FILE_LOG_L(laDEV, llERROR) << "  CHoodSensorDevice::ExecReferenceMovement fct call failed";
                    }
                    else if((pFunctionModuleTaskError) && (pFunctionModuleTaskError->m_ErrorHdlInfo == DCL_ERR_TIMEOUT))
                    {
                        SetErrorParameter(EVENT_GRP_DCL_HOOD_DEV, ERROR_DCL_HOOD_DEV_REQ_STATUS_TIMEOUT, (quint16)m_TaskID);
                        FILE_LOG_L(laDEV, llERROR) << "  CHoodSensorDevice: while reference movement.. timeout";
                    }
                    else
                    {
                        SetErrorParameter(EVENT_GRP_DCL_HOOD_DEV, ERROR_DCL_HOOD_DEV_UNKNOWN_ERROR, (quint16)m_TaskID);
                        FILE_LOG_L(laDEV, llERROR) << "  CHoodSensorDevice: while reference movement.. ";
                    }
                }

                //reset the tasks to the 'unused' state
                m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(m_DeviceTask[idx].m_FctModTaskMap);
                m_DeviceTask[idx].m_TaskState = DeviceTask::DEV_TASK_STATE_UNUSED;

                /*if(m_pIHoodSensorDev != 0)
                {
                    m_pIHoodSensorDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
                }*/
            }
        }

        m_ErrorTaskState = HOOD_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_ErrorTaskState == HOOD_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_ErrorTaskState = HOOD_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_ErrorTaskState == HOOD_DEV_ERRTASK_STATE_RESET)
    {
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;

        // reset the function module references
        m_pHoodSensor = 0;
    }
}


/****************************************************************************/
/*!
 *  \brief    Handles the data signal of the digital input function module
 *
 *      InputValue = 1, Hood is open
 *      InputValue = 0, Hood is closed
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if valid data,
 *                      otherwise the return value of the error detecting function
 *  \iparam   InputValue = input value of the dig. channel
 */
/****************************************************************************/
void CHoodSensorDevice::HoodStatusChanged(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 InputValue)
{
    CANDigitalInputTask* pDigInputTask = dynamic_cast<CANDigitalInputTask*>
            (m_DeviceTask[HOOD_DEV_TASK_REQ_SENSOR_STATUS].m_FctModTaskMap[0]);

    FILE_LOG_L(laDEV, llINFO) << "  CHoodSensorDevice: " << (int) m_instanceID << " module:" << std::hex << InstanceID << " HoodStatusChanged: " << InputValue;

    if((m_pHoodSensor == 0) || (InstanceID != m_pHoodSensor->GetModuleHandle()))
    {
        FILE_LOG_L(laDEV, llINFO) << "   Modulehandle sensor status does not match " << std::hex << InstanceID;
    }
    else
    {
        FILE_LOG_L(laDEV, llINFO) << "   Hood State: " << (int) m_HoodState << " SubStateAction: " << (int) m_TaskID;

        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            //device task was activated, forward the request parameter to the corresponding function module task
            pDigInputTask->m_Value = InputValue;

            if (HOOD_OPEN == InputValue)
            {
                m_HoodState = HOOD_STATE_OPEN;
            }
            else
            {
                m_HoodState = HOOD_STATE_CLOSED;
            }

            // To differentiate if scheduler initiated status request or actual event
            if (pDigInputTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
            {
                pDigInputTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
            }
            else
            {
                m_TaskID = HOODDEV_TASKID_COMMAND_HDL;
                m_DeviceTask[HOOD_DEV_TASK_REQ_SENSOR_STATUS].m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
            }
        }
        else
        {
            FILE_LOG_L(laDEV, llINFO) << "  CHoodSensorDevice::HoodStatusChanged: error reading sensor status";

            pDigInputTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
        }
    }
}

/****************************************************************************/
/*!
*  \brief    This slot receives the digital input error signal
*
*  \iparam   InstanceID = instance identifier of the device
*  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_DIG_INP_SLV
*  \iparam   ErrorCode  = from CAN-message
*  \iparam   ErrorData  = from CAN-message
*  \iparam   ErrorTime  = error message reception time on master side
*
****************************************************************************/
void CHoodSensorDevice::HoodStatusError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_ErrorTaskState = HOOD_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief   Request sensor status of the hood
 *
 *           This function starts asynchronous request to get current hood status.
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANDigitalInputPrivate class.
 *           When finished, the slot HoodStatusChanged() will be called by
 *           CANDigitalInputPrivate, and forwards the acknowledge of this request to the interface class
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t CHoodSensorDevice::ReqHoodStatus()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(HOOD_DEV_TASK_REQ_SENSOR_STATUS))
    {
        FILE_LOG_L(laDEV, llINFO) << " CLoaderDevice";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CLoaderDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Helper function, activates the device task and it's fct-mdoule tasks
 *
 *  \iparam   DeviceTaskIndex = Device's task index
 *
 *  \return  true, if the task was activated, otherwise false
 *
 ****************************************************************************/
bool CHoodSensorDevice::ActivateDeviceTask(quint8 DeviceTaskIndex)
{
    bool Activated = false;
    FunctionModuleTask* pFunctionModuleTask;

    if((m_TaskID == HOODDEV_TASKID_FREE) || (m_TaskID == HOODDEV_TASKID_COMMAND_HDL))
    {
        if(m_DeviceTask[DeviceTaskIndex].m_TaskState == DeviceTask::DEV_TASK_STATE_UNUSED)
        {
            QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(m_DeviceTask[DeviceTaskIndex].m_FctModTaskMap);

            //st the device task's function module tasks to 'Start'-state
            while (iterFctModTask.hasNext())
            {
                pFunctionModuleTask = iterFctModTask.next().value();
                if(pFunctionModuleTask)
                {
                    pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
                }
            }
            m_DeviceTask[DeviceTaskIndex].m_TaskState = DeviceTask::DEV_TASK_STATE_START;
            m_TaskID = HOODDEV_TASKID_COMMAND_HDL;
            Activated = true;
        }
    }

    return Activated;
}
} //namespace

