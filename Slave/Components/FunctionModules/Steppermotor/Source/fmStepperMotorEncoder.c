/****************************************************************************/
/*! \file
 * 
 *  \brief  Position sampling with encoder device of function module
 *          'stepper motor'
 *
 *  $Rev:    $ 0.1
 *  $Date:   $ 27.06.2012
 *  $Author: $ Rainer Boehles
 *
 *  \b Description:
 *
 *      This module contains functions to control and process position data
 *      from encoder. An incremental rotatory encoder with quadrature output 
 *      is supported.
 *      The encoder is counting each transistion on both signals.
 *      The encoder counter value is converted to the encoder position (half-steps).
 *      In case of rotatory movement the encoder position is reseted at motor's
 *      reset position.
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
#include "halCommon.h"
#include "halInterrupt.h"
#include "halEncoder.h"
#include "fmStepperMotor.h"
#include "fmStepperMotorEncoder.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define SM_ENCODER_SAMPLERATE 1                     //!< encoder sample rate (ms)

#define SM_ENCODER_MODE    TIM_MODE_ENCODER_3       //!< mode used when encoder is opened by hal ( see halEncoderOpen() )
#if TIM_MODE_ENCODER_3==SM_ENCODER_MODE
    #define SM_ENCODER_COUNT_SHIFT 2                //!< used as multiplicator to compute encoder resolution
#elif TIM_MODE_ENCODER_2==SM_ENCODER_MODE
    #define SM_ENCODER_COUNT_SHIFT 1
#elif TIM_MODE_ENCODER_1==SM_ENCODER_MODE
    #define SM_ENCODER_COUNT_SHIFT 1
#else
    #error You must define encoder mode here
#endif


/******************************************************************************/
/*! 
 *  \brief  Initialize encoder data
 *
 *      Encoder data is initialized.
 *      All configuration data is set to 'unconfigured', which means all
 *      configuation data have to be set first by master, before devices
 *      can be used.
 * 
 *  \iparam  Encoder = pointer to encoder data
 * 
 ******************************************************************************/
void smInitEncoder(smEncoder_t *Encoder)
{
    Encoder->ConfigMask = 0;    // nothing configured

    Encoder->Config.Exists      = FALSE;
    Encoder->Config.SampleRate  = SM_ENCODER_SAMPLERATE;
    Encoder->Handle             = 0;
    Encoder->Time               = bmGetTime();

    Encoder->StepLoss.Config.WarnLimit   = 0;
    Encoder->StepLoss.Config.ErrorLimit  = 0;
}


/******************************************************************************/
/*! 
 *  \brief  Set encoder configuration
 *
 *      Configuration data received from master is applied to the encoder.
 * 
 *  \iparam  Encoder = pointer to encoder data
 *  \iparam  Param   = pointer to configuration data received from master
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smConfigureEncoder(smEncoder_t *Encoder, ConfigData_ENC_t* Param)
{
    Encoder->Config.Exists = Param->flag.exist;
    DB2_TO_VAL(Param->resolution, Encoder->Config.Resolution);
    DB2_TO_VAL(Param->stepLoss_WarnLimit, Encoder->StepLoss.Config.WarnLimit);
    DB2_TO_VAL(Param->stepLoss_ErrLimit, Encoder->StepLoss.Config.ErrorLimit);

    Encoder->ConfigMask |= BIT(0);

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Check completeness of encoder configuration
 *
 *      Check if all necessary configuration data for encoder have been received
 * 
 *  \iparam  Encoder = pointer to encoder data
 * 
 *  \return  true, if configuration is complete
 *
 ******************************************************************************/
Bool smEncoderConfigIsComplete (smEncoder_t *Encoder)
{
    UInt8 CompleteMask = BIT(0);

    return (CompleteMask == Encoder->ConfigMask);
}


