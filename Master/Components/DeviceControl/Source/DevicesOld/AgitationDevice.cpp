/******************************************************************/
/*! \file AgitationDevice.cpp
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 08.07.2010
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CAgitationDevice
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

#include "DeviceControl/Include/Devices/AgitationDevice.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

#define AGITATION_TIMEOUT_REQ_ACTSPEED  500   //!< timeout speed request
#define AGITATION_TIMEOUT_REQ_ACTPOS    500   //!< timeout position request
#define AGITATION_TIMEOUT_REFRUN_REQ   5000   //!< timeout reference run
#define AGITATION_TIMEOUT_SET_SPEED    5000   //!< timeout for speed acknowledge

// motor speeds for the predefined agitation speed
#define AGITATION_MOTOR_SPEED_05    400   //!< agitation speed 0.5 revisions/ second
#define AGITATION_MOTOR_SPEED_10    800   //!< agitation speed 1.0 revisions/ second
#define AGITATION_MOTOR_SPEED_15    1200   //!< agitation speed 1.5 revisions/ second
#define AGITATION_MOTOR_SPEED_20    1600   //!< agitation speed 2.0 revisions/ second

// limits for conversion of motorspeed to type 'agitation speed'
/// \todo adapt
#define AGITATION_LIMIT_SPEED_05    300   //!< agitation speed 0.5 motor speed limit
#define AGITATION_LIMIT_SPEED_10    500   //!< agitation speed 1.0 motor speed limit
#define AGITATION_LIMIT_SPEED_15    700   //!< agitation speed 1.5 motor speed limit
#define AGITATION_LIMIT_SPEED_20    900   //!< agitation speed 2.0 motor speed limit

/****************************************************************************/
/*!
 *  \brief    Constructor for the CAgitationDevice
 *
 *
 *  \param    pDeviceProcessing = pointer to DeviceProcessing
 *  \param    Type = Device type string
 *
 *  \return
 *
 ****************************************************************************/
 CAgitationDevice::CAgitationDevice(DeviceProcessing* pDeviceProcessing, QString Type)
     : CBaseDevice(pDeviceProcessing, Type),
    m_pMotorAgitation(0),
    m_AgitationMotionProfile(0)
{
    m_MainState    = DEVICE_MAIN_STATE_START;
    m_TaskID       = AGITATION_TASKID_INIT;
    m_ErrorTaskState = AGITATION_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_AGITATION;

    FILE_LOG_L(laDEV, llINFO) << " create agitation device";
}

 /****************************************************************************/
 /*!
  *  \brief    Destructor of CAgitationDevice
  *
  *
  *  \return
  *
  ****************************************************************************/
