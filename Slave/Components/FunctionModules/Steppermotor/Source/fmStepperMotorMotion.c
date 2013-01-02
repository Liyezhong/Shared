/****************************************************************************/
/*! \file
 * 
 *  \brief Motion setup/control functions of module 'stepper motor'
 *
 *  $Version: $ 0.1
 *  $Date:    $ 04.07.2012
 *  $Author:  $ Rainer Boehles
 *
 *  \b Description:
 *
 *      This file contains the functions for the motion setup/ 
 *      control of the 'stepper motor' module.
 *       
 *      Motion data is calculated and setup to enable start of
 *         - Position or
 *         - Speed
 *      movement. After starting the motion all further motion
 *      control is done in the motion ISR.
 *
 *      A movement is defined by a parameter set including
 *      parameters for each phase.
 *      A movement can cover all phases from phase 0 to phase 8
 *      (e.g. position movement), or just a part of the phases
 *      (e.g. spin up motor from stand still to target speed uses
 *      phase 0 to 4).
 *
 *      For more complex motions multiple movements can be combined.
 *      e.g if motor is runing with a positive target speed and speed  
 *      should change to negative target speed, then first movement
 *      will stop motor and second movement spin motor up again in 
 *      opposite direction. This is accomplished by preparing a 
 *      parameter set for each movement and then switch from first
 *      parameter set to second set while motion is still active.
 *
 *      Special parameter values are used to control single phase.
 *      If phase parameter for phase duration is set to 
 *          0, then phase is skipped
 *         -1, then phase is not left
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

#include "stdlib.h"

#ifndef SIMULATION
#include "global.h"
#include "halcommon.h"
#else
#include "Global.h"
#define E_COMMAND_REJECTED -1
#define E_PARAMETER_OUT_OF_RANGE -1
#define E_SMOT_MOTION_PROFILE_UNFIT -1
#define E_SMOT_STOP_BY_OFFLIMIT -1
#endif

#include "fmStepperMotor.h"
#include "fmStepperMotorMotionCalc.h"
#include "fmStepperMotorMotionISR.h"


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/
Error_t smStopRotationAtPositionRequest (smData_t *Data, Int32 StopPosition, Int8 ProfileIndex);


/******************************************************************************/
/*! 
 *  \brief  Initialize motion data
 *
 *      Motion data is initialized.
 * 
 *  \iparam  Motion   = pointer to motion data
 *  \iparam  Instance = unit number of capture compare register
 * 
 ******************************************************************************/
void smInitMotion (Motion_t *Motion, UInt16 Instance)
{
    Motion->CCR.Unit = Instance;

    Motion->ActSet = 0;
    Motion->NewSet = 0;

    Motion->State = MS_IDLE;
    Motion->AtTargetPosition = FALSE;
    Motion->AtTargetSpeed = FALSE;
    Motion->Pos = 0;
    Motion->ResetPos = 0;
    Motion->SinIndex = 0;

    Motion->Stop = SM_SC_ALWAYS;

    Motion->LSTrigger = FALSE;
}


#ifndef SIMULATION

/*****************************************************************************/
/*!
 *  \brief   Start motor movement
 *
 *      Motor current is set to configured 'run scale' value.
 *      Stop conditions are reset and encoder is prepared.
 *      Capture compare register is setup to trigger the initial
 *      interrupt in approximately 0,5 ms.
 *      By enabling CCR interrupt the motion control is ready to perform and
 *      control movement from inside ISR with occurence of first interrupt.
 *
 *  \iparam  Data = Pointer to module instance's data
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/
Error_t smStartMotion(smData_t *Data)
{
    Error_t RetCode;
    UInt32  Counter;

#ifdef STEPPER_TRACE_ISR_TIMING
    ClearTiming();
#endif

    // lock the stepper bus until movement is finished
    if ((RetCode = halStepperControl(Data->Motor.Handle, STP_CTRL_LOCK_BUS, TRUE)) < 0) {
        return RetCode;
    }
    
    // use run current while moving
    if ((RetCode = halStepperCurrent(Data->Motor.Handle, Data->Motor.Config.RunCS)) < 0) {
        return RetCode;
    }

    Data->Motion.Stop = SM_SC_NONE;     // reset stop condition
    Data->Encoder.AtSpeedZero = FALSE;  // reset flag for target speed 0

    // reset step loss variables
    Data->Encoder.StepLoss.Detected = FALSE;
    Data->Encoder.StepLoss.Count = 0;
//    Data->Motion.AccLostSteps = 0;
    Data->Encoder.StepLoss.LastEncPos = Data->Encoder.Pos;
    Data->Encoder.StepLoss.PosOverflow = FALSE;
    Data->Encoder.StepLoss.EncPosOverflow = FALSE;
    Data->Encoder.StepLoss.EncXNeg = FALSE;
    Data->Encoder.FirstTime = TRUE;  // next timer interrupt will be the first one for this movement

    // read the timer value, calculate CCR value for next CCR irq, and and store CCR value
    if ((RetCode = halTimerRead(smHandleTimer, TIM_REG_COUNTER, &Counter)) < 0) {
        return RetCode;
    }
    Data->Motion.CCR.LastValue = (UInt16) (Counter &0x0000ffff);
    // set duration to next capture compare interrupt (with 72Mhz and Prescaler 6 the time for 1 tick is ~83ns)
    // next capture compare interrupt will initiate the first step
    Data->Motion.CCR.LastValue += 6000;   // ~ 500 usec   
    halCapComWrite(smHandleTimer, Data->Motion.CCR.Unit, Data->Motion.CCR.LastValue);
    
    // enable the CaptureCompare Irq
    return halCapComControl(smHandleTimer, Data->Motion.CCR.Unit, TIM_INTR_ENABLE);
}


/*****************************************************************************/
/*!
 *  \brief   Stop motor movement
 *
 *      This function is called from main module task when motor is already
 *      stopped from inside ISR by disabling CCR interrupt. Final stuff after
 *      motor have stopped is done here to reduce ISR run time.
 *
 *      Timeout is started to apply configured 'stop scale' value after timeout
 *      to motor current. Expiration of timeout is checked by the module task.
 *
 *      CAN events are send to master if special condition cause the motor stop.
 *
 *  \iparam  Instance = Module instance number
 *
 *****************************************************************************/
