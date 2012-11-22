/******************************************************************/
/*! \file GrapplerNavigator.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 24.08.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementatin of the class CGrapplerNavigator
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

// -esym(534, QMap::insert)

#include "DeviceControl/Include/Devices/GrapplerNavigator.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include <stdlib.h>

namespace DeviceControl
{

//define for Grappler position offsets
#define OFFSET_POSITION_Y_LIQUID_LEVEL  -60   //!< y position offset for lequid level measurement
#define OFFSET_POSITION_X_LIQUID_LEVEL   20   //!< x position offset for lequid level measurement

/****************************************************************************/
/*!
 *  \brief  Constructor of CGrapplerNavigator
 */
/****************************************************************************/
CGrapplerNavigator::CGrapplerNavigator()
{
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CGrapplerNavigator
 */
/****************************************************************************/
CGrapplerNavigator::~CGrapplerNavigator()
{
}

/****************************************************************************/
/*!
 *  \brief    Create the function module tasks need to execute a reference run of all motors
 *
 *            The motors reference run is done in the following order:
 *             - z-axis
 *             - y-axis
 *             - x-axis
 *
 *           \todo check the reference run preconditions:
 *
 *
 *  \iparam   pGrapplerDev = Grappler device
 *  \iparam   TaskMap = list of tasks used for this action
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *            otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CGrapplerNavigator::CreateReferenceRunFctModTasks(CGrapplerDevice* pGrapplerDev,
                                                               FunctionModuleTaskMap& TaskMap)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CANStepperMotorTask* pCANStepperMotorTask;
    quint8  TaskIndex = 1;

    FILE_LOG_L(laDEV, llDEBUG) << " CGrapplerNavigator: 0x" << std::hex << (int) pGrapplerDev->m_instanceID;

    //------------------------- Z-axis ------------------------------
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = pGrapplerDev->m_pMotorZAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 1000;
    pCANStepperMotorTask->m_SubCommandData = 1;  // enable motor
    TaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    FILE_LOG_L(laDEV, llDEBUG1) << " add task 'SetState'";
    delete pCANStepperMotorTask;

    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pGrapplerDev->m_pMotorZAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    TaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    FILE_LOG_L(laDEV, llINFO) << "      add: reference run clamp motor";
    delete pCANStepperMotorTask;

    //------------------------- Y-axis ------------------------------
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pGrapplerDev->m_pMotorYAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 1000;
    pCANStepperMotorTask->m_SubCommandData = 1;  // enable motor
    TaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    FILE_LOG_L(laDEV, llDEBUG1) << " add task 'SetState'";
    delete pCANStepperMotorTask;

    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject =  pGrapplerDev->m_pMotorYAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    TaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    FILE_LOG_L(laDEV, llINFO) << "      add: reference run needle axis";
    delete pCANStepperMotorTask;

    //------------------------- X-axis ------------------------------
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pGrapplerDev->m_pMotorXAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 1000;
    pCANStepperMotorTask->m_SubCommandData = 1;  // enable motor
    TaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    FILE_LOG_L(laDEV, llDEBUG1) << " add task 'SetState'";
    delete pCANStepperMotorTask;

    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject =  pGrapplerDev->m_pMotorXAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    TaskMap[TaskIndex] = pCANStepperMotorTask;
    FILE_LOG_L(laDEV, llINFO) << "    add: reference run " << TaskMap.count() << " tasks added";
    delete pCANStepperMotorTask;

    return RetVal;

}

/****************************************************************************/
/*!
 *  \brief    Create the function module tasks to request all motors positions
 *
 *            All motors actual positions should be requested. The function creates three
 *            tasks, one per motor, and sets the tasks Target to FCTMOD_TASK_REQ_MOTOR_POS
 *            The tasks could be executed simultaneous.
 *
 *  \iparam   pGrapplerDev = Grappler device
 *  \iparam   TaskMap = list of tasks used for this action
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *            otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CGrapplerNavigator::CreatePositionReqFctModTasks(CGrapplerDevice* pGrapplerDev,
                                                              FunctionModuleTaskMap& TaskMap)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    CANStepperMotorTask* pCANStepperMotorTask;
    quint8               TaskIndex = 1;

    FILE_LOG_L(laDEV, llINFO) << "           Action:PositionRequest";

    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject =  pGrapplerDev->m_pMotorXAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    TaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    delete pCANStepperMotorTask;

    FILE_LOG_L(laDEV, llINFO) << "      add: request motor position Z-Axis";

    //-------------------------------------------------------
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject =  pGrapplerDev->m_pMotorZAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    TaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;
    delete pCANStepperMotorTask;

    FILE_LOG_L(laDEV, llINFO) << "      add: request motor position Y-Axis";

    //-------------------------------------------------------
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject =  pGrapplerDev->m_pMotorYAxis;
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    TaskMap[TaskIndex] = pCANStepperMotorTask;
    delete pCANStepperMotorTask;

    FILE_LOG_L(laDEV, llINFO) << "      add: request motor position X-Axis";

    FILE_LOG_L(laDEV, llINFO) << "  CGrapplerNavigator::CalculateGrapplerPostionRequestTasks finished: " << TaskMap.count() << " tasks in list";

    return retval;
}

/****************************************************************************/
/*!
 *  \brief    Create the function module tasks need to execute a station action
 *
 *            While execition a station action, several task will be need.
 *            The complete action is divided into individual motor movement tasks.
 *            Each motor movement task needs a CANStepperMotorTask instance to store it's parameters
 *            and control the movement completion.
 *            The RFID reader and liquid level measurement needs each a task as well.
 *
 *  \iparam   pGrapplerDev = Grappler device
 *  \iparam   TaskMap = list of tasks used for this action
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *            otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CGrapplerNavigator::CreateStationActionFctModTasks(CGrapplerDevice* pGrapplerDev,
                                                                FunctionModuleTaskMap& TaskMap)
{
    CANStepperMotorTask* pCANStepperMotorTask;
    CANDigitalInputTask* pCANDigitalInputTask;
    CANRFIDTask* pCANRFIDTask;

    Q_UNUSED(pGrapplerDev);

    FILE_LOG_L(laDEV, llINFO) << "  create CANStepperMotorTasks";

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
        TaskMap[idx] = pCANStepperMotorTask;
        delete pCANStepperMotorTask;
    }


    //-------------------------------------------------------
    pCANDigitalInputTask = new CANDigitalInputTask();
    pCANDigitalInputTask->m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_DIG_INPUT;
    pCANDigitalInputTask->m_DigInpTaskID = CANDigitalInputTask::FCTMOD_DI_TASK_ID_REQ_VALUE;
    pCANDigitalInputTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANDigitalInputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_UNDEF;
    pCANDigitalInputTask->m_TaskIndex = FunctionModuleTask::TaskIndexUndef;
    pCANDigitalInputTask->m_TriggerTaskIndex = FunctionModuleTask::TaskIndexUndef;
    pCANDigitalInputTask->m_pCANObject = 0;
    pCANDigitalInputTask->m_Value = 0;
    pCANDigitalInputTask->m_Timeout = 0;
    TaskMap[10] = pCANDigitalInputTask;
    delete pCANDigitalInputTask;

    FILE_LOG_L(laDEV, llINFO) << "  add CANDigitalInputTask";

    pCANRFIDTask = new CANRFIDTask();
    pCANRFIDTask->m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_RFID;
    //pCANRFIDTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_UNDEF;
    pCANRFIDTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANRFIDTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_UNDEF;
    pCANRFIDTask->m_TaskIndex = FunctionModuleTask::TaskIndexUndef;
    pCANRFIDTask->m_TriggerTaskIndex = FunctionModuleTask::TaskIndexUndef;
    pCANRFIDTask->m_pCANObject = 0;
    pCANRFIDTask->m_UID = 0;
    pCANRFIDTask->m_RFIDData = 0;
    pCANRFIDTask->m_Timeout = 5000;
    TaskMap[11] = pCANRFIDTask;
    delete pCANRFIDTask;

    FILE_LOG_L(laDEV, llINFO) << "  add CANRFIDTask";

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief    Calculate the function module tasks need to request the actual motor positions
 *
 *            This functions prepares the device task 'position request' for execution.
 *            Because there are just three CAN messages to send, the only thing to do is setting
 *            the function modules to it's init state
 *
 *  \iparam   pGrappler = Grappler device instance
 *  \iparam   TaskMap = function module task list
 *
 *  \return   So far, always DCL_ERR_FCT_CALL_SUCCESS
 */
/****************************************************************************/
ReturnCode_t CGrapplerNavigator::CalculatePositionRequestTasks(CGrapplerDevice* pGrappler,
                                                               FunctionModuleTaskMap& TaskMap)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    FunctionModuleTask* pFunctionModuleTask;
    QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(TaskMap);

    Q_UNUSED(pGrappler);

    // set all tasks to state 'init'
    while (iterFctModTask.hasNext())
    {
        pFunctionModuleTask = iterFctModTask.next().value();
        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Calculate the task need to execute a grappler action.
 *
 *            This is the function where all the movement stuff is precalculated.
 *            The most important thing when calling this function is to run the 'positoin request' task
 *            before, because this task uses the actual motor positions taken from the CANStepperMotor-objects
 *            without running a request again.
 *
 *            The following actions are supported:
 *             STATION_ACTION_READ_RFID - read the RFID tag at the rack at the requested vessel.
 *             STATION_ACTION_READ_LIQUID_LEVEL - read the liquid level at the  requested vessel.
 *
 *  \iparam   pGrappler = Grappler device instance
 *  \iparam   TaskMap = function module task list
 *  \iparam   stationColumn = the target station's column index
 *  \iparam   stationRow = the target station's row index
 *  \iparam   StationAction = action to perform at the station (vessel)
 *  \iparam   TimeRequest = if true, return the estimated time for the action, the action itself is not performed
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CGrapplerNavigator::CalculateGrapplerActionTasks(CGrapplerDevice* pGrappler,
                                                              FunctionModuleTaskMap& TaskMap,
                                                              qint16 stationColumn,
                                                              qint16 stationRow,
                                                              StationAction_t StationAction,
                                                              bool TimeRequest)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;

    FunctionModuleTask* pFunctionModuleTask;
    CANStepperMotorTask* pCANStepperMotorTask;
    CANDigitalInputTask* pCANDigitalInputTask;
    CANRFIDTask* pCANRFIDTask;

    Position_t     TargetPosX;
    Position_t     TargetPosY;
    Position_t     TargetPosZ;
    Position_t     ActualPosX;
    Position_t     ActualPosY;
    Position_t     ActualPosZ;

    quint16 EstimatedTime = 0;
    quint16 EstimatedCycleTimeX = 0, EstimatedCycleTimeY = 0;

    quint8  TaskIndex = 1;
    bool fSimultanFlag = false;

    //check the pointers used in this method
    if((pGrappler == 0) ||
       (pGrappler->m_pMotorXAxis == 0) ||
       (pGrappler->m_pMotorYAxis == 0) ||
       (pGrappler->m_pMotorZAxis == 0) ||
       (pGrappler->m_pLiquidLevel == 0) ||
       (pGrappler->m_pRFID == 0))
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    //Actual motor position (a postion request task was done before starting this task!)
//    ActualPosX = pGrappler->m_pMotorXAxis->GetActPosition();
//    ActualPosY = pGrappler->m_pMotorYAxis->GetActPosition();
//    ActualPosZ = pGrappler->m_pMotorZAxis->GetActPosition();


    FILE_LOG_L(laDEV, llINFO) << " ############################################################";
    FILE_LOG_L(laDEV, llINFO) << "  CGrapplerNavigator: " << std::hex << (quint32) pGrappler->m_instanceID << ", Action:" << (int) StationAction;
    pGrappler->GetStationXYPositionFromRowCol(stationColumn, stationRow, StationAction, TargetPosX, TargetPosY);

    //depending on the requested action the z-position must be determined
    pGrappler->GetZPositionFromAction(TargetPosZ, StationAction);
    FILE_LOG_L(laDEV, llINFO) << "     Actual position X,Y,Z:" << ActualPosX << ", " << ActualPosY << ", " << ActualPosZ;
    FILE_LOG_L(laDEV, llINFO) << "     Target position X,Y,Z:" << TargetPosX << ", " << TargetPosY << ", " << TargetPosZ;

    // here TargetPosX and TargetPosY have the target station's center positions
    CorrectMotorPosAccordingAction(TargetPosX, TargetPosY, StationAction);
    FILE_LOG_L(laDEV, llINFO) << "     Action position X,Y:" << TargetPosX << ", " << TargetPosY;
    // at this point TargetPosX and TargetPosY have the grapper positions of the target station
    // according the requested action (attach/detach)

    if((StationAction == STATION_ACTION_TRANSP_RACK_TO) ||
       (StationAction == STATION_ACTION_MOVE_EMTPY_TO))
    {
        // - ensure z-axis to be at least at the minimum height (for loaded or unloaded transport, depending on action)
        // - move x- and y-axis to target station
        // calculate the time consumption of the positioning
        if(ActualPosX != TargetPosX)
        {
//            EstimatedCycleTimeX = pGrappler->m_pMotorXAxis->CalcMotionTime(TargetPosX, pGrappler->m_ProfileXAxisUnload);
            FILE_LOG_L(laDEV, llINFO) << "  X-axis:" << ActualPosX << " to " << TargetPosX << " profile: " << (int) pGrappler->m_ProfileXAxisUnload;
            FILE_LOG_L(laDEV, llINFO) << "  Estimated Time X-axis:" << EstimatedCycleTimeX;
        }
        if(ActualPosY != TargetPosY)
        {
//            EstimatedCycleTimeY = pGrappler->m_pMotorYAxis->CalcMotionTime(TargetPosY, pGrappler->m_ProfileYAxisUnload);
            FILE_LOG_L(laDEV, llINFO) << "  Y-axis:" << ActualPosY << " to " << TargetPosY << " profile: " << (int) pGrappler->m_ProfileYAxisUnload;
            FILE_LOG_L(laDEV, llINFO) << "  Estimated Time Y-axis:" << EstimatedCycleTimeY;
        }

        if(StationAction == STATION_ACTION_TRANSP_RACK_TO)
        {
            TargetPosZ = pGrappler->m_PositionZAxisTransportLoad;
        }
        else if(StationAction == STATION_ACTION_MOVE_EMTPY_TO)
        {
            TargetPosZ = pGrappler->m_PositionZAxisTransportUnload;
        }

        //if(ActualPosZ > TargetPosZ)
        if(abs(ActualPosZ > TargetPosZ) > 10)
        {
            if(TimeRequest)
            {
//                EstimatedTime = pGrappler->m_pMotorZAxis->CalcMotionTime(ActualPosZ, pGrappler->m_ProfileZAxisUpLoadFast);
                FILE_LOG_L(laDEV, llINFO) << "  Estimated Time Z-axis:" << EstimatedTime;
            }
            else
            {
                //move Z-axis up to enable x/y-movement
                pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(TaskMap,
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
                pCANStepperMotorTask->m_pCANObject = pGrappler->m_pMotorZAxis;
                pCANStepperMotorTask->m_TargetPos = TargetPosZ;
                pCANStepperMotorTask->m_Profile = pGrappler->m_ProfileZAxisUpLoadFast;
                pCANStepperMotorTask->m_SubCommandData = 0;
                pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_ACKN;;
                //oder
                //pCANStepperMotorTask->m_TargetPos = pGrappler->m_PositionZAxisTransportLoad;
                //pCANStepperMotorTask->m_Profile = pGrappler->m_ProfileZAxisUpLoadSlow;
                /// \todo second profil while moving up with load!!!

                pCANStepperMotorTask->m_Timeout = 5000;
                TaskIndex++;

                FILE_LOG_L(laDEV, llINFO) << " add: Z-Axis task, ID:" << (int) pCANStepperMotorTask->m_TaskIndex;
                FILE_LOG_L(laDEV, llINFO) << "    TargetPos: " << pCANStepperMotorTask->m_TargetPos << ", profile: " << (int) pCANStepperMotorTask->m_Profile;
            }
        }

        if(ActualPosX != TargetPosX)
        {
            if(!TimeRequest)
            {
                //this task moves the x axis to it's target position
                pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(TaskMap,
                                                                                   FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                                   FunctionModuleTask::FCTMOD_STATE_UNUSED);
                pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
                if(pCANStepperMotorTask == 0)
                {
                    return DCL_ERR_NULL_PTR_ACCESS;
                }

                pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
                pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
                if(TaskIndex == 1)
                {
                    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
                    pCANStepperMotorTask->m_TriggerTaskIndex = 0;
                }
                else
                {
                    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
                    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
                }
                pCANStepperMotorTask->m_TaskIndex = TaskIndex;
                pCANStepperMotorTask->m_pCANObject = pGrappler->m_pMotorXAxis;
                pCANStepperMotorTask->m_TargetPos = TargetPosX;
                pCANStepperMotorTask->m_Profile = pGrappler->m_ProfileXAxisUnload;
                if(EstimatedCycleTimeX < EstimatedCycleTimeY)
                {
                    Speed_t Speed;
                    Position_t Distance;
                    Distance = abs(TargetPosX - ActualPosX);
                    Speed = pGrappler->m_pMotorXAxis->GetSpeedFromPeriod(Distance, pGrappler->m_ProfileXAxisUnload, EstimatedCycleTimeY);
                    /// \todo Negative speed to quint16 ??
                    pCANStepperMotorTask->m_SubCommandData = Speed;

                    /// \todo add sub command: use speed from message data
                    pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_ACKN;
                }
                else
                {
                    pCANStepperMotorTask->m_SubCommandData = 0;
                    pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_ACKN;;
                }

                pCANStepperMotorTask->m_Timeout = 5000;
                TaskIndex++;


                //by setting this flag to true, the y-movement will be executed simultaneously
                fSimultanFlag = false;;

                FILE_LOG_L(laDEV, llINFO) << " add: X-Axis task, ID:" << (int) pCANStepperMotorTask->m_TaskIndex;
                FILE_LOG_L(laDEV, llINFO) << "    TargetPos: " << pCANStepperMotorTask->m_TargetPos << ", profile: " << (int) pCANStepperMotorTask->m_Profile;
                FILE_LOG_L(laDEV, llINFO) << "    SubCommandID: " << (int) pCANStepperMotorTask->m_SubCommandID << ", SubCommandData: " << pCANStepperMotorTask->m_SubCommandData;
            }
        }

        if(ActualPosY != TargetPosY)
        {
            if(!TimeRequest)
            {
                //this task moves the y axis to it's target position
                pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(TaskMap,
                                                                                   FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                                   FunctionModuleTask::FCTMOD_STATE_UNUSED);
                pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
                if(pCANStepperMotorTask == 0)
                {
                    return DCL_ERR_NULL_PTR_ACCESS;
                }

                pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
                pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
                if(TaskIndex == 1)
                {
                    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
                    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
                }
                else
                {
                    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
                    if(fSimultanFlag)
                    {
                        pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
                        pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
                    }
                    else
                    {
                        pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
                    }
                }
                pCANStepperMotorTask->m_TaskIndex = TaskIndex;
                pCANStepperMotorTask->m_pCANObject = pGrappler->m_pMotorYAxis;
                pCANStepperMotorTask->m_TargetPos = TargetPosY;
                if(EstimatedCycleTimeY < EstimatedCycleTimeX)
                {
                    Speed_t Speed;
                    Position_t Distance;
                    Distance = abs(TargetPosY - ActualPosY);
                    Speed = pGrappler->m_pMotorYAxis->GetSpeedFromPeriod(Distance, pGrappler->m_ProfileYAxisUnload, EstimatedCycleTimeX);
                    /// \todo Negative speed to quint16 ??
                    pCANStepperMotorTask->m_SubCommandData = Speed;

                    /// \todo //use speed from message data:  | CANStepperMotor::FM_SM_SUBCMD_MOTION_SPEED
                    pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_ACKN;
                }
                else
                {
                    pCANStepperMotorTask->m_SubCommandData = 0;
                    pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_ACKN;;
                }
                pCANStepperMotorTask->m_Timeout = 5000;
                //TaskIndex++;

                FILE_LOG_L(laDEV, llINFO) << " add: Y-Axis task, ID:" << (int) pCANStepperMotorTask->m_TaskIndex;
                FILE_LOG_L(laDEV, llINFO) << "    TargetPos: " << pCANStepperMotorTask->m_TargetPos << ", profile: " << (int) pCANStepperMotorTask->m_Profile;
                FILE_LOG_L(laDEV, llINFO) << "    SubCommandID: " << (int) pCANStepperMotorTask->m_SubCommandID << ", SubCommandData: " << pCANStepperMotorTask->m_SubCommandData;

            }
        }
    }
    else if(StationAction == STATION_ACTION_ATTACH_TO_RACK)
    {
        // grappler moves down z-axis and attaches to rack (by moving y-axis)
        if(ActualPosZ != TargetPosZ)
        {
            //this task moves the z axis down into cuvette
            pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(TaskMap,
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
            pCANStepperMotorTask->m_pCANObject = pGrappler->m_pMotorZAxis;
            pCANStepperMotorTask->m_TargetPos = TargetPosZ;
            pCANStepperMotorTask->m_Profile = pGrappler->m_ProfileZAxisUpLoadFast;
            pCANStepperMotorTask->m_SubCommandData = 0;
            pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_ACKN;
            pCANStepperMotorTask->m_Timeout = 5000;
            TaskIndex++;

            FILE_LOG_L(laDEV, llINFO) << " add: Z-Axis task, ID:" << (int) pCANStepperMotorTask->m_TaskIndex;
            FILE_LOG_L(laDEV, llINFO) << "    TargetPos: " << pCANStepperMotorTask->m_TargetPos << ", profile: " << (int) pCANStepperMotorTask->m_Profile;
            FILE_LOG_L(laDEV, llINFO) << "    SubCommandID: " << (int) pCANStepperMotorTask->m_SubCommandID << ", SubCommandData: " << pCANStepperMotorTask->m_SubCommandData;
        }

        //this task moves the y-axis to attach the rack
        pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(TaskMap,
                                                                           FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                           FunctionModuleTask::FCTMOD_STATE_UNUSED);
        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pCANStepperMotorTask == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
        pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
        pCANStepperMotorTask->m_TaskIndex = TaskIndex;
        if(TaskIndex == 1)
        {
            pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
            pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
        }
        else
        {
            pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
            pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
        }
        pCANStepperMotorTask->m_pCANObject = pGrappler->m_pMotorYAxis;
        pCANStepperMotorTask->m_TargetPos = TargetPosY;
        pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_ACKN;
        pCANStepperMotorTask->m_SubCommandData = 0;
        pCANStepperMotorTask->m_Timeout = 5000;
        //TaskIndex++;

        FILE_LOG_L(laDEV, llINFO) << " add: Y-Axis task, ID:" << (int) pCANStepperMotorTask->m_TaskIndex;
        FILE_LOG_L(laDEV, llINFO) << "    TargetPos: " << pCANStepperMotorTask->m_TargetPos << ", profile: " << (int) pCANStepperMotorTask->m_Profile;
        FILE_LOG_L(laDEV, llINFO) << "    SubCommandID: " << (int) pCANStepperMotorTask->m_SubCommandID << ", SubCommandData: " << pCANStepperMotorTask->m_SubCommandData;

    }
    else if(StationAction == STATION_ACTION_DETACH_RACK)
    {
        // grappler detaches rack (by moving y-axis) and moves z-axis up to x/y-move height
        // grappler moves down z-axis and attaches to rack (by moving y-axis)
        if(ActualPosY != TargetPosY)
        {
            //this task moves the y axis to detach the grappler from rack
            pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(TaskMap,
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
            pCANStepperMotorTask->m_pCANObject = pGrappler->m_pMotorYAxis;
            pCANStepperMotorTask->m_TargetPos = TargetPosY;
            pCANStepperMotorTask->m_SubCommandData = 0;
            pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_ACKN;
            pCANStepperMotorTask->m_Timeout = 5000;
            TaskIndex++;

            FILE_LOG_L(laDEV, llINFO) << " add: Y-Axis task, ID:" << (int) pCANStepperMotorTask->m_TaskIndex;
            FILE_LOG_L(laDEV, llINFO) << "    TargetPos: " << pCANStepperMotorTask->m_TargetPos << ", profile: " << (int) pCANStepperMotorTask->m_Profile;
            FILE_LOG_L(laDEV, llINFO) << "    SubCommandID: " << (int) pCANStepperMotorTask->m_SubCommandID << ", SubCommandData: " << pCANStepperMotorTask->m_SubCommandData;
        }

        //this task moves the z-axis up
        pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(TaskMap,
                                                                           FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                           FunctionModuleTask::FCTMOD_STATE_UNUSED);
        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pCANStepperMotorTask == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
        pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
        pCANStepperMotorTask->m_TaskIndex = TaskIndex;
        if(TaskIndex == 1)
        {
            pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
            pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
        }
        else
        {
            pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
            pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
        }
        pCANStepperMotorTask->m_pCANObject = pGrappler->m_pMotorZAxis;
        pCANStepperMotorTask->m_TargetPos = TargetPosZ;
        pCANStepperMotorTask->m_Profile = pGrappler->m_ProfileZAxisUpLoadFast;
        pCANStepperMotorTask->m_SubCommandData = 0;
        pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_ACKN;
        pCANStepperMotorTask->m_Timeout = 5000;
        //TaskIndex++;

        FILE_LOG_L(laDEV, llINFO) << " add: Z-Axis task, ID:" << (int) pCANStepperMotorTask->m_TaskIndex;
        FILE_LOG_L(laDEV, llINFO) << "    TargetPos: " << pCANStepperMotorTask->m_TargetPos << ", profile: " << (int) pCANStepperMotorTask->m_Profile;
        FILE_LOG_L(laDEV, llINFO) << "    SubCommandID: " << (int) pCANStepperMotorTask->m_SubCommandID << ", SubCommandData: " << pCANStepperMotorTask->m_SubCommandData;

    }
    else if(StationAction == STATION_ACTION_PULL_UP_RACK)
    {
        // move up z-axis with rack to transport height
        pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(TaskMap,
                                                                           FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                           FunctionModuleTask::FCTMOD_STATE_UNUSED);
        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pCANStepperMotorTask == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        //the first z-axis command moves it up with slow speed to reduce liquid carry-over
        pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
        pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
        pCANStepperMotorTask->m_TaskIndex = TaskIndex;
        pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
        pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
        pCANStepperMotorTask->m_pCANObject = pGrappler->m_pMotorZAxis;
        pCANStepperMotorTask->m_TargetPos = TargetPosZ;
        //movement profile for slow speed rack lift up
        pCANStepperMotorTask->m_Profile = pGrappler->m_ProfileZAxisUpLoadSlow;
        //pCANStepperMotorTask->m_SubCommandData = 0;
        pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_ACKN;
        pCANStepperMotorTask->m_Timeout = 5000;
        TaskIndex++;

        //the second z-axis command moves it up with fast speed. When fast speed starts the rack is above the liquid level
        pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(TaskMap,
                                                                           FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                           FunctionModuleTask::FCTMOD_STATE_UNUSED);
        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pCANStepperMotorTask == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
        pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
        pCANStepperMotorTask->m_TaskIndex = TaskIndex;
        pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
        pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
        pCANStepperMotorTask->m_pCANObject = pGrappler->m_pMotorZAxis;
        pCANStepperMotorTask->m_TargetPos = TargetPosZ;
        pCANStepperMotorTask->m_Profile = pGrappler->m_ProfileZAxisUpLoadFast;
        //function module should use a time delay to start this command
        pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_TIME_DELAY;
        //time delay for CAN-bus position command which starts fast speed rack lift up (accelerationg from the previos slow speed cmd)
        pCANStepperMotorTask->m_SubCommandData = pGrappler->m_CmdDelayZAxisUpLoadFast;

        pCANStepperMotorTask->m_Timeout = 5000;
        //TaskIndex++;

        FILE_LOG_L(laDEV, llINFO) << " add: Z-Axis task, ID:" << (int) pCANStepperMotorTask->m_TaskIndex;
        FILE_LOG_L(laDEV, llINFO) << "    TargetPos: " << pCANStepperMotorTask->m_TargetPos << ", profile: " << (int) pCANStepperMotorTask->m_Profile;
        FILE_LOG_L(laDEV, llINFO) << "    SubCommandID: " << (int) pCANStepperMotorTask->m_SubCommandID << ", SubCommandData: " << pCANStepperMotorTask->m_SubCommandData;
    }
    else if(StationAction == STATION_ACTION_LET_DOWN_RACK)
    {
        // grappler moves down z-axis, rack relieves from grappler, but not detached
        pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(TaskMap,
                                                                           FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                           FunctionModuleTask::FCTMOD_STATE_UNUSED);
        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pCANStepperMotorTask == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        pCANStepperMotorTask->m_MotorTaskID = CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS;
        pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
        pCANStepperMotorTask->m_TaskIndex = TaskIndex;
        pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
        pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
        pCANStepperMotorTask->m_pCANObject = pGrappler->m_pMotorZAxis;
        pCANStepperMotorTask->m_TargetPos = TargetPosZ;
        pCANStepperMotorTask->m_Profile = pGrappler->m_ProfileZAxisUpLoadFast;
        pCANStepperMotorTask->m_SubCommandData = 0;
        pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_ACKN;
        pCANStepperMotorTask->m_Timeout = 5000;
        //TaskIndex++;

        FILE_LOG_L(laDEV, llINFO) << " add: Z-Axis task, ID:" << (int) pCANStepperMotorTask->m_TaskIndex;
        FILE_LOG_L(laDEV, llINFO) << "    TargetPos: " << pCANStepperMotorTask->m_TargetPos << ", profile: " << (int) pCANStepperMotorTask->m_Profile;
        FILE_LOG_L(laDEV, llINFO) << "    SubCommandID: " << (int) pCANStepperMotorTask->m_SubCommandID << ", SubCommandData: " << pCANStepperMotorTask->m_SubCommandData;
    }
    else if(StationAction == STATION_ACTION_READ_RFID)
    {
        //this task reads the RFID tag
        pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(TaskMap,
                                                                           FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                           FunctionModuleTask::FCTMOD_STATE_UNUSED);
        pCANRFIDTask = dynamic_cast<CANRFIDTask*>(pFunctionModuleTask);
        if(pCANRFIDTask == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        pCANRFIDTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
        if(TaskIndex == 1)
        {
            pCANRFIDTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
            pCANRFIDTask->m_TriggerTaskIndex = TaskIndex;
        }
        else
        {
            pCANRFIDTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
            pCANRFIDTask->m_TriggerTaskIndex = TaskIndex - 1;
        }
        pCANRFIDTask->m_TaskIndex = TaskIndex;
        pCANRFIDTask->m_pCANObject = pGrappler->m_pRFID;

        FILE_LOG_L(laDEV, llINFO) << " add: read RFID, task " << (int)pCANRFIDTask->m_TaskIndex;
    }
    else if(StationAction == STATION_ACTION_READ_LIQUID_LEVEL)
    {
        //this task reads the analog input connected to the liquid level sensor
        pFunctionModuleTask = CFunctionModuleTaskManager::GetTaskWithState(TaskMap,
                                                                           FunctionModuleTask::FCTMOD_TASK_ID_MOTOR,
                                                                           FunctionModuleTask::FCTMOD_STATE_UNUSED);
        pCANDigitalInputTask = dynamic_cast<CANDigitalInputTask*>(pFunctionModuleTask);
        if(pCANDigitalInputTask == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        pCANDigitalInputTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
        if(TaskIndex == 1)
        {
            pCANDigitalInputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
            pCANDigitalInputTask->m_TriggerTaskIndex = TaskIndex;
        }
        else
        {
            pCANDigitalInputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
            pCANDigitalInputTask->m_TriggerTaskIndex = TaskIndex - 1;
        }
        pCANDigitalInputTask->m_TaskIndex = TaskIndex;
        pCANDigitalInputTask->m_pCANObject = pGrappler->m_pLiquidLevel;

        FILE_LOG_L(laDEV, llINFO) << " add: read LiquidLevel, task " << (int)pCANDigitalInputTask->m_TaskIndex;
    }

    return retval;

}


/****************************************************************************/
/*!
 *  \brief    Correct the x- and y- axis position according to the requested action
 *
 *   Depending on the action, the x- and y-axis must positioned to different positions
 *   Possible actions are rack grab, rack discard, move over vessel, read RFID, read liquid level
 *
 *  \xparam   NextStationPosX = Station x-position to correct
 *  \xparam   NextStationPosY = Station y-position to correct
 *  \iparam   StationAction = station action
 *
 *  \return   void
 */
/****************************************************************************/
void CGrapplerNavigator::CorrectMotorPosAccordingAction(Position_t& NextStationPosX, Position_t& NextStationPosY, StationAction_t StationAction)
{
    switch (StationAction)
    {
        case (STATION_ACTION_READ_LIQUID_LEVEL):
            NextStationPosY += OFFSET_POSITION_Y_LIQUID_LEVEL;
            NextStationPosX += OFFSET_POSITION_X_LIQUID_LEVEL;
            break;
        default:
            //pfui
            break;
    }
}

} //namespace
