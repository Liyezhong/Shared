/****************************************************************************/
/*! \file
 * 
 *  \brief  Interrupt service routine performing the (micro-)stepping of 
 *          function module 'stepper motor'
 *
 *  $Rev:    $ 0.1
 *  $Date:   $ 27.06.2012
 *  $Author: $ Rainer Boehles
 *
 *  \b Description:
 *
 *      This ISR is performing the actual stepper movement by writing a new step
 *      value to the motor driver at each execution loop of the ISR.
 *      Step value is incremented/decremented at each execution loop to rotate
 *      motor in the desired direction. The amount by which step value is changed
 *      depends from the desired stepping. If step value is changed by 64
 *      the motor is moved 1 full step, 32 moves 1 half step,... down to 1 which
 *      means highest resolution with 64 micro step per full step.
 *
 *      The ISR is triggered by the capture compare register assigned to a instance.
 *      The related timer is free running and common to all instances. The timer
 *      itself is never stopped, only CCR is modified according to the calculated
 *      speed and CCR interrupt is enabled/disabled to start/stop movement. Also
 *      the ISR is common to all instances and the correct instance which needs to
 *      be served have to be detemined first.
 *
 *      Speed calculation depends from active s-curve phase. Which phases are used
 *      and which distance is traveled at each phase is determined by a precalculated
 *      parameter set.
 *
 *      To provide more complex movements it is possible to switch parameter sets
 *      'on the fly'.
 *      e.g. movement can stop at a defined position while a rotatory movement
 *      with target speed is active. To accomplish this task the trigger point
 *      at which motor needs to decelerate is calculated outside the ISR and a
 *      new parameter set is created. When ISR detects that trigger point is crossed
 *      the ISR switches to the new parameter set.
 *
 *      Motor position is tracked by incrementing/decrementing motor position at
 *      each half step. If a encoder is available and interrupt interval time is
 *      sufficient long, then motor position is checked aginst encoder position.
 *      An case of a step loss the ISR will try to adjust the lost steps by keeping
 *      motor rotating without counting steps until encoder and motor position get's
 *      equal again or step loss get's above an error limit. If encoder is ahead
 *      of motor position, then ISR will try to stop the motor when target position
 *      is crossed.
 *
 *      Necessary actions are performed in this order:
 *          (1) determine the motor instance this interrupt is for
 *          (2) reload the capture-compare register with the next value
 *          (3) if encoder exist do half-step position checks for encoder
 *              (3.1) calculate step deviation
 *              (3.2) validate step deviation
 *              (3.3) check if target is reached
 *              (3.4) check step loss / step loss correction / ahead steps / deviation limits
 *          (4) initiate motor movement to next (micro-)step position
 *          (5) do half-step position checks
 *              (5.1) update revolution counter (life cycle data)
 *              (5.2) update actual position
 *              (5.3) check if parameter set switch is pending
 *              (5.4) check if movement have to be switched to next phase
 *              (5.5) check for stop condition initiated from outside the ISR
 *              (5.6) check if movement should stop
 *          (6) do phase depending speed calculation
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

// optimization can be switched off with STEPPER_ISR_TIME_OPTIMIZATION_OFF
// stepper ISR execution time can be traced with STEPPER_TRACE_ISR_TIMING.

#include "stdlib.h"
#include "Global.h"

#include "fmStepperMotor.h"
#include "fmStepperMotorMotionISR.h"

#ifndef SIMULATION
    #include "halTimer.h"
    #include "halStepper.h"

#include "halSysTick.h"
#else
    #define E_SMOT_STOP_BY_STEPDIFF -1
    #define W_SMOT_STEPDIFF -1
    #define E_SMOT_STOP_BY_STEPLOSS -1
    #define W_SMOT_STEPLOSS -1
    #define E_SMOT_TIMER_OVERLOAD -1
#endif /*SIMULATION*/


#ifdef TRACE
    #include <tchar.h>
    #include <stdio.h>

extern FILE* stpLog;
extern FILE* posLog;

