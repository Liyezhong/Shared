/****************************************************************************/
/*! \file fmPressurePid.c
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

#include "Global.h"
#include "bmTime.h"
#include "fmPressurePid.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

static const Int16 pressPidHysteresis = 40;          //!< Hystersis factor for relay switches in auto-tuning 

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Private Variables 
//****************************************************************************/

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

/*****************************************************************************/
/*! 
 *  \brief Resets the input and output parameters of the PID controller
 *
 *      This method sets all input and output parameters of the PID controller
 *      to null. This is needed when the controller operation is stopped and
 *      started again after a while.
 * 
 *  \xparam Param = Structure containg all parameters for the PID
 *
 ****************************************************************************/

void pressPidReset (PressPidParams_t *Param)
{
    Param->Error = 0;
    Param->Integral = 0;
    Param->Output = 0;
}


/*****************************************************************************/
/*! 
 *  \brief Checks PID parameters
 *
 *      This method checks if the parameters of the PID controller have been
 *      set and if they are in a valid range.
 * 
 *  \iparam Param = Structure containg all parameters for the PID
 * 
 *  \return True or false
 *
 ****************************************************************************/

Bool pressPidParamsOk (const PressPidParams_t *Param)
{
    if (/*Param->Ti == 0 || */Param->Ts == 0) {
        return FALSE;
    }
    return Param->Ready;
}


/*****************************************************************************/
/*! 
 *  \brief Returns the output value
 *
 *      This method returns the output value of the digital PID controller.
 * 
 *  \iparam Param = Structure containg all parameters for the PID
 * 
 *  \return Output value
 *
 ****************************************************************************/

Int32 pressPidGetOutput (const PressPidParams_t *Param)
{
    return (Param->Output);
}


/*****************************************************************************/
/*! 
 *  \brief Implements a PID controller
 *
 *      This method implements a PID controller. It receives the difference
 *      between the actual and the desired pressure and the parameters 
 *      needed by the PID controller. These values can be computed by hand or
 *      they can be calculated by the auto tuning algorithm.
 * 
 *  \xparam Param = Structure containg all parameters for the PID
 *  \iparam DesiredTemp = Desired pressure
 *  \iparam ActualTemp = Measured pressure
 *
 *  \return Returns if the PID controller is configured correctly
 *
 ****************************************************************************/
 
Bool pressPidCalculate (PressPidParams_t *Param, Int32 DesiredPress, Int32 ActualPress, UInt8 Direction)
{
    Int32 Error;
    Int32 Proportional;
    Int32 Integral;
    Int32 Derivative;

    if (pressPidParamsOk (Param) == FALSE) {
        return FALSE;
    }

	// Direction: 0-Pressure, 1-Vacuum
    if (Direction == 0) {
		// Compute error
	    if (DesiredPress > Param->MaxPress) {
	        Error = Param->MaxPress - ActualPress;
	    }
	    else {
	        Error = DesiredPress - ActualPress;
	    }
	}
	else {
		// Compute error
        if (DesiredPress < Param->MinPress) {
		    Error = ActualPress - Param->MinPress;
		}
	    else {
	        Error = ActualPress - DesiredPress;
	    }
	}


    // Compute proportional part
    Proportional = (Param->Kc * Error) / 100;
    
    // Compute integral part
	if (Param->Ti != 0) {
	    Integral = ((Param->Kc * Error * Param->Ts) / Param->Ti) / 100;
	    // Anti windup
	    if ( (Param->Output > 0 && Param->Output < Param->Range) ) {
	        Param->Integral += Integral;
	    }
	}
	else {
	    Param->Integral = 0;
	}    

    // Compute derivative part
    Derivative = ((Param->Kc * (Error - Param->Error) * Param->Td) / Param->Ts) / 100;
    
    // Compute the output value
    Param->Output = Proportional + Param->Integral + Derivative;
    
    // Overflow control
    if (Param->Output > Param->Range) {
        Param->Output = Param->Range;
    }
    else if (Param->Output < 0) {
        Param->Output = 0;
    }
    
    Param->Error = Error;
    
    //printf("P:%5d %5d %5d %5d, %5d\n", Proportional, Param->Integral, Error, Param->Output, ActualPress);
    
    return TRUE;
}


