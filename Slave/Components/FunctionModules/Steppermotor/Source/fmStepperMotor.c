/****************************************************************************/
/*! \file fmStepperMotor.c
 * 
 *  \brief Function module to control stepper motor
 *
 *  $Version: $ 0.1
 *  $Date:    $ 17.10.2010
 *  $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This modul contains all functions to control a stepper motor.
 *       
 *       The function module provides position and speed based movement
 *       
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

//#include "bmCommon.h"
//#include "bmPermData.h"

#include "fmStepperMotor.h"
#include "fmStepperMotorCAN.h"
#include "fmStepperMotorMotionISR.h"


/* Definition of non-volatile data elements */
#define PARAM_LAYOUT_VERSION BUILD_PARAMETER(0,2)    //!< Partition layout version
//#define PARAM_REV_COUNTER    BUILD_PARAMETER(2,4)    //!< step counter (total amount of revisions)
//#define PARAM_TRIP_COUNTER   BUILD_PARAMETER(6,4)    //!< trip counter (total amount of trips)
//#define PARAM_OPERATE_HOURS  BUILD_PARAMETER(10,4)    //!< oparation hours (total amount of activity)

#define SM_PARTITION_VERSION             1     //!< non-volatile data storage partition layout version
#define SM_PARTITION_SIZE               14     //!<  non-volatile data storage partition size

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define MODULE_VERSION         0x0001   //!< Version number of module


/*! Definition of non-volatile data elements */
#define MEM_PARAM_LAYOUT_VERSION     BUILD_PARAMETER(0,2)  //!< layout versio of the stored data following
#define MEM_PARAM_LIFE_TIME          BUILD_PARAMETER(2,4)  //!< life time
#define MEM_PARAM_LIFE_CYCLES        BUILD_PARAMETER(6,4)  //!< movement cycles during life time

/* Definition of non-volatile data elements */
#define PARAM_LAYOUT_VERSION BUILD_PARAMETER(0,2)    //!< Partition layout version
#define PARAM_REV_COUNTER    BUILD_PARAMETER(2,4)    //!< step counter (total amount of revisions)
#define PARAM_TRIP_COUNTER   BUILD_PARAMETER(6,4)    //!< trip counter (total amount of trips)
#define PARAM_OPERATE_HOURS  BUILD_PARAMETER(10,4)   //!< oparation hours (total amount of activity)

#define SM_PARTITION_VERSION             1     //!< non-volatile data storage partition layout version
#define SM_PARTITION_SIZE               14     //!< non-volatile data storage partition size


/*! Defines for the Flags member of the module instance data */
//#define FLAG_DATA_MODE           0x03    //!< Data mode bitmask
//#define FLAG_DATA_VALUE          0x00    //!< Data mode: immediate value
//#define FLAG_CLOCK_SOURCE        0x10    //!< Clock source select
//#define FLAG_USE_NOTIFICATION    0x40    //!< Enable notification
//#define FLAG_NEW_DATA            0x8000  //!< New data received


//*****************************************************************************/
// Private Type Definitions 
//*****************************************************************************/


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/
static Error_t smModuleControl (UInt16 Instance, bmModuleControlID_t ControlID);
static Error_t smModuleStatus  (UInt16 Instance, bmModuleStatusID_t StatusID);
static Error_t smModuleTask    (UInt16 Instance);
static Error_t smResetPartition (smData_t *Data);

//****************************************************************************/
// Private Variables 
//****************************************************************************/

/*! Persistent parameter descriptor table (used for verification) */
static const bmParamRange_t PermDataTable[] = {
    { PARAM_LAYOUT_VERSION, 1, SM_PARTITION_VERSION, SM_PARTITION_VERSION },
    { PARAM_REV_COUNTER, 0, MAX_UINT32, 0 },
    { PARAM_TRIP_COUNTER, 0, MAX_UINT32, 0 },
    { PARAM_OPERATE_HOURS, 0, MAX_UINT32, 0 },
};

static UInt8  smInstanceCount = 0;        //!< Number of module instances

//****************************************************************************/
// Module global variables 
//****************************************************************************/
smData_t *smDataTable = NULL;   //!< Data table for all instances
Handle_t  smHandleTimer = 0;    //!< Timer handle, one timer for up to four motors


