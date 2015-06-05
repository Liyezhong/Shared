/****************************************************************************/
/*! \file bmStorage.c
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
 */
/****************************************************************************/

#include <stdlib.h>
#include <stddef.h>
#include "Global.h"
#include "halLayer.h"
#include "bmCommon.h"
#include "bmError.h"
#include "bmDebug.h"
#include "bmStorage.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define HEAD_PARTITION_ID   0xFFFE9732   //!< Partition ID of table header
#define TEMP_PARTITION_ID   0xFFFFFFFF   //!< ID for temporary partition

#define MODE_BITMASK        0x00FF   //!< Bit mask for "mode" bits
#define FLAG_BITMASK        0xFF00   //!< Bit mask for "flag" bits
#define FLAG_READ_ENABLE    0x0100   //!< Opened for reading
#define FLAG_WRITE_ENABLE   0x0200   //!< Opened for writing
#define FLAG_CHKSUM_ERROR   0x2000   //!< Checksum error detected
#define FLAG_MODIFIED       0x8000   //!< Partition modified


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Data structure to hold partition descriptor
typedef struct {
    UInt32 PartitionID;         //!< Partition identifier
    UInt32 Address;             //!< Physical start address of partition
    UInt16 Size;                //!< Size of partition
    UInt16 Free;                //!< Free memory block following this partition
    UInt16 Mode;                //!< Allowed access mode
    UInt16 Checksum;            //!< Checksum of partition
} bmPartitionDescriptor_t;

//! Size of a partition descriptor
#define PARTITION_DESCRIPTOR_SIZE   sizeof(bmPartitionDescriptor_t)

//! Offset to checksum field in partition descriptor
#define PARTITION_CHECKSUM_OFFSET   offsetof(bmPartitionDescriptor_t, Checksum)


//****************************************************************************/
// Private Variables
//****************************************************************************/

static bmPartitionDescriptor_t *Partitions = NULL; //!< Partition table

static UInt16 PartitionTableSize = 0;   //!< Size of partition table (entries)
static UInt8  CheckBuffer[32];          //!< Buffer for checksum calculation
static Handle_t Device;                 //!< Storage device handle

static Bool UpdateChecksumOnWrite;      //!< Checksum update on write/close
static Bool WriteProtected = FALSE;     //!< Write protection state


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t bmUpdateChecksum  (
                 Handle_t Handle, UInt16 Address, UInt8 *Buffer, UInt16 Size);

static Error_t bmReallocatePartition      (UInt32 PartitionID, UInt16 Size);
static Error_t bmSearchPartition          (UInt32 PartitionID);
static Error_t bmFindFreePartitionEntry   (void);
static Error_t bmReadPartitionTable       (void);

static UInt16  bmCalculateChecksum        (Handle_t Handle);
static Error_t bmWritePartitionChecksum   (Handle_t Handle, UInt16 CheckSum);
static Error_t bmWritePartitionDescriptor (Handle_t Handle);


/*****************************************************************************/
/*!
 *  \brief   Create/open a partition
 *
 *      Create a new partition with the numerical name given by PartitionID.
 *      The partition content is cleared to zero. "Size" determines the size
 *      of the created partition, "Mode" the access mode. The partition can
 *      later be opened only in a mode that is compatible with the mode
 *      specified during creation.
 *
 *      If a partition with the same partition ID already exists and the mode
 *      bit MODE_OPEN_EXISTING is set, that partition is opened instead of
 *      creating a new one. The data in the partition will be left unchanged,
 *      i.e. it will not be cleared to zero. If MODE_OPEN_EXISTING is not
 *      set and the partition already exists, an error is returned.
 *
 *      If there is not enough free memory available anymore for a partition
 *      of that size an error is returned. This applies also, if Size is 0
 *      or Mode is invalid.
 *
 *  \iparam  PartitionID = Partition identifier
 *  \iparam  Mode        = Open mode (read, write, etc.)
 *  \iparam  Size        = Size of new partition
 *
 *  \return  Partition handle or (negative) error code
 *
 ****************************************************************************/

