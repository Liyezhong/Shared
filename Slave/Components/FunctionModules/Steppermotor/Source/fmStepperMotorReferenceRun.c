/****************************************************************************/
/*! \file fmStepperMotorReferenceRun.c
 * 
 *  \brief Functions determining the motion control calculation of function module 
 *         'stepper motor'
 *
 *  $Version: $ 0.1
 *  $Date:    $ 17.10.2010
 *  $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This file contains the functions to calculate the movements 
 *       of the 'stepper motor'.     
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
#include "Global.h"
#include "fmStepperMotor.h"
#include "fmStepperMotorReferenceRun.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/



void smInitReferenceRun (ReferenceRun_t *RefRun)
{
    RefRun->ConfigMask = 0;  // nothing configured

    RefRun->Config.RefPos = 0;
    RefRun->Config.Offset = 0;
    RefRun->Config.SlowSpeed = 0;
    RefRun->Config.FastSpeed = 0;
    RefRun->Config.Timeout = 0;
    RefRun->Config.MaxDistance = 0;
    RefRun->Config.ReverseDist = 0;
}


Error_t smConfigureReferenceRun1(ReferenceRun_t *RefRun, ConfigData_REFRUN_P1_t* Param)
{
    RefRun->Config.RefPos = Param->refRun_RefPos;
    DB4_TO_VAL(Param->refRun_PosOffset, RefRun->Config.Offset);
    DB2_TO_VAL(Param->refRun_Timeout, RefRun->Config.Timeout);

    RefRun->ConfigMask |= BIT(0);

	return NO_ERROR;
}


Error_t smConfigureReferenceRun2(ReferenceRun_t *RefRun, ConfigData_REFRUN_P2_t* Param)
{
    DB4_TO_VAL(Param->refRun_MaxDist, RefRun->Config.MaxDistance);
    DB2_TO_VAL(Param->refRun_HighSpeed, RefRun->Config.FastSpeed);

    RefRun->ConfigMask |= BIT(1);

	return NO_ERROR;
}


Error_t smConfigureReferenceRun3(ReferenceRun_t *RefRun, ConfigData_REFRUN_P3_t* Param)
{
    DB4_TO_VAL(Param->refRun_ReverseDist, RefRun->Config.ReverseDist);
    DB2_TO_VAL(Param->refRun_LowSpeed, RefRun->Config.SlowSpeed);

    RefRun->ConfigMask |= BIT(2);

	return NO_ERROR;
}


Bool smReferenceRunConfigIsComplete (ReferenceRun_t *RefRun)
{
    UInt8 CompleteMask = (BIT(0) | BIT(1) | BIT(2));

    return (CompleteMask == RefRun->ConfigMask);
}

                                                                                          
Error_t smCheckReferenceRunConfig(ReferenceRun_t *RefRun, LimitSwitches_t *LimitSwitches)
{
    if (!smReferenceRunConfigIsComplete(RefRun))
        return E_SMOT_CONFIG_INCOMPLETE;

    // check if reference position is valid.
    if (0 == RefRun->Config.RefPos)
        return E_SMOT_CONFIG_REFRUN;
    if ((smPosCodeIsValid(LimitSwitches, RefRun->Config.RefPos) < 0))
        return E_SMOT_CONFIG_REFRUN;
    
    // reverse distance can't exceed max distance
    if (RefRun->Config.ReverseDist > RefRun->Config.MaxDistance)
        return E_SMOT_CONFIG_REFRUN;

    return NO_ERROR;
}



Bool smAtRefPos(smData_t *Data)
{
    return (Data->RefRun.RefPosValue == Data->LimitSwitches.PosCode.Value);
}


void smCheckRefPos(smData_t *Data)
{
    Bool AtRefPos = smAtRefPos(Data);
    // check for leading edge of reference position signal
    if (AtRefPos)
    {
        if (!Data->RefRun.AtRefPos)
        {
            Data->Motion.Stop = SM_SC_ALWAYS;   // emit stop signal
        }
    }

    Data->RefRun.AtRefPos = AtRefPos;    
}


Error_t smCheckRefRunSpeedRequest(smData_t *Data, Bool *Done)
{
    Int32 Distance;

    *Done = FALSE;

    // stop motor if timeout have expired
    if(bmTimeExpired(Data->RefRun.StartTime) > Data->RefRun.Config.Timeout)
        Data->Motion.Stop = SM_SC_ALWAYS;

    // stop motor if max distance is exceeded
    Distance = Data->RefRun.StartPos - Data->Motion.Pos;
    if (Distance < 0)
        Distance = -Distance;
    if(Distance > Data->RefRun.MaxDistance)
        Data->Motion.Stop = SM_SC_ALWAYS;

    smCheckRefPos(Data);

    // check if motor already stopped
    if (MS_IDLE != Data->Motion.State)
        return NO_ERROR;

    // check if timeout have not expired
    if(bmTimeExpired(Data->RefRun.StartTime) > Data->RefRun.Config.Timeout)
        return E_SMOT_REFRUN_TIMEOUT;

    // check if max distance is exceeded
    if(Distance > Data->RefRun.MaxDistance)
        return E_SMOT_REFRUN_MAXDISTANCE;

    // check if correct limit switch is active
    if (!smAtRefPos(Data))
        return E_SMOT_REFRUN_INVALID_LS;    // motor was not stopped by reference position

    // done, motor stopped at reference position
    *Done = TRUE;
    return NO_ERROR;
}


Error_t smRefRunSpeedRequest(smData_t *Data, Int16 Speed, ReferenceRunState_t State)
{
    Data->RefRun.State = State;
    Data->RefRun.StartTime = bmGetTime();
    Data->RefRun.StartPos = Data->Motion.Pos;

    if (Data->RefRun.RotationDir != Data->Motor.FrameworkConfig.RotationDir)
//    if (Motion->RefRun.Config.RotationDir != Motion->Motor.Config.RotationDir)
        Speed = -Speed;
    return smSpeedRequest (Data->Instance, Speed, Data->RefRun.Profile);
}


Error_t smCheckRefRunPositionRequest(smData_t *Data, Bool *Done)
{
    *Done = FALSE;

    // stop motor if timeout have expired
    if(bmTimeExpired(Data->RefRun.StartTime) > Data->RefRun.Config.Timeout)
        Data->Motion.Stop = SM_SC_ALWAYS;

    smCheckRefPos(Data);

    // check if motor already stopped
    if (MS_IDLE != Data->Motion.State)
        return NO_ERROR;

    // check if timeout have not expired
    if(bmTimeExpired(Data->RefRun.StartTime) > Data->RefRun.Config.Timeout)
        return E_SMOT_REFRUN_TIMEOUT;

    // check if limit switch is active (none should be active)
    if (0 != Data->LimitSwitches.PosCode.Value)
        return E_SMOT_REFRUN_INVALID_LS;    // motor was stopped by limit switch

    // done, motor stopped at reference position
    *Done = TRUE;
    return NO_ERROR;
}


Error_t smRefRunPositionRequest(smData_t *Data, Int32 Position, ReferenceRunState_t State)
{
    Data->RefRun.State = State;
    Data->RefRun.StartTime = bmGetTime();
    Data->RefRun.StartPos = Data->Motion.Pos;

    if (Data->RefRun.RotationDir == Data->Motor.FrameworkConfig.RotationDir)
//    if (Motion->RefRun.Config.RotationDir == Motion->Motor.Config.RotationDir)
        Position = -Position;
    return smPositionRequest (Data->Instance, Position, Data->RefRun.Profile);
}


/******************************************************************************/
/*! 
 *  \brief   Handles the reference run procedure
 *
 *           The reference run is subdivide into three motion cycles
 *             - move with 'high' speed into the limit switch which is
 *               configured for reference position detection
 *             - move out of this limit switch just a small position offset
 *             - with low speed for exact postion detection, move into the limit switch again.
 *           After determining the reference position a position offset is calculated to it. 
 *           This enables the module to use a basis position not equal to reference position.
 *  
 * The reference run task follows the task states:
 *   - SM_TASK_STATE_REF_RUN_REQ                      Request via can message received
 *   - SM_TASK_STATE_REF_RUN_FAST_MOTION_START        Move the motor with 'high' speed to limit switch
 *   - SM_TASK_STATE_REF_RUN_FAST_MOTION_FINISHED     High speed movement has finished, limit switch was hit, set 'approximated' reference position
 *   - SM_TASK_STATE_REF_RUN_REVERSE_MOTION_START     Move motor reverse
 *   - SM_TASK_STATE_REF_RUN_REVERSE_MOTION_FINISHED  Revers movemtn has finishec
 *   - SM_TASK_STATE_REF_RUN_SLOW_MOTION_START        Move the motor with 'slow' speed to limit switch
 *   - SM_TASK_STATE_REF_RUN_SLOW_MOTION_FINISHED     Slow speed movement has finished, limit switch was hit, set reference position 'exactly'
 *   - SM_TASK_STATE_REF_RUN_ERROR
 *
 *  \xparam  Data = Module instance data pointer
 *
 *  \return  NO_ERROR or error code
 *
 ******************************************************************************/    