/******************************************************************************/
/*! 
 *  \brief   Flush life time data to storage 
 *
 *      Updates the life cycle counters in non-volatile storage, i.e.:
 *   
 *      - Number of motor revisions
 *      - Number of motor trips
 *      - Operating time
 * 
 *      This is done by incrementing the life cycle counter in non-volatile 
 *      storage by the number of seconds expired since last updating. If
 *      succeeded, the counters-since-last-update are reset to zero.    
 * 
 *      Time measurement is done in milliseconds, but converted to 
 *      seconds before saving it into non-volatile memory.
 *
 *  \xparam  Data     = Module instance data pointer
 *  \iparam  Interval = Interval time to flush data [ms]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smFlushLifeTime(smData_t *Data, UInt16 Interval) 
{
    Error_t Status1 = NO_ERROR;
    Error_t Status2 = NO_ERROR;
    
    //if (bmTimeExpired(Data->Diagnostic.Interval) >= Interval) 
    {
        
        // update life time on/off cycles in non-volatile storage
        //if (LifeTime->Counter) 
        {
            UInt32 Revisions, ModuloRev;
            Status1 = bmIncStorageItem(Data->Memory, PARAM_TRIP_COUNTER, Data->LifeTimeData.TripCounter);                        
            if (Status1 == NO_ERROR) {
                Data->LifeTimeData.TripCounter = 0;
            }
            if(Data->LifeTimeData.StepCounter > 0){
                Revisions = Data->LifeTimeData.StepCounter / Data->Motor.Config.Resolution;
                ModuloRev = Data->LifeTimeData.StepCounter % Data->Motor.Config.Resolution;
                Status2 = bmIncStorageItem(Data->Memory, PARAM_REV_COUNTER, Revisions);
                if (Status2 == NO_ERROR) {
                    Data->LifeTimeData.StepCounter = ModuloRev;
                }
            }
            
            Status2 = bmIncStorageItem(Data->Memory, PARAM_OPERATE_HOURS, Data->LifeTimeData.OperationTime);
            if (Status2 == NO_ERROR) {
                Data->LifeTimeData.OperationTime = 1;
            }
        }

        Data->LifeTimeData.StartTime = bmGetTime();
    }        
    return (Status1 == NO_ERROR ? Status2 : Status1);
}


/*****************************************************************************/
/*! 
 *  \brief   Module control function
 *
 *      This function is called by the base module to control the function
 *      module. Depending on the ControlID parameter, the following actions
 *      are performed:
 * 
 *      - MODULE_CONTROL_STOP
 *      - MODULE_CONTROL_RESUME
 *      - MODULE_CONTROL_STANDBY  
 *      - MODULE_CONTROL_WAKEUP
 *      - MODULE_CONTROL_SHUTDOWN        
 * 
 *  \iparam  Instance  = Instance number of this module [in]
 *  \iparam  ControlID = Control code to select sub-function [in]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
static Error_t smModuleControl (UInt16 Instance, bmModuleControlID_t ControlID)
{
    smData_t* Data = &smDataTable[Instance];
    Error_t RetVal = NO_ERROR;

    switch (ControlID)
    {
        case MODULE_CONTROL_RESUME:  //!< Resume stopped instance
        case MODULE_CONTROL_WAKEUP:  //!< Wakeup instance from standby
            Data->ModuleState = MODULE_STATE_READY;
            break;

        case MODULE_CONTROL_STOP:  //!< Emergency stop
            Data->ModuleState = MODULE_STATE_STOPPED;
            break;

        case MODULE_CONTROL_SHUTDOWN:  //!< Go into shutdown mode
            Data->ModuleState = MODULE_STATE_STANDBY;
            break;

        case MODULE_CONTROL_RESET:  //!< Reset/disable module instance
			Data->Flags.Enable = !Data->Flags.Enable;
            break;
                                
        case MODULE_CONTROL_FLUSH_DATA:  //!< Flush permanent data to storage
            RetVal = smFlushLifeTime(Data, 0);
            break;
            
        case MODULE_CONTROL_RESET_DATA:  //!< Reset permanent data to default
            RetVal = smResetPartition(Data);
            break;

        default:             
            return (E_PARAMETER_OUT_OF_RANGE);  
    }

    return RetVal;

}

/*****************************************************************************/
/*! 
 *  \brief   Module Status Request
 *
 *      This function is called by the base module to request the status of
 *      the function module. Depending on the StatusID parameter, the 
 *      following status value is returned:
 * 
 *      - MODULE_STATUS_INSTANCES
 *      - MODULE_STATUS_MODULE_ID
 *      - MODULE_STATUS_VERSION
 *      - MODULE_STATUS_ENABLED
 *      - MODULE_STATUS_STATE
 *      - MODULE_STATUS_VALUE
 *      - MODULE_STATUS_ABORTED
 * 
 *  \iparam  Instance = Instance number of this module
 *  \iparam  StatusID = selects which status is requested
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t smModuleStatus(UInt16 Instance, bmModuleStatusID_t StatusID)
{
    smData_t* Data = &smDataTable[Instance];

    switch (StatusID)
    {                        
        case MODULE_STATUS_STATE:
            if (!Data->Flags.Enable)
                return (MODULE_STATE_DISABLED);
            return (Data->ModuleState);
        
        case MODULE_STATUS_MODULE_ID:
            return (MODULE_ID_STEPPER);
            
        case MODULE_STATUS_INSTANCES:
            return (smInstanceCount);
            
        case MODULE_STATUS_VERSION:
            return (MODULE_VERSION);
        
        case MODULE_STATUS_VALUE:
            //ToDo: check for Int32
            return (Data->Motion.Pos);
        default:
            break;            
    }
    
    return (E_PARAMETER_OUT_OF_RANGE);

}


Error_t smCheckRetCode (UInt16 Channel, Error_t RetCode)
{
    if (RetCode < 0)
        return bmSignalEvent(Channel, RetCode, TRUE, 0);
    return NO_ERROR;
}


smCheckForPosCodeError(smData_t* Data)
{
    if ((Data->LimitSwitches.PosCode.InvalidPosition) || (Data->LimitSwitches.PosCode.InvalidValue)) 
        Data->State = SM_STATE_INIT;  // in case of an invalid position code the reference run must be repeated
}


smCheckStopCondition(smData_t* Data)
{
    // if motor stopped because of position code
    if ((SM_SC_DIR_CCW == Data->Motion.Stop) || (SM_SC_DIR_CW == Data->Motion.Stop))
        bmSignalEvent(Data->Channel, E_SMOT_STOP_BY_POSCODE, TRUE, 0);    
    
    // if motor stopped because position get's off-limit
    if (SM_SC_OFFLIMIT == Data->Motion.Stop)
        bmSignalEvent(Data->Channel, E_SMOT_STOP_BY_OFFLIMIT, TRUE, 0);
}


Error_t smCheckEncoder(smData_t* Data)
{
    Error_t RetCode = NO_ERROR;

    if (MS_IDLE == Data->Motion.State)
    {
        if (NO_ERROR == (RetCode = smSampleEncoder(Data->Instance, TRUE)))
            Data->Motion.Pos = Data->Encoder.Pos;
    }
    else
        RetCode = smSampleEncoder(Data->Instance, FALSE);

    return RetCode;
}


Error_t smCheckPosition(smData_t* Data)
{
    Error_t RetCode = NO_ERROR;

    // tasks which have to be performed when motor is standing still
    if (MS_IDLE == Data->Motion.State)      
    {
        RetCode = smSampleLimitSwitches(Data->Instance, TRUE);
    }
    // tasks which have to be performed when motor is moving
    else                                    
    {
        if (Data->Motion.LSTrigger)
        {
            Data->Motion.LSTrigger = FALSE;
            RetCode = smSampleLimitSwitches(Data->Instance, FALSE);
        }
    }

    // tasks which have to be performed when actual position is known (framework already initialized)
    if (NO_ERROR == RetCode)
    {
        // check motion status
        switch (Data->State)
        {
        case SM_STATE_IDLE:
            // break; intentionally omitted
        case SM_STATE_POSITION:
            // break; intentionally omitted
        case SM_STATE_SPEED:
            // check for off-limit condition
            if ((Data->Motion.Pos < Data->Motor.FrameworkConfig.MinPosition) || (Data->Motion.Pos > Data->Motor.FrameworkConfig.MaxPosition))
            {
                if (!Data->OffLimit)
                {
                    Data->OffLimit = TRUE;
                    Data->Motion.Stop = SM_SC_OFFLIMIT;
                }
            }
            else
                Data->OffLimit = FALSE;

            RetCode = smCheckPosCode(Data->Instance);   // validate pos-code against position
            break;
        default:
            break;
        }
    }

    return RetCode;
}


Error_t smMovementAck (smData_t* Data, Bool Success)
{
    Int16 Speed = Data->Motion.Nominal.v >> 5; // speed (in half-step/s²) = velocity / ISR_MICROSTEPS_PER_HALFSTEP;
    SM_AckState_t Ack;

    if (Data->Motion.Param[Data->Motion.ActSet].NegPosCount)
        Speed = -Speed;

    if (Success)
    {
        Data->State = SM_STATE_IDLE;
        Ack = SM_ACK;
    }
    else
    {
        smCheckStopCondition(Data);
//        Data->State = SM_STATE_INIT;
        Data->State = SM_STATE_IDLE;
        Ack = SM_NACK;
    }
    smCheckForPosCodeError(Data);
    smCheckEncoder(Data);
    return smSendMovementAckn (Data->Channel, Data->Motion.Pos, Speed, Ack);
}


Error_t smCheckState (smData_t* Data)
{
    Error_t RetCode = NO_ERROR;

    smMotionState_t MotionState;    //!< motors motion state

    // check motion status
    switch (Data->State)
    {
    case SM_STATE_INIT:
        Data->LimitSwitches.PosCode.LastValue = -2;
        Data->LimitSwitches.PosCode.InvalidValue = FALSE;
        Data->LimitSwitches.PosCode.InvalidPosition = FALSE;
        Data->LastMotionState = Data->Motion.State;
        Data->OffLimit = FALSE;
        break;

    case SM_STATE_REFRUN:
        if ((RetCode = smReferenceRunTask (Data->Instance)) < 0)
        {
            Data->State = SM_STATE_INIT;  // in case of an error reference run must be repeated
            bmSignalEvent(Data->Channel, RetCode, TRUE, 0);
            RetCode = smRefRunAck(Data->Channel, Data->Motion.Pos, Data->LimitSwitches.PosCode.Value, SM_NACK);
            break;
        }
        if (SM_RRS_FINISHED == Data->RefRun.State)
        {
            Data->State = SM_STATE_IDLE;
            RetCode = smRefRunAck(Data->Channel, Data->Motion.Pos, Data->LimitSwitches.PosCode.Value, SM_ACK);
        }
        break;

    case SM_STATE_IDLE:
        MotionState = Data->Motion.State;
        if ((MS_IDLE == MotionState) && (MS_IDLE != Data->LastMotionState))   // detect motor stop
            smCheckStopCondition(Data);
        Data->LastMotionState = MotionState;

        smCheckForPosCodeError(Data);

        break;

    case SM_STATE_POSITION:
        if (MS_IDLE == Data->Motion.State)
        {
            RetCode = smMovementAck (Data, Data->Motion.AtTargetPosition);
        }
        break;

    case SM_STATE_SPEED:
        if ((MS_IDLE == Data->Motion.State) || Data->Motion.AtTargetSpeed)
        {
            RetCode = smMovementAck (Data, Data->Motion.AtTargetSpeed); 
        }
        break;

    default:
        break;
    }

    return RetCode;
}


/*****************************************************************************/
/*!
 *  \brief   Module Task Function
 *
 *      This function is called by the task scheduler periodically.
 *      It's purpose is to perform all actions to provide the modules
 *      functionality.
 *
 *  \iparam  Instance = Instance number of this module
 *
 *  \return  State of instance or (negative) error code
 *
 *****************************************************************************/
