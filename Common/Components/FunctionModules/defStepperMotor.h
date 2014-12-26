/****************************************************************************/
/*! \file
 * 
 *  \brief  Common declarations of function module 'stepper motor' to be used
 *          by master and slave
 *
 *
 *  $Version: $ 0.1
 *  $Date:    $ 02.03.2012
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

#ifndef DEFSTEPPERMOTOR_H_
#define DEFSTEPPERMOTOR_H_

#include "ModuleIDs.h"

//********************************************************************************/
// Public Constants and Definitions
//********************************************************************************/

#define MAX_MOTION_PROFIL       5       //!< max allowed count of motion profiles

//-------------------------------------------------------------------------------
// Events for this function module (Errors, Warnings, Infos)
//-------------------------------------------------------------------------------

//! to create unique error code for stepper module
#define BUILD_SM_ERRCODE(Class,Code)   BUILD_ERRCODE(MODULE_ID_STEPPER, Class, Code)

//! command was received while the motor's state is not able to execute it
#define E_SMOT_INVALID_STATE                BUILD_SM_ERRCODE(ERRCLASS_ERROR, 1)

//! configuration data is not complete
#define E_SMOT_CONFIG_INCOMPLETE            BUILD_SM_ERRCODE(ERRCLASS_ERROR, 2)

//! position code configuration doesn't match limit switch configuration
#define E_SMOT_CONFIG_LIMITSWITCH           BUILD_SM_ERRCODE(ERRCLASS_ERROR, 3)

//! configuration data for unknown stepper driver received
#define E_SMOT_CONFIG_INVALID_DRIVER        BUILD_SM_ERRCODE(ERRCLASS_ERROR, 4)

//! invalid reference run configuration
#define E_SMOT_CONFIG_REFRUN                BUILD_SM_ERRCODE(ERRCLASS_ERROR, 5)

//! invalid amount of profiles
#define E_SMOT_CONFIG_PROFILES              BUILD_SM_ERRCODE(ERRCLASS_ERROR, 6)

//! profile data for given vMax invalid
#define E_SMOT_CONFIG_INVALID_PROFILE       BUILD_SM_ERRCODE(ERRCLASS_ERROR, 7)

//! profile data for vMin/microsteps not compatible to timer reolution
#define E_SMOT_CONFIG_INVALID_PROFILE_VMIN  BUILD_SM_ERRCODE(ERRCLASS_ERROR, 8)

//! profile data for vMax/microsteps not compatible to interrupt timing
#define E_SMOT_CONFIG_INVALID_PROFILE_VMAX  BUILD_SM_ERRCODE(ERRCLASS_ERROR, 9)

//! during a reference run a timeout occurred
#define E_SMOT_REFRUN_TIMEOUT               BUILD_SM_ERRCODE(ERRCLASS_ERROR, 10)

//! during a reference run the max distance was exceeded
#define E_SMOT_REFRUN_MAXDISTANCE           BUILD_SM_ERRCODE(ERRCLASS_ERROR, 11)

//! during a reference run the motor stopped unexpectedly
#define E_SMOT_REFRUN_ERROR                 BUILD_SM_ERRCODE(ERRCLASS_ERROR, 12)

//! all stepper operations stopped because of timer overload - BOARD RESTART NEEDED
#define E_SMOT_TIMER_OVERLOAD               BUILD_SM_ERRCODE(ERRCLASS_ERROR, 13)

//! position code from limitswitches is invalid
#define E_SMOT_INVALID_POSCODE              BUILD_SM_ERRCODE(ERRCLASS_ERROR, 20)

//! position code from limitswitches does not match with actual motor position
#define E_SMOT_INVALID_POSCODE_POSITION     BUILD_SM_ERRCODE(ERRCLASS_ERROR, 21)

//! movement rotation direction from this position code not allowed
#define E_SMOT_INVALID_POSCODE_DIR          BUILD_SM_ERRCODE(ERRCLASS_ERROR, 22)

