/******************************************************************/
/*! \file CoverLineNavigator.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 19.01.2011
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

// -esym(534, QMap*::insert)
// -esym(534, QMap::insert)

#include "DeviceControl/Include/Devices/CoverLineNavigator.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor of CCoverLineNavigator
 */
/****************************************************************************/
CCoverLineNavigator::CCoverLineNavigator()
{
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CCoverLineNavigator
 */
/****************************************************************************/
CCoverLineNavigator::~CCoverLineNavigator()
{
}

/****************************************************************************/
/*!
 *  \brief  Returning first function module having a certain state
 *
 *      Returns the first FunctionModuleTask instance from the list with the
 *      specified FunctionModuleState_t.
 *
 *  \iparam taskList = The task list
 *  \iparam taskState = The required task state
 *
 *  \return Pointer to the first FunctionModuleTask instance with the
 *          requested FunctionModuleState_t, if any. Otherwise 0 will be
 *          returned.
 */
/****************************************************************************/
/*FunctionModuleTask* CCoverLineNavigator::GetFirstTaskWithState(FunctionModuleTaskMap& taskMap, FunctionModuleTask::FunctionModuleTaskState_t taskState)
{
    FunctionModuleTask* pFunctionModuleTask = 0;
    FunctionModuleTask* pFunctionModuleTaskRet = 0;
    QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(taskMap);


    while (iterFctModTask.hasNext())
    {
        pFunctionModuleTask = iterFctModTask.next().value();
        if(pFunctionModuleTask->m_TaskState == taskState)
        {
            pFunctionModuleTaskRet = pFunctionModuleTask;
            break;
        }
    }

    return pFunctionModuleTaskRet;
}*/

/****************************************************************************/
/*!
 *  \brief  Create tasks needed for the reference runs
 *
 *      Calculate the tasks need to execute a reference run of all cover line
 *      motors. The motors reference run is done in the following order:
 *       - slide clamp motor
 *       - needle motor
 *       - slide transport motor
 *       - cover slip transport z-axis motor
 *       - cover slip transport y-axis motor
 *       - slide elevator motor
 *
 *  \todo   Check the reference run preconditions:
 *
 *  \iparam pCoverLine = Cover line device
 *  \iparam pDeviceTask = Device task for this action
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *          otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CCoverLineNavigator::CreateReferenceRunFctModTasks(CCoverLineDevice* pCoverLine,
                                                                DeviceTask* pDeviceTask)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    CANStepperMotorTask* pCANStepperMotorTask;

    // reference run clamp axis
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = REFRUN_FCT_TASK_CLAMP;
    pCANStepperMotorTask->m_TriggerTaskIndex = REFRUN_FCT_TASK_CLAMP;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CLAMP];
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // reference run needle axis
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = REFRUN_FCT_TASK_NEEDLE;
    pCANStepperMotorTask->m_TriggerTaskIndex = REFRUN_FCT_TASK_CLAMP;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_NEEDLE];
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // reference run slide shifter axis
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = REFRUN_FCT_TASK_SLIDE;
    pCANStepperMotorTask->m_TriggerTaskIndex = REFRUN_FCT_TASK_NEEDLE;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_SLIDE];
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // reference run elevator axis
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = REFRUN_FCT_TASK_ELEVATOR;
    pCANStepperMotorTask->m_TriggerTaskIndex = REFRUN_FCT_TASK_SLIDE;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_ELEVATOR];
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // reference run cover slip transporter z-axis
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = REFRUN_FCT_TASK_CS_Z;
    pCANStepperMotorTask->m_TriggerTaskIndex = REFRUN_FCT_TASK_ELEVATOR;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Z];
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

     // reference run cover slip transporter y-axis
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = REFRUN_FCT_TASK_CS_Y;
    pCANStepperMotorTask->m_TriggerTaskIndex = REFRUN_FCT_TASK_CS_Z;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Y];
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

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Create tasks needed for basic positioning
 *
 *      Create the function module tasks need to take the motors basis
 *      position. Before the motors are moved, their actual positions are
 *      requested. Than the motors basis positioning is done in the following
 *      order:
 *       - clamp-axis
 *       - needle-axis
 *       - coverslip z-axis
 *       - slide shift-axis
 *       - elevator-axis
 *       - coverslip y-axis
 *
 *  \iparam pCoverLine = Cover line device
 *  \iparam pDeviceTask = Device task for this action
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *          otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CCoverLineNavigator::CreateBasisPosFctModTasks(CCoverLineDevice* pCoverLine,
                                                            DeviceTask* pDeviceTask)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    CANStepperMotorTask* pCANStepperMotorTask;

    // position request clamp axis"
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_BASIS_POS_REQ_MOTOR_CLAMP;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_BASIS_POS_REQ_MOTOR_CLAMP;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CLAMP];
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // position request needle axis"
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_BASIS_POS_REQ_MOTOR_NEEDLE;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_BASIS_POS_REQ_MOTOR_CLAMP;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_NEEDLE];
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // position request slide shift axis"
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_BASIS_POS_REQ_MOTOR_SLIDE;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_BASIS_POS_REQ_MOTOR_NEEDLE;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_SLIDE];
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // position request elevator axis"
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_BASIS_POS_REQ_MOTOR_ELEVATOR;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_BASIS_POS_REQ_MOTOR_SLIDE;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_ELEVATOR];
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // position request cover slip z-axis
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_BASIS_POS_REQ_MOTOR_Z;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_BASIS_POS_REQ_MOTOR_ELEVATOR;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Z];
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // position request cover slip y-axis"
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_BASIS_POS_REQ_MOTOR_Y;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_BASIS_POS_REQ_MOTOR_Z;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Y];
    pCANStepperMotorTask->m_TargetPos = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    FILE_LOG_L(laDEV, llINFO) << "      add: position request all axis";

    /// \todo check: Problem mit simultanueos, da nicht alle Positionen sicher angekommen sind wenn die Bewegung loslegt

    // move the motors to their basis position
    // needle, clamp and coverslip z-axis are started simultaneously
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_BASIS_POS_MOTOR_NEEDLE;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_BASIS_POS_REQ_MOTOR_Y;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_NEEDLE];
    pCANStepperMotorTask->m_TargetPos = 5;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_BASIS_POS_MOTOR_CLAMP;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_BASIS_POS_MOTOR_NEEDLE;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CLAMP];
    pCANStepperMotorTask->m_TargetPos = 5;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_BASIS_POS_MOTOR_Z;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_BASIS_POS_MOTOR_CLAMP;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Z];
    pCANStepperMotorTask->m_TargetPos = 10;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    //move slide shift to the 'elevator position'
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_BASIS_POS_MOTOR_SLIDE;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_BASIS_POS_MOTOR_CLAMP;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_SLIDE];
    pCANStepperMotorTask->m_TargetPos = 10;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    //move elevator to the highest position, this enables the slide shifter to shift out the topmost rack
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_BASIS_POS_MOTOR_ELEVATOR;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_BASIS_POS_MOTOR_SLIDE;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_ELEVATOR];
    //target position
    pCANStepperMotorTask->m_TargetPos = 1500;
    //movement profile to use for this positioning
    pCANStepperMotorTask->m_Profile = 1;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_BASIS_POS_MOTOR_Y;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_BASIS_POS_MOTOR_Z;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Y];
    pCANStepperMotorTask->m_TargetPos = 10;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    FunctionModuleTask* pFunctionModuleTask;
    QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(pDeviceTask->m_FctModTaskMap);
    while (iterFctModTask.hasNext())
    {
        pFunctionModuleTask = iterFctModTask.next().value();
        pFunctionModuleTask->m_pDeviceTask = pDeviceTask;
    }

    FILE_LOG_L(laDEV, llINFO) << "  CGrapplerNavigator::CalculateGrapplerReferenceRunTasks finished: " << pDeviceTask->m_FctModTaskMap.count() << " tasks in list";

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Create the function module tasks need for elevator position
 *
 *      The elevator is positioned to the slide extraction position. The
 *      signal of the slide detection sensor is requested, if no slide is
 *      detected, the elevator is positioned to the next slides extraction
 *      position.
 *
 *  \iparam pCoverLine = Cover line device
 *  \iparam pDeviceTask = Device task for this action
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *          otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CCoverLineNavigator::CreateRackPosTasks(CCoverLineDevice* pCoverLine,
                                                     DeviceTask* pDeviceTask)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CANStepperMotorTask* pCANStepperMotorTask;
    CANDigitalInputTask* pCANDigitalInputTask;

    FILE_LOG_L(laDEV, llINFO) << "           Action: Rack position";

    //task for elevator position
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_RACK_POS_MOTOR_ELEVATOR;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_RACK_POS_MOTOR_ELEVATOR;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_ELEVATOR];
    pCANStepperMotorTask->m_TargetPos = 1500;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    //task for slide detection, reads digital input
    pCANDigitalInputTask = new CANDigitalInputTask();
    pCANDigitalInputTask->m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_DIG_INPUT;
    pCANDigitalInputTask->m_DigInpTaskID = CANDigitalInputTask::FCTMOD_DI_TASK_ID_REQ_VALUE;
    pCANDigitalInputTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANDigitalInputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANDigitalInputTask->m_TaskIndex = FCT_TASK_ID_RACK_POS_DIG_INP_SLIDE_DETECT;
    pCANDigitalInputTask->m_TriggerTaskIndex = FCT_TASK_ID_RACK_POS_MOTOR_ELEVATOR;
    pCANDigitalInputTask->m_pCANObject = (CModule*) pCoverLine->m_pDigInpSlideDetection;
    pCANDigitalInputTask->m_Value = 0;
    pCANDigitalInputTask->m_Timeout = 500;
    pDeviceTask->m_FctModTaskMap[pCANDigitalInputTask->m_TaskIndex] = pCANDigitalInputTask;

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
 *  \brief Creates tasks needed to shift out a slide
 *
 *      Create the function module tasks need for the slide shift out
 *      process. This process is the shift out movement of the slide shifter
 *      and the simultaneous control of the edm pump to cover the slide with
 *      the edm. At the end of the process the clamp mechanism fixes the
 *      slide at the cover position. The process includes the following
 *      tasks:
 *       - slide shift motor for the slide's pull of movement (from rack to
 *         coverslide position) this movement must be done with various speed
 *         parameters
 *       - edm pump control
 *       - clamp of the slide, after slide pull of is finished
 *
 *  \iparam pCoverLine = Cover line device
 *  \iparam pDeviceTask = Device task for this action
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *          otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CCoverLineNavigator::CreateSlideShiftOutTasks(CCoverLineDevice* pCoverLine,
                                                           DeviceTask* pDeviceTask)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8  TaskIndex = 1;
    CANStepperMotorTask* pCANStepperMotorTask;

    FILE_LOG_L(laDEV, llDEBUG) << " CreateSlideShiftOutTasks";

    //task for needle position
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_NEEDLE];
    pCANStepperMotorTask->m_TargetPos = 100;
    pCANStepperMotorTask->m_Timeout = 1000;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;

    //task for edm pump
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_EDM_PUMP];
    pCANStepperMotorTask->m_TargetPos = 650;
    pCANStepperMotorTask->m_Timeout = 500;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;

    //task for slide shift slow speed
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_SLIDE];
    pCANStepperMotorTask->m_TargetPos = 900;
    pCANStepperMotorTask->m_Profile = 2;
    pCANStepperMotorTask->m_Timeout = 1000;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;

    //task for slide shift high speed
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_TIMEOFFSET;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_SLIDE];
    pCANStepperMotorTask->m_TargetPos = 1200;
    pCANStepperMotorTask->m_Profile = 1;
    pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_TIME_DELAY;     // time delay, internal triggered
    pCANStepperMotorTask->m_SubCommandData = 300; // 300ms time delay for motor position command execution
    pCANStepperMotorTask->m_OffsetTime = 200;     // 200ms time delay (start condition of the task)
    pCANStepperMotorTask->m_Timeout = 1000;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;

    //task for slide shift slow speed
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_TIMEOFFSET;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_SLIDE];
    pCANStepperMotorTask->m_TargetPos = 1500;
    pCANStepperMotorTask->m_Profile = 2;
    pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_TIME_DELAY;     // time delay, internal triggered
    pCANStepperMotorTask->m_SubCommandData = 600; // 300ms time delay for motor position command execution
    pCANStepperMotorTask->m_OffsetTime = 200;     // 200ms time delay (start condition of the task)
    pCANStepperMotorTask->m_Timeout = 1500;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;

    //task for clamp
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CLAMP];
    pCANStepperMotorTask->m_TargetPos = 100;
    pCANStepperMotorTask->m_Timeout = 2000;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;

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
 *  \brief Creating the tasks needed to grap the cover slips
 *
 *      Create the function module tasks need for the coverslip grab process.
 *      The coverslip grab process is the movemnt of the coverslip transport
 *      grabber to the depot, the   dropping of the z-axis and the vacuum
 *      control to grab a coverslip from the depot. The z-axis is lift up,
 *      and the coverslip is tested by the hall sensor.
 *
 *      The process includes the following tasks:
 *       - drop z-axix to depot
 *       - activate vaccum to grab coverslip
 *       - lift up z-axis
 *       - test coverslip exsitence and quality
 *
 *  \iparam pCoverLine = Cover line device
 *  \iparam pDeviceTask = Device task for this action
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *          otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CCoverLineNavigator::CreateCoverSlipGrabTasks(CCoverLineDevice* pCoverLine,
                                                           DeviceTask* pDeviceTask)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CANStepperMotorTask*  pCANStepperMotorTask;
    CANDigitalOutputTask* pCANDigitalOutputTask;
    CANAnalogInputTask*   pCANAnalogInputTask;

    FILE_LOG_L(laDEV, llINFO) << " CreateCoverSlipGrabTasks";

    // move y-axis to the coverslip grab position
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Y_GRAB_POS;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Y_GRAB_POS;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Y];
    pCANStepperMotorTask->m_TargetPos = 10;
    pCANStepperMotorTask->m_Profile = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // move z-axis down to the coverslips
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_DOWN;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Y_GRAB_POS;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Z];
    pCANStepperMotorTask->m_TargetPos = 800;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // switch on the vacuum pump
    pCANDigitalOutputTask = CBaseDevice::GetNewCANDigitalOutputTask(CANDigitalOutputTask::FCTMOD_DO_TASK_ID_SET_VALUE);
    pCANDigitalOutputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANDigitalOutputTask->m_TaskIndex = FCT_TASK_ID_CS_GRAB_VACUUM_PUMP_ON;
    pCANDigitalOutputTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_DOWN;
    pCANDigitalOutputTask->m_pCANObject = (CModule*) pCoverLine->m_pDigOutpVacuumPump;
    pCANDigitalOutputTask->m_Value = 1;
    pCANDigitalOutputTask->m_Timeout = 500;
    pDeviceTask->m_FctModTaskMap[pCANDigitalOutputTask->m_TaskIndex] = pCANDigitalOutputTask;

    // switch on the vacuum valve 1
    pCANDigitalOutputTask = CBaseDevice::GetNewCANDigitalOutputTask(CANDigitalOutputTask::FCTMOD_DO_TASK_ID_SET_VALUE);
    pCANDigitalOutputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANDigitalOutputTask->m_TaskIndex = FCT_TASK_ID_CS_GRAB_VACUUM_VALVE_1_ON;
    pCANDigitalOutputTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_DOWN;
    pCANDigitalOutputTask->m_pCANObject = (CModule*) pCoverLine->m_pDigOutpVacuumValve1;
    pCANDigitalOutputTask->m_Value = 1;
    pCANDigitalOutputTask->m_Timeout = 500;
    pDeviceTask->m_FctModTaskMap[pCANDigitalOutputTask->m_TaskIndex] = pCANDigitalOutputTask;

    // switch on the vacuum valve 2
    pCANDigitalOutputTask = CBaseDevice::GetNewCANDigitalOutputTask(CANDigitalOutputTask::FCTMOD_DO_TASK_ID_SET_VALUE);
    pCANDigitalOutputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANDigitalOutputTask->m_TaskIndex = FCT_TASK_ID_CS_GRAB_VACUUM_VALVE_2_ON;
    pCANDigitalOutputTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_DOWN;
    pCANDigitalOutputTask->m_pCANObject = (CModule*) pCoverLine->m_pDigOutpVacuumValve2;
    pCANDigitalOutputTask->m_Value = 1;
    pCANDigitalOutputTask->m_Timeout = 500;
    pDeviceTask->m_FctModTaskMap[pCANDigitalOutputTask->m_TaskIndex] = pCANDigitalOutputTask;

    // receive the analog signal from vacuum sensor if the vacuum cup contacts the coverslip
    pCANAnalogInputTask = new CANAnalogInputTask();
    pCANAnalogInputTask->m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_ANA_INPUT;
    pCANAnalogInputTask->m_AnaInpTaskID = CANAnalogInputTask::FCTMOD_AI_TASK_ID_WAIT_VALUE_ABOVE;
    pCANAnalogInputTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANAnalogInputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANAnalogInputTask->m_TaskIndex = FCT_TASK_ID_CS_GRAB_VACUUM_SENSOR_ON;
    pCANAnalogInputTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_DOWN;
    pCANAnalogInputTask->m_pCANObject = (CModule*) pCoverLine->m_pAnaInpPressureSensor;
    pCANAnalogInputTask->m_Value = 0;
    pCANAnalogInputTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANAnalogInputTask->m_TaskIndex] = pCANAnalogInputTask;

    // stop command for z-axis, will be send after the vacuum sensor detects coverslip contact
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_SPEED);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_MANUAL;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_STOP;
    //pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_DOWN;  //obsolete
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Z];
    pCANStepperMotorTask->m_TargetSpeed = 0;  // motor stop command
    pCANStepperMotorTask->m_Timeout = 1000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // move z-axis with the coverslip up to transport postion
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_UP;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_STOP;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Z];
    pCANStepperMotorTask->m_TargetPos = 50;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // receive the analog signal from hall sensor to ensure a coverslip is transported
    pCANAnalogInputTask = new CANAnalogInputTask();
    pCANAnalogInputTask->m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_ANA_INPUT;
    pCANAnalogInputTask->m_AnaInpTaskID = CANAnalogInputTask::FCTMOD_AI_TASK_ID_REQ_ACT_VALUE;
    pCANAnalogInputTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANAnalogInputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANAnalogInputTask->m_TaskIndex = FCT_TASK_ID_CS_GRAB_HALL_VALUE;
    pCANAnalogInputTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_UP;
    pCANAnalogInputTask->m_pCANObject = (CModule*) pCoverLine->m_pAnaInpHallSensor1;
    pCANAnalogInputTask->m_Value = 0;
    pCANAnalogInputTask->m_Timeout = 500;
    pDeviceTask->m_FctModTaskMap[pCANAnalogInputTask->m_TaskIndex] = pCANAnalogInputTask;

    // move y-axis up to cover postion (wait position closed to the cover position)
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_CS_GRAB_MOTOR_CS_Y_COVER_POS;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_GRAB_HALL_VALUE;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Y];
    pCANStepperMotorTask->m_TargetPos = 900;
    pCANStepperMotorTask->m_Profile = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    FunctionModuleTask* pFunctionModuleTask;
    QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(pDeviceTask->m_FctModTaskMap);
    while (iterFctModTask.hasNext())
    {
        pFunctionModuleTask = iterFctModTask.next().value();
        pFunctionModuleTask->m_pDeviceTask = pDeviceTask;
    }

    FILE_LOG_L(laDEV, llINFO) << "      add: reference run clamp motor";

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Create the function module tasks need to shift back the slide
 *
 *      The process includes just the following task:
 *       - shift back slide
 *
 *  \iparam pCoverLine = Cover line device
 *  \iparam pDeviceTask = Device task for this action
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *          otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CCoverLineNavigator::CreateSlideShiftBackTasks(CCoverLineDevice* pCoverLine,
                                                            DeviceTask* pDeviceTask)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8  TaskIndex = 1;
    CANStepperMotorTask*  pCANStepperMotorTask;

    FILE_LOG_L(laDEV, llINFO) << " CreateSlideShiftBackTasks";

    //task for slide shift back
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_SLIDE];
    pCANStepperMotorTask->m_TargetPos = 10;
    pCANStepperMotorTask->m_Profile = 1;
    pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_ACKN;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;

    pCANStepperMotorTask->m_pDeviceTask = pDeviceTask;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Creates tasks to slip the cover onto the slide
 *
 *      Create the function module tasks need cover the slide with the cover
 *      slip. This is the 'most important action', the slide will be cover
 *      now. The coverslip is transported down to the slide, the clamp opened
 *      and the vauum valves are switch off to release the coverslip. Then
 *      the z-axis is moved up, and the hall sensor will be read to ensure
 *      the coverslip was left.
 *
 *      The process includes the following tasks:
 *       - move z-axis down to slide
 *       - switch vacuum valves 1 and 2 off  to release the coverslip
 *       - open the clamp
 *       - lift up z-axis
 *       - test hall sensor to ensure the coverslip was left down
 *
 *  \iparam pCoverLine = Cover line device
 *  \iparam pDeviceTask = Device task for this action
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *          otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CCoverLineNavigator::CreateCoverSlipCoverTasks(CCoverLineDevice* pCoverLine,
                                                            DeviceTask* pDeviceTask)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8  TaskIndex = 1;
    CANStepperMotorTask*  pCANStepperMotorTask;
    CANDigitalOutputTask* pCANDigitalOutputTask;
    CANAnalogInputTask*   pCANAnalogInputTask;

    FILE_LOG_L(laDEV, llINFO) << " CreateCoverSlipCoverTasks";

    // task 1, move y-axis to cover position
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Y];
    pCANStepperMotorTask->m_TargetPos = 1200;
    pCANStepperMotorTask->m_Profile = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;

    //task 2 for z-axis cover movement middle speed
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Z];
    pCANStepperMotorTask->m_TargetPos = 600;
    pCANStepperMotorTask->m_Profile = 1;
    pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_ACKN;
    pCANStepperMotorTask->m_SubCommandData = 0; // 300 ms
    pCANStepperMotorTask->m_Timeout = 2000;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;

    //task 3 for z-axis cover movement slow speed
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_TIMEOFFSET;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Z];
    pCANStepperMotorTask->m_TargetPos = 900;
    pCANStepperMotorTask->m_Profile = 2;
    pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_POS_DELAY;     // time delay, internal triggered
    pCANStepperMotorTask->m_SubCommandData = 300; // 300 ms
    pCANStepperMotorTask->m_OffsetTime = 200;     // 200ms time delay (start condition of the task)
    pCANStepperMotorTask->m_Timeout = 1500;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;

    //task 4 for clamp open
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_TIMEOFFSET;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 2;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CLAMP];
    pCANStepperMotorTask->m_TargetPos = 10;
    pCANStepperMotorTask->m_Profile = 1;
    pCANStepperMotorTask->m_OffsetTime = 1000;     // 200ms time delay (start condition of the task)
    pCANStepperMotorTask->m_Timeout = 1500;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;

    // task 5, switch the vacuum valve 1
    pCANDigitalOutputTask = CBaseDevice::GetNewCANDigitalOutputTask(CANDigitalOutputTask::FCTMOD_DO_TASK_ID_SET_VALUE);
    pCANDigitalOutputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANDigitalOutputTask->m_TaskIndex = TaskIndex;
    pCANDigitalOutputTask->m_TriggerTaskIndex = TaskIndex - 2;
    pCANDigitalOutputTask->m_pCANObject = (CModule*) pCoverLine->m_pDigOutpVacuumValve1;
    pCANDigitalOutputTask->m_Value = 0;
    pCANDigitalOutputTask->m_Timeout = 500;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANDigitalOutputTask;
    TaskIndex++;

    // task 6, switch the vacuum valve 2
    pCANDigitalOutputTask = CBaseDevice::GetNewCANDigitalOutputTask(CANDigitalOutputTask::FCTMOD_DO_TASK_ID_SET_VALUE);
    pCANDigitalOutputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS;
    pCANDigitalOutputTask->m_TaskIndex = TaskIndex;
    pCANDigitalOutputTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANDigitalOutputTask->m_pCANObject = (CModule*) pCoverLine->m_pDigOutpVacuumValve2;
    pCANDigitalOutputTask->m_Value = 0;
    pCANDigitalOutputTask->m_Timeout = 500;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANDigitalOutputTask;
    TaskIndex++;

    //task 7 for z-axis move up, delay 200 ms
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = TaskIndex;
    pCANStepperMotorTask->m_TriggerTaskIndex = TaskIndex - 3;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Z];
    pCANStepperMotorTask->m_TargetPos = 50;
    pCANStepperMotorTask->m_Profile = 1;
    pCANStepperMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_TIME_DELAY;     // time delay, internal triggered
    pCANStepperMotorTask->m_SubCommandData = 200; // 300 ms
    pCANStepperMotorTask->m_Timeout = 2000;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANStepperMotorTask;
    TaskIndex++;

    //task 8, receive the analog signal from hall sensor to ensure removed coverslip
    pCANAnalogInputTask = new CANAnalogInputTask();
    pCANAnalogInputTask->m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_ANA_INPUT;
    pCANAnalogInputTask->m_AnaInpTaskID = CANAnalogInputTask::FCTMOD_AI_TASK_ID_REQ_ACT_VALUE;
    pCANAnalogInputTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANAnalogInputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANAnalogInputTask->m_TaskIndex = TaskIndex;
    pCANAnalogInputTask->m_TriggerTaskIndex = TaskIndex - 1;
    pCANAnalogInputTask->m_pCANObject = (CModule*) pCoverLine->m_pAnaInpHallSensor1;
    pCANAnalogInputTask->m_Value = 0;
    pCANAnalogInputTask->m_Timeout = 500;
    pDeviceTask->m_FctModTaskMap[TaskIndex] = pCANAnalogInputTask;

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
 *  \brief Creates tasks to discard a broken cover slips
 *
 *      Create the function module tasks need to discard a (broken) cover
 *      slip. This action moves a cover slip, which was grabbed before, to
 *      the discard position and releases the vaccum to discard the it. Then
 *      the y-axis will be moved back to the depot positon.
 *
 *      The process includes the following tasks:
 *       - move y-axis to discard position
 *       - move x-axis a little bit down
 *       - switch vacuum valves 1 and 2 off to release the coverslip
 *       - move x-axis up
 *       - move y-axis to depot position
 *
 *  \iparam pCoverLine = Cover line device
 *  \iparam pDeviceTask = Device task for this action
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *          otherwise the return value of the error detecting function
 */
/****************************************************************************/
ReturnCode_t CCoverLineNavigator::CreateCoverSlipDiscardTasks(CCoverLineDevice* pCoverLine,
                                                              DeviceTask* pDeviceTask)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CANStepperMotorTask*  pCANStepperMotorTask;
    CANDigitalOutputTask* pCANDigitalOutputTask;
    //CANAnalogInputTask*   pCANAnalogInputTask;

    FILE_LOG_L(laDEV, llINFO) << " CreateCoverSlipDiscardTasks";

    // move y-axis to discard position
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_FIRST;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Y_DISCARD_POS;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Y_DISCARD_POS;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Y];
    pCANStepperMotorTask->m_TargetPos = 2000;  //y-axis discard position
    pCANStepperMotorTask->m_Profile = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // move z-axis down to discover
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Z_DOWN;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Y_DISCARD_POS;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Z];
    pCANStepperMotorTask->m_TargetPos = 400;
    pCANStepperMotorTask->m_Profile = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    //switch the vacuum valve 1 off
    pCANDigitalOutputTask = CBaseDevice::GetNewCANDigitalOutputTask(CANDigitalOutputTask::FCTMOD_DO_TASK_ID_SET_VALUE);
    pCANDigitalOutputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_TIMEOFFSET;
    pCANDigitalOutputTask->m_TaskIndex = FCT_TASK_ID_CS_DISCARD_VACUUM_VALVE_1_OFF;
    pCANDigitalOutputTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Z_DOWN;
    pCANDigitalOutputTask->m_pCANObject = (CModule*) pCoverLine->m_pDigOutpVacuumValve1;
    pCANDigitalOutputTask->m_Value = 0;
    pCANDigitalOutputTask->m_OffsetTime = 500;
    pCANDigitalOutputTask->m_Timeout = 500;
    pDeviceTask->m_FctModTaskMap[pCANDigitalOutputTask->m_TaskIndex] = pCANDigitalOutputTask;

    //switch the vacuum valve 2 off
    pCANDigitalOutputTask = CBaseDevice::GetNewCANDigitalOutputTask(CANDigitalOutputTask::FCTMOD_DO_TASK_ID_SET_VALUE);
    pCANDigitalOutputTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_TIMEOFFSET;
    pCANDigitalOutputTask->m_TaskIndex = FCT_TASK_ID_CS_DISCARD_VACUUM_VALVE_2_OFF;
    pCANDigitalOutputTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Z_DOWN;
    pCANDigitalOutputTask->m_pCANObject = (CModule*) pCoverLine->m_pDigOutpVacuumValve2;
    pCANDigitalOutputTask->m_Value = 0;
    pCANDigitalOutputTask->m_OffsetTime = 500;
    pCANDigitalOutputTask->m_Timeout = 500;
    pDeviceTask->m_FctModTaskMap[pCANDigitalOutputTask->m_TaskIndex] = pCANDigitalOutputTask;

    // move z-axis uo to transport position
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Z_UP;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Z_DOWN;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Z];
    pCANStepperMotorTask->m_TargetPos = 50;
    pCANStepperMotorTask->m_Profile = 0;
    pCANStepperMotorTask->m_Timeout = 5000;
    pDeviceTask->m_FctModTaskMap[pCANStepperMotorTask->m_TaskIndex] = pCANStepperMotorTask;

    // move y-axis to coverslip depot position
    pCANStepperMotorTask = CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS);
    pCANStepperMotorTask->m_StartTrigger = FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED;
    pCANStepperMotorTask->m_TaskIndex = FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Y_DEPOT_POS;
    pCANStepperMotorTask->m_TriggerTaskIndex = FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Z_UP;
    pCANStepperMotorTask->m_pCANObject = pCoverLine->m_pMotorCoverLine[CL_MOTOR_CS_Y];
    pCANStepperMotorTask->m_TargetPos = 50;  //y-axis discard position
    pCANStepperMotorTask->m_Profile = 0;
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
 *  \brief  Handles the tasks of the cover line process
 *
 *  \iparam pCoverLineDev = Cover line device
 *  \iparam deviceTaskMap = Device task map of the cover kine process
 */
