/****************************************************************************/
/*! \file
 *
 *  \brief Function module for stepper motor
 *
 *  $Rev:    $ 0.1
 *  $Date:   $ 11.06.2012
 *  $Author: $ Rainer Boehles
 *
 *  \b Description:
 *
 *      This is the main module for function module stepper motor.
 *
 *      It contains the necessary interface functions to be integrated by the
 *      base module:
 *          - module / stepper instances initialization
 *          - module task
 *          - module control
 *          - module status
 *
 *      This module also contains functions to open and close the utilized
 *      hardware devices like stepper motor, limit switches and encoder.
 *      The devices are opened when the motor is enabled and closed when
 *      disabled.
 *
 *      When stepper module is 'shutdown' by the base module the motor is
 *      stopped and power is switched off, wheras in case of 'emergency stop'
 *      the motor will only be stopped, but not powered off.
 *
 *      During 'normal mode' the module task is called periodically by base
 *      module. If motor is 'enabled' the motor task will perform the actions
 *      needed according to the stepper module state, like
 *          - sampling limit switches
 *          - sampling encoder
 *          - switch off motor power after configured delay
 *          - manage reference run
 *          - manage target position/speed movements
 *          - update life cycle data
 *
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
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

#define MODULE_VERSION          60010   //!< Version number of module (above 60000 for test versions)


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

void smInitInstance (smData_t *Data);


//****************************************************************************/
// Private Variables 
//****************************************************************************/

static UInt8  smInstanceCount = 0;      //!< Number of module instances


//****************************************************************************/
// Module global variables 
//****************************************************************************/

smData_t *smDataTable = NULL;           //!< Data table for all instances
Handle_t  smHandleTimer = 0;            //!< Timer handle, one timer for up to four motors


/*****************************************************************************/
/*! 
 *  \brief   Module control function
 *
 *      This function is called by the base module to control the function
 *      module. Depending on the ControlID parameter, the following actions
 *      are performed:
 * 
 *      - MODULE_CONTROL_STOP
 *          Motor is stopped immediately, but still powered on.
 *          No commands accpted any more.
 *      - MODULE_CONTROL_RESUME
 *          Return to normal operation.
 *      - MODULE_CONTROL_SHUTDOWN
 *          Motor is stopped immediately, motor is powered off.
 *          No commands accpted any more.
 *      - MODULE_CONTROL_WAKEUP
 *          Return to normal operation.
 *          If module motor was enabled, when module was 'shutdown', then
 *          module is re-enabled.
 *      - MODULE_CONTROL_RESET
 *          Reseting the module into state it was, when module have been
 *          intialized by base module.
 *      - MODULE_CONTROL_FLUSH_DATA
 *          Write all permanent data to non volatile memory
 *      - MODULE_CONTROL_RESET_DATA
 *          Reset all permanent data in non volatile memory
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
        case MODULE_CONTROL_STOP:       //!< Emergency stop
            Data->Motion.Stop = SM_SC_ALWAYS;               // stop any movement
            if (MS_IDLE != Data->Motion.State) {
                bmSignalEvent(Data->Channel, E_SMOT_STOP_BY_EMERGENCYSTOP, TRUE, 0);
            }
            Data->Flags.Stopped = TRUE;                     // set stop flag to enter emergency stop mode
            break;

        case MODULE_CONTROL_RESUME:     //!< Resume stopped instance
            Data->Flags.Stopped = FALSE;                   // leave emergency stop mode
            break;

        case MODULE_CONTROL_SHUTDOWN:   //!< Go into shutdown mode
            Data->Motion.Stop = SM_SC_ALWAYS;               // stop any movement
            if (MS_IDLE != Data->Motion.State) {
                bmSignalEvent(Data->Channel, E_SMOT_STOP_BY_SHUTDOWN, TRUE, 0);
            }
            Data->Flags.Shutdown = TRUE;                    // set shutdown flag to enter standby mode
            break;

        case MODULE_CONTROL_WAKEUP:     //!< Wakeup instance from standby
            if (Data->Flags.Enable)                         // enable module again if it was enabled before
            {
                if ((RetVal = smEnable(Data, Data->Flags.dbg_skipRefRun)) < 0) {
                    return RetVal;
                }
            }
            Data->Flags.Shutdown = FALSE;                   // leave standby mode
            break;

        case MODULE_CONTROL_RESET:      //!< Reset module instance
            halCapComControl(smHandleTimer, Data->Motion.CCR.Unit, TIM_INTR_DISABLE);    // stop motion by disabling timer CCR irq
            if ((RetVal = smCloseDevices(Data)) < 0) {      // close hal devices, this will also switch off driver stage
                return RetVal;
            }

            if (Data->Memory.Handle >= 0)                           // close memory
            {
                if ((RetVal = bmClosePermStorage(Data->Memory.Handle)) < 0) {
                    return RetVal;
                }
            }

            if (Data->Profiles.Set) {
                free (Data->Profiles.Set);                  // deallocate motion profile memory
            }

            smInitInstance(Data);                           // reinitialize the instance data

            break;
                                
        case MODULE_CONTROL_FLUSH_DATA: //!< Flush permanent data to storage
            RetVal = smFlushMemory(Data->Instance, 0);
            break;
            
        case MODULE_CONTROL_RESET_DATA: //!< Reset permanent data to default
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
 *      - MODULE_STATUS_STATE
            active module state
 *      - MODULE_STATUS_INSTANCES
 *          amount of stepper instances
 *      - MODULE_STATUS_MODULE_ID
 *          ID of function module stepper motor
 *      - MODULE_STATUS_VERSION
 *          software version of function module stepper motor
 *      - MODULE_STATUS_VALUE
 *          stpper motor position
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
            if (!Data->Flags.Enable) {
                return (MODULE_STATE_DISABLED);
            }
            if (Data->Flags.Stopped) {
                return (MODULE_STATE_STOPPED);
            }
            if (MS_IDLE != Data->Motion.State) {
                return (MODULE_STATE_BUSY);
            }
            if (Data->Flags.Shutdown) {
                return (MODULE_STATE_STANDBY);
            }
            return (MODULE_STATE_READY);
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


/******************************************************************************/
/*!
 *  \brief  Check return code and send CAN event for any error code
 *
 *      Supplied 'return code' is checked if it stands for an error code.
 *      In case of an error code a CAN event is send to master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  RetCode = return code to be checked
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smCheckRetCode (UInt16 Channel, Error_t RetCode)
{
    if (RetCode < 0) {
        return bmSignalEvent(Channel, RetCode, TRUE, 0);
    }

    return NO_ERROR;
}


/******************************************************************************/
/*!
 *  \brief  Check if position code from limit switches is invalid
 *
 *      Position code from limit switches can have an invalid value or can
 *      not match to the actual motor position. Because both cases are fatal
 *      a reference run is forced to reinitialize the reference framework.
 *
 *  \iparam  Data = Pointer to module instance's data
 *
 ******************************************************************************/
