/****************************************************************************/
/*! \file fmStepperMotorLimitSwitch.c
 *
 *  \brief Limit switch sampling and position encoding functions used by function
 *         module stepper motor
 *
 *  $Rev:    $ 0.1
 *  $Date:   $ 11.06.2012
 *  $Author: $ Rainer Boehles
 *
 *  \b Description:
 *
 *      This module contains functions to read and evaluate limit switch
 *      signals. Up to 2 limit switches are supported. Their signals are
 *      filtered by debouncing and validated. If signals look plausible then 
 *      signal status is converted to encode a position. This position code can
 *      be used to validate actual motor position by checking if correct signal
 *      is active at position ranges where it should be, and inactive where no
 *      limit switch is expected.
 *
 *      Position encoding by limit switches is simply done by composing both 
 *      limit switch signals to a binary value. First limit switch signal is
 *      used as bit 0, and second limit switch as bit 1. Thus position code
 *      values can range from 0 (no limit switch is active) to 3 (both switches
 *      are active).
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
#include "Global.h"

#include "halDeviceID.h"
#include "halCommon.h"
#include "halPorts.h"

#include "bmTime.h"

#include "fmStepperMotor.h"
#include "fmStepperMotorLimitSwitch.h"

#include <stdio.h>
/******************************************************************************/
/*! 
 *  \brief  Initialize data for all limit switches and position codes
 *
 *      All limit switch and position code data is initialized.
 *      All configuration data is set to 'unconfigured', which means all
 *      configuration data have to be set first by master, before devices
 *      can be used.
 * 
 *  \iparam  LimitSwitches = pointer to limit switches data
 * 
 ******************************************************************************/