/******************************************************************************/
/*! 
 *  \brief  Initialize encoder position
 *
 *      Encoder counter value is calculated from supplied init position and 
 *      applied to the encoder device. Further counting starts from the applied
 *      counter value.
 * 
 *  \iparam  Encoder         = pointer to encoder data
 *  \iparam  MotorResolution = motor resolution (full step), needed to convert
 *                             position to encoder counter value
 *  \iparam  Pos             = initialization position
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smInitEncoderPos(smEncoder_t *Encoder, UInt16 MotorResolution, Int32 Pos)
{
    Error_t RetCode = NO_ERROR;

    MotorResolution = (MotorResolution << 1);   // convert from full-step to half-step

    Encoder->Value = Pos % MotorResolution;
    if (Pos < 0) {
        Encoder->Value += MotorResolution;
    }
    Encoder->Value = (10 * Encoder->Value) * (Encoder->Config.Resolution << SM_ENCODER_COUNT_SHIFT) / MotorResolution;
    if ((Encoder->Value % 10) < 5) {
        Encoder->Value /= 10;
    }
    else {
        Encoder->Value = Encoder->Value / 10 + 1;
    }

    RetCode = halEncoderWrite (Encoder->Handle, ENC_REG_POSITION, Encoder->Value);
    
    if (NO_ERROR == RetCode)
    {
        Encoder->Pos     = Pos;
        Encoder->BasePos = Pos / MotorResolution;
        Encoder->BasePos *= MotorResolution;
        if (Pos < 0) {
            Encoder->BasePos -= MotorResolution;
        }
    }
    return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief  Open encoder device
 *
 *      Encoder device is opened.
 *      Maximum count value for one full turn of encoder is applied from the
 *      encoder configuration.
 *      Encoder count direction is set up to match motor position count direction.
 *      Encoder position is initialized with zero.
 *
 *  \iparam  Encoder    = pointer to encoder data
 *  \iparam  Instance   = Module instance number
 *  \iparam  Motor      = pointer to motor data
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smOpenEncoder (smEncoder_t *Encoder, UInt8 Instance, smMotor_t *Motor)
{
    Error_t RetCode = NO_ERROR;

    if(Encoder->Config.Exists)
    {
        if ((RetCode = halEncoderOpen(HAL_STEPPER_ENCODER + Instance, 0, NULL)) < 0) {
            return RetCode;
        }

        Encoder->Handle = RetCode;

        RetCode = halEncoderWrite (Encoder->Handle, ENC_REG_ENDPOSITION, (Encoder->Config.Resolution << SM_ENCODER_COUNT_SHIFT) - 1);
        if (NO_ERROR == RetCode) {
            RetCode = halEncoderControl (Encoder->Handle, ENC_INVERSE_COUNT, SMOT_ROTDIR_CW == Motor->FrameworkConfig.RotationDir);
        }
        if (NO_ERROR == RetCode) {
            RetCode = smInitEncoderPos (Encoder, Motor->Config.Resolution, 0);
        }

        if (RetCode < 0)
        {
            smCloseEncoder(Encoder);
            return RetCode;
        }
    }

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Close encoder device
 *
 *      Encoder device is closed.
 * 
 *  \iparam  Encoder = pointer to encoder data
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smCloseEncoder (smEncoder_t *Encoder)
{
    Error_t RetCode = NO_ERROR;

    if (0 != Encoder->Handle)
    {
        RetCode = halEncoderClose(Encoder->Handle);
        Encoder->Handle = 0;
    }

    return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief  Read encoder and calculate encoder position
 *
 *      The encoder counter value is read and converted to the encoder position.
 *      Conversion take care for encoder counter value overflow, which occurs
 *      at each full turn. To minimize position errors the calculated
 *      encoder position is rounded. This is due to different resolutions of
 *      motor and encoder.
 *      In case of roatatory movement the encoder position too is reseted
 *      at motor's reset position.
 * 
 *  \iparam  Data = Module instance data pointer
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smReadEncoder (smData_t *Data)
{
    Error_t RetCode = NO_ERROR;

    smEncoder_t *Encoder = &Data->Encoder;
    UInt16 EncoderResolution = (Encoder->Config.Resolution << SM_ENCODER_COUNT_SHIFT); // convert to real resolution

    UInt16 ResetPosition = Data->Motor.FrameworkConfig.ResetPosition;

    UInt32  Value;
    Int32   Diff;
    Int32   Pos;

    if ((RetCode = halEncoderRead(Encoder->Handle, ENC_REG_POSITION, &Value)) < 0) {
        return RetCode;
    }

    Encoder->Time = bmGetTime();

    Value = Value % EncoderResolution;
    if (Value == Encoder->Value) {
        return NO_ERROR;
    }

    Diff = Value - Encoder->Value;
    if (Diff < (-(EncoderResolution >> 1))) {                                       // check for overflow
        Encoder->BasePos += (Data->Motor.Config.Resolution << 1);                   // correction for overflow
    }
    else
    {
        if (Diff > (EncoderResolution >> 1)) {                                      // check for overflow
            Encoder->BasePos -= (Data->Motor.Config.Resolution << 1);               // correction for overflow
        }
    }
    if (0 != ResetPosition) {
        Encoder->BasePos = Encoder->BasePos % ResetPosition;
    }

// position value calculation with rounding
    Pos = (Value << 3) * Data->Motor.Config.Resolution / (EncoderResolution >> 1);  // pos = value * 8 ...
    if ((Pos & 7) < 4) {                                                            // (pos % 8) < 4
        Pos = Pos >> 3;                                                             // pos / 8
    }
    else {
        Pos = (Pos >> 3) + 1;                                                       // (pos / 8) + 1
    }
    Pos += Encoder->BasePos;

// do correction for rotatory position value
    if (0 != ResetPosition)
    {
        Pos = (Pos % ResetPosition);
        if (Pos < 0) {
            Pos += ResetPosition;
        }
    }
    Encoder->Pos = Pos; // ATTENTION! because encoder position is used in ISR always modify the value in a single operation (or disable stepper interrupts)

    Encoder->Value = Value;

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Sample encoder
 *
 *      Encoder counter value is sampled and converted to encoder position.
 *      Sampling is either performed immediately or after defined
 *      sample rate time (ms).
 *
 * 
 *  \iparam  Instance       = Module instance number
 *  \iparam  UseSampleRate  = true, use defined sample rate
 *                            false, sample immediately
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smSampleEncoder(UInt16 Instance, Bool UseSampleRate)
{
    smData_t *Data = &smDataTable[Instance];
    smEncoder_t *Encoder = &Data->Encoder;

    Error_t RetCode = NO_ERROR;

    if (UseSampleRate)
    {
        if (bmTimeExpired(Encoder->Time) >= Encoder->Config.SampleRate) {
            RetCode = smReadEncoder(Data);
        }
    }
    else {
        RetCode = smReadEncoder(Data);
    }

    return RetCode;
}
