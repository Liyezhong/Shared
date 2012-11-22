/******************************************************************/
/*! \file RackTransferDevice.cpp
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 03.09.2010
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CRackTransferDevice
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

#include "DeviceControl/Include/Devices/RackTransferDevice.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

#define RACKTRANSFER_DEV_TIMEOUT_REFRUN_REQ       5000  ///< timeout adapter reference run
#define RACKTRANSFER_DEV_TIMEOUT_SET_POS          5000  ///< timeout adapter postioning
#define RACKTRANSFER_DEV_TIMEOUT_ACTPOS_REQ        500  ///< timeout actual position request

#define RACKTRANSFER_DEV_MOTOR_LOAD_POSITION       100  ///< motor position at rack load position
#define RACKTRANSFER_DEV_MOTOR_TRANSFER_POSITION  1190  ///< motor position at rack unload position
#define RACKTRANSFER_DEV_ADAPTER_LOAD_MAX_POS      120  ///< max motor position 'load' for RackAdapterPosition_t assignment
#define RACKTRANSFER_DEV_ADAPTER_TRANSFER_MIN_POS 1170  ///< min motor position 'unload' for RackAdapterPosition_t assignment

/****************************************************************************/
/*!
 *  \brief    Constructor of CRackTransferDevice
 *
 *
 *  \param    pDeviceProcessing = pointer to DeviceProcessing
 *
 *  \return
 *
 ***************************************************************************/
CRackTransferDevice::CRackTransferDevice(DeviceProcessing* pDeviceProcessing, QString Type) :
    CBaseDevice(pDeviceProcessing, Type),
    m_pMotorRacktransfer(0), m_SpeedMotionProfile(0), m_LoadPosition(0), m_TransferPosition(0)
{
    m_MainState    = DEVICE_MAIN_STATE_START;
    m_MainStateOld = m_MainState;
    m_TaskID       = RACKTRANSFER_TASKID_INIT;
    m_ErrorTaskState = RACKTRANSFER_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_RACKTRANSFER;

    FILE_LOG_L(laDEV, llINFO) << " create CRackTransferDevice";

}

/****************************************************************************/
/*!
 *  \brief    Destructor of CRackTransferDevice
 *
 *
 *  \return
 *
 ****************************************************************************/
CRackTransferDevice::~CRackTransferDevice()
{
    m_pDevProc = 0;
    m_pMotorRacktransfer = 0;
}

/****************************************************************************/
/*!
 *  \brief    clean up the error state
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if no error appears
 *
 ****************************************************************************/
