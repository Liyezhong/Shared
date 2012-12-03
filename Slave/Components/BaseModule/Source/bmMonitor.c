/****************************************************************************/
/*! \file bmMonitor.c
 *
 *  \brief Power monitoring functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions to monitor power supply on the
 *       slave. This includes the supply voltage and the maximum allowed
 *       supply current of the board.
 *
 *       If voltage falls below a minimal level, or current exceeds a max.
 *       level, an error message is send to the master. Furthermode, the
 *       master can request the actual voltage and current.
 *
 *       The thresholds for voltage and current are configurable by
 *       the HAL layer (constants), the board option store (board
 *       specific) or can be configured via CAN bus.
 *
 *       Monitoring is possible only if the hardware offers analog
 *       inputs delivering values proportional to the voltage/current.
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
#include "halLayer.h"
#include "bmCommon.h"
#include "bmCan.h"
#include "bmError.h"
#include "bmBlink.h"
#include "bmTime.h"
#include "bmScheduler.h"
#include "bmSystem.h"
#include "bmStorage.h"
#include "bmDispatch.h"
#include "bmUtilities.h"
#include "bmMonitor.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define MONITOR_MODE_ENABLE    0x80  //!< Enable monitoring (configuration)

#define DEFAULT_FILTER_COUNT   5     //!< Default filter coefficient
#define DEFAULT_SAMPLE_RATE    5     //!< Default sample rate


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Data structure to hold monitoring variables
typedef struct {
    bmPowerState_t State;       //!< Actual power state
    UInt16 Enabled:1;           //!< Power monitor enable
    UInt16 ErrFlag:1;           //!< Error signaled (flag)
    UInt16 Filter:8;            //!< Number of samples to average over
    UInt16 SampleRate;          //!< Sample rate (ms)
    UInt32 SampleTime;          //!< Time of last sampling
    Int16 Threshold1;           //!< Lower analog input threshold
    Int16 Threshold2;           //!< Upper analog input threshold
    Int16 History[16];          //!< History of input values (for filtering)
    UInt16 InCount;             //!< Number of samples in history buffer
    UInt16 NextIn;              //!< Next-in pointer into history buffer
    Int16 Value;                //!< Filtered input value
    UInt16 Failures;            //!< Number of detected fails
    Handle_t Handle;            //!< Handle for input channel
} bmPowerMonitorState_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Supply voltage and safe monitor states
static bmPowerMonitorState_t Voltage[2] = { { POWER_GOOD, 0 }, { POWER_GOOD, 0 } }; 

//! Supply current monitor state
static bmPowerMonitorState_t Current = { POWER_GOOD, 0 };

//! Actual main power state (received from master)
static bmPowerState_t MainPowerState = POWER_GOOD;


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t bmConfigVoltageMonitor (UInt16 Channel, CanMessage_t *Message);
static Error_t bmConfigCurrentMonitor (UInt16 Channel, CanMessage_t *Message);
static Error_t bmConfigSafeMonitor    (UInt16 Channel, CanMessage_t *Message);
static Error_t bmSendVoltageState     (UInt16 Channel, CanMessage_t *Message);
static Error_t bmSendCurrentState     (UInt16 Channel, CanMessage_t *Message);
static Error_t bmSendSafeState        (UInt16 Channel, CanMessage_t *Message);
static Error_t bmGetFilteredInput     (bmPowerMonitorState_t *Monitor);

static Error_t bmConfigPowerMonitor (bmPowerMonitorState_t *Monitor, CanMessage_t *Message);

static bmPowerState_t bmGetSupplyVoltageState (UInt8 Index);
static bmPowerState_t bmGetSupplyCurrentState (void);

static void bmInitVoltageMonitorIndex (UInt8 Index);
static Error_t bmMonitorSupplyVoltageIndex (UInt8 Index);


/*****************************************************************************/
/*!
 *  \brief   Get actual power supply status
 *
 *      Returns the actual power supply voltage status, which can be:
 *
 *      - Power good
 *      - Power warning (not really good, but still acceptable)
 *      - Power fail
 *      - Power unknown
 *
 *      It takes into account the local voltage monitor (if supported by
 *      hardware), and the remote power status signalled by the master.
 *      POWER_UNKNOWN is returned, if the voltage can't be determined.
 *
 *  \return  Power health status
 *
 ****************************************************************************/

