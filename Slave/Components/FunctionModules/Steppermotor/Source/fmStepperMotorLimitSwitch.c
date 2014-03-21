/****************************************************************************/
/*! \file fmStepperMotorLimitSwitch.c
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

#include "halDeviceID.h"
#include "halCommon.h"
#include "halPorts.h"

#include "bmTime.h"

#include "fmStepperMotor.h"
#include "fmStepperMotorLimitSwitch.h"

#include <stdio.h>
//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/





void smInitLimitSwitches (LimitSwitches_t *LimitSwitches)
{
    UInt8                   Idx;
    LimitSwitch_t           *LimitSwitch;
    LimitSwitchPosConfig_t  *PosConfig;

    LimitSwitches->ConfigMask = 0;  // nothing configured

    LimitSwitches->PosCode.Value = 0;
    LimitSwitches->PosCode.InvalidValue = FALSE;
    LimitSwitches->PosCode.InvalidPosition = FALSE;
    LimitSwitches->PosCode.LastValue = -2;

    for (Idx = 0; Idx < NUM_OF_LIMIT_SWITCHES; Idx++)
    {
        LimitSwitch = &LimitSwitches->Device[Idx];
        LimitSwitch->Config.Exists = FALSE;
        LimitSwitch->Handle = 0;
        LimitSwitch->Time = 0;
        LimitSwitch->ActValue = 0;
        LimitSwitch->TmpValue = 0;
        LimitSwitch->DebounceCount = 0;
    }

    // position code 0 means no limit switch is active. this is always ok.
    PosConfig = &LimitSwitches->PosCodeConfig[0];
    PosConfig->Valid = TRUE;
    PosConfig->Stop = FALSE;
    // all other position codes assumed as invalid
    for (Idx = 1; Idx < (1 << NUM_OF_LIMIT_SWITCHES); Idx++)
    {
        PosConfig = &LimitSwitches->PosCodeConfig[Idx];
        PosConfig->Valid = FALSE;
        PosConfig->Stop = TRUE;

        PosConfig->Position = 0;
        PosConfig->Width = 0;
        PosConfig->Deviation = 0;
        
        PosConfig->RotDirCheck = TRUE;
        PosConfig->HitSkip = 0;
    }
}


Error_t smConfigureLimitSwitch(LimitSwitches_t* LimitSwitches, UInt8 Index, ConfigData_LS_t* Param)
{
    LimitSwitch_t* LimitSwitch = &LimitSwitches->Device[Index];

    LimitSwitch->Config.Exists      = Param->flag.exist;
    LimitSwitch->Config.Polarity    = Param->flag.polarity;
    LimitSwitch->Config.SampleRate  = Param->sampleRate;
    LimitSwitch->Config.Debounce    = Param->debounceCount;

    LimitSwitches->ConfigMask |= BIT(Index);

    return NO_ERROR;
}


Error_t smConfigurePosCode(LimitSwitches_t* LimitSwitches, UInt8 Index, ConfigData_LSPOS_t* Param)
{
    LimitSwitchPosConfig_t* PosCodeConfig = &LimitSwitches->PosCodeConfig[Index];

    PosCodeConfig->Valid        = Param->flag.valid;
    PosCodeConfig->Stop         = Param->flag.stop;
    PosCodeConfig->StopDir      = Param->flag.stopDir;
    PosCodeConfig->RotDirCheck  = Param->flag.rotDirCheck;
    PosCodeConfig->HitSkip      = Param->flag.hitSkip;

    DB4_TO_VAL(Param->position, PosCodeConfig->Position);
    PosCodeConfig->Width        = (Param->width >> 1);
    PosCodeConfig->Deviation    = Param->deviation;

    LimitSwitches->ConfigMask |= BIT(Index + 4);

    return NO_ERROR;
}


Bool smLimitSwitchesConfigIsComplete (LimitSwitches_t *LimitSwitches)
{
    UInt8 Idx;
    UInt8 CompleteMask = 0;

    for (Idx = 0; Idx < NUM_OF_LIMIT_SWITCHES; Idx++)
        CompleteMask |= BIT(Idx);   
    for (Idx = 1; Idx < (1 << NUM_OF_LIMIT_SWITCHES); Idx++)
        CompleteMask |= BIT(Idx + 4);   

    return (CompleteMask == LimitSwitches->ConfigMask);
}


Error_t smCheckLimitSwitchesConfig(LimitSwitches_t *LimitSwitches)
{
    UInt8 Idx;

    if (!smLimitSwitchesConfigIsComplete(LimitSwitches))
        return E_SMOT_CONFIG_INCOMPLETE;

// for each valid position code (except 0) the related limit switches have to exist
    for (Idx = 1; Idx < (1 << NUM_OF_LIMIT_SWITCHES); Idx++)
    {
        switch (Idx)
        {
        case 1:
            if (LimitSwitches->PosCodeConfig[Idx].Valid)
            {
                if (!LimitSwitches->Device[0].Config.Exists)
                    return E_SMOT_CONFIG_LIMITSWITCH;
            }
            break;
        case 2:
            if (LimitSwitches->PosCodeConfig[Idx].Valid)
            {
                if (!LimitSwitches->Device[1].Config.Exists)
                    return E_SMOT_CONFIG_LIMITSWITCH;
            }
            break;
        case 3:
            if (LimitSwitches->PosCodeConfig[Idx].Valid)
            {
                if ((!LimitSwitches->Device[0].Config.Exists) && (!LimitSwitches->Device[1].Config.Exists))
                    return E_SMOT_CONFIG_LIMITSWITCH;
            }
            break;
        default:
            return E_SMOT_CONFIG_LIMITSWITCH;
        }
    }

    return NO_ERROR;
}


Error_t smOpenLimitSwitch(Device_t PortID, LimitSwitch_t *LimitSwitch)
{
    Error_t RetCode = NO_ERROR;

    if(!LimitSwitch->Config.Exists)
        return NO_ERROR;

    if ((RetCode = halPortOpen (PortID, HAL_OPEN_READ)) < 0)
    {
        LimitSwitch->Handle = 0;
        return RetCode;
    }

    LimitSwitch->Handle = RetCode;
    if (NO_ERROR == (RetCode = halPortRead (LimitSwitch->Handle, &(LimitSwitch->ActValue))))
        LimitSwitch->ActValue ^= LimitSwitch->Config.Polarity;

    return RetCode;
}


Error_t smOpenLimitSwitches (LimitSwitches_t *LimitSwitches, UInt8 Instance)
{
    Error_t RetCode = NO_ERROR;

    // open limit switch 1
    RetCode = smOpenLimitSwitch(HAL_STEPPER_LIMIT1 + Instance, &(LimitSwitches->Device[0]));

    // open limit switch 2
    if (NO_ERROR == RetCode)
        RetCode = smOpenLimitSwitch(HAL_STEPPER_LIMIT2 + Instance, &(LimitSwitches->Device[1]));

    return RetCode;
}


Error_t smCloseLimitSwitch(LimitSwitch_t *LimitSwitch)
{
    Error_t RetCode = NO_ERROR;

    if(0 == LimitSwitch->Handle)
        return NO_ERROR;

    if ((RetCode = halPortClose (LimitSwitch->Handle)) < 0)
        return RetCode;
    
    LimitSwitch->Handle = 0;

    return NO_ERROR;
}


Error_t smCloseLimitSwitches (LimitSwitches_t *LimitSwitches)
{
    Error_t RetCode = NO_ERROR;

    // close limit switch 1
    RetCode = smCloseLimitSwitch(&(LimitSwitches->Device[0]));

    // close limit switch 2
    if (NO_ERROR == RetCode)
        RetCode = smCloseLimitSwitch(&(LimitSwitches->Device[1]));

    return RetCode;
}


Error_t smRead_LimitSwitch(LimitSwitch_t *LimitSwitch)
{
    Error_t ErrCode;    
    UInt16 NewValue;

    if ((ErrCode = halPortRead (LimitSwitch->Handle, &NewValue)) < 0)
        return ErrCode;

    LimitSwitch->Time = bmGetTime();

    NewValue ^= LimitSwitch->Config.Polarity;

    // while value is still unchanged or if it's the first transition, the debounce conter is reseted
    if (NewValue == LimitSwitch->ActValue || NewValue != LimitSwitch->TmpValue)
        LimitSwitch->DebounceCount = 0;
    else
    {
        if (++LimitSwitch->DebounceCount >= LimitSwitch->Config.Debounce)
        {
            LimitSwitch->ActValue = NewValue;               // value is assumed to be stable now
        }
    }
    LimitSwitch->TmpValue = NewValue;

    return NO_ERROR;
}


Error_t smPosCodeIsValid(LimitSwitches_t *Switches, Int8 PosCode)
{
    Bool Valid;
//    Int16 PosCode = Switches->PosCode;

    Valid = ((PosCode >= 0) && (PosCode < (1 << NUM_OF_LIMIT_SWITCHES)));
    if (Valid)
        Valid = Switches->PosCodeConfig[PosCode].Valid;     //lint !e676

    if (!Valid)
        return E_SMOT_INVALID_POSCODE;

    return NO_ERROR;
}


Error_t smPosCodeIsAtValidPosition(LimitSwitches_t *LimitSwitches, Int32 Position, UInt16 ResetPosition)
{
    Error_t RetCode = NO_ERROR;
    LimitSwitchPosConfig_t* PosCodeConfig;
    Int32 Min;
    Int32 Max;

    if (NO_ERROR == smPosCodeIsValid(LimitSwitches, LimitSwitches->PosCode.Value))    // only valid position codes need to be checked
    {
        Position = Position + ResetPosition;
        if (0 != LimitSwitches->PosCode.Value)
        {
            PosCodeConfig = &LimitSwitches->PosCodeConfig[LimitSwitches->PosCode.Value];
            if (0 != PosCodeConfig->Width)  // if check is not disabled
            {
                // if limit switch is active the actual position must be within the tolerated bandwidth
                Min = PosCodeConfig->Position - PosCodeConfig->Width - PosCodeConfig->Deviation + ResetPosition;
                Max = PosCodeConfig->Position + PosCodeConfig->Width + PosCodeConfig->Deviation + ResetPosition;
                if ((Position < Min) || (Position > Max))
                    RetCode = E_SMOT_INVALID_POSCODE_POSITION;
            }
        }       
        else                                
        {        
            UInt8 Idx;
            // if no limit switch is active the actual position must be outside of each valid position code bandwidth
            for (Idx = 1; Idx < (1 << NUM_OF_LIMIT_SWITCHES); Idx++)
            {
                PosCodeConfig = &LimitSwitches->PosCodeConfig[Idx];
                if (!PosCodeConfig->Valid)
                    continue;
                if (0 == PosCodeConfig->Width)  // if check is disabled
                    continue;
                Min = PosCodeConfig->Position - PosCodeConfig->Width + PosCodeConfig->Deviation + ResetPosition;
                Max = PosCodeConfig->Position + PosCodeConfig->Width - PosCodeConfig->Deviation + ResetPosition;
                if ((Position > Min) && (Position < Max))
                {
                    RetCode = E_SMOT_INVALID_POSCODE_POSITION;
                    break;
                }
            }
        }
    }

    return RetCode;
}


Error_t smCheckPosCode(UInt16 Instance)
{
    smData_t *Data = &smDataTable[Instance];
    PosCode_t *PosCode = &Data->LimitSwitches.PosCode;

    Error_t RetCode = NO_ERROR;
    if ((RetCode = smPosCodeIsAtValidPosition(&Data->LimitSwitches, Data->Motion.Pos, Data->Motor.FrameworkConfig.ResetPosition)) < 0)
    {
        Data->Motion.Stop = SM_SC_ALWAYS;
        PosCode->InvalidPosition = TRUE;
        if (PosCode->Value != PosCode->LastValue)
        {
            PosCode->LastValue = PosCode->Value;
            RetCode = bmSignalEvent(Data->Channel, RetCode, TRUE, PosCode->Value);
        }
    }
    return RetCode;
}


Error_t smRotDirIsAllowed(Bool CCW, LimitSwitches_t *Switches)
{
    if (FALSE == Switches->PosCodeConfig[Switches->PosCode.Value].RotDirCheck)
        return NO_ERROR;
    
    if (FALSE == Switches->PosCodeConfig[Switches->PosCode.Value].Stop)
        return NO_ERROR;

    if (CCW)
    {
        if (Switches->PosCodeConfig[Switches->PosCode.Value].StopDir == SMOT_ROTDIR_CCW)
            return E_SMOT_INVALID_POSCODE_DIR;
    }
    else
    {
        if (Switches->PosCodeConfig[Switches->PosCode.Value].StopDir == SMOT_ROTDIR_CW)
            return E_SMOT_INVALID_POSCODE_DIR; 
    }

    return NO_ERROR;
}


Error_t smSampleLimitSwitches(UInt16 Instance, Bool UseSampleRate)
{
    smData_t *Data = &smDataTable[Instance];

    Error_t         RetCode = NO_ERROR;
    LimitSwitches_t *LimitSwitches = &Data->LimitSwitches;
    LimitSwitch_t   *Device;
    UInt8           Idx;
    Int16           NewValue = 0;

    for (Idx = 0; Idx < NUM_OF_LIMIT_SWITCHES; Idx++)
    {
        Device = &LimitSwitches->Device[Idx];
        if (!Device->Config.Exists)
            continue;

        if (UseSampleRate)
        {
            if (bmTimeExpired(Device->Time) >= Device->Config.SampleRate)
                RetCode = smRead_LimitSwitch(Device);
        }
        else
            RetCode = smRead_LimitSwitch(Device);

        if (RetCode < 0)
        {
            NewValue = -1;                  // set to undefined
        }
        else
        {
            if (Device->ActValue)
                NewValue |= (1 << Idx);
        }
    }

    if (NewValue != Data->LimitSwitches.PosCode.Value)
    {
        Error_t RetValue;

        LimitSwitches->PosCode.Value = NewValue;
        
        Data->Motion.LSHitCnt++;
        
        if ((RetValue = smPosCodeIsValid(LimitSwitches, LimitSwitches->PosCode.Value)) < 0)
        {
            Device = &LimitSwitches->Device[Idx];

            Data->Motion.Stop = SM_SC_ALWAYS;      // an invalid position code will stop movement

            LimitSwitches->PosCode.InvalidValue = TRUE;
            if (LimitSwitches->PosCode.Value != LimitSwitches->PosCode.LastValue)
            {
                LimitSwitches->PosCode.LastValue = LimitSwitches->PosCode.Value;
                RetCode = bmSignalEvent(Data->Channel, RetValue, TRUE, 0);
            }
            
            Data->Motion.LSHitCnt = 0;
            
            printf("stop error\n");

        }
        else
        {
            // emit stop signal if limit switch is configured to stop movement
            if (LimitSwitches->PosCodeConfig[LimitSwitches->PosCode.Value].Stop)
            {   
                // always stop if reference run is active
                if (SM_STATE_REFRUN == Data->State) {
/*                
                    if (LimitSwitches->PosCodeConfig[LimitSwitches->PosCode.Value].HitSkip == 0) {
                        Data->Motion.Stop = SM_SC_ALWAYS;
                    }
                    else if (LimitSwitches->PosCodeConfig[LimitSwitches->PosCode.Value].HitSkip < Data->Motion.LSHitCnt) {
                        Data->Motion.LSHitCnt = 0;
                        Data->Motion.Stop = SM_SC_ALWAYS;
                    }
*/                  
                    Data->Motion.Stop = SM_SC_ALWAYS;
                                        
                }
                else
                {
                    //  otherwise stop if motor is running in stop direction  
                    if (SMOT_ROTDIR_CCW == LimitSwitches->PosCodeConfig[LimitSwitches->PosCode.Value].StopDir)
                        Data->Motion.Stop = SM_SC_DIR_CCW;
                    else
                        Data->Motion.Stop = SM_SC_DIR_CW;
                }
            }
        }
    }

    return RetCode;
}
