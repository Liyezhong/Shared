/******************************************************************/
/*! \file RackHandlingDevice.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 14.02.2011
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CRackHandlingDevice
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/******************************************************************/

#include "DeviceControl/Include/Devices/RackHandlingDevice.h"
#include "DeviceControl/Include/Devices/RackHandlingNavigator.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/SystemPaths.h"

#include <QStringList>

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief    Constructor of CRackHandlingDevice
 *
 *
 *  \param    pDeviceProcessing = pointer to DeviceProcessing
 *  \param    Type = Device type string
 *
 *
 ***************************************************************************/
CRackHandlingDevice::CRackHandlingDevice(DeviceProcessing* pDeviceProcessing, QString Type)
    : CBaseDevice(pDeviceProcessing, Type),
    m_pRackHandlingNavigator(0), m_pMotorXAxis(0), m_pMotorYAxis(0), m_pMotorZAxis(0), m_pMotorGrabber(0),
    m_ReqStation(RACKHDL_STATION_UNDEF), m_ReqStationAktion(STATION_ACTION_UNDEF)
{
    m_MainState = DEVICE_MAIN_STATE_START;
    m_MainStateOld = m_MainState;

    // normal operation tasks and states, error state
    m_taskID       = RACKHDL_DEV_TASKID_INIT;
    m_taskState    = RACKHDL_DEV_TASK_STATE_FREE;
    m_errTaskState = RACKHDL_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_RACK_HANDLING;

    m_pRackHandlingNavigator = new CRackHandlingNavigator();


    FILE_LOG_L(laDEV, llINFO) << " create Rackhandling device.";
}

/****************************************************************************/
/*!
 *  \brief    Destructor of CRackHandlingDevice
 *
 *
 ****************************************************************************/
CRackHandlingDevice::~CRackHandlingDevice()
{
    m_pDevProc = 0;
    m_pMotorXAxis = 0;
    m_pMotorYAxis = 0;
    m_pMotorZAxis = 0;
    m_pMotorGrabber = 0;

    try
    {
        if(m_pRackHandlingNavigator)
        {
            delete m_pRackHandlingNavigator;
            m_pRackHandlingNavigator = 0;
        }
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
 */
/****************************************************************************/
void CRackHandlingDevice::HandleTasks()
{
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
        ReturnCode_t RetVal;

        RetVal = HandleInitializationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_CONFIG;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            FILE_LOG_L(laDEV, llERROR) << "   CRackHandlingDevice:HandleInitializationState:" << (int) RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_errTaskState = RACKHDL_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        ReturnCode_t RetVal;

        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_DEV_TASK_CFG;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            FILE_LOG_L(laDEV, llERROR) << "   CRackHandlingDevice:HandleConfigurationState:" << (int) RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_errTaskState = RACKHDL_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_DEV_TASK_CFG)
    {
        ReturnCode_t RetVal;

        RetVal = HandleDeviceTasksConfigurationState();
        FILE_LOG_L(laDEV, llINFO) << "   CRackHandlingDevice::" << (int) RetVal;
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_IDLE;
            m_taskID = RACKHDL_DEV_TASKID_FREE;
        }
    }

    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   CRackHandlingDevice:handleTasks: new state: " << (int) m_MainState;
        m_MainStateOld = m_MainState;
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the classes initialisation state.
 *
 *           This function attaches the function modules to the pointers.
 *
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CRackHandlingDevice::HandleInitializationState()
{
    quint32 InstanceID;
    FILE_LOG_L(laDEV, llINFO) << "  CRackHandlingDevice, InstandeID " << (int) m_instanceID;

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorRackHdlXAxisKey);
    m_pMotorXAxis = (CStepperMotor*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pMotorXAxis == 0)
    {
        // the function module could not be allocated
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, MotorRackHdlXAxis not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorRackHdlYAxisKey);
    m_pMotorYAxis = (CStepperMotor*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pMotorYAxis == 0)
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, MotorRackHdlYAxis not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorRackHdlZAxisKey);
    m_pMotorZAxis = (CStepperMotor*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pMotorZAxis == 0)
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, MotorRackHdlZAxis not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_MotorRackHdlGrabberKey);
    m_pMotorGrabber = (CStepperMotor*) m_pDevProc->GetFunctionModule(InstanceID);
    if(m_pMotorGrabber == 0)
    {
        SetErrorParameter(EVENT_GRP_DCL_COVERLINE_DEV, ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED, (quint16) InstanceID);
        FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, MotorRackHdlGrabber not allocated.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;

}

/****************************************************************************/
/*!
 *  \brief   Handles the classes configuration state.
 *
 *           This function connects each function module's signals to the internal slots.
 *           Furthermore, the station layout will be read.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if configuration was successfully executed
 *           otherwise DCL_ERR_FCT_CALL_FAILED
 *
 ****************************************************************************/
