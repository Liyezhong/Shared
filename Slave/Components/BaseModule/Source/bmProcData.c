/****************************************************************************/
/*! \file bmProcData.c
 *
 *  \brief Process data handling module
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions handling process data for all
 *       function modules on this node. The function modules only have to
 *       deliver their process data values via their status function.
 *       The rest is done by this module.
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
#include "halLayer.h"
#include "bmError.h"
#include "bmTime.h"
#include "bmScheduler.h"
#include "bmUtilities.h"
#include "bmDispatch.h"
#include "bmSystem.h"
#include "bmCan.h"
#include "bmProcData.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define MODE_TYPE_BITMASK     3   //!< Bitmask to select the mode
#define MODE_CLOCK_SOURCE     8   //!< Bitmask to select clock source

#define MODE_DISABLED         0   //!< Mode: disabled
#define MODE_EVENT_DRIVEN     1   //!< Mode: enable event driven sending
#define MODE_TIME_DRIVEN      2   //!< Mode: enable time driven sending
#define MODE_POLLING          3   //!< Mode: enable polling

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Contains all variables for a single module instance
typedef struct {
    UInt16 Mode;                  //!< Mode of sending
    UInt16 Interval;              //!< Interval time
    UInt16 Threshold;             //!< Min delta before sending
    UInt32 TimeStamp;             //!< Time of last state change
    UInt32 LastValue;             //!< Last sent data value
} bmProcessData_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

static bmProcessData_t *DataTable; //!< Data table for all tasks
static UInt16 DataTableSize = 0;   //!< Size of data table

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t bmConfigureProcessData (UInt16 Channel, CanMessage_t *Message);
static Error_t bmRequestProcessData   (UInt16 Channel, CanMessage_t *Message);
static Error_t bmSendProcessData      (UInt16 Channel, UInt16 Value);
static UInt32  bmProcessTimeExpired   (bmProcessData_t *Data);
static UInt32  bmGetProcessTime       (bmProcessData_t *Data);


/*****************************************************************************/
/*!
 *  \brief   Monitor process data task
 *
 *      This function is called by the task scheduler on a regular base.
 *      It checks if monitoring of process data for this module instance
 *      is enabled and, if this is the case, checks if a new data value
 *      has to be sent. Depending on the mode setting data will be sent
 *      if one of the following conditions is met:
 *
 *      - data change is greater than a configured threshold
 *      - a configured time span has elapsed
 *
 *      Time spans can be in timer ticks (ms) or system clock ticks.
 *
 *  \iparam  Channel     = Logical channel number
 *  \iparam  ModuleState = State of module
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmMonitorProcessData (UInt16 Channel, bmModuleState_t ModuleState) {

    bmProcessData_t *Data = &DataTable[Channel];
    UInt16 Mode = Data->Mode & MODE_TYPE_BITMASK;
    UInt16 Value;

    if (Channel >= DataTableSize) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (Mode != MODE_DISABLED) {
        if (ModuleState != MODULE_STATE_DISABLED) {

            // check if time since last sending exceeds specified interval
            if (bmProcessTimeExpired(Data) >= Data->Interval) {

                Value = bmGetModuleStatus (Channel, MODULE_STATUS_VALUE, 0);

                if (Mode == MODE_EVENT_DRIVEN) {
                    if (bmGetDelta (Value, Data->LastValue) >= Data->Threshold) {
                        bmSendProcessData (Channel, Value);
                    }
                }
                else if (Mode == MODE_TIME_DRIVEN) {
                    bmSendProcessData (Channel, Value);
                }
            }
        }
        else {
            Data->Mode = MODE_DISABLED;
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Send message containing process data
 *
 *      Sends supplied data value as a process data message via the
 *      specified "Channel". Furthermore, the time of sending and the
 *      last sent value in the process data structure are updated.
 *      The message contains the process data value.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Value   = Process data value
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendProcessData (UInt16 Channel, UInt16 Value) {

    bmProcessData_t *Data = &DataTable[Channel];
    CanMessage_t Message;

    if (Channel < DataTableSize) {

        Message.CanID = MSG_PROC_DATA;
        bmSetMessageItem (&Message, Value, 0, 2);
        Message.Length = 2;

        Data->TimeStamp = bmGetProcessTime(Data);
        Data->LastValue = Value;

        return (canWriteMessage(Channel, &Message));
    }
    return (E_PARAMETER_OUT_OF_RANGE);
}


/*****************************************************************************/
/*!
 *  \brief   Request the digital ports input value
 *
 *      This function is called by the CAN message dispatcher when a data
 *      request message is received from the master. As a response a data
 *      message is sent back to the master. Requesting data is always
 *      possible, independent of the mode selected, as long as process
 *      data collection is not disabled.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmRequestProcessData (UInt16 Channel, CanMessage_t *Message) {

    bmProcessData_t *Data = &DataTable[Channel];
    Error_t Value;

    if (Channel >= DataTableSize) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if ((Data->Mode & MODE_TYPE_BITMASK) != MODE_DISABLED) {

        Value = bmGetModuleStatus (Channel, MODULE_STATUS_VALUE, 0);
        if (Value < 0) {
            return (Value);
        }
        return (bmSendProcessData (Channel, (UInt16)Value));
    }
    return (E_PROCESS_DATA_DISABLED);
}


/*****************************************************************************/
/*!
 *  \brief   Process data configuration message handler
 *
 *      This function is called by the message dispatcher, when a process
 *      data configuration message from the master is received. The message
 *      contains the following informations:
 *
 *      - Mode
 *      - Send interval
 *      - Hysteresis (threshold)
 *
 *      Mode is used to define the mode of operation. The following modes
 *      can be selected via mode byte:
 *
 *      - Activate/Deactivate
 *      - Time driven sending
 *      - Event driven sending
 *      - Send on polling
 *
 *      Time and event driven modes can be either synchronous (uses
 *      system clock) or asynchronous (uses internal timer).
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmConfigureProcessData (UInt16 Channel, CanMessage_t *Message) {

    bmProcessData_t *Data = &DataTable[Channel];

    if (Channel >= DataTableSize) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    // Check if module can deliver process data
    if (bmGetModuleStatus (Channel, MODULE_STATUS_VALUE, 0) < 0) {
        return (E_PROCESS_DATA_UNSUPPORTED);
    }
    // Extract configuration from message
    if (Message->Length >= 5) {
        Data->Mode      = bmGetMessageItem(Message, 0, 1);
        Data->Interval  = bmGetMessageItem(Message, 1, 2);
        Data->Threshold = bmGetMessageItem(Message, 3, 2);
        Data->TimeStamp = 0;

        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Get actual time
 *
 *      This function returns the actual time and is used to handle timed
 *      process data sending. The time base used depends on the "Mode"
 *      setting in the supplied "Data" structure. It can be eigther the
 *      internal time base (ms) or the system wide sysclock.
 *
 *  \iparam  Data = Pointer to task's process data
 *
 *  \return  Actual time/tick value
 *
 ****************************************************************************/

