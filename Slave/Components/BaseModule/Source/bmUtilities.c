/****************************************************************************/
/*! \file bmUtilities.c
 *
 *  \brief Common utility functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *         This module contains common utility function not associated
 *         to a special module.
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
#include "halLayer.h"
#include "halFlash.h"
#include "bmCommon.h"
#include "bmError.h"
#include "bmCan.h"
#include "bmTime.h"
#include "bmStorage.h"
#include "bmUtilities.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define OPTIONS_MODULID      0     //!< Offset to module ID
#define OPTIONS_INSTANCES    1     //!< Offset to instance count
#define OPTIONS_DATA         2     //!< Offset to data
#define OPTIONS_SIGNATURE    0     //!< Offset to signature (header)
#define OPTIONS_DATACOUNT    1     //!< Offset to data count (header)

#define CRC32_POLYNOMIAL     0x04C11DB7u   //!< CRC32 polynomial
#define CRC32_INITIAL_VALUE  0xFFFFFFFFu   //!< CRC32 start value


//****************************************************************************/
// Private Variables
//****************************************************************************/

static UInt32 *BoardOptions = NULL;   //!< Pointer to board option block
static UInt32  BoardOptionsSize = 0;  //!< Number of board option words

static UInt16  NodeIndex = 0;         //!< Node index (from DIP switch)
static UInt16  NodeType  = 0;         //!< Node type (from board options)



/*****************************************************************************/
/*!
 *  \brief   Get board option word
 *
 *      An option data block is resident in the one-time-programmable flash
 *      memory of the processor. It contains one option word for each module
 *      instance. Using this function, the option word of a certain module
 *      instance can be retrieved. The ModuleID and Instance parameter
 *      selects which option word to return. If the requested option can't
 *      be found in the option list, the supplied Default is returned.
 *
 *      If the Instance is MODUL_INSTANCES, the number of instances of that
 *      module is returned. If this information is not present, null is
 *      returned.
 *
 *  \iparam  ModuleID = Module ID
 *  \iparam  Instance = Module instance
 *  \iparam  Default  = Value to return if option is not available
 *
 *  \return  Board option word (or default)
 *
 ****************************************************************************/

UInt32 bmGetBoardOptions (UInt16 ModuleID, UInt16 Instance, UInt32 Default) {

    UInt32 *Options = BoardOptions;

    while (Options < BoardOptions + BoardOptionsSize) {

        if (Options[OPTIONS_MODULID] == ModuleID) {

            if (Instance == MODUL_INSTANCES) {
                return (Options[OPTIONS_INSTANCES]);
            }
            if (Options[OPTIONS_INSTANCES] <= Instance) {
                return (Default);
            }
            return (Options[OPTIONS_DATA + Instance]);
        }
        Options += Options[OPTIONS_INSTANCES] + OPTIONS_DATA;
    }
    if (Instance == MODUL_INSTANCES) {
        return (0);
    }
    return (Default);
}


