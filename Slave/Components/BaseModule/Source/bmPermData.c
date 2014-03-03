/****************************************************************************/
/*! \file bmPermData.c
 *
 *  \brief Permanent data handling module
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to handle permanent (persistent)
 *       data. It implements a level above the storage module and offers
 *       a simpler interface to the non-volatile storage.
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
#include "bmCan.h"
#include "bmError.h"
#include "bmUtilities.h"
#include "bmStorage.h"
#include "bmPermData.h"


/*****************************************************************************/
/*!
 *  \brief   Open a data partition
 *
 *      Opens a data partition in non-volatile storage and returns the
 *      partition handle, which can further be used to read/write data
 *      from that partition.  If the partition doesn't exist, it is
 *      created. If the existing partition is smaller than the given
 *      Size, it is automatically expanded. If the partition contains
 *      checksum errors, the checksum is corrected and all data in the
 *      partition is erased (must later be set to defaults).
 *
 *      Each module instance can have it's own partition for exclusive
 *      use. The partition identifier is determined combining the
 *      ModuleID and Instance number. If a function module requires
 *      only one partition, it must call this function only once for
 *      the module, instead of once per module instance. The instance
 *      number should be zero in this case.
 *
 *  \iparam  ModuleID = Module ID
 *  \iparam  Instance = Module instance
 *  \iparam  Size     = Partition size
 *
 *  \return  Partition handle or (negative) error code
 *
 *****************************************************************************/

Handle_t bmOpenPermStorage (UInt16 ModuleID, UInt16 Instance, UInt16 Size) {

    const UInt16 OpenMode = MODE_OPEN_EXISTING | MODE_OPEN_RW;
    UInt32 PartitionID = bmGetPartitionID(ModuleID, Instance);
    Handle_t Handle;
    Error_t Status;

    UInt16 PartitionSize = bmGetPartitionSize(PartitionID);

    // if partition is too small, expand it
    if (PartitionSize > 0 && PartitionSize < Size) {
        if ((Status = bmResizePartition (PartitionID, Size)) < 0) {
            return (Status);
        }
    }
    // open/create base module's data partition
    Handle = bmCreatePartition (PartitionID, OpenMode , Size);
    if (Handle >= 0) {

        // check partition for errors; erase partition content in case
        if (bmCheckPartition(Handle) < 0) {
            if (bmRepairPartition(Handle) == NO_ERROR) {
                bmErasePartition(Handle);
            }
        }
    }
    return (Handle);
}


/*****************************************************************************/
/*!
 *  \brief   Close a data partition
 *
 *      Close the data partition associated with Handle-
 *
 *  \iparam  Handle = Partition handle
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

Handle_t bmClosePermStorage (Handle_t Handle) {

    return (bmClosePartition(Handle));
}


/*****************************************************************************/
/*!
 *  \brief   Get data item from non-volatile storage
 *
 *      Reads a data item from non-volatile storage and returns it. The
 *      ParamID identifies not only the address of the parameter, but
 *      also the size. If reading of data fails, the Default value is
 *      returned. 
 *
 *  \iparam  Handle  = Partition handle
 *  \iparam  ParamID = Parameter identifier
 *  \iparam  Default = Default value (to return in case of read error)
 *
 *  \return  Value of read data item
 *
 *****************************************************************************/

UInt32 bmGetStorageItem (Handle_t Handle, UInt32 ParamID, UInt32 Default) {

    UInt32 Address = GET_PARAM_ADDRESS(ParamID);
    UInt32 SizeOf  = GET_PARAM_SIZE(ParamID);
    UInt8  Buffer[sizeof(UInt32)];
    UInt32 Result = 0;
    UInt32 i;

    if (SizeOf > sizeof(Buffer)) {
        return (Default);
    }
    if (bmReadPartition (Handle, Address, Buffer, SizeOf) < 0) {
        return (Default);
    }
    for (i=0; i < SizeOf; i++) {
        Result += Buffer[i] << (i * 8);
    }
    return (Result);
}


