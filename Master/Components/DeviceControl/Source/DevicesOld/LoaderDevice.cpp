/******************************************************************/
/*! \file LoaderDevice.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CLoaderDevice
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

#include "DeviceControl/Include/Devices/LoaderDevice.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

#define LOADER_DEV_TIMEOUT_RFID_REQ    1000  ///< timeout rfid read operation
#define LOADER_DEV_TIMEOUT_REFRUN_REQ  5000  ///< timeout drawer reference run
#define LOADER_DEV_TIMEOUT_POS_REQ     5000  ///< timeout drawer postioning

//#define LOADER_DEV_MOTOR_OPEN_POS      1950  ///< motor position at drawer open position
//#define LOADER_DEV_MOTOR_CLOSE_POS      100  ///< motor position at drawer closed position
#define LOADER_MAX_DEVIATION_CLOSED_STATE     50  ///< max motor deviation for position 'closed' for LoaderPosition_t assignment
#define LOADER_MIN_DEVIATION_OPEN_STATE       50  ///< min motor deviation for position 'open' for LoaderPosition_t assignment


/****************************************************************************/
/*!
 *  \brief  Constructor of CLoaderDevice
 *
 *  \iparam pDeviceProcessing = pointer to DeviceProcessing
 *  \iparam Type = Device type string
 *  \iparam InstanceID = identifier of the class instance
 */
/****************************************************************************/
CLoaderDevice::CLoaderDevice(DeviceProcessing* pDeviceProcessing, QString Type, DevInstanceID_t InstanceID)
        : CBaseDevice(pDeviceProcessing, Type),
    m_pMotor(0), m_pDrawerButton(0), m_pDrawerLEDFree(0), m_pDrawerLEDBlocked(0),
    m_ActLoaderPosition(LOADER_UNDEF),
    m_DrawerOpenSpeedMotionProfile(0), m_DrawerCloseSpeedMotionProfile(0),
    m_DrawerClosedPosition(0), m_DrawerOpenPosition(0)
{
    m_instanceID = InstanceID;
    Q_ASSERT((m_instanceID == DEVICE_INSTANCE_ID_LOADER) || (m_instanceID == DEVICE_INSTANCE_ID_UNLOADER));

    m_blockState = BLOCKING_STATE_FREE;

    for(int idx = 0; idx < LOADER_RFID_MAX; idx++)
    {
        m_pRFID = 0;
        m_rfidState[idx] = RFID_CHANNEL_STATE_FREE;
        m_rfidMSB[idx] = 0;
        m_rfidLSB[idx] = 0;
        m_rfidHdlInfo[idx] = DCL_ERR_FCT_CALL_SUCCESS;
        m_rfidErrorCode[idx] = 0;
        //m_RFIDused[idx] = false;
        m_RFIDused = false;
    }

    m_bLEDRefresh = true;
    m_bAcknowledgeNeed = false;

    m_MainState    = DEVICE_MAIN_STATE_START;
    m_MainStateOld = m_MainState;
    m_TaskID       = LOADERDEV_TASKID_INIT;
    m_ErrorTaskState = LOADER_DEV_ERRTASK_STATE_FREE;

    m_ReqRfidChannel = LOADER_ID_RFID_UNDEF;

    FILE_LOG_L(laDEV, llINFO) << " loader device created :" << std::hex << (int) m_instanceID;
}

/****************************************************************************/
/*!
 *  \brief    Destructor of CLoaderDevice
 *
 *
 *  \return
 *
 ****************************************************************************/