Error_t bmCreatePartition (UInt32 PartitionID, UInt16 Mode, UInt16 Size) {

    Handle_t FreeEntry, FreeBlock = 0;
    Error_t Status;
    UInt32 Address;

    if (bmSearchPartition(PartitionID) < 0) {

        if ((Mode & MODE_OPEN_RW) == 0 || Size == 0) {
            return (E_PARAMETER_OUT_OF_RANGE);
        }
        // find free entry in partition table
        if ((FreeEntry = bmFindFreePartitionEntry()) < 0) {
            return (E_PARTITION_TABLE_FULL);
        }
        // find free block of memory in storage
        while (Partitions[FreeBlock].Free < Size) {
            if (FreeBlock >= PartitionTableSize) {
                return (E_MEMORY_FULL);
            }
            FreeBlock++;
        }
        if (WriteProtected) {
            return (E_STORAGE_PROTECTED);
        }
        // initialze new partition table entry
        Address = Partitions[FreeBlock].Address + Partitions[FreeBlock].Size;
        Partitions[FreeEntry].PartitionID = PartitionID;
        Partitions[FreeEntry].Address  = Address;
        Partitions[FreeEntry].Size     = Size;
        Partitions[FreeEntry].Mode     = Mode & MODE_OPEN_RW;
        Partitions[FreeEntry].Free     = Partitions[FreeBlock].Free - Size;
        Partitions[FreeBlock].Free     = 0;
        Partitions[FreeEntry].Checksum = 0;

        // update partition table in non volatile storage
        if ((Status = bmWritePartitionDescriptor(FreeEntry)) < 0 ||
            (Status = bmWritePartitionDescriptor(FreeBlock)) < 0) {
            return (Status);
        }
        halStorageErase (Device, Partitions[FreeEntry].Address, Size);
    }
    else if ((Mode & MODE_OPEN_EXISTING) == 0) {
        return (E_PARTITION_EXISTS);
    }
    return (bmOpenPartition(PartitionID, Mode & MODE_OPEN_RW, Size));
}


/*****************************************************************************/
/*!
 *  \brief   Open an existing partition
 *
 *      Open an existing partition. The partition ID parameter determines
 *      the partition to open. It is the numerical "name" of a partition.
 *      The size of the partition to open must be at least "size" bytes,
 *      else an error will be returned.
 *
 *      The mode parameter determines the access mode (read/write/erase).
 *      A partition can only be read if opened in read-mode and only be
 *      written if opened in write mode. Any combination of read, write
 *      and erase can be specified.
 *
 *      The function returns a partition handle that can be used to access
 *      data in that partition according to the specified mode.
 *
 *  \iparam  PartitionID = Partition identifier ("name")
 *  \iparam  Mode        = Open mode (read/write/erase)
 *  \iparam  Size        = Minimum size of partition
 *
 *  \return  Partition handle or (negative) error code
 *
 ****************************************************************************/

Error_t bmOpenPartition (UInt32 PartitionID, UInt16 Mode, UInt16 Size) {

    Handle_t Handle = bmSearchPartition(PartitionID);

    if (Handle >= 0) {
        if (Partitions[Handle].Mode & FLAG_BITMASK) {
            return (E_PARTITION_ALREADY_OPEN);
        }
        if ((Mode & MODE_BITMASK) == 0) {
            return (E_PARTITION_ACCESS_MODE);
        }
        if ((Partitions[Handle].Mode & Mode) != Mode) {
            return (E_PARTITION_ACCESS_MODE);
        }
        if (Size > Partitions[Handle].Size) {
            return (E_SIZE_OUT_OF_RANGE);
        }
        if (Partitions[Handle].Checksum != bmCalculateChecksum(Handle)) {
            Partitions[Handle].Mode |= FLAG_CHKSUM_ERROR;
        }
        if (Mode & MODE_OPEN_READ) {
            Partitions[Handle].Mode |= FLAG_READ_ENABLE;
        }
        if (Mode & MODE_OPEN_WRITE) {
            Partitions[Handle].Mode |= FLAG_WRITE_ENABLE;
        }
    }
    return (Handle);
}


