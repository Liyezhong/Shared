/****************************************************************************/
/*! \file bmStrings.h
 *
 *  \brief Debug string table definition
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module defines text strings to be used during debug only. It
 *       supplies ID-to-string conversion tables to the debug module, to
 *       enable the display of user friendly names instead of numerical
 *       IDs on the debug console. The IDs to be converted are:
 *
 *       - CAN-ID names
 *       - Event messages (Errors, Warnings, Infos)
 *
 *       Each function module can define it's own conversion tables for
 *       it's local defined identifiers, but is also free to not define
 *       strings. In this case IDs are displayed in numerical format.
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

#ifndef BM_STRINGS_H
#define BM_STRINGS_H

//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Structure to hold a CAN ID number/string pair
typedef struct {
    UInt32 CanID;           //!< CAN ID (numerical)
    const char *Name;       //!< CAN ID name as a string
} bmCanIdNames_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

UInt32 bmGetErrorStrings (const ErrorString_t *ErrorStrings[]);
UInt32 bmGetCanIdStrings (const bmCanIdNames_t *CanIdStrings[]);

//****************************************************************************/

#endif /*BM_STRINGS_H*/
