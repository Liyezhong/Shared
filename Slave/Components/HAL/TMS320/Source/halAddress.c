/****************************************************************************/
/*! \file halAddress.c
 * 
 *  \brief  Address access functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 12.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to convert logical to physical 
 *       addresses, to allow access to program-external informations
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
#include "halCommon.h"
#include "halError.h"
#include "halAddress.h"

//****************************************************************************/
// Private Constant Definitions 
//****************************************************************************/

/*! Fixed address vectors (uses INT1...INT6 vectors) */
#define VECTOR_RESET                (void**)0x000D00
#define VECTOR_BOOTLOADER_STARTUP   (void**)0x000D02   
#define VECTOR_BOOTLOADER_UPDATE    (void**)0x000D04  
#define VECTOR_BOOTLOADER_SIGNATURE (void**)0x000D06   
#define VECTOR_BOOTLOADER_INFO      (void**)0x000D08   
#define VECTOR_BOARD_OPTIONS        (void**)0x000D0A   
#define VECTOR_BOARD_INFO           (void**)0x000D0C   


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
 *      - Processor reset entry point
 * 
 *      The unused vector table of the processor is used to store the
 *      address vectors (the old vector table is obsolete because the
 *      peripheral expansion unit has it's own vector table).
 * 
 *  \iparam  AddressID = Logical address ID
 * 
 *  \return  Physical address
 *
 ****************************************************************************/

void *halGetAddress (ADDRESS_IDENTIFIER_t AddressID)
{
    switch (AddressID) {
        case ADDRESS_BOARD_OPTION_BLOCK:
            return (*VECTOR_BOARD_OPTIONS);

        case ADDRESS_BOARD_HARDWARE_INFO:
            return (*VECTOR_BOARD_INFO);
            
        case ADDRESS_BOOTLOADER_SIGNATURE:
            return (*VECTOR_BOOTLOADER_SIGNATURE);
            
        case ADDRESS_BOOTLOADER_INFO:
            return (*VECTOR_BOOTLOADER_INFO);            
    
        case ADDRESS_BOOTLOADER_STARTUP:
            return (*VECTOR_BOOTLOADER_STARTUP);
            
        case ADDRESS_BOOTLOADER_UPDATE:
            return (*VECTOR_BOOTLOADER_UPDATE);
            
        case ADDRESS_PROCESSOR_RESET:
            return (*VECTOR_RESET);            
    }
    return (NULL);
}


/*****************************************************************************/
/*! 
 *  \brief   Set physical address
 *
 *      Assigns the supplied logical AddressID to a physical address. This 
 *      allows access to prgram-extern informations like:
 * 
 *      - Board option block
 *      - Board information block (hw version, etc.)
 *      - Bootloader information block (version number, etc.)
 *      - Bootloader startup entry point
 *      - Bootloader software update entry point
 *      - Processor reset entry point
 * 
 *      Normally, the assignment is done from outside of the firmware by the
 *      boot loader. This function is used during debugging only, where the
 *      vectors are not set externally.
 * 
 *  \iparam  AddressID = Logical address ID
 *  \iparam  Address   = Physical address
 * 
 *  \return  Physical address
 *
 ****************************************************************************/

ERROR_t halSetAddress (ADDRESS_IDENTIFIER_t AddressID, void *Address)
{
    halUnprotectRegisters();
    
    switch (AddressID) {
        case ADDRESS_BOARD_OPTION_BLOCK:
            *VECTOR_BOARD_OPTIONS = Address; break;

        case ADDRESS_BOARD_HARDWARE_INFO:
            *VECTOR_BOARD_INFO = Address; break;
            
        case ADDRESS_BOOTLOADER_SIGNATURE:
            *VECTOR_BOOTLOADER_SIGNATURE = Address; break;            

        case ADDRESS_BOOTLOADER_INFO:
            *VECTOR_BOOTLOADER_INFO = Address; break;
            
        case ADDRESS_BOOTLOADER_STARTUP:
            *VECTOR_BOOTLOADER_STARTUP = Address; break;
            
        case ADDRESS_BOOTLOADER_UPDATE:
            *VECTOR_BOOTLOADER_UPDATE = Address; break;            
    }
    halProtectRegisters();
    
    return (NO_ERROR);
}

//****************************************************************************/
