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
}


Error_t smConfigureEncoder(Encoder_t *Encoder, ConfigData_ENC_t* Param)
{
    Encoder->Config.Exists = Param->flag.exist;
//    Encoder->Config.RotationDir	= Param->flag.rotDir;
	DB2_TO_VAL(Param->resolution, Encoder->Config.Resolution);

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

    Encoder->Value = Pos % (MotorResolution << 1);
    if (Pos < 0)
        Encoder->Value += (MotorResolution << 1);
    Encoder->Value = Encoder->Value * Encoder->Config.Resolution / MotorResolution;
    if (Pos >= 0)
        Encoder->Value++;

    RetCode = halEncoderWrite (Encoder->Handle, ENC_REG_POSITION, Encoder->Value);
    
    if (NO_ERROR == RetCode)
    {
        Encoder->Pos     = Pos;
        Encoder->BasePos = Pos / (MotorResolution << 1);
        Encoder->BasePos *= (MotorResolution << 1);
        if (Pos < 0)
            Encoder->BasePos -= (MotorResolution << 1);
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

        RetCode = halEncoderWrite (Encoder->Handle, ENC_REG_ENDPOSITION, (Encoder->Config.Resolution << 1) - 1);
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


UInt32  encDuration;
UInt32  encMax=0;
UInt32  encMin=0xffffffff;

Error_t smReadEncoder (smData_t *Data)
{
    Error_t RetCode = NO_ERROR;

    Encoder_t *Encoder = &Data->Encoder;

    UInt16 ResetPosition = Data->Motor.FrameworkConfig.ResetPosition;

    UInt32  Value;
    Int32   Diff;

encDuration = halGetFastTick();

    if ((RetCode = halEncoderRead(Encoder->Handle, ENC_REG_POSITION, &Value)) < 0)
        return RetCode;
    Encoder->Time = bmGetTime();

    Value = Value % (Encoder->Config.Resolution << 1);
    if (Value == Encoder->Value)
        return NO_ERROR;

    Diff = Value - Encoder->Value;
    if (Diff < (-Encoder->Config.Resolution))                       // check for overflow
        Encoder->BasePos += (Data->Motor.Config.Resolution << 1);       // correction for overflow
    else
    {
        if (Diff > Encoder->Config.Resolution)                      // check for overflow
            Encoder->BasePos -= (Data->Motor.Config.Resolution << 1);   // correction for overflow
    }

    Encoder->Pos = Encoder->BasePos + Value * Data->Motor.Config.Resolution / Encoder->Config.Resolution;
    if (0 != ResetPosition)
        Encoder->Pos = (Encoder->Pos + ResetPosition) % ResetPosition;

    Encoder->Value = Value;

encDuration = halGetFastTick() - encDuration;
if (encDuration > encMax)
    encMax = encDuration;
if (encDuration < encMin)
    encMin = encDuration;

    return NO_ERROR;
}


Error_t smSampleEncoder(UInt16 Instance, Bool UseSampleRate)
{
    smData_t *Data = &smDataTable[Instance];
    Encoder_t *Encoder = &Data->Encoder;

    Error_t         RetCode = NO_ERROR;

	if (FALSE == Encoder->Config.Exists)
        return NO_ERROR;

    if (UseSampleRate)
    {
        if (bmTimeExpired(Encoder->Time) >= Encoder->Config.SampleRate)
            RetCode = smReadEncoder(Data);
    }
    else
        RetCode = smReadEncoder(Data);

    return RetCode;
}
