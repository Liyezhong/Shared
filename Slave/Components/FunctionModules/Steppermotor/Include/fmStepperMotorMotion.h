/****************************************************************************/
/*! \file fmStepperMotorMotion.h
 * 
 *  \brief Motion setup/control functions of module 'stepper motor'
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

//********************************************************************************/
// Public Constants and Definitions
//********************************************************************************/

#define SM_NUM_OF_PARAMETERSETS             3   //!< amount of movement parameter sets

#define MSEC                                (1000)              //!< mili-seconds per second
#define USEC                                (1000*1000)         //!< micro-seconds per second
#define NSEC                                (1000*1000*1000)    //!< nano-seconds per second

#define SM_MICROSTEPS_PER_HALFSTEP          32  //!< step unit used for calculation
#define SM_MICROSTEPS_PER_FULLSTEP          64  //!< step unit used for calculation


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! stepper motion state
typedef enum {
    MS_IDLE,                //!< motor is not moving
    MS_POSITION,            //!< motor is moving to a target position
    MS_SPEED,               //!< motor is moving to reach, or already moving with target speed
    MS_STOP,                //!< motor is stopping
} smMotionState_t;


//! movement phases of s-curve trajectory
typedef enum {
    PH_0_START,             //!< move with constant start speed 
    PH_1_ACC_JERK_UP,       //!< move with increasing acceleration
    PH_2_ACC_CONST,         //!< move with constant acceleration
    PH_3_ACC_JERK_DOWN,     //!< move with decreasing acceleration
    PH_4_VEL_CONST,         //!< move with constant target speed 
    PH_5_DEC_JERK_UP,       //!< move with increasing deceleration
    PH_6_DEC_CONST,         //!< move with constant deceleration
    PH_7_DEC_JERK_DOWN,     //!< move with decreasing deceleration
    PH_8_END,               //!< move with constant end speed 

    NUM_OF_MOTION_PHASES
} smPhase_t;


//! parameter set of single movement phase
typedef struct {
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
typedef struct {
    smPhase_t               OldPhase;       //!< old phase from which switch should happen
    Int64                   Position;       //!< and position within the old phase
} smParamSwitchTrigger_t;


//! data for parameter set switch
typedef struct {
    smParamSwitchTrigger_t  Trigger;        //!< definitions which will trigger a pending parameter switch
    smPhase_t               NewPhase;       //!< new phase to which should be switched
} smParamSwitch_t;


//! data for movement parameter set
typedef struct {
//! parameters for each single phase
    smPhaseParam_t          Ph[NUM_OF_MOTION_PHASES];

//! step width used to increase step count at each interrupt (in micro-steps)
    Int8                    dSteps;

//! flag for negative position count, if set means position counter is decremented
    Bool                    NegPosCount;

//! flag for rotation dir, if set means motor will rotate CCW. can be opposite to position counter
    Bool                    CCWRotDir;

//! data for parameter set switch
    smParamSwitch_t         SwitchSet;

//! only for speed request: to store the requested target speed
    Int16                   Speed;

//! only for stop of rotatory movement: to store the actual position each time a new parameter set is used
    UInt16                  OffsetPos;

//! only for stop of rotatory movement: to store the start phase each time a new parameter set is used
    smPhase_t               OffsetPhase;
} smParamSet_t;


//! actual nominal movement values 
typedef struct {
    Int64   s;          //!< position in microsteps
    Int64   v;          //!< velocity in microsteps/sec
#ifdef TRACE
    Int64   a;          //!< acceleration
    Int64   t;          //!< elpased time
#endif
} smNominalData_t;


//! CCR data
typedef struct {
    UInt16  Unit;       //!< Capture compare unit no
    UInt16  LastValue;  //!< value written to ccr at last irq execution
} smCCR_t;


//! control values for stopping the motor movement
typedef enum {
    SM_SC_NONE,         //!< don't stop
    SM_SC_DIR_CW,       //!< stop if moving direction is clockwise
    SM_SC_DIR_CCW,      //!< stop if moving direction is counter-clockwise
    SM_SC_OFFLIMIT,     //!< stop if actual position is off-limit
    SM_SC_ALWAYS,       //!< stop always
} smStopCondition_t;


//! motion control data
typedef struct {
//! array of parameter sets for combined movements
    smParamSet_t            Param[SM_NUM_OF_PARAMETERSETS];

//! actual used parameter set
    Int8                    ActSet;

//! new parameter set which should be used
    Int8                    NewSet;

//! always keeps actual values for position, velocity, acceleration of whole movement
    smNominalData_t         Nominal;

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

//! motors motion state, idle or moving to target speed/position
    volatile smMotionState_t    State;

//! set to signal that position movement have reached the target position
    Bool                    AtTargetPosition;

//! set to signal that speed movement have reached the target speed
    Bool                    AtTargetSpeed;

//! absolute position (in half-step)
    Int32                   Pos;
//! position value at which the actual position is reset to 0. Ignored if reset value is 0.
    UInt16                  ResetPos;

//! index into sinus table
    UInt8                   SinIndex;

//! stop movement according to set condition. stop position is always at half-step value.
    smStopCondition_t       Stop;

//! at each half-step this flag is set to trigger poll of limit switch status in the module task
    Bool                    LSTrigger;

//! CCR data used for ISR timing
    smCCR_t                 CCR;
    UInt8                   LSHitCnt;   
//! used to store return code from HAL functions called in ISR
    Error_t                 HALStatus;

} Motion_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

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
