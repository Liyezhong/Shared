/****************************************************************************/
/*! \file halStorage.c
 * 
 *  \brief  Non-volatile storage access functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 10.05.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to access a non-volatile serial
 *       memory and the processor internal flash memory.
 * 
 *       ATTENTION: The real storage access isn't implemented until now.
 *       Access to non-volatile storage is redirected to a memory block
 *       on the heap. The consequence is that data are not non-volatile, 
 *       but must be rewritten on each startup of the firmware. This is
 *       for debugging only.
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
#include "Global.h"
#include "halError.h"
#include "halConfigure.h"
#include "halCommon.h"
#include "halStorage.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define STORAGE_SIZE                400

//****************************************************************************/
// Implementation 
//****************************************************************************/

HANDLE_t halOpenStorage (CHANNEL_t DeviceID, UInt16 Mode)
{
    UInt8 *Storage;
    
    if (DeviceID != HAL_STORAGE_FRAM) {
        return (halSetError(E_RESSOURCE_NOT_EXISTS));
    }
    Storage = calloc (halStorageSize(0), sizeof(UInt8));     
    if (isPtrNull(Storage)) {
        return (halSetError(E_STORAGE_OPEN_ERROR));
    }
    return ((HANDLE_t)Storage);
}

//****************************************************************************/

HANDLE_t halCloseStorage (HANDLE_t Handle)
{
    UInt8 *Storage = (UInt8*)Handle;

    if (Handle > 0) {
        free(Storage);
        Storage = NULL;
    }
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t halReadStorage (HANDLE_t Handle, UInt32 Address, void* buffer, UInt32 Size)
{
    UInt8 *Storage = (UInt8*)Handle;
    UInt8 *Destination = buffer;
    UInt32 Count;
    
    if (Handle <= 0) {
        return (halSetError(E_STORAGE_READ_ERROR));
    }
    if (Address + Size > halStorageSize(0)) {
        return (halSetError(E_PARAMETER_OUT_OF_RANGE));
    }
    for (Count=0; Count < Size; Count++) {
        Destination[Count] = Storage[Address+Count];
    }    
    return (Count);
}

//****************************************************************************/

ERROR_t halWriteStorage (HANDLE_t Handle, UInt32 Address, void* buffer, UInt32 Size)
{
    UInt8 *Storage = (UInt8*)Handle;
    UInt8 *Source = buffer;
    UInt32 Count;
    
    if (Handle <= 0) {
        return (halSetError(E_STORAGE_WRITE_ERROR));
    }
    if (Address + Size > halStorageSize(0)) {
        return (halSetError(E_PARAMETER_OUT_OF_RANGE));
    }
    for (Count=0; Count < Size; Count++) {
        Storage[Address+Count] = Source[Count];
    }    
    return (Count);
}

//****************************************************************************/

HANDLE_t halEraseStorage (HANDLE_t Handle, UInt32 Address, UInt32 Size)
{
    char buffer[32] = {0};
    UInt32 Count;

    if (Size > halStorageSize(Handle)) {
        return (halSetError(E_PARAMETER_OUT_OF_RANGE));
    }        
    Count = MIN(sizeof(buffer), Size);

    while (Count) {        
        halWriteStorage (Handle, Address, buffer, Count);
        Address += Count; Size -= Count;
        Count = MIN(sizeof(buffer), Size);
    }
    return (NO_ERROR);
}

//****************************************************************************/

UInt32 halStorageSize (HANDLE_t Handle)
{
    UInt32 Size = STORAGE_SIZE;
    
    // return word count on TMS320 plattform
    if (BITS_PER_BYTE == 16) {
        Size = STORAGE_SIZE / 2;
    }
    return (Size);
}

//****************************************************************************/

