/****************************************************************************/
/*! \file fmRfidIso15693Link.h
 * 
 *  \brief Low level functions of the RFID ISO/IEC 15693 module.
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

#ifndef FMRFIDLINK15693_H
#define FMRFIDLINK15693_H

#include "Global.h"

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

#define RFID15693_LINK_CMD_INVENTORY    0x01    //!< Inventory command code according to ISO/IEC 15693-3
#define RFID15693_LINK_CMD_SINGLE_READ  0x20    //!< Read single block command code according to ISO/IEC 15693-3
#define RFID15693_LINK_CMD_SINGLE_WRITE 0x21    //!< Write single block command code according to ISO/IEC 15693-3
#define RFID15693_LINK_CMD_BLOCK_LOCK   0x22    //!< Lock block command code according to ISO/IEC 15693-3

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

/*! The state of an RFID transmission */
typedef enum {
    LINK_TXSTATE_SOF,
    LINK_TXSTATE_DATA,
    LINK_TXSTATE_EOF,
    LINK_TXSTATE_FINISHED
} Rfid15693TransmitState_t;

/*! The state of an RFID reception */
typedef enum {
    LINK_RXSTATE_WAIT,
    LINK_RXSTATE_SOF1,
    LINK_RXSTATE_SOF2,
    LINK_RXSTATE_DATA,
    LINK_RXSTATE_EOF
} Rfid15693ReceiveState_t;

/*! Containg the information transmitted and received during an RFID transaction */
typedef struct {
    UInt8 TxBuffer[17]; //!< Bit stream transmitted to the RFID chip
    UInt8 TxLength;     //!< Length of the transmit bit stream
    UInt8 TxCount;      //!< Bits already transmitted
    
    UInt8 RxBuffer[13]; //!< Bit stream received from the RFID chip
    UInt8 RxLength;     //!< Length of the receive bit stream
    UInt8 RxCount;      //!< Bits already received
    
    UInt8 PosCount;     //!< Holds the current pulse position for data coding
    Rfid15693TransmitState_t TransmitState; //!< State of the transmission process
    Rfid15693ReceiveState_t ReceiveState;   //!< State of the reception process
    Bool SymbolShift;   //!< Synchronized to the middle of a receive symbol
} Rfid15693Stream_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

void rfid15693LinkWriteBit (UInt8* Buffer, UInt8 Position, UInt8 Bit);
UInt8 rfid15693LinkReadBit (UInt8* Buffer, UInt8 Position);

void rfid15693LinkInventory (Rfid15693Stream_t *Stream);
void rfid15693LinkReadSingleBlock (Rfid15693Stream_t *Stream, UInt64 Uid, UInt8 BlockNumber);
void rfid15693LinkWriteSingleBlock (Rfid15693Stream_t *Stream, UInt64 Uid, UInt8 BlockNumber, UInt32 Data);
void rfid15693LinkLockBlock (Rfid15693Stream_t *Stream, UInt64 Uid, UInt8 BlockNumber);

Error_t rfid15693LinkCheckMessage (Rfid15693Stream_t *Stream);
UInt64 rfid15693GetUniqueId (Rfid15693Stream_t *Stream);
UInt32 rfid15693GetDataBlock (Rfid15693Stream_t *Stream);


//****************************************************************************/

#endif /*FMRFIDLINK15693_H*/
