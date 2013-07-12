/****************************************************************************/
/*! \file fmPressurePump.c
 * 
 *  \brief Functions controlling a pumping element.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.08.2012
 *   $Author:  $ Brandon Shao
 *
 *  \b Description:
 *
 *  The methods defined here control the pressure of a pumping element. The
 *  needed input parameter is computed by a PID controller. There are also
 *  functions that determine the connection between various pumping elements.
 *  Furthermore, the current usage in milliamperes of the elements is computed
 *  here.
 *
 *  \b Company:
 *
 *       Leica Biosystems Shanghai.
 * 
 *  (C) Copyright 2010 by Leica Biosystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice 
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "Global.h"
#include "bmError.h"
#include "bmMonitor.h"
#include "bmTime.h"
#include "halLayer.h"
#include "ModuleIds.h"
#include "fmPressure.h"
#include "fmPressurePump.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! Contains the data for the pump current measurements */
typedef struct {
    Handle_t HandleSwitch;      //!< Handle for the pumping element circuit switch
    Handle_t *HandleControl;    //!< Array of handles for the pumping element control outputs
#ifdef ASB15_VER_B
    Handle_t *HandlePWMControl;
#endif    
    Int16 MinValue;             //!< Minimal value of the last current half-wave
    Int16 MaxValue;             //!< Maximal value of the last current half-wave
    UInt16 EffectiveCurrent;    //!< Effective value of the pumping element current in mA
    Bool ParallelCircuit;       //!< Pumping elment circuit is switched serially or parallelly
    PressPumpParams_t Params;   //!< Parameters for the pump current measurements
    UInt16 Instances;           //!< Total number of module instances
    UInt16 MaxActive;           //!< Maximum number of active pumping elements
    UInt32 *StartingTime;       //!< Time in milliseconds an output pulse is started
    UInt32 *OperatingTime;      //!< Time in milliseconds an output pulse is active
    UInt32 SampleTime;          //!< Time the last sample was taken
    Bool Failed;                //!< The pump check failed
} PressPumpData_t;


/*! Data structure to hold monitoring variables */
typedef struct {
    UInt16 Filter:8;    //!< Number of samples to average over
    Int16 History[16];  //!< History of input values (for filtering)
    UInt16 InCount;     //!< Number of samples in history buffer
    UInt16 NextIn;      //!< Next-in pointer into history buffer
    Int16 Value;        //!< Filtered input value
    Handle_t Handle;    //!< Handle for input channel
} PressPumpMonitor_t;

//****************************************************************************/
// Private Variables 
//****************************************************************************/

/*! Global data for the pumping element control functionality */
static PressPumpData_t PressPumpData;
/*! Global data for input filtering functionality */
static PressPumpMonitor_t PressPumpMonitor;  


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static Error_t pressPumpGetFilteredInput (PressPumpMonitor_t *Monitor);


