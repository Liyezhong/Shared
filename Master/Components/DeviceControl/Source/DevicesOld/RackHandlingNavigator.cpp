/******************************************************************/
/*! \file RackHandlingNavigator.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 15.02.2011
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the CCoverLineNavigator class
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

#include "DeviceControl/Include/Devices/RackHandlingNavigator.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

#define MOTOR_POS_GRABBER_GRAB     600   //!< Grabber motor position 'grab'
#define MOTOR_POS_GRABBER_DISCARD   40   //!< Grabber motor position 'open'
#define MOTOR_POS_Y_BASIS           50   //!< y-axis basisr motor position

/****************************************************************************/
/*!
 *  \brief    Constructor of CRackHandlingNavigator
 *
 *
 *  \return
 *
 ***************************************************************************/
CRackHandlingNavigator::CRackHandlingNavigator()
{
}

/****************************************************************************/
/*!
 *  \brief    Destructor of CRackHandlingNavigator
 *
 *
 *  \return
 *
 ****************************************************************************/
CRackHandlingNavigator::~CRackHandlingNavigator()
{

}

/****************************************************************************/
/*!
 *  \brief    Calculate the tasks need to execute a reference run of all cover line motors
 *
 *            The motors reference run is done in the following order:
 *             - y-axis
 *             - z-axis
 *             - x-axis
 *             - grabber axis
 *
 *           \todo check the reference run preconditions:
 *
 *
 *  \iparam   pCoverLine = Cover line device
 *  \iparam   pDeviceTask = device task for this action
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *            otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CRackHandlingNavigator::CreateReferenceRunFctModTasks(CRackHandlingDevice* pRackHdlDev, DeviceTask* pDeviceTask)
{
    /// \todo Reihenfolge der Referenzfahrten bestimmen. Irgendwas hackt immer...
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CANStepperMotorTask* pCANStepperMotorTask;

    // reference run y-axis
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_REFRUN_FCT_TASK_Y_AXIS;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_REFRUN_FCT_TASK_Y_AXIS;
    pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorYAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // reference run z-axis
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_REFRUN_FCT_TASK_Z_AXIS;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_REFRUN_FCT_TASK_Y_AXIS;
    pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorZAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // reference run x-axis
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_REFRUN_FCT_TASK_X_AXIS;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_REFRUN_FCT_TASK_Z_AXIS;
    pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorXAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // reference run grabber
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_REFRUN_FCT_TASK_GRABBER;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_REFRUN_FCT_TASK_X_AXIS;
    pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorGrabber;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    FunctionModuleTask* pFunctionModuleTask;
    QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(pDeviceTask->m_FctModTaskMap);
    while (iterFctModTask.hasNext())
    {
        pFunctionModuleTask = iterFctModTask.next().value();
        pFunctionModuleTask->m_pDeviceTask = pDeviceTask;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Create the function module tasks need to request the actual motor positions
 *
 *
 *
 *  \iparam   pRackHdlDev = Cover line device
 *  \iparam   pDeviceTask = device task for this action
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *            otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CRackHandlingNavigator::CreatePosRequestTasks(CRackHandlingDevice* pRackHdlDev, DeviceTask* pDeviceTask)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CANStepperMotorTask* pCANStepperMotorTask;

    // reference run y-axis
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_POSREQ_FCT_TASK_X_AXIS;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_POSREQ_FCT_TASK_X_AXIS;
    pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorXAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 1000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // reference run z-axis
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_POSREQ_FCT_TASK_Y_AXIS;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_POSREQ_FCT_TASK_X_AXIS;
    pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorYAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 1000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // reference run x-axis
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_POSREQ_FCT_TASK_Z_AXIS;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_POSREQ_FCT_TASK_X_AXIS;
    pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorZAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 1000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // reference run grabber
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_POSREQ_FCT_TASK_GRABBER_AXIS;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_POSREQ_FCT_TASK_X_AXIS;
    pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorGrabber;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 1000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    FunctionModuleTask* pFunctionModuleTask;
    QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(pDeviceTask->m_FctModTaskMap);
    while (iterFctModTask.hasNext())
    {
        pFunctionModuleTask = iterFctModTask.next().value();
        pFunctionModuleTask->m_pDeviceTask = pDeviceTask;
    }

    return RetVal;

}

/****************************************************************************/
/*!
 *  \brief    Create the function module tasks need to execute a transport sequence
 *
 *
 *
 *  \iparam   pRackHdlDev = Cover line device
 *  \iparam   pDeviceTask = device task for this action
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *            otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CRackHandlingNavigator::CreateTransportFctModTasks(CRackHandlingDevice* pRackHdlDev, DeviceTask* pDeviceTask)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CANStepperMotorTask* pCANStepperMotorTask;

    Q_UNUSED(pRackHdlDev);

    FILE_LOG_L(laDEV, llINFO) << "  create CANStepperMotorTasks";

    /// \todo replace 10 with define
    for(quint8 idx = 0; idx < 10; idx++)
    {
        pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_UNDEF);
        pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_UNUSED;
        pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_UNDEF;
        pCANStepperMotorTask->m_TaskIndex = FunctionModuleTask::TaskIndexUndef;
        pCANStepperMotorTask->m_TriggerTaskIndex = FunctionModuleTask::TaskIndexUndef;;
        pCANStepperMotorTask->m_pCANObject = 0;
        pCANStepperMotorTask->m_TargetPos = 0;
        pCANStepperMotorTask->m_Timeout = 0;
        pDeviceTask->m_FctModTaskMap[idx] = pCANStepperMotorTask;
    }
    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Calculate the tasks need to execute a rack position sequence
 *
 *
 *
 *  \iparam   pRackHdlDev = Cover line device
 *  \iparam   taskMap = device task for this action
 *  \iparam   m_ReqStation
 *  \iparam   m_ReqStationAktion
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *            otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CRackHandlingNavigator::CalculateTransportActionTasks(CRackHandlingDevice* pRackHdlDev,
                                                                  FunctionModuleTaskMap& taskMap,
                                                                  RackHdlStation_t reqStation,
                                                                  StationAction_t  reqStationAktion)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    FunctionModuleTask* pFunctionModuleTask;
    CANStepperMotorTask* pCANStepperMotorTask;

    Position_t     nextStationPosX;
    Position_t     nextStationPosY;
    Position_t     nextStationPosZ;
    Position_t     nextStationPosGrabber = MOTOR_POS_GRABBER_GRAB;
    /*Position_t     ActMotorPosX;
    Position_t     ActMotorPosY;
    Position_t     ActMotorPosZ;
    Position_t     ActMotorPosGrabber;*/

    quint8  TaskIndex = 1;

    if((pRackHdlDev == 0) ||
       (pRackHdlDev->m_pMotorXAxis == 0) ||
       (pRackHdlDev->m_pMotorYAxis == 0) ||
       (pRackHdlDev->m_pMotorZAxis == 0) ||
       (pRackHdlDev->m_pMotorGrabber == 0))
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    FILE_LOG_L(laDEV, llINFO) << "  CRackHandlingNavigator: station / action :" << (int) reqStation << ", " << (int) reqStationAktion;

    RetVal = pRackHdlDev->GetStationPositions(reqStation, nextStationPosX, nextStationPosY, nextStationPosZ);
    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
    {
        return RetVal;
    }

    if(reqStationAktion == STATION_ACTION_ATTACH_TO_RACK)
    {
        nextStationPosGrabber = MOTOR_POS_GRABBER_GRAB;
    }
    else //if(reqStationAktion == STATION_ACTION_DISCARD)
    {
        nextStationPosGrabber = MOTOR_POS_GRABBER_DISCARD;
    }

    FILE_LOG_L(laDEV, llINFO) << "  CRackHandlingNavigator: X, Y, Z, grabber position:" << nextStationPosX << ", " << nextStationPosY << ", " << nextStationPosZ << ", " << nextStationPosGrabber;

    /*ActMotorPosX = pRackHdlDev->m_pMotorXAxis->GetActPosition();
    ActMotorPosY = pRackHdlDev->m_pMotorYAxis->GetActPosition();
    ActMotorPosZ = pRackHdlDev->m_pMotorZAxis->GetActPosition();
    ActMotorPosGrabber = pRackHdlDev->m_pMotorGrabber->GetActPosition();*/

    if(reqStationAktion == STATION_ACTION_ATTACH_TO_RACK)
    {
        //open grabber
        pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(taskMap,
                                                                           FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                           FunctionModuleTask::FCTMOD_STATE_UNUSED);
        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pCANStepperMotorTask == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
        pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
        pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
        pCANStepperMotorTask->m_TaskIndex = TaskIndex;
        pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
        pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorGrabber;
        pCANStepperMotorTask->m_TargetPos = MOTOR_POS_GRABBER_DISCARD;
        pCANStepperMotorTask->m_Timeout = 5000;
        TaskIndex++;

        FILE_LOG_L(laDEV, llINFO) << " add: open grabber: " << pCANStepperMotorTask->m_TargetPos << ", task " << (int) pCANStepperMotorTask->m_TaskIndex;
    }

    //########################################
    //move y-axis to basis
    pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(taskMap,
                                                                       FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                       FunctionModuleTask::FCTMOD_STATE_UNUSED);
    pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
    if(pCANStepperMotorTask == 0)
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }
    pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
    pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorYAxis;
    pCANStepperMotorTask->m_TargetPos = MOTOR_POS_Y_BASIS;
    pCANStepperMotorTask->m_Timeout = 5000;
    TaskIndex++;

    FILE_LOG_L(laDEV, llINFO) << " add: Y-axis basis pos: " << pCANStepperMotorTask->m_TargetPos << ", task " << (int) pCANStepperMotorTask->m_TaskIndex;

    //########################################
    //move x-axis to target station
    pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(taskMap,
                                                                       FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                       FunctionModuleTask::FCTMOD_STATE_UNUSED);
    pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
    if(pCANStepperMotorTask == 0)
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }
    pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
    pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorXAxis;
    pCANStepperMotorTask->m_TargetPos = nextStationPosX;
    pCANStepperMotorTask->m_Timeout = 5000;
    TaskIndex++;

    FILE_LOG_L(laDEV, llINFO) << " add: X-axis basis pos: " << pCANStepperMotorTask->m_TargetPos << ", task " << (int) pCANStepperMotorTask->m_TaskIndex;

    //########################################
    //move z-axis to target station
    pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(taskMap,
                                                                       FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                       FunctionModuleTask::FCTMOD_STATE_UNUSED);
    pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
    if(pCANStepperMotorTask == 0)
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }
    pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
    pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorZAxis;
    pCANStepperMotorTask->m_TargetPos = nextStationPosZ;
    pCANStepperMotorTask->m_Timeout = 5000;
    TaskIndex++;

    FILE_LOG_L(laDEV, llINFO) << " add: Z-axis basis pos: " << pCANStepperMotorTask->m_TargetPos << ", task " << (int) pCANStepperMotorTask->m_TaskIndex;

    //########################################
    //move y-axis to target station
    pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(taskMap,
                                                                       FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                       FunctionModuleTask::FCTMOD_STATE_UNUSED);
    pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
    if(pCANStepperMotorTask == 0)
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }
    pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
    pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorYAxis;
    pCANStepperMotorTask->m_TargetPos = nextStationPosY;
    pCANStepperMotorTask->m_Timeout = 5000;
    TaskIndex++;

    FILE_LOG_L(laDEV, llINFO) << " add: Y-axis pos: " << pCANStepperMotorTask->m_TargetPos << ", task " << (int) pCANStepperMotorTask->m_TaskIndex;

    if(reqStationAktion == STATION_ACTION_ATTACH_TO_RACK)
    {
        //########################################
        //close grabber at target station
        pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(taskMap,
                                                                           FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                           FunctionModuleTask::FCTMOD_STATE_UNUSED);
        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pCANStepperMotorTask == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }
        pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
        pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
        pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
        pCANStepperMotorTask->m_TaskIndex = TaskIndex;
        pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
        pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorGrabber;
        pCANStepperMotorTask->m_TargetPos = MOTOR_POS_GRABBER_GRAB;
        pCANStepperMotorTask->m_Timeout = 5000;

        FILE_LOG_L(laDEV, llINFO) << " close grabber: " << pCANStepperMotorTask->m_TargetPos << ", task " << (int) pCANStepperMotorTask->m_TaskIndex;
    }
    else if(reqStationAktion == STATION_ACTION_DETACH_RACK)
    {
        //########################################
        //close grabber at target station
        pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(taskMap,
                                                                           FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                           FunctionModuleTask::FCTMOD_STATE_UNUSED);
        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pCANStepperMotorTask == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }
        pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
        pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
        pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
        pCANStepperMotorTask->m_TaskIndex = TaskIndex;
        pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
        pCANStepperMotorTask->m_pCANObject = pRackHdlDev->m_pMotorGrabber;
        pCANStepperMotorTask->m_TargetPos = MOTOR_POS_GRABBER_DISCARD;
        pCANStepperMotorTask->m_Timeout = 5000;

        FILE_LOG_L(laDEV, llINFO) << " open grabber: " << pCANStepperMotorTask->m_TargetPos << ", task " << (int) pCANStepperMotorTask->m_TaskIndex;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Event handling for reference run task
 *
 *
 *
 *  \iparam   pFunctionModuleTask =
 *  \iparam   pDeviceTask =
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *            otherwise the return value of the error detecting function
 */
