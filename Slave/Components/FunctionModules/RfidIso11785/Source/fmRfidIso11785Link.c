/****************************************************************************/
/*! \file fmRfidIso11785Link.c
 *
 *  \brief Low level functions of the RFID ISO 11785 module.
 *
 *   $Version: $ 0.4
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *      This file holds the low level functions of the RFID  ISO 11785
 *      function module. These methods build the messages sent to the RFID tag
 *      and evaluate the data received from it. All these functions only work
 *      with the "EM Microelectronic EM4305" RFID tag IC.
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

#include "ModuleIDs.h"
#include "fmRfidIso11785Link.h"
#include "fmRfidIso11785.h"
#include "halLayer.h"
#include "bmError.h"
#include "bmTime.h"

//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define TIMEOUT 33  //!< Timeout of an RFID transaction in milliseconds

#define LENGTH_CMD      5   //!< Length of the command field (Parity bits included)
#define LENGTH_ADDR     7   //!< Length of the address field (Parity bits included)
#define LENGTH_DATA     45  //!< Length of the data field (Parity bits included)
#define LENGTH_PREAMBLE 8   //!< Length of the preamble

#define PATTERN_PREAMBLE 0x50   //!< Response pattern in case of success
#define PATTERN_ERROR    0x80   //!< Response pattern in case of failure

static const UInt8 CmdLogin = 0x18; //!< Login command (Parity bits included)
static const UInt8 CmdRead = 0x12;  //!< Read command (Parity bits included)
static const UInt8 CmdWrite = 0x14; //!< Write command (Parity bits included)

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//****************************************************************************/
// Private Variables
//****************************************************************************/

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static void  rfid11785LinkCopyBits (UInt8* Dest, UInt8 DstPos, const UInt8* Source, UInt8 SrcPos, UInt8 Length);
static UInt8 rfid11785LinkComputeParity (const UInt8* Stream, UInt8 Position, UInt8 Stride, UInt8 Length);
static void  rfid11785LinkCopyDataField (UInt8* Stream, UInt8 Position, UInt32 DataWord);

/*****************************************************************************/
/*!
 *  \brief   Writes a certain bit into a bit stream
 *
 *      This method sets a certain bit in a bit stream.
 *
 *  \oparam  Stream = The bit stream
 *  \iparam  Position = Position in the stream
 *  \iparam  Bit = The value of the bit
 *
 ****************************************************************************/

void rfid11785LinkWriteBit (UInt8* Stream, UInt8 Position, UInt8 Bit)
{
    UInt8 Index = Position / 8;
    UInt8 SubIndex = Position % 8;

    if (Bit == 1) {
        Stream[Index] |= (1U << SubIndex);
    }
    else {
        Stream[Index] &= ~(1U << SubIndex);
    }

    return;
}


/*****************************************************************************/
/*!
 *  \brief   Reads a certain bit from a bit stream
 *
 *      This method fetches a certain bit from a bit stream.
 *
 *  \iparam  Stream = The bit stream
 *  \iparam  Position = Position in the stream
 *
 *  \return  The value of the bit
 *
 ****************************************************************************/

UInt8 rfid11785LinkReadBit (const UInt8* Stream, UInt8 Position)
{
    UInt8 Bit;
    UInt8 Index = Position / 8;
    UInt8 SubIndex = Position % 8;

    Bit = (Stream[Index] >> SubIndex) & 0x01;

    return (Bit);
}


/*****************************************************************************/
/*!
 *  \brief   Copies bits between bit streams
 *
 *      This method reads a number of bits from the source bit stream and
 *      copies them over to a certain position in the destination stream.
 *
 *  \oparam  Dest = Destination bit stream
 *  \oparam  DstPos = Starting position in the destination stream
 *  \iparam  Source = Source bit stream
 *  \iparam  SrcPos = Starting position in the source stream
 *  \iparam  Length = Number of bits to be copied
 *
 ****************************************************************************/

static void rfid11785LinkCopyBits (UInt8* Dest, UInt8 DstPos, const UInt8* Source, UInt8 SrcPos, UInt8 Length)
{
    UInt8 i;
    UInt8 Bit;

    for (i = 0; i < Length; i++) {
        Bit = rfid11785LinkReadBit (Source, SrcPos + i);
        rfid11785LinkWriteBit (Dest, DstPos + i, Bit);
    }

    return;
}


/*****************************************************************************/
/*!
 *  \brief   Computes an even parity bit
 *
 *  \riskid  SWRA 5.5.1: Unreliable and wrong recognition of racks
 *
 *      This method computes a parity bit from the information in a bit stream
 *      The step size (stride) can be set as well as the number of bits used
 *      for computation.
 *
 *  \iparam  Stream = The bit stream
 *  \iparam  Position = Position in the stream
 *  \iparam  Stride = Step size from bit to bit
 *  \iparam  Length = Number of bits
 *
 *  \return  Computed parity bit
 *
 ****************************************************************************/