//! during a movement the motor stopped at an position code signal
#define E_SMOT_STOP_BY_POSCODE              BUILD_SM_ERRCODE(ERRCLASS_ERROR, 23)

//! during a movement the motor stopped because actual position get off-limit
#define E_SMOT_STOP_BY_OFFLIMIT             BUILD_SM_ERRCODE(ERRCLASS_ERROR, 24)

//! during a movement the motor stopped because step difference between encoder and counter get's too big
#define E_SMOT_STOP_BY_STEPDIFF             BUILD_SM_ERRCODE(ERRCLASS_ERROR, 25)

//! during a movement the motor stopped because of too much lost steps
#define E_SMOT_STOP_BY_STEPLOSS             BUILD_SM_ERRCODE(ERRCLASS_ERROR, 26)

//! during a movement the motor stopped because of shutdown request from master
#define E_SMOT_STOP_BY_SHUTDOWN             BUILD_SM_ERRCODE(ERRCLASS_ERROR, 27)

//! during a movement the motor stopped by emergency stop request
#define E_SMOT_STOP_BY_EMERGENCYSTOP        BUILD_SM_ERRCODE(ERRCLASS_ERROR, 28)

//! encoder usage not compatible to motion profile
#define E_SMOT_PROFILE_ENCODER_UNFIT        BUILD_SM_ERRCODE(ERRCLASS_ERROR, 29)

//! requested speed exceeds vMin/vMax of selected profile
#define E_SMOT_MOTION_PROFILE_UNFIT         BUILD_SM_ERRCODE(ERRCLASS_ERROR, 30)


//! during a movement step loss was detected
#define W_SMOT_STEPLOSS                     BUILD_SM_ERRCODE(ERRCLASS_WARNING, 1)

//! during a movement step difference was detected
#define W_SMOT_STEPDIFF                     BUILD_SM_ERRCODE(ERRCLASS_WARNING, 2)


//! need initialization
#define I_SMOT_NEED_INIT                    BUILD_SM_ERRCODE(ERRCLASS_INFO, 1)

//! rotation has been stopped
#define I_SMOT_ROTATION_STOPPED             BUILD_SM_ERRCODE(ERRCLASS_INFO, 2)


//-------------------------------------------------------------------------------
// CAN message ID's and DLC's used by this function module
//-------------------------------------------------------------------------------

//! CAN msg ID - Set the configuration parameter
#define MSG_SMOT_CONFIG                 BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 1)  // CAN-ID: 0x1028xxx1

//! CAN msg ID - Acknowledge the configuration parameter
#define MSG_SMOT_CONFIG_ACK             BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 0)  // CAN-ID: 0x1028xxx0
#define MSG_SMOT_CONFIG_ACK_DLC         sizeof(Msg_ConfigAckData_t)
//!< CAN msg DLC - Acknowledge the configuration parameter

//! CAN msg ID - Enable / Disable the stepper
#define MSG_SMOT_SET_ENABLE             BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 1)  // CAN-ID: 0x1038xxx1
#define MSG_SMOT_SET_ENABLE_DLC         sizeof(Msg_EnableData_t)
//!< CAN msg DLC - Enable / Disable the stepper

//! CAN msg ID - Acknowledge the enable / disable stepper
#define MSG_SMOT_SET_ENABLE_ACK         BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 0)  // CAN-ID: 0x1038xxx0
#if 0
#define MSG_SMOT_SET_ENABLE_ACK_DLC     sizeof(Msg_EnableAckData_t)
#endif
//[Brandon 2014/04/09]: change message length back to 0 for backward compatability
#define MSG_SMOT_SET_ENABLE_ACK_DLC     0
//!< CAN msg DLC - Acknowledge the enable / disable stepper

//! CAN msg ID - Request a reference movement
#define MSG_SMOT_REQ_REF_RUN            BUILD_CAN_ID(CMD_CLASS_FUNCTION, 9, 1)  // CAN-ID: 0x1048xxx1
#define MSG_SMOT_REQ_REF_RUN_DLC        sizeof(Msg_RefRunData_t)
//!< CAN msg DLC - Request a reference movement

