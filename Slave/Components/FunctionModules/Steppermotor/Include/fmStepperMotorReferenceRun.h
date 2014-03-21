
#ifndef FMSTEPPERMOTORREFERENCERUN_H
#define FMSTEPPERMOTORREFERENCERUN_H

#ifndef SIMULATION
#include "defStepperMotor.h"
#endif


typedef enum
{
    SM_RRS_FAST_MOTION_START,       //!< start reference run fast motion to limit switch
    SM_RRS_FAST_MOTION,             //!< reference run fast motion is active
    SM_RRS_REVERSE_MOTION_START,    //!< start reference run reverse motion
    SM_RRS_REVERSE_MOTION,          //!< reference run reverse motion is active
    SM_RRS_SLOW_MOTION_START,       //!< start reference run slow motion to limit switch
    SM_RRS_SLOW_MOTION,             //!< reference run slow motion is active
    SM_RRS_FINISHED,                //!< reference run sucessfully finished
} ReferenceRunState_t;


typedef struct
{
    UInt8                   RefPos;      //!< the limit switch position code used as reference position
    Int16                   FastSpeed;   //!< high speed during reference run in half-steps/sec
    Int16                   SlowSpeed;   //!< low speed during reference run in half-steps/sec
    Int32                   MaxDistance; //!< maximum number of half-steps during reference run
    UInt32                  Timeout;     //!< maximum duration to perform each movement in ms
    Int32                   ReverseDist; //!< distance for reverse move between high and low speed cycle
    Int32                   Offset;      //!< offset between the reference position and internal position system
    //UInt8                   RefPosSkip;
} ReferenceRunConfig_t;


typedef struct
{
    ReferenceRunConfig_t    Config;
    ReferenceRunState_t     State;
    UInt32                  StartTime;      //!< start time of reference run in ms
    UInt32                  StartPos;       //!< start position of reference run in half-steps
    Int32                   MaxDistance;    //!< maximum number of half-steps during reference run
    UInt8                   Profile;        //!< motion profile used by reference run
    UInt8                   RefPosValue;    //!< limit switches value which represents reference position
    StepperMotorRotDir_t    RotationDir;    //!< the motor axis rotation dir while reference run is executed
    Bool                    AtRefPos;       //!< motor is at reference position
    UInt8                   ConfigMask;     //!< flags to track which part is already configured
} ReferenceRun_t;


#ifndef SIMULATION

void smInitReferenceRun (ReferenceRun_t *RefRun);

Error_t smConfigureReferenceRun1(ReferenceRun_t *RefRun, ConfigData_REFRUN_P1_t* Param);
Error_t smConfigureReferenceRun2(ReferenceRun_t *RefRun, ConfigData_REFRUN_P2_t* Param);
Error_t smConfigureReferenceRun3(ReferenceRun_t *RefRun, ConfigData_REFRUN_P3_t* Param);

Error_t smCheckReferenceRunConfig(ReferenceRun_t *RefRun, LimitSwitches_t *LimitSwitches, Motor_t *Motor);

Error_t smReferenceRunTask(UInt16 Instance);

#endif

#endif /*FMSTEPPERMOTORREFERENCERUN_H*/
