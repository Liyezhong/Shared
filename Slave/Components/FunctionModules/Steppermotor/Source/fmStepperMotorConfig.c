/****************************************************************************/
/*! \file fmStepperMotorConfig.c
 * 
 *  \brief Functions determining the configuration of function module 'stepper motor'
 *
 *  $Version: $ 0.1
 *  $Date:    $ 17.10.2010
 *  $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This file contains the functions need by the configuration 
 *       of the 'stepper motor' function module.     
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

#include <stdlib.h> 

#include "Global.h"

#include "fmStepperMotor.h"
#include "fmStepperMotorConfig.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/


/******************************************************************************/
/*! 
 *  \brief   Configure the stepper motor
 *
 *      Extracts the configuration options for the stepper motor from 
 *      the received CAN message and sets the module instance data 
 *      structure accordingly. The following parameters are taken from the
 *      CAN message:
 *      - DB[0]  = sub index, the information received with this can message ID is subdivided into
 *                 several CAN messages. The first byte DB[0] is the sub index of the CAN message
 *                 Data bytes 1..7 will be evaluated as described below:
 *       SubIndex == MSG_CONFIG_SUB_INDEX_LS1/2: configuration limit switch 1/2
 *          DB[1]   = [0/1] 1 - Limit switch 1/2 exists physically
 *                        0 - Limit switch 1/2 doesn't exists physically
 *          DB[2]   = [0/1] The limit switch's orientation
 *                        0 - CW motor axis rotation will move the axis closer to limit switch 1/2
 *                        1 - CCW motor axis rotation will move the axis closer to limit switch 1/2
 *          DB[3]   = The polarity of limit switch 1/2
 *          DB[4]   = Sample rate if the assigned digital input port
 *          DB[5]   = Debounce counter of the assigned digital input port
 *          DB[6,7] = reserved
 *
 *       SubIndex == MSG_CONFIG_SUB_INDEX_ENC: configuration encoder
 *          DB[1]    = [0/1] 1 - Encoder exists physically
 *                           0 - Encoder doesn't exists physically
 *          DB[2,3]  = The encoder's resolution, the number of tick of a revolution
 *          DB[4]    = The encoder's orientation
 *                        0 - CW motor axis rotation will increase the encoder ticks
 *                        1 - CW motor axis rotation will decrease the encoder ticks
 *          DB[5..7] = reserved
 * 
 *       SubIndex == MSG_CONFIG_SUB_INDEX_MOTOR1: motor configuration data block 1
 *          DB[1,2] = The motor's step resolution, the number of half-steps per revolution
 *          DB[3] = Direction of rotation
 *                        0 - CW motor axis rotation results in increasing half-step counter
 *                        1 - CCW motor axis rotation results in increasing half-step counter  
 *          DB[4..5] = Motor minimum speed in half-steps/sec (Int16)
 *          DB[6] = rotation type of the axis
 *          DB[7] = reserved
 *
 *       SubIndex == MSG_CONFIG_SUB_INDEX_MOTOR2: motor configuration data block 2
 *          DB[1..4] = Motor minimum position in half steps (Int32)
 *          DB[5,6]  = Number of steps for one revolution of rotator. axis (incl. mechanic ratio)
 *          DB[7] = reserved
 *
 *       SubIndex == MSG_CONFIG_SUB_INDEX_MOTOR3: motor configuration data block 3
 *          DB[1..4] = Motor maximum position in half steps (Int32)
 *          DB[5]    = Direction of rotation at reference run
 *                        0 - CW motor axis rotation at reference run
 *                        1 - CCW motor axis rotation at reference run
 *          DB[6,7] = Motor slow speed at reference run
 *
 *       SubIndex == MSG_CONFIG_SUB_INDEX_MOTOR4: motor configuration data block 4
 *          DB[1..4] = Maximum movement of half steps at reference run (Int32)
 *          DB[5,6]  = Motor high speed at reference run
 *                        0 - CW motor axis rotation at reference run
 *                        1 - CCW motor axis rotation at reference run
 *          DB[7] = reserved
 *
 *       SubIndex == MSG_CONFIG_SUB_INDEX_MOTOR5: motor configuration data block 5
 *          DB[1..4] = Reverese position offset at reference run (Int32)
 *
 *       SubIndex == MSG_CONFIG_SUB_INDEX_MOTOR6: motor configuration data block 6
 *          DB[1..4] = Offset reference positon - base position
 *          DB[5,6]  = Motor low speed at reference run
 *          DB[7]    = Count of movement profiles
 *
 *       SubIndex == MSG_CONFIG_SUB_INDEX_MOTOR7: motor configuration data block 7
 *          DB[1,2] = Warning limit step loss in half-steps
 *          DB[3,4] = Error limit step loss in half-steps
 *          DB[5,6] = current limit
 *          DB[7]   = motor control mode 
 * 
 *   The messages will be send from master in the order with increasing index, but as a 
 *   result of side effect, it's not certain this function will be called in the correct 
 *   sub index sequence.
 *   To ensure correct processing of CAN-messages with promiscuous order, at variable 
 *   'CompleteConfigMask' a bit will be set for each received subindex. 
 *  If all ecpected bits have been set, the receipt is complete.
 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or error code
 *
 ******************************************************************************/


