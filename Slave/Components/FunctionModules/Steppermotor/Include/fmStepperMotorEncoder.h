/****************************************************************************/
/*! \file fmStepperMotorEncoder.h
 * 
 *  $Rev:    $ 0.1
 *  $Date:   $ 21.06.2012
 *  $Author: $ Rainer Boehles
 *
 *  \brief 
 *
 *         
 * <dl compact><dt>Company:</dt><dd> Leica Biosystems Nussloch GmbH </dd></dl>
 *
 * (c) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 * This is unpublished proprietary source code of Leica.
 * The copyright notice does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef FMSTEPPERMOTORENCODER_H
#define FMSTEPPERMOTORENCODER_H


#define ENCODER_MODE    TIM_MODE_ENCODER_3      // mode used when encoder is opened by hal ( see halEncoderOpen() )
#if TIM_MODE_ENCODER_3==ENCODER_MODE
    #define ENCODER_COUNT_SHIFT 2
#elif TIM_MODE_ENCODER_2==ENCODER_MODE
    #define ENCODER_COUNT_SHIFT 1
#elif TIM_MODE_ENCODER_1==ENCODER_MODE
    #define ENCODER_COUNT_SHIFT 1
#else
    #error You must define encoder mode here
#endif


#ifndef SIMULATION
#include "defStepperMotor.h"
//#else
//    typedef enum
//    {
//      SMOT_ROTDIR_CW    = 0,  //!< clockwise
//      SMOT_ROTDIR_CCW   = 1   //!< counterclockwise
//    } StepperMotorRotDir_t;
#endif


// step loss configuration data
typedef struct
{
    Int16                   WarnLimit;          //!< warning limit for step loss counter
    Int16                   ErrorLimit;         //!< error limit fpr step loss counter
} StepLossConfig_t;


// encoder configuration data
typedef struct
{
    StepLossConfig_t        Config;             //!< step loss configuration
    Bool                    Detected;           //!< indicates that step loss was detected
    Int32                   Count;              //!< actual amount of lost steps (in half-step)
//    Int32                   AccLostSteps;       //!< accumulated amount of lost steps (in half-step)
    Int32                   LastEncPos;         //!< last position value of encoder
    Bool                    EncPosOverflow;     //!< set if encoder position overflow is detected
    Bool                    EncXNeg;            //!< encoder crossed reset position in negative counting direction
    Bool                    PosOverflow;        //!< set if position overflow is detected
} StepLoss_t;


// encoder configuration data
typedef struct
{
    Bool                    Exists;            //!< flag to store the encoder's physically existence (use as HAL-channel?)
//    StepperMotorRotDir_t    RotationDir;       //!< the encoder's rotation dir (can be different from the motor's)
    UInt16                  Resolution;        //!< encoder resolution (ticks per revolution)
    UInt8                   SampleRate;        //!< encoder sample rate (ms) 
} EncoderConfig_t;


typedef struct
{
    EncoderConfig_t         Config;             //!< configuration of encoder
    Handle_t                Handle;             //!< HAL handle

    Int32                   Pos;                //!< calculated encoder position (in half-steps)
    Int32                   BasePos;            //!< last encoder position, where encoder value was reseted to 0 (in half-steps)
    UInt32                  Value;              //!< last value read from encoder register (in encoder resolution)
    UInt32                  Time;               //!< last time of reading encoder (in ms)
    Bool                    FirstTime;          //!< indicates first time encoder evaluation in ISR
    Bool                    AtSpeedZero;        //!< set if target speed 0 will be reached with next IRQ, which will be final one and should terminate movement

    StepLoss_t              StepLoss;           //!< step loss data and configuration

    Int32                   TargetPos;          //!< target position

    UInt8                   ConfigMask;         //!< flags to track which part is already configured
} Encoder_t;



//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/



//****************************************************************************/
// Module Function Prototypes
//****************************************************************************/

#ifndef SIMULATION

void smInitEncoder(Encoder_t *Profiles);

Error_t smConfigureEncoder(Encoder_t *Encoder, ConfigData_ENC_t* Param);

Bool smEncoderConfigIsComplete (Encoder_t *Encoder);

Error_t smInitEncoderPos(Encoder_t *Encoder, UInt16 MotorResolution, Int32 Pos);

Error_t smOpenEncoder (Encoder_t *Encoder, UInt8 Instance, UInt16 MotorResolution);

Error_t smCloseEncoder (Encoder_t *Encoder);

Error_t smSampleEncoder(UInt16 Instance, Bool UseSampleRate);

#endif

#endif /*FMSTEPPERMOTORENCODER_H*/
