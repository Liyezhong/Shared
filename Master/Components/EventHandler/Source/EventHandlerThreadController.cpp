/****************************************************************************/
/*! \file EventHandler.cpp
 *
 *  \brief Frontend for Event processing and logging.
 *
 *         All events shall be sent to this Module. It will filter an
 *         incoming event according to its type and forward the event
 *         to Error Handler / Data Logger.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Y.Novak
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
//#define MY_EVNT_DEBUG


#include <EventHandler/Include/EventHandlerThreadController.h>
#include <Global/Include/Utils.h>

#include <QMetaType>
#include <QThread>
#include <QDebug>

/****************************************************************************/
/**
 * \brief Namespace containing all classes for system event handling.
 */
/****************************************************************************/
namespace EventHandler {

/****************************************************************************/
/*!
 *  \brief    Constructor for the EventHandlerThreadController
 *
 *  \return
 *
 ****************************************************************************/
EventHandlerThreadController::EventHandlerThreadController(Global::gSourceType TheLoggingSource) :
    Threads::ThreadController(TheLoggingSource, "EventHandler")
{
    // Register LoggingSource types with moc
    qRegisterMetaType<Global::LoggingSource>("Global::LoggingSource");

}

/****************************************************************************/
/*!
 *  \brief    Destructor for the EventHandlerThreadController
 *
 *  \return
 *
 ****************************************************************************/
EventHandlerThreadController::~EventHandlerThreadController()
{
}

/****************************************************************************/
/*!
 *  \brief    API function to call before EventHandler can be used
 *
 *      This function does everything to initialize the EventHandler. It
 *      shall be called before EventHandler can be used.
 *
 ****************************************************************************/
void EventHandlerThreadController::CreateAndInitializeObjects()
{
}

/****************************************************************************/
/*!
 *  \brief    This SLOT handles all incoming events.
 *
 *      This SLOT shall be used to report all events and their current
 *      status. The status might be of two types:
 *        - event active
 *        - event closed (e.g. condition which caused event is not
 *          present anymore)
 *
 *      This function forwards the event to ErrorHandler / DataLogger
 *      according to the event type.
 *
 *  \param    TheEventEntry = reference to incoming event
 *
 *  \return
 *
 ****************************************************************************/
void EventHandlerThreadController::ProcessEvent(const DataLogging::EventEntry &TheEventEntry)
{
    /// \todo forward to error handler only after runnning in own thread? (error handling during initialization)

    // check if this event needs to be forwarded to Error Handler
    Global::EventType gtype = TheEventEntry.GetEventType();
    if ((gtype == Global::EVTTYPE_FATAL_ERROR) ||
        (gtype == Global::EVTTYPE_ERROR) ||
        (gtype == Global::EVTTYPE_WARNING))
    {
        // forward to the error processor
        ForwardToErrorHandler(TheEventEntry);
    }

    /// \todo what do we do with UNDEFINED events?
}

/****************************************************************************/
/*!
 *  \brief Destroy all used objects and prepare for shutdown.
 *
 *         See detailed description in the Base Class's header
 *
 *  \return
 */
/****************************************************************************/
void EventHandlerThreadController::CleanupAndDestroyObjects()
{
}

/****************************************************************************/
/*!
 *  \brief This method is called when the base class receives Go signal.
 *
 *         See detailed description in the Base Class's header
 *
 *  \return
 */
/****************************************************************************/
void EventHandlerThreadController::OnGoReceived()
{
}

/****************************************************************************/
/*!
 *  \brief This method is called when the base class receives Stop signal.
 *
 *         See detailed description in the Base Class's header
 *
 *  \return
 */
/****************************************************************************/
void EventHandlerThreadController::OnStopReceived()
{
}

/****************************************************************************/
/**
 * \brief Power will fail shortly.
 *
 * Power will fail shortly.
 */
/****************************************************************************/
void EventHandlerThreadController::OnPowerFail()
{
    /// \todo implement
}

/****************************************************************************/
bool EventHandlerThreadController::LocalProcessErrorEvent(const DataLogging::EventEntry & /*TheEventEntry*/) {
    /// \todo implement
    return false;
}

} // end namespace EventHandler