bmPowerState_t bmGetPowerSupplyStatus (void) {

    if (Voltage[0].State < MainPowerState) {
        return (MainPowerState);
    }
    return (Voltage[0].State);
}


/*****************************************************************************/
/*!
 *  \brief   Get actual safe supply status
 *
 *      Returns the actual safe supply voltage status, which can be:
 *
 *      - Power good
 *      - Power warning (not really good, but still acceptable)
 *      - Power fail
 *      - Power unknown
 *
 *      It takes into account the local safe voltage monitor (if supported 
 *      by hardware), and the remote power status signalled by the master.
 *      POWER_UNKNOWN is returned, if the voltage can't be determined.
 *
 *  \return  Power health status
 *
 ****************************************************************************/

bmPowerState_t bmGetSafeSupplyStatus (void) {

    if (Voltage[1].State < MainPowerState) {
        return (MainPowerState);
    }
    return (Voltage[1].State);
}


/*****************************************************************************/
/*!
 *  \brief   Set master power health status
 *
 *      Evaluates the received power status message and sets the global
 *      MainPowerState accordingly. The power status message contains
 *      a power state enumeration, which represents power good, power
 *      warning or power fail status.
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "set main power status" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSetMasterPowerState (UInt16 Channel, CanMessage_t *Message) {

    bmPowerState_t PowerState;

    if (Message->Length >= 1) {

        PowerState = (bmPowerState_t) bmGetMessageItem(Message, 0, 1);
        if (PowerState > POWER_FAILED) {
            return (E_PARAMETER_OUT_OF_RANGE);
        }
        if (PowerState == POWER_FAILED) {
            bmSetGlobalControl (MODULE_CONTROL_FLUSH_DATA);
            bmWriteProtectStorage(PROTECT_BY_USV, TRUE);
        }
        else {
            bmWriteProtectStorage(PROTECT_BY_USV, FALSE);
        }
        MainPowerState = PowerState;

        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Monitor board supply voltage by index
 *
 *      Monitors the board supply or safe voltage according to the passed index.
 *      If the voltage falls below the "fail" threshold, an error is signalled 
 *      and a blink code displayed on the error LED. If the voltage rises above 
 *      "good" level, the error will be annulled and the blinking switched off. 
 *      The number of voltage fails is counted. If voltage can not be determined,
 *      an error is signalled and the voltage monitor disabled. Voltage
 *      is sampled in a configurable time interval.
 *
 *      Non-volatile storage gets write protected as long as voltage is
 *      below "fail" threshold.
 *
 *  \iparam Index = Number of the voltage monitor
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmMonitorSupplyVoltageIndex (UInt8 Index) {

    if (Voltage[Index].Enabled || Voltage[Index].ErrFlag) {
        if (bmTimeExpired(Voltage[Index].SampleTime) > Voltage[Index].SampleRate) {

            bmPowerState_t State =
                Voltage[Index].Enabled ? bmGetSupplyVoltageState(Index) : POWER_GOOD;

            if (State != Voltage[Index].State) {

                if (State == POWER_GOOD) {
                    if (Voltage[Index].ErrFlag) {
                        bmSignalErrorLed (LED_BLINK_CODE4, FALSE);
                        bmSignalEvent (0,
                            E_SUPPLY_VOLTAGE_LOW, FALSE, Voltage[Index].Value);
                        Voltage[Index].ErrFlag = FALSE;
                    }
                }
                else if (State == POWER_FAILED) {
                    if (!Voltage[Index].ErrFlag) {
                        bmSignalErrorLed (LED_BLINK_CODE4, TRUE);
                        bmSignalEvent (0,
                            E_SUPPLY_VOLTAGE_LOW, TRUE, Voltage[Index].Value);

                        if (Voltage[Index].Failures < MAX_UINT16) {
                            Voltage[Index].Failures++;
                        }
                        Voltage[Index].ErrFlag = TRUE;
                    }
                    bmSetGlobalControl (MODULE_CONTROL_FLUSH_DATA);
                }
                else if (State == POWER_UNKNOWN) {
                    bmSignalEvent (0, E_VOLTAGE_MONITOR_FAILURE, TRUE, 0);
                    Voltage[Index].Enabled = FALSE;
                }
                bmWriteProtectStorage (
                    PROTECT_BY_SUPPLY_MONITOR, State == POWER_FAILED);

                Voltage[Index].State = State;
            }
            Voltage[Index].SampleTime = bmGetTime();
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Monitor board supply voltage
 *
 *      Monitors the board supply voltage. If the voltage falls below the
 *      "fail" threshold, an error is signalled and a blink code displayed
 *      on the error LED. If the voltage rises above "good" level, the
 *      error will be annulled and the blinking switched off. The number
 *      of voltage fails is counted. If voltage can not be determined,
 *      an error is signalled and the voltage monitor disabled. Voltage
 *      is sampled in a configurable time interval.
 *
 *      Non-volatile storage gets write protected as long as voltage is
 *      below "fail" threshold.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmMonitorSupplyVoltage (void) {

    return bmMonitorSupplyVoltageIndex(0);
}


/*****************************************************************************/
/*!
 *  \brief   Monitor board supply safe voltage
 *
 *      Monitors the board supply safe voltage. If the voltage falls below 
 *      the "fail" threshold, an error is signalled and a blink code 
 *      displayed on the error LED. If the voltage rises above "good" level, 
 *      the error will be annulled and the blinking switched off. The number
 *      of voltage fails is counted. If voltage can not be determined,
 *      an error is signalled and the voltage monitor disabled. Voltage
 *      is sampled in a configurable time interval.
 *
 *      Non-volatile storage gets write protected as long as voltage is
 *      below "fail" threshold.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmMonitorSupplySafe (void) {

    return bmMonitorSupplyVoltageIndex(1);
}


/*****************************************************************************/
/*!
 *  \brief   Monitor board supply current
 *
 *      Monitors the board supply current. If the current rises above the
 *      "fail" threshold, an error is signalled. If current falls below
 *      "good" level, the error will be annulled. The number of current
 *      fails is counted. If current can not be determined, an error is
 *      signalled and the current monitor disabled. Current is sampled
 *      in a configurable time interval.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmMonitorSupplyCurrent (void) {

    if (Current.Enabled || Current.ErrFlag) {
        if (bmTimeExpired(Current.SampleTime) > Current.SampleRate) {

            bmPowerState_t State =
                Current.Enabled ? bmGetSupplyCurrentState() : POWER_GOOD;

            if (State != Current.State) {

                if (State == POWER_GOOD) {
                    if (Current.ErrFlag) {
                        bmSignalEvent (0,
                            E_SUPPLY_CURRENT_HIGH, FALSE, Current.Value);
                        Current.ErrFlag = FALSE;
                    }
                }
                else if (State == POWER_FAILED) {
                    if (!Current.ErrFlag) {
                        bmSignalEvent (0,
                            E_SUPPLY_CURRENT_HIGH, TRUE, Current.Value);
                        if (Current.Failures < MAX_UINT16) {
                            Current.Failures++;
                        }
                        Current.ErrFlag = TRUE;
                    }
                }
                else if (State == POWER_UNKNOWN) {
                    bmSignalEvent (0, E_CURRENT_MONITOR_FAILURE, TRUE, 0);
                    Current.Enabled = FALSE;
                }
                Current.State = State;
            }
            Current.SampleTime = bmGetTime();
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Get actual state of power or safe supply voltage by index
 *
 *      Reads the voltage monitor analog input and qualifies it to one of
 *      the following classes:
 *
 *      - POWER_GOOD
 *      - POWER_WARNING
 *      - POWER_FAILED
 *      - POWER_UNKNOWN
 *
 *      POWER_UNKNOWN is returned, if the voltage can not be read due to an
 *      error accessing the input channel.
 *
 *  \iparam Index = Number of the voltage monitor
 *
 *  \return  Power state (see description)
 *
 ****************************************************************************/