/*****************************************************************************/
/*!
 *  \brief   Read data from partition
 *
 *      Read a block of data from a partition. The partition must be open
 *      to do this. The address parameter determines the relative offset
 *      from the beginning of the partition. The read data is copied into
 *      the buffer pointed to by "buffer". The size parameter specifies
 *      the number of bytes/words to read.
 *
 *  \iparam  Handle  = Partition handle
 *  \iparam  Address = Address of data (relative offset)
 *  \oparam  Buffer  = Pointer to data buffer
 *  \iparam  Size    = Size of buffer
 *
 *  \return  Number of bytes read or (negative) error code
 *
 ****************************************************************************/

Error_t bmReadPartition (Handle_t Handle, UInt32 Address, void *Buffer, UInt16 Size) {

    bmPartitionDescriptor_t *Partition = &Partitions[Handle];

    if (Handle < 0 || Handle >= PartitionTableSize) {
        return (E_PARTITION_HANDLE_INVALID);
    }
    if (Buffer == NULL) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if ((Partition->Mode & FLAG_READ_ENABLE) == 0) {
        return (E_PARTITION_ACCESS_MODE);
    }
    if (Address >= Partition->Size) {
        return (E_ADDRESS_OUT_OF_RANGE);
    }
    if (Address + Size > Partition->Size) {
        return (E_SIZE_OUT_OF_RANGE);
    }
    if (Partitions[Handle].Mode & FLAG_CHKSUM_ERROR) {
        return (E_CHECKSUM_ERROR);
    }
    Address += Partition->Address;

    return (halStorageRead (Device, Address, Buffer, Size));
}


/*****************************************************************************/
/*!
 *  \brief   Write data to partition
 *
 *      Writes a block of data into a partition. The partition must be open
 *      to do this. The address parameter determines the relative offset
 *      from the beginning of the partition. The buffer parameter has to
 *      point to the data to be written. The size parameter specifies
 *      the number of bytes/words to write.
 *
 *  \iparam  Handle  = Partition handle
 *  \iparam  Address = Address of data (partition relative)
 *  \oparam  Buffer  = Pointer to data buffer
 *  \iparam  Size    = Size of buffer
 *
 *  \return  Number of bytes written or (negative) error code
 *
 ****************************************************************************/

Error_t bmWritePartition (Handle_t Handle, UInt32 Address, void *Buffer, UInt16 Size) {

    bmPartitionDescriptor_t *Partition = &Partitions[Handle];

    if (Handle < 0 || Handle >= PartitionTableSize) {
        return (E_PARTITION_HANDLE_INVALID);
    }
    if (Buffer == NULL) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if ((Partition->Mode & FLAG_WRITE_ENABLE) == 0) {
        return (E_PARTITION_ACCESS_MODE);
    }
    if (Address >= Partition->Size) {
        return (E_ADDRESS_OUT_OF_RANGE);
    }
    if (Address + Size > Partition->Size) {
        return (E_SIZE_OUT_OF_RANGE);
    }
    if (Partitions[Handle].Mode & FLAG_CHKSUM_ERROR) {
        return (E_CHECKSUM_ERROR);
    }
    if (WriteProtected) {
        return (E_STORAGE_PROTECTED);
    }
    if (bmUpdateChecksum (Handle, Address, Buffer, Size) < 0) {
        return (E_CHECKSUM_WRITE_ERROR);
    }
    Partitions[Handle].Mode |= FLAG_MODIFIED;
    Address += Partition->Address;

    return (halStorageWrite (Device, Address, Buffer, Size));
}


/*****************************************************************************/
/*!
 *  \brief   Erase data partition
 *
 *      Sets all data in a partition to zero. The partition must be open
 *      in write mode to do this.
 *
 *      Other than with bmDeletePartition() not the partition is deleted,
 *      but only the data in the partition is erased. The partition
 *      continues to exist.
 *
 *  \iparam  Handle = Partition handle
 *
 *  \return  Number of bytes erased or (negative) error code
 *
 ****************************************************************************/

Error_t bmErasePartition (Handle_t Handle) {

    bmPartitionDescriptor_t *Partition = &Partitions[Handle];
    Error_t Status;

    if (Handle < 0 || Handle >= PartitionTableSize) {
        return (E_PARTITION_HANDLE_INVALID);
    }
    if ((Partition->Mode & FLAG_WRITE_ENABLE) == 0) {
        return (E_PARTITION_ACCESS_MODE);
    }
    if (WriteProtected) {
        return (E_STORAGE_PROTECTED);
    }
    Status = halStorageErase (Device,
        Partitions[Handle].Address, Partitions[Handle].Size);

    if (Status >= 0) {
        UInt16 CheckSum = bmCalculateChecksum(Handle);

        if (bmWritePartitionChecksum(Handle, CheckSum) < 0) {
            return (E_CHECKSUM_WRITE_ERROR);
        }
    }
    Partitions[Handle].Mode |= FLAG_MODIFIED;

    return (Status);
}


