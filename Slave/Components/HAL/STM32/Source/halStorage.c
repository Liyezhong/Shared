/****************************************************************************/
/*! \file halStorage.c
 *
 *  \brief  Logical memory access functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 27.04.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains all functions to access logical memories, no
 *      matter what type the memory is or what bus it is connected to.
 *      The following memories are supported actually:
 *
 *      - Processor internal FLASH memory
 *      - Non-volatile FRAM memory (I2C bus)
 *      - RAM memory
 *
 *      Other memory types may be added later. The RAM memory is intended 
 *      to emulate the other memory types, in cases where the target hard-
 *      ware isn't yet available.
 *
 *      The functions in this module use lower level functions to access
 *      the real hardware, which are in separate source modules. Only the 
 *      RAM memory is implemented directly here.
 *
 *      One or multiple logical memories can reside on a single physical
 *      memory. The size and start addresses of the logical memories are
 *      configured in the HAL configuration file.
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

#include <stdlib.h>
#include "Global.h"
#include "halCommon.h"
#include "halError.h"
#include "halMain.h"
#include "halFlash.h"
#include "halEeprom.h"
#include "halStorage.h"


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Structure to hold the state of a logical memory device
typedef struct {
    UInt8 *Address;     //!< Address pointer (for RAM only)
    UInt8 Flags;        //!< Open flags
} StorageData_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Data table holding the variables for all logical memories
static StorageData_t *DataTable;


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t halStorageGetIndex   (Handle_t Handle, UInt16 Mode);
static Error_t halStorageFindDevice (Device_t DeviceID);



/*****************************************************************************/
/*!
 *  \brief   Open a logical memory
 *
 *      Opens a logical memory and returns a handle to be used to access it.
 *      DeviceID must be the numerical "name" of a logical memory defined in 
 *      the HAL configuration file. Mode determines the access mode to the
 *      memory (read, write, erase, or any combination).
 *
 *      In case of a logical RAM the memory is requested from the heap. The
 *      logical RAM might be useful to emulate other memory types if the 
 *      required hardware isn't yet available.
 *
 *  \iparam  DeviceID = Logical memory ID
 *  \iparam  Mode     = Access mode
 *
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

Error_t halStorageOpen (Device_t DeviceID, UInt16 Mode) {

    const Int32 Index = halStorageFindDevice(DeviceID);

    if (Index >= 0) {
        if (DataTable[Index].Flags & HAL_OPEN_RW) {
            return (E_DEVICE_ALREADY_OPEN);
        }
        if (Mode & ~halStorageDescriptors[Index].Mode) {
            return (E_DEVICE_ACCESS_MODE);
        }
        if (halStorageDescriptors[Index].Class == MEM_CLASS_RAM) {

            UInt8 *Address = calloc (halStorageDescriptors[Index].Size, 1);

            if (Address == NULL) {
                return (E_STORAGE_OPEN_ERROR);
            }
            DataTable[Index].Address = Address;
        }
        DataTable[Index].Flags |= Mode;

        return (DEVICE_CLASS_STORAGE + Index);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Close an open memory
 *
 *      Closes an open logical memory. In case of a logical RAM the memory
 *      is freed and returned to the heap pool.
 *
 *  \iparam  Handle = Handle of open memory
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halStorageClose (Handle_t Handle) {

    const Int32 Index = halStorageGetIndex(Handle, HAL_OPEN_MODE);

    if (Index >= 0) {
        if (halStorageDescriptors[Index].Class == MEM_CLASS_RAM) {
            if (DataTable[Index].Address != NULL) {
                free (DataTable[Index].Address);
            }
        }
        DataTable[Index].Flags &= ~HAL_OPEN_MODE;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Read from logical memory
 *
 *      Reads a number of bytes from the logical memory associated with 
 *      Handle and copies it into Buffer. Address must be the Address to 
 *      read from, relative to the starting address of the logical memory.
 *      Any attempt to read beyond the logical memory is answered with an 
 *      error code. Buffer must be large enough to take 'Count' bytes. 
 *      The memory must have been opened in READ mode to read it.
 *
 *  \iparam  Handle  = Handle of logical memory
 *  \iparam  Address = Address to read from
 *  \xparam  Buffer  = Buffer to return the read data
 *  \iparam  Count   = Number of bytes to read
 *
 *  \return  Number of bytes read or (negative) error code
 *
 ****************************************************************************/