CAgitationDevice::~CAgitationDevice()
{
    try
    {
        m_pDevProc = 0;
        m_pMotorAgitation = 0;
    }
    catch (...)
    {
        return;
    }
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
void CAgitationDevice::HandleTasks()
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
            FILE_LOG_L(laDEV, llERROR) << "   CAgitationDevice:HandleInitializationState:" << (int) RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = AGITATION_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_DEV_TASK_CFG;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            FILE_LOG_L(laDEV, llERROR) << "   CAgitationDevice:HandleConfigurationState:" << (int) RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = AGITATION_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_DEV_TASK_CFG)
    {
        RetVal = HandleDeviceTaskConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_IDLE;
            m_TaskID = AGITATION_TASKID_FREE;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            FILE_LOG_L(laDEV, llERROR) << "   CAgitationDevice:HandleDeviceTaskConfigurationState:" << (int) RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = AGITATION_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules pointer variables
 *           m_pMotorAgitation - Motor to move the agitation
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CAgitationDevice::HandleInitializationState()
{
    FILE_LOG_L(laDEV, llINFO) << " CAgitationDevice::HandleInitializationState()";
    m_pMotorAgitation = (CStepperMotor*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorAgitationKey));
    if(m_pMotorAgitation == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_AGITATION_DEV, ERROR_DCL_AGITATION_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_AGITATION);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_AGITATION not allocated.";
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
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CAgitationDevice::HandleConfigurationState()
{
    if(!connect(m_pMotorAgitation, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)), this, SLOT(StepperMotorReferenceMovementAckn(quint32, ReturnCode_t, qint32))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AGITATION_DEV, ERROR_DCL_AGITATION_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_AGITATION);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportReferenceMovementAckn' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if (!connect(m_pMotorAgitation, SIGNAL(ReportSetStateAckn(quint32, ReturnCode_t)), this, SLOT(StepperMotorSetStateAckn(quint32, ReturnCode_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportActState' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pMotorAgitation, SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16)), this, SLOT(StepperMotorMovementAckn(quint32, ReturnCode_t, qint32, qint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AGITATION_DEV, ERROR_DCL_AGITATION_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_AGITATION);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportMovementAckn' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pMotorAgitation, SIGNAL(ReportPosition(quint32, ReturnCode_t, qint32, qint8)), this, SLOT(StepperMotorActPosition(quint32, ReturnCode_t, qint32))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AGITATION_DEV, ERROR_DCL_AGITATION_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_AGITATION);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportPosition' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pMotorAgitation, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(StepperMotorError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_AGITATION_DEV, ERROR_DCL_AGITATION_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_AGITATION);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportError' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    // read process settings
    m_AgitationMotionProfile = GetProcSettingPosition("agitation_motion_profile");

    return DCL_ERR_FCT_CALL_SUCCESS;

}

/****************************************************************************/
/*!
 *  \brief   Create and configure the device tasks
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CAgitationDevice::HandleDeviceTaskConfigurationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CANStepperMotorTask* pCANStepperMotorTask;
    quint8  TaskIndex = 0;

    // Reference run task list
    // -> Set motor state 'enabled' (activate the motor driver IC)
    // -> Proceed reference run
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = m_pMotorAgitation;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pCANStepperMotorTask->m_SubCommandData = 1;  // enable motor
    m_DeviceTask[AGITATION_DEV_TASK_REFRUN].m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    FILE_LOG_L(laDEV, llDEBUG1) << " CAgitationDevice: add task 'SetState'";

    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = m_pMotorAgitation;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    m_DeviceTask[AGITATION_DEV_TASK_REFRUN].m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " CAgitationDevice: add task 'ReferenceRun'";

    //######################################
    // set agitation speed
    // -> motor speed command
    TaskIndex = 0;
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_SPEED);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = m_pMotorAgitation;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    m_DeviceTask[AGITATION_DEV_TASK_SET_SPEED].m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " CAgitationDevice: add task 'MoveSpeed'";

    //######################################
    // request agitation speed
    // -> request act speed command
    TaskIndex = 0;
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_SPEED);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = m_pMotorAgitation;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    m_DeviceTask[AGITATION_DEV_TASK_REQ_ACTSPEED].m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " CAgitationDevice: add task 'ReqActSpeed'";

    //######################################
    // request agitation position
    // -> request act position command
    TaskIndex = 0;
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = m_pMotorAgitation;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    m_DeviceTask[AGITATION_DEV_TASK_REQ_ACTPOS].m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " CAgitationDevice: add task 'ReqActPosition'";

    return RetVal;

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
void CAgitationDevice::HandleIdleState()
{
    if(m_TaskID == AGITATION_TASKID_FREE)
    {
        ; // there are no pending tasks
    }
    else if(m_TaskID == AGITATION_TASKID_COMMAND_HDL)
    {
        HandleDeviceTaskActions();
    }
    else if(m_TaskID == AGITATION_TASKID_ERROR)
    {
        //Just wait for 'ExitErrormode'
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << " Invalid taskID: " << (int) m_TaskID;

        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        SetErrorParameter(EVENT_GRP_DCL_AGITATION_DEV, ERROR_DCL_AGITATION_DEV_INVALID_STATE, (quint16) m_TaskID);
        m_TaskID = AGITATION_TASKID_FREE;

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = AGITATION_ERRTASK_STATE_REPORT_IFACE;
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the task to execute the device tasks and their fct-module tasks
 *
 *   The function is called from HandleTasks() if m_TaskID == LOADERDEV_TASKID_COMMAND_HDL.
 *   The function loops thru the device tasks and processes the tasks pending for execution.
 *
 */
