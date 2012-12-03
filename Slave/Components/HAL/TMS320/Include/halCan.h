/****************************************************************************/
/*! \file halCAN.h
 * 
 *  \brief CAN message read/write functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 10.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to access and manage CAN controllers.
 *       This is currently a simulation only, since the real thing is not
 *       yet implemented.
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

#ifndef HAL_CAN_H
#define HAL_CAN_H

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

//! CAN interface control IDs (address identifier)
typedef enum {
    CAN_INTR_ENABLE  = 1,   //!< CAN bus interrupt enable
    CAN_INTR_DISABLE = 2,   //!< CAN bus interrupt disable
    CAN_INTR_CLEAR   = 3    //!< CAN bus clear interrupt flag 
} CAN_CONTROL_t;


//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

/*! CAN message buffer */
typedef struct {
    UInt32 CanID;           //!< CAN identifier
    UInt16 Length;          //!< Length of data
    UInt8  Data[8];         //!< Data bytes
    UInt32 Timestamp;       //!< Timestamp
} CAN_MESSAGE_t;

typedef void (*HAL_INTERRUPT_HANDLER) (UInt16 InterruptID);

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

ERROR_t  canRegisterInterrupt (UInt16 InterruptID, HAL_INTERRUPT_HANDLER IntHandler);

HANDLE_t halCanOpen    (CHANNEL_t ChannelID);
HANDLE_t halCanClose   (HANDLE_t Handle);
ERROR_t  halCanRead    (HANDLE_t Handle, CAN_MESSAGE_t* Message);
ERROR_t  halCanWrite   (HANDLE_t Handle, CAN_MESSAGE_t* Message);
ERROR_t  halCanControl (HANDLE_t Handle, UInt16 ParamID, UInt16 Value);
ERROR_t  halCanInject  (CAN_MESSAGE_t* Message);

ERROR_t  halInitializeCan (void);

//****************************************************************************/

#endif /*HAL_CAN_H*/
