/****************************************************************************/
/*! \file fmRfidIso15693Link.c
 * 
 *  \brief Low level functions of the RFID ISO 15693 module.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 23.03.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *      This file holds the low level functions of the RFID ISO/IEC 15693
 *      function module. These methods build the messages sent to the RFID tag
 *      and evaluate the data received from it. These functions work with all
 *      RFID transponders that are specified regarding the ISO/IEC 15693
 *      standard.
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

#include "fmRfidIso15693Link.h"
#include "fmRfidIso15693.h"
#include "ModuleIDs.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define RFID15693_LINK_FLAGS_INVENTORY  0x26    //!< Inventory flags according to ISO/IEC 15693
#define RFID15693_LINK_FLAGS_REQUEST    0x22    //!< Request flags according to ISO/IEC 15693

#define RFID15693_LINK_CRC_POLYNOMIAL   0x8408  //!< CRC polynomial according to ISO/IEC 13239

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Private Variables 
//****************************************************************************/

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static UInt16 rfid15693LinkComputeCrc (UInt8 *Buffer, UInt8 Length);


/*****************************************************************************/
/*! 
 *  \brief   Writes a certain bit into a bit stream
 *
 *      This method sets a certain bit in a bit stream.
 * 
 *  \oparam  Buffer = The bit stream
 *  \iparam  Position = Position in the stream
 *  \iparam  Bit = The value of the bit
 *
 ****************************************************************************/

void rfid15693LinkWriteBit (UInt8* Buffer, UInt8 Position, UInt8 Bit)
{
    UInt8 Index = Position / 8;
    UInt8 SubIndex = Position % 8;
    
    if (Bit == 1) {
        Buffer[Index] |= BIT(SubIndex);
    }
    else {
        Buffer[Index] &= ~BIT(SubIndex);
    }
    
    return;
}


/*****************************************************************************/
/*! 
 *  \brief   Reads a certain bit from a bit stream
 *
 *      This method fetches a certain bit from a bit stream.
 * 
 *  \iparam  Buffer = The bit stream
 *  \iparam  Position = Position in the stream
 * 
 *  \return  The value of the bit
 *
 ****************************************************************************/

UInt8 rfid15693LinkReadBit (UInt8* Buffer, UInt8 Position)
{
    UInt8 Bit;
    UInt8 Index = Position / 8;
    UInt8 SubIndex = Position % 8;
    
    Bit = (Buffer[Index] >> SubIndex) & 0x01;
    
    return (Bit);
}


/*****************************************************************************/
/*! 
 *  \brief   CRC computation according to ISO/IEC 15693
 *
 *      This method computes the CRC check sum of an RFID message acorrding to
 *      ISO/IEC 15693
 * 
 *  \iparam  Buffer = RFID message
 *  \iparam  Length = Message length
 * 
 *  \return  CRC check sum
 *
 ****************************************************************************/

UInt16 rfid15693LinkComputeCrc (UInt8 *Buffer, UInt8 Length)
{
    UInt8 i;
    UInt8 j;
    UInt16 Crc = 0xFFFF;
    
    for (i = 0; i < Length; i++) {
        Crc = Crc ^ Buffer[i];
         for (j = 0; j < 8; j++) {
            if ((Crc & 1) == 1) {
                Crc = (Crc >> 1) ^ RFID15693_LINK_CRC_POLYNOMIAL;
            }
            else {
                Crc = (Crc >> 1);
            }
         }
    }
    return Crc;
}


/*****************************************************************************/
/*! 
 *  \brief   RFID 15693 inventory message
 *
 *      This method assembles an inventory request message according to
 *      ISO/IEC 15693. It also initializes the data values required by the
 *      transmission.
 * 
 *  \xparam  Stream = Structure containing the message data
 *
 ****************************************************************************/

void rfid15693LinkInventory (Rfid15693Stream_t *Stream)
{
    UInt16 Crc;

    Stream->TxCount = 0;
    Stream->TxLength = 40;
    Stream->RxCount = 0;
    Stream->RxLength = 96;

    Stream->TxBuffer[0] = RFID15693_LINK_FLAGS_INVENTORY;
    Stream->TxBuffer[1] = RFID15693_LINK_CMD_INVENTORY;
    Stream->TxBuffer[2] = 0;
    Crc = ~rfid15693LinkComputeCrc (Stream->TxBuffer, 3);
    Stream->TxBuffer[3] = Crc;
    Stream->TxBuffer[4] = Crc >> 8;
    
    Stream->PosCount = 1;
    Stream->TransmitState = LINK_TXSTATE_SOF;
    Stream->ReceiveState = LINK_RXSTATE_WAIT;
    Stream->SymbolShift = FALSE;
}