//! CAN msg ID - Acknowledge the reference movement request
#define MSG_SMOT_REQ_REF_RUN_ACK        BUILD_CAN_ID(CMD_CLASS_FUNCTION, 9, 0)  // CAN-ID: 0x1048xxx0
#define MSG_SMOT_REQ_REF_RUN_ACK_DLC    sizeof(Msg_RefRunReqAckData_t)
//!< CAN msg DLC - Acknowledge the reference movement request

//! CAN msg ID - Acknowledges that execution of a reference movement is finished
#define MSG_SMOT_REFERENCE_RUN_ACK      BUILD_CAN_ID(CMD_CLASS_FUNCTION, 3, 0)  // CAN-ID: 0x1018xxx0
#define MSG_SMOT_REFERENCE_RUN_ACK_DLC  sizeof(Msg_RefRunAckData_t)
//!< CAN msg DLC - Acknowledges that execution of a reference movement is finished

//! CAN msg ID - Sets the motors target position, start the movement
#define MSG_SMOT_TARGET_POS             BUILD_CAN_ID(CMD_CLASS_FUNCTION, 10, 1) // CAN-ID: 0x1050xxx1
#define MSG_SMOT_TARGET_POS_DLC         sizeof(Msg_TargetPositionData_t)
//!< CAN msg DLC - Sets the motors target position, start the movement

//! CAN msg ID - Acknowledge the reception of a target position command (the request, but not the movement!)
#define MSG_SMOT_TARGET_POS_ACK         BUILD_CAN_ID(CMD_CLASS_FUNCTION, 10, 0) // CAN-ID: 0x1050xxx0
#define MSG_SMOT_TARGET_POS_ACK_DLC     sizeof(Msg_TargetPosAckData_t)
//!< CAN msg DLC - Acknowledge the reception of a target position command (the request, but not the movement!)

//! CAN msg ID - Sets the motors target speed, start the movement
#define MSG_SMOT_TARGET_SPEED           BUILD_CAN_ID(CMD_CLASS_FUNCTION, 11, 1) // CAN-ID: 0x1058xxx1
#define MSG_SMOT_TARGET_SPEED_DLC       sizeof(Msg_TargetSpeedData_t)
//!< CAN msg DLC - Sets the motors target speed, start the movement

//! CAN msg ID - Acknowledge the reception of a target speed command (the request, but not the movement!)
#define MSG_SMOT_TARGET_SPEED_ACK       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 11, 0)
#define MSG_SMOT_TARGET_SPEED_ACK_DLC   sizeof(Msg_TargetSpeedAckData_t)
//!< CAN msg DLC - Acknowledge the reception of a target speed command (the request, but not the movement!)

//! CAN msg ID - Acknowledge the execution of a movement (position or speed target, not reference run)
#define MSG_SMOT_MOVEMENT_ACK           BUILD_CAN_ID(CMD_CLASS_FUNCTION, 12, 0) // CAN-ID: 0x1060xxx0
#define MSG_SMOT_MOVEMENT_ACK_DLC       sizeof(Msg_MovementAckData_t)
//!< CAN msg DLC - Acknowledge the execution of a movement (position or speed target, not reference run)

//! CAN msg ID - Request the actual motor position
#define MSG_SMOT_ACT_POS_REQ            BUILD_CAN_ID(CMD_CLASS_FUNCTION, 13, 1) // CAN-ID: 0x1068xxx1
#define MSG_SMOT_ACT_POS_REQ_DLC        0
//!< CAN msg DLC - Request the actual motor position

//! CAN msg ID - Send the actual motor position
#define MSG_SMOT_ACT_POS                BUILD_CAN_ID(CMD_CLASS_FUNCTION, 13, 0) // CAN-ID: 0x1068xxx1
#define MSG_SMOT_ACT_POS_DLC            sizeof(Msg_PositionData_t)
//!< CAN msg DLC - Send the actual motor position

