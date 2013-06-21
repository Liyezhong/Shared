/****************************************************************************/
/*! \file fmStepperMotor.h
 * 
 *  $Rev:    $ 0.1
 *  $Date:   $ 20.10.2010
 *  $Author: $ Norbert Wiedmann
 *
 *  \brief Publics for the function module 'stepper motor'
 *
 *         
 * <dl compact><dt>Company:</dt><dd> Leica Biosystems Nussloch GmbH </dd></dl>
 *
 * (c) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 * This is unpublished proprietary source code of Leica.
 * The copyright notice does not evidence any actual or intended publication.
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
#define  VAL_TO_DB2(val, data)  (data.hi = (val >> 8) & 0xff);  (data.lo = (val & 0xff))
#define  DB2_TO_VAL(data, val)  (val = (data.hi << 8) + data.lo)

#define  DB3_TO_VAL(data, val)  (val = (data.db_2 << 16) + (data.db_1 << 8) + data.db_0)

#define  VAL_TO_DB4(val, data)  (data.db_3 = ((val >> 24) & 0xff));  (data.db_2 = ((val >> 16) & 0xff));  (data.db_1 = ((val >> 8) & 0xff));  (data.db_0 = (val & 0xff))
#define  DB4_TO_VAL(data, val)  (val = (data.db_3 << 24) + (data.db_2 << 16) + (data.db_1 << 8) + data.db_0)



/*! module main state */
typedef enum { SM_STATE_INIT,          //!< initialisation done, ref run needed next
               SM_STATE_REFRUN,        //!< execution reference run
               SM_STATE_IDLE,          //!< idle, motor is standing still
               SM_STATE_POSITION,      //!< movement to target position is executed
               SM_STATE_SPEED          //!< movement to reach target speed is executed
} SM_State_t;


typedef struct
{
    UInt8   Enable          : 1;
    UInt8   dbg_skipRefRun  : 1;
} ControlFlags_t;


/*! Contains all variables for an instance of this module */
typedef struct
{
    UInt16              Instance;           //!< Instance number of this module
    UInt16              Channel;            //!< Logical CAN channel
#ifndef SIMULATION
    bmModuleState_t     ModuleState;        //!< Module state
#endif
    ControlFlags_t      Flags;              //!< mode control flags

// device data
    smMemory_t          Memory;             //!< non-volatile storage  
    Motor_t             Motor;              //!< motor data   
    LimitSwitches_t     LimitSwitches;      //!< limit switch data   
    Encoder_t           Encoder;            //!< encoder data

// motion profiles
    smProfiles_t        Profiles;

// reference run control 
    ReferenceRun_t      RefRun;

// motion control 
    Motion_t            Motion;

// module control
    SM_State_t          State;              //!< stepper module state

    smMotionState_t     LastMotionState;    //!< last known motors motion state
    Bool                OffLimit;           //!< last known off-limit state
} smData_t;


//****************************************************************************/
// Module Function Prototypes
//****************************************************************************/
//!< motor life cycle data request
#ifndef SIMULATION
Error_t smCanMsgReqLifeCycleData (UInt16 Channel, CanMessage_t* Message);
#endif

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
