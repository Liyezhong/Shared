/****************************************************************************/
/*! \file
 * 
 *  \brief  Functions to manage stepper motor device of function module 'stepper motor'
 *
 *  $Rev:    $ 0.1
 *  $Date:   $ 21.06.2012
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

#ifndef FMSTEPPERMOTORDEVICE_H
#define FMSTEPPERMOTORDEVICE_H


#ifndef SIMULATION
#include "defStepperMotor.h"
#include "fmStepperMotorMemory.h"
#else
    typedef enum
    {
        SMOT_DRIVER_DEFAULT     = 0,    ///< default driver, default configuration from the HAL configuration file is used
        SMOT_DRIVER_TMC26X      = 1     ///< trinamic TMC26x driver
    } StepperMotorDriverType_t;
    typedef enum
    {
        SMOT_ROTDIR_CW    = 0,  //!< clockwise
        SMOT_ROTDIR_CCW   = 1   //!< counterclockwise
    } StepperMotorRotDir_t;
#endif


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! stepper motor revolution counter data
typedef struct
{
    UInt32                  Count;              //!< count of motor revolutions
    UInt32                  FullSteps;          //!< counts the motor steps in full-steps. reset at every full revolution
} smRevolutionCount_t;

//! stepper motor operation time data
typedef struct
{
    UInt32                  Minutes;            //!< operation time in minutes
    UInt32                  Duration;           //!< duration since last measurement start time in ms
    UInt32                  Start;              //!< measurement start time in ms
} smOperationTime_t;

//! stepper motor life cycle data
typedef struct
{
    smRevolutionCount_t     Revolutions;        //!< for keeping track of motor revolutions
    smOperationTime_t       OperationTime;      //!< for keeping track of motor operation time
} smLifeCycle_t;


//! TMC26x motor driver register set
typedef struct {
    UInt32 Mode[4];                             //!< trinamic TMC26x register settings
} TMC26xReg_t;

//! TMC26x motor driver configuration data
typedef struct
{
    UInt8                   ConfigMask;         //!< mask to determine complete stepper motor driver configuration
    TMC26xReg_t             RegValue;           //!< data used to initialize trinamic TMC26x stepper motor driver
} TMC26x_t;

//! motor driver configuration data
typedef struct {
    StepperMotorDriverType_t    Type;           //!< used stepper motor driver type
    union
    {
        TMC26x_t    TMC26x;                     //!< data used to initialize trinamic TMC26x stepper motor driver
    } Data;                                     //!< motor driver data
} smMotorDriver_t;


//! stepper motor configuration data
typedef struct
{
    UInt16                  Resolution;         //!< motor resolution (full steps per turn)
    UInt8                   RunCS;              //!< run current scale in RunCS*1/32 percent, used when motor is moving
    UInt8                   StopCS;             //!< stop current scale in StopCS*1/32 percent, used when motor is standing still
    UInt16                  StopCSDelay;        //!< stop current scale is applied after this delay time (in ms )
} smMotorConfig_t;


//! reference framework configuration data
typedef struct
{
    StepperMotorRotDir_t    RotationDir;        //!< the motor's rotation dir (causes an increasing step counter)
    Int32                   MinPosition;        //!< minimum motor position in half-steps
    Int32                   MaxPosition;        //!< maximum motor position in half-steps

//    StepperMotorRotType_t   RotationType;      //!< the motor's rotation type: linear or rotatorically axis
    UInt16                  ResetPosition;      //!< position value at which the actual position is reset to 0 (needed for rotatory movement). Ignored if reset value is 0.
} smFrameworkConfig_t;


//! stepper motor data
typedef struct
{
    smMotorDriver_t         Driver;             //!< motor driver data
    smMotorConfig_t         Config;             //!< stepper motor configuration  
    smFrameworkConfig_t     FrameworkConfig;    //!< framework configuration
    Handle_t                Handle;             //!< stepper motor HAL handle  
    Bool                    InversePosCount;    //!< if set, counter clock wise rotation dir of motor will increase position counter
    UInt32                  StopCSTimeout;      //!< timeout for switching motor currrent back from run current to stop current value
    UInt32                  StopCSTime;         //!< start time for StopCSTimeout
    smLifeCycle_t           LifeCycle;          //!< life cycle data of stepper motor
    UInt8                   ConfigMask;         //!< flags to track which part is already configured
} smMotor_t;


//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/



//****************************************************************************/
// Module Function Prototypes
//****************************************************************************/
#ifndef SIMULATION

void smInitMotor (smMotor_t *Motor, smMemory_t *Memory);
Error_t smConfigureTMC26x(smMotor_t* Motor, UInt32 RegVal, UInt8 Index);
Error_t smConfigureMotor(smMotor_t* Motor, ConfigData_MOT_P1_t* Param);
Error_t smConfigureFramework1(smMotor_t* Motor, ConfigData_MOT_P2_t* Param);
Error_t smConfigureFramework2(smMotor_t* Motor, ConfigData_MOT_P3_t* Param);
Bool smMotorConfigIsComplete(smMotor_t* Motor);
Error_t smOpenMotor (smMotor_t* Motor, UInt8 Instance);
Error_t smCloseMotor (smMotor_t* Motor);
void smCheckMotorStopCS (smMotor_t *PtrMotor, UInt16 Channel);
void smInitLifeCycleData(smMotor_t *Motor, smMemory_t *Memory);
void smCountOperationTime (smMotor_t *Motor);

#endif


#endif /*FMSTEPPERMOTORDEVICE_H*/
