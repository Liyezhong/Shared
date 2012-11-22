

#include "Global.h"

#include "fmStepperMotor.h"

#include "fmStepperMotorMotionISR.h"

#ifndef SIMULATION
	#include "halTimer.h"
	#include "halStepper.h"

#include "halSysTick.h"
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



// optimization can be switched off with STEPPER_ISR_TIME_OPTIMIZATION_OFF
// stepper ISR execution time can be traced with STEPPER_TRACE_ISR_TIMING.

void smMotionISR (UInt32 UserTag, UInt32 IntrFlags)
{
	Motion_t			*pMove;
	smParamSet_t	    *pParam;

	UInt8				Instance;
    UInt16				CCROffset;
    Int32               SinIndex;   // Sinus table index

	Encoder_t			*Encoder;
    Int32               StepDiff;

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
		Instance = 0;
//		return;
	}

    pMove = &smDataTable[Instance].Motion;

    //############################################################
    // reload the capture-compare register with the next value
    //############################################################
    
    //ulTimerValue is the timer intervall in nano seconds
    // each tick has a period from 1.000.000.000 nano seconds per second / 72.000.000 tick per second
    // -> relaod value offset
    // the prescaler was set to 6
	pParam = &pMove->Param[pMove->ActSet];
	pMove->dt = pParam->dSteps * ISR_TIMEBASE / pMove->Nominal.v;	// elapsed 탎 to perform step
    CCROffset = (pMove->dt * 12);   // processor clock divided by pre-scaler = 72Mhz / 6 = 12 
    
    // The read current value at ccr ihas been stored
    // -> add the time offset, this ensures correct timing of the next irq call
    pMove->CCR.LastValue += CCROffset;

    if (!pMove->StepLoss)
    {
    	pMove->s += pParam->dSteps;             // this will be the micro step position within actual phase after step have been performed by stepper motor
    	pMove->t += pMove->dt;                  // and the time
    }
    else
    {
        // keep motor rotating
        if (pParam->CCWRotDir)
            pMove->SinOffset -= pParam->dSteps;     
        else
            pMove->SinOffset += pParam->dSteps;
    }

#ifndef SIMULATION
    //write the new value to ccr-register
    // the irq-flag will be cleared inside the 'halCapComWrite' function
    #ifdef STEPPER_ISR_TIME_OPTIMIZATION_OFF
        halCapComWrite(smHandleTimer, Instance, pMove->CCR.LastValue);
    #else
        halCapComWriteTimer1_Simplified(Instance, pMove->CCR.LastValue);
    #endif
#endif /*SIMULATION*/

    // write micro-step value to stepper motor
	if (pParam->CCWRotDir)
    	SinIndex = pMove->SinOffset - pMove->s;
	else
	    SinIndex = pMove->SinOffset + pMove->s;
	SinIndex = SinIndex & 0xff;
#ifndef SIMULATION
    #ifdef STEPPER_ISR_TIME_OPTIMIZATION_OFF
    	halStepperWrite(smDataTable[Instance].Motor.Handle, SinIndex);
    #else
        halStepperWrite_Simplified(Instance, SinIndex);
    #endif
#endif /*SIMULATION*/



#ifdef TRACE
	pMove->Nominal.s += pParam->dSteps; // this will be the micro step position after step have been performed by stepper motor
	pMove->Nominal.t += pMove->dt;
	fprintf(stpLog, "%f,%lli,%lli,%lli,%li,%i\n", pMove->Nominal.t/1000.0, pMove->Nominal.s, pMove->Nominal.v, pMove->Nominal.a, pMove->dt, SinIndex);
//	fflush(stpLog);
#endif /*TRACE*/

// at each half-step position we have to check for special conditions (e.g. phase changes, end position reached ...)
	if (0 == (SinIndex & (ISR_MICROSTEPS_PER_HALFSTEP-1)))
	{

// trigger sampling of limit switches
        pMove->LSTrigger = TRUE;
                                                                                                                                                                                                                                                                                                               

// check encoder position
        Encoder = &smDataTable[Instance].Encoder;
        if (Encoder->Config.Exists)
        {
            UInt16 ResetPosition = smDataTable[Instance].Motor.FrameworkConfig.ResetPosition;

            // calculate absolute difference between encoder and nominal position
            if (pMove->Pos > Encoder->Pos)
                StepDiff = pMove->Pos - Encoder->Pos;
            else
                StepDiff = Encoder->Pos - pMove->Pos;
            if (0 != ResetPosition)                         // take care of overflow for rotatory movement
            {
                if (StepDiff > (ResetPosition >> 1))        // we assume the shorter of both possible distances as the valid one
                    StepDiff = ResetPosition - StepDiff; 
            }

            if (StepDiff > 300)
            {
                pMove->State = MS_IDLE;                     // if step difference is getting too large then terminate movement
                if (pMove->StepLoss)
                    pMove->AccLostSteps += pMove->LostSteps;
            }

            else
            {
                if (pMove->dt > ISR_ENCODER_TIME_LIMIT)     // we can only adjust step loss if there is enough time to read position info from encoder in main loop
                {
                    if (!pMove->StepLoss)                   // so far there was no step loss detected
                    {
                        if (StepDiff >= 8)                  // a step difference of 8 half-steps means one full commutation
                            pMove->StepLoss = TRUE;         // now we have detected a step loss
                    }
                    else                                    // step loss have been detect before
                    {
                        if (pParam->NegPosCount)            // check if actual position is in sync again with encoder position
                        {
                            if (Encoder->Pos <= pMove->Pos)
                                pMove->StepLoss = FALSE;    // step loss is corrected
                        }
                        else
                        {
                            if (Encoder->Pos >= pMove->Pos)
                                pMove->StepLoss = FALSE;    // step loss is corrected

                        }
//                        if (   ((pParam->NegPosCount) && (Encoder->Pos <= pMove->Pos))
//                            || ((!pParam->NegPosCount) && (Encoder->Pos >= pMove->Pos)))
                        if (!pMove->StepLoss)               // count the lost steps
                        {
                            pMove->AccLostSteps += pMove->LostSteps;
                            pMove->LostSteps = 0;
                        }
                        else
                            pMove->LostSteps++;
                    }
                }
            }
        }


        if (!pMove->StepLoss)
        {
    // maintain actual position 
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
    //	    fflush(posLog);
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
    					pMove->SinOffset = SinIndex;                                    // remember actual offset into sinus table
    
    					pMove->j = pParam->Ph[pMove->Phase].j;
    					pMove->t = pParam->dSteps * ISR_TIMEBASE / pMove->Nominal.v;	// elapsed 탎
    //todo is it ok to use "pMove->t = pMove->dt;" instead of above line ?
    				}
    			}
    		}


