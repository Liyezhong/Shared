/****************************************************************************/
/*! \file fmStepperMotorReferenceRun.c
 * 
 *  \brief  Functions to configure and perform reference run of the stepper motor
 *
 *  $Rev:    $ 0.1
 *  $Date:   $ 11.06.2012
 *  $Author: $ Rainer Boehles
 *
 *  \b Description:
 *
 *      This module contains functions to configure and perform a reference run
 *      of the stepper motor.
 *      Reference run uses target speed and target position movement to move
 *      to the reference position. After run is finished successfully the
 *      stepper motor reference framework is initialized with the configured
 *      offset value and the stepper module is ready to except target movement
 *      commands from the master.
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

#include "Global.h"
#include "fmStepperMotor.h"
#include "fmStepperMotorReferenceRun.h"
#include <stdio.h>

/******************************************************************************/
/*! 
 *  \brief  Initialize reference run data
 *
 *      Reference run data is initialized.
 *      All configuration data is set to 'unconfigured', which means all
 *      configuration data have to be set first by master, before reference run
 *      can be executed.
 * 
 *  \iparam  RefRun = pointer to reference run data
 * 
 ******************************************************************************/
void smInitReferenceRun (smReferenceRun_t *RefRun) {

    RefRun->ConfigMask = 0;  // nothing configured

    RefRun->Config.RefPos = 0;
    RefRun->Config.Offset = 0;
    RefRun->Config.SlowSpeed = 0;
    RefRun->Config.FastSpeed = 0;
    RefRun->Config.Timeout = 0;
    RefRun->Config.MaxDistance = 0;
    RefRun->Config.ReverseDist = 0;
}


/******************************************************************************/
/*! 
 *  \brief  Set reference run configuration (part 1)
 *
 *      Configuration data received from master is applied to the reference run
 *      parameters.
 * 
 *  \iparam  RefRun = pointer to reference run data
 *  \iparam  Param  = pointer to configuration data received from master
 * 
 *  \return  NO_ERROR
 *
 ******************************************************************************/
