/****************************************************************************/
/*! \file bmDispatch.h
 *
 *  \brief CAN Message Dispatcher
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains the CAN command dispatcher, which is responsible
 *       for the handling of incoming CAN messages. Other modules have to
 *       register the messages they want to handle. The dispatcher calls the
 *       registered handler function when the associated message is received.
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

#ifndef BM_DISPATCH_H
#define BM_DISPATCH_H


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! CAN command handler function type
typedef Error_t (*bmCanCallBack_t) (UInt16, CanMessage_t*);


//! Structure used to register CAN message handlers
typedef struct {
    UInt32 CanID;               //!< CAN identifier
    bmCanCallBack_t Handler;    //!< Function to handle the command
} bmCallbackEntry_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t canRegisterMessages  (UInt16 ModuleID, bmCallbackEntry_t *Commands, UInt16 Count);
Error_t canRegisterBroadcast (bmCallbackEntry_t *Commands, UInt16 Count);
Error_t canDispatchMessages  (void);
Error_t canInitDispatcher    (UInt16 ModuleCount);

//****************************************************************************/

#endif /*BM_DISPATCH_H*/
