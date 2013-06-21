/****************************************************************************/
/*! \file fmSimulate.c
 * 
 *  \brief Function module to simulate hardware input patterns
 *
 *  $Version: $ 0.1
 *  $Date:    $ 06.08.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *         This module simulates, in combination with the HAL simulation,
 *         data input from hardware. It can simulate CAN receive messages
 *         and digital/analog data input. Data patterns can be defined
 *         including data/messages over time. Several test cases are
 *         defined, to test several aspects of the slave software. 
 *         More tests can be added.
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Global.h"
#include "halLayer.h"
#include "Basemodule.h"
//#include "bmSystem.h"
#include "fmAnaInput.h"
#include "fmAnaOutput.h"
#include "fmDigiInput.h"
#include "fmDigiOutput.h"
#include "fmSimulate.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define MODULE_VERSION              0x0001

/*! Field numbers for message descriptor strings */
typedef enum {
    CSV_FIELD_DURATION = 0,         //!< Test step duration field number 
    CSV_FIELD_CHANNEL  = 1,         //!< CAN channel field number
    CSV_FIELD_CANiD    = 2,         //!< CAN ID field numer
    CSV_FIELD_DATA_1   = 3,         //!< CAN data byte 1 field number
    CSV_FIELD_DATA_8   = 11         //!< CAN data byte 8 field number
} CsvFieldIndex_t;

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! CAN test message simulator state variables */
typedef struct {
    const CanLoopbackData_t *Data;
    UInt16 Count;
    UInt16 Index;
    UInt32 Delay;
    UInt32 StartTime;
} CanLoopbackTest_t;

/*! CAN test message descriptor table */
typedef struct {
    const CanLoopbackData_t *Messages;
    UInt16 TableSize;
    UInt16 InputPattern;
} TestCaseMessages_t;

//****************************************************************************/
// Private Variables 
//****************************************************************************/

static CanLoopbackTest_t CanLoopback = {0};
static Bool WriteTestPatternToFile = 0;

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static Error_t simParseMessageString (CanLoopbackData_t *TestData, char *String);
static Error_t simModuleTask (UInt16 Instance);


/*****************************************************************************/
/*! 
 *  \brief   Read CAN test messages from external file
 *
 *      Test patterns can be read from an external file into a data structure. 
 *      This allows changing test patterns without recompilation and flashing 
 *      firmware.
 * 
 *  \iparam  Patterns = Message descriptor table
 *  \iparam  Count    = Number of entries in table
 *
 *  \return  Nothing
 *
 *****************************************************************************/

static UInt16 simReadCanTestPatterns (
                FILE *fp, CanLoopbackData_t *MessageTable, UInt16 TableSize) {

    UInt16 MessageCount = 0;
    UInt16 LineNum = 0;
    UInt16 NoSpace;
    char Buffer[256];
    char *Comment;
    
    while (fgets (Buffer, sizeof(Buffer), fp) != NULL) {
        
        if ((Comment = strstr (Buffer, "//")) != NULL) {
            *Comment = '\0';
        }
        NoSpace = strspn (Buffer, " \t\n");
        
        if (Buffer[NoSpace]) {
            UInt16 Fields = 
                simParseMessageString (&MessageTable[MessageCount], Buffer);
            
            if (Fields <= CSV_FIELD_CANiD) {
                dbgPrint ("- Error in line %d of message file", LineNum+1);
            }
            else if (MessageCount >= TableSize) {
                dbgPrint ("- Message beyond line %d not loaded", LineNum+1);
                break;
            }
            else {
                MessageCount++;
            }
        }
        LineNum++;           
    }
    dbgPrint ("- %d test messages loaded", MessageCount);
    return (MessageCount);
} 


/*****************************************************************************/
/*! 
 *  \brief   Write CAN test messages to external file
 *
 *      Test patterns can be written to an external file to be read back
 *      later. This allows changing test patterns without recompilation
 *      and flashing the firmware.
 * 
 *  \iparam  Patterns = Message descriptor table
 *  \iparam  Count    = Number of entries in table
 *
 *  \return  Nothing
 *
 *****************************************************************************/

