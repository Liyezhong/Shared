/****************************************************************************/
/*! \file halUtilities.h
 *
 *  \brief  Hardware Utility Functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains common utility function not associated
 *      to another module. This includes:
 *
 *      - Power monitoring
 *      - CRC checksum calculation
 *      - Unique hardware identifier
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

#ifndef HAL_UTILITIES_H
#define HAL_UTILITIES_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

#define SIZEOF_UNIQUE_DEVICE_ID   96   //!< Size of unique hardware id (bits)

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Logical address IDs (address identifier)
typedef enum {
    ADDRESS_BOARD_OPTION_BLOCK        = 0,  //!< Board options list
    ADDRESS_BOARD_HARDWARE_INFO       = 1,  //!< Board hardware info block address
    ADDRESS_BOOTLOADER_STARTUP        = 2,  //!< Bootloader startup entry address
    ADDRESS_BOOTLOADER_SIGNATURE      = 3,  //!< Bootloader signature address
    ADDRESS_BOOTLOADER_UPDATE         = 4,  //!< Bootloader sw update entry address
    ADDRESS_BOOTLOADER_INFO           = 5,  //!< Bootloader info block
    ADDRESS_BOARD_OPTION_BLOCK_FLASH  = 6,  //!< Board options list written into flash
    ADDRESS_BOARD_HARDWARE_INFO_FLASH = 7,  //!< Board hardware info block address written into flash
    ADDRESS_BOOTLOADER_INFO_FLASH     = 8,  //!< Bootloader info block written into flash
    NUMBER_OF_ADDRESS_IDS                   //!< Number of address identifiers
} AddressID_t;

//! Identifiers to request the various memory sizes
typedef enum {
    MEMORY_SIZE_FLASH,                 //!< Flash memory size
    MEMORY_SIZE_RAM                    //!< RAM memory size
} MemoryID_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

void*  halGetAddress     (AddressID_t AddressID);
void*  halSetAddress     (AddressID_t AddressID, void *Address);

UInt16 halGetUniqueID    (const UInt8 **Buffer);
UInt32 halCalculateCrc32 (UInt32 *Datablock, UInt32 Length);
UInt32 halGetMemorySize  (MemoryID_t MemoryID);
Bool   halPowerGood      (void);
Error_t halUtilitiesInit (void);

UInt32 halGetHighestBitSet (UInt32 Value);

//****************************************************************************/

#endif /*HAL_UTILITIES_H*/
