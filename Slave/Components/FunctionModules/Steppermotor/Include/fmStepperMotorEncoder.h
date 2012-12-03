/****************************************************************************/
/*! \file
 * 
 *  \brief  Encoder position sampling of function module 'stepper motor'
 *
 *  $Rev:    $ 0.1
 *  $Date:   $ 27.06.2012
 *  $Author: $ Rainer Boehles
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

#ifndef FMSTEPPERMOTORENCODER_H
#define FMSTEPPERMOTORENCODER_H

#ifndef SIMULATION
#include "defStepperMotor.h"
#endif


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! step loss configuration data
typedef struct
{
    Int16                   WarnLimit;          //!< warning limit for step loss counter
    Int16                   ErrorLimit;         //!< error limit for step loss counter
} smStepLossConfig_t;


//! step loss data
typedef struct
{
    smStepLossConfig_t      Config;             //!< step loss configuration
    Bool                    Detected;           //!< indicates that step loss was detected
    Int32                   Count;              //!< actual amount of lost steps (in half-step)
    Int32                   StepDiff;           //!< amount of step deviation (in half-step), updated after movement is finished
//    Int32                   AccLostSteps;       //!< accumulated amount of lost steps (in half-step)
    Int32                   LastEncPos;         //!< last position value of encoder
    Bool                    EncPosOverflow;     //!< set if encoder position overflow is detected
    Bool                    EncXNeg;            //!< encoder crossed reset position in negative counting direction
    Bool                    PosOverflow;        //!< set if position overflow is detected
} smStepLoss_t;


//! encoder configuration data
typedef struct
{
    Bool                    Exists;            //!< flag to store the encoder's physically existence
    UInt16                  Resolution;        //!< encoder resolution
    UInt8                   SampleRate;        //!< encoder sample rate (ms) 
} smEncoderConfig_t;


//! encoder data
typedef struct
{
    smEncoderConfig_t       Config;             //!< configuration of encoder
    Handle_t                Handle;             //!< HAL handle

    Int32                   Pos;                //!< calculated encoder position (in half-steps)
    Int32                   BasePos;            //!< last encoder position, where encoder value was reseted to 0 (in half-steps)
    UInt32                  Value;              //!< last value read from encoder register (in encoder resolution)
    UInt32                  Time;               //!< last time of reading encoder (in ms)
    Bool                    FirstTime;          //!< indicates first time encoder evaluation in ISR
    Bool                    AtSpeedZero;        //!< set if target speed 0 will be reached with next IRQ, which will be final one and should terminate movement

    smStepLoss_t            StepLoss;           //!< step loss data and configuration

    Int32                   TargetPos;          //!< target position

    UInt8                   ConfigMask;         //!< flags to track which part is already configured
} smEncoder_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

#ifndef SIMULATION

void smInitEncoder(smEncoder_t *Profiles);
Error_t smConfigureEncoder(smEncoder_t *Encoder, ConfigData_ENC_t* Param);
Bool smEncoderConfigIsComplete (smEncoder_t *Encoder);
Error_t smInitEncoderPos(smEncoder_t *Encoder, UInt16 MotorResolution, Int32 Pos);
Error_t smOpenEncoder (smEncoder_t *Encoder, UInt8 Instance, smMotor_t *Motor);
Error_t smCloseEncoder (smEncoder_t *Encoder);
Error_t smSampleEncoder(UInt16 Instance, Bool UseSampleRate);

#endif

#endif /*FMSTEPPERMOTORENCODER_H*/
