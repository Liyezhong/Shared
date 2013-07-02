/****************************************************************************/
/*! \file defStepperMotor.h
 * 
 *  $Rev:    $ 0.1
 *  $Date:   $ 02.03.2012
 *  $Author: $ Rainer Boehles
 *
 *  \brief stepper module common declarations for master and slave
 *
 *         
 * <dl compact><dt>Company:</dt><dd> Leica Biosystems Nussloch GmbH </dd></dl>
 *
 * (c) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 * This is unpublished proprietary source code of Leica.
 * The copyright notice does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef DEFSTEPPERMOTOR_H_
#define DEFSTEPPERMOTOR_H_

#include "ModuleIDs.h"

typedef struct
{
    UInt8  hi;
    UInt8  lo;
} __attribute__((packed)) Msg_DB2_t;

typedef struct
{
    UInt8  db_2;
    UInt8  db_1;
    UInt8  db_0;
} __attribute__((packed)) Msg_DB3_t;

typedef struct
{
    UInt8  db_3;
    UInt8  db_2;
    UInt8  db_1;
    UInt8  db_0;
} __attribute__((packed)) Msg_DB4_t;


/*! number of predefined motion profiles */
#define MAX_MOTION_PROFIL      5        //!< max allowed count of motion profiles


//@{**************************************************************************/
//! Events for this function module (Errors, Warnings, Infos)
//****************************************************************************/

#define BUILD_SM_ERRCODE(Class,Code)   BUILD_ERRCODE(MODULE_ID_STEPPER, Class, Code)

//! A command was received while the motor's state is not able to execute it
#define E_SMOT_INVALID_STATE                BUILD_SM_ERRCODE(ERRCLASS_ERROR, 1)

 //! received configuration data is not complete
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

//! During a reference run a timeout occured
#define E_SMOT_REFRUN_TIMEOUT               BUILD_SM_ERRCODE(ERRCLASS_ERROR, 10)

//! During a reference run the max distance was exceeded
#define E_SMOT_REFRUN_MAXDISTANCE           BUILD_SM_ERRCODE(ERRCLASS_ERROR, 11)

//! During a reference run the motor stopped at an unexpected limitswitch position
#define E_SMOT_REFRUN_INVALID_LS            BUILD_SM_ERRCODE(ERRCLASS_ERROR, 12)

//! position code from limitswitches is invalid
#define E_SMOT_INVALID_POSCODE              BUILD_SM_ERRCODE(ERRCLASS_ERROR, 20)

//! position code from limitswitches does not match with actual position
#define E_SMOT_INVALID_POSCODE_POSITION     BUILD_SM_ERRCODE(ERRCLASS_ERROR, 21)

//! movement rotation direction from this position code not allowed
#define E_SMOT_INVALID_POSCODE_DIR          BUILD_SM_ERRCODE(ERRCLASS_ERROR, 22)

//! during a movement the motor stopped at an position code signal
#define E_SMOT_STOP_BY_POSCODE              BUILD_SM_ERRCODE(ERRCLASS_ERROR, 23)

//! during a movement the motor stopped because actual position get off-limit
#define E_SMOT_STOP_BY_OFFLIMIT             BUILD_SM_ERRCODE(ERRCLASS_ERROR, 24)

//! requested speed exceeds vMin/vMax of selected profile
#define E_SMOT_MOTION_PROFILE_UNFIT         BUILD_SM_ERRCODE(ERRCLASS_ERROR, 30)

//@} End of doxygen group


//@{**************************************************************************/
//! CAN message ID's and DLC's used by this function module
//****************************************************************************/

//! Set the configuration parameter
#define MSG_SMOT_CONFIG                 BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 1)  // CAN-ID: 0x1028xxx1

//! Acknowledge the configuration parameter
#define MSG_SMOT_CONFIG_ACK             BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 0)  // CAN-ID: 0x1028xxx0
#define MSG_SMOT_CONFIG_ACK_DLC         0

//! Enable / Disable the stepper
#define MSG_SMOT_SET_ENABLE             BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 1)  // CAN-ID: 0x1038xxx1
#define MSG_SMOT_SET_ENABLE_DLC         sizeof(Msg_EnableData_t)

