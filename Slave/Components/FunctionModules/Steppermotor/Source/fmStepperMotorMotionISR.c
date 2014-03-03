

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


//inline Int32 abs(Int32 x)
//{
//  return x > 0 ? x : -x;
//}


// optimization can be switched off with STEPPER_ISR_TIME_OPTIMIZATION_OFF
// stepper ISR execution time can be traced with STEPPER_TRACE_ISR_TIMING.

void smMotionISR (UInt32 UserTag, UInt32 IntrFlags)
{
    Motion_t            *pMove;
    smParamSet_t        *pParam;

    UInt8               Instance;
    UInt16              CCROffset;

    Encoder_t           *Encoder;
    StepLoss_t          *StepLoss;
    Int32               StepDiff = 0;

    Motor_t             *Motor;

#ifdef STEPPER_TRACE_ISR_TIMING
    isrDuration = halGetFastTick();
#endif

    //############################################################
    // determine the motor instance this interrupt is for
    //############################################################
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

    pMove = &smDataTable[Instance].Motion;
    pParam = &pMove->Param[pMove->ActSet];

    //############################################################
    // reload the capture-compare register with the next value
    //############################################################
    
    //ulTimerValue is the timer intervall in nano seconds
    // each tick has a period from 1.000.000.000 nano seconds per second / 72.000.000 tick per second
    // -> relaod value offset
    // the prescaler was set to 6
    pMove->dt = pParam->dSteps * ISR_TIMEBASE / pMove->Nominal.v;   // elapsed 탎 to perform step
    CCROffset = (pMove->dt * 12);   // processor clock divided by pre-scaler = 72Mhz / 6 = 12 
    
    // The read current value at ccr ihas been stored
    // -> add the time offset, this ensures correct timing of the next irq call
    pMove->CCR.LastValue += CCROffset;

#ifndef SIMULATION
    //write the new value to ccr-register
    // the irq-flag will be cleared inside the 'halCapComWrite' function
    #ifdef STEPPER_ISR_TIME_OPTIMIZATION_OFF
        halCapComWrite(smHandleTimer, Instance, pMove->CCR.LastValue);
    #else
        //halCapComWriteTimer1_Simplified(Instance, pMove->CCR.LastValue);
        halCapComWrite_Simplified(smTimerNo, Instance, pMove->CCR.LastValue);
    #endif
#endif /*SIMULATION*/


    Encoder = &smDataTable[Instance].Encoder;
    StepLoss = &Encoder->StepLoss;
    Motor = &smDataTable[Instance].Motor;


// perform check only at half-step position
    if (0 == (pMove->SinIndex & (ISR_MICROSTEPS_PER_HALFSTEP-1)))
    {
// check encoder position
        if (Encoder->Config.Exists)
        {
            UInt16 ResetPosition = Motor->FrameworkConfig.ResetPosition;

            Int32 MotPos    = pMove->Pos;
            Int32 EncPos    = Encoder->Pos;            
            Int32 LastPos   = StepLoss->LastEncPos;

// take care of overflow for rotatory movement. correct motor, encoder and last encoder position accordingly
            if (0 != ResetPosition)     
            {
                Bool Overflow;  // set if actual position already crossed reset position but encoder not (or vice versa)

                if (pParam->NegPosCount)
                    StepLoss->PosOverflow = StepLoss->PosOverflow || ((((ResetPosition - 1) == MotPos) && (!Encoder->FirstTime)));
                else
                    StepLoss->PosOverflow = StepLoss->PosOverflow || (((0 == MotPos) && (!Encoder->FirstTime)));
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
                        if (pParam->NegPosCount)
                            MotPos -= ResetPosition;        // neg. counting direction -> make motor position neg. number
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
                        else
                            MotPos -= ResetPosition;        // pos. counting direction -> make motor position neg. number
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
            if (pParam->NegPosCount)
                StepDiff = EncPos - MotPos;
            else
                StepDiff = MotPos - EncPos;
//            StepDiff = StepDiff % ResetPosition;

    
// if step difference is getting too large then terminate movement
            if (abs(StepDiff) > Motor->Config.Resolution)
            {                                               
                if (0 != StepLoss->Config.ErrorLimit)
                {
                    pMove->State = MS_IDLE;                 
                }
            }


// check if target position is reached
            if (  (0 == ResetPosition)                      // check always for linear movement
                ||(pMove->Phase > PH_4_VEL_CONST))          // check only during decelaration for rotatory movement
            {
                if (MS_POSITION == pMove->State)            // target position movement not yet terminated because of step difference
                {
                    if (pParam->NegPosCount)
                    {
                        if ((LastPos >= Encoder->TargetPos) && (EncPos <= Encoder->TargetPos))
                            pMove->AtTargetPosition = TRUE; // target position is reached
                    }
                    else
                    {
                        if ((LastPos <= Encoder->TargetPos) && (EncPos >= Encoder->TargetPos))
                            pMove->AtTargetPosition = TRUE; // target position is reached
                    }
                    if (pMove->AtTargetPosition)            
                        pMove->State = MS_IDLE;             // this will terminate movement
                }
            }

// check if target speed 0 is reached
            if (MS_SPEED == pMove->State)                   // target speed movement not yet terminated because of step difference
            {
                if (Encoder->AtSpeedZero)
                {
                    pMove->AtTargetSpeed = TRUE;            
                    pMove->State = MS_IDLE;                 // this will terminate movement
                }
            }

                        
            StepLoss->LastEncPos = Encoder->Pos;


            // check on reliable step difference
            if (pMove->dt > ISR_ENCODER_TIME_LIMIT)         // we can only check step difference if there is enough time to read position info from encoder in main loop
            {
                if (MS_IDLE != pMove->State)                // not yet terminated because of step difference or target position rached
                {
                    if (  (0 != StepLoss->Config.ErrorLimit)    // stop movement if step difference exceeds the error limit
                        &&(abs(StepDiff) >= StepLoss->Config.ErrorLimit))
                    {
                        pMove->State = MS_IDLE;                     
                    }
                    else
                    {                                           // check for step loss
                        if (!StepLoss->Detected)                // so far there was no step loss detected
                        {
                            if (StepDiff >= 8)                  // a step difference of 8 half-steps means one full commutation
                                StepLoss->Detected = TRUE;      // now we have detected a step loss
                            else
                            {
                                if (PH_8_END == pMove->Phase)
                                {
                                    if (MS_POSITION == pMove->State)
                                    {
                                        if (pMove->s >= pParam->Ph[pMove->Phase].ds)    // we should have already reached the target
                                        {                                               // check if actual position deviates from encoder position
                                            if (StepDiff > 0)                   // only step loss can be adjusted
                                            {
                                                    if (pParam->NegPosCount)                
                                                    {
                                                        if (EncPos > MotPos)
                                                            StepLoss->Detected = TRUE;  // detected step loss
                                                    }
                                                    else
                                                    {
                                                        if (EncPos < MotPos)
                                                            StepLoss->Detected = TRUE;  // detected step loss
                                                    }
                                            }
                                            else
                                            {
                                                pMove->State = MS_IDLE;                     
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
                                    pMove->State = MS_IDLE;                     
                                }
                            }
                            if (pParam->NegPosCount)                // check if actual position is in sync again with encoder position
                            {
                                if (EncPos <= MotPos)
                                    StepLoss->Detected = FALSE; // step loss is corrected
                            }
                            else
                            {
                                if (EncPos >= MotPos)
                                    StepLoss->Detected = FALSE; // step loss is corrected
                            }
                        }
                    }
                }
            }
        } /*if (Encoder->Config.Exists)*/
    }


// perform the micro step
    if (MS_IDLE != pMove->State)
    {
        if (!StepLoss->Detected)
        {
            pMove->s += pParam->dSteps;             // this will be the micro step position within actual phase after step have been performed by stepper motor
            pMove->t += pMove->dt;                  // and the time
        }
    
        // move motor by increasing/decreasing sinus index with micro step value
        if (pParam->CCWRotDir)
            pMove->SinIndex -= pParam->dSteps;
        else
            pMove->SinIndex += pParam->dSteps;

        // write value to stepper motor
#ifndef SIMULATION
    #ifdef STEPPER_ISR_TIME_OPTIMIZATION_OFF
        halStepperWrite(Motor->Handle, pMove->SinIndex);
    #else
        halStepperWrite_Simplified(Instance, pMove->SinIndex);
    #endif
#endif /*SIMULATION*/

#ifdef TRACE
        pMove->Nominal.s += pParam->dSteps; // this will be the micro step position after step have been performed by stepper motor
        pMove->Nominal.t += pMove->dt;
        fprintf(stpLog, "%f,%lli,%lli,%lli,%li,%i\n", pMove->Nominal.t/1000.0, pMove->Nominal.s, pMove->Nominal.v, pMove->Nominal.a, pMove->dt, pMove->SinIndex);
//      fflush(stpLog);
#endif /*TRACE*/
    }


// at each half-step position we have to check for special conditions (e.g. phase changes, end position reached ...)
    if (0 == (pMove->SinIndex & (ISR_MICROSTEPS_PER_HALFSTEP-1)))
    {
        if (MS_IDLE != pMove->State)
        {

// trigger sampling of limit switches
            pMove->LSTrigger = TRUE;


// maintain life cycle revolution counter at full-step position
            if (0 == (pMove->SinIndex & (MICROSTEPS_PER_FULLSTEP-1)))
            {
                Motor->LifeCycle.Revolutions.FullSteps++;
                if(Motor->LifeCycle.Revolutions.FullSteps == Motor->Config.Resolution)
                {
                    Motor->LifeCycle.Revolutions.FullSteps = 0;
                    Motor->LifeCycle.Revolutions.Count++;    
                }
            }


// maintain actual position 
            if (!StepLoss->Detected)
            {
                if (pParam->NegPosCount)
                {
                    pMove->Pos--;
                    if (MICROSTEPS_PER_FULLSTEP == pParam->dSteps)
                        pMove->Pos--;
                    if (0 != pMove->ResetPos)
                    {
                        if (pMove->Pos < 0)
                            pMove->Pos = pMove->ResetPos + pMove->Pos;
                    }
                }
                else
                {
                    pMove->Pos++;
                    if (MICROSTEPS_PER_FULLSTEP == pParam->dSteps)
                        pMove->Pos++;
                    if (0 != pMove->ResetPos)
                        pMove->Pos = pMove->Pos % pMove->ResetPos;
                }
        
    #ifdef TRACE
                if (pParam->NegPosCount)
                    fprintf(posLog, "%f,%i,%lli,%lli\n", pMove->Nominal.t/1000.0, pMove->Pos, -pMove->Nominal.v >> 5, -pMove->Nominal.a >> 5);
                else
                    fprintf(posLog, "%f,%i,%lli,%lli\n", pMove->Nominal.t/1000.0, pMove->Pos, pMove->Nominal.v >> 5, pMove->Nominal.a >> 5);
        //      fflush(posLog);
    #endif /*TRACE*/


// check if parameter set switch is pending 
                if (pMove->ActSet != pMove->NewSet)
                {
                    if (pParam->SwitchSet.Trigger.OldPhase == pMove->Phase)
                    {
                        if ((pParam->SwitchSet.Trigger.Position < 0) || (pMove->s == pParam->SwitchSet.Trigger.Position))
                        {
                            pMove->Phase = pParam->SwitchSet.NewPhase;
        
                            pMove->ActSet++;
                            if (NUM_OF_PARAMETERSETS == pMove->ActSet)
                                pMove->ActSet = 0;
                            pParam = &pMove->Param[pMove->ActSet];
        
                            pParam->OffsetPos   = pMove->Pos;
                            pParam->OffsetPhase = pMove->Phase;
        
                            pMove->v0 = pParam->Ph[pMove->Phase].v;
                            pMove->a0 = pParam->Ph[pMove->Phase].a;
        
                            pMove->Nominal.v = pMove->v0;
#ifdef TRACE
                            pMove->Nominal.a = pMove->a0;
                            pMove->Nominal.s = 0;
#endif
        
                            pMove->s = 0;
        
                            pMove->j = pParam->Ph[pMove->Phase].j;
                            pMove->t = pParam->dSteps * ISR_TIMEBASE / pMove->Nominal.v;    // elapsed 탎
        //todo is it ok to use "pMove->t = pMove->dt;" instead of above line ?
                        }
                    }
                }
    
    
// check if movement have to be switched to next phase
                if (pParam->Ph[pMove->Phase].dt >= 0)
                {
                    if (pMove->s >= pParam->Ph[pMove->Phase].ds)
                    {
                        if (PH_8_END == pMove->Phase)                           // target position or target speed 0 is reached 
                        {
                            if (!Encoder->Config.Exists)
                            {                                                   // without encoder stop immediately
                                if (MS_POSITION == pMove->State)
                                    pMove->AtTargetPosition = TRUE;             // target position is reached
                                else
                                    pMove->AtTargetSpeed = TRUE;                // target speed 0 is reached
                                pMove->State = MS_IDLE;                         // this will terminate movement
                            }
                            else
                            {
                                if (MS_POSITION == pMove->State)
                                {
                                    if (pMove->dt <= ISR_ENCODER_TIME_LIMIT)    // terminate movement immediately, if no step loss adjustment is possible
                                    {
                                        pMove->AtTargetPosition = TRUE;         // target position is reached
                                        pMove->State = MS_IDLE;                 // this will terminate movement
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
                            while (pMove->Phase < PH_8_END)     
                            {
                                pMove->Phase++;
                                if (0 != pParam->Ph[pMove->Phase].dt)
                                    break;
                            }
            
                            pMove->v0 = pParam->Ph[pMove->Phase].v;
                            pMove->a0 = pParam->Ph[pMove->Phase].a;
            
                            pMove->Nominal.v = pMove->v0;
#ifdef TRACE
                            pMove->Nominal.a = pMove->a0;
#endif
            
                            pMove->j = pParam->Ph[pMove->Phase].j;
            //              pMove->t = pParam->dSteps * ISR_TIMEBASE / pMove->Nominal.v;    // elapsed 탎
                            pMove->t = pMove->dt;   // elapsed 탎
            
                            pMove->s = 0;
                        }
                    }
                }
            } /*if (!StepLoss->Detected)*/
        } /*if (MS_IDLE != pMove->State)*/


// check for stop condition initiated from outside the ISR
        if (SM_SC_NONE != pMove->Stop)
        {
            switch (pMove->Stop)
            {
            case SM_SC_DIR_CW:              // stop if moving CW
                if (!pParam->CCWRotDir)
                    pMove->State = MS_IDLE; // this will terminate movement
                else
                    pMove->Stop = SM_SC_NONE;
                break;
            case SM_SC_DIR_CCW:             // stop if moving CCW
                if (pParam->CCWRotDir)
                    pMove->State = MS_IDLE; // this will terminate movement
                else
                    pMove->Stop = SM_SC_NONE;
                break;
            case SM_SC_OFFLIMIT:            // stop because position is off-limit
                // break; intentionally omitted
            case SM_SC_ALWAYS:              // stop always
                pMove->State = MS_IDLE;     // this will terminate movement
                break;
            default:                        // don't stop
                break;
            }
        }

// stop movement
        if (MS_IDLE == pMove->State)
        {
#ifndef SIMULATION
            smStopMotion(smDataTable[Instance].Instance);

            if (  (Encoder->Config.Exists)
                &&(0 != StepLoss->Config.ErrorLimit))
            {
                if (StepLoss->Count > StepLoss->Config.ErrorLimit)
                    bmSignalEvent(smDataTable[Instance].Channel, E_SMOT_STOP_BY_STEPLOSS, TRUE, StepLoss->Count);
                else
                {
                    if (StepLoss->Count > StepLoss->Config.WarnLimit)
                        bmSignalEvent(smDataTable[Instance].Channel, W_SMOT_STEPLOSS, TRUE, StepLoss->Count);
                }
                if (  (abs(StepDiff) > Motor->Config.Resolution)
                    ||(abs(StepDiff) > StepLoss->Config.ErrorLimit))
                    bmSignalEvent(smDataTable[Instance].Channel, E_SMOT_STOP_BY_STEPDIFF, TRUE, StepDiff);
                else
                {
                    if (-StepDiff > StepLoss->Config.WarnLimit)
                        bmSignalEvent(smDataTable[Instance].Channel, W_SMOT_STEPDIFF, TRUE, StepDiff);
                }
            }

#endif

//          pMove->Stop = SM_SC_NONE;           // clear stop signal, e.g. from limitswitch
            pMove->Nominal.v = 0;               // set actual speed to zero

            return;
        }
    } /*if (0 == (SinIndex & (ISR_MICROSTEPS_PER_HALFSTEP-1)))*/


// in case of step loss no speed calculation is needed, because actual speed will be used to adjust the step loss
    if (StepLoss->Detected)
        return;


// do phase depending speed calculation
    switch (pMove->Phase)
    {
    case PH_0_START:
        break;

    case PH_1_ACC_JERK_UP:
        pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2000000000000;
//      pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2/ISR_TIMEBASE/ISR_TIMEBASE;
        if (pMove->Nominal.v > pParam->Ph[PH_1_ACC_JERK_UP].vE)
            pMove->Nominal.v = pParam->Ph[PH_1_ACC_JERK_UP].vE;
#ifdef TRACE
        pMove->Nominal.a = pMove->a0 + pMove->j*pMove->t/ISR_TIMEBASE;
        if (pMove->Nominal.a > pParam->Ph[PH_1_ACC_JERK_UP].aE)
            pMove->Nominal.a = pParam->Ph[PH_1_ACC_JERK_UP].aE;
#endif
        break;

    case PH_2_ACC_CONST:
        pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE;
        if (pMove->Nominal.v > pParam->Ph[PH_2_ACC_CONST].vE)
            pMove->Nominal.v = pParam->Ph[PH_2_ACC_CONST].vE;
        break;

    case PH_3_ACC_JERK_DOWN:
        pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2000000000000;
//      pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2/ISR_TIMEBASE/ISR_TIMEBASE;
        if (pMove->Nominal.v > pParam->Ph[PH_3_ACC_JERK_DOWN].vE)
            pMove->Nominal.v = pParam->Ph[PH_3_ACC_JERK_DOWN].vE;
#ifdef TRACE
        pMove->Nominal.a = pMove->a0 + pMove->j*pMove->t/ISR_TIMEBASE;
        if (pMove->Nominal.a < pParam->Ph[PH_3_ACC_JERK_DOWN].aE)
            pMove->Nominal.a = pParam->Ph[PH_3_ACC_JERK_DOWN].aE;
#endif
        break;

    case PH_4_VEL_CONST:
        if (0 == (pMove->SinIndex & (ISR_MICROSTEPS_PER_HALFSTEP-1)))   // do check only at half-step position
            pMove->AtTargetSpeed = (pParam->Ph[PH_4_VEL_CONST].dt < 0); // if endless movement then target speed reached
        if (pMove->AtTargetSpeed && pMove->ResetPos)
            pMove->s = pMove->s % (pMove->ResetPos << 5);
        break;

    case PH_5_DEC_JERK_UP:
        pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2000000000000;
//      pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2/ISR_TIMEBASE/ISR_TIMEBASE;
        if (pMove->Nominal.v < pParam->Ph[PH_5_DEC_JERK_UP].vE)
            pMove->Nominal.v = pParam->Ph[PH_5_DEC_JERK_UP].vE;
#ifdef TRACE
        pMove->Nominal.a = pMove->a0 + pMove->j*pMove->t/ISR_TIMEBASE;
        if (pMove->Nominal.a < pParam->Ph[PH_5_DEC_JERK_UP].aE)
            pMove->Nominal.a = pParam->Ph[PH_5_DEC_JERK_UP].aE;
#endif
        break;

    case PH_6_DEC_CONST:
        pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE;
        if (pMove->Nominal.v < pParam->Ph[PH_6_DEC_CONST].vE)
            pMove->Nominal.v = pParam->Ph[PH_6_DEC_CONST].vE;
        break;

    case PH_7_DEC_JERK_DOWN:
        pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2000000000000;
//      pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2/ISR_TIMEBASE/ISR_TIMEBASE;
        if (pMove->Nominal.v < pParam->Ph[PH_7_DEC_JERK_DOWN].vE)
            pMove->Nominal.v = pParam->Ph[PH_7_DEC_JERK_DOWN].vE;
#ifdef TRACE
        pMove->Nominal.a = pMove->a0 + pMove->j*pMove->t/ISR_TIMEBASE;
        if (pMove->Nominal.a > pParam->Ph[PH_7_DEC_JERK_DOWN].aE)
            pMove->Nominal.a = pParam->Ph[PH_7_DEC_JERK_DOWN].aE;
#endif
        break;

    case PH_8_END:
        if (0 == (pMove->SinIndex & (ISR_MICROSTEPS_PER_HALFSTEP-1)))   // do check only at half-step position
            pMove->AtTargetSpeed = (pParam->Ph[PH_8_END].dt < 0);       // if endless movement then target speed reached
        if (pMove->AtTargetSpeed && pMove->ResetPos)
            pMove->s = pMove->s % (pMove->ResetPos << 5);
        break;

    default:
        break;
    }

#ifdef STEPPER_TRACE_ISR_TIMING
    isrDuration = halGetFastTick() - isrDuration;
    timing[pMove->Phase][isrDuration]++;
#endif
}