static Error_t smModuleTask (UInt16 Instance)
{
    smData_t* Data = &smDataTable[Instance];
    
    if (!Data->Flags.Enable)
        return (MODULE_STATE_DISABLED);
    
    // tasks which have to be performed when motor is standing still
    if (MS_IDLE == Data->Motion.State)      
    {
        smCheckRetCode (Data->Channel, smCheckMotorStopCS(&Data->Motor));
    }
    
    smCheckRetCode (Data->Channel, smCheckEncoder (Data));

    smCheckRetCode (Data->Channel, smCheckPosition (Data));
    
    smCheckRetCode (Data->Channel, smCheckState (Data));
    

    return (Data->ModuleState);
}


/*! 
 *  \brief   Verify module's data partition
 *
 *      Verifies the content of the data partition for plausibility. Each 
 *      data item must be inside the range defined for that item. If only
 *      a single data item is outside it's range, ALL data items are reset 
 *      to defaults. If the partition is ok, the partition layout version 
 *      is checked. If the layout version in the partition is older than 
 *      the one required by the actual software, data layout in the  
 *      partition will be converted. 
 * 
 *      If the layout version is zero, all data items in the partition
 *      are set to defaults as well. This may happen, if the partition
 *      was erased due to a checksum error or during manufacturing.
 * 
 *  \xparam  Data = Module instance data pointer
 * 
 *  \return  NO_ERROR or error code
 *
 ******************************************************************************/