/*****************************************************************************/
/*!
 *  \brief   Check if the external board option block is valid
 *
 *      This function checks, if the external board option block is valid. It
 *      checks the pointer to the block, the signature and the length of the
 *      data block and it performs a CRC check on it.
 *
 *  \iparam  Handle = Handle of the internal flash memory
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmCheckBoardOptionBlock (Handle_t Handle) {

    UInt32 Length;

    if (BoardOptions == NULL) {
        return (E_UNEXPECTED_PARAMETERS);
    }

    if (BoardOptions[OPTIONS_SIGNATURE] != INFOBLOCK_SIGNATURE) {
        return (E_UNEXPECTED_PARAMETERS);
    }

    Length = BoardOptions[OPTIONS_DATACOUNT];
    if ((UInt32) BoardOptions >= halStorageBase (Handle) && 
            (UInt32) BoardOptions < halStorageBase (Handle) + halStorageSize (Handle)) {
        if (Length * sizeof(UInt32) > halStorageSize (Handle) - ((UInt32) BoardOptions - halStorageBase (Handle))) {
            return (E_PARAMETER_OUT_OF_RANGE);
        }
    }

    if (bmCalculateCrc (BoardOptions, Length * sizeof(UInt32)) != BoardOptions[Length]) {
        return (E_CHECKSUM_ERROR);
    }

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Locate external board option block
 *
 *      Locates the external board option data block and verifies it by
 *      checking the signature and CRC. If the option block is correct,
 *      the global variable BoardOptions and BoardOptionsSize are set.
 *      If the data block is invalid, BoardOptions and BoardOptionsSize
 *      are set to 0.
 *
 *  \iparam  Handle = Handle of the internal flash memory
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmLocateBoardOptionBlock (Handle_t Handle) {

    Error_t Status = NO_ERROR;

    // Get the data from the internal flash memory
    BoardOptions = (UInt32*) halGetAddress(ADDRESS_BOARD_OPTION_BLOCK_FLASH);
    // Check for validity
    if (bmCheckBoardOptionBlock(Handle) < NO_ERROR) {
        // Get the default data
        BoardOptions = (UInt32*) halGetAddress(ADDRESS_BOARD_OPTION_BLOCK);
        // Check for validity
        if ((Status  = bmCheckBoardOptionBlock(Handle)) < NO_ERROR) {
            return (Status);
        }
    }

    BoardOptionsSize = BoardOptions[OPTIONS_DATACOUNT] - OPTIONS_DATA;
    BoardOptions += OPTIONS_DATA;

    return (Status);
}


/*****************************************************************************/
/*!
 *  \brief   Get board info block
 *
 *      Locates the board info block outside the scope of this program,
 *      checks if it is valid and returns a pointer to that structure.
 *      Validity is checked by checking the signature and verifying
 *      the checksum. If data in the info block is not valid, NULL
 *      is returned.
 *
 *      The board info block is written to a fixed address location in
 *      flash memory during manufacturing.
 *
 *  \return  Pointer to board info block (or NULL)
 *
 ****************************************************************************/

bmBoardInfoBlock_t* bmGetBoardInfoBlock (void) {

    static bmBoardInfoBlock_t *InfoBlock = NULL;
    static Bool IsVerified = FALSE;

    if (!IsVerified) {
        InfoBlock = (bmBoardInfoBlock_t*) halGetAddress(ADDRESS_BOARD_HARDWARE_INFO_FLASH);

        if (InfoBlock == NULL || InfoBlock->Signature != INFOBLOCK_SIGNATURE) {
            // Try to read board info block from stack memory instead
            InfoBlock = (bmBoardInfoBlock_t*) halGetAddress(ADDRESS_BOARD_HARDWARE_INFO);
            if (InfoBlock == NULL || InfoBlock->Signature != INFOBLOCK_SIGNATURE) {
                InfoBlock = NULL;
            }
        }

        if (InfoBlock != NULL) {
            if (bmCalculateCrc(InfoBlock, sizeof(bmBoardInfoBlock_t) - sizeof(UInt32)) != InfoBlock->Checksum) {
                InfoBlock = NULL;
            }
        }

        IsVerified = TRUE;
    }
    return (InfoBlock);
}


/*****************************************************************************/
/*!
 *  \brief   Get bootloader info block
 *
 *      Locates the bootloader info block outside the scope of this program,
 *      checks if it is valid and returns a pointer to that structure.
 *      Validity of the info block is verified by checking the signature
 *      and a checksum. If data in the info block is not valid, NULL
 *      is returned.
 *
 *      The bootloader info block is written to a fixed address location in
 *      flash memory during manufacturing. It is part of the bootloader.
 *
 *  \return  Pointer to bootloader info block (or NULL)
 *
 ****************************************************************************/

bmBootInfoBlock_t* bmGetLoaderInfoBlock (void) {

    static bmBootInfoBlock_t *InfoBlock = NULL;
    static Bool IsVerified = FALSE;

    if (!IsVerified) {
        InfoBlock = (bmBootInfoBlock_t*) halGetAddress(ADDRESS_BOOTLOADER_INFO_FLASH);

        if (InfoBlock == NULL || InfoBlock->Signature != INFOBLOCK_SIGNATURE) {
            // Try to read boot info block from stack memory instead
            InfoBlock = (bmBootInfoBlock_t*) halGetAddress(ADDRESS_BOOTLOADER_INFO);
            if (InfoBlock == NULL || InfoBlock->Signature != INFOBLOCK_SIGNATURE) {
                InfoBlock = NULL;
            }
        }

        if (InfoBlock != NULL) {
            if (bmCalculateCrc(InfoBlock,
                sizeof(bmBootInfoBlock_t) - sizeof(UInt32)) != InfoBlock->Checksum) {
                InfoBlock = NULL;
            }
        }

        IsVerified = TRUE;
    }
    return (InfoBlock);
}


