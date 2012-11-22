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


#ifndef SIMULATION
#include "defStepperMotor.h"
//#else
//    typedef enum
//    {
//	    SMOT_ROTDIR_CW    = 0,  //!< clockwise
//	    SMOT_ROTDIR_CCW   = 1   //!< counterclockwise
//    } StepperMotorRotDir_t;
#endif



// encoder configuration data
typedef struct
{
    Bool                    Exists;            //!< flag to store the encoder's physically existence (use as HAL-channel?)
//    StepperMotorRotDir_t    RotationDir;       //!< the encoder's rotation dir (can be different from the motor's)
    UInt16                  Resolution;        //!< encoder resolution (ticks per revolution)
    UInt8                   SampleRate;        //!< encoder sample rate (ms) 

//    Int16                   StepLossWarnLimit; //!< warning limit for step loss counter
//    Int16                   StepLossErrorLimit;//!< error limit fpr step loss counter
} EncoderConfig_t;


typedef struct
{
    EncoderConfig_t         Config;             //!< configuration of encoder
    Handle_t                Handle;             //!< HAL handle

    Int32                   Pos;                //!< calculated encoder position (in half-steps)
    Int32                   BasePos;            //!< last encoder position, where encoder value was reseted to 0 (in half-steps)
    UInt32                  Value;              //!< last value read from encoder register (in encoder resolution)
    UInt32                  Time;               //!< last time of reading encoder (in ms)

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
