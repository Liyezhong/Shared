//*****************************************************************************/
/*! \file fmAnaOutput.c
 *
 *  \brief Function Module for Analog Outputs
 *
 *   $Version: $ 0.2
 *   $Date:    $ 27.01.2011
 *   $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions to control and write analog
 *       output channels. It supports enhanced features like timed output
 *       including delays and durations.
 *
 *       If a delay time is specified in an output command, the new output
 *       value is applied not before the delay time has expired. If a
 *       duration time is specified, the new output becomes active for
 *       the specified duration and falls back to the previous value
 *       when the time has expired. Delay and duration can be combined.
 *
 *       The function module uses logical channels; the mapping to a real
 *       hardware channel is done by the hardware abstraction layer. The
 *       width of an output channel is generally 16 bits and is scaled
 *       to the physical resolution by the hardware abstraction layer.
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 ******************************************************************************/

#include <stdlib.h>
#include "Global.h"
#include "Basemodule.h"
#include "fmAnaOutput.h"


//*****************************************************************************/
// Private Constants and Macros
//*****************************************************************************/

#define MODULE_VERSION           0x0001  //!< Software version number
#define MAX_PORT_VALUE           0xFFFF  //!< Maximal analog port value
#define MIN_PORT_WIDTH           1       //!< Minimal analog port width
#define MAX_PORT_WIDTH           16      //!< Maximal analog port width

/*! Defines for the Mode member of the module instance data */
#define MODE_MODULE_ENABLE       0x80    //!< Module enabled
#define MODE_OFF_AT_SHUTDOWN     0x40    //!< Set output off on shutdown
#define MODE_OFF_AT_NOTSTOP      0x20    //!< Set output off on notstop

/*! Defines for the Flags member of the module instance data */
#define FLAG_DATA_MODE           0x03    //!< Data mode bitmask
#define FLAG_DATA_VALUE          0x00    //!< Data mode: immediate value
#define FLAG_DATA_INCREMENT      0x01    //!< Data mode: increment by value
#define FLAG_DATA_DECREMENT      0x02    //!< Data mode: decrement by value
#define FLAG_DATA_BELOW_MAX      0x03    //!< Data mode: offset from maximum

#define FLAG_CLOCK_SOURCE        0x10    //!< Clock source select
#define FLAG_USE_NOTIFICATION    0x40    //!< Enable notification
#define FLAG_NEW_DATA            0x8000  //!< New data received

#define LIFETIME_UPDATE_INTERVAL 5000    //!< Lifetime update interval

#define OPTION_LIFETIME_DATA     1       //!< Enable collection of lifetime data

//@{ Start of doxygen group
//!  Definition of non-volatile item used by function module
#define PARAM_LAYOUT_VERSION     BUILD_PARAMETER(0,2)
#define PARAM_LIFE_TIME          BUILD_PARAMETER(2,4)
#define PARAM_LIFE_CYCLES        BUILD_PARAMETER(6,4)
#define PARAM_TOTAL_SIZE         16
//@} End of doxygen group

#define PARTITION_VERSION        1       //!< Partition layout version


//*****************************************************************************/
// Private Type Definitions
//*****************************************************************************/

//! Output task state maschine states
typedef enum {
    STATE_IDLE,                 //!< Wait for new output request
    STATE_DELAY,                //!< Wait 'till output delay time expired
    STATE_DURATION,             //!< Wait 'till output duration expired
    STATE_OUTPUT,               //!< Output new data value
    STATE_NOTIFY,               //!< Send "done" notification
    STATE_ABORT                 //!< Output aborted (force inactive)
} aoOutputState_t;

//! Contains all variables to maintain life cycle data
typedef struct {
    UInt16 Running:1;           //!< Life timer running flag
    UInt32 Counter;             //!< Number of on/off cycles
    UInt32 Duration;            //!< Accumulated life time
    UInt32 StartTime;           //!< Measurement start time
    UInt32 Interval;            //!< Storage update interval
} aoLifeTimeData_t;