/*****************************************************************************/
/*!
 *  \brief  Initializes the handling of the pumping elements
 *
 *      This methods opens the peripheral handlers needed for the control of
 *      the pumping elements. It also allocates the memory required by this
 *      task and initializes the pumping element switching circuit and the
 *      measurement of the effective current through the elements.
 *
 *  \oparam  Params = Points to the public data of this module
 *  \iparam  CurrentChannel = HAL channel for the current measurement
 *  \iparam  SwitchChannel = HAL channel for the circuit switch
 *  \iparam  ControlChannel = HAL channel for the pump control
 *  \iparam  Instances = Total number of module instances
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

#ifdef ASB15_VER_A
Error_t pressPumpInit (PressPumpParams_t **Params, Device_t CurrentChannel, Device_t SwitchChannel, Device_t ControlChannel, UInt16 Instances)
#endif

#ifdef ASB15_VER_B
Error_t pressPumpInit (PressPumpParams_t **Params, Device_t CurrentChannel, Device_t ControlChannel, Device_t PWMControlChannel, UInt16 Instances)
#endif
{
    UInt16 i;
    //Error_t Error;

    // Allocating memory
    PressPumpData.HandleControl = calloc (Instances, sizeof(Handle_t));
    if (NULL == PressPumpData.HandleControl) {
        return (E_MEMORY_FULL);
    }
    
#ifdef ASB15_VER_B
    PressPumpData.HandlePWMControl = calloc (Instances, sizeof(Handle_t));
    if (NULL == PressPumpData.HandlePWMControl) {
        return (E_MEMORY_FULL);
    }
#endif

    PressPumpData.StartingTime = calloc (Instances, sizeof(UInt32));
    if (NULL == PressPumpData.StartingTime) {
        return (E_MEMORY_FULL);
    }
    PressPumpData.OperatingTime = calloc (Instances, sizeof(UInt32));
    if (NULL == PressPumpData.OperatingTime) {
        return (E_MEMORY_FULL);
    }

    // Open current measurement sensor
    PressPumpMonitor.Handle = halAnalogOpen (CurrentChannel, HAL_OPEN_READ, 0, NULL);
    if (PressPumpMonitor.Handle < 0) {
        return (PressPumpMonitor.Handle);
    }

#ifdef ASB15_VER_A
    // Open pumping element control outputs
    for (i = 0; i < Instances; i++) {
        PressPumpData.HandleControl[i] = halPortOpen (ControlChannel + i, HAL_OPEN_WRITE);
        if (PressPumpData.HandleControl[i] < 0) {
            return (PressPumpData.HandleControl[i]);
        }
    }
#endif

#ifdef ASB15_VER_B
    // Open pumping element control outputs
    for (i = 0; i < Instances; i++) {
        PressPumpData.HandleControl[i] = halPortOpen (ControlChannel + i, HAL_OPEN_WRITE);
        if (PressPumpData.HandleControl[i] < 0) {
            return (PressPumpData.HandleControl[i]);
        }
        // set inactive low for 24V power supply
        #if 0
        halPortWrite(PressPumpData.HandleControl[i], 0);
        #endif
        
        halPortWrite(PressPumpData.HandleControl[i], 1);
    }
    
    // Open pumping element PWM control outputs
    for (i = 0; i < Instances; i++) {
        PressPumpData.HandlePWMControl[i] = halAnalogOpen (PWMControlChannel + i, HAL_OPEN_WRITE, 0, NULL);
        if (PressPumpData.HandlePWMControl[i] < 0) {
            return (PressPumpData.HandlePWMControl[i]);
        }
    }
    
    //halAnalogWrite(PressPumpData.HandleControl[0], 0x7FFF);
#endif    

    
    PressPumpData.Instances = Instances;
    PressPumpData.MinValue = MAX_INT16;
    PressPumpData.MaxValue = MIN_INT16;
    PressPumpData.SampleTime = bmGetTime ();
    *Params = &PressPumpData.Params;
    
    PressPumpMonitor.Filter = 3;
    
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief  Resets pump current measurements
 *
 *      This function resets all runtime parameters of the pump current
 *      measurements to their default values. This function should always be
 *      called, when the pumping process has been stopped and it is restarted.
 *
 ****************************************************************************/

void pressPumpReset ()
{
    //PressPumpMonitor.InCount = 0;
    //PressPumpData.MinValue = MAX_INT16;
    //PressPumpData.MaxValue = MIN_INT16;
    PressPumpData.MaxActive = 0;
}


/*****************************************************************************/
/*!
 *  \brief   Get filtered analog input
 *
 *      Reads an analog input value, saves it in the data history buffer,
 *      calculates the mean value using the history buffer and returns
 *      the averaged result. The variable "Data->Filter" defines how many
 *      entries in the history buffer will be taken to average over.
 *      The filtered result is returned in "Data->Value".
 *
 *  \iparam  Monitor = Pointer to power monitor data structure
 *
 *  \return  Filtered analog input value
 *
 *****************************************************************************/

static Error_t pressPumpGetFilteredInput (PressPumpMonitor_t *Monitor) {

    Int32 Average = 0;
    Error_t Status;
    UInt16 Index;
    UInt16 Count;
    UInt16 i;

    Status = halAnalogRead (
        Monitor->Handle, &Monitor->History[Monitor->NextIn]);

    if (Status == NO_ERROR) {

        if (Monitor->InCount < ELEMENTS(Monitor->History)) {
            Monitor->InCount++;
        }
        Count = MIN(Monitor->Filter+1, Monitor->InCount);

        Index = Monitor->NextIn - Count;
        if (Index >= ELEMENTS(Monitor->History)) {
           Index += ELEMENTS(Monitor->History);
        }
        for (i=0; i < Count; i++) {
            if (++Index >= ELEMENTS(Monitor->History)) {
                Index = 0;
            }
            Average += Monitor->History[Index];
        }
        if (++Monitor->NextIn >= ELEMENTS(Monitor->History)) {
            Monitor->NextIn = 0;
        }
        if (Count) {
            Monitor->Value = Average / Count;
        }
    }
    return (Status);
}


