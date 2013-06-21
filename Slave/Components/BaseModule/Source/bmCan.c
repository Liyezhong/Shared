/****************************************************************************/
/*! \file bmCan.c
 *
 *  \brief CAN message read/write functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to read/write CAN-Messages.
 *       It uses the hardware abstraction layer to access the CAN
 *       controller. Messages read or written are stored in queues.
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
#include "halLayer.h"
#include "bmError.h"
#include "bmDebug.h"
#include "bmUtilities.h"
#include "bmBlink.h"
#include "bmTime.h"
#include "bmCan.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define CAN_SEND_QUEUE_SIZE    16    //!< Size of transmit queue (messages)
#define CAN_RECV_QUEUE_SIZE    16    //!< Size of receive queue (messages)

//! Macro to extract the channel number out of a CAN-ID
#define GET_CANiD_CHANNEL(i)   (((i) >> CANiD_SHIFT_CHANNEL) & CANiD_MAX_CHANNEL)

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Data structure to hold queue state and message data
typedef struct {
    UInt16 Size;            //!< Size of queue (max. number of elements)
    UInt16 Count;           //!< Number of elements in queue
    UInt16 NextIn;          //!< Index to next free position
    UInt16 NextOut;         //!< Index to next-out message
    CanMessage_t *Data;     //!< Message buffer
} bmCanQueue_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

static UInt32 NodeAddress;            //!< CAN address of this node

static Bool RecvMessageLost = FALSE;  //!< Number of lost receive messages
static Bool SendMessageLost = FALSE;  //!< Number of lost transmit messages

static bmCanQueue_t SendQueue;        //!< Message transmission queue
static bmCanQueue_t RecvQueue;        //!< Message reception queue

static Handle_t CanHandle;            //!< CAN controller handle (HAL)

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t canCreateQueue (bmCanQueue_t *Queue, UInt16 MaxSize);
static Error_t canWriteQueue  (bmCanQueue_t *Queue, CanMessage_t *Message);
static Error_t canReadQueue   (bmCanQueue_t *Queue, CanMessage_t *Message);
static Error_t canTestQueue   (bmCanQueue_t *Queue, CanMessage_t *Message);
static Error_t canSkipQueue   (bmCanQueue_t *Queue, UInt16 Elements);
static Error_t canQueueCount  (bmCanQueue_t *Queue);

static void canHandleInterrupts  (UInt32 UserTag, UInt32 IntrFlags);
static void canHandleTxInterrupt (void);
static void canHandleRxInterrupt (void);
 


/*****************************************************************************/
/*!
 *  \brief   Write transmit message
 *
 *      Writes the given "Message" into the send queue and enables the
 *      transmit interrupt on the CAN controller, to asynchronously
 *      send the messages from the queue. If the write queue is full,
 *      the SendMessagesLost counter is incremented and an error is
 *      returned. Before writing the message into the queue, the given
 *      Channel number is injected into the CAN-ID of the message.
 *
 *  \iparam  Channel = Channel ID
 *  \iparam  Message = Message to send
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t canWriteMessage (UInt16 Channel, CanMessage_t *Message) {

    Error_t Count;

    if (Message != NULL && Channel <= CANiD_MAX_CHANNEL) {

        // insert channel number into CAN identifier
        Message->CanID =
            (Message->CanID & ~CANiD_MASK_CHANNEL) |
            (NodeAddress & CANiD_MASK_ADDRESS)     |
            (Channel << CANiD_SHIFT_CHANNEL);

        // protect queue against concurrent access in interrupt context
        halCanControl(CanHandle, CAN_INTR_TxREADY, OFF);

        Count = canWriteQueue (&SendQueue, Message);

        if (Count < 1) {
            SendMessageLost = TRUE;
        }
        canHandleTxInterrupt();
        if (canQueueCount(&SendQueue) > 0) {
            halCanControl (CanHandle, CAN_INTR_TxREADY, ON);
        }
        //dbgPrintCanMessage (Message, 'w');

        return (NO_ERROR);
    }
    return (E_PARAMETER_OUT_OF_RANGE);
}


/*****************************************************************************/
/*!
 *  \brief   Read receive message
 *
 *      Reads the next message from the receive queue and copies it to the
 *      structure pointed to by "Message". The Channel-ID is extracted from
 *      the CAN-ID of the message and copied to the buffer pointed to by
 *      "Channel". If "Message" is null or reading the message fails, a
 *      negative error code is returned; else the number of read messages
 *      is returned, which can be 1 or 0.
 *
 *  \oparam  Channel = Buffer to copy channel ID to
 *  \oparam  Message = Structure to copy message to
 *
 *  \return  Number of read messages [0,1] or (negative) error code
 *
 ****************************************************************************/