UInt16 simWriteTestPatterns (const CanLoopbackData_t *Patterns, UInt16 Count) {

    const char *Filename = "test.txt";
    FILE *fp;
    int i, k;

    if ((fp = fopen(Filename, "w+")) != NULL) {
        dbgPrint ("Writing test messages to %s", Filename);
        
        for (k=0; k < Count; k++) {
            
            fprintf (fp, " %u,", Patterns->Delay);
            fprintf (fp, " %u,", Patterns->Channel);
            fprintf (fp, " 0x%08lx", Patterns->Message.CanID);
            
            for (i=0; i < Patterns->Message.Length; i++) {
                fprintf (fp, ", 0x%02x", Patterns->Message.Data[i]);
            }
            fprintf (fp, "\n");
            Patterns++;
        }
        fclose (fp);
        dbgPrint ("- %d messages written", Count);        
        return (Count);
    }
    return (0);  
}


/*****************************************************************************/
/*! 
 *  \brief   Load test patterns from external file
 *
 *      Loads test patterns from external file and registeres them to the
 *      simulator.
 * 
 *  \iparam  Pathname = Pathname to load pattern from
 * 
 *  \return  Number of test patterns in file
 *
 *****************************************************************************/

UInt16 simImportCanTestPatterns (char *Pathname) {

    CanLoopbackData_t *MessageTable1, *MessageTable2;
    const UInt16 TableSize = 50;
    UInt16 MessageCount = 0;
    FILE *fp = stdin;

    if ((fp = fopen(Pathname, "r")) != NULL) {
        dbgPrint ("Loading test messages from %s", Pathname);
            
        MessageTable1 = calloc (TableSize, sizeof(CanLoopbackData_t));
        if (isPtrNull(MessageTable1)) {
            dbgPrint ("- No memory for test patterns");
            return (0);
        }
        MessageCount = simReadCanTestPatterns (fp, MessageTable1, TableSize);
        fclose (fp);
    
        MessageTable2 = 
            realloc (MessageTable1, MessageCount * sizeof(CanLoopbackData_t));
                            
        if (!isPtrNull(MessageTable2)) {
            MessageTable1 = MessageTable2;
        }        
        simCanRegisterTestMessages (MessageTable1, MessageCount);
    }    
    return (MessageCount);
}


/*****************************************************************************/
/*! 
 *  \brief   Parse a message descriptor string
 *
 *      Parses the supplied String containing a message descriptor, 
 *      converts it to binary and returns the result in TestData.
 * 
 *  \iparam  TestData = Structure to return the result
 *  \iparam  String   = Message descriptor string
 *
 *  \return  Nothing
 *
 *****************************************************************************/

static Error_t simParseMessageString (CanLoopbackData_t *TestData, char *String) {

    UInt16 Length = 0;
    UInt16 Field  = 0;
    UInt32 Value;
    
    String--;
    do {    
        Value = strtoul (String+1, &String, 0);
        
        if (Field == CSV_FIELD_DURATION) {
            TestData->Delay = Value;
        }
        else if (Field == CSV_FIELD_CHANNEL) {
            TestData->Channel = Value;
        }
        else if (Field == CSV_FIELD_CANiD) {
            TestData->Message.CanID = Value;
        }
        else if (Field <= CSV_FIELD_DATA_8) {
            TestData->Message.Data[Length++] = Value;
        }
        String += strspn (String, " \t\n");
        if (*String != ',' && *String != '\0') {
            return (0);
        }
        Field++;
        
    } while (*String != '\0' && Field <= CSV_FIELD_DATA_8);
     
    TestData->Message.Length = Length;
    
    while (Length < 8) {
        TestData->Message.Data[Length++] = 0;
    }      
    return (Field);
}


/*****************************************************************************/
/*! 
 *  \brief   Module Task Function
 *
 *      This task is responsible to inject CAN messages into the input stream
 *      to simulate the receive of messages. The messages are taken from a
 *      test descriptor, which includes not only the message, but also the
 *      CAN channel and a time offset. The next message from the descriptor
 *      is taken, when the time since the last message has expired. 
 * 
 *      This function is called by the task scheduler periodically. 
 * 
 *  \iparam  Instance = Instance number of this module
 *
 *  \return  State of instance or (negative) error code
 *
 *****************************************************************************/

