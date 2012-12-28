/****************************************************************************/
/*! \file halSpiBus.h
 * 
 *  \brief  SPI bus interface
 *
 *  $Version: $ 0.1
 *  $Date:    $ 02.07.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage the on-chip SPI
 *      interfaces. Since SPI is a bus, where multiple bus members (slaves) 
 *      can be connected to, each interface can be opened several times - 
 *      once for each bus member. The transfer to or from a certain slave 
 *      is done using synchronous read and write functions, using the slave 
 *      handle returned by the open function. Synchronously means, that the 
 *      function returns not until the transaction is complete. 
 *
 *      Attention: The stepper motors also use an SPI interface. Because
 *      of the very tight timing requirements the stepper motor module
 *      directly accesses the SPI hardware instead of using this module.
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

#ifndef HAL_SPIBUS_H
#define HAL_SPIBUS_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

#define SPI_FORMAT_MSB1ST       0   //!< Shift direction: MSB 1st
#define SPI_FORMAT_LSB1ST       1   //!< Shift direction: LSB 1st
#define SPI_FORMAT_CLK_PHASE1   0   //!< Start with 1st clock transition
#define SPI_FORMAT_CLK_PHASE2   2   //!< Start with 2nd clock transition
#define SPI_FORMAT_CLK_IDLE_0   0   //!< Clock = 0 when idle
#define SPI_FORMAT_CLK_IDLE_1   4   //!< Clock = 1 when idle


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Status IDs for requesting the state of SPI interfaces
typedef enum {    
    SPI_STAT_LOCKED,           //!< Return lock status
    SPI_STAT_CSPORT,           //!< For HAL internal use only
    SPI_STAT_MODE              //!< For HAL internal use only
} SpiStatID_t;

//! Control IDs for controlling SPI interfaces
typedef enum {
    SPI_CTRL_LOCK              //!< Lock bus for exclusive use
} SpiCtrlID_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halSpiOpen     (Device_t DeviceID, UInt32 UserTag, halIntrHandler_t Handler);
Error_t halSpiClose    (Handle_t Handle);
Error_t halSpiInit     (void);

Error_t halSpiSetup    (Handle_t Handle, UInt32 Baudrate, UInt32 Format);
Error_t halSpiTransfer (Handle_t Handle, UInt8 *Buffer, UInt32 Count);

//****************************************************************************/

#endif