/*****************************************************************************/
/*!
 *  \brief   Close partition
 *
 *      Closes an open partition. If the global ChecksumUpdateOnClose flag
 *      is set to true, the checksum is not written on every write operation
 *      to the partition, but now. This mode of operation might be dangerous
 *      if it can't be guaranteed, that bmClosePartition() being called before
 *      power goes down. Saver (but slower) is to update the checksum on every
 *      write, i.e. set ChecksumUpdateOnClose to false.
 *
 *  \iparam  Handle = Partition handle
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmClosePartition (Handle_t Handle) {

    Error_t Status = NO_ERROR;

    if (Handle < 0 || Handle >= PartitionTableSize) {
        return (E_PARTITION_HANDLE_INVALID);
    }
    if ((Partitions[Handle].Mode & FLAG_BITMASK) == 0) {
        return (E_PARTITION_NOT_OPENED);
    }
    if (!UpdateChecksumOnWrite) {
        if ((Partitions[Handle].Mode & FLAG_MODIFIED) &&
           (~Partitions[Handle].Mode & FLAG_CHKSUM_ERROR)) {

            Status = bmWritePartitionChecksum (
                Handle, bmCalculateChecksum(Handle));
        }
    }
    Partitions[Handle].Mode &= ~FLAG_BITMASK;

    return (Status);
}


/*****************************************************************************/
/*!
 *  \brief   Flushes all open partitions
 *
 *      Writes all not yet written data to all open partitions. This
 *      function should be called, if UpdateChecksumOnWrite is set
 *      to FALSE (which means UpdateChecksumOnClose), and the node
 *      is shutting down.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmFlushPartitions (void) {

    Error_t ErrCode = NO_ERROR;
    Error_t Status;
    UInt32 Index;   //!< Partitiontable index

    if (WriteProtected) {
        return (E_STORAGE_PROTECTED);
    }
    for (Index=0; Index < PartitionTableSize; Index++) {
        if ((Partitions[Index].Mode & FLAG_MODIFIED) &&
           (~Partitions[Index].Mode & FLAG_CHKSUM_ERROR) && !UpdateChecksumOnWrite) {
            UInt16 CheckSum = bmCalculateChecksum(Index);

            if ((Status = bmWritePartitionChecksum(Index, CheckSum)) < NO_ERROR) {
                ErrCode = Status;
                continue;
            }
            Partitions[Index].Mode &= ~FLAG_MODIFIED;
        }
    }

    if ((Status = halStorageWait (Device)) < NO_ERROR) {
        ErrCode = Status;
    }

    return (ErrCode);
}


/*****************************************************************************/
/*!
 *  \brief   Get size of a partition
 *
 *      Returns the size of the partition. If the partition doesn't exist
 *      a size of zero is returned.
 *
 *  \iparam  PartitionID = Partition identifier
 *
 *  \return  Size of partition or 0 if partition doesn_t exist
 *
 ****************************************************************************/

UInt16 bmGetPartitionSize (UInt32 PartitionID) {

    Handle_t Handle;

    if ((Handle = bmSearchPartition(PartitionID)) > 0) {
        return (Partitions[Handle].Size);
    }
    return (0);
}


