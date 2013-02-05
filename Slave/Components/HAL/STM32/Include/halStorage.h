/****************************************************************************/
/*! \file halStorage.h
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

#ifndef HAL_STORAGE_H
#define HAL_STORAGE_H


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halStorageOpen  (Device_t DeviceID, UInt16 Mode);
Error_t halStorageClose (Handle_t Handle);
Error_t halStorageInit  (void);

Error_t halStorageRead  (Handle_t Handle, UInt32 Address, void *Buffer, UInt32 Size);
Error_t halStorageWrite (Handle_t Handle, UInt32 Address, void *Buffer, UInt32 Size);
Error_t halStorageErase (Handle_t Handle, UInt32 Address, UInt32 Size);
UInt32  halStorageSize  (Handle_t Handle);
UInt32  halStorageBase  (Handle_t Handle);

Error_t halStorageProtect (Handle_t Handle, Bool State);
Error_t halStorageWait (Handle_t Handle);

//****************************************************************************/

#endif /*HAL_STORAGE_H*/
