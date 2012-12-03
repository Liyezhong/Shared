/****************************************************************************/
/*! \file
 * 
 *  \brief Functions to initialize, configure and validate motion profiles
 * 
 *
 *  $Version: $ 0.1
 *  $Date:    $ 27.07.2012
 *  $Author:  $ Rainer Boehles
 *
 *  \b Description:
 *
 *       This file contains the functions to initialize, configure  
 *       and validate a set of motion profiles for the 'stepper motor'
 *       function module.
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

#ifndef SIMULATION
#include "fmStepperMotor.h"
#else
#define E_SMOT_CONFIG_INVALID_PROFILE -1
#endif

#include "fmStepperMotorMotionISR.h"
#include "fmStepperMotorMotionCalc.h"
#include "fmStepperMotorMotionProfile.h"


#ifndef SIMULATION

/*****************************************************************************/
/*!
 *  \brief   Motion profile initialization
 *
 *      This function is called by stepper motor init. function to 
 *      init. empty motion profiles.
 *
 *  \iparam  Profiles  = Pointer to motion profiles data
 *
 ****************************************************************************/
void smInitProfiles (smProfiles_t *Profiles)
{
    Profiles->Count = 0;
    Profiles->Set = NULL;
}


/*****************************************************************************/
/*!
 *  \brief   Configure motion profile (part1)
 *
 *      Sets up first part of motion profile as received by CAN message.
 *
 *  \iparam  Profiles = Pointer to motion profiles data
 *  \iparam  Index = index of motion profile 
 *  \iparam  Param = motion profile data received by CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/
Error_t smConfigureProfile1(smProfiles_t *Profiles, UInt8 Index, ProfileData_P1_t* Param)
{
    smProfile_t *Profile;

    if (NULL == Profiles->Set) {
        return E_PARAMETER_OUT_OF_RANGE;
    }
    
    if (Index >= Profiles->Count) {
        return E_PARAMETER_OUT_OF_RANGE;
    }

    Profile = &(Profiles->Set[Index]);

    DB2_TO_VAL (Param->minSpeed, Profile->Config.vMin);
    DB2_TO_VAL (Param->targetSpeed, Profile->Config.vMax);
    Profile->Config.MicroSteps = Param->microsteps;     // microstep resolution per full step

// check and convert micro-steps to step-width
    switch(Profile->Config.MicroSteps)                                          
    {
        case 64:
            Profile->StepWidth = 1;
            break;
        case 32:
            Profile->StepWidth = 2;
            break;
        case 16:
            Profile->StepWidth = 4;
            break;
        case 8:
            Profile->StepWidth = 8;
            break;
        case 4:
            Profile->StepWidth = 16;
            break;
        case 2:
            Profile->StepWidth = 32;
            break;
        case 1:
            Profile->StepWidth = 64;
            break;
        default:
            return E_PARAMETER_OUT_OF_RANGE;
    }

    Profile->ConfigMask |= BIT(0);

    return NO_ERROR;
}


/*****************************************************************************/
/*!
 *  \brief   Configure motion profile (part2)
 *
 *      Sets up second part of motion profile as received by CAN message.
 *
 *  \iparam  Profiles = Pointer to motion profiles data
 *  \iparam  Index = index of motion profile 
 *  \iparam  Param = motion profile data received by CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/
Error_t smConfigureProfile2(smProfiles_t *Profiles, UInt8 Index, ProfileData_P2_t* Param)
{
    smProfile_t *Profile;

    if (NULL == Profiles->Set) {
        return E_PARAMETER_OUT_OF_RANGE;
    }
    
    if (Index >= Profiles->Count) {
        return E_PARAMETER_OUT_OF_RANGE;
    }

    Profile = &(Profiles->Set[Index]);

    DB2_TO_VAL (Param->acceleration, Profile->Config.acc);
    if (Profile->Config.acc < 1) {          // acceleration have to be a positive value
        return E_PARAMETER_OUT_OF_RANGE;
    }

    DB2_TO_VAL (Param->accJerkTime, Profile->Config.accJUpT);
    DB2_TO_VAL (Param->accJerkTime, Profile->Config.accJDownT);

//          pProfile->RampType      = configData->part.prof2.rampType;
    Profile->ConfigMask |= BIT(1);

    return NO_ERROR;
}


/*****************************************************************************/
/*!
 *  \brief   Configure motion profile (part3)
 *
 *      Sets up third part of motion profile as received by CAN message.
 *
 *  \iparam  Profiles = Pointer to motion profiles data
 *  \iparam  Index = index of motion profile 
 *  \iparam  Param = motion profile data received by CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/
Error_t smConfigureProfile3(smProfiles_t *Profiles, UInt8 Index, ProfileData_P3_t* Param)
{
    smProfile_t *Profile;

    if (NULL == Profiles->Set) {
        return E_PARAMETER_OUT_OF_RANGE;
    }
    
    if (Index >= Profiles->Count) {
        return E_PARAMETER_OUT_OF_RANGE;
    }

    Profile = &(Profiles->Set[Index]);

    DB2_TO_VAL (Param->deceleration, Profile->Config.dec);
    if (Profile->Config.dec < 1) {          // deceleration have to be a positive value
        return E_PARAMETER_OUT_OF_RANGE;
    }

    DB2_TO_VAL (Param->decJerkTime, Profile->Config.decJUpT);
    DB2_TO_VAL (Param->decJerkTime, Profile->Config.decJDownT);

    Profile->ConfigMask |= BIT(2);

    return NO_ERROR;
}


/*****************************************************************************/
/*!
 *  \brief   Check if data for all motion profiles is complete
 *
 *      Each profile is configured by receivement of 3 CAN messages.
 *      This function checks if all 3 parts for all profiles have been received.
 *
 *  \iparam  Profiles = Pointer to motion profiles data
 *
 *  \return  TRUE if motion profile data is complete
 *
 *****************************************************************************/