#include "halConsole.h"

static char* simGetUserCommand (void) {

    static UInt16 iCount = 0;
    static char iBuffer[80];
    UInt8 inKey = 0;

    while (halConsoleRead(&inKey) > 0) {
        // Handle normal characters
        if (iCount < ELEMENTS(iBuffer) - 1) {
            if (inKey >= 0x20) {
                iBuffer[iCount++] = inKey;
                iBuffer[iCount] = 0; 
            }
        }
        // Handle the Backspace key
        else if (inKey == 0x08) {
            if (iCount > 0) {
                iCount--;
            }
            halConsoleWrite(0x08);
            halConsoleWrite(0x20);
        }
        // Handle the Escape key (delete line)
        else if (inKey == 0x1b || inKey == '\r') {
            inKey = '\n';
            iCount = 0;
        }
        halConsoleWrite (inKey);
        }
        if (inKey == '\r') {
            return (iBuffer);
        }
    }
    return (NULL);
}


static Error_t simModuleTask (UInt16 Instance) {

    CAN_MESSAGE_t Message;
    UInt32 Channel;
    char *Command;
    
    if (canLoopback.Index < canLoopback.Count) {
    
        if (bmTimeExpired(canLoopback.StartTime) >= canLoopback.Delay) {
            
            Message = canLoopback.Data[canLoopback.Index].Message;    
            Channel = canLoopback.Data[canLoopback.Index].Channel;
            
            if ((Message.CanID & CANiD_MASK_BROADCAST) == 0) {
                Message.CanID &= ~(CANiD_MASK_ADDRESS | CANiD_MASK_CHANNEL);
                
                Message.CanID |= CANiD_MASK_ADDRESS & bmGetNodeAddress();
 
                Message.CanID |= 
                    (Channel & CANiD_MAX_CHANNEL) << CANiD_SHIFT_CHANNEL;
            }
            Message.CanID &= ~CANiD_MASK_BROADCAST;
                            
            if (canLoopback.Data[canLoopback.Index].Delay) {
                halCanInject(&Message);
            }
            if (++canLoopback.Index < canLoopback.Count) {    
                canLoopback.Delay += canLoopback.Data[canLoopback.Index].Delay;
            }
        }
    }
    Command = simGetUserCommand();
    if (Command) {
        printf ("Processing command: %s\n", Command);
    }

    return (MODULE_STATE_READY);
}


/*****************************************************************************/
/*! 
 *  \brief   Register receive messages
 *
 *      This function can be used to register CAN receive messages, by
 *      supplying a message descriptor table and the table size. The
 *      processing of the registered messages is done by the task
 *      function above. 
 * 
 *      ATTENTION: The test descriptor must be constant; that means it 
 *      must persist also after calling this function.   
 * 
 *  \iparam  MessageDescriptor = Message descriptor table
 *  \iparam  MessageTableSize  = Number of entries in table
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

Error_t simCanRegisterTestMessages (
        const CanLoopbackData_t *MessageDescriptor, UInt16 MessageTableSize) {

    if (!isPtrNull(MessageDescriptor)) {
        
        if (WriteTestPatternToFile) {
            simWriteTestPatterns (MessageDescriptor, MessageTableSize);
        }
        canLoopback.Data  = MessageDescriptor;
        canLoopback.Count = MessageTableSize;
        canLoopback.Index = 0;
        canLoopback.Delay = canLoopback.Data[0].Delay;
        canLoopback.StartTime = 0; //bmGetTime();
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Initializes this function module
 *
 *      It registers itself to the task scheduler and calls the test
 *      case selection function.
 *
 *      This function is called once during startup by the base module. 
 *  
 *  \iparam  ModuleID  = Module identifier
 *  \iparam  Instances = Number of instances of this module
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t simInitializeModule (UInt16 ModuleID, UInt16 Instances) {

    static MODULE_INTERFACE_t Interface = {
        simModuleTask, NULL, NULL
    };
    Error_t Status;

    // register function module to the scheduler    
    Status = bmRegisterModule (ModuleID, Instances, &Interface);
    if (Status < 0) {
        return (Status);
    }    
    return (NO_ERROR);
}

//****************************************************************************/