static Error_t smVerifyPartition(smData_t *Data)
{
    /*Error_t RetVal = NO_ERROR;
    UInt32 LayoutVersion;
        
    if (Data->Memory >= 0) {
        
        if (!bmVerifyNonVolatileData(Data->Memory, PermDataTable, ELEMENTS(PermDataTable))) {
            bmSignalEvent(Data->Channel, E_PERSISTENTS_INVALID, 1, 0);            
            RetVal = smResetPartition(Data);
            if(RetVal != NO_ERROR)
            {
                //exit function in case of error
                return RetVal;
            }
        }
        
        LayoutVersion = bmGetStorageItem(Data->Memory, PARTITION_VERSION, 0);

        // if required, upgrade partition to new layout format
        if (LayoutVersion < PARTITION_VERSION) {
            // currently no conversion required
            bmSignalEvent(Data->Channel, I_PARTITION_CONVERTED, 1, LayoutVersion);                                
        }
    }*/

    return (NO_ERROR);        
}

/******************************************************************************/
/*! 
 *  \brief   Reset non-volatile data to default
 *
 *      Resets all data in instance's data partition to defaults and sends
 *      an info message about that fact to the master for logging.
 * 
 *      This function is called by the base module via the module control
 *      function, whenever a reset-counter service command is received.
 *
 *  \xparam  Data = Module instance data pointer
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
static Error_t smResetPartition(smData_t *Data)
{
    Error_t Status;
    
    if (Data->Memory >= 0) {               									  
        bmSignalEvent(Data->Channel, I_PARTITION_RESET, 1, 0);
        
        Data->LifeTimeData.StepCounter = 0;
        Data->LifeTimeData.TripCounter = 0;
        Data->LifeTimeData.OperationTime = 0;
                        
        Status = bmResetStorageItems (
            Data->Memory, PermDataTable, ELEMENTS(PermDataTable));
        
        return (Status);
    }

    return (E_STORAGE_OPEN_ERROR);
}


Error_t smCloseDevices(smData_t *Data)
{
    Error_t RetCode = NO_ERROR;

	// close stepper motor device
    RetCode = smCloseMotor(&Data->Motor);

    // close encoder device
    if (NO_ERROR == RetCode)
        RetCode = smCloseEncoder(&Data->Encoder);

    // close limit switch devices
    if (NO_ERROR == RetCode)
        RetCode = smCloseLimitSwitches(&Data->LimitSwitches);

	return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief   Connect the HAL components to the motor instance
 *
 *           If exists, the limit switches and the endocer will be opened
 *
 *  \xparam  Data = Module instance data pointer
 *
 *  \return  NO_ERROR or error code
 *
 ******************************************************************************/
