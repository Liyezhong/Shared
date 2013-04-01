/******************************************************************/
/*! \file FunctionModuleTaskManager.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 24.08.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implemetation of the class CFunctionModuleTaskManager
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

#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/Global/dcl_log.h"

#include <typeinfo>

namespace DeviceControl
{


/****************************************************************************/
/*!
 *  \brief  Constructor of DeviceTask
 */
/****************************************************************************/
DeviceTask::DeviceTask():
    m_Key(0), m_TaskState(DEV_TASK_STATE_UNUSED), m_TaskInfo(DEV_TASK_INFO_UNUSED),
    m_aktionTimespan(0)
{
}

/*DeviceTask::DeviceTask(quint8 CountFctModTasks) : m_TaskState(DEV_TASK_STATE_UNUSED)
{
    FunctionModuleTask* pFunctionModuleTask;
    for(quint8 idx = 0; idx < CountFctModTasks; idx++)
    {
        pFunctionModuleTask = new FunctionModuleTask();
        m_FctModTaskList.append(pFunctionModuleTask);
    }
}*/

/****************************************************************************/
/*!
 *  \brief  Creates and returns the function module task
 *
 *  \return Pointer to the function module task
 */
/****************************************************************************/
FunctionModuleTask* DeviceTask::CreateFunctionModuleTask()
{
    FunctionModuleTask* pFunctionModuleTask;
    pFunctionModuleTask = new FunctionModuleTask();
    return pFunctionModuleTask;
}

/****************************************************************************/
/*!
 *  \brief  Constructor of CFunctionModuleTaskManager
 */