//! Contains all variables for an instance of this module
typedef struct {
    bmModuleState_t ModuleState; //!< Module state
    aoOutputState_t State;      //!< Output state maschine state
    Handle_t Memory;            //!< Non-volatile storage handle
    Handle_t Port;              //!< Handle to access port (HAL)
    UInt16 Channel;             //!< Logical CAN channel
    UInt32 Options;             //!< Module instance board options

    UInt16 Mode;                //!< Mode bits (configuration message)
    UInt16 Flags;               //!< Flag bits (output message)
    UInt16 Delay;               //!< Output data delay time
    UInt16 Duration;            //!< Output data duration
    UInt32 StartTime;           //!< Start time of interval

    UInt16 BitCount;            //!< Resolution of output channel
    UInt16 BitShift;            //!< Number of bits to shift before output
    UInt16 NewValue;            //!< New (next) output value
    UInt16 CurValue;            //!< Current output value
    UInt16 OldValue;            //!< Previous output value
    UInt16 MaxValue;            //!< Maximum output value
    UInt16 OffValue;            //!< Output value for 'inactive'
    UInt16 OffLimit;            //!< Output threshold for 'inactive'

    aoLifeTimeData_t LifeTime;  //!< Life time parameters
} aoInstanceData_t;


//*****************************************************************************/
// Private Variables
//*****************************************************************************/

//! Persistent parameter descriptor table (used for verification)
static const bmParamRange_t PermDataTable[] = {
    { PARAM_LAYOUT_VERSION, 1, PARTITION_VERSION, PARTITION_VERSION },
    { PARAM_LIFE_TIME,      0, 0, 0 },
    { PARAM_LIFE_CYCLES,    0, 0, 0 }
};

static aoInstanceData_t *aoDataTable; //!< Data table for all instances
static UInt16 aoInstanceCount = 0;    //!< Number of module instances
static UInt16 aoModuleID = 0;         //!< Module identifier


//*****************************************************************************/
// Private Function Prototypes
//*****************************************************************************/

static Error_t aoModuleTask       (UInt16 Instance);
static Error_t aoModuleStatus     (UInt16 Instance, bmModuleStatusID_t StatusID);
static Error_t aoModuleControl    (UInt16 Instance, bmModuleControlID_t ControlID);

static Error_t aoChangeOutput     (aoInstanceData_t *Data, UInt16 Value);
static Error_t aoSendNotification (aoInstanceData_t *Data);
static Error_t aoGetModuleState   (aoInstanceData_t *Data);
static UInt32  aoTimeExpired      (aoInstanceData_t *Data);
static UInt32  aoGetTime          (aoInstanceData_t *Data);

static Error_t aoFlushLifeTime    (aoInstanceData_t *Data, UInt16 Interval);
static Error_t aoCountLifeTime    (aoInstanceData_t *Data, Bool State);
static Error_t aoVerifyPartition  (aoInstanceData_t *Data);
static Error_t aoResetPartition   (aoInstanceData_t *Data);

static Error_t aoConfigureOutput  (UInt16 Channel, CanMessage_t *Message);
static Error_t aoSetOutputState   (UInt16 Channel, CanMessage_t *Message);
static Error_t aoGetOutputState   (UInt16 Channel, CanMessage_t *Message);


