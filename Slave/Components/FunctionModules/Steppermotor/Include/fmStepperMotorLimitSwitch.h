/****************************************************************************/
/*! \file fmStepperMotorLimitSwitch.h
 * 
 *  \brief Limit switch sampling and position encoding functions used by function
 *         module stepper motor
 *
 *  $Rev:    $ 0.1
 *  $Date:   $ 11.06.2012
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


//********************************************************************************/
// Public Constants and Definitions
//********************************************************************************/

#define SM_NUM_OF_LIMIT_SWITCHES    2           //!< amount of supported limit switches

#define POSCODE_UNDEFINED           255         //!< position code value is undefined, because no stable value is sampled yet

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! limit switch configuration
typedef struct {
    Bool                    Exists;             //!< flag to store the limit switch's physically existence
    UInt8                   Polarity;           //!< Logic level in 'not active' state
} smLimitSwitchConfig_t;

//! limit switch data
typedef struct {
    smLimitSwitchConfig_t   Config;             //!< limit switch configuration
    Handle_t                Handle;             //!< HAL handle
    UInt16                  Value;              //!< Input value, only valid if ErrCode is NO_ERROR
    Error_t                 ErrCode;            //!< NO_ERROR for a valid input value or (negative) error code
} smLimitSwitch_t;

//! position code configuration
typedef struct {
    Bool                    Valid;              //!< Limit switch position code is plausible
    Bool                    Stop;               //!< Motor should stop
    StepperMotorRotDir_t    StopDir;            //!< stop at position code if movement runs in this direction

    Int32                   Position;           //!< centered limit switch position in half-steps
    UInt8                   Width;              //!< limit switch width in half-steps
    UInt8                   Deviation;          //!< tolerated deviation (+/-) in half-steps
    Bool                    RotDirCheck;        //!< Enable/Disable rotation direction check
    UInt8                   HitSkip;            //!< If set to 1, the first hit of reference run is skipped
} smLimitSwitchPosConfig_t;

//! position code data
typedef struct {
    UInt8                   Value;              //!< stable input value of all limit switches. undefined, if ErrCode is negative
    Error_t                 ErrCode;            //!< NO_ERROR for a valid input value or (negative) error code
    Int32                   Position;           //!< motor position at time of reading limit switches
    UInt32                  Time;               //!< last time of reading limit switches
    UInt8                   TmpValue;           //!< Input value in transition
    UInt8                   DebounceCount;      //!< Debounce counter
} smPosCode_t;

//! limit switches configuration
typedef struct {
    UInt8                   SampleRate;         //!< input's sample rate (ms)
    UInt8                   Debounce;           //!< number of samples for debounce
} smLimitSwitchesConfig_t;

//! data object for all limit switches and position codes
typedef struct {
    smLimitSwitchesConfig_t     Config;                                         //!< limit switches configuration
    smLimitSwitch_t             Device[SM_NUM_OF_LIMIT_SWITCHES];               //!< data for each single limit switch
    smPosCode_t                 PosCode;                                        //!< data for position code
    smLimitSwitchPosConfig_t    PosCodeConfig[1 << SM_NUM_OF_LIMIT_SWITCHES];   //!< configuration of each possible position code
    UInt8                       ConfigMask;                                     //!< flags to track which part is already configured
} smLimitSwitches_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

#ifndef SIMULATION
void smInitLimitSwitches (smLimitSwitches_t *LimitSwitches);
Error_t smConfigureLimitSwitch(smLimitSwitches_t* LimitSwitches, UInt8 Index, ConfigData_LS_t* Param);
Error_t smConfigurePosCode(smLimitSwitches_t* LimitSwitches, UInt8 Index, ConfigData_LSPOS_t* Param);
Error_t smCheckLimitSwitchesConfig(smLimitSwitches_t *LimitSwitches);
Error_t smOpenLimitSwitches (smLimitSwitches_t *LimitSwitches, UInt8 Instance);
Error_t smCloseLimitSwitches (smLimitSwitches_t *LimitSwitches);
Error_t smSampleLimitSwitches(UInt16 Instance, Bool UseSampleRate);
Error_t smPosCodeIsValid(smLimitSwitches_t *Switches, Int8 PosCode);
void UpdatePosCodeErrStatus(smPosCode_t *PosCode, Error_t ErrCode ,UInt16 Channel);
void smCheckPosCode(UInt16 Instance);
Error_t smRotDirIsAllowed(Bool CCW, smLimitSwitches_t *Switches);
#endif

#endif /*FMSTEPPERMOTORLIMITSWITCH_H*/