/*****************************************************************************/
/*!
 *  \brief  Samples effective current through pumps
 *
 *      This function reads the analog input value delivered by the current
 *      sensor. It should be called by the task function as often as possible.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
Error_t pressSampleCurrent(void)
{
    Error_t Error;
    
    if (bmTimeExpired (PressPumpData.SampleTime) != 0) {
        PressPumpData.SampleTime = bmGetTime ();

        // Reading analog input value from sensor
        Error = pressPumpGetFilteredInput (&PressPumpMonitor);
        if (Error < NO_ERROR) {
            return Error;
        }

        // Maximum detection
        if (PressPumpMonitor.Value > PressPumpData.MaxValue) {
            PressPumpData.MaxValue = PressPumpMonitor.Value;
            //PressPumpData.EffectiveCurrent = PressPumpData.MaxValue;
        }
    }
    
    return (NO_ERROR);
}

/*****************************************************************************/
/*!
 *  \brief  Calculates the effective current through pumps
 *
 *      This function computes the effective current through pumps. It should 
 *      be called by the task function as often as possible.
 *
 *
 ****************************************************************************/
 
void pressCalcEffectiveCurrent(UInt16 Instance)
{
    PressPumpParams_t *Params = &PressPumpData.Params;  

    // EffectiveCurrent (mA) = CurrentGain (mA/V) * Amplitude (mV)
    // => (1 / 1000)    
    PressPumpData.EffectiveCurrent = ((Int32) Params->CurrentGain * PressPumpData.MaxValue) / 1000;
    
    PressPumpData.MaxValue = MIN_INT16;
}


/*****************************************************************************/
/*!
 *  \brief  Checks the measured current through the pump
 *
 *      This method updates active status of pumps. It should be called by the 
 *      task function as often as possible.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
Error_t pressPumpProgress (Bool PumpControl)
{
    UInt16 i;
    UInt16 Active;
    Error_t Error;

    Active = pressPumpActive ();
    if (Active > PressPumpData.MaxActive) {
        PressPumpData.MaxActive = Active;
    }
    //printf("Active:%d\n", Active);

#ifdef ASB15_VER_A
#if 0
    // Switch off the pumping elements
    for (i = 0; i < PressPumpData.Instances; i++) {
        if (bmTimeExpired (PressPumpData.StartingTime[i]) >= PressPumpData.OperatingTime[i]) {
            Error = halPortWrite (PressPumpData.HandleControl[i], 0);
            if (Error < NO_ERROR) {
                return (Error);
            }
        }
    }
#endif
#endif


#ifdef ASB15_VER_B
    if ( PumpControl ) {
        // Switch off the pumping elements
        for (i = 0; i < PressPumpData.Instances; i++) {
            if (bmTimeExpired (PressPumpData.StartingTime[i]) >= PressPumpData.OperatingTime[i]) {
                Error = halAnalogWrite(PressPumpData.HandlePWMControl[i], 0);
                if (Error < NO_ERROR) {
                    return (Error);
                }
            }
        }
    }
#endif

#if 0
    if (bmTimeExpired (PressPumpData.SampleTime) != 0) {
        PressPumpData.SampleTime = bmGetTime ();

        // Reading analog input value from sensor
        Error = pressPumpGetFilteredInput (&PressPumpMonitor);
        if (Error < NO_ERROR) {
            return Error;
        }

        // Minimum / Maximum detection
        if (PressPumpMonitor.Value < PressPumpData.MinValue) {
            PressPumpData.MinValue = PressPumpMonitor.Value;
        }
        if (PressPumpMonitor.Value > PressPumpData.MaxValue) {
            PressPumpData.MaxValue = PressPumpMonitor.Value;
        }
    }
#endif    

    return (NO_ERROR);
}

/*****************************************************************************/
/*!
 *  \brief  Checks the measured current through the pump
 *
 *      This method checks if the current is in the range specified by the
 *      master computer. If not, the module issues an error message.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
Error_t pressPumpCheck (void)
{
    //Error_t Error;
    UInt16 Current;
    UInt16 ActiveCount = PressPumpData.MaxActive;
    PressPumpParams_t *Params = &PressPumpData.Params;
    
    //PressPumpData.EffectiveCurrent = PressPumpData.MaxValue;
    //PressPumpData.MinValue = MAX_INT16;
    
    //PressPumpData.MaxValue = MIN_INT16;
    PressPumpData.Failed = FALSE;
    PressPumpData.MaxActive = 0;

    // Check the current through the pumps
    // All pumping elements are off
    if (ActiveCount == 0) {
        //printf("Pump Current[aaa]:%d\n", PressPumpData.EffectiveCurrent);
        if (PressPumpData.EffectiveCurrent > Params->DesiredCurThreshold) {
        
            // Calculate current effective current
            UInt16 RealEffectiveCurrent;            
            RealEffectiveCurrent = ((Int32) Params->CurrentGain * PressPumpMonitor.Value) / 1000;        
            
            if(RealEffectiveCurrent > Params->DesiredCurThreshold) {
                PressPumpData.Failed = TRUE;
                printf("Pump Current Err[aaa]:%d\n", PressPumpData.EffectiveCurrent);
            }
        }
        
    }
    else {
        Current = PressPumpData.EffectiveCurrent / ActiveCount;
        
        //printf("Pump Current[bbb]:%d\n", PressPumpData.EffectiveCurrent);
        
        // Check if the current is out of range
        if (Current + Params->DesiredCurThreshold < Params->DesiredCurrent ||
                Current > Params->DesiredCurrent + Params->DesiredCurThreshold) {
                printf("Pump Current Err[bbb]:%d\n", PressPumpData.EffectiveCurrent);
                PressPumpData.Failed = TRUE;
        }
    }
    
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief  Returns if the current is in range
 *
 *      This method returns if the pump current measurement is in an error
 *      state or not.
 *
 *  \return  Pump current check failed or not
 *
 ****************************************************************************/
 