/******************************************************************************/
/*!
 *  \brief   Function module control
 *
 *      This function is called by the base module to control this function
 *      module. Depending on the ControlID parameter, the following actions
 *      are performed:
 *
 *      - MODULE_CONTROL_STOP
 *      - MODULE_CONTROL_RESUME
 *      - MODULE_CONTROL_SHUTDOWN
 *      - MODULE_CONTROL_WAKEUP
 *      - MODULE_CONTROL_FLUSH_DATA
 *      - MODULE_CONTROL_RESET_DATA
 *      - MODULE_CONTROL_RESET
 *
 *  \iparam  Instance  = Instance number of this module
 *  \iparam  ControlID = Control code to select sub-function
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t aoModuleControl (UInt16 Instance, bmModuleControlID_t ControlID) {

    aoInstanceData_t *Data = &aoDataTable[Instance];

    switch (ControlID) {

        case MODULE_CONTROL_RESUME:
        case MODULE_CONTROL_WAKEUP:
            Data->ModuleState = MODULE_STATE_READY;
            break;

        case MODULE_CONTROL_STOP:
            if (Data->Mode & MODE_OFF_AT_NOTSTOP) {
                Data->State = STATE_ABORT;
            }
            Data->ModuleState = MODULE_STATE_STOPPED;
            break;

        case MODULE_CONTROL_SHUTDOWN:
            if (Data->Mode & MODE_OFF_AT_SHUTDOWN) {
                Data->Flags &= ~FLAG_USE_NOTIFICATION;
                Data->State = STATE_ABORT;
            }
            Data->ModuleState = MODULE_STATE_STANDBY;
            break;

        case MODULE_CONTROL_RESET:
            Data->Mode = Data->Flags = 0;
            break;

        case MODULE_CONTROL_FLUSH_DATA:
            return (aoFlushLifeTime(Data, 0));

        case MODULE_CONTROL_RESET_DATA:
            return (aoResetPartition(Data));

        default:
            return (E_PARAMETER_OUT_OF_RANGE);
    }
    return (NO_ERROR);
}


/******************************************************************************/
/*!
 *  \brief   Function module status request
 *
 *      This function is called by the base module to request the status of
 *      this function module. Depending on the StatusID parameter, the
 *      following status values are returned:
 *
 *      - MODULE_STATUS_INSTANCES
 *      - MODULE_STATUS_MODULE_ID
 *      - MODULE_STATUS_VERSION
 *      - MODULE_STATUS_STATE
 *      - MODULE_STATUS_VALUE
 *
 *  \iparam  Instance = Instance number of this module
 *  \iparam  StatusID = selects which status is requested
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t aoModuleStatus (UInt16 Instance, bmModuleStatusID_t StatusID) {

    aoInstanceData_t *Data = &aoDataTable[Instance];

    switch (StatusID) {
        case MODULE_STATUS_STATE:
            return (aoGetModuleState(Data));

        case MODULE_STATUS_VALUE:
            return (Data->CurValue);

        case MODULE_STATUS_MODULE_ID:
            return (aoModuleID);

        case MODULE_STATUS_INSTANCES:
            return (aoInstanceCount);

        case MODULE_STATUS_VERSION:
            return (MODULE_VERSION);
    }
    return (E_PARAMETER_OUT_OF_RANGE);
}


/******************************************************************************/
/*!
 *  \brief   Return actual module state
 *
 *      Returns the actual module state of the module instance.
 *      Possible states are:
 *
 *      - MODULE_STATE_IDLE
 *      - MODULE_STATE_BUSY
 *      - MODULE_STATE_STANDBY
 *      - MODULE_STATE_SHUTDOWN
 *      - MODULE_STATE_STOPPED
 *      - MODULE_STATE_DISABLED
 *
 *  \iparam  Data = Pointer to instance data variables
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t aoGetModuleState (aoInstanceData_t *Data) {

    if (Data->State != STATE_IDLE) {
        return (MODULE_STATE_BUSY);
    }
    if ((Data->Mode & MODE_MODULE_ENABLE) == 0) {
        return (MODULE_STATE_DISABLED);
    }
    return (Data->ModuleState);
}


/******************************************************************************/
/*!
 *  \brief   Module's task function
 *
 *      Updates the analog output according to the configured settings.
 *      Since output changes can't be processed always in one step, the
 *      function implements a state maschine. If a output delay time is
 *      configured, output is changed not before the delay time has
 *      expired. If a output duration is configured, the output remains
 *      at the output level only for the specified duration. If enabled,
 *      a notification message is send when the output has reached it's
 *      final state. Moreover, the life time counters are updated.
 *
 *      This function is called by the base module periodically.
 *
 *  \iparam  Instance = Module instance number
 *
 *  \return  Module instance state or (negative) error code
 *
 ******************************************************************************/

