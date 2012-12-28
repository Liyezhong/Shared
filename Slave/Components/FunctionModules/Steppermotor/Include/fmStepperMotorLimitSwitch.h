
#ifndef FMSTEPPERMOTORLIMITSWITCH_H
#define FMSTEPPERMOTORLIMITSWITCH_H


#ifndef SIMULATION
#include "defStepperMotor.h"
#else
    //typedef enum
    //{
       // SMOT_ROTDIR_CW    = 0,  //!< clockwise
       // SMOT_ROTDIR_CCW   = 1   //!< counterclockwise
    //} StepperMotorRotDir_t;
#endif


#define NUM_OF_LIMIT_SWITCHES   2           //!< amount of supported limit switches


typedef struct
{
    Bool                 Exists;            //!< flag to store the limit switch's physically existence
    UInt8                Polarity;          //!< Logic level in 'not active' state
    UInt8                SampleRate;        //!< Digital input's sample rate (ms) 
    UInt8                Debounce;          //!< number of samples for debounce
} LimitSwitchConfig_t;


typedef struct
{
    LimitSwitchConfig_t  Config;            //!< limit switch configuration
    Handle_t             Handle;            //!< HAL handle
    UInt32               Time;              //!< last time of reading limit switch
    UInt16               ActValue;          //!< Actual stable state of input
    UInt16               TmpValue;          //!< Input value in transition    
    UInt8                DebounceCount;     //!< Debounce counter
} LimitSwitch_t;


typedef struct
{
    Bool                 Valid;             //!< Limit switch position code is plausible
    Bool                 Stop;              //!< Motor's should stop
    StepperMotorRotDir_t StopDir;           //!< stop at position if movement runs in this direction

    Int32                Position;          //!< centered limit switch position in half-steps
    UInt8                Width;             //!< limit switch width in half-steps
    UInt8                Deviation;         //!< tolerated deviation (+/-) in half-steps
    Bool                 RotDirCheck;
    UInt8                HitSkip;
} LimitSwitchPosConfig_t;


typedef struct
{
    Int8                 Value;             //!< input value of all limit switches. negative, if value is undefined
    Bool                 InvalidValue;      //!< value of Limit switch position code is unplausible
    Bool                 InvalidPosition;   //!< value of Limit switch position code is unplausible at actual position
    Int8                 LastValue;         //!< last reported unplausible Limit switch position code
} PosCode_t;

typedef struct
{
    LimitSwitch_t           Device[NUM_OF_LIMIT_SWITCHES];              //!< data for each single limit switch
    PosCode_t               PosCode;                                    //!< data for position code
    LimitSwitchPosConfig_t  PosCodeConfig[1 << NUM_OF_LIMIT_SWITCHES];  //!< configuration of each possible position code
    UInt8                   ConfigMask;                                 //!< flags to track which part is already configured
} LimitSwitches_t;


#ifndef SIMULATION
void smInitLimitSwitches (LimitSwitches_t *LimitSwitches);

Error_t smConfigureLimitSwitch(LimitSwitches_t* LimitSwitches, UInt8 Index, ConfigData_LS_t* Param);

Error_t smConfigurePosCode(LimitSwitches_t* LimitSwitches, UInt8 Index, ConfigData_LSPOS_t* Param);

Error_t smCheckLimitSwitchesConfig(LimitSwitches_t *LimitSwitches);

Error_t smOpenLimitSwitches (LimitSwitches_t *LimitSwitches, UInt8 Instance);

Error_t smCloseLimitSwitches (LimitSwitches_t *LimitSwitches);



Error_t smSampleLimitSwitches(UInt16 Instance, Bool UseSampleRate);

Error_t smPosCodeIsValid(LimitSwitches_t *Switches, Int8 PosCode);

Error_t smCheckPosCode(UInt16 Instance);

Error_t smRotDirIsAllowed(Bool CCW, LimitSwitches_t *Switches);
#endif

#endif /*FMSTEPPERMOTORLIMITSWITCH_H*/
