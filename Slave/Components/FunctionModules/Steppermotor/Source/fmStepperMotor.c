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
#include "fmStepperMotor.h"
#include "fmStepperMotorCAN.h"
#include "fmStepperMotorConfig.h"
#include "fmStepperMotorMotionISR.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define MODULE_VERSION                  60002   //!< Version number of module


//*****************************************************************************/
// Private Type Definitions 
//*****************************************************************************/


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/
static Error_t smModuleControl (UInt16 Instance, bmModuleControlID_t ControlID);
static Error_t smModuleStatus  (UInt16 Instance, bmModuleStatusID_t StatusID);
static Error_t smModuleTask    (UInt16 Instance);

void smInitInstance (smData_t *Data);


//****************************************************************************/
// Private Variables 
//****************************************************************************/

static UInt8  smInstanceCount = 0;        //!< Number of module instances


//****************************************************************************/
// Module global variables 
//****************************************************************************/
smData_t *smDataTable = NULL;   //!< Data table for all instances
Handle_t  smHandleTimer = 0;    //!< Timer handle, one timer for up to four motors
UInt16    smTimerNo = 0;                //!< Physical timer number

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
        case MODULE_CONTROL_STOP:  //!< Emergency stop
            Data->Motion.Stop = SM_SC_ALWAYS;               // stop any movement
            Data->ModuleState = MODULE_STATE_STOPPED;
            break;

        case MODULE_CONTROL_RESUME:  //!< Resume stopped instance
            Data->ModuleState = MODULE_STATE_READY;
            break;

        case MODULE_CONTROL_SHUTDOWN:  //!< Go into shutdown mode
            Data->Motion.Stop = SM_SC_ALWAYS;               // stop any movement
            while (MS_IDLE != Data->Motion.State)           // wait until movement is stopped
            {
            }
            if ((RetVal = smCloseDevices(Data)) < 0)        // close hal devices, this will also switch off driver stage
                return RetVal;
            Data->ModuleState = MODULE_STATE_STANDBY;
            break;

        case MODULE_CONTROL_WAKEUP:  //!< Wakeup instance from standby
            if (Data->Flags.Enable)                         // enable module again if it was enabled before
            {
                if ((RetVal = smEnable(Data, Data->Flags.dbg_skipRefRun)) < 0)
                    return RetVal;
            }
            Data->ModuleState = MODULE_STATE_READY;
            break;

        case MODULE_CONTROL_RESET:  //!< Reset module instance
            Data->Motion.Stop = SM_SC_ALWAYS;                       // stop any movement
            while (MS_IDLE != Data->Motion.State)           // wait until movement is stopped 
            if ((RetVal = smCloseDevices(Data)) < 0)                // close hal devices, this will also switch off driver stage
                return RetVal;

            if (Data->Memory.Handle >= 0)                           // close memory
            {
                if ((RetVal = bmClosePermStorage(Data->Memory.Handle)) < 0)
                    return RetVal;
            }

            if (Data->Profiles.Set)
                free (Data->Profiles.Set);                          // deallocate motion profile memory

            smInitInstance(Data);                                   // reinitialize the instance data

            break;
                                
        case MODULE_CONTROL_FLUSH_DATA:  //!< Flush permanent data to storage
            RetVal = smFlushMemory(Data->Instance, 0);
            break;
            
        case MODULE_CONTROL_RESET_DATA:  //!< Reset permanent data to default
            RetVal = smResetMemory(Data->Instance);
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
            if (MODULE_STATE_READY == Data->ModuleState)
            {
                if (!Data->Flags.Enable)
                    return (MODULE_STATE_DISABLED);
                if (MS_IDLE != Data->Motion.State)
                    return (MODULE_STATE_BUSY);
            }
            return (Data->ModuleState);
        
        case MODULE_STATUS_MODULE_ID:
            return (MODULE_ID_STEPPER);
            
        case MODULE_STATUS_INSTANCES:
            return (smInstanceCount);
            
        case MODULE_STATUS_VERSION:
            return (MODULE_VERSION);
        
        case MODULE_STATUS_VALUE:
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


void smCheckForPosCodeError(smData_t* Data)
{
    if ((Data->LimitSwitches.PosCode.InvalidPosition) || (Data->LimitSwitches.PosCode.InvalidValue)) 
        Data->State = SM_STATE_INIT;  // in case of an invalid position code the reference run must be repeated
}


void smCheckStopCondition(smData_t* Data)
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

    if (FALSE == Data->Encoder.Config.Exists)
        return NO_ERROR;

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
    
    smCheckRetCode (Data->Channel, smFlushMemory(Data->Instance, SM_MEMORY_UPDATE_INTERVAL));

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
    
    smCountOperationTime(&Data->Motor);

    return (Data->ModuleState);
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

    return RetCode;
}


