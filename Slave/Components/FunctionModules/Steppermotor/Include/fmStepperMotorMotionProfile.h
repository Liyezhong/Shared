/****************************************************************************/
/*! \file fmStepperMotorMotionProfile.h
 * 
 *  \brief Functions to initialize, configure and validate motion profiles
 * 
 *
 *  $Version: $ 0.1
 *  $Date:    $ 27.07.2012
 *  $Author:  $ Rainer Boehles
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

#ifndef FMSTEPPERMOTORMOTIONPROFILE_H
#define FMSTEPPERMOTORMOTIONPROFILE_H


#ifndef SIMULATION
#include "defStepperMotor.h"
#endif


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! motion profile configuration data
typedef struct {
    Int32   vMin;           //!< minimal speed
    Int32   vMax;           //!< maximal speed
    Int32   acc;            //!< acceleration
    Int32   accJUpT;        //!< duration increasing acceleration jerk
    Int32   accJDownT;      //!< duration decreasing acceleration jerk
    Int32   dec;            //!< deceleration
    Int32   decJUpT;        //!< duration increasing deceleration jerk
    Int32   decJDownT;      //!< duration decreasing deceleration jerk
    Int8    MicroSteps;     //!< microstep resolution, 1-2-4-8-16-32-64 microsteps per full-step
} smProfileConfig_t;


//! pre-calculated minimal move distance / jerk value
typedef struct {
    Int32               s;  //!< minimal move distance in hs
    Int8                j;  //!< associated jerk duration in percent
} smDistanceElement_t;

//! array of pre-calculated minimal move distance / jerk values (for 100%, 90%, 80% ... down to 10% )
typedef smDistanceElement_t smDistance_t [10];


//! all data for a single motion profile
typedef struct {

    smProfileConfig_t   Config;         //!< predefined motion profiles
    UInt8               ConfigMask;     //!< flags to track which part of motion profile is already configured
    Int8                StepWidth;      //!< step width based on microstep resolution
    smDistance_t        Distance;       //!< pre-calulation data
} smProfile_t;


//! data to maintain all motion profiles
typedef struct {
    UInt8               Count;          //!< amount of motion profiles
    smProfile_t        *Set;            //!< array of motion profile data
} smProfiles_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

#ifndef SIMULATION

//! Motion profile initialization
void smInitProfiles (smProfiles_t *Profiles);

//! Configure motion profile (part1)
Error_t smConfigureProfile1(smProfiles_t *Profiles, UInt8 Index, ProfileData_P1_t* Param);

//! Configure motion profile (part2)
Error_t smConfigureProfile2(smProfiles_t *Profiles, UInt8 Index, ProfileData_P2_t* Param);

//! Configure motion profile (part3)
Error_t smConfigureProfile3(smProfiles_t *Profiles, UInt8 Index, ProfileData_P3_t* Param);

//! Check and validate all motion profiles
Error_t smCheckProfilesConfig(smProfiles_t *Profiles);

#else

//! Evaluate motion profile and perform pre-calculation
Bool smEvalProfile (smProfile_t *Profile);

#endif


#endif /*FMSTEPPERMOTORMOTIONPROFILE_H*/
