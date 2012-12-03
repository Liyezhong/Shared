/****************************************************************************/
/*! \file bmDebug.c
 *
 *  \brief Debug Function
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains function to print messages, errors or other
 *      textual informations to the debug console. It works only, if the
 *      standard output can be redirected to a console.
 *
 *      Calls to functions of this module can be stay in code even in non-
 *      debug builds, since they are replaced by empty inline functions
 *      in this case. Activation/deactivation of this module is controlled
 *      by the compiler define DEBUG. If this symbol is omitted, this
 *      module is automatically skipped.
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
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Global.h"
#include "halInterrupt.h"
#include "bmError.h"
#include "bmCan.h"
#include "bmTime.h"
#include "bmScheduler.h"
#include "bmSystem.h"
#include "bmStrings.h"
#include "bmDebug.h"

#ifdef DEBUG    // include this file only if DEBUG is defined

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Structure to hold a CAN ID lookup table
typedef struct {
    const bmCanIdNames_t *Names;    //!< Table of CAN ID/string pairs
    UInt16 Count;                   //!< Size of table
} dbgCanIdNameDescriptor_t;

//! Structure to hold an event lookup table
typedef struct {
    const ErrorString_t *Names;     //!< Table of event code/string pairs
    UInt16 Count;                   //!< Size of table
} dbgEventNameDescriptor_t;

//! Structure to assign a describting string to an exception reason
typedef struct {
    const UInt32 Value;             //!< Exception reason value
    const char *Name;               //!< Exception name 
} dbgExceptionDescriptor_t;

//! Exception callback function pointer
extern halExceptionHandler_t halExceptionNotifier;


//****************************************************************************/
// Private Variables
//****************************************************************************/

static dbgCanIdNameDescriptor_t *CanIdLookupTable;  //!< CAN ID string table
static dbgEventNameDescriptor_t *EventLookupTable;  //!< Event string table

static UInt16 CanIdTableSize  = 0;      //!< Size of CanIdLookupTable
static UInt16 EventTableSize  = 0;      //!< Size of EventLookupTable
static UInt16 EventTableIndex = 0;      //!< EventLookupTable entries

static Bool PrintTimeStamp = TRUE;      //!< Enable time stamp printing
static Bool Initialized    = FALSE;     //!< Module is initialized


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static const char* dbgSearchCanIdName (UInt32 CanID, UInt16 Channel);


