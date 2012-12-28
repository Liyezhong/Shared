/****************************************************************************/
/*! \file fmStepperMotorEncoder.c
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
#include "halCommon.h"
#include "halInterrupt.h"
#include "halEncoder.h"
//#include "bmCommon.h"
//#include "Basemodule.h"

//// #include "bmCommon.h"
//// #include "Basemodule.h"
//#include "bmTime.h"
//#include "halPorts.h"
#include "fmStepperMotor.h"
#include "fmStepperMotorEncoder.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define SM_ENCODER_SAMPLERATE 1


void smInitEncoder(Encoder_t *Encoder)
{
    Encoder->ConfigMask = 0;  // nothing configured

    Encoder->Config.Exists      = FALSE;
    Encoder->Config.SampleRate  = SM_ENCODER_SAMPLERATE;
    Encoder->Handle             = 0;
    Encoder->Time               = bmGetTime();

    Encoder->StepLoss.Config.WarnLimit   = 0;
    Encoder->StepLoss.Config.ErrorLimit  = 0;
}


Error_t smConfigureEncoder(Encoder_t *Encoder, ConfigData_ENC_t* Param)
{
    Encoder->Config.Exists = Param->flag.exist;
//    Encoder->Config.RotationDir   = Param->flag.rotDir;
    DB2_TO_VAL(Param->resolution, Encoder->Config.Resolution);
    DB2_TO_VAL(Param->stepLoss_WarnLimit, Encoder->StepLoss.Config.WarnLimit);
    DB2_TO_VAL(Param->stepLoss_ErrLimit, Encoder->StepLoss.Config.ErrorLimit);

    Encoder->ConfigMask |= BIT(0);

    return NO_ERROR;
}


Bool smEncoderConfigIsComplete (Encoder_t *Encoder)
{
    UInt8 CompleteMask = BIT(0);

    return (CompleteMask == Encoder->ConfigMask);
}


Error_t smInitEncoderPos(Encoder_t *Encoder, UInt16 MotorResolution, Int32 Pos)
{
    Error_t RetCode = NO_ERROR;

    MotorResolution = (MotorResolution << 1);   // convert from full-step to half-step

    Encoder->Value = Pos % MotorResolution;
    if (Pos < 0)
        Encoder->Value += MotorResolution;
    Encoder->Value = (10 * Encoder->Value) * (Encoder->Config.Resolution << ENCODER_COUNT_SHIFT) / MotorResolution;
    if ((Encoder->Value % 10) < 5)
        Encoder->Value /= 10;
    else
        Encoder->Value = Encoder->Value / 10 + 1;

    RetCode = halEncoderWrite (Encoder->Handle, ENC_REG_POSITION, Encoder->Value);
    
    if (NO_ERROR == RetCode)
    {
        Encoder->Pos     = Pos;
        Encoder->BasePos = Pos / MotorResolution;
        Encoder->BasePos *= MotorResolution;
        if (Pos < 0)
            Encoder->BasePos -= MotorResolution;
//        Encoder->Time = bmGetTime();
    }
    return RetCode;
}


Error_t smOpenEncoder (Encoder_t *Encoder, UInt8 Instance, UInt16 MotorResolution)
{
    Error_t RetCode = NO_ERROR;

    if(Encoder->Config.Exists)
    {
//        if ((RetCode = halEncoderOpen(HAL_STEPPER_ENCODER + Instance, MaxLimit, smEncoderISR)) >= 0)
        if ((RetCode = halEncoderOpen(HAL_STEPPER_ENCODER + Instance, 0, NULL)) < 0)
            return RetCode;

        Encoder->Handle = RetCode;

        RetCode = halEncoderWrite (Encoder->Handle, ENC_REG_ENDPOSITION, (Encoder->Config.Resolution << ENCODER_COUNT_SHIFT) - 1);    
        if (NO_ERROR == RetCode)
            RetCode = smInitEncoderPos (Encoder, MotorResolution, 0);
//RetCode = halEncoderControl(Data->BasisParam.Encoder.HandleHAL, ENC_INTR_OVERFLOW, ON);

        if (RetCode < 0)
        {
            smCloseEncoder(Encoder);
            return RetCode;
        }
    }

    return NO_ERROR;
}


Error_t smCloseEncoder (Encoder_t *Encoder)
{
    Error_t RetCode = NO_ERROR;

    if (0 != Encoder->Handle)
    {
        RetCode = halEncoderClose(Encoder->Handle);
        Encoder->Handle = 0;
    }

    return RetCode;
}


//UInt32  encDuration;
//UInt32  encMax=0;
//UInt32  encMin=0xffffffff;

Error_t smReadEncoder (smData_t *Data)
{
    Error_t RetCode = NO_ERROR;

    Encoder_t *Encoder = &Data->Encoder;
    UInt16 EncoderResolution = (Encoder->Config.Resolution << ENCODER_COUNT_SHIFT); // convert to real resolution

    UInt16 ResetPosition = Data->Motor.FrameworkConfig.ResetPosition;

    UInt32  Value;
    Int32   Diff;
    Int32   Pos;

//encDuration = halGetFastTick();

    if ((RetCode = halEncoderRead(Encoder->Handle, ENC_REG_POSITION, &Value)) < 0)
        return RetCode;
    Encoder->Time = bmGetTime();

    Value = Value % EncoderResolution;
    if (Value == Encoder->Value)
        return NO_ERROR;

    Diff = Value - Encoder->Value;
    if (Diff < (-(EncoderResolution >> 1)))                                         // check for overflow
        Encoder->BasePos += (Data->Motor.Config.Resolution << 1);                   // correction for overflow
    else
    {
        if (Diff > (EncoderResolution >> 1))                                        // check for overflow
            Encoder->BasePos -= (Data->Motor.Config.Resolution << 1);               // correction for overflow
    }
    if (0 != ResetPosition)
        Encoder->BasePos = Encoder->BasePos % ResetPosition;

// position value calculation with rounding
    Pos = (Value << 3) * Data->Motor.Config.Resolution / (EncoderResolution >> 1);  // pos = value * 8 ...
    if ((Pos & 7) < 4)                                                              // (pos % 8) < 4
        Pos = Pos >> 3;                                                             // pos / 8
    else
        Pos = (Pos >> 3) + 1;                                                       // (pos / 8) + 1
    Pos += Encoder->BasePos;

// do correction for rotatory position value
    if (0 != ResetPosition)
    {
        Pos = (Pos % ResetPosition);
        if (Pos < 0)
            Pos += ResetPosition;
    }
    Encoder->Pos = Pos; // ATTENTION! because encoder position is used in ISR always modify the value in a single operation (or disable stepper interrupts)

    Encoder->Value = Value;

//encDuration = halGetFastTick() - encDuration;
//if (encDuration > encMax)
//    encMax = encDuration;
//if (encDuration < encMin)
//    encMin = encDuration;

    return NO_ERROR;
}


Error_t smSampleEncoder(UInt16 Instance, Bool UseSampleRate)
{
    smData_t *Data = &smDataTable[Instance];
    Encoder_t *Encoder = &Data->Encoder;

    Error_t RetCode = NO_ERROR;

    if (UseSampleRate)
    {
        if (bmTimeExpired(Encoder->Time) >= Encoder->Config.SampleRate)
            RetCode = smReadEncoder(Data);
    }
    else
        RetCode = smReadEncoder(Data);

    return RetCode;
}
