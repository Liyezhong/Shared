/****************************************************************************/
/*! \file halSerial.h
 *
 *  \brief  Serial communication interface
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage the on-chip
 *      asynchronous serial communication interfaces (UART).
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

#ifndef HAL_SERIAL_H
#define HAL_SERIAL_H


//****************************************************************************/
// Public Constants
//****************************************************************************/

//! Status IDs for requesting the status using halSerialStatus()
typedef enum {
    SCI_STAT_RxREADY,         //!< Receiver ready (data available)
    SCI_STAT_TxREADY,         //!< Transmitter ready for sending
    SCI_STAT_TxEMPTY,         //!< Transmitter empty (all send)
    SCI_STAT_RxIDLE           //!< Receive line idle
} SciStatID_t;

//! Control IDs for halSerialControl()
typedef enum {
    SCI_INTR_RxREADY,         //!< Receiver interrupt enable
    SCI_INTR_TxREADY,         //!< Transmit interrupt enable
    SCI_INTR_RxIDLE,          //!< Idle interrupt enable
    SCI_CTRL_TxBREAK          //!< Send break sequence
} SciCtrlID_t;

//! Parity mode defines
typedef enum {
    SCI_PARITY_NONE   = 0,    //!< Parity disable
    SCI_PARITY_ODD    = 1,    //!< Use odd parity
    SCI_PARITY_EVEN   = 3     //!< Use even parity
} SciParityType_t;

//! Hardware handshake mode defines (RTS+CTS can be combined)
typedef enum {
    SCI_FLOW_NONE     = 0,    //!< No flow control
    SCI_FLOW_CTS      = 1,    //!< Transmit flow control using CTS
    SCI_FLOW_RTS      = 2     //!< Receiver flow control using RTS
} SciFlowCtrl_t;

//! Interrupt flag bits (passed to interrupt handler)
typedef enum {
    SCI_IFLAG_RxIDLE  = 1,    //!< Idle line detected
    SCI_IFLAG_RxREADY = 2,    //!< Receive data available
    SCI_IFLAG_TxDONE  = 4,    //!< Transmission complete
    SCI_IFLAG_TxREADY = 8     //!< Transmitter ready
} SciIntrFlags_t;


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Structure to define the data communication format
typedef struct {
    UInt32 Databits :4;       //!< Number of databits (7-9)
    UInt32 Stopbits :2;       //!< Number of stopbits (1-2)
    UInt32 Parity   :2;       //!< Parity mode (odd, even, none)
    UInt32 FlowCtrl :2;       //!< Flow control selection
    UInt32 Baudrate;          //!< Baud rate (bits per second)
} SciFormat_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halSerialOpen    (Device_t DeviceID, UInt32 UserTag, halIntrHandler_t Handler);
Error_t halSerialClose   (Handle_t Handle);
Error_t halSerialInit    (void);

Error_t halSerialSetup   (Handle_t Handle, const SciFormat_t *Format);
Error_t halSerialRead    (Handle_t Handle, UInt16 *Buffer);
Error_t halSerialWrite   (Handle_t Handle, UInt16 Data);

Error_t halSerialControl (Handle_t Handle, SciCtrlID_t ControlID, Bool State);
Error_t halSerialStatus  (Handle_t Handle, SciStatID_t StatusID);

//****************************************************************************/

#endif
