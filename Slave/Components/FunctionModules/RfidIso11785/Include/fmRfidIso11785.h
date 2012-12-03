/****************************************************************************/
/*! \file fmRfidIso11785.h
 * 
 *  \brief Functional module controlling RFID ISO 11785 communication.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 12.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *      This module contains the top level source code of the RFID ISO 11785
 *      communication controller. The code in this file implements functions,
 *      that are needed to communicate with the EM Microelectronic EM4205/4305
 *      RFID transponder. This transponder uses a propietary protocol on top
 *      of ISO 11785 and thus is not standard compliant. The module controls
 *      the EM Microelectronic EM4095 RFID front end IC via the Hardware
 *      Abstraction Layer (HAL). For this purpose, the module uses digital I/O
 *      ports and a timer unit to control the time critical communication
 *      procedure through the generation of interrupts.
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

#ifndef FMRFID11785_H
#define FMRFID11785_H

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

// RFID module related errors
//! An RFID transaction is in progress
#define E_RFID11785_TRANSACTION_ACTIVE      BUILD_ERRCODE(MODULE_ID_RFID11785, ERRCLASS_ERROR, 0)
//! Data transmission to the tag went wrong
#define E_RFID11785_ERROR_PATTERN           BUILD_ERRCODE(MODULE_ID_RFID11785, ERRCLASS_ERROR, 1)
//! Data reception from the tag went wrong
#define E_RFID11785_UNKNOWN_PATTERN         BUILD_ERRCODE(MODULE_ID_RFID11785, ERRCLASS_ERROR, 2)
//! Detected a parity error in a read response
#define E_RFID11785_PARITY_ERROR            BUILD_ERRCODE(MODULE_ID_RFID11785, ERRCLASS_ERROR, 3)
//! Timeout of the RFID transaction, tag is possibly not in range
#define E_RFID11785_TRANSACTION_TIMEOUT     BUILD_ERRCODE(MODULE_ID_RFID11785, ERRCLASS_ERROR, 4)

// RFID module related CAN identifiers
//! Sets the configuration parameters of the module
#define MSG_RFID11785_SET_CONFIG            BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 1)
//! Sends a login message to the RFID transponder
#define MSG_RFID11785_SEND_LOGIN            BUILD_CAN_ID(CMD_CLASS_FUNCTION, 6, 1)
//! Sets a user data register of the RFID transponder
#define MSG_RFID11785_WRITE_USER_DATA       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 1)
//! Sets the password register of the RFID transponder
#define MSG_RFID11785_WRITE_PASSWORD        BUILD_CAN_ID(CMD_CLASS_FUNCTION, 8, 1)
//! Sets the configuration register of the RFID transponder
#define MSG_RFID11785_WRITE_CONFIG          BUILD_CAN_ID(CMD_CLASS_FUNCTION, 9, 1)

//! Requests user data read from the RFID transponder
#define MSG_RFID11785_READ_USER_DATA        BUILD_CAN_ID(CMD_CLASS_FUNCTION, 10, 1)
//! Requests the UID number of the RFID transponder
#define MSG_RFID11785_READ_UID_NUMBER       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 11, 1)

//! Acknowledges the set configuration parameters message
#define MSG_RFID11785_ACK_CONFIG            BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 0)
//! Responds to a login message to the RFID transponder
#define MSG_RFID11785_RESP_LOGIN            BUILD_CAN_ID(CMD_CLASS_FUNCTION, 6, 0)
//! Responds to write user data messages sent to the RFID transponder
#define MSG_RFID11785_RESP_WRITE_USER_DATA  BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 0)
//! Responds to write password messages sent to the RFID transponder
#define MSG_RFID11785_RESP_WRITE_PASSWORD   BUILD_CAN_ID(CMD_CLASS_FUNCTION, 8, 0)
//! Responds to write configuration messages sent to the RFID transponder
#define MSG_RFID11785_RESP_WRITE_CONFIG     BUILD_CAN_ID(CMD_CLASS_FUNCTION, 9, 0)

//! Responds user data read from the RFID transponder
#define MSG_RFID11785_RESP_READ_USER_DATA   BUILD_CAN_ID(CMD_CLASS_FUNCTION, 10, 0)
//! Responds the UID number of the RFID transponder
#define MSG_RFID11785_RESP_READ_UID_NUMBER  BUILD_CAN_ID(CMD_CLASS_FUNCTION, 11, 0)

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t rfid11785InitializeModule (UInt16 ModuleID, UInt16 NumberOfInstances);
void rfid11785RegisterDebugNames  (UInt16 ModuleID);

//****************************************************************************/

#endif /*FMRFID11785_H*/
