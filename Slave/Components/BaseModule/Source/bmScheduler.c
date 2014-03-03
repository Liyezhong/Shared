/****************************************************************************/
/*! \file bmScheduler.c
 *
 *  \brief Task Handling and Scheduling
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions to handle and schedule the
 *       function module's tasks. A function module must register itself
 *       to the scheduler during initialization to be taken into account
 *       by the task scheduler. All informations regarding to a task can
 *       be requested using the dedicated functions of this module.
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#include <stdlib.h>
#include "Global.h"
#include "halLayer.h"
#include "bmCommon.h"
#include "bmCan.h"
#include "bmDebug.h"
#include "bmUtilities.h"
#include "bmStatistics.h"
#include "bmDispatch.h"
#include "bmService.h"
#include "bmMonitor.h"
#include "bmSystem.h"
#include "bmTime.h"
#include "bmBlink.h"
#include "bmError.h"
#include "ModuleIDs.h"
#include "bmScheduler.h"
#include "bmProcData.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

//! Task state enumeration
typedef enum {
    TASK_STATE_RUNABLE,           //!< Task is runnable
    TASK_STATE_SHUTDOWN,          //!< Task is shutting down
    TASK_STATE_SUSPEND            //!< Task is suspended
} bmTaskStates_t;

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Task descriptor, holding the state of a task (used for task table)
typedef struct {
    UInt16 ModuleID;              //!< Unique module ID
    UInt16 ModuleNr;              //!< Module number (index into module table)
    UInt16 Instance;              //!< Module instance
    UInt16 Priority:8;            //!< Task priority
    UInt16 AutoPrio:1;            //!< Auto priority
    UInt16 State:4;               //!< Task state
    UInt16 ErrFlag:1;             //!< Task error flag
} bmTaskDescriptor_t;

//! Module descriptor, holding the state of a module (used for module table)
typedef struct {
    UInt16 ModuleID;              //!< Unique module ID
    UInt16 Instances;             //!< Number of instances of this module
    bmModuleInterface_t Interface; //!< Module's interface functions
} bmModuleDescriptor_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

//! TaskTable contains the task descriptors of all tasks
static bmTaskDescriptor_t *TaskTable;     //!< Task descriptor table
static UInt16 TaskTableSize = 0;          //!< Size of task descriptor table
static UInt16 NumberOfTasks = 0;          //!< Number of tasks

//! ModuleTable contains the module descriptors of all modules 
static bmModuleDescriptor_t *ModuleTable; //!< Module descriptor table
static UInt16 ModuleTableSize;            //!< Size of module descriptor table
static UInt16 NumberOfModules = 0;        //!< Number of modules

static UInt16 ScheduleCounter = 0;        //!< Task schedule counter
static UInt16 curTaskID = 0;              //!< Task ID of running task

static UInt32 BoardOptions = 0;           //!< Base module's board option bits

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Bool bmIsTaskRunnable (bmTaskDescriptor_t *Task, bmNodeState_t State);
static bmModuleState_t bmCallModuleTask (UInt16 TaskID);



/*****************************************************************************/
/*!
 *  \brief   Register function module to the basemodule
 *
 *      Each function module has to register itself to the base module using
 *      this function. Only registered modules will be considered by the
 *      scheduler. Not registered modules will be handled like not existent.
 *      The function module has to supply it's module ID, the number of
 *      module instances, and the interface functions. The interface
 *      functions are the task, control, and status functions, which are
 *      used by the base module to interact with the function module.
 *      The registered modules are inserted into the module table, each
 *      single instance of the module in the task table.
 *
 *  \iparam  ModuleID  = ID of the module to register
 *  \iparam  Instances = Number of instances of that module
 *  \iparam  Interface = Interface function pointers
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmRegisterModule (
    UInt16 ModuleID, UInt16 Instances, bmModuleInterface_t *Interface) {

    bmModuleDescriptor_t *Module;
    bmTaskDescriptor_t *Task;
    int Instance;

    if (Interface == NULL) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (NumberOfTasks + Instances > TaskTableSize) {
        return (E_TASK_TABLE_FULL);
    }
    if (NumberOfModules + 1 > ModuleTableSize) {
        return (E_MODULE_TABLE_FULL);
    }
    if (bmGetModuleInstances(ModuleID)) {
        return (E_MODULE_ALREADY_EXISTS);
    }
    Module = &ModuleTable[NumberOfModules];
    Module->ModuleID  = ModuleID;
    Module->Instances = Instances;
    Module->Interface = *Interface;

    for (Instance=0; Instance < Instances; Instance++) {

        Task = &TaskTable[NumberOfTasks++];

        Task->ModuleID = ModuleID;
        Task->ModuleNr = NumberOfModules;
        Task->Instance = Instance;
        Task->State    = TASK_STATE_RUNABLE;
        Task->Priority = TASK_PRIORITY_NORMAL;
        Task->AutoPrio = TRUE;
    }
    NumberOfModules++;
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief    Returns the task ID of a module instance
 *
 *      Searches the task table for the task with the given module ID and
 *      module instance number and returns it's task ID. If no such task
 *      can be found, an error is returned.
 *
 *  \iparam   ModuleID = Module ID of the ask module
 *  \iparam   Instance = Instance number of that module
 *
 *  \return   Task-ID or constant TASK_ID_UNKNOWN
 *
 ****************************************************************************/

