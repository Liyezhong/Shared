/****************************************************************************/
/*! \file blMain.c
 *
 *  \brief Bootloader main functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 12.10.2011
 *  $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *       This module is the boot loader of the microcontroller. At every start
 *       of the controller the boot loader checks, if a loadable firmware
 *       image is present in the flash memory of the microcontroller. If this
 *       is the case, a CRC check is performed on the firmware image and the
 *       actual firmware is started. The bootloader is also able to write a
 *       new image into flash memory. The update process can be started, when
 *       no firmware image is stored in the memory or when the boot loader is
 *       explicitly started from the base module for this purpose. The update
 *       is controlled over CAN bus.
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

#include "blAssembler.h"
#include "blMain.h"

#include "bmCan.h"
#include "bmCommon.h"
#include "bmError.h"
#include "bmTime.h"
#include "bmUpdate.h"
#include "bmUtilities.h"
#include "halCan.h"
#include "halClocks.h"
#include "halDeviceID.h"
#include "halFlash.h"
#include "halMain.h"
#include "halPorts.h"
#include "halStorage.h"
#include "ModuleIDs.h"


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Update state definitions
typedef enum {
    UPDATE_STATE_UNINITIALIZED, //!< Update mode uninitialized
    UPDATE_STATE_INITIALIZED,   //!< Update mode initialized
    UPDATE_STATE_STARTED        //!< Update in progress
} blUpdateState_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

static Handle_t blHandleLed[3]; //!< Handle for the three board LEDs
static Handle_t blHandleFlash;  //!< Handle for the flash memory
static blUpdateState_t blUpdateState = UPDATE_STATE_UNINITIALIZED;  //!< Update State
static UInt32 blImageLength;    //!< Length of the firmware image
static UInt32 blImageOffset;    //!< Offset to start within the image flash memory area
static UInt32 blImageCount;     //!< Amount of data already programmed to flash memory
static UInt8 blFirmwareState;   //!< State of the firmware image (valid (0), invalid (1), not found (2))
static UInt32 blSectorStart;    //!< Beginning of the firmware image area in flash
static UInt32 blSectorSize;     //!< The size of the flash sector used by the firmware

static UInt32 blInfoLength;     //!< Length of the firmware image
static UInt32 blInfoOffset;     //!< Offset to start within the image flash memory area
static UInt32 blInfoCount;      //!< Amount of data already programmed to flash meory

static UInt32 blInfoStartOffset;    //!< Beginning of the info block area in flash

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t blInitializeHal (void);
static Error_t blInitBaseModule (void);
static Error_t blCheckFirmware (void);
static Error_t blUpdateModeRequest (CanMessage_t *Message);
static Error_t blUpdateHeader (CanMessage_t *Message);
static Error_t blUpdateData (CanMessage_t *Message, UInt8 Count);
static Error_t blUpdateTrailer (CanMessage_t *Message);
static Error_t blUpdateInfoInit (CanMessage_t *Message);
static Error_t blUpdateInfo (CanMessage_t *Message);
static Error_t blReadCanCommands (void);
static Error_t blSendUpdateRequired(void);
static void blTaskFunction (void);

typedef void (*blFunction)(void);


/*****************************************************************************/
/*!
 *  \brief   Initialize HAL
 *
 *      This function initializes the parts of the Hardware Abstraction Layer
 *      (HAL) that are required by the boot loader. These parts are the
 *      clocks, interrupts, system tick timer, digital I/O ports and the flash
 *      memory. The method also creates the handles required to access the
 *      three board LEDs and the flash memory.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t blInitializeHal (void)
{
    Error_t Status;

    halSetInitState (INIT_IN_PROGRESS);

    if ((Status = halClocksInit ()) < NO_ERROR) {
        return (Status);
    }
    if ((Status = halInterruptInit ()) < NO_ERROR) {
        return (Status);
    }
    if ((Status = halSysTickInit ()) < NO_ERROR) {
        return (Status);
    }
    if ((Status = halPortInit ()) < NO_ERROR) {
        return (Status);
    }
    if ((Status = halCanInit ()) < NO_ERROR) {
        return (Status);
    }
    if ((Status = halStorageInit ()) < NO_ERROR) {
        return (Status);
    }

    halSetInitState (INIT_SUCCEEDED);

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize Base Module
 *
 *      This function initializes the parts of the Base Module that are
 *      required by the boot loader. These parts are the utility functions and
 *      the CAN network layer.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t blInitBaseModule (void)
{
    Error_t Status;

    if ((Status = bmInitUtilities ()) < NO_ERROR) {
        return (Status);
    }
    if ((Status = canInitializeLayer ()) < NO_ERROR) {
        return (Status);
    }

    if ((blHandleLed[0] = halPortOpen (HAL_STATUS_LED1, HAL_OPEN_WRITE)) < NO_ERROR) {
        return (blHandleLed[0]);
    }
    if ((blHandleLed[1] = halPortOpen (HAL_STATUS_LED2, HAL_OPEN_WRITE)) < NO_ERROR) {
        return (blHandleLed[1]);
    }
    if ((blHandleLed[2] = halPortOpen (HAL_STATUS_LED3, HAL_OPEN_WRITE)) < NO_ERROR) {
        return (blHandleLed[2]);
    }
    if ((blHandleFlash = halStorageOpen (HAL_STORAGE_FLASH, HAL_OPEN_RWE)) < NO_ERROR) {
        return (blHandleFlash);
    }

    // The first double word contains the size and the second one the CRC of the firmware
    blSectorStart = bmGetBootLoaderSize() + 2 * sizeof(UInt32);
    blSectorSize = halStorageSize (blHandleFlash) - 3 * halFlashBlockSize () - blSectorStart;

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Checks if a valid firmware is present
 *
 *      This method checks if a valid firmware image can be found in the flash
 *      memory of the microcontroller. It returns the result of this check. If
 *      a firmware image is present, its size will be checked. The integrity
 *      of the firmware code is verified using the CRC checksum stored in
 *      flash.
 *
 *  \oparam  Result = Firmware is valid (0), invalid (1), not found (2)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t blCheckFirmware (void)
{
    Error_t Status;
    UInt32 ActualCrc;
    UInt32 DesiredCrc;
    UInt32 ImageLength;

    blFirmwareState = 0;

    // The firmware image length of CRC value cannot be set
    // if the binary file is programmed into flash memory with emulator.
    // Set DEBUG preprocessor to skip firmware check.
#ifndef DEBUG
    if ((Status = halStorageRead (blHandleFlash, blSectorStart - 2 * sizeof(UInt32), &ImageLength, sizeof(UInt32))) <
            NO_ERROR) {
        return (Status);
    }
    if ((Status = halStorageRead (blHandleFlash, blSectorStart - sizeof(UInt32), &ActualCrc, sizeof(UInt32))) <
            NO_ERROR) {
        return (Status);
    }

    if (ImageLength == 0 || ImageLength == 0xFFFFFFFF || ActualCrc == 0 || ActualCrc == 0xFFFFFFFF) {
        blFirmwareState = 2;
    }
    else if (blSectorSize < ImageLength) {
        blFirmwareState = 1;
    }
    else {
        if ((Status = bmFlashCrc (&DesiredCrc, blHandleFlash, blSectorStart, ImageLength)) < NO_ERROR) {
            blFirmwareState = 1;
            return (Status);
        }
        if (ActualCrc != DesiredCrc) {
            blFirmwareState = 1;
        }
    }
#endif

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Handles update mode request messages
 *
 *      This message is received by the boot loader, when the master request
 *      the start of the firmware update procedure. Before that the boot
 *      loader frequently sends update mode required messages to the master.
 *      The incoming message is only handled by this function, when the boot
 *      loader is in the uninitialized state. If this is the case, a update
 *      mode acknowledge message is returned to the master. Then the state of
 *      the boot loader is changed initialized.
 *
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t blUpdateModeRequest (CanMessage_t *Message)
{
    UInt8 Mode;
    Error_t Status;
    CanMessage_t Response;

    if (Message->Length < 1) {
        return (E_MISSING_PARAMETERS);
    }

    Mode = bmGetMessageItem (Message, 0, 1);
    if (Mode == 0 && blFirmwareState != 0) {
        return (NO_ERROR);
    }

    Response.CanID = MSG_ASM_UPDATE_MODE_ACK;
    Response.Length = 0;
    Status = canWriteMessage (BASEMODULE_CHANNEL, &Response);
    if (Status < NO_ERROR) {
        return (Status);
    }

    blUpdateState = UPDATE_STATE_INITIALIZED;
    if (Mode == 0) {
        // Adding 4, because the first 4 bytes contain the stack pointer
        blFunction blJumpToApplication = (blFunction)(*(UInt32*) (halStorageBase(blHandleFlash) + blSectorStart + 4));

        // Start the firmware
        if ((Status = halInterruptDeinit()) < NO_ERROR) {
            return (Status);
        }
        blSetMsp( *( (UInt32*) (halStorageBase(blHandleFlash) + blSectorStart) ) );
        blJumpToApplication();
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Handles update header messages
 *
 *      This function is called, when a update header message is received from
 *      the master. The message contains two parameters, the length of the
 *      firmware image and the starting offset within the target area of the
 *      flash memory. The message is only processed, when the boot loader is
 *      at least in the initialized state. The function also erases the
 *      required area in the flash memory. The function also returns an
 *      acknowledgement message to the master. This message indicates, if the
 *      firmware update is possible or if the firmware can not fit into flash.
 *      It also changes the boot loader state to started.
 *
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t blUpdateHeader (CanMessage_t *Message)
{
    Error_t Status;
    UInt8 State = 0;
    CanMessage_t Response;

    if (Message->Length < 8) {
        return (E_MISSING_PARAMETERS);
    }
    if (blUpdateState == UPDATE_STATE_UNINITIALIZED) {
        return (NO_ERROR);
    }

    blImageLength = bmGetMessageItem (Message, 0, 4);
    blImageOffset = bmGetMessageItem (Message, 4, 4);
    blImageCount = 0;

    if (blSectorSize < blImageLength) {
        State = 1;
    }
    else if (blSectorSize < blImageOffset + blImageLength) {
        State = 2;
    }

    if (State == 0) {
        if (blImageOffset == 0) {
            if ((Status = halStorageErase(blHandleFlash, blSectorStart - 2 * sizeof(UInt32),
                    blImageLength + 2 * sizeof(UInt32))) < NO_ERROR) {
                return (Status);
            }
            if ((Status = halStorageWrite (blHandleFlash, blSectorStart - 2 * sizeof(UInt32),
                    &blImageLength, sizeof(UInt32))) < NO_ERROR) {
                return (Status);
            }
        }

        if ((Status = halPortWrite(blHandleLed[1], 1)) < NO_ERROR) {
            return (Status);
        }
        blUpdateState = UPDATE_STATE_STARTED;
    }

    Response.CanID = MSG_ASM_UPDATE_HEADER_ACK;
    bmSetMessageItem (&Response, State, 0, 1);
    Response.Length = 1;

    return (canWriteMessage (BASEMODULE_CHANNEL, &Response));
}


/*****************************************************************************/
/*!
 *  \brief   Handles update data messages
 *
 *      This function is called, when a update data message is received from
 *      the master. It basically writes the message data into flash memory.
 *      The message is only processed, when the boot loader is in the started
 *      state. The data messages implement a simple flow control mechanism to
 *      recognize double CAN frames. The function also returns an
 *      acknowledgement message to the master. This message indicates, if the
 *      data has been successfully written.
 *
 *  \iparam  Message = Received CAN message
 *  \iparam  Count = Flow control count (0 or 1)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t blUpdateData (CanMessage_t *Message, UInt8 Count)
{
    UInt32 Length;
    Error_t Status;
    CanMessage_t Response;

    if (blUpdateState != UPDATE_STATE_STARTED) {
        return (NO_ERROR);
    }
    if (blImageCount + Message->Length > blImageLength) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if ((blImageCount / 8) % 2 != Count) {
        return (NO_ERROR);
    }

    if (Message->Length % 2 == 1) {
        Length = Message->Length + 1;
    }
    else {
        Length = Message->Length;
    }
    if ((Status = halStorageWrite (blHandleFlash, blSectorStart + blImageOffset + blImageCount,
            Message->Data, Length)) < NO_ERROR) {
        return (Status);
    }
    blImageCount += Message->Length;

    if (Count == 0) {
        Response.CanID = MSG_ASM_UPDATE_ACK_0;
    }
    else {
        Response.CanID = MSG_ASM_UPDATE_ACK_1;
    }
    Response.Length = 0;
    return (canWriteMessage (BASEMODULE_CHANNEL, &Response));
}


/*****************************************************************************/
/*!
 *  \brief   Handles update trailer messages
 *
 *      This function is called, when a update trailer message is received
 *      from the master. The message contains the CRC checksum of the firmware
 *      image. It is only processed, when the boot loader is in the started
 *      state. The function calculates the CRC checksum of the data written to
 *      flash memory. It also returns an acknowledgement message to the
 *      master. This message indicates, if the firmware update has been
 *      successful or if the calculated checksum deviates from the intended
 *      checksum. It also changes the boot loader state to uninitialized.
 *
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t blUpdateTrailer (CanMessage_t *Message)
{
    Error_t Status;
    UInt8 State = 0;
    CanMessage_t Response;

    if (Message->Length < 4) {
        return (E_MISSING_PARAMETERS);
    }
    if (blUpdateState != UPDATE_STATE_STARTED) {
        return (NO_ERROR);
    }

    if (blImageCount != blImageLength) {
        State = 2;
        blFirmwareState = 1;
    }
    else {
        UInt32 ActualCrc;
        UInt32 DesiredCrc = bmGetMessageItem (Message, 0, 4);

        if ((Status = bmFlashCrc (&ActualCrc, blHandleFlash, blSectorStart, blImageOffset + blImageLength)) <
                NO_ERROR) {
            return (Status);
        }
        if (ActualCrc != DesiredCrc) {
            State = 1;
            blFirmwareState = 1;
        }
        else {
            if ((Status = halStorageWrite (blHandleFlash, blSectorStart - sizeof(UInt32), &ActualCrc, sizeof(UInt32))) <
                    NO_ERROR) {
                return (Status);
            }
            blFirmwareState = 0;
        }
    }

    if ((Status = halPortWrite(blHandleLed[1], 0)) < NO_ERROR) {
        return (Status);
    }

    Response.CanID = MSG_ASM_UPDATE_TRAILER_ACK;
    bmSetMessageItem (&Response, State, 0, 1);
    Response.Length = 1;

    blUpdateState = UPDATE_STATE_UNINITIALIZED;
    return (canWriteMessage (BASEMODULE_CHANNEL, &Response));
}


/*****************************************************************************/
/*!
 *  \brief   Handles update info initialization messages
 *
 *      This function is called, when a update info initialization message is
 *      received from the master. The message contains three parameters,
 *      the length of the info block, the starting offset within the target
 *      area of the flash memory and the info block type. The start address of
 *      info block in flash memory is then set according to the info type.
 *      The required area in the flash memory is also erased.
 *      The message is only processed, when the boot loader is at least in the
 *      initialized state. The function also returns an acknowledgement message
 *      to the master. This message indicates, if the firmware update is
 *      possible or if the firmware can not fit into flash.
 *      It also changes the boot loader state to started.
 *
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t blUpdateInfoInit (CanMessage_t *Message)
{
    UInt8 Type;
    Error_t Status;
    UInt8 State = 0;
    CanMessage_t Response;

    if (Message->Length < 8) {
        return (E_MISSING_PARAMETERS);
    }
    if (blUpdateState == UPDATE_STATE_UNINITIALIZED) {
        return (NO_ERROR);
    }

    blInfoCount  = 0;
    blInfoLength = bmGetMessageItem (Message, 0, 4);
    blInfoOffset = bmGetMessageItem (Message, 4, 3);
    Type   = bmGetMessageItem (Message, 7, 1);

    switch (Type) {
        case 0:
            blInfoStartOffset = halStorageSize(blHandleFlash) - 3 * halFlashBlockSize();
            break;
        case 1:
            blInfoStartOffset = halStorageSize(blHandleFlash) - 2 * halFlashBlockSize();
            break;
        case 2:
            blInfoStartOffset = halStorageSize(blHandleFlash) - 1 * halFlashBlockSize();
            break;
    }

    // Each info block should be limited into ONE page of flash memory
    if (blInfoLength > halFlashBlockSize ()) {
        State = 1;
    }
    else if (blInfoOffset + blInfoLength > halFlashBlockSize ()) {
        State = 2;
    }

    if (State == 0) {
        if (blInfoOffset == 0) {
            if ((Status = halStorageErase(blHandleFlash, blInfoStartOffset, blInfoLength)) < NO_ERROR) {
                return (Status);
            }
        }

        if ((Status = halPortWrite(blHandleLed[1], 1)) < NO_ERROR) {
            return (Status);
        }
        blUpdateState = UPDATE_STATE_STARTED;
    }

    Response.CanID = MSG_ASM_UPDATE_INFO_INIT_ACK;
    bmSetMessageItem (&Response, State, 0, 1);
    Response.Length = 1;

    return (canWriteMessage (BASEMODULE_CHANNEL, &Response));
}


/*****************************************************************************/
/*!
 *  \brief   Handles update info messages
 *
 *      This function is called, when a update info message is received from
 *      the master. It basically writes the message data into flash memory.
 *      The function also calculates the CRC checksum of the data written to
 *      flash memory after the last frame of info block is stored. The message
 *      is only processed, when the boot loader is in the started state. It
 *      also returns an acknowledgement message to the master. This message
 *      indicates, if the info update has been successful or if the
 *      calculated checksum deviates from the intended checksum.
 *
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t blUpdateInfo (CanMessage_t *Message)
{
    Error_t Status;
    UInt32 ActualCrc;
    UInt32 DesiredCrc;
    CanMessage_t Response;

    if (blUpdateState != UPDATE_STATE_STARTED) {
        return (NO_ERROR);
    }
    if (blInfoCount + Message->Length > blInfoLength) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }

    if ((Status = halStorageWrite (blHandleFlash, blInfoStartOffset + blInfoOffset + blInfoCount,
            Message->Data, Message->Length)) < NO_ERROR) {
        return (Status);
    }
    blInfoCount += Message->Length;

    // Check if all info has been received
    if (blInfoCount == blInfoLength) {
        if ((Status = halStorageRead (blHandleFlash, blInfoStartOffset + blInfoLength - 4, &ActualCrc, 4)) <
                NO_ERROR) {
            return (Status);
        }

        if ((Status = bmFlashCrc (&DesiredCrc, blHandleFlash, blInfoStartOffset, blInfoLength - 4)) < NO_ERROR) {
            return (Status);
        }

        Response.CanID = MSG_ASM_UPDATE_INFO_ACK;
        Response.Length = 1;

        if (ActualCrc == DesiredCrc) {
            bmSetMessageItem (&Response, 0, 0, 1);
        }
        else {
            bmSetMessageItem (&Response, 1, 0, 1);
        }

        blUpdateState = UPDATE_STATE_UNINITIALIZED;

        return (canWriteMessage (BASEMODULE_CHANNEL, &Response));
    }
    else {
        Response.CanID = MSG_ASM_UPDATE_INFO_ACK;
        Response.Length = 0;

        return (canWriteMessage (BASEMODULE_CHANNEL, &Response));
    }
}


/*****************************************************************************/
/*!
 *  \brief   Reads CAN messages and calls their handler
 *
 *      This function is called periodically. It reads CAN messages from the
 *      receive queue and checks their command ID. Then it starts the message
 *      handler that corresponds to the ID.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t blReadCanCommands (void)
{
    Error_t Status;
    UInt16 Channel;
    UInt16 Command;
    CanMessage_t Message;

    if ((Status = canReadMessage (&Channel, &Message)) < NO_ERROR) {
        return (Status);
    }
    else if (Status == 1 && bmGetNodeAddress() == (Message.CanID & CANiD_MASK_ADDRESS) && Channel == BASEMODULE_CHANNEL) {
        Command = Message.CanID >> CANiD_SHIFT_CMDCODE;
        if (Command == (MSG_ASM_UPDATE_MODE_REQUEST >> CANiD_SHIFT_CMDCODE)) {
            Status = blUpdateModeRequest(&Message);
        }
        else if (Command == (MSG_ASM_UPDATE_HEADER >> CANiD_SHIFT_CMDCODE)) {
            Status = blUpdateHeader(&Message);
        }
        else if (Command == (MSG_ASM_UPDATE_DATA_0 >> CANiD_SHIFT_CMDCODE)) {
            Status = blUpdateData(&Message, 0);
        }
        else if (Command == (MSG_ASM_UPDATE_DATA_1 >> CANiD_SHIFT_CMDCODE)) {
            Status = blUpdateData(&Message, 1);
        }
        else if (Command == (MSG_ASM_UPDATE_TRAILER >> CANiD_SHIFT_CMDCODE)) {
            Status = blUpdateTrailer(&Message);
        }
        else if (Command == (MSG_ASM_UPDATE_INFO_INIT >> CANiD_SHIFT_CMDCODE)) {
            Status = blUpdateInfoInit(&Message);
        }
        else if (Command == (MSG_ASM_UPDATE_INFO >> CANiD_SHIFT_CMDCODE)) {
            Status = blUpdateInfo(&Message);
        }
        return (Status);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Sends update required messages
 *
 *      This function is called periodically. When the boot loader is in the
 *      uninitialized state it sends a update required message to the master
 *      on every single second.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t blSendUpdateRequired (void)
{
    static UInt32 StartTime = 0;
    Error_t Status;
    CanMessage_t Message;

    if (blUpdateState == UPDATE_STATE_UNINITIALIZED && bmTimeExpired (StartTime) > 1000) {
        bmBoardInfoBlock_t *BoardInfos = bmGetBoardInfoBlock();
        bmBootInfoBlock_t *LoaderInfos = bmGetLoaderInfoBlock();

        Message.CanID = MSG_ASM_UPDATE_REQUIRED;
        bmSetMessageItem (&Message, blFirmwareState, 0, 1);
        bmSetMessageItem (&Message, BoardInfos->VersionMajor, 1, 1);
        bmSetMessageItem (&Message, BoardInfos->VersionMinor, 2, 1);
        bmSetMessageItem (&Message, LoaderInfos->VersionMajor, 3, 1);
        bmSetMessageItem (&Message, LoaderInfos->VersionMinor, 4, 1);
        Message.Length = 5;

        StartTime = bmGetTime();

        if ((Status = canWriteMessage (BASEMODULE_CHANNEL, &Message)) < NO_ERROR) {
            return (Status);
        }
    }

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Task function
 *
 *      This function is called periodically. It processes received CAN
 *      messages and sends update required message in the uninitialized state.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static void blTaskFunction (void)
{
    Error_t Status;

    if ((Status = blReadCanCommands ()) < NO_ERROR) {
        blUpdateState = UPDATE_STATE_UNINITIALIZED;
        bmTransmitEvent (BASEMODULE_CHANNEL, Status, 0);
    }
    if ((Status = blSendUpdateRequired ()) < NO_ERROR) {
        blUpdateState = UPDATE_STATE_UNINITIALIZED;
        bmTransmitEvent (BASEMODULE_CHANNEL, Status, 0);
    }
}


/*****************************************************************************/
/*!
 *  \brief   Start function
 *
 *      This is the main start function of the boot loader. It starts the
 *      initialization of some parts of the HAL and the Base Module. It also
 *      resets the board LEDs and unprotects the flash memory. After that,
 *      it calls the task function in a loop. This function should never
 *      returned once the initialization is completed.
 *
 ****************************************************************************/

void blStartBootloader (void)
{
    Error_t Status;

    if ((Status = blInitializeHal ()) < NO_ERROR) {
        return;
    }
    if ((Status = blInitBaseModule ()) < NO_ERROR) {
        return;
    }

    if ((Status = halStorageProtect (blHandleFlash, FALSE)) < NO_ERROR) {
        bmTransmitEvent (BASEMODULE_CHANNEL, Status, 0);
    }
    if ((Status = halPortWrite(blHandleLed[0], 1)) < NO_ERROR) {
        bmTransmitEvent (BASEMODULE_CHANNEL, Status, 0);
    }
    if ((Status = halPortWrite(blHandleLed[1], 0)) < NO_ERROR) {
        bmTransmitEvent (BASEMODULE_CHANNEL, Status, 0);
    }
    if ((Status = halPortWrite(blHandleLed[2], 0)) < NO_ERROR) {
        bmTransmitEvent (BASEMODULE_CHANNEL, Status, 0);
    }
    if ((Status = blCheckFirmware ()) < NO_ERROR) {
        bmTransmitEvent (BASEMODULE_CHANNEL, Status, 0);
    }

    for (;;) {
        blTaskFunction ();
    }
}

//****************************************************************************/