/*****************************************************************************/
/*!
 *  \brief   Sets data item in non-volatile storage
 *
 *      Writes a data item to non-volatile storage. ParamID identifies
 *      not only the address of the parameter, but also the size.
 *      If writing fails, an error is returned.
 *
 *  \iparam  Handle  = Partition handle
 *  \iparam  ParamID = Parameter identifier
 *  \iparam  Value   = Parameter value
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

Error_t bmSetStorageItem (Handle_t Handle, UInt32 ParamID, UInt32 Value) {

    UInt32 Address = GET_PARAM_ADDRESS(ParamID);
    UInt32 SizeOf  = GET_PARAM_SIZE(ParamID);
    UInt8  Buffer[sizeof(UInt32)];
    Error_t Status;
    UInt32 i;

    if (SizeOf > sizeof(Buffer)) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    for (i=0; i < SizeOf; i++) {
        Buffer[i] = (Value >> (i * 8)) & MAX_UINT8;
    }
    Status = bmWritePartition (Handle, Address, Buffer, SizeOf);
    if (Status < 0) {
        return (Status);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Increments data item in non-volatile storage
 *
 *      Increments a data item in non-volatile storage by given Count.
 *      ParamID identifies not only the address of the parameter, but also
 *      the size. The value in storage will be limited to MAX_UINT32.
 *      Further increments will be ignored, but not interpreted as an
 *      error. If writing to storage fails, an error is returned.
 *
 *  \iparam  Handle  = Partition handle
 *  \iparam  ParamID = Parameter identifier
 *  \iparam  Count   = Increment count
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

Error_t bmIncStorageItem (Handle_t Handle, UInt32 ParamID, UInt32 Count) {

    UInt32 Value = bmGetStorageItem (Handle, ParamID, MAX_UINT32);

    if (Value < MAX_UINT32 && Count > 0) {
        if (Value + Count >= Value) {
            Value += Count;
        }
        else {
            Value = MAX_UINT32;
        }
        return (bmSetStorageItem (Handle, ParamID, Value));
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Decrements data item in non-volatile storage
 *
 *      Decrements a data item in non-volatile storage by given Count.
 *      ParamID identifies not only the address of the parameter, but 
 *      also the size. The value in storage will not fall below 0. 
 *      Further decrements will be ignored, but not interpreted as an
 *      error. If writing to storage fails, an error is returned.
 *
 *  \iparam  Handle  = Partition handle
 *  \iparam  ParamID = Parameter identifier
 *  \iparam  Count   = Decrement count
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

Error_t bmDecStorageItem (Handle_t Handle, UInt32 ParamID, UInt32 Count) {

    UInt32 Value = bmGetStorageItem (Handle, ParamID, MAX_UINT32);

    if (Value > 0 && Count > 0) {
        if (Value > Count) {
            Value -= Count;
        }
        else {
            Value = 0;
        }
        return (bmSetStorageItem (Handle, ParamID, Value));
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Verify data items in a partition
 *
 *      Verifies data items in a partition for plausibility. The supplied
 *      Ranges table contains the address of the data item, and the valid
 *      range (as min/max value). All data items must be inside their
 *      specified ranges. If this is the case, TRUE is returned, else
 *      FALSE is returned.
 *
 *      Data items where both, the min and max values are zero, are omitted
 *      from verification.
 *
 *  \iparam  Handle = Partition handle
 *  \iparam  Ranges = Parameter descriptor table
 *  \iparam  Count  = Number of entries in descriptor table
 *
 *  \return  Result of verification (TRUE: succeeded, FALSE: failed)
 *
 *****************************************************************************/

Bool bmVerifyStorageItems (
            Handle_t Handle, const bmParamRange_t *Ranges, UInt16 Count) {

    UInt32 Value;
    UInt32 i;

    if (Ranges != NULL) {
        for (i=0; i < Count; i++) {
            if (Ranges[i].MinValue || Ranges[i].MaxValue) {
                Value = bmGetStorageItem (Handle, Ranges[i].Address, MAX_UINT32);

                if (Value < Ranges[i].MinValue ||
                    Value > Ranges[i].MaxValue) {
                    return (FALSE);
                }
            }
        }
    }
    return (TRUE);
}


/*****************************************************************************/
/*!
 *  \brief   Reset data items in a partition to default
 *
 *      Resets data items in a partition to default. The supplied Ranges
 *      table contains the address of the data item, and the default value
 *      for each item.
 *
 *  \iparam  Handle = Partition handle
 *  \iparam  Ranges = Parameter descriptor table
 *  \iparam  Count  = Number of entries in descriptor table
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

Error_t bmResetStorageItems (
            Handle_t Handle, const bmParamRange_t *Ranges, UInt16 Count) {

    UInt16 ErrCount = 0;
    UInt32 i;

    if (Ranges != NULL) {
        for (i=0; i < Count; i++) {
            if (bmSetStorageItem (Handle, Ranges[i].Address, Ranges[i].Default) < 0) {
                ErrCount++;
            }
        }
        if (ErrCount) {
            return (E_PARTITION_RESET_FAILED);
        }
    }
    return (NO_ERROR);
}


//****************************************************************************/