//! Acknowledge the enable / disable stepper
#define MSG_SMOT_SET_ENABLE_ACK         BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 0)  // CAN-ID: 0x1038xxx0
#define MSG_SMOT_SET_ENABLE_ACK_DLC     0

//! Request a reference movement
#define MSG_SMOT_REQ_REF_RUN      		BUILD_CAN_ID(CMD_CLASS_FUNCTION, 9, 1)  // CAN-ID: 0x1048xxx1
#define MSG_SMOT_REQ_REF_RUN_DLC        sizeof(Msg_RefRunData_t)

//! Acknowledge the reference movement request
#define MSG_SMOT_REQ_REF_RUN_ACK  		BUILD_CAN_ID(CMD_CLASS_FUNCTION, 9, 0)  // CAN-ID: 0x1048xxx0
#define MSG_SMOT_REQ_REF_RUN_ACK_DLC    sizeof(Msg_RefRunReqAckData_t)

//! Acknowledges that execution of a reference movement is finished
#define MSG_SMOT_REFERENCE_RUN_ACK      BUILD_CAN_ID(CMD_CLASS_FUNCTION, 3, 0)  // CAN-ID: 0x1018xxx0
#define MSG_SMOT_REFERENCE_RUN_ACK_DLC  sizeof(Msg_RefRunAckData_t)

//! Sets the motors target position, start the movement
#define MSG_SMOT_TARGET_POS             BUILD_CAN_ID(CMD_CLASS_FUNCTION, 10, 1) // CAN-ID: 0x1050xxx1
#define MSG_SMOT_TARGET_POS_DLC         sizeof(Msg_TargetPositionData_t)

//! Acknowledge the receiption of a target position command (the request, but not the movement!)
#define MSG_SMOT_TARGET_POS_ACK         BUILD_CAN_ID(CMD_CLASS_FUNCTION, 10, 0) // CAN-ID: 0x1050xxx0
#define MSG_SMOT_TARGET_POS_ACK_DLC     sizeof(Msg_TargetPosAckData_t)

//! Sets the motors target speed, start the movement
#define MSG_SMOT_TARGET_SPEED           BUILD_CAN_ID(CMD_CLASS_FUNCTION, 11, 1) // CAN-ID: 0x1058xxx1
#define MSG_SMOT_TARGET_SPEED_DLC       sizeof(Msg_TargetSpeedData_t)

//! Acknowledge the receiption of a target speed command (the request, but not the movement!)
#define MSG_SMOT_TARGET_SPEED_ACK       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 11, 0)
#define MSG_SMOT_TARGET_SPEED_ACK_DLC   sizeof(Msg_TargetSpeedAckData_t)

//! Acknowledge the execution of a movement (position or speed target, not reference run)
#define MSG_SMOT_MOVEMENT_ACK           BUILD_CAN_ID(CMD_CLASS_FUNCTION, 12, 0) // CAN-ID: 0x1060xxx0
#define MSG_SMOT_MOVEMENT_ACK_DLC       sizeof(Msg_MovementAckData_t)

//! Request the actual motor position
#define MSG_SMOT_ACT_POS_REQ            BUILD_CAN_ID(CMD_CLASS_FUNCTION, 13, 1) // CAN-ID: 0x1068xxx1
#define MSG_SMOT_ACT_POS_REQ_DLC        0

//! Send the actual motor position
#define MSG_SMOT_ACT_POS                BUILD_CAN_ID(CMD_CLASS_FUNCTION, 13, 0) // CAN-ID: 0x1068xxx1
#define MSG_SMOT_ACT_POS_DLC            sizeof(Msg_PositionData_t)

//! Request the actual motor speed
#define MSG_SMOT_ACT_SPEED_REQ          BUILD_CAN_ID(CMD_CLASS_FUNCTION, 14, 1) // CAN-ID: 0x1070xxx1
#define MSG_SMOT_ACT_SPEED_REQ_DLC      0

