/****************************************************************************/
/*! \file bmMain.c
 *
 *  \brief Base modules main functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module initializes the base module and all function modules by
 *       calling their initialization functions. It also contains the task,
 *       status, and control functions of the basemodule itself, which are
 *       called by the task scheduler
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
#include "bmUtilities.h"
#include "bmScheduler.h"
#include "bmStatistics.h"
#include "bmDispatch.h"
#include "bmDebug.h"
#include "bmSystem.h"
#include "bmService.h"
#include "bmMonitor.h"
#include "bmPermData.h"
#include "bmProcData.h"
#include "bmTime.h"
#include "bmBlink.h"
#include "bmError.h"
#include "bmStorage.h"
#include "bmMain.h"
#include "bmUpdate.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define LIFETIME_UPDATE_INTERVAL  60000   //!< Life time update interval
#define VOLTAGE_STABLE_TIMEOUT    3000    //!< Wait for power stable timeout


//****************************************************************************/
// Private Variables
//****************************************************************************/

static UInt32 ModuleState = MODULE_STATE_READY; //!< Actual module state

static UInt32 BoardOptions;         //!< Board options for base module

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t bmBaseModuleTask     (UInt16 Instance);
static Error_t bmBaseModuleControl  (UInt16 Instance, bmModuleControlID_t ControlID);
static Error_t bmBaseModuleStatus   (UInt16 Instance, bmModuleStatusID_t StatusID);
static Error_t bmWaitForPowerStable (void);

static Error_t bmInitCoreModules (UInt16 ModuleCount, UInt16 TaskCount);
static Error_t bmInitOptionalModules (UInt16 TaskCount);

static Error_t bmInitFunctionModules (
                 const bmModuleParameters_t *ModuleInitTable, UInt16 ModuleCount);



