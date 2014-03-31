/****************************************************************************/
/*! \file fmStepperMotorDevice.c
 * 
 *  \brief  Functions to manage stepper motor device of function module 'stepper motor'
 *
 *  $Rev:    $ 0.1
 *  $Date:   $ 21.06.2012
 *  $Author: $ Rainer Boehles
 *
 *  \b Description:
 *
 *      This module contains functions to configure and access stepper motor
 *      device and the related motor driver. Reference framework configuration,
 *      which is used for motor positioning, and life cycle data for stepper
 *      motor are also part of this module.
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

#include "Global.h"
#include "fmStepperMotor.h"
#include "fmStepperMotorDevice.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define SM_OPERATION_TIME_UNIT 60000        //!< operation time is counted in minutes = 60*1000 ms



/******************************************************************************/
/*! 
 *  \brief  Initialize life cycle data
 *
 *      Life cycle data is initialized.
 *      Operation time is reset to zero.
 *      Revolution counter is initialized with last stored value.
 * 
 *  \iparam  Motor  = pointer to motor data
 *  \iparam  Memory = pointer to non volatile memory data
 * 
 ******************************************************************************/
void smInitLifeCycleData(smMotor_t *Motor, smMemory_t *Memory) {

    Motor->LifeCycle.OperationTime.Start = 0;
    Motor->LifeCycle.OperationTime.Duration = 0;
    Motor->LifeCycle.OperationTime.Minutes = 0;

    Motor->LifeCycle.Revolutions.FullSteps = 0;
    Motor->LifeCycle.Revolutions.Count = smGetRevolution(Memory);

    Motor->LifeCycle.DirChanges.Count = smGetDirChanges(Memory);
    Motor->LifeCycle.DirChanges.CCWRotDir = FALSE;
}


/******************************************************************************/
/*! 
 *  \brief  Initialize motor data
 *
 *      Motor data is initialized.
 *      All configuration data for motor, motor driver and reference framework
 *      are set to 'unconfigured', which means all configuration data have to be
 *      set first by master, before devices can be used.
 *      Also Life cycle data is initialized.
 * 
 *  \iparam  Motor  = pointer to motor data
 *  \iparam  Memory = pointer to non volatile memory data
 * 
 ******************************************************************************/
void smInitMotor (smMotor_t *Motor, smMemory_t *Memory) {

    Motor->ConfigMask = 0;  // nothing configured

    Motor->Handle = 0;
    Motor->Driver.Type = SMOT_DRIVER_DEFAULT;       // use default driver configuration from HAL configuration file

    smInitLifeCycleData (Motor, Memory);

    Motor->Driver.Data.TMC26x.ConfigMask = 0;        
}


/******************************************************************************/
/*! 
 *  \brief  Set motor driver configuration
 *
 *      Configuration data (a set of register values) received from master is
 *      applied to the TMC26x motor driver.
 * 
 *  \iparam  Motor  = pointer to motor data
 *  \iparam  RegVal = register value
 *  \iparam  Index  = index into register value set
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smConfigureTMC26x(smMotor_t* Motor, UInt32 RegVal, UInt8 Index) {

    Motor->Driver.Data.TMC26x.RegValue.Mode[Index] = RegVal & 0xfffff;
    
    Motor->ConfigMask |= BIT(Index + 4);

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Set motor configuration
 *
 *      Configuration data received from master is applied to the motor.
 * 
 *  \iparam  Motor = pointer to motor data
 *  \iparam  Param = pointer to configuration data received from master
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smConfigureMotor(smMotor_t* Motor, ConfigData_MOT_P1_t* Param) {

    DB2_TO_VAL(Param->resolution, Motor->Config.Resolution);
    Motor->Config.RunCS  = Param->run_CurrentScale;
    Motor->Config.StopCS = Param->stop_CurrentScale;
    DB2_TO_VAL(Param->stopCurrent_Delay, Motor->Config.StopCSDelay);

    Motor->Driver.Type = Param->driverType;
    if ((SMOT_DRIVER_DEFAULT != Motor->Driver.Type) && (SMOT_DRIVER_TMC26X != Motor->Driver.Type)) {
        return E_SMOT_CONFIG_INVALID_DRIVER;
    }

    Motor->ConfigMask |= BIT(0);

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Set reference framework configuration (part 1)
 *
 *      Configuration data (first part) received from master is applied to the
 *      reference framework.
 * 
 *  \iparam  Motor = pointer to motor data
 *  \iparam  Param = pointer to configuration data received from master
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smConfigureFramework1(smMotor_t* Motor, ConfigData_MOT_P2_t* Param) {

    Motor->FrameworkConfig.RotationDir = Param->flag.rotDir;
    Motor->InversePosCount = (SMOT_ROTDIR_CCW == Motor->FrameworkConfig.RotationDir);

    DB4_TO_VAL(Param->minPos, Motor->FrameworkConfig.MinPosition);
    DB2_TO_VAL(Param->resetPos, Motor->FrameworkConfig.ResetPosition);

    Motor->ConfigMask |= BIT(1);

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Set reference framework configuration (part 2)
 *
 *      Configuration data (second part) received from master is applied to the
 *      reference framework.
 * 
 *  \iparam  Motor = pointer to motor data
 *  \iparam  Param = pointer to configuration data received from master
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smConfigureFramework2(smMotor_t* Motor, ConfigData_MOT_P3_t* Param) {

    DB4_TO_VAL(Param->maxPos, Motor->FrameworkConfig.MaxPosition);

    Motor->ConfigMask |= BIT(2);

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Check completeness of motor configuration
 *
 *      Check if all necessary configuration data for motor have been received
 * 
 *  \iparam  Motor = pointer to motor data
 * 
 *  \return  true, if configuration is complete
 *
 ******************************************************************************/
