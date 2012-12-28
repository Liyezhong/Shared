/****************************************************************************/
/*! \file bmScheduler.h
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
 *       be requested using dedicated functions of this module.
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

#ifndef BM_SCHEDULER_H
#define BM_SCHEDULER_H


//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//! Control IDs for the module's control functions
typedef enum {
    MODULE_CONTROL_STOP       = 0,  //!< Emergency stop
    MODULE_CONTROL_RESUME     = 1,  //!< Resume stopped instance
    MODULE_CONTROL_SHUTDOWN   = 2,  //!< Go into shutdown mode
    MODULE_CONTROL_WAKEUP     = 3,  //!< Wakeup instance from standby
    MODULE_CONTROL_FLUSH_DATA = 4,  //!< Flush permanent data to storage
    MODULE_CONTROL_RESET_DATA = 5,  //!< Reset permanent data to default
    MODULE_CONTROL_RESET      = 6,  //!< Reset/disable module instance
    MODULE_CONTROL_COUNT            //!< MUST BE last entry in enum
} bmModuleControlID_t;

//! Status IDs for the module's status functions
typedef enum {
    MODULE_STATUS_INSTANCES   = 0,  //!< Get module instance index
    MODULE_STATUS_MODULE_ID   = 1,  //!< Get module ID
    MODULE_STATUS_VERSION     = 2,  //!< Get module version number
    MODULE_STATUS_STATE       = 3,  //!< Get module state (MODULE_STATES_t)
    MODULE_STATUS_VALUE       = 4,  //!< Get module's primary value
    MODULE_STATUS_COUNT             //!< MUST BE last entry in enum
} bmModuleStatusID_t;

//! Module instance state infos (returned by modules status functions)
typedef enum {
    MODULE_STATE_READY        = 0,  //!< Module instance is ready
    MODULE_STATE_BUSY         = 1,  //!< Module instance is busy
    MODULE_STATE_STOPPED      = 2,  //!< Module instance is stopped
    MODULE_STATE_STANDBY      = 3,  //!< Module instance is in standby
    MODULE_STATE_DISABLED     = 4,  //!< Module instance is disabled
    MODULE_STATE_COUNT              //!< MUST BE last entry in enum
} bmModuleState_t;

//! Module instance state infos (returned by modules status functions)
typedef enum {
    TASK_PRIORITY_AUTO        = 0,  //!< Automatic task priority
    TASK_PRIORITY_HIGH        = 1,  //!< High task priority
    TASK_PRIORITY_NORMAL      = 3,  //!< Normal task priority
    TASK_PRIORITY_LOW         = 6,  //!< Low task priority
} bmModulePrority_t;

#define TASK_ID_UNKNOWN     0xFFFF  //!< Return value by bmGetTaskID


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Function pointer to module control functions
typedef Error_t (*bmModuleControl_t) (UInt16, bmModuleControlID_t);

//! Function pointer to module status functions
typedef Error_t (*bmModuleStatus_t) (UInt16, bmModuleStatusID_t);

//! Function pointer to module task functions
typedef Error_t (*bmModuleTask_t) (UInt16);

//! Function pointer to module initialization functions
typedef Error_t (*bmModuleInit_t) (UInt16, UInt16);


//! Module interface definition structure (function pointers)
typedef struct {
    bmModuleTask_t    Task;        //!< Modules task function
    bmModuleControl_t Control;     //!< Modules contol function
    bmModuleStatus_t  Status;      //!< Modules status function
} bmModuleInterface_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t bmRegisterModule    (
            UInt16 ModuleID, UInt16 Instances, bmModuleInterface_t *Interface);

UInt16  bmGetTaskID          (UInt16 ModuleID, UInt16 Instance);
Error_t bmGetModuleInstances (UInt16 ModuleID);
Error_t bmGetModulIndex      (UInt16 ModuleID);
Error_t bmGetModuleID2       (UInt16 ModuleIndex);
Error_t bmGetModuleID        (UInt16 TaskID);
Error_t bmGetInstance        (UInt16 TaskID);
Error_t bmGetChannel         (UInt16 TaskID);
Error_t bmGetTaskPriority    (UInt16 TaskID);
Error_t bmSetTaskPriority    (UInt16 TaskID, UInt16 Priority);
Error_t bmGetModuleStatus    (UInt16 TaskID, bmModuleStatusID_t StatusID, UInt32 Default);
Error_t bmSetModuleControl   (UInt16 TaskID, bmModuleControlID_t ControlID);
Error_t bmSetGlobalControl   (bmModuleControlID_t ControlID);
Bool    bmModulesInState     (bmModuleState_t StateID, Bool Value);
Error_t bmScheduleTasks      (void);
Error_t bmGetNumberOfTasks   (void);
Error_t bmGetNumberOfModules (void);
Error_t bmInitScheduler      (UInt16 NumberOfModules, UInt16 NumberOfTasks);

//****************************************************************************/

#endif /*BM_SCHEDULER_H*/
