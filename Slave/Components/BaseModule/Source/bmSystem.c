/****************************************************************************/
/*! \file bmSystem.c
 *
 *  \brief System message handling
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions handling CAN system messages.
 *       This includes:
 *
 *       - Configuration management (master/slave)
 *       - Mode change requests
 *       - Heartbeat handling
 *       - Emergency stop handling (notstopp)
 *       - Node reset
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
#include "bmCommon.h"
#include "halLayer.h"
#include "bmError.h"
#include "bmTime.h"
#include "bmBlink.h"
#include "bmScheduler.h"
#include "bmStorage.h"
#include "bmUtilities.h"
#include "bmCan.h"
#include "bmDispatch.h"
#include "bmMonitor.h"
#include "bmSystem.h"
#include "bmDebug.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define CAN_PROTOCOL_VERSION         1     //!< Leica CAN protocol version
#define MAX_RESET_SEQUENCE_GAP       500   //!< Max. reset sequence gap time

/* Heartbeat mode bit access macros */
#define HEARTBEAT_SEND_ENABLE        0x80  //!< Heartbeat send enable
#define HEARTBEAT_CHECK_ENABLE       0x40  //!< Heartbeat checking enable
#define HEARTBEAT_UNSTOPPABLE        0x20  //!< Heartbeat unstoppable

/* Heartbeat default settings */
#define DEFAULT_HEARTBEAT_INTERVAL   500   //!< Default heartbeat interval
#define DEFAULT_HEARTBEAT_TIMEOUT    5000  //!< Default master heartbeat timeout


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Enumeration to control emergency stops
typedef enum {
    RESET_EMERGENCY_STOP = 0,   //!< Clear emergency stop state
    STOPPED_BY_HEARTBEAT = 1,   //!< Emergency stop by heartbeat loss
    STOPPED_BY_NOTSTOP   = 2    //!< Emergency stop by master command
} bmEmergencyStopReason_t;

//! Contains all variables to maintain heartbeats
typedef struct {
    UInt16 Mode;                //!< Heartbeat mode bits
    UInt16 Interval;            //!< Heartbeat send interval (from slave)
    UInt16 Timeout;             //!< Heartbeat receive interval (from master)
    UInt32 LastSendTime;        //!< Last heartbeat send time (from slave)
    UInt32 LastRecvTime;        //!< Last heartbeat receive time (from master)
} bmHeartBeatData_t;

typedef void (*bmBootLoaderVector)(void);  //!< Address of bootloader


//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Node state requested by master (not yet done)
static bmNodeState_t ModeRequest = NODE_STATE_UNDEFINED;

//! Actual node state of this slave
static bmNodeState_t NodeState = NODE_STATE_UNDEFINED;

//! Energency stop state
static UInt16 EmergencyStop = RESET_EMERGENCY_STOP;

//! Heart beat related variables
static bmHeartBeatData_t Heartbeat = {0};


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t bmSendConfiguration  (UInt16 Channel, CanMessage_t *Message);
static Error_t bmSetEmergencyStop   (UInt16 Channel, CanMessage_t *Message);
static Error_t bmReceiveHeartbeat   (UInt16 Channel, CanMessage_t *Message);
static Error_t bmConfigureHeartbeat (UInt16 Channel, CanMessage_t *Message);
static Error_t bmRequestModeChange  (UInt16 Channel, CanMessage_t *Message);
static Error_t bmRequestNodeState   (UInt16 Channel, CanMessage_t *Message);
static Error_t bmNotifyNodeState    (void);


/*****************************************************************************/
/*!
 *  \brief   Set node state
 *
 *      Changes the state of this node. Possible states are:
 *
 *       - STARTUP  = system startup
 *       - IDENTIFY = node identification
 *       - NORMAL   = normal operation mode
 *       - SERVICE  = service mode
 *       - UPDATE   = firmware update
 *       - STANDBY  = standby (ready for power off
 *       - SHUTDOWN = normal shutdown (finishes running actions)
 *       - SLEEPING = node is sleeping
 *
 *  \iparam  NewState = New node state
 *
 *  \return  Node state before calling this function
 *
 ****************************************************************************/

