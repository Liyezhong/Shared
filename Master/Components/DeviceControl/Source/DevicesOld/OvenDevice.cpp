/******************************************************************/
/*! \file OvenDevice.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class COvenDevice
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

#include "DeviceControl/Include/Devices/OvenDevice.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

#define OVEN_MOTORPOS_CLOSED 100    //!< stepper motor position if cover closed
#define OVEN_MOTORPOS_OPENED 1200   //!< stepper motor position if cover opened

//######### Timeout defines #########
#define OVEN_COVER_TIMEOUT_REFRUN_REQ 5000      //!< cover reference run
#define OVEN_COVER_TIMEOUT_POS_REQ 5000         //!< cover positioning
#define OVEN_COVER_TIMEOUT_ACTPOS_REQ 5000      //!< cover act. position reqest
#define OVEN_COVER_TIMEOUT_ACTTEMP_REQ 750      //!< temperature
#define OVEN_COVER_TIMEOUT_ACTSTATUS_REQ 750    //!< temp. ctrl. status

/****************************************************************************/
/*!
 *  \brief  Constructor of COvenDevice
 *
 *  \iparam pDeviceProcessing = Pointer to the DeviceProcessing instance
 *  \iparam Type = Device type string
 */
/****************************************************************************/
COvenDevice::COvenDevice(DeviceProcessing* pDeviceProcessing, QString Type) : CBaseDevice(pDeviceProcessing, Type),
    m_pTempCtrl(0), m_pMotorOvenCover(0), m_CoverOpenSpeedMotionProfile(0), m_CoverCloseSpeedMotionProfile(0),
    m_CoverClosedPosition(0), m_CoverOpenPosition(0)
{
    m_MainState      = DEVICE_MAIN_STATE_START;
    m_MainStateOld   = m_MainState;
    m_TaskID         = OVENDEV_TASKID_INIT;
    m_ErrorTaskState   = OVEN_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_OVEN;

    FILE_LOG_L(laDEV, llINFO) << " oven device created";
}

/****************************************************************************/
/*!
 *  \brief  Destructor of COvenDevice
 */
/****************************************************************************/
COvenDevice::~COvenDevice()
{
    m_pDevProc = 0;
    m_pTempCtrl = 0;
    m_pMotorOvenCover = 0;
}

/****************************************************************************/
/*!
 *  \brief  Clean up the error state
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if no error appears
 */
/****************************************************************************/
ReturnCode_t COvenDevice::ClearErrorState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    //RetVal = m_pTempCtrl->ClearErrorTask();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        //RetVal = m_pMotorOvenCover->ClearErrorTask();
    }

    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        m_TaskID = OVENDEV_TASKID_FREE;

    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Handles the internal state machine
 *
 *      This function should be called periodically, it handles the state
 *      machine of the class. Furthermore, the HandleTask-function of the
 *      interface class will be called. Each main state has a individual
 *      handling function, which will be called according to the currently
 *      active main state.
 */