CLoaderDevice::~CLoaderDevice()
{
    // Zero all the pointer variables
    m_pRFID = 0;
    m_pDevProc = 0;
    m_pMotor = 0;
    m_pDrawerButton = 0;
    m_pDrawerButton = 0;
    m_pDrawerLEDFree = 0;
    m_pDrawerLEDBlocked = 0;
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
void CLoaderDevice::HandleTasks()
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
            m_ErrorTaskState = LOADER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_FCT_MOD_CFG;
            m_TaskID = LOADERDEV_TASKID_FREE;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = LOADER_DEV_ERRTASK_STATE_REPORT_IFACE;
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
            m_ErrorTaskState = LOADER_DEV_ERRTASK_STATE_FREE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << " CLoaderDevice, id " << (int) m_instanceID << ": new state: " << (int) m_MainState;
        m_MainStateOld = m_MainState;
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules pointer variables
 *           m_pRFID  - RFID channels
 *           m_pMotor - Motor to move the drawer
 *           m_pDrawerButton - loader button function module
 *           m_pDrawerLEDFree - loader status 'free' LED
 *           m_pDrawerLEDBlocked - loader status 'blocked' LED
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS or error return code
 *
 ****************************************************************************/
ReturnCode_t CLoaderDevice::HandleInitializationState()
{
    quint32 InstanceID;

    FILE_LOG_L(laDEV, llINFO) << "  CLoaderDevice::HandleInitializationState() id:" << (int) m_instanceID;

    m_pMotor = (CStepperMotor*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorDrawerKey));
    if(m_pMotor == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_MOTOR_LOADER not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_DrawerRFIDKey);
    if(InstanceID == 0)
    {
        FILE_LOG_L(laDEV, llINFO) << "   FCTMOD_RFID_LOADER not used.";
    }
    else
    {
        m_RFIDused = true;
        m_pRFID = (CRfid11785*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pRFID == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RFID_LOADER);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RFID1_LOADER not allocated.";
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }

    m_pDrawerButton = (CDigitalInput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_DrawerButtonKey));
    if(m_pDrawerButton == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_BUTTON_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_BUTTON_LOADER not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    m_pDrawerLEDFree = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_DrawerLEDFreeKey));
    if(m_pDrawerLEDFree == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_LED_FREE_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_LED_FREE_LOADER not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    m_pDrawerLEDBlocked = (CDigitalOutput*) m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(CANObjectKeyLUT::m_DrawerLEDBlockedKey));
    if(m_pDrawerLEDBlocked == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_LED_BLOCKED_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_LED_BLOCKED_LOADER not allocated.";
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
ReturnCode_t CLoaderDevice::HandleConfigurationState()
{
    //Motor
    if (!connect(m_pMotor, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)), this, SLOT(StepperMotorReferenceMovementAckn(quint32, ReturnCode_t, qint32))))
    {
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportReferenceMovementAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if (!connect(m_pMotor, SIGNAL(ReportSetStateAckn(quint32, ReturnCode_t)), this, SLOT(StepperMotorSetStateAckn(quint32, ReturnCode_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportActState' failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if (!connect(m_pMotor, SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16)), this, SLOT(StepperMotorMovementAckn(quint32, ReturnCode_t, qint32, qint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportMovementAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if (!connect(m_pMotor, SIGNAL(ReportPosition(quint32, ReturnCode_t, qint32, qint8)), this, SLOT(StepperMotorActPosition(quint32, ReturnCode_t, qint32))))
    {
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportPosition'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if (!connect(m_pMotor, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(StepperMotorError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect motor signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //RFID
    if(m_RFIDused == true)
    {
        if (!connect(m_pRFID, SIGNAL(ReportUserData(quint32, ReturnCode_t, quint32)),
                       this, SLOT(RFIDDataReceived(quint32, ReturnCode_t, quint32))))
        {
            SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RFID_UNLOADER);
            FILE_LOG_L(laDEV, llERROR) << "   Connect RFID signal 'ReportReadData'failed.";
            return DCL_ERR_FCT_CALL_FAILED;
        }

        if (!connect(m_pRFID, SIGNAL(ReportUID(quint32, ReturnCode_t, quint32)),
                       this, SLOT(RFIDDataReceived(quint32, ReturnCode_t, quint32))))
        {
            SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RFID_UNLOADER);
            FILE_LOG_L(laDEV, llERROR) << "   Connect RFID signal 'ReportReadData'failed.";
            return DCL_ERR_FCT_CALL_FAILED;
        }

    }

    //Button
    if (!connect(m_pDrawerButton, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), this, SLOT(DigInputChanged(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_BUTTON_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect dig. input signal 'ReportActInputValue'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if (!connect(m_pDrawerButton, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(DigInputError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_BUTTON_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect dig. input signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    // LEDs
    if (!connect(m_pDrawerLEDFree, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(DigInputError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_LED_FREE_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect dig. output (free) signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if (!connect(m_pDrawerLEDBlocked, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(DigInputError(quint32, quint16, quint16, quint16, QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_LED_BLOCKED_LOADER);
        FILE_LOG_L(laDEV, llERROR) << "   Connect dig. output (blocked) signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    // read process settings
    m_DrawerOpenSpeedMotionProfile = GetProcSettingPosition("drawer_open_speed_motion_profile");
    m_DrawerCloseSpeedMotionProfile = GetProcSettingPosition("drawer_close_speed_motion_profile");
    m_DrawerClosedPosition = GetProcSettingPosition("drawer_closed_position");
    m_DrawerOpenPosition = GetProcSettingPosition("drawer_open_position");

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   Create and configure the device tasks
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CLoaderDevice::ConfigureDeviceTasks()
{
    CANStepperMotorTask*    pCANStepperMotorTask;
    CANRFIDTask*            pCANRFIDTask;
    quint8  TaskIndex = 0;

    //######################################
    // Reference run task list
    // -> Set motor state 'enabled' (activate the motor driver IC)
    // -> Proceed reference run
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = m_pMotor;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pCANStepperMotorTask->m_SubCommandData = 1;  // enable motor
    m_DeviceTask[LOADER_DEV_TASK_REFRUN].m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    FILE_LOG_L(laDEV, llDEBUG1) << " CLoaderDevice: add task 'SetState'";

    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = m_pMotor;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    m_DeviceTask[LOADER_DEV_TASK_REFRUN].m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    FILE_LOG_L(laDEV, llDEBUG1) << " CLoaderDevice: add task 'ReferenceRun'";

    //######################################
    // set drawer position
    // -> motor position command
    TaskIndex = 0;
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = m_pMotor;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    m_DeviceTask[LOADER_DEV_TASK_DRAWER_POS_SET].m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " CLoaderDevice: add task 'MovePos'";

    //######################################
    // request drawer position
    // -> request act position command
    TaskIndex = 0;
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = m_pMotor;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    m_DeviceTask[LOADER_DEV_TASK_DRAWER_ACTPOS_REQ].m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " CLoaderDevice: add task 'ReqActPos'";

    //######################################
    // request RFID data
    // -> request RFID data
    TaskIndex = 0;
    pCANRFIDTask = CBaseDevice::GetNewCANRFIDTask(CANRFIDTask::FCTMOD_RFID_TASK_ID_REQ_DATA); //suha doubt
    pCANRFIDTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANRFIDTask->m_TaskIndex = TaskIndex;
    pCANRFIDTask->m_TriggerTaskIndex = TaskIndex;
    pCANRFIDTask->m_pCANObject = m_pRFID;
    pCANRFIDTask->m_Timeout = 5000;
    m_DeviceTask[LOADER_DEV_TASK_READ_RFID].m_FctModTaskMap[TaskIndex] = pCANRFIDTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " CLoaderDevice: add task 'ReqRFIDdata'";

    //######################################
    // request RFID TAG
    // -> request RFID TAG
    TaskIndex = 0;
    pCANRFIDTask = CBaseDevice::GetNewCANRFIDTask(CANRFIDTask::FCTMOD_RFID_TASK_ID_REQ_TAG); //suha doubt
    pCANRFIDTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANRFIDTask->m_TaskIndex = TaskIndex;
    pCANRFIDTask->m_TriggerTaskIndex = TaskIndex;
    pCANRFIDTask->m_pCANObject = m_pRFID;
    pCANRFIDTask->m_Timeout = 5000;
    m_DeviceTask[LOADER_DEV_TASK_READ_TAG].m_FctModTaskMap[TaskIndex] = pCANRFIDTask;
    FILE_LOG_L(laDEV, llDEBUG1) << " CLoaderDevice: add task 'ReqRFIDdata'";

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
void CLoaderDevice::HandleIdleState()
{
    if(m_bLEDRefresh == true)
    {
        /// \todo mh, there is some space for improvements
        HandleLEDState();
        m_bLEDRefresh = false;
    }

    if(m_TaskID == LOADERDEV_TASKID_FREE)
    {
        ; // there are no pending tasks
    }
    else if(m_TaskID == LOADERDEV_TASKID_COMMAND_HDL)
    {
        HandleDeviceTaskActions();
    }
    else if(m_TaskID == LOADERDEV_TASKID_ERROR)
    {
        //Just wait for 'ExitErrormode'
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << " Invalid taskID: " << (int) m_TaskID;

        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_INVALID_STATE, (quint16) m_TaskID);

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = LOADER_DEV_ERRTASK_STATE_REPORT_IFACE;
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
void CLoaderDevice::HandleDeviceTaskActions()
{
    if((m_pDevProc == 0) || (m_pMotor == 0) || (m_pRFID == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_NULL_PTR_ACCESS, (quint16) m_TaskID);

        //forward the error information. The error relevant datas should already have been set.
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = LOADER_DEV_ERRTASK_STATE_REPORT_IFACE;

        return;
    }

    for (quint8 idx = 0; idx < LOADER_DEVICE_TASK_COUNT; idx++)
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

                    FILE_LOG_L(laDEV, llINFO) << "  CLoaderDevice: task for startup found.";
                    RetVal = m_pDevProc->GetFunctionModuleTaskManager()->StartTask(pFunctionModuleTask);
                    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
                    {
                        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_FAILED;
                        m_TaskID = LOADERDEV_TASKID_ERROR;
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_REQ_REFRUN_FCTCALL_FAILED, (quint16) m_TaskID);

                        FILE_LOG_L(laDEV, llERROR) << "  CLoaderDevice: failed to start task: " << (int) pFunctionModuleTask->m_TaskID;
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
                FILE_LOG_L(laDEV, llERROR) << "  CLoaderDevice: Action state error.";
                m_TaskID = LOADERDEV_TASKID_ERROR;
                m_DeviceTask[idx].m_TaskState = DeviceTask::DEV_TASK_STATE_ERROR;
                m_ErrorTaskState = LOADER_DEV_ERRTASK_STATE_REPORT_IFACE;
            }
            else if(bActionStateInProgress == false)
            {
                FILE_LOG_L(laDEV, llINFO) << "  CLoaderDevice: positions received, next start movement..";

                //reset the tasks to the 'unused' state
                //m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(m_DeviceTask[idx].m_FctModTaskMap);
                m_DeviceTask[idx].m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
            }
        }

        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            if(idx == LOADER_DEV_TASK_REFRUN)
            {
                FunctionModuleTask* pFunctionModuleTask = 0;
                LoaderPosition_t LoaderPosition = LOADER_UNDEF;

                pFunctionModuleTask = m_DeviceTask[idx].m_FctModTaskMap[1];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
                {
                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                    if(pMotorTask)
                    {
                        LoaderPosition = GetDrawerPositionFromMotorPos(pMotorTask->m_ActPos);
                    }
                }

                //m_pILoaderDev->RouteReferenceRunAckn(m_instanceID, m_lastErrorHdlInfo, LoaderPosition);
            }
            else if(idx == LOADER_DEV_TASK_DRAWER_POS_SET)
            {
                FunctionModuleTask* pFunctionModuleTask = 0;
                LoaderPosition_t LoaderPosition = LOADER_UNDEF;

                pFunctionModuleTask = m_DeviceTask[idx].m_FctModTaskMap[0];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
                {
                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                    if(pMotorTask)
                    {
                        LoaderPosition = GetDrawerPositionFromMotorPos(pMotorTask->m_ActPos);
                    }
                }
                m_ActLoaderPosition = LoaderPosition;
                //m_pILoaderDev->RouteDrawerPositionAckn(m_instanceID, m_lastErrorHdlInfo, LoaderPosition, m_blockState);
            }
            else if(idx == LOADER_DEV_TASK_DRAWER_ACTPOS_REQ)
            {
                FunctionModuleTask* pFunctionModuleTask = 0;
                LoaderPosition_t LoaderPosition = LOADER_UNDEF;

                pFunctionModuleTask = m_DeviceTask[idx].m_FctModTaskMap[0];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
                {
                    CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

                    if(pMotorTask)
                    {
                        LoaderPosition = GetDrawerPositionFromMotorPos(pMotorTask->m_ActPos);
                    }
                }
                //m_pILoaderDev->RouteActDrawerPosition(m_instanceID, m_lastErrorHdlInfo, LoaderPosition);
            }
            else if(idx == LOADER_DEV_TASK_READ_RFID)
            {
                FunctionModuleTask* pFunctionModuleTask = 0;
                quint32  RFIDData = 0;

                pFunctionModuleTask = m_DeviceTask[idx].m_FctModTaskMap[0];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_RFID)
                {
                    CANRFIDTask* pRFIDTask = dynamic_cast<CANRFIDTask*>(pFunctionModuleTask);

                    if(pRFIDTask)
                    {
                        RFIDData = pRFIDTask->m_RFIDData;
                    }
                }
                //m_pILoaderDev->RouteRFIDData(m_instanceID, m_lastErrorHdlInfo, m_ReqRfidChannel, RFIDData);
            }
            else if(idx == LOADER_DEV_TASK_READ_TAG)
            {
                FunctionModuleTask* pFunctionModuleTask = 0;
                quint32  RFIDData = 0;

                pFunctionModuleTask = m_DeviceTask[idx].m_FctModTaskMap[0];
                if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_RFID_TAG)
                {
                    CANRFIDTask* pRFIDTask = dynamic_cast<CANRFIDTask*>(pFunctionModuleTask);

                    if(pRFIDTask)
                    {
                        RFIDData = pRFIDTask->m_RFIDData;
                    }
                }
                //m_pILoaderDev->RouteRFIDTAG(m_instanceID, m_lastErrorHdlInfo, m_ReqRfidChannel, RFIDData);
            }

            //reset the tasks to the 'unused' state
            m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(m_DeviceTask[idx].m_FctModTaskMap);
            m_DeviceTask[idx].m_TaskState = DeviceTask::DEV_TASK_STATE_UNUSED;
            m_TaskID = LOADERDEV_TASKID_FREE;
        }
    }
}


/****************************************************************************/
/*!
 *  \brief    Handles the error state of the class.
 *
 *   This task controls the error handling of the class.
 *   The function is called from HandleIdleState() if m_MainState == OVEN_DEV_MAIN_STATE_ERROR..
 *   This task consists of the following states:
 *
 *     LOADER_DEV_ERRTASK_STATE_REPORT_IFACE:
 *      This state reports the error to the interface class. The interface class will change to error state
 *      and blocks future requests
 *
 *     LOADER_DEV_ERRTASK_STATE_REPORT_DEVPROC:
 *      This state reports the error to the device processing class.
 *
 *     LOADER_DEV_ERRTASK_STATE_IDLE:
 *      Hold the class in error state, do nothing
 *
 *    LOADER_DEV_ERRTASK_STATE_RESET
 *      Reset the error state, set all states to it's initialisation state
 *
 *  \return   void
 */
/****************************************************************************/
void CLoaderDevice::HandleErrorState()
{
    if(m_ErrorTaskState == LOADER_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_ErrorTaskState == LOADER_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        for (quint8 idx = 0; idx < LOADER_DEVICE_TASK_COUNT; idx++)
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

                if(idx == LOADER_DEV_TASK_REFRUN)
                {
                    /*if(m_pILoaderDev != NULL)
                    {
                        m_pILoaderDev->RouteReferenceRunAckn(m_instanceID, m_lastErrorHdlInfo, LOADER_UNDEF);
                    }*/
                    if((pFunctionModuleTaskError) && (pFunctionModuleTaskError->m_ErrorHdlInfo == DCL_ERR_FCT_CALL_FAILED))
                    {
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_REQ_REFRUN_FCTCALL_FAILED, (quint16) m_TaskID);
                        FILE_LOG_L(laDEV, llERROR) << "  CLoaderDevice::ExecReferenceMovement fct call failed";
                    }
                    else if((pFunctionModuleTaskError) && (pFunctionModuleTaskError->m_ErrorHdlInfo == DCL_ERR_TIMEOUT))
                    {
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_REQ_REFRUN_TIMEOUT, (quint16)m_TaskID);
                        FILE_LOG_L(laDEV, llERROR) << "  CLoaderDevice: while reference movement.. timeout";
                    }
                    else
                    {
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_UNKNOWN_ERROR, (quint16)m_TaskID);
                        FILE_LOG_L(laDEV, llERROR) << "  CLoaderDevice: while reference movement.. ";
                    }
                }
                else if(idx == LOADER_DEV_TASK_DRAWER_POS_SET)
                {
                    /*if(m_pILoaderDev != NULL)
                    {
                        m_pILoaderDev->RouteDrawerPositionAckn(m_instanceID, m_lastErrorHdlInfo, LOADER_UNDEF, m_blockState);
                    }*/
                    if((pFunctionModuleTaskError) && (pFunctionModuleTaskError->m_ErrorHdlInfo == DCL_ERR_FCT_CALL_FAILED))
                    {
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_SET_DRAWER_POS_FCTCALL_FAILED, (quint16) m_TaskID);
                    }
                    else if((pFunctionModuleTaskError) && (pFunctionModuleTaskError->m_ErrorHdlInfo == DCL_ERR_TIMEOUT))
                    {
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_SET_DRAWER_POS_TIMEOUT, (quint16)m_TaskID);
                    }
                    else
                    {
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_UNKNOWN_ERROR, (quint16)m_TaskID);
                        FILE_LOG_L(laDEV, llERROR) << "  CLoaderDevice: while reference movement.. ";
                    }
                }
                else if(idx == LOADER_DEV_TASK_DRAWER_ACTPOS_REQ)
                {
                    /*if(m_pILoaderDev != NULL)
                    {
                        m_pILoaderDev->RouteActDrawerPosition(m_instanceID, m_lastErrorHdlInfo, LOADER_UNDEF);
                    }*/
                    if((pFunctionModuleTaskError) && (pFunctionModuleTaskError->m_ErrorHdlInfo == DCL_ERR_FCT_CALL_FAILED))
                    {
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_REQ_DRAWER_POS_FCTCALL_FAILED, (quint16) m_TaskID);
                    }
                    else if((pFunctionModuleTaskError) && (pFunctionModuleTaskError->m_ErrorHdlInfo == DCL_ERR_TIMEOUT))
                    {
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_REQ_DRAWER_POS_TIMEOUT, (quint16)m_TaskID);
                    }
                    else
                    {
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_UNKNOWN_ERROR, (quint16)m_TaskID);
                        FILE_LOG_L(laDEV, llERROR) << "  CLoaderDevice: while reference movement.. ";
                    }
                }
                else if(idx == LOADER_DEV_TASK_READ_RFID)
                {
                    /*if(m_pILoaderDev != NULL)
                    {
                        m_pILoaderDev->RouteRFIDData(m_instanceID, m_lastErrorHdlInfo, m_ReqRfidChannel, 0);
                    }*/
                    if((pFunctionModuleTaskError) && (pFunctionModuleTaskError->m_ErrorHdlInfo == DCL_ERR_FCT_CALL_FAILED))
                    {
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_REQ_RFID_DATA_FCTCALL_FAILED, (quint16) m_TaskID);
                    }
                    else if((pFunctionModuleTaskError) && (pFunctionModuleTaskError->m_ErrorHdlInfo == DCL_ERR_TIMEOUT))
                    {
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_REQ_RFID_DATA_TIMEOUT, (quint16)m_TaskID);
                    }
                    else
                    {
                        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_UNKNOWN_ERROR, (quint16)m_TaskID);
                        FILE_LOG_L(laDEV, llERROR) << "  CLoaderDevice: while reference movement.. ";
                    }
                }

                //reset the tasks to the 'unused' state
                m_pDevProc->GetFunctionModuleTaskManager()->ResetTasks(m_DeviceTask[idx].m_FctModTaskMap);
                m_DeviceTask[idx].m_TaskState = DeviceTask::DEV_TASK_STATE_UNUSED;

                /*if(m_pILoaderDev != 0)
                {
                    m_pILoaderDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
                }*/
            }
        }
        m_ErrorTaskState = LOADER_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_ErrorTaskState == LOADER_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_ErrorTaskState = LOADER_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_ErrorTaskState == LOADER_DEV_ERRTASK_STATE_RESET)
    {
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;
        // reset the function module references
        m_pMotor = 0;
        m_pDrawerButton = 0;
        m_pDrawerLEDFree = 0;
        m_pDrawerLEDBlocked = 0;
        for(int idx = 0; idx < LOADER_RFID_MAX; idx++)
        {
            //m_pRFID[idx] = 0;
            m_pRFID = 0;
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Set the LED state according to the drawler's blocking state.
 *
 *            If any changes according the LED states detected, the function sets the
 *            output values of the both digital output modules according to the loader's
 *            blocking state.
 *
 *
 ****************************************************************************/
void CLoaderDevice::HandleLEDState()
{
    ReturnCode_t RetVal;

    if((m_pDrawerLEDBlocked == 0) || (m_pDrawerLEDFree == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_LOADER_DEV, ERROR_DCL_LOADER_DEV_NULL_PTR_ACCESS, (quint16) m_TaskID);

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_ErrorTaskState = LOADER_DEV_ERRTASK_STATE_REPORT_IFACE;
    }
    else if(m_blockState == BLOCKING_STATE_FREE)
    {
        FILE_LOG_L(laDEV, llINFO) << "   set unblocked! " << (int) m_blockState << "  " << (int) m_TaskID;
        // no blocking, activate green LED, switch of the red LED
        RetVal = m_pDrawerLEDBlocked->SetOutputValue(0);
        if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laDEV, llERROR) << "  unable to set 'LED blocked output! " << (int) RetVal;
        }
        RetVal = m_pDrawerLEDFree->SetOutputValue(1);
        if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laDEV, llERROR) << "  unable to set 'LED free' output! " << (int) RetVal;
        }
    }
    else
    {
        FILE_LOG_L(laDEV, llINFO) << "   set blocked! " << (int) m_blockState << "  " << (int) m_TaskID;
        // blocking state, activate red LED, switch of the green LED
        RetVal = m_pDrawerLEDBlocked->SetOutputValue(1);
        if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laDEV, llERROR) << "  unable to set 'LED blocked output! " << (int) RetVal;
        }
        RetVal = m_pDrawerLEDFree->SetOutputValue(0);
        if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laDEV, llERROR) << "  unable to set 'LED free' output! " << (int) RetVal;
        }
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
void CLoaderDevice::StepperMotorError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_ErrorTaskState = LOADER_DEV_ERRTASK_STATE_REPORT_IFACE;
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
void CLoaderDevice::StepperMotorReferenceMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position)
{
    CANStepperMotorTask* pMotorTask = 0;

    Q_UNUSED(Position);
    FILE_LOG_L(laDEV, llDEBUG) << " CLoaderDevice<" << (int) m_instanceID << "> StepperMotor 0x" << std::hex << InstanceID;

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
            FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while Reference run... error";
            pMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
        }
        pMotorTask->m_ActPos = (Position_t) Position;

        //This assumption depends on the mechanic:
        m_ActLoaderPosition = LOADER_CLOSED;

        //get the next motor task from the device task map, with state 'PROGRESS', and taskID 'STATE', if any
        pMotorTask = GetMotorFctModTaskInProcess(InstanceID, pMotorTask, CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the stepper motors set state acknowledge notification
 *
 *  \iparam InstanceID = instance identifier of the function module
 *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the movement was successfully done,
 *                    otherwise the return value of the error detecting function
 */
/****************************************************************************/
void CLoaderDevice::StepperMotorSetStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo)
{
    FILE_LOG_L(laDEV, llINFO) << " CLoaderDevice<" << (int) m_instanceID << "> 0x" << std::hex << InstanceID;

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
            FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while Reference run... error";
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
CANStepperMotorTask* CLoaderDevice::GetMotorFctModTaskInProcess(quint32 InstanceID,
                                                                CANStepperMotorTask* pMotorTaskPrev,
                                                                CANStepperMotorTask::CANStepperMotorTaskID_t FctTaskID)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llINFO) << " CLoaderDevice<" << (int) m_instanceID << "> 0x" << std::hex << InstanceID;

    CANStepperMotorTask* pMotorTask = 0;
    CANStepperMotorTask* pMotorTaskRet = 0;

    for(quint8 idx = 0; idx < LOADER_DEVICE_TASK_COUNT; idx++)
    {
        if(m_DeviceTask[idx].m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            pFunctionModuleTask = 0;
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pMotorTaskPrev);
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
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pMotorTask);
            }
        }
    }

    return pMotorTaskRet;
}