UInt16 bmGetTaskID (UInt16 ModuleID, UInt16 Instance) {

    UInt16 TaskID;

    for (TaskID=0; TaskID < NumberOfTasks; TaskID++) {

        if (TaskTable[TaskID].ModuleID == ModuleID &&
            TaskTable[TaskID].Instance == Instance) {

            return (TaskID);
        }
    }
    return (TASK_ID_UNKNOWN);
}


/*****************************************************************************/
/*!
 *  \brief    Returns the channel number of a task
 *
 *      Returns the channel ID associated to a certain task. The task is
 *      identified by the task ID. If a task with that ID dosn't exists,
 *      an error is returned.
 *
 *  \iparam   TaskID = Task identifier
 *
 *  \return   Positive channel number or negative error code
 *
 ****************************************************************************/

Error_t bmGetChannel (UInt16 TaskID) {

    if (TaskID < NumberOfTasks) {
        return (TaskID);
    }
    return (E_TASK_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief    Returns the module ID of a task
 *
 *      Returns the module id of the task identified by TaskID. If a task
 *      with that ID doesn't exist, an error is returned.
 *
 *  \iparam   TaskID = Task identifier
 *
 *  \return   Positive module ID or negative error code
 *
 ****************************************************************************/

Error_t bmGetModuleID (UInt16 TaskID) {

    if (TaskID < NumberOfTasks) {
        return (TaskTable[TaskID].ModuleID);
    }
    return (E_TASK_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief    Returns the module ID of a task
 *
 *      Returns the module ID of the module with the given ModuleIndex. The
 *      module index is an index into the module descriptor table.
 *
 *      This function is for internal use of the base module only!
 *
 *  \iparam   ModuleIndex = Module table index
 *
 *  \return   Positive module ID or negative error code
 *
 ****************************************************************************/

Error_t bmGetModuleID2 (UInt16 ModuleIndex) {

    if (ModuleIndex < NumberOfModules) {
        return (ModuleTable[ModuleIndex].ModuleID);
    }
    return (E_PARAMETER_OUT_OF_RANGE);
}


/*****************************************************************************/
/*!
 *  \brief    Returns the module index number of a task
 *
 *      Returns the index into the module descriptor table of the task
 *      identified by TaskID. If a task with that ID doesn't exist,
 *      an error is returned. Attention: The module index number is
 *      NOT the module ID!
 *
 *      This function is for internal use of the base module only!
 *
 *  \iparam   TaskID = Task identifier
 *
 *  \return   Positive module index or negative error code
 *
 ****************************************************************************/

Error_t bmGetModulIndex (UInt16 TaskID) {

    if (TaskID < NumberOfTasks) {
        return (TaskTable[TaskID].ModuleNr);
    }
    return (E_TASK_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief    Returns the module instance number of a task
 *
 *      Returns the module instance number of the task identified by TaskID.
 *      If a task with that ID doesn't exist, an error is returned.
 *
 *  \iparam   TaskID = Task identifier
 *
 *  \return   Positive instance number or negative error code
 *
 ****************************************************************************/

Error_t bmGetInstance (UInt16 TaskID) {

    if (TaskID < NumberOfTasks) {
        return (TaskTable[TaskID].Instance);
    }
    return (E_TASK_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief    Returns the schedule priority of a task
 *
 *      Returns the scheduling priority of the task identified by TaskID.
 *      If a task with that ID doesn't exist, an error is returned.
 *
 *  \iparam   TaskID = Task identifier
 *
 *  \return   Positive task priority or negative error code
 *
 ****************************************************************************/

Error_t bmGetTaskPriority (UInt16 TaskID) {

    if (TaskID < NumberOfTasks) {
        return (TaskTable[TaskID].Priority);
    }
    return (E_TASK_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief    Changes the schedule priority of a task
 *
 *      Sets task priority of the task identified by TaskID to the supplied
 *      priority. If a task with that ID doesn't exist, an error is returned.
 *      Lower values represent higher, higher values lower priorities.
 *      A priority of 0 enables auto-priority, which means the priority is
 *      dynamically modulated depending on task state.
 *
 *      Returns the priority of the task before calling this function.
 *
 *  \iparam   TaskID   = Task identifier
 *  \iparam   Priority = New priority
 *
 *  \return   Priority before calling this function
 *
 ****************************************************************************/

Error_t bmSetTaskPriority (UInt16 TaskID, UInt16 Priority) {

    UInt16 oldPriority;

    if (TaskID < NumberOfTasks) {
        oldPriority = TaskTable[TaskID].Priority;
        if (Priority) {
            TaskTable[TaskID].Priority = Priority;
            TaskTable[TaskID].AutoPrio = FALSE;
        }
        else {
            TaskTable[TaskID].Priority = TASK_PRIORITY_NORMAL;
            TaskTable[TaskID].AutoPrio = TRUE;
        }
        return (oldPriority);
    }
    return (E_TASK_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief    Returns the number of registered tasks
 *
 *      Returns the number of registered tasks. Each module instance is
 *      counted as a task.
 *
 *  \return   Number of registered tasks
 *
 ****************************************************************************/

Error_t bmGetNumberOfTasks (void) {

    return (NumberOfTasks);
}


/*****************************************************************************/
/*!
 *  \brief    Returns the number of registered modules
 *
 *      Returns the number of registered modules.
 *
 *  \return   Number of registered modules
 *
 ****************************************************************************/

Error_t bmGetNumberOfModules (void) {

    return (NumberOfModules);
}


/*****************************************************************************/
/*!
 *  \brief    Return number of instances of a module
 *
 *      Returns the number of instances of the module given by the module ID
 *      parameter. If a module with this ID doen't exist, 0 is returned.
 *      Thus, a return value > 0 indicates that a module exists.
 *
 *  \iparam   ModuleID = Module ID of the ask module
 *
 *  \return   Number of instances of the module\n
 *            -  0 = module doesn't exist\n
 *            - >0 = module exists
 *
 ****************************************************************************/

Error_t bmGetModuleInstances (UInt16 ModuleID) {

    UInt16 i;

    for (i=0; i < NumberOfModules; i++) {

        if (ModuleTable[i].ModuleID == ModuleID) {
            return (ModuleTable[i].Instances);
        }
    }
    return (0);
}


/*****************************************************************************/
/*!
 *  \brief    Check if function modules are in ask state
 *
 *      Calls the status functions of all module instances to get the module
 *      state and and compares it to the supplied Value. If all modules
 *      deliver a state equal to Value, TRUE is returned. If at least one
 *      module returns a different value, FALSE is returned
 *
 *      This function is intended for internal use of the base module only!
 *
 *  \iparam   StateID = Module state to compare to
 *  \iparam   Value   = Value of the module state (TRUE, FALSE)
 *
 *  \return   TRUE if all modules return the asked status, FALSE if at
 *            least one task returns another status
 *
 ****************************************************************************/

Bool bmModulesInState (bmModuleState_t StateID, Bool Value) {

    Bool State;
    int i;

    for (i = 0; i < NumberOfTasks; i++) {
        if (bmGetModuleStatus (i, MODULE_STATUS_STATE, Value) == StateID) {
            State = TRUE;
        }
        else {
            State = FALSE;
        }
        if (State != Value) {
            return (FALSE);
        }
    }
    return (TRUE);
}


/*****************************************************************************/
/*!
 *  \brief    Call all module's control function
 *
 *      The control function of all registered function modules can be called
 *      using this function. The ControlID identifies the function to perform
 *      and is passed to the module's control function.
 *
 *      This function is for internal use of the base module only!
 *
 *  \iparam   ControlID = ID of the module to register
 *
 *  \return   NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmSetGlobalControl (bmModuleControlID_t ControlID) {

    Error_t Status = NO_ERROR;
    Error_t Result;
    UInt16 TaskID;

    for (TaskID = 0; TaskID < NumberOfTasks; TaskID++) {
        if ((Result = bmSetModuleControl(TaskID, ControlID)) < 0) {
            bmSignalEvent (TaskID, Result, 1, 0);
            Status = Result;
        }
    }
    return (Status);
}


/*****************************************************************************/
/*!
 *  \brief    Call module's control function
 *
 *      This function can be used to call the control function of a certain
 *      function module instance. The function module is specified by TaskID.
 *      Depending on the ControlID parameter, the following actions can be
 *      initiated:
 *
 *      - MODULE_CONTROL_STOP
 *      - MODULE_CONTROL_RESUME
 *      - MODULE_CONTROL_SHUTDOWN
 *      - MODULE_CONTROL_WAKEUP
 *      - MODULE_CONTROL_FLUSH_DATA
 *      - MODULE_CONTROL_RESET_DATA
 *      - MODULE_CONTROL_RESET
 *
 *      This function is intended for internal use of the base module only!
 *
 *  \iparam   TaskID    = Task identifier
 *  \iparam   ControlID = Selects which control to pass
 *
 *  \return   NO_ERROR or negative error code
 *
 ****************************************************************************/

Error_t bmSetModuleControl (UInt16 TaskID, bmModuleControlID_t ControlID) {

    UInt16 Instance = TaskTable[TaskID].Instance;
    UInt16 ModuleNr = TaskTable[TaskID].ModuleNr;
    bmModuleControl_t SetTaskControl;

    if (ControlID >= MODULE_CONTROL_COUNT) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (TaskID == BASEMODULE_TASK_ID && curTaskID != BASEMODULE_TASK_ID) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (TaskID < NumberOfTasks) {

        SetTaskControl = ModuleTable[ModuleNr].Interface.Control;
        if (SetTaskControl != NULL) {

            if (ControlID == MODULE_CONTROL_SHUTDOWN) {
                if (TaskTable[TaskID].State >= TASK_STATE_SHUTDOWN) {
                    return (NO_ERROR);
                }
                TaskTable[TaskID].State = TASK_STATE_SHUTDOWN;
            }
            else if (ControlID == MODULE_CONTROL_WAKEUP) {
                if (TaskTable[TaskID].State == TASK_STATE_RUNABLE) {
                    return (NO_ERROR);
                }
                TaskTable[TaskID].State = TASK_STATE_RUNABLE;
            }
            return (SetTaskControl (Instance, ControlID));
        }
        return (NO_ERROR);
    }
    return (E_TASK_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief    Call module's status function
 *
 *      This function can be used to call the status function of a certain
 *      function module instance. The function module is specified by TaskID.
 *      Depending on the StatusID parameter, the following status can be
 *      requested:
 *
 *      - MODULE_STATUS_INSTANCES
 *      - MODULE_STATUS_MODULE_ID
 *      - MODULE_STATUS_VERSION
 *      - MODULE_STATUS_STATE
 *      - MODULE_STATUS_ABORTED
 *      - MODULE_STATUS_VALUE
 *
 *      On other status IDs an error is returned.
 *
 *  \iparam   TaskID   = Task identifier
 *  \iparam   StatusID = Selects which status is requested
 *  \iparam   Default  = Value returned if task doesn't exists
 *
 *  \return   Positive status value or negative error code
 *
 ****************************************************************************/

Error_t bmGetModuleStatus (
    UInt16 TaskID, bmModuleStatusID_t StatusID, UInt32 Default) {

    UInt16 Instance = TaskTable[TaskID].Instance;
    UInt16 ModuleNr = TaskTable[TaskID].ModuleNr;

    if (StatusID >= MODULE_STATUS_COUNT) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (TaskID < NumberOfTasks) {
        bmModuleStatus_t GetTaskStatus =
            ModuleTable[ModuleNr].Interface.Status;

        if (GetTaskStatus != NULL) {
            return (GetTaskStatus (Instance, StatusID));
        }
    }
    return (Default);
}


/*****************************************************************************/
/*!
 *  \brief    Call module's task function
 *
 *      This function calls the task function of the function module
 *      associated with TaskID. Depending on the board options the
 *      task statistics measurement is performed or not. Processing
 *      of process data for that task is called as well, since this
 *      is a functionality belonging to the function module.
 *
 *      Returns the module instance state or an error.
 *
 *  \iparam   TaskID = Task identifier
 *
 *  \return   Module state or (negative) error code
 *
 ****************************************************************************/

static bmModuleState_t bmCallModuleTask (UInt16 TaskID) {

    UInt16 Instance = TaskTable[TaskID].Instance;
    UInt16 ModuleNr = TaskTable[TaskID].ModuleNr;
    bmModuleState_t ModuleState;

    if (TaskID < NumberOfTasks) {

        bmModuleTask_t ModuleTask = ModuleTable[ModuleNr].Interface.Task;

        if (ModuleTask != NULL) {
            Error_t Status;

            if (BoardOptions & OPTION_TASK_STATISTICS) {
                bmControlStatistics (TaskID, STAT_TASK_START);
            }
            if ((Status = ModuleTask(Instance)) < NO_ERROR) {
                ModuleState = MODULE_STATE_DISABLED;
            }
            else {
                ModuleState = (bmModuleState_t) Status;
            }

            if (BoardOptions & OPTION_TASK_STATISTICS) {
                bmControlStatistics (TaskID, STAT_TASK_STOP);
            }
            if (BoardOptions & OPTION_PROCESS_DATA) {
                bmMonitorProcessData (TaskID, ModuleState);
            }
            return (ModuleState);
        }
    }
    return (MODULE_STATE_DISABLED);
}


/*****************************************************************************/
/*!
 *  \brief    Check if a certain task is allowed to run
 *
 *      This function is called by the scheduler to check, if a certain task
 *      is allowed to run. Function module tasks should not run if...
 *
 *      - Task is sleeping
 *      - Task is suspended
 *      - Node is in startup, configuration or identification phase
 *
 *      The base modules own task function is always runnable.
 *
 *  \xparam   Task      = Pointer to task descriptor
 *  \iparam   NodeState = Node state
 *
 *  \return   TRUE if the task runable, FALSE if not
 *
 ****************************************************************************/

static Bool bmIsTaskRunnable (bmTaskDescriptor_t *Task, bmNodeState_t NodeState) {

    if (Task->ModuleID > 0) {
        if (Task->State == TASK_STATE_SUSPEND ||
            NodeState   <= NODE_STATE_IDENTIFY  ||
            NodeState   == NODE_STATE_STANDBY) {
            return (FALSE);
        }
    }
    return (TRUE);
}


/*****************************************************************************/
/*!
 *  \brief    Task Scheduler
 *
 *      This is the task scheduler. It calls the task functions of all
 *      registered function modules, as well as the base modules system
 *      tasks. Task with lower priority values are called more often than
 *      task with higher priority values. A task function is only called,
 *      if it is allowed to be called. The function bmIsTaskAllowedToRun
 *      is called to check this. As an extra, statistics are collected
 *      to help to detect misbehaving tasks. If the called task function
 *      returns an error code, that error is send to the master.
 *
 *  \return   NO_ERROR or negative error code
 *
 ****************************************************************************/

Error_t bmScheduleTasks (void) {

    bmTaskDescriptor_t *Task = TaskTable;
    UInt16 TasksRunable = 0;

    for (curTaskID = 0; curTaskID < NumberOfTasks; curTaskID++) {

        if (Task->Priority != 0 && (ScheduleCounter % Task->Priority) == 0) {

            bmNodeState_t NodeState = bmGetNodeState();

            if (bmIsTaskRunnable (Task, NodeState)) {
                bmModuleState_t ModuleState = bmCallModuleTask(curTaskID);

                if (Task->AutoPrio) {
                    if (ModuleState == MODULE_STATE_BUSY) {
                        Task->Priority = TASK_PRIORITY_HIGH;
                    }
                    else if (ModuleState == MODULE_STATE_DISABLED) {
                        Task->Priority = TASK_PRIORITY_LOW;
                    }
                    else {
                        Task->Priority = TASK_PRIORITY_NORMAL;
                    }
                }
                if (Task->State == TASK_STATE_SHUTDOWN &&
                   (ModuleState == MODULE_STATE_DISABLED ||
                    ModuleState == MODULE_STATE_STANDBY)) {

                    Task->State = TASK_STATE_SUSPEND;
                }
            }
        }
        if (Task->State != TASK_STATE_SUSPEND) {
            TasksRunable++;
        }
        Task++; // point to next task's descriptor
    }
    curTaskID = 0;

    if (bmGetNodeState() == NODE_STATE_SHUTDOWN && !TasksRunable) {
        bmSetNodeState (NODE_STATE_STANDBY);
    }
    canDispatchMessages();
    ScheduleCounter++;

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Initializes this module
 *
 *      This function initializes this module. It allocates memory for the
 *      module- and task descriptor tables and registers the basemodule's
 *      own task function. The size of the module descriptor table is given
 *      by the ModuleCount parameter, the size of the task descriptor table 
 *      is given by the TaskCount parameter, not counting the basemodules 
 *      own task.
 *
 *      This function is called once during startup.
 *
 *  \iparam  ModuleCount = Number of function modules
 *  \iparam  TaskCount   = Number of function tasks
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmInitScheduler (UInt16 ModuleCount, UInt16 TaskCount) {

    BoardOptions = bmGetBoardOptions (
        BASEMODULE_MODULE_ID, OPTIONS_BASEMODULE, DEFAULT_BOARD_OPTIONS);

    // allocate module descriptor table
    ModuleTable = calloc (ModuleCount, sizeof(bmModuleDescriptor_t));
    if (ModuleTable == NULL) {
        return (E_MEMORY_FULL);
    }
    ModuleTableSize = ModuleCount;

    // allocate task descriptor table
    TaskTable = calloc (TaskCount+1, sizeof(bmTaskDescriptor_t));
    if (TaskTable == NULL) {
        return (E_MEMORY_FULL);
    }
    TaskTableSize = TaskCount+1;
 
    return (NO_ERROR);
}

//****************************************************************************/