/*****************************************************************************/
/*!
 *  \brief   Returns the node address
 *
 *      Returns the node address of this node. The node address is a
 *      combination of the node type and the node index.
 *
 *  \return  Node address
 *
 ****************************************************************************/

UInt16 bmGetNodeAddress (void) {

    return ((NodeType << 1) | (NodeIndex << 8));
}


/*****************************************************************************/
/*!
 *  \brief   Get item from message's data
 *
 *      Extract a data item from the supplied Message and returns it as a
 *      32 bit unsigned integer. The parameter Offsets specifies the byte
 *      offset of the item inside the message's data, Size specifies the
 *      size of the item in bytes. Items can be from 1-4 bytes in length.
 *      If size is 0 or the parameters are invalid, 0 is returned.
 *
 *  \oparam  Message = CAN message buffer
 *  \iparam  Offset  = Byte offset in message buffer
 *  \iparam  Size    = Size of data item (bytes)
 *
 *  \return  Value of data item
 *
 ****************************************************************************/

UInt32 bmGetMessageItem (CanMessage_t *Message, UInt16 Offset, UInt16 Size) {

    UInt32 Result = 0;

    if (Message != NULL && (Offset+Size <= 8)) {
        while (Size--) {
            Result = (Result << 8) + Message->Data[Offset++];
        }
    }
    return (Result);
}


/*****************************************************************************/
/*!
 *  \brief   Set item in message's data
 *
 *      Sets a data item in the data area of the supplied message to the
 *      given Value. The parameter Offsets specifies the byte offset of
 *      the item inside the message's data, Size specifies the size of
 *      the item in bytes. Items can be from 1-4 bytes in length. If
 *      the parameters are invalid, the message is left touched.
 *
 *  \oparam  Message = CAN message buffer
 *  \iparam  Value   = New value of data item
 *  \iparam  Offset  = Byte offset in message buffer
 *  \iparam  Size    = Size of data item (bytes)
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void bmSetMessageItem (
        CanMessage_t *Message, UInt32 Value, UInt16 Offset, UInt16 Size) {

    if (Message != NULL && (Offset+Size <= 8)) {
        while (Size--) {
            Message->Data[Offset++] = (UInt8)(Value >> (Size * 8)) & 0xFF;
        }
    }
    return;
}


/*****************************************************************************/
/*!
 *  \brief   Returns a standardized partition ID
 *
 *      Returns a standardized partition ID to be used to identify a data
 *      partition in nonvolatile storage. The partition ID is calculated
 *      using the supplied ModuleID and Instance. If a module wants to
 *      use a single partition for all instances the instance number 0
 *      should be supplied.
 *
 *  \iparam  ModuleID = Module ID
 *  \iparam  Instance = Module instance
 *
 *  \return  calculated partition ID
 *
 ****************************************************************************/

UInt32 bmGetPartitionID (UInt16 ModuleID, UInt16 Instance) {

    return ((ModuleID << 8) | (Instance & 0xFF));
}


/*****************************************************************************/
/*!
 *  \brief   Returns absolute delta of two values
 *
 *      Returns the absolute delta of the two parameters. Absolute delta
 *      means, that the result is always positive.
 *
 *  \iparam  Value1 = 1st value
 *  \iparam  Value2 = 2nd value
 *
 *  \return  absolute delta of value1 and value2
 *
 ****************************************************************************/

UInt16 bmGetDelta (UInt16 Value1, UInt16 Value2) {

    if (Value1 > Value2) {
        return (Value1 - Value2);
    }
    return (Value2 - Value1);
}


/*****************************************************************************/
/*!
 *  \brief   Checks if password is correct
 *
 *      Calculates a password using year, month, day, and hour component of
 *      the current date/time and compares it with the supplied "Password".
 *      If the passwords match, TRUE is returned; if not, FALSE is returned.
 *      If date/time is not valid (not yet send by master) the password is
 *      treated as invalid.
 *
 *  \iparam  Password = Password to verify
 *
 *  \return  Password correct (TRUE) or incorrect (FALSE)
 *
 ****************************************************************************/