void smCheckForPosCodeError(smData_t* Data)
{
    if (NO_ERROR != Data->LimitSwitches.PosCode.ErrCode) {
        Data->State = SM_STATE_INIT;  // in case of an invalid position code the reference run must be repeated
    }
}


/******************************************************************************/
/*!
 *  \brief  Check encoder position
 *
 *      If encoder does exist the encoder position is sampled.
 *      If the motor is idle, then the actual encoder position is used as
 *      actual motor position.
 *
 *  \iparam  Data = Pointer to module instance's data
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smCheckEncoder(smData_t* Data)
{
    Error_t RetCode = NO_ERROR;

    if (FALSE == Data->Encoder.Config.Exists) {
        return NO_ERROR;
    }

    if (MS_IDLE == Data->Motion.State)
    {
        if (NO_ERROR == (RetCode = smSampleEncoder(Data->Instance, TRUE))) {
            Data->Motion.Pos = Data->Encoder.Pos;
        }
    }
    else {
        RetCode = smSampleEncoder(Data->Instance, FALSE);
    }

    return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief  Check limit switch position code against motor position
 *
 *      Limit switches are sampled to get actual position code.
 *      If position code is valid and the reference framwork is already initialized
 *      then it's value is checked to match with actual motor position.
 *      Also actual motor position is checked not to be at an 'off limit'
 *      position. It case it is motor will be stopped immediately.
 *
 *  \iparam  Data = Pointer to module instance's data
 *
 ******************************************************************************/
void smCheckPosition(smData_t* Data)
{
    Error_t ErrCode = NO_ERROR;

    // tasks which have to be performed when motor is standing still
    if (MS_IDLE == Data->Motion.State) {
        ErrCode = smSampleLimitSwitches(Data->Instance, TRUE);
    }
    // tasks which have to be performed when motor is moving
    else {
        if (Data->Motion.LSTrigger)
        {
            Data->Motion.LSTrigger = FALSE;
            ErrCode = smSampleLimitSwitches(Data->Instance, FALSE);
        }
    }

    // tasks which have to be performed when actual position is known (framework already initialized)
    if (NO_ERROR == ErrCode) {
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
            else {
                Data->OffLimit = FALSE;
            }

            smCheckPosCode(Data->Instance); // validate pos-code against position
            break;
        default:
            break;
        }
    }
}