Error_t smConfigureReferenceRun1(smReferenceRun_t *RefRun, ConfigData_REFRUN_P1_t* Param) {

    RefRun->Config.RefPos = Param->refRun_RefPos;
    DB4_TO_VAL(Param->refRun_PosOffset, RefRun->Config.Offset);
    DB2_TO_VAL(Param->refRun_Timeout, RefRun->Config.Timeout);

    RefRun->ConfigMask |= BIT(0);

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Set reference run configuration (part 2)
 *
 *      Configuration data received from master is applied to the reference run
 *      parameters.
 * 
 *  \iparam  RefRun = pointer to reference run data
 *  \iparam  Param  = pointer to configuration data received from master
 * 
 *  \return  NO_ERROR
 *
 ******************************************************************************/
Error_t smConfigureReferenceRun2(smReferenceRun_t *RefRun, ConfigData_REFRUN_P2_t* Param) {

    DB4_TO_VAL(Param->refRun_MaxDist, RefRun->Config.MaxDistance);
    DB2_TO_VAL(Param->refRun_HighSpeed, RefRun->Config.FastSpeed);

    RefRun->ConfigMask |= BIT(1);

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Set reference run configuration (part 3)
 *
 *      Configuration data received from master is applied to the reference run
 *      parameters.
 * 
 *  \iparam  RefRun = pointer to reference run data
 *  \iparam  Param  = pointer to configuration data received from master
 * 
 *  \return  NO_ERROR
 *
 ******************************************************************************/
Error_t smConfigureReferenceRun3(smReferenceRun_t *RefRun, ConfigData_REFRUN_P3_t* Param) {

    DB4_TO_VAL(Param->refRun_ReverseDist, RefRun->Config.ReverseDist);
    DB2_TO_VAL(Param->refRun_LowSpeed, RefRun->Config.SlowSpeed);

    RefRun->ConfigMask |= BIT(2);

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Check completeness of reference run configuration parameters
 *
 *      Check if all necessary configuration data for reference run have been received
 * 
 *  \iparam  RefRun = pointer to reference run data
 * 
 *  \return  true, if configuration is complete
 *
 ******************************************************************************/
Bool smReferenceRunConfigIsComplete (smReferenceRun_t *RefRun) {

    UInt8 CompleteMask = (BIT(0) | BIT(1) | BIT(2));

    return (CompleteMask == RefRun->ConfigMask);
}


/******************************************************************************/
/*! 
 *  \brief  Check completeness and validate reference run configuration
 *
 *      Check if all necessary configuration data for reference run have been
 *      received and is plausible.
 *
 *  \iparam  RefRun         = pointer to reference run data
 *  \iparam  LimitSwitches  = pointer to limit switches data
 *  \iparam  Motor          = pointer to motor data
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smCheckReferenceRunConfig(smReferenceRun_t *RefRun, smLimitSwitches_t *LimitSwitches, smMotor_t *Motor) {

    if (!smReferenceRunConfigIsComplete(RefRun)) {
        return E_SMOT_CONFIG_INCOMPLETE;
    }

    // check if reference position is valid.
    if (0 != RefRun->Config.RefPos) {
        if ((smPosCodeIsValid(LimitSwitches, RefRun->Config.RefPos) < 0)) {
            return E_SMOT_CONFIG_REFRUN;
        }
    }

    // check if offset is valid
    if (0 != Motor->FrameworkConfig.ResetPosition) {
        if ((RefRun->Config.Offset < 0) || (RefRun->Config.Offset >= Motor->FrameworkConfig.ResetPosition)) {
            return E_SMOT_CONFIG_REFRUN;
        }
    }
    
    // reverse distance can't exceed max distance
    if (RefRun->Config.ReverseDist > RefRun->Config.MaxDistance) {
        return E_SMOT_CONFIG_REFRUN;
    }

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Check if motor have reached reference position
 *
 *      Check if position code indicates that motor have reached reference position.
 *
 *  \xparam  Data = Module instance data pointer
 *  \iparam  bPrint = Enable/Disable printing debug info
 * 
 *  \return  true, if motor have reached reference position
 *
 ******************************************************************************/
Bool smAtRefPos(smData_t *Data, Bool bPrint)
{
    if (Data->RefRun.RefPosValue == Data->LimitSwitches.PosCode.Value)
    {
        return TRUE;
    }
    else
    {
        //if (bPrint)
        //{
        //    printf("RP:%d LP:%d\n", Data->RefRun.RefPosValue, Data->LimitSwitches.PosCode.Value);
        //}
        return FALSE;
    }
    //return (Data->RefRun.RefPosValue == Data->LimitSwitches.PosCode.Value);
}


/******************************************************************************/
/*! 
 *  \brief  Stop motor if motor have reached reference position
 *
 *      Check if position code indicates that motor have just reached reference
 *      position. If this is the case then stop the motor.
 *
 *  \xparam  Data = Module instance data pointer
 * 
 ******************************************************************************/
void smCheckRefPos(smData_t *Data) {

    Bool AtRefPos = smAtRefPos(Data, FALSE);
    // check for leading edge of reference position signal
    if (AtRefPos) {
        if (!Data->RefRun.AtRefPos) {
            Data->Motion.Stop = SM_SC_ALWAYS;   // emit stop signal
        }
    }

    Data->RefRun.AtRefPos = AtRefPos;
}


/******************************************************************************/
/*! 
 *  \brief  Check if speed position movement for reference run is done
 *
 *      While target speed movement is active timeout, traveled distance and
 *      reference position are checked. If reference position is reached within
 *      timeout then reference run is switched to next state, otherwise reference
 *      run is aborted with an error.
 *
 *  \iparam  Data = stepper instance data pointer
 *  \oparam  Done = pointer to done flag, set to true if movement does not fail
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smCheckRefRunSpeedRequest(smData_t *Data, Bool *Done) {

    Int32 Distance;

    *Done = FALSE;

    // stop motor if timeout have expired
    if(bmTimeExpired(Data->RefRun.StartTime) > Data->RefRun.Config.Timeout) {
        Data->Motion.Stop = SM_SC_ALWAYS;
    }

    // stop motor if max distance is exceeded
    Distance = Data->RefRun.StartPos - Data->Motion.Pos;
    if (Distance < 0) {
        Distance = -Distance;
    }
    if((UInt32)Distance > Data->RefRun.MaxDistance) {
        Data->Motion.Stop = SM_SC_ALWAYS;
    }

    smCheckRefPos(Data);

    // check if motor already stopped
    if (MS_IDLE != Data->Motion.State) {
        return NO_ERROR;
    }

    // check if timeout have not expired
    if(bmTimeExpired(Data->RefRun.StartTime) > Data->RefRun.Config.Timeout) {
        return E_SMOT_REFRUN_TIMEOUT;
    }

    // check if max distance is exceeded
    if((UInt32)Distance > Data->RefRun.MaxDistance) {
        return E_SMOT_REFRUN_MAXDISTANCE;
    }

    // check if correct limit switch is active
    if (!smAtRefPos(Data, TRUE)) {
        //printf("SM E4\n");
        return E_SMOT_REFRUN_ERROR;     // motor was not stopped by reference position
    }

    // done, motor stopped at reference position
    *Done = TRUE;
    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Start target speed movement for reference run
 *
 *      Starts target speed movement to move towards reference point
 *      (step 1 and 3 of reference run).
 *
 *  \iparam  Data   = stepper instance data pointer
 *  \iparam  Speed  = target speed
 *  \iparam  State  = new reference run state to be applied
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smRefRunSpeedRequest(smData_t *Data, Int16 Speed, smReferenceRunState_t State) {

    Data->RefRun.State = State;
    Data->RefRun.StartTime = bmGetTime();
    Data->RefRun.StartPos = Data->Motion.Pos;

    if (Data->RefRun.RotationDir != Data->Motor.FrameworkConfig.RotationDir) {
        Speed = -Speed;
    }
    return smSpeedRequest (Data->Instance, Speed, Data->RefRun.Profile);
}


/******************************************************************************/
/*! 
 *  \brief  Check if target position movement for reference run is done
 *
 *      While target position movement is active timeout and reference position
 *      are checked. If reference position is reached within timeout then
 *      reference run is switched to next state, otherwise reference run is
 *      aborted with an error.
 *
 *  \iparam  Data = stepper instance data pointer
 *  \oparam  Done = pointer to done flag, set to true if movement does not fail
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smCheckRefRunPositionRequest(smData_t *Data, Bool *Done) {

    *Done = FALSE;

    // stop motor if timeout have expired
    if(bmTimeExpired(Data->RefRun.StartTime) > Data->RefRun.Config.Timeout) {
        Data->Motion.Stop = SM_SC_ALWAYS;
    }

    smCheckRefPos(Data);

    // check if motor already stopped
    if (MS_IDLE != Data->Motion.State) {
        return NO_ERROR;
    }

    // check if timeout have not expired
    if(bmTimeExpired(Data->RefRun.StartTime) > Data->RefRun.Config.Timeout) {
        return E_SMOT_REFRUN_TIMEOUT;
    }

    // check if limit switch is active (none should be active)
    if (0 != Data->LimitSwitches.PosCode.Value) {
        return E_SMOT_REFRUN_ERROR;     // motor was stopped unexpectedly
    }

    // done, motor stopped at reference position
    *Done = TRUE;
    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Start target position movement for reference run
 *
 *      Starts target position movement to move away the reverse distance
 *      (step 2 of reference run).
 *
 *  \iparam  Data       = stepper instance data pointer
 *  \iparam  Position   = distance
 *  \iparam  State      = new reference run state to be applied
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smRefRunPositionRequest(smData_t *Data, Int32 Position, smReferenceRunState_t State) {

    Data->RefRun.State = State;
    Data->RefRun.StartTime = bmGetTime();
    Data->RefRun.StartPos = Data->Motion.Pos;

    if (Data->RefRun.RotationDir == Data->Motor.FrameworkConfig.RotationDir) {
        Position = -Position;
    }
    return smPositionRequest (Data->Instance, Position, Data->RefRun.Profile);
}


/******************************************************************************/
/*! 
 *  \brief  Perform reference run
 *
 *      This routine is called periodically from the stepper module task function,
 *      to perform a reference run. Reference run is needed to initialize the 
 *      reference framework.
 *
 *      Reference run is done in 3 steps:
 *          1.) move with fast speed into direction of reference position
 *          2.) when reference position is reached move away the reverse distance
 *          3.) move with slow speed into direction of reference position
 *      Step 1 is skipped if limit switches indicate that reference run is started
 *      at the reference run position.
 *      Step 2 and 3 can be skipped by configuration settings.
 *      Step 1 and 3 uses target speed command, step 2 uses target position cmd.
 *      Start, mentioned steps and final success /fail are handled and indicated
 *      by the reference run state.
 *
 *      During reference run maximal distance and timeout are checked, to avoid
 *      endless reference run movement.
 *
 *  \iparam  Instance       = Module instance number
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smReferenceRunTask(UInt16 Instance) {

    smData_t         *Data   = &smDataTable[Instance];
    smReferenceRun_t *RefRun = &Data->RefRun;

    Error_t RetCode = NO_ERROR;

    Bool    Done;
    
    switch (RefRun->State) {
    case SM_RRS_FAST_MOTION_START:       //!< start reference run fast motion to limit switch
        // wait if stable position code is not available yet
        if (POSCODE_UNDEFINED == Data->LimitSwitches.PosCode.Value) {
            break;
        }

        Data->Motion.Pos = 0;
        smInitEncoderPos(&Data->Encoder, Data->Motor.Config.Resolution, Data->Motion.Pos);

        // figure out rotation direction to move towards the reference position
        RefRun->RefPosValue = RefRun->Config.RefPos;
        RefRun->RotationDir = Data->LimitSwitches.PosCodeConfig[RefRun->Config.RefPos].StopDir;

        // reset position errors
        if (E_SMOT_INVALID_POSCODE_POSITION == Data->LimitSwitches.PosCode.ErrCode) {
            UpdatePosCodeErrStatus (&Data->LimitSwitches.PosCode, NO_ERROR, Data->Channel);
        }

        // if already standing at reference position then start with movement to reverse distance
        RefRun->AtRefPos = smAtRefPos(Data, FALSE);
        if (RefRun->AtRefPos) {
            if (Data->LimitSwitches.PosCodeConfig[RefRun->Config.RefPos].HitSkip == 0) {
            RefRun->State = SM_RRS_REVERSE_MOTION_START;
            break;
            }
        }

        // if not standing at reference position then start with fast speed movement towards reference position
        RefRun->MaxDistance = RefRun->Config.MaxDistance;
        RetCode = smRefRunSpeedRequest(Data, RefRun->Config.FastSpeed, SM_RRS_FAST_MOTION);

        break;

    case SM_RRS_FAST_MOTION:             //!< reference run fast motion is active
        //printf("ref_fast\n");
        if ((RetCode = smCheckRefRunSpeedRequest (Data, &Done)) >= 0) {
            if (Done) {
                RefRun->State = SM_RRS_REVERSE_MOTION_START;
            }
        }
        //else
        //{
            //printf("SM E1\n");
        //}
        break;

    case SM_RRS_REVERSE_MOTION_START:    //!< start reference run reverse motion
        //printf("ref_rev_start\n");
        // skip second run if slow speed is zero
        // or reverse distance is zero
        if ((0 == RefRun->Config.ReverseDist) || (0 == RefRun->Config.SlowSpeed)) {
            RefRun->State = SM_RRS_ASSIGN_OFFSET;
        }
        else {
            // move to reverse distance
            Data->Motion.Pos = 0;
            smInitEncoderPos(&Data->Encoder, Data->Motor.Config.Resolution, Data->Motion.Pos);
            RetCode = smRefRunPositionRequest(Data, RefRun->Config.ReverseDist, SM_RRS_REVERSE_MOTION);
        }
        break;

    case SM_RRS_REVERSE_MOTION:         //!< reference run reverse motion is active
        //printf("SM_RRS_REVERSE_MOTION\n");
        if ((RetCode = smCheckRefRunPositionRequest (Data, &Done)) >= 0) {
            if (Done) {
                RefRun->State = SM_RRS_SLOW_MOTION_START;
            }
        }
        break;

    case SM_RRS_SLOW_MOTION_START:       //!< start reference run slow motion to limit switch
        //printf("SM_RRS_SLOW_MOTION_START\n");
        RefRun->MaxDistance = RefRun->Config.ReverseDist * 3 / 2;   // for distance supervision use 150% of reverse distance
        RetCode = smRefRunSpeedRequest(Data, RefRun->Config.SlowSpeed, SM_RRS_SLOW_MOTION);
        break;

    case SM_RRS_SLOW_MOTION:    //!< reference run slow motion is active
        //printf("SM_RRS_SLOW_MOTION\n");
        if ((RetCode = smCheckRefRunSpeedRequest (Data, &Done)) >= 0) {
            if (Done) {
                RefRun->State = SM_RRS_ASSIGN_OFFSET;
            }
        }
        break;

    case SM_RRS_ASSIGN_OFFSET:          //!< reference run done, assign offset value
        // assign offset to actual position to initialize the reference framework
        Data->Motion.Pos += RefRun->Config.Offset;
        Data->LimitSwitches.PosCode.Position = Data->Motion.Pos;
        smInitEncoderPos(&Data->Encoder, Data->Motor.Config.Resolution, Data->Motion.Pos);
        RefRun->State = SM_RRS_FINISHED;
        break;

    case SM_RRS_FINISHED:    //!< reference run sucessfully finished
        break;

    default:
        break;
    }

    return RetCode;
}