/****************************************************************************/
void CCoverLineNavigator::HandleCoverLineDeviceTasks(CCoverLineDevice* pCoverLineDev, DeviceTaskMap& deviceTaskMap)
{
    //FILE_LOG_L(laDEV, llINFO) << " SubState ==  " << (int) m_SubStateCover;
    if(pCoverLineDev->m_CoverSlipControl.m_SubStateCover == 1)
    {
        if(deviceTaskMap[DEVICE_TASK_BASIS_POS]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            FILE_LOG_L(laDEV, llINFO) << " device task DEVICE_TASK_RACK_POS == " << (int) deviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState;
            if(deviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState == DeviceTask::DEV_TASK_STATE_INIT)
            {
                //move the rack to the topmost position
                deviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
                ((DeviceTask*) deviceTaskMap[DEVICE_TASK_RACK_POS])->m_timeAction.Trigger();
                ((DeviceTask*) deviceTaskMap[DEVICE_TASK_RACK_POS])->m_aktionTimespan = 8000;

                FILE_LOG_L(laDEV, llINFO) << "##########################################################";
                FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_RACK_POS to 'progress'.";

                //grab a coverslip from depot
                deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
                ((DeviceTask*) deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB])->m_timeAction.Trigger();
                ((DeviceTask*) deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB])->m_aktionTimespan = 12000;
                FILE_LOG_L(laDEV, llINFO) << "##########################################################";
                FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_COVER_SLIP_GRAB to 'progress'.";

                pCoverLineDev->m_CoverSlipControl.m_SubStateCover = 2;
            }
        }
    }
    else if(pCoverLineDev->m_CoverSlipControl.m_SubStateCover == 2)
    {
        /// \todo Move the rack to topmost position. If a slide was detected while move up, the coverslib grab task
        //      will be started.

        //rack is in topmost position and a coverslip was grabbed
        if((deviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED) &&
           (deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED))
        {
            //shift out the slide
            deviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_OUT]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            ((DeviceTask*) deviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_OUT])->m_timeAction.Trigger();
            ((DeviceTask*) deviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_OUT])->m_aktionTimespan = 15000;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_SLIDE_SHIFT_OUT to 'progress'.";

            //move coverslip to 'coverslip prepare position'
            /*m_deviceTaskMap[DEVICE_TASK_COVER_SLIP_PREP_COV]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            ((DeviceTask*) m_deviceTaskMap[DEVICE_TASK_COVER_SLIP_PREP_COV])->m_timeAction.Trigger();
            ((DeviceTask*) m_deviceTaskMap[DEVICE_TASK_COVER_SLIP_PREP_COV])->m_aktionTimespan = 8000;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_COVER_SLIP_PREP_COV to 'progress'.";*/

            pCoverLineDev->m_CoverSlipControl.m_SubStateCover = 3;
            FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 3";
        }
        else if (deviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED_WITH_INFO)
        {
            pCoverLineDev->m_CoverSlipControl.m_SubStateCover = 99;
            FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 99";
        }
        //coverslip grabbing has an unattractive result
        else if (deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED_WITH_INFO)
        {
            //the grabbed coverslip is brocken
            if(deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskInfo == DeviceTask::DEV_TASK_STATE_CS_GRAB_COVERSLIP_BROCKEN)
            {
                // set the coverslip grab tastto 'ready'
                deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskState = DeviceTask::DEV_TASK_STATE_INIT;
                // start the coverslip engage task
                FILE_LOG_L(laDEV, llINFO) << "##########################################################";
                FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_COVER_SLIP_DISCARD to 'progress'.";
                deviceTaskMap[DEVICE_TASK_COVER_SLIP_DISCARD]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
                ((DeviceTask*) deviceTaskMap[DEVICE_TASK_COVER_SLIP_DISCARD])->m_timeAction.Trigger();
                ((DeviceTask*) deviceTaskMap[DEVICE_TASK_COVER_SLIP_DISCARD])->m_aktionTimespan = 15000;

                pCoverLineDev->m_CoverSlipControl.m_SubStateCover = 10;
                FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 10";
            }
        }
        //rack positoning task has an unattractive result
        else if (deviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED_WITH_INFO)
        {
            //there was nor slide detected at the rack
            if(deviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskInfo == DeviceTask::DEVICE_TASK_STATE_RACK_POS_NO_SLIDE)
            {
                //finish the stuff
                pCoverLineDev->m_CoverSlipControl.m_SubStateCover = 99;
                FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 99";
            }
        }
    }
    else if(pCoverLineDev->m_CoverSlipControl.m_SubStateCover == 3)
    {
        //slide was shift out, clamped and ready to cover, and a coverslip is at the cover 'coverslip prepare position'
        if(deviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_OUT]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            //cover the slide
            deviceTaskMap[DEVICE_TASK_COVER_SLIP_EXEC_COVER]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            ((DeviceTask*) deviceTaskMap[DEVICE_TASK_COVER_SLIP_EXEC_COVER])->m_timeAction.Trigger();
            ((DeviceTask*) deviceTaskMap[DEVICE_TASK_COVER_SLIP_EXEC_COVER])->m_aktionTimespan = 20000;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_COVER_SLIP_EXEC_COVER to 'progress'.";

            FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 3";
            pCoverLineDev->m_CoverSlipControl.m_SubStateCover = 4;
        }
    }
    else if(pCoverLineDev->m_CoverSlipControl.m_SubStateCover == 4)
    {
        //slide was covered
        if(deviceTaskMap[DEVICE_TASK_COVER_SLIP_EXEC_COVER]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            //shift the slide back into the rack
            deviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_BACK]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            ((DeviceTask*) deviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_BACK])->m_timeAction.Trigger();
            ((DeviceTask*) deviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_BACK])->m_aktionTimespan = 6000;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_SLIDE_SHIFT_BACK to 'progress'.";

            FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 5";
            pCoverLineDev->m_CoverSlipControl.m_SubStateCover = 5;
        }
    }
    else if(pCoverLineDev->m_CoverSlipControl.m_SubStateCover == 5)
    {
        if(deviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_BACK]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            FunctionModuleTask*  pFunctionModuleTask;
            CANStepperMotorTask* pCANStepperMotorTask;

            //the cover procedure of a slide was finished
            ResetDeviceTask(deviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_OUT]);
            ResetDeviceTask(deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]);
            ResetDeviceTask(deviceTaskMap[DEVICE_TASK_COVER_SLIP_EXEC_COVER]);
            ResetDeviceTask(deviceTaskMap[DEVICE_TASK_SLIDE_SHIFT_BACK]);
            ResetDeviceTask(deviceTaskMap[DEVICE_TASK_COVER_SLIP_DISCARD]);
            ResetDeviceTask(deviceTaskMap[DEVICE_TASK_RACK_POS]);

            // restart rack positioning to enable the coverage of the next slide
            deviceTaskMap[DEVICE_TASK_RACK_POS]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            ((DeviceTask*) deviceTaskMap[DEVICE_TASK_RACK_POS])->m_timeAction.Trigger();
            ((DeviceTask*) deviceTaskMap[DEVICE_TASK_RACK_POS])->m_aktionTimespan = 6000;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_RACK_POS to 'progress'.";

            //grab a coverslip from depot
            deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            ((DeviceTask*) deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB])->m_timeAction.Trigger();
            ((DeviceTask*) deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB])->m_aktionTimespan = 12000;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_COVER_SLIP_GRAB to 'progress'.";


            pFunctionModuleTask = deviceTaskMap[DEVICE_TASK_RACK_POS]->m_FctModTaskMap[1];
            pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
            if(pCANStepperMotorTask)
            {
                if(pCANStepperMotorTask->m_TargetPos > 50)
                {
                    //move rack to the next slide position
                    pCANStepperMotorTask->m_TargetPos -= 50;
                    FILE_LOG_L(laDEV, llINFO) << "  TargetPosition: " <<  pCANStepperMotorTask->m_TargetPos;
                    deviceTaskMap[DEVICE_TASK_RACK_POS]->m_FctModTaskMap[1]->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
                    deviceTaskMap[DEVICE_TASK_RACK_POS]->m_FctModTaskMap[2]->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
                }
                else
                {
                    //move rack to the transfer position, device task will finish there
                    pCANStepperMotorTask->m_TargetPos = -20;
                    deviceTaskMap[DEVICE_TASK_RACK_POS]->m_FctModTaskMap[1]->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
                    FILE_LOG_L(laDEV, llINFO) << "  finish, all slides covered.";

                    //Set anything to finish the processing!!
                }
            }
            pCoverLineDev->m_CoverSlipControl.m_SubStateCover = 2;
            FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 2";
        }
    }
    else if(pCoverLineDev->m_CoverSlipControl.m_SubStateCover == 10)
    {
        //coverslip was engaged
        if(deviceTaskMap[DEVICE_TASK_COVER_SLIP_DISCARD]->m_TaskState == DeviceTask::DEV_TASK_STATE_FINISHED)
        {
            ResetDeviceTask(deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]);
            ResetDeviceTask(deviceTaskMap[DEVICE_TASK_COVER_SLIP_DISCARD]);

            pCoverLineDev->m_CoverSlipControl.m_SubStateCover = 2;
            FILE_LOG_L(laDEV, llINFO) << "  set SubStateCover to 2";

            //try next coverslip
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task DEVICE_TASK_COVER_SLIP_GRAB to 'progress'.";
            deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB]->m_TaskState = DeviceTask::DEV_TASK_STATE_PROGRESS;
            ((DeviceTask*) deviceTaskMap[DEVICE_TASK_COVER_SLIP_GRAB])->m_timeAction.Trigger();
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Event handling for reference run function module task
 *
 *      Will be called whenever a function module signal whichs
 *
 *  \iparam pFunctionModuleTask = Function module task for this action
 *  \iparam pDeviceTask = Device task for this action
 */
