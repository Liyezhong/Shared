/****************************************************************************/
/*! \file fmTemperatureHeater.c
 * 
 *  \brief Functions controlling a heating element.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *  The methods defined here control the temperature of a heating element. The
 *  needed input parameter is computed by a PID controller. There are also
 *  functions that determine the connection between various heating elements.
 *  Furthermore, the current usage in milliamperes of the elements is computed
 *  here.
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

#include <stdio.h>
#include <stdlib.h>
#include "Global.h"
#include "bmError.h"
#include "bmMonitor.h"
#include "bmTime.h"
#include "halLayer.h"
#include "ModuleIds.h"
#include "fmTemperature.h"
#include "fmTemperatureHeater.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! The state of the heater circuit */
typedef enum {
    TEMP_HEATER_STATE_UNDEFINED = 0,//!< State not yet decided
    TEMP_HEATER_STATE_SERIAL = 1,   //!< Swicthed serially
    TEMP_HEATER_STATE_PARALLEL = 2, //!< Swicthed parallelly
    TEMP_HEATER_STATE_DC = 3        //!< No switch for DC heater
} TempHeaterState_t;
/*! Contains the data for the heater current measurements */
typedef struct {
    Handle_t HandleSwitch;      //!< Handle for the heating element circuit switch
    Handle_t *HandleControl;    //!< Array of handles for the heating element control outputs
    Int16 MinValue;             //!< Minimal value of the last current half-wave
    Int16 MaxValue;             //!< Maximal value of the last current half-wave
    UInt16 EffectiveCurrent;    //!< Effective value of the heating element current in mA
    TempHeaterState_t State;    //!< Heating elment circuit is switched serially or parallelly
    TempHeaterParams_t *Params; //!< Array of parameters for the heater current measurements
    UInt16 Instances;           //!< Total number of module instances
    UInt16 MaxActive;           //!< Maximum number of active heating elements
    UInt16 MaxActDesiredCurrent;//!< Maximum desired current of active heating elements
    UInt16 MaxActDesiredCurThreshold;
    UInt32 MaxActiveStatus;
    UInt32 MaxActiveStatus2;
    Bool *ActiveStatus;         //!< Active status for each heating element
    UInt32 *StartingTime;       //!< Time in milliseconds an output pulse is started
    UInt32 *OperatingTime;      //!< Time in milliseconds an output pulse is active
    UInt32 SampleTime;          //!< Time the last sample was taken
    Bool Failed;                //!< The heater check failed
} TempHeaterData_t;


/*! Data structure to hold monitoring variables */
typedef struct {
    UInt16 Filter:8;    //!< Number of samples to average over
    Int16 History[16];  //!< History of input values (for filtering)
    UInt16 InCount;     //!< Number of samples in history buffer
    UInt16 NextIn;      //!< Next-in pointer into history buffer
    Int16 Value;        //!< Filtered input value
    Handle_t Handle;    //!< Handle for input channel
} TempHeaterMonitor_t;

//****************************************************************************/
// Private Variables 
//****************************************************************************/

/*! Global data for the heating element control functionality */
static TempHeaterData_t TempHeaterData;
/*! Global data for input filtering functionality */
static TempHeaterMonitor_t TempHeaterMonitor;  


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static Error_t tempHeaterGetFilteredInput (TempHeaterMonitor_t *Monitor);
static Error_t tempHeaterSwitch (Handle_t Handle, TempHeaterState_t State);

//static UInt16 DesiredCurrent;