//! Send the actual motor speed
#define MSG_SMOT_ACT_SPEED              BUILD_CAN_ID(CMD_CLASS_FUNCTION, 14, 0) // CAN-ID: 0x1070xxx0
#define MSG_SMOT_ACT_SPEED_DLC          sizeof(Msg_SpeedData_t)

//! Debug message (motor movement data)
#define MSG_SMOT_DEBUG                  BUILD_CAN_ID(CMD_CLASS_FUNCTION, 18, 0) // CAN-ID: 0x1090xxx0
#define MSG_SMOT_DEBUG_DLC              sizeof(Msg_DebugData_t)

//! Debug message
#define MSG_SMOT_DEBUG2                 BUILD_CAN_ID(CMD_CLASS_FUNCTION, 19, 0) // CAN-ID: 0x1098xxx0
#define MSG_SMOT_DEBUG2_DLC             sizeof(Msg_Debug2Data_t)

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
//@{**************************************************************************/
//! type declarations and data structures for CAN message data
//****************************************************************************/

    typedef struct
    {
        UInt8  enable      			: 1;
        UInt8  dbg_skipRefRun   	: 1;
        UInt8  dbg_sendMovementData	: 1;
        UInt8  reserved				: 5;
    } __attribute__((packed)) Msg_EnableData_t;


    typedef enum
    {
        SM_ACK,                     // success
        SM_NACK                     // failed
    } __attribute__((packed)) SM_AckState_t;


    typedef struct
    {
        UInt8           profile;    // index of motion profile to use for movement
    } __attribute__((packed)) Msg_RefRunData_t;


    typedef struct
    {
        SM_AckState_t   ack;        // status of reference run request
    } __attribute__((packed)) Msg_RefRunReqAckData_t;


    typedef struct
    {
        SM_AckState_t   ack;        // status of finished reference run
        UInt8           posCode;    // limit switch position code
        Msg_DB4_t       pos;        // actual position (makes only sense if reference run not failed. should be same as reference offset)
    } __attribute__((packed)) Msg_RefRunAckData_t;


    typedef struct
    {
        SM_AckState_t   ack;        // status of target position request
    } __attribute__((packed)) Msg_TargetPosAckData_t;


    typedef struct
    {
        Msg_DB4_t       pos;        // target position
        UInt8           profile;    // index of motion profile to use for movement
    } __attribute__((packed)) Msg_TargetPositionData_t;


    typedef struct
    {
        SM_AckState_t   ack;        // status of target speed request
    } __attribute__((packed)) Msg_TargetSpeedAckData_t;


    typedef struct
    {
        Msg_DB2_t       speed;      // target position
        UInt8           profile;    // index of motion profile to use for movement
    } __attribute__((packed)) Msg_TargetSpeedData_t;


    typedef struct
    {
        Msg_DB4_t       pos;        // actual position
        Msg_DB2_t       speed;      // actual speed
        SM_AckState_t   ack;        // status of finished movement
    } __attribute__((packed)) Msg_MovementAckData_t;


    typedef struct
    {
        Msg_DB4_t       pos;        // half-step position count
        UInt8           posCode;    // limit switch position code
        SM_AckState_t   ack;        // status of finished movement
    } __attribute__((packed)) Msg_PositionData_t;


    typedef struct
    {
        Msg_DB2_t       speed;      // actual speed (half-step/sec?
        SM_AckState_t   ack;        // status of finished movement
    } __attribute__((packed)) Msg_SpeedData_t;

//! CAN data bytes for Operation Time msg
typedef struct
{
    Msg_DB4_t       hours;      //!< actual motor operation time (hours)
    SM_AckState_t   ack;        //!< status for requested data
} __attribute__((packed)) Msg_OperationTimeData_t;


//! CAN data bytes for Revolution Count msg
typedef struct
{
    Msg_DB4_t       count;      //!< actual motor revolution count
    SM_AckState_t   ack;        //!< status for requested data
} __attribute__((packed)) Msg_RevolutionsData_t;


//! CAN data bytes for Direction Change Count msg
typedef struct
{
    Msg_DB4_t       count;      //!< actual motor dirction change count
    SM_AckState_t   ack;        //!< status for requested data
} __attribute__((packed)) Msg_DirChangeData_t;

