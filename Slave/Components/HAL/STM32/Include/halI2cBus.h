/****************************************************************************/
/*! \file halI2cBus.h
 *
 *  \brief   I2C Inter-IC-Bus interface
 *
 *  $Version: $ 0.1
 *  $Date:    $ 12.05.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage the on-chip
 *      Inter-IC-Bus (I2C) interfaces. Since I2C is a bus, where multiple
 *      bus members (slaves) can be connected to, each interface can be
 *      opened several times - once for each bus member. The transfer to
 *      or from a certain slave is done using asynchronous read and write
 *      functions, using the slave handle returned by the open function.
 *      Asynchronous means, that the function returns immediately, w/o
 *      waiting for the completion of the transaction. When the operation
 *      has finished, a user function is called to notify the caller. 
 *      The user can supply this callback function when opening the I2C
 *      interface for a slave. Each slave can have its own callback.
 *
 *      The read/write transactions are stored in a so called Job Queue,
 *      and processed from this queue asynchronously per interrupt. The
 *      I2C protocol is event driven, the data transfer is done using
 *      direct memory access (DMA) controller.
 *
 *      Attention: If using baudrates greater than the maximum specified
 *      for standard mode the interface is switched into fast mode. As a
 *      workaround according to STM errata sheet this limit is reduced
 *      from 100kHz to 88kHz.
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

#ifndef HAL_I2C_H
#define HAL_I2C_H


//****************************************************************************/
// Public Constants
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Pointer to notification callback function
typedef void (*I2cCallback_t) (UInt16 JobID, Error_t Status);

//! Structure to hold a receive/transmit job
typedef struct {
    UInt8  Header;          //!< I2C header (slave ID, direction)
    UInt8  AddrLen;         //!< I2C address length
    UInt32 Address;         //!< I2C address 
    UInt8 *Buffer;          //!< Data buffer pointer
    UInt16 Count;           //!< Number of bytes to transfer
    UInt16 JobID;           //!< Job identifier
    I2cCallback_t Notify;   //!< User callback pointer
} I2cJob_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halI2cOpen  (Device_t Device, UInt8 SlaveNo, I2cCallback_t Callback);
Error_t halI2cSetup (Handle_t Handle, UInt32 Baudrate, UInt32 RiseTime);
Error_t halI2cClose (Handle_t Handle);
Error_t halI2cWait  (Handle_t Handle, UInt32 Timeout);
Error_t halI2cInit  (void);

Error_t halI2cExchange (Handle_t Handle, 
               UInt32 Address, UInt8 *Buffer, UInt16 Count, Bool Direction);

//****************************************************************************/

#endif