/****************************************************************************/
void CAgitationDevice::HandleDeviceTaskActions()
{
    if((m_pDevProc == 0) || (m_pMotorAgitation == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_AGITATION_DEV, ERROR_DCL_IAGITATION_DEV_NULL_PTR_ACCESS, (quint16) m_TaskID);

        //forward the error information. The error relevant datas should already have been set.
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = AGITATION_ERRTASK_STATE_REPORT_IFACE;

        return;
    }

    for (quint8 idx = 0; idx < MAX_AGITATION_DEVICE_TASK_ID; idx++)
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

                    FILE_LOG_L(laDEV, llINFO) << "  CAgitationDevice: task for startup found.";
                    RetVal = m_pDevProc->GetFunctionModuleTaskManager()->StartTask(pFunctionModuleTask);
                    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
                    {
                        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_FAILED;
                        m_TaskID = AGITATION_TASKID_ERROR;
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_REQ_REFRUN_FCTCALL_FAILED, (quint16) m_TaskID);

                        FILE_LOG_L(laDEV, llERROR) << "  CAgitationDevice: failed to start task: " << (int) pFunctionModuleTask->m_TaskID;
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
                FILE_LOG_L(laDEV, llERROR) << "  CAgitationDevice: Action state error.";
                m_TaskID = AGITATION_TASKID_ERROR;
                m_DeviceTask[idx].m_TaskState = DeviceTask::DEV_TASK_STATE_ERROR;
                m_ErrorTaskState = AGITATION_ERRTASK_STATE_REPORT_IFACE;
            }
            else if(bActionStateInProgress == false)
            {
                FILE_LOG_L(laDEV, llINFO) << "  CAgitationDevice: all tasks executed.";

                //reset the tasks to the 'unused' state
                //m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(m_DeviceTask[idx].m_FctModTaskMap);
                m_DeviceTask[idx].m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
            }
        }

        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            if(idx == AGITATION_DEV_TASK_REFRUN)
            {
                FunctionModuleTask* pFunctionModuleTask = 0;
                AgitationPosition_t AgitationPosition = AGITATION_POS_UNDEF;

                pFunctionModuleTask = m_DeviceTask[idx].m_FctModTaskMap[1];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
                {
                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                    if(pMotorTask)
                    {
                        AgitationPosition = GetAgitationPosFromMotorPos(pMotorTask->m_ActPos);
                    }
                }
                //m_pIAgitationDev->RouteReferenceRunAckn(m_instanceID, m_lastErrorHdlInfo, AgitationPosition);
            }
            else if(idx == AGITATION_DEV_TASK_SET_SPEED)
            {
                FunctionModuleTask* pFunctionModuleTask = 0;
                AgitationSpeed_t AgitationSpeed = AGITATION_SPEED_UNDEF;

                pFunctionModuleTask = m_DeviceTask[idx].m_FctModTaskMap[0];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
                {
                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                    if(pMotorTask)
                    {
                        AgitationSpeed = GetAgitationSpeedFromMotorSpeed(pMotorTask->m_ActSpeed);
                    }
                }
                //m_pIAgitationDev->RouteSpeedAckn(m_instanceID, m_lastErrorHdlInfo, AgitationSpeed);
            }
            else if(idx == AGITATION_DEV_TASK_REQ_ACTSPEED)
            {
                FunctionModuleTask* pFunctionModuleTask = 0;
                AgitationSpeed_t AgitationSpeed = AGITATION_SPEED_UNDEF;

                pFunctionModuleTask = m_DeviceTask[idx].m_FctModTaskMap[0];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
                {
                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                    if(pMotorTask)
                    {
                        AgitationSpeed = GetAgitationSpeedFromMotorSpeed(pMotorTask->m_ActSpeed);
                    }
                }
                //m_pIAgitationDev->RouteActAgitationSpeed(m_instanceID, m_lastErrorHdlInfo, AgitationSpeed);
            }
            else if(idx == AGITATION_DEV_TASK_REQ_ACTPOS)
            {
                FunctionModuleTask* pFunctionModuleTask = 0;
                AgitationPosition_t AgitationPosition = AGITATION_POS_UNDEF;

                pFunctionModuleTask = m_DeviceTask[idx].m_FctModTaskMap[0];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
                {
                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                    if(pMotorTask)
                    {
                        AgitationPosition = GetAgitationPosFromMotorPos(pMotorTask->m_ActPos);
                    }
                }
                //m_pIAgitationDev->RouteActPosition(m_instanceID, m_lastErrorHdlInfo, AgitationPosition);
            }

            //reset the tasks to the 'unused' state
            m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(m_DeviceTask[idx].m_FctModTaskMap);
            m_DeviceTask[idx].m_TaskState = DeviceTask::DEV_TASK_STATE_UNUSED;
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the error state of the class.
 *
 *   This task controls the error handling of the class.
 *   The function is called from HandleIdleState() if m_mainState == AGITATION_DEV_MAIN_STATE_ERROR.
 *   This task consists of the following states:
 *
 *     AGITATION_DEV_ERRTASK_STATE_REPORT_IFACE:
 *      This state reports the error to the interface class. The interface class will change to error state
 *      and blocks future requests
 *
 *     AGITATION_DEV_ERRTASK_STATE_REPORT_DEVPROC:
 *      This state reports the error to the device processing class.
 *
 *     AGITATION_DEV_ERRTASK_STATE_IDLE:
 *      Hold the class in error state
 *
 *     AGITATION_DEV_ERRTASK_STATE_RESET
 *      Reset the error state, set all states to it's initialisation state
 *
 *  \return   void
 */
