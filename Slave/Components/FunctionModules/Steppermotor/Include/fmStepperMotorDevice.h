/****************************************************************************/
/*! \file fmStepperMotorDevice.h
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

#ifndef FMSTEPPERMOTORDEVICE_H
#define FMSTEPPERMOTORDEVICE_H


#ifndef SIMULATION
#include "defStepperMotor.h"
#else
    typedef enum
    {
		SMOT_DRIVER_DEFAULT		= 0,	///< default driver, default configuration from the HAL configuration file is used
		SMOT_DRIVER_TMC26X		= 1		///< trinamic TMC26x driver
	} StepperMotorDriverType_t;
    typedef enum
    {
	    SMOT_ROTDIR_CW    = 0,  //!< clockwise
	    SMOT_ROTDIR_CCW   = 1   //!< counterclockwise
    } StepperMotorRotDir_t;
#endif



/*! TMC26x motor driver register set */
typedef struct {
    UInt32 Mode[4];                             //!< trinamic TMC26x register settings
} TMC26xReg_t;

/*! TMC26x motor driver configuration data */
typedef struct
{
	UInt8                 ConfigMask;  		    //!< mask to determine complete stepper motor driver configuration
	TMC26xReg_t			  RegValue;			    //!< data used to initialize trinamic TMC26x stepper motor driver
} TMC26x_t;

/*! stepper motor driver configuration parameters */
typedef struct {
	StepperMotorDriverType_t    Type;		   	//!< used stepper motor driver type
	union
	{
		TMC26x_t    TMC26x;			            //!< data used to initialize trinamic TMC26x stepper motor driver
	} Data;
} MotorDriver_t;


// motor configuration data
typedef struct
{
    UInt16                  Resolution;         //!< motor resolution (full steps per turn)
    UInt8                   RunCS;              //!< run current scale in RunCS*1/32 percent, used when motor is moving
    UInt8                   StopCS;             //!< stop current scale in StopCS*1/32 percent, used when motor is standing still
    UInt16                  StopCSDelay;        //!< stop current scale is applied after this delay time (in ms )
} MotorConfig_t;


// motor configuration data
typedef struct
{
    StepperMotorRotDir_t    RotationDir;        //!< the motor's rotation dir (causes an increasing step counter)
    Int32                   MinPosition;        //!< minimum motor position in half-steps
    Int32                   MaxPosition;        //!< maximum motor position in half-steps

//    StepperMotorRotType_t   RotationType;      //!< the motor's rotation type: linear or rotatorically axis
    UInt16                  ResetPosition;      //!< position value at which the actual position is reset to 0 (needed for rotatory movement). Ignored if reset value is 0.
} FrameworkConfig_t;



typedef struct
{
    MotorDriver_t           Driver;             //!< motor driver data
    MotorConfig_t           Config;             //!< stepper motor configuration  
    FrameworkConfig_t       FrameworkConfig;    //!< framework configuration
    Handle_t                Handle;             //!< stepper motor HAL handle  
    Bool                    InversePosCount;    //!< if set, counter clock wise rotation dir of motor will increase position counter
    UInt32                  StopCSTimeout;      //!< timeout for switching motor currrent back from run current to stop current value
    UInt32                  StopCSTime;         //!< start time for StopCSTimeout
    UInt8                   ConfigMask;         //!< flags to track which part is already configured
} Motor_t;


//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/



//****************************************************************************/
// Module Function Prototypes
//****************************************************************************/
#ifndef SIMULATION

void smInitMotor (Motor_t *Motor);

Error_t smConfigureTMC26x(Motor_t* Motor, UInt32 RegVal, UInt8 Index);
Error_t smConfigureMotor(Motor_t* Motor, ConfigData_MOT_P1_t* Param);
Error_t smConfigureFramework1(Motor_t* Motor, ConfigData_MOT_P2_t* Param);
Error_t smConfigureFramework2(Motor_t* Motor, ConfigData_MOT_P3_t* Param);
Bool    smMotorConfigIsComplete(Motor_t* Motor);

Error_t smOpenMotor (Motor_t* Motor, UInt8 Instance);
Error_t smCloseMotor (Motor_t* Motor);

Error_t smCheckMotorStopCS (Motor_t *PtrMotor);

#endif


#endif /*FMSTEPPERMOTORDEVICE_H*/
