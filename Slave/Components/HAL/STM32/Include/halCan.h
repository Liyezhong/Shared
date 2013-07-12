/****************************************************************************/
/*! \file halCan.h
 *
 *  \brief CAN message read/write functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage CAN controllers.
 *      It supports up to 2 CAN controllers (STM32 connectivity line).
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

#ifndef HAL_CAN_H
#define HAL_CAN_H

#include "halCommon.h"
#include "halInterrupt.h"

//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//! CAN interface control IDs (used with halCanControl function)
typedef enum {
    CAN_INTR_RxREADY     = 1,  //!< Control receiver ready interrupt
    CAN_INTR_TxREADY     = 2,  //!< Control transmitter ready interrupt
    CAN_INTR_ERROR       = 3,  //!< Control CAN bus error interrupt
    CAN_INTR_BUSOFF      = 4,  //!< Control bus-off interrupt
    CAN_INTR_PASSIVE     = 5,  //!< Control bus passive interrupt
    CAN_INTR_WARNING     = 6,  //!< Control warning level interrupt
    CAN_INTR_OVERRUN     = 7,  //!< Control receiver overrun interrupt
} CanCtrlID_t;

//! CAN interface status IDs (used with halCanStatus function)
typedef enum {
    CAN_STAT_RxCOUNT     = 1,  //!< Request receive message count
    CAN_STAT_TxREADY     = 2,  //!< Request tranmit mailbox status
    CAN_STAT_TxEMPTY     = 3,  //!< All messages send
    CAN_STAT_RxERRORS    = 4,  //!< Request receive error counter
    CAN_STAT_TxERRORS    = 5,  //!< Request transmit error counter
    CAN_STAT_BUS_ERROR   = 6,  //!< Request last bus error
    CAN_STAT_BUS_STATE   = 7   //!< Request actual bus state
} CanStatID_t;

//! CAN bus error codes returned by halCanStatus(CAN_STAT_BUS_ERROR)
typedef enum {
    CAN_NO_ERROR         = 0,  //!< No error detected
    CAN_STUFFING_ERROR   = 1,  //!< Bit stuffing error
    CAN_FORMAT_ERROR     = 2,  //!< Data format error
    CAN_ACKN_ERROR       = 3,  //!< Acknowledge error
    CAN_RECESSIVE_ERROR  = 4,  //!< Bit recessive error
    CAN_DOMINANT_ERROR   = 5,  //!< Bit dominant error
    CAN_CRC_ERROR        = 6,  //!< CRC checksum error
    CAN_INVALID_ERROR    = 7,  //!< Undefined error
} CanBusError_t;

//! CAN bus state IDs returned by halCanStatus(CAN_STAT_BUS_STATE)
typedef enum {
    CAN_STATE_ACTIVE     = 0,  //!< CAN bus is in active state
    CAN_STATE_WARNING    = 1,  //!< CAN bus is in warning state
    CAN_STATE_PASSIVE    = 2,  //!< CAN bus is in passive state
    CAN_STATE_BUSOFF     = 3,  //!< CAN bus is in bus-off state
} CanBusState_t;

//! CAN interrupt flag bits (passed to interrupt handlers)
typedef enum {
    CAN_IFLAG_RxREADY    = 1,  //!< Receive interrupt pending
    CAN_IFLAG_TxREADY    = 2,  //!< Transmit interrupt pending
    CAN_IFLAG_SYSTEM     = 4   //!< CAN bus system error pending
} CanIntrFlags_t;


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! CAN message buffer
typedef struct {
    UInt32 CanID;           //!< CAN identifier
    UInt16 Length;          //!< Length of data
    UInt8  Data[8];         //!< Data bytes
    UInt32 Timestamp;       //!< Timestamp
} CanMessage_t;

//! CAN acceptance filter specification
typedef struct {
    UInt32 CanID;           //!< CAN identifier
    UInt32 Mask;            //!< CAN filter mask
} CanIdFilter_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halCanOpen  (Device_t DeviceID, UInt32 UserTag, halIntrHandler_t Handler, Bool CheckHalStatus);
Error_t halCanSetup (Handle_t Handle, CanIdFilter_t *Filters, UInt16 Count);
Error_t halCanClose (Handle_t Handle);
Error_t halCanRead  (Handle_t Handle, CanMessage_t *Message);
Error_t halCanWrite (Handle_t Handle, CanMessage_t *Message);
Error_t halCanInit  (void);

Error_t halCanStatus  (Handle_t Handle, CanStatID_t StatusID);
Error_t halCanControl (Handle_t Handle, CanCtrlID_t ControlID, Bool State);

//****************************************************************************/

#endif /*HAL_CAN_H*/
