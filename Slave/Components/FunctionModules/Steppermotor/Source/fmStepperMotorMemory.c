/****************************************************************************/
/*! \file fmStepperMotorMemory.c
 * 
 *  \brief Functions to manage and access non-volatile memory used by function
 *         module stepper motor
 *
 *  $Version: $ 0.1
 *  $Date:    $ 02.10.2010
 *  $Author:  $ Rainer Boehles
 *
 *  \b Description:
 *
 *      This module contains functions to control and access stepper motor
 *      data in non volatile memory. Typically life cycle data, which
 *      should endure power loss, is stored and retrieved by this functions.
 *
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
#include "Basemodule.h"
#include "fmStepperMotor.h"
#include "fmStepperMotorMemory.h"


//*****************************************************************************/
// Private Constants and Macros 
//*****************************************************************************/

//!  Definition of non-volatile data used by stepper module
#define SM_PARAM_LAYOUT_VERSION     BUILD_PARAMETER(0,2)    //!< Partition layout version
#define SM_PARAM_OPERATE_HOURS      BUILD_PARAMETER(2,4)    //!< operation hours (total amount of activity)
#define SM_PARAM_REV_COUNTER        BUILD_PARAMETER(6,4)    //!< step counter (total amount of revisions)
#define SM_PARAM_DIRCHG_COUNTER     BUILD_PARAMETER(10,4)   //!< direction changes counter (total amount of direction changes)
#define SM_PARTITION_SIZE           14                      //!< non-volatile data storage partition size

#define SM_PARTITION_VERSION        1                       //!< non-volatile data storage partition layout version


//****************************************************************************/
// Private Variables 
//****************************************************************************/

//! Persistent parameter descriptor table (used for verification)
static const bmParamRange_t smPermDataTable[] = {
    { SM_PARAM_LAYOUT_VERSION, 1,  SM_PARTITION_VERSION,   SM_PARTITION_VERSION },
    { SM_PARAM_OPERATE_HOURS,  0,  MAX_UINT32,             0 },
    { SM_PARAM_REV_COUNTER,    0,  MAX_UINT32,             0 },
    { SM_PARAM_DIRCHG_COUNTER, 0,  MAX_UINT32,             0 },
};


/*****************************************************************************/
/*!
 *  \brief   Get 'revolution count' from non-volatile storage
 *
 *      Reads 'revolution count' from non-volatile storage and returns it.
 *      If reading of data fails, 0 is returned. 
 *
 *  \iparam  Memory  = pointer to non volatile data object
 *
 *  \return  Value of read data item or 0
 *
 *****************************************************************************/
UInt32 smGetRevolution(smMemory_t *Memory) {

    return bmGetStorageItem (Memory->Handle, SM_PARAM_REV_COUNTER, 0);
}


/*****************************************************************************/
/*!
 *  \brief   Get 'operation time' from non-volatile storage
 *
 *      Reads 'operation time' from non-volatile storage and returns it.
 *      If reading of data fails, 0 is returned. 
 *
 *  \iparam  Memory  = pointer to non volatile data object
 *
 *  \return  Value of read data item or 0
 *
 *****************************************************************************/
UInt32 smGetOperationTime(smMemory_t *Memory) {

    return bmGetStorageItem (Memory->Handle, SM_PARAM_OPERATE_HOURS, 0);
}


/*****************************************************************************/
/*!
 *  \brief   Get 'revolution count' from non-volatile storage
 *
 *      Reads 'revolution count' from non-volatile storage and returns it.
 *      If reading of data fails, 0 is returned. 
 *
 *  \iparam  Memory  = pointer to non volatile data object
 *
 *  \return  Value of read data item or 0
 *
 *****************************************************************************/
UInt32 smGetDirChanges(smMemory_t *Memory) {

    return bmGetStorageItem (Memory->Handle, SM_PARAM_DIRCHG_COUNTER, 0);
}