void smInitLimitSwitches (smLimitSwitches_t *LimitSwitches) {

    UInt8                       Idx;
    smLimitSwitch_t             *LimitSwitch;
    smLimitSwitchPosConfig_t    *PosConfig;

    LimitSwitches->ConfigMask = 0;  // nothing configured

    LimitSwitches->Config.Debounce = 0;
    LimitSwitches->Config.SampleRate = 10;

    LimitSwitches->PosCode.Value = POSCODE_UNDEFINED;
    LimitSwitches->PosCode.ErrCode = NO_ERROR;
    LimitSwitches->PosCode.Position = 0;
    LimitSwitches->PosCode.Time = 0;
    LimitSwitches->PosCode.TmpValue = POSCODE_UNDEFINED;
    LimitSwitches->PosCode.DebounceCount = 0;

    for (Idx = 0; Idx < SM_NUM_OF_LIMIT_SWITCHES; Idx++) {
        LimitSwitch = &LimitSwitches->Device[Idx];
        LimitSwitch->Config.Exists = FALSE;
        LimitSwitch->Handle = 0;

        LimitSwitch->Value = 0;
        LimitSwitch->ErrCode = NO_ERROR;
    }

    // position code 0 means no limit switch is active. this is always ok.
    PosConfig = &LimitSwitches->PosCodeConfig[0];
    PosConfig->Valid = TRUE;
    PosConfig->Stop = FALSE;
    // all other position codes assumed as invalid
    for (Idx = 1; Idx < (1 << SM_NUM_OF_LIMIT_SWITCHES); Idx++) {
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


/******************************************************************************/
/*! 
 *  \brief  Set limit switch configuration
 *
 *      Configuration data received from master is applied to the limit switch.
 * 
 *  \iparam  LimitSwitches  = pointer to limit switches data
 *  \iparam  Index          = Index of limit switch to set
 *  \iparam  Param          = pointer to configuration data received from master
 * 
 *  \return  NO_ERROR
 *
 ******************************************************************************/
Error_t smConfigureLimitSwitch(smLimitSwitches_t* LimitSwitches, UInt8 Index, ConfigData_LS_t* Param) {

	smLimitSwitch_t* LimitSwitch = &LimitSwitches->Device[Index];
    
    LimitSwitches->Config.SampleRate = Param->sampleRate;
    LimitSwitches->Config.Debounce   = Param->debounceCount;

    LimitSwitch->Config.Exists      = Param->flag.exist;
    LimitSwitch->Config.Polarity    = Param->flag.polarity;

    LimitSwitches->ConfigMask |= BIT(Index);

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Set position code configuration
 *
 *      Configuration data received from master is applied to the position code.
 * 
 *  \iparam  LimitSwitches  = pointer to limit switches data
 *  \iparam  Index          = position code number
 *  \iparam  Param          = pointer to configuration data received from master
 * 
 *  \return  NO_ERROR
 *
 ******************************************************************************/
Error_t smConfigurePosCode(smLimitSwitches_t* LimitSwitches, UInt8 Index, ConfigData_LSPOS_t* Param) {

    smLimitSwitchPosConfig_t* PosCodeConfig = &LimitSwitches->PosCodeConfig[Index];

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


/******************************************************************************/
/*! 
 *  \brief  Check completeness of limit switches / position codes configuration
 *
 *      Check if all necessary configuration data for limit switches and
 *      position codes have been received
 * 
 *  \iparam  LimitSwitches  = pointer to limit switches data
 * 
 *  \return  true, if configuration is complete
 *
 ******************************************************************************/
Bool smLimitSwitchesConfigIsComplete (smLimitSwitches_t *LimitSwitches) {

    UInt8 Idx;
    UInt8 CompleteMask = 0;

    for (Idx = 0; Idx < SM_NUM_OF_LIMIT_SWITCHES; Idx++)
        CompleteMask |= BIT(Idx);   
    for (Idx = 1; Idx < (1 << SM_NUM_OF_LIMIT_SWITCHES); Idx++)
        CompleteMask |= BIT(Idx + 4);   

    return (CompleteMask == LimitSwitches->ConfigMask);
}


/******************************************************************************/
/*! 
 *  \brief  Validate limit switches / position codes configuration
 *
 *      Check if all necessary configuration data for limit switches and
 *      position codes have been received and are plausible.
 * 
 *  \iparam  LimitSwitches  = pointer to limit switches data
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smCheckLimitSwitchesConfig(smLimitSwitches_t *LimitSwitches) {

    UInt8 Idx;

    if (!smLimitSwitchesConfigIsComplete(LimitSwitches)) {
        return E_SMOT_CONFIG_INCOMPLETE;
    }

// for each valid position code (except 0) the related limit switches have to exist
    for (Idx = 1; Idx < (1 << SM_NUM_OF_LIMIT_SWITCHES); Idx++) {
        switch (Idx) {
        case 1:
            if (LimitSwitches->PosCodeConfig[Idx].Valid) {
                if (!LimitSwitches->Device[0].Config.Exists) {
                    return E_SMOT_CONFIG_LIMITSWITCH;
                }
            }
            break;
        case 2:
            if (LimitSwitches->PosCodeConfig[Idx].Valid) {
                if (!LimitSwitches->Device[1].Config.Exists) {
                    return E_SMOT_CONFIG_LIMITSWITCH;
                }
            }
            break;
        case 3:
            if (LimitSwitches->PosCodeConfig[Idx].Valid) {
                if ((!LimitSwitches->Device[0].Config.Exists) || (!LimitSwitches->Device[1].Config.Exists)) {
                    return E_SMOT_CONFIG_LIMITSWITCH;
                }
            }
            break;
        default:
            return E_SMOT_CONFIG_LIMITSWITCH;
        }
    }

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Open limit switch device
 *
 *      Limit switch device is opened and actual value is read.
 * 
 *  \iparam  PortID      = Logical port ID
 *  \iparam  LimitSwitch = pointer to limit switch data
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smOpenLimitSwitch(Device_t PortID, smLimitSwitch_t *LimitSwitch) {

    Error_t RetCode = NO_ERROR;

    if(!LimitSwitch->Config.Exists) {
        return NO_ERROR;
    }

    if ((RetCode = halPortOpen (PortID, HAL_OPEN_READ)) < 0) {
        LimitSwitch->Handle = 0;
        return RetCode;
    }

    LimitSwitch->Handle = RetCode;
    if (NO_ERROR == (RetCode = halPortRead (LimitSwitch->Handle, &(LimitSwitch->Value)))) {
        LimitSwitch->Value ^= LimitSwitch->Config.Polarity;
    }

    return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief  Open all limit switch devices
 *
 *      Both limit switch devices are opened
 * 
 *  \iparam  LimitSwitches = pointer to limit switches data
 *  \iparam  Instance = Module instance number
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smOpenLimitSwitches (smLimitSwitches_t *LimitSwitches, UInt8 Instance) {

    Error_t RetCode = NO_ERROR;

    // open limit switch 1
    RetCode = smOpenLimitSwitch(HAL_STEPPER_LIMIT1 + Instance, &(LimitSwitches->Device[0]));

    // open limit switch 2
    if (NO_ERROR == RetCode) {
        RetCode = smOpenLimitSwitch(HAL_STEPPER_LIMIT2 + Instance, &(LimitSwitches->Device[1]));
    }

    return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief  Close limit switch device
 *
 *      Limit switch device is closed.
 * 
 *  \iparam  LimitSwitch = pointer to limit switch data
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smCloseLimitSwitch(smLimitSwitch_t *LimitSwitch) {

    Error_t RetCode = NO_ERROR;

    if(0 == LimitSwitch->Handle) {
        return NO_ERROR;
    }

    if ((RetCode = halPortClose (LimitSwitch->Handle)) < 0) {
        return RetCode;
    }
    
    LimitSwitch->Handle = 0;

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Close all limit switch devices
 *
 *      Both limit switch devices are closed.
 * 
 *  \iparam  LimitSwitches = pointer to limit switches data
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smCloseLimitSwitches (smLimitSwitches_t *LimitSwitches) {

    Error_t RetCode = NO_ERROR;

    // close limit switch 1
    RetCode = smCloseLimitSwitch(&(LimitSwitches->Device[0]));

    // close limit switch 2
    if (NO_ERROR == RetCode) {
        RetCode = smCloseLimitSwitch(&(LimitSwitches->Device[1]));
    }

    return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief  Update position code error status
 *
 *      If validity changes a CAN signal is send to inform master.
 * 
 *  \iparam  PosCode = pointer to position code
 *  \iparam  ErrCode = NO_ERROR or (negative) error code
 *  \iparam  Channel = Logical CAN channel
 * 
 ******************************************************************************/
void UpdatePosCodeErrStatus(smPosCode_t *PosCode, Error_t ErrCode, UInt16 Channel) {

    if (ErrCode != PosCode->ErrCode) {
/* error gone message not supported by base module yet. uncomment to activate gone message
        if (NO_ERROR != PosCode->ErrCode) {
            bmSignalEvent(Channel, PosCode->ErrCode, FALSE, 0);
        }
*/
        if (NO_ERROR != ErrCode) {
            bmSignalEvent(Channel, ErrCode, TRUE, PosCode->Value);
        }
        PosCode->ErrCode = ErrCode;
    }
}


/******************************************************************************/
/*! 
 *  \brief  Validate position code value
 *
 *      Position code value is checked to be in the range from 0 to 3.
 *      In addition the position code have to be configured as a valid value.
 * 
 *  \iparam  LimitSwitches = pointer to limit switches data
 *  \iparam  PosCode = position code
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smPosCodeIsValid(smLimitSwitches_t *LimitSwitches, Int8 PosCode) {

    Bool Valid;

    Valid = ((PosCode >= 0) && (PosCode < (1 << SM_NUM_OF_LIMIT_SWITCHES)));
    if (Valid) {
        Valid = LimitSwitches->PosCodeConfig[PosCode].Valid;     //lint !e676
    }

    if (!Valid) {
        return E_SMOT_INVALID_POSCODE;
    }

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Validate position code against actual position
 *
 *      For each position code the position range at which the position code is
 *      active is configured by position, signal width and deviation.
 *
 *      The position code is checked to be only active in the defined range and
 *      inactive outside the defined range.
 *
 *      Validation of a specific position code is skipped if signal width is
 *      configured as 0.
 *
 *
 *  \iparam  LimitSwitches  = pointer to limit switches data
 *  \iparam  ResetPosition  = position at which motor position is reset to 0,
 *                            needed for rotatory movement (half steps)
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smPosCodeIsAtValidPosition(smLimitSwitches_t *LimitSwitches, UInt16 ResetPosition) {

    Error_t ErrCode = NO_ERROR;
    smLimitSwitchPosConfig_t* PosCodeConfig;
    Int32 Min;
    Int32 Max;
    Int32 Position = LimitSwitches->PosCode.Position+ ResetPosition;
    UInt8 Idx;

    // check if actual position is within one of the defined position code ranges
    for (Idx = 1; Idx < (1 << SM_NUM_OF_LIMIT_SWITCHES); Idx++) {
        PosCodeConfig = &LimitSwitches->PosCodeConfig[Idx];
        if (!PosCodeConfig->Valid) {
            continue;
        }
        if (0 == PosCodeConfig->Width) {    // no range data defined
            continue;
        }
        Min = PosCodeConfig->Position - PosCodeConfig->Width + PosCodeConfig->Deviation + ResetPosition;
        Max = PosCodeConfig->Position + PosCodeConfig->Width - PosCodeConfig->Deviation + ResetPosition;
        if ((Position > Min) && (Position < Max)) {         // within core range
            if (Idx != LimitSwitches->PosCode.Value) {      // the position code must match
                ErrCode = E_SMOT_INVALID_POSCODE_POSITION;
            }
            return ErrCode;
        }
        Min = PosCodeConfig->Position - PosCodeConfig->Width - PosCodeConfig->Deviation + ResetPosition;
        Max = PosCodeConfig->Position + PosCodeConfig->Width + PosCodeConfig->Deviation + ResetPosition;
        if ((Position >= Min) && (Position <= Max)) {       // within extended range the position code can match
            if ((0 != LimitSwitches->PosCode.Value) && (Idx != LimitSwitches->PosCode.Value)) {
                ErrCode = E_SMOT_INVALID_POSCODE_POSITION;
            }
            return ErrCode;
        }
    }

    // actual position is not within one of the defined position code ranges
    // check if a position code is active that belongs to a defined range
    if (0 != LimitSwitches->PosCode.Value) {
        PosCodeConfig = &LimitSwitches->PosCodeConfig[LimitSwitches->PosCode.Value];
        if ((PosCodeConfig->Valid) && (0 != PosCodeConfig->Width)) {
                ErrCode = E_SMOT_INVALID_POSCODE_POSITION;
        }
    }

    return ErrCode;
}


/******************************************************************************/
/*! 
 *  \brief  Check if position code is plausible
 *
 *      Position code is validated against motor position.
 *      If position code is invalid any movement is stopped immediately.
 *      If validity changes a CAN signal is send to inform master.
 *
 *
 *  \iparam  Instance = Module instance number
 * 
 ******************************************************************************/
void smCheckPosCode(UInt16 Instance) {

    smData_t *Data       = &smDataTable[Instance];
    smPosCode_t *PosCode = &Data->LimitSwitches.PosCode;
    Error_t ErrCode      = NO_ERROR;

    if ((NO_ERROR != PosCode->ErrCode) || (POSCODE_UNDEFINED == PosCode->Value)) {
        return;     // only valid values can be checked
    }

    ErrCode = smPosCodeIsAtValidPosition(&Data->LimitSwitches, Data->Motor.FrameworkConfig.ResetPosition);
    if (NO_ERROR != ErrCode) {
        Data->Motion.Stop = SM_SC_ALWAYS;
    }

    UpdatePosCodeErrStatus (PosCode, ErrCode, Data->Channel);
}


/******************************************************************************/
/*! 
 *  \brief  Check if rotation direction is permitted from actual position
 *
 *      Actual position code is checked if configuration permits movement into
 *      specified rotation direction.
 *
 *
 *  \iparam  LimitSwitches  = pointer to limit switches data
 *  \iparam  CCW            = true if direction is counter clock wise
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smRotDirIsAllowed(Bool CCW, smLimitSwitches_t *LimitSwitches) {

    if (FALSE == LimitSwitches->PosCodeConfig[LimitSwitches->PosCode.Value].RotDirCheck)
        return NO_ERROR;
    
    if (FALSE == LimitSwitches->PosCodeConfig[LimitSwitches->PosCode.Value].Stop)
        return NO_ERROR;

    if (CCW) {
        if (LimitSwitches->PosCodeConfig[LimitSwitches->PosCode.Value].StopDir == SMOT_ROTDIR_CCW) {
            return E_SMOT_INVALID_POSCODE_DIR;
        }
    }
    else {
        if (LimitSwitches->PosCodeConfig[LimitSwitches->PosCode.Value].StopDir == SMOT_ROTDIR_CW) {
            return E_SMOT_INVALID_POSCODE_DIR;
        }
    }

    return NO_ERROR;
}


/******************************************************************************/
/*!
 *  \brief  Read limit switch
 *
 *      Limit switch signal is read and signal polarity is adjusted.
 *      If validity of read signal changes a CAN signal is send to inform master.
 *
 *  \iparam  LimitSwitch = pointer to limit switch data
 *  \iparam  Channel     = Logical CAN channel
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smReadLimitSwitch(smLimitSwitch_t *LimitSwitch, UInt16 Channel) {

    Error_t ErrCode;

    ErrCode = halPortRead (LimitSwitch->Handle, &LimitSwitch->Value);
    if (ErrCode != LimitSwitch->ErrCode) {
/* error gone message not supported by base module yet. uncomment to activate gone message
        if (NO_ERROR != LimitSwitch->ErrCode) {
            bmSignalEvent(Channel, LimitSwitch->ErrCode, FALSE, 0);
        }
*/
        if (NO_ERROR != ErrCode) {
            bmSignalEvent(Channel, ErrCode, TRUE, 0);
        }
        LimitSwitch->ErrCode = ErrCode;
    }
    LimitSwitch->Value ^= LimitSwitch->Config.Polarity;

    return ErrCode;
}


/******************************************************************************/
/*!
 *  \brief  Read position code
 *
 *      All limit switches are read to get actual position code.
 *      If debounced value is found to be stable the new value is considered
 *      as active position code.
 *
 *      If validity of read signal changes a CAN signal is send to inform master.
 *
 *  \iparam  LimitSwitches  = pointer to limit switches data
 *  \iparam  Position       = pointer to motor position data
 *  \iparam  Channel        = Logical CAN channel
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smReadPosCode(smLimitSwitches_t *LimitSwitches, Int32 *Position, UInt16 Channel) {

    Error_t         ErrCode = NO_ERROR;
    UInt8           Idx;
    smLimitSwitch_t *Device;
    UInt8           NewValue = 0;

    for (Idx = 0; Idx < SM_NUM_OF_LIMIT_SWITCHES; Idx++) {
        Device = &LimitSwitches->Device[Idx];
        if (!Device->Config.Exists) {
            continue;
        }

        if (NO_ERROR != smReadLimitSwitch(Device, Channel)) {
            ErrCode = E_SMOT_INVALID_POSCODE;
            break;
        }
        else {
            if (Device->Value) {
                NewValue |= (1 << Idx);
            }
        }
    }

    if (NO_ERROR == ErrCode) {
        LimitSwitches->PosCode.Position = *Position;
        LimitSwitches->PosCode.Time = bmGetTime();
        ErrCode = LimitSwitches->PosCode.ErrCode;
        // while value is still unchanged or if it's the first transition, the debounce counter is reseted
        if (NewValue == LimitSwitches->PosCode.Value || NewValue != LimitSwitches->PosCode.TmpValue) {
            LimitSwitches->PosCode.DebounceCount = 0;
        }
        else {
            if (++LimitSwitches->PosCode.DebounceCount >= LimitSwitches->Config.Debounce) {
                LimitSwitches->PosCode.Value = NewValue;    // value is assumed to be stable now
                ErrCode = smPosCodeIsValid(LimitSwitches, LimitSwitches->PosCode.Value);
            }
        }
        LimitSwitches->PosCode.TmpValue = NewValue;
    }

    UpdatePosCodeErrStatus (&LimitSwitches->PosCode, ErrCode, Channel);

    return ErrCode;
}


/******************************************************************************/
/*! 
 *  \brief  Sample and evaluate limit switches
 *
 *      All limit switch signals are read and converted to a position code.
 *      Sampling is either performed immediately or after the configured
 *      sample rate time (ms).
 *
 *      If a new position code is available but invalid, then motor is stopped
 *      immediately.
 *      If a new position code is available and valid, then motor is stopped if
 *      actual position code is configured to stop motor for actual direction.
 * 
 * 
 *  \iparam  Instance       = Module instance number
 *  \iparam  UseSampleRate  = true, use configured sample rate
 *                            false, sample immediately
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smSampleLimitSwitches(UInt16 Instance, Bool UseSampleRate) {

    smData_t *Data = &smDataTable[Instance];

    Error_t             ErrCode = NO_ERROR;
    smLimitSwitches_t   *LimitSwitches = &Data->LimitSwitches;
    UInt8               Value = LimitSwitches->PosCode.Value;

    if (UseSampleRate) {
        if (bmTimeExpired(LimitSwitches->PosCode.Time) >= LimitSwitches->Config.SampleRate) {
            ErrCode = smReadPosCode(LimitSwitches, &Data->Motion.Pos, Data->Channel);
        }
    }
    else {
        ErrCode = smReadPosCode(LimitSwitches, &Data->Motion.Pos, Data->Channel);
    }

    if (NO_ERROR != ErrCode) {
        Data->Motion.Stop = SM_SC_ALWAYS;   // an invalid position code will always stop movement
    }
    else {
        if (Value != Data->LimitSwitches.PosCode.Value) {   // if position code value have changed
            // emit stop signal if limit switch is configured to stop movement
            if (LimitSwitches->PosCodeConfig[LimitSwitches->PosCode.Value].Stop) {
                // never stop if reference run is active
                if (SM_STATE_REFRUN != Data->State) {
                    //  otherwise stop if motor is running in stop direction
                    if (SMOT_ROTDIR_CCW == LimitSwitches->PosCodeConfig[LimitSwitches->PosCode.Value].StopDir) {
                        Data->Motion.Stop = SM_SC_DIR_CCW;
                    }
                    else {
                        Data->Motion.Stop = SM_SC_DIR_CW;
                    }
                }
            }
        }
    }

    return ErrCode;
}