/*****************************************************************************/
/*!
 *  \brief   Delete an existing partition
 *
 *      Deletes a partition and frees the associated memory. The deleted
 *      memory block and the free memory following that block are added
 *      to the free block size of the partition associated with the memory
 *      block immediately preceding the deleted memory block.
 *
 *  \iparam  PartitionID = Partition identifier
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmDeletePartition (UInt32 PartitionID) {

    Handle_t Handle;
    Error_t Status;
    UInt16 Offset;
    UInt16 i;

    if ((Handle = bmSearchPartition(PartitionID)) < 0) {
        return (Handle);
    }
    if (Partitions[Handle].Mode & FLAG_BITMASK) {
        return (E_PARTITION_IS_OPEN);
    }
    if (WriteProtected) {
        return (E_STORAGE_PROTECTED);
    }
    // find partition associated with memory block before the one to delete
    for (i=0; i < PartitionTableSize; i++) {

        Offset = Partitions[i].Address + Partitions[i].Size + Partitions[i].Free;

        if (Offset == Partitions[Handle].Address) {
            Partitions[i].Free += Partitions[Handle].Size + Partitions[Handle].Free;
            Partitions[Handle].PartitionID =
            Partitions[Handle].Size     =
            Partitions[Handle].Free     =
            Partitions[Handle].Mode     =
            Partitions[Handle].Address  =
            Partitions[Handle].Checksum = 0;

            // update partition table in non volatile storage
            if ((Status = bmWritePartitionDescriptor(i)) < 0) {
                return (Status);
            }
            return (bmWritePartitionDescriptor(Handle));
        }
    }
    return (E_PARTITION_TABLE_CORRUPT);
}


/*****************************************************************************/
/*!
 *  \brief   Check partition for errors
 *
 *      Checks the open partition for errors. In case of an error, an error
 *      code is returned. If everything is ok, NO_ERROR is retuned.
 *
 *  \iparam  Handle = Partition handle
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmCheckPartition (Handle_t Handle) {

    if (Handle >= PartitionTableSize) {
        return (E_PARTITION_HANDLE_INVALID);
    }
    if ((Partitions[Handle].Mode & FLAG_BITMASK) == 0) {
        return (E_PARTITION_NOT_OPENED);
    }
    if (Partitions[Handle].Mode & FLAG_CHKSUM_ERROR) {
        return (E_CHECKSUM_ERROR);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Repair checksum error
 *
 *      Repair (recalculate) checksum of an invalid data partition. After
 *      repairing the partition can be read and written again. Care must
 *      be taken: only the checksum is corrected, not the data in the
 *      partition. The partition may contain invalid data if a checksum
 *      error was detected.
 *
 *  \iparam  Handle = Partition handle
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmRepairPartition (Handle_t Handle) {

    UInt16 Checksum;

    if (Handle >= PartitionTableSize) {
        return (E_PARTITION_HANDLE_INVALID);
    }
    if ((Partitions[Handle].Mode & FLAG_BITMASK) == 0) {
        return (E_PARTITION_NOT_OPENED);
    }
    if (WriteProtected) {
        return (E_STORAGE_PROTECTED);
    }
    if (Partitions[Handle].Mode & FLAG_CHKSUM_ERROR) {
        Checksum = bmCalculateChecksum(Handle);
        Partitions[Handle].Mode &= ~FLAG_CHKSUM_ERROR;

        return (bmWritePartitionChecksum(Handle, Checksum));
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Resize partition
 *
 *      Changes the size of an existing partition to newSize. Shrinking
 *      is done always in place. Size expansion is done in place, if
 *      sufficient memory behind the partition is free; otherwise the
 *      partition is reallocated and moved to another position.
 *
 *      If newSize is zero, the partition will be deleted.
 *
 *  \iparam  PartitionID = Partition identifier
 *  \iparam  newSize     = New size of partition
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmResizePartition (UInt32 PartitionID, UInt16 newSize) {

    Int16 Difference;
    Handle_t Handle;

    // delete partition if requested size is 0
    if (!newSize) {
        return (bmDeletePartition(PartitionID));
    }
    if ((Handle = bmSearchPartition(PartitionID)) < 0) {
        return (Handle);
    }
    if (Partitions[Handle].Mode & FLAG_BITMASK) {
        return (E_PARTITION_IS_OPEN);
    }
    if (WriteProtected) {
        return (E_STORAGE_PROTECTED);
    }
    Difference = (Int16)(newSize - Partitions[Handle].Size);

    if (Difference) {
        // shrink partition if new size is smaller than old one
        if (Difference < 0) {
            Partitions[Handle].Free -= Difference;
            Partitions[Handle].Size += Difference;
        }

        // expand partition if new size is larger than old one
        if (Difference > 0) {
            if (Difference <= Partitions[Handle].Free) {
                Partitions[Handle].Free -= Difference;
                Partitions[Handle].Size += Difference;
            }
            else {
                return (bmReallocatePartition (PartitionID, newSize));
            }
        }
        return (bmWritePartitionDescriptor(Handle));
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Reallocate partition
 *
 *      Duplicates the partition with the supplied PartitionID and changes
 *      its size to "Size". The content of the original partition is copied
 *      into the new partition and the original partition is deleted.
 *
 *  \iparam  PartitionID = Partition identifier
 *  \iparam  Size        = New size of partition
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmReallocatePartition (UInt32 PartitionID, UInt16 Size) {

    Handle_t oldHandle, newHandle;
    UInt32 srcOffset, dstOffset;
    Error_t Status;
    UInt16 i;

    if (WriteProtected) {
        return (E_STORAGE_PROTECTED);
    }
    if ((oldHandle = bmSearchPartition(PartitionID)) < 0) {
        return (oldHandle);
    }
    if ((newHandle = bmCreatePartition (TEMP_PARTITION_ID,
            Partitions[oldHandle].Mode | MODE_OPEN_EXISTING, Size)) < 0) {
        return (newHandle);
    }
    bmClosePartition (newHandle);

    srcOffset = Partitions[oldHandle].Address;
    dstOffset = Partitions[newHandle].Address;
    Size      = Partitions[oldHandle].Size;

    for (i=0; i < Size; i++) {
        if (halStorageRead (Device, srcOffset++, CheckBuffer, 1) < 1) {
            return (E_STORAGE_READ_ERROR);
        }
        if (halStorageWrite (Device, dstOffset++, CheckBuffer, 1) < 1) {
            return (E_STORAGE_WRITE_ERROR);
        }
    }
    Partitions[newHandle].Checksum = Partitions[oldHandle].Checksum;

    if ((Status = bmDeletePartition (PartitionID)) < 0) {
        return (Status);
    }
    Partitions[newHandle].PartitionID = PartitionID;

    return (bmWritePartitionDescriptor(newHandle));
}


/*****************************************************************************/
/*!
 *  \brief   Set/clear write protection
 *
 *      Sets or clears write protection for entire storage. Different sources
 *      can set/clear write protection. Write protection persists as long as
 *      at least one of the sources have write protection set. In other words,
 *      all sources that have set write protection have to remove it, before
 *      protection is removed. The following sources are defined so far:
 *
 *      - USV (Master)
 *      - Supply voltage monitor (local)
 *      - Node state (standby)
 *
 *  \iparam  Source = Source (caller)
 *  \iparam  State  = Write protection state (TRUE: set, FALSE: clear)
 *
 *  \return  Actual write protection status (over all sources)
 *
 ****************************************************************************/

