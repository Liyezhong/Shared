/****************************************************************************/
/*! \file fmStepperMotorMotionCalc.c
 * 
 *  \brief Calculation of movement values bases on s-curve motion profile
 * 
 *
 *  $Version: $ 0.1
 *  $Date:    $ 04.07.2012
 *  $Author:  $ Rainer Boehles
 *
 *  \b Description:
 *
 *      This file contains the functions to calculate the values for each phase  
 *      of a movement. A movement consists of several phases during which motor
 *      is accelerated from start speed to target speed and then decelerated from
 *      target speed to stop speed. 
 *      Calculation is done according to preconfigured values from a motion
 *      profile for S-curve trajectory.
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

#include "fmStepperMotorMotionCalc.h"

/*****************************************************************************/
/*!
 *  \brief   Calculate phase values for movement with constant speed
 *
 *      Based on the values of preceding phase the values for next phase
 *      are calculated.
 *      Values are calculated with respect to the given distance to be moved.
 *
 *      Formula for constant velocity:
 *          Pt = P0 + V0*T
 *          P0 and V0, are the starting position and velocities
 *          Pt is the position at time T
 *
 *
 *      Precondition: preset values for PhData_0 start position and speed.
 *
 *  \xparam  PhData_0  = Pointer to phase values of preceding phase
 *  \oparam  PhData_T  = Pointer to phase values of next phase
 *  \iparam  s         = distance to move
 *
 ****************************************************************************/
void smCalcConstSpeed (smPhaseData_t* PhData_0, smPhaseData_t* PhData_T, Int32 s) {

    Int64 t = SM_PROFILE_TIMEBASE;
    t = t * s / PhData_0->v;
    if (PhData_T) {
        PhData_T->s = PhData_0->s + s;
        PhData_T->v = PhData_0->v;
        PhData_T->a = 0;
        PhData_T->t = t;
    }
    PhData_0->ds = s;
    PhData_0->dv = 0;
    PhData_0->da = 0;
    PhData_0->dt = t;
}


/*****************************************************************************/
/*!
 *  \brief   Calculate phase values for movement with constant acceleration/deceleration
 *
 *      Based on the values of preceding phase the values for next phase
 *      are calculated.
 *      Values are calculated with respect to the given constant acceleration/
 *      deceleration and duration.
 *
 *      Formula for constant acceleration / deceleration:
 *          Pt = P0 + V0*T + 1/2*A*T*T
 *          Vt = V0 + A*T
 *          P0 and V0, are the starting position and velocities
 *          Pt and Vt, are the position and velocity at time T
 *          A is the profile acceleration
 *
 *
 *      Precondition: preset values for PhData_0 start position, speed and time
 *
 *  \xparam  PhData_0  = Pointer to phase values of preceding phase
 *  \oparam  PhData_T  = Pointer to phase values of next phase
 *  \iparam  a         = positive for acceleration / negative for deceleration
 *  \iparam  dt        = duration
 *
 ****************************************************************************/
void smCalcConstAccDec (smPhaseData_t* PhData_0, smPhaseData_t* PhData_T, Int32 a, Int64 dt) {

    PhData_T->s =   PhData_0->s
                  + PhData_0->v * dt / SM_PROFILE_TIMEBASE
                  + a * dt * dt / 2 / SM_PROFILE_TIMEBASE/SM_PROFILE_TIMEBASE;

    PhData_T->v =   PhData_0->v
                  + a * dt / SM_PROFILE_TIMEBASE;

    PhData_T->a = a;

    PhData_T->t =   PhData_0->t
                  + dt;

    PhData_0->ds = PhData_T->s - PhData_0->s;
    PhData_0->dv = PhData_T->v - PhData_0->v;
    PhData_0->da = PhData_T->a - PhData_0->a;
    PhData_0->dt = dt;
}


/*****************************************************************************/
/*!
 *  \brief   Calculate phase values for movement with jerk
 *
 *      Based on the values of preceding phase the values for next phase
 *      are calculated.
 *      Values are calculated with respect to the given constant jerk and
 *      duration.
 *      During jerk the acceleration / deceleration is increased or decreased
 *
 *
 *      Formula for constant jerk:
 *          Pt = P0 + V0*T + 1/2*A0*T*T + 1/6*J*T*T*T
 *          Vt = V0 + A0*T + 1/2*J*T*T
 *          At = A0 + J*T
 *          P0, V0, and A0 are the starting position, velocity, and accelerations
 *          Pt , Vt, and At are the position, velocity, and acceleration at time T
 *          J is the profile jerk (time rate of change of acceleration)
 *          Same for deceleration
 *
 *
 *      Precondition: preset values for PhData_0 start position, speed, acceleration/
 *                    deceleration and time
 *
 *  \xparam  PhData_0  = Pointer to phase values of preceding phase
 *  \oparam  PhData_T  = Pointer to phase values of next phase
 *  \iparam  j         = positive for increasing, negative for decreasing change of
 *                       acceleration/deceleration
 *  \iparam  dt        = duration
 *
 ****************************************************************************/
