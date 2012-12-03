/****************************************************************************/
/*! \file halUtilities.c
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
 *      - Address conversion (logical to pyhsical)
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

#include <stdlib.h>
#include "Global.h"
#include "halClocks.h"
#include "halCommon.h"
#include "halDeviceID.h"
#include "halError.h"
#include "halMain.h"
#include "halUtilities.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define CRC_CR_RESET      0x0001    //!< Reset CRC calculation unit

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Layout of processor internal CRC calculation unit registers
typedef volatile struct {
    UInt32 DR;              //!< CRC data register
    UInt32 IDR;             //!< independent data register (8 bits)
    UInt32 CR;              //!< CRC control register
} CrcRegFile_t;

//! Layout of Device Electronic Signature (DES) registers
typedef volatile struct {
    UInt16 FSR;             //!< Flash memory size (kByte)
    UInt16 RESERVED[3];     //!< Reserved
    UInt8  UID[12];         //!< Unique identifier (96 bit)
} DesRegFile_t;



//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Hardware registers of the internal CRC calculation unit
static volatile CrcRegFile_t *CRC = (CrcRegFile_t*) 0x40023000;

//! Hardware registers of the device electronic identifier
static volatile DesRegFile_t *DES = (DesRegFile_t*) 0x1FFFF7E0;

//! Pointer to logical-to-physical address lookup table
static void **halAdressTable = NULL;



/*****************************************************************************/
/*!
 *  \brief   Get physical address
 *
 *      Converts the supplied logical AddressID to a physical address and
 *      returns it. This allows access to program-extern informations like:
 *
 *      - Board option block
 *      - Board information block (hw version, etc.)
 *      - Bootloader information block (version number, etc.)
 *      - Bootloader startup entry point
 *      - Bootloader software update entry point
 *
 *  \iparam  AddressID = Logical address ID
 *
 *  \return  Physical address or NULL in case of error
 *
 ****************************************************************************/

void *halGetAddress (AddressID_t AddressID) {

    if (halAdressTable != NULL) {
        if (AddressID < NUMBER_OF_ADDRESS_IDS) {
           return (halAdressTable[AddressID]);
        }
    }
    return (NULL);
}


/*****************************************************************************/
/*!
 *  \brief   Set physical address
 *
 *      Assigns the supplied logical AddressID to a physical address. This
 *      allows access to program-extern informations like:
 *
 *      - Board option block
 *      - Board information block (hw version, etc.)
 *      - Bootloader information block (version number, etc.)
 *      - Bootloader startup entry point
 *      - Bootloader software update entry point
 *
 *      Normally, the assignment is done from outside of the firmware by the
 *      boot loader. This function is used during debugging only, where the
 *      vectors are not set externally.
 *
 *  \iparam  AddressID = Logical address ID
 *  \iparam  Address   = Physical address
 *
 *  \return  Physical address or NULL in case of error
 *
 ****************************************************************************/

void *halSetAddress (AddressID_t AddressID, void *Address) {

    if (halAdressTable == NULL) {
        halAdressTable = calloc(NUMBER_OF_ADDRESS_IDS, sizeof(void*));
    }
    if (halAdressTable != NULL) {
        if (AddressID < NUMBER_OF_ADDRESS_IDS) {
           halAdressTable[AddressID] = Address;
           return (Address);
        }
    }
    return (NULL);
}


/*****************************************************************************/
/*!
 *  \brief   Get unique device identifier
 *
 *      If the hardware offers a unique device identifier, this function
 *      returns a pointer to this identifier in Buffer. In case of the STM32 
 *      the identifier is 96 bits long, so an array of twelve bytes is 
 *      returned. The return value is the size of the ID in number of bytes.
 *      If the hardware doesn't offer a unique device ID, 0 is returned.
 *
 *  \iparam  Buffer = Pointer to ID buffer pointer
 *
 *  \return  Size of the unique ID or (negative) error code
 *
 ****************************************************************************/

UInt16 halGetUniqueID (const UInt8 **Buffer) {

    if (Buffer != NULL) {
        *Buffer = (const UInt8*) DES->UID;
    }
    return (ELEMENTS(DES->UID));
}


/*****************************************************************************/
/*!
 *  \brief   Get program memory size
 *
 *      Returns the size of the memory in Bytes. Depending on MemoryID one
 *      of the following sizes are returned:
 *
 *      - Flash memory size
 *      - RAM size
 *
 *      If an unknown MemoryID is supplied, a size of 0 is returned. 
 *
 *  \iparam  MemoryID = ID of memory
 *
 *  \return  Size of selected storage [Bytes]
 *
 ****************************************************************************/

UInt32 halGetMemorySize (MemoryID_t MemoryID) {

    switch (MemoryID) {
        case MEMORY_SIZE_FLASH:
            if (halProcessorInfo.SizeFLASH >= DES->FSR ||
                halProcessorInfo.SizeFLASH == 0) {
                return (DES->FSR * 1024);
            }
            return (halProcessorInfo.SizeFLASH * 1024);
            
        case MEMORY_SIZE_RAM:
            return (halProcessorInfo.SizeRAM * 1024);
    }
    return (0);
}


/*****************************************************************************/
/*!
 *  \brief   Calculates the CRC checksum over a data block
 *
 *      Calculates the CRC32 over the supplied data block and returns the
 *      complement of the remainder. The polynomial used is:
 *
 *          x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
 *
 *  \iparam  Datablock = Pointer to data block
 *  \iparam  Length    = Number of 32 bit values
 *
 *  \return  CRC32 checksum over data block
 *
 ****************************************************************************/

UInt32 halCalculateCrc32 (UInt32 *Datablock, UInt32 Length) {

    UInt32 i;

    CRC->CR = CRC_CR_RESET;    // reset crc unit for new calculation

    for (i=0; i < Length; i++) {
        CRC->DR = *Datablock++;
    }
    return (CRC->DR ^ MAX_UINT32);
}


/*****************************************************************************/
/*!
 *  \brief   Returns highest bit set
 *
 *      Finds the most significant bit set in parameter Value. The bits are
 *      numbered 0...31, where 0 is the least and 31 the most significant
 *      bit. If no bit is set in Value, 0xFFF.. is returned.
 *
 *  \iparam  Value = Value to analyze
 *
 *  \return  Most significant bit set
 *
 ****************************************************************************/

UInt32 halGetHighestBitSet (UInt32 Value) {

    return (31 - __clz(Value));
}


/*****************************************************************************/
/*!
 *  \brief   Initialize utility module
 *
 *      Initializes the utilities module. This includes setting up the
 *      logical-to-physical address lookup table pointer and the setup
 *      of the processor internal CRC calculation hardware. 
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halUtilitiesInit (void) {

    if (halGetInitState() == INIT_IN_PROGRESS) {
        if (halAdressTable == NULL) {
            halAdressTable = calloc(NUMBER_OF_ADDRESS_IDS, sizeof(void*));
        }
        halPeripheralClockEnable (PERIPHERAL_CRC, ON);
    }
    return (NO_ERROR);
}

//****************************************************************************/