ReturnCode_t CRackTransferDevice::ClearErrorState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    //RetVal = m_pTempCtrl->ClearErrorTask();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        //RetVal = m_pMotorOvenCover->ClearErrorTask();
    }

    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        m_TaskID = RACKTRANSFER_TASKID_FREE;

    }

    return RetVal;
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
void CRackTransferDevice::HandleTasks()
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
            m_ErrorTaskState = RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_FCT_MOD_CFG;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_FCT_MOD_CFG)
    {
        RetVal = ConfigureDeviceTasks();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_IDLE;
            m_TaskID = RACKTRANSFER_TASKID_FREE;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   CRackTransferDevice:handleTasks, new state: " << (int) m_MainState;
        m_MainStateOld = m_MainState;
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules to the pointers,
 *
 *
 *  \return  void
 *
 ****************************************************************************/
ReturnCode_t CRackTransferDevice::HandleInitializationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    FILE_LOG_L(laDEV, llINFO) << "  CRackTransferDevice::HandleInitializationState()";
    m_pMotorRacktransfer = (CStepperMotor*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_RackTransferMotorKey));
    if(m_pMotorRacktransfer == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_RACKTRANS_DEV, EVENT_DCL_RACKTFR_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_RACK_TRANSFER);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_RACK_TRANSFER not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    return RetVal;

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
ReturnCode_t CRackTransferDevice::HandleConfigurationState()
{
    if (!connect(m_pMotorRacktransfer, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)), this, SLOT(StepperMotorReferenceMovementAckn(quint32, ReturnCode_t, qint32))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RACKTRANS_DEV, EVENT_DCL_RACKTFR_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_RACK_TRANSFER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportReferenceMovementAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pMotorRacktransfer, SIGNAL(ReportSetStateAckn(quint32, ReturnCode_t)), this, SLOT(StepperMotorSetStateAckn(quint32, ReturnCode_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RACKTRANS_DEV, EVENT_DCL_RACKTFR_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_RACK_TRANSFER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportSetStateAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if (!connect(m_pMotorRacktransfer, SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16)), this, SLOT(StepperMotorMovementAckn(quint32, ReturnCode_t, qint32, qint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RACKTRANS_DEV, EVENT_DCL_RACKTFR_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_RACK_TRANSFER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportMovementAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if (!connect(m_pMotorRacktransfer, SIGNAL(ReportPosition(quint32, ReturnCode_t, qint32, qint8)), this, SLOT(StepperMotorPosition(quint32, ReturnCode_t, qint32))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RACKTRANS_DEV, EVENT_DCL_RACKTFR_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_RACK_TRANSFER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportPosition'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if (!connect(m_pMotorRacktransfer, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(StepperMotorError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RACKTRANS_DEV, EVENT_DCL_RACKTFR_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_RACK_TRANSFER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    // read process settings
    m_SpeedMotionProfile = GetProcSettingPosition("speed_motion_profile");
    m_LoadPosition = GetProcSettingPosition("load_position");
    m_TransferPosition = GetProcSettingPosition("transfer_position");

    return DCL_ERR_FCT_CALL_SUCCESS;

}

/****************************************************************************/
/*!
 *  \brief   Handles the configuration of the device tasks
 *
 *           this code creates the device tasks which will be processed when a
 *           request was passed to this class. Each device tasl contains one or more function
 *           module tasks, which will be created and inserted here as well.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if configuration was successfully executed
 *           otherwise DCL_ERR_FCT_CALL_FAILED
 *
 ****************************************************************************/
ReturnCode_t CRackTransferDevice::ConfigureDeviceTasks()
{
    DeviceTask* pDeviceTask;
    CANStepperMotorTask* pCANStepperMotorTask;
    quint8  TaskIndex = 1;

    //######################################
    // Reference run task list
    // -> create the task list
    // -> create task 'set motor state enabled' (activate the motor driver IC)
    // -> create task 'reference run'

    pDeviceTask = CBaseDevice::GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, RTFR_TASK_REFRUN);
    m_DeviceTaskMap[RTFR_TASK_REFRUN] = pDeviceTask;

    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = m_pMotorRacktransfer;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pCANStepperMotorTask->m_SubCommandData = 1;  // enable motor
    m_DeviceTaskMap[RTFR_TASK_REFRUN]->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    FILE_LOG_L(laDEV, llDEBUG1) << " CRackTransferDevice: add task 'SetState'";

    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = m_pMotorRacktransfer;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    m_DeviceTaskMap[RTFR_TASK_REFRUN]->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    FILE_LOG_L(laDEV, llDEBUG1) << " COvenDevice: add task 'ReferenceRun'";

    //######################################
    // set oven cover position
    // -> motor position command
    pDeviceTask = CBaseDevice::GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, RTFR_TASK_SET_POS);
    m_DeviceTaskMap[RTFR_TASK_SET_POS] = pDeviceTask;

    TaskIndex = 1;
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = m_pMotorRacktransfer;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    m_DeviceTaskMap[RTFR_TASK_SET_POS]->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " COvenDevice: add task 'SetState'";

    //######################################
    // request oven cover position
    // -> request act position command
    pDeviceTask = CBaseDevice::GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, RTFR_TASK_REQ_POS);
    m_DeviceTaskMap[RTFR_TASK_REQ_POS] = pDeviceTask;

    TaskIndex = 1;
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = m_pMotorRacktransfer;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    m_DeviceTaskMap[RTFR_TASK_REQ_POS]->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " CRackTransferDevice: add task 'SetState'";

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
void CRackTransferDevice::HandleIdleState()
{
    if(m_TaskID == RACKTRANSFER_TASKID_FREE)
    {
        ; // there are no pending tasks
    }
    else if(m_TaskID == RACKTRANSFER_TASKID_COMMAND_HDL)
    {
        HandleDeviceTaskActions();
    }
    else if(m_TaskID == RACKTRANSFER_TASKID_ERROR)
    {
        //Just wait for 'ExitErrormode'
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << " Invalid taskID: " << (int) m_TaskID;

        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        SetErrorParameter(EVENT_GRP_DCL_RACKTRANS_DEV, ERROR_DCL_IRACKTFR_DEV_INVALID_STATE, (quint16) m_TaskID);
        m_TaskID = RACKTRANSFER_TASKID_FREE;

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_IFACE;
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the task to execute the device tasks and their fct-module tasks
 *
 *   The function is called from HandleTasks() if m_TaskID == RACKTRANSFER_TASKID_COMMAND_HDL.
 *   The function loops thru the device tasks and processes the tasks pending for execution.
 *
 */
/****************************************************************************/
void CRackTransferDevice::HandleDeviceTaskActions()
{
    bool ActiveDeviceTaskFound = false;
    QMapIterator<quint16, DeviceTask*> iterDeviceTask(m_DeviceTaskMap);
    DeviceTask*         pDeviceTask;
    FunctionModuleTask* pFunctionModuleTask = 0;
    ReturnCode_t RetVal;

    if((m_pDevProc == 0) || (m_pMotorRacktransfer == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_RACKTRANS_DEV, ERROR_DCL_IRACKTFR_DEV_NULL_PTR_ACCESS, (quint16) m_TaskID);

        //forward the error information. The error relevant datas should already have been set.
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_IFACE;

        return;
    }

    //loop thru all device tasks
    while(iterDeviceTask.hasNext())
    {
        pDeviceTask = iterDeviceTask.next().value();
        if(pDeviceTask->m_TaskState == DeviceTask::DEV_TASK_STATE_START)
        {
            pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
        }

        if(pDeviceTask->m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            // execute the device task.
            bool bActionStateInProgress = false;
            bool bActionStateWithErrorDetect = false;

            //for TaskID-control
            ActiveDeviceTaskFound = true;

            //get the funtion module task to start, if any
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(pDeviceTask->m_FctModTaskMap,
                                                                                             bActionStateInProgress,
                                                                                             bActionStateWithErrorDetect);

            while(pFunctionModuleTask)
            {
                if(pFunctionModuleTask)
                {
                    FILE_LOG_L(laDEV, llINFO) << " CRackTransferDevice: task for startup found.";
                    RetVal = m_pDevProc->GetFunctionModuleTaskManager()->StartTask(pFunctionModuleTask);
                    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
                    {
                        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_FAILED;
                        m_TaskID = RACKTRANSFER_TASKID_ERROR;

                        /// \todo ERROR_DCL_RACKTFR_DEV_REQ_REFRUN_FCTCALL_FAILED must be replaced by the ..._FCTCALL_FAILED - code of the corresponding task
                        SetErrorParameter(EVENT_GRP_DCL_RACKTRANS_DEV, ERROR_DCL_RACKTFR_DEV_REQ_REFRUN_FCTCALL_FAILED, (quint16) m_TaskID);

                        FILE_LOG_L(laDEV, llERROR) << " CRackTransferDevice: failed to start task.";
                        return;
                    }
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(pDeviceTask->m_FctModTaskMap,
                                                                                                 bActionStateInProgress,
                                                                                                 bActionStateWithErrorDetect);
            }

            if(bActionStateWithErrorDetect == true)
            {
                FILE_LOG_L(laDEV, llERROR) << " CRackTransferDevice: Action state error.";
                m_TaskID = RACKTRANSFER_TASKID_ERROR;
            }
            else if(bActionStateInProgress == false)
            {
                //there are no more pending function module tasks, finish the processing
                FILE_LOG_L(laDEV, llINFO) << " CRackTransferDevice: positions received, next start movement..";

                pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
            }

            RetVal = m_pDevProc->GetFunctionModuleTaskManager()->CheckTasksTimeOut(pDeviceTask->m_FctModTaskMap, pFunctionModuleTask);
            if(RetVal == DCL_ERR_TIMEOUT)
            {

            }

        }

        if(pDeviceTask->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            //for TaskID-control
            ActiveDeviceTaskFound = true;

            if(pDeviceTask->m_Key == RTFR_TASK_REFRUN)
            {
                pFunctionModuleTask = pDeviceTask->m_FctModTaskMap[1];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
                {
                    RackAdapterPosition_t AdapterPosition = RACK_ADAPTER_POS_UNDEF;

                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                    if(pMotorTask)
                    {
                        AdapterPosition = GetAdapterPosFromMotorPos(pMotorTask->m_ActPos);
                        //m_pIRackTransferDev->RouteReferenceRunAckn(m_instanceID, m_lastErrorHdlInfo, AdapterPosition);
                    }
                    else
                    {
                        //m_pIRackTransferDev->RouteReferenceRunAckn(m_instanceID, DCL_ERR_NULL_PTR_ACCESS, AdapterPosition);
                    }
                }
                else
                {
                    //m_pIRackTransferDev->RouteReferenceRunAckn(m_instanceID, DCL_ERR_NULL_PTR_ACCESS, RACK_ADAPTER_POS_UNDEF);
                }
                //reset the tasks to the 'unused' state
                m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(pDeviceTask->m_FctModTaskMap);
                pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_UNUSED;
            }
            else if(pDeviceTask->m_Key == RTFR_TASK_SET_POS)
            {
                pFunctionModuleTask = pDeviceTask->m_FctModTaskMap[1];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
                {
                    RackAdapterPosition_t AdapterPosition = RACK_ADAPTER_POS_UNDEF;

                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                    if(pMotorTask)
                    {
                        AdapterPosition = GetAdapterPosFromMotorPos(pMotorTask->m_ActPos);
                        //m_pIRackTransferDev->RoutePositionMovementAckn(m_instanceID, m_lastErrorHdlInfo, AdapterPosition);
                    }
                    else
                    {
                        //m_pIRackTransferDev->RoutePositionMovementAckn(m_instanceID, DCL_ERR_NULL_PTR_ACCESS, AdapterPosition);
                    }
                }
                else
                {
                    //m_pIRackTransferDev->RoutePositionMovementAckn(m_instanceID, DCL_ERR_NULL_PTR_ACCESS, RACK_ADAPTER_POS_UNDEF);
                }
                //reset the tasks to the 'unused' state
                m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(pDeviceTask->m_FctModTaskMap);
                pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_UNUSED;
            }
            else if(pDeviceTask->m_Key == RTFR_TASK_REQ_POS)
            {
                RackAdapterPosition_t AdapterPosition = RACK_ADAPTER_POS_UNDEF;

                pFunctionModuleTask = pDeviceTask->m_FctModTaskMap[1];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
                {
                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                    if(pMotorTask)
                    {
                        AdapterPosition = GetAdapterPosFromMotorPos(pMotorTask->m_ActPos);
                    }
                    else
                    {
                        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
                    }
                }
                else
                {
                    m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
                }
                //m_pIRackTransferDev->RouteActPosition(m_instanceID, m_lastErrorHdlInfo, AdapterPosition);

                //reset the tasks to the 'unused' state
                m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(pDeviceTask->m_FctModTaskMap);
                pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_UNUSED;
            }
        }
    }

    if(ActiveDeviceTaskFound == false)
    {
        m_TaskID = RACKTRANSFER_TASKID_FREE;
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
 *     RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_IFACE:
 *      This state reports the error to the interface class. The interface class will change to error state
 *      and blocks future requests
 *
 *     RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_DEVPROC:
 *      This state reports the error to the device processing class.
 *
 *     RACKTRANSFER_DEV_ERRTASK_STATE_IDLE:
 *      Hold the class in error state, do nothing
 *
 *    RACKTRANSFER_DEV_ERRTASK_STATE_RESET
 *      Reset the error state, set all states to it's initialisation state
 *
 *  \return   void
 */
/****************************************************************************/
void CRackTransferDevice::HandleErrorState()
{
    if(m_ErrorTaskState == RACKTRANSFER_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_ErrorTaskState == RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        /*if(m_pIRackTransferDev != 0)
        {
            m_pIRackTransferDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }*/
        m_ErrorTaskState = RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_ErrorTaskState == RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
//        m_ErrorTaskState = RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_DEVPROC;
        m_ErrorTaskState = RACKTRANSFER_DEV_ERRTASK_STATE_RESET;
    }
    else if(m_ErrorTaskState == RACKTRANSFER_DEV_ERRTASK_STATE_RESET)
    {
        m_TaskID = RACKTRANSFER_TASKID_FREE;
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;
        // reset the function module references
        m_pMotorRacktransfer = 0;
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the stepper motors 'set state' acknowledge
 *
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *                      otherwise the return value of the error detecting function
 *  \iparam   State = the motor's state
 *
 ****************************************************************************/
void CRackTransferDevice::StepperMotorSetStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo)
{
    FILE_LOG_L(laDEV, llINFO) << " CRackTransferDevice<" << (int) m_instanceID << "> 0x" << std::hex << InstanceID;

    CANStepperMotorTask* pMotorTask = 0;

    //get the first Motor task from the device task map, with state 'PROGRESS', and taskID 'STATE'
    pMotorTask = GetMotorFctModTaskInProcess(InstanceID, pMotorTask, CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE);
    while(pMotorTask)
    {
        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pMotorTask->m_TaskIndex << " state to finished.";
            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
        }
        else
        {
            FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while SetState.. error";
            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
        }

        //get the next motor task from the device task map, with state 'PROGRESS', and taskID 'STATE', if any
        pMotorTask = GetMotorFctModTaskInProcess(InstanceID, pMotorTask, CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE);
    }
}

/****************************************************************************/
/*!
 *  \brief    Return function module tasks from device task currently in process
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   pMotorTaskPrev = DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *                      otherwise the return value of the error detecting function
 *  \iparam   FctTaskID = motor task id
 *
 *  \return   Function module task currently in process and matching the FctTaskID
 *
 ****************************************************************************/
CANStepperMotorTask* CRackTransferDevice::GetMotorFctModTaskInProcess(quint32 InstanceID,
                                                                      CANStepperMotorTask* pMotorTaskPrev,
                                                                      CANStepperMotorTask::CANStepperMotorTaskID_t FctTaskID)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llINFO) << " CRackTransferDevice<" << (int) m_instanceID << "> 0x" << std::hex << InstanceID;

    CANStepperMotorTask* pMotorTask = 0;
    CANStepperMotorTask* pMotorTaskRet = 0;

    //loop thru all device tasks
    QMapIterator<quint16, DeviceTask*> iterDeviceTask(m_DeviceTaskMap);
    DeviceTask*         pDeviceTask;

    while(iterDeviceTask.hasNext())
    {
        pDeviceTask = iterDeviceTask.next().value();

        if(pDeviceTask->m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            pFunctionModuleTask = 0;
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(pDeviceTask->m_FctModTaskMap, InstanceID, pMotorTaskPrev);
            while(pFunctionModuleTask)
            {
                FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState;
                Q_ASSERT(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR);
                pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                if(pMotorTask)
                {
                    if((pMotorTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS) &&
                       (pMotorTask->m_MotorTaskID == FctTaskID))
                    {
                        pMotorTaskRet = pMotorTask;
                        break;
                    }
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(pDeviceTask->m_FctModTaskMap, InstanceID, pMotorTask);
            }
        }
    }
    return pMotorTaskRet;

}

/****************************************************************************/
/*!
 *  \brief    Handles the stepper motors reference run acknowledge
 *
 *            The internal state machine will be reset, and the acknowledge information is forwarded to
 *            the IRackTransferDevice class
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *                      otherwise the return value of the error detecting function
 *  \iparam   Position = the motor's actual position
 *
 *  \return  void
 *
 ****************************************************************************/
void CRackTransferDevice::StepperMotorReferenceMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position)
{
    FILE_LOG_L(laDEV, llINFO) << " CRackTransferDevice<" << (int) m_instanceID << "> 0x" << std::hex << InstanceID;

    CANStepperMotorTask* pMotorTask = 0;

    //get the first Motor task from the device task map, with state 'PROGRESS', and taskID 'STATE'
    pMotorTask = GetMotorFctModTaskInProcess(InstanceID, pMotorTask, CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    while(pMotorTask)
    {
        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pMotorTask->m_TaskIndex << " state to finished.";
            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
        }
        else
        {
            FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while SetState.. error";
            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
        }
        //The reported state is inserted into sub command data...
        pMotorTask->m_ActPos = (Position_t) Position;

        //get the next motor task from the device task map, with state 'PROGRESS', and taskID 'STATE', if any
        pMotorTask = GetMotorFctModTaskInProcess(InstanceID, pMotorTask, CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE);
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the stepper motor's movement acknowledge
 *
 *            The internal state machine will be reset, and the acknowledge information is forwarded to
 *            the IRackTransferDevice class
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the movement was successfully done,
 *                      otherwise the return value of the error detecting function
 *  \iparam   Position = the motor's actual position
 *  \iparam   Speed = the motor's actual speed
 *
 *  \return  void
 *
 ****************************************************************************/
void CRackTransferDevice::StepperMotorMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position, qint16 Speed)
{
    Q_UNUSED(Speed);
    FILE_LOG_L(laDEV, llINFO) << " CRackTransferDevice<" << (int) m_instanceID << "> 0x" << std::hex << InstanceID;

    CANStepperMotorTask* pMotorTask = 0;

    //get the first Motor task from the device task map, with state 'PROGRESS', and taskID 'STATE'
    pMotorTask = GetMotorFctModTaskInProcess(InstanceID, pMotorTask, CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    while(pMotorTask)
    {
        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pMotorTask->m_TaskIndex << " state to finished.";
            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
        }
        else
        {
            FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while SetState.. error";
            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
        }
        //The reported state is inserted into sub command data...
        pMotorTask->m_ActPos = (Position_t) Position;

        //get the next motor task from the device task map, with state 'PROGRESS', and taskID 'STATE', if any
        pMotorTask = GetMotorFctModTaskInProcess(InstanceID, pMotorTask, CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    }

}

/****************************************************************************/
/*!
 *  \brief    Handles the stepper motor's actual position callback
 *
 *            The internal state machine will be reset, and the position information is forwarded to
 *            the IRackTransferDevice class
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the position was received,
 *                      otherwise the return value of the error detecting function
 *  \iparam   Position = the motor's actual position
 *
 *  \return  void
 *
 ****************************************************************************/
void CRackTransferDevice::StepperMotorPosition(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position)
{
    FILE_LOG_L(laDEV, llINFO) << " CRackTransferDevice<" << (int) m_instanceID << "> 0x" << std::hex << InstanceID;

    CANStepperMotorTask* pMotorTask = 0;

    //get the first Motor task from the device task map, with state 'PROGRESS', and taskID 'STATE'
    pMotorTask = GetMotorFctModTaskInProcess(InstanceID, pMotorTask, CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    while(pMotorTask)
    {
        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pMotorTask->m_TaskIndex << " state to finished.";
            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
        }
        else
        {
            FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while SetState.. error";
            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
        }
        //The reported state is inserted into sub command data...
        pMotorTask->m_ActPos = (Position_t) Position;

        //get the next motor task from the device task map, with state 'PROGRESS', and taskID 'STATE', if any
        pMotorTask = GetMotorFctModTaskInProcess(InstanceID, pMotorTask, CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
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
void CRackTransferDevice::StepperMotorError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_ErrorTaskState = RACKTRANSFER_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief   Request the reference run of the adapter motor
 *
 *           The function starts an asynchronous request to execute the reference run of the motor.
 *           The request will be accepted if no other request (task) is currently active.
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANStepperMotor class.
 *           When finished, the slot StepperMotorReferenceMovementAckn() will be called by
 *           CANStepperMotor, and forwards the acknowledge of this request to the interface class
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t CRackTransferDevice::ReqReferenceRun()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(RTFR_TASK_REFRUN))
    {
        FILE_LOG_L(laDEV, llINFO) << " CRackTransferDevice";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CRackTransferDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Set the adapter position, start movement
 *
 *           The function starts an asynchronous request to start a motor movement.
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANStepperMotor class.
 *           When finished, the slot StepperMotorMovementAckn() will be called by
 *           CANStepperMotor, and forwards the acknowledge of this request to the interface class
 *
 *  \iparam  AdapterPosition = Requested adapter position
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t CRackTransferDevice::ReqAdapterMoveToPosition(RackAdapterPosition_t AdapterPosition)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(RTFR_TASK_SET_POS))
    {
        FunctionModuleTask* pFunctionModuleTask = 0;
        pFunctionModuleTask = m_DeviceTaskMap[RTFR_TASK_SET_POS]->m_FctModTaskMap[1];

        CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pMotorTask)
        {
            pMotorTask->m_TargetPos = GetMotorPosFromAdapterPos(AdapterPosition);
            pMotorTask->m_Profile = m_SpeedMotionProfile;
            FILE_LOG_L(laDEV, llINFO) << " CRackTransferDevice";
        }
        else
        {
            RetVal = DCL_ERR_NULL_PTR_ACCESS;
            FILE_LOG_L(laDEV, llERROR) << " CRackTransferDevice: DCL_ERR_NULL_PTR_ACCESS";
        }
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CRackTransferDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Request the drawers actual position
 *
 *           The function starts an asynchronous request of the adapter position.
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANStepperMotor class.
 *           When finished, the slot StepperMotorActPosition() will be called by
 *           CANStepperMotor, and forwards the acknowledge of this request to the interface class
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t CRackTransferDevice::ReqActAdapterPosition()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(RTFR_TASK_REQ_POS))
    {
        FILE_LOG_L(laDEV, llINFO) << " CRackTransferDevice";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CRackTransferDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;

}

/****************************************************************************/
/*!
 *  \brief   Convert motor position [steps] to adapter position type
 *
 *  \iparam  motorPos = Motor position to convert
 *
 *  \return  RackAdapterPosition_t the adapter position as converted from motorPos
 *
 ****************************************************************************/
RackAdapterPosition_t CRackTransferDevice::GetAdapterPosFromMotorPos(qint32 motorPos)
{
    RackAdapterPosition_t adapterPosition  = RACK_ADAPTER_POS_UNDEF;

    if(motorPos <= RACKTRANSFER_DEV_ADAPTER_LOAD_MAX_POS)
    {
        adapterPosition = RACK_ADAPTER_POS_LOAD;
    }
    else if(motorPos >= RACKTRANSFER_DEV_ADAPTER_TRANSFER_MIN_POS)
    {
        adapterPosition = RACK_ADAPTER_POS_TRANSFER;
    }

    return adapterPosition;
}

/****************************************************************************/
/*!
 *  \brief   Convert adapter position type to motor position [steps]
 *
 *  \iparam  adapterPos = adapter position
 *
 *  \return  qint32 The motor position in steps at the adapter position
 *
 ****************************************************************************/

qint32 CRackTransferDevice::GetMotorPosFromAdapterPos(RackAdapterPosition_t adapterPos)
{
    Position_t motorPosition  = RACKTRANSFER_DEV_MOTOR_LOAD_POSITION;  // default

    if(adapterPos == RACK_ADAPTER_POS_LOAD)
    {
//        motorPosition = RACKTRANSFER_DEV_MOTOR_LOAD_POSITION;
        motorPosition = m_LoadPosition;
    }
    else if(adapterPos == RACK_ADAPTER_POS_TRANSFER)
    {
//        motorPosition = RACKTRANSFER_DEV_MOTOR_TRANSFER_POSITION;
        motorPosition = m_TransferPosition;
    }

    return motorPosition;
}

/****************************************************************************/
/*!
 *  \brief   Helper function, activates the device task and it's fct-mdoule tasks
 *
 *  \iparam   DeviceTaskIndex = Device's task index within the map
 *
 *  \return   true, if the dask was activated, otherwise false
 *
 ****************************************************************************/
bool CRackTransferDevice::ActivateDeviceTask(quint8 DeviceTaskIndex)
{
    bool Activated = false;
    FunctionModuleTask* pFunctionModuleTask;

    if((m_TaskID == RACKTRANSFER_TASKID_FREE) || (m_TaskID == RACKTRANSFER_TASKID_COMMAND_HDL))
    {
        if(m_DeviceTaskMap[DeviceTaskIndex]->m_TaskState == DeviceTask::DEV_TASK_STATE_UNUSED)
        {
            QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(m_DeviceTaskMap[DeviceTaskIndex]->m_FctModTaskMap);

            //st the device task's function module tasks to 'Start'-state
            while (iterFctModTask.hasNext())
            {
                pFunctionModuleTask = iterFctModTask.next().value();
                if(pFunctionModuleTask)
                {
                    pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
                }
            }
            m_DeviceTaskMap[DeviceTaskIndex]->m_TaskState = DeviceTask::DEV_TASK_STATE_START;
            m_TaskID = RACKTRANSFER_TASKID_COMMAND_HDL;
            Activated = true;
        }
    }

    return Activated;

}


} //namespace