/****************************************************************************/
CFunctionModuleTaskManager::CFunctionModuleTaskManager()
{
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CFunctionModuleTaskManager
 */
/****************************************************************************/
CFunctionModuleTaskManager::~CFunctionModuleTaskManager()
{
}

/****************************************************************************/
/*!
 *  \brief    Returns a pointer to the task which is pending for being started up, if any
 *
 *
 *  \iparam   taskMap = Task map containing the function module tasks
 *  \xparam   bActionStateInProgress = Returns true if already a task with state
 *                                     GRAPPLER_DEV_STATE_PROGRESS or GRAPPLER_DEV_STATE_INIT is in the task list.
 *                                     Returns false, if all tasks are finished, used by the calling device as finishing condition
 *  \xparam   bActionStateWithErrorDetect = returns true if a task with taskState==GrapplerDevTask::GRAPPLER_DEV_STATE_ERROR
 *            was detect.
 *
 *  \return   Pointer to FunctionModuleTask instance whichs task state is GRAPPLER_DEV_STATE_INIT and which can be started now.
 *            If no one found, 0-pointer will be returned.
 */
/****************************************************************************/
FunctionModuleTask* CFunctionModuleTaskManager::GetStartupTask(FunctionModuleTaskMap& taskMap, bool& bActionStateInProgress, bool& bActionStateWithErrorDetect)
{
    QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(taskMap);
    FunctionModuleTask* pFunctionModuleTask;
    FunctionModuleTask* pTriggerGrapplerDevTask;
    FunctionModuleTask* pFunctionModuleTaskRet = 0;
    bool DoNotStart = false;

    while (iterFctModTask.hasNext())
    {
        DoNotStart = false;
        pFunctionModuleTask = iterFctModTask.next().value();
        if(pFunctionModuleTask)
        {
            if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_FINISHED)
            {
                //this task has already finished, do not consider it
                continue;
            }
            else if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
            {
                // this task is currently in progress, set the flag used outside for 'state in progress' detection
                bActionStateInProgress = true;
            }
            else if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_INIT)
            {
                // this task is currently waiting for being set in progress state,
                // set the flag used outside for 'state in progress' detection
                // (there is not always a task in state 'progress', may one is waiting with a time delay.)
                bActionStateInProgress = true;

                // as long no startup task was found, check for one
                if(pFunctionModuleTaskRet == 0)
                {
                    //##################################
                    //check start condition for this task
                    if(pFunctionModuleTask->m_StartTrigger == FunctionModuleTask::FCTMOD_TASK_START_FIRST)
                    {
                        // the task is the 'first' task in queue, start it
                        FILE_LOG_L(laDEV, llINFO) << "  start task (FIRST) index " << (int) pFunctionModuleTask->m_TaskIndex;
                        pFunctionModuleTaskRet = pFunctionModuleTask;
                        break;
                    }
                    else if(pFunctionModuleTask->m_StartTrigger == FunctionModuleTask::FCTMOD_TASK_START_PREVIOUS_FINISHED)
                    {
                        // the task should be started if the previous task (m_TriggerTaskIndex) has finished
                        // -> get the previous task
                        pTriggerGrapplerDevTask = GetTaskFromIndex(taskMap, pFunctionModuleTask->m_TriggerTaskIndex);
                        //and check it for having finished
                        if(pTriggerGrapplerDevTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_FINISHED)
                        {
                            // check the previous task (m_TriggerTaskIndex) has a simultaneous start condition?
                            if(pTriggerGrapplerDevTask->m_StartTrigger == FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS)
                            {
                                quint8 Test = 0;
                                FILE_LOG_L(laDEV, llDEBUG) << "  check all simultaneous tasks for having finished. TaskID " << (int) pTriggerGrapplerDevTask->m_TaskIndex;
                                pTriggerGrapplerDevTask = GetTaskFromIndex(taskMap, pTriggerGrapplerDevTask->m_TriggerTaskIndex);
                                while(pTriggerGrapplerDevTask != 0)
                                {
                                    Test++;
                                    if(pTriggerGrapplerDevTask->m_StartTrigger != FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS)
                                    {
                                        break;
                                    }
                                    //FILE_LOG_L(laDEV, llDEBUG) << "  check simultaneous task for having finished. TaskID " << (int) pTriggerGrapplerDevTask->m_TaskIndex;
                                    if(pTriggerGrapplerDevTask->m_TaskState != FunctionModuleTask::FCTMOD_STATE_FINISHED)
                                    {
                                        //FILE_LOG_L(laDEV, llDEBUG) << "  simultaneous task not finished! TaskID " << (int) pTriggerGrapplerDevTask->m_TaskIndex;
                                        //one of the previous 'simultaneous' started task is not finished
                                        // -> start condition for this task is not fulfilled
                                        DoNotStart = true;
                                        break;
                                    }
                                    pTriggerGrapplerDevTask = GetTaskFromIndex(taskMap, pTriggerGrapplerDevTask->m_TriggerTaskIndex);
                                    if(Test > 10)
                                    {
                                        Test = 0;
                                    }
                                }
                            }
                            if(DoNotStart == false)
                            {
                                // if yes, start this task
                                FILE_LOG_L(laDEV, llINFO) << " startup task (NEXT)" << (int) pFunctionModuleTask->m_TaskIndex << ", FctMod: " << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();
                                pFunctionModuleTaskRet = pFunctionModuleTask;
                            }
                            //break;
                        }
                    }
                    else if(pFunctionModuleTask->m_StartTrigger == FunctionModuleTask::FCTMOD_TASK_START_SIMULTANEOUS)
                    {
                        // the task should be started simultaneously to the task referenced by 'm_TriggerTaskIndex'
                        // -> get the 'trigger'-task
                        pTriggerGrapplerDevTask = GetTaskFromIndex(taskMap, pFunctionModuleTask->m_TriggerTaskIndex);
                        if(pTriggerGrapplerDevTask != 0)
                        {
                            // and check, if it has already started (progress or finished..?)
                            if((pTriggerGrapplerDevTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS) ||
                               (pTriggerGrapplerDevTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_FINISHED))
                            {
                                // if yes, start this task
                                FILE_LOG_L(laDEV, llINFO) << " startup task (SIMULTAN) " << (int) pFunctionModuleTask->m_TaskIndex << ", FctMod: " << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();
                                pFunctionModuleTaskRet = pFunctionModuleTask;
                                //break;
                            }
                        }
                        else
                        {
                            FILE_LOG_L(laDEV, llERROR) << "  Null pointer returned by GetTaskFromIndex(" << (int) pFunctionModuleTask->m_TriggerTaskIndex << "), task: " << (int) pFunctionModuleTask->m_TaskIndex << ", FctMod: " << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();
                        }
                    }
                    else if(pFunctionModuleTask->m_StartTrigger == FunctionModuleTask::FCTMOD_TASK_START_TIMEOFFSET)
                    {
                        // the task should be started with a time offset to the start time of the task referenced by 'm_TriggerTaskIndex'
                        // -> get the 'trigger'-task
                        pTriggerGrapplerDevTask = GetTaskFromIndex(taskMap, pFunctionModuleTask->m_TriggerTaskIndex);
                        if(pTriggerGrapplerDevTask != 0)
                        {
                            // and check if it has already been started ( progress or finished)
                            // and the time offset has passed
                            if(((pTriggerGrapplerDevTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS) ||
                                (pTriggerGrapplerDevTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_FINISHED)) &&
                               ((m_Timer.Elapsed() - pTriggerGrapplerDevTask->startTime) >= pFunctionModuleTask->m_OffsetTime))
                            {
                                FILE_LOG_L(laDEV, llINFO) << "  startup task (TIME)" << (int) pFunctionModuleTask->m_TaskIndex << ", FctMod: " << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();
                                pFunctionModuleTaskRet = pFunctionModuleTask;
                                //break;
                            }
                        }
                        else
                        {
                            FILE_LOG_L(laDEV, llERROR) << "  Null pointer returned by GetTaskFromIndex(" << (int) pFunctionModuleTask->m_TriggerTaskIndex << "), task: " << (int) pFunctionModuleTask->m_TaskIndex << ", FctMod: " << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();
                        }
                    }
                }
            }
            else if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_WAIT_PROGRESS)
            {
                // this task is currently wait for progress, set the flag
                bActionStateInProgress = true;
                // the task was set for beeing started manually, start it
                FILE_LOG_L(laDEV, llINFO) << "  start task (MANUAL) index " << (int) pFunctionModuleTask->m_TaskIndex;
                pFunctionModuleTaskRet = pFunctionModuleTask;
                break;
            }
            else if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_UNUSED)
            {
                break;
            }
            else if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_ERROR)
            {
                // this task is currently set to error state, set the flag
                bActionStateWithErrorDetect = true;
                break;
            }
        }
    }

    return  pFunctionModuleTaskRet;
}