/*****************************************************************************/
/*!
 *  \brief   Register CAN-ID strings for debugging
 *
 *      Register CAN-ID strings to the debug module to enable printing of
 *      CAN-IDs on the debug console textual istead of numerical. Each
 *      function module can register it's own set of private CAN-IDs.
 *      It isn't a must to use this functionality, but debugging is
 *      more user friendly using it. The caller has to supply the module
 *      ID and a table of CAN-ID/string pairs.
 *
 *      Attention: The module must have registered itself to the scheduler
 *      before calling this function. The supplied table is not copied,
 *      but assumed to be constant.
 *
 *  \iparam  ModuleID  = Module ID
 *  \iparam  Table     = Table of ID/string pairs
 *  \iparam  TableSize = Number of events in table
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void dbgRegisterCanIdNames (
    UInt16 ModuleID, const bmCanIdNames_t *Table, UInt16 TableSize) {

    UInt16 TaskID = bmGetTaskID(ModuleID, 0);

    if (TaskID != TASK_ID_UNKNOWN) {
        Int32 Index = bmGetModulIndex(TaskID);

        if (Index >= 0 && Index < CanIdTableSize) {
            CanIdLookupTable[Index].Names = Table;
            CanIdLookupTable[Index].Count = TableSize;
        }
    }
}


/*****************************************************************************/
/*!
 *  \brief   Print CAN-ID name list
 *
 *      Prints all registered CAN-ID names side-by-side with the numerical
 *      representation and prepended with the define keyword on the debug
 *      console. This can be used to copy it to a text editor and insert
 *      it into another project.
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void dbgPrintCanIdNameList (void) {

    UInt16 i, k, t;

    printf("// List of CAN Messages:\n");

    for (i = 0; i < CanIdTableSize; i++) {
        const bmCanIdNames_t *Table = CanIdLookupTable[i].Names;
        const UInt16 ModuleID = bmGetModuleID2(i);

        printf("// CAN IDs for module with module ID %d\n", ModuleID);
        for (k=0; k < CanIdLookupTable[i].Count; k++) {
            printf ("#define %-28s 0x%08lx\n", Table[k].Name, Table[k].CanID);
            for (t=0; t < 50; t++) {
                halShortDelay(100);
            }
        }
        printf("\n");
    }

    printf ("// List of Error/Info Codes:\n");

    for (i=0; i < EventTableSize; i++) {
        const ErrorString_t *Names = EventLookupTable[i].Names;

        for (k=0; k < EventLookupTable[i].Count; k++) {
            printf ("#define %-28s 0x%08x\n", Names[k].ErrText, Names[k].ErrCode);
            for (t=0; t < 50; t++) {
                halShortDelay(100);
            }
        }
        printf("\n");
    }
}


/*****************************************************************************/
/*!
 *  \brief   Register event strings for debugging
 *
 *      Register event strings (errors, warnings, infos) to the debug module
 *      to enable printing of events on the debug console textual istead of
 *      numerical. Each function module can register it's own set of local
 *      events. It isn't a must to use this functionality, but debugging is
 *      more user friendly using it. The caller has to supply the table of 
 *      event code/string pairs.
 *
 *      Attention: The module must have registered itself to the scheduler
 *      before calling this function. The supplied table is not copied,
 *      but assumed to be constant.
 *
 *  \iparam  Table     = Table of ID/string pairs
 *  \iparam  TableSize = Number of events in table
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void dbgRegisterEventNames (const ErrorString_t *Table, UInt32 TableSize) {

    if (EventTableIndex < EventTableSize) {
        EventLookupTable[EventTableIndex].Names = Table;
        EventLookupTable[EventTableIndex].Count = TableSize;
        EventTableIndex++;
    }
}


/*****************************************************************************/
/*!
 *  \brief   Print CAN message on debug console
 *
 *      Prints the supplied CAN message on the debug console. If the textual
 *      representaion of the CAN ID is registered, the CAN ID is printed in
 *      textual format, else it is printed as a 32 bit hexadecimal number.
 *      The following informations are printed:
 *
 *      - Timestamp
 *      - CAN ID
 *      - Channel number
 *      - Data bytes (0...8)
 *
 *  \iparam  Message   = CAN message
 *  \iparam  Direction = Direction (r: read, w: write)
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void dbgPrintCanMessage (CanMessage_t *Message, char Direction) {

    UInt8 *Data = Message->Data;
    const char *MsgName;
    char Buffer[32];
    char Format[64];
    UInt16 Channel;
    UInt16 i;

    Channel = (Message->CanID & CANiD_MASK_CHANNEL) >> CANiD_SHIFT_CHANNEL;
    MsgName = dbgSearchCanIdName(Message->CanID, Channel);

    if (MsgName == NULL) {
        snprintf (Buffer, sizeof(Buffer), "Message 0x%08lx", Message->CanID);
        MsgName = Buffer;
    }
    if (Direction == 'r') {
        strcpy (Format, "[Ch%d] Get %-26s ( ");
    }
    else {
        strcpy (Format, "[Ch%d] Put %-26s ( ");
    }
    for (i=0; i < Message->Length; i++) {
        strcat (Format, "%02x ");
        Message->Data[i] &= 0xFF;
    }
    strcat (Format, ")");

    dbgPrint (Format, Channel, MsgName,
        Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6], Data[7]);
}


/*****************************************************************************/
/*!
 *  \brief   Searches CAN-ID string in lookup table
 *
 *      Searches the registered lookup table for the supplied CAN-ID and
 *      returns the CAN-ID string for printing on the debug console. Only
 *      the lookup table of the module associated to Channel are searched.
 *      If no lookup table for that module is registered, or the searched
 *      CAN ID is not in this table, NULL is returned.
 *
 *  \iparam  CanID   = CAN ID
 *  \iparam  Channel = Communication channel
 *
 *  \return  String representation of CAN ID or NULL
 *
 ****************************************************************************/