static UInt8 rfid11785LinkComputeParity (const UInt8* Stream, UInt8 Position, UInt8 Stride, UInt8 Length)
{
    UInt8 i;
    UInt8 Parity = 0;

    for (i = 0; i < Length; i++) {
        Parity ^= rfid11785LinkReadBit(Stream, Position + i * Stride);
    }

    return Parity;
}


/*****************************************************************************/
/*!
 *  \brief   Copies a data word to into a bit stream
 *
 *      This method copies a 32 bit data word into a bit stream in the format
 *      of the RFID transponder's data packet. It also sets the required
 *      parity bits.
 *
 *  \oparam  Stream = The bit stream
 *  \iparam  Position = Starting position in the bit stream
 *  \iparam  DataWord = 32 bit source data word
 *
 *  \return  Computed parity bit
 *
 ****************************************************************************/

static void rfid11785LinkCopyDataField (UInt8* Stream, UInt8 Position, UInt32 DataWord)
{
    UInt8 i;
    UInt8 Parity;
    UInt8 Buffer[4];

    for (i = 0; i < 4; i++) {
        Buffer[i] = DataWord >> (8 * i);
        // Copy the bytes of the password
        rfid11785LinkCopyBits (Stream, Position + 9 * i, Buffer, 8 * i, 8);
        // Compute and set byte parity
        Parity = rfid11785LinkComputeParity(Buffer, 8 * i, 1, 8);
        rfid11785LinkWriteBit (Stream, Position + 9 * (i + 1) - 1, Parity);
    }
    // Compute and set column parity
    for (i = 0; i < 8; i++) {
        Parity = rfid11785LinkComputeParity((UInt8*) Buffer, i, 8, 4);
        rfid11785LinkWriteBit (Stream, Position + 36 + i, Parity);
    }
    // The last bit of the data packet
    rfid11785LinkWriteBit (Stream, Position + 44, 0);
}