/****************************************************************************/
/*!
 *  \brief    Returns the FunctionModuleTask* specified by taskIndex
 *
 *   Detailed description of this function. This description may span
 *   several lines (as shown here).
 *
 *  \iparam   taskMap = map with the function module tasks
 *  \iparam   taskIndex = the task index if the requested task
 *
 * \return   FunctionModuleTask* to the requested task
 */
/****************************************************************************/
FunctionModuleTask* CFunctionModuleTaskManager::GetTaskFromIndex(FunctionModuleTaskMap& taskMap, quint8 taskIndex)
{
    FunctionModuleTask* pFunctionModuleTask;
    FunctionModuleTask* pFunctionModuleTaskRet = 0;
    QMapIterator<quint8, FunctionModuleTask*> iterFctModDevTask(taskMap);

    while (iterFctModDevTask.hasNext())
    {
        pFunctionModuleTask = iterFctModDevTask.next().value();
        if(pFunctionModuleTask->m_TaskIndex == taskIndex)
        {
            pFunctionModuleTaskRet = pFunctionModuleTask;
            break;
        }
    }

    return pFunctionModuleTaskRet;
}


/****************************************************************************/
/*!
 *  \brief  Returns the task with the specified instanceID
 *
 *      This function can be called to return a specified task. Usually it is
 *      called when a function module's request was acknowledged and the
 *      grappler device needs the corresponding task to set it 'Finished'.
 *
 *  \iparam taskMap = The grappler device's task list
 *  \iparam instanceID = The instance identifier of the object which task should be returned
 *  \iparam pLastFunctionModuleTask = Task that was active before
 *
 *  \return The GrapplerDevTask* which instanceID matches the specified one
 */
/****************************************************************************/
FunctionModuleTask* CFunctionModuleTaskManager::GetTaskFromInstanceID(FunctionModuleTaskMap& taskMap,
                                                                      quint32 instanceID,
                                                                      FunctionModuleTask* pLastFunctionModuleTask)
{
    QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(taskMap);
    FunctionModuleTask* pFunctionModuleTask;
    FunctionModuleTask* pFunctionModuleTaskRet = 0;
    bool lastTaskFound = false;

    //FILE_LOG_L(laDEV, llINFO) << "   CTaskManager::GetTaskFromInstanceID " << std::hex << instanceID;

    while (iterFctModTask.hasNext())
    {
        pFunctionModuleTask = iterFctModTask.next().value();
        if(pFunctionModuleTask)
        {
            // to speed up, loop until the pLastGrapplerDevTask is received,
            if((pLastFunctionModuleTask != 0) && (lastTaskFound == false))
            {
                if(pFunctionModuleTask == pLastFunctionModuleTask)
                {
                    lastTaskFound = true;
                }
                continue;
            }

            if((pFunctionModuleTask->m_pCANObject != 0) &&
               (pFunctionModuleTask->m_pCANObject->GetModuleHandle() == instanceID))
            {
                pFunctionModuleTaskRet = pFunctionModuleTask;
                FILE_LOG_L(laDEV, llINFO) << " return '" << typeid(pFunctionModuleTask->m_pCANObject).name() <<
                                             "'' task, index: " << (int) pFunctionModuleTask->m_TaskIndex;
                break;
            }
        }
    }

    return pFunctionModuleTaskRet;
}