Bool pressPumpFailed ()
{
    return PressPumpData.Failed;
}


/*****************************************************************************/
/*! 
 *  \brief   Sets the actuating variable of a pumping element
 *
 *      This function receives the actuating variable computed by the PID
 *      controller. This input value is converted into a pulse width
 *      modulation signal that is passed to an analog output port.
 * 
 *  \iparam  OperatingTime = Active time in milliseconds
 *  \iparam  EndTime = End of the sampling period in milliseconds
 *  \iparam  Instance = Instance number
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

#ifdef ASB15_VER_A 
Error_t pressPumpActuate (UInt32 OperatingTime, UInt32 EndTime, UInt16 Instance)
{
    PressPumpData.StartingTime[Instance] = bmGetTime ();

    // Adaptions to the zero crossing relay, it can only switch every 20 ms
    if (OperatingTime > 0 && OperatingTime < 20) {
        OperatingTime = 20;
    }
    
    // Minimal pulse length is 20 ms plus last time active is 20 ms before end time
    if (PressPumpData.StartingTime[Instance] > EndTime - 40) {
        PressPumpData.OperatingTime[Instance] = 0;
    }
    else if (PressPumpData.StartingTime[Instance] + OperatingTime > EndTime - 20) {
        PressPumpData.OperatingTime[Instance] = EndTime - 20 - PressPumpData.StartingTime[Instance];
    }
    else {
        PressPumpData.OperatingTime[Instance] = OperatingTime;
    }

    if (PressPumpData.OperatingTime[Instance] == 0) {
        return (halPortWrite (PressPumpData.HandleControl[Instance], 0));
    }

    return (halPortWrite (PressPumpData.HandleControl[Instance], 1));
}
#endif

#ifdef ASB15_VER_B
Error_t pressPumpActuate (UInt32* OperatingTime, UInt32 EndTime, UInt16 Instance)
{
    UInt32 OptTime = 0;
    
    if ( OperatingTime != NULL) {
        OptTime = *OperatingTime;
    }
    
    PressPumpData.StartingTime[Instance] = bmGetTime ();

    // Adaptions to the zero crossing relay, it can only switch every 20 ms
    if (OptTime > 0 && OptTime < 20) {
        OptTime = 20;
    }
    
    // Minimal pulse length is 20 ms plus last time active is 20 ms before end time
    if (PressPumpData.StartingTime[Instance] > EndTime - 40) {
        PressPumpData.OperatingTime[Instance] = 0;
    }
    else if (PressPumpData.StartingTime[Instance] + OptTime > EndTime - 20) {
        PressPumpData.OperatingTime[Instance] = EndTime - 20 - PressPumpData.StartingTime[Instance];
    }
    else {
        PressPumpData.OperatingTime[Instance] = OptTime;
    }

    //TODO:
    //Find a more accurate coefficient to convert PID output to opt. time
#if 0    
    if (OperatingTime != NULL) {
        *OperatingTime = PressPumpData.OperatingTime[Instance];
    }
    
    //if (OperatingTime != NULL) {
        //printf("Opt[ONOFF]:%d\n", PressPumpData.OperatingTime[Instance]);
    //}
#endif    

    if (PressPumpData.OperatingTime[Instance] == 0) {
        if (OperatingTime != NULL) {
            *OperatingTime = 0;
        }
        return (halAnalogWrite(PressPumpData.HandlePWMControl[Instance], 0));
    }
            
    if (OperatingTime != NULL) {
        *OperatingTime = EndTime - PressPumpData.StartingTime[Instance];
    }
        
    return (halAnalogWrite(PressPumpData.HandlePWMControl[Instance], 0xFFFF));
}
#endif


#ifdef ASB15_VER_B
/*****************************************************************************/
/*! 
 *  \brief   Sets the actuating PWM variable of a pumping element
 *
 *      This function receives the actuating PWM variable computed by the PID
 *      controller. This input value is converted into a pulse width
 *      modulation signal that is passed to an analog output port.
 * 
 *  \iparam  ActuatingPwmWidth = PWM duty in percentage
 *  \iparam  EndTime = End of the sampling period in milliseconds
 *  \iparam  Instance = Instance number
 *  \iparam  OperatingTime = Returned active time in milliseconds
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t pressPumpActuatePwm (Int32 ActuatingPwmWidth, UInt32 EndTime, UInt16 Instance, UInt32* OperatingTime)
{
    
    PressPumpData.StartingTime[Instance] = bmGetTime();
    
    if (ActuatingPwmWidth < 0 ) {
        ActuatingPwmWidth = 0;
    }
 
 /*
 
 200  -> 30% Duty
 8000 -> 90% Duty
 
 (OpTime-200)/(8000-200) = (Duty-30)/(90-30)
 
 Duty = (OpTime-200)/(8000-200)*(90-30)+30
 
 PWMWidth = (Duty*0xFFFF)/100 = [(OpTime-200)*(90-30)/(8000-200)+30] * (0xFFFF/100)
 
 */
 
    if (ActuatingPwmWidth == 0) {
        PressPumpData.OperatingTime[Instance] = 0;
        if (OperatingTime != NULL) {
            *OperatingTime = 0;
        }
        //printf("Opt[PWM]:%d\n", 0);
        return (halAnalogWrite(PressPumpData.HandlePWMControl[Instance], 0));
    }
    else {
        PressPumpData.OperatingTime[Instance] = EndTime - PressPumpData.StartingTime[Instance];
        if (OperatingTime != NULL) {
            *OperatingTime = PressPumpData.OperatingTime[Instance];
        }
        //printf("Opt[PWM]:%d\n", PressPumpData.OperatingTime[Instance]);
        return (halAnalogWrite(PressPumpData.HandlePWMControl[Instance], ActuatingPwmWidth*0xFFFF/100));
    }

 
   //return (NO_ERROR);   
}
#endif



