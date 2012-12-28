/****************************************************************************/
/*! \file halAddress.h
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

#ifndef HAL_ADDRESS_H
#define HAL_ADDRESS_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

//! Logical address IDs (address identifier)
typedef enum {
    ADDRESS_BOARD_OPTION_BLOCK,     //!< Board options list
    ADDRESS_BOARD_HARDWARE_INFO,    //!< Board hardware info block address
    ADDRESS_BOOTLOADER_STARTUP,     //!< Bootloader startup entry address
    ADDRESS_BOOTLOADER_SIGNATURE,   //!< Bootloader signature address
    ADDRESS_BOOTLOADER_UPDATE,      //!< Bootloader sw update entry address
    ADDRESS_BOOTLOADER_INFO,        //!< Bootloader info block 
    ADDRESS_PROCESSOR_RESET         //!< processor reset address 
} ADDRESS_IDENTIFIER_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

void   *halGetAddress (ADDRESS_IDENTIFIER_t AddressID);
ERROR_t halSetAddress (ADDRESS_IDENTIFIER_t AddressID, void *Address);

//****************************************************************************/

#endif /*HAL_ADDRESS_H*/