/****************************************************************************/
void CRackHandlingNavigator::HandleRefRunTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask)
{
    Q_UNUSED(pDeviceTask);

    FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished.";
    pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
}

/****************************************************************************/
/*!
 *  \brief    Event handling for rack positiong task
 *
 *
 *
 *  \iparam   pFunctionModuleTask =
 *  \iparam   pDeviceTask =
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *            otherwise the return value of the error detecting function
 */
/****************************************************************************/
void CRackHandlingNavigator::HandleRackHandlingTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask)
{
    Q_UNUSED(pFunctionModuleTask);
    Q_UNUSED(pDeviceTask);
}

/****************************************************************************/
/*!
 *  \brief    Event handling for rack positiong task
 *
 *
 *
 *  \iparam   pFunctionModuleTask =
 *  \iparam   pDeviceTask =
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *            otherwise the return value of the error detecting function
 */
/****************************************************************************/
void CRackHandlingNavigator::HandleTransportActionTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask)
{
    Q_UNUSED(pFunctionModuleTask);
    Q_UNUSED(pDeviceTask);

    pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
}

FunctionModuleTask* CRackHandlingNavigator::GetFirstTaskWithState(FunctionModuleTaskMap& TaskMap, FunctionModuleTask::FunctionModuleTaskState_t TaskState)
{
    FunctionModuleTask* pFunctionModuleTask = 0;
    Q_UNUSED(TaskMap);
    Q_UNUSED(TaskState);

    return pFunctionModuleTask;
}

}//namespace