bmNodeState_t bmSetNodeState (bmNodeState_t NewState) {

    bmNodeState_t OldState = NodeState;

    if (NewState != NodeState) {
        switch (NewState) {

            case NODE_STATE_NORMAL:
                bmSignalStatusLed (LED_BLINK_1HZ); break;

            case NODE_STATE_STARTUP:
                bmSignalStatusLed (LED_STATIC_ON); break;

            case NODE_STATE_IDENTIFY:
                bmSignalStatusLed (LED_FLASH_1HZ); break;

            case NODE_STATE_ASSEMBLY:
                bmSignalStatusLed (LED_BLINK_2HZ); break;

            case NODE_STATE_SHUTDOWN:
                bmSignalStatusLed (LED_FLASH_2HZ); break;

            case NODE_STATE_STANDBY:
                bmWriteProtectStorage(PROTECT_BY_NODE_STATE, TRUE);
                bmSignalStatusLed (LED_FLASH_SLOW);
                break;
        }
        if (OldState == NODE_STATE_STANDBY) {
            bmWriteProtectStorage(PROTECT_BY_NODE_STATE, FALSE);
        }
        NodeState = NewState;
    }
    dbgPrintNodeState (NewState);
    bmNotifyNodeState();

    return (OldState);
}


/*****************************************************************************/
/*!
 *  \brief   Returns actual node state
 *
 *      Returns the current state of this node. Possible states are:
 *
 *       - STARTUP  = system startup
 *       - IDENTIFY = node identification
 *       - NORMAL   = normal operation mode
 *       - SERVICE  = service mode
 *       - UPDATE   = firmware update
 *       - STOPPED  = emergency stopp
 *       - STANDBY  = standby (ready for power off
 *       - SHUTDOWN = normal shutdown (finishes running actions)
 *
 *  \return  Node state
 *
 ****************************************************************************/

bmNodeState_t bmGetNodeState (void) {

    return (NodeState);
}


/*****************************************************************************/
/*!
 *  \brief   Processes mode change commands
 *
 *      Processes mode change requests received from master. Mode changes,
 *      except for shutdowns, are accepted only if all function modules are
 *      in idle state, else they were rejected. Shutdown requests on the
 *      other hand are always processed.
 *
 *  \return   Actual node state
 *
 ****************************************************************************/

bmNodeState_t bmProcessModeChange (void) {

    if (ModeRequest != NODE_STATE_UNDEFINED) {

        bmNodeState_t OldState = bmSetNodeState(ModeRequest);

        if (ModeRequest == NODE_STATE_SHUTDOWN) {
            bmSetGlobalControl (MODULE_CONTROL_FLUSH_DATA);
            bmSetGlobalControl (MODULE_CONTROL_SHUTDOWN);
        }
        else if (ModeRequest == NODE_STATE_IDENTIFY) {
            bmSetGlobalControl (MODULE_CONTROL_FLUSH_DATA);
            bmSetGlobalControl (MODULE_CONTROL_RESET);
        }
        else if (ModeRequest == NODE_STATE_BOOTING) {
            bmSetGlobalControl (MODULE_CONTROL_FLUSH_DATA);
            bmSetGlobalControl (MODULE_CONTROL_RESET);
            canFlushMessages (1000);
            halHardwareReset(); 
        }
        else if (ModeRequest == NODE_STATE_ASSEMBLY) {
            bmSetGlobalControl (MODULE_CONTROL_FLUSH_DATA);
            bmSetGlobalControl (MODULE_CONTROL_RESET);
        }
        if (ModeRequest < NODE_STATE_SHUTDOWN) {
            if (OldState >= NODE_STATE_SHUTDOWN) {
                bmSetGlobalControl (MODULE_CONTROL_WAKEUP);
            }
        }
        ModeRequest = NODE_STATE_UNDEFINED;
    }
    return (NodeState);
}


