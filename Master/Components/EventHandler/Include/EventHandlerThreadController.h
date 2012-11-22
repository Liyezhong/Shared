/****************************************************************************/
/*! \file EventHandlerThreadController.h
 *
 *  \brief Frontend for Event processing and logging.
 *
 *         All events shall be sent to this class. It will filter an
 *         incoming event according to its type and forward the event
 *         Error Handler / Data Logger.
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

#ifndef EVENTHANDLER_EVENTHANDLERTHREADCONTROLLER_H
#define EVENTHANDLER_EVENTHANDLERTHREADCONTROLLER_H

#include <Global/Include/GlobalDefines.h>
#include <DataLogging/Include/EventEntry.h>
#include <Threads/Include/ThreadController.h>

namespace EventHandler {

/****************************************************************************/
/**
 * \brief This is a system level EventHandler.
 *
 * All system level events (e.g. NOT local component events) from all
 * ThreadControllers are sent to this (and only to this!) class. EventHandler
 * will forward events to DataLogger and, if needed, to ErrorHandler.
 */
/****************************************************************************/
class EventHandlerThreadController : public Threads::ThreadController {
    Q_OBJECT

public:

    EventHandlerThreadController(Global::gSourceType TheLoggingSource);
    virtual ~EventHandlerThreadController();
    virtual void CreateAndInitializeObjects();
    virtual void CleanupAndDestroyObjects();

protected:

    void OnGoReceived();
    void OnStopReceived();
    virtual void OnPowerFail();

private:

    EventHandlerThreadController();                                          ///< Not implemented.
    // copy constructor & "=" operator
    EventHandlerThreadController(const EventHandlerThreadController &);                      ///< Not implemented.
    const EventHandlerThreadController & operator = (const EventHandlerThreadController &);  ///< Not implemented.

    /****************************************************************************/
    /**
     * \brief Process error event from local object.
     *
     * See detailed description in the base class' header.
     *
     * \param[in]  TheEventEntry = event from one of local objects
     * \return      true if event was processed.
     */
    /****************************************************************************/
    bool LocalProcessErrorEvent(const DataLogging::EventEntry & TheEventEntry);
signals:
    /******************************************************************************/
    /*!
    *  \brief    This signal is emitted to forward an incoming event to
    *            the ErrorHandler.
    *
    *  \param    TheEventEntry  = event object
    *
    *******************************************************************************/
    void ForwardToErrorHandler(const DataLogging::EventEntry &);

    /******************************************************************************/
    /*!
    *  \brief    This signal is emitted to forward a system error to
    *            the RemoteCare Server.
    *
    *  \param    TheEventEntry  = event object
    *
    *******************************************************************************/
    void ForwardToRemoteCare(const DataLogging::EventEntry &);


public slots:

    void ProcessEvent(const DataLogging::EventEntry &TheEventEntry);

};

} // end namespace EventHandler

#endif // EVENTHANDLER_EVENTHANDLERTHREADCONTROLLER_H
