/****************************************************************************/
/*! \file halStorage.h
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
 *       but must be rewritten on each startup of the firmware.
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

#ifndef HAL_STORAGE_H
#define HAL_STORAGE_H


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

HANDLE_t halOpenStorage  (CHANNEL_t DeviceID, UInt16 Mode);
ERROR_t  halReadStorage  (HANDLE_t Handle, UInt32 Address, void* buffer, UInt32 Size);
ERROR_t  halWriteStorage (HANDLE_t Handle, UInt32 Address, void* buffer, UInt32 Size);
HANDLE_t halEraseStorage (HANDLE_t Handle, UInt32 Address, UInt32 Size);
HANDLE_t halCloseStorage (HANDLE_t Handle);
UInt32   halStorageSize  (HANDLE_t Handle);

//****************************************************************************/

#endif /*HAL_STORAGE_H*/