void smStopMotion(UInt16 Instance)
{
    smData_t     *Data     = &smDataTable[Instance];
    smEncoder_t  *Encoder  = &Data->Encoder;
    smStepLoss_t *StepLoss = &Encoder->StepLoss;

    // unlock the stepper bus
    halStepperControl(Data->Motor.Handle, STP_CTRL_LOCK_BUS, FALSE);

    // start timeout to reduce motor current to stop current value
    Data->Motor.StopCSTimeout = Data->Motor.Config.StopCSDelay;
    Data->Motor.StopCSTime = bmGetTime();

    // if motor stopped because of position code
    if ((SM_SC_DIR_CCW == Data->Motion.Stop) || (SM_SC_DIR_CW == Data->Motion.Stop)) {
        bmSignalEvent(Data->Channel, E_SMOT_STOP_BY_POSCODE, TRUE, 0);
    }

    // if motor stopped because position get's off-limit
    if (SM_SC_OFFLIMIT == Data->Motion.Stop) {
        bmSignalEvent(Data->Channel, E_SMOT_STOP_BY_OFFLIMIT, TRUE, 0);
    }

    // if step deviations occured
    if (  (Encoder->Config.Exists) && (0 != StepLoss->Config.ErrorLimit)) {
        if (StepLoss->Count > StepLoss->Config.ErrorLimit) {
            bmSignalEvent(smDataTable[Instance].Channel, E_SMOT_STOP_BY_STEPLOSS, TRUE, StepLoss->Count);
        }
        else {
            if (StepLoss->Count > StepLoss->Config.WarnLimit) {
                bmSignalEvent(smDataTable[Instance].Channel, W_SMOT_STEPLOSS, TRUE, StepLoss->Count);
            }
        }
        if (  (abs(StepLoss->StepDiff) > Data->Motor.Config.Resolution)
            ||(abs(StepLoss->StepDiff) > StepLoss->Config.ErrorLimit)) {
            bmSignalEvent(smDataTable[Instance].Channel, E_SMOT_STOP_BY_STEPDIFF, TRUE, StepLoss->StepDiff);
        }
        else {
            if (-StepLoss->StepDiff > StepLoss->Config.WarnLimit) {
                bmSignalEvent(smDataTable[Instance].Channel, W_SMOT_STEPDIFF, TRUE, StepLoss->StepDiff);
            }
        }
    }

}

#endif


/*****************************************************************************/
/*!
 *  \brief   Determine next phase to use
 *
 *      From supplied phase the next phase to use is determined and returned
 *      to caller. A phase is skipped if phase duration is set to 0.
 *
 *  \iparam  Param           = Pointer to one phase of parameter set
 *  \iparam  Phase           = phase number (can be 0 to 8)
 *
 *  \return  next phase
 *
 *****************************************************************************/
smPhase_t smNextPhase(smPhaseParam_t *Param, Int8 Phase)
{
    while (Phase < PH_8_END)
    {
        Phase++;
        if (0 != Param[(smPhase_t)Phase].dt) {
            break;
        }
    }
    return (smPhase_t)Phase;
}


/*****************************************************************************/
/*!
 *  \brief   Determine rotation direction
 *
 *      According to sign of desired distance / speed and the configuration
 *      of reference framework, the rotation dir for motor is determined.
 *      Also it's determined if position counter have to be incremented or
 *      decremented for the supplied parameter set.
 *
 *  \iparam  InversePosCount = count dir enforced by reference framework
 *  \iparam  Param           = Pointer to parameter set
 *  \iparam  Value           = signed distance / speed value
 *
 *  \return  absolute value of function parameter 'Value'
 *
 *****************************************************************************/
Int32 smCheckDirection (Bool InversePosCount, smParamSet_t *Param, Int32 Value)
{
    if (Value < 0)
    {
        Param->NegPosCount = TRUE;      // position counter is decremented
        if (FALSE == InversePosCount) {
            Param->CCWRotDir = TRUE;    // motor should rotate CCW
        }
        else {
            Param->CCWRotDir = FALSE;   // motor should rotate CW
        }
        return -Value;
    }
    else
    {
        Param->NegPosCount = FALSE;     // position counter is incremented
        if (TRUE == InversePosCount) {
            Param->CCWRotDir = TRUE;    // motor should rotate CCW
        }
        else {
            Param->CCWRotDir = FALSE;   // motor should rotate CW
        }
        return Value;
    }
}


/*****************************************************************************/
/*!
 *  \brief   Reset data from one phase of parameter set
 *
 *      All members of one phase of the parameter set are reseted to zero.
 *
 *  \iparam  Param = Pointer to one phase of parameter set
 *
 *****************************************************************************/
void smResetSinglePhaseParam (smPhaseParam_t *Param)
{
    Param->s = 0;
    Param->ds = 0;
    Param->v = 0;
    Param->vE = 0;
    Param->a = 0;
#ifdef TRACE
    Param->aE = 0;
#endif
    Param->j = 0;
    Param->dt = 0;
}


/*****************************************************************************/
/*!
 *  \brief   Setup one phase of parameter set from calculated phase data
 *
 *      Result from phase data calculation is used to setup one phase of
 *      the parameter set by selecting the essential values and perfom unit
 *      conversion.
 *
 *  \iparam  PhaseData  = pointer to phase data
 *  \iparam  Param      = Pointer to one phase of parameter set
 *
 *****************************************************************************/