/*****************************************************************************/
/*! 
 *  \brief   Returns the electric current through the pumping elements
 *
 *      This function returns the electric current flowing through the pumping
 *      elements in milliamperes.
 * 
 *  \return  Effective current value in milliamperes
 *
 ****************************************************************************/

UInt16 pressPumpCurrent (void)
{
    return PressPumpData.EffectiveCurrent;
}


/*****************************************************************************/
/*! 
 *  \brief   Returns the number of active pumping elements
 *
 *      This function returns the number of currently active pumping elements.
 *
 *  \return  Number of active pumping elements or an error code
 *
 ****************************************************************************/

UInt16 pressPumpActive()
{
    UInt16 i;
    UInt16 Active = 0;
    UInt32 Time = bmGetTime ();

    for (i = 0; i < PressPumpData.Instances; i++) {
        if (PressPumpData.OperatingTime[i] != 0 && Time > PressPumpData.StartingTime[i] &&
                Time < PressPumpData.StartingTime[i] + PressPumpData.OperatingTime[i] + 10) {
            Active++;
        }
    }

    return (Active);
}


#ifdef ASB15_VER_B
/*****************************************************************************/
/*!
 *  \brief  Enable/Disable 24V power supply of 4-wire pump
 *
 *      This function changes the state of 24V power supply connected to   
 *      4-wire pump according to input flag.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
Error_t pressPumpEnablePower(Bool PowerState, UInt16 Instance)
{
    if (PowerState) {
        return halPortWrite(PressPumpData.HandleControl[Instance], 1);
    }
    else {
        return halPortWrite(PressPumpData.HandleControl[Instance], 0);
    }
}
#endif

//****************************************************************************/