#endif /*TRACE*/


#ifdef STEPPER_TRACE_ISR_TIMING

    UInt32  isrDuration;
    UInt16  timing[NUM_OF_MOTION_PHASES][40];

    void ClearTiming()
    {
        UInt8 p,i;
        for (p=0; p<NUM_OF_MOTION_PHASES; p++)
        {
            for (i=0; i<40; i++)
                timing[p][i] = 0;
        }
    }

#endif


/******************************************************************************/
/*! 
 *  \brief  Stepper motor interrupt sevice routine
 *
 *      Interrupt service routine for the stepper motor.
 *      For more detailed description see module header (\sa fmStepperMotorMotionISR.c)
 * 
 *  \iparam  UserTag    = tag used to pass user data to a ISR (not used by this ISR)
 *  \iparam  IntrFlags  = index of CCR, used to identify the related instance
 * 
 ******************************************************************************/
void smMotionISR (UInt32 UserTag, UInt32 IntrFlags)
{
    Motion_t            *Motion;
    smParamSet_t        *Param;

    UInt8               Instance;
    UInt16              CCROffset;

    smEncoder_t         *Encoder;
    smStepLoss_t        *StepLoss;
    Int32               StepDiff = 0;

    smMotor_t           *Motor;

#ifdef STEPPER_TRACE_ISR_TIMING
    isrDuration = halGetFastTick();
#endif

//--------------------------------------------------------------------------
// (1) determine the motor instance this interrupt is for
//--------------------------------------------------------------------------
// the motor this irq is called for can be identified by the interrupt flags of the
// capture compare unit. 
    if(IntrFlags & 0x0002)
    {
        Instance = 0;
        IntrFlags &= ~0x0002;
    }
    else if(IntrFlags & 0x0004)
    {
        Instance = 1;
        IntrFlags &= ~0x0004;
    }
    else if(IntrFlags & 0x0008)
    {
        Instance = 2;
        IntrFlags &= ~0x0008;
    }
    else if(IntrFlags & 0x0010)
    {
        Instance = 3;
        IntrFlags &= ~0x0010;
    }
    else
    {
// !!! overload !!!
// switch off timer
#ifndef SIMULATION
        halTimerControl(smHandleTimer, TIM_CTRL_STOP);
        bmSignalEvent(smDataTable[0].Channel, E_SMOT_TIMER_OVERLOAD, TRUE, 0);
#endif /*SIMULATION*/
        return;
    }

    Motion = &smDataTable[Instance].Motion;
    Param = &Motion->Param[Motion->ActSet];

//--------------------------------------------------------------------------
//(2) reload the capture-compare register with the next value
//--------------------------------------------------------------------------
    // timer intervall is calculated from actual speed
    // each tick has a period from 1.000.000.000 nano seconds per second / 72.000.000 tick per second
    // the prescaler was set to 6
    Motion->dt = Param->dSteps * ISR_TIMEBASE / Motion->Nominal.v;  // elapsed 탎 to perform step
    CCROffset = (Motion->dt * 12);                                  // processor clock divided by pre-scaler = 72Mhz / 6 = 12 

    // The current CCR value has been stored
    // -> add the time offset, this ensures correct timing of the next irq call
    Motion->CCR.LastValue += CCROffset;

#ifndef SIMULATION
    //write the new value to ccr-register
    // the irq-flag will be cleared inside the 'halCapComWrite' function
    #ifdef STEPPER_ISR_TIME_OPTIMIZATION_OFF
        halCapComWrite(smHandleTimer, Instance, Motion->CCR.LastValue);
    #else
        halCapComWriteTimer1_Simplified(Instance, Motion->CCR.LastValue);
    #endif
#endif /*SIMULATION*/

//--------------------------------------------------------------------------
// (3) if encoder exist do half-step position checks for encoder
//--------------------------------------------------------------------------

    Encoder = &smDataTable[Instance].Encoder;
    StepLoss = &Encoder->StepLoss;
    Motor = &smDataTable[Instance].Motor;

// perform check only at half-step position
    if (0 == (Motion->SinIndex & (ISR_MICROSTEPS_PER_HALFSTEP-1)))
    {
// check encoder position
        if (Encoder->Config.Exists)
        {
            UInt16 ResetPosition = Motor->FrameworkConfig.ResetPosition;

            Int32 MotPos    = Motion->Pos;
            Int32 EncPos    = Encoder->Pos;
            Int32 LastPos   = StepLoss->LastEncPos;

//--------------------------------------------------------------------------
// (3.1) calculate step deviation
//--------------------------------------------------------------------------
// take care of overflow for rotatory movement. correct motor, encoder and last encoder position accordingly
            if (0 != ResetPosition)
            {
                Bool Overflow;  // set if actual position already crossed reset position but encoder not (or vice versa)

                if (Param->NegPosCount) {
                    StepLoss->PosOverflow = StepLoss->PosOverflow || ((((ResetPosition - 1) == MotPos) && (!Encoder->FirstTime)));
                }
                else {
                    StepLoss->PosOverflow = StepLoss->PosOverflow || (((0 == MotPos) && (!Encoder->FirstTime)));
                }
                if (LastPos < EncPos)
                {
                    if ((EncPos - LastPos) > (ResetPosition >> 1))
                    {                                       // encoder just crossed the reset position in neg. counting direction
                        StepLoss->EncXNeg = TRUE;
                        StepLoss->EncPosOverflow ^= TRUE;
                        LastPos += ResetPosition;           // make last encoder position greater than encoder position
                    }
                }
                else
                {
                    if ((LastPos - EncPos) > (ResetPosition >> 1))
                    {                                       // encoder just crossed the reset position in pos. counting direction
                        StepLoss->EncXNeg = FALSE;
                        StepLoss->EncPosOverflow ^= TRUE;
                        LastPos -= ResetPosition;           // make last encoder position neg. number
                    }
                }

                Overflow = (StepLoss->PosOverflow ^ StepLoss->EncPosOverflow);
                if (Overflow)
                {                                           // only one of motor or encoder crossed the reset position
                    if (StepLoss->PosOverflow)              // motor crossed reset position, but encoder not
                    {
                        if (Param->NegPosCount) {
                            MotPos -= ResetPosition;        // neg. counting direction -> make motor position neg. number
                        }
                        else
                        {
                            EncPos -= ResetPosition;        // pos. counting direction -> make encoder position neg. number
                            LastPos -= ResetPosition;       // make last encoder position neg. number
                        }
                    }
                    else
                    {                                       // encoder crossed reset position
                        if (StepLoss->EncXNeg)              // check in which direction reset position was crossed
                        {
                            EncPos -= ResetPosition;        // neg. counting direction -> make encoder position neg. number
                            LastPos -= ResetPosition;       // make last encoder position neg. number
                        }
                        else {
                            MotPos -= ResetPosition;        // pos. counting direction -> make motor position neg. number
                        }
                    }
                }
                else
                {
                    if (StepLoss->PosOverflow)
                    {
                        StepLoss->PosOverflow = FALSE;
                        StepLoss->EncPosOverflow = FALSE;
                        StepLoss->EncXNeg = FALSE;
                    }
                }
                Encoder->FirstTime = FALSE;
            }

// calculate difference between encoder and nominal position
            if (Param->NegPosCount) {
                StepDiff = EncPos - MotPos;
            }
            else {
                StepDiff = MotPos - EncPos;
            }
//            StepDiff = StepDiff % ResetPosition;

//--------------------------------------------------------------------------
// (3.2) validate step deviation
//--------------------------------------------------------------------------
// if step difference is getting too large then terminate movement
            if (abs(StepDiff) > Motor->Config.Resolution) {
                if (0 != StepLoss->Config.ErrorLimit) {
                    Motion->State = MS_STOP;
                }
            }

//--------------------------------------------------------------------------
// (3.3) check if target is reached
//--------------------------------------------------------------------------
            if (  (0 == ResetPosition)                          // check always for linear movement
                ||(Motion->Phase > PH_4_VEL_CONST))             // check only during decelaration for rotatory movement
            {
                if (MS_POSITION == Motion->State)               // target position movement not yet terminated because of step difference
                {
                    if (Param->NegPosCount)
                    {
                        if ((LastPos >= Encoder->TargetPos) && (EncPos <= Encoder->TargetPos)) {
                            Motion->AtTargetPosition = TRUE;    // target position is reached
                        }
                    }
                    else
                    {
                        if ((LastPos <= Encoder->TargetPos) && (EncPos >= Encoder->TargetPos)) {
                            Motion->AtTargetPosition = TRUE;    // target position is reached
                        }
                    }
                    if (Motion->AtTargetPosition) {
                        Motion->State = MS_STOP;                // this will terminate movement
                    }
                }
            }

// check if target speed 0 is reached
            if (MS_SPEED == Motion->State)                      // target speed movement not yet terminated because of step difference
            {
                if (Encoder->AtSpeedZero)
                {
                    Motion->AtTargetSpeed = TRUE;
                    Motion->State = MS_STOP;                    // this will terminate movement
                }
            }

            StepLoss->LastEncPos = Encoder->Pos;

//--------------------------------------------------------------------------
// (3.4) check step loss / step loss correction / ahead steps / deviation limits
//--------------------------------------------------------------------------
            // do check only on reliable step difference
            if (Motion->dt > ISR_ENCODER_TIME_LIMIT)            // we can only check step difference if there is enough time to read position info from encoder in main loop
            {
                if (MS_STOP != Motion->State)                   // not yet terminated because of step difference or target position rached
                {
                    if (  (0 != StepLoss->Config.ErrorLimit)    // stop movement if step difference exceeds the error limit
                        &&(abs(StepDiff) >= StepLoss->Config.ErrorLimit))
                    {
                        Motion->State = MS_STOP;
                    }
                    else
                    {                                           // check for step loss
                        if (!StepLoss->Detected)                // so far there was no step loss detected
                        {
                            if (StepDiff >= 8) {                // a step difference of 8 half-steps means one full commutation
                                StepLoss->Detected = TRUE;      // now we have detected a step loss
                            }
                            else
                            {
                                if (PH_8_END == Motion->Phase)
                                {
                                    if (MS_POSITION == Motion->State)
                                    {
                                        if (Motion->s >= Param->Ph[Motion->Phase].ds)
                                        {                       // we should have already reached the target
                                            if (StepDiff > 0)   // check if actual position deviates from encoder position
                                            {                   // only step loss can be adjusted
                                                    if (Param->NegPosCount)
                                                    {
                                                        if (EncPos > MotPos) {
                                                            StepLoss->Detected = TRUE;  // detected step loss
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (EncPos < MotPos) {
                                                            StepLoss->Detected = TRUE;  // detected step loss
                                                        }
                                                    }
                                            }
                                            else
                                            {
                                                Motion->State = MS_STOP;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else                                        // step loss have been detect before
                        {
                            StepLoss->Count++;
                            if (0 != StepLoss->Config.ErrorLimit)   // stop movement if step loss exceeds the error limit
                            {
                                if (StepLoss->Count > StepLoss->Config.ErrorLimit)  
                                {
                                    Motion->State = MS_STOP;
                                }
                            }
                            if (Param->NegPosCount)                 // check if actual position is in sync again with encoder position
                            {
                                if (EncPos <= MotPos) {
                                    StepLoss->Detected = FALSE;     // step loss is corrected
                                }
                            }
                            else
                            {
                                if (EncPos >= MotPos) {
                                    StepLoss->Detected = FALSE;     // step loss is corrected
                                }
                            }
                        }
                    }
                }
            }
        } /*if (Encoder->Config.Exists)*/
    }

//--------------------------------------------------------------------------
//(4) initiate motor movement to next (micro-)step position
//--------------------------------------------------------------------------

// perform the micro step
    if (MS_STOP != Motion->State)
    {
        if (!StepLoss->Detected)
        {
            Motion->s += Param->dSteps;         // this will be the micro step position within actual phase after step have been performed by stepper motor
            Motion->t += Motion->dt;            // and the time
        }

        // move motor by increasing/decreasing sinus index with micro step value
        if (Param->CCWRotDir) {
            Motion->SinIndex -= Param->dSteps;
        }
        else {
            Motion->SinIndex += Param->dSteps;
        }

        // write value to stepper motor
#ifndef SIMULATION
    #ifdef STEPPER_ISR_TIME_OPTIMIZATION_OFF
        halStepperWrite(Motor->Handle, Motion->SinIndex);
    #else
        halStepperWrite_Simplified(Instance, Motion->SinIndex);
    #endif
#endif /*SIMULATION*/

#ifdef TRACE
        Motion->Nominal.s += Param->dSteps; // this will be the micro step position after step have been performed by stepper motor
        Motion->Nominal.t += Motion->dt;
        fprintf(stpLog, "%f,%lli,%lli,%lli,%li,%i\n", Motion->Nominal.t/1000.0, Motion->Nominal.s, Motion->Nominal.v, Motion->Nominal.a, Motion->dt, Motion->SinIndex);
//      fflush(stpLog);
#endif /*TRACE*/
    }

//--------------------------------------------------------------------------
// If more then one interrupt is pending then call interrupt routine recursively.
// Calling ISR from here recursively instead of looping over the interrupt flags
// will minimize the step latency when 2 motors need to step at the same time.
//--------------------------------------------------------------------------
    if(IntrFlags) {
        smMotionISR(0,IntrFlags);
    }

//--------------------------------------------------------------------------
// (5) do half-step position checks
//--------------------------------------------------------------------------

// at each half-step position we have to check for special conditions (e.g. phase changes, end position reached ...)
    if (0 == (Motion->SinIndex & (ISR_MICROSTEPS_PER_HALFSTEP-1)))
    {
        if (MS_STOP != Motion->State)
        {

// trigger sampling of limit switches
            Motion->LSTrigger = TRUE;

//--------------------------------------------------------------------------
// (5.1) update revolution counter  (life cycle data)
//--------------------------------------------------------------------------
// maintain life cycle revolution counter at full-step position
            if (0 == (Motion->SinIndex & (SM_MICROSTEPS_PER_FULLSTEP-1)))
            {
                Motor->LifeCycle.Revolutions.FullSteps++;
                if(Motor->LifeCycle.Revolutions.FullSteps == Motor->Config.Resolution)
                {
                    Motor->LifeCycle.Revolutions.FullSteps = 0;
                    Motor->LifeCycle.Revolutions.Count++; 
                }
            }

//--------------------------------------------------------------------------
// (5.2) update actual position
//--------------------------------------------------------------------------
// maintain actual position 
            if (!StepLoss->Detected)
            {
                if (Param->NegPosCount)
                {
                    Motion->Pos--;
                    if (SM_MICROSTEPS_PER_FULLSTEP == Param->dSteps) {
                        Motion->Pos--;
                    }
                    if (0 != Motion->ResetPos)
                    {
                        if (Motion->Pos < 0) {
                            Motion->Pos = Motion->ResetPos + Motion->Pos;
                        }
                    }
                }
                else
                {
                    Motion->Pos++;
                    if (SM_MICROSTEPS_PER_FULLSTEP == Param->dSteps) {
                        Motion->Pos++;
                    }
                    if (0 != Motion->ResetPos) {
                        Motion->Pos = Motion->Pos % Motion->ResetPos;
                    }
                }

    #ifdef TRACE
                if (Param->NegPosCount)
                    fprintf(posLog, "%f,%i,%lli,%lli\n", Motion->Nominal.t/1000.0, Motion->Pos, -Motion->Nominal.v >> 5, -Motion->Nominal.a >> 5);
                else
                    fprintf(posLog, "%f,%i,%lli,%lli\n", Motion->Nominal.t/1000.0, Motion->Pos, Motion->Nominal.v >> 5, Motion->Nominal.a >> 5);
        //      fflush(posLog);
    #endif /*TRACE*/

//--------------------------------------------------------------------------
// (5.3) check if parameter set switch is pending
//--------------------------------------------------------------------------
                if (Motion->ActSet != Motion->NewSet)
                {
                    if (Param->SwitchSet.Trigger.OldPhase == Motion->Phase)
                    {
                        if ((Param->SwitchSet.Trigger.Position < 0) || (Motion->s == Param->SwitchSet.Trigger.Position))
                        {
                            Motion->Phase = Param->SwitchSet.NewPhase;

                            Motion->ActSet++;
                            if (SM_NUM_OF_PARAMETERSETS == Motion->ActSet) {
                                Motion->ActSet = 0;
                            }
                            Param = &Motion->Param[Motion->ActSet];

                            Param->OffsetPos   = Motion->Pos;
                            Param->OffsetPhase = Motion->Phase;

                            Motion->v0 = Param->Ph[Motion->Phase].v;
                            Motion->a0 = Param->Ph[Motion->Phase].a;

                            Motion->Nominal.v = Motion->v0;
#ifdef TRACE
                            Motion->Nominal.a = Motion->a0;
                            Motion->Nominal.s = 0;
#endif

                            Motion->s = 0;

                            Motion->j = Param->Ph[Motion->Phase].j;
                            Motion->t = Param->dSteps * ISR_TIMEBASE / Motion->Nominal.v;   // elapsed 탎
                            //todo is it ok to use "Motion->t = Motion->dt;" instead of above line ?
                        }
                    }
                }

//--------------------------------------------------------------------------
// (5.4) check if movement have to be switched to next phase
//--------------------------------------------------------------------------
                if (Param->Ph[Motion->Phase].dt >= 0)
                {
                    if (Motion->s >= Param->Ph[Motion->Phase].ds)
                    {
                        if (PH_8_END == Motion->Phase)                          // target position or target speed 0 is reached 
                        {
                            if (!Encoder->Config.Exists)
                            {                                                   // without encoder stop immediately
                                if (MS_POSITION == Motion->State) {
                                    Motion->AtTargetPosition = TRUE;            // target position is reached
                                }
                                else {
                                    Motion->AtTargetSpeed = TRUE;               // target speed 0 is reached
                                }
                                Motion->State = MS_STOP;                        // this will terminate movement
                            }
                            else
                            {
                                if (MS_POSITION == Motion->State)
                                {
                                    if (Motion->dt <= ISR_ENCODER_TIME_LIMIT)   // terminate movement immediately, if no step loss adjustment is possible
                                    {
                                        Motion->AtTargetPosition = TRUE;        // target position is reached
                                        Motion->State = MS_STOP;                // this will terminate movement
                                    }
                                }
                                else
                                {                                               // target speed 0 is reached
                                    Encoder->AtSpeedZero = TRUE;                // movement will be stopped at next interrupt
                                }
                            }
                        }
                        else                                                    // use next phase
                        {                               
                            while (Motion->Phase < PH_8_END)     
                            {
                                Motion->Phase++;
                                if (0 != Param->Ph[Motion->Phase].dt) {
                                    break;
                                }
                            }

                            Motion->v0 = Param->Ph[Motion->Phase].v;
                            Motion->a0 = Param->Ph[Motion->Phase].a;

                            Motion->Nominal.v = Motion->v0;
#ifdef TRACE
                            Motion->Nominal.a = Motion->a0;
#endif

                            Motion->j = Param->Ph[Motion->Phase].j;
            //              Motion->t = Param->dSteps * ISR_TIMEBASE / Motion->Nominal.v;    // elapsed 탎
                            Motion->t = Motion->dt;   // elapsed 탎

                            Motion->s = 0;
                        }
                    }
                }
            } /*if (!StepLoss->Detected)*/
        } /*if (MS_STOP != Motion->State)*/


//--------------------------------------------------------------------------
// (5.5) check for stop condition initiated from outside the ISR
//--------------------------------------------------------------------------
        if (SM_SC_NONE != Motion->Stop)
        {
            switch (Motion->Stop)
            {
            case SM_SC_DIR_CW:                  // stop if moving CW
                if (!Param->CCWRotDir) {
                    Motion->State = MS_STOP;    // this will terminate movement
                }
                else {
                    Motion->Stop = SM_SC_NONE;
                }
                break;
            case SM_SC_DIR_CCW:                 // stop if moving CCW
                if (Param->CCWRotDir) {
                    Motion->State = MS_STOP;    // this will terminate movement
                }
                else {
                    Motion->Stop = SM_SC_NONE;
                }
                break;
            case SM_SC_OFFLIMIT:                // stop because position is off-limit
                // break; intentionally omitted
            case SM_SC_ALWAYS:                  // stop always
                Motion->State = MS_STOP;        // this will terminate movement
                break;
            default:                            // don't stop
                break;
            }
        }

//--------------------------------------------------------------------------
// (5.6) check if movement should stop
//--------------------------------------------------------------------------
        if (MS_STOP == Motion->State) {
#ifndef SIMULATION
            halCapComControl(smHandleTimer, Motion->CCR.Unit, TIM_INTR_DISABLE);    // stop motion by disabling timer CCR irq
            StepLoss->StepDiff = StepDiff;
#endif
            Motion->Nominal.v = 0;      // set actual speed to zero
            return;
        }
    } /*if (0 == (SinIndex & (ISR_MICROSTEPS_PER_HALFSTEP-1)))*/


// in case of step loss no speed calculation is needed, because actual speed will be used to adjust the step loss
    if (StepLoss->Detected) {
        return;
    }

//--------------------------------------------------------------------------
// (6) do phase depending speed calculation
//--------------------------------------------------------------------------
    switch (Motion->Phase)
    {
    case PH_0_START:
        break;

    case PH_1_ACC_JERK_UP:
        Motion->Nominal.v = Motion->v0 + Motion->a0*Motion->t/ISR_TIMEBASE + Motion->j*Motion->t*Motion->t/2000000000000;
//      Motion->Nominal.v = Motion->v0 + Motion->a0*Motion->t/ISR_TIMEBASE + Motion->j*Motion->t*Motion->t/2/ISR_TIMEBASE/ISR_TIMEBASE;
        if (Motion->Nominal.v > Param->Ph[PH_1_ACC_JERK_UP].vE) {
            Motion->Nominal.v = Param->Ph[PH_1_ACC_JERK_UP].vE;
        }
#ifdef TRACE
        Motion->Nominal.a = Motion->a0 + Motion->j*Motion->t/ISR_TIMEBASE;
        if (Motion->Nominal.a > Param->Ph[PH_1_ACC_JERK_UP].aE) {
            Motion->Nominal.a = Param->Ph[PH_1_ACC_JERK_UP].aE;
        }
#endif
        break;

    case PH_2_ACC_CONST:
        Motion->Nominal.v = Motion->v0 + Motion->a0*Motion->t/ISR_TIMEBASE;
        if (Motion->Nominal.v > Param->Ph[PH_2_ACC_CONST].vE) {
            Motion->Nominal.v = Param->Ph[PH_2_ACC_CONST].vE;
        }
        break;

    case PH_3_ACC_JERK_DOWN:
        Motion->Nominal.v = Motion->v0 + Motion->a0*Motion->t/ISR_TIMEBASE + Motion->j*Motion->t*Motion->t/2000000000000;
//      Motion->Nominal.v = Motion->v0 + Motion->a0*Motion->t/ISR_TIMEBASE + Motion->j*Motion->t*Motion->t/2/ISR_TIMEBASE/ISR_TIMEBASE;
        if (Motion->Nominal.v > Param->Ph[PH_3_ACC_JERK_DOWN].vE) {
            Motion->Nominal.v = Param->Ph[PH_3_ACC_JERK_DOWN].vE;
        }
#ifdef TRACE
        Motion->Nominal.a = Motion->a0 + Motion->j*Motion->t/ISR_TIMEBASE;
        if (Motion->Nominal.a < Param->Ph[PH_3_ACC_JERK_DOWN].aE) {
            Motion->Nominal.a = Param->Ph[PH_3_ACC_JERK_DOWN].aE;
        }
#endif
        break;

    case PH_4_VEL_CONST:
        if (0 == (Motion->SinIndex & (ISR_MICROSTEPS_PER_HALFSTEP-1))) { // do check only at half-step position
            Motion->AtTargetSpeed = (Param->Ph[PH_4_VEL_CONST].dt < 0); // if endless movement then target speed reached
        }
        if (Motion->AtTargetSpeed && Motion->ResetPos) {
            Motion->s = Motion->s % (Motion->ResetPos << 5);
        }
        break;

    case PH_5_DEC_JERK_UP:
        Motion->Nominal.v = Motion->v0 + Motion->a0*Motion->t/ISR_TIMEBASE + Motion->j*Motion->t*Motion->t/2000000000000;
//      Motion->Nominal.v = Motion->v0 + Motion->a0*Motion->t/ISR_TIMEBASE + Motion->j*Motion->t*Motion->t/2/ISR_TIMEBASE/ISR_TIMEBASE;
        if (Motion->Nominal.v < Param->Ph[PH_5_DEC_JERK_UP].vE) {
            Motion->Nominal.v = Param->Ph[PH_5_DEC_JERK_UP].vE;
        }
#ifdef TRACE
        Motion->Nominal.a = Motion->a0 + Motion->j*Motion->t/ISR_TIMEBASE;
        if (Motion->Nominal.a < Param->Ph[PH_5_DEC_JERK_UP].aE) {
            Motion->Nominal.a = Param->Ph[PH_5_DEC_JERK_UP].aE;
        }
#endif
        break;

    case PH_6_DEC_CONST:
        Motion->Nominal.v = Motion->v0 + Motion->a0*Motion->t/ISR_TIMEBASE;
        if (Motion->Nominal.v < Param->Ph[PH_6_DEC_CONST].vE) {
            Motion->Nominal.v = Param->Ph[PH_6_DEC_CONST].vE;
        }
        break;

    case PH_7_DEC_JERK_DOWN:
        Motion->Nominal.v = Motion->v0 + Motion->a0*Motion->t/ISR_TIMEBASE + Motion->j*Motion->t*Motion->t/2000000000000;
//      Motion->Nominal.v = Motion->v0 + Motion->a0*Motion->t/ISR_TIMEBASE + Motion->j*Motion->t*Motion->t/2/ISR_TIMEBASE/ISR_TIMEBASE;
        if (Motion->Nominal.v < Param->Ph[PH_7_DEC_JERK_DOWN].vE) {
            Motion->Nominal.v = Param->Ph[PH_7_DEC_JERK_DOWN].vE;
        }
#ifdef TRACE
        Motion->Nominal.a = Motion->a0 + Motion->j*Motion->t/ISR_TIMEBASE;
        if (Motion->Nominal.a > Param->Ph[PH_7_DEC_JERK_DOWN].aE) {
            Motion->Nominal.a = Param->Ph[PH_7_DEC_JERK_DOWN].aE;
        }
#endif
        break;

    case PH_8_END:
        if (0 == (Motion->SinIndex & (ISR_MICROSTEPS_PER_HALFSTEP-1))) { // do check only at half-step position
            Motion->AtTargetSpeed = (Param->Ph[PH_8_END].dt < 0);       // if endless movement then target speed reached
        }
        if (Motion->AtTargetSpeed && Motion->ResetPos) {
            Motion->s = Motion->s % (Motion->ResetPos << 5);
        }
        break;

    default:
        break;
    }

#ifdef STEPPER_TRACE_ISR_TIMING
    isrDuration = halGetFastTick() - isrDuration;
    timing[Motion->Phase][isrDuration]++;
#endif
}