/*****************************************************************************/
/*!
 *  \brief   Start base module
 *
 *      Initializes the hardware abstraction layer, the basemodule's core
 *      components and its optional components. After the basemodule has
 *      initialized itself, it initializes all function modules contained
 *      in the ModuleInitTable as well. This is done by calling the init
 *      functions of the function modules.
 *
 *      After initialization is done the node is normally switched into
 *      identification mode, to signal the master that the node is there.
 *      Master/slave identification can be configured to be skipped using
 *      the board options. If disabled the node switches to normal mode
 *      w/o identify itself to the master. This is usefull especially
 *      during debugging or testing.
 *
 *      At the end the task scheduler is called, which is responsible to
 *      call all registered task function endlessly.
 *
 *      ATTENTION: This function should never return, unless there is an
 *      error during basemodule's core initialization, which prohibits
 *      proper operation of the node.
 *
 *  \iparam  ModuleInitTable = Function module descriptor table
 *  \iparam  ModuleCount     = Size of module descriptor table
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmStartBaseModule (
              const bmModuleParameters_t *ModuleInitTable, UInt16 ModuleCount) {

    UInt16 MaxTaskCount = 1;
    Error_t Status;
    UInt16 i;

    // initialize hardware abstraction layer (abort if failed)
    if ((Status = halInitializeLayer()) < 0) {
        return (Status);
    }
    // determine maximum number of tasks
    for (i=0; i < ModuleCount; i++) {
        MaxTaskCount += ModuleInitTable[i].NumberOfInstances;
    }
    bmInitializeStorage (MaxTaskCount);

    // initialize core base module (abort if failed)
    if ((Status = bmInitCoreModules (ModuleCount+1, MaxTaskCount)) < 0) {
        return (Status);
    }
    // initialize debug module (inactive in non-debug version)
    dbgInitializeModule(ModuleCount+2);
    bmSetNodeState (NODE_STATE_STARTUP);

    // get board options for base module
    BoardOptions = bmGetBoardOptions(
        BASEMODULE_MODULE_ID, OPTIONS_BASEMODULE, DEFAULT_BOARD_OPTIONS);

    // wait 'till power supply is acceptable (if enabled)
    if (BoardOptions & OPTION_WAIT_POWER_UP) {
        bmWaitForPowerStable();
    }
    if (halGetResetReason() == RESET_CAUSED_BY_WATCHDOG) {
        bmSignalEvent (BASEMODULE_CHANNEL, E_RESET_BY_WATCHDOG, 1, 0);
    }
    // initialize all function modules
    bmInitFunctionModules(ModuleInitTable, ModuleCount);

    // initialize optional components
    bmInitOptionalModules (MaxTaskCount);

    #ifdef PRINT_DEFINE_LIST
    dbgPrintCanIdNameList();
    #endif

    // set node state (depending on board options)
    if (BoardOptions & OPTION_IDENTIFICATION) {
        bmSetNodeState (NODE_STATE_IDENTIFY);
    }
    else {
        bmSetNodeState (NODE_STATE_NORMAL);
    }
    for (;;) {
        bmScheduleTasks();
    }
    //should never come here
}


/*****************************************************************************/
/*!
 *  \brief   Initializes the basemodule core components
 *
 *      Initializes the core components of the base module, which includes:
 *
 *      - CAN communication layer
 *      - CAN message dispatcher
 *      - System message handler
 *      - Service message handler
 *      - System timer component
 *      - Task scheduler
 *      - Utility functions
 *
 *      Beside this, the basemodule's own task/control/status functions are
 *      registered to the scheduler. The parameter ModuleCount gives the
 *      number of function modules (not counting base module), TaskCount
 *      gives the max. number of tasks (module instances).
 *
 *      The base module can't run without these components!
 *
 *  \iparam  ModuleCount = Number of modules
 *  \iparam  TaskCount   = Number of tasks (module instances)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmInitCoreModules (UInt16 ModuleCount, UInt16 TaskCount) {

    static bmModuleInterface_t Interface = {
        bmBaseModuleTask, bmBaseModuleControl, bmBaseModuleStatus
    };
    Error_t Status;

    ModuleCount++;  // one more for the base module itself

    if ((Status = bmInitUtilities()) != NO_ERROR) {
        return (Status);
    }
    if ((Status = canInitializeLayer()) != NO_ERROR) {
        return (Status);
    }
    if ((Status = canInitDispatcher(ModuleCount)) != NO_ERROR) {
        return (Status);
    }
    Status = bmInitScheduler (ModuleCount, TaskCount);
    if (Status != NO_ERROR) {
        return (Status);
    }
    Status = bmRegisterModule (BASEMODULE_MODULE_ID, 1, &Interface);
    if (Status != NO_ERROR) {
        return (Status);
    }
    if ((Status = bmInitTimeModule()) != NO_ERROR) {
        return (Status);
    }
    if ((Status = bmInitSystemCommands()) != NO_ERROR) {
        return (Status);
    }
    if ((Status = bmInitUpdate()) != NO_ERROR) {
        return (Status);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Initializes the basemodule's optional components
 *
 *      Initializes the optional components of the base module, as far as
 *      they are enabled in the base module's board options:
 *
 *      - Status/Error LED
 *      - Process data handling
 *      - Task statistics collection
 *      - Power supply monitoring
 *      - Service Commands
 *
 *      The base module is operational even without these components!
 *
 *  \iparam  TaskCount = Number of tasks (module instances)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmInitOptionalModules (UInt16 TaskCount) {

    Error_t Status;
    Bool MonitorEnabled = FALSE;

    // signal initialization start to the master (if enabled)
    if (BoardOptions & OPTION_INFO_MESSAGES) {
        bmSignalEvent (BASEMODULE_CHANNEL,
            I_INITIALIZATION_START, 1, BASEMODULE_MODULE_ID);
    }

    // initialize service module
    if ((Status = bmInitServiceModule()) != NO_ERROR) {
        bmSignalEvent (BASEMODULE_CHANNEL, Status, 1, 0);
    }
    // initialize onboard status LED (if enabled)
    if (BoardOptions & OPTION_STATUS_LED) {
        if ((Status = bmInitStatusLeds()) != NO_ERROR) {
            bmSignalEvent (BASEMODULE_CHANNEL, Status, 1, 0);
        }
    }
    // initialize process data (if enabled)
    if (BoardOptions & OPTION_PROCESS_DATA) {
        if ((Status = bmInitProcessData(TaskCount)) != NO_ERROR) {
            bmSignalEvent (BASEMODULE_CHANNEL, Status, 1, 0);
        }
    }
    // initialize task statistics (if enabled)
    if (BoardOptions & OPTION_TASK_STATISTICS) {
        if ((Status = bmInitializeStatistics(TaskCount)) != NO_ERROR) {
            bmSignalEvent (BASEMODULE_CHANNEL, Status, 1, 0);
        }
    }
    // initialize power monitoring (if enabled)
    if (BoardOptions & OPTION_VOLTAGE_MONITOR) {
        bmInitVoltageMonitor();
        MonitorEnabled = TRUE;
    }
    if (BoardOptions & OPTION_SAFE_MONITOR) {
        bmInitSafeMonitor();
        MonitorEnabled = TRUE;
    }
    if (BoardOptions & OPTION_CURRENT_MONITOR) {
        bmInitCurrentMonitor();
        MonitorEnabled = TRUE;
    }
    if (MonitorEnabled) {
        if ((Status = bmInitPowerMonitor()) != NO_ERROR) {
            bmSignalEvent (BASEMODULE_CHANNEL, Status, 1, 0);
        }
    }

    // signal initialization start to the master (if enabled)
    if (BoardOptions & OPTION_INFO_MESSAGES) {
        bmSignalEvent (BASEMODULE_CHANNEL,
            I_INITIALIZATION_DONE, 1, BASEMODULE_MODULE_ID);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize all function modules
 *
 *      Initializes all function modules which are present in the supplied
 *      ModuleInitTable by calling their initialization function. A pointer
 *      to the init function for each module is part of the ModuleInitTable.
 *      The number of instances is also included in the ModuleInitTable for
 *      each function module. If the number of instances in the table is 0,
 *      the board options are checked to determine the number of instances.
 *      If the number of instances there is also 0, the function module
 *      will not be initialized, thus stay inactive.
 *
 *      For each function module initialized an info message is send to the
 *      master to record the startup process in an event log file.
 *
 *  \iparam  ModuleInitTable = Function module descriptor table
 *  \iparam  ModuleCount     = Size of module descriptor table
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmInitFunctionModules (
           const bmModuleParameters_t *ModuleInitTable, UInt16 ModuleCount) {

    Error_t Status;
    UInt16 i;

    for (i=0; i < ModuleCount; i++) {

        UInt16 Instances = ModuleInitTable->NumberOfInstances;
        UInt16 ModuleID  = ModuleInitTable->ModuleID;

        if (!Instances) {
            Instances = bmGetBoardOptions(ModuleID, MODUL_INSTANCES, 0);
        }
        if (Instances) {
            if (BoardOptions & OPTION_INFO_MESSAGES) {
                bmSignalEvent (0, I_INITIALIZATION_START, 1, ModuleID);
            }
            Status = ModuleInitTable->InitFunction(ModuleID, Instances);

            if (Status < 0) {
                bmSignalEvent (0, Status, 1, ModuleID);
            }
            if (BoardOptions & OPTION_INFO_MESSAGES) {
                bmSignalEvent (0, I_INITIALIZATION_DONE, 1, ModuleID);
            }
        }
        ModuleInitTable++;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Wait until supply voltage is good
 *
 *      Checks the supply voltage and waits until it is good. If it isn't
 *      good within a timeout, an error is signalled on the error LED.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmWaitForPowerStable (void) {

    UInt32 StartTime = bmGetTime();
    Bool ErrorSignaled = FALSE;

    while (!halPowerGood()) {

        if (!ErrorSignaled &&
             bmTimeExpired(StartTime) > VOLTAGE_STABLE_TIMEOUT) {

            bmSignalErrorLed (LED_BLINK_CODE4, TRUE);
            ErrorSignaled = TRUE;
        }
    }
    if (ErrorSignaled) {
        bmSignalErrorLed (LED_BLINK_CODE4, FALSE);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief    The basemodule's own task
 *
 *      This function is called by the task scheduler periodically. It's
 *      purpose is to perform all actions to provide the base module's
 *      functionality. Therefore, the task functions of all base module
 *      internal components are called.
 *
 *  \iparam   Instance = Instance number of this module
 *
 *  \return   NO_ERROR or (negative) error code
 *
 *****************************************************************************/