/****************************************************************************/
/*!
 *  \brief   Returns the task with the specified instanceID from the map
 *
 *           This function can be called to return a specified task. Usually it is called when a
 *           function module's request was acknowledged and the device needs the corresponding task
 *           to set it 'Finished'
 *
 *  \iparam   deviceMap = device task map containing the function module tasks
 *  \iparam   instanceID = The instance identifier of the object which task should be returned
 *  \iparam   pLastFunctionModuleTask = Task from whichs position is search from, if != 0
 *  \param    DeviceTaskKey = The key if the returned task
 *
 *  \return  The FunctionModuleTask* which instanceID matches the specified one
 *
 ****************************************************************************/
FunctionModuleTask* CFunctionModuleTaskManager::GetTaskFromInstanceID(DeviceTaskMap& deviceMap, quint32 instanceID, FunctionModuleTask* pLastFunctionModuleTask, int& DeviceTaskKey)
{
    QMapIterator<quint16, DeviceTask*> iterDeviceTask(deviceMap);

    DeviceTask*         pDeviceTask;
    FunctionModuleTask* pFunctionModuleTask;
    FunctionModuleTask* pFunctionModuleTaskRet = 0;
    bool lastTaskFound = false;

    //FILE_LOG_L(laDEV, llINFO) << "   CTaskManager::GetTaskFromInstanceID " << std::hex << instanceID;

    while(iterDeviceTask.hasNext())
    {
        pDeviceTask = iterDeviceTask.next().value();
        FILE_LOG_L(laDEV, llDEBUG2) << "  Device: " << (int)pDeviceTask->m_Key << " state: " << (int) pDeviceTask->m_TaskState;
        if((pDeviceTask != 0) && (pDeviceTask->m_TaskState == DeviceTask::DEV_TASK_STATE_PROGRESS))
        {
            QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(pDeviceTask->m_FctModTaskMap);

            //FILE_LOG_L(laDEV, llINFO) << "  Device has " << pDeviceTask->m_FctModTaskMap.size() << " fct tasks";

            while (iterFctModTask.hasNext())
            {
                pFunctionModuleTask = iterFctModTask.next().value();
                //FILE_LOG_L(laDEV, llINFO) << "  FunctionModule: " << (int)pFunctionModuleTask->m_TaskID;
                if(pFunctionModuleTask)
                {
                    if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_UNUSED)
                    {
                        //do not consider unused tasks
                        continue;
                    }

                    // to speed up, loop until the pLastGrapplerDevTask is received,
                    if((pLastFunctionModuleTask != 0) && (lastTaskFound == false))
                    {
                        if(pFunctionModuleTask == pLastFunctionModuleTask)
                        {
                            lastTaskFound = true;
                        }
                        continue;
                    }

                    if(pFunctionModuleTask->m_pCANObject->GetModuleHandle() == instanceID)
                    {
                        pFunctionModuleTaskRet = pFunctionModuleTask;
                        DeviceTaskKey = pDeviceTask->m_Key;
                        FILE_LOG_L(laDEV, llDEBUG2) << " return " << typeid(pFunctionModuleTask->m_pCANObject).name() << " task, index: " << (int) pFunctionModuleTask->m_TaskIndex;
                        break;
                    }
                    else
                    {
                        FILE_LOG_L(laDEV, llDEBUG2) << " no, " << std::hex << pFunctionModuleTask->m_pCANObject->GetModuleHandle() << " != " << std::hex << instanceID;
                    }
                }
            }
            // exit the device task loop if already a function module task was found
            if(pFunctionModuleTaskRet != 0)
            {
                break;
            }
        }
    }

    return pFunctionModuleTaskRet;
}