//@{**************************************************************************/
//! type declarations used for configurtion data
//****************************************************************************/

/*! rotation direction (from back view) */
    typedef enum
    {
	    SMOT_ROTDIR_CW    = 0,  //!< clockwise
	    SMOT_ROTDIR_CCW   = 1   //!< counterclockwise
    } __attribute__((packed)) StepperMotorRotDir_t;


/*! rotation type of the mechanical part of the axis: linear or rotatorically */
    typedef enum
    {
        ROTATION_TYPE_LINEAR    = 0, ///< linear
        ROTATION_TYPE_ROT       = 1  ///< rotatorically
    } __attribute__((packed)) StepperMotorRotType_t;

/*! list of supported stepper motor drivers */
    typedef enum
    {
		SMOT_DRIVER_DEFAULT		= 0,	///< default driver, default configuration from the HAL configuration file is used
		SMOT_DRIVER_TMC26X		= 1		///< trinamic TMC26x driver
	} __attribute__((packed)) StepperMotorDriverType_t;

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

		TMC26x_DRVCONF	= 0x20,
		TMC26x_SGCSCONF	= 0x21,
		TMC26x_SMARTEN	= 0x22,
		TMC26x_CHOPCONF	= 0x23
    } __attribute__((packed)) ParamSubIndex_t;


    typedef enum
    {
        P1      = 1,
        P2      = 2,
        P3      = 3
    } __attribute__((packed)) ProfileSubIndex_t;


    typedef union
    {
        struct
        {
            ParamSubIndex_t     index       : 7;
            UInt8               reserved    : 1;
        } __attribute__((packed)) param;
        struct
        {
            ProfileSubIndex_t   index       : 4;
            UInt8               no          : 3;
            UInt8               reserved    : 1;
        } __attribute__((packed)) profile;
        struct
        {
            UInt8               reserved    : 7;
            UInt8               profileData : 1;
        } __attribute__((packed)) type;
    } __attribute__((packed)) SubIndex_t;


    typedef struct
    {
        UInt8                   exist		: 1;
        UInt8                   polarity	: 1;
        UInt8                   reserved	: 5;
    } __attribute__((packed)) ConfigData_LS_Flag_t;


    typedef struct
    {
        ConfigData_LS_Flag_t                flag;
        UInt8                               sampleRate;
        UInt8                               debounceCount;
    } __attribute__((packed)) ConfigData_LS_t;


    typedef struct
    {
        UInt8                       valid		: 1;
        UInt8                       stop		: 1;
        StepperMotorRotDir_t        stopDir		: 1;
        UInt8                       rotDirCheck : 1;
        UInt8                       hitSkip     : 1;
        UInt8                       reserved	: 3;
    } __attribute__((packed)) ConfigData_LSPOS_Flag_t;


    typedef struct
    {
        ConfigData_LSPOS_Flag_t     flag;
        Msg_DB4_t                   position;       //!< centered limit switch position in half-steps
        UInt8                       width;          //!< limit switch width in half-steps
        UInt8                       deviation;      //!< tolerated deviation (+/-) in half-steps
    } __attribute__((packed)) ConfigData_LSPOS_t;


//        //from ..\Platform\Slave\Components\FunctionModules\Steppermotor\Source\fmStepperMotorConfig.c

////        else if(SubIndex == MSG_CONFIG_SUB_INDEX_LS1_2)
////        {
////            // configuration data of limit switch 1, data block 2
////            Data->BasisParam.LimitSwitch1.Position = bmGetMessageItem(Message, CANMSG_DB1, 2);
////            Data->BasisParam.LimitSwitch1.PositionWidth = bmGetMessageItem(Message, CANMSG_DB3, 2);
////            // set the sub message bit in the mask which is used for complete configuration receipt detection
////            Data->BasisParam.ConfigMask |= (1 << MSG_CONFIG_SUB_INDEX_LS1_2);
////        }