void smSetSinglePhaseParam (smPhaseParam_t *Param, smPhaseData_t *PhaseData)
{
    Param->s = PhaseData->s * ISR_MICROSTEPS_PER_HALFSTEP / SM_PROFILE_MICROSTEPS_PER_HALFSTEP;
    Param->ds = PhaseData->ds * ISR_MICROSTEPS_PER_HALFSTEP / SM_PROFILE_MICROSTEPS_PER_HALFSTEP;
    Param->v = PhaseData->v * ISR_MICROSTEPS_PER_HALFSTEP / SM_PROFILE_MICROSTEPS_PER_HALFSTEP;
    Param->vE = (PhaseData->v + PhaseData->dv) * ISR_MICROSTEPS_PER_HALFSTEP / SM_PROFILE_MICROSTEPS_PER_HALFSTEP;
    Param->a = PhaseData->a * ISR_MICROSTEPS_PER_HALFSTEP / SM_PROFILE_MICROSTEPS_PER_HALFSTEP;
#ifdef TRACE
    Param->aE = (PhaseData->a + PhaseData->da) * ISR_MICROSTEPS_PER_HALFSTEP / SM_PROFILE_MICROSTEPS_PER_HALFSTEP;
#endif
    if (PhaseData->dt) {
        Param->j = (Int64)SM_PROFILE_TIMEBASE * PhaseData->da / PhaseData->dt;
    }
    else {
        Param->j = 0;
    }
    Param->j *= ISR_MICROSTEPS_PER_HALFSTEP / SM_PROFILE_MICROSTEPS_PER_HALFSTEP;
    Param->dt = PhaseData->dt;
}


/*****************************************************************************/
/*!
 *  \brief   Setup parameter set from precalculated speed/jerk values
 *
 *      Parameter set to travel target distance is setup according to the
 *      selected profile. To speed up parameter calculation first a suitable
 *      precalculated value set for speed/jerk is selected, which are known to
 *      fit to the desired travel distance. If no suitable precalculated set is
 *      found the function returns with an error.
 *
 *      Next the found value set is used to calculate data for each phase
 *      which will match the exact travel distance. If no valid solution is
 *      possible the function returns with an error.
 *
 *      At last the calculated phase data is used to create the parameter set
 *
 *  \iparam  Distance   = target distance (in half-step)
 *  \iparam  Profile    = pointer to motion profile
 *  \iparam  Param      = Pointer to new parameter set
 *
 *  \return  0 for success, -1 if failed
 *
 *****************************************************************************/
Int8 smSetupSCurveParam (Int32 Distance, smProfile_t *Profile, smPhaseParam_t *Param)
{
    Int8 i, j;
    Int8 Found;
    Int8 ph;
    smProfileConfig_t   Config;                             // motion profile used for precalculation
    smPhaseData_t       PhaseData[NUM_OF_MOTION_PHASES];    // calculated values for each phase of the profile

    Distance *= SM_PROFILE_MICROSTEPS_PER_HALFSTEP;

    Found = FALSE;
    for (i=10; i>0; i--)
    {
        if (Profile->Distance[i-1].s < 0) {
            break;
        }
        if (Profile->Distance[i-1].s <= Distance)
        {
            Found = TRUE;
            break;
        }
    }
    if (FALSE == Found) {
        return -1;
    }

    Config = Profile->Config;
    Config.vMax = Config.vMax * i / 10;
    j = Profile->Distance[i-1].j;
    Config.acc          = Profile->Config.acc * j / 10;
    Config.accJUpT      = Profile->Config.accJUpT * j / 10;
    Config.accJDownT    = Profile->Config.accJDownT * j / 10;
    Config.dec          = Profile->Config.dec * j / 10;
    Config.decJUpT      = Profile->Config.decJUpT * j / 10;
    Config.decJDownT    = Profile->Config.decJDownT * j / 10;

    if (smCalcMotionProfilePhaseData(Distance - Profile->Distance[i-1].s, &Config, PhaseData) < 0) {
        return -1;
    }

    for (ph = PH_0_START; ph < NUM_OF_MOTION_PHASES; ph++)
    {
        smSetSinglePhaseParam (&Param[ph], &PhaseData[ph]) ;
    }

    return 0;
}


/*****************************************************************************/
/*!
 *  \brief   Setup parameter set to move target distance
 *
 *      Parameter set to travel target distance is setup according to the
 *      selected profile.
 *
 *      If travel distance is too small to use s-curve trajection then only
 *      Phase 8 will be used, which means motor will travel the whole distance
 *      with profiles min speed. Otherwise all phases are used.
 *
 *  \iparam  Distance   = target distance (in half-step)
 *  \iparam  Profile    = pointer to motion profile
 *  \iparam  Param      = Pointer to new parameter set
 *
 *****************************************************************************/
void smSetupMoveParam (Int32 Distance, smProfile_t *Profile, smParamSet_t *Param)
{
    Int8 ph;

    if (smSetupSCurveParam(Distance, Profile, Param->Ph) < 0)
    {
        // s-curve motion not possible. drive whole distance with constant speed
        for (ph = PH_0_START; ph < NUM_OF_MOTION_PHASES; ph++)
        {
            if (PH_8_END != ph) {
                smResetSinglePhaseParam (&Param->Ph[ph]);
            }
            else
            {
                Param->Ph[ph].s    = 0;
                Param->Ph[ph].ds   = Distance * ISR_MICROSTEPS_PER_HALFSTEP / SM_PROFILE_MICROSTEPS_PER_HALFSTEP;
                Param->Ph[ph].v    = Profile->Config.vMin * ISR_MICROSTEPS_PER_HALFSTEP / SM_PROFILE_MICROSTEPS_PER_HALFSTEP;
                Param->Ph[ph].vE   = Param->Ph[ph].v;
                Param->Ph[ph].a    = 0;
#ifdef TRACE
                Param->Ph[ph].aE   = 0;
#endif
                Param->Ph[ph].j    = 0;
                Param->Ph[ph].dt   = SM_PROFILE_TIMEBASE * Param->Ph[ph].ds / Param->Ph[ph].v;
            }
        }
    }
}