Bool smProfilesConfigIsComplete(smProfiles_t *Profiles)
{
    UInt8 Idx;
    UInt8 CompleteMask = (BIT(0) | BIT(1) | BIT(2));

    for (Idx = 0; Idx < Profiles->Count; Idx++)
    {
        if (CompleteMask != Profiles->Set[Idx].ConfigMask) {
            return FALSE;
        }
    }

    return TRUE;
}

#endif

/*****************************************************************************/
/*!
 *  \brief   Evaluate motion profile and perform pre-calculation
 *
 *      The "minimal move distance" needed to perform a movement according to 
 *      the given motion profile values is calculated. Minimal distance means 
 *      all movement phases are performed to reach target speed (vMax) and then 
 *      slow down back to minimal speed (vMin). Only the phase when motor is 
 *      moving with constant target speed is omitted.
 *
 *      There are unuseable combinations, e.g. if target speed is rather low,  
 *      but acceleration jerk time is quite long with high acceleration values.
 *      Such a profile is invalid.
 *
 *      If a movement should only move a small distance the profile values have
 *      to be reduced. This is done during pre-calculation. Target speed is  
 *      reduced by 10% and "minimal move distance" is calculated and stored.
 *      Again profile can become unuseable because target speed gets to low.
 *      In this case additionally the jerk is also reduced by 10% and again
 *      "minimal move distance" is calculated and stored. This steps are repeated 
 *      from 100% down to 10%. If profile is still unuseable then "minimal move
 *      distance" is set to "-1".
 *      Later when stepper is requested to move a certain distance the stored
 *      "minimal move distance" can be used to select which vMax / jerk
 *      values are reasonable to perform the movement. 
 *
 *
 *  \iparam  Set = Pointer to motion profile data
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/
Bool smEvalProfile (smProfile_t *Set)
{
    Int32 vMax;
    Int8 i,j;

    smProfileConfig_t   Config;                                 // motion profile configuration used for precalculation
    smPhaseData_t       PhaseData[NUM_OF_MOTION_PHASES];        // calculated values for each phase of the profile

    vMax = Set->Config.vMax;

// calculate minimal distance needed to reach certain velocity.
// for each loop velocity is reduced by 10% (from 100% to 10%)
    for (i=10; i>0; i--)
    {
        Config = Set->Config;
        Config.vMax = vMax * i / 10;
        Set->Distance[i-1].s = smCalcMotionProfilePhaseData(0, &Config, PhaseData);
        if (Set->Distance[i-1].s > 0) {
            Set->Distance[i-1].j = 10;
        }
        else
        {
// the profile should at least be suitable if profiles max speed is used
            if (vMax == Config.vMax) {
                return FALSE;
            }
// if preset values don't work for the reduced velocity
// we try to reduce the jerk duration also, to get even shorter distances
// jerk duration is reduced by 10% (from 90% to 10%)
            for (j=9; j>0; j--)
            {
                Config.acc          = Set->Config.acc * j / 10;
                Config.accJUpT      = Set->Config.accJUpT * j / 10;
                Config.accJDownT    = Set->Config.accJDownT * j / 10;
                Config.dec          = Set->Config.dec * j / 10;
                Config.decJUpT      = Set->Config.decJUpT * j / 10;
                Config.decJDownT    = Set->Config.decJDownT * j / 10;
                Set->Distance[i-1].s = smCalcMotionProfilePhaseData(0, &Config, PhaseData);
                if (Set->Distance[i-1].s > 0)
                {
                    Set->Distance[i-1].j = j;
                    break;
                }
            }
            if (0 == j) {
                break;
            }
        }
    }
// mark speeds which can't be used because profile parameter preset
    for (j=0; j<i; j++)
        Set->Distance[j].s = -1;

    return TRUE;
}


/*****************************************************************************/
/*!
 *  \brief   Check motion profile timings
 *
 *      Perform some checks on motion profile timíng. There are hardware 
 *      limitaitions to max./min. period on which ISR is entered. If this limits
 *      are exceeded no proper movement can be achieved.
 *
 *      1) Max timing (limit for slow movement) is determined by timer:
 *      - depends on count rate and max count value (width of CCR is 16 bit)
 *
 *      2) Min timing (limit for fast movement) is determined by duration of ISR:
 *      - depends on amount of code executed inside ISR. The worst case value
 *      have to be calculated/measured and defined in ISR_WORST_CASE_DURATION
 *
 *      Also pre-calculation is performed to get specific values which can be
 *      used later when a movement command is received.
 *
 *      3) At least the original profile data should be useable at profiles
 *      target speed vMax (\sa smEvalProfile).
 *
 *
 *  \iparam  Profiles = Pointer to motion profiles data
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/
Error_t smCheckProfilesTiming(smProfiles_t *Profiles)
{
    UInt8               Idx;
    Int32               CountRate;
    Int32               MaxCount;
    smProfileConfig_t  *Config;

    for (Idx = 0; Idx < Profiles->Count; Idx++)
    {
        Config = &(Profiles->Set[Idx].Config);

#ifndef SIMULATION

// interrupt interval time at given vMinv/microstep combination should not exceed maximal timer value
        CountRate = halTimerStatus (smHandleTimer, TIM_STAT_COUNTRATE);
        if (CountRate < 0) {
            return CountRate;
        }

        MaxCount = halTimerStatus (smHandleTimer, TIM_STAT_MAXCOUNT);
        if (MaxCount < 0) {
            return MaxCount;
        }

        if (USEC / (Config->MicroSteps * Config->vMin / 2) >= (USEC * (Int64)MaxCount / CountRate)) {
            return E_SMOT_CONFIG_INVALID_PROFILE_VMIN;
        }

// interrupt interval time at given vMax/microstep combination should not be shorter than worst case interrupt processing time
        if (USEC / (Config->vMax * Config->MicroSteps / 2) < ISR_WORST_CASE_DURATION) {
            return E_SMOT_CONFIG_INVALID_PROFILE_VMAX;
        }

#endif

        Config->vMin        = Config->vMin * SM_PROFILE_MICROSTEPS_PER_HALFSTEP;    // minimal speed
        Config->vMax        = Config->vMax * SM_PROFILE_MICROSTEPS_PER_HALFSTEP;    // maximal speed
        Config->acc         = Config->acc * SM_PROFILE_MICROSTEPS_PER_HALFSTEP;     // acceleration
        Config->accJUpT     = Config->accJUpT * SM_PROFILE_TIMEBASE / 1000;         // duration of increasing acceleration jerk
        Config->accJDownT   = Config->accJDownT * SM_PROFILE_TIMEBASE / 1000;       // duration of decreasing acceleration jerk    
        Config->dec         = Config->dec * SM_PROFILE_MICROSTEPS_PER_HALFSTEP;     // deceleration
        Config->decJUpT     = Config->decJUpT * SM_PROFILE_TIMEBASE / 1000;         // duration of increasing deceleration jerk
        Config->decJDownT   = Config->decJDownT * SM_PROFILE_TIMEBASE / 1000;       // duration decreasing deceleration jerk

// check plausibility of profile data
        if (FALSE == smEvalProfile (&Profiles->Set[Idx])) {     // do precalculation for each profile
            return E_SMOT_CONFIG_INVALID_PROFILE;
        }
    }

    return NO_ERROR;
}


/*****************************************************************************/
/*!
 *  \brief   Check and validate all motion profiles
 *
 *      Perform some checks on received motion profiles:
 *
 *      - limitation: maximum amount of profiles can't exceed MAX_MOTION_PROFIL.
 *      - completeness: all data parts for all motion profiles have to be received
 *      - plausibility: useable motion profile data with reasonable timing 
 *
 *      Also pre-calculation is performed to get specific values which can be
 *      used later when a movement command is received. If moved distance is small
 *      the max. values of a profile can't be used. Instead the pre-calculated 
 *      values will be used.
 *
 *  \iparam  Profiles = Pointer to motion profiles data
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/
Error_t smCheckProfilesConfig(smProfiles_t *Profiles)
{
#ifndef SIMULATION

    if ((Profiles->Count < 1) || (Profiles->Count > MAX_MOTION_PROFIL)) {
        return E_SMOT_CONFIG_PROFILES;
    }
    if (!smProfilesConfigIsComplete(Profiles)) {
        return E_SMOT_CONFIG_INCOMPLETE;
    }

#endif

    return smCheckProfilesTiming(Profiles);
}