void smCalcJerk (smPhaseData_t* PhData_0, smPhaseData_t* PhData_T, Int64 j, Int64 dt) {

    PhData_T->s =   PhData_0->s
                  + PhData_0->v * dt / SM_PROFILE_TIMEBASE
                  + PhData_0->a * dt * dt / 2 / SM_PROFILE_TIMEBASE / SM_PROFILE_TIMEBASE
                  + j * dt / SM_PROFILE_TIMEBASE * dt / SM_PROFILE_TIMEBASE * dt / SM_PROFILE_TIMEBASE / 6;

    PhData_T->v =   PhData_0->v
                  + PhData_0->a * dt / SM_PROFILE_TIMEBASE
                  + j * dt * dt / 2 / SM_PROFILE_TIMEBASE / SM_PROFILE_TIMEBASE;

    PhData_T->a =   PhData_0->a
                  + j * dt/SM_PROFILE_TIMEBASE;

    PhData_T->t =   PhData_0->t
                  + dt;

    PhData_0->ds = PhData_T->s - PhData_0->s;
    PhData_0->dv = PhData_T->v - PhData_0->v;
    PhData_0->da = PhData_T->a - PhData_0->a;
    PhData_0->dt = dt;
}


/*****************************************************************************/
/*!
 *  \brief   Calculate start speed of jerk phase
 *
 *      For a given jerk phase with known end speed, duration, acceleration/
 *      deceleration change and jerk the start speed is calculated.
 *      This is used to validate a profile. If the calculated start speed is
 *      below the end speed of preceding phase a profile is invalid.
 *
 *      Formula for constant jerk start speed:
 *          V0 = Vt - A0*T - 1/2*J*T*T
 *          V0 and A0 are the starting velocity, and accelerations
 *          Vt and At are the velocity, and acceleration at time T
 *          J is the profile jerk (time rate of change of acceleration)
 *          Same for deceleration
 *
 *
 *  \iparam  vT         = speed at end of jerk phase
 *  \iparam  t          = duration of jerk phase
 *  \iparam  a          = positive for acceleration / negative for deceleration
 *  \iparam  j          = positive for increasing, negative for decreasing change of
 *                        acceleration/deceleration
 *
 *  \return             start speed
 *
 ****************************************************************************/
Int32 smCalcJerk_v0 (Int32 vT, Int64 t, Int32 a, Int64 j) {

    Int32 v0 =   vT
               - a * t / SM_PROFILE_TIMEBASE
               - j * t * t / 2 / SM_PROFILE_TIMEBASE / SM_PROFILE_TIMEBASE;

    return v0;
}


/*****************************************************************************/
/*!
 *  \brief   Calculate phase values for skipped phase
 *
 *      Values of preceding phase are copied to next phase.
 *      Because phase should be skipped none of values change.
 *
 *
 *      Precondition: preset values for PhData_0
 *
 *  \iparam  PhData_0  = Pointer to phase values of preceding phase
 *  \oparam  PhData_T  = Pointer to phase values of next phase
 *
 ****************************************************************************/
void smSkipPhase (smPhaseData_t* PhData_0, smPhaseData_t* PhData_T) {

    PhData_T->s = PhData_0->s;
    PhData_T->v = PhData_0->v;
    PhData_T->a = PhData_0->a;
    PhData_T->t = PhData_0->t;
    PhData_0->ds = 0;
    PhData_0->dv = 0;
    PhData_0->da = 0;
    PhData_0->dt = 0;
}


/*****************************************************************************/
/*!
 *  \brief   Calculate phase values for motor acceleration
 *
 *      Motor is accelerated from phase 0 to phase 4. Values for this phases
 *      are calculated with respect to motion profile and the distance to be
 *      moved during phase 4 (movement with constant speed). 
 *
 *      Optional the jerk phases can be skipped, which means motor is constantly
 *      accelerated from start speed to end speed. This is needed if motor speed
 *      is changed by small amount and jerk can not be performed.
 *
 *
 *      Precondition: preset values for phase 0
 *
 *  \iparam  DistanceConstVel   = distance to move during phase 4 (in half-steps)
 *  \iparam  Config             = Pointer to s-curve motion profile configuration
 *  \oparam  PhaseData          = Pointer to array for all phase values
 *  \iparam  UseJerk            = FALSE to skip jerk phases
 *
 *  \return  distance moved from start of phase 0 to end of phase 4
 *
 ****************************************************************************/
