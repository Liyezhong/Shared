/****************************************************************************/
/*! \file bmStorage.h
 *
 *  \brief Storage Handling Functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions to access and organize partitions
 *       in non-volatile storage.
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef BM_STORAGE_H
#define BM_STORAGE_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

#define MODE_OPEN_READ      0x01     //!< Mode: open for reading
#define MODE_OPEN_WRITE     0x02     //!< Mode: open for writing
#define MODE_OPEN_RW        0x03     //!< Mode: open for reading/writing
#define MODE_OPEN_EXISTING  0x08     //!< Mode: open if already existing


//****************************************************************************/
// Public Constants
//****************************************************************************/

//! Sources allowed to write protect storage (one bit per source)
typedef enum {
    PROTECT_BY_USV            = 1,   //!< write protect by USV
    PROTECT_BY_SUPPLY_MONITOR = 2,   //!< write protect by voltage monitor
    PROTECT_BY_NODE_STATE     = 4    //!< write protect by node state
} bmWriteProtectSourceID_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t bmOpenPartition    (UInt32 PartitionID, UInt16 Mode, UInt16 Size);
Error_t bmCreatePartition  (UInt32 PartitionID, UInt16 Mode, UInt16 Size);
Error_t bmResizePartition  (UInt32 PartitionID, UInt16 Size);
Error_t bmDeletePartition  (UInt32 PartitionID);
UInt16  bmGetPartitionSize (UInt32 PartitionID);
Error_t bmFormatStorage    (UInt16 PartitionTableSize);

Error_t bmReadPartition    (Handle_t Handle, UInt32 Address, void *Buffer, UInt16 Size);
Error_t bmWritePartition   (Handle_t Handle, UInt32 Address, void *Buffer, UInt16 Size);
Error_t bmErasePartition   (Handle_t Handle);
Error_t bmClosePartition   (Handle_t Handle);
Error_t bmCheckPartition   (Handle_t Handle);
Error_t bmRepairPartition  (Handle_t Handle);
Error_t bmFlushPartitions  (void);

Bool    bmWriteProtectStorage (bmWriteProtectSourceID_t Source, Bool State);
Error_t bmInitializeStorage (UInt16 PartitionTableSize);

//****************************************************************************/

#endif /*BM_STORAGE_H*/