/****************************************************************************/
void CCoverLineNavigator::HandleRefRunTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask)
{
    Q_UNUSED(pDeviceTask);

    FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished.";
    pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
}

/****************************************************************************/
/*!
 *  \brief  Event handling for basis positioning task
 *
 *      The task moves the axis to its basis position, there are just
 *      CANStepperMotorTask to handle. the function cecks if there is at
 *      least one unfinished task left. If not, the device task state is set
 *      to 'finished'.
 *
 *  \iparam pFunctionModuleTask = Function module task for this action
 *  \iparam pDeviceTask = Device task for this action
 */
/****************************************************************************/
void CCoverLineNavigator::HandleBasisPositionTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask)
{
    QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(pDeviceTask->m_FctModTaskMap);
    FunctionModuleTask* pFunctionModuleTask1;
    bool Finished = true;

    if((pFunctionModuleTask == 0) || (pDeviceTask == 0))
    {
        return;
    }

    FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished.";
    pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;

    while (iterFctModTask.hasNext())
    {
        pFunctionModuleTask1 =  iterFctModTask.next().value();
        if((pFunctionModuleTask1) && (pFunctionModuleTask1->m_TaskState != FunctionModuleTask::FCTMOD_STATE_FINISHED))
        {
            Finished = false;
        }
    }
    if(Finished)
    {
        //set device task state to finished if ther are no more unfinished fct-module tasks left
        pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
        FILE_LOG_L(laDEV, llINFO) << "##########################################################";
        FILE_LOG_L(laDEV, llINFO) << "  set device task 'BasisPosition' to 'finished'.";
    }
}