//! CAN msg ID - Request the actual motor speed
#define MSG_SMOT_ACT_SPEED_REQ          BUILD_CAN_ID(CMD_CLASS_FUNCTION, 14, 1) // CAN-ID: 0x1070xxx1
#define MSG_SMOT_ACT_SPEED_REQ_DLC      0
//!< CAN msg DLC - Request the actual motor speed

//! CAN msg ID - Send the actual motor speed
#define MSG_SMOT_ACT_SPEED              BUILD_CAN_ID(CMD_CLASS_FUNCTION, 14, 0) // CAN-ID: 0x1070xxx0
#define MSG_SMOT_ACT_SPEED_DLC          sizeof(Msg_SpeedData_t)
//!< CAN msg DLC - Send the actual motor speed

//! CAN msg ID - Request motor operation time
#define MSG_SMOT_OPTIME_REQ             BUILD_CAN_ID(CMD_CLASS_FUNCTION, 15, 1) // CAN-ID: 0x1078xxx1 
#define MSG_SMOT_OPTIME_REQ_DLC         0
//!< CAN msg DLC - Request motor operation time

//! CAN msg ID - Send motor operation time
#define MSG_SMOT_OPTIME                 BUILD_CAN_ID(CMD_CLASS_FUNCTION, 15, 0) // CAN-ID: 0x1078xxx1 
#define MSG_SMOT_OPTIME_DLC             sizeof(Msg_OperationTimeData_t)
//!< CAN msg DLC - Send motor operation time

//! CAN msg ID - Request motor revolution count
#define MSG_SMOT_REVCOUNT_REQ           BUILD_CAN_ID(CMD_CLASS_FUNCTION, 16, 1) // CAN-ID: 0x1080xxx1 
#define MSG_SMOT_REVCOUNT_REQ_DLC       0
//!< CAN msg DLC - Request motor revolution count

//! CAN msg ID - Send motor revolution count
#define MSG_SMOT_REVCOUNT               BUILD_CAN_ID(CMD_CLASS_FUNCTION, 16, 0) // CAN-ID: 0x1080xxx1 
#define MSG_SMOT_REVCOUNT_DLC           sizeof(Msg_RevolutionsData_t)
//!< CAN msg DLC - Send motor revolution count

//! CAN msg ID - Request motor direction change count
#define MSG_SMOT_DIRCOUNT_REQ           BUILD_CAN_ID(CMD_CLASS_FUNCTION, 17, 1) // CAN-ID: 0x1088xxx1 
#define MSG_SMOT_DIRCOUNT_REQ_DLC       0
//!< CAN msg DLC - Request motor direction change count

//! CAN msg ID - Send motor direction change count
#define MSG_SMOT_DIRCOUNT               BUILD_CAN_ID(CMD_CLASS_FUNCTION, 17, 0) // CAN-ID: 0x1088xxx1 
#define MSG_SMOT_DIRCOUNT_DLC           sizeof(Msg_DirChangeData_t)
//!< CAN msg DLC - Send motor direction change count


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

#pragma pack(push)
#pragma pack(1)

//! used to access values which are encoded in 2 CAN data bytes
typedef struct {
    UInt8  hi;      //!< 2nd data byte
    UInt8  lo;      //!< 1st data byte
}  Msg_DB2_t;

//! used to access values which are encoded in 3 CAN data bytes
typedef struct {
    UInt8  db_2;    //!< 3rd data byte
    UInt8  db_1;    //!< 2nd data byte
    UInt8  db_0;    //!< 1st data byte
}  Msg_DB3_t;

//! used to access values which are encoded in 4 CAN data bytes
typedef struct {
    UInt8  db_3;    //!< 4th data byte
    UInt8  db_2;    //!< 3rd data byte
    UInt8  db_1;    //!< 2nd data byte
    UInt8  db_0;    //!< 1st data byte
}  Msg_DB4_t;


//-------------------------------------------------------------------------------
// type declarations and data structures for CAN data bytes
//-------------------------------------------------------------------------------