Bool bmCheckPassword (UInt16 Password) {

#ifdef RTC_ENABLE
    UInt16 Calculated;
    DateTime_t Time;

    #ifdef DEBUG
    if (Password == 99) {
        return (TRUE);
    }
    #endif

    if (bmReadDateTime (&Time) == NO_ERROR) {
        Calculated =
           ((Time.Year * Time.Month * Time.Day) + Time.Hours) ^ 0x8320u;

        if (Password == Calculated) {
            return (TRUE);
        }
    }
    return (FALSE);
#else 
    return (TRUE);
#endif
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
 *  \iparam  Datablock = Pointer to the data block
 *  \iparam  Length    = Size of data block (in bytes)
 *
 *  \return  CRC32 value
 *
 ****************************************************************************/

UInt32 bmCalculateCrc (void *Datablock, UInt32 Length) {

    UInt8 *DataPtr = (UInt8*)Datablock;
    UInt32 Crc32 = CRC32_INITIAL_VALUE;
    UInt32 i, k;

    // Perform modulo-2 division for each byte
    for (i=0; i < Length; i++) {

        Crc32 ^= DataPtr[i] << (32 - 8);

        // Perform modulo-2 division for each bit in byte
        for (k=0; k < 8; k++) {
            if (Crc32 & BIT(31)) {
                Crc32 = (Crc32 << 1) ^ CRC32_POLYNOMIAL;
            }
            else {
                Crc32 = (Crc32 << 1);
            }
        }
    }
    return (Crc32 ^ MAX_UINT32);
} 


/*****************************************************************************/
/*!
 *  \brief   Initializes this module
 *
 *      This function initializes this module. It reads the node type from
 *      the option data block and node index from a hardware DIP switch
 *      connected to a digital input port. If a DIP switch is not present
 *      and not enabled in the board options, the node index is taken
 *      from the board options as well.
 *
 *      This function is called once during startup!
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmInitUtilities (void) {

    bmBoardInfoBlock_t *BoardInfo = NULL;
    Handle_t Handle;
    UInt32 Options;
    Error_t Status;

    // Get a handle for the internal flash memory
    if ((Handle = halStorageOpen(HAL_STORAGE_FLASH, HAL_OPEN_READ)) < NO_ERROR) {
        return (Handle);
    }

    // Set the data block addresses
    halSetAddress(ADDRESS_BOARD_HARDWARE_INFO_FLASH, (void *) (
            halStorageBase(Handle) + halStorageSize(Handle) - 3 * halFlashBlockSize()));
    halSetAddress(ADDRESS_BOOTLOADER_INFO_FLASH, (void *) (
            halStorageBase(Handle) + halStorageSize(Handle) - 2 * halFlashBlockSize()));
    halSetAddress(ADDRESS_BOARD_OPTION_BLOCK_FLASH, (void *) (
            halStorageBase(Handle) + halStorageSize(Handle) -     halFlashBlockSize())); 

    // Locates the board info block
    if ((BoardInfo = bmGetBoardInfoBlock()) == NULL) {
        return (E_INFOBLOCK_MISSING);
    }

    // localize board options in external flash (if not already done)
    if (!BoardOptionsSize) {
        if ((Status = bmLocateBoardOptionBlock(Handle)) < NO_ERROR) {
            return (Status);
        }
    }

    // Closing the flash memory handle
    if ((Status = halStorageClose(Handle)) < NO_ERROR) {
        return (Status);
    }

    if ((Options = bmGetBoardOptions (BASEMODULE_MODULE_ID, OPTIONS_BASEMODULE, 0)) == NULL) {
        return (E_BOARD_OPTION_MISSING);
    }

    // get node type and index from board options
    NodeType = BoardInfo->NodeType;
    if (NodeType > CANiD_MAX_NODETYPE || NodeType < 1) {
        return (E_ILLEGAL_NODE_TYPE);
    }
    NodeIndex = bmGetBoardOptions (BASEMODULE_MODULE_ID, OPTIONS_NODE_INDEX, 0);

    // read node index form DIP switch (if present)
    if (Options & OPTION_NODE_INDEX_DIP) {
        // read-in dip switch to get node index
        if ((Handle = halPortOpen (HAL_CAN_NODE_INDEX, HAL_OPEN_READ)) < NO_ERROR) {
            return (Handle);
        }
        if ((Status = halPortRead (Handle, &NodeIndex)) < NO_ERROR) {
            return (Status);
        }
        if ((Status = halPortClose (Handle)) < NO_ERROR) {
            return (Status);
        }
    }
    if (NodeIndex > CANiD_MAX_NODEINDEX) {
        return (E_ILLEGAL_NODE_INDEX);
    }
    return (NO_ERROR);
}

//****************************************************************************/