static const char* dbgSearchCanIdName (UInt32 CanID, UInt16 Channel) {

    Int32 Index = bmGetModulIndex(Channel);
    const bmCanIdNames_t *LookupTable;
    Int32 TableSize;

    if (Index < CanIdTableSize) {

        LookupTable = CanIdLookupTable[Index].Names;
        TableSize   = CanIdLookupTable[Index].Count;

        // mask all address informations out of the CAN id
        CanID &= ~(CANiD_MASK_ADDRESS | CANiD_MASK_CHANNEL);

        for (Index=0; Index < TableSize; Index++) {
            if (LookupTable[Index].CanID == CanID) {
                return (LookupTable[Index].Name);
            }
        }
    }
    return (NULL);
}


/*****************************************************************************/
/*!
 *  \brief   Print event on debug console
 *
 *      Prints supplied event (error, warning, info) on the debug console.
 *      If the textual representation of the event code is registered, the
 *      event is printed in text format; else it is printed as a number.
 *      The print out is supplemented by a time stamp and the event class,
 *      which can be Info, Warning, Error, or Fatal.
 *
 *  \iparam  Channel  = Channel number
 *  \iparam  ErrCode  = Event code
 *  \iparam  ErrState = Event state (asserted, removed)
 *  \iparam  ErrInfo  = Additional data
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void dbgPrintError (
        UInt16 Channel, Error_t ErrCode, UInt16 ErrState, UInt32 ErrInfo) {
    
    const char *Classes[] = { "INFO", "ERROR", "WARNING", "FATAL" };
    const char *ClassString;
    const char *StateString;
    UInt16 ErrorClass;
    UInt16 i, k;

    ErrorClass  = (ErrCode & ERRCODE_MASK_CLASS) >> 28;
    ClassString = (ErrorClass < ELEMENTS(Classes)) ? Classes[ErrorClass] : "?";
    StateString = (ErrState) ? "" : "- solved";

    for (i=0; i < EventTableSize; i++) {
        const ErrorString_t *Names = EventLookupTable[i].Names;

        for (k=0; k < EventLookupTable[i].Count; k++) {
            if (Names[k].ErrCode == ErrCode) {
                dbgPrint ("[Ch%d] *** %s: %s (%ld) %s",
                    Channel, ClassString, Names[k].ErrText, ErrInfo, StateString);
                return;
            }
        }
    }
    dbgPrint ("[Ch%d] *** %s: %lx (%ld) %s",
        Channel, ClassString, ErrCode, ErrInfo, StateString);
}


/*****************************************************************************/
/*!
 *  \brief   Print node state on debug console
 *
 *      Prints the supplied node state in textual format on debug console.
 *      If the supplied node state is invalid "UNDEFINED" is printed.
 *
 *  \iparam  NodeState = Node state
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void dbgPrintNodeState (UInt16 NodeState) {

    static const bmCanIdNames_t NodeStateNames[] = {
        { NODE_STATE_UNDEFINED, "UNDEFINED" },
        { NODE_STATE_BOOTING,   "BOOTING"   },
        { NODE_STATE_STARTUP,   "STARTUP"   },
        { NODE_STATE_IDENTIFY,  "IDENTIFY"  },
        { NODE_STATE_CONFIGURE, "CONFIGURE" },
        { NODE_STATE_NORMAL,    "NORMAL"    },
        { NODE_STATE_SHUTDOWN,  "SHUTDOWN"  },
        { NODE_STATE_ASSEMBLY,  "ASSEMBLY"  },
        { NODE_STATE_STANDBY,   "STANDBY"   }
    };
    UInt16 i;

    for (i=0; i < ELEMENTS(NodeStateNames); i++) {
        if (NodeStateNames[i].CanID == NodeState) {
            dbgPrint ("Node state = %s", NodeStateNames[i].Name);
            return;
        }
    }
    dbgPrint ("Node state = %s", NodeStateNames[0].Name);
}


/*****************************************************************************/
/*!
 *  \brief   Print exception info
 *
 *      Prints the reason and address of an exception. The exception info
 *      must be passed as parameter Exception. This function is called by
 *      the HAL exception handler, provided that the exception user call-
 *      back is registered to point to this function.
 *
 *  \iparam  Exception = Exception info structure
 *
 *  \return  Nothing
 *
 ****************************************************************************/