/*****************************************************************************/
/*!
 *  \brief   Request to start movement to target position
 *
 *      Motor is currently standing still and should now move to supplied
 *      target position. To perform the movement the motion parameters
 *      are setup according to the selected profile. 
 *
 *      Before motion is started it is checked if motor can move into desired
 *      direction, e.g. if motor is standing at a limit switch the configuration
 *      may only allow rotation in certain direction.
 *
 *
 *  \iparam  Data           = Pointer to module instance's data
 *  \iparam  Position       = new target speed (in half-step)
 *  \iparam  ProfileIndex   = index of motion profile
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/
Error_t smNewPositionRequest (smData_t *Data, Int32 Position, Int8 ProfileIndex)
{
    Error_t RetCode = NO_ERROR;

    Int32 Distance;

    smParamSet_t        *PtrParam;
    smProfile_t         *Profile;

    Data->Motion.ActSet = 0;
    Data->Motion.NewSet = 0;

    PtrParam = &Data->Motion.Param[Data->Motion.ActSet];
    Profile = &Data->Profiles.Set[ProfileIndex];

    Distance = Position - Data->Motion.Pos;
    // don't start movement if requested position is same as actual postion
    if (0 == Distance) {
        Data->Motion.AtTargetPosition = TRUE;
        return NO_ERROR;
    }

    Distance = smCheckDirection(Data->Motor.InversePosCount, PtrParam, Distance); 
    PtrParam->dSteps = Profile->StepWidth;

    // check if we can move into the requested direction
#ifndef SIMULATION
    if ((RetCode = smRotDirIsAllowed(PtrParam->CCWRotDir, &Data->LimitSwitches)) < 0) {
        return RetCode;
    }
#endif

    smSetupMoveParam(Distance, Profile, PtrParam);
    PtrParam->OffsetPos = Data->Motion.Pos;

    // determine start phase 
    Data->Motion.Phase = PH_0_START;
    if (0 == Data->Motion.Param[Data->Motion.ActSet].Ph[Data->Motion.Phase].dt) {
        Data->Motion.Phase = smNextPhase(Data->Motion.Param[Data->Motion.ActSet].Ph, Data->Motion.Phase);
    }
    PtrParam->OffsetPhase = Data->Motion.Phase;

    Data->Motion.s = 0;
    Data->Motion.v0 = PtrParam->Ph[Data->Motion.Phase].v;
    Data->Motion.a0 = PtrParam->Ph[Data->Motion.Phase].a;
    Data->Motion.j = PtrParam->Ph[Data->Motion.Phase].j;
    Data->Motion.t = 0;

    Data->Motion.Nominal.s = 0;
    Data->Motion.Nominal.v = Data->Motion.v0;
#ifdef TRACE
    Data->Motion.Nominal.a = Data->Motion.a0;
    Data->Motion.Nominal.t = 0;
#endif

    if (0 == Data->Motor.FrameworkConfig.ResetPosition) {
        Data->Encoder.TargetPos = Position;
    }
    else
    {
        Data->Encoder.TargetPos = Position % Data->Motor.FrameworkConfig.ResetPosition;
        if (Data->Encoder.TargetPos < 0) {
            Data->Encoder.TargetPos += Data->Motor.FrameworkConfig.ResetPosition;
        }
    }
    Data->Motion.AtTargetPosition = FALSE;
    Data->Motion.State = MS_POSITION;

#ifdef SIMULATION
    return 0;
#else
    if ((RetCode = smStartMotion(Data)) < 0)
    {
        Data->Motion.State = MS_IDLE;
        smStopMotion (Data->Instance);
    }
    return RetCode;
#endif
}


/*****************************************************************************/
/*!
 *  \brief   Request to move to target position
 *
 *      Motor should move to supplied target position. To perform the position
 *      change the motion parameters are setup according to the selected profile.
 *
 *      Position change is only allowed if motor is standing still or
 *      a rotatory movement with target speed is active.
 *      In all other situations the request is rejected.
 *
 *
 *  \iparam  Instance       = Instance number of this module
 *  \iparam  Position       = new target position (in half-step)
 *  \iparam  ProfileIndex   = index of motion profile
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/
Error_t smPositionRequest (UInt16 Instance, Int32 Position, UInt8 ProfileIndex)
{
    smData_t *Data = &smDataTable[Instance];

    Int32   AbsSpeed;               // absolute value of current speed
    Error_t RetCode = NO_ERROR;

    // check that signals from limit switches are ok
#ifndef SIMULATION
    if ((RetCode = smPosCodeIsValid(&Data->LimitSwitches, Data->LimitSwitches.PosCode.Value)) < 0) {
        return RetCode;
    }
#endif
    switch (Data->Motion.State)
    {
// motor is standing still and now we want to move to target position
    case MS_IDLE:
        RetCode = smNewPositionRequest (Data, Position, ProfileIndex);
        break;
// move to new target position while position run is in progress. NOT SUPPORTED YET
    case MS_POSITION:
        RetCode = E_COMMAND_REJECTED;
        break;
// stop at target position while speed run is already in progress
    case MS_SPEED:
        if (0 != Data->Motor.FrameworkConfig.ResetPosition)     // only supported for rotatory movement
        {
            // check if current speed is within the profiles speed limits
            if (Data->Motion.Param[Data->Motion.ActSet].Speed < 0) {
                AbsSpeed = -Data->Motion.Param[Data->Motion.ActSet].Speed;
            }
            else {
                AbsSpeed = Data->Motion.Param[Data->Motion.ActSet].Speed;
            }
        
            if (AbsSpeed < Data->Profiles.Set[ProfileIndex].Config.vMin / SM_PROFILE_MICROSTEPS_PER_HALFSTEP) {
                return E_SMOT_MOTION_PROFILE_UNFIT;
            }
        
            if (AbsSpeed > Data->Profiles.Set[ProfileIndex].Config.vMax / SM_PROFILE_MICROSTEPS_PER_HALFSTEP) {
                return E_SMOT_MOTION_PROFILE_UNFIT;
            }

            RetCode = smStopRotationAtPositionRequest (Data, Position, ProfileIndex);
        }
        else {
            RetCode = E_COMMAND_REJECTED;
        }
        break;

// unknown motion state
    default:
        RetCode = E_COMMAND_REJECTED;
        break;
    }

    return RetCode;
}


/*****************************************************************************/
/*!
 *  \brief   Setup parameter set to change target speed
 *
 *      Parameter set to change motor speed from start speed to end speed is
 *      setup according to the selected profile.
 *
 *      If end speed is greater then start speed motor must accelerate. In
 *      this case the parameter set will use Phase 5 to Phase 8.
 *
 *      If start speed is greater then end speed motor must decelerate. In
 *      this case the parameter set will use Phase 1 to Phase 4.
 *
 *      If speed difference is very small the jerk phases can't be used. In
 *      this case only Phase 2 or Phase 6 will be used.
 *
 *  \iparam  StartSpeed = actual target speed (in half-step/s)
 *  \iparam  EndSpeed   = new target speed (in half-step/s)
 *  \iparam  Profile    = pointer to motion profile
 *  \iparam  Param      = Pointer to new parameter set
 *
 *  \return  amount of half-steps needed to perform the speed change
 *
 *****************************************************************************/
