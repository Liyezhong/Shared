/****************************************************************************/
/*! \file fmStepperMotorMotion.h
 * 
 *  \brief Public's for motion setup/control
 * 
 *
 *  $Version: $ 0.1
 *  $Date:    $ 04.07.2012
 *  $Author:  $ Rainer Boehles
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

#ifndef FMSTEPPERMOTORMOTION_H
#define FMSTEPPERMOTORMOTION_H

#ifdef SIMULATION
    #include "Global.h"
#endif

#define NUM_OF_PARAMETERSETS                3   //!< amount of movement parameter sets


#define MSEC                                (1000)
#define USEC                                (1000*1000)
#define NSEC                                (1000*1000*1000)

#define MICROSTEPS_PER_HALFSTEP             32  //!< step unit used for calculation
#define MICROSTEPS_PER_FULLSTEP             64  //!< step unit used for calculation

#define PROC

//! stepper motion state
typedef enum
{
    MS_IDLE,                //!< motor is not moving
    MS_POSITION,            //!< motor is moving to a target position
    MS_SPEED,               //!< motor is moving to reach, or already moving with target speed
} smMotionState_t;


//! all movement phases of s-curve trajectory
typedef enum
{
    PH_0_START,             //!< move with constant start speed 
    PH_1_ACC_JERK_UP,       //!< move with increasing acceleration
    PH_2_ACC_CONST,         //!< move with constant acceleration
    PH_3_ACC_JERK_DOWN,     //!< move with decreasing acceleration
    PH_4_VEL_CONST,         //!< move with constant target speed 
    PH_5_DEC_JERK_UP,       //!< move with increasing deeleration
    PH_6_DEC_CONST,         //!< move with constant deceleration
    PH_7_DEC_JERK_DOWN,     //!< move with decreasing deceleration
    PH_8_END,               //!< move with constant end speed 

    NUM_OF_MOTION_PHASES
} smPhase_t;


//! parameter set of single movement phase
typedef struct
{
    Int32   s;              //!< start position
    Int32   ds;             //!< distance moved 
    Int32   v;              //!< start velocity
    Int32   vE;             //!< end velocity
    Int32   a;              //!< start acceleration
#ifdef TRACE
    Int32   aE;             //!< end acceleration
#endif
    Int32   j;              //!< jerk
    Int32   dt;             //!< duration
} smPhaseParam_t;


//! trigger definition at which parameter set should be switched
typedef struct
{
    smPhase_t               OldPhase;       //!< old phase from which switch should happen
    Int64                   Position;       //!< and position within the old phase
} smParamSwitchTrigger_t;


//! data for parameter set switch
typedef struct
{
    smParamSwitchTrigger_t  Trigger;        //!< definitions which will trigger a pending parameter switch
    smPhase_t               NewPhase;       //!< new phase to which should be switched
} smParamSwitch_t;


//! data for movement parameter set
typedef struct
{
    smPhaseParam_t          Ph[NUM_OF_MOTION_PHASES];   //!< parameters for each single phase
    Int8                    dSteps;                     //!< step width used to increase step count at each interrupt (in micro-steps)

    Bool                    NegPosCount;                //!< flag for negative position count, if set means position counter is decremented

    Bool                    CCWRotDir;                  //!< flag for rotation dir, if set means motor will rotate CCW. can be opposite to position counter

    smParamSwitch_t         SwitchSet;                  //!< data for parameter set switch

    Int16                   Speed;                      //!< only for speed request: to store the requested target speed
    UInt16                  OffsetPos;                  //!< only for stop of rotatory movement: to store the actual position each time a new parameter set is used
    smPhase_t               OffsetPhase;                //!< only for stop of rotatory movement: to store the start phase each time a new parameter set is used
} smParamSet_t;


//! actual nominal movement values 
typedef struct
{
    Int64   s;          //!< position in microsteps
    Int64   v;          //!< velocity in microsteps/sec
#ifdef TRACE
    Int64   a;          //!< acceleration
    Int64   t;          //!< elpased time
#endif
} smNominalData_t;


//! CCR data
typedef struct
{
    UInt16  Unit;       //!< Capture compare unit no
    UInt16  LastValue;  //!< value written to ccr at last irq execution
} smCCR_t;


//! control values for stopping the motor movement
typedef enum
{
    SM_SC_NONE,         //!< don't stop
    SM_SC_DIR_CW,       //!< stop if moving direction is clockwise
    SM_SC_DIR_CCW,      //!< stop if moving direction is counter-clockwise
    SM_SC_OFFLIMIT,     //!< stop if actual position is off-limit
    SM_SC_ALWAYS,       //!< stop always
} smStopCondition_t;


//! motion control data
typedef struct
{
    smParamSet_t            Param[NUM_OF_PARAMETERSETS];    //!< array of parameter sets for combined movements
    Int8                    ActSet;             //!< actual used parameter set
    Int8                    NewSet;             //!< new parameter set which should be used

    smNominalData_t         Nominal;            //!< always keeps actual values for position, velocity, acceleration of whole movement

    smPhase_t               Phase;              //!< actual phase
    Int64                   v0;                 //!< velocity at start of phase
    Int64                   a0;                 //!< acceleration at start of phase
    Int64                   s;                  //!< distance within actual phase
    Int64                   j;                  //!< jerk of actual phase
    Int64                   t;                  //!< duration of actual phase
#ifdef STEPPER_ISR_TIME_OPTIMIZATION_OFF
    Int64                   dt;                 //!< interval time for next step
#else
    Int32                   dt;                 //!< interval time for next step
#endif

    smMotionState_t         State;              //!< motors motion state, idle or moving to target speed/position

    Bool                    AtTargetPosition;   //!< set to signal that position movement have reached the target position
    Bool                    AtTargetSpeed;      //!< set to signal that speed movement have reached the target speed

    Int32                   Pos;                //!< absolute position (in half-step)
    UInt16                  ResetPos;           //!< position value at which the actual position is reset to 0. Ignored if reset value is 0.

    UInt8                   SinIndex;           //!< index into sinus table

    smStopCondition_t       Stop;               //!< stop movement according to set condition. stop position is always at half-step value.

    Bool                    LSTrigger;          //!< at each half-step this flag is set to trigger poll of limit switch status in the module task

    smCCR_t                 CCR;                //!< CCR data used for ISR timing
    UInt8                   LSHitCnt;   
} Motion_t;


//! init. motion control data
void smInitMotion (Motion_t *Motion, UInt16 Instance);

//! determine next phase
smPhase_t smNextPhase(smPhaseParam_t *Param, Int8 Phase);

//! move to target position
Error_t smPositionRequest (UInt16 Instance, Int32 Position, UInt8 ProfileIndex);

//! move with target speed
Error_t smSpeedRequest (UInt16 Instance, Int32 Speed, UInt8 ProfileIndex);

//! stop the motion
void smStopMotion(UInt16 Instance);



#endif /*FMSTEPPERMOTORMOTION_H*/
