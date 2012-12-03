/****************************************************************************/
/*! \file bmDispatch.c
 *
 *  \brief CAN Message Dispatcher
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains the CAN command dispatcher, which is responsible
 *       for the handling of incoming CAN messages. Other modules have to
 *       register the messages they want to handle. The dispatcher calls the
 *       registered handler function when the associated message is received.
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
#include "bmCommon.h"
#include "bmError.h"
#include "bmCan.h"
#include "bmSystem.h"
#include "bmDispatch.h"
#include "bmUtilities.h"
#include "bmScheduler.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define CAN_BROADCAST_ADDRESS  0   //!< Broadcast node address
#define TABLE_INITIAL_SIZE     10  //!< Initial command dispatch table size
#define TABLE_SIZE_INCREMENT   5   //!< Dispatch table increment step size

//! Macro to extract the command class out of the CAN-ID
#define GET_CANiD_CMDCLASS(i) \
                    (((i) & CANiD_MASK_CMDCLASS) >> CANiD_SHIFT_CMDCLASS)


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! CAN message callback table for a module
typedef struct {
    UInt16 Count;                  //!< Number of entries in lookup table
    UInt16 Size;                   //!< Current size of lookup table
    bmCallbackEntry_t *Entries;    //!< Lookup table for that module
} bmCallbackTable_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

static bmCallbackTable_t *Modules; //!< Lookup tables of all modules
static UInt16 ModuleCount = 0;     //!< Number of entries in module table
static UInt32 NodeAddress = 0;     //!< Address of this node

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t canLookupMessageTable (UInt16 Channel, CanMessage_t *Message);
static Bool    canMessageAcceptable  (UInt32 CanID);


/*****************************************************************************/
/*!
 *  \brief   Reads and dispatches received CAN messages
 *
 *      Reads messages from the input queue, searches for the given CAN
 *      ID in the modules message lookup table and calls the associated
 *      handler function. If the message is not registered, an error
 *      message is send to the master, including the CAN command as
 *      additional info. The error message is send to the channel the
 *      received message was received from.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t canDispatchMessages (void) {

    CanMessage_t Message;
    Error_t Status;
    UInt16 Channel;

    while ((Status = canReadMessage(&Channel, &Message)) > 0) {

        UInt32 Address = Message.CanID & CANiD_MASK_ADDRESS;

        // normalize CAN ID (strip own node address)
        if (Address == NodeAddress) {
            Message.CanID =
                Message.CanID & ~(CANiD_MASK_ADDRESS | CANiD_MASK_CHANNEL);
        }
        // normalize broadcast CAN ID (set broadcast bit)
        else if (Address == CAN_BROADCAST_ADDRESS) {
            Message.CanID =
                (Message.CanID & ~CANiD_MASK_CHANNEL) | CANiD_MASK_BROADCAST;
            Channel = BASEMODULE_CHANNEL;
        }

        // search CAN ID in lookup table and call registered handler
        if ((Status = canLookupMessageTable(Channel, &Message)) < 0) {
            UInt16 errInfo =
                ((Message.CanID & CANiD_MASK_COMMAND) >> 16) |
                ((Message.CanID & CANiD_MASK_MASTER));

            bmSignalEvent (Channel, Status, TRUE, errInfo);
        }
    }
    // message read error; send error message to master
    if (Status < 0) {
        bmSignalEvent (BASEMODULE_CHANNEL, Status, TRUE, 0);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Search for CAN message in lookup table
 *
 *      Searches for the given CAN identifier in the modules lookup table
 *      and calls the associated handler routine. All address informations
 *      in the CAN-ID of the message will be cleared before the search, so
 *      that only the command is taken into account. If the CAN ID is not
 *      registered by the addressed module, an error is returned
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = CAN message to search for
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t canLookupMessageTable (UInt16 Channel, CanMessage_t *Message) {

    bmCallbackEntry_t *Entry;
    UInt16 Count;
    UInt32 CanID;
    Int32 Index;

    // check if message acceptable in current context
    if (!canMessageAcceptable(Message->CanID)) {
        return (E_COMMAND_REJECTED);
    }
    // get module's command lookup table
    Index = bmGetModulIndex (Channel);
    if (Index < 0 || Index >= ModuleCount) {
        return (E_TASK_NOT_EXISTS);
    }
    Entry = Modules[Index].Entries;
    Count = Modules[Index].Count;
    CanID = Message->CanID;

    // all base module messages can be broadcasts
    if (Channel == 0) {
        CanID &= ~CANiD_MASK_BROADCAST;
    }
    // search module's lookup table
    for (Index=0; Index < Count; Index++) {

        if (Entry->CanID == CanID) {
            return (Entry->Handler(Channel, Message));
        }
        Entry++;
    }
    return (E_UNKNOWN_MESSAGE);
}


/*****************************************************************************/
/*!
 *  \brief   Check if CAN message is acceptable
 *
 *      Checks if the supplied message is allowed in current node mode.
 *      System messages are always allowed, all other messages are not
 *      allowed in emergency stop state. In normal operation mode, all
 *      messages are allowed, in service mode only messages of the
 *      command class "service".
 *
 *  \iparam  CanID = CAN identifier
 *
 *  \return  TRUE if command acceptable, FALSE if not
 *
 ****************************************************************************/