Error_t smOpenDevices(smData_t *Data)
{
    Error_t RetCode = NO_ERROR;
	UInt8 Instance;
//    UInt16 MaxLimit = 0;

	RetCode = bmGetInstance(Data->Channel);
	if (RetCode < 0)
		return RetCode;
	Instance = RetCode;

    RetCode = smOpenMotor(&Data->Motor, Instance);

    // open limit switches
	if (NO_ERROR == RetCode)
        RetCode = smOpenLimitSwitches(&Data->LimitSwitches, Instance);

    // open encoder device
	if (NO_ERROR == RetCode)
        RetCode = smOpenEncoder(&Data->Encoder, Instance, Data->Motor.Config.Resolution);

	// in case one of the devices could not be opened, try to close already opened devices
	if (RetCode < 0)
		smCloseDevices(Data);

    //Start the motor's life cycle data supervision
	if (RetCode >= 0)
        Data->LifeTimeData.StartTime = bmGetTime();

	return RetCode;
}


Error_t smInitTimer(void)
{
    Error_t RetCode = NO_ERROR;

    TimerMode_t TimerMode = {
        //TIM_MODE_COUNT_DOWN, TIM_MODE_INTERVAL, TIM_MODE_INTERNAL, 0
        TIM_MODE_COUNT_UP, TIM_MODE_INTERVAL, TIM_MODE_INTERNAL, 0
    };

    const UInt16 TimerPrescale = 36000;   

    //Open the timer, there is one timer for up to four motors
    // and register the timer's interrupt handler, which will be called on capture compare events as well
    if ((RetCode = halTimerOpen(HAL_STEPPER1_TIMER, 5, smMotionISR)) < 0) {
        return RetCode;
    }
    smHandleTimer = RetCode;

    //setup the timer with mode and prescaler settings
    RetCode = halTimerSetup(smHandleTimer, &TimerMode, TimerPrescale);
    
    // set the timer's prescaler.
    // with a cycle time of 1sec/72MHz = 13.889 ns a prescaler value of 6 lets the timer count with
    // a cycle time of 83.33 ns
    if (RetCode >= 0)
        RetCode = halTimerWrite(smHandleTimer, TIM_REG_PRESCALER, 0x0006);

    // the timers reload register is set to 0xffff. 
    if (RetCode >= 0)
        RetCode = halTimerWrite(smHandleTimer, TIM_REG_RELOAD, 0xffff);

    // start the timer
    if (RetCode >= 0)
        RetCode = halTimerControl(smHandleTimer, TIM_CTRL_START);

    //halTimerControl(DataTable[instance].HandleTimer, TIM_INTR_ENABLE);

    if (RetCode < 0)
    {
        halTimerClose (smHandleTimer);
        smHandleTimer = 0;
        return RetCode;
    }

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief   Initializes this function module
 *
 *      Initializes this function module by registering it to the task 
 *      scheduler and registers all handler functions to the CAN message 
 *      dispatcher. Furthermore, it allocates and initializes memory for 
 *      the variables for all module instances. In addition, following
 *      steps are performed for each instance of the module:
 * 
 *      - Open and verify instance's data partition (storage)
 *      - Requests the instance's board options  
 * 
 *      This function is called once during startup by the base module.
 *  
 *  \iparam  ModuleID  = Module identifier
 *  \iparam  Instances = Number of instances of this module
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smInitializeModule(UInt16 ModuleID, UInt16 Instances)
{
    Error_t RetCode = NO_ERROR;
    Int8 Instance;

    // assignment between receiveable CAN messages and callback functions
    static bmCallbackEntry_t Commands[] = {
        { MSG_SMOT_CONFIG,             smConfigure},

        { MSG_SMOT_SET_ENABLE,         smSetEnableState},
        { MSG_SMOT_REQ_REF_RUN,        smReferenceRun},
        { MSG_SMOT_TARGET_POS,         smTargetPosition},
        { MSG_SMOT_TARGET_SPEED,       smTargetSpeed},

        { MSG_SMOT_ACT_POS_REQ,        smReqPosition},        
        { MSG_SMOT_ACT_SPEED_REQ,      smReqSpeed},

        { MSG_SMOT_LIFE_CYCLE_REQ,     smCanMsgReqLifeCycleData},
    };
    
    // interface function for module adaption
    static bmModuleInterface_t Interface = {
        smModuleTask, 
        smModuleControl, 
        smModuleStatus
    };

    // allocate the module instances data storage
    size_t size = sizeof(smData_t);
    smDataTable = calloc(Instances, size);
    // adapt Stack_Size / Heap_Size 

    //DataTable = calloc(Instances, size);
    if (NULL == smDataTable) {
        return E_MEMORY_FULL;
    }

    // register function module to the scheduler    
    if ((RetCode = bmRegisterModule(ModuleID, Instances, &Interface)) < 0)
        return RetCode;
    
    //smRegisterDebugNames(ModuleID);

    // init and open timer device
     if ((RetCode = smInitTimer()) < 0)
        return RetCode;
        
    // init each module instance
    for(Instance=0; Instance < Instances; Instance++)
    {
        smData_t *Data = &smDataTable[Instance];

        Data->Instance = Instance;          // store instance number of this module
        Data->Channel = bmGetChannel(bmGetTaskID(ModuleID, Instance)); 

        Data->ModuleState = MODULE_STATE_READY;
        Data->Flags.Enable = FALSE;

        Data->Memory = bmOpenPermStorage(ModuleID, Instance, SM_PARTITION_SIZE);

        // init lifetime data
        Data->LifeTimeData.LastMoveDir = SMOT_MOVEDIR_UNDEF;

		// init devices
        smInitMotor(&Data->Motor);
        smInitLimitSwitches(&Data->LimitSwitches);
        smInitEncoder(&Data->Encoder);

        // init profiles
        smInitProfiles(&smDataTable[Instance].Profiles);

        // init control
        smInitReferenceRun(&Data->RefRun);
        smInitMotion(&Data->Motion, Instance);
        Data->State = SM_STATE_INIT;
        Data->LastMotionState = Data->Motion.State;
        Data->OffLimit = FALSE;

                       
        smVerifyPartition(&smDataTable[Instance]);
    }
    
    RetCode = canRegisterMessages(ModuleID, Commands, ELEMENTS(Commands));

    smInstanceCount = Instances;
    
    return RetCode;
}