//! success/failed status id's
#ifndef TESSY
    typedef enum {
        SM_ACK,                     //!< success or valid
        SM_NACK                     //!< failed or invalid
    } __attribute__((packed)) SM_AckState_t;
#else
    typedef enum {
        SM_ACK,                     //!< success or valid
        SM_NACK                     //!< failed or invalid
    } SM_AckState_enum_t;
    typedef UInt8 SM_AckState_t;
#endif


//! CAN data bytes for Enable Request msg
typedef struct {
    UInt8  enable               : 1;  //!< set to enable module
    UInt8  dbg_skipRefRun       : 1;  //!< set to skip reference run when module is enabled
    UInt8  dbg_sendMovementData : 1;  //!< unused
    UInt8  reserved             : 5;  //!< unused data bits
}  Msg_EnableData_t;


//! CAN data bytes for Enable Response msg
typedef struct {
    SM_AckState_t   ack;        //!< status for configuration request
}  Msg_EnableAckData_t;


//! CAN data bytes for Reference Run Request msg
typedef struct {
    UInt8           profile;    //!< index of motion profile to use for reference run
}  Msg_RefRunData_t;


//! CAN data bytes for Reference Run Response msg
typedef struct {
    SM_AckState_t   ack;        //!< status for reference run request
}  Msg_RefRunReqAckData_t;


//! CAN data bytes for Reference Run Finished msg
typedef struct {
    SM_AckState_t   ack;        //!< status for finished reference run
    UInt8           posCode;    //!< limit switch position code
    Msg_DB4_t       pos;        //!< actual position (half-step)
}  Msg_RefRunAckData_t;


//! CAN data bytes for Target Position Request msg
typedef struct {
    Msg_DB4_t       pos;        //!< target position (half-step)
    UInt8           profile;    //!< index of motion profile to use for movement
}  Msg_TargetPositionData_t;


//! CAN data bytes for Target Position Response msg
typedef struct {
    SM_AckState_t   ack;        //!< status for target position request
}  Msg_TargetPosAckData_t;


//! CAN data bytes for Target Speed Request msg
typedef struct {
    Msg_DB2_t       speed;      //!< target speed (half-step/sec)
    UInt8           profile;    //!< index of motion profile to use for movement
}  Msg_TargetSpeedData_t;


//! CAN data bytes for Target Position Response msg
typedef struct {
    SM_AckState_t   ack;        //!< status for target speed request
}  Msg_TargetSpeedAckData_t;


//! CAN data bytes for Movement Finished msg
typedef struct {
    Msg_DB4_t       pos;        //!< actual position (half-step)
    Msg_DB2_t       speed;      //!< actual speed (half-step/sec)
    SM_AckState_t   ack;        //!< status for finished movement
}  Msg_MovementAckData_t;


//! CAN data bytes for Position Data msg
typedef struct {
    Msg_DB4_t       pos;        //!< actual position (half-step)
    UInt8           posCode;    //!< limit switch position code
    SM_AckState_t   ack;        //!< status for requested data
}  Msg_PositionData_t;


//! CAN data bytes for Sped Data msg
typedef struct {
    Msg_DB2_t       speed;      //!< actual speed (half-step/sec)
    SM_AckState_t   ack;        //!< status for requested data
}  Msg_SpeedData_t;


//! CAN data bytes for Operation Time msg
typedef struct {
    Msg_DB4_t       hours;      //!< actual motor operation time (hours)
    SM_AckState_t   ack;        //!< status for requested data
}  Msg_OperationTimeData_t;


//! CAN data bytes for Revolution Count msg
typedef struct {
    Msg_DB4_t       count;      //!< actual motor revolution count
    SM_AckState_t   ack;        //!< status for requested data
}  Msg_RevolutionsData_t;


//! CAN data bytes for Direction Change Count msg
typedef struct {
    Msg_DB4_t       count;      //!< actual motor direction change count
    SM_AckState_t   ack;        //!< status for requested data
}  Msg_DirChangeData_t;