/****************************************************************************/
/*!
 *  \brief    Handles the stepper motors position movement acknowledge
 *
 *            The internal state machine will be reset, and the acknowledge information is forwarded to
 *            the ILoaderDevice class
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
void CLoaderDevice::StepperMotorMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position, qint16 Speed)
{
    FunctionModuleTask* pFunctionModuleTask = 0;
    Q_UNUSED(Speed);

    FILE_LOG_L(laDEV, llDEBUG) << " CLoaderDevice<" << (int) m_instanceID << "> StepperMotor 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < LOADER_DEVICE_TASK_COUNT; idx++)
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
 *  \brief    Handles the stepper motor's actual position callback
 *
 *            The internal state machine will be reset, and the position information is forwarded to
 *            the ILoaderDevice class
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the position was received,
 *                      otherwise the return value of the error detecting function
 *  \iparam   Position = the motor's actual position
 *
 *  \return  void
 *
 ****************************************************************************/
void CLoaderDevice::StepperMotorActPosition(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llDEBUG) << " CLoaderDevice<" << (int) m_instanceID << "> StepperMotor 0x" << std::hex << InstanceID;

    quint8 idx = LOADER_DEV_TASK_DRAWER_ACTPOS_REQ;
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

/****************************************************************************/
/*!
*  \brief    This slot receives the RFID modul error signal
*
*  \iparam   InstanceID = instance identifier of the device
*  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_RFID_SLV
*  \iparam   ErrorCode  = from CAN-message
*  \iparam   ErrorData  = from CAN-message
*  \iparam   ErrorTime  = error message reception time on master side
*
****************************************************************************/
void CLoaderDevice::RFIDError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_ErrorTaskState = LOADER_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief    Handles the RFID data answer signal
 *
 *            The received data are stored at the field of the corresponding RFID-variables.
 *            The acknowledge to the interface class is send if all pending channels haver received
 *            the data. The acknwoledge is done in the task handling function
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if the movement was successfully done,
 *                      otherwise the return value of the error detecting function
 *  \iparam   RfidData = RFID data
 *
 *  \return  void
 *
 ****************************************************************************/