//        typedef struct
//        {
//            quint8   index;
//            quint8   position_Hi;
//            quint8   position_Lo;
//            quint8   width_Hi;
//            quint8   width_Lo;
//            quint8   reserved1;
//            quint8   reserved2;
//            quint8   reserved3;
//        } __attribute__((packed)) ConfigData_LS_P2_t;

        //from ..\Platform\Slave\Components\FunctionModules\Steppermotor\Source\fmStepperMotorConfig.c
        //        *       SubIndex == MSG_CONFIG_SUB_INDEX_ENC: configuration encoder
        //        *          DB[1]    = [0/1] 1 - Encoder exists physically
        //        *                           0 - Encoder doesn't exists physically
        //        *          DB[2,3]  = The encoder's resolution, the number of tick of a revolution
        //        *          DB[4]    = The encoder's orientation
        //        *                        0 - CW motor axis rotation will increase the encoder ticks
        //        *                        1 - CW motor axis rotation will decrease the encoder ticks
        //        *          DB[5..7] = reserved

//        else if(SubIndex == MSG_CONFIG_SUB_INDEX_ENC)
//        {
//            // configuration data of encoder
//            Data->BasisParam.Encoder.Exists = Message->Data[CANMSG_DB1];
//            Data->BasisParam.Encoder.Resolution = bmGetMessageItem(Message, CANMSG_DB2, 2);
//            if(Message->Data[CANMSG_DB4] == 0x01){
//                Data->BasisParam.Encoder.RotationDir = SMOT_ROTDIR_CW;
//            }
//            else if(Message->Data[CANMSG_DB4] == 0x02){
//                Data->BasisParam.Encoder.RotationDir = SMOT_ROTDIR_CCW;
//            }
//            else{
//                Data->BasisParam.Encoder.RotationDir = SMOT_ROTDIR_UNDEF;
//                RetCode = E_MISSING_PARAMETERS;
//            }
//            // set the sub message bit in the mask which is used for complete configuration receipt detection
//            Data->BasisParam.ConfigMask |= (1 << MSG_CONFIG_SUB_INDEX_ENC);
//        }


        typedef struct
        {
            UInt8                   exist		: 1;
            StepperMotorRotDir_t	rotDir		: 1;
            UInt8                   reserved	: 6;
        } __attribute__((packed)) ConfigData_ENC_Flag_t;

        typedef struct
        {
            ConfigData_ENC_Flag_t   flag;
            Msg_DB2_t               resolution;
            Msg_DB2_t               stepLoss_WarnLimit;
            Msg_DB2_t               stepLoss_ErrLimit;
        } __attribute__((packed)) ConfigData_ENC_t;


        //from ..\Platform\Slave\Components\FunctionModules\Steppermotor\Source\fmStepperMotorConfig.c
//        *       SubIndex == MSG_CONFIG_SUB_INDEX_MOTOR1: motor configuration data block 1
//        *          DB[1,2] = The motor's step resolution, the number of half-steps per revolution
//        *          DB[3] = Direction of rotation
//        *                        0 - CW motor axis rotation results in increasing half-step counter
//        *                        1 - CCW motor axis rotation results in increasing half-step counter
//        *          DB[4..5] = Motor minimum speed in half-steps/sec (Int16)
//        *          DB[6] = rotation type of the axis
//        *          DB[7] = reserved

        //        else if(SubIndex == MSG_CONFIG_SUB_INDEX_MOTOR1)
//        {
//            // motor configuration data, part 1
//            Data->BasisParam.Resolution = bmGetMessageItem(Message, CANMSG_DB1, 2);

//            if(Message->Data[CANMSG_DB3] == 0x01){
//                Data->BasisParam.RotationDir = SMOT_ROTDIR_CW;
//            }
//            else if(Message->Data[CANMSG_DB3] == 0x02){
//                Data->BasisParam.RotationDir = SMOT_ROTDIR_CCW;
//            }
//            else{
//                Data->BasisParam.RotationDir = SMOT_ROTDIR_UNDEF;
//                RetCode = E_MISSING_PARAMETERS;
//            }
//            Data->BasisParam.MinSpeed = bmGetMessageItem(Message, CANMSG_DB4, 2);
//            if(Message->Data[CANMSG_DB6] == 0x02){
//                Data->BasisParam.RotationType = ROTATION_TYPE_ROT;
//            }
//            else{
//                Data->BasisParam.RotationType = ROTATION_TYPE_LINEAR;
//            }

