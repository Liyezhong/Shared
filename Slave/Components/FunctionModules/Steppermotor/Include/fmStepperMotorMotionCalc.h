/****************************************************************************/
/*! \file fmStepperMotorMotionCalc.h
 * 
 *  \brief Calculation of movement values bases on s-curve motion profile
 * 
 *
 *  $Version: $ 0.1
 *  $Date:    $ 04.07.2012
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

#ifndef FMSTEPPERMOTORMOTIONCALC_H
#define FMSTEPPERMOTORMOTIONCALC_H

#include "fmStepperMotorMotionProfile.h"
#include "fmStepperMotorMotion.h"


//********************************************************************************/
// Public Constants and Definitions
//********************************************************************************/

//! step unit used for precalculation of phase data
#define SM_PROFILE_MICROSTEPS_PER_HALFSTEP  1

//! time unit used for precalculation of phase data
#define SM_PROFILE_TIMEBASE                 MSEC

//! amount of half-step used for phase 0 (motor start with constant speed)
#define SM_START_HSTEPS                     5 * SM_PROFILE_MICROSTEPS_PER_HALFSTEP

//! amount of half-step used for phase 8 (motor run with constant speed and then stop)
#define SM_END_HSTEPS                       5 * SM_PROFILE_MICROSTEPS_PER_HALFSTEP


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! calculated values for a single phase
typedef struct {
    Int32   s;  //!< start position relative to actual movement (in half-steps)
    Int32   v;  //!< start velocity (in half-steps/timebase)
    Int32   a;  //!< start acceleration (in half-steps/timebase²)
    Int32   t;  //!< start time relative to actual movement (in timebase)
    Int32   ds; //!< position change(in half-steps)
    Int32   dv; //!< velocity change (in half-steps/timebase)
    Int32   da; //!< acceleration change (in half-steps/timebase²)
    Int32   dt; //!< duration (in timebase)
} smPhaseData_t;

//! perform calculation of phase values for acceleration (phase 0 to phase 4)
Int32 smCalcAccPhaseData (Int32 DistanceConstVel, smProfileConfig_t *Config, smPhaseData_t *PhaseData, Bool UseJerk);

//! perform calculation of phase values for deceleration (phase 5 to phase 8)
Int32 smCalcDecPhaseData (smProfileConfig_t *Config, smPhaseData_t *PhaseData, Bool UseJerk);

//! perform calculation of all phase values (phase 0 to phase 8)
Int32 smCalcMotionProfilePhaseData (Int32 DistanceConstVel, smProfileConfig_t *Config, smPhaseData_t *PhaseData);


#endif /*FMSTEPPERMOTORMOTIONCALC_H*/