void CLoaderDevice::RFIDDataReceived(quint32 InstanceID, ReturnCode_t HdlInfo, quint32 RfidData)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    FILE_LOG_L(laDEV, llDEBUG) << " CLoaderDevice<" << (int) m_instanceID << "> RFID 0x" << std::hex << InstanceID;

    for(quint8 idx = 0; idx < LOADER_DEVICE_TASK_COUNT; idx++)
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
                        FILE_LOG_L(laDEV, llERROR) << " while ReadRFID.. error";
                        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;// set task to state 'error'
                        //m_pILoaderDev->RouteRFIDTAG(m_instanceID, m_lastErrorHdlInfo, m_ReqRfidChannel, 0);
                    }

                    CANRFIDTask* pRFIDTask = dynamic_cast<CANRFIDTask*>(pFunctionModuleTask);
                    if(pRFIDTask)
                    {
                        pRFIDTask->m_RFIDData = RfidData;
                    }
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTask[idx].m_FctModTaskMap, InstanceID, pFunctionModuleTask);
            }
        }
    }
}

/****************************************************************************/
/*!
*  \brief    This slot receives the digital input error signal
*
*  \iparam   InstanceID = instance identifier of the device
*  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_RFID_SLV
*  \iparam   ErrorCode  = from CAN-message
*  \iparam   ErrorData  = from CAN-message
*  \iparam   ErrorTime  = error message reception time on master side
*
****************************************************************************/
void CLoaderDevice::DigInputError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_ErrorTaskState = LOADER_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief    Handles the data signal of the digital input function module
 *
 *      If the button is pressed (input > 0), a request to change the drawer
 *      position will be started. If blocking state is 'blocked', the signal
 *      will be ignored.
 *
 *  \iparam   InstanceID = instance identifier of the function module
 *  \iparam   HdlInfo = DCL_ERR_FCT_CALL_SUCCESS if valid data,
 *                      otherwise the return value of the error detecting function
 *  \iparam   InputValue = input value of the dig. channel
 */