Error_t smEnable(smData_t *Data, Bool SkipRefRun)
{
    Error_t RetCode  = NO_ERROR;

    if ((RetCode = smConfigIsValid(Data)) < 0)  // check if configuration data is complete and valid
        return RetCode;
    
    if ((RetCode = smOpenDevices(Data)) < 0)    // open hal devices, this will also switch on driver stage and init step value to zero
        return RetCode;
    
    if ((RetCode = halStepperControl(Data->Motor.Handle, STP_CTRL_CLEAR_ERROR, ON)) >= 0)   // clear pending error status
    {
        Data->Motion.ResetPos = Data->Motor.FrameworkConfig.ResetPosition;
        Data->Motion.SinIndex = 0;
        Data->Motor.StopCSTimeout = Data->Motor.Config.StopCSDelay;
        Data->Motor.StopCSTime = bmGetTime();
    }
    
    if (RetCode < 0)
    {
        smCloseDevices(Data);
        return RetCode;
    }
    
    // set actual motor position to reference position offset
    Data->Motion.Pos = Data->RefRun.Config.Offset;
    smInitEncoderPos(&Data->Encoder, Data->Motor.Config.Resolution, Data->Motion.Pos);
    
    Data->Motion.State = MS_IDLE;
    
    if (!SkipRefRun)
    {
        Data->State = SM_STATE_INIT;
    }
    else
    //*** for DEBUG purposes only : skip need to do reference run first ***
    {
        Data->State = SM_STATE_IDLE;
    }
    //*** END for DEBUG purposes only
    
    Data->Flags.Enable = 1;
    Data->Flags.dbg_skipRefRun = SkipRefRun;

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

    // Open the timer, there is one timer for up to four motors
    // and register the timer's interrupt handler, which will be called on capture compare events as well
    if ((RetCode = halTimerOpen(HAL_STEPPER1_TIMER, 5, smMotionISR)) < 0) {
        return RetCode;
    }
    smHandleTimer = RetCode;

    // Get physical timer number
    RetCode = halTimerStatus (smHandleTimer, TIM_STAT_UNITNO);

    //setup the timer with mode and prescaler settings
    if (RetCode >= 0) {
        smTimerNo = RetCode;
        RetCode = halTimerSetup(smHandleTimer, &TimerMode, TimerPrescale);
    }

    // set the timer's prescaler.
    // with a cycle time of 1sec/72MHz = 13.889 ns a prescaler value of 6 lets the timer count with
    // a cycle time of 83.33 ns
    if (RetCode >= 0) {
        RetCode = halTimerWrite(smHandleTimer, TIM_REG_PRESCALER, 0x0006);
    }

    // the timers reload register is set to 0xffff. 
    if (RetCode >= 0) {
        RetCode = halTimerWrite(smHandleTimer, TIM_REG_RELOAD, 0xffff);
    }

    // start the timer
    if (RetCode >= 0) {
        RetCode = halTimerControl(smHandleTimer, TIM_CTRL_START);
    }

    //halTimerControl(DataTable[instance].HandleTimer, TIM_INTR_ENABLE);

    if (RetCode < 0) {
        halTimerClose (smHandleTimer);
        smHandleTimer = 0;
        smTimerNo = 0;
        return RetCode;
    }

    return NO_ERROR;
}


void smInitInstance (smData_t *Data)
{
    Data->ModuleState = MODULE_STATE_READY;
    Data->Flags.Enable = FALSE;
    Data->Flags.dbg_skipRefRun = FALSE;
    
    // init permanent storage
    smInitMemory(Data->Instance);
    
    // init devices
    smInitMotor(&Data->Motor, &Data->Memory);
    smInitLimitSwitches(&Data->LimitSwitches);
    smInitEncoder(&Data->Encoder);
    
    // init profiles
    smInitProfiles(&Data->Profiles);
    
    // init control
    smInitReferenceRun(&Data->RefRun);
    smInitMotion(&Data->Motion, Data->Instance);
    Data->State = SM_STATE_INIT;
    Data->LastMotionState = Data->Motion.State;
    Data->OffLimit = FALSE;
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

        { MSG_SMOT_OPTIME_REQ,         smReqOperationTime},
        { MSG_SMOT_REVCOUNT_REQ,       smReqRevolutionCount},
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

        smInitInstance(Data);
    }
    
    RetCode = canRegisterMessages(ModuleID, Commands, ELEMENTS(Commands));

    smInstanceCount = Instances;
    
    return RetCode;
}