//-------------------------------------------------------------------------------
// type declarations and data structures for CAN configuration data bytes
//-------------------------------------------------------------------------------

//! CAN data bytes for Configuration Response msg
typedef struct {
    SM_AckState_t   ack;        //!< status for configuration request
}  Msg_ConfigAckData_t;

//! rotation direction id's
#ifndef TESSY
    typedef enum {
        SMOT_ROTDIR_CW      = 0,    //!< clockwise
        SMOT_ROTDIR_CCW     = 1     //!< counterclockwise
    } __attribute__((packed)) StepperMotorRotDir_t;
#else
    typedef enum {
        SMOT_ROTDIR_CW      = 0,    //!< clockwise
        SMOT_ROTDIR_CCW     = 1     //!< counterclockwise
    } StepperMotorRotDir_enum_t;
    typedef UInt8 StepperMotorRotDir_t;
#endif


/*
//! rotation type of the mechanical part of the axis: linear or rotatorically
    typedef enum
    {
        ROTATION_TYPE_LINEAR    = 0, ///< linear
        ROTATION_TYPE_ROT       = 1  ///< rotatorically
    } __attribute__((packed)) StepperMotorRotType_t;
*/

//! stepper motor driver id's
#ifndef TESSY
    typedef enum {
        SMOT_DRIVER_DEFAULT = 0,    //!< default driver, default configuration from the HAL configuration file is used
        SMOT_DRIVER_TMC26X  = 1     //!< trinamic TMC26x driver
    } __attribute__((packed)) StepperMotorDriverType_t;
#else
    typedef enum {
        SMOT_DRIVER_DEFAULT = 0,    //!< default driver, default configuration from the HAL configuration file is used
        SMOT_DRIVER_TMC26X  = 1     //!< trinamic TMC26x driver
    } StepperMotorDriverType_enum_t;
    typedef UInt8 StepperMotorDriverType_t;
#endif


//! id's of standard parameter parts
#ifndef TESSY
    typedef enum
    {
        LS1     = 1,
        LS2     = 2,
        POS1    = 3,
        POS2    = 4,
        POS3    = 5,
        ENC     = 6,
        MOT1    = 7,
        MOT2    = 8,
        MOT3    = 9,

        REFRUN1 = 10,
        REFRUN2 = 11,
        REFRUN3 = 12,

        TMC26x_DRVCONF  = 0x20,
        TMC26x_SGCSCONF = 0x21,
        TMC26x_SMARTEN  = 0x22,
        TMC26x_CHOPCONF = 0x23
    } __attribute__((packed)) ParamSubIndex_t;
#else
    typedef enum
    {
        LS1     = 1,
        LS2     = 2,
        POS1    = 3,
        POS2    = 4,
        POS3    = 5,
        ENC     = 6,
        MOT1    = 7,
        MOT2    = 8,
        MOT3    = 9,

        REFRUN1 = 10,
        REFRUN2 = 11,
        REFRUN3 = 12,

        TMC26x_DRVCONF  = 0x20,
        TMC26x_SGCSCONF = 0x21,
        TMC26x_SMARTEN  = 0x22,
        TMC26x_CHOPCONF = 0x23
    } ParamSubIndex_enum_t;
    typedef UInt8 ParamSubIndex_t;
#endif


//! id's of profile parameter parts
#ifndef TESSY
    typedef enum {
        P1      = 1,                //!< profile parameters part 1
        P2      = 2,                //!< profile parameters part 2
        P3      = 3                 //!< profile parameters part 3
    } __attribute__((packed)) ProfileSubIndex_t;
#else
    typedef enum {
        P1      = 1,                //!< profile parameters part 1
        P2      = 2,                //!< profile parameters part 2
        P3      = 3                 //!< profile parameters part 3
    } ProfileSubIndex_enum_t;
    typedef UInt8 ProfileSubIndex_t;
#endif


