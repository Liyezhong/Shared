/****************************************************************************/
/*! \file ErrorHandler.h
 *
 *  \brief Base class of the Error Processing Module.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Y.Novak
 *
 *  \b Description:
 *
 *       This Module shall be used to process all events of type
 *         - Fatal Error
 *         - Temporary Error
 *         - Warning
 *
 *       This Module keeps track of all occurring error conditions (e.g.
 *       error occurred or error was fixed) and triggers appropriate actions.
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

#ifndef EVENTHANDLER_ERRORHANDLER_H
#define EVENTHANDLER_ERRORHANDLER_H

#include <DataLogging/Include/LoggingObject.h>
#include <DataLogging/Include/EventEntry.h>

namespace EventHandler {

/****************************************************************************/
/**
 * \brief This is a base class for a project specific ErrorHandler.
 *
 * In the current system, ErrorHandler object lives in a context of the
 * EventHandlerThreadController. It needs to be attached to the
 * EventHandlerThreadController after initialization (it is done via API of
 * the MasterThreadController platform class).
 *
 * This class has an overview of the whole system's configuration and status.
 * It keeps track of all existing conditions and events. For every error
 * reported by any of ThreadControllers it will generate appropriate
 * Action(s) to put the system into corresponding state.
 * Details are project-specific.
 *
 * \warning  Every derived class must implement the ReceiveEvent slot!
 */
/****************************************************************************/
class ErrorHandler : public DataLogging::LoggingObject
{
    Q_OBJECT

public:

    ErrorHandler(LoggingObject *pParent, const Global::LoggingSource & source);
    ~ErrorHandler();

private:

    ErrorHandler();                        ///< Not implemented
    ErrorHandler(const LoggingObject &);   ///< Not implemented.

signals:
    /******************************************************************************/
    /*!
    *  \brief    This signal is emitted to send system error to Leica
    *            Remote Care Server.
    *
    *  \param    EventEntry = the event (error) object
    *
    *******************************************************************************/
    void SendErrorToRemoteCare(const DataLogging::EventEntry &);

public slots:

    /******************************************************************************/
    /*!
    *  \brief    This slot is used to forward events from EventHandler to
    *            ErrorHandler and must be implemented by the derived class.
    *
    *  \param    EventEntry = event object
    *
    *******************************************************************************/
    virtual void ReceiveEvent(const DataLogging::EventEntry &) = 0;

};

} // end namespace EventHandler

#endif // EVENTHANDLER_ERRORHANDLER_H