static Error_t aoModuleTask (UInt16 Instance) {

    aoInstanceData_t *Data = &aoDataTable[Instance];

    if ((Data->Mode & MODE_MODULE_ENABLE) || (Data->State != STATE_IDLE)) {

        switch (Data->State) {

            case STATE_IDLE:
                if (Data->Flags & FLAG_NEW_DATA) {
                    Data->State = Data->Delay ? STATE_DELAY : STATE_OUTPUT;
                    if (Data->NewValue != Data->CurValue) {
                        Data->OldValue = Data->CurValue;
                    }
                    Data->StartTime = aoGetTime(Data);
                }
                break;

            case STATE_DELAY:
                if (aoTimeExpired(Data) > Data->Delay) {
                    Data->StartTime = aoGetTime(Data);
                    Data->State = STATE_OUTPUT;
                }
                break;

            case STATE_DURATION:
                if (aoTimeExpired(Data) > Data->Duration) {
                    aoChangeOutput (Data, Data->OldValue);
                    Data->State = STATE_NOTIFY;
                }
                break;

            case STATE_NOTIFY:
                if (Data->Flags & FLAG_USE_NOTIFICATION) {
                    aoSendNotification (Data);
                }
                Data->Flags &= ~FLAG_NEW_DATA;
                Data->State = STATE_IDLE;
                break;

            case STATE_ABORT:
                aoChangeOutput (Data, Data->OffValue);
                Data->State =
                    (Data->Flags & FLAG_NEW_DATA) ? STATE_NOTIFY : STATE_IDLE;
                break;
        }
        if (Data->State == STATE_OUTPUT) {
            aoChangeOutput (Data, Data->NewValue);
            Data->State = Data->Duration ? STATE_DURATION : STATE_NOTIFY;
        }
        if (Data->Options & OPTION_LIFETIME_DATA) {
            aoFlushLifeTime(Data, LIFETIME_UPDATE_INTERVAL);
        }
    }
    return (aoGetModuleState(Data));
}