/****************************************************************************/
void COvenDevice::HandleTasks()
{
    ReturnCode_t RetVal;

    if(m_MainState == DEVICE_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(m_MainState == DEVICE_MAIN_STATE_START)
    {
        //fall through
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
            m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_FCT_MOD_CFG;
            m_TaskID = OVENDEV_TASKID_FREE;
            /// \todo maybe we need a state to ensure the reference run call!!
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_REPORT_IFACE;
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
            m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   COvenDevice:handleTasks, new state: " << (int) m_MainState;
        m_MainStateOld = m_MainState;
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules pointer variables
 *           m_pTempCtrl - Oven temperature control
 *           m_pMotorOvenCover - Motor to move the oven cover
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS or error return code
 *
 ****************************************************************************/
ReturnCode_t COvenDevice::HandleInitializationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    FILE_LOG_L(laDEV, llINFO) << "  COvenDevice::HandleInitializationState()";

    m_pTempCtrl = (CTemperatureControl*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_OvenTempCtrlKey));
    if(m_pTempCtrl == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_OVEN);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_TEMPCTRL_OVEN not allocated.";
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    m_pMotorOvenCover = (CStepperMotor*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_OvenCoverMotorKey));
    if(m_pMotorOvenCover == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_OVEN_COVER);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_OVEN_COVER not allocated.";
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
ReturnCode_t COvenDevice::HandleConfigurationState()
{
    if(!connect(m_pMotorOvenCover, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)),
                this, SLOT(StepperMotorError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_OVEN_COVER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pMotorOvenCover, SIGNAL(ReportSetStateAckn(quint32, ReturnCode_t)),
                this, SLOT(StepperMotorSetStateAckn(quint32, ReturnCode_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_OVEN_COVER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportSetStateAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pMotorOvenCover, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)),
                this, SLOT(StepperMotorReferenceMovementAckn(quint32, ReturnCode_t, qint32))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_OVEN_COVER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportReferenceMovementAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pMotorOvenCover, SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16)),
                this, SLOT(StepperMotorMovementAckn(quint32, ReturnCode_t, qint32, qint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_OVEN_COVER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportMovementAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pMotorOvenCover, SIGNAL(ReportPosition(quint32, ReturnCode_t, qint32, qint8)),
                this, SLOT(StepperMotorActPosition(quint32, ReturnCode_t, qint32))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_OVEN_COVER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportPosition'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrl, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)),
                this, SLOT(TempCtrlError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_OVEN);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrl, SIGNAL(ReportActTemperature(quint32, ReturnCode_t, quint8, qreal)),
                this, SLOT(TempCtrlActTemperature(quint32, ReturnCode_t, quint8, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_OVEN);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportActTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrl, SIGNAL(ReportRefTemperature(quint32, ReturnCode_t, qreal)),
                this, SLOT(ReportRefTemperature(quint32, ReturnCode_t, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_OVEN);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportSetTemperatureAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrl, SIGNAL(ReportActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t)),
                this, SLOT(TempCtrlActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_OVEN);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportActStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrl, SIGNAL(ReportSetStatusAckn(quint32, ReturnCode_t, TempCtrlStatus_t)),
                this, SLOT(TempCtrlSetStatusAckn(quint32, ReturnCode_t, TempCtrlStatus_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_OVEN);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportSetStatusAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrl, SIGNAL(ReportActOperatingMode(quint32, ReturnCode_t, TempCtrlOperatingMode_t)),
                this, SLOT(TempCtrlActOperatingMode(quint32, ReturnCode_t, TempCtrlOperatingMode_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_OVEN);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportActOperatingMode'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrl, SIGNAL(ReportSetOperatingModeAckn(quint32, ReturnCode_t, TempCtrlOperatingMode_t)),
                this, SLOT(TempCtrlSetOperatingModeAckn(quint32, ReturnCode_t, TempCtrlOperatingMode_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_TEMPCTRL_OVEN);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temp. control signal 'ReportSetOperatingModeAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    // read process settings
    m_CoverOpenSpeedMotionProfile = GetProcSettingPosition("cover_open_speed_motion_profile");
    m_CoverCloseSpeedMotionProfile = GetProcSettingPosition("cover_close_speed_motion_profile");
    m_CoverClosedPosition = GetProcSettingPosition("cover_closed_position");
    m_CoverOpenPosition = GetProcSettingPosition("cover_open_position");

    return DCL_ERR_FCT_CALL_SUCCESS;

}

/****************************************************************************/
/*!
 *  \brief   Create and configure the device tasks
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t COvenDevice::ConfigureDeviceTasks()
{
    CANStepperMotorTask*    pCANStepperMotorTask;
    CANTemperatureCtrlTask* pCANTemperatureCtrlTask;
    quint8  TaskIndex = 1;

    //######################################
    // Reference run task list
    // -> Set motor state 'enabled' (activate the motor driver IC)
    // -> Proceed reference run
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = m_pMotorOvenCover;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pCANStepperMotorTask->m_SubCommandData = 1;  // enable motor
    m_DeviceTask[OVEN_DEV_TASK_REFRUN].m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    FILE_LOG_L(laDEV, llDEBUG1) << " COvenDevice: add task 'SetState'";

    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = m_pMotorOvenCover;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    m_DeviceTask[OVEN_DEV_TASK_REFRUN].m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    FILE_LOG_L(laDEV, llDEBUG1) << " COvenDevice: add task 'ReferenceRun'";

    //######################################
    // set oven cover position
    // -> motor position command
    TaskIndex = 1;
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = m_pMotorOvenCover;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    m_DeviceTask[OVEN_DEV_TASK_COVER_POS_SET].m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " COvenDevice: add task 'SetState'";

    //######################################
    // request oven cover position
    // -> request act position command
    TaskIndex = 1;
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = m_pMotorOvenCover;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    m_DeviceTask[OVEN_DEV_TASK_COVER_POS_REQ].m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " COvenDevice: add task 'MovePos'";

    //######################################
    // Set oven temperature
    // -> set temperature
    TaskIndex = 1;
    pCANTemperatureCtrlTask = CBaseDevice::GetNewCANTempCtrlTask(CANTemperatureCtrlTask::FCTMOD_TEMP_TASK_ID_SET_TEMP);
    pCANTemperatureCtrlTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANTemperatureCtrlTask->m_TaskIndex = TaskIndex;
    pCANTemperatureCtrlTask->m_TriggerTaskIndex = TaskIndex;
    pCANTemperatureCtrlTask->m_pCANObject = m_pTempCtrl;
    pCANTemperatureCtrlTask->m_Temperatur = 0;
    pCANTemperatureCtrlTask->m_Timeout = 5000;
    m_DeviceTask[OVEN_DEV_TASK_TEMP_SET].m_FctModTaskMap[TaskIndex] = pCANTemperatureCtrlTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " COvenDevice: add task 'ReqPos'";

    //######################################
    // request actual oven temperature
    // -> request temperature
    TaskIndex = 1;
    pCANTemperatureCtrlTask = CBaseDevice::GetNewCANTempCtrlTask(CANTemperatureCtrlTask::FCTMOD_TEMP_TASK_ID_REQ_TEMP);
    pCANTemperatureCtrlTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANTemperatureCtrlTask->m_TaskIndex = TaskIndex;
    pCANTemperatureCtrlTask->m_TriggerTaskIndex = TaskIndex;
    pCANTemperatureCtrlTask->m_pCANObject = m_pTempCtrl;
    pCANTemperatureCtrlTask->m_Temperatur = 0;
    pCANTemperatureCtrlTask->m_Timeout = 5000;
    m_DeviceTask[OVEN_DEV_TASK_TEMP_REQ].m_FctModTaskMap[TaskIndex] = pCANTemperatureCtrlTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " COvenDevice: add task 'SetState'";

    //######################################
    // Set temperature control state
    // -> set temperature
    TaskIndex = 1;
    pCANTemperatureCtrlTask = CBaseDevice::GetNewCANTempCtrlTask(CANTemperatureCtrlTask::FCTMOD_TEMP_TASK_ID_SET_STATE);
    pCANTemperatureCtrlTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANTemperatureCtrlTask->m_TaskIndex = TaskIndex;
    pCANTemperatureCtrlTask->m_TriggerTaskIndex = TaskIndex;
    pCANTemperatureCtrlTask->m_pCANObject = m_pTempCtrl;
    pCANTemperatureCtrlTask->m_State = TEMPCTRL_STATUS_UNDEF;
    pCANTemperatureCtrlTask->m_Timeout = 5000;
    m_DeviceTask[OVEN_DEV_TASK_TEMP_STATUS_SET].m_FctModTaskMap[TaskIndex] = pCANTemperatureCtrlTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " COvenDevice: add task 'SetState'";

    //######################################
    // request actual temperature ctrl. state
    // -> request temperature
    TaskIndex = 1;
    pCANTemperatureCtrlTask = CBaseDevice::GetNewCANTempCtrlTask(CANTemperatureCtrlTask::FCTMOD_TEMP_TASK_ID_REQ_STATE);
    pCANTemperatureCtrlTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANTemperatureCtrlTask->m_TaskIndex = TaskIndex;
    pCANTemperatureCtrlTask->m_TriggerTaskIndex = TaskIndex;
    pCANTemperatureCtrlTask->m_pCANObject = m_pTempCtrl;
    pCANTemperatureCtrlTask->m_State = TEMPCTRL_STATUS_UNDEF;
    pCANTemperatureCtrlTask->m_Timeout = 5000;
    m_DeviceTask[OVEN_DEV_TASK_TEMP_STATUS_REQ].m_FctModTaskMap[TaskIndex] = pCANTemperatureCtrlTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " COvenDevice: add task 'SetState'";

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief    Internal function for idle state machine processing
 *
 *   The function handles the idle state, which is active if the class is 'ready for tasks'
 *   Depending on the pending task, which is stored in m_TaskID, the task specific handling
 *   function will be called.
 *
 *  \return   void
 */
/****************************************************************************/
void COvenDevice::HandleIdleState()
{
    if(m_TaskID == OVENDEV_TASKID_FREE)
    {
        ; // there are no pending tasks
    }
    else if(m_TaskID == OVENDEV_TASKID_COMMAND_HDL)
    {
        //HandleCommandRequestTask();
        HandleDeviceTaskActions();
    }
    else if(m_TaskID == OVENDEV_TASKID_ERROR)
    {
        //Just wait for 'ExitErrormode'
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << " Invalid taskID: " << (int) m_TaskID;

        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_INVALID_STATE, (quint16) m_TaskID);

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_REPORT_IFACE;
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the task to execute the device tasks and their fct-module tasks
 *
 *   The function is called from HandleTasks() if m_TaskID == OVENDEV_TASKID_COMMAND_HDL.
 *   The function loops thru the device tasks and processes the tasks pending for execution.
 *
 */
/****************************************************************************/
void COvenDevice::HandleDeviceTaskActions()
{
    if((m_pDevProc == 0) || (m_pTempCtrl == 0) || (m_pMotorOvenCover == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_NULL_PTR_ACCESS, (quint16) m_TaskID);

        //forward the error information. The error relevant datas should already have been set.
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_REPORT_IFACE;

        return;
    }

    for (quint8 idx = 0; idx < OVEN_DEVICE_TASK_COUNT; idx++)
    {
        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_START)
        {
            m_DeviceTask[idx].m_TaskState =DeviceTask:: DEV_TASK_STATE_PROGRESS;
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

                    FILE_LOG_L(laDEV, llINFO) << "  COvenDevice: task for startup found.";
                    RetVal = m_pDevProc->GetFunctionModuleTaskManager()->StartTask(pFunctionModuleTask);
                    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
                    {
                        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_FAILED;
                        m_TaskID = OVENDEV_TASKID_ERROR;
                        SetErrorParameter(EVENT_GRP_DCL_OVEN_DEV, ERROR_DCL_OVEN_DEV_REQ_COVER_MOVE_FCTCALL_FAILED, (quint16) m_TaskID);

                        FILE_LOG_L(laDEV, llERROR) << "  COvenDevice: failed to start task.";
                        return;
                    }
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(m_DeviceTask[idx].m_FctModTaskMap,
                                                                                                 bActionStateInProgress,
                                                                                                 bActionStateWithErrorDetect);
            }

            if(bActionStateWithErrorDetect == true)
            {
                FILE_LOG_L(laDEV, llERROR) << "  COvenDevice: Action state error.";
                m_TaskID = OVENDEV_TASKID_ERROR;
            }
            else if(bActionStateInProgress == false)
            {
                FILE_LOG_L(laDEV, llINFO) << "  COvenDevice: positions received, next start movement..";

                //reset the tasks to the 'unused' state
                //m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(m_DeviceTask[idx].m_FctModTaskMap);
                m_DeviceTask[idx].m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
            }
        }

        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            if(idx == OVEN_DEV_TASK_REFRUN)
            {
                FunctionModuleTask* pFunctionModuleTask = 0;
                OvenCoverPosition_t OvenCoverPosition = OVEN_COVER_UNDEF;

                pFunctionModuleTask = m_DeviceTask[idx].m_FctModTaskMap[1];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
                {
                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                    if(pMotorTask)
                    {
                        OvenCoverPosition = GetOvencoverPositionFromMotorPos(pMotorTask->m_ActPos);
                    }
                }
                //m_pIOvenDev->RouteReferenceRunAckn(m_instanceID, m_lastErrorHdlInfo, OvenCoverPosition);
            }
            else if(idx == OVEN_DEV_TASK_COVER_POS_SET)
            {
                FunctionModuleTask* pFunctionModuleTask = 0;
                OvenCoverPosition_t OvenCoverPosition = OVEN_COVER_UNDEF;

                pFunctionModuleTask = m_DeviceTask[idx].m_FctModTaskMap[1];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
                {
                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                    if(pMotorTask)
                    {
                        OvenCoverPosition = GetOvencoverPositionFromMotorPos(pMotorTask->m_ActPos);
                    }
                }
                //m_pIOvenDev->RouteOvenCoverPositionAckn(m_instanceID, m_lastErrorHdlInfo, OvenCoverPosition);
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
 *   The function is called from HandleIdleState() if m_MainState == LOADER_DEV_MAIN_STATE_ERROR.
 *   This task consists of the following states:
 *
 *     OVEN_DEV_ERRTASK_STATE_REPORT_IFACE:
 *      This state reports the error to the interface class. The interface class will change to error state
 *      and blocks future requests
 *
 *     OVEN_DEV_ERRTASK_STATE_REPORT_DEVPROC:
 *      This state reports the error to the device processing class.
 *
 *     OVEN_DEV_ERRTASK_STATE_IDLE:
 *      Hold the class in error state
 *
 *     OVEN_DEV_ERRTASK_STATE_RESET
 *      Reset the error state, set all states to it's initialisation state
 *
 *  \return   void
 */
/****************************************************************************/
void COvenDevice::HandleErrorState()
{
    if(m_ErrorTaskState == OVEN_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_ErrorTaskState == OVEN_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        /*if(m_pIOvenDev != 0)
        {
            m_pIOvenDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }*/
        m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_ErrorTaskState == OVEN_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_ErrorTaskState == OVEN_DEV_ERRTASK_STATE_RESET)
    {
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;
        // reset the function module references
        m_pTempCtrl = 0;
        m_pMotorOvenCover = 0;
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
void COvenDevice::StepperMotorError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief  Handles the stepper motors 'set state' acknowledge
 *
 *  \iparam InstanceID = instance identifier of the function module
 *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *                    otherwise the return value of the error detecting function
 *
 ****************************************************************************/
void COvenDevice::StepperMotorSetStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llINFO) << "  COvenDevice<" << (int) m_instanceID << ">StepperMotorSetStateAckn 0x" << std::hex << InstanceID;

    //pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_taskList, instanceID, pFunctionModuleTask);
    for(quint8 idx = 0; idx < OVEN_DEVICE_TASK_COUNT; idx++)
    {
        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            pFunctionModuleTask = 0;
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);

            while(pFunctionModuleTask)
            {
                if(pFunctionModuleTask)
                {
                    FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState;
                    Q_ASSERT(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR);

                    if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
                    {
                        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
                        {
                            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished.";
                            pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
                        }
                        else
                        {
                            // error was detected from motor side
                            FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while SetState.. error";
                            pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
                        }

                    }
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }


    /*if((m_pMotorOvenCover) && (m_pMotorOvenCover->GetModuleHandle() == InstanceID))
    {
        if(m_TaskID == OVENDEV_TASKID_COMMAND_HDL)
        {
            ResetModuleCommand(OVENDEV_CMD_COVER_REFRUN);
        }
    }*/
}

/****************************************************************************/
/*!
 *  \brief    Handles the stepper motors reference run acknowledge
 *
 *            The internal state machine will be reset, and the acknowledge information is forwarded to
 *            the IOvenDevice class
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *                      otherwise the return value of the error detecting function
 *  \iparam   Position = the motor's actual position
 *
 *  \return  void
 *
 ****************************************************************************/
void COvenDevice::StepperMotorReferenceMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position)
{
    FunctionModuleTask* pFunctionModuleTask = 0;
    Q_UNUSED(Position);

    FILE_LOG_L(laDEV, llINFO) << " COvenDevice<" << (int) m_instanceID << "> StepperMotor 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < OVEN_DEVICE_TASK_COUNT; idx++)
    {
        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            pFunctionModuleTask = 0;
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);

            while(pFunctionModuleTask)
            {
                if(pFunctionModuleTask)
                {
                    FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState;
                    Q_ASSERT(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR);

                    if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
                    {
                        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
                        {
                            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished.";
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
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the stepper motors position movement acknowledge
 *
 *            The internal state machine will be reset, and the acknowledge information is forwarded to
 *            the IOvenDevice class
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the movement was successfully done,
 *                      otherwise the return value of the error detecting function
 *  \iparam   Position = the motor's actual position
 *  \iparam   Speed = actual motor speed
 *
 *  \return  void
 *
 ****************************************************************************/
void COvenDevice::StepperMotorMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position, qint16 Speed)
{
    Q_UNUSED(Speed);

    FunctionModuleTask* pFunctionModuleTask = 0;
    Q_UNUSED(Position);

    FILE_LOG_L(laDEV, llINFO) << " COvenDevice<" << (int) m_instanceID << "> StepperMotor 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < OVEN_DEVICE_TASK_COUNT; idx++)
    {
        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            pFunctionModuleTask = 0;
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);

            while(pFunctionModuleTask)
            {
                if(pFunctionModuleTask)
                {
                    FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState;
                    Q_ASSERT(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR);

                    if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
                    {
                        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
                        {
                            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished.";
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
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the stepper motor's actual position callback
 *
 *            The internal state machine will be reset, and the position information is forwarded to
 *            the IOvenDevice class
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the position was received,
 *                      otherwise the return value of the error detecting function
 *  \iparam   Position = the motor's actual position
 *
 *  \return  void
 *
 ****************************************************************************/
void COvenDevice::StepperMotorActPosition(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llDEBUG) << " COvenDevice<" << (int) m_instanceID << "> StepperMotor 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < OVEN_DEVICE_TASK_COUNT; idx++)
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
                        FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while Req. ActPosition.. error";
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
 *  \brief    Handles the temperature control's error message
 *
 *            Activate error main state to forward the error information to
 *            the IOvenDevice and DeviceProcessing class
 *
 *  \iparam   InstanceID = instance identifier of the device
 *  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_TEMP_SLV
 *  \iparam   ErrorCode  = from CAN-message
 *  \iparam   ErrorData  = from CAN-message
 *  \iparam   ErrorTime  = error message reception time on master side
 *
 ****************************************************************************/
void COvenDevice::TempCtrlError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_ErrorTaskState = OVEN_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief    Reference temperature notification (acknowledge)
 *
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the temp. control op. mode  was received,
 *                      otherwise the return value of the error detecting function
 *  \iparam   Temperature = the temperature as set to the oven's temp. control
 *
 ****************************************************************************/
void COvenDevice::ReportRefTemperature(quint32 InstanceID, ReturnCode_t HdlInfo, qreal Temperature)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llDEBUG) << " COvenDevice<" << (int) m_instanceID << "> TempCtrl 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < OVEN_DEVICE_TASK_COUNT; idx++)
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
                        // error was detected from function module side
                        FILE_LOG_L(laDEV, llERROR) << "  while req. RefTemperature.. error";
                        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
                    }

                    CANTemperatureCtrlTask* pTempCtrlTask = dynamic_cast<CANTemperatureCtrlTask*>(pFunctionModuleTask);
                    if(pTempCtrlTask)
                    {
                        pTempCtrlTask->m_Temperatur = Temperature;
                    }

                    /*if (m_pIOvenDev != NULL)
                    {
                        m_pIOvenDev->RouteSetTemperature(m_instanceID, m_lastErrorHdlInfo, Temperature);
                    }*/
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the temperature control temperature information signal
 *
 *            The internal state machine will be reset, and the temperature information is
 *            forwarded to the IOvenDevice class
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the temperature was received,
 *                      otherwise the return value of the error detecting function
 *  \iparam   OvenTemperature = the oven's actual position
 *
 *  \return  void
 *
 ****************************************************************************/
void COvenDevice::TempCtrlActTemperature(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Index, qreal OvenTemperature)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llDEBUG) << " COvenDevice<" << (int) m_instanceID << "> TempCtrl 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < OVEN_DEVICE_TASK_COUNT; idx++)
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
                        // error was detected from function module side
                        FILE_LOG_L(laDEV, llERROR) << "  while req. RefTemperature.. error";
                        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
                    }

                    CANTemperatureCtrlTask* pTempCtrlTask = dynamic_cast<CANTemperatureCtrlTask*>(pFunctionModuleTask);
                    if(pTempCtrlTask)
                    {
                        pTempCtrlTask->m_Temperatur = OvenTemperature;
                    }

                    /*if (m_pIOvenDev != NULL)
                    {
                        m_pIOvenDev->RouteActTemperature(m_instanceID, m_lastErrorHdlInfo, OvenTemperature);
                    }*/
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the temperature control status information signal
 *
 *      The internal state machine will be reset, and the status of the temperature
 *      control is forwarded to the IOvenDevice class
 *
 *  \iparam InstanceID = instance identifier of the function module
 *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the temp. control status  was received,
 *                    otherwise the return value of the error detecting function
 *  \iparam TempCtrlStatus = the status of then oven's temp. control
 *
 *  \return void
 *
 ****************************************************************************/
void COvenDevice::TempCtrlActStatus(quint32 InstanceID, ReturnCode_t HdlInfo, TempCtrlStatus_t TempCtrlStatus, TempCtrlMainsVoltage_t)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llDEBUG) << " COvenDevice<" << (int) m_instanceID << "> TempCtrl 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < OVEN_DEVICE_TASK_COUNT; idx++)
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
                        // error was detected from function module side
                        FILE_LOG_L(laDEV, llERROR) << "  while ActStatus.. error";
                        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
                    }

                    CANTemperatureCtrlTask* pTempCtrlTask = dynamic_cast<CANTemperatureCtrlTask*>(pFunctionModuleTask);
                    if(pTempCtrlTask)
                    {
                        pTempCtrlTask->m_State = TempCtrlStatus;
                    }

                    /*if (m_pIOvenDev != NULL)
                    {
                        m_pIOvenDev->RouteActOvenStatus(m_instanceID, m_lastErrorHdlInfo, TempCtrlStatus);
                    }*/
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Set temperature control status acknowledge notification
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the temp. control op. mode  was received,
 *                      otherwise the return value of the error detecting function
 *  \iparam   TempCtrlStatus = the status as set to the oven's temp. control
 *
 ****************************************************************************/
void COvenDevice::TempCtrlSetStatusAckn(quint32 InstanceID, ReturnCode_t HdlInfo, TempCtrlStatus_t TempCtrlStatus)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llDEBUG) << " COvenDevice<" << (int) m_instanceID << "> TempCtrl 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < OVEN_DEVICE_TASK_COUNT; idx++)
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
                        // error was detected from function module side
                        FILE_LOG_L(laDEV, llERROR) << "  while req. SetStatusAckn.. error";
                        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
                    }

                    CANTemperatureCtrlTask* pTempCtrlTask = dynamic_cast<CANTemperatureCtrlTask*>(pFunctionModuleTask);
                    if(pTempCtrlTask)
                    {
                        pTempCtrlTask->m_State = TempCtrlStatus;
                    }

                    /*if (m_pIOvenDev != NULL)
                    {
                        m_pIOvenDev->RouteSetOvenStatus(m_instanceID, m_lastErrorHdlInfo, TempCtrlStatus);
                    }*/
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the temperature control operating mode information signal
 *
 *            The internal state machine will be reset, and the temperature control's
 *            operating mode information is forwarded to the IOvenDevice class
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the temp. control op. mode  was received,
 *                      otherwise the return value of the error detecting function
 *  \iparam   TempCtrlOperatingMode = the actual operating mode of then oven's temp. control
 *
 *
 ****************************************************************************/
void COvenDevice::TempCtrlActOperatingMode(quint32 InstanceID, ReturnCode_t HdlInfo, TempCtrlOperatingMode_t TempCtrlOperatingMode)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llDEBUG) << " COvenDevice<" << (int) m_instanceID << "> TempCtrl 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < OVEN_DEVICE_TASK_COUNT; idx++)
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
                        // error was detected from function module side
                        FILE_LOG_L(laDEV, llERROR) << "  while req. SetStatusAckn.. error";
                        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
                    }

                    CANTemperatureCtrlTask* pTempCtrlTask = dynamic_cast<CANTemperatureCtrlTask*>(pFunctionModuleTask);
                    if(pTempCtrlTask)
                    {
                        pTempCtrlTask->m_OperationMode = TempCtrlOperatingMode;
                    }
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Set temperature control operating mode acknowledge notification
 *
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the temp. control op. mode  was received,
 *                      otherwise the return value of the error detecting function
 *  \iparam   TempCtrlOpMode = the operating mode as set to the oven's temp. control
 *
 ****************************************************************************/
void COvenDevice::TempCtrlSetOperatingModeAckn(quint32 InstanceID, ReturnCode_t HdlInfo, TempCtrlOperatingMode_t TempCtrlOpMode)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(HdlInfo);
    Q_UNUSED(TempCtrlOpMode);
}

/****************************************************************************/
/*!
 *  \brief   Request the actual oven temperature
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANTemperatureControl class.
 *           When finished, the slot TempCtrlActTemperature() will be called by
 *           CANTemperatureControl, and forwards the acknowledge of this request to the interface class
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t COvenDevice::ReqActTemperature()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(OVEN_DEV_TASK_TEMP_REQ))
    {
        FILE_LOG_L(laDEV, llDEBUG) << " COvenDevice";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " COvenDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Set the oven's reference temperature
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANTemperatureControl class.
 *           When finished, the slot TempCtrlReferenceTemperature() will be called by
 *           CANTemperatureControl, and forwards the acknowledge of this request to the interface class
 *
 *  \iparam  Temperatur = the reference temperature in 0.1 °C
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t COvenDevice::SetTemperature(qint16 Temperatur)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(OVEN_DEV_TASK_TEMP_SET))
    {
        //device task was activated, forward the request parameter to the corresponding function module task
        FunctionModuleTask* pFunctionModuleTask = 0;
        pFunctionModuleTask = m_DeviceTask[OVEN_DEV_TASK_TEMP_SET].m_FctModTaskMap[1];

        CANTemperatureCtrlTask* pTempCtrlTask = dynamic_cast<CANTemperatureCtrlTask*>(pFunctionModuleTask);
        if(pTempCtrlTask)
        {
            pTempCtrlTask->m_Temperatur = Temperatur;
            FILE_LOG_L(laDEV, llDEBUG) << " COvenDevice";
        }
        else
        {
            RetVal = DCL_ERR_NULL_PTR_ACCESS;
            FILE_LOG_L(laDEV, llERROR) << " COvenDevice: DCL_ERR_NULL_PTR_ACCESS";
        }
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " COvenDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Request the reference run of the oven cover motor
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
 */
/****************************************************************************/
ReturnCode_t COvenDevice::ReqReferenceRun()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(OVEN_DEV_TASK_REFRUN))
    {
        FILE_LOG_L(laDEV, llINFO) << " COvenDevice";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " COvenDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Request the oven cover movement
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANStepperMotor class.
 *           When finished, the slot StepperMotorMovementAckn() will be called by
 *           CANStepperMotor, and forwards the acknowledge of this request to the interface class
 *
 *  \iparam  CoverPosition = Requested oven cover position
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t COvenDevice::ReqCoverMoveToPosition(OvenCoverPosition_t CoverPosition)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(OVEN_DEV_TASK_COVER_POS_SET))
    {
        //devie task was activated, forward the request parameter to the corresponding function module task
        FunctionModuleTask* pFunctionModuleTask = 0;
        pFunctionModuleTask = m_DeviceTask[OVEN_DEV_TASK_COVER_POS_SET].m_FctModTaskMap[1];

        CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pMotorTask)
        {
            pMotorTask->m_TargetPos = GetMotorPosFromCoverPos(CoverPosition);
            if(CoverPosition == OVEN_COVER_CLOSED)
            {
                pMotorTask->m_Profile = m_CoverCloseSpeedMotionProfile;
            }
            else if(CoverPosition == OVEN_COVER_OPENED)
            {
                pMotorTask->m_Profile = m_CoverOpenSpeedMotionProfile;
            }
            FILE_LOG_L(laDEV, llDEBUG) << " COvenDevice";
        }
        else
        {
            RetVal = DCL_ERR_NULL_PTR_ACCESS;
            FILE_LOG_L(laDEV, llERROR) << " COvenDevice: DCL_ERR_NULL_PTR_ACCESS";
        }
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " COvenDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Request the oven cover's actual position
 *
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
ReturnCode_t COvenDevice::ReqActCoverPosition()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(OVEN_DEV_TASK_COVER_POS_REQ))
    {
        FILE_LOG_L(laDEV, llDEBUG) << " COvenDevice";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " COvenDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Set the status of the temperature control
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANTemperatureControl class.
 *           When finished, the slot TempCtrlActStatus() will be called by
 *           CANTemperatureControl, and forwards the acknowledge of this request to the interface class
 *
 *  \iparam  TempCtrlStatus = status of the oven's temperature control
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t COvenDevice::SetTempCtrlStatus(TempCtrlStatus_t TempCtrlStatus)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(OVEN_DEV_TASK_TEMP_STATUS_SET))
    {
        FunctionModuleTask* pFunctionModuleTask = 0;
        pFunctionModuleTask = m_DeviceTask[OVEN_DEV_TASK_TEMP_STATUS_SET].m_FctModTaskMap[1];

        CANTemperatureCtrlTask* pTempCtrlTask = dynamic_cast<CANTemperatureCtrlTask*>(pFunctionModuleTask);
        if(pTempCtrlTask)
        {
            pTempCtrlTask->m_State = TempCtrlStatus;
        }
        FILE_LOG_L(laDEV, llDEBUG) << " COvenDevice";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " COvenDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Request the actual status of the temperature control
 *
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANTemperatureControl class.
 *           When finished, the slot TempCtrlActStatus() will be called by
 *           CANTemperatureControl, and forwards the acknowledge of this request to the interface class
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t COvenDevice::ReqTempCtrlStatus()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(OVEN_DEV_TASK_TEMP_STATUS_REQ))
    {
        FILE_LOG_L(laDEV, llDEBUG) << " COvenDevice";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " COvenDevice invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Returns the oven covers position as OvenCoverPosition_t
 *
 *            Depending on the exact motor position, the cover position will be set to
 *            OVEN_COVER_OPENED or OVEN_COVER_CLOSED, all other positions result in OVEN_COVER_UNDEF
 *
 *  \iparam   MotorPosition = The stepper motors position
 *
 *  \return   The motor position as OvenCoverPosition_t
 */
/****************************************************************************/
OvenCoverPosition_t COvenDevice::GetOvencoverPositionFromMotorPos(qint32 MotorPosition)
{
    OvenCoverPosition_t CoverPosition;

    FILE_LOG_L(laDEV, llINFO) << "  GetOvencoverPositionFromMotorPos: " << MotorPosition;

    if(MotorPosition > 1150)
    {
        CoverPosition = OVEN_COVER_OPENED;
        FILE_LOG_L(laDEV, llINFO) << "  OVEN_COVER_OPENED";
    }
    else if(MotorPosition < 150)
    {
        CoverPosition = OVEN_COVER_CLOSED;
        FILE_LOG_L(laDEV, llINFO) << "  OVEN_COVER_CLOSED";
    }
    else
    {
        CoverPosition = OVEN_COVER_UNDEF;
        FILE_LOG_L(laDEV, llINFO) << "  OVEN_COVER_UNDEF";
    }
    return CoverPosition;
}

/****************************************************************************/
/*!
 *  \brief    Returns the covor motor posotion from OvenCoverPosition_t
 *
 *            Depending on the OvenCoverPosition_t type, the exact motor position
 *            in half steps is returned.
 *
 *  \iparam   CoverPosition = The oven cover position
 *
 *  \return   The motor position as Position_t
 */
/****************************************************************************/
Position_t COvenDevice::GetMotorPosFromCoverPos(OvenCoverPosition_t CoverPosition)
{
    Position_t MotorPos = 0;

    if(CoverPosition == OVEN_COVER_CLOSED)
    {
        MotorPos = m_CoverClosedPosition;
    }
    else if(CoverPosition == OVEN_COVER_OPENED)
    {
        MotorPos = m_CoverOpenPosition;
    }

    return MotorPos;
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
bool COvenDevice::ActivateDeviceTask(quint8 DeviceTaskIndex)
{
    bool Activated = false;
    FunctionModuleTask* pFunctionModuleTask;

    if((m_TaskID == OVENDEV_TASKID_FREE) || (m_TaskID == OVENDEV_TASKID_COMMAND_HDL))
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
            m_TaskID = OVENDEV_TASKID_COMMAND_HDL;
            Activated = true;
        }
    }

    return Activated;

}

} //namespace