/*****************************************************************************/
/*! 
 *  \brief Starts the PID auto tuning process
 *
 *      This method starts the auto tuning algorithm of the PID controller. It
 *      checks, if the process can be started and initializes the internal
 *      data structures required for process control.
 * 
 *  \xparam Param = Structure containg all parameters for the PID
 *
 ****************************************************************************/
 
void pressPidAutoStart (PressPidParams_t *Param)
{
    Param->AutoTune.RelayState = FALSE;
    Param->AutoTune.Cycles = 0;
    Param->AutoTune.Period = 0;
    Param->AutoTune.Amplitude = 0;
    return;
}


/*****************************************************************************/
/*! 
 *  \brief Controls the auto tuning process
 *
 *      This method is instantly called by the task function. It controls the
 *      auto tuning process and computes and returns the actuating value for
 *      the heating elements in respect to the current pressure. It
 *      furthermore stores the measured values.
 * 
 *  \xparam Param = Structure containg all parameters for the PID
 *  \iparam DesiredPress = Desired pressure
 *  \iparam ActualPress = Measured pressure
 *
 ****************************************************************************/

void pressPidAutoProgress (PressPidParams_t *Param, Int32 DesiredPress, Int32 ActualPress)
{
#if 0
    UInt16 Number;
    Bool RelaySwitch = FALSE;
    PressAutoTune_t *AutoTune = &Param->AutoTune; 
    
    // Detecting minima and maxima
    if (ActualPress > AutoTune->PeriodMax) {
        AutoTune->PeriodMax = ActualPress;
    }
    if (ActualPress < AutoTune->PeriodMin) {
        AutoTune->PeriodMin = ActualPress;
    }
    
    // Switching the "relay"
    if (ActualPress > (DesiredPress + pressPidHysteresis / 2) && AutoTune->RelayState == TRUE) {
        Param->Output = 0;
        Param->AutoTune.RelayState = FALSE;
        RelaySwitch = TRUE;
    }
    else if (ActualPress < (DesiredPress - pressPidHysteresis / 2) && AutoTune->RelayState == FALSE) {
        // Toggling at around 12.5% of the total output range
        Param->Output = Param->Range >> 3;
        Param->AutoTune.RelayState = TRUE;
        RelaySwitch = TRUE;
    }
    
    // Compute period and amplitude mean value
    if (RelaySwitch == TRUE) {
        if ((AutoTune->Cycles % 2) == 1) {
            if (AutoTune->Cycles != 1) {
                Number = AutoTune->Cycles / 2;
                AutoTune->Period = ((Number - 1) * AutoTune->Period + bmTimeExpired(AutoTune->PeriodStart)) / Number;
                AutoTune->Amplitude = ((Number - 1) * AutoTune->Amplitude + AutoTune->PeriodMax - AutoTune->PeriodMin) / Number;
            }
            AutoTune->PeriodStart = bmGetTime();
            AutoTune->PeriodMax = 0;
            AutoTune->PeriodMin = MAX_UINT16;
        }
        AutoTune->Cycles++;
    }
 #endif
    
    return;
}

/*****************************************************************************/
/*! 
 *  \brief Stops the auto tuning process
 *
 *      This method stops the auto tuning algorithm and evaluates the values
 *      measured during the process. It returns the control parameters for the
 *      PID regulator.
 * 
 *  \xparam Param = Structure containg all parameters for the PID
 *
 ****************************************************************************/

void pressPidAutoStop (PressPidParams_t *Param)
{
    UInt32 Kcu;
    
    Param->AutoTune.Amplitude -= pressPidHysteresis;
    Kcu = ((4000UL * (Param->Range >> 3) * 100) / 3142) / Param->AutoTune.Amplitude;
    
    // Using Ziegler-Nichols algorithm
    Param->Kc = (3 * Kcu) / 5;
    Param->Ti = Param->AutoTune.Period / 20;
    Param->Td = Param->AutoTune.Period / 80;
    
    return;
}

//****************************************************************************/