/******************************************************************************/
/*!
 *  \brief  Change analog output
 *
 *      Changes the analog output by writing the supplied 'Value' to the
 *      analog output port. The configured resolution bitmask is applied
 *      to the value before writing.
 *
 *      If life time counters are enabled, they are switched on or off,
 *      depending on Value. If Value is unequal the configured OFF-level,
 *      counting is started. In the opposite case counting is stopped.
 *
 *  \xparam  Data  = Module instance data pointer
 *  \iparam  Value = Value to write to the port
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t aoChangeOutput (aoInstanceData_t *Data, UInt16 Value) {

    Error_t Status;

    if ((Status = halAnalogWrite (Data->Port, Value << Data->BitShift)) < NO_ERROR) {
        return Status;
    } 
    Data->CurValue = Value;   
        
    if (Data->Options & OPTION_LIFETIME_DATA) {
        aoCountLifeTime (Data, Value > Data->OffLimit);
    }
    return (NO_ERROR);
}


/******************************************************************************/
/*!
 *  \brief   Set new analog output value
 *
 *      Extracts the output parameters from a output data message and
 *      sets the module instance data variables accordingly. The analog
 *      output is not modified directly in this function, but in the
 *      modules task function. The following parameters are taken from
 *      the output data message:
 *
 *      - Data value (16 bit)
 *      - Delay time (0 = no delay)
 *      - Duration time (0 = undefinitely)
 *      - Flag bits
 *
 *      The flag bits define the data mode (immediate value, increment,
 *      decrement, offset from maximum), notification enable, and time
 *      base select for delay/duration time (timer or system clock).
 *      If time base is the internal timer, times are in ms. If time
 *      base is system clock, times are in number of sysclock ticks.
 *      In case of increment/decrement the output is saturated to the
 *      minimal or maximal limit of the output range.
 *
 *      This function is called by the CAN message dispatcher whenever a
 *      output data message is received from the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t aoSetOutputState (UInt16 Channel, CanMessage_t *Message) {

    aoInstanceData_t *Data = &aoDataTable[bmGetInstance(Channel)];

    if (!(Data->Mode & MODE_MODULE_ENABLE)) {
        return (E_MODULE_NOT_ENABLED);
    }
    if (Message->Length >= 7) {

        UInt32 newDataValue = bmGetMessageItem(Message, 1, 2);
        if (newDataValue > Data->MaxValue) {
            return (E_PARAMETER_OUT_OF_RANGE);
        }
        Data->Flags    = bmGetMessageItem(Message, 0, 1);
        Data->Duration = bmGetMessageItem(Message, 3, 2);
        Data->Delay    = bmGetMessageItem(Message, 5, 2);

        switch (Data->Flags & FLAG_DATA_MODE) {

            case FLAG_DATA_INCREMENT:
                newDataValue = Data->NewValue + newDataValue;
                break;

            case FLAG_DATA_DECREMENT:
                newDataValue = Data->NewValue - newDataValue;
                break;

            case FLAG_DATA_BELOW_MAX:
                newDataValue = Data->MaxValue - newDataValue;
                break;
        }
        // saturate/limit output to min/max value
        if ((Int32)newDataValue > Data->MaxValue) {
            newDataValue = Data->MaxValue;
        }
        else if ((Int32)newDataValue < 0) {
            newDataValue = 0;
        }
        Data->NewValue = newDataValue;

        Data->Flags |= FLAG_NEW_DATA;
        Data->State = STATE_IDLE;
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/******************************************************************************/
/*!
 *  \brief   Configure analog output
 *
 *      Extracts general configuration options for the analog output from
 *      a received configuration message and sets the module instance data
 *      structure accordingly. The following parameters are taken from the
 *      configuration message:
 *
 *      - Mode of operation
 *      - Resolution of output (number of bits)
 *      - Output value for 'Inactive' state
 *      - Output threshold for 'Inactive' (life time counting)
 *
 *      The Inactive value is used for setting the output OFF in case of
 *      Notstop, Standby or Shutdown (what can be individually selected
 *      via Mode parameter). The ouput threshold is used to distinguish
 *      off state from on state for life time counting. All values equal
 *      or smaller than this limit are interpreted as 'off'.
 *
 *      The resolution (bit count) is used to calculate a shift count,
 *      which is applied to each output value before writing it to the
 *      analog output port.
 *
 *      This function is called by the CAN message dispatcher when an
 *      configuration message is received from the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t aoConfigureOutput (UInt16 Channel, CanMessage_t *Message) {

    aoInstanceData_t *Data = &aoDataTable[bmGetInstance(Channel)];

    if (Data->Port < 0) {
        return (E_MODULE_NOT_USEABLE);
    }
    if (Message->Length >= 6) {

        Data->Mode     = bmGetMessageItem(Message, 0, 1);
        Data->BitCount = bmGetMessageItem(Message, 1, 1);
        Data->OffValue = bmGetMessageItem(Message, 2, 2);
        Data->OffLimit = bmGetMessageItem(Message, 4, 2);

        if (Data->BitCount <  MAX_PORT_WIDTH &&
            Data->BitCount >= MIN_PORT_WIDTH) {
            Data->MaxValue = ~(0xFFFFUL << Data->BitCount);
        }
        else {
            Data->BitCount = MAX_PORT_WIDTH;
            Data->MaxValue = MAX_PORT_VALUE;
        }
        Data->BitShift = MAX_PORT_WIDTH - Data->BitCount;

        if (Data->OffValue > Data->MaxValue ||
            Data->OffLimit > Data->MaxValue) {

            if (Data->OffValue > Data->MaxValue) {
                Data->OffValue = Data->MaxValue;
            }
            if (Data->OffLimit > Data->MaxValue) {
                Data->OffLimit = Data->MaxValue;
            }
            Data->Mode = ~MODE_MODULE_ENABLE;
            return (E_PARAMETER_OUT_OF_RANGE);
        }
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/******************************************************************************/
/*!
 *  \brief   Send output status notification
 *
 *      Sends an output status notification message to the master. The
 *      status message includes the following informations:
 *
 *      - Phase (Idle, Delay, Duration, Abort, Notification)
 *      - Actual output value/state
 *      - Expired delay time
 *      - Expired duration time
 *
 *      The actual output value represents the analog output without
 *      considering the invert bitmask. The phase indicates, if the
 *      last output command has been completed or is still under
 *      procession. In the latter case, it indicates the output phase,
 *      i.e. delay, duration, aborting, or notification.
 *
 *  \iparam  Data = Module instance data pointer
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t aoSendNotification (aoInstanceData_t *Data) {

    CanMessage_t Message;
    UInt16 Expired;

    if (Data->Mode & MODE_MODULE_ENABLE) {

        Message.CanID = MSG_AO_OUTPUT_STATE;
        bmSetMessageItem (&Message, Data->State, 0, 1);
        bmSetMessageItem (&Message, Data->CurValue, 1, 2);

        if (Data->State == STATE_DURATION) {
            Expired = aoTimeExpired(Data);
        }
        else if (Data->State > STATE_DURATION) {
            Expired = Data->Duration;
        }
        else {
            Expired = 0;
        }
        bmSetMessageItem (&Message, Expired, 3, 2);

        if (Data->State == STATE_DELAY) {
            Expired = aoTimeExpired(Data);
        }
        else if (Data->State > STATE_DELAY) {
            Expired = Data->Delay;
        }
        else {
            Expired = 0;
        }
        bmSetMessageItem (&Message, Expired, 5, 2);
        Message.Length = 7;

        return (canWriteMessage(Data->Channel, &Message));
    }
    return (E_MODULE_NOT_CONFIGURED);
}


/******************************************************************************/
/*!
 *  \brief   Request output status notification
 *
 *      This function is called by the CAN message dispatcher wheneven an
 *      output status request message is received from the master. It
 *      calls the aoSendNotification function to send the actual output
 *      status to the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t aoGetOutputState (UInt16 Channel, CanMessage_t *Message) {

    aoInstanceData_t *Data = &aoDataTable[bmGetInstance(Channel)];

    return (aoSendNotification (Data));
}


/******************************************************************************/
/*!
 *  \brief   Get actual time
 *
 *      This function returns the actual time and is used to handle timed
 *      outputs (delay/duration). The time base used depends on the "Flags"
 *      variable in the supplied "Data" structure. It can be either the
 *      internal time base (ms) or the system wide sysclock.
 *
 *  \iparam  Data = Module instance data pointer
 *
 *  \return  Actual time/tick value
 *
 ******************************************************************************/

