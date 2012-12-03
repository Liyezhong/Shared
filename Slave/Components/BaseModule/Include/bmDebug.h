/****************************************************************************/
/*! \file bmDebug.h
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
 *      by the compiler define DEBUG. If this symbol is defined, printing
 *      to the console is done. If it isn't defined, printing is disabled.
 *
 *  \b Company:
 *
 *      Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef BM_DEBUG_H
#define BM_DEBUG_H

#include "bmStrings.h"

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/


//! Structure to hold an event number/string pair
typedef struct {
    Error_t ErrorCode;      //!< Event code (numerical)
    const char *ErrorText;  //!< Event description as a string
} dbgErrorText_t;


//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//@{ Start of doxygen group

#ifndef DEBUG
//! Empty debug function macro used in release code
#define dbgRegisterCanIdNames(ModuleID,Table,Count)     {}
#define dbgRegisterEventNames(Table,Count)              {}
#define dbgPrintError(Channel,ErrCode,ErrState,ErrInfo) {}
#define dbgPrintCanMessage(Message,Direction)           {}
#define dbgPrintNodeState(Mode)                         {}
#define dbgPrint(Format,...)                            {}
#define dbgInitializeModule(ModuleCount)                {}
#else

void dbgPrintError (
        UInt16 Channel, Error_t ErrCode, UInt16 ErrState, UInt32 ErrInfo);

void dbgRegisterCanIdNames (
        UInt16 ModuleID, const bmCanIdNames_t *CanIdNames, UInt16 Count);

void dbgRegisterEventNames (const ErrorString_t *Table, UInt32 Count);
void dbgPrintCanMessage    (CanMessage_t *Message, char Direction);
void dbgPrintNodeState     (UInt16 NodeState);
void dbgInitializeModule   (UInt16 ModuleCount);
void dbgPrint              (char *Format, ...);
#endif 

//****************************************************************************/

#endif /*BM_DEBUG_H*/