static Bool canMessageAcceptable (UInt32 CanID) {

    UInt16 CmdClass = GET_CANiD_CMDCLASS(CanID);
    bmNodeState_t NodeState = bmGetNodeState();

    // system commands are always allowed
    if (CmdClass == CMD_CLASS_SYSTEM) {
        return (TRUE);
    }
    // other commands are forbidden in stop mode
    if (bmGetEmergencyStop()) {
        return (FALSE);
    }
    // Assembly commands are allowed in assembly mode only
    if (CmdClass == CMD_CLASS_ASSEMBLY) {
        return (NodeState == NODE_STATE_ASSEMBLY);
    }
    // in normal mode, everything is allowed
    if (NodeState == NODE_STATE_NORMAL) {
        return (TRUE);
    }
    return (FALSE);
}


/*****************************************************************************/
/*!
 *  \brief   Register CAN handler functions
 *
 *      Registers the supplied CAN-IDs for the module specified by ModuleID.
 *      The user has to supply a table containing CAN-ID/handler function
 *      pairs (cmdTable). The Count parameter must contain the size of
 *      this table. The memory for the lookup table is allocated on heap.
 *      If a module with that ID doesn't exists or no memory for the table
 *      can be allocated, an error is returned
 *
 *  \iparam  ModuleID = Function module ID
 *  \iparam  Commands = Table of CAN command/handler function pairs
 *  \iparam  Count    = Number of entries in that table
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t canRegisterMessages (
                UInt16 ModuleID, bmCallbackEntry_t *Commands, UInt16 Count) {

    bmCallbackTable_t *ModuleTable;
    bmCallbackEntry_t *Entries;
    UInt32 newSize;
    UInt16 TaskID;
    UInt16 i;

    // locate modules lookup table
    if ((TaskID = bmGetTaskID (ModuleID, 0)) == TASK_ID_UNKNOWN) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    ModuleTable = &Modules[bmGetModulIndex(TaskID)];

    // extend buffer size if it's not big enough
    if (ModuleTable->Size < ModuleTable->Count + Count) {

        newSize = ModuleTable->Size + Count + TABLE_SIZE_INCREMENT;
        Entries = realloc (
            ModuleTable->Entries, newSize * sizeof(bmCallbackEntry_t));

        if (Entries == NULL) {
            return (E_MEMORY_FULL);
        }
        ModuleTable->Entries = Entries;
        ModuleTable->Size = newSize;
    }
    // point to end of lookup table
    Entries = &ModuleTable->Entries[ModuleTable->Count];

    // insert registered commands into the table
    for (i=0; i < Count; i++) {
        Entries[i] = Commands[i];
    }
    ModuleTable->Count += Count;

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Register CAN handler functions to all modules
 *
 *      Registers a number of CAN IDs in each module's lookup table.
 *      This function is used by the base module, when it wants to
 *      handle CAN messages in the name of function modules. This
 *      applies to statistics, triggers, and diagnostics messages.
 *
 *      This function is for base module's internal use only!
 *
 *  \iparam  Commands = Table of CAN command/handler function pairs
 *  \iparam  Count    = Number of entries in that table
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t canRegisterBroadcast (bmCallbackEntry_t *Commands, UInt16 Count) {

    UInt16 NumberOfModules = bmGetNumberOfModules();
    Error_t Status;
    UInt16 i;

    for (i=0; i < NumberOfModules; i++) {
        Status = canRegisterMessages (bmGetModuleID2(i), Commands, Count);
        if (Status < 0) {
            return (Status);
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Initializes this module
 *
 *      This function initializes this module. It allocates memory for the
 *      module table and a callback lookup table for each module. The size
 *      of the module table is speicified by NumberOfModules. An inital
 *      size for the per-module message lookup tables is allocated.
 *
 *      This function is called once during startup!
 *
 *  \iparam  NumberOfModules = Number of function modules
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t canInitDispatcher (UInt16 NumberOfModules) {

    UInt16 i;

    Modules = calloc (NumberOfModules, sizeof(bmCallbackTable_t));
    if (Modules == NULL) {
        return (E_MEMORY_FULL);
    }
    for (i=0; i < NumberOfModules; i++) {

        Modules[i].Entries = calloc (TABLE_INITIAL_SIZE, sizeof(bmCallbackEntry_t));
        Modules[i].Size    = TABLE_INITIAL_SIZE;
        Modules[i].Count   = 0;

        if (Modules[i].Entries == NULL) {
            Modules[i].Size = 0;
            return (E_MEMORY_FULL);
        }
        ModuleCount++;
    }
    NodeAddress = bmGetNodeAddress();

    return (NO_ERROR);
}

//****************************************************************************/
