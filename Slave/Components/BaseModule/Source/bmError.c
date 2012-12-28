/****************************************************************************/
/*! \file bmError.c
 *
 *  \brief Error Handling Functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all error/event handling functions. This
 *       includes sending of errors/events to the master via CAN and
 *       caching errors/events in those cases, where they can not be
 *       send immediately.
 *
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

#include "Global.h"
#include "halLayer.h"
#include "bmCommon.h"
#include "bmUtilities.h"
#include "bmTime.h"
#include "bmCan.h"
#include "bmSystem.h"
#include "bmStrings.h"
#include "bmDebug.h"
#include "bmError.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define ERROR_CACHE_SIZE    32      //!< Size of event/error queue

//! Macro to extract event class from error code
#define GET_ERROR_CLASS(e)  (((e) & ERRCODE_MASK_CLASS)  >> 28)

//! Macro to extract module ID from error code
#define GET_ERROR_MODULE(e) (((e) & ERRCODE_MASK_MODULE) >> 16)

//! Macro to extract event number from error code
#define GET_ERROR_NUMBER(e) ((e) & ERRCODE_MASK_NUMBER)


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Event status structure (used to cache events in a queue)
typedef struct {
   Error_t ErrCode;                 //!< Error/event code
   UInt32  ErrInfo;                 //!< Additional info
   UInt16  Channel;                 //!< Channel this error belongs to
} bmEventStatus_t;

//! Event cache queue structure (used to cache events in a queue)
typedef struct {
   UInt16 InCount;                  //!< Number of events in cache
   UInt16 NextIn;                   //!< Next-event-in index
   UInt16 NextOut;                  //!< Next-event-out index
   UInt16 LostCount;                //!< Number of lost events
   bmEventStatus_t Events[ERROR_CACHE_SIZE]; //!< Event buffer
} bmEventCache_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

static bmEventCache_t Cache = {0};   //!< Event cache data


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t bmEnqueEvent (
    UInt16 Channel, Error_t ErrCode, UInt32 ErrInfo);


/*****************************************************************************/
/*!
 *  \brief   Signal error to the master
 *
 *      Sends an event message via CAN to the master. Extracts the event
 *      class (error, warning, info, fatal) from the error code and uses
 *      it to determine the CAN ID to be used. The error message is send
 *      in the "name" of the module with the supplied ChannelID. A time-
 *      stamp is added to the message (the millicesond of the day).
 *
 *      If the node is in standby/shutdown, the event will be ignored.
 *      If the node is in master/slave configuration state, events will
 *      be cached in a queue. When node changes to normal state, all
 *      cached events are send to the master.
 *
 *  \iparam  Channel  = Channel ID
 *  \iparam  ErrCode  = Error code
 *  \iparam  ErrState = Error state (TRUE: active, FALSE: resolved)
 *  \iparam  ErrInfo  = Additional info
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmSignalEvent (UInt16 Channel, Error_t ErrCode, Bool ErrState, UInt32 ErrInfo) {

    bmNodeState_t NodeState = bmGetNodeState();

    dbgPrintError (Channel, ErrCode, ErrState, ErrInfo);

    if (NodeState < NODE_STATE_STANDBY) {
        if (NodeState > NODE_STATE_IDENTIFY) {
            return (bmTransmitEvent (Channel, ErrCode, ErrInfo));
        }
        else {
            return (bmEnqueEvent (Channel, ErrCode, ErrInfo));
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Send error/event to the master
 *
 *      Sends an error message via CAN to the master. Extracts the error
 *      class (error, warning, info, fatal) from the error code and uses
 *      it to determine the CAN ID to be used. The error message is send
 *      in the "name" of the module with the supplied ChannelID. A time-
 *      stamp is added to the message (the millicesond of the day).
 *
 *  \iparam  Channel   = Channel ID
 *  \iparam  ErrCode   = Error code
 *  \iparam  ErrInfo   = Additional info
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmTransmitEvent (UInt16 Channel, Error_t ErrCode, UInt32 ErrInfo) {

    static const UInt32 EventID[8] = {
        MSG_EVENT_INFO, MSG_EVENT_ERROR, MSG_EVENT_WARNING, MSG_EVENT_FATAL
    };
    CanMessage_t Message;

    if (ErrCode != NO_ERROR) {
        Message.CanID = EventID[GET_ERROR_CLASS(ErrCode)];
        Message.Length = 6;

        bmSetMessageItem (&Message, GET_ERROR_MODULE(ErrCode), 0, 2);
        bmSetMessageItem (&Message, GET_ERROR_NUMBER(ErrCode), 2, 2);
        bmSetMessageItem (&Message, ErrInfo, 4, 2);

        if (canWriteMessage(Channel, &Message) < 0) {
            return (E_ERROR_MESSAGE_LOST);
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Cache error or event
 *
 *      Stores a given error or event in the error queue for later
 *      transmission. If the event queue is already full, the global
 *      errLostCount variable is incremented and an error returned.
 *
 *      The purpose of enqueing events is, that sending of events
 *      before finishing master/slave configuration is forbidden.
 *      The enqueued event will be send, when node state changes
 *      to normal. This is done in bmProcessEventCache().
 *
 *  \iparam  Channel   = Channel ID
 *  \iparam  ErrCode   = Error code
 *  \iparam  ErrInfo   = Additional info
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmEnqueEvent (UInt16 Channel, Error_t ErrCode, UInt32 ErrInfo) {

    if (Cache.InCount < ERROR_CACHE_SIZE) {

        bmEventStatus_t *Event = &Cache.Events[Cache.NextIn];

        Event->ErrCode = ErrCode;
        Event->ErrInfo = ErrInfo;
        Event->Channel = Channel;

        if (++Cache.NextIn >= ERROR_CACHE_SIZE) {
            Cache.NextIn = 0;
        }
        Cache.InCount++;
        return (NO_ERROR);
    }
    Cache.LostCount++;

    return (E_ERROR_MESSAGE_LOST);
}


/*****************************************************************************/
/*!
 *  \brief   Process cached errors/events
 *
 *      Checks if node has left master/slave configuration phase and, if
 *      this is the case, sends all events from the event queue to the
 *      master. If the global var errLostCount is greater than zero, an
 *      additional E_ERROR_MESSAGE_LOST is send, using the number of lost
 *      errors as additional info.
 *
 *      On each call of this function only one single event from the queue
 *      is send and removed from the queue. This prohibits an overflow in
 *      the CAN send queue, by given the CAN controller enough time to
 *      transmit the message.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmProcessEventCache (void) {

    bmNodeState_t NodeState = bmGetNodeState();

    if (NodeState > NODE_STATE_CONFIGURE) {

        // clear cache if node is in shutdown/sleeping state
        if (NodeState >= NODE_STATE_SHUTDOWN) {
            Cache.InCount = Cache.NextIn = Cache.NextOut = Cache.LostCount = 0;
        }
        // if errors in cache, dequeue one and send it to master
        else if (Cache.InCount) {
            bmEventStatus_t *Event = &Cache.Events[Cache.NextOut];

            bmTransmitEvent (Event->Channel, Event->ErrCode, Event->ErrInfo);

            if (++Cache.NextOut >= ERROR_CACHE_SIZE) {
                Cache.NextOut = 0;
            }
            Cache.InCount--;
        }
        // check if errors lost and send notification to master
        else if (Cache.LostCount) {
            bmTransmitEvent (BASEMODULE_CHANNEL, E_ERROR_MESSAGE_LOST, Cache.LostCount);

            Cache.LostCount = 0;
        }
    }
    return (NO_ERROR);
}

//****************************************************************************/