static UInt32 aoGetTime (aoInstanceData_t *Data) {

    if (Data->Flags & FLAG_CLOCK_SOURCE) {
        return (bmGetSysClock());
    }
    return (bmGetTime());
}


/******************************************************************************/
/*!
 *  \brief   Get time expired
 *
 *      This function returns time expired since setting the "StartTime"
 *      in the data structure pointed to by "Data". The time base used
 *      depends on the Flags variable in the supplied "Data" structure.
 *      It can be either the internal time base (ms) or the system
 *      wide sysclock.
 *
 *  \iparam  Data = Module instance data pointer
 *
 *  \return  Actual time/tick value
 *
 ******************************************************************************/

static UInt32 aoTimeExpired (aoInstanceData_t *Data) {

    if (Data->Flags & FLAG_CLOCK_SOURCE) {
        return (bmSysClockExpired(Data->StartTime));
    }
    return (bmTimeExpired(Data->StartTime));
}


/******************************************************************************/
/*!
 *  \brief   Control life time counters
 *
 *      Updates life cycle counters in the given instance data structure.
 *      Counters are not directly written to non-volatile storage, because
 *      this is a more or less expensive task. Instead of this, they are
 *      maintained in the instance's data structure and written to non-
 *      volatile storage in larger time intervals.
 *
 *      Life time data consists of:
 *
 *      - Total duration the output was in ON state
 *      - Number of times the output was switched form OFF to ON
 *
 *      The counters in the instance's data structure contain the counts
 *      since last updating life time data in non-volatile storage. Each
 *      time non-volatile storage is updated, the counters start counting
 *      with zero. Time measurement is performed in milliseconds.
 *
 *      Counters start when this function is called with State set to TRUE.
 *      Counters are stopped, when called with State set to FALSE.
 *
 *  \xparam  Data  = Module instance data pointer
 *  \iparam  OnOff = Switch counters on (true) or off (false)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t aoCountLifeTime (aoInstanceData_t *Data, Bool OnOff) {

    if (Data->ModuleState != MODULE_STATE_STANDBY) {

        aoLifeTimeData_t *LifeTime = &Data->LifeTime;

        if (LifeTime->Running) {
            LifeTime->Duration += bmTimeExpired(LifeTime->StartTime);
        }
        if (OnOff) {
            LifeTime->StartTime = bmGetTime();

            if (!LifeTime->Running) {
                LifeTime->Counter++;
            }
        }
        //dbgPrint("LifeTimeCounter = %s", OnOff ? "ON" : "OFF");
        LifeTime->Running = OnOff;
    }
    return (NO_ERROR);
}


/******************************************************************************/
/*!
 *  \brief   Flush life time data to storage
 *
 *      Updates the life cycle counters in non-volatile storage, i.e.:
 *
 *      - Number of seconds the output was in ON state
 *      - Number of times the output was switched form off to on
 *
 *      This is done by incrementing the life cycle counter in non-volatile
 *      storage by the number of seconds expired since last updating. If
 *      succeeded, the counters-since-last-update are reset to zero.
 *
 *      Time measurement is done in milliseconds, but converted to
 *      seconds before saving it into non-volatile memory.
 *
 *  \xparam  Data     = Module instance data pointer
 *  \iparam  Interval = Interval time to flush data [ms]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t aoFlushLifeTime (aoInstanceData_t *Data, UInt16 Interval) {

    aoLifeTimeData_t *LifeTime = &Data->LifeTime;
    Error_t Status1 = NO_ERROR;
    Error_t Status2 = NO_ERROR;

    if (bmTimeExpired(LifeTime->Interval) >= Interval) {

        // update life time on/off cycles in non-volatile storage
        if (LifeTime->Counter) {

            Status1 = bmIncStorageItem (
                Data->Memory, PARAM_LIFE_CYCLES, LifeTime->Counter);

            if (Status1 == NO_ERROR) {
                LifeTime->Counter = 0;
            }
        }
        // update life time duration in non-volatile storage
        if (LifeTime->Duration) {
            Status2 = bmIncStorageItem (
                Data->Memory, PARAM_LIFE_TIME, LifeTime->Duration / 1000);

            if (Status2 == NO_ERROR) {
                LifeTime->Duration %= 1000;
            }
        }
        Data->LifeTime.Interval = bmGetTime();
    }
    return (Status1 == NO_ERROR ? Status2 : Status1);
}


/******************************************************************************/
/*!
 *  \brief   Send life time data
 *
 *      Sends the life time data to the master. Life time data consists of:
 *
 *      - Number of off-to-on transitions
 *      - Total on-times (minutes)
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request life time data" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t aoSendLifeTimeData (UInt16 Channel, CanMessage_t *Message) {

    aoInstanceData_t *Data = &aoDataTable[bmGetInstance(Channel)];
    CanMessage_t Response;

    if (Data->Memory >= 0) {

        UInt32 LifeTime =
            bmGetStorageItem (Data->Memory, PARAM_LIFE_TIME, 0);
        UInt32 LifeCycles =
            bmGetStorageItem (Data->Memory, PARAM_LIFE_CYCLES, 0);

        Response.CanID = MSG_AO_LIFETIME_DATA;
        bmSetMessageItem (&Response, LifeTime / 60, 0, 4);
        bmSetMessageItem (&Response, LifeCycles, 4, 4);
        Response.Length = 8;

        return (canWriteMessage(Channel, &Response));
    }
    return (E_STORAGE_OPEN_ERROR);
}


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
 *  \xparam  Data = Module instance data pointer
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t aoResetPartition (aoInstanceData_t *Data) {

    Error_t Status;

    if (Data->Memory >= 0) {

        bmSignalEvent(Data->Channel, I_PARTITION_RESET, 1, 0);
        Data->LifeTime.Counter = Data->LifeTime.Duration = 0;

        Status = bmResetStorageItems (
            Data->Memory, PermDataTable, ELEMENTS(PermDataTable));

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
 *  \xparam  Data = Module instance data pointer
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t aoVerifyPartition (aoInstanceData_t *Data) {

    UInt32 LayoutVersion;

    if (Data->Memory >= 0) {

        if (!bmVerifyStorageItems (
                Data->Memory, PermDataTable, ELEMENTS(PermDataTable))) {

            bmSignalEvent(Data->Channel, E_PERSISTENTS_INVALID, 1, 0);
            aoResetPartition(Data);
        }
        LayoutVersion =
            bmGetStorageItem(Data->Memory, PARAM_LAYOUT_VERSION, 0);

        // if required, upgrade partition to new layout format
        if (LayoutVersion < PARTITION_VERSION) {
            // currently no conversion required
            bmSignalEvent(
                Data->Channel, I_PARTITION_CONVERTED, 1, LayoutVersion);
        }
        return (NO_ERROR);
    }
    return (E_STORAGE_OPEN_ERROR);
}


/******************************************************************************/
/*!
 *  \brief   Initializes this function module
 *
 *      Initializes this function module by registering it to the task
 *      scheduler and registers all handler functions to the CAN message
 *      dispatcher. Furthermore, it allocates and initializes memory for
 *      the variables for all module instances. In addition, following
 *      steps are performed for each instance of the module:
 *
 *      - Open analog output
 *      - Open and verify instance's data partition (storage)
 *      - Requests the instance's board options
 *
 *      This function is called once during startup by the base module.
 *
 *  \iparam  ModuleID  = Module identifier
 *  \iparam  Instances = Number of instances of this module
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

Error_t aoInitializeModule (UInt16 ModuleID, UInt16 Instances) {

    static bmCallbackEntry_t Commands[] = {
        { MSG_AO_SET_OUTPUT_STATE,  aoSetOutputState   },
        { MSG_AO_REQ_OUTPUT_STATE,  aoGetOutputState   },
        { MSG_AO_REQ_LIFETIME_DATA, aoSendLifeTimeData },
        { MSG_AO_CONFIG_OUTPUT,     aoConfigureOutput  }
    };
    static bmModuleInterface_t Interface = {
        aoModuleTask, aoModuleControl, aoModuleStatus
    };
    Error_t Status;
    UInt16 i;

    // allocate module instances data storage
    aoDataTable = calloc (Instances, sizeof(aoInstanceData_t));
    if (aoDataTable == NULL) {
        return (E_MEMORY_FULL);
    }
    // register function module to the scheduler
    Status = bmRegisterModule (ModuleID, Instances, &Interface);
    if (Status < 0) {
        return (Status);
    }
    // registers text strings for debug console
    #ifdef DEBUG
    aoRegisterDebugNames(ModuleID);
    #endif

    // open channel, port and partition for each module instance
    for (i=0; i < Instances; i++) {

        aoDataTable[i].Port =
            halAnalogOpen(HAL_ANALOG_OUTPUTS + i, HAL_OPEN_WRITE, 0, NULL);
        aoDataTable[i].Options =
            bmGetBoardOptions (ModuleID, i, OPTION_LIFETIME_DATA);
        aoDataTable[i].Channel =
            bmGetChannel (bmGetTaskID(ModuleID, i));
        aoDataTable[i].Memory =
            bmOpenPermStorage (ModuleID, i, PARAM_TOTAL_SIZE);

        aoDataTable[i].ModuleState = MODULE_STATE_READY;
        aoVerifyPartition(&aoDataTable[i]);
    }
    aoInstanceCount = Instances;
    aoModuleID = ModuleID;

    return (canRegisterMessages (ModuleID, Commands, ELEMENTS(Commands)));
}

/******************************************************************************/