//! configuration index
typedef union {
    struct {
        ParamSubIndex_t         index       : 7;    //!< index of standard parameter
        UInt8                   reserved    : 1;    //!< place holder
    }  param;                //!< index of standard parameter
    struct {
        ProfileSubIndex_t       index       : 4;    //! index of profile parameter part
        UInt8                   no          : 3;    //! index of profile parameter set
        UInt8                   reserved    : 1;    //!< place holder
    }  profile;              //!< index of profile parameter
    struct {
        UInt8                   reserved    : 7;    //!< place holder
        UInt8                   profileData : 1;    //!< true, if remaining bits are used as profile index
    }  type;                 //!< index type
}  SubIndex_t;


//! limit switch configuration flags
typedef struct {
    UInt8                       exist       : 1;    //!< limit switch does physically exist
    UInt8                       polarity    : 1;    //!< Logic level in 'not active' state
    UInt8                       reserved    : 6;    //!< unused bits
}  ConfigData_LS_Flag_t;


//! limit switch configuration data
typedef struct {
    ConfigData_LS_Flag_t        flag;
    UInt8                       sampleRate;         //!< digital input's sample rate (ms) 
    UInt8                       debounceCount;      //!< number of samples for debounce
}  ConfigData_LS_t;


//! position code configuration flags
typedef struct {
    UInt8                       valid       : 1;    //!< Limit switch position code is plausible
    UInt8                       stop        : 1;    //!< Motor should stop
    StepperMotorRotDir_t        stopDir     : 1;    //!< stop at position code if movement runs in this direction
    UInt8                       rotDirCheck : 1;    //!< Enable/Disable rotation direction check
    UInt8                       hitSkip     : 1;    //!< If set to 1, the first hit of reference run is skipped
    UInt8                       reserved    : 3;
}  ConfigData_LSPOS_Flag_t;


//! position code configuration data
typedef struct {
    ConfigData_LSPOS_Flag_t     flag;               //!< position code flags
    Msg_DB4_t                   position;           //!< centered limit switch position in half-steps
    UInt8                       width;              //!< limit switch width in half-steps
    UInt8                       deviation;          //!< tolerated deviation (+/-) in half-steps
}  ConfigData_LSPOS_t;


//! encoder configuration flags
typedef struct {
    UInt8                       exist       : 1;    //!< encoder does physically exist
    StepperMotorRotDir_t        rotDir      : 1;
    UInt8                       reserved    : 6;
}  ConfigData_ENC_Flag_t;


//! encoder configuration data
typedef struct {
    ConfigData_ENC_Flag_t       flag;               //!< encoder flags
    Msg_DB2_t                   resolution;         //!< encoder resolution
    Msg_DB2_t                   stepLoss_WarnLimit; //!< warning limit for step loss counter
    Msg_DB2_t                   stepLoss_ErrLimit;  //!< error limit for step loss counter
}  ConfigData_ENC_t;


//! TMC26X motor driver configuration data
typedef struct {
    Msg_DB3_t                   regVal;             //!< register value used to initialize trinamic TMC26x stepper motor driver
}  ConfigData_DRV_TMC26X_t;


//! motor configuration flags
typedef struct {
    StepperMotorRotDir_t        rotDir          : 1;    //!< the motor's rotation dir (causes an increasing step counter)
    UInt8                       reserved        : 3;    //!< unused bits
    UInt8                       motionProf_cnt  : 4;    //!< amount of motion profiles
}  ConfigData_MOT_Flag_t;


//! motor configuration data (part 1)
typedef struct {
    Msg_DB2_t                   resolution;         //!< motor resolution (full steps per turn)
    UInt8                       run_CurrentScale;   //!< run current scale, used when motor is moving (1..32)
    UInt8                       stop_CurrentScale;  //!< stop current scale, used when motor is standing still (1..32)
    Msg_DB2_t                   stopCurrent_Delay;  //!< stop current scale is applied after this delay time (in ms )
    StepperMotorDriverType_t    driverType;         //!< used stepper motor driver type
}  ConfigData_MOT_P1_t;