UInt32 smSetupSpeedParam (Int32 StartSpeed, Int32 EndSpeed, smProfile_t *Profile, smPhaseParam_t *Param)
{
    Int8 ph;
    smProfileConfig_t   Config;                             // motion profile configuration used for precalculation
    smPhaseData_t       PhaseData[NUM_OF_MOTION_PHASES];    // calculated values for each phase of the profile
    Int32 Distance;

    Config = Profile->Config;
    if (StartSpeed < EndSpeed)
    {
        PhaseData[PH_0_START].s = 0;
        PhaseData[PH_0_START].v = StartSpeed * SM_PROFILE_MICROSTEPS_PER_HALFSTEP;
        PhaseData[PH_0_START].a = 0;
        PhaseData[PH_0_START].t = 0;
        Config.vMax = EndSpeed * SM_PROFILE_MICROSTEPS_PER_HALFSTEP;
        Distance = smCalcAccPhaseData(1 * SM_PROFILE_MICROSTEPS_PER_HALFSTEP, &Config, PhaseData, TRUE);
        if (Distance < 0)
        {
            // we can only perform const acceleration
            Distance = smCalcAccPhaseData(1 * SM_PROFILE_MICROSTEPS_PER_HALFSTEP, &Config, PhaseData, FALSE);
        }
    }
    else
    {
        PhaseData[PH_5_DEC_JERK_UP].s = 0;
        PhaseData[PH_5_DEC_JERK_UP].v = StartSpeed * SM_PROFILE_MICROSTEPS_PER_HALFSTEP;
        PhaseData[PH_5_DEC_JERK_UP].a = 0;
        PhaseData[PH_5_DEC_JERK_UP].t = 0;
        if (EndSpeed * SM_PROFILE_MICROSTEPS_PER_HALFSTEP > Config.vMin) {
            Config.vMin = EndSpeed * SM_PROFILE_MICROSTEPS_PER_HALFSTEP;
        }
        Distance = smCalcDecPhaseData(&Config, PhaseData, TRUE);
        if (Distance < 0)
        {
            // we can only perform const deceleration
            Distance = smCalcDecPhaseData(&Config, PhaseData, FALSE);
        }
    }


    for (ph = PH_0_START; ph < PH_5_DEC_JERK_UP; ph++)
    {
        if (StartSpeed < EndSpeed) {
            smSetSinglePhaseParam (&Param[ph], &PhaseData[ph]);
        }
        else {
            smResetSinglePhaseParam (&Param[ph]);
        }
    }

    for (ph = PH_5_DEC_JERK_UP; ph < NUM_OF_MOTION_PHASES; ph++)
    {
        if (StartSpeed < EndSpeed) {
            smResetSinglePhaseParam (&Param[ph]);
        }
        else {
            smSetSinglePhaseParam (&Param[ph], &PhaseData[ph]);
        }
    }

    if (0 != EndSpeed)
    {
        if (StartSpeed < EndSpeed) {
            Param[PH_4_VEL_CONST].dt = -1;
        }
        else {
            Param[PH_8_END].dt = -1;
        }
    }

    return Distance;
}


/*****************************************************************************/
/*!
 *  \brief   Setup new parameter set to change target speed
 *
 *      Motor is currently moving with constant speed. Speed should now change
 *      to supplied target speed. To perform the speed change a new parameter
 *      set is setup according to the selected profile. Start speed is taken
 *      from active Phase (either Phase 4 or Phase 8 of actual parameter set).
 *
 *      Trigger for parameter set switch is setup and the amount of half-steps
 *      needed to perform the speed change is returned to caller.
 *
 *  \iparam  Speed      = new target speed (in half-step/s)
 *  \iparam  Profile    = pointer to motion profile
 *  \iparam  Param      = Pointer to actual parameter set
 *  \iparam  NewParam   = Pointer to new parameter set
 *
 *  \return  amount of half-steps needed to perform the speed change
 *
 *****************************************************************************/
