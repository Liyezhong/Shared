/****************************************************************************/
/*! \file fmPressureFan.c
 * 
 *  \brief Functions controlling a fan element.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.08.2012
 *   $Author:  $ Brandon Shao
 *
 *  \b Description:
 *
 *  The methods defined here control the fan element. There are also
 *  functions that determine the connection between various fan elements.
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

#include <stdlib.h>
#include "Global.h"
#include "bmError.h"
#include "bmMonitor.h"
#include "bmTime.h"
#include "halLayer.h"
#include "ModuleIds.h"
#include "fmPressure.h"
#include "fmPressureFan.h"
#include "bmDebug.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! Contains the data for the fan current measurements */
typedef struct {
    Handle_t *HandleControl;    //!< Array of handles for the fan element control outputs
    Int16 MinValue;             //!< Minimal value of the last current half-wave
    Int16 MaxValue;             //!< Maximal value of the last current half-wave
    UInt16 EffectiveCurrent;    //!< Effective value of the fan element current in mA
    PressFanParams_t Params;    //!< Parameters for the fan current measurements
    UInt16 Instances;           //!< Total number of module instances
    Bool*  ActiveStatus;        //!< Array of flags if fan is active or not
    UInt16 MaxActive;           //!< Maximum number of active fan elements
    UInt32 SampleTime;          //!< Time the last sample was taken
    Bool Failed;                //!< The fan check failed
} PressFanData_t;


/*! Data structure to hold monitoring variables */
typedef struct {
    UInt16 Filter:8;    //!< Number of samples to average over
    Int16 History[16];  //!< History of input values (for filtering)
    UInt16 InCount;     //!< Number of samples in history buffer
    UInt16 NextIn;      //!< Next-in pointer into history buffer
    Int16 Value;        //!< Filtered input value
    Handle_t Handle;    //!< Handle for input channel
} PressFanMonitor_t;

//****************************************************************************/
// Private Variables 
//****************************************************************************/

/*! Global data for the fan element control functionality */
static PressFanData_t PressFanData;
/*! Global data for input filtering functionality */
static PressFanMonitor_t PressFanMonitor;  


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static Error_t pressFanGetFilteredInput (PressFanMonitor_t *Monitor);


/*****************************************************************************/
/*!
 *  \brief  Initializes the handling of the fan elements
 *
 *      This methods opens the peripheral handlers needed for the control of
 *      the fan elements. It also allocates the memory required by this
 *      task and initializes the fan element switching circuit and the
 *      measurement of the effective current through the elements.
 *
 *  \oparam  Params = Points to the public data of this module
 *  \iparam  CurrentChannel = HAL channel for the current measurement
 *  \iparam  ControlChannel = HAL channel for the fan control
 *  \iparam  Instances = Total number of module instances
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t pressFanInit (PressFanParams_t **Params, Device_t CurrentChannel, Device_t ControlChannel, UInt16 Instances)
{
    UInt16 i;
    //Error_t Error;

    // Allocating memory
    PressFanData.HandleControl = calloc (Instances, sizeof(Handle_t));
    if (NULL == PressFanData.HandleControl) {
        return (E_MEMORY_FULL);
    }

    PressFanData.ActiveStatus = calloc (Instances, sizeof(Bool));
    if (NULL == PressFanData.ActiveStatus) {
        return (E_MEMORY_FULL);
    }
    
    // Open current measurement sensor
    PressFanMonitor.Handle = halAnalogOpen (CurrentChannel, HAL_OPEN_READ, 0, NULL);
    if (PressFanMonitor.Handle < 0) {
        return (PressFanMonitor.Handle);
    }
    

    // Open fan element control outputs
    for (i = 0; i < Instances; i++) {
        PressFanData.HandleControl[i] = halPortOpen (ControlChannel + i, HAL_OPEN_WRITE);
        if (PressFanData.HandleControl[i] < 0) {
            return (PressFanData.HandleControl[i]);
        }
    }
    
    PressFanData.Instances = Instances;
    PressFanData.MinValue = MAX_INT16;
    PressFanData.MaxValue = MIN_INT16;
    PressFanData.SampleTime = bmGetTime ();
    *Params = &PressFanData.Params;
    
    PressFanMonitor.Filter = 3;
    
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief  Resets fan current measurements
 *
 *      This function resets all runtime parameters of the fan current
 *      measurements to their default values. This function should always be
 *      called, when the fan process has been stopped and it is restarted.
 *
 ****************************************************************************/