/****************************************************************************/
void CAgitationDevice::HandleErrorState()
{
    if(m_ErrorTaskState == AGITATION_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_ErrorTaskState == AGITATION_ERRTASK_STATE_REPORT_IFACE)
    {
        /*if(m_pIAgitationDev != 0)
        {
            m_pIAgitationDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }*/
        m_ErrorTaskState = AGITATION_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_ErrorTaskState == AGITATION_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_ErrorTaskState = AGITATION_ERRTASK_STATE_IDLE;
    }
    else if(m_ErrorTaskState == AGITATION_ERRTASK_STATE_RESET)
    {
        m_TaskID = AGITATION_TASKID_FREE;
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;
        // reset the function module references
        m_pMotorAgitation = 0;
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot receives the stepper motor error signal
 *
 *  \iparam   InstanceID = instance identifier of the device
 *  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_MOTOR_SLV
 *  \iparam   ErrorCode  = from CAN-message
 *  \iparam   ErrorData  = from CAN-message
 *  \iparam   ErrorTime  = error message reception time on master side
 *
 ****************************************************************************/
void CAgitationDevice::StepperMotorError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_ErrorTaskState = AGITATION_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief    Handles the stepper motors set state acknowledge notification
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the movement was successfully done,
 *                      otherwise the return value of the error detecting function
 */
/****************************************************************************/
void CAgitationDevice::StepperMotorSetStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llDEBUG) << " CAgitationDevice<" << (int) m_instanceID << "> StepperMotor 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < MAX_AGITATION_DEVICE_TASK_ID; idx++)
    {
        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            pFunctionModuleTask = 0;
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);

            while(pFunctionModuleTask)
            {
                if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
                {
                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
                    if((pMotorTask) && (pMotorTask->m_MotorTaskID == CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE))
                    {
                        pMotorTask->m_ErrorHdlInfo = HdlInfo;
                        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
                        {
                            FILE_LOG_L(laDEV, llDEBUG) << "  set task " << (int) pMotorTask->m_TaskIndex << " state to finished.";
                            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
                        }
                        else
                        {
                            // error was detected from motor side
                            FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while SetState.. error";
                            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
                        }
                    }
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief   Receives the motor's reference run acknowledge signal
 *
 *           This function will be called from CANStepperMotor and forwards the
 *           agitation's reference run acknowledge information.
 *           The function is called after a reference run was requested,
 *           and this command was acknowledged by the slave's function module.
 *           The function converts the motor position to the AgitationPosition_t type
 *
 *  \iparam   InstanceID = instance identifier of the device
 *  \iparam   HdlInfo    = DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *                        otherwise the return value of the error detecting function
 *  \iparam   Position   = the agitation motor's position
 *
 ****************************************************************************/
void CAgitationDevice::StepperMotorReferenceMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llDEBUG) << " CAgitationDevice<" << (int) m_instanceID << "> StepperMotor 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < MAX_AGITATION_DEVICE_TASK_ID; idx++)
    {
        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            pFunctionModuleTask = 0;
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);

            while(pFunctionModuleTask)
            {
                if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
                {
                    pFunctionModuleTask->m_ErrorHdlInfo = HdlInfo;
                    if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
                    {
                        FILE_LOG_L(laDEV, llDEBUG) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished.";
                        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
                    }
                    else
                    {
                        // error was detected from motor side
                        FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while RefRun.. error";
                        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
                    }

                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
                    if(pMotorTask)
                    {
                        pMotorTask->m_ActPos = (Position_t) Position;
                    }
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief   Receives the motor's movement acknowledge signal
 *
 *           This function will be called from CANStepperMotor and forwards the
 *           agitation's target speed acknowledge information.
 *           The function is called after a target speed was requested,
 *           and this command was acknowledged by the slave's function module.
 *           The function converts the motor speed to the AgitationSpeed_t type
 *
 *  \iparam   InstanceID = instance identifier of the device
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the movement was successfully done,
 *                      otherwise the return value of the error detecting function
 *  \iparam   Position = the agitation motor's actual position
 *  \iparam   Speed = the agitation motor's actual speed
 *
 *  \return  void
 *
 ****************************************************************************/
void CAgitationDevice::StepperMotorMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position, qint16 Speed)
{
    FunctionModuleTask* pFunctionModuleTask = 0;
    Q_UNUSED(Speed);

    FILE_LOG_L(laDEV, llDEBUG) << " CAgitationDevice<" << (int) m_instanceID << "> StepperMotor 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < MAX_AGITATION_DEVICE_TASK_ID; idx++)
    {
        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            pFunctionModuleTask = 0;
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);

            while(pFunctionModuleTask)
            {
                if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
                {
                    pFunctionModuleTask->m_ErrorHdlInfo = HdlInfo;
                    if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
                    {
                        FILE_LOG_L(laDEV, llDEBUG) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished.";
                        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
                    }
                    else
                    {
                        // error was detected from motor side
                        FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while SerPos.. error";
                        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
                    }

                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
                    if(pMotorTask)
                    {
                        pMotorTask->m_ActPos = (Position_t) Position;
                    }
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief   Receives the motor's actual position signal
 *
 *           This function will be called from CANStepperMotor and forwards the
 *           agitation's position information.
 *           The function is called after the motor's actual position  was requested,
 *           and this command was respond by the slave's function module.
 *           The function converts the motor speed to the AgitationPositin_t type
 *
 *  \iparam   InstanceID = instance identifier of the device
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the movement was successfully done,
 *                      otherwise the return value of the error detecting function
 *  \iparam   Position = the agitation motor's actual position
 *
 *  \return  void
 *
 ****************************************************************************/
void CAgitationDevice::StepperMotorActPosition(quint32 InstanceID, ReturnCode_t HdlInfo, Position_t Position)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llDEBUG) << " CAgitationDevice<" << (int) m_instanceID << "> StepperMotor 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < MAX_AGITATION_DEVICE_TASK_ID; idx++)
    {
        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            pFunctionModuleTask = 0;
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);

            while(pFunctionModuleTask)
            {
                if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
                {
                    pFunctionModuleTask->m_ErrorHdlInfo = HdlInfo;
                    if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
                    {
                        FILE_LOG_L(laDEV, llDEBUG) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished.";
                        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
                    }
                    else
                    {
                        // error was detected from motor side
                        FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while ActPosition.. error";
                        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
                    }

                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
                    if(pMotorTask)
                    {
                        pMotorTask->m_ActPos = (Position_t) Position;
                    }
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief   Request the agitation's motor's reference run
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANStepperMotor class.
 *           When finished, the slot StepperMotorReferenceMovementAckn() will be called by
 *           CANStepperMotor, and forwards the acknowledge of this request to the interface class
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 *
 ****************************************************************************/
ReturnCode_t CAgitationDevice::ReqReferenceRun()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(AGITATION_DEV_TASK_REFRUN))
    {
        FILE_LOG_L(laDEV, llINFO) << " CAgitationDevice";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CAgitationDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;

}

/****************************************************************************/
/*!
 *  \brief   Request the agitation's speed
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANStepperMotor class.
 *           When finished, the slot StepperMotorMovementAckn() will be called by
 *           CANStepperMotor, and forwards the acknowledge of this request to the interface class
 *
 *  \iparam   AgitationSpeed = Agitation speed
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 *
 ****************************************************************************/
ReturnCode_t CAgitationDevice::DriveSpeed(AgitationSpeed_t AgitationSpeed)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(AGITATION_DEV_TASK_SET_SPEED))
    {
        //device task was activated, forward the request parameter to the corresponding function module task
        FunctionModuleTask* pFunctionModuleTask = 0;
        pFunctionModuleTask = m_DeviceTask[AGITATION_DEV_TASK_SET_SPEED].m_FctModTaskMap[0];

        CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pMotorTask)
        {
            Speed_t MotorSpeed;
            pMotorTask->m_Profile = m_AgitationMotionProfile;
            if(AgitationSpeed == AGITATION_SPEED_STOP)
            {
                pMotorTask->m_TargetSpeed = 0;
                //ensure the motor will stop at 0 degree position
                pMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_SPEED_STOP_POS;
                pMotorTask->m_SubCommandData = 0;
            }
            else
            {
                MotorSpeed = GetMotorSpeedFromAgitationSpeed(AgitationSpeed);
                pMotorTask->m_TargetSpeed = MotorSpeed;
                pMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_NULL;
            }
            FILE_LOG_L(laDEV, llDEBUG) << " CAgitationDevice";
        }
        else
        {
            RetVal = DCL_ERR_NULL_PTR_ACCESS;
            FILE_LOG_L(laDEV, llERROR) << " CAgitationDevice: DCL_ERR_NULL_PTR_ACCESS";
        }
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CAgitationDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Request the actual agitation position
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANStepperMotor class.
 *           When the position was received, the slot StepperMotorPosition() will be called by
 *           CANStepperMotor, and forwards the position to the interface class
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 *
 ****************************************************************************/
ReturnCode_t CAgitationDevice::ReqActPosition()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(AGITATION_DEV_TASK_REQ_ACTPOS))
    {
        FILE_LOG_L(laDEV, llINFO) << " CAgitationDevice";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CAgitationDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Request the actual agitation speed
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANStepperMotor class.
 *           When the speed was received, the slot TBD() will be called by
 *           CANStepperMotor, and forwards the speed to the interface class
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 *
 ****************************************************************************/
ReturnCode_t CAgitationDevice::ReqActSpeed()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(AGITATION_DEV_TASK_REQ_ACTSPEED))
    {
        FILE_LOG_L(laDEV, llINFO) << " CAgitationDevice";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CAgitationDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Calculate the AgitationSpeed_t from the agitation's motor speed
 *
 *           Because the motor speed does not exactly match the requested speed, there
 *           are speed areas defined for each element of the AgitationSpeed_t enum type
 *
 *  \iparam  MotorSpeed = the motor speed
 *
 *  \return  The agitation speed as AgitationSpeed_t
 *
 ****************************************************************************/
AgitationSpeed_t CAgitationDevice::GetAgitationSpeedFromMotorSpeed(qint16 MotorSpeed)
{
    AgitationSpeed_t AgitationSpeed = AGITATION_SPEED_UNDEF;

    if(MotorSpeed == 0)
    {
        AgitationSpeed = AGITATION_SPEED_STOP;
    }
    else if(MotorSpeed <= AGITATION_LIMIT_SPEED_05)
    {
        AgitationSpeed = AGITATION_SPEED_05;
    }
    else if(MotorSpeed <= AGITATION_LIMIT_SPEED_10)
    {
        AgitationSpeed = AGITATION_SPEED_10;
    }
    else if(MotorSpeed <= AGITATION_LIMIT_SPEED_15)
    {
        AgitationSpeed = AGITATION_SPEED_15;
    }
    else if(MotorSpeed <= AGITATION_LIMIT_SPEED_20)
    {
        AgitationSpeed = AGITATION_SPEED_20;
    }

    return AgitationSpeed;
}

/****************************************************************************/
/*!
 *  \brief   Calculate the AgitationPosition_t from the agitation's motor position
 *
 *  \iparam  motorPos = the motor position
 *
 *  \return  The agitation position as AgitationPosition_t
 *
 ****************************************************************************/
AgitationPosition_t CAgitationDevice::GetAgitationPosFromMotorPos(Position_t motorPos)
{
    AgitationPosition_t agitationPosition = AGITATION_POS_UNDEF;

    /// \todo no only 0
    if(motorPos == 0)
    {
        agitationPosition = AGITATION_POS_BASING_POINT;
    }

    return agitationPosition;
}


/****************************************************************************/
/*!
 *  \brief   Calculate the motor speed from AgitationSpeed_t
 *
 *  \iparam  Speed = Agitation speed type
 *
 *  \return  The motor speed in halfsteps/sec
 *
 ****************************************************************************/
qint16 CAgitationDevice::GetMotorSpeedFromAgitationSpeed(AgitationSpeed_t Speed)
{
    qint16 MotorSpeed;

    if(Speed == AGITATION_SPEED_05)
    {
        MotorSpeed = AGITATION_MOTOR_SPEED_05;
    }
    else if(Speed == AGITATION_SPEED_10)
    {
        MotorSpeed = AGITATION_MOTOR_SPEED_10;
    }
    else if(Speed == AGITATION_SPEED_15)
    {
        MotorSpeed = AGITATION_MOTOR_SPEED_15;
    }
    else if(Speed == AGITATION_SPEED_20)
    {
        MotorSpeed = AGITATION_MOTOR_SPEED_20;
    }
    else
    {
        MotorSpeed = 0;
    }

    return MotorSpeed;
}

/****************************************************************************/
/*!
 *  \brief   Helper function, activates the device task and it's fct-mdoule tasks
 *
 *  \iparam   DeviceTaskIndex = Device's task index
 *
 *  \return  true, if the dask was activated, otherwise false
 *
 ****************************************************************************/
bool CAgitationDevice::ActivateDeviceTask(quint8 DeviceTaskIndex)
{
    bool Activated = false;
    FunctionModuleTask* pFunctionModuleTask;

    if((m_TaskID == AGITATION_TASKID_FREE) || (m_TaskID == AGITATION_TASKID_COMMAND_HDL))
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
            m_TaskID = AGITATION_TASKID_COMMAND_HDL;
            Activated = true;
        }
    }

    return Activated;

}

} //namespace
