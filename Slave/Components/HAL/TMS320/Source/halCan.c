/****************************************************************************/
/*! \file halCan.c
 * 
 *  \brief CAN message read/write functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 12.10.2010
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

#include <stdlib.h>
#include "Global.h"
#include "halCommon.h"
#include "bmDebug.h"
#include "halConfigure.h"
#include "halError.h"
#include "halCan.h"

//****************************************************************************/
// Private Variables 
//****************************************************************************/

static HAL_INTERRUPT_HANDLER canTransmitInterruptHandler = NULL;
static HAL_INTERRUPT_HANDLER canReceiveInterruptHandler  = NULL;

static Bool canReceiveInterruptEnable  = FALSE;
static Bool canTransmitInterruptEnable = FALSE;

static CAN_MESSAGE_t canInMessage = {0};

//****************************************************************************/
// Implementation
//****************************************************************************/

HANDLE_t halCanOpen (CHANNEL_t Channel)
{
    return (0x1234);
}

//****************************************************************************/

ERROR_t halCanControl (HANDLE_t Handle, UInt16 InterruptID, UInt16 Value)
{
    Bool oldTransmitInterruptEnable = canTransmitInterruptEnable;
    
    if (InterruptID == INTERRUPT_CAN1_RX) {
        if (Value == CAN_INTR_ENABLE)
            canReceiveInterruptEnable = TRUE; 
        else if (Value == CAN_INTR_DISABLE)
            canReceiveInterruptEnable = FALSE; 
    }
    if (InterruptID == INTERRUPT_CAN1_TX) {
        if (Value == CAN_INTR_ENABLE) {
            canTransmitInterruptEnable = TRUE;

            if (!oldTransmitInterruptEnable && canTransmitInterruptHandler) {
                (*canTransmitInterruptHandler) (INTERRUPT_CAN1_TX);
            }
        } 
        else if (Value == CAN_INTR_DISABLE)
            canTransmitInterruptEnable = FALSE; 
    }
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t halCanWrite (HANDLE_t Handle, CAN_MESSAGE_t* Message)
{
    if (Handle != 0x1234 || Message == NULL) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    dbgPrintCanMessage (Message, 'w');    
    return (1); 
}

//****************************************************************************/

ERROR_t halCanRead (HANDLE_t Handle, CAN_MESSAGE_t* Message)
{
    if (Handle != 0x1234)
        return (E_PARAMETER_OUT_OF_RANGE);
    
    if (Message != NULL) {        
        if (canInMessage.CanID != 0) {
            dbgPrintCanMessage (&canInMessage, 'r');    
            *Message = canInMessage;
            canInMessage.CanID = 0;
            return (1);
        }
    }    
    return (0);        
}

//****************************************************************************/

ERROR_t canRegisterInterrupt ( 
    UInt16 InterruptID, HAL_INTERRUPT_HANDLER Handler)
{
    if (InterruptID == INTERRUPT_CAN1_RX) {
        canReceiveInterruptHandler = Handler;
    }
    if (InterruptID == INTERRUPT_CAN1_TX) {
        canTransmitInterruptHandler = Handler;
    }
    return (NO_ERROR);
}   

//****************************************************************************/

ERROR_t halCanInject (CAN_MESSAGE_t* Message)
{
    if (!isPtrNull(Message)) {
        if (canInMessage.CanID == 0) {
            canInMessage = *Message;
        }
        if (canReceiveInterruptHandler && canReceiveInterruptEnable) {
            (*canReceiveInterruptHandler) (INTERRUPT_CAN1_RX);
        }
    }    
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t halInitializeCanLayer (void) 
{
    return (NO_ERROR);
}

//****************************************************************************/