UInt32 smSetupNewSpeedParamSet (Int32 Speed, smProfile_t *Profile, smParamSet_t *Param, smParamSet_t *NewParam)
{
    Int32 StartSpeed;
    Int32 Distance;

    NewParam->dSteps = Profile->StepWidth;

    if (-1 == Param->Ph[PH_4_VEL_CONST].dt)
    {
        StartSpeed = Param->Ph[PH_4_VEL_CONST].v;
        Param->SwitchSet.Trigger.OldPhase = PH_4_VEL_CONST;
    }
    else
    {
        StartSpeed = Param->Ph[PH_8_END].v;
        Param->SwitchSet.Trigger.OldPhase = PH_8_END;
    }

    StartSpeed = StartSpeed / ISR_MICROSTEPS_PER_HALFSTEP;
    Distance = smSetupSpeedParam (StartSpeed, Speed, Profile, NewParam->Ph);

    Param->SwitchSet.Trigger.Position = -1;             // don't care position
    Param->SwitchSet.NewPhase = PH_1_ACC_JERK_UP;       // new phase to use
//  Param->SwitchSet.NewPhase = PH_0_START;             // new phase to use
    if (0 == NewParam->Ph[Param->SwitchSet.NewPhase].dt) {
        Param->SwitchSet.NewPhase = smNextPhase(NewParam->Ph, Param->SwitchSet.NewPhase);
    }

    return Distance;
}