/****************************************************************************/
void CLoaderDevice::DigInputChanged(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 InputValue)
{
    FILE_LOG_L(laDEV, llINFO) << "  CLoaderDevice: " << (int) m_instanceID << " module:" << std::hex << InstanceID << " buttonInputChanged: " << InputValue;

    if((m_pDrawerButton == 0) || (InstanceID != m_pDrawerButton->GetModuleHandle()))
    {
        FILE_LOG_L(laDEV, llINFO) << "   Modulehandle input button does not match " << std::hex << InstanceID;
    }
    else if(InputValue > 0)
    {
        FILE_LOG_L(laDEV, llINFO) << "   Blockstate: " << (int) m_blockState << " SubStateAction: " << (int) m_TaskID;
        // The button was pressed
        if((m_blockState == BLOCKING_STATE_FREE) && (m_TaskID == LOADERDEV_TASKID_FREE))
        {
            if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
            {
                LoaderPosition_t LoaderPosition;
                ReturnCode_t RetVal;

                if(m_ActLoaderPosition == LOADER_CLOSED)
                {
                    LoaderPosition = LOADER_OPEN;
                    FILE_LOG_L(laDEV, llINFO) << "  CLoaderDevice::buttonInputChanged: open Loader";
                }
                else // Open and undef
                {
                    LoaderPosition = LOADER_CLOSED;
                    FILE_LOG_L(laDEV, llINFO) << "  CLoaderDevice::buttonInputChanged: close Loader";
                }
                RetVal = SetDrawerPosition(LoaderPosition);
                if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
                {
                    ; // Handle error
                }
            }
        }
        else
        {
            FILE_LOG_L(laDEV, llINFO) << "  CLoaderDevice::buttonInputChanged: movement request declined, loader is blocked or state invalid.";
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Request the reference run of the drawer motor
 *
 *           The function starts an asynchronous request to execute the reference run of the motor.
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
ReturnCode_t CLoaderDevice::ReferenceRun()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(LOADER_DEV_TASK_REFRUN))
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
 *  \brief   Set the drawer position, start movement
 *
 *           The function starts an asynchronous request to start a motor movement.
 *           The request will be accepted if no other request (task) is currently active
 *           The request is handled by the task's handle function, it will be forwarded there
 *           to the CANStepperMotor class.
 *           When finished, the slot StepperMotorMovementAckn() will be called by
 *           CANStepperMotor, and forwards the acknowledge of this request to the interface class
 *
 *  \iparam  LoaderPosition = Requested drawer position
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded.
 *           If there is always another task active, or the error state is active,
 *           DCL_ERR_INVALID_STATE is returned.
 */
/****************************************************************************/
ReturnCode_t CLoaderDevice::SetDrawerPosition(LoaderPosition_t LoaderPosition)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(LOADER_DEV_TASK_DRAWER_POS_SET))
    {
        //device task was activated, forward the request parameter to the corresponding function module task
        FunctionModuleTask* pFunctionModuleTask = 0;
        pFunctionModuleTask = m_DeviceTask[LOADER_DEV_TASK_DRAWER_POS_SET].m_FctModTaskMap[0];

        CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pMotorTask)
        {
            if(LoaderPosition == LOADER_OPEN)
            {
                pMotorTask->m_TargetPos = m_DrawerOpenPosition;
                pMotorTask->m_Profile = m_DrawerOpenSpeedMotionProfile;
            }
            else if(LoaderPosition == LOADER_CLOSED)
            {
                pMotorTask->m_TargetPos = m_DrawerClosedPosition;
                pMotorTask->m_Profile  = m_DrawerCloseSpeedMotionProfile;
            }
            FILE_LOG_L(laDEV, llDEBUG) << " CLoaderDevice";
        }
        else
        {
            RetVal = DCL_ERR_NULL_PTR_ACCESS;
            FILE_LOG_L(laDEV, llERROR) << " CLoaderDevice: DCL_ERR_NULL_PTR_ACCESS";
        }
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
 *  \brief   Request the drawers actual position
 *
 *           The function starts an asynchronous request of the motor position.
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
ReturnCode_t  CLoaderDevice::ReqActDrawerPosition()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(LOADER_DEV_TASK_DRAWER_ACTPOS_REQ))
    {
        FILE_LOG_L(laDEV, llDEBUG) << " CLoaderDevice";
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
 *  \brief   Set the loaders blocking state.
 *
 *           The blocking state can be set to 'BLOCKING_STATE_FREE' or 'BLOCKING_STATE_BLOCKED',
 *           a blocked drawler can not be opened until its blocking state sill be set to 'BLOCKING_STATE_FREE'
 *           Blocking state can only be changed if the drawler is in LOADER_CLOSED position
 *
 *  \param   BlockState = The blocking state to be set
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the blocking state was set, otherwise DCL_ERR_FCT_CALL_FAILED
 *  request can be forwarded
 *
 ****************************************************************************/
ReturnCode_t  CLoaderDevice::SetBlockingState(BlockState_t BlockState)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_FAILED;

    if(m_ActLoaderPosition == LOADER_CLOSED && m_TaskID == LOADERDEV_TASKID_FREE)
    {
        m_blockState = BlockState;
        m_bLEDRefresh = true;
        RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Request the loaders RFID data.
 *
 *           The function starts an asynchronous request to the RFID data. The RFID data will be returned by calling
 *           IResponse::LoaderReportRFIDData
 *           The requested channel can be specified with rfidChannelReq, if all channel's data should be returned,
 *           IResponse::LoaderReportRFIDData will be called one time for each of the five channels.
 *
 *  \param   RfidChannelReq = The requested channel (or all channels)
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise DCL_ERR_FCT_CALL_FAILED
 *
 ****************************************************************************/
ReturnCode_t CLoaderDevice::ReqRFIDData(LoaderRFIDChannel_t RfidChannelReq)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(LOADER_DEV_TASK_READ_RFID))
    {
        //device task was activated, forward the request parameter to the corresponding function module task
        FunctionModuleTask* pFunctionModuleTask = 0;
        pFunctionModuleTask = m_DeviceTask[LOADER_DEV_TASK_READ_RFID].m_FctModTaskMap[0];

        if(pFunctionModuleTask)
        {
            if ((RetVal = m_pRFID->SetState(1, GetRFIDIndexFromChannel(RfidChannelReq))) == DCL_ERR_FCT_CALL_SUCCESS)
            {
                pFunctionModuleTask->m_pCANObject = m_pRFID;
                m_ReqRfidChannel = RfidChannelReq;
                FILE_LOG_L(laDEV, llDEBUG) << " CLoaderDevice";
            }
        }
        else
        {
            RetVal = DCL_ERR_NULL_PTR_ACCESS;
            FILE_LOG_L(laDEV, llERROR) << " CLoaderDevice: DCL_ERR_NULL_PTR_ACCESS";
        }
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
 *  \brief   Request the loaders RFID TAG
 *
 *           The function starts an asynchronous request to the RFID TAG. The RFID TAG will be returned by calling
 *           IResponse::LoaderReportRFIDData
 *           The requested channel can be specified with rfidChannelReq, if all channel's data should be returned,
 *           IResponse::LoaderReportRFIDData will be called one time for each of the five channels.
 *
 *  \param   RfidChannelReq = The requested channel (or all channels)
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise DCL_ERR_FCT_CALL_FAILED
 *
 ****************************************************************************/
ReturnCode_t  CLoaderDevice::ReqRFIDTAG(LoaderRFIDChannel_t RfidChannelReq)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(ActivateDeviceTask(LOADER_DEV_TASK_READ_TAG))
    {
        //device task was activated, forward the request parameter to the corresponding function module task
        FunctionModuleTask* pFunctionModuleTask = 0;
        pFunctionModuleTask = m_DeviceTask[LOADER_DEV_TASK_READ_TAG].m_FctModTaskMap[0];

        if(pFunctionModuleTask)
        {
            pFunctionModuleTask->m_pCANObject = m_pRFID;
            m_ReqRfidChannel = RfidChannelReq;
            FILE_LOG_L(laDEV, llDEBUG) << " CLoaderDevice";
        }
        else
        {
            RetVal = DCL_ERR_NULL_PTR_ACCESS;
            FILE_LOG_L(laDEV, llERROR) << " CLoaderDevice: DCL_ERR_NULL_PTR_ACCESS";
        }
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
 *  \brief   Returns the loader's blocking state.
 *
 *
 *  \return  The blocking, which can be 'BLOCKING_STATE_FREE' or 'BLOCKING_STATE_BLOCKED'
 *
 ****************************************************************************/
BlockState_t CLoaderDevice::GetBlockingState()
{
    return m_blockState;
}

/****************************************************************************/
/*!
 *  \brief   Convert motor position [steps] to loader position type
 *
 *  \iparam  motorPos = Motor position to convert
 *
 *  \return  LoaderPosition_t The loader position as converted from motorPos
 *
 ****************************************************************************/
LoaderPosition_t CLoaderDevice::GetDrawerPositionFromMotorPos(Position_t motorPos)
{
    LoaderPosition_t drawerPosition;
    if(motorPos <= m_DrawerClosedPosition + LOADER_MAX_DEVIATION_CLOSED_STATE)
        drawerPosition = LOADER_CLOSED;

    else if(motorPos >= m_DrawerOpenPosition - LOADER_MIN_DEVIATION_OPEN_STATE)
        drawerPosition = LOADER_OPEN;
    else
        drawerPosition = LOADER_UNDEF;

    return drawerPosition;
}

/****************************************************************************/
/*!
 *  \brief   Convert RFID channel type to function module index
 *
 *  \iparam  RfidChannel = RFID channel type
 *
 *  \return  RFID function module index
 *
 ****************************************************************************/
int CLoaderDevice::GetRFIDIndexFromChannel(LoaderRFIDChannel_t RfidChannel)
{
    int rfidChannelIndex = 0;

    switch (RfidChannel)
    {
        case LOADER_ID_RFID_1:
            rfidChannelIndex = 0;
            break;
        case LOADER_ID_RFID_2:
            rfidChannelIndex = 1;
            break;
        case LOADER_ID_RFID_3:
            rfidChannelIndex = 2;
            break;
        case LOADER_ID_RFID_4:
            rfidChannelIndex = 3;
            break;
        case LOADER_ID_RFID_5:
            rfidChannelIndex = 4;
            break;
        default:
            //pfui, at this moment I don't know what I shall do.. !!
            rfidChannelIndex = 0;
            break;
    }

    return rfidChannelIndex;
}

/****************************************************************************/
/*!
 *  \brief   Convert function module index to RFID channel type
 *
 *  \iparam  idx = RFID function module index
 *
 *  \return  RFID channel type
 *
 ****************************************************************************/
LoaderRFIDChannel_t CLoaderDevice::GetChannelFromRFIDIndex(int idx)
{
    LoaderRFIDChannel_t rfidChannel;

    switch (idx)
    {
        case 0:
            rfidChannel = LOADER_ID_RFID_1;
            break;
        case 1:
            rfidChannel = LOADER_ID_RFID_2;
            break;
        case 2:
            rfidChannel = LOADER_ID_RFID_3;
            break;
        case 3:
            rfidChannel = LOADER_ID_RFID_4;
            break;
        case 4:
            rfidChannel = LOADER_ID_RFID_5;
            break;
        default:
            //pfui, at this moment I don't know what I shall do.. !!
            rfidChannel = LOADER_ID_RFID_1;
            break;
    }

    return rfidChannel;
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
bool CLoaderDevice::ActivateDeviceTask(quint8 DeviceTaskIndex)
{
    bool Activated = false;
    FunctionModuleTask* pFunctionModuleTask;

    if((m_TaskID == LOADERDEV_TASKID_FREE) || (m_TaskID == LOADERDEV_TASKID_COMMAND_HDL))
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
            m_TaskID = LOADERDEV_TASKID_COMMAND_HDL;
            Activated = true;
        }
    }

    return Activated;
}

} //namespace
