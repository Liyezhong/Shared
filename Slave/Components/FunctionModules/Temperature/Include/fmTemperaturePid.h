/****************************************************************************/
/*! \file fmTemperaturePid.h
 * 
 *  \brief Functions implementing the PID controller.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *  This file holds a PID controller that performs the temperature regulation.
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

#ifndef FMTEMPERATUREPID_H
#define FMTEMPERATUREPID_H

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
    UInt16 Period;      //!< Mean time of one period of the temperature curve in milliseconds
    UInt16 Amplitude;   //!< Mean amplitude of the temperature curve in 0.01 degree Celsius steps
    UInt32 PeriodStart; //!< Starting time of the relay period in milliseconds
    UInt16 PeriodMax;   //!< Maximal amplitude of the relay period in 0.01 degree Celsius steps
    UInt16 PeriodMin;   //!< Minimal amplitude of the relay period in 0.01 degree Celsius steps
} TempAutoTune_t;

/*! Contains all parameters for the PID controller algorithm */
typedef struct {
    Bool Ready;     //!< Indicates all paramters are set
    Int32 Kc;       //!< Gain parameter
    UInt16 Ti;      //!< Reset time in hundredth of seconds
    UInt16 Td;      //!< Derivative time in hundredth of seconds
    UInt16 Ts;      //!< Sampling time in hundredth of seconds
    Int16 Error;    //!< Previous error between desired and actual temperature
    Int32 Integral; //!< Integral part accumulator
    Int32 Output;   //!< Output value of the PID controller
    UInt16 MaxTemp; //!< Maximal temperature in 0.01 degree Celsius steps
    UInt16 Range;   //!< Output range of the PID controller
    TempAutoTune_t AutoTune; //!< Structure containing auto tuning information
} TempPidParams_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

void  tempPidReset (TempPidParams_t *Param);
Int16 tempPidGetOutput (const TempPidParams_t *Param);
Bool  tempPidCalculate (TempPidParams_t *Param, Int16 DesiredTemp, UInt16 ActualTemp);

void tempPidAutoStart (TempPidParams_t *Param);
void tempPidAutoProgress (TempPidParams_t *Param, UInt16 DesiredTemp, UInt16 ActualTemp);
void tempPidAutoStop (TempPidParams_t *Param);

//****************************************************************************/

#endif /*FMTEMPERATUREPID_H*/
