/****************************************************************************/
/*! \file fmStepperMotorDevice.c
 * 
 *  \brief Functions determining the motion control calculation of function module 
 *         'stepper motor'
 *
 *  $Version: $ 0.1
 *  $Date:    $ 17.10.2010
 *  $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This file contains the functions to calculate the movements 
 *       of the 'stepper motor'.     
 *       
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 * 
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
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


//#define SM_OPERATION_TIME_UNIT 3600000       //!< operation time is counted in hours = 60*60*1000 ms
#define SM_OPERATION_TIME_UNIT 60000        //!< operation time is counted in minutes = 60*1000 ms



void smResetLifeCycleData(Motor_t *Motor, smMemory_t *Memory)
{
    Motor->LifeCycle.OperationTime.Start = 0;
    Motor->LifeCycle.OperationTime.Duration = 0;
    Motor->LifeCycle.OperationTime.Minutes = 0;

    Motor->LifeCycle.Revolutions.FullSteps = 0;
    Motor->LifeCycle.Revolutions.Count = smGetRevolution(Memory);
}


void smInitMotor (Motor_t *Motor, smMemory_t *Memory)
{
    Motor->ConfigMask = 0;  // nothing configured

    Motor->Handle = 0;
    Motor->Driver.Type = SMOT_DRIVER_DEFAULT;       // use default driver configuration from HAL configuration file

    smResetLifeCycleData (Motor, Memory);

    Motor->Driver.Data.TMC26x.ConfigMask = 0;        
}


Error_t smConfigureTMC26x(Motor_t* Motor, UInt32 RegVal, UInt8 Index)
{
    Motor->Driver.Data.TMC26x.RegValue.Mode[Index] = RegVal & 0xfffff;
    
    Motor->ConfigMask |= BIT(Index + 4);

    return NO_ERROR;
}


Error_t smConfigureMotor(Motor_t* Motor, ConfigData_MOT_P1_t* Param)
{
//            Param->RotationType           = configData->part.mot1.flag.rotType;
    DB2_TO_VAL(Param->resolution, Motor->Config.Resolution);
//            DB2_TO_VAL(configData->part.mot1.minSpeed, Param->MinSpeed);

    Motor->Config.RunCS  = Param->run_CurrentScale;
    Motor->Config.StopCS = Param->stop_CurrentScale;
    DB2_TO_VAL(Param->stopCurrent_Delay, Motor->Config.StopCSDelay);

//            DB2_TO_VAL(configData->part.mot7.currentLimit, Param->CurrentLimit);

    Motor->Driver.Type = Param->driverType;
    if ((SMOT_DRIVER_DEFAULT != Motor->Driver.Type) && (SMOT_DRIVER_TMC26X != Motor->Driver.Type))
        return E_SMOT_CONFIG_INVALID_DRIVER;

    Motor->ConfigMask |= BIT(0);

    return NO_ERROR;
}


Error_t smConfigureFramework1(Motor_t* Motor, ConfigData_MOT_P2_t* Param)
{
    Motor->FrameworkConfig.RotationDir = Param->flag.rotDir;
    Motor->InversePosCount = (SMOT_ROTDIR_CCW == Motor->FrameworkConfig.RotationDir);

    DB4_TO_VAL(Param->minPos, Motor->FrameworkConfig.MinPosition);
    DB2_TO_VAL(Param->resetPos, Motor->FrameworkConfig.ResetPosition);

    Motor->ConfigMask |= BIT(1);

    return NO_ERROR;
}


Error_t smConfigureFramework2(Motor_t* Motor, ConfigData_MOT_P3_t* Param)
{
    DB4_TO_VAL(Param->maxPos, Motor->FrameworkConfig.MaxPosition);

    Motor->ConfigMask |= BIT(2);

    return NO_ERROR;
}


Bool smMotorConfigIsComplete (Motor_t* Motor)
{
    UInt8 CompleteMask = (BIT(0) | BIT(1) | BIT(2));
    if (SMOT_DRIVER_TMC26X == Motor->Driver.Type)
        CompleteMask |= (BIT(4) | BIT(5) | BIT(6) | BIT(7));
    return (CompleteMask == Motor->ConfigMask);
}


Error_t smOpenMotor (Motor_t* Motor, UInt8 Instance)
{
    Error_t RetCode = NO_ERROR;

    // open stepper device
    switch (Motor->Driver.Type)
    {
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
    if (RetCode < 0)
        return RetCode;

    Motor->Handle = RetCode;
   
    Motor->LifeCycle.OperationTime.Start = bmGetTime();     // start the motor's operation time supervision

    return NO_ERROR;
}


Error_t smCloseMotor (Motor_t* Motor)
{
    Error_t RetCode = NO_ERROR;

    if (0 != Motor->Handle)
    {
        RetCode = halStepperClose(Motor->Handle);
        Motor->Handle = 0;
        Motor->LifeCycle.OperationTime.Duration += bmTimeExpired(Motor->LifeCycle.OperationTime.Start);  // keep expired operation time
    }

    return RetCode;
}


Error_t smCheckMotorStopCS (Motor_t *Motor)
{
    Error_t RetCode;

    if (0 == Motor->StopCSTimeout)
        return NO_ERROR; // nothing to do because timeout is already finished

    if (bmTimeExpired(Motor->StopCSTime) < Motor->StopCSTimeout)
        return NO_ERROR; // nothing to do because timeout is not yet expired

    Motor->StopCSTimeout = 0;
              
    // lock the stepper bus to apply stop current
    if (NO_ERROR == (RetCode = halStepperControl(Motor->Handle, STP_CTRL_LOCK_BUS, TRUE)))
        RetCode = halStepperCurrent(Motor->Handle, Motor->Config.StopCS);       // apply stop current
    halStepperControl(Motor->Handle, STP_CTRL_LOCK_BUS, FALSE);

    if (NO_ERROR != RetCode)
    {
        // retry next time
        Motor->StopCSTime = bmGetTime();
        Motor->StopCSTimeout = 10;
    }
    return RetCode;
}


void smCountOperationTime (Motor_t *Motor)
{
    smOperationTime_t *OperationTime = &Motor->LifeCycle.OperationTime;
    UInt32 Duration = OperationTime->Duration + bmTimeExpired(OperationTime->Start);
    
    if (Duration / SM_OPERATION_TIME_UNIT)
    {
        OperationTime->Start = bmGetTime();
        OperationTime->Minutes += (Duration / SM_OPERATION_TIME_UNIT);
        OperationTime->Duration = Duration % SM_OPERATION_TIME_UNIT;
    }
}