/*****************************************************************************/
/*!
 *  \brief   Request node state
 *
 *      This function is called by the message dispatcher, whenever a node
 *      state change request from the master is received. It set's the global
 *      variable ModeRequest only; the processing is done in the task
 *      function bmProcessModeChange().
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmRequestModeChange (UInt16 Channel, CanMessage_t *Message) {

    if (Message->Length >= 1) {

        bmNodeState_t NewState = (bmNodeState_t) Message->Data[0];

        // Check if requested state is a valid state
        if (NewState >= NUMBER_OF_NODE_STATES || 
            NewState == NODE_STATE_UNDEFINED) {
            return (E_PARAMETER_OUT_OF_RANGE);
        }
         // Can't leave initialization states via state change command
        if (NodeState < NODE_STATE_CONFIGURE) {
            return (E_ILLEGAL_MODE_CHANGE);
        }
        // Change to update mode is allowed only out of standby mode
        if (NewState == NODE_STATE_ASSEMBLY && NodeState < NODE_STATE_ASSEMBLY) {
            return (E_ILLEGAL_MODE_CHANGE);
        }
        // Standby/startup state can't be entered directly
        if (NewState == NODE_STATE_STANDBY || NewState == NODE_STATE_STARTUP) {
            return (E_ILLEGAL_MODE_CHANGE);
        }
        // State changes are not allowed if node is busy (except for shutdown)
        if (NewState != NODE_STATE_SHUTDOWN) {
            if (! bmModulesInState (MODULE_STATE_BUSY, FALSE)) {
                return (E_NODE_BUSY);
            }
        }
        ModeRequest = NewState;
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Send actual node state to the master
 *
 *      This function is called by the message dispatcher, whenever a mode
 *      request message from the master is received. It sends the actual 
 *      node state to the master. 
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmRequestNodeState (UInt16 Channel, CanMessage_t *Message) {

    return (bmNotifyNodeState());
}


/*****************************************************************************/
/*!
 *  \brief   Send actual node state to the master
 *
 *      Sends the actual node state to the master. The node state message
 *      includes not only the actual node state, but also the emergency
 *      stop state and the power supply state of the node.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmNotifyNodeState (void) {

    CanMessage_t Message;

    Message.CanID = MSG_SYS_NODESTATE;
    bmSetMessageItem (&Message, bmGetNodeState(), 0, 1);
    bmSetMessageItem (&Message, bmGetEmergencyStop(), 1, 1);
    bmSetMessageItem (&Message, bmGetPowerSupplyStatus(), 2, 1);
    bmSetMessageItem (&Message, bmGetSafeSupplyStatus(), 3, 1);
    Message.Length = 4;

    return (canWriteMessage(BASEMODULE_CHANNEL, &Message));
}


/*****************************************************************************/
/*!
 *  \brief   Heartbeat control task
 *
 *      If sending of heartbeats is enabled, it checks if the heartbeat
 *      interval time since last sending a heartbeat is exhausted. If
 *      this is the case, a new heartbeat message is send and the interval
 *      timer is reset.
 *
 *      If checking of master heartbeats is enabled, it checks if the
 *      time since last receiving a master heartbeat exceeds the heart
 *      beat timeout. If this is the case, the node is switched into
 *      emergency stop mode. This mode can be left only by receiving a
 *      new heartbeat message from the master.
 *
 *      This function is called by the task scheduler on a regular base.
 *
 *  \return   NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmProcessHeartbeat (void) {

    CanMessage_t Message;

    // send heartbeat to master if interval time exhausted
    if (Heartbeat.Mode & HEARTBEAT_SEND_ENABLE) {
        if (bmTimeExpired (Heartbeat.LastSendTime) > Heartbeat.Interval) {

            Message.CanID  = MSG_SYS_SLAVE_HEARTBEAT;
            Message.Length = 0;
            canWriteMessage(BASEMODULE_CHANNEL, &Message);
            Heartbeat.LastSendTime = bmGetTime();
        }
    }
    // check heartbeat from master, activate emergency stop if missing
    if (Heartbeat.Mode & HEARTBEAT_CHECK_ENABLE) {
        if (bmTimeExpired (Heartbeat.LastRecvTime) > Heartbeat.Timeout) {

            EmergencyStop |= STOPPED_BY_HEARTBEAT;
            Heartbeat.LastRecvTime = bmGetTime();
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Master heartbeat message handler
 *
 *      This function is called by the message dispatcher, whenever a heart
 *      beat message from the master is received. It resets the global time
 *      variable "LastMasterHeartbeat" to the current time. This time is
 *      used to detect loss of master heartbeats: If the time since last
 *      update of this variable exceeds the heartbeat interval time, the
 *      master heart beat is supposed to be lost. This check is done in
 *      the task function bmProcessHeartbeat(). If the node is stopped
 *      by the loss of heartbeat, this condition will be reset on receipt
 *      of the next heartbeat.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmReceiveHeartbeat (UInt16 Channel, CanMessage_t *Message) {

    if (EmergencyStop) {
        EmergencyStop &= ~STOPPED_BY_HEARTBEAT;
    }
    Heartbeat.LastRecvTime = bmGetTime();
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Heartbeat configuration message handler
 *
 *      This function is called by the message dispatcher, whenever a heart
 *      beat configuration message from the master is received. The message
 *      contains the following informations:
 *
 *      - Heartbeat mode
 *      - Heartbeat interval (slave heartbeat)
 *      - Heartbeat timeout (master heartbeat)
 *
 *      Mode includes individual bits to enable sending of heartbeats,
 *      enable checking received heartbeats, or make heartbeats unstoppable.
 *      If set to unstoppable, heartbeats can still be enabled, but not
 *      disabled anymore. This applies to both, the sending of heartbeats
 *      and the checking of master heartbeats.
 *
 *      If the interval or timeout is zero, default values are used instead.
 *      If heartbeat checking is disabled, a possible STOPPED_BY_HEARTBEAT
 *      condition will be cleared.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmConfigureHeartbeat (UInt16 Channel, CanMessage_t *Message) {
    
    if (Message->Length >= 5) {

        // set slave heartbeat send interval
        if (!(Heartbeat.Mode & HEARTBEAT_UNSTOPPABLE) ||
            !(Heartbeat.Mode & HEARTBEAT_SEND_ENABLE)) {
            Heartbeat.Interval = bmGetMessageItem(Message, 1, 2);
        }
        // set master heartbeat timeout
        if (!(Heartbeat.Mode & HEARTBEAT_UNSTOPPABLE) ||
            !(Heartbeat.Mode & HEARTBEAT_CHECK_ENABLE)) {
            Heartbeat.Timeout = bmGetMessageItem(Message, 3, 2);
            Heartbeat.LastRecvTime = bmGetTime();
        }
        // set heartbeat control bits
        if (Heartbeat.Mode & HEARTBEAT_UNSTOPPABLE) {
            Heartbeat.Mode |= bmGetMessageItem(Message, 0, 1);
        }
        else {
            Heartbeat.Mode = bmGetMessageItem(Message, 0, 1);
        }
        if (!Heartbeat.Interval) {
            Heartbeat.Interval = DEFAULT_HEARTBEAT_INTERVAL;
        }
        if (!Heartbeat.Timeout) {
            Heartbeat.Timeout = DEFAULT_HEARTBEAT_TIMEOUT;
        }
        // if heartbeat check disabled, clear emergency stop
        if (~Heartbeat.Mode & HEARTBEAT_CHECK_ENABLE) {
            EmergencyStop &= ~STOPPED_BY_HEARTBEAT;
        }
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Emergency stop control task
 *
 *      This function is called by the task scheduler on a regular base.
 *      It checks if the emergency stop status has changed since last
 *      calling this function and, if this is the case, processes that
 *      change. If emergency stop is getting active, an error code is
 *      issued on the status LED and all function modules are informed
 *      about this situation by calling their control functions. If
 *      emergency stop is getting inactive the blink code is stopped,
 *      and all function modules are informed to resume operation.
 *      Additionally, if at least one of the function modules has
 *      really aborted a running operation, a notification is send
 *      to the master.
 *
 *  \return   NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmProcessEmergencyStop (void) {

    static UInt16 OldEmergencyState = RESET_EMERGENCY_STOP;
    static Bool OperationAborted;
    CanMessage_t Message;

    // stop all function modules when emergency stop flag active
    if (EmergencyStop != OldEmergencyState) {

        if (EmergencyStop && !OldEmergencyState) {
            if (bmModulesInState(MODULE_STATE_BUSY, FALSE)) {
                OperationAborted = FALSE;
            }
            else {
                OperationAborted = TRUE;
            }
            bmSetGlobalControl (MODULE_CONTROL_STOP);
            bmSignalErrorLed (LED_BLINK_4HZ, TRUE);
        }
        if (!EmergencyStop && OldEmergencyState) {

            // if module had aborted a running operation, send notification
            if (OperationAborted) {

                Message.CanID  = MSG_SYS_ABORTED_BY_NOTSTOP;
                Message.Length = 0;
                canWriteMessage(BASEMODULE_CHANNEL, &Message);
            }
            bmSetGlobalControl (MODULE_CONTROL_RESUME);
            bmSignalErrorLed (LED_BLINK_4HZ, FALSE);
        }
        OldEmergencyState = EmergencyStop;
        dbgPrint ("Notstopp = %d", EmergencyStop);
        bmNotifyNodeState();
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Emergency stop message handler
 *
 *      This function is called by the message dispatcher, whenever an
 *      emergency stop message from the master is received. It sets or
 *      clears (depends on message data) the STOPPED_BY_NOTSTOP bit in
 *      the global variable EmergencyStop. Further processing of this
 *      situation is handled in the emergency stop task function
 *      (see bmProcessEmergencyStop).
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSetEmergencyStop (UInt16 Channel, CanMessage_t *Message) {

    if (Message->Length > 0 && Message->Data[0]) {
        EmergencyStop |= STOPPED_BY_NOTSTOP;
    }
    else {
        EmergencyStop &= ~STOPPED_BY_NOTSTOP;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Get emergency stop status
 *
 *      Returns TRUE if node is in emergency stop mode or FALSE if not.
 *
 *  \return  Emergency stop status
 *
 ****************************************************************************/

