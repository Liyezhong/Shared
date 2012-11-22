/****************************************************************************/
/*! \file EventHandler.h
 * 
 *  \brief Frontend for Event processing and logging.
 *
 *         All events shall be sent to this class. It will filter an
 *         incoming event according to its type and forward the event
 *         Error Handler / Data Logger.
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

#ifndef _EVENTHANDLER_H_
#define _EVENTHANDLER_H_

#include <QtCore>

#include <Global/Include/GlobalDefines.h>
#include <DataLogging/Include/EventEntry.h>
#include <Threads/Include/ThreadController.h>
#include <EventHandler/Include/ActionEntry.h>

using namespace DataLogging;
//class EventHandlerThreadController;

namespace EventHandler {

//TODO:
//typedef void (EventHandlerThreadController::*pToReceiveAction)(const ActionEntry &);


class EventHandlerThreadController : public Threads::ThreadController {
    Q_OBJECT

//typedef void (*pToReceiveAction)(const ActionEntry &);

public:

    EventHandlerThreadController(tSourceType LoggingSource);
    virtual ~EventHandlerThreadController();
    bool CreateAndInitializeObjects();
    void CleanupAndDestroyObjects();
    const char * FetchActionSignal(tSourceType LoggingSource);

protected:

    void OnGoReceived();
    void OnStopReceived();

private:

    EventHandlerThreadController();                                          ///< Not implemented.
	// copy constructor & "=" operator
    EventHandlerThreadController(const EventHandlerThreadController &);                      ///< Not implemented.
    const EventHandlerThreadController & operator = (const EventHandlerThreadController &);  ///< Not implemented.
    void initActionLists();

signals:
	/******************************************************************************/
	/*!
	*  \brief    This signal is emitted to forward an incoming event to
	*            the DataLogger.
	*
	*  \param    TheEventEntry  = The event
	*
	*******************************************************************************/
	void forwardToDataLogger(const EventEntry &TheEventEntry);

	/******************************************************************************/
	/*!
	*  \brief    This signal is emitted to forward an incoming event to
	*            the ErrorHandler.
	*
	*  \param    TheEventEntry  = The event
	*
	*******************************************************************************/
	void forwardToErrorHandler(const EventEntry &TheEventEntry);

	// WARNING: extend Nof these signals if you need more receivers!
	void sendAction_1(const ActionEntry &);
	void sendAction_2(const ActionEntry &);
	void sendAction_3(const ActionEntry &);
	void sendAction_4(const ActionEntry &);
	void sendAction_5(const ActionEntry &);
	void sendAction_6(const ActionEntry &);
	void sendAction_7(const ActionEntry &);
	void sendAction_8(const ActionEntry &);
	void sendAction_9(const ActionEntry &);


public slots:

    void ProcessEvent(const EventEntry &TheEventEntry);
    void ReceiveEventEntry(const EventEntry &TheEventEntry);
    /****************************************************************************/
    /**
     * \brief API for receiving global system action.
     *
     *        See detailed description in the Base Class's header.
     *
     * \param[in]  ActionEntry = action from the system event handler
     */
    /****************************************************************************/
    void ReceiveAction(const ActionEntry &);

    //TODO: remove this test slot
    void SendTestAction();


private:

    /*! Hash of Events with their corresponding event type
        Format: <event code, event type> */
    QHash<quint32, QString> m_eventsAndTypes;

    /*! Hash of currently active Events
        Format: <event source + event code, pointer to event object> */
    QHash<quint32, EventEntry*> m_activeEvents;

    /*! Hash of system's action Receivers
        Format: <logging source, pointer to EventHandler's Signal> */
    QHash<tSourceType, const char *> m_actionReceivers;

    /*! TODO */
    QList<const char *> m_freeActionsList;
    QList<const char *> m_takenActionsList;
    //QList<pToReceiveAction> m_pToSignals;

};

}; // namespace EventHandler

#endif // _EVENTHANDLER_H_
