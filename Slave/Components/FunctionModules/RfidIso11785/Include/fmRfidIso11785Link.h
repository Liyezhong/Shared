/****************************************************************************/
/*! \file fmRfidIso11785Link.h
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

#ifndef FMRFIDLINK11785_H
#define FMRFIDLINK11785_H

#include "Global.h"

//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

/*! Containg the status of an RFID transaction */
typedef struct {
    Bool ReadCommand;   //!< Indicates whether this is a read command

    UInt8 TxBuffer[8];  //!< Bit stream transmitted to the RFID chip
    UInt8 TxLength;     //!< Length of the transmit bit stream
    UInt8 TxCount;      //!< Bits already transmitted

    UInt8 RxBuffer[7];  //!< Bit stream received from the RFID chip
    UInt8 RxLength;     //!< Length of the receive bit stream
    UInt8 RxCount;      //!< Bits already received

    UInt8 CycleCount;   //!< Counts RFID signal cycles
    Bool OldValid;      //!< Indicates if the count value is valid
    UInt32 OldCount;    //!< Old counter value from the capture unit
    UInt32 OldTime;     //!< Time of the previous signal state
    UInt32 StartTime;   //!< Time the process started in ms
    Error_t IrqError;   //!< Stores a possible error code during interrupt handling
} Rfid11785Stream_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

void rfid11785LinkWriteBit (UInt8* Stream, UInt8 Position, UInt8 Bit);
UInt8 rfid11785LinkReadBit (const UInt8* Stream, UInt8 Position);
void rfid11785LinkStartLogin (UInt32 Password, Rfid11785Stream_t* DataStream);
void rfid11785LinkStartWrite (UInt8 Address, UInt32 Data, Rfid11785Stream_t* DataStream);
void rfid11785LinkStartRead (UInt8 Address, Rfid11785Stream_t* DataStream);
Error_t rfid11785LinkComplete (Rfid11785Stream_t *DataStream);
Error_t rfid11785LinkPatternOk (Rfid11785Stream_t *DataStream);
Error_t rfid11785LinkParityOk (Rfid11785Stream_t *DataStream);
UInt32 rfid11785LinkGetData (Rfid11785Stream_t *DataStream);

//****************************************************************************/

#endif /*FMRFIDLINK11785_H*/