Error_t smConfigTMC26x(Motor_t *Motor, UInt32 RegVal, UInt8 Index, UInt8 Dlc)
{
	// stepper motor driver driver configuration
	if (Dlc != sizeof(ConfigData_DRV_TMC26X_t) + sizeof(SubIndex_t))
		return E_MISSING_PARAMETERS;

    return smConfigureTMC26x(Motor, RegVal, Index);	
}


Error_t smConfigureLimitSwitchParam(LimitSwitches_t* LimitSwitches, UInt8 Index, ConfigData_LS_t* Param, UInt8 Dlc)
{
	if (Dlc != sizeof(ConfigData_LS_t) + sizeof(SubIndex_t))
		return E_MISSING_PARAMETERS;

	return smConfigureLimitSwitch(LimitSwitches, Index, Param);
}


Error_t smConfigurePosCodeParam(LimitSwitches_t* LimitSwitches, UInt8 Index, ConfigData_LSPOS_t* Param, UInt8 Dlc)
{
	if (Dlc != sizeof(ConfigData_LSPOS_t) + sizeof(SubIndex_t))
		return E_MISSING_PARAMETERS;

	return smConfigurePosCode(LimitSwitches, Index, Param);
}


Error_t smConfigureParam(smData_t *Data, ConfigData_Param_t *Param, UInt8 Dlc)
{
    Error_t RetCode = NO_ERROR;

	switch (Param->index.param.index)
	{
	case LS1:
	    // configuration data of limit switch 1
		RetCode = smConfigureLimitSwitchParam(&Data->LimitSwitches, 0, &Param->part.ls1, Dlc);
		break;
	case LS2:
	    // configuration data of limit switch 2
		RetCode = smConfigureLimitSwitchParam(&Data->LimitSwitches, 1, &Param->part.ls2, Dlc);
		break;
	case POS1:
	    // configuration data of limit switch position code 1
		RetCode = smConfigurePosCodeParam(&Data->LimitSwitches, 1, &Param->part.pos1, Dlc);
		break;
	case POS2:
	    // configuration data of limit switch position code 2
		RetCode = smConfigurePosCodeParam(&Data->LimitSwitches, 2, &Param->part.pos2, Dlc);
		break;
	case POS3:
	    // configuration data of limit switch position code 3
		RetCode = smConfigurePosCodeParam(&Data->LimitSwitches, 3, &Param->part.pos3, Dlc);
		break;
	case ENC:
        // configuration data of encoder
		if (Dlc != sizeof(ConfigData_ENC_t) + sizeof(SubIndex_t))
			return E_MISSING_PARAMETERS;
		RetCode = smConfigureEncoder(&Data->Encoder, &Param->part.enc);
		break;
	case MOT1:
        // motor configuration data, part 1
		if (Dlc != sizeof(ConfigData_MOT_P1_t) + sizeof(SubIndex_t))
			return E_MISSING_PARAMETERS;
		RetCode = smConfigureMotor(&Data->Motor, &Param->part.mot1);
		break;
	case MOT2:
        // motor configuration data, part 2
		if (Dlc != sizeof(ConfigData_MOT_P2_t) + sizeof(SubIndex_t))
			return E_MISSING_PARAMETERS;

		if ((RetCode = smConfigureFramework1(&Data->Motor, &Param->part.mot2)) < 0)
			return RetCode;

        // check if amount of motion profiles have changed
        if (Data->Profiles.Count != Param->part.mot2.flag.motionProf_cnt)
        {
            // check if new amount of motion profiles is valid
            UInt8 Count = Param->part.mot2.flag.motionProf_cnt;
            if((Count < 1) || (Count > MAX_MOTION_PROFIL))
                return E_PARAMETER_OUT_OF_RANGE;                
    
            Data->Profiles.Count = Count;

            // deallocate old motion profile memory
            if (Data->Profiles.Set)
                free (Data->Profiles.Set);
    
            // allocate new memory for motion profiles
            Data->Profiles.Set = calloc(Data->Profiles.Count, sizeof(smProfile_t));
            if (NULL == Data->Profiles.Set)
            {
                Data->Profiles.Count = 0;
    	        return (E_MEMORY_FULL);
            }
            else
            {
                UInt8 Idx;
                // set motion profiles as unconfigured
                for (Idx=0; Idx<MAX_MOTION_PROFIL; Idx++)
                    Data->Profiles.Set[Idx].ConfigMask = 0;     // nothing configured
            }
        }
		break;
	case MOT3:
        // motor configuration data, part 3
		if (Dlc != sizeof(ConfigData_MOT_P3_t) + sizeof(SubIndex_t))
			return E_MISSING_PARAMETERS;

		RetCode = smConfigureFramework2(&Data->Motor, &Param->part.mot3);
		break;
/*
	case MOT7:
        {
            // motor configuration data, part 7
			if (Dlc != sizeof(ConfigData_MOT_P7_t) + sizeof(SubIndex_t))
				return E_MISSING_PARAMETERS;

            DB2_TO_VAL(Param->part.mot7.stepLoss_WarnLimit, Param->StepLossWarnLimit);
            DB2_TO_VAL(Param->part.mot7.stepLoss_ErrLimit, Param->StepLossErrorLimit);

            // set the sub message bit in the mask which is used for complete configuration receipt detection
            Param->ConfigMask |= (1 << MOT7);
        }
		break;
*/
    case REFRUN1:
        // reference run configuration data, part 1
		if (Dlc != sizeof(ConfigData_REFRUN_P1_t) + sizeof(SubIndex_t))
			return E_MISSING_PARAMETERS;

		RetCode = smConfigureReferenceRun1(&Data->RefRun, &Param->part.refRun1);
		break;
    case REFRUN2:
        // reference run configuration data, part 1
		if (Dlc != sizeof(ConfigData_REFRUN_P2_t) + sizeof(SubIndex_t))
			return E_MISSING_PARAMETERS;

		RetCode = smConfigureReferenceRun2(&Data->RefRun, &Param->part.refRun2);
		break;
    case REFRUN3:
        // reference run configuration data, part 1
		if (Dlc != sizeof(ConfigData_REFRUN_P3_t) + sizeof(SubIndex_t))
			return E_MISSING_PARAMETERS;

		RetCode = smConfigureReferenceRun3(&Data->RefRun, &Param->part.refRun3);
		break;
	case TMC26x_DRVCONF:
		{
			UInt32 RegVal;
			DB3_TO_VAL(Param->part.drvTMC26x.regVal, RegVal);
    		if ((RetCode = smConfigTMC26x(&Data->Motor, RegVal, 0, Dlc)) < 0)
    			return RetCode;
		}
		break;
	case TMC26x_SGCSCONF:
		{
			UInt32 RegVal;
			DB3_TO_VAL(Param->part.drvTMC26x.regVal, RegVal);
    		if ((RetCode = smConfigTMC26x(&Data->Motor, RegVal, 1, Dlc)) < 0)
    			return RetCode;
		}
		break;
	case TMC26x_SMARTEN:
		{
			UInt32 RegVal;
			DB3_TO_VAL(Param->part.drvTMC26x.regVal, RegVal);
    		if ((RetCode = smConfigTMC26x(&Data->Motor, RegVal, 2, Dlc)) < 0)
    			return RetCode;
		}
		break;
	case TMC26x_CHOPCONF:
		{
			UInt32 RegVal;
			DB3_TO_VAL(Param->part.drvTMC26x.regVal, RegVal);
    		if ((RetCode = smConfigTMC26x(&Data->Motor, RegVal, 3, Dlc)) < 0)
    			return RetCode;
		}
		break;

	default:
		return E_PARAMETER_OUT_OF_RANGE;
	}
	
    return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief   Set the stepper motor's movement profile data
 *
 *   Extracts the drive configuration paramters for the stepper motor's movement profile from 
 *   the received CAN message and sets the module instance's data structure accordingly. 
 *   The following parameters are taken from the CAN message:
 *      - DB[0]  = profile index, up to MAX_MOTION_PROFIL movement profiles can be predefined
 *      - DB[1]  = profile sub index, the information received with this can message ID is subdivided into
 *                 several CAN messages. The second byte DB[1] is the sub index of the CAN message
 *
 *      Data bytes 2..7 will be evaluated as described below:
 *       SubIndex == MSG_DRIVE_PARAM_SUB_INDEX_1: acceleration and speed
 *          DB[2,3]  = Acceleration in 0.1 rev/sec²
 *          DB[4,5]  = Target speed in half-steps/sec
 *          DB[6,7]  = Deceleration in 0.1 rev/sec²
 *
 *       SubIndex == MSG_DRIVE_PARAM_SUB_INDEX_2: microstep mode and jerk
 *          DB[2]    = Microstepmode in µsteps/step, allowed values are (1-2-4-8-16-32-64)
 *          DB[3]    = ramp type
 *          DB[4,5]  = acceleration jerk time in ms (both are identical: T1, T3)
 *          DB[6,7]  = deceleration jerk time in ms (both are identical: T5, T7)
 *
 * The message sill be send from master in the order with increasing index, but as a result 
 * of side effect, it's not certain this function will be called in the correct sub index sequence
 ' To ensure correct processing of CAN-messages with promiscuous order, at variable 'CompleteConfigMask' 
 * a bit will be set for each received subindex. If all ecpected bits have been set, the receipt is complete.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or error code
 *
 ******************************************************************************/
Error_t smConfigureProfile(smProfiles_t *Profiles, UInt8 Index, ConfigData_Param_t *Param, UInt8 Dlc)
{
    Error_t RetCode = NO_ERROR;

	switch (Param->index.profile.index)
	{
	case P1:
        // profile configuration data, part 1
		if (Dlc != sizeof(ProfileData_P1_t) + sizeof(SubIndex_t))
			return E_MISSING_PARAMETERS;
		RetCode = smConfigureProfile1(Profiles, Index, &Param->part.prof1);
		break;
	case P2:
        // profile configuration data, part 2
		if (Dlc != sizeof(ProfileData_P2_t) + sizeof(SubIndex_t))
			return E_MISSING_PARAMETERS;
		RetCode = smConfigureProfile2(Profiles, Index, &Param->part.prof2);
		break;
	case P3:
        // profile configuration data, part 3
		if (Dlc != sizeof(ProfileData_P3_t) + sizeof(SubIndex_t))
			return E_MISSING_PARAMETERS;

		RetCode = smConfigureProfile3(Profiles, Index, &Param->part.prof3);
		break;

	default:
		return E_PARAMETER_OUT_OF_RANGE;
	}

	return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief   Check the motor configuration data plausibility, and assigns HAL elements
 *
 *            Verifies the content of the configuration data, the following checks are done:
 *            - at least one limit switch must be configured
 *            - if two limit switches are configured, the should have
 *              different orientation paramter (one at each end)
 *			  - for LINEAR type there have to be a limit switch in reference run direction  
 * 
 *  \xparam  Data = Module instance data pointer
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smConfigIsValid(smData_t* Data)
{
    Error_t RetCode = NO_ERROR;

// check if parameters are complete
    if ((RetCode = smCheckLimitSwitchesConfig(&Data->LimitSwitches)) < 0)
        return RetCode; 
    if (!smEncoderConfigIsComplete(&Data->Encoder))
        return E_SMOT_CONFIG_INCOMPLETE; 
    if (!smMotorConfigIsComplete(&Data->Motor))
        return E_SMOT_CONFIG_INCOMPLETE; 
#ifndef SEPIA_GRABBER
    if ((RetCode = smCheckReferenceRunConfig(&Data->RefRun, &Data->LimitSwitches)) < 0)
        return RetCode; 
#endif
// check if profiles are complete and valid
    if ((RetCode = smCheckProfilesConfig(&Data->Profiles)) < 0)
        return RetCode; 

	return NO_ERROR;
}															  