/******************************************************************************/
/*! 
 *  \brief  Send 'movement finished' CAN msg to the master
 *
 *      When target position / speed is reached a CAN message is send to the
 *      master to inform about succes / fail of the movement command.
 *      In addition the msg contains actual motor speed and position.
 *
 *  \iparam  Data    = Pointer to module instance's data
 *  \iparam  Success = true for success, false for movement failed
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smMovementAck (smData_t* Data, Bool Success)
{
    Int16 Speed = Data->Motion.Nominal.v >> 5; // speed (in half-step/s²) = velocity / ISR_MICROSTEPS_PER_HALFSTEP;
    SM_AckState_t Ack;

    if (Data->Motion.Param[Data->Motion.ActSet].NegPosCount) {
        Speed = -Speed;
    }

    if (Success)
    {
        Data->State = SM_STATE_IDLE;
        Ack = SM_ACK;
    }
    else
    {
//        Data->State = SM_STATE_INIT;
        Data->State = SM_STATE_IDLE;
        Ack = SM_NACK;
    }
    smCheckForPosCodeError(Data);
    smCheckEncoder(Data);
    return smSendMovementAckn (Data->Channel, Data->Motion.Pos, Speed, Ack);
}


/******************************************************************************/
/*! 
 *  \brief  Perform state dependent actions
 *
 *      Performs the actions needed if stepper module is in one of following
 *      states:
 *          - SM_STATE_INIT = reference framework is not yet initialized
 *              Position supervision data is reset
 *          - SM_STATE_REFRUN = reference run is active
 *              Execute reference run and send CAN msg about success/fail to
 *              master when reference run is finished
 *          - SM_STATE_IDLE = module is in normal operation mode
 *              Because motor can rotate although no movement cmd is active
 *              (from preceeding target speed cmd), it's checked if the motor
 *              have been stopped. If motor was stopped then the stop condition
 *              is send to master and if needed a reference run is forced by
 *              switching back to state SM_STATE_INIT
 *          - SM_STATE_POSITION = target position movement is active
 *              Send CAN msg about success/fail to master when movement is
 *              finished
 *          - SM_STATE_SPEED = target speed movement is active
 *              Send CAN msg about success/fail to master when movement is
 *              finished
 *
 *  \iparam  Data = Pointer to module instance's data
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smCheckState (smData_t* Data)
{
    Error_t RetCode = NO_ERROR;

    // check motion status
    switch (Data->State)
    {
    case SM_STATE_INIT:
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
 *      If module is not 'shutdown', 'emergency stopped' or disabled the module
 *      task will take care for:
 *          - switch off motor power after configured delay
 *          - sample encoder
 *          - sample limit switches and validate position code
 *          - perform actions specific for a certain stepper module state
 *          - update life cycle data
 *
 *  \iparam  Instance = Instance number of this module
 *
 *  \return  State of instance
 *
 *****************************************************************************/
static Error_t smModuleTask (UInt16 Instance)
{
    smData_t* Data = &smDataTable[Instance];

    smCheckRetCode (Data->Channel, smFlushMemory(Data->Instance, SM_MEMORY_UPDATE_INTERVAL));

    if (!Data->Flags.Enable) {
        return (MODULE_STATE_DISABLED);
    }

    // tasks which have to be performed when motor is standing still
    if (MS_IDLE == Data->Motion.State) {
        smCheckMotorStopCS(&Data->Motor, Data->Channel);
    }

    // stop the movement if motor have stopped
    if (MS_STOP == Data->Motion.State) {
        smStopMotion(smDataTable[Instance].Instance);
        Data->Motion.State = MS_IDLE;
    }

    smCheckRetCode (Data->Channel, smCheckEncoder (Data));

    smCheckPosition (Data);

    smCheckRetCode (Data->Channel, smCheckState (Data));

    smCountOperationTime(&Data->Motor);

    // as long as motor is moving the module is busy
    if (MS_IDLE != Data->Motion.State) {
        return (MODULE_STATE_BUSY);
    }

    // if motor is idle and shutdown is requested then enter the standby mode
    if (Data->Flags.Shutdown) {
        Error_t ErrCode;
        if (NO_ERROR != (ErrCode = smCloseDevices(Data))) { // close hal devices, this will also switch off driver stage
            bmSignalEvent(Data->Channel, ErrCode, TRUE, 0);
        }
        return (MODULE_STATE_STANDBY);
    }

    if (Data->Flags.Stopped) {
        return (MODULE_STATE_STOPPED);
    }

    return (MODULE_STATE_READY);
}