/*****************************************************************************/
/*! 
 *  \brief   RFID 15693 read single block message
 *
 *      This method assembles a read single block request message according to
 *      ISO/IEC 15693. It also initializes the data values required by the
 *      transmission.
 * 
 *  \xparam  Stream = Structure containing the message data
 *  \iparam  Uid = Unique ID of the transponder
 *  \iparam  BlockNumber = Number of the memory block
 *
 ****************************************************************************/

void rfid15693LinkReadSingleBlock (Rfid15693Stream_t *Stream, UInt64 Uid, UInt8 BlockNumber)
{
    UInt8 i;
    UInt16 Crc;

    Stream->TxCount = 0;
    Stream->TxLength = 104;
    Stream->RxCount = 0;
    Stream->RxLength = 56;

    Stream->TxBuffer[0] = RFID15693_LINK_FLAGS_REQUEST;
    Stream->TxBuffer[1] = RFID15693_LINK_CMD_SINGLE_READ;
    for (i = 0; i < 8; i++) {
        Stream->TxBuffer[2 + i] = Uid >> (8 * i);
    }
    Stream->TxBuffer[10] = BlockNumber;
    Crc = ~rfid15693LinkComputeCrc (Stream->TxBuffer, 11);
    Stream->TxBuffer[11] = Crc;
    Stream->TxBuffer[12] = Crc >> 8;
    
    Stream->PosCount = 1;
    Stream->TransmitState = LINK_TXSTATE_SOF;
    Stream->ReceiveState = LINK_RXSTATE_WAIT;
    Stream->SymbolShift = FALSE;
}


/*****************************************************************************/
/*! 
 *  \brief   RFID 15693 write single block message
 *
 *      This method assembles a write single block request message according
 *      to ISO/IEC 15693. It also initializes the data values required by the
 *      transmission.
 * 
 *  \xparam  Stream = Structure containing the message data
 *  \iparam  Uid = Unique ID of the transponder
 *  \iparam  BlockNumber = Number of the memory block
 *  \iparam  Data = Data written to the memory block
 *
 ****************************************************************************/

void rfid15693LinkWriteSingleBlock (Rfid15693Stream_t *Stream, UInt64 Uid, UInt8 BlockNumber, UInt32 Data)
{
    UInt8 i;
    UInt16 Crc;

    Stream->TxCount = 0;
    Stream->TxLength = 136;
    Stream->RxCount = 0;
    Stream->RxLength = 24;

    Stream->TxBuffer[0] = RFID15693_LINK_FLAGS_REQUEST;
    Stream->TxBuffer[1] = RFID15693_LINK_CMD_SINGLE_WRITE;
    for (i = 0; i < 8; i++) {
        Stream->TxBuffer[2 + i] = Uid >> (8 * i);
    }
    Stream->TxBuffer[10] = BlockNumber;
    for (i = 0; i < 4; i++) {
        Stream->TxBuffer[11 + i] = Data >> (8 * i);
    }
    Crc = ~rfid15693LinkComputeCrc (Stream->TxBuffer, 15);
    Stream->TxBuffer[15] = Crc;
    Stream->TxBuffer[16] = Crc >> 8;
    
    Stream->PosCount = 1;
    Stream->TransmitState = LINK_TXSTATE_SOF;
    Stream->ReceiveState = LINK_RXSTATE_WAIT;
    Stream->SymbolShift = FALSE;
}


/*****************************************************************************/
/*! 
 *  \brief   RFID 15693 lock block message
 *
 *      This method assembles a lock block request message according to
 *      ISO/IEC 15693. It also initializes the data values required by the
 *      transmission.
 * 
 *  \xparam  Stream = Structure containing the message data
 *  \iparam  Uid = Unique ID of the transponder
 *  \iparam  BlockNumber = Number of the memory block
 *
 ****************************************************************************/