/*****************************************************************************/
/*!
 *  \brief   Starts an RFID login transmission
 *
 *      This method builds the bit stream of an RFID login message. The
 *      message will transmit a 32 bit password to the RFID tranceiver and
 *      enables the access to password protected registers. This method does
 *      not perform the transaction itself.
 *
 *  \iparam Password = Password sent to the RFID tag
 *  \xparam DataStream = This structure is filled with the bit stream
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

void rfid11785LinkStartLogin (UInt32 Password, Rfid11785Stream_t* DataStream)
{
    DataStream->ReadCommand = FALSE;
    DataStream->CycleCount = 0;
    DataStream->TxCount = 0;
    DataStream->RxCount = 1;
    DataStream->IrqError = NO_ERROR;

    // Reset the preamble
    DataStream->RxBuffer[0] = 0;

    DataStream->TxLength = LENGTH_CMD + LENGTH_DATA;
    DataStream->RxLength = LENGTH_PREAMBLE;

    // Copy the login command bits
    rfid11785LinkCopyBits (DataStream->TxBuffer, 0, &CmdLogin, 0, LENGTH_CMD);
    // Copy the password
    rfid11785LinkCopyDataField (DataStream->TxBuffer, LENGTH_CMD, Password);
}


/*****************************************************************************/
/*!
 *  \brief   Starts an RFID write function
 *
 *      This method builds the bit stream of an RFID write message. The
 *      message will write a 32 bit register of the RFID tranceiver. It does
 *      not perform the transaction itself.
 *
 *  \iparam  Address = Destination Address
 *  \iparam  Data = Data written to the register
 *  \xparam DataStream = This structure is filled with the bit stream
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

void rfid11785LinkStartWrite (UInt8 Address, UInt32 Data, Rfid11785Stream_t* DataStream)
{
    UInt8 Parity;

    DataStream->ReadCommand = FALSE;
    DataStream->CycleCount = 0;
    DataStream->TxCount = 0;
    DataStream->RxCount = 1;
    DataStream->IrqError = NO_ERROR;

    // Reset the preamble
    DataStream->RxBuffer[0] = 0;

    DataStream->TxLength = LENGTH_CMD + LENGTH_ADDR + LENGTH_DATA;
    DataStream->RxLength = LENGTH_PREAMBLE;

    // Copy the write command bits
    rfid11785LinkCopyBits (DataStream->TxBuffer, 0, &CmdWrite, 0, LENGTH_CMD);
    // Copy the write address
    rfid11785LinkCopyBits (DataStream->TxBuffer, LENGTH_CMD, &Address, 0, LENGTH_ADDR - 1);
    // Compute and set parity bit
    Parity = rfid11785LinkComputeParity (DataStream->TxBuffer, LENGTH_CMD, 1, LENGTH_ADDR - 3);
    rfid11785LinkWriteBit (DataStream->TxBuffer, LENGTH_CMD + LENGTH_ADDR - 1, Parity);
    // Copy the data word
    rfid11785LinkCopyDataField (DataStream->TxBuffer, LENGTH_CMD + LENGTH_ADDR, Data);
}


/*****************************************************************************/
/*!
 *  \brief   Starts an RFID read function
 *
 *      This method builds the bit stream of an RFID read message. The message
 *      will read a 32 bit register of the RFID tranceiver. It does not
 *      perform the transaction itself.
 *
 *  \iparam Address = Register read address
 *  \xparam DataStream = This structure is filled with the bit stream
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

void rfid11785LinkStartRead (UInt8 Address, Rfid11785Stream_t* DataStream)
{
    UInt8 Parity;

    DataStream->ReadCommand = TRUE;
    DataStream->CycleCount = 0;
    DataStream->TxCount = 0;
    DataStream->RxCount = 1;
    DataStream->IrqError = NO_ERROR;

    // Reset the preamble
    DataStream->RxBuffer[0] = 0;

    DataStream->TxLength = LENGTH_CMD + LENGTH_ADDR;
    DataStream->RxLength = LENGTH_PREAMBLE + LENGTH_DATA;

    // Copy the read command bits
    rfid11785LinkCopyBits (DataStream->TxBuffer, 0, &CmdRead, 0, LENGTH_CMD);
    // Copy the read address
    rfid11785LinkCopyBits (DataStream->TxBuffer, LENGTH_CMD, &Address, 0, LENGTH_ADDR - 1);
    // Compute and set parity bit
    Parity = rfid11785LinkComputeParity (DataStream->TxBuffer, LENGTH_CMD, 1, LENGTH_ADDR - 3);
    rfid11785LinkWriteBit (DataStream->TxBuffer, LENGTH_CMD + LENGTH_ADDR - 1, Parity);
}


/*****************************************************************************/
/*!
 *  \brief   Checks an RFID transaction for completion
 *
 *      This method checks if an RFID transaction has completed successfully.
 *      If this is the case, it will return TRUE.
 *
 *  \iparam  DataStream = This structure contains the transaction state
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t rfid11785LinkComplete (Rfid11785Stream_t *DataStream)
{
    if (DataStream->IrqError < NO_ERROR) {
        return (DataStream->IrqError);
    }
    else if (bmTimeExpired (DataStream->StartTime) >= TIMEOUT) {
        return (1);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Checks the response pattern of the RFID tag
 *
 *      This method checks if the pattern sent as a response from the RFID tag
 *      to the microcontroller indicates the occurrence of an error in the
 *      request message transmitted to the tag in advance. It also detects
 *      invalid response patterns and reports them accordingly.
 *
 *  \iparam  DataStream = This structure contains the transaction state
 *
 *  \return  1 (completed), NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t rfid11785LinkPatternOk (Rfid11785Stream_t *DataStream)
{
    if (DataStream->RxCount == DataStream->RxLength) {
        if (DataStream->RxBuffer[0] == PATTERN_PREAMBLE) {
            return (NO_ERROR);
        }
        else if (DataStream->RxBuffer[0] == PATTERN_ERROR) {
            return (E_RFID11785_ERROR_PATTERN);
        }
        else {
            return (E_RFID11785_UNKNOWN_PATTERN);
        }
    }
    return (E_RFID11785_TRANSACTION_TIMEOUT);
}


/*****************************************************************************/
/*!
 *  \brief   Checks the parity bits in a read response data package
 *
 *  \riskid  SWRA 5.5.1: Unreliable and wrong recognition of racks
 *
 *      This method controls the parity bits in the read response message
 *      that was received from the RFID tag. It reports a failure when one
 *      parity has the wrong value.
 *
 *  \iparam  DataStream = This structure contains the transaction state
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t rfid11785LinkParityOk (Rfid11785Stream_t *DataStream)
{
    UInt8 i;
    UInt8 ParityComp;
    UInt8 ParityRead;

    // Compute byte parity
    for (i = 0; i < 4; i++) {
        ParityComp = rfid11785LinkComputeParity (DataStream->RxBuffer, 8 + 9 * i, 1, 8);
        ParityRead = rfid11785LinkReadBit (DataStream->RxBuffer, 8 + 9 * (i + 1) - 1);
        if (ParityComp != ParityRead) {
            return (E_RFID11785_PARITY_ERROR);
        }
    }
    // Compute column parity
    for (i = 0; i < 8; i++) {
        ParityComp = rfid11785LinkComputeParity (DataStream->RxBuffer, 8 + i, 9, 4);
        ParityRead = rfid11785LinkReadBit (DataStream->RxBuffer, 8 + 36 + i);
        if (ParityComp != ParityRead) {
            return (E_RFID11785_PARITY_ERROR);
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Returns data from a read message
 *
 *      This method returns the data of a read message after a
 *      read transaction completed successfully.
 *
 *  \iparam  DataStream = This structure contains the transaction data
 *
 *  \return  Default read message
 *
 ****************************************************************************/

UInt32 rfid11785LinkGetData (Rfid11785Stream_t *DataStream)
{
    UInt8 i;
    UInt8 Buffer[4];
    UInt32 Result = 0;

    for (i = 0; i < 4; i++) {
        rfid11785LinkCopyBits (Buffer, 8 * i, DataStream->RxBuffer, 8 + 9 * i, 8);
        Result |= ((UInt32) (Buffer[i] & 0xFF)) << (8 * i);
    }

    return Result;
}


//****************************************************************************/
