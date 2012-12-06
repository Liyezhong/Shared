/****************************************************************************/
/*! \file bmUpdate.c
 *
 *  \brief Boot loader update functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 09.10.2012
 *  $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *         This module contains functions that update the boot loader of the
 *         Slave. Some of them are also used by the boot loader to itself to
 *         to perform a regular software update.
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
#include "bmCan.h"
#include "bmCommon.h"
#include "bmDispatch.h"
#include "bmError.h"
#include "bmUpdate.h"
#include "bmUtilities.h"
#include "halStorage.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define CRC32_POLYNOMIAL     0x04C11DB7u   //!< CRC32 polynomial
#define CRC32_INITIAL_VALUE  0xFFFFFFFFu   //!< CRC32 start value


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Update state definitions
typedef enum {
    UPDATE_STATE_INITIALIZED,   //!< Update mode initialized
    UPDATE_STATE_STARTED        //!< Update in progress
} bmUpdateState_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

static Handle_t bmHandleFlash;  //!< Handle for the flash memory
static bmUpdateState_t bmUpdateState = UPDATE_STATE_INITIALIZED;  //!< Update State
static UInt32 bmImageLength;    //!< Length of the boot loader image
static UInt32 bmImageOffset;    //!< Offset to start within the image flash memory area
static UInt32 bmImageCount;     //!< Amount of data already programmed to flash memory

static const UInt32 blSectorSize = 0x6000;  //!< The size of the flash sector used by the boot loader

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t bmUpdateHeader (UInt16 Channel, CanMessage_t *Message);
static Error_t bmUpdateData (UInt16 Channel, CanMessage_t *Message, UInt8 Count);
static Error_t bmUpdateData0 (UInt16 Channel, CanMessage_t *Message);
static Error_t bmUpdateData1 (UInt16 Channel, CanMessage_t *Message);
static Error_t bmUpdateTrailer (UInt16 Channel, CanMessage_t *Message);


/*****************************************************************************/
/*!
 *  \brief   Handles update header messages
 *
 *      This function is called, when a update header message is received from
 *      the master. The message contains two parameters, the length of the
 *      image and the starting offset within the target area of the flash
 *      memory. The function erases the required area in the flash memory. It
 *      also returns an acknowledgement message to the master. This message
 *      indicates, if the update is possible or if the image can not fit into
 *      flash. It also changes the update state to started.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmUpdateHeader (UInt16 Channel, CanMessage_t *Message)
{
    Error_t Status;
    UInt8 State = 0;
    CanMessage_t Response;

    if (Message->Length < 8) {
        return (E_MISSING_PARAMETERS);
    }

    bmImageLength = bmGetMessageItem (Message, 0, 4);
    bmImageOffset = bmGetMessageItem (Message, 4, 4);
    bmImageCount = 0;

    if (bmGetBootLoaderSize() < bmImageLength) {
        State = 1;
    }
    else if (bmGetBootLoaderSize() < bmImageOffset + bmImageLength) {
        State = 2;
    }

    if (State == 0) {
        if (bmImageOffset == 0) {
            if ((Status = halStorageErase(bmHandleFlash, 0, bmImageLength)) < NO_ERROR) {
                return (Status);
            }
        }

        bmUpdateState = UPDATE_STATE_STARTED;
    }

    Response.CanID = MSG_ASM_UPDATE_HEADER_ACK;
    bmSetMessageItem (&Response, State, 0, 1);
    Response.Length = 1;

    return (canWriteMessage (Channel, &Response));
}


/*****************************************************************************/
/*!
 *  \brief   Handles update data messages
 *
 *      This function is called, when a update data message is received from
 *      the master. It basicaly writes the message data into flash memory. The
 *      message is only processed, when the update process is in the started
 *      state. The data messages implement a simple flow control mechanism to
 *      recognize double CAN frames. The function also returns an
 *      acknowledgement message to the master. This message indicates, if the
 *      data has been sucessfully written.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *  \iparam  Count = Flow control count (0 or 1)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmUpdateData (UInt16 Channel, CanMessage_t *Message, UInt8 Count)
{
    UInt32 Length;
    Error_t Status;
    CanMessage_t Response;

    if (bmUpdateState != UPDATE_STATE_STARTED) {
        return (NO_ERROR);
    }
    if (bmImageCount + Message->Length > bmImageLength) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if ((bmImageCount / 8) % 2 != Count) {
        return (NO_ERROR);
    }

    if (Message->Length % 2 == 1) {
        Length = Message->Length + 1;
    }
    else {
        Length = Message->Length;
    }
    if ((Status = halStorageWrite (bmHandleFlash, bmImageOffset + bmImageCount, Message->Data, Length)) < NO_ERROR) {
        return (Status);
    }
    bmImageCount += Message->Length;

    if (Count == 0) {
        Response.CanID = MSG_ASM_UPDATE_ACK_0;
    }
    else {
        Response.CanID = MSG_ASM_UPDATE_ACK_1;
    }
    Response.Length = 0;
    return (canWriteMessage (Channel, &Response));
}


/*****************************************************************************/
/*!
 *  \brief   Calls the message handler for update data messages
 *
 *      This function calls the message handler for update data messages.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
static Error_t bmUpdateData0 (UInt16 Channel, CanMessage_t *Message)
{
    return (bmUpdateData (Channel, Message, 0));
}


/*****************************************************************************/
/*!
 *  \brief   Calls the message handler for update data messages
 *
 *      This function calls the message handler for update data messages.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
static Error_t bmUpdateData1 (UInt16 Channel, CanMessage_t *Message)
{
    return (bmUpdateData (Channel, Message, 1));
}


/*****************************************************************************/
/*!
 *  \brief   Handles update trailer messages
 *
 *      This function is called, when a update trailer message is received
 *      from the master. The message contains the CRC checksum of the image.
 *      It is only processed, when the boot loader is in the started state.
 *      The function calculates the CRC checksum of the data written to flash
 *      memory. It also returns an acknowledgement message to the master.
 *      This message indicates, if the update has been successful or if the
 *      calculated checksum deviates from the intended checksum. It also
 *      changes the boot loader state to initialized.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmUpdateTrailer (UInt16 Channel, CanMessage_t *Message)
{
    Error_t Status;
    UInt8 State = 0;
    CanMessage_t Response;

    if (Message->Length < 4) {
        return (E_MISSING_PARAMETERS);
    }
    if (bmUpdateState != UPDATE_STATE_STARTED) {
        return (NO_ERROR);
    }

    if (bmImageCount != bmImageLength) {
        State = 2;
    }
    else {
        UInt32 ActualCrc;
        UInt32 DesiredCrc = bmGetMessageItem (Message, 0, 4);

        if ((Status = bmFlashCrc (&ActualCrc, bmHandleFlash, 0, bmImageOffset + bmImageLength)) < NO_ERROR) {
            return (Status);
        }
        if (ActualCrc != DesiredCrc) {
            State = 1;
        }
    }

    Response.CanID = MSG_ASM_UPDATE_TRAILER_ACK;
    bmSetMessageItem (&Response, State, 0, 1);
    Response.Length = 1;

    bmUpdateState = UPDATE_STATE_INITIALIZED;
    return (canWriteMessage (BASEMODULE_CHANNEL, &Response));
}


/*****************************************************************************/
/*!
 *  \brief   Calculates the CRC checksum over a data block in flash memory
 *
 *      Calculates the CRC32 over the supplied data block  in flash memory and
 *      returns the complement of the remainder. The polynomial used is:
 *
 *          x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
 *
 *  \oparam Crc = CRC32 value
 *  \iparam Handle = Handle of the flash memory
 *  \iparam Address = Address offset in flash memory
 *  \iparam Length = Size of data block (in bytes)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmFlashCrc (UInt32 *Crc, Handle_t Handle, UInt32 Address, UInt32 Length)
{
    UInt8 Data;
    Error_t Status;
    UInt32 Crc32 = CRC32_INITIAL_VALUE;
    UInt32 i, k;

    // Perform modulo-2 division for each byte
    for (i=0; i < Length; i++) {
        if ((Status = halStorageRead (Handle, Address + i, &Data, 1)) < NO_ERROR) {
            return Status;
        }
        Crc32 ^= Data << (32 - 8);

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
    *Crc = (Crc32 ^ MAX_UINT32);
    return NO_ERROR;
}


/*****************************************************************************/
/*!
 *  \brief   Returns the size of the boot loader
 *
 *      This message returns the size of the sector in flash memory that is
 *      reserved for the boot loader.
 *
 *  \return  Size of the boot loader
 *
 ****************************************************************************/

UInt32 bmGetBootLoaderSize(void)
{
    return (blSectorSize);
}


/*****************************************************************************/
/*!
 *  \brief   Initializes this module
 *
 *      This function initializes this module. It opens the flash memory and 
 *      registers the message handlers of the module.
 *
 *      This function is called once during startup!
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmInitUpdate (void)
{
    static bmCallbackEntry_t Commands[] = {
        { MSG_ASM_UPDATE_HEADER,    bmUpdateHeader },
        { MSG_ASM_UPDATE_DATA_0,    bmUpdateData0 },
        { MSG_ASM_UPDATE_DATA_1,    bmUpdateData1 },
        { MSG_ASM_UPDATE_TRAILER,   bmUpdateTrailer }
    };
    Error_t Status;

    if ((bmHandleFlash = halStorageOpen (HAL_STORAGE_FLASH, HAL_OPEN_RWE)) < NO_ERROR) {
        return (bmHandleFlash);
    }
    if ((Status = halStorageProtect (bmHandleFlash, FALSE)) < NO_ERROR) {
        return (Status);
    }

    // register CAN message to handle by this module
    Status = canRegisterMessages (BASEMODULE_MODULE_ID, Commands, ELEMENTS(Commands));
    return (Status);
}

//****************************************************************************/