static Error_t bmBaseModuleTask (UInt16 Instance) {

    static UInt32 LifeTimeUpdate = 0;

    bmNodeState_t NodeState = bmProcessModeChange();

    if (NodeState == NODE_STATE_IDENTIFY) {
        bmSendHardwareID();
    }
    else if (NodeState != NODE_STATE_STANDBY) {

        if (NodeState == NODE_STATE_NORMAL || NodeState == NODE_STATE_ASSEMBLY) {
            bmProcessEmergencyStop();
            bmProcessHeartbeat();
        }
        if (BoardOptions & OPTION_LIFECYCLE_DATA) {
            if (bmTimeExpired(LifeTimeUpdate) > LIFETIME_UPDATE_INTERVAL) {
                bmUpdateLifeTimeCounter();
                LifeTimeUpdate = bmGetTime();
            }
        }
        if (BoardOptions & OPTION_VOLTAGE_MONITOR) {
            bmMonitorSupplyVoltage();
        }
        if (BoardOptions & OPTION_SAFE_MONITOR) {
            bmMonitorSupplySafe();
        }
        if (BoardOptions & OPTION_CURRENT_MONITOR) {
            bmMonitorSupplyCurrent();
        }
        bmProcessEventCache();
    }
    if (BoardOptions & OPTION_STATUS_LED) {
        bmStatusLedTask();
    }
    halWatchdogTrigger();
    canTaskFunction();
    halControlTask();

    return (ModuleState);
}