/****************************************************************************/
/*!
 *  \brief   Returns the task with the specified identifier and state
 *
 *           This function can be called to return a specified task. Usually it is called when a
 *           device task needs a function module task to create it's execution procedure.
 *
 *  \iparam   FctModTaskMap = the function module map containing tasks
 *  \iparam   TaskID = TaskID need (e.g. motor task)
 *  \iparam   TaskState = Task state
 *
 *  \return  The FunctionModuleTask* which matches the specified paramters TaskID and TaskState
 *
 ****************************************************************************/
FunctionModuleTask* CFunctionModuleTaskManager::GetTaskWithState(FunctionModuleTaskMap& FctModTaskMap,
                                                                 FunctionModuleTask::FunctionModuleTaskID_t TaskID,
                                                                 FunctionModuleTask::FunctionModuleTaskState_t TaskState)
{
    FunctionModuleTask* pFunctionModuleTask;
    FunctionModuleTask* pFunctionModuleTaskRet = 0;
    QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(FctModTaskMap);

    FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager: found " << FctModTaskMap.count() << " tasks.";

    while (iterFctModTask.hasNext())
    {
        pFunctionModuleTask = iterFctModTask.next().value();
        if((pFunctionModuleTask->m_TaskState == TaskState) && (pFunctionModuleTask->m_TaskID == TaskID))
        {
            pFunctionModuleTaskRet = pFunctionModuleTask;
            break;
        }
    }

    return pFunctionModuleTaskRet;
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
/*void CCoverLineDevice::CFunctionModuleTaskManager(bool& bActionStateInProgress, bool& bActionStateWithErrorDetect)
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

            pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(pDeviceTask->m_FctModTaskMap,
                                                                                             bActionStateInProgress,
                                                                                             bActionStateWithErrorDetect);
            while(pFunctionModuleTask)
            {
                ReturnCode_t RetVal;

                if(bActionStateWithErrorDetect == true)
                {
                    m_taskState = COVERLINE_DEV_TASK_STATE_ERROR;
                    FILE_LOG_L(laDEV, llERROR) << "  CCoverLineDevice: task with error state:" << pFunctionModuleTask->m_pCANObject->GetKey().toStdString();
                }


                FILE_LOG_L(laDEV, llINFO) << "  CCoverLineDevice: task for startup found.";
                RetVal = m_pDevProc->GetFunctionModuleTaskManager()->StartTask(pFunctionModuleTask);
                if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
                {
                    FILE_LOG_L(laDEV, llERROR) << "  CCoverLineDevice: failed to start task.";
                }
                pFunctionModuleTask = m_pDevProc->GetFunctionModuleTaskManager()->GetStartupTask(pDeviceTask->m_FctModTaskMap,
                                                                                                 bActionStateInProgress,
                                                                                                 bActionStateWithErrorDetect);
            }
        }
    }
}*/

/****************************************************************************/
/*!
 *  \brief   Start a function module task
 *
 *           This function starts the function module task. The identifier, and the object type specific
 *           identifier is used to tdetermine the correct function call
 *
 *  \iparam   pFunctionModuleTask = function module taks to start
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the reference run was successful,
 *            otherwise the return value of the error detecting function
 *
 ****************************************************************************/
ReturnCode_t CFunctionModuleTaskManager::StartTask(FunctionModuleTask* pFunctionModuleTask)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_FAILED;

    if(pFunctionModuleTask == 0)
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    if(pFunctionModuleTask->m_pDeviceTask)
    {
        FILE_LOG_L(laDEV, llINFO) << "  Key: " << pFunctionModuleTask->m_pCANObject->GetKey().toStdString() << ", DeviceTask: " << pFunctionModuleTask->m_pDeviceTask->m_Key;
    }
    else
    {
        FILE_LOG_L(laDEV, llINFO) << "  ID: " << (int) pFunctionModuleTask->m_TaskID;
    }

    if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
    {
        CStepperMotor* pMotor = dynamic_cast<CStepperMotor*>(pFunctionModuleTask->m_pCANObject);
        CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
        if((pMotorTask == 0) || (pMotor == 0))
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        if(pMotorTask->m_MotorTaskID == CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_POS)
        {
            RetVal = pMotor->DriveToPosition(pMotorTask->m_TargetPos, pMotorTask->m_Profile, (StepperMotorSubCommandMotion_t) pMotorTask->m_SubCommandID, pMotorTask->m_SubCommandData);
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::StartTask -> DriveToPosition, instance id:" << std::hex << pMotor->GetModuleHandle();
        }
        else if(pMotorTask->m_MotorTaskID == CANStepperMotorTask::FCTMOD_SM_TASK_ID_MOVE_SPEED)
        {
            RetVal = pMotor->DriveSpeed(pMotorTask->m_TargetSpeed, pMotorTask->m_Profile, (StepperMotorSubCommandMotion_t) pMotorTask->m_SubCommandID, pMotorTask->m_SubCommandData);
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::StartTask -> DriveSpeed, instance id:" << std::hex << pMotor->GetModuleHandle();
        }
        else if(pMotorTask->m_MotorTaskID == CANStepperMotorTask::FCTMOD_SM_TASK_ID_REQ_POS)
        {
            RetVal = pMotor->ReqActPosition();
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::StartTask -> ReqPosition, instance id:" << std::hex << pMotor->GetModuleHandle();
        }
        else if(pMotorTask->m_MotorTaskID == CANStepperMotorTask::FCTMOD_SM_TASK_ID_REFRUN)
        {
            RetVal = pMotor->ExecReferenceMovement();
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::StartTask -> ExecReferenceMovement, instance id:" << std::hex << pMotor->GetModuleHandle();
        }
        else if(pMotorTask->m_MotorTaskID == CANStepperMotorTask::FCTMOD_SM_TASK_ID_STATE)
        {
            //motor state to set is kept in the SubCommandData: 0 = disable, others enable
            RetVal = pMotor->SetMotorState(pMotorTask->m_SubCommandData != 0);
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::StartTask -> SetMotorState, instance id:" << std::hex << pMotor->GetModuleHandle();
        }

        pFunctionModuleTask->startTime = m_Timer.Elapsed();
    }
    else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_TEMP_CTRL)
    {
        CTemperatureControl* pTempCtrl = dynamic_cast<CTemperatureControl*>(pFunctionModuleTask->m_pCANObject);
        CANTemperatureCtrlTask* pTempCtrlTask = dynamic_cast<CANTemperatureCtrlTask*>(pFunctionModuleTask);
        if((pTempCtrlTask == 0) || (pTempCtrl == 0))
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        if(pTempCtrlTask->m_TempCtrlTaskID == CANTemperatureCtrlTask::FCTMOD_TEMP_TASK_ID_REQ_OPMODE)
        {
            RetVal = pTempCtrl->ReqOperatingMode();
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::StartTask -> ReqOperatingMode, instance id:" << std::hex << pTempCtrl->GetModuleHandle();
        }
        if(pTempCtrlTask->m_TempCtrlTaskID == CANTemperatureCtrlTask::FCTMOD_TEMP_TASK_ID_REQ_STATE)
        {
            RetVal = pTempCtrl->ReqStatus();
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::StartTask -> ReqStatus, instance id:" << std::hex << pTempCtrl->GetModuleHandle();
        }
        else if(pTempCtrlTask->m_TempCtrlTaskID == CANTemperatureCtrlTask::FCTMOD_TEMP_TASK_ID_REQ_TEMP)
        {
            RetVal = pTempCtrl->ReqActTemperature(0);
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::StartTask -> ReqActTemperature, instance id:" << std::hex << pTempCtrl->GetModuleHandle();
        }
        else if(pTempCtrlTask->m_TempCtrlTaskID == CANTemperatureCtrlTask::FCTMOD_TEMP_TASK_ID_SET_OPMODE)
        {
            RetVal = pTempCtrl->SetOperatingMode(pTempCtrlTask->m_OperationMode);
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::StartTask -> SetOperatingMode, instance id:" << std::hex << pTempCtrl->GetModuleHandle();
        }
        else if(pTempCtrlTask->m_TempCtrlTaskID == CANTemperatureCtrlTask::FCTMOD_TEMP_TASK_ID_SET_STATE)
        {
            RetVal = pTempCtrl->SetStatus(pTempCtrlTask->m_State);
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::StartTask -> SetStatus, instance id:" << std::hex << pTempCtrl->GetModuleHandle();
        }
        else if(pTempCtrlTask->m_TempCtrlTaskID == CANTemperatureCtrlTask::FCTMOD_TEMP_TASK_ID_SET_TEMP)
        {
            RetVal = pTempCtrl->SetTemperature(pTempCtrlTask->m_Temperatur);
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::StartTask -> SetTemperature, instance id:" << std::hex << pTempCtrl->GetModuleHandle();
        }

        pFunctionModuleTask->startTime = m_Timer.Elapsed();
    }
    else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_RFID)
    {
        CRfid11785* pRFID = dynamic_cast<CRfid11785*>(pFunctionModuleTask->m_pCANObject);
        CANRFIDTask* pRfidCtrlTask = dynamic_cast<CANRFIDTask*>(pFunctionModuleTask);
        if(pRFID == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        RetVal = pRFID->ReqUserData(pRfidCtrlTask->m_Address);
        pFunctionModuleTask->startTime = m_Timer.Elapsed();
        FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::StartTask -> ReqData.";
    }
    else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_RFID_TAG)
    {
        CRfid11785* pRFID = dynamic_cast<CRfid11785*>(pFunctionModuleTask->m_pCANObject);

        if(pRFID == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        RetVal = pRFID->ReqUID();
        pFunctionModuleTask->startTime = m_Timer.Elapsed();
        FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::StartTask -> ReqData.";
    }
    else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_ANA_INPUT)
    {
        CAnalogInput* pAnalogInput = dynamic_cast<CAnalogInput*>(pFunctionModuleTask->m_pCANObject);
        //CANAnalogInputTask* pAnalogInputTask = dynamic_cast<CANAnalogInputTask*>(pFunctionModuleTask);
        if(pAnalogInput == 0)
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        RetVal = pAnalogInput->ReqActInputValue();
        pFunctionModuleTask->startTime = m_Timer.Elapsed();
        FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::Start CANAnalogInputTask -> ReqActInputValue.";
    }
    else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_DIG_INPUT)
    {
        CANDigitalInputTask* pDigInpTask = dynamic_cast<CANDigitalInputTask*>(pFunctionModuleTask);
        CDigitalInput* pDigitalInput = dynamic_cast<CDigitalInput*>(pFunctionModuleTask->m_pCANObject);
        if((pDigInpTask == 0) || (pDigitalInput == 0))
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        if(pDigInpTask->m_DigInpTaskID == CANDigitalInputTask::FCTMOD_DI_TASK_ID_REQ_VALUE)
        {
            RetVal = pDigitalInput->ReqActInputValue();
            pFunctionModuleTask->startTime = m_Timer.Elapsed();
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::Start CANDigitalInputTask -> ReqActInputValue.";
        }
        else if(pDigInpTask->m_DigInpTaskID == CANDigitalInputTask::FCTMOD_DI_TASK_ID_WAIT_VALUE)
        {
            pFunctionModuleTask->startTime = m_Timer.Elapsed();
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::Start CANDigitalInputTask -> ReqActInputValue.";
        }
    }
    else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_DIG_OUTPUT)
    {
        CANDigitalOutputTask* pDigOutpTask = dynamic_cast<CANDigitalOutputTask*>(pFunctionModuleTask);
        CDigitalOutput* pDigitalOutput = dynamic_cast<CDigitalOutput*>(pFunctionModuleTask->m_pCANObject);
        if((pDigOutpTask == 0) || (pDigitalOutput == 0))
        {
            return DCL_ERR_NULL_PTR_ACCESS;
        }

        if(pDigOutpTask->m_DigOutpTaskID == CANDigitalOutputTask::FCTMOD_DO_TASK_ID_SET_VALUE)
        {
            RetVal = pDigitalOutput->SetOutputValue(pDigOutpTask->m_Value);
            pFunctionModuleTask->startTime = m_Timer.Elapsed();
            FILE_LOG_L(laDEV, llINFO) << "   CFunctionModuleTaskManager::Start CANDigitalOutputTask -> SetOutputValue.";
        }
    }

    FILE_LOG_L(laDEV, llINFO) << "   start task :" << (int) pFunctionModuleTask->m_TaskIndex << " target: " <<
                                 (int) pFunctionModuleTask->m_TaskID << ", ret: " << (int) RetVal;

    pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_PROGRESS;

    return RetVal;

}