Int32 smCalcAccPhaseData (Int32 DistanceConstVel, smProfileConfig_t *Config, smPhaseData_t *PhaseData, Bool UseJerk) {

    Int64 dt_constAcc;      // duration of constant acceleration

    smCalcConstSpeed (&PhaseData[PH_0_START], &PhaseData[PH_1_ACC_JERK_UP], SM_START_HSTEPS);

    if (TRUE == UseJerk) {  // we will use jerk and constant acceleration phase
    // increasing acceleration jerk
        smCalcJerk (&PhaseData[PH_1_ACC_JERK_UP],
                    &PhaseData[PH_2_ACC_CONST],
                    (Int64)Config->acc * SM_PROFILE_TIMEBASE / Config->accJUpT,
                    Config->accJUpT);
        if (PhaseData[PH_2_ACC_CONST].v >= Config->vMax) {
            return -1;
        }

    // calculate velocity at start of decreasing acceleration jerk to know the duration
        PhaseData[PH_3_ACC_JERK_DOWN].v = smCalcJerk_v0 (Config->vMax,
                                                         Config->accJDownT,
                                                         Config->acc,
                                                         (Int64)-Config->acc * SM_PROFILE_TIMEBASE / Config->accJDownT);
        if (PhaseData[PH_3_ACC_JERK_DOWN].v < PhaseData[PH_2_ACC_CONST].v) {
            return -1;
        }
        dt_constAcc =   (Int64)(PhaseData[PH_3_ACC_JERK_DOWN].v
                      - PhaseData[PH_2_ACC_CONST].v) * SM_PROFILE_TIMEBASE / Config->acc;

    // constant acceleration
        smCalcConstAccDec (&PhaseData[PH_2_ACC_CONST], &PhaseData[PH_3_ACC_JERK_DOWN], Config->acc, dt_constAcc);
        if (PhaseData[PH_3_ACC_JERK_DOWN].v >= Config->vMax) {
            return -1;
        }
                         
        smCalcJerk (&PhaseData[PH_3_ACC_JERK_DOWN],
                    &PhaseData[PH_4_VEL_CONST],
                    (Int64)-Config->acc * SM_PROFILE_TIMEBASE/Config->accJDownT,
                    Config->accJDownT);
        if (PhaseData[PH_4_VEL_CONST].v > Config->vMax) {
            return -1;
        }
    }
    else {  // we will only use constant acceleration phase
        smSkipPhase (&PhaseData[PH_1_ACC_JERK_UP], &PhaseData[PH_2_ACC_CONST]);
        PhaseData[PH_2_ACC_CONST].a = Config->acc;
        dt_constAcc = (Int64)(Config->vMax-PhaseData[PH_2_ACC_CONST].v) * SM_PROFILE_TIMEBASE / Config->acc;
        smCalcConstAccDec (&PhaseData[PH_2_ACC_CONST], &PhaseData[PH_3_ACC_JERK_DOWN], Config->acc, dt_constAcc);
        PhaseData[PH_3_ACC_JERK_DOWN].a = 0;
        smSkipPhase (&PhaseData[PH_3_ACC_JERK_DOWN], &PhaseData[PH_4_VEL_CONST]);
    }

    smCalcConstSpeed (&PhaseData[PH_4_VEL_CONST], &PhaseData[PH_5_DEC_JERK_UP], DistanceConstVel);

    return PhaseData[PH_4_VEL_CONST].s + PhaseData[PH_4_VEL_CONST].ds;
}


/*****************************************************************************/
/*!
 *  \brief   Calculate phase values for motor deceleration
 *
 *      Motor is accelerated from phase 5 to phase 8. Values for this phases
 *      are calculated with respect to motion profile. 
 *
 *      Optional the jerk phases can be skipped, which means motor is constantly
 *      decelerated from start speed to end speed. This is needed if motor speed
 *      is changed by small amount and jerk can not be performed.
 *
 *
 *      Precondition: preset values for phase 4
 *
 *  \iparam  Config             = Pointer to s-curve motion profile configuration
 *  \oparam  PhaseData          = Pointer to array for all phase values
 *  \iparam  UseJerk            = FALSE to skip jerk phases
 *
 *  \return  distance moved from start of phase 0 to end of phase 8
 *
 ****************************************************************************/
