/****************************************************************************/
/*! \file
 * 
 *  \brief  Configuration functions of the function module 'stepper motor'
 *
 *  $Rev:    $ 0.1
 *  $Date:   $ 11.06.2012
 *  $Author: $ Rainer Boehles
 *
 *  \b Description:
 *
 *      This file contains functions to apply configuration data received from
 *      the master to the related instances.
 *      It does also perform the final check of configuration completeness and 
 *      correctness before the stepper module is enabled.
 *
 *      Because it is not possible to transfer all stepper configuration data in
 *      one CAN msg, the data is split up into multiple messages and reassembled
 *      here.
 *      Same is done for motion profile data. Because amount of profiles is not
 *      fixed, the memory to store configuration data for profiles is allocated
 *      from the heap and have to be cleaned up properly, when number of profiles
 *      is changing.
 *      Also the order in which configuration data chunks are received is not
 *      fixed the profile configuration data should be send after other standard
 *      parameters to avoid reseting profile data by a mofified amount of profiles.
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

#include <stdlib.h> 

#include "Global.h"

#include "fmStepperMotor.h"
#include "fmStepperMotorConfig.h"



/******************************************************************************/
/*! 
 *  \brief  Set motor driver configuration data
 *
 *      Configuration data received from master is applied to the appropriate
 *      motor driver register set.
 * 
 *  \iparam  Motor  = pointer to motor data
 *  \iparam  RegVal = register value  received from master
 *  \iparam  Index  = index number into driver register set
 *  \iparam  Dlc    = amount of CAN data bytes
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smConfigTMC26x(smMotor_t *Motor, UInt32 RegVal, UInt8 Index, UInt8 Dlc)
{
    // stepper motor driver driver configuration
    if (Dlc != sizeof(ConfigData_DRV_TMC26X_t) + sizeof(SubIndex_t)) {
        return E_MISSING_PARAMETERS;
    }

    return smConfigureTMC26x(Motor, RegVal, Index); 
}


/******************************************************************************/
/*! 
 *  \brief  Set position code configuration data
 *
 *      Configuration data received from master is applied to the appropriate
 *      position code instance.
 * 
 *  \iparam  LimitSwitches  = pointer to limit switches data
 *  \iparam  Index          = index number of the position code
 *  \iparam  Param          = pointer to configuration data received from master
 *  \iparam  Dlc            = amount of CAN data bytes
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smConfigurePosCodeParam(smLimitSwitches_t* LimitSwitches, UInt8 Index, ConfigData_LSPOS_t* Param, UInt8 Dlc)
{
    if (Dlc != sizeof(ConfigData_LSPOS_t) + sizeof(SubIndex_t)) {
        return E_MISSING_PARAMETERS;
    }

    return smConfigurePosCode(LimitSwitches, Index, Param);
}


/******************************************************************************/
/*! 
 *  \brief  Set standard configuration data
 *
 *      Configuration data received from master is applied to the appropriate
 *      instance.
 * 
 *  \xparam  Data  = stepper instance data pointer
 *  \iparam  Param = pointer to configuration data received from master
 *  \iparam  Dlc   = amount of CAN data bytes
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smConfigureParam(smData_t *Data, ConfigData_Param_t *Param, UInt8 Dlc)
{
    Error_t RetCode = NO_ERROR;

    switch (Param->index.param.index)
    {
    case LS:
        // configuration data of limit switch 1 and 2
        if (Dlc != sizeof(ConfigData_LS_t) + sizeof(SubIndex_t)) {
            return E_MISSING_PARAMETERS;
        }
        RetCode = smConfigureLimitSwitches(&Data->LimitSwitches, &Param->part.ls);
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
        if (Dlc != sizeof(ConfigData_ENC_t) + sizeof(SubIndex_t)) {
            return E_MISSING_PARAMETERS;
        }
        RetCode = smConfigureEncoder(&Data->Encoder, &Param->part.enc);
        break;
    case MOT1:
        // motor configuration data, part 1
        if (Dlc != sizeof(ConfigData_MOT_P1_t) + sizeof(SubIndex_t)) {
            return E_MISSING_PARAMETERS;
        }
        RetCode = smConfigureMotor(&Data->Motor, &Param->part.mot1);
        break;
    case MOT2:
        // motor configuration data, part 2
        if (Dlc != sizeof(ConfigData_MOT_P2_t) + sizeof(SubIndex_t)) {
            return E_MISSING_PARAMETERS;
        }

        if ((RetCode = smConfigureFramework1(&Data->Motor, &Param->part.mot2)) < 0) {
            return RetCode;
        }

        // check if amount of motion profiles have changed
        if (Data->Profiles.Count != Param->part.mot2.flag.motionProf_cnt)
        {
            // check if new amount of motion profiles is valid
            UInt8 Count = Param->part.mot2.flag.motionProf_cnt;
            if((Count < 1) || (Count > MAX_MOTION_PROFIL)) {
                return E_PARAMETER_OUT_OF_RANGE;
            }
    
            Data->Profiles.Count = Count;

            // deallocate old motion profile memory
            if (Data->Profiles.Set) {
                free (Data->Profiles.Set);
            }
    
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
        if (Dlc != sizeof(ConfigData_MOT_P3_t) + sizeof(SubIndex_t)) {
            return E_MISSING_PARAMETERS;
        }

        RetCode = smConfigureFramework2(&Data->Motor, &Param->part.mot3);
        break;
    case REFRUN1:
        // reference run configuration data, part 1
        if (Dlc != sizeof(ConfigData_REFRUN_P1_t) + sizeof(SubIndex_t)) {
            return E_MISSING_PARAMETERS;
        }

        RetCode = smConfigureReferenceRun1(&Data->RefRun, &Param->part.refRun1);
        break;
    case REFRUN2:
        // reference run configuration data, part 1
        if (Dlc != sizeof(ConfigData_REFRUN_P2_t) + sizeof(SubIndex_t)) {
            return E_MISSING_PARAMETERS;
        }

        RetCode = smConfigureReferenceRun2(&Data->RefRun, &Param->part.refRun2);
        break;
    case REFRUN3:
        // reference run configuration data, part 1
        if (Dlc != sizeof(ConfigData_REFRUN_P3_t) + sizeof(SubIndex_t)) {
            return E_MISSING_PARAMETERS;
        }

        RetCode = smConfigureReferenceRun3(&Data->RefRun, &Param->part.refRun3);
        break;
    case TMC26x_DRVCONF:
        {
            UInt32 RegVal;
            DB3_TO_VAL(Param->part.drvTMC26x.regVal, RegVal);
            if ((RetCode = smConfigTMC26x(&Data->Motor, RegVal, 0, Dlc)) < 0) {
                return RetCode;
            }
        }
        break;
    case TMC26x_SGCSCONF:
        {
            UInt32 RegVal;
            DB3_TO_VAL(Param->part.drvTMC26x.regVal, RegVal);
            if ((RetCode = smConfigTMC26x(&Data->Motor, RegVal, 1, Dlc)) < 0) {
                return RetCode;
            }
        }
        break;
    case TMC26x_SMARTEN:
        {
            UInt32 RegVal;
            DB3_TO_VAL(Param->part.drvTMC26x.regVal, RegVal);
            if ((RetCode = smConfigTMC26x(&Data->Motor, RegVal, 2, Dlc)) < 0) {
                return RetCode;
            }
        }
        break;
    case TMC26x_CHOPCONF:
        {
            UInt32 RegVal;
            DB3_TO_VAL(Param->part.drvTMC26x.regVal, RegVal);
            if ((RetCode = smConfigTMC26x(&Data->Motor, RegVal, 3, Dlc)) < 0) {
                return RetCode;
            }
        }
        break;

    default:
        return E_PARAMETER_OUT_OF_RANGE;
    }
    
    return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief  Set profile configuration data
 *
 *      Profile data received from master is applied to the appropriate profile.
 * 
 *  \xparam  Profiles = pointer to profiles data
 *  \iparam  Index    = index number of the profile
 *  \iparam  Param    = pointer to configuration data received from master
 *  \iparam  Dlc      = amount of CAN data bytes
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smConfigureProfile(smProfiles_t *Profiles, UInt8 Index, ConfigData_Param_t *Param, UInt8 Dlc)
{
    Error_t RetCode = NO_ERROR;

    switch (Param->index.profile.index)
    {
    case P1:
        // profile configuration data, part 1
        if (Dlc != sizeof(ProfileData_P1_t) + sizeof(SubIndex_t)) {
            return E_MISSING_PARAMETERS;
        }
        RetCode = smConfigureProfile1(Profiles, Index, &Param->part.prof1);
        break;
    case P2:
        // profile configuration data, part 2
        if (Dlc != sizeof(ProfileData_P2_t) + sizeof(SubIndex_t)) {
            return E_MISSING_PARAMETERS;
        }
        RetCode = smConfigureProfile2(Profiles, Index, &Param->part.prof2);
        break;
    case P3:
        // profile configuration data, part 3
        if (Dlc != sizeof(ProfileData_P3_t) + sizeof(SubIndex_t)) {
            return E_MISSING_PARAMETERS;
        }

        RetCode = smConfigureProfile3(Profiles, Index, &Param->part.prof3);
        break;

    default:
        return E_PARAMETER_OUT_OF_RANGE;
    }

    return RetCode;
}


/******************************************************************************/
/*! 
 *  \brief  Validate whole configuration
 *
 *      Check if all configuration data have been received and is plausible.
 * 
 *  \xparam  Data = stepper instance data pointer
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smConfigIsValid(smData_t* Data)
{
    Error_t RetCode = NO_ERROR;

// check if parameters are complete
    if ((RetCode = smCheckLimitSwitchesConfig(&Data->LimitSwitches)) < 0) {
        return RetCode;
    }

    if (!smEncoderConfigIsComplete(&Data->Encoder)) {
        return E_SMOT_CONFIG_INCOMPLETE;
    }

    if (!smMotorConfigIsComplete(&Data->Motor)) {
        return E_SMOT_CONFIG_INCOMPLETE;
    }

    if ((RetCode = smCheckReferenceRunConfig(&Data->RefRun, &Data->LimitSwitches, &Data->Motor)) < 0) {
        return RetCode;
    }

// check if profiles are complete and valid
    if ((RetCode = smCheckProfilesConfig(&Data->Profiles)) < 0) {
        return RetCode;
    }

    return NO_ERROR;
}