static void dbgPrintException (halExceptionInfo_t *Exception) {

    dbgExceptionDescriptor_t Source[] = {
        { EXCEPTION_NMI,          "Nonmaskable interrupt" },
        { EXCEPTION_HARDFAULT,    "Hard fault"            },
        { EXCEPTION_MEMORY,       "Memory fault"          },
        { EXCEPTION_BUS_ERROR,    "Bus error"             },
        { EXCEPTION_USE_ERROR,    "Usage error"           }
    };
    dbgExceptionDescriptor_t Reasons[] = {
        { EXCPT_DIVIDE_BY_ZERO,   "Divide by zero"        },
        { EXCPT_UNALIGNED_ACCESS, "Unaligned access"      },
        { EXCPT_UNKNOWN_OPCODE,   "Unknown instruction"   },
        { EXCPT_INVALID_PC,       "Invalid PC"            },
        { EXCPT_CODE_ACCESS,      "Code access violation" },
        { EXCPT_DATA_ACCESS,      "Data access violation" },
        { EXCPT_DATA_BUS_ERROR,   "Data bus error"        },
        { EXCPT_STACKING_ERROR,   "Stacking problem"      }
    };
    const char *ReasonString = "Unspecific failure";
    const char *SourceString = "Unspecific exception";
    UInt32 i;

    for (i=0; i < ELEMENTS(Source); i++) {
        if (Exception->ExceptionID == Source[i].Value) {
            SourceString = Source[i].Name;
            break;
        }
    }
    for (i=0; i < ELEMENTS(Reasons); i++) {
        if (Exception->FaultReason & Reasons[i].Value) {
            ReasonString = Reasons[i].Name;
            break;
        }
    }
    dbgPrint ("-----------------------------");
    dbgPrint ("EXCEPTION: %s", SourceString);
    dbgPrint ("- Description : %s", ReasonString);
    dbgPrint ("- Code address: 0x%08x", Exception->FaultAddress);
    dbgPrint ("- Data address: 0x%08x", Exception->FaultAccess);
    dbgPrint ("- Reason code : 0x%08x", Exception->FaultReason);
    dbgPrint ("-----------------------------");
}


/*****************************************************************************/
/*!
 *  \brief   Print parameters on debug console
 *
 *      Prints the supplied variable length parameter list on the debug
 *      console. Usage is the same as for the C printf function. The
 *      only difference is, that this function automatically prepends a
 *      timestamp before the string and appends a newline at the end of
 *      the string.
 *
 *  \iparam  Format = Format string (as for printf)
 *  \iparam  ...    = Variable number of arguments
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void dbgPrint (char *Format,...) {

    UInt32 Timestamp = 0;
    va_list Marker;

    va_start (Marker, Format);

    if (PrintTimeStamp) {
        if (Initialized) {
            Timestamp = bmGetTime();
        }
        printf  ("%06ldms: ", Timestamp);
    }
    vprintf (Format, Marker);                               //lint !e530
    printf ("\n");
    va_end (Marker);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize debug print module
 *
 *      Initialize this debug module by allocating memory to hold the lookup
 *      tables for the supplied number of modules. This function must be
 *      called once during startup, before using any of the functions in
 *      this module.
 *
 *  \iparam  ModuleCount = number of function modules
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

void dbgInitializeModule (UInt16 ModuleCount) {

    const ErrorString_t *ErrorStrings;
    const bmCanIdNames_t *CanIdStrings;
    UInt32 TableSize;

    if (!Initialized) {
        CanIdLookupTable = calloc (ModuleCount, sizeof(dbgCanIdNameDescriptor_t));
        if (CanIdLookupTable == NULL) {
            return;
        }
        CanIdTableSize = ModuleCount;
    
        EventLookupTable = calloc (ModuleCount, sizeof(dbgEventNameDescriptor_t));
        if (EventLookupTable == NULL) {
            return;
        }
        EventTableSize  = ModuleCount;
        EventTableIndex = 0;

        TableSize = bmGetCanIdStrings (&CanIdStrings);
        dbgRegisterCanIdNames(0, CanIdStrings, TableSize);

        TableSize = halGetErrorStrings (&ErrorStrings);
        dbgRegisterEventNames (ErrorStrings, TableSize);
    
        TableSize = bmGetErrorStrings (&ErrorStrings);
        dbgRegisterEventNames(ErrorStrings, TableSize);
    
        // Register exception user callback
        halExceptionNotifier = dbgPrintException;
    }
    Initialized = TRUE;
}

//****************************************************************************/

#endif