static bmPowerState_t bmGetSupplyVoltageState (UInt8 Index) {

    if (Voltage[Index].Handle >= 0) {

        if (bmGetFilteredInput(&Voltage[Index]) == NO_ERROR) {

            if (Voltage[Index].Value >= Voltage[Index].Threshold2) {
                return (POWER_GOOD);
            }
            else if (Voltage[Index].Value <= Voltage[Index].Threshold1) {
                return (POWER_FAILED);
            }
            return (POWER_WARNING);
        }
    }
    return (POWER_UNKNOWN);
}


/*****************************************************************************/
/*!
 *  \brief   Get actual state of the board supply current
 *
 *      Reads the supply current monitor analog input and qualifies it to
 *      one of the following classes:
 *
 *      - POWER_GOOD
 *      - POWER_WARNING
 *      - POWER_FAILED
 *      - POWER_UNKNOWN
 *
 *      POWER_UNKNOWN ist returned, if the current can not be read due to an
 *      error accessing the input channel.
 *
 *  \return  Power state (see description)
 *
 ****************************************************************************/

static bmPowerState_t bmGetSupplyCurrentState (void) {

    if (Current.Handle >= 0) {
        if (bmGetFilteredInput(&Current) == NO_ERROR) {

            if (Current.Value <= Current.Threshold1) {
                return (POWER_GOOD);
            }
            else if (Current.Value >= Current.Threshold2) {
                return (POWER_FAILED);
            }
            return (POWER_WARNING);
        }
    }
    return (POWER_UNKNOWN);
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

static Error_t bmGetFilteredInput (bmPowerMonitorState_t *Monitor) {

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
 *  \brief   Send board supply voltage status
 *
 *      Sends the actual status of the supply voltage of the slave board
 *      to the master. Power status includes the following informations:
 *
 *      - Supply voltage state (ok, not ok)
 *      - Supply voltage value
 *      - Number of times voltage was low (fail counter)
 *
 *      The fail counters are cleared after sending this message. If
 *      voltage monitoring is disabled an error is returned.
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request power supply voltage" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendVoltageState (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;

    if (Voltage[0].Enabled) {

        Response.CanID  = MSG_SRV_VOLTAGE_STATE;
        Response.Length = 5;

        bmSetMessageItem (&Response, Voltage[0].State, 0, 1);
        bmSetMessageItem (&Response, Voltage[0].Value, 1, 2);
        bmSetMessageItem (&Response, Voltage[0].Failures, 3, 2);
        Voltage[0].Failures = 0;

        return (canWriteMessage(Channel, &Response));
    }
    return (E_VOLTAGE_MONITOR_OFF);
}


/*****************************************************************************/
/*!
 *  \brief   Send board safe supply voltage status
 *
 *      Sends the actual status of the safe supply voltage of the slave board
 *      to the master. Power status includes the following informations:
 *
 *      - Supply voltage state (ok, not ok)
 *      - Supply voltage value
 *      - Number of times voltage was low (fail counter)
 *
 *      The fail counters are cleared after sending this message. If
 *      voltage monitoring is disabled an error is returned.
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request power supply voltage" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendSafeState (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;

    if (Voltage[1].Enabled) {

        Response.CanID  = MSG_SRV_SAFE_STATE;
        Response.Length = 5;

        bmSetMessageItem (&Response, Voltage[1].State, 0, 1);
        bmSetMessageItem (&Response, Voltage[1].Value, 1, 2);
        bmSetMessageItem (&Response, Voltage[1].Failures, 3, 2);
        Voltage[1].Failures = 0;

        return (canWriteMessage(Channel, &Response));
    }
    return (E_VOLTAGE_MONITOR_OFF);
}


/*****************************************************************************/
/*!
 *  \brief   Send board supply current status
 *
 *      Sends the actual status of the supply current of the slave board
 *      to the master. Power status includes the following informations:
 *
 *      - Supply current state (ok, not ok)
 *      - Supply current value
 *      - Number of times voltage was low (fail counter)
 *
 *      The fail counters are cleared after sending this message. If
 *      current monitoring is disabled an error is returned.
 *
 *      This function is called by the message dispatcher whenever the
 *      service command "request power supply current" is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendCurrentState (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;

    if (Current.Enabled) {

        Response.CanID  = MSG_SRV_CURRENT_STATE;
        Response.Length = 5;

        bmSetMessageItem (&Response, Current.State, 0, 1);
        bmSetMessageItem (&Response, Current.Value, 1, 2);
        bmSetMessageItem (&Response, Current.Failures, 3, 2);
        Current.Failures = 0;

        return (canWriteMessage(Channel, &Response));
    }
    return (E_CURRENT_MONITOR_OFF);
}


/*****************************************************************************/
/*!
 *  \brief   Configure power monitor
 *
 *      Configures the power monitor. The following settings can be changed:
 *
 *      - Power monitor enable/disable
 *      - Filter coefficient (number of samples)
 *      - Filter sample rate
 *      - Power good threshold
 *      - Power fail threshold
 *
 *      The power monitor doesn't need to be configured to be operational,
 *      but the configuration message can be used to change the default
 *      voltage/current limits or to disable monitoring.
 *
 *  \iparam  Monitor = Pointer to power monitor data structure
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmConfigPowerMonitor (
                    bmPowerMonitorState_t *Monitor, CanMessage_t *Message) {

    if (Message->Length >= 7) {

        UInt8 ModeFlags     = bmGetMessageItem(Message, 0, 1);
        Monitor->Filter     = bmGetMessageItem(Message, 1, 1);
        Monitor->SampleRate = bmGetMessageItem(Message, 2, 1);
        Monitor->Threshold1 = bmGetMessageItem(Message, 3, 2);
        Monitor->Threshold2 = bmGetMessageItem(Message, 5, 2);

        // swap thresholds if lower is greater than upper
        if (Monitor->Threshold1 > Monitor->Threshold2) {
            UInt16 Threshold = Monitor->Threshold1;
            Monitor->Threshold1 = Monitor->Threshold2;
            Monitor->Threshold2 = Threshold;
        }
        if (!Monitor->SampleRate) {
            Monitor->SampleRate = DEFAULT_SAMPLE_RATE;
        }
        Monitor->Enabled = (ModeFlags & MONITOR_MODE_ENABLE) ? 1 : 0;
        if (!Monitor->Enabled) {
            Monitor->InCount = 0;
        }
        Monitor->SampleTime = bmGetTime();

        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Configure supply voltage monitor
 *
 *      Configures the supply voltage monitor. This is just a stub, which
 *      calls the power monitor configuration function, which does the
 *      real thing. See there for more information.
 *
 *      This function is called by the CAN message dispatcher whenever a
 *      voltage monitor configuration message is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmConfigVoltageMonitor (UInt16 Channel, CanMessage_t *Message) {

    if (Voltage[0].Handle < 0) {
        return (E_VOLTAGE_MONITOR_PORT);
    }
    return (bmConfigPowerMonitor (&Voltage[0], Message));
}


/*****************************************************************************/
/*!
 *  \brief   Configure safe supply voltage monitor
 *
 *      Configures the safe supply voltage monitor. This is just a stub, 
 *      which calls the power monitor configuration function, which does 
 *      the real thing. See there for more information.
 *
 *      This function is called by the CAN message dispatcher whenever a
 *      voltage monitor configuration message is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmConfigSafeMonitor (UInt16 Channel, CanMessage_t *Message) {

    if (Voltage[1].Handle < 0) {
        return (E_VOLTAGE_MONITOR_PORT);
    }
    return (bmConfigPowerMonitor (&Voltage[1], Message));
}


/*****************************************************************************/
/*!
 *  \brief   Configure supply current monitor
 *
 *      Configures the supply current monitor. This is just a stub, which
 *      calls the power monitor configuration function, which does the
 *      real thing. See there for more information.
 *
 *      This function is called by the CAN message dispatcher whenever a
 *      current monitor configuration message is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmConfigCurrentMonitor (UInt16 Channel, CanMessage_t *Message) {

    if (Current.Handle < 0) {
        return (E_CURRENT_MONITOR_PORT);
    }
    return (bmConfigPowerMonitor (&Current, Message));
}


/*****************************************************************************/
/*!
 *  \brief   Initializes callback function entries for CAN messages 
 *
 *      Initializes callback function entries for CAN messages belonging to
 *		monitors, which are registered to the message dispatcher, for
 *      power/safe supply voltage and current.   
 *      CAN message handling functions for all kinds of monitors will be 
 *		registered to message dispatcher even only one kind of monitor 
 *      is enabled.
 *
 *      This function is called once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmInitPowerMonitor (void) {

    static bmCallbackEntry_t Commands[] = {
        { MSG_CFG_CURRENT_MONITOR,   bmConfigCurrentMonitor },
        { MSG_SRV_REQ_CURRENT_STATE, bmSendCurrentState     },
        { MSG_SYS_MASTER_POWER,      bmSetMasterPowerState  },
        { MSG_CFG_VOLTAGE_MONITOR,   bmConfigVoltageMonitor },
        { MSG_SRV_REQ_VOLTAGE_STATE, bmSendVoltageState     },
        { MSG_CFG_SAFE_MONITOR,      bmConfigSafeMonitor    },
        { MSG_SRV_REQ_SAFE_STATE,    bmSendSafeState        },
    };    

	return (canRegisterMessages (0, Commands, ELEMENTS(Commands)));
}

/*****************************************************************************/
/*!
 *  \brief   Initializes the supply voltage monitor by index
 *
 *      Initializes the voltage monitor by opening the analog input connected
 *      to the board supply voltage and setting up the structure holding the
 *      state of the voltage monitor. All data elements in this structure are
 *      assumed to be set to 0 before calling this function.
 *
 *      Voltage thresholds are looked up in the board options for the base
 *      module. If the voltage thresholds are defined there, they are taken
 *      over and the voltage monitor is enabled. If thresholds are not
 *      defined there, the voltage monitor is disabled, but can be enabled
 *      later by the master using appropriate configuration messages.
 *
 *      Anyway, CAN messages belonging to the supply voltage monitor are
 *      registered to the message dispatcher.
 *
 *      This function is called once during startup.
 *
 *  \iparam Index = Number of the voltage monitor
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static void bmInitVoltageMonitorIndex (UInt8 Index) {

    if (Index == 0) {
        Voltage[Index].Handle = halAnalogOpen (HAL_SUPPLY_VOLTAGE_1, HAL_OPEN_READ, 0, NULL);
    }
    else if (Index == 1) {
        Voltage[Index].Handle = halAnalogOpen (HAL_SUPPLY_VOLTAGE_2, HAL_OPEN_READ, 0, NULL);
    }

    if ( (Index == 0 || Index == 1) && Voltage[Index].Handle >= 0 ) {

        UInt32 Limit1 = bmGetBoardOptions (0, OPTIONS_VOLTAGE_LIMIT1, 0);
        UInt32 Limit2 = bmGetBoardOptions (0, OPTIONS_VOLTAGE_LIMIT2, 0);

        if (Limit1 || Limit2) {
            Voltage[Index].Threshold1 = Limit2;
            Voltage[Index].Threshold2 = Limit1;
            Voltage[Index].Enabled = TRUE;
        }
        Voltage[Index].SampleRate = DEFAULT_SAMPLE_RATE;
        Voltage[Index].Filter  = DEFAULT_FILTER_COUNT;
        Voltage[Index].State   = POWER_GOOD;
        Voltage[Index].ErrFlag = FALSE;
    }
}


/*****************************************************************************/
/*!
 *  \brief   Initializes the supply voltage monitor
 *
 *      Initializes the voltage monitor by opening the analog input connected
 *      to the board supply voltage and setting up the structure holding the
 *      state of the voltage monitor. All data elements in this structure are
 *      assumed to be set to 0 before calling this function.
 *
 *      Voltage thresholds are looked up in the board options for the base
 *      module. If the voltage thresholds are defined there, they are taken
 *      over and the voltage monitor is enabled. If thresholds are not
 *      defined there, the voltage monitor is disabled, but can be enabled
 *      later by the master using appropriate configuration messages.
 *
 *      Anyway, CAN messages belonging to the supply voltage monitor are
 *      registered to the message dispatcher.
 *
 *      This function is called once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

void bmInitVoltageMonitor (void) {

    bmInitVoltageMonitorIndex (0);
}


/*****************************************************************************/
/*!
 *  \brief   Initializes the safe supply voltage monitor
 *
 *      Initializes the safe voltage monitor by opening the analog input 
 *      connected to the board safe supply voltage and setting up the  
 *      structure holding the state of the voltage monitor. All data   
 *      elements in this structure are assumed to be set to 0 before  
 *      calling this function.
 *
 *      Voltage thresholds are looked up in the board options for the base
 *      module. If the voltage thresholds are defined there, they are taken
 *      over and the voltage monitor is enabled. If thresholds are not
 *      defined there, the voltage monitor is disabled, but can be enabled
 *      later by the master using appropriate configuration messages.
 *
 *      Anyway, CAN messages belonging to the supply voltage monitor are
 *      registered to the message dispatcher.
 *
 *      This function is called once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

void bmInitSafeMonitor (void) {
    bmInitVoltageMonitorIndex (1);
}


/*****************************************************************************/
/*!
 *  \brief   Initializes the supply current monitor
 *
 *      Initializes the current monitor by opening the analog input connected
 *      to the board supply current and setting up the structure holding the
 *      state of the current monitor. All data elements in this structure are
 *      assumed to be set to 0 before calling this function.
 *
 *      Current thresholds are looked up in the board options for the base
 *      module. If the current thresholds are defined there, they are taken
 *      over and the current monitor gets enabled. If thresholds are not
 *      defined there, the current monitor is disabled., but can be enabled
 *      later by the master using appropriate configuration messages.
 *
 *      Anyway, CAN messages belonging to the supply current monitor are
 *      registered to the message dispatcher.
 *
 *      This function is called once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

void bmInitCurrentMonitor (void) {

    Current.Handle = halAnalogOpen (HAL_SUPPLY_CURRENT, HAL_OPEN_READ, 0, NULL);

    if (Current.Handle >= 0) {

        UInt32 Limit1 = bmGetBoardOptions (0, OPTIONS_CURRENT_LIMIT1, 0);
        UInt32 Limit2 = bmGetBoardOptions (0, OPTIONS_CURRENT_LIMIT2, 0);

        if (Limit1 || Limit2) {
            Current.Threshold1 = Limit1;
            Current.Threshold2 = Limit2;
            Current.Enabled = TRUE;
        }
        Current.SampleRate = DEFAULT_SAMPLE_RATE;
        Current.Filter  = DEFAULT_FILTER_COUNT;
        Current.State   = POWER_GOOD;
        Current.ErrFlag = FALSE;
    }
}

//****************************************************************************/
