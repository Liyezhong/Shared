/****************************************************************************/
/*! \file fmPressureTime.c
 * 
 *  \brief Methods maintaining the operating time.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.08.2012
 *   $Author:  $ Brandon Shao
 *
 *  \b Description:
 *
 *  The functions in this file control the operating time of the heating
 *  elements and the electronics. They store and read out minute grained time
 *  values from a non-volatile memory device.
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

#include <stdlib.h>
#include "Global.h"
#include "bmError.h"
#include "bmPermData.h"
#include "bmScheduler.h"
#include "halError.h"
#include "fmPressureTime.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define PARTITION_VERSION 1 //!< Partition layout version

#define PARAM_LAYOUT_VERSION BUILD_PARAMETER(0,2) //!< Partition layout version

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Private Variables 
//****************************************************************************/

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static Error_t pressTimeVerifyPartition (const PressTimeParams_t *Params, UInt16 Channel);

/******************************************************************************/
/*! 
 *  \brief   Reset non-volatile data to default
 *
 *      Resets all data in instance's data partition to defaults and sends
 *      an info message about that fact to the master for logging.
 * 
 *      This function is called by the base module via the module control
 *      function, whenever a reset-counter service command is received.
 *
 *  \iparam  Params = Lifetime counter data structure
 *  \iparam  Channel = Channel of the current module instance
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

Error_t pressTimeResetPartition (const PressTimeParams_t *Params, UInt16 Channel)
{
    Error_t Status;
    
    if (Params->Handle >= 0) {               
        bmSignalEvent(Channel, I_PARTITION_RESET, 1, 0);
                        
        Status = bmResetStorageItems (Params->Handle, Params->PermDataTable, Params->NumberPumps + 1);
        return (Status);
    }
    return (E_STORAGE_OPEN_ERROR);
}


/******************************************************************************/
/*! 
 *  \brief   Verify module's data partition
 *
 *      Verifies the content of the data partition for plausibility. Each 
 *      data item must be inside the range defined for that item. If only
 *      a single data item is outside it's range, ALL data items are reset 
 *      to defaults. If the partition is ok, the partition layout version 
 *      is checked. If the layout version in the partition is older than 
 *      the one required by the actual software, data layout in the  
 *      partition will be converted. 
 * 
 *      If the layout version is zero, all data items in the partition
 *      are set to defaults as well. This may happen, if the partition
 *      was erased due to a checksum error or during manufacturing.
 * 
 *  \iparam  Params = Lifetime counter data structure
 *  \iparam  Channel = Channel of the current module instance
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t pressTimeVerifyPartition (const PressTimeParams_t *Params, UInt16 Channel)
{
    Error_t Error;
    UInt32 LayoutVersion;
        
    if (Params->Handle >= 0) {
        if (!bmVerifyStorageItems (Params->Handle, Params->PermDataTable, Params->NumberPumps + 1)) {
            bmSignalEvent (Channel, E_PERSISTENTS_INVALID, 1, 0);            
            Error = pressTimeResetPartition (Params, Channel);
            if (Error < NO_ERROR) {
                return (Error);
            }
        }
        LayoutVersion = bmGetStorageItem (Params->Handle, PARAM_LAYOUT_VERSION, 0);       

        // if required, upgrade partition to new layout format
        if (LayoutVersion < PARTITION_VERSION) {
            // currently no conversion required
            bmSignalEvent (Channel, I_PARTITION_CONVERTED, 1, LayoutVersion);                                
        }
        return (NO_ERROR);
    }
    return (E_STORAGE_OPEN_ERROR);        
}


/*****************************************************************************/
/*! 
 *  \brief   Initializes heating element life cycle counters
 *
 *      This function opens and initializes the partition holding the
 *      lifecycle counter stored on non-volatile memory.
 * 
 *  \iparam  ModuleID = ID of the module
 *  \iparam  Instance = Number of the instance
 *  \oparam  Params = Lifetime counter data structure
 *  \oparam  NumberPumps = Number of heating elements
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t pressTimeInit (UInt16 ModuleID, UInt16 Instance, PressTimeParams_t *Params, UInt8 NumberPumps)
{
    UInt8 i;
    Error_t Error;
    UInt16 Size;
    
    Params->Start = GET_PARAM_SIZE (PARAM_LAYOUT_VERSION);
    Size = Params->Start + NumberPumps * sizeof(UInt32);
    
    printf("Press size:%d\n", Size);
    // Open storage
    Params->Handle = bmOpenPermStorage (ModuleID, Instance, Size);
    if (Params->Handle < 0) {
        return (Params->Handle);
    }
    
    // Allocate parameter ranges
    Params->PermDataTable = calloc (NumberPumps + 1, sizeof(bmParamRange_t));
    if (Params->PermDataTable == NULL) {
        return (E_MEMORY_FULL);
    }
    
    Params->PermDataTable[0].Address = PARAM_LAYOUT_VERSION;
    Params->PermDataTable[0].MinValue = 1;
    Params->PermDataTable[0].MaxValue = PARTITION_VERSION;
    Params->PermDataTable[0].Default = PARTITION_VERSION;
    
    for (i = 0; i < NumberPumps; i++) {
        Params->PermDataTable[i+1].Address = BUILD_PARAMETER(Params->Start + i * sizeof(UInt32), sizeof(UInt32));
        Params->PermDataTable[i+1].MinValue = 0;
        Params->PermDataTable[i+1].MaxValue = MAX_UINT32;
        Params->PermDataTable[i+1].Default = 0;
    }

    Params->NumberPumps = NumberPumps;
    
    Error = pressTimeVerifyPartition (Params, bmGetChannel(bmGetTaskID(ModuleID, Instance)));
    if (Error != NO_ERROR) {
        return Error;
    }
    
    return (Error);
}

/*****************************************************************************/
/*! 
 *  \brief   Reads a timestamp from non-volatile memory
 *
 *      This function reads a timestamp from a non-volatile memory. It
 *      includes one parameter, which is the index number of the value. The
 *      timestamp is then returned by this function. They are given in
 *      seconds.
 * 
 *  \iparam  Params = Lifetime counter data structure
 *  \iparam  Number = Number of the heating element
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

UInt32 pressTimeRead (const PressTimeParams_t *Params, UInt8 Number)
{
    return bmGetStorageItem (Params->Handle, BUILD_PARAMETER(Params->Start + Number * sizeof(UInt32), sizeof(UInt32)), 0);
}


/*****************************************************************************/
/*! 
 *  \brief   Reset a timestamp on non-volatile memory
 *
 *      This function resets a timestamp stored on non-volatile memory. It
 *      includes a parameter indicating the number of the heating element. The
 *      timestamp is given in seconds. This operation is necessary when one of
 *      the heating elements is exchanged.
 *
 *  \iparam  Params = Lifetime counter data structure
 *  \iparam  Number = Number of the heating element
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t pressTimeReset (const PressTimeParams_t *Params, UInt8 Number)
{
    return bmSetStorageItem (Params->Handle, BUILD_PARAMETER(Params->Start + Number * sizeof(UInt32), sizeof(UInt32)), 0);
}


/*****************************************************************************/
/*! 
 *  \brief   Updating a timestamp
 *
 *      This function adds a given offset in microseconds to the operating
 *      time of all heating element. When a full second is reached, this
 *      information is automatically stored in a non-volatile memory device.
 *
 *  \xparam  Params = Lifetime counter data structure
 *  \iparam  TimeMs = Pulse time in milliseconds
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t pressTimeUpdate (PressTimeParams_t *Params, UInt32 TimeMs)
{
    UInt8 i;
    Error_t Error;
    UInt32 Counter = 0;
    
    Params->TimeMs += TimeMs;
    
    while (Params->TimeMs >= 1000) {
        Counter++;
        Params->TimeMs -= 1000;
    }

    if (Counter != 0) {
        for (i = 0; i < Params->NumberPumps; i++) {
            Error = bmIncStorageItem (Params->Handle, BUILD_PARAMETER(Params->Start + i * sizeof(UInt32), sizeof(UInt32)), Counter);
            if (Error < NO_ERROR) {
                return (Error);
            }
        }
    }
    
    return (NO_ERROR);
}

//****************************************************************************/