Bool bmWriteProtectStorage (bmWriteProtectSourceID_t Source, Bool State) {

    if (State) {
        WriteProtected |= Source;
    }
    else {
        WriteProtected &= ~Source;
    }
    dbgPrint("Storage Write Protection = %d", WriteProtected);

    return (WriteProtected);
}


/*****************************************************************************/
/*!
 *  \brief   Formats and partitions memory
 *
 *      Formats the storage by erasing all data and setting up an empty
 *      partition table. New partitions must be created after that, to
 *      read or write again from/to storage.
 *
 *  \iparam  PartitionCount = Number of partitions
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmFormatStorage (UInt16 PartitionCount) {

    UInt32 MemorySize = halStorageSize(Device);
    bmPartitionDescriptor_t Header;
    Error_t Status;

    if (!PartitionCount++) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (PartitionCount * PARTITION_DESCRIPTOR_SIZE > MemorySize) {
        return (E_STORAGE_TOO_SMALL);
    }
    if (WriteProtected) {
        return (E_STORAGE_PROTECTED);
    }
    if ((Status = halStorageErase(Device, 0, MemorySize)) < 0) {
        return (Status);
    }
    Header.PartitionID = HEAD_PARTITION_ID;
    Header.Size     = PartitionCount * PARTITION_DESCRIPTOR_SIZE;
    Header.Free     = MemorySize - Header.Size;
    Header.Address  = 0;
    Header.Mode     = 0;
    Header.Checksum = 0;

    Status = halStorageWrite (Device, 0, &Header, PARTITION_DESCRIPTOR_SIZE);
    if (Status < 0) {
        return (Status);
    }
    return (bmReadPartitionTable());
}

#ifdef ASB_FCT

/*****************************************************************************/
/*!
 *  \brief   Erase memory
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
Error_t bmEraseStorage (void) {

    Error_t Status;

    if (WriteProtected) {
        return (E_STORAGE_PROTECTED);
    }

    if ((Status = halStorageErase(Device, 0, PARTITION_DESCRIPTOR_SIZE)) < 0) {
        return (Status);
    }

    return 0;
}
#endif

/*****************************************************************************/
/*!
 *  \brief   Calculate partition checksum
 *
 *      Read entire partition and calculates the checksum over all data in
 *      the partition. The checksum is returned.
 *
 *  \iparam  Handle = Partition handle
 *
 *  \return  Calculated partition checksum
 *
 ****************************************************************************/