/****************************************************************************/
/*!
 *  \brief  Rest all task of a function task mal to unused state
 *
 *  \iparam TaskMap = Function module task map
 */
/****************************************************************************/
void CFunctionModuleTaskManager::ResetTasks(FunctionModuleTaskMap& TaskMap)
{
    QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(TaskMap);
    FunctionModuleTask* pFunctionModuleTask;

    while (iterFctModTask.hasNext())
    {
        pFunctionModuleTask = iterFctModTask.next().value();
        if(pFunctionModuleTask)
        {
            pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_UNUSED;

            if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_MOTOR)
            {
                CANStepperMotorTask* pMotorTask = dynamic_cast<CANStepperMotorTask*>(pFunctionModuleTask);
                if(pMotorTask != 0)
                {
                    pMotorTask->m_TargetPos = 0;
                    pMotorTask->m_Profile = 0;
                    pMotorTask->m_SubCommandID = SM_SUBCMD_MOTION_NULL;
//                    pMotorTask->m_SubCommandData = 0;
                    pMotorTask->m_TargetSpeed = 0;
                }
            }
            else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_TEMP_CTRL)
            {
                CANTemperatureCtrlTask* pTempCtrlTask = dynamic_cast<CANTemperatureCtrlTask*>(pFunctionModuleTask);
                if(pTempCtrlTask != 0)
                {
                    pTempCtrlTask->m_OperationMode = TEMPCTRL_OPMODE_FULL;
                    pTempCtrlTask->m_State = TEMPCTRL_STATUS_OFF;
                    pTempCtrlTask->m_Temperatur = 0;
                }
            }
            else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_RFID)
            {
                CANRFIDTask* pRFIDTask = dynamic_cast<CANRFIDTask*>(pFunctionModuleTask);
                if(pRFIDTask != 0)
                {
                    pRFIDTask->m_UID = 0;
                    pRFIDTask->m_RFIDData = 0;
                }
            }
            else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_ANA_INPUT)
            {
                CANAnalogInputTask* pAnalogInputTask = dynamic_cast<CANAnalogInputTask*>(pFunctionModuleTask);
                if(pAnalogInputTask != 0)
                {
                    pAnalogInputTask->m_Value = 0;
                }
            }
            else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_DIG_INPUT)
            {
                CANDigitalInputTask* pDigInpTask = dynamic_cast<CANDigitalInputTask*>(pFunctionModuleTask);
                if(pDigInpTask != 0)
                {
                    pDigInpTask->m_Value = 0;
                }
            }
            else if(pFunctionModuleTask->m_TaskID == FunctionModuleTask::FCTMOD_TASK_ID_DIG_OUTPUT)
            {
                CANDigitalOutputTask* pDigOutpTask = dynamic_cast<CANDigitalOutputTask*>(pFunctionModuleTask);
                if(pDigOutpTask != 0)
                {
                    pDigOutpTask->m_Value = 0;
                }
            }
        }
    }  // end of while
}