void pressFanReset ()
{
    PressFanData.MaxActive = 0;
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

static Error_t pressFanGetFilteredInput (PressFanMonitor_t *Monitor) {

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
 *  \brief  Samples effective current through fans
 *
 *      This function reads the analog input value delivered by the current
 *      sensor. It should be called by the task function as often as possible.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
Error_t pressFanSampleCurrent(void)
{
    Error_t Error;
    
    if (bmTimeExpired (PressFanData.SampleTime) != 0) {
        PressFanData.SampleTime = bmGetTime ();

        // Reading analog input value from sensor
        Error = pressFanGetFilteredInput (&PressFanMonitor);
        if (Error < NO_ERROR) {
            return Error;
        }

        // Maximum detection
        if (PressFanMonitor.Value > PressFanData.MaxValue) {
            PressFanData.MaxValue = PressFanMonitor.Value;
        }
    }
    
    return (NO_ERROR);
}

/*****************************************************************************/
/*!
 *  \brief  Calculates the effective current through fans
 *
 *      This function computes the effective current through fans. It should 
 *      be called by the task function as often as possible.
 *
 *
 ****************************************************************************/
 
void pressFanCalcEffectiveCurrent(UInt16 Instance)
{
    PressFanParams_t *Params = &PressFanData.Params;  

    // EffectiveCurrent (mA) = CurrentGain (mA/V) * Amplitude (mV)
    // => (1 / 1000)    
    PressFanData.EffectiveCurrent = ((Int32) Params->CurrentGain * PressFanData.MaxValue) / 1000;
    
    PressFanData.MaxValue = MIN_INT16;
}


/*****************************************************************************/
/*!
 *  \brief  Checks the measured current through the fan
 *
 *      This method updates active status of fans. It should be called by the 
 *      task function as often as possible.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
Error_t pressFanProgress (void)
{
    UInt16 Active;

    Active = pressFanActive ();
    if (Active > PressFanData.MaxActive) {
        PressFanData.MaxActive = Active;
    }
    //dbgPrint("Active:%d\n", Active);

    return (NO_ERROR);
}

/*****************************************************************************/
/*!
 *  \brief  Checks the measured current through the fan
 *
 *      This method checks if the current is in the range specified by the
 *      master computer. If not, the module issues an error message.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
Error_t pressFanCheck (void)
{
    UInt16 Current;
    UInt16 ActiveCount = PressFanData.MaxActive;
    PressFanParams_t *Params = &PressFanData.Params;
    
    PressFanData.Failed = FALSE;
    PressFanData.MaxActive = 0;

    // Check the current through the fans
    // All fan elements are off
    if (ActiveCount == 0) {
        //dbgPrint("Fan Current[%d]:%d\n", ActiveCount, PressFanData.EffectiveCurrent);
        if (PressFanData.EffectiveCurrent > Params->DesiredCurThreshold) {
        
            // Calculate current effective current
            UInt16 RealEffectiveCurrent;            
            RealEffectiveCurrent = ((Int32) Params->CurrentGain * PressFanMonitor.Value) / 1000;        
            
            if(RealEffectiveCurrent > Params->DesiredCurThreshold) {
                PressFanData.Failed = TRUE;
                dbgPrint("Fan Current Err[aaa]:%d\n", PressFanData.EffectiveCurrent);
            }
        }
        
    }
    else {
        Current = PressFanData.EffectiveCurrent / ActiveCount;
        
        //dbgPrint("Fan Current[%d]:%d\n", ActiveCount, PressFanData.EffectiveCurrent);
        
        // Check if the current is out of range
        if (Current + Params->DesiredCurThreshold < Params->DesiredCurrent ||
                Current > Params->DesiredCurrent + Params->DesiredCurThreshold) {
                dbgPrint("Fan Current Err[bbb]:%d\n", PressFanData.EffectiveCurrent);
                PressFanData.Failed = TRUE;
        }
    }
    
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief  Returns if the current is in range
 *
 *      This method returns if the fan current measurement is in an error
 *      state or not.
 *
 *  \return  Fan current check failed or not
 *
 ****************************************************************************/
 
Bool pressFanFailed ()
{
    return PressFanData.Failed;
}


/*****************************************************************************/
/*! 
 *  \brief   Sets the status of a fan element
 *
 *      This function receives the actuating variable passed from master. 
 *      This input value is then passed to the control port.
 * 
 *  \iparam  FanStatus = Fan on/off
 *  \iparam  Instance = Instance number
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t pressFanControl (Bool FanStatus, UInt16 Instance)
{   
    Error_t Error;
    
    Error = halPortWrite (PressFanData.HandleControl[Instance], FanStatus);
    
    if (Error != NO_ERROR) {
        return Error;
    }
    
    PressFanData.ActiveStatus[Instance] = FanStatus;
    
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Returns the electric current through the fan elements
 *
 *      This function returns the electric current flowing through the fan
 *      elements in milliamperes.
 * 
 *  \return  Effective current value in milliamperes
 *
 ****************************************************************************/

UInt16 pressFanCurrent (void)
{
    return PressFanData.EffectiveCurrent;
}


/*****************************************************************************/
/*! 
 *  \brief   Returns the number of active fan elements
 *
 *      This function returns the number of currently active fan elements.
 *
 *  \return  Number of active fan elements or an error code
 *
 ****************************************************************************/

UInt16 pressFanActive()
{
    UInt16 i;
    UInt16 Active = 0;

    for (i = 0; i < PressFanData.Instances; i++) {
        if (PressFanData.ActiveStatus[i]) {
            Active++;
        }
    }

    return (Active);
}

//****************************************************************************/