Error_t smReferenceRunTask(UInt16 Instance)
{
    smData_t        *Data   = &smDataTable[Instance];
    ReferenceRun_t  *RefRun = &Data->RefRun;

    Error_t RetCode = NO_ERROR;

    Bool    Done;
    
    switch (RefRun->State)
    {
    case SM_RRS_FAST_MOTION_START:       //!< start reference run fast motion to limit switch
        Data->Motion.Pos = 0;
        smInitEncoderPos(&Data->Encoder, Data->Motor.Config.Resolution, Data->Motion.Pos);

        // figure out rotation direction to move towards the reference position
        RefRun->RefPosValue = RefRun->Config.RefPos;
        RefRun->RotationDir = Data->LimitSwitches.PosCodeConfig[RefRun->Config.RefPos].StopDir;

        // if already standing at reference position then start with movement to reverse distance
        RefRun->AtRefPos = smAtRefPos(Data);
        if (RefRun->AtRefPos)
        {
            RefRun->State = SM_RRS_REVERSE_MOTION_START;
            break;
        }

        // if not standing at reference position then start with fast speed movement towards reference position
        RefRun->MaxDistance = RefRun->Config.MaxDistance;
        RetCode = smRefRunSpeedRequest(Data, RefRun->Config.FastSpeed, SM_RRS_FAST_MOTION);

        break;

    case SM_RRS_FAST_MOTION:             //!< reference run fast motion is active
        if ((RetCode = smCheckRefRunSpeedRequest (Data, &Done)) >= 0)
        {
            if (Done)
                RefRun->State = SM_RRS_REVERSE_MOTION_START;
        }
        break;

    case SM_RRS_REVERSE_MOTION_START:    //!< start reference run reverse motion
        // skip second run if slow speed is zero
        // or reverse distance is zero
        if ((0 == RefRun->Config.ReverseDist) || (0 == RefRun->Config.SlowSpeed))      
        {
            // assign offset to actual position
            Data->Motion.Pos = RefRun->Config.Offset;
            RefRun->State = SM_RRS_FINISHED;
        }
        else
        {
            // move to reverse distance
            Data->Motion.Pos = 0;
            smInitEncoderPos(&Data->Encoder, Data->Motor.Config.Resolution, Data->Motion.Pos);
            RetCode = smRefRunPositionRequest(Data, RefRun->Config.ReverseDist, SM_RRS_REVERSE_MOTION);
        }
        break;

    case SM_RRS_REVERSE_MOTION:         //!< reference run reverse motion is active
        if ((RetCode = smCheckRefRunPositionRequest (Data, &Done)) >= 0)
        {
            if (Done)
                RefRun->State = SM_RRS_SLOW_MOTION_START;
        }
        break;

    case SM_RRS_SLOW_MOTION_START:       //!< start reference run slow motion to limit switch
        RefRun->MaxDistance = RefRun->Config.ReverseDist * 3 / 2;   // for distance supervision use 150% of reverse distance
        RetCode = smRefRunSpeedRequest(Data, RefRun->Config.SlowSpeed, SM_RRS_SLOW_MOTION);
        break;

    case SM_RRS_SLOW_MOTION:    //!< reference run slow motion is active
        if ((RetCode = smCheckRefRunSpeedRequest (Data, &Done)) >= 0)
        {
            if (Done)
            {
                // assign offset to actual position to initialize the reference framework
                Data->Motion.Pos = RefRun->Config.Offset;
                smInitEncoderPos(&Data->Encoder, Data->Motor.Config.Resolution, Data->Motion.Pos);
                RefRun->State = SM_RRS_FINISHED;
            }
        }
        break;

    case SM_RRS_FINISHED:    //!< reference run sucessfully finished
        break;

    default:
        break;
    }

    return RetCode;
}