Bool bmGetEmergencyStop (void) {

    return (EmergencyStop);
}


/*****************************************************************************/
/*!
 *  \brief   Send hardware identification message
 *
 *      This function sends hardware identification messages to the master
 *      (one per second), until the master resonses with a hardware ID
 *      confirmation message. The receive of a confirmation message is
 *      signaled by the global variable IdentificationDone and handled
 *      by bmConfirmHardwareID(). The hardware ID message contains:
 *
 *      - Node class
 *      - Protocol version
 *      - Hardware version number
 *      - Software version number
 *      - Number of logical channels (tasks)
 *
 *      The return value indicates, if the identification is still in
 *      progress (FALSE) or is already confirmed by the master (TRUE).
 *
 *      This function must be called repetitive during startup as long
 *      as it doesn't return TRUE.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmSendHardwareID (void) {

    static UInt32 StartTime = 0;
    CanMessage_t HardwareID;

    if (bmGetNodeState() == NODE_STATE_IDENTIFY) {
        if (bmTimeExpired(StartTime) > 1000) {

            bmBoardInfoBlock_t *Infos = bmGetBoardInfoBlock();

            HardwareID.CanID = MSG_SYS_HARDWARE_ID;
            bmSetMessageItem (&HardwareID, Infos->NodeClass, 0, 1);
            bmSetMessageItem (&HardwareID, CAN_PROTOCOL_VERSION, 1, 1);
            bmSetMessageItem (&HardwareID, Infos->VersionMajor,  2, 1);
            bmSetMessageItem (&HardwareID, Infos->VersionMinor,  3, 1);
            bmSetMessageItem (&HardwareID, SOFTWARE_VERSION,     4, 2);
            bmSetMessageItem (&HardwareID, bmGetNumberOfTasks(), 6, 1);
            HardwareID.Length = 7;

            canWriteMessage(BASEMODULE_CHANNEL, &HardwareID);
            StartTime = bmGetTime();
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Hardware ID confirmation message handler
 *
 *      This function is called by the message dispatcher, whenever a
 *      hardware ID confirmation message from the master is received.
 *      It changes the node mode from IDENTIFY to CONFIGURE.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmConfirmHardwareID (UInt16 Channel, CanMessage_t *Message) {

    if (bmGetNodeState() == NODE_STATE_IDENTIFY) {
        bmSetNodeState (NODE_STATE_CONFIGURE);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Send node configuration message(s)
 *
 *      Sends a configuration message for each module instance on that slave
 *      to the master. Each configuration message contains:
 *
 *      - Module ID
 *      - Module instance
 *      - Logical channel number
 *      - Module version number
 *
 *      This function is called by the message dispatcher, whenever a
 *      configuration request message from the master is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendConfiguration (UInt16 Channel, CanMessage_t *Message) {

    UInt16 NumberOfTasks = bmGetNumberOfTasks();
    CanMessage_t Response;
    UInt16 Version;
    UInt16 Task;

    if (bmGetNodeState() != NODE_STATE_CONFIGURE) {
        return (E_COMMAND_REJECTED);
    }
    for (Task=0; Task < NumberOfTasks; Task++) {

        Version = bmGetModuleStatus(Task, MODULE_STATUS_VERSION, 0);

        Response.CanID = MSG_SYS_CONFIGURATION;
        bmSetMessageItem (&Response, bmGetModuleID(Task), 0, 2);
        bmSetMessageItem (&Response, bmGetInstance(Task), 2, 1);
        bmSetMessageItem (&Response, bmGetChannel(Task), 3, 1);
        bmSetMessageItem (&Response, Version, 4, 2);
        Response.Length = 6;

        canWriteMessage(BASEMODULE_CHANNEL, &Response);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Node reset message handler
 *
 *      Initiates a node hardware reset, if the required conditions are met.
 *      Multiple reset commands containing different data patterns must be
 *      received within a time span, before the reset is really performed.
 *      If the maximal allowed time between two reset commands is exceeded
 *      or the expected data pattern in one of the commands doesn't match,
 *      the sequence must be restartet.
 *
 *      This function is called by the message dispatcher, whenever a
 *      node reset message from the master is received.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmProcessNodeReset (UInt16 Channel, CanMessage_t *Message) {

    static const UInt8 PatternSequence[] = { 0x55, 0xE7 };
    static UInt32 LastPatternTime = 0;
    static UInt16 PatternIndex = 0;

    if (Message->Length >= 1) {

        UInt8 Pattern = bmGetMessageItem(Message, 0, 1);

        if (PatternIndex > 0 &&
            bmTimeExpired(LastPatternTime) > MAX_RESET_SEQUENCE_GAP) {
            PatternIndex = 0;
            return (E_RESET_PATTERN_TIMEOUT);
        }
        if (Pattern != PatternSequence[PatternIndex]) {
            PatternIndex = 0;
            return (E_RESET_PATTERN_SEQUENCE);
        }
        else {
            PatternIndex++;
        }
        if (PatternIndex >= ELEMENTS(PatternSequence)) {
            bmSetGlobalControl (MODULE_CONTROL_FLUSH_DATA);
            bmSetGlobalControl (MODULE_CONTROL_RESET);
            canFlushMessages (1000);

            // Do hardware reset (should never return)
            halHardwareReset();
            PatternIndex = 0;

            return (E_HARDWARE_RESET_FAILED);
        }
        LastPatternTime = bmGetTime();
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Initializes this module
 *
 *      Initializes this module by registering all system messages to the
 *      message dispatcher.
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmInitSystemCommands (void) {

    static bmCallbackEntry_t Commands[] = {
        { MSG_SYS_MASTER_HEARTBEAT,  bmReceiveHeartbeat   },
        { MSG_SYS_CFG_HEARTBEAT,     bmConfigureHeartbeat },
        { MSG_SYS_SET_NODESTATE,     bmRequestModeChange  },
        { MSG_SYS_REQ_NODESTATE,     bmRequestNodeState   },
        { MSG_SYS_NOTSTOP,           bmSetEmergencyStop   },
        { MSG_SYS_REQ_CONFIGURATION, bmSendConfiguration  },
        { MSG_SYS_ACK_HARDWARE_ID,   bmConfirmHardwareID  },
        { MSG_SYS_RESET,             bmProcessNodeReset   }
    };

    return (canRegisterMessages (0, Commands, ELEMENTS(Commands)));
}

//****************************************************************************/