/******************************************************************************/
/*!
 *  \brief   Flush life cycle data to non-volatile storage 
 *
 *      Updates the life cycle counters in non-volatile storage:
 *
 *      - Operating time
 *      - Number of motor revisions
 *
 *      'Operation time' is updated by incrementing the life cycle counter in
 *       non-volatile storage by the number of hours expired since last updating.
 *
 *      'Revolution count' is updated by overwriting value in non-volatile storage
 *      with new value, if value have changed.
 *
 *      'Direction change count' is updated by overwriting value in non-volatile storage
 *      with new value, if value have changed.
 *
 *
 *  \iparam  Instance = Module instance number
 *  \iparam  Interval = Interval time to flush data [ms]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smFlushMemory(UInt16 Instance, UInt16 Interval) {

    Error_t Status1 = NO_ERROR;
    Error_t Status2 = NO_ERROR;
    Error_t Status3 = NO_ERROR;

    smData_t *Data = &smDataTable[Instance];

    if (bmTimeExpired(Data->Memory.Interval) >= Interval) {
        smLifeCycle_t *LifeCycle = &Data->Motor.LifeCycle;

        if (LifeCycle->Revolutions.Count != smGetRevolution(&Data->Memory)) {
            Status1 = bmSetStorageItem(Data->Memory.Handle, SM_PARAM_REV_COUNTER, LifeCycle->Revolutions.Count);
        }

        if (LifeCycle->OperationTime.Hours) {
            Status2 = bmIncStorageItem(Data->Memory.Handle, SM_PARAM_OPERATE_HOURS, LifeCycle->OperationTime.Hours);
            if (Status2 == NO_ERROR) {
                LifeCycle->OperationTime.Hours = 0;
            }
        }

        if (LifeCycle->DirChanges.Count != smGetDirChanges(&Data->Memory)) {
            Status3 = bmSetStorageItem(Data->Memory.Handle, SM_PARAM_DIRCHG_COUNTER, LifeCycle->DirChanges.Count);
        }

        Data->Memory.Interval = bmGetTime();
    }

    if (NO_ERROR != Status1) {
        return Status1;
    }
    if (NO_ERROR != Status2) {
        return Status2;
    }
    return (Status3);
}


/******************************************************************************/
/*!
 *  \brief   Reset non-volatile data to default
 *
 *      Resets all data in instance's data partition to defaults and sends
 *      an info message about that fact to the master for logging.
 * 
 *  \iparam  Instance = Module instance number
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smResetMemory(UInt16 Instance) {

    Error_t Status;

    smData_t *Data = &smDataTable[Instance];

    if (Data->Memory.Handle >= 0) {
        bmSignalEvent(Data->Channel, I_PARTITION_RESET, 1, 0);

        Status = bmResetStorageItems (Data->Memory.Handle, smPermDataTable, ELEMENTS(smPermDataTable));

        smInitLifeCycleData(&Data->Motor, &Data->Memory);

        return (Status);
    }

    return (E_STORAGE_OPEN_ERROR);
}


/******************************************************************************/
/*!
 *  \brief   Verify module's data partition
 *
 *      Verifies the content of the data partition for plausibility. Each 
 *      data item must be inside the range defined for that item. If only
 *      a single data item is outside it's range, ALL data items are reset 
 *      to defaults. If the partition is ok, the partition layout version 
 *      is checked. If the layout version in the partition is older than 
 *      the one required by the actual software, data layout in the  
 *      partition will be converted.
 * 
 *      If the layout version is zero, all data items in the partition
 *      are set to defaults as well. This may happen, if the partition
 *      was erased due to a checksum error or during manufacturing.
 * 
 *  \iparam  Instance = Module instance number
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
Error_t smVerifyMemory (UInt16 Instance) {

    Error_t RetCode;

    UInt32 LayoutVersion;

    smData_t *Data = &smDataTable[Instance];
    smMemory_t *Memory = &Data->Memory;
            
    if (Memory->Handle >= 0) {
        if (!bmVerifyStorageItems (Memory->Handle, smPermDataTable, ELEMENTS(smPermDataTable))) {
            bmSignalEvent(Data->Channel, E_PERSISTENTS_INVALID, 1, 0);
            if ((RetCode = smResetMemory(Instance)) < 0) {
                return RetCode;
            }
        }
        LayoutVersion = bmGetStorageItem(Memory->Handle, SM_PARAM_LAYOUT_VERSION, 0);

        // if required, upgrade partition to new layout format
        if (LayoutVersion < SM_PARTITION_VERSION) {
            return E_PERSISTENTS_INVALID;
            // currently no conversion required
//            bmSignalEvent(Data->Channel, I_PARTITION_CONVERTED, 1, LayoutVersion);
        }
        else {
            // downgrade not supported
            if (SM_PARTITION_VERSION < LayoutVersion) {
                return E_PERSISTENTS_INVALID;
            }
        }
        return (NO_ERROR);
    }
    return (E_STORAGE_OPEN_ERROR);
}


/******************************************************************************/
/*!
 *  \brief   Open module's data partition
 *
 *      To gain access to modules data in non-volatile storage the data partition
 *      is opened and the content is verified.
 * 
 *  \iparam  Instance = Module instance number
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/
void smInitMemory(UInt16 Instance) {

    Error_t RetCode;

    smData_t *Data = &smDataTable[Instance];
    smMemory_t *Memory = &Data->Memory;

    Memory->Handle = bmOpenPermStorage(MODULE_ID_STEPPER, Instance, SM_PARTITION_SIZE);
    if (Memory->Handle > 0) {
        if ((RetCode = smVerifyMemory(Instance)) < 0) {
            bmSignalEvent(Data->Channel, RetCode, 1, 0);
            if ((RetCode = bmClosePermStorage(Memory->Handle)) < 0) {
                bmSignalEvent(Data->Channel, RetCode, 1, 0);
            }
        }
    }

    Memory->Interval = 0;
}