static UInt32 bmGetProcessTime (bmProcessData_t *Data) {

    if (Data->Mode & MODE_CLOCK_SOURCE) {
        return (bmGetSysClock());
    }
    return (bmGetTime());
}


/*****************************************************************************/
/*!
 *  \brief   Get time expired
 *
 *      This function returns time expired since setting the "TimeStamp"
 *      in the data structure pointed to by "Data". The time base used
 *      depends on the "Mode" setting in the supplied "Data" structure.
 *      It can be either the internal time base (ms) or the system
 *      wide sysclock.
 *
 *  \iparam  Data = Pointer to task's process data
 *
 *  \return  Actual time/tick value
 *
 ****************************************************************************/

static UInt32 bmProcessTimeExpired (bmProcessData_t *Data) {

    if (Data->Mode & MODE_CLOCK_SOURCE) {
        return (bmSysClockExpired(Data->TimeStamp));
    }
    return (bmTimeExpired(Data->TimeStamp));
}


/*****************************************************************************/
/*!
 *  \brief   Initializes this module
 *
 *      Initializes this module by registering the process data configuration
 *      message for all modules to the message dispatcher. Furthermore, the
 *      storage for the per-task process data structure is allocated.
 *
 *      This function is called only once during startup.
 *
 *  \iparam  TaskCount = Number of tasks
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmInitProcessData (UInt16 TaskCount) {

    static bmCallbackEntry_t Commands[] = {
        { MSG_PROC_CONFIGURE,  bmConfigureProcessData },
        { MSG_PROC_REQ_DATA,   bmRequestProcessData   }
    };

    // allocate process data table
    DataTable = calloc (TaskCount, sizeof(bmProcessData_t));
    if (DataTable == NULL) {
        return (E_MEMORY_FULL);
    }
    DataTableSize = TaskCount;

    return (canRegisterBroadcast (Commands, ELEMENTS(Commands)));
}

//****************************************************************************/