/*****************************************************************************/
/*!
 *  \brief   Base module control function
 *
 *      This function is called by the scheduler to control the base module
 *      itself. It's purpose is to be able to handle the base module the
 *      same way as the function modules. This is one of the 3 interface
 *      functions each module has to supply.
 *
 *      Depending on ControlID the following actions are performed:
 *
 *      - MODULE_CONTROL_STANDBY
 *      - MODULE_CONTROL_SHUTDOWN
 *      - MODULE_CONTROL_WAKEUP
 *      - MODULE_CONTROL_FLUSH_DATA
 *      - MODULE_CONTROL_RESET_DATA
 *      - MODULE_CONTROL_RESET
 *
 *  \iparam  Instance  = Instance number of this module
 *  \iparam  ControlID = Control identifier
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmBaseModuleControl (UInt16 Instance, bmModuleControlID_t ControlID) {

    switch (ControlID) {

        case MODULE_CONTROL_SHUTDOWN:
            ModuleState = MODULE_STATE_STANDBY;
            return (bmFlushPartitions());

        case MODULE_CONTROL_WAKEUP:
            ModuleState = MODULE_STATE_READY;
            break;

        case MODULE_CONTROL_FLUSH_DATA:
            return (bmUpdateLifeTimeCounter());

        case MODULE_CONTROL_RESET_DATA:
            return (bmResetPartition());

        case MODULE_CONTROL_RESET:
            return (bmFlushPartitions());
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief    Base module status function
 *
 *      This function is called by the scheduler to request the status of
 *      the base module itself. It's purpose is to be able to handle the
 *      base module the same way as the function modules. This is one of
 *      the 3 interface functions each module has to supply.
 *
 *      Depending on the StatusID parameter, the following status values
 *      are returned:
 *
 *      - MODULE_STATUS_INSTANCES
 *      - MODULE_STATUS_MODULE_ID
 *      - MODULE_STATUS_VERSION
 *      - MODULE_STATUS_STATE
 *
 *      On other status IDs an error is returned.
 *
 *  \iparam   Instance = Instance number of this module
 *  \iparam   StatusID = selects which status is requested
 *
 *  \return   Positive status value or negative error code
 *
 ****************************************************************************/

static Error_t bmBaseModuleStatus (UInt16 Instance, bmModuleStatusID_t StatusID) {

    switch (StatusID) {

        case MODULE_STATUS_STATE:
            return (ModuleState);

        case MODULE_STATUS_MODULE_ID:
            return (BASEMODULE_MODULE_ID);

        case MODULE_STATUS_INSTANCES:
            return (1);

        case MODULE_STATUS_VERSION:
            return (BASEMODULE_VERSION);

        case MODULE_STATUS_VALUE:
            return (E_FUNCTION_UNSUPPORTED);
    }
    return (E_PARAMETER_OUT_OF_RANGE);
}

//****************************************************************************/