static UInt32 ActiveStatus;
/*****************************************************************************/
/*!
 *  \brief  Initializes the handling of the heating elements
 *
 *      This methods opens the peripheral handlers needed for the control of
 *      the heating elements. It also allocates the memory required by this
 *      task and initializes the heating element switching circuit and the
 *      measurement of the effective current through the elements.
 *
 *  \oparam  Params = Points to the public data of this module
 *  \iparam  CurrentChannel = HAL channel for the current measurement
 *  \iparam  SwitchChannel = HAL channel for the circuit switch
 *  \iparam  ControlChannel = HAL channel for the heater control
 *  \iparam  Instances = Total number of module instances
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t tempHeaterInit (TempHeaterParams_t **Params, Device_t CurrentChannel, Device_t SwitchChannel, Device_t ControlChannel, TempHeaterType_t HeaterType, UInt16 Instances)
{
    UInt16 i;
    Error_t Error;

    // Allocating memory
    TempHeaterData.HandleControl = calloc (Instances, sizeof(Handle_t));
    if (NULL == TempHeaterData.HandleControl) {
        return (E_MEMORY_FULL);
    }
    TempHeaterData.StartingTime = calloc (Instances, sizeof(UInt32));
    if (NULL == TempHeaterData.StartingTime) {
        return (E_MEMORY_FULL);
    }
    TempHeaterData.OperatingTime = calloc (Instances, sizeof(UInt32));
    if (NULL == TempHeaterData.OperatingTime) {
        return (E_MEMORY_FULL);
    }
    TempHeaterData.Params = calloc (Instances, sizeof(TempHeaterParams_t));
    if (NULL == TempHeaterData.Params) {
        return (E_MEMORY_FULL);
    }
    TempHeaterData.ActiveStatus = calloc (Instances, sizeof(Bool));
    if (NULL == TempHeaterData.ActiveStatus) {
        return (E_MEMORY_FULL);
    }    

    // Open current measurement sensor
    TempHeaterMonitor.Handle = halAnalogOpen (CurrentChannel, HAL_OPEN_READ, 0, NULL);
    if (TempHeaterMonitor.Handle < 0) {
        return (TempHeaterMonitor.Handle);
    }

    // Open the heating element circuit switch
    if (HeaterType == TYPE_HEATER_AC) {     
        TempHeaterData.HandleSwitch = halPortOpen (SwitchChannel, HAL_OPEN_WRITE);
        if (TempHeaterData.HandleSwitch < 0) {
            return (TempHeaterData.HandleSwitch);
        }
    }

    // Open heating element control outputs
    for (i = 0; i < Instances; i++) {
        TempHeaterData.HandleControl[i] = halPortOpen (ControlChannel + i, HAL_OPEN_WRITE);
        if (TempHeaterData.HandleControl[i] < 0) {
            return (TempHeaterData.HandleControl[i]);
        }
    }
   
    // Set circuit to serial mode (default for 220V)
    if (HeaterType == TYPE_HEATER_AC) {
        Error = tempHeaterSwitch (TempHeaterData.HandleSwitch, TEMP_HEATER_STATE_UNDEFINED);
        if (Error < NO_ERROR) {
            return (Error);
        }
    }
    else {
        TempHeaterData.State = TEMP_HEATER_STATE_DC;
    }
    
    TempHeaterData.Instances = Instances;
    TempHeaterData.MinValue = MAX_INT16;
    TempHeaterData.SampleTime = bmGetTime ();
    
    *Params = TempHeaterData.Params;
    
    TempHeaterMonitor.Filter = 2;

    if (HeaterType == TYPE_HEATER_AC) {
        while (TempHeaterMonitor.InCount <= TempHeaterMonitor.Filter) {
            if (bmTimeExpired (TempHeaterData.SampleTime) != 0) {
                TempHeaterData.SampleTime = bmGetTime ();
                Error = tempHeaterGetFilteredInput (&TempHeaterMonitor);
                if (Error < NO_ERROR) {
                    return (Error);
                }
            }
        }
        TempHeaterData.MaxValue = TempHeaterMonitor.Value;
    }
    else if (HeaterType == TYPE_HEATER_DC) {
        TempHeaterData.MaxValue = MIN_INT16;
    }

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief  Resets heater current measurements
 *
 *      This function resets all runtime parameters of the heater current
 *      measurements to their default values. This function should always be
 *      called, when the heating process has been stopped and it is restarted.
 *
 ****************************************************************************/