/******************************************************************************/
/*! 
 *  \brief   Close all devices
 *
 *      Stepper motor, limit switches and encoder devies are closed.
 *
 *  \iparam  Data = Module instance data pointer
 *
 *  \return  NO_ERROR or error code
 *
 ******************************************************************************/
Error_t smCloseDevices(smData_t *Data)
{
    Error_t RetCode = NO_ERROR;

    // close stepper motor device
    RetCode = smCloseMotor(&Data->Motor);

    // close encoder device
    if (NO_ERROR == RetCode) {
        RetCode = smCloseEncoder(&Data->Encoder);
    }

    // close limit switch devices
    if (NO_ERROR == RetCode) {
        RetCode = smCloseLimitSwitches(&Data->LimitSwitches);
    }

    return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief   Open all devices
 *
 *      Stepper motor, limit switches and encoder devies are opened.
 *      In case one of them failed to get opened properly, all devices will be
 *      closed again.
 *
 *  \iparam  Data = Module instance data pointer
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
    if (RetCode < 0) {
        return RetCode;
    }
    Instance = RetCode;

    RetCode = smOpenMotor(&Data->Motor, Instance);

    // open limit switches
    if (NO_ERROR == RetCode) {
        RetCode = smOpenLimitSwitches(&Data->LimitSwitches, Instance);
    }

    // open encoder device
    if (NO_ERROR == RetCode) {
        RetCode = smOpenEncoder(&Data->Encoder, Instance, &Data->Motor);
    }

    // in case one of the devices could not be opened, try to close already opened devices
    if (RetCode < 0) {
        smCloseDevices(Data);
    }

    return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief   Enable stepper module
 *
 *      A complete and valid configuration must have been received from master
 *      before module can be enabled.
 *      If this is the case then all needed devices will be opened and timeout
 *      to set motor current back to 'stop scale' is started.
 *      If a error occurs all devices will be closed again and module will stay
 *      disabled.
 *      If all went error free the module state will either go to SM_STATE_INIT
 *      to force a reference run or to normal operation state (SM_STATE_IDLE).
 *      Which state is entered depends from input parameter 'SkipRefRun'.
 *
 *  \iparam  Data       = Module instance data pointer
 *  \iparam  SkipRefRun = if false a reference run is needed
 *                        if true reference run is skipped
 *
 *  \return  NO_ERROR or error code
 *
 ******************************************************************************/
Error_t smEnable(smData_t *Data, Bool SkipRefRun)
{
    Error_t RetCode  = NO_ERROR;

    if ((RetCode = smConfigIsValid(Data)) < 0) {    // check if configuration data is complete and valid
        return RetCode;
    }
    
    if ((RetCode = smOpenDevices(Data)) < 0) {      // open hal devices, this will also switch on driver stage and init step value to zero
        return RetCode;
    }
    
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
    // set position code value to undefined
    Data->LimitSwitches.PosCode.Value = POSCODE_UNDEFINED;

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


/******************************************************************************/
/*! 
 *  \brief   Initialize timer
 *
 *      All instances uses the same timer to create correct interval times for
 *      step timing. This timer is initialized once and is running endless.
 *      Timer device is opened and timer mode and prescaler are set to run with
 *      83.33 ns cycle time.
 *
 *  \return  NO_ERROR or error code
 *
 ******************************************************************************/
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
 *  \brief   Initalize all data members of one stepper instance
 *
 *      Complete instance data is reseted.
 *
 *  \iparam  Data       = Module instance data pointer
 *
 ******************************************************************************/
void smInitInstance (smData_t *Data)
{
    Data->Flags.Enable = FALSE;
    Data->Flags.Shutdown = FALSE;
    Data->Flags.Stopped = FALSE;
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
    Data->OffLimit = FALSE;
}


/******************************************************************************/
/*! 
 *  \brief   Initializes this function module
 *
 *      Initializes this function module by registering it to the task 
 *      scheduler and registers all handler functions to the CAN message 
 *      dispatcher. Furthermore, it allocates and initializes memory for 
 *      the variables for all module instances. In addition the common
 *      timer device and each instance of the module are initialized.
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
    if ((RetCode = bmRegisterModule(ModuleID, Instances, &Interface)) < 0) {
        return RetCode;
    }
    
    //smRegisterDebugNames(ModuleID);

    // init and open timer device
    if ((RetCode = smInitTimer()) < 0) {
        return RetCode;
    }
        
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