// check if movement have to be switched to next phase
    		if (pParam->Ph[pMove->Phase].dt >= 0)
    		{
    			if (pMove->s >= pParam->Ph[pMove->Phase].ds)
    			{
                    if (PH_8_END == pMove->Phase)   // stop the movement if target is reached 
                    {
    		            if (MS_POSITION == pMove->State)
                            pMove->AtTargetPosition = TRUE; // target position is reached
                        else
                            pMove->AtTargetSpeed = TRUE;    // target speed 0 is reached
                        pMove->State = MS_IDLE;             // this will terminate movement
                    }
                    else                            // use next phase
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
        //				pMove->t = pParam->dSteps * ISR_TIMEBASE / pMove->Nominal.v;	// elapsed 탎
                        pMove->t = pMove->dt;	// elapsed 탎
        
        				pMove->s = 0;
                        pMove->SinOffset = SinIndex;                                    // remember actual offset into sinus table
                    }
    			}
    		}
        } /*if (!pMove->StepLoss)*/


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
#endif
//    		pMove->Stop = SM_SC_NONE;           // clear stop signal, e.g. from limitswitch
            pMove->SinOffset = SinIndex;        // remember actual offset into sinus table
            pMove->Nominal.v = 0;               // set actual speed to zero
			return;
		}
	} /*if (0 == (SinIndex & (ISR_MICROSTEPS_PER_HALFSTEP-1)))*/


// in case of step loss no speed calculation is needed, because actual speed will be used to adjust the step loss
    if (pMove->StepLoss)
        return;


// do phase depending speed calculation
	switch (pMove->Phase)
	{
    case PH_0_START:
		break;

    case PH_1_ACC_JERK_UP:
		pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2000000000000;
//		pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2/ISR_TIMEBASE/ISR_TIMEBASE;
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
//		pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2/ISR_TIMEBASE/ISR_TIMEBASE;
		if (pMove->Nominal.v > pParam->Ph[PH_3_ACC_JERK_DOWN].vE)
			pMove->Nominal.v = pParam->Ph[PH_3_ACC_JERK_DOWN].vE;
#ifdef TRACE
		pMove->Nominal.a = pMove->a0 + pMove->j*pMove->t/ISR_TIMEBASE;
		if (pMove->Nominal.a < pParam->Ph[PH_3_ACC_JERK_DOWN].aE)
			pMove->Nominal.a = pParam->Ph[PH_3_ACC_JERK_DOWN].aE;
#endif
		break;

    case PH_4_VEL_CONST:
	    if (0 == (pMove->s & (ISR_MICROSTEPS_PER_HALFSTEP-1)))          // do check only at half-step position
		    pMove->AtTargetSpeed = (pParam->Ph[PH_4_VEL_CONST].dt < 0); // if endless movement then target speed reached
        if (pMove->AtTargetSpeed && pMove->ResetPos)
            pMove->s = pMove->s % (pMove->ResetPos << 5);
		break;

    case PH_5_DEC_JERK_UP:
		pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2000000000000;
//		pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2/ISR_TIMEBASE/ISR_TIMEBASE;
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
//		pMove->Nominal.v = pMove->v0 + pMove->a0*pMove->t/ISR_TIMEBASE + pMove->j*pMove->t*pMove->t/2/ISR_TIMEBASE/ISR_TIMEBASE;
		if (pMove->Nominal.v < pParam->Ph[PH_7_DEC_JERK_DOWN].vE)
			pMove->Nominal.v = pParam->Ph[PH_7_DEC_JERK_DOWN].vE;
#ifdef TRACE
		pMove->Nominal.a = pMove->a0 + pMove->j*pMove->t/ISR_TIMEBASE;
		if (pMove->Nominal.a > pParam->Ph[PH_7_DEC_JERK_DOWN].aE)
			pMove->Nominal.a = pParam->Ph[PH_7_DEC_JERK_DOWN].aE;
#endif
		break;

    case PH_8_END:
	    if (0 == (pMove->s & (ISR_MICROSTEPS_PER_HALFSTEP-1)))      // do check only at half-step position
		    pMove->AtTargetSpeed = (pParam->Ph[PH_8_END].dt < 0);   // if endless movement then target speed reached
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