static UInt16 bmCalculateChecksum (Handle_t Handle) {

    UInt32 Offset = Partitions[Handle].Address;
    UInt16 Length = Partitions[Handle].Size;
    UInt16 CheckSum = 0;
    UInt16 Address = 0;
    Int32  SegSize = 0;
    Int32  Count;
    Int16  i;

    while (Length) {
        SegSize = MIN (Length, sizeof(CheckBuffer));

        Count = halStorageRead (Device, Offset, CheckBuffer, SegSize);
        if (Count != SegSize) {
            return (0);
        }
        for (i=0; i < Count; i++) {
            CheckSum += (UInt16)CheckBuffer[i] * ++Address;
        }
        Offset += Count;
        Length -= Count;
    }
    return (CheckSum);
}


/*****************************************************************************/
/*!
 *  \brief   Update partition checksum
 *
 *      Updates the partition checksum by calculating the difference between
 *      the old content of the storage and the new content to be written into
 *      storage. The difference is weighted by Address and signed added to the
 *      actual checksum value in the partition descriptor. The result is the
 *      checksum over the partition including the new data to be written.
 *
 *      The old storage content is read into a temporary checksum buffer, the
 *      new content is supplied as parameter "Buffer". The number of modified
 *      bytes is supplied as "Size" parameter.
 *
 *      The checksum is updated only if the global ChecksumUpdateOnClose is
 *      false. If this is not the case, checksum is calculated on close.
 *
 *  \iparam  Handle  = Partition handle
 *  \iparam  Address = Address of data (relative offset)
 *  \iparam  Buffer  = Pointer to data buffer
 *  \iparam  Size    = Number of bytes in buffer
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmUpdateChecksum (
          Handle_t Handle, UInt16 Address, UInt8 *Buffer, UInt16 Size) {

    Int32 Count;
    UInt16 i;

    if (UpdateChecksumOnWrite) {
        UInt32 Offset = Partitions[Handle].Address + Address;
        UInt16 newSum = Partitions[Handle].Checksum;

        while (Size) {
            Count = MIN (Size, sizeof(CheckBuffer));
            Count = halStorageRead (Device, Offset, CheckBuffer, Count);

            if (Count < 0) {
                return (E_STORAGE_READ_ERROR);
            }
            for (i=0; i < Count; i++) {
                newSum += (UInt16)(Buffer[i] - CheckBuffer[i]) * ++Address;
            }
            Offset += Count;
            Buffer += Count;
            Size -= Count;
        }
        return (bmWritePartitionChecksum(Handle, newSum));
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Read partition table from storage
 *
 *      Reads the partition table from storage into the module global
 *      partition descriptor table. The memory to hold that table is
 *      dynamically allocated from heap. If the read partition table
 *      is invalid, an error is returned.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmReadPartitionTable (void) {

    UInt32 MemorySize = halStorageSize(Device);
    bmPartitionDescriptor_t Header;
    Error_t Status;

    // read partition table header from storage
    Status = halStorageRead (Device, 0, &Header, PARTITION_DESCRIPTOR_SIZE);
    if (Status < 0) {
        return (Status);
    }
    // check if partition table header is valid
    if (Header.PartitionID != HEAD_PARTITION_ID) {
        return (E_STORAGE_UNFORMATTED);
    }
    if (MemorySize < Header.Size) {
        return (E_STORAGE_TOO_SMALL);
    }
    // allocate module descriptor table
    Partitions = malloc(Header.Size);
    if (Partitions == NULL) {
        return (E_MEMORY_FULL);
    }
    PartitionTableSize = Header.Size / PARTITION_DESCRIPTOR_SIZE;

    // read partition table from storage
    Status = halStorageRead (Device, 0, Partitions, Header.Size);
    if (Status < 0) {
        return (Status);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Search partition ID in partition table
 *
 *      Searches the partition descriptor table for a partition with the
 *      supplied partition ID and returns the table index to the found
 *      descriptor. The index can be used as a partition handle. If the
 *      partition in question doesn't exist, an error is returned.
 *
 *  \iparam  PartitionID = ID of partition to search for
 *
 *  \return  Partitiontable index or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSearchPartition (UInt32 PartitionID) {

    UInt32 i;

    for (i=1; i < PartitionTableSize; i++) {
        if (Partitions[i].PartitionID == PartitionID &&
            Partitions[i].Size != 0) {

            return (i);
        }
    }
    return (E_PARTITION_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief   Find free partition table entry
 *
 *      Searches the partition descriptor table for an unused partition
 *      descriptor and returns the table index to that descriptor.
 *      If a free descriptor can't be find, an error is returned.
 *
 *  \return  Partitiontable index or (negative) error code
 *
 ****************************************************************************/

