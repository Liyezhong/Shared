/****************************************************************************/
/*! \file
 *
 *  \brief Function module for stepper motor
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

#ifndef FMSTEPPERMOTOR_H_
#define FMSTEPPERMOTOR_H_

#ifndef SIMULATION
#include "Basemodule.h"
#endif 

#include "fmStepperMotorDevice.h"
#include "fmStepperMotorLimitSwitch.h"
#include "fmStepperMotorEncoder.h"
#include "fmStepperMotorMotionProfile.h"
#include "fmStepperMotorReferenceRun.h"
#include "fmStepperMotorMotion.h"
#include "fmStepperMotorMemory.h"


//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/
//! convert 16 bit value to 2 CAN data bytes
#define  VAL_TO_DB2(val, data)  (data.hi = (val >> 8) & 0xff);  (data.lo = (val & 0xff))
//! convert 2 CAN data bytes to 16 bit value
#define  DB2_TO_VAL(data, val)  (val = (data.hi << 8) + data.lo)

//! convert 3 CAN data bytes to 32 bit value
#define  DB3_TO_VAL(data, val)  (val = (data.db_2 << 16) + (data.db_1 << 8) + data.db_0)

//! convert 32 bit value to 4 CAN data bytes
#define  VAL_TO_DB4(val, data)  (data.db_3 = ((val >> 24) & 0xff));  (data.db_2 = ((val >> 16) & 0xff));  (data.db_1 = ((val >> 8) & 0xff));  (data.db_0 = (val & 0xff))
//! convert 4 CAN data bytes to 32 bit value
#define  DB4_TO_VAL(data, val)  (val = (data.db_3 << 24) + (data.db_2 << 16) + (data.db_1 << 8) + data.db_0)


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! stepper module state id's
typedef enum { SM_STATE_INIT,          //!< initialisation done, ref run needed next
               SM_STATE_REFRUN,        //!< execution reference run
               SM_STATE_IDLE,          //!< idle, motor is standing still
               SM_STATE_POSITION,      //!< movement to target position is executed
               SM_STATE_SPEED          //!< movement to reach target speed is executed
} smState_t;


//! stepper module flags
typedef struct
{
    UInt8   Enable          : 1;        //!< enable flag, if true stepper module is ready to execut motion commands
    UInt8   Stopped         : 1;        //!< module stopped by emergency stop
    UInt8   Shutdown        : 1;        //!< module should/is shutdown
    UInt8   dbg_skipRefRun  : 1;        //!< skip reference run flag, if true reference run is not forced after module is enabled
} smControlFlags_t;


//! Contains all variables for a instance of stepper motor module
typedef struct
{
    UInt16              Instance;           //!< Instance number of this module
    UInt16              Channel;            //!< Logical CAN channel
    smControlFlags_t    Flags;              //!< mode control flags

// device data
    smMemory_t          Memory;             //!< non-volatile storage  
    smMotor_t           Motor;              //!< motor data   
    smLimitSwitches_t   LimitSwitches;      //!< limit switch data   
    smEncoder_t         Encoder;            //!< encoder data

// motion profiles
    smProfiles_t        Profiles;           //!< motion profiles data

// reference run control 
    smReferenceRun_t    RefRun;             //!< reference run data

// motion control 
    Motion_t            Motion;             //!< motion data

// module control
    smState_t           State;              //!< stepper module state

    Bool                OffLimit;           //!< last known off-limit state
} smData_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t smCloseDevices(smData_t *Data);
Error_t smOpenDevices(smData_t *Data);
Error_t smEnable(smData_t *Data, Bool SkipRefRun);
Error_t smInitializeModule(UInt16 ModuleID, UInt16 NumberOfInstances);


//****************************************************************************/
// Public variables
//****************************************************************************/               

extern smData_t *smDataTable;       //!< Data table for all instances
extern Handle_t  smHandleTimer;     //!< Timer handle, one timer for up to four motors


#endif /*FMSTEPPERMOTOR_H_*/
