/****************************************************************************/
/*! \file fmRfidIso15693.h
 * 
 *  \brief Functional module controlling RFID ISO 15693 communication.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 23.03.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *      This module contains the top level source code of the RFID ISO/IEC
 *      15693 communication controller. The code in this file implements
 *      functions, that are needed to communicate with RFID transponders
 *      implementing the ISO/IEC 15693 standard. The module controls the EM
 *      Microelectronic EM4094 RFID front end IC via the Hardware Abstraction
 *      Layer (HAL). For this purpose, the module uses digital I/O ports and a
 *      timer unit to control the time critical communication procedure
 *      through the generation of interrupts.
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

#ifndef FMRFID15693_H
#define FMRFID15693_H

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

// RFID module related errors
//! An RFID transaction is in progress
#define E_RFID15693_TRANSACTION_ACTIVE  BUILD_ERRCODE(MODULE_ID_RFID15693, ERRCLASS_ERROR, 0)
//! Reception of an unknown symbol from the tag
#define E_RFID15693_UNKNOWN_RXSYMBOL    BUILD_ERRCODE(MODULE_ID_RFID15693, ERRCLASS_ERROR, 1)
//! Invalid length of the receive message
#define E_RFID15693_INVALID_RXLENGTH    BUILD_ERRCODE(MODULE_ID_RFID15693, ERRCLASS_ERROR, 2)
//! Mismatch in the CRC check sum of a received message
#define E_RFID15693_CHECKSUM_MISMATCH   BUILD_ERRCODE(MODULE_ID_RFID15693, ERRCLASS_ERROR, 3)
//! Timeout of an RFID transaction
#define E_RFID15693_TRANSACTION_TIMEOUT BUILD_ERRCODE(MODULE_ID_RFID15693, ERRCLASS_ERROR, 4)
//! The ISO/IEC 15693 error flag is set
#define E_RFID15693_RESPONSE_ERRORFLAG  BUILD_ERRCODE(MODULE_ID_RFID15693, ERRCLASS_ERROR, 5)

//! RFID configuration acknowledgement
#define E_RFID15693_ACKNOWLEDGEMENT     BUILD_ERRCODE(MODULE_ID_RFID15693, ERRCLASS_INFO, 0)

// RFID module related CAN identifiers
//! Sets the configuration parameters of the module
#define MSG_RFID15693_SET_CONFIG        BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 1)
//! Acquires the UID from an RFID transponder
#define MSG_RFID15693_ACQUIRE_UID       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 6, 1)
//! Responds the acquired UID from an RFID transponder
#define MSG_RFID15693_RESP_ACQUIRE_UID  BUILD_CAN_ID(CMD_CLASS_FUNCTION, 6, 0)
//! Sets UID value in the function module
#define MSG_RFID15693_SET_UID           BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 1)
//! Requests the UID value stored in the function module
#define MSG_RFID15693_REQ_UID           BUILD_CAN_ID(CMD_CLASS_FUNCTION, 8, 1)
//! Responds the UID value stored in the function module
#define MSG_RFID15693_RESP_UID          BUILD_CAN_ID(CMD_CLASS_FUNCTION, 8, 0)
//! Reads a memory block from the RFID transponder
#define MSG_RFID15693_READ_BLOCK        BUILD_CAN_ID(CMD_CLASS_FUNCTION, 9, 1)
//! Responds a memory block from the RFID transponder
#define MSG_RFID15693_RESP_READ_BLOCK   BUILD_CAN_ID(CMD_CLASS_FUNCTION, 9, 0)
//! Writes a memory block in the RFID transponder
#define MSG_RFID15693_WRITE_BLOCK       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 10, 1)
//! Responds to a write command
#define MSG_RFID15693_RESP_WRITE_BLOCK  BUILD_CAN_ID(CMD_CLASS_FUNCTION, 10, 0)
//! Locks a memory block in the RFID transponder
#define MSG_RFID15693_LOCK_BLOCK        BUILD_CAN_ID(CMD_CLASS_FUNCTION, 11, 1)
//! Responds to a lock command
#define MSG_RFID15693_RESP_LOCK_BLOCK   BUILD_CAN_ID(CMD_CLASS_FUNCTION, 11, 0)

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t rfid15693InitializeModule (UInt16 ModuleID, UInt16 NumberOfInstances);
void rfid15693RegisterDebugNames  (UInt16 ModuleID);

//****************************************************************************/

#endif /*FMRFID15693_H*/