/****************************************************************************/
/*!
 *  \brief  Event handling for basis positioning task
 *
 *      The task moves the axis to its basis position, there are just
 *      CANStepperMotorTask to handle. the function checks if there is at
 *      least one unfinished task left. If not, the device task state is set
 *      to 'finished'.
 *
 *  \iparam pFunctionModuleTask = Function module task for this action
 *  \iparam pDeviceTask = Device task for this action
 */
/****************************************************************************/
void CCoverLineNavigator::HandleRackPositionTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask)
{
    CANStepperMotorTask* pCANStepperMotorTask;

    if((pFunctionModuleTask == 0) || (pDeviceTask == 0))
    {
        return;
    }

    FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished.";
    pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;

    if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
    {

        FILE_LOG_L(laDEV, llINFO) << "  FunctionModuleTask::FCTMOD_TASK_ID_MOTOR";
        //read of digital input 'slide detection' is done automatically
        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pCANStepperMotorTask)
        {
            if(pCANStepperMotorTask->m_TargetPos == -20)
            {
                FILE_LOG_L(laDEV, llINFO) << " Set RackPosition task to DEV_TASK_STATE_FINISHED_WITH_INFO.";
                pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED_WITH_INFO;
            }
        }
    }
    else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_DIG_INPUT)
    {
        FunctionModuleTask*  pFunctionModuleTask1;
        CANDigitalInputTask* pCANDigitalInputTask;

        FILE_LOG_L(laDEV, llINFO) << "  FunctionModuleTask::FCTMOD_TASK_ID_DIG_INPUT";

        pCANDigitalInputTask = dynamic_cast<CANDigitalInputTask*>(pFunctionModuleTask);
        if(pCANDigitalInputTask != 0)
        {
            if(pCANDigitalInputTask->m_Value == 0)
            {
                //there is no slideto shift out at current elevator position.
                //pick up the motor task to determine the elevator positon
                pFunctionModuleTask1 = pDeviceTask->m_FctModTaskMap[FCT_TASK_ID_RACK_POS_MOTOR_ELEVATOR];
                pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask1);
                if(pCANStepperMotorTask)
                {
                    //examine the elevator motor positon
                    if(pCANStepperMotorTask->m_TargetPos > 50)
                    {
                        //the elevator hasn't reached its lowest slide position,
                        // move rack to the next slide position
                        pCANStepperMotorTask->m_TargetPos -= 50;
                        FILE_LOG_L(laDEV, llINFO) << "  TargetPosition: " <<  pCANStepperMotorTask->m_TargetPos;

                        QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(pDeviceTask->m_FctModTaskMap);

                        while (iterFctModTask.hasNext())
                        {
                            pFunctionModuleTask1 =  iterFctModTask.next().value();
                            pFunctionModuleTask1->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
                        }
                    }
                    else
                    {
                        //the elevator hasreached its lowest slide position, no more slides to cover are left
                        // move rack to the transfer position, device task will finish there
                        pCANStepperMotorTask->m_TargetPos = -20;
                        /// \todo do not finich device task here, finich it when it reached the transfer positon!!!
                        pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
                        FILE_LOG_L(laDEV, llINFO) << "  finish, all slides covered.";
                    }
                }
            }
            else
            {
                //mark the device task as finished to force acknowledge
                pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
                FILE_LOG_L(laDEV, llINFO) << "##########################################################";
                FILE_LOG_L(laDEV, llINFO) << "  set device task 'RackPosition' to 'finished'.";
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Event handling for coverslip grabbing task
 *
 *      The task moves the z-axis down into the coverslip depot, grabs a
 *      coverslip, moves z axis up and positions the y-axis to the cover
 *      position.
 *
 *  \iparam pFunctionModuleTask = Function module task for this action
 *  \iparam pDeviceTask = Device task for this action
 *  \iparam pCoverLineDev = Cover line device
 */
/****************************************************************************/
void CCoverLineNavigator::HandleCoverslipGrabTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask, CCoverLineDevice* pCoverLineDev)
{
    CANAnalogInputTask*  pCANAnalogInputTask;
    CANStepperMotorTask* pCANStepperMotorTask;
    FunctionModuleTask*  pFunctionModuleTask1;

    if((pFunctionModuleTask == 0) || (pDeviceTask == 0) || (pCoverLineDev == 0))
    {
        return;
    }

    if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_ANA_INPUT)
    {
        FILE_LOG_L(laDEV, llINFO) << "  FunctionModuleTask::FCTMOD_TASK_ID_ANA_INPUT";

        pCANAnalogInputTask = dynamic_cast<CANAnalogInputTask*>(pFunctionModuleTask);
        if(pCANAnalogInputTask)
        {
            if(pCANAnalogInputTask->m_pCANObject->GetModuleHandle() == pCoverLineDev->m_pAnaInpPressureSensor->GetModuleHandle())
            {
                //if the vacuum sensors value increases the level, a motor stop command will be sent to the z-axsis
                if(pCANAnalogInputTask->m_Value > 0xE000)
                {
                    FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished.";
                    pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;

                    //get the function module task 'z-axis motor stop'
                    pFunctionModuleTask1 = pDeviceTask->m_FctModTaskMap[FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_STOP];
                    pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask1);
                    if(pCANStepperMotorTask)
                    {
                        // and activate it by setting its state to 'progress'
                        FILE_LOG_L(laDEV, llINFO) << "  Stop coverslip z-axis motor.";
                        pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_WAIT_PROGRESS;
                    }
                }
            }
            else if(pCANAnalogInputTask->m_pCANObject->GetModuleHandle() == pCoverLineDev->m_pAnaInpHallSensor1->GetModuleHandle())
            {
                FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pCANAnalogInputTask->m_TaskIndex << " state to finished.";
                pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;

                //check hall sensor to identify a usable coverslip
                //if the vacuum sensors value increases the level, a motor stop command will be sent to the z-axsis
                if(pCANAnalogInputTask->m_Value > 0xE000)
                {
                    FILE_LOG_L(laDEV, llINFO) << "  Set condition 'CoverslipOK' to true.";
                    pCoverLineDev->m_CoverSlipControl.m_ConditionCoverslipOK = true;

                    //coverslip OK, move it to cover position
                    //get the function module task 'y-axis motor move to cover position'
                    pFunctionModuleTask1 = pDeviceTask->m_FctModTaskMap[FCT_TASK_ID_CS_GRAB_MOTOR_CS_Y_COVER_POS];
                    pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask1);
                    if(pCANStepperMotorTask)
                    {
                        //and activate it
                        FILE_LOG_L(laDEV, llINFO) << "  Move y-axis to cover position.";
                        pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_WAIT_PROGRESS;
                    }
                    //set state 'coverslip OK' to enable the 'Slide shift out' task

                }
                else
                {
                    //coverslip not OK, move it to desease position
                    FILE_LOG_L(laDEV, llINFO) << " Set CoverslipGrab task to DEV_TASK_STATE_FINISHED_WITH_INFO.";
                    pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED_WITH_INFO;
                    pDeviceTask->m_TaskInfo = DeviceTask::DEV_TASK_STATE_CS_GRAB_COVERSLIP_BROCKEN;
                    pCoverLineDev->m_CoverSlipControl.m_ConditionCoverslipFailed = true;
                }

            }
        } //else invalide base class
    }
    else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
    {
        FILE_LOG_L(laDEV, llINFO) << "  FunctionModuleTask::FCTMOD_TASK_ID_MOTOR";

        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pCANStepperMotorTask)
        {
            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pCANStepperMotorTask->m_TaskIndex << " state to finished.";
            pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;

            if((pCANStepperMotorTask->m_pCANObject->GetModuleHandle() == pCoverLineDev->m_pMotorCoverLine[CL_MOTOR_CS_Y]->GetModuleHandle()) &&
               (pCANStepperMotorTask->m_TaskIndex > 3))
            {
                //mark the device task as finished to force acknowledge
                pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
                FILE_LOG_L(laDEV, llINFO) << "##########################################################";
                FILE_LOG_L(laDEV, llINFO) << "  set device task 'CoverslipGrab' to 'finished'.";
            }
        } //else invalide base class
    }
}