//! motor configuration data (part 2)
typedef struct {
    ConfigData_MOT_Flag_t       flag;               //!< motor flags
    Msg_DB4_t                   minPos;             //!< minimum motor position in half-steps
    Msg_DB2_t                   resetPos;           //!< position value at which the actual position is reset to 0 (needed for rotatory movement). Ignored if reset value is 0.
}  ConfigData_MOT_P2_t;


//! motor configuration data (part 3)
typedef struct {
    Msg_DB4_t                   maxPos;             //!< maximum motor position in half-steps
}  ConfigData_MOT_P3_t;


//! reference run configuration data (part 1)
typedef struct {
    UInt8                       refRun_RefPos;      //!< the limit switch position code used as reference position
    Msg_DB4_t                   refRun_PosOffset;   //!< position value used for initializing the reference framework
    Msg_DB2_t                   refRun_Timeout;     //!< maximum duration to perform each movement in ms
}  ConfigData_REFRUN_P1_t;


//! reference run configuration data (part 2)
typedef struct {
    Msg_DB4_t                   refRun_MaxDist;     //!< maximum number of half-steps during reference run
    Msg_DB2_t                   refRun_HighSpeed;   //!< high speed during reference run in half-steps/sec
}  ConfigData_REFRUN_P2_t;


//! reference run configuration data (part 3)
typedef struct {
    Msg_DB4_t                   refRun_ReverseDist; //!< distance for reverse move between high and low speed cycle
    Msg_DB2_t                   refRun_LowSpeed;    //!< low speed during reference run in half-steps/sec
}  ConfigData_REFRUN_P3_t;


//! profile configuration data (part 1)
typedef struct {
    UInt8                       microsteps;         //!<  microsteps, 1-2-4-8-16-32-64 are allowed
    Msg_DB2_t                   minSpeed;           //!<  minimal speed (used as start- and stop speed)
    Msg_DB2_t                   targetSpeed;        //!<  target speed in half-steps/sec 
}  ProfileData_P1_t;


//! profile configuration data (part 2)
typedef struct {
    Msg_DB2_t                   acceleration;       //!<  acceleration ramp, half-steps/s?
    Msg_DB2_t                   accJerkTime;        //!<  acceleration jerk phase time in ms
}  ProfileData_P2_t;


//! profile configuration data (part 3)
typedef struct {
    Msg_DB2_t                   deceleration;       //!<  deceleration ramp, half-steps/sec?
    Msg_DB2_t                   decJerkTime;        //!<  deceleration jerk phase time in ms
}  ProfileData_P3_t;


//! configuration data
typedef struct {
        SubIndex_t  index;
    union {
        ConfigData_LS_t         ls1;                //!< limit switch configuration data
        ConfigData_LS_t         ls2;                //!< limit switch configuration data
        ConfigData_LSPOS_t      pos1;               //!< position code configuration data 1
        ConfigData_LSPOS_t      pos2;               //!< position code configuration data 2
        ConfigData_LSPOS_t      pos3;               //!< position code configuration data 3
        ConfigData_ENC_t        enc;                //!< encoder configuration data
        ConfigData_MOT_P1_t     mot1;               //!< motor configuration data 1
        ConfigData_MOT_P2_t     mot2;               //!< motor configuration data 2
        ConfigData_MOT_P3_t     mot3;               //!< motor configuration data 3
        ConfigData_REFRUN_P1_t  refRun1;            //!< encoder configuration data 1
        ConfigData_REFRUN_P2_t  refRun2;            //!< encoder configuration data 2
        ConfigData_REFRUN_P3_t  refRun3;            //!< encoder configuration data 3

        ConfigData_DRV_TMC26X_t drvTMC26x;          //!< TMC26X motor driver configuration data

        ProfileData_P1_t        prof1;              //!< profile configuration data 1
        ProfileData_P2_t        prof2;              //!< profile configuration data 2
        ProfileData_P3_t        prof3;              //!< profile configuration data 3
    }  part;                 //!< configuration data part
}  ConfigData_Param_t;

#pragma pack(pop)

#endif /*DEFSTEPPERMOTOR_H_*/