//            // set the sub message bit in the mask which is used for complete configuration receipt detection
//            Data->BasisParam.ConfigMask |= (1 << MSG_CONFIG_SUB_INDEX_MOTOR1);
//        }

    typedef struct
    {
//            UInt8                       enable          : 1;
//            StepperMotorRotType_t       rotType         : 1;
        StepperMotorRotDir_t        rotDir          : 1;
        UInt8                       reserved	    : 3;
        UInt8                       motionProf_cnt  : 4;
    } __attribute__((packed)) ConfigData_MOT_Flag_t;


    typedef struct
    {
        Msg_DB2_t               	resolution;
        UInt8                       run_CurrentScale;
        UInt8                       stop_CurrentScale;
        Msg_DB2_t                   stopCurrent_Delay;
		StepperMotorDriverType_t	driverType;
    } __attribute__((packed)) ConfigData_MOT_P1_t;


    typedef struct
    {
        ConfigData_MOT_Flag_t   	flag;
        Msg_DB4_t                   minPos;
        Msg_DB2_t                   resetPos;           //!< position value at which the actual position is reset to 0 (needed for rotatory movement). Ignored if reset value is 0.
//            Msg_DB2_t   currentLimit;
    } __attribute__((packed)) ConfigData_MOT_P2_t;


    typedef struct
    {
        Msg_DB4_t                   maxPos;
    } __attribute__((packed)) ConfigData_MOT_P3_t;


    typedef struct
    {
        UInt8                       refRun_RefPos;      //!< the limit switch position code used as reference position
        Msg_DB4_t                   refRun_PosOffset;
        Msg_DB2_t                   refRun_Timeout;     //!< maximum duration to perform each movement in ms
    } __attribute__((packed)) ConfigData_REFRUN_P1_t;


    typedef struct
    {
/*
        UInt8                       refRun_RefPosSkip;
*/
        Msg_DB4_t                   refRun_MaxDist;
        Msg_DB2_t                   refRun_HighSpeed;
    } __attribute__((packed)) ConfigData_REFRUN_P2_t;


    typedef struct
    {
        Msg_DB4_t                   refRun_ReverseDist;
        Msg_DB2_t                   refRun_LowSpeed;
    } __attribute__((packed)) ConfigData_REFRUN_P3_t;


    typedef struct
    {
        Msg_DB3_t	regVal;	            // register value
    } __attribute__((packed)) ConfigData_DRV_TMC26X_t;


    typedef struct
    {
        UInt8       microsteps;         //!<  microsteps, 1-2-4-8-16-32-64 are allowed
        Msg_DB2_t   minSpeed;           //!<  minimal speed (used as start- and stop speed)
        Msg_DB2_t   targetSpeed;        //!<  target speed in half-steps/sec 
    } __attribute__((packed)) ProfileData_P1_t;


    typedef struct
    {
        Msg_DB2_t   acceleration;       //!<  acceleration ramp, half-steps/s?
        Msg_DB2_t   accJerkTime;        //!<  acceleration jerk phase time in ms
    } __attribute__((packed)) ProfileData_P2_t;


    typedef struct
    {
        Msg_DB2_t   deceleration;       //!<  deceleration ramp, half-steps/sec?
        Msg_DB2_t   decJerkTime;        //!<  deceleration jerk phase time in ms
//        UInt8       rampType;           
    } __attribute__((packed)) ProfileData_P3_t;



    typedef struct
    {
        SubIndex_t  index;
        union
        {
            ConfigData_LS_t     	ls1;
            ConfigData_LS_t     	ls2;
            ConfigData_LSPOS_t      pos1;
            ConfigData_LSPOS_t      pos2;
            ConfigData_LSPOS_t      pos3;
            ConfigData_ENC_t    	enc;
            ConfigData_MOT_P1_t 	mot1;
            ConfigData_MOT_P2_t 	mot2;
            ConfigData_MOT_P3_t 	mot3;
            ConfigData_REFRUN_P1_t 	refRun1;
            ConfigData_REFRUN_P2_t 	refRun2;
            ConfigData_REFRUN_P3_t 	refRun3;

			ConfigData_DRV_TMC26X_t	drvTMC26x;

            ProfileData_P1_t    	prof1;
            ProfileData_P2_t    	prof2;
            ProfileData_P3_t    	prof3;
        } __attribute__((packed)) part;
    } __attribute__((packed)) ConfigData_Param_t;