Error_t canReadMessage (UInt16 *Channel, CanMessage_t *Message) {

    UInt16 Count;

    if (Message != NULL) {

        // protect queue against concurrent access in interrupt context
        halCanControl (CanHandle, CAN_INTR_RxREADY, OFF);
        Count = canReadQueue (&RecvQueue, Message);
        halCanControl (CanHandle, CAN_INTR_RxREADY, ON);

        if (Count > 0) {
            if (Channel != NULL) {
                *Channel = GET_CANiD_CHANNEL(Message->CanID);
            }
            //dbgPrintCanMessage (Message, 'r');
        }
        return (Count);
    }
    return (E_PARAMETER_OUT_OF_RANGE);
}


/*****************************************************************************/
/*!
 *  \brief   Flush transmit messages
 *
 *      Waits until all messages in the transmit queue and mailboxes are
 *      send. The maximal wait time
 *
 *  \iparam  Timeout = Max. wait time [ms]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t canFlushMessages (UInt32 Timeout) {

    UInt32 StartTime = bmGetTime();

    // wait until send queue is empty
    while (canQueueCount(&SendQueue) > 0) {
        if (bmTimeExpired(StartTime) > Timeout) {
            return (E_TIMEOUT);
        }
    }
    // wait until all messages send
    while (!halCanStatus (CanHandle, CAN_STAT_TxEMPTY)) {
        if (bmTimeExpired(StartTime) > Timeout) {
            return (E_TIMEOUT);
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   CAN receive interrupt handler
 *
 *      This function is called by the HAL interrupt controller, when a
 *      receive interrupt is raised in the CAN peripheral. It reads a 
 *      message from the CAN controller and writes it to the receive queue. 
 *      If the queue is already full, the RecvMessagesLost variable is 
 *      incremented.
 *
 *  \return  Nothing
 *
 ****************************************************************************/

static void canHandleRxInterrupt (void) {

    CanMessage_t Message;

    while (halCanRead (CanHandle, &Message) > 0) {

        if (canWriteQueue (&RecvQueue, &Message) <= 0) {
            RecvMessageLost = TRUE;
        }
        else {
            RecvMessageLost = FALSE;
        }
    }
}


/*****************************************************************************/
/*!
 *  \brief   CAN transmit interrupt handler
 *
 *      This function is called by the HAL interrupt controller, when a
 *      transmit interrupt is raised in the CAN peripheral. It checks 
 *      if a new messages is waiting in the send queue, and writes it 
 *      to the CAN controller. If the write succeeds, the message is 
 *      removed from the send queue.
 *
 *      If the send queue is empty transmit interrupts are disabled.
 *
 *  \return  Nothing
 *
 ****************************************************************************/

static void canHandleTxInterrupt (void) {

    CanMessage_t Message;

    while (canTestQueue (&SendQueue, &Message) > 0) {

        if (halCanWrite (CanHandle, &Message) > 0) {
            canSkipQueue (&SendQueue, 1);
        }
        else break;
    }
    if (canQueueCount(&SendQueue) == 0) {
        halCanControl (CanHandle, CAN_INTR_TxREADY, OFF);
    }
}


/*****************************************************************************/
/*!
 *  \brief   CAN master interrupt handler
 *
 *      This function is called by the HAL interrupt controller, when one of
 *      the interrupts are raised by the CAN controller, and that interrupt 
 *      is enabled. The reason of the interrupt is indicated by IntrFlags,
 *      which contains bits for the following reasons:
 *
 *      - Transmit mailbox available
 *      - Receive message available
 *      - Error/status interrupt
 *
 *      It calls the responsible sub-handler to service the interrupt.
 *
 *  \iparam  IntrFlags = Interrupt reason bitmask
 *  \iparam  UserTag   = User tag defined at registration time
 *
 *  \return  Nothing
 *
 ****************************************************************************/

static void canHandleInterrupts (UInt32 UserTag, UInt32 IntrFlags) {

    if (IntrFlags & CAN_IFLAG_RxREADY) {
        canHandleRxInterrupt();
    }    
    if (IntrFlags & CAN_IFLAG_TxREADY) {
        canHandleTxInterrupt();
    }    
    if (IntrFlags & CAN_IFLAG_SYSTEM) {
        //canHandleErrorInterrupt();
    }    
}