void rfid15693LinkLockBlock (Rfid15693Stream_t *Stream, UInt64 Uid, UInt8 BlockNumber)
{
    UInt8 i;
    UInt16 Crc;

    Stream->TxCount = 0;
    Stream->TxLength = 104;
    Stream->RxCount = 0;
    Stream->RxLength = 24;

    Stream->TxBuffer[0] = RFID15693_LINK_FLAGS_REQUEST;
    Stream->TxBuffer[1] = RFID15693_LINK_CMD_BLOCK_LOCK;
    for (i = 0; i < 8; i++) {
        Stream->TxBuffer[2 + i] = Uid >> (8 * i);
    }
    Stream->TxBuffer[10] = BlockNumber;
    Crc = ~rfid15693LinkComputeCrc (Stream->TxBuffer, 11);
    Stream->TxBuffer[11] = Crc;
    Stream->TxBuffer[12] = Crc >> 8;
    
    Stream->PosCount = 1;
    Stream->TransmitState = LINK_TXSTATE_SOF;
    Stream->ReceiveState = LINK_RXSTATE_WAIT;
    Stream->SymbolShift = FALSE;
}


/*****************************************************************************/
/*! 
 *  \brief   Checks the validity of a received message
 *
 *      This method computes the CRC check sum of an RFID message according to
 *      ISO/IEC 15693 and compares it to the checksum that was attached to a
 *      received message. It also verifies the length if the message and if
 *      the error flag was set according to ISO/IEC 15693
 * 
 *  \iparam  Stream = RFID message
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t rfid15693LinkCheckMessage (Rfid15693Stream_t *Stream)
{
    UInt16 ActualCrc;
    UInt16 DesiredCrc;
    
    // Adapt the length to the error response format
    if (rfid15693LinkReadBit (Stream->RxBuffer, 0) == 1) {
        Stream->RxLength = 32;
    }
    // Check the message length
    if (Stream->RxLength != Stream->RxCount - 2) {
        return (E_RFID15693_INVALID_RXLENGTH);
    }
    
    // CRC check
    ActualCrc = Stream->RxBuffer[Stream->RxLength / 8 - 2];
    ActualCrc |= (UInt16) Stream->RxBuffer[Stream->RxLength / 8 - 1] << 8;
    ActualCrc = ~ActualCrc;
    
    DesiredCrc = rfid15693LinkComputeCrc (Stream->RxBuffer, Stream->RxLength / 8 - 2);
    
    if (ActualCrc != DesiredCrc) {
        return (E_RFID15693_CHECKSUM_MISMATCH);
    }
    
    // Check the ISO/IEC 15693 error flag
    if (rfid15693LinkReadBit (Stream->RxBuffer, 0) == 1) {
        return (E_RFID15693_RESPONSE_ERRORFLAG);
    }
    
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Extracts the unique ID from a received message
 *
 *      This method extracts and returns the unique ID from a received
 *      inventory message.
 * 
 *  \iparam  Stream = RFID message
 * 
 *  \return  Unique ID
 *
 ****************************************************************************/

UInt64 rfid15693GetUniqueId (Rfid15693Stream_t *Stream)
{
    UInt64 UniqueID;
    
    UniqueID = Stream->RxBuffer[2];
    UniqueID |= Stream->RxBuffer[3] << 8;
    UniqueID |= Stream->RxBuffer[4] << 16;
    UniqueID |= (UInt32) Stream->RxBuffer[5] << 24;
    UniqueID |= (UInt64) Stream->RxBuffer[6] << 32;
    UniqueID |= (UInt64) Stream->RxBuffer[7] << 40;
    UniqueID |= (UInt64) Stream->RxBuffer[8] << 48;
    UniqueID |= (UInt64) Stream->RxBuffer[9] << 56;
    
    return (UniqueID);
}


/*****************************************************************************/
/*! 
 *  \brief   Extracts the data block from a read response message
 *
 *      This method extracts and returns the 32 bit data block from a received
 *      read message.
 * 
 *  \iparam  Stream = RFID message
 * 
 *  \return  Unique ID
 *
 ****************************************************************************/

UInt32 rfid15693GetDataBlock (Rfid15693Stream_t *Stream)
{
    UInt32 DataBlock;
    
    DataBlock = Stream->RxBuffer[1];
    DataBlock |= Stream->RxBuffer[2] << 8;
    DataBlock |= Stream->RxBuffer[3] << 16;
    DataBlock |= Stream->RxBuffer[4] << 24;
    
    return (DataBlock);
}

//****************************************************************************/
