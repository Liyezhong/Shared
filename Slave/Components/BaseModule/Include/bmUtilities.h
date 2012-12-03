/****************************************************************************/
/*! \file bmUtilities.h
 *
 *  \brief Common utility functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *         This module contains common utility function not associated
 *         to a special module.
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

#ifndef BM_UTILITIES_H
#define BM_UTILITIES_H

//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

#define MODUL_INSTANCES         0xFFFF  //!< Request number of module instances


//****************************************************************************/
//! The board info block contains informations about the board. 
//****************************************************************************/

typedef struct {
    UInt32 Signature;           //!< Signature to detect info block
    UInt8  NodeType;            //!< Node type
    UInt8  NodeClass;           //!< Node class
    UInt8  VersionMajor;        //!< Major hardware version number
    UInt8  VersionMinor;        //!< Minor hardware version number
    UInt8  ProductionYear;      //!< Year of production
    UInt8  ProductionMonth;     //!< Month of production
    UInt8  ProductionDay;       //!< Day of production
    UInt8  EndTestYear;         //!< Year of end-test
    UInt8  EndTestMonth;        //!< Month of end-test
    UInt8  EndTestDay;          //!< Day of end-test
    UInt8  EndTestResult;       //!< End-test result (ok, failed, open)
    char   BoardName[16];       //!< Board name string
    char   SerialNum[32];       //!< Serial number string
    UInt32 Checksum;            //!< Checksum to verfiy info block
} bmBoardInfoBlock_t;

//****************************************************************************/
//! The bootloader info block contains informations about the bootloader
//****************************************************************************/

typedef struct {
    UInt32 Signature;           //!< Signature for valid info block
    UInt8  VersionMajor;        //!< Major hardware version number
    UInt8  VersionMinor;        //!< Minor hardware version number
    UInt8  CreationYear;        //!< Year of production
    UInt8  CreationMonth;       //!< Month of production
    UInt8  CreationDay;         //!< Day of production
    UInt32 Checksum;            //!< Checksum to verfiy info block
} bmBootInfoBlock_t;

//****************************************************************************/
//! Structure to access the various fields of a CAN-ID
//****************************************************************************/

typedef struct {
    UInt32 Master:1;            //!< Master/slave bit
    UInt32 NodeType:7;          //!< Node type
    UInt32 NodeIndex:4;         //!< Node index
    UInt32 Channel:7;           //!< Logical channel
    UInt32 CmdCode:7;           //!< Command code
    UInt32 CmdClass:3;          //!< Command class
    UInt32 Broadcast:1;         //!< Broadcast message indicator
    UInt32 Reserved:1;          //!< Reserved
    UInt32 Remote:1;            //!< Remote request bit
} bmCanIdFields_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

UInt32  bmGetMessageItem  (CanMessage_t *Message, UInt16 Offset, UInt16 Size);
void    bmSetMessageItem  (CanMessage_t *Message, UInt32 Value, UInt16 Offset, UInt16 Size);

UInt32  bmGetBoardOptions (UInt16 ModuleID, UInt16 Instance, UInt32 Default);
UInt32  bmGetPartitionID  (UInt16 ModuleID, UInt16 Instance);
UInt16  bmGetNodeAddress  (void);

UInt32  bmCalculateCrc    (void *Datablock, UInt32 Length);
Bool    bmCheckPassword   (UInt16 Password);

UInt16  bmGetDelta        (UInt16 Value1, UInt16 Value2);
Error_t bmInitUtilities   (void);

bmBootInfoBlock_t  *bmGetLoaderInfoBlock (void);
bmBoardInfoBlock_t *bmGetBoardInfoBlock  (void);

//****************************************************************************/

#endif /*BM_UTILITIES_H*/
