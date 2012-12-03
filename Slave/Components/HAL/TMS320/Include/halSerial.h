/****************************************************************************/
/*! \file halSerial.h
 * 
 *  \brief  Serial communication interface
 *
 *  $Version: $ 0.1
 *  $Date:    $ 21.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to access and manage the on-chip
 *       serial communication interfaces (SCI) of the TMS processor.
 * 
 *       ATTENTION: This is still an early version of the module. 
 *       Development was stopped after changing the processor for
 *       the colorado/sepia project. For instance, the hardware  
 *       FIFO extension of the SCI peripherals are not yet used.
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

#ifndef HAL_SERIAL_H
#define HAL_SERIAL_H


//****************************************************************************/
// Public Constants
//****************************************************************************/

//! Status IDs for requesting the status using halSerialStatus()
typedef enum {
    SCI_STAT_TX_READY,          //!< Transmitter ready status
    SCI_STAT_TX_EMPTY,          //!< Transmitter empty status
    SCI_STAT_RX_READY,          //!< Receiver ready status
    SCI_STAT_RX_BREAK,          //!< Break signal received
    SCI_STAT_RECEIVE_ERROR,     //!< Receive error detected
    SCI_STAT_FRAMING_ERROR,     //!< Framing error detected
    SCI_STAT_OVERRUN_ERROR,     //!< Overrun error detected
    SCI_STAT_PARITY_ERROR       //!< Parity error detected
} SCI_STATUS_t;

//! Control IDs for halSerialControl()
typedef enum {
    SCI_INTR_RxENABLE,          //!< Receiver interrupt enable
    SCI_INTR_RxDISABLE,         //!< Receiver interrupt disable
    SCI_INTR_TxENABLE,          //!< Transmit interrupt enable
    SCI_INTR_TxDISABLE,         //!< Transmit interrupt disable
    SCI_CTRL_RESET              //!< Reset channel
} SCI_CONTROL_t;

//! Parity mode defines
typedef enum {
    SCI_NO_PARITY   = 0,        //!< Parity disabled
    SCI_PARITY_ODD  = 1,        //!< Use odd parity
    SCI_PARITY_EVEN = 3         //!< Use even parity
} SCI_FORMAT_PARITY_t;


//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

//! Structure to define the data communication format 
typedef struct {
    UInt16 Databits:4;          //!< Number of databits (1-8)
    UInt16 Stopbits:2;          //!< Number of stopbits (1-2)
    UInt16 Parity:2;            //!< Parity mode    
} SCI_FORMAT_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

ERROR_t halSerialOpen  (CHANNEL_t Channel, SCI_FORMAT_t *Format, UInt16 Baudrate);
ERROR_t halSerialMode  (HANDLE_t Handle, SCI_FORMAT_t *Format, UInt16 Baudrate);
ERROR_t halSerialRead  (HANDLE_t Handle, UInt8 *Buffer, Bool Blocking);
ERROR_t halSerialWrite (HANDLE_t Handle, UInt8 Data, Bool Blocking);
ERROR_t halSerialClose (HANDLE_t Handle);

ERROR_t halSerialControl (HANDLE_t Handle, SCI_CONTROL_t ControlID);
ERROR_t halSerialStatus  (HANDLE_t Handle, SCI_STATUS_t StatusID);

ERROR_t halSerialInitialize (void);

//****************************************************************************/

#endif