Error_t halStorageRead (
            Handle_t Handle, UInt32 Address, void *Buffer, UInt32 Count) {

    const Int32 Index = halStorageGetIndex(Handle, HAL_OPEN_READ);
    UInt32 i;

    if (Index >= 0) {
        UInt32 Offset = halStorageDescriptors[Index].Address;

        if (Address + Count > halStorageDescriptors[Index].Size) {
            return (E_STORAGE_ADDRESS);
        }    
        switch (halStorageDescriptors[Index].Class) {
            case MEM_CLASS_FRAM:
                return (halEepromRead (Address+Offset, Buffer, Count));

            case MEM_CLASS_FLASH:
                return (halFlashRead (Address, Buffer, Count));

            case MEM_CLASS_RAM:
            {
                UInt8 *Source = DataTable[Index].Address + Address;
                UInt8 *Target = (UInt8*) Buffer;

                for (i=0; i < Count; i++) {
                    Target[i] = Source[i];
                }
                return (Count);
            }
        }
        return (E_STORAGE_CLASS);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Write to logical memory
 *
 *      Writes a number of bytes to the logical memory associated with 
 *      Handle. Address must be the Address to write to, relative to the 
 *      starting address of the logical memory. Buffer must point to the
 *      data to write, Count gives the number of byte to write.
 * 
 *      If Address is greater than the logical memory size or an attempt 
 *      is made to write beyond the limits of the logical memory, an error
 *      is returned. The memory must have been opened in WRITE mode to be
 *      able to write it. If write protection is active, an error is 
 *      returned.
 *
 *  \iparam  Handle  = Handle of logical memory
 *  \iparam  Address = Address to write to
 *  \iparam  Buffer  = Pointer to data to write
 *  \iparam  Count   = Number of bytes to write
 *
 *  \return  Number of bytes written or (negative) error code
 *
 ****************************************************************************/

Error_t halStorageWrite (
            Handle_t Handle, UInt32 Address, void *Buffer, UInt32 Count) {

    const Int32 Index = halStorageGetIndex(Handle, HAL_OPEN_WRITE);
    UInt32 i;

    if (Index >= 0) {
        UInt32 Offset = halStorageDescriptors[Index].Address;

        if (Address + Count > halStorageDescriptors[Index].Size) {
            return (E_STORAGE_ADDRESS);
        }    
        switch (halStorageDescriptors[Index].Class) {
            case MEM_CLASS_FRAM:
                return (halEepromWrite (Address+Offset, Buffer, Count));

            case MEM_CLASS_FLASH:
                return (halFlashWrite (Address, Buffer, Count));

            case MEM_CLASS_RAM:
            {
                UInt8 *Source = DataTable[Index].Address + Address;
                UInt8 *Target = (UInt8*) Buffer;

                for (i=0; i < Count; i++) {
                    Source[i] = Target[i];
                }
                return (Count);
            }
        }
        return (E_STORAGE_CLASS);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Erase logical memory
 *
 *      Erases a number of bytes in the logical memory associated with Handle. 
 *      Address must be the starting address of the memory block to erase, 
 *      relative to the starting address of the logical memory. Count must 
 *      be the number of bytes to erase.
 
 *      If Address is greater than the logical memory size or an attempt is 
 *      made to erase beyond the limits of the logical memory, an error is 
 *      returned. If the memory is write protected the operation is aborted 
 *      and an error returned.
 *
 *      The memory must have been opened in ERASE mode to be able to erase
 *      it. If this is not the case, an error is returned. In case of FLASH
 *      memory, there may be hardware restrictions regarding the address 
 *      alignment (which must be on a page boundary) and the byte count
 *      (which must be a multiple of a page size).
 *
 *  \iparam  Handle  = Handle of logical memory
 *  \iparam  Address = Address of block to erase
 *  \iparam  Count   = Number of bytes to erase
 *
 *  \return  Number of bytes erased or (negative) error code
 *
 ****************************************************************************/

Error_t halStorageErase (Handle_t Handle, UInt32 Address, UInt32 Count) {

    const Int32 Index = halStorageGetIndex(Handle, HAL_OPEN_ERASE);
    UInt32 i;

    if (Index >= 0) {
        UInt32 Offset = halStorageDescriptors[Index].Address;

        if (Address + Count > halStorageDescriptors[Index].Size) {
            return (E_STORAGE_ADDRESS);
        }    
        switch (halStorageDescriptors[Index].Class) {
            case MEM_CLASS_FRAM:
                return (halEepromErase (Address+Offset, Count));

            case MEM_CLASS_FLASH:
                return (halFlashErase (Address, Count));

            case MEM_CLASS_RAM:
            {
                UInt8 *Target = DataTable[Index].Address + Address;

                for (i=0; i < Count; i++) {
                    Target[i] = 0;
                }
                return (Count);
            }
        }
        return (E_STORAGE_CLASS);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Set/remove write protection
 *
 *      Sets or removes the write protection of the memory associated with
 *      Handle. If State is TRUE write protection is activated, if State is 
 *      FALSE it is deactivated. 
 *
 *      The write protection applies to the physical memory, not only the
 *      logical memory. This implies that write protecting a logical memory
 *      may also write protect other logical memories mapped to the same
 *      hardware memory chip. 
 *
 *  \iparam  Handle = Handle of logical memory
 *  \iparam  State  = Write protection state (on, off)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halStorageProtect (Handle_t Handle, Bool State) {

    const Int32 Index = halStorageGetIndex(Handle, HAL_OPEN_WRITE);

    if (Index >= 0) {
        switch (halStorageDescriptors[Index].Class) {
            case MEM_CLASS_FRAM:
                return (halEepromProtect(State));

            case MEM_CLASS_FLASH:
                return (halFlashProtect(State));

            case MEM_CLASS_RAM:
                return (NO_ERROR);
        }
        return (E_STORAGE_CLASS);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Get size of logical memory
 *
 *      Returns the size of the logical memory associated with Handle.
 *      The information is taken from the HAL configuration file. The
 *      size is returned as number of bytes.
 *
 *  \iparam  Handle = Handle of logical memory
 *
 *  \return  Memory size or (negative) error code
 *
 ****************************************************************************/

UInt32 halStorageSize (Handle_t Handle) {

    const Int32 Index = halStorageGetIndex(Handle, HAL_OPEN_RWE);

    if (Index >= 0) {
        return (halStorageDescriptors[Index].Size);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Get base address of logical memory
 *
 *      Returns the base address of the logical memory associated with Handle.
 *      The information is taken from the HAL configuration file.
 *
 *  \iparam  Handle = Handle of logical memory
 *
 *  \return  Memory size or (negative) error code
 *
 ****************************************************************************/

UInt32 halStorageBase (Handle_t Handle) {

    const Int32 Index = halStorageGetIndex(Handle, HAL_OPEN_RWE);

    if (Index >= 0) {
        return (halStorageDescriptors[Index].Address);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Get index of logical memory
 *
 *      Returns the index of a memory associated with Handle. The index is
 *      used to address an entry in the MemoryData table, which contains
 *      all variables of the logical memory.
 *
 *  \iparam  Handle = Handle of logical memory
 *  \iparam  Mode   = Access mode (read, write)
 *
 *  \return  Memory table index or (negative) error code
 *
 ****************************************************************************/

static Error_t halStorageGetIndex (Handle_t Handle, UInt16 Mode) {

    register const UInt32 Index = Handle ^ DEVICE_CLASS_STORAGE;

    if (Index < halStorageDescriptorCount) {

        const UInt16 Flags = DataTable[Index].Flags;

        if (!(Flags & HAL_OPEN_RW)) {
            return (E_DEVICE_NOT_OPEN);
        }
        if (!(Flags & Mode)) {
            return (E_DEVICE_ACCESS_MODE);
        }
        return ((Error_t) Index);
    }
    return (E_DEVICE_HANDLE_INVALID);
}


/*****************************************************************************/
/*!
 *  \brief   Search logical memory
 *
 *      Searches the logical memory descriptor for an entry with the asked
 *      DeviceID. If successful, the memory index is returned. If no such 
 *      entry exists, an error is returned.
 *
 *  \iparam  DeviceID = Logical memory ID
 *
 *  \return  Memory table index or (negative) error code
 *
 ****************************************************************************/

static Error_t halStorageFindDevice (Device_t DeviceID) {

    const halStorageDescriptor_t *Descriptor = halStorageDescriptors;
    UInt32 Index;

    if ((DeviceID & DEVICE_CLASS_MASK) == DEVICE_CLASS_STORAGE) {

        halInitState_t InitState = halGetInitState();

        if (InitState == INIT_NOT_DONE || InitState == INIT_FAILED) {
            return (E_HAL_NOT_INITIALIZED);
        }
        for (Index=0; Index < halStorageDescriptorCount; Index++) {

            if (Descriptor->DeviceID == DeviceID) {
                if (~DataTable[Index].Flags & HAL_FLAG_INITZED) {
                    return (E_DEVICE_UNCONFIGURED);
                }
                return (Index);
            }
            Descriptor++; // next descriptor
        }
    }
    return (E_UNKNOWN_DEVICE_ID);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize this module
 *
 *      Initializes this module by evaluating the logical memory descriptor 
 *      table from the global HAL configuration file. There is no check that
 *      different logical memories are mapped to the same physical memory
 *      addresses. This allows to specify a logical memory which is a sub-
 *      set of another logical memory.
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halStorageInit (void) {

    UInt32 StorageTypes = 0;
    Error_t Status;
    UInt32 i;

    if (halGetInitState() == INIT_IN_PROGRESS) {

        DataTable = calloc (halStorageDescriptorCount, sizeof(StorageData_t));
        if (DataTable == NULL) {
            return (E_HEAP_MEMORY_FULL);
        }
        for (i=0; i < halStorageDescriptorCount; i++) {
            StorageTypes |= BIT(halStorageDescriptors[i].Class);
        }
        if (StorageTypes & BIT(MEM_CLASS_FLASH)) {
            if ((Status = halFlashInit()) < 0) {
                return (Status);
            }
        }
        if (StorageTypes & BIT(MEM_CLASS_FRAM)) {
            if ((Status = halEepromInit()) < 0) {
                return (Status);
            }
        }
        for (i=0; i < halStorageDescriptorCount; i++) {
            DataTable[i].Flags = HAL_FLAG_INITZED;
        }
    }
    return (NO_ERROR);
}

//****************************************************************************/

