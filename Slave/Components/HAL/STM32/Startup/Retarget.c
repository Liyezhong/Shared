/****************************************************************************/
/*! \file Retarget.c
 * 
 *  \brief  Standard I/O redirection module
 *
 *  $Version: $ 0.1
 *  $Date:    $ 09.12.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module replaces several functions used by the standard I/O
 *      library. This allows to use printf to display debug informations
 *      on the debug console.
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
#include "Global.h"
#include "halConsole.h"
#include "halCan.h"
#include "bmCan.h"
#include "bmUtilities.h"

#pragma import(__use_no_semihosting)


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

/* Standard IO device handles */
#define STDIN             0x8001    // Standard input stream
#define STDOUT            0x8002    // Standard output stream
#define STDERR            0x8003    // Standard error stream
#define ITM_STREAM        0x8010    // ITM stream

//****************************************************************************/
// Private Variables 
//****************************************************************************/

/* Standard IO device name defines. */
const char __stdin_name[]  = "STDIN";
const char __stdout_name[] = "STDOUT";
const char __stderr_name[] = "STDERR";


//****************************************************************************/
// I/O Retargetting Code 
//****************************************************************************/
                                
int _sys_open (const char *name, int openmode) {  

    if (!strcmp (name, "itm")) {
        return (ITM_STREAM);
    }    
    return(0);
}

/*****************************************************************************/

// A positive number representing the number of characters not written (so 
// any nonzero return value denotes a failure of some sort).
// A negative number indicating an error.

int _sys_write (int Handle, const UInt8 *Buffer, UInt32 Length, int mode) {

    if (Handle == ITM_STREAM) {
        while (Length--) {
            if (*Buffer == '\n') {
                //halConsoleWrite ( '\r');
            }       
            halConsoleWrite (*Buffer++);
        }
        return(0);
    }
    return (0);
}

/*****************************************************************************/

// The number of characters not read (that is, len - result were read).
// An error or EOF indication:

int _sys_read (int Handle, UInt8 *Buffer, UInt32 Size, int Mode) {

    if (Handle == ITM_STREAM) {
        if (Size--) {
            do {
                *Buffer = halConsoleRead();
            } while (*Buffer == 0);
        }
        return (Size);
    }
    return (0);
}

/*****************************************************************************/

int _sys_close (int Handle) {

    return (0);
}

/*****************************************************************************/

int _sys_istty (int Handle) {

    if (Handle > 0x8000) {
        return (1);
    }
    return (0);
}

/*****************************************************************************/

void _ttywrch (int c) {

    fputc (c, stdout);
    fflush (stdout);
}

/*****************************************************************************/

int _sys_seek (int Handle, long pos) {

    return (0);
}

/*****************************************************************************/

int _sys_ensure (int Handle) {

    return (-1);
}

/*****************************************************************************/

long _sys_flen (int Handle) {

    return (0);
}

/*****************************************************************************/

int _sys_tmpnam (char *name, int sig, unsigned maxlen) {

    return (1);
}

/*****************************************************************************/

char *_sys_command_string (char *Command, int Length) {

    return (Command);
}

/*****************************************************************************/
/*!
 *  \brief   System exit function
 *
 *      When firmware is terminated this function will be called.
 *
 *      The ReturnCode is send as CAN-event message if CAN bus is useable.
 *      Then a hardware reset is issued, which leads firmware to reenter the
 *      bootloader.
 *
 *  \iparam  ReturnCode = (negative) error code
 *
 ****************************************************************************/
void _sys_exit (int ReturnCode) {

    // send CAN event if bus is ready
    static const UInt32 EventID[8] = {
        MSG_EVENT_INFO, MSG_EVENT_ERROR, MSG_EVENT_WARNING, MSG_EVENT_FATAL
    };
    Handle_t CanHandle = halCanOpen(HAL_CAN_SYSTEM, 0, NULL, FALSE);
    CanMessage_t Message;
    Message.CanID = EventID[(((ReturnCode) & ERRCODE_MASK_CLASS)  >> 28)];
    Message.Length = 6;
    bmSetMessageItem (&Message, (((ReturnCode) & ERRCODE_MASK_MODULE) >> 16), 0, 2);
    bmSetMessageItem (&Message, ((ReturnCode) & ERRCODE_MASK_NUMBER), 2, 2);
    bmSetMessageItem (&Message, 0, 4, 2);
    if (CanHandle > 0) {
        volatile Int32 i;
        halCanWrite (CanHandle, &Message);
        // give some time to transmit the message
        for (i=0; i < 300000; i++);
    }

    // Do hardware reset (should never return)
    halHardwareReset();

    // stop, if reset failed
    for(;;);
}

/*****************************************************************************/
