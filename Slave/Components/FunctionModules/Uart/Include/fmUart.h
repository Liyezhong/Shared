/****************************************************************************/
/*! \file fmUart.h
 * 
 *  \brief Functional module controlling a serial communications interface.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 26.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *  This module connects the CAN interface which is used for master to slave
 *  communication with the internal serial communication interface of the
 *  microcontroller. It forwards configuration parameters to the serial
 *  interface implemented in the hardware abstraction layer (HAL). Beside of
 *  that, it writes to and reads from this serial interface. There are two
 *  ways data read from the UART controller is forwarded to the master. These
 *  modes are called request mode and immediate mode. In request mode, the
 *  data is sent in response to explicit requests received from the master.
 *  In immediate mode, the slave itself immediately sends data words fetched
 *  from the serial interface.
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

#ifndef FMUART_H
#define FMUART_H

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

// UART module related errors
//! Module is in immediate mode
#define E_UART_IMMEDIATE_MODE   BUILD_ERRCODE(MODULE_ID_UART, ERRCLASS_ERROR, 0)
//! Module is still disabled
#define E_UART_MODULE_DISABLED  BUILD_ERRCODE(MODULE_ID_UART, ERRCLASS_ERROR, 1)
//! Module is busy transmitting data
#define E_UART_MODULE_BUSY      BUILD_ERRCODE(MODULE_ID_UART, ERRCLASS_ERROR, 2)

// UART related CAN identifiers
//! Sets configuration and control parameters
#define MSG_UART_SET_CONF       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 1)
//! Writes a data package to the serial interface
#define MSG_UART_SET_DATA       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 6, 1)

//! Requests data from the serial interface
#define MSG_UART_REQ_DATA       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 1)

//! Responds data from the serial interface
#define MSG_UART_RESP_DATA      BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 0)

//! Acknowledges transmission of data to UART
#define MSG_UART_ACK_TRANSMIT   BUILD_CAN_ID(CMD_CLASS_FUNCTION, 6, 0)

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t uartInitializeModule (UInt16 ModuleID, UInt16 NumberOfInstances);
void uartRegisterDebugNames  (UInt16 ModuleID);

//****************************************************************************/

#endif /*FMUART_H*/