/*****************************************************************************/
/*!
 *  \brief   Request to stop a lasting target speed movement at target position
 *
 *      Motor is currently running with certain target speed and should now
 *      stop at supplied target position. To perform the speed deceleration
 *      the motion parameters are setup according to the selected profile. 
 *
 *      This request is only supported for rotatory movements, which means a
 *      reset-position is configured and position count will always be in
 *      between 0 and the reset-position.
 *
 *      Rotation direction is always kept same as lasting one. The motor have
 *      to decelearate to stop. For deceleration a new parameter set for
 *      phase 4 to phase 8 is calculated. From known step count for deceleration
 *      the exact trigger point is calculated to stop at target position.
 *      Then a parameter set switch is initiated, which is handled by the ISR.
 *
 *
 *  \iparam  Data           = Pointer to module instance's data
 *  \iparam  StopPosition   = target position (in half-step)
 *  \iparam  ProfileIndex   = index of motion profile
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/
Error_t smStopRotationAtPositionRequest (smData_t *Data, Int32 StopPosition, Int8 ProfileIndex)
{
    Error_t         RetCode = NO_ERROR;

    UInt32          StopDistance;   // half-steps needed to spin down from act. speed till stop of motor
    Int32           ResetPosition = Data->Motor.FrameworkConfig.ResetPosition;
    UInt64          Offset;
    Bool            InverseDirection;

    Int8            NewSet;
    smParamSet_t    *ActParam;
    smParamSet_t    *NewParam;

    smProfile_t     *Profile = &Data->Profiles.Set[ProfileIndex];

// check preconditions
    if ((StopPosition < 0) || (StopPosition >= ResetPosition)) {
        return E_COMMAND_REJECTED;      // stop position have to be inside position value range of one rotation
    }

    if (Data->Motion.ActSet != Data->Motion.NewSet) {
        return E_COMMAND_REJECTED;      // a parameter switch is already pending. no further allowed.
    }

    //if ((Data->Motion.actSet+1) >= NUM_OF_PARAMETERSETS)
    //  return -1;

    Data->Encoder.TargetPos = StopPosition;

// prepare new parameter set
    ActParam = &Data->Motion.Param[Data->Motion.ActSet];    // used to access actual parameter set
    NewSet = Data->Motion.ActSet + 1;
    if (SM_NUM_OF_PARAMETERSETS == NewSet) {
        NewSet = 0;
    }
    NewParam = &Data->Motion.Param[NewSet];                 // used to set up new parameter set

    StopPosition = smCheckDirection(Data->Motor.InversePosCount, NewParam, StopPosition); 
    NewParam->dSteps = Profile->StepWidth;

    // check if we can move into the requested direction
#ifndef SIMULATION
    if ((RetCode = smRotDirIsAllowed(NewParam->CCWRotDir, &Data->LimitSwitches)) < 0) {
        return RetCode;
    }
#endif

    InverseDirection = (ActParam->NegPosCount != NewParam->NegPosCount);
    NewParam->NegPosCount = ActParam->NegPosCount;  // keep position count flag
    NewParam->CCWRotDir = ActParam->CCWRotDir;      // keep rotation dir

// calculate decelaration phases and get step count for deceleration
    StopDistance = smSetupNewSpeedParamSet(0, Profile, ActParam, NewParam);     // spin down to min speed and stop

// calculate trigger position
    if (-1 == ActParam->Ph[PH_4_VEL_CONST].dt) {
        Offset = ActParam->Ph[PH_4_VEL_CONST].s;
    }
    else {
        Offset = ActParam->Ph[PH_8_END].s;
    }
    Offset -= ActParam->Ph[ActParam->OffsetPhase].s;
    Offset /= ISR_MICROSTEPS_PER_HALFSTEP;

    if (!InverseDirection)
    {
        StopPosition -= StopDistance;   // now stop position is the trigger position in hs
        Offset += ActParam->OffsetPos;
        StopPosition -= Offset;
    }
    else
    {
        StopPosition += StopDistance;   // now stop position is the trigger position in hs
        Offset -= ActParam->OffsetPos;
        StopPosition = ResetPosition - StopPosition - Offset;
    }
    StopPosition = StopPosition % ResetPosition;
    if (StopPosition < 0) {
        StopPosition += ResetPosition;
    }

    ActParam->SwitchSet.Trigger.Position = StopPosition;
    ActParam->SwitchSet.Trigger.Position *= ISR_MICROSTEPS_PER_HALFSTEP;

// init. movement to stop position
    Data->Motion.AtTargetPosition = FALSE;
    Data->Motion.State = MS_POSITION;

    Data->Motion.NewSet = NewSet;       // the new parameter set to use

    return RetCode;
}


/*****************************************************************************/
/*!
 *  \brief   Request to change target speed of lasting target speed movement
 *
 *      Motor is currently running with certain target speed and should now
 *      rotate with supplied new target speed. To perform the speed acceleration
 *      the motion parameters are setup according to the selected profile. 
 *
 *      If new rotation direction is same as lasting one the motor have to 
 *      accelerate or decelearate to reach new target speed. For acceleration
 *      a new parameter set for phase 0 to phase 4 is calculated. For 
 *      deceleration a new parameter set for phase 4 to phase 8 is calculated.
 *      Then a parameter set switch is initiated, which is handled by the ISR.
 *
 *      If new rotation direction is oposite to lasting one the motor first have 
 *      to stop, which means deceleration to zero and then have to accelerate 
 *      to new target speed. Two new parameter sets need to be calculated before
 *      parameter set switch is initiated.
 *
 *
 *  \iparam  Data           = Pointer to module instance's data
 *  \iparam  Speed          = new target speed (in half-step/s)
 *  \iparam  ProfileIndex   = index of motion profile
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/
Error_t smChangeSpeedRequest (smData_t *Data, Int32 Speed, Int8 ProfileIndex)
{
    Error_t RetCode = NO_ERROR;

    Int8 NewSet1;
    Int8 NewSet2;

    smParamSet_t *ActParam;
    smParamSet_t *NewParam;
    smParamSet_t *NewParam2;

    Bool InverseDirection;

    if (Data->Motion.ActSet != Data->Motion.NewSet) {
        return E_COMMAND_REJECTED;      // a parameter switch is already pending. no further allowed.
    }

    //if ((Data->Motion.actSet+1) >= NUM_OF_PARAMETERSETS)
    //  return -1;

    ActParam = &Data->Motion.Param[Data->Motion.ActSet];        // actual parameter set
    NewSet1 = Data->Motion.ActSet + 1;
    if (SM_NUM_OF_PARAMETERSETS == NewSet1) {
        NewSet1 = 0;
    }
    NewParam = &Data->Motion.Param[NewSet1];                    // used to set up new parameter set

    Speed = smCheckDirection(Data->Motor.InversePosCount, NewParam, Speed);

    // check if we can move into the requested direction
#ifndef SIMULATION
    if ((RetCode = smRotDirIsAllowed(NewParam->CCWRotDir, &Data->LimitSwitches)) < 0) {
        return RetCode;
    }
#endif

    InverseDirection = (ActParam->NegPosCount != NewParam->NegPosCount);

// if direction is opposite we first have to stop the movement
    if ((0 == Speed) || (InverseDirection))             // stop motor rotation
    {
        NewParam->NegPosCount = ActParam->NegPosCount;  // keep position count flag
        NewParam->CCWRotDir = ActParam->CCWRotDir;      // keep rotation dir
        smSetupNewSpeedParamSet(0, &Data->Profiles.Set[ProfileIndex], ActParam, NewParam);      // spin down to min speed
    }
    else {
        smSetupNewSpeedParamSet(Speed, &Data->Profiles.Set[ProfileIndex], ActParam, NewParam);  // change to new speed
    }

    if ((0 == Speed) || (!InverseDirection))
    {
        Data->Motion.NewSet = NewSet1;      // the new parameter set to use
    }
// if direction is opposite we now have to restart with new rotation dir.
    else
    {
        NewSet2 = NewSet1 + 1;
        if (SM_NUM_OF_PARAMETERSETS == NewSet2) {
            NewSet2 = 0;
        }
        NewParam2 = &Data->Motion.Param[NewSet2];       // used to set up second new parameter set

        NewParam2->NegPosCount = !NewParam->NegPosCount;
        NewParam2->CCWRotDir = !NewParam->CCWRotDir;

        smSetupNewSpeedParamSet(Speed, &Data->Profiles.Set[ProfileIndex], NewParam, NewParam2);

        Data->Motion.NewSet = NewSet2;      // the new parameter set to use
    }

    Data->Motion.AtTargetSpeed = FALSE;

    return 0;
}


/*****************************************************************************/
/*!
 *  \brief   Request to start movement with target speed
 *
 *      Motor is currently standing still and should now rotate with supplied
 *      target speed. To perform the speed acceleration the motion parameters
 *      are setup according to the selected profile. 
 *
 *      Before motion is started it is checked if motor can move into desired
 *      direction, e.g. if motor is standing at a limit switch the configuration
 *      may only allow rotation in certain direction.
 *
 *
 *  \iparam  Data           = Pointer to module instance's data
 *  \iparam  Speed          = new target speed (in half-step/s)
 *  \iparam  ProfileIndex   = index of motion profile
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/
Error_t smNewSpeedRequest (smData_t *Data, Int32 Speed, Int8 ProfileIndex)
{
    Error_t RetCode = NO_ERROR;

    Int32 StartSpeed;

    smParamSet_t        *Param;
    smProfile_t         *Profile;

    Data->Motion.ActSet = 0;
    Data->Motion.NewSet = 0;

    Param = &Data->Motion.Param[Data->Motion.ActSet];
    Profile = &Data->Profiles.Set[ProfileIndex];

    Speed = smCheckDirection(Data->Motor.InversePosCount, Param, Speed);
    Param->dSteps = Profile->StepWidth;
    StartSpeed = Profile->Config.vMin / SM_PROFILE_MICROSTEPS_PER_HALFSTEP;

    // check if we can move into the requested direction
    if (  ((Param->NegPosCount) && (Data->Motion.Pos <=Data->Motor.FrameworkConfig.MinPosition))
        ||((!Param->NegPosCount) && (Data->Motion.Pos >=Data->Motor.FrameworkConfig.MaxPosition))) {
        return (E_SMOT_STOP_BY_OFFLIMIT);
    }
#ifndef SIMULATION
    if ((RetCode = smRotDirIsAllowed(Param->CCWRotDir, &Data->LimitSwitches)) < 0) {
        return RetCode;
    }
#endif

    smSetupSpeedParam (StartSpeed, Speed, Profile, Param->Ph);
    Param->OffsetPos = Data->Motion.Pos;

    // determine start phase 
    Data->Motion.Phase = PH_0_START;
    if (0 == Param->Ph[Data->Motion.Phase].dt) {
        Data->Motion.Phase = smNextPhase(Param->Ph, Data->Motion.Phase);
    }
    Param->OffsetPhase = Data->Motion.Phase;

    Data->Motion.s = 0;
    Data->Motion.v0 = Param->Ph[Data->Motion.Phase].v;
    Data->Motion.a0 = Param->Ph[Data->Motion.Phase].a;
    Data->Motion.j = Param->Ph[Data->Motion.Phase].j;
    Data->Motion.t = 0;

    Data->Motion.Nominal.s = 0;
    Data->Motion.Nominal.v = Data->Motion.v0;
#ifdef TRACE
    Data->Motion.Nominal.a = Data->Motion.a0;
    Data->Motion.Nominal.t = 0;
#endif

    Data->Motion.AtTargetSpeed = FALSE;
    Data->Motion.State = MS_SPEED;

#ifdef SIMULATION
    return 0;
#else
    if ((RetCode = smStartMotion(Data)) < 0)
    {
        Data->Motion.State = MS_IDLE;
        smStopMotion (Data->Instance);
    }
    return RetCode;
#endif

}


/*****************************************************************************/
/*!
 *  \brief   Request to move with target speed
 *
 *      Motor should rotate with supplied target speed. To perform the speed
 *      change the motion parameters are setup according to the selected profile.
 *
 *      Speed change are only allowed if motor is standing still or already  
 *      rotating with target speed. If a position move is still pending then 
 *      the request is rejected.
 *      Only exception is a request to move with target speed zero, which
 *      is used to stop movement.
 *
 *
 *  \iparam  Instance       = Instance number of this module
 *  \iparam  Speed          = new target speed (in half-step/s)
 *  \iparam  ProfileIndex   = index of motion profile
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/
Error_t smSpeedRequest (UInt16 Instance, Int32 Speed, UInt8 ProfileIndex)
{
    smData_t *Data = &smDataTable[Instance];

    Error_t RetCode = NO_ERROR;

    Int32   AbsSpeed;   // absolute value of requested speed

// check if requested speed is within the profiles speed limits
// speed 0 is always excepted. it is used to stop the motor.
    if (0 != Speed)
    {
        if (Speed < 0) {
            AbsSpeed = -Speed;
        }
        else {
            AbsSpeed = Speed;
        }
    
        if (AbsSpeed < Data->Profiles.Set[ProfileIndex].Config.vMin / SM_PROFILE_MICROSTEPS_PER_HALFSTEP) {
//      if (AbsSpeed < Data->config.profiles[ProfileIndex].vMin / PROFILE_MICROSTEPS_PER_HALFSTEP)
            return E_SMOT_MOTION_PROFILE_UNFIT;
        }
    
        if (AbsSpeed > Data->Profiles.Set[ProfileIndex].Config.vMax / SM_PROFILE_MICROSTEPS_PER_HALFSTEP) {
//      if (AbsSpeed > Data->config.profiles[ProfileIndex].vMax / PROFILE_MICROSTEPS_PER_HALFSTEP)
            return E_SMOT_MOTION_PROFILE_UNFIT;
        }
    }

    // check that signals from limit switches are ok
#ifndef SIMULATION
    if ((RetCode = smPosCodeIsValid(&Data->LimitSwitches, Data->LimitSwitches.PosCode.Value)) < 0) {
        return RetCode;
    }
#endif

    switch (Data->Motion.State)
    {
// motor is standing still and now we want to move with new target
    case MS_IDLE:
        // start movement if requested speed is different from actual speed
        if (0 != Speed)
        {
            RetCode = smNewSpeedRequest (Data, Speed, ProfileIndex);
            if (RetCode >= 0) {
                Data->Motion.Param[Data->Motion.NewSet].Speed = Speed; // remember the requested target speed
            }
        }
        else {
            Data->Motion.AtTargetSpeed = TRUE;  // fake successful speed movement
        }
        break;
//  move with target speed while position run is in progress. only target speed 0 is allowed to stop the movement.
    case MS_POSITION:
        if (0 != Speed) {
            RetCode = E_COMMAND_REJECTED;
        }
        else
        {   
            // make the movement to leave phase 4 (phase with constant speed)
            smParamSet_t *ActParam;
            ActParam = &Data->Motion.Param[Data->Motion.ActSet];        // actual parameter set
            Data->Motion.State = MS_SPEED;
            ActParam->Ph[PH_4_VEL_CONST].dt = 0;            
            ActParam->Ph[PH_4_VEL_CONST].ds = 0;            
            ActParam->Ph[PH_4_VEL_CONST].s = 0;         
        }
        break;

//  move with new target speed while speed run is already in progress
    case MS_SPEED:
        // check if current speed is within the profiles speed limits
        if (Data->Motion.Param[Data->Motion.NewSet].Speed < 0) {
            AbsSpeed = -Data->Motion.Param[Data->Motion.NewSet].Speed;
        }
        else {
            AbsSpeed = Data->Motion.Param[Data->Motion.NewSet].Speed;
        }
    
        if (AbsSpeed < Data->Profiles.Set[ProfileIndex].Config.vMin / SM_PROFILE_MICROSTEPS_PER_HALFSTEP) {
            return E_SMOT_MOTION_PROFILE_UNFIT;
        }
    
        if (AbsSpeed > Data->Profiles.Set[ProfileIndex].Config.vMax / SM_PROFILE_MICROSTEPS_PER_HALFSTEP) {
            return E_SMOT_MOTION_PROFILE_UNFIT;
        }


        // start movement if requested speed is different from last requested  speed
        if (Speed != (Data->Motion.Param[Data->Motion.NewSet].Speed))
        {
            RetCode = smChangeSpeedRequest (Data, Speed, ProfileIndex);
            if (RetCode >= 0) {
                Data->Motion.Param[Data->Motion.NewSet].Speed = Speed; // remember the requested target speed
            }
        }
        break;

// unknown motion state
    default:
        RetCode = E_COMMAND_REJECTED;
        break;
    }

    return RetCode;
}