void tempHeaterReset ()
{
    //TempHeaterMonitor.InCount = 0;
    //TempHeaterData.MinValue = MAX_INT16;
    TempHeaterData.MaxActive = 0;
    TempHeaterData.MaxActDesiredCurrent = 0;
    TempHeaterData.MaxActDesiredCurThreshold = 0;
    TempHeaterData.MaxActiveStatus = 0;
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

static Error_t tempHeaterGetFilteredInput (TempHeaterMonitor_t *Monitor) {

    Int32 Average = 0;
    Error_t Status;
    UInt16 Index;
    UInt16 Count;
    UInt16 i;

    Status = halAnalogRead (Monitor->Handle, &Monitor->History[Monitor->NextIn]);
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
 *  \brief  Checks if the effective current is in range
 *
 *      This function reads the analog input value delivered by the current
 *      sensor and computes the effective current. It is also able to detect a
 *      zero-crossing of the AC wave. The function furthermore deactivates the
 *      pulses controlling the heating elements. It should be called by the
 *      task function as often as possible.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t tempHeaterProgress ()
{
    UInt16 i;
    UInt16 Active, ActiveDesiredCurrent, ActiveDesiredCurThreshold;
    Error_t Error;

    Active = tempHeaterActive ();
    if (Active > TempHeaterData.MaxActive) {
        TempHeaterData.MaxActive = Active;
        TempHeaterData.MaxActiveStatus = ActiveStatus;
    }
    
    ActiveDesiredCurrent = tempGetActiveDesiredCurrent ();
    if (ActiveDesiredCurrent > TempHeaterData.MaxActDesiredCurrent) {
        TempHeaterData.MaxActDesiredCurrent = ActiveDesiredCurrent;
    }
    
    ActiveDesiredCurThreshold = tempGetActiveDesiredCurThreshold ();
    if (ActiveDesiredCurThreshold > TempHeaterData.MaxActDesiredCurThreshold) {
        TempHeaterData.MaxActDesiredCurThreshold = ActiveDesiredCurThreshold;
    }    

    // Switch off the heating elements
    for (i = 0; i < TempHeaterData.Instances; i++) {
        if (bmTimeExpired (TempHeaterData.StartingTime[i]) > TempHeaterData.OperatingTime[i]) {
            Error = halPortWrite (TempHeaterData.HandleControl[i], 0);
            if (Error < NO_ERROR) {
                return (Error);
            }
        }
    }

#if 0
    if (bmTimeExpired (TempHeaterData.SampleTime) != 0) {
        TempHeaterData.SampleTime = bmGetTime ();

        // Reading analog input value from sensor
        Error = tempHeaterGetFilteredInput (&TempHeaterMonitor);
        if (Error < NO_ERROR) {
            return (Error);
        }

        // Minimum / Maximum detection
        if (TempHeaterMonitor.Value < TempHeaterData.MinValue) {
            TempHeaterData.MinValue = TempHeaterMonitor.Value;
        }
    }
#endif

    return (NO_ERROR);
}

Error_t tempSampleCurrent(TempHeaterType_t HeaterType)
{
    Error_t Error;
    
    if (bmTimeExpired (TempHeaterData.SampleTime) != 0) {
        TempHeaterData.SampleTime = bmGetTime ();

        // Reading analog input value from sensor
        Error = tempHeaterGetFilteredInput (&TempHeaterMonitor);
        if (Error < NO_ERROR) {
            return (Error);
        }

        // Minimum / Maximum detection
        if (HeaterType == TYPE_HEATER_AC) {
            if (TempHeaterMonitor.Value < TempHeaterData.MinValue) {
                TempHeaterData.MinValue = TempHeaterMonitor.Value;
            }
        }
        else if (HeaterType == TYPE_HEATER_DC) {
            if (TempHeaterMonitor.Value > TempHeaterData.MaxValue) {
                TempHeaterData.MaxValue = TempHeaterMonitor.Value;
            }
        }
        
    }

    return (NO_ERROR);
}

/*****************************************************************************/
/*!
 *  \brief  Returns cicruit switching state
 *
 *      This small method returns, if the heating elements are switched
 *      serially or in parallel.
 *
 *  \return  Circuit is switched parallely (TRUE) or serially (FALSE)
 *
 ****************************************************************************/

Bool tempHeaterParallel (void)
{
    if (TempHeaterData.State == TEMP_HEATER_STATE_PARALLEL) {
        return (TRUE);
    }
    return (FALSE);
}


UInt8 tempHeaterSwitchState (void)
{
    return (TempHeaterData.State);
}

/*****************************************************************************/
/*!
 *  \brief  Checks the measured current through the heating elements
 *
 *      This method checks if the current is in the range specified by the
 *      master computer. If it is not, the function also checks if it is in
 *      the range of a 100 to 127V network. When this is the case, the heating
 *      elements are switched in parallel. When neither one is correct, the
 *      module issues an error message.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
void tempCalcEffectiveCurrent(/*UInt16 Instance, */TempHeaterType_t HeaterType)
{
    UInt16 Diff;
    
    TempHeaterParams_t *Params = TempHeaterData.Params;  
    
    if (HeaterType == TYPE_HEATER_AC) {
    
        if (TempHeaterData.MaxValue >= TempHeaterData.MinValue) {
            Diff = TempHeaterData.MaxValue - TempHeaterData.MinValue;
        }
        else {
            Diff = TempHeaterData.MinValue - TempHeaterData.MaxValue;
        }  
        // Compute effective current
        // EffectiveCurrent (mA) = CurrentGain (mA/V) * Amplitude (mV) / sqrt(2)
        // => (1 / 1000 * 10000) / 14142 = 10 / 14142
        //TempHeaterData.EffectiveCurrent = (((Int32) Params[Instance].CurrentGain * Diff) * 10) / 14142;
        TempHeaterData.EffectiveCurrent = (((Int32) Params[0].CurrentGain * Diff) * 10) / 14142;
        
        TempHeaterData.MinValue = MAX_INT16;   
    }
    else if (HeaterType == TYPE_HEATER_DC) {
    
        TempHeaterData.EffectiveCurrent = TempHeaterData.MaxValue;
        
        TempHeaterData.MaxValue = MIN_INT16;
    }
    
    //printf("Max:%d Min:%d EC:%d\n", TempHeaterData.MaxValue, TempHeaterData.MinValue, TempHeaterData.EffectiveCurrent);
}


Error_t tempHeaterCheck (/*UInt16 Instance, */TempHeaterType_t HeaterType)
{
    Error_t Error;
    UInt16 Current = 0;
    UInt16 DesiredCurThreshold = 0, DesiredCurrent = 0;
    UInt16 CurrentDeviation = 0;
    UInt16 ActiveDesiredCurrent = TempHeaterData.MaxActDesiredCurrent;
    UInt16 ActiveDesiredCurThreshold = TempHeaterData.MaxActDesiredCurThreshold;
    UInt16 ActiveCount = TempHeaterData.MaxActive;
    
    TempHeaterParams_t *Params = TempHeaterData.Params;
    //DesiredCurThreshold = Params->DesiredCurThreshold;
    CurrentDeviation = Params[0].CurrentDeviation;
    

    TempHeaterData.Failed = FALSE;
    TempHeaterData.MaxActive = 0;
    TempHeaterData.MaxActDesiredCurrent = 0;
    TempHeaterData.MaxActDesiredCurThreshold = 0;
    TempHeaterData.MaxActiveStatus2 = TempHeaterData.MaxActiveStatus;
    TempHeaterData.MaxActiveStatus = 0;


    if (HeaterType == TYPE_HEATER_DC) {
        // Check the current through the DC heaters
        // All heating elements are off    
        if (ActiveCount == 0) {
            //printf("DC[0]:%d %d %d %d\n", TempHeaterData.EffectiveCurrent, TempHeaterMonitor.Value, DesiredCurrent, CurrentDeviation);
                        
            if (TempHeaterData.EffectiveCurrent > CurrentDeviation &&
                TempHeaterMonitor.Value > CurrentDeviation) {
                TempHeaterData.Failed = TRUE;
            }
        }
        else {
            Current = TempHeaterData.EffectiveCurrent / ActiveCount;
            DesiredCurrent = ActiveDesiredCurrent / ActiveCount;
            DesiredCurThreshold = ActiveDesiredCurThreshold / ActiveCount;
            //printf("DC:%d %d %d [%d]\n", Current, DesiredCurrent, DesiredCurThreshold, ActiveCount);
            
            // Check if the current is out of range
            if (Current + DesiredCurThreshold < DesiredCurrent ||
                    Current > DesiredCurrent + DesiredCurThreshold) {
                    TempHeaterData.Failed = TRUE;
            }
        }
    }
    else if (HeaterType == TYPE_HEATER_AC) {

        // Check the current through the AC heaters
        // All heating elements are off
        if (ActiveCount == 0) {
            //printf("AC[0]:%d %d %d [%d]\n", TempHeaterData.EffectiveCurrent, DesiredCurrent, DesiredCurThreshold, ActiveCount);
            if (TempHeaterData.EffectiveCurrent > CurrentDeviation) {
                
                // Calculate current effective current
                UInt16 Diff, RealEffectiveCurrent;
                if (TempHeaterData.MaxValue >= TempHeaterMonitor.Value) {
                    Diff = TempHeaterData.MaxValue - TempHeaterMonitor.Value;
                }
                else {
                    Diff = TempHeaterMonitor.Value - TempHeaterData.MaxValue;
                }                
                RealEffectiveCurrent = (((Int32) Params[0].CurrentGain * Diff) * 10) / 14142;
                
                if (RealEffectiveCurrent > CurrentDeviation) {
                    TempHeaterData.Failed = TRUE;
                    printf("AC[0] Err:%d %d %d [%d]\n", RealEffectiveCurrent, DesiredCurrent, CurrentDeviation, ActiveCount);
                }
            }
        }
        // Heating elements are switched serially
        else if (TempHeaterData.State != TEMP_HEATER_STATE_PARALLEL) {
            Current = TempHeaterData.EffectiveCurrent / ActiveCount;
            DesiredCurrent = ActiveDesiredCurrent / ActiveCount;
            DesiredCurThreshold = ActiveDesiredCurThreshold / ActiveCount;
            //printf("220V:%d %d %d [%d]\n", Current, DesiredCurrent, DesiredCurThreshold, ActiveCount);
            
            // Check if the current is out of range (200 - 240V)
            if (Current + DesiredCurThreshold < DesiredCurrent ||
                    Current > DesiredCurrent + DesiredCurThreshold) {                    
                // Check if the current is out of range (100 - 127V)
                if (Current + DesiredCurThreshold / 2 < DesiredCurrent / 2 ||
                        Current > DesiredCurrent / 2 + DesiredCurThreshold / 2) {
                    TempHeaterData.Failed = TRUE;
                    printf("220V ErrA:%d %d %d\n", Current, DesiredCurrent, DesiredCurThreshold);
                }
                else if (TempHeaterData.State == TEMP_HEATER_STATE_UNDEFINED) {
                    
                        // Switched for 110V            
                    Error = tempHeaterSwitch(TempHeaterData.HandleSwitch, TEMP_HEATER_STATE_PARALLEL);
                    if (Error < NO_ERROR) {
                            printf("Switch for 110V failed\n");
                            return (Error);
                        }
                        printf("Switch for 110V\n");
                }
                else {
                    TempHeaterData.Failed = TRUE;
                }
            }
            else {
                TempHeaterData.State = TEMP_HEATER_STATE_SERIAL;
            }
        }
        // Heating elements are switched parallelly
        else {
            Current = TempHeaterData.EffectiveCurrent / ActiveCount;
            DesiredCurrent = ActiveDesiredCurrent / ActiveCount;
            DesiredCurThreshold = ActiveDesiredCurThreshold / ActiveCount;
            
            //printf("110V:%d %d %d [%d]\n", Current, DesiredCurrent, DesiredCurThreshold, ActiveCount);
            
            // Check if the current is out of range (100 - 127V)
            if (Current / 2 + DesiredCurThreshold < DesiredCurrent ||
                    Current / 2 > DesiredCurrent + DesiredCurThreshold) {
                // Switched for 220V
                Error = tempHeaterSwitch(TempHeaterData.HandleSwitch, TEMP_HEATER_STATE_UNDEFINED);
                if (Error < NO_ERROR) {
                    return (Error);
                }
                printf("Switch for 220V\n");

                TempHeaterData.Failed = TRUE;
                printf("110V Err:%d %d %d\n", Current, DesiredCurrent, DesiredCurThreshold);
            }
        }
    
    }
    
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief  Returns if the current is in range
 *
 *      This method returns if the heater current measurement is in an error
 *      state or not.
 *
 *  \return  Heater current check failed or not
 *
 ****************************************************************************/
 
Bool tempHeaterFailed ()
{
    return TempHeaterData.Failed;
}


/*****************************************************************************/
/*! 
 *  \brief   Sets the actuating variable of a heating element
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
 
Error_t tempHeaterActuate (UInt32 OperatingTime, UInt32 EndTime, UInt16 Instance)
{
    TempHeaterData.StartingTime[Instance] = bmGetTime ();

    // If the mains voltage domain is undecided, only one instance may be active
    if (TempHeaterData.State == TEMP_HEATER_STATE_UNDEFINED && tempHeaterActive()) {
        OperatingTime = 0;
    }
    // Adaptions to the zero crossing relay, it can only switch every 30 ms
    if (OperatingTime > 0 && OperatingTime < 30) {
        OperatingTime = 30;
    }

    // Minimal pulse length is 30 ms plus last time active is 20 ms before end time
    if (TempHeaterData.StartingTime[Instance] > EndTime - 50) {
        TempHeaterData.OperatingTime[Instance] = 0;
    }
    else if (TempHeaterData.StartingTime[Instance] + OperatingTime > EndTime - 20) {
        TempHeaterData.OperatingTime[Instance] = EndTime - 20 - TempHeaterData.StartingTime[Instance];
    }
    else {
        TempHeaterData.OperatingTime[Instance] = OperatingTime;
    }

    if (TempHeaterData.OperatingTime[Instance] == 0) {
        return (halPortWrite (TempHeaterData.HandleControl[Instance], 0));
    }
    return (halPortWrite (TempHeaterData.HandleControl[Instance], 1));
}


/*****************************************************************************/
/*! 
 *  \brief   Controls the connection between different heating elements
 *
 *      This function is able to switch the electric power circuit of the
 *      heating elements. For example, they can be connected in series or
 *      in parallel. This is necessary due to different voltage levels in
 *      different countries. 
 *
 *  \iparam  Handle = Handle of the digital port switching the circuit
 *  \iparam  Parallel = Parallel (TRUE) or serial (FALSE)
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t tempHeaterSwitch (Handle_t Handle, TempHeaterState_t State)
{
    UInt16 Value;

    if (State == TEMP_HEATER_STATE_PARALLEL) {
        Value = 1;
    }
    else {
        Value = 0;
    }

    TempHeaterData.State = State;

    return (halPortWrite(Handle, Value));
}


/*****************************************************************************/
/*! 
 *  \brief   Returns the electric current through the heating elements
 *
 *      This function returns the electric current flowing through the heating
 *      elements in milliamperes.
 * 
 *  \return  Effective current value in milliamperes
 *
 ****************************************************************************/

UInt16 tempHeaterCurrent (void)
{
    return TempHeaterData.EffectiveCurrent;
}


/*****************************************************************************/
/*! 
 *  \brief   Returns the number of active heating elements
 *
 *      This function returns the number of currently active heating elements.
 *
 *  \return  Number of active heating elements or an error code
 *
 ****************************************************************************/

UInt16 tempHeaterActive()
{
    UInt16 i;
    UInt16 Active = 0;
    UInt32 Time = bmGetTime ();
    
    ActiveStatus = 0;
 
    for (i = 0; i < TempHeaterData.Instances; i++) {
        if (TempHeaterData.OperatingTime[i] != 0 && Time >= TempHeaterData.StartingTime[i] &&
                Time < TempHeaterData.StartingTime[i] + TempHeaterData.OperatingTime[i] + 10) {
            TempHeaterData.ActiveStatus[i] = TRUE;
            Active++;
            ActiveStatus |= (1<<i);
        }
        else {
            TempHeaterData.ActiveStatus[i] = FALSE;
        }
    }

    return (Active);
}


/*****************************************************************************/
/*! 
 *  \brief   Returns the total active desired current 
 *
 *      This function returns the total desired current of currently active 
 *      heating elements.
 *
 *  \return  Total desired current of active heating elements
 *
 ****************************************************************************/

UInt16 tempGetActiveDesiredCurrent(void)
{
    UInt16 i;
    UInt16 DesiredCurrent = 0;
    
    for (i = 0; i < TempHeaterData.Instances; i++) {
        if (TempHeaterData.ActiveStatus[i]) {
            DesiredCurrent += TempHeaterData.Params[i].DesiredCurrent;
        }
    }
    
    return (DesiredCurrent);  
}

/*****************************************************************************/
/*! 
 *  \brief   Returns the total active desired current threshold
 *
 *      This function returns the total desired current threshold of currently 
 *      active heating elements.
 *
 *  \return  Total desired current threshold of active heating elements
 *
 ****************************************************************************/

UInt16 tempGetActiveDesiredCurThreshold(void)
{
    UInt16 i;
    UInt16 DesiredCurThreshold = 0;
    
    for (i = 0; i < TempHeaterData.Instances; i++) {
        if (TempHeaterData.ActiveStatus[i]) {
            DesiredCurThreshold += TempHeaterData.Params[i].DesiredCurThreshold;
        }
    }
    
    return (DesiredCurThreshold);  
}

/*****************************************************************************/
/*! 
 *  \brief   Returns the active status of heating elements
 *
 *      This function returns the status of heating elements which indicates
 *      currently active/inactive heating elements by set/clear bits.
 *
 *  \return  Active status of heating elements
 *
 ****************************************************************************/

UInt32 tempGetActiveStatus(void)
{
    return TempHeaterData.MaxActiveStatus2;
}

/*
void tempResetActiveStatus(void)
{
    UInt16 i;
 
    for (i = 0; i < TempHeaterData.Instances; i++) {
        TempHeaterData.ActiveStatus[i] = FALSE;
    }
}
*/