/****************************************************************************/
/*!
 *  \brief  Event handling for coverslip discard task
 *
 *      The task moves the coverslip to discard position, releases it, and
 *      moves back to coverslip depot.
 *
 *  \iparam pFunctionModuleTask = Function module task for this action
 *  \iparam pDeviceTask = Device task for this action
 *  \iparam pCoverLineDev = Cover line device
 */
/****************************************************************************/
void CCoverLineNavigator::HandleCoverslipDiscardTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask, CCoverLineDevice* pCoverLineDev)
{
    if((pFunctionModuleTask == 0) || (pDeviceTask == 0) || (pCoverLineDev == 0))
    {
        return;
    }

    pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
    if(pFunctionModuleTask->m_TaskIndex == FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Y_DEPOT_POS)
    {
        FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pFunctionModuleTask->m_TaskIndex << " state to finished.";
        pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
    }
}

/****************************************************************************/
/*!
 *  \brief  Event handling for slide shift out task
 *
 *      The task moves the slide shifter out, controls the EDM pump and,
 *      after reaching the cover position, activates the clamp.
 *
 *  \iparam pFunctionModuleTask = Function module task for this action
 *  \iparam pDeviceTask = Device task for this action
 *  \iparam pCoverLineDev = Cover line device
 */
/****************************************************************************/
void CCoverLineNavigator::HandleSlideShiftOutTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask, CCoverLineDevice* pCoverLineDev)
{
    CANStepperMotorTask* pCANStepperMotorTask;

    if((pFunctionModuleTask == 0) || (pDeviceTask == 0) || (pCoverLineDev == 0))
    {
        return;
    }

    if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
    {
        FILE_LOG_L(laDEV, llINFO) << "  FunctionModuleTask::FCTMOD_TASK_ID_MOTOR";

        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pCANStepperMotorTask)
        {
            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pCANStepperMotorTask->m_TaskIndex << " state to finished.";
            pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;

            if(pCANStepperMotorTask->m_pCANObject->GetModuleHandle() == pCoverLineDev->m_pMotorCoverLine[CL_MOTOR_CLAMP]->GetModuleHandle())
            {
                //mark the device task as finished to force acknowledge
                pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
                FILE_LOG_L(laDEV, llINFO) << "##########################################################";
                FILE_LOG_L(laDEV, llINFO) << "  set device task 'SlideShiftOut' to 'finished'.";
            }
        }//else invalide base class
    }
}

