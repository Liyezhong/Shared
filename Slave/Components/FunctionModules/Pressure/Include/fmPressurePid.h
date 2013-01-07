/****************************************************************************/
/*! \file fmPressurePid.h
 * 
 *  \brief Functions implementing the PID controller.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.08.2012
 *   $Author:  $ Brandon Shao
 *
 *  \b Description:
 *
 *  This file holds a PID controller that performs the pressure regulation.
 *  It includes an auto-tuning algorithm that is able to automatically find
 *  the parameters of the PID regulator.
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

#ifndef FMPRESSUREPID_H
#define FMPRESSUREPID_H

#include "Global.h"

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

/*! Contains all parameters for the auto tuning algorithm */
typedef struct {
    Bool RelayState;    //!< State of the "relay", TRUE = on, FALSE = off
    UInt16 Cycles;      //!< Number of relay cycles
    UInt16 Period;      //!< Mean time of one period of the pressure curve in milliseconds
    UInt16 Amplitude;   //!< Mean amplitude of the pressure curve in 0.01 degree Celsius steps
    UInt32 PeriodStart; //!< Starting time of the relay period in milliseconds
    UInt16 PeriodMax;   //!< Maximal amplitude of the relay period in 0.01 degree Celsius steps
    UInt16 PeriodMin;   //!< Minimal amplitude of the relay period in 0.01 degree Celsius steps
} PressAutoTune_t;

/*! Contains all parameters for the PID controller algorithm */
typedef struct {
    Bool Ready;     //!< Indicates all paramters are set
    Int32 Kc;       //!< Gain parameter
    UInt16 Ti;      //!< Reset time in hundredth of seconds
    UInt16 Td;      //!< Derivative time in hundredth of seconds
    UInt16 Ts;      //!< Sampling time in hundredth of seconds
    Int32 Error;    //!< Previous error between desired and actual pressure
    Int32 Integral; //!< Integral part accumulator
    Int32 Output;   //!< Output value of the PID controller
    Int32 MaxPress; //!< Maximal pressure in 1 Pa step
    Int32 MinPress; //!< Minimal pressure in 1 Pa step
    Int32 Range;    //!< Output range of the PID controller
    PressAutoTune_t AutoTune; //!< Structure containing auto tuning information
} PressPidParams_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

void  pressPidReset (PressPidParams_t *Param);
Int32 pressPidGetOutput (const PressPidParams_t *Param);
Bool  pressPidCalculate (PressPidParams_t *Param, Int32 DesiredPress, Int32 ActualPress, UInt8 Direction);

void pressPidAutoStart (PressPidParams_t *Param);
void pressPidAutoProgress (PressPidParams_t *Param, Int32 DesiredPress, Int32 ActualPress);
void pressPidAutoStop (PressPidParams_t *Param);

//****************************************************************************/

#endif /*FMPRESSUREPID_H*/