/****************************************************************************/
/*!
 *  \brief   Check the function module execution timeout
 *
 *           If the function module execution timeout runs out, it's error state will
 *           be set to DCL_ERR_TIMEOUT.
 *
 *  \iparam   TaskMap = function module tasks map
 *  \iparam   pFctModTaskTimeout = Functions modul with timeout error, if set.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if no timeout was found, otherwise DCL_ERR_TIMEOUT
 *
 ****************************************************************************/
ReturnCode_t CFunctionModuleTaskManager::CheckTasksTimeOut(FunctionModuleTaskMap& TaskMap, FunctionModuleTask* pFctModTaskTimeout)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(TaskMap);
    FunctionModuleTask* pFunctionModuleTask;

    while (iterFctModTask.hasNext())
    {
        pFunctionModuleTask = iterFctModTask.next().value();
        if(pFunctionModuleTask)
        {
            if(pFunctionModuleTask->m_TaskState == FunctionModuleTask::FCTMOD_STATE_PROGRESS)
            {
                int elapsedTime;

                // check for timeout
                elapsedTime = m_Timer.Elapsed();
                if((elapsedTime - pFunctionModuleTask->startTime) > pFunctionModuleTask->m_Timeout)
                {
                    pFunctionModuleTask->m_ErrorHdlInfo = DCL_ERR_TIMEOUT;
                    RetVal = DCL_ERR_TIMEOUT;
                    FILE_LOG_L(laDEV, llERROR) << " CFunctionModuleTaskManager: timeout: " << pFctModTaskTimeout;
                    pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_ERROR;  // set task to state 'error'
                    pFctModTaskTimeout = pFunctionModuleTask;
                }
            }
        }
    }

    return RetVal;
}

} //namespace