/****************************************************************************/
/*!
 *  \brief  Event handling for covering task
 *
 *      The task moves the z-axis down to position the coverslip on the
 *      slide. Vacuum will be switched off, clamp opens, and z-axis moves up
 *      to free the covered slide. (It's the task where the basic
 *      functionality of this machine is handled.)
 *
 *  \iparam pFunctionModuleTask = Function module task for this action
 *  \iparam pDeviceTask = Device task for this action
 *  \iparam pCoverLineDev = Cover line device
 */
/****************************************************************************/
void CCoverLineNavigator::HandleCoverTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask, CCoverLineDevice* pCoverLineDev)
{
    CANStepperMotorTask*  pCANStepperMotorTask;
    CANAnalogInputTask*   pCANAnalogInputTask;
    Q_UNUSED(pCoverLineDev);

    if((pFunctionModuleTask == 0) || (pDeviceTask == 0))
    {
        return;
    }

    if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
    {
        FILE_LOG_L(laDEV, llINFO) << "  FunctionModuleTask::FCTMOD_TASK_ID_MOTOR";

        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pCANStepperMotorTask)
        {
            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pCANStepperMotorTask->m_TaskIndex << " state to finished.";
            pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
        }//else invalide base class
    }
    else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_ANA_INPUT)
    {
        FILE_LOG_L(laDEV, llINFO) << "  FunctionModuleTask::FCTMOD_TASK_ID_ANA_INPUT";

        pCANAnalogInputTask = dynamic_cast<CANAnalogInputTask*>(pFunctionModuleTask);
        if(pCANAnalogInputTask)
        {
            pCANAnalogInputTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;

            //mark the device task as finished
            pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task 'Cover' to 'finished'.";
        }
        //else invalide base class

    }
    else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_DIG_INPUT)
    {
        CANDigitalInputTask* pCANDigitalInputTask;

        FILE_LOG_L(laDEV, llINFO) << "  FunctionModuleTask::FCTMOD_TASK_ID_DIG_INPUT";

        pCANDigitalInputTask = dynamic_cast<CANDigitalInputTask*>(pFunctionModuleTask);
        if(pCANDigitalInputTask)
        {
            pCANDigitalInputTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Event handling for slide shift back task
 *
 *      The task moves the slide, now covered, back to the rack.
 *
 *  \iparam pFunctionModuleTask = Function module task for this action
 *  \iparam pDeviceTask = Device task for this action
 *  \iparam pCoverLineDev = Cover line device
 */
/****************************************************************************/
void CCoverLineNavigator::HandleSlideShiftBackTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask, CCoverLineDevice* pCoverLineDev)
{
    CANStepperMotorTask*  pCANStepperMotorTask;
    Q_UNUSED(pCoverLineDev);

    if((pFunctionModuleTask == 0) || (pDeviceTask == 0))
    {
        return;
    }
    if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
    {
        FILE_LOG_L(laDEV, llINFO) << "  FunctionModuleTask::FCTMOD_TASK_ID_MOTOR";

        pCANStepperMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if(pCANStepperMotorTask)
        {
            FILE_LOG_L(laDEV, llINFO) << "  set task " << (int) pCANStepperMotorTask->m_TaskIndex << " state to finished.";
            pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_FINISHED;

            //mark the device task as finished
            pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_FINISHED;
            FILE_LOG_L(laDEV, llINFO) << "##########################################################";
            FILE_LOG_L(laDEV, llINFO) << "  set device task 'SlideShiftBack' to 'finished'.";
        }
    }

}

}  //namespace