static Error_t bmFindFreePartitionEntry (void) {

    UInt32 i;

    for (i=1; i < PartitionTableSize; i++) {
        if (Partitions[i].Mode == 0 && Partitions[i].Size == 0) {
            return (i);
        }
    }
    return (E_PARTITION_TABLE_FULL);
}


/*****************************************************************************/
/*!
 *  \brief   Write partition descriptor
 *
 *      Writes the partition descriptor from the partition table into
 *      non-volatile memory.
 *
 *  \iparam  Handle = Partition handle
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmWritePartitionDescriptor (Handle_t Handle) {

    UInt32 Offset = PARTITION_DESCRIPTOR_SIZE * Handle;
    bmPartitionDescriptor_t Partition;

    Partition = Partitions[Handle];
    Partition.Mode &= MODE_BITMASK;

    if (halStorageWrite (Device, Offset, &Partition, sizeof(Partition)) < 0) {
        return (E_PARTITION_TABLE_UPDATE);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Write partition checksum
 *
 *      Writes the supplied partition CheckSum into non-volatile memory.
 *
 *  \iparam  Handle   = Partition handle
 *  \iparam  CheckSum = Partition checksum
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmWritePartitionChecksum (Handle_t Handle, UInt16 CheckSum) {

    UInt32 Offset =
        (PARTITION_DESCRIPTOR_SIZE * Handle) + PARTITION_CHECKSUM_OFFSET;

    Partitions[Handle].Checksum = CheckSum;

    if (halStorageWrite (Device, Offset, &CheckSum, sizeof(CheckSum)) < 0) {
        return (E_CHECKSUM_WRITE_ERROR);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize storage module
 *
 *      Opens the hardware storage using the HAL and reads the partition table
 *      from storage into a module-global descriptor table. If no partition
 *      table exists in storage, the storage will be formatted and an empty
 *      partition table created. The size of the newly created partition
 *      table is PartitionTableSize.
 *
 *      This function is called only once during startup.
 *
 *  \iparam  PartTableSize = Size of partition table
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmInitializeStorage (UInt16 PartTableSize) {
    #ifndef ASB_FCT
    Error_t Status;
    #endif

    Device = halStorageOpen (HAL_STORAGE_FRAM, HAL_OPEN_RW | HAL_OPEN_ERASE);
    if (Device < 0) {
        return (Device);
    }
    if (bmReadPartitionTable() < 0) {
    #ifndef ASB_FCT
        if ((Status = bmFormatStorage(PartTableSize)) < 0) {
            return (Status);
        }
    #endif
    }

#ifdef ASB_FCT
    bmEraseStorage();
#endif

    // select checksum update mode (TRUE: on write, FALSE: on close)
    UpdateChecksumOnWrite = TRUE;

    return (NO_ERROR);
}

//****************************************************************************/