Int32 smCalcDecPhaseData (smProfileConfig_t *Config, smPhaseData_t *PhaseData, Bool UseJerk) {

    Int64 dt_constDec;

    if (TRUE == UseJerk) {  // we will use jerk and constant deceleration phase
        smCalcJerk (&PhaseData[PH_5_DEC_JERK_UP],
                    &PhaseData[PH_6_DEC_CONST],
                    (Int64)-Config->dec*SM_PROFILE_TIMEBASE/Config->decJUpT,
                    Config->decJUpT);
        if (PhaseData[PH_6_DEC_CONST].v <= Config->vMin) {
            return -1;
        }

    // calculate velocity at start of decreasing deceleration jerk to know the duration
        PhaseData[PH_7_DEC_JERK_DOWN].v = smCalcJerk_v0 (Config->vMin,
                                                         Config->decJDownT,
                                                         -Config->dec,
                                                         (Int64)Config->dec * SM_PROFILE_TIMEBASE / Config->decJDownT);
        if (PhaseData[PH_7_DEC_JERK_DOWN].v > PhaseData[PH_6_DEC_CONST].v) {
            return -1;
        }
        dt_constDec =   (Int64)(PhaseData[PH_6_DEC_CONST].v
                      - PhaseData[PH_7_DEC_JERK_DOWN].v) * SM_PROFILE_TIMEBASE / Config->dec;

        smCalcConstAccDec (&PhaseData[PH_6_DEC_CONST], &PhaseData[PH_7_DEC_JERK_DOWN], -Config->dec, dt_constDec);
        if (PhaseData[PH_7_DEC_JERK_DOWN].v <= Config->vMin) {
            return -1;
        }

        smCalcJerk (&PhaseData[PH_7_DEC_JERK_DOWN],
                    &PhaseData[PH_8_END],
                    (Int64)Config->dec * SM_PROFILE_TIMEBASE / Config->decJDownT,
                    Config->decJDownT);
        if (PhaseData[PH_8_END].v < Config->vMin) {
            return -1;
        }
    }
    else {  // we will only use constant deceleration phase
        smSkipPhase (&PhaseData[PH_5_DEC_JERK_UP], &PhaseData[PH_6_DEC_CONST]);
        PhaseData[PH_6_DEC_CONST].a = -Config->dec;
        dt_constDec = (Int64)(PhaseData[PH_6_DEC_CONST].v - Config->vMin) * SM_PROFILE_TIMEBASE / Config->dec;
        smCalcConstAccDec (&PhaseData[PH_6_DEC_CONST], &PhaseData[PH_7_DEC_JERK_DOWN], -Config->dec, dt_constDec);
        PhaseData[PH_7_DEC_JERK_DOWN].a = 0;
        smSkipPhase (&PhaseData[PH_7_DEC_JERK_DOWN], &PhaseData[PH_8_END]);
    }

    smCalcConstSpeed (&PhaseData[PH_8_END], NULL, SM_END_HSTEPS);

    return PhaseData[PH_8_END].s + PhaseData[PH_8_END].ds;
}


/*****************************************************************************/
/*!
 *  \brief   Calculate phase values for motor movement
 *
 *      To perform a movement the motor is accelerated from phase 0 to phase 4,
 *      moved with constant speed in phase 4 and then is decelerated from
 *      phase 5 to phase 8.
 *      Values for all this phases are calculated with respect to motion profile
 *      and the distance to be moved during phase 4.
 *
 *
 *      Precondition: preset values for phase 0
 *
 *  \iparam  DistanceConstVel   = distance to move during phas 4 (in half-steps)
 *  \iparam  Config             = Pointer to s-curve motion profile configuration
 *  \oparam  PhaseData          = Pointer to array for all phase values
 *
 *  \return  distance moved from start of phase 0 to end of phase 8
 *
 ****************************************************************************/
Int32 smCalcMotionProfilePhaseData (Int32 DistanceConstVel, smProfileConfig_t *Config, smPhaseData_t *PhaseData) {

    Int32 Distance;

    PhaseData[PH_0_START].s = 0;
    PhaseData[PH_0_START].v = Config->vMin;
    PhaseData[PH_0_START].a = 0;
    PhaseData[PH_0_START].t = 0;

    Distance = smCalcAccPhaseData (DistanceConstVel, Config, PhaseData, TRUE);
    if (Distance < 0) {
        return Distance;
    }
    Distance = smCalcDecPhaseData (Config, PhaseData, TRUE);

    return Distance;
}