Bool smMotorConfigIsComplete (smMotor_t* Motor) {

    UInt8 CompleteMask = (BIT(0) | BIT(1) | BIT(2));
    if (SMOT_DRIVER_TMC26X == Motor->Driver.Type) {
        CompleteMask |= (BIT(4) | BIT(5) | BIT(6) | BIT(7));
    }
    return (CompleteMask == Motor->ConfigMask);
}


/******************************************************************************/
/*! 
 *  \brief  Open motor device
 *
 *      Motor device is opened. Either with default driver values, or with
 *      the configured driver specific values.
 *      If motor is successfully opened the operation time starts to get counted.

 * 
 *  \iparam  Motor    = pointer to motor data
 *  \iparam  Instance = Module instance number
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smOpenMotor (smMotor_t* Motor, UInt8 Instance) {

    Error_t RetCode = NO_ERROR;

    // open stepper device
    switch (Motor->Driver.Type) {
    case SMOT_DRIVER_DEFAULT:
        RetCode = halStepperOpen(HAL_STEPPER_MOTOR + Instance);
        break;
    case SMOT_DRIVER_TMC26X:
        RetCode = halStepperOpenExt(HAL_STEPPER_MOTOR + Instance, Motor->Driver.Data.TMC26x.RegValue.Mode, ELEMENTS(Motor->Driver.Data.TMC26x.RegValue.Mode));
        break;
    default:
        RetCode = E_SMOT_CONFIG_INVALID_DRIVER;
        break;
    }
    if (RetCode < 0) {
        return RetCode;
    }

    Motor->Handle = RetCode;
   
    Motor->LifeCycle.OperationTime.Start = bmGetTime();     // start the motor's operation time supervision

    return NO_ERROR;
}


/******************************************************************************/
/*! 
 *  \brief  Close motor device
 *
 *      Motor device is closed.
 *      Operation time counting stops.
 * 
 *  \iparam  Motor = pointer to motor data
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smCloseMotor (smMotor_t* Motor) {

    Error_t RetCode = NO_ERROR;

    if (0 != Motor->Handle) {
        RetCode = halStepperClose(Motor->Handle);
        Motor->Handle = 0;
        Motor->LifeCycle.OperationTime.Duration += bmTimeExpired(Motor->LifeCycle.OperationTime.Start);  // keep expired operation time
    }

    return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief  Check and apply motor's stop current
 *
 *      After motor have stopped the delay to apply stop current is started.
 *      This delay is checked here and the stop current is applied, if
 *      the delay have timed out.
 * 
 *  \iparam  Motor   = pointer to motor data
 *  \iparam  Channel = Logical CAN channel
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
void smCheckMotorStopCS (smMotor_t *Motor, UInt16 Channel) {

    Error_t ErrCode;

    if (0 == Motor->StopCSTimeout) {
        return; // nothing to do because timeout is already finished
    }

    if (bmTimeExpired(Motor->StopCSTime) < Motor->StopCSTimeout) {
        return; // nothing to do because timeout is not yet expired
    }

    Motor->StopCSTimeout = 0;
              
    // lock the stepper bus to apply stop current
    if (NO_ERROR == (ErrCode = halStepperControl(Motor->Handle, STP_CTRL_LOCK_BUS, TRUE))) {
        ErrCode = halStepperCurrent(Motor->Handle, Motor->Config.StopCS);       // apply stop current
    }
    halStepperControl(Motor->Handle, STP_CTRL_LOCK_BUS, FALSE);

    if (NO_ERROR != ErrCode) {
        bmSignalEvent(Channel, ErrCode, TRUE, 0);
        // retry next time
        Motor->StopCSTime = bmGetTime();
        Motor->StopCSTimeout = 10;
    }
}


/******************************************************************************/
/*! 
 *  \brief  Count operation time
 *
 *      Motor's total operation time is updated.
 * 
 *  \iparam  Motor = pointer to motor data
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
void smCountOperationTime (smMotor_t *Motor) {

    smOperationTime_t *OperationTime = &Motor->LifeCycle.OperationTime;
    UInt32 Duration = OperationTime->Duration + bmTimeExpired(OperationTime->Start);
    
    if (Duration / SM_OPERATION_TIME_UNIT) {
        OperationTime->Start = bmGetTime();
        OperationTime->Minutes += (Duration / SM_OPERATION_TIME_UNIT);
        OperationTime->Duration = Duration % SM_OPERATION_TIME_UNIT;
    }
}