/*****************************************************************************/
/*!
 *  \brief   Creates a new messaeg queue
 *
 *      Creates a new message queue with a capacity of "NumberOfElements"
 *      elements and initializes the given queue descriptor "Queue".
 *
 *      If one of the parameters are 0, or there isn't enough free memory
 *      available on the heap, an error code is returned.
 *
 *  \oparam  Queue    = Queue descriptor pointer
 *  \iparam  Elements = Capacity of the queue (messages)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t canCreateQueue (bmCanQueue_t *Queue, UInt16 Elements) {

    if (Queue == NULL || !Elements) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    Queue->Data = calloc (Elements, sizeof(CanMessage_t));

    if (Queue->Data != NULL) {

        Queue->Size   = Elements;
        Queue->NextIn = Queue->NextOut = 0;
        Queue->Count  = 0;
        return (NO_ERROR);
    }
    Queue->Size = 0;
    return (E_MEMORY_FULL);
}


/*****************************************************************************/
/*!
 *  \brief   Delete all messages in queue
 *
 *      Destroys the given "Queue".
 *
 *  \xparam  Queue = Queue descriptor pointer
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

/*static*/ Error_t canDeleteQueue (bmCanQueue_t *Queue) {

    if (Queue == NULL) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (Queue->Data != NULL) {
        free (Queue->Data);
    }
    Queue->NextIn = Queue->NextOut = 0;
    Queue->Count  = 0;
    Queue->Size   = 0;

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Skip messages in queue
 *
 *      Removes the given number of "Elements" from top of the "Queue".
 *      If "Elements" is greater than the number of elements in the queue,
 *      all elements will be removed w/o errors.
 *
 *  \xparam  Queue    = Queue descriptor pointer
 *  \iparam  Elements = Number of messages to skip
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t canSkipQueue (bmCanQueue_t *Queue, UInt16 Elements) {

    if (Queue == NULL) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (Elements > Queue->Count) {
        Elements = Queue->Count;
    }
    Queue->NextOut += Elements;
    if (Queue->NextOut >= Queue->Size) {
        Queue->NextOut -= Queue->Size;
    }
    Queue->Count -= Elements;

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Write message to queue
 *
 *      Checks if there is at least space for one more message in the given
 *      Queue, and copies the Message pointed to by the "Message" into it.
 *
 *      If one of the parameters are null or the queue is not allocated
 *      or full, a (negative) error code is returned. In case of success
 *      the number of written messages [1] is returned.
 *
 *  \xparam  Queue   = Queue descriptor pointer
 *  \iparam  Message = Message to be written
 *
 *  \return  Number of written messages or (negative) error code
 *
 ****************************************************************************/

static Error_t canWriteQueue (bmCanQueue_t *Queue, CanMessage_t *Message) {

    if (Queue == NULL || Message == NULL) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (Queue->Data == NULL) {
        return (E_QUEUE_NOT_ALLOCATED);
    }
    if (Queue->Count == Queue->Size) {
        return (E_QUEUE_FULL);
    }
    Queue->Data[Queue->NextIn] = *Message;

    if (++Queue->NextIn == Queue->Size) {
        Queue->NextIn = 0;
    }
    Queue->Count++;

    return (1);   // one message written
}


/*****************************************************************************/
/*!
 *  \brief   Read message from queue
 *
 *      Check if there is at least one message in the given Queue, removes
 *      the oldest message from the queue and copies it into the structure
 *      pointed to by Message. The return value indicates the number of
 *      messages read, which can be 1 or 0.
 *
 *      If one of the parameters are null or the queue is not allocated,
 *      a (negative) error code is returned.
 *
 *  \xparam  Queue   = Queue descriptor pointer
 *  \oparam  Message = Buffer to copy message to
 *
 *  \return  Number of read messages [0,1] or (negative) error code
 *
 ****************************************************************************/

static Error_t canReadQueue (bmCanQueue_t *Queue, CanMessage_t *Message) {

    if (Queue == NULL || Message == NULL) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (Queue->Data == NULL) {
        return (E_QUEUE_NOT_ALLOCATED);
    }
    if (Queue->Count) {
        *Message = Queue->Data[Queue->NextOut];

        if (++Queue->NextOut == Queue->Size) {
            Queue->NextOut = 0;
        }
        Queue->Count--;

        return (1); // one message read
    }
    return (0); // no message read
}


/*****************************************************************************/
/*!
 *  \brief   Read message from queue w/o removal
 *
 *      Check if there is at least one message in the given Queue, and
 *      copies it into the structure pointed to by "Message". Other than
 *      canReadQueue, the message is not removed from the queue.
 *
 *      If the "Queue" parameters is null or the queue is not allocated,
 *      a (negative) error code is returned. If only the count of enqueued
 *      messages is of interest, "Message" can be set to null.
 *
 *  \iparam  Queue   = Queue descriptor pointer
 *  \oparam  Message = Buffer to copy message to
 *
 *  \return  Number of messages in the queue or (negative) error code
 *
 ****************************************************************************/

static Error_t canTestQueue (bmCanQueue_t *Queue, CanMessage_t *Message) {

    if (Queue == NULL) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (Queue->Data == NULL) {
        return (E_QUEUE_NOT_ALLOCATED);
    }
    if (Queue->Count && Message != NULL) {
        *Message = Queue->Data[Queue->NextOut];
    }
    return (Queue->Count);
}


/*****************************************************************************/
/*!
 *  \brief   Get number of elements in queue
 *
 *      Returns the number of elements in the queue.
 *
 *  \xparam  Queue = Queue descriptor pointer
 *
 *  \return  Number of elements in queue or (negative) error code
 *
 ****************************************************************************/

static Error_t canQueueCount (bmCanQueue_t *Queue) {

    if (Queue == NULL) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    return (Queue->Count);
}


/*****************************************************************************/
/*!
 *  \brief   CAN task function
 *
 *      This function must be called by the scheduler in a regular interval.
 *      It is responsible for processing of asynchronous situations, like
 *      loss of messages. Currently, a blink code is issued in case of a
 *      lost message and an error signaled to the master. Since an error
 *      message can't be send if transmission is disturbed, transmit errors
 *      will be send not before the CAN bus is operational again.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t canTaskFunction (void) {

    static Bool RecvErrorFlaged = FALSE;
    static Bool SendErrorFlaged = FALSE;
    static Bool BlinkErrorLed = FALSE;
    Bool ErrFlag;

    // signal loss of receive message
    if (RecvMessageLost != RecvErrorFlaged) {
        if (RecvMessageLost) {
            bmSignalEvent (0, E_RECEIVE_MESSAGE_LOST, TRUE, 0);
        }
        RecvErrorFlaged = RecvMessageLost;
    }
    // handle loss of transmit message
    if (SendMessageLost != SendErrorFlaged) {
        if (!SendMessageLost) {
            bmSignalEvent (0, E_TRANSMIT_MESSAGE_LOST, TRUE, 0);
        }
        SendErrorFlaged = SendMessageLost;
    }
    ErrFlag = (RecvErrorFlaged || SendErrorFlaged) ? TRUE : FALSE;

    if (ErrFlag != BlinkErrorLed) {
        bmSignalErrorLed (LED_BLINK_CODE3, ErrFlag);
    }
    BlinkErrorLed = ErrFlag;

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Setup CAN acceptance filters
 *
 *      Sets up the CAN acceptance filters and returns them in the given
 *      Filters table. Size determines the max. number of entries in the
 *      Filters table. Returns the number of filter entries or an error
 *      code, if the buffer is too small to hold all filters.
 *
 *  \oparam  Filters = Buffer to return acceptance filters
 *  \iparam  Size    = Size of acceptance filter buffer
 *
 *  \return  Number of filters or (negative) error code
 *
 ****************************************************************************/

Error_t canSetupAcceptFilters (CanIdFilter_t *Filters, UInt32 Size) {

    NodeAddress = bmGetNodeAddress();

    if (Size >= 3) {

        // Enable all messages from the master
        Filters[0].Mask  = CANiD_MASK_ADDRESS | CANiD_MASK_MASTER;
        Filters[0].CanID = NodeAddress | CANiD_MASK_MASTER;

        // Enable remote request messages
        Filters[1].Mask  = CANiD_MASK_ADDRESS | CANiD_MASK_MASTER;
        Filters[1].CanID = NodeAddress;

        // Enable all broadcast messages
        Filters[2].Mask  = CANiD_MASK_ADDRESS;
        Filters[2].CanID = 0;

        return (3);
    }
    return (E_PARAMETER_OUT_OF_RANGE);
}


/*****************************************************************************/
/*!
 *  \brief   Initializes this module
 *
 *      Initializes this module by allocating the send and receive queues,
 *      opening the CAN controller and registering the interrupt handlers
 *      on the HAL. Receive interrupts are enabled.
 *
 *      This function is called once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t canInitializeLayer (void) {

    CanIdFilter_t Filters[10];
    Error_t Status;
    Int32 Count;

    if ((Count = canSetupAcceptFilters (Filters, ELEMENTS(Filters))) < 0) {
        return (CanHandle);
    }
    if ((CanHandle = halCanOpen(HAL_CAN_SYSTEM, 0, canHandleInterrupts)) < 0) {
        return (CanHandle);
    }
    if ((Status = halCanSetup(CanHandle, Filters, Count)) < 0) {
        return (Status);
    }
    if ((Status = canCreateQueue(&SendQueue, CAN_SEND_QUEUE_SIZE)) < 0) {
        return (Status);
    }    
    if ((Status = canCreateQueue (&RecvQueue, CAN_RECV_QUEUE_SIZE)) < 0) {
        return (Status);
    }
    halCanControl (CanHandle, CAN_INTR_RxREADY, ON);

    return (NO_ERROR);
}

//****************************************************************************/