//@{**************************************************************************/
//! type declarations used for debug data (motor movement data)
//****************************************************************************/


//from ..\Platform\Slave\Components\FunctionModules\Steppermotor\Source\fmStepperMotorCAN.c
//    Error_t smCanMsgSendDebugInfo(STEPPER_MOTOR_DATA_t *Data)
//    {
//        CanMessage_t Message;
//        Int16  nDummy;
//        UInt16 uDummy;

//        static Int32 LastPosition = 0;
//        static UInt8 LastPositionUnchanged = 0;

//        if(LastPosition == Data->MovementParam.ActPos){
//            LastPositionUnchanged++;
//            if(LastPositionUnchanged > 4){
//                LastPositionUnchanged++;
//            }
//        }
//        else{
//            LastPosition = Data->MovementParam.ActPos;
//            LastPositionUnchanged = 0;
//        }

//        Message.CanID  = MSG_SMOT_DEBUG;
//        //nDummy = (Int16) Data->MovementParam.ActPosEncoder;
//        nDummy = (Int16) Data->MovementParam.ActPos;
//        bmSetMessageItem (&Message, nDummy, 0, 2);

//        //nDummy = (Int16) Data->MovementParam.ActAcc;
//        // for agitation testing purpose, temporarly send degrees
//        nDummy = smGetDegreesFromPosition(Data, Data->MovementParam.ActPos);
//        bmSetMessageItem (&Message, nDummy, 2, 2);  //Todo: really send the acceleration?

//        nDummy = (Int16) Data->MovementParam.ActSpeed;
//        bmSetMessageItem (&Message, nDummy, 4, 2);  //Todo: really send the speed?

//        //uDummy = (UInt16)(bmGetTime() - Data->MovementParam.MovementBaseStartTime);
//        uDummy = (UInt16) (Data->MovementParam.MovementActBaseTime);
//        bmSetMessageItem (&Message, uDummy, 6, 2);  //Todo: really send the speed?

//        Message.Length = MSG_SMOT_DEBUG_DLC;

//        return (canWriteMessage(Data->Channel, &Message));
//    }

typedef struct
{
    Msg_DB2_t  pos;
    Msg_DB2_t  acc;
    Msg_DB2_t  speed;
    Msg_DB2_t  baseTime;
} __attribute__((packed))  Msg_DebugData_t;


//@{**************************************************************************/
//! type declarations used for debug2 data
//****************************************************************************/


//from ..\Platform\Slave\Components\FunctionModules\Steppermotor\Source\fmStepperMotorDebug.c
//    Error_t smDebugSendMotionCalcValues(STEPPER_MOTOR_DATA_t *Data, StepperMotorCurvePreCalc_t* pCurvePreCalc)
//    {
//        Error_t RetCode;
//        CanMessage_t Message;

//        Message.CanID  = MSG_SMOT_DEBUG2;
//        Message.Length = MSG_SMOT_DEBUG_DLC;

//        Message.Data[CANMSG_DB0] = 0;
//        bmSetMessageItem(&Message, pCurvePreCalc->sSges1, 1, 4);
//        RetCode = canWriteMessage(Data->Channel, &Message);

//        Message.Data[CANMSG_DB0] = 1;

typedef struct
{
    UInt8       index;
    Msg_DB4_t   val;
    UInt8       reserved_5;
    UInt8       reserved_6;
    UInt8       reserved_7;
} __attribute__((packed))  Msg_Debug2Data_t;



#endif /*DEFSTEPPERMOTOR_H_*/