ReturnCode_t CRackHandlingDevice::HandleConfigurationState()
{
    ReturnCode_t RetVal;

    RetVal = ReadStationLayout();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = ReadAdjustment();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            if(!connect(m_pMotorXAxis, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)), this, SLOT(StepperMotorReferenceMovementAckn(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_X_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect X-motor 'ReportReferenceMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorYAxis, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)), this, SLOT(StepperMotorReferenceMovementAckn(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Y_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Y-motor 'ReportReferenceMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorZAxis, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)), this, SLOT(StepperMotorReferenceMovementAckn(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Z_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Z-motor 'ReportReferenceMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }

            if(!connect(m_pMotorGrabber, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)), this, SLOT(StepperMotorReferenceMovementAckn(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Z_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Grabber-motor 'ReportReferenceMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }

            if(!connect(m_pMotorXAxis, SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16)), this, SLOT(StepperMotorMovementAckn(quint32, ReturnCode_t, qint32, qint16))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_X_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect X-motor 'ReportMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorYAxis, SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16)), this, SLOT(StepperMotorMovementAckn(quint32, ReturnCode_t, qint32, qint16))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Y_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Y-motor 'ReportMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorZAxis, SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16)), this, SLOT(StepperMotorMovementAckn(quint32, ReturnCode_t, qint32, qint16))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Z_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Z-motor 'ReportMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorGrabber, SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16)), this, SLOT(StepperMotorMovementAckn(quint32, ReturnCode_t, qint32, qint16))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Z_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Grabber-motor 'ReportMovementAckn' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }

            if(!connect(m_pMotorXAxis, SIGNAL(ReportActPosition(quint32, ReturnCode_t, qint32, qint8)), this, SLOT(StepperMotorPosition(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_X_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect X-motor 'ReportActPosition' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorYAxis, SIGNAL(ReportActPosition(quint32, ReturnCode_t, qint32, qint8)), this, SLOT(StepperMotorPosition(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Y_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Y-motor 'ReportActPosition' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorZAxis, SIGNAL(ReportActPosition(quint32, ReturnCode_t, qint32, qint8)), this, SLOT(StepperMotorPosition(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Z_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Z-motor 'ReportActPosition' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorGrabber, SIGNAL(ReportActPosition(quint32, ReturnCode_t, qint32, qint8)), this, SLOT(StepperMotorPosition(quint32, ReturnCode_t, qint32))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Z_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Grabber-motor 'ReportActPosition' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }

            if(!connect(m_pMotorXAxis, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(StepperMotorError(quint32, quint16, quint16, quint16, QDateTime))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_X_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect x-motor 'ReportError' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorYAxis, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(StepperMotorError(quint32, quint16, quint16, quint16, QDateTime))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Y_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect y-motor 'ReportError' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorZAxis, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(StepperMotorError(quint32, quint16, quint16, quint16, QDateTime))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Z_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect z-motor 'ReportError' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
            if(!connect(m_pMotorGrabber, SIGNAL(ReportError(quint32, quint16, quint16, quint16, QDateTime)), this, SLOT(StepperMotorError(quint32, quint16, quint16, quint16, QDateTime))))
            {
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_MOTOR_Z_AXIS_GRAP1);
                FILE_LOG_L(laDEV, llERROR) << "   Connect Grabber-motor 'ReportError' failed.";
                return DCL_ERR_FCT_CALL_FAILED;
            }
        }
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Initialize and configures the devise tasks
 *
 *   This device has three device tasks
 *     - reference run
 *     - motor position request, get all motors position
 *     - station action, position the grabber and execute the requested action (rack grap/discard)
 *   The function calls the rck handling navigator functions to configure the device tasks
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRackHandlingDevice::HandleDeviceTasksConfigurationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    DeviceTask* pDeviceTask;

    if(m_pRackHandlingNavigator == 0)
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    pDeviceTask = GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, RACKHDL_DEV_TASK_REFRUN);
    m_DeviceTaskMap[RACKHDL_DEV_TASK_REFRUN] = pDeviceTask;
    RetVal = m_pRackHandlingNavigator->CreateReferenceRunFctModTasks(this, pDeviceTask);
    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
    {
        return RetVal;
    }

    pDeviceTask = GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, RACKHDL_DEV_TASK_POS_REQ);
    m_DeviceTaskMap[RACKHDL_DEV_TASK_POS_REQ] = pDeviceTask;
    RetVal = m_pRackHandlingNavigator->CreatePosRequestTasks(this, pDeviceTask);
    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
    {
        return RetVal;
    }

    pDeviceTask = GetNewDeviceTask(DeviceTask::DEV_TASK_STATE_UNUSED, RACKHDL_DEV_TASK_TRANSPORT);
    m_DeviceTaskMap[RACKHDL_DEV_TASK_TRANSPORT] = pDeviceTask;
    RetVal = m_pRackHandlingNavigator->CreateTransportFctModTasks(this, pDeviceTask);
    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
    {
        return RetVal;
    }


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
 */
/****************************************************************************/
void CRackHandlingDevice::HandleIdleState()
{

    if(m_taskID == RACKHDL_DEV_TASKID_FREE)
    {
        ; // there are no pending tasks
    }
    else if(m_taskID == RACKHDL_DEV_TASKID_REFRUN)
    {
        HandleReferenceRunTask();
    }
    else if(m_taskID == RACKHDL_DEV_TASKID_ACTION_REQ)
    {
        HandleActionTask();
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << " Invalid taskID: " << (int) m_taskID;

        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_INVALID_STATE, (quint16) m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = RACKHDL_DEV_ERRTASK_STATE_REPORT_IFACE;
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the reference run task
 *
 *   This task controls the reference run procedure.
 *   The function is called from HandleIdleState() if m_taskID == RACKHDL_DEV_TASKID_REFRUN.
 *   It handles the reference run task, which consists of the following states:
 *
 *     RACKHDL_DEV_TASK_STATE_REQ :
 *        Calls the RackHandlingNavigator::RACKHDL_DEV_TASK_STATE_ERROR function which creates the
 *        commands to execute all motor's reference run in correct order
 *
 *     RACKHDL_DEV_TASK_STATE_PROGRESS_ACTION_STEPS:
 *        Starts the tasks, check timeout to observe the actions, and forwards the acknowledge to
 *        the interface class.
 *
 *     RACKHDL_DEV_TASK_STATE_ERROR:
 *        Forward the error to device processing class by switching to error main state to
 *        Send the task's acknowledge signal, hdlInfo is set from
 *        m_lastErrorHdlInfo, which contains the error code as returned by the
 *        failed code
 *
 *     All other states, which are unexpected, will result in a RACKHDL_DEV_TASK_STATE_ERROR
 *      state to forward the error
 *
 */
/****************************************************************************/
void CRackHandlingDevice::HandleReferenceRunTask()
{
    if((m_pDevProc == 0)/* || (m_pRackHandlingNavigator == 0)*/)
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_NULL_PTR_ACCESS, (quint16)m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = RACKHDL_DEV_ERRTASK_STATE_REPORT_IFACE;
        return;
    }

    if(m_taskState == RACKHDL_DEV_TASK_STATE_REQ)
    {
        ((DeviceTask*) m_DeviceTaskMap[RACKHDL_DEV_TASK_REFRUN])->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
        m_taskState = RACKHDL_DEV_TASK_STATE_PROGRESS_ACTION_STEPS;
        FILE_LOG_L(laDEV, llINFO) << "##########################################################";
        FILE_LOG_L(laDEV, llINFO) << "  set device task 'Reference run' to 'progress'.";
    }
    else if (m_taskState == RACKHDL_DEV_TASK_STATE_PROGRESS_ACTION_STEPS)
    {
        bool bActionStateInProgress = false;
        bool bActionStateWithErrorDetect = false;

        DeviceTask* pDeviceTask;

        pDeviceTask = m_DeviceTaskMap[RACKHDL_DEV_TASK_REFRUN];

        CheckTasksForStartup(bActionStateInProgress, bActionStateWithErrorDetect);

        if(bActionStateWithErrorDetect == true)
        {
            m_taskState = RACKHDL_DEV_TASK_STATE_ERROR;
        }
        else if(bActionStateInProgress == false)
        {
            FunctionModuleTask* pFunctionModuleTask1;
            QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(pDeviceTask->m_FctModTaskMap);

            // set all tasks to state 'unused'
            while (iterFctModTask.hasNext())
            {
                pFunctionModuleTask1 = iterFctModTask.next().value();
                pFunctionModuleTask1->m_TaskState = FunctionModuleTask::FCTMOD_STATE_UNUSED;
            }

            // reset the device's state
            pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task 'Reference run'' to 'finished'.";

            // route the information to interface class
            //m_pIRackHandlingDev->RouteReferenceRunAckn(m_instanceID, m_lastErrorHdlInfo);

            //reset the states
            ResetTask();
        }
    }
    else if(m_taskState == RACKHDL_DEV_TASK_STATE_ERROR)
    {
        FILE_LOG_L(laDEV, llERROR) << "  CRackHandlingDevice: error RACKHDL_DEV_TASKID_REFRUN ";

        //m_pIRackHandlingDev->RouteReferenceRunAckn(m_instanceID, m_lastErrorHdlInfo);

        ResetTask();

        // its a rack handling error, the error was not reported from any function module before
        // report error to device processing
        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = RACKHDL_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else
    {
        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        m_taskState = RACKHDL_DEV_TASK_STATE_ERROR;
        SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_INVALID_STATE, (quint16) m_taskState);
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles all the tasks need to execute a requested action
 *
 *           An action is separated into several tasks:
 *            - three tasks to request the actual motor positions. The actual motor positions
 *              are the base to calculate the movements for this action
 *           When received the positions, call CalculateGrapplerActionTasks to calculate the tasks need to
 *           arrange the requested action. Mostly there are some positioning tasks to transport the grappler to
 *           a vessel.
 *           If an RFID read, or a liquid level read was requested, an RFID- or liquid level sensor data request task will
 *           be added.
 *
 *
 *
 ****************************************************************************/
void CRackHandlingDevice::HandleActionTask()
{
    if((m_pDevProc == 0) || (m_pRackHandlingNavigator == 0))
    {
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_RACKHDL_DEV_NULL_PTR_ACCESS, (quint16)m_taskID);
        ResetTask();

        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = RACKHDL_DEV_ERRTASK_STATE_REPORT_IFACE;
        return;
    }

    if(m_taskState == RACKHDL_DEV_TASK_STATE_REQ)
    {
        // Request the motor positions before starting any movement.
        // Calculate the tasks to send the 'ActPositionReq' CAN message to all motors.
        // The actual position will be need by movement calculation for this action

        m_taskState = RACKHDL_DEV_TASK_STATE_ACTPOS_REQ_SEND;
        ((DeviceTask*) m_DeviceTaskMap[RACKHDL_DEV_TASK_POS_REQ])->m_TaskState = DeviceTask::DEV_TASK_STATE_START;
        ((DeviceTask*) m_DeviceTaskMap[RACKHDL_DEV_TASK_POS_REQ])->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
    }
    else if(m_taskState == RACKHDL_DEV_TASK_STATE_ACTPOS_REQ_SEND)
    {
        // execute the 'position request' device task.
        // the 'ActPositionReq' will be send to each motor, and this state awaits the response CAN messages
        FunctionModuleTask* pFunctionModuleTask = 0;
        bool bActionStateInProgress = false;
        bool bActionStateWithErrorDetect = false;

        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(((DeviceTask*) m_DeviceTaskMap[RACKHDL_DEV_TASK_POS_REQ])->m_FctModTaskMap,
                                                                                         bActionStateInProgress,
                                                                                         bActionStateWithErrorDetect);

        while(pFunctionModuleTask)
        {
            if(pFunctionModuleTask)
            {
                ReturnCode_t RetVal;

                FILE_LOG_L(laDEV, llINFO) << "  CRackHandlingDevice: task for startup found.";
                RetVal = m_pDevProc->GetFunctionModuleTaskManager()->StartTask(pFunctionModuleTask);
                if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_FAILED;
                    m_taskState = RACKHDL_DEV_TASK_STATE_ERROR;
                    SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, EVENT_DCL_RACKHDL_DEV_REQACTPOS_FCT_CALL_FAILED, (quint16) m_taskState);

                    FILE_LOG_L(laDEV, llERROR) << "  CRackHandlingDevice: failed to start task.";
                    return;
                }
            }
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(((DeviceTask*) m_DeviceTaskMap[RACKHDL_DEV_TASK_POS_REQ])->m_FctModTaskMap,
                                                                                             bActionStateInProgress,
                                                                                             bActionStateWithErrorDetect);
        }

        if(bActionStateWithErrorDetect == true)
        {
            FILE_LOG_L(laDEV, llERROR) << "  CRackHandlingDevice: Action state error.";
            m_taskState = RACKHDL_DEV_TASK_STATE_ERROR;
        }
        else if(bActionStateInProgress == false)
        {
            FILE_LOG_L(laDEV, llINFO) << "  CRackHandlingDevice: positions received, next start movement..";

            FunctionModuleTask* pFunctionModuleTask1;
            QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(((DeviceTask*) m_DeviceTaskMap[RACKHDL_DEV_TASK_POS_REQ])->m_FctModTaskMap);

            // set all tasks to state 'unused'
            while (iterFctModTask.hasNext())
            {
                pFunctionModuleTask1 = iterFctModTask.next().value();
                pFunctionModuleTask1->m_TaskState = FunctionModuleTask::FCTMOD_STATE_UNUSED;
            }

            ((DeviceTask*) m_DeviceTaskMap[RACKHDL_DEV_TASK_POS_REQ])->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
            m_taskState = RACKHDL_DEV_TASK_STATE_ACTPOS_REQ_ACKN;
        }
    }
    else if(m_taskState == RACKHDL_DEV_TASK_STATE_ACTPOS_REQ_ACKN)
    {
        // The motor positions have been received, calculate the task to execute the requested transport action.
        ReturnCode_t result;

        // calculate the tasks to execute the requested action
        result = m_pRackHandlingNavigator->CalculateTransportActionTasks(this,
                                                                         ((DeviceTask*) m_DeviceTaskMap[RACKHDL_DEV_TASK_TRANSPORT])->m_FctModTaskMap,
                                                                         m_ReqStation,
                                                                         m_ReqStationAktion);

        if(result == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_taskState = RACKHDL_DEV_TASK_STATE_PROGRESS_ACTION_STEPS;
            ((DeviceTask*) m_DeviceTaskMap[RACKHDL_DEV_TASK_TRANSPORT])->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
        }
        else
        {
            m_lastErrorHdlInfo = result;
            m_taskState = RACKHDL_DEV_TASK_STATE_ERROR;
            SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, EVENT_DCL_RACKHDL_DEV_CALC_TRANSP_FCT_CALL_FAILED, (quint16) m_taskID);

            FILE_LOG_L(laDEV, llERROR) << "   CRackHandlingDevice: error while CalculateGrapplerActionTasks: " << (int) result;
        }
    }
    else if(m_taskState == RACKHDL_DEV_TASK_STATE_PROGRESS_ACTION_STEPS)
    {
        // execute the fuction module tasks stored in the device task list
        FunctionModuleTask* pFunctionModuleTask = 0;
        bool bActionStateInProgress = false;
        bool bActionStateWithErrorDetect = false;

        // check for function module task pending to start
        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(((DeviceTask*) m_DeviceTaskMap[RACKHDL_DEV_TASK_TRANSPORT])->m_FctModTaskMap,
                                                                                         bActionStateInProgress,
                                                                                         bActionStateWithErrorDetect);
        if(pFunctionModuleTask)
        {
            ReturnCode_t RetVal;

            FILE_LOG_L(laDEV, llINFO) << "   task ready to start found.";
            RetVal = m_pDevProc->GetFunctionModuleTaskManager()->StartTask(pFunctionModuleTask);
            if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_lastErrorHdlInfo = RetVal;
                m_taskState = RACKHDL_DEV_TASK_STATE_ERROR;
                SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, EVENT_DCL_RACKHDL_DEV_TRANSP_TASK_START_FCT_CALL_FAILED, (quint16) m_taskState);

                FILE_LOG_L(laDEV, llERROR) << "  CRackHandlingDevice: failed to start task.";
                return;
            }
        }

        if(bActionStateWithErrorDetect == true)
        {
            FILE_LOG_L(laDEV, llINFO) << "   CRackHandlingDevice: an error occured";
            m_taskState = RACKHDL_DEV_TASK_STATE_ERROR;
        }
        else if(bActionStateInProgress == false)
        {
            //the request was finished, reset the states
            FunctionModuleTask* pFunctionModuleTask1;
            QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(((DeviceTask*) m_DeviceTaskMap[RACKHDL_DEV_TASK_TRANSPORT])->m_FctModTaskMap);

            // set all function module tasks to state 'unused'
            while (iterFctModTask.hasNext())
            {
                pFunctionModuleTask1 = iterFctModTask.next().value();
                pFunctionModuleTask1->m_TaskState = FunctionModuleTask::FCTMOD_STATE_UNUSED;
            }
            FILE_LOG_L(laDEV, llINFO) << "   CRackHandlingDevice: we are finished, set all tasks to state 'unused'";

            //reset internal state
            ResetTask();

            //m_pIRackHandlingDev->RoutePositioningAckn(m_instanceID, m_lastErrorHdlInfo);
        }
    }
    else if(m_taskState == RACKHDL_DEV_TASK_STATE_ERROR)
    {
        FILE_LOG_L(laDEV, llERROR) << "   CRackHandlingDevice: error GRAPPLER_DEV_TASKID_ACTION_REQ ";

        //m_pIRackHandlingDev->RoutePositioningAckn(m_instanceID, m_lastErrorHdlInfo);

        ResetTask();

        // its a grappler error, the error was not reported from any function module before
        // report error to device processing
        m_MainState = DEVICE_MAIN_STATE_ERROR;
        m_errTaskState = RACKHDL_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "   CRackHandlingDevice: invalid state: " << (int) m_taskState;
        m_lastErrorHdlInfo = DCL_ERR_INVALID_STATE;
        m_taskState = RACKHDL_DEV_TASK_STATE_ERROR;
        SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, ERROR_DCL_IRACKHDL_DEV_INVALID_STATE, (quint16) m_taskState);
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the error state of the class.
 *
 *   This task controls the error handling of the class.
 *   The function is called from HandleIdleState() if m_MainState == DEVICE_MAIN_STATE_ERROR.
 *   This task consists of the following states:
 *
 *     RACKHDL_DEV_ERRTASK_STATE_REPORT_IFACE:
 *      This state reports the error to the interface class. The interface class will change to error state
 *      and blocks future requests
 *
 *     RACKHDL_DEV_ERRTASK_STATE_REPORT_DEVPROC:
 *      This state reports the error to the device processing class.
 *
 *     RACKHDL_DEV_ERRTASK_STATE_IDLE:
 *      Hold the class in error state
 *
 *     RACKHDL_DEV_ERRTASK_STATE_RESET
 *      Reset the error state, set all states to it's initialisation state
 *
 *  \return   void
 */
/****************************************************************************/
void CRackHandlingDevice::HandleErrorState()
{
    if(m_errTaskState == RACKHDL_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_errTaskState == RACKHDL_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        /*if(m_pIRackHandlingDev != 0)
        {
            m_pIRackHandlingDev->RouteError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }*/
        m_errTaskState = RACKHDL_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_errTaskState == RACKHDL_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_errTaskState = RACKHDL_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_errTaskState == RACKHDL_DEV_ERRTASK_STATE_RESET)
    {
        ResetTask();
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;

        // reset the function module references
        m_pMotorXAxis = 0;
        m_pMotorYAxis = 0;
        m_pMotorZAxis = 0;
        m_pMotorGrabber = 0;
    }
}

/****************************************************************************/
/*!
 *  \brief    Checks, if there are function module task pending for beeing started, and start them
 *
 *  \iparam   bActionStateInProgress =
 *  \iparam   bActionStateWithErrorDetect =
 *  \return   void
 */
/****************************************************************************/
void CRackHandlingDevice::CheckTasksForStartup(bool& bActionStateInProgress, bool& bActionStateWithErrorDetect)
{
    FunctionModuleTask* pFunctionModuleTask = 0;

    //search for function module tasks which are ready to be started
    QMapIterator<quint16, DeviceTask*> iterDeviceTask(m_DeviceTaskMap);
    DeviceTask*         pDeviceTask;

    //loop thru the device task map
    while(iterDeviceTask.hasNext())
    {
        pDeviceTask = iterDeviceTask.next().value();

        //only tasks with state 'progress' have function module tasks to startup
        if(pDeviceTask->m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS)
        {
            // get a task to start up, if any
            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(pDeviceTask->m_FctModTaskMap,
                                                                                             bActionStateInProgress,
                                                                                             bActionStateWithErrorDetect);
            while(pFunctionModuleTask)
            {
                ReturnCode_t RetVal;

                //bringt der auch error tasks? Ja
                if(bActionStateWithErrorDetect == true)
                {
                    m_taskState = RACKHDL_DEV_TASK_STATE_ERROR;
                    FILE_LOG_L(laDEV, llERROR) << "  CCoverLineDevice: task with error state:" << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();
                }

                FILE_LOG_L(laDEV, llINFO) << "  CCoverLineDevice: task for startup found.";
                RetVal = m_pDevProc->GetFunctionModuleTaskManager()->StartTask(pFunctionModuleTask);
                if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
                {
                    FILE_LOG_L(laDEV, llERROR) << "  CCoverLineDevice: failed to start task.";
                }

                // get the nest task to start up, if any
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(pDeviceTask->m_FctModTaskMap,
                                                                                                 bActionStateInProgress,
                                                                                                 bActionStateWithErrorDetect);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot will be called from CANStepperMoter if a reference run was acknowledged
 *
 *
 *  \iparam   instanceID = instance identifier of the CANStepperMotor
 *  \iparam   hdlInfo = return code
 *  \iparam   position = The actual motor position
 *
 */
/****************************************************************************/
void CRackHandlingDevice::StepperMotorReferenceMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position)
{
    FunctionModuleTask*   pFunctionModuleTask = 0;
    CANStepperMotorTask*  pCANStepperMotorTask;
    int DeviceTaskIndex;

    FILE_LOG_L(laDEV, llINFO) << "  CRackHandlingDevice InstanceID  0x" << std::hex << instanceID;

    pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    if(pFunctionModuleTask == 0)
    {
        //is this an error?
        FILE_LOG_L(laDEV, llINFO) << " no task found.";
        return;
    }

    while(pFunctionModuleTask)
    {
        FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState;
        Q_ASSERT(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR);

        if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
        {
            if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
            {
                pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
                if(pCANStepperMotorTask)
                {
                    pCANStepperMotorTask->m_ActPos = position;

                    ForwardTaskToDeviceTaskHandler(DeviceTaskIndex, pFunctionModuleTask);
                }
            }
            else
            {
                // error was detected from motor side
                FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while driveToPosition.. error";
                pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
            }
            break;
        }
        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot will be called from CANStepperMotor if a movement was acknowledged
 *
 *
 *  \iparam   instanceID = instance identifier of the CANStepperMotor
 *  \iparam   HdlInfo = return code
 *  \iparam   Position = The actual motor position
 *  \iparam   Speed = Actual motor speed
 *
 */
/****************************************************************************/
void CRackHandlingDevice::StepperMotorMovementAckn(quint32 instanceID, ReturnCode_t HdlInfo, qint32 Position, qint16 Speed)
{
    FunctionModuleTask* pFunctionModuleTask = 0;
    CANStepperMotorTask* pCANStepperMotorTask;
    int DeviceTaskIndex;

    FILE_LOG_L(laDEV, llINFO) << "  CRackHandlingDevice, instanceID 0x" << std::hex << instanceID;

    pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    if(pFunctionModuleTask == 0)
    {
        //this is suspicious.., log the function module task map

        FILE_LOG_L(laDEV, llWARNING) << " CRackHandlingDevice, no task found.";

    }

    while(pFunctionModuleTask)
    {
        FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState << " key: " << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();
        Q_ASSERT(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR);

        if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
        {
            if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
            {
                pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
                if(pCANStepperMotorTask)
                {
                    pCANStepperMotorTask->m_ActPos = Position;
                    pCANStepperMotorTask->m_ActSpeed = Speed;

                    ForwardTaskToDeviceTaskHandler(DeviceTaskIndex, pFunctionModuleTask);
                }
            }
            else
            {
                // error was detected from motor side
                FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while driveToPosition.. error";
                pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
            }
            //break; no, there are multible task, maybe
        }

        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
        if(!pFunctionModuleTask)
        {
            FILE_LOG_L(laDEV, llDEBUG) << " GetTaskFromInstanceID " << std::hex << instanceID << " return 0 -> exit";
        }
    }    
}

/****************************************************************************/
/*!
 *  \brief    This slot receives the actual position from CANStepperMotor
 *
 *
 *  \iparam   instanceID = instance identifier of the CANStepperMotor
 *  \iparam   hdlInfo = return code
 *  \iparam   position = The actual motor position
 *
 */
/****************************************************************************/
void CRackHandlingDevice::StepperMotorPosition(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position)
{
    CANStepperMotorTask* pCANStepperMotorTask;
    FunctionModuleTask* pFunctionModuleTask = 0;
    int DeviceTaskIndex;

    Q_UNUSED(position);

    FILE_LOG_L(laDEV, llINFO) << "  CRackHandlingDevice, InstanceID 0x" << std::hex << instanceID;

    pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    if(pFunctionModuleTask == 0)
    {
        FILE_LOG_L(laDEV, llINFO) << " no task found.";
    }

    while(pFunctionModuleTask)
    {
        FILE_LOG_L(laDEV, llINFO) << "  found task with state: " << (int) pFunctionModuleTask->m_TaskState << " key: " << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();

        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);

        if((pCANStepperMotorTask != 0) &&
           (pCANStepperMotorTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS))
        {
            if(hdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
            {
                FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pCANStepperMotorTask->m_TaskIndex << " state to finished.";
                pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
            }
            else
            {
                // error was detected from motor side
                FILE_LOG_L(laDEV, llERROR) << " CANStepperMotor: while driveToPosition.. error";
                pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
            }
            break;
        }
        pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetTaskFromInstanceID(m_DeviceTaskMap, instanceID, pFunctionModuleTask, DeviceTaskIndex);
    }
}

/****************************************************************************/
/*!
 *  \brief   This slot receives the motor's error signal
 *
 *           This function will be called from CANStepperMotor and forwards the
 *           motor's error information.
 *           The function is called if an error was detected by the slave's function module.
 *
 *  \iparam   InstanceID = instance identifier of the device
 *  \iparam   ErrorGroup = EVENT_GRP_DCL_FCTMOD_MOTOR_SLV
 *  \iparam   ErrorCode  = from CAN-message
 *  \iparam   ErrorData  = from CAN-message
 *  \iparam   ErrorTime  = error message reception time on master side
 *
 ****************************************************************************/
void CRackHandlingDevice::StepperMotorError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    Q_UNUSED(InstanceID);
    SetErrorParameter(ErrorGroup, ErrorCode, ErrorData);
    m_lastErrorTime = ErrorTime;
    m_MainState = DEVICE_MAIN_STATE_ERROR;
    m_errTaskState = RACKHDL_DEV_ERRTASK_STATE_REPORT_IFACE;
}

/****************************************************************************/
/*!
 *  \brief    Forwards the slot information of a function module's task to the device task handler
 *
 *
 *  \iparam   DeviceTaskIndex = device task index
 *  \iparam   pFunctionModuleTask = function module task
 *
 */
/****************************************************************************/
void CRackHandlingDevice::ForwardTaskToDeviceTaskHandler(int DeviceTaskIndex, FunctionModuleTask* pFunctionModuleTask)
{
    DeviceTask* pDeviceTask;

    Q_UNUSED(pFunctionModuleTask);

    pDeviceTask = m_DeviceTaskMap[DeviceTaskIndex];

    FILE_LOG_L(laDEV, llINFO) << "  DeviceTaskIndex: " << DeviceTaskIndex;
    if(m_pRackHandlingNavigator)
    {
        if(DeviceTaskIndex == RACKHDL_DEV_TASK_REFRUN)
        {
            m_pRackHandlingNavigator->HandleRefRunTaskEvent(pFunctionModuleTask, pDeviceTask);
        }
        else if(DeviceTaskIndex == RACKHDL_DEV_TASK_POS_REQ)
        {
            m_pRackHandlingNavigator->HandleRackHandlingTaskEvent(pFunctionModuleTask, pDeviceTask);
        }
        else if(DeviceTaskIndex == RACKHDL_DEV_TASK_TRANSPORT)
        {
            m_pRackHandlingNavigator->HandleTransportActionTaskEvent(pFunctionModuleTask, pDeviceTask);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Request the rack handling reference run
 *
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted, otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRackHandlingDevice::ReqReferenceRun()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    if((m_taskID == RACKHDL_DEV_TASKID_FREE) && (m_taskState == RACKHDL_DEV_TASK_STATE_FREE))
    {
        m_taskID = RACKHDL_DEV_TASKID_REFRUN;
        m_taskState = RACKHDL_DEV_TASK_STATE_REQ;
        FILE_LOG_L(laDEV, llINFO) << "    CRackHandlingDevice::ReqReferenceRun ";
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "   CRackHandlingDevice::ReqReferenceRun failed, taskID:" << (int) m_taskID <<" taskState:" << (int) m_taskState;
        retval = DCL_ERR_INVALID_STATE;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief    Request a rack handling action
 *
 *  An action can be one of:
 *   - position with grab or discard a rack
 *
 *  \iparam   RackHdlStation = Rarget station
 *  \iparam   StationAktion = Action, which should be executed at the station
 *
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted, otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRackHandlingDevice::ReqTransportAction(RackHdlStation_t RackHdlStation, StationAction_t StationAktion)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    if(m_taskID == RACKHDL_DEV_TASKID_FREE)
    {
        m_ReqStation       = RackHdlStation;
        m_ReqStationAktion = StationAktion;
        m_taskID = RACKHDL_DEV_TASKID_ACTION_REQ;
        m_taskState = RACKHDL_DEV_TASK_STATE_REQ;
        FILE_LOG_L(laDEV, llINFO) << "   CRackHandlingDevice::ReqStationAction: " << (int) m_ReqStation << ", action:" << (int) m_ReqStationAktion;
    }
    else
    {
        FILE_LOG_L(laDEV, llERROR) << "   CRackHandlingDevice::ReqStationAction failed, taskID:" << (int) m_taskID <<" taskState:" << (int) m_taskState;
        retval = DCL_ERR_INVALID_STATE;
    }

    return retval;
}

/****************************************************************************/
/*!
 *  \brief    Reset the task state machine
 *
 *
 *
 *  \return   void
 */
/****************************************************************************/
void CRackHandlingDevice::ResetTask()
{
    m_taskID = RACKHDL_DEV_TASKID_FREE;
    m_taskState = RACKHDL_DEV_TASK_STATE_FREE;
}

/****************************************************************************/
/*!
 *  \brief    Get the station position for motor movement
 *
 *  \iparam   StationKey = Target station key string
 *  \iparam   StationPosX = X-axis motor position
 *  \iparam   StationPosY = Y-axis motor position
 *  \iparam   StationPosZ = Z-axis motor position
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted, otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRackHandlingDevice::GetStationPositions(RackHdlStation_t StationKey, Position_t& StationPosX, Position_t& StationPosY, Position_t& StationPosZ)

{
    RackHdlStationPos* pRackHdlStationPos;

    pRackHdlStationPos = m_MapStationPos[StationKey];
    if(pRackHdlStationPos)
    {
        StationPosX = pRackHdlStationPos->PositionX;
        StationPosY = pRackHdlStationPos->PositionY;
        StationPosZ = pRackHdlStationPos->PositionZ;
    }
    else
    {
        return DCL_ERR_INVALID_PARAM;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;

}

/****************************************************************************/
/*!
 *  \brief    Read the rack handling's station layout data from file
 *
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted, otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRackHandlingDevice::ReadStationLayout()
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    QString errorStr;
    QString errorMsg;
    int errorLine;
    int errorColumn;

    QDomDocument domDocument;
    QString fileName = Global::SystemPaths::Instance().GetSettingsPath() + "/StationLayout.xml";

    FILE_LOG_L(laCONFIG_FILE, llINFO) <<  "  read station layout data for rack handling " << std::hex << (int) m_instanceID <<
                                          " from '" << fileName.toStdString() << "'";

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        errorMsg = QObject::tr(" Cannot read file %1:\n%2.")
                                .arg(fileName)
                                .arg(file.errorString());
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_LAYOUT_READ_FAILED, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,&errorColumn))
    {
        errorMsg = QObject::tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr);
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_LAYOUT_PARSE_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "sepia_station_layout")
    {
        errorMsg = QObject::tr("The file is not a sepia station layout file. Tag 'station_layout' missed!");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();
        FILE_LOG_L(laCONFIG_FILE, llERROR) << "   '" << root.tagName().toStdString() << "'";

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_LAYOUT_DOCTYPE_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }
    else if (root.hasAttribute("version")
              && root.attribute("version") != "1.0")
    {
        errorMsg = QObject::tr("The sepia station layout file version is not valid.");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_LAYOUT_VERSION_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //##############################
    // station layout data
    //##############################
    QDomElement childStationLayout = root.firstChildElement("station_layout");
    if(childStationLayout.isNull())
    {
        errorMsg = QObject::tr("  child 'station_layout' not found.");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_LAYOUT_FORMAT_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        QDomNode nodeStation;
        QDomNode nodeStationKey, nodeStationPosX, nodeStationPosY, nodeStationPosZ;
        QDomNamedNodeMap attrmapStations;

        QString strStationKey, strStationPosX, strStationPosY, strStationPosZ;
        Position_t StationPosX, StationPosY, StationPosZ;
        RackHdlStation_t RackHdlStationType;
        bool ok;

        nodeStation = childStationLayout.firstChildElement("station");
        FILE_LOG_L(laCONFIG_FILE, llINFO) <<  "row found.";
        while (!nodeStation.isNull())
        {
            FILE_LOG_L(laDEV, llDEBUG) << " read station postion.";
            attrmapStations = nodeStation.attributes();
            nodeStationKey = attrmapStations.namedItem("key");
            nodeStationPosX = attrmapStations.namedItem("pos_x");
            nodeStationPosY = attrmapStations.namedItem("pos_y");
            nodeStationPosZ = attrmapStations.namedItem("pos_z");

            strStationKey = nodeStationKey.nodeValue();
            strStationPosX = nodeStationPosX.nodeValue();
            strStationPosY = nodeStationPosY.nodeValue();
            strStationPosZ = nodeStationPosZ.nodeValue();

            StationPosX = strStationPosX.toShort(&ok, 10);
            StationPosY = strStationPosY.toShort(&ok, 10);
            StationPosZ = strStationPosZ.toShort(&ok, 10);

            RackHdlStationPos* pRackHdlStationPos;
            pRackHdlStationPos = new RackHdlStationPos();
            pRackHdlStationPos->Key = strStationKey;
            pRackHdlStationPos->PositionX = StationPosX;
            pRackHdlStationPos->PositionY = StationPosY;
            pRackHdlStationPos->PositionZ = StationPosZ;

            //Get the rack handling station type variable by converting the string stored at xml file
            RackHdlStationType = GetRackHdlStationTypeFromKey(pRackHdlStationPos->Key);

            // store station data at the map
            m_MapStationPos[RackHdlStationType] = pRackHdlStationPos;

            nodeStation = nodeStation.nextSiblingElement("station");
        }

        if(m_MapStationPos.count() < 8)
        {
            errorMsg = QObject::tr("  invalid format, data missed.");
            FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

            SetErrorParameter(EVENT_GRP_DCL_RACKHDL_DEV, EVENT_DCL_RACKHDL_DEV_ADJUSTMENT_FORMAT_ERROR, m_MapStationPos.count());
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);

            return DCL_ERR_FCT_CALL_FAILED;
        }
    }

    return retCode;

}

/****************************************************************************/
/*!
 *  \brief    Read the grappler's adjustment data from file
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the request was accepted, otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CRackHandlingDevice::ReadAdjustment()
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    /*QString errorStr;
    QString errorMsg;
    int errorLine;
    int errorColumn;

    QDomDocument domDocument;
    QString fileName = Global::SystemPaths::Instance().GetSettingsPath() + "/adjustment.xml";

    FILE_LOG_L(laCONFIG_FILE, llINFO) <<  "  read adjustment data for grappler " << std::hex << (int) m_instanceID <<
                                          " from '" << fileName.toStdString() << "'";

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        errorMsg = QObject::tr(" Cannot read file %1:\n%2.")
                                .arg(fileName)
                                .arg(file.errorString());
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_READ_FAILED, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,&errorColumn))
    {
        errorMsg = QObject::tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr);
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_PARSE_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "colorado_adjustment")
    {
        errorMsg = QObject::tr("The file is not a colorado adjustment file. Tag 'colorado_adjustment' missed!");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();
        FILE_LOG_L(laCONFIG_FILE, llERROR) << "   '" << root.tagName().toStdString() << "'";

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_DOCTYPE_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }
    else if (root.hasAttribute("version")
              && root.attribute("version") != "1.0")
    {
        errorMsg = QObject::tr("The colorado adjustment file version is not valid.");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_VERSION_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //##############################
    // adjustment_data
    //##############################
    QDomElement child = root.firstChildElement("adjustment_data");
    if(child.isNull())
    {
        errorMsg = QObject::tr("  child 'adjustment_data' not found.");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_FORMAT_ERROR, 0);
        m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        return DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        QDomNode nodeGrappler;
        QDomNode nodeRowIndex, nodePosIndex, nodeRowAdjust, nodeColumnAdjust;
        QDomNamedNodeMap attrmapNodeRow;
        QDomNamedNodeMap attrmapNodeVessel;
        QString strRowIndex, strPosIndex, strRowAdjust, strColumnAdjust;
        int posXAdjust, posYAdjust;
        int rowCount = 0, vesselCount = 0;
        short rowIndex, posIndex;;
        bool ok;

        if(m_instanceID == DEVICE_INSTANCE_ID_GRAPPLER_1)
        {
            FILE_LOG_L(laDEV, llDEBUG4) << " open grappler_left";
            nodeGrappler = child.firstChildElement("grappler_left");
        }
        else
        {
            FILE_LOG_L(laDEV, llDEBUG4) << " open grappler_right";
            nodeGrappler = child.firstChildElement("grappler_right");
        }

        if(!nodeGrappler.isNull())
        {
            QDomNode nodeRow = nodeGrappler.firstChildElement("row");
            FILE_LOG_L(laCONFIG_FILE, llDEBUG4) <<  "node base_module found.";
            while (!nodeRow.isNull())
            {
                rowCount++;

                FILE_LOG_L(laDEV, llDEBUG4) << " open row.";
                attrmapNodeRow = nodeRow.attributes();
                nodeRowIndex = attrmapNodeRow.namedItem("index");
                strRowIndex = nodeRowIndex.nodeValue();
                rowIndex = strRowIndex.toShort(&ok, 10);
                rowIndex -= 1;

                QDomNode nodeVessel = nodeRow.firstChildElement("vessel");
                FILE_LOG_L(laCONFIG_FILE, llDEBUG4) <<  "node vessel found.";
                while (!nodeVessel.isNull())
                {
                    vesselCount++;
                    attrmapNodeVessel = nodeVessel.attributes();

                    nodePosIndex = attrmapNodeVessel.namedItem("pos");
                    strPosIndex = nodePosIndex.nodeValue();
                    posIndex = strPosIndex.toShort(&ok, 10);
                    posIndex -= 1;

                    nodeRowAdjust = attrmapNodeVessel.namedItem("row");
                    strRowAdjust = nodeRowAdjust.nodeValue();
                    posYAdjust = strRowAdjust.toInt(&ok, 10);

                    nodeColumnAdjust = attrmapNodeVessel.namedItem("column");
                    strColumnAdjust = nodeColumnAdjust.nodeValue();
                    posXAdjust = strColumnAdjust.toInt(&ok, 10);

                    m_stationAdjust[rowIndex][posIndex].positionX = posXAdjust;
                    m_stationAdjust[rowIndex][posIndex].positionY = posYAdjust;

                    FILE_LOG_L(laDEV, llDEBUG4) << " at pos " << rowIndex << ", " << posIndex << ": row/column data: " <<
                                     m_stationAdjust[rowIndex][posIndex].positionX << " / " << m_stationAdjust[rowIndex][posIndex].positionY;

                    nodeVessel = nodeVessel.nextSiblingElement("vessel");
                }

                nodeRow = nodeRow.nextSiblingElement("row");
            }

            if((rowCount != 4) || (vesselCount != 4*18))
            {
                errorMsg = QObject::tr("  invalid format, row- or vessel count invalid.");
                FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

                SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_FORMAT_ERROR, vesselCount);
                m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);

                return DCL_ERR_FCT_CALL_FAILED;
            }
        }
        else
        {
            errorMsg = QObject::tr("  child 'grappler_' not found.");
            FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

            SetErrorParameter(EVENT_GRP_DCL_GRAPPLER_DEV, EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_FORMAT_ERROR, 2);
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);

            return DCL_ERR_FCT_CALL_FAILED;
        }
    }*/


    return retCode;

}

/****************************************************************************/
/*!
 *  \brief    Returns the x-y-position for the specified station
 *
 *   The station is specified by the RackHdlStation_t type. The function returns the x-y-z position used by the motor axis
 *   to position at this station.
 *   The positions will be returned in motor half stepps
 *   The positions are calculated by adding the stations base position and its adjustment datas
 *
 *  \iparam   Station = The target station
 *  \iparam   MotorPosXAxis = X-axis motor position in half steps
 *  \oparam   MotorPosYAxis = Y-axis motor position in half steps
 *  \oparam   MotorPosZAxis = Z-axis motor position in half steps
 *  \oparam   MotorPosGrabber = Grapper-axis motor position in half steps
 *
 */
/****************************************************************************/
void CRackHandlingDevice::GetMotorPositionsFromStation(RackHdlStation_t Station, Position_t& MotorPosXAxis, Position_t& MotorPosYAxis, Position_t& MotorPosZAxis, Position_t& MotorPosGrabber)
{
    Q_UNUSED(Station);
    MotorPosXAxis = 0;
    MotorPosYAxis = 0;
    MotorPosZAxis = 0;
    MotorPosGrabber = 0;

}

/****************************************************************************/
/*!
 *  \brief    Returns the rack handling station type from the key
 *
 *            The function impelements the Lookup table for 'key' to 'RackHdlStation_t' convertion
 *
 *  \iparam   Key = string containing the requested rack handling station
 *
 *  \return   rack handling station type
 */
/****************************************************************************/
RackHdlStation_t CRackHandlingDevice::GetRackHdlStationTypeFromKey(QString Key)
{
    RackHdlStation_t RackHdlStation;

    if(Key == "transfer")
    {
        RackHdlStation = RACKHDL_STATION_TRANSFER;
    }
    else if(Key == "loader_1")
    {
        RackHdlStation = RACKHDL_STATION_LOADER_1;}
    else if(Key == "loader_2")
    {
        RackHdlStation = RACKHDL_STATION_LOADER_2;
    }
    else if(Key == "rotation_input")
    {
        RackHdlStation = RACKHDL_STATION_ROTATION_INSERT;
    }
    else if(Key == "rotation_output")
    {
        RackHdlStation = RACKHDL_STATION_ROTATION_REMOVE;
    }
    else if(Key == "coverline_1")
    {
        RackHdlStation = RACKHDL_STATION_COVER_LINE_1;
    }
    else if(Key == "coverline_2")
    {
        RackHdlStation = RACKHDL_STATION_COVER_LINE_2;
    }
    else if(Key == "oven")
    {
        RackHdlStation = RACKHDL_STATION_OVEN;
    }
    else if(Key == "unloader_1")
    {
        RackHdlStation = RACKHDL_STATION_UNLOADER_1;
    }
    else if(Key == "unloader_2")
    {
        RackHdlStation = RACKHDL_STATION_UNLOADER_2;
    }
    else if(Key == "unloader_3")
    {
        RackHdlStation = RACKHDL_STATION_UNLOADER_3;
    }
    else
    {
        RackHdlStation = RACKHDL_STATION_UNDEF;
    }

    return RackHdlStation;

}

} //namespace
