/****************************************************************************/
/*! \file EventHandler.cpp
 * 
 *  \brief Frontend for Event processing and logging.
 *
 *         All events shall be sent to this Module. It will filter an
 *         incoming event according to its type and forward the event
 *         to Error Handler / Data Logger.
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


#include <EventHandler/Include/EventHandler.h>
#include <EventHandler/Include/EventDefsReader.h>

#include <iostream>

using namespace DataLogging;
using namespace Global;

// ======================================================================== //
/*
   WARNING: it is usually good to avoid global variables, but the problem is
   that moc is using full name for generated signals and it does not seem to
   be able to work with namespaces in this case. So these variables are
   defined globally outside the class, just as signals are to be able to use
   function pointers (if defined as members, the system will not compile):
*/
typedef void (EventHandler::EventHandlerThreadController::*pToSignal)(const ActionEntry &);

// Hash of connected Signals (a pair of LoggingSource and pointer to connected Signal):
QHash<tSourceType, pToSignal> g_pMyConnectedSignals;

// List of pointers to all available signals:
QList<pToSignal> g_pListOfMySignals;

// ======================================================================== //


namespace EventHandler {

/****************************************************************************/
/*!
 *  \brief    Constructor
 *
 *  \return
 *
 ****************************************************************************/
EventHandlerThreadController::EventHandlerThreadController(tSourceType LoggingSource) :
						Threads::ThreadController(LoggingSource)
{
    qDebug() << (QString)(QString::number(thread()->currentThreadId(), 10) + " -> " + __PRETTY_FUNCTION__);
    qRegisterMetaType<ActionEntry>("ActionEntry");
    initActionLists();

}

EventHandlerThreadController::~EventHandlerThreadController()
{
	qDebug() << (QString)(QString::number(thread()->currentThreadId(), 10) + " -> " + __PRETTY_FUNCTION__);

    // delete all instantiated EventEntry objects!
    QList<EventEntry*> values = m_activeEvents.values();
    for (int i = 0; i < values.size(); i++) {
        if (values[i] != NULL) {
            delete values[i];
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    API function to call before EventHandler can be used
 *
 *      This function does everything to initialize the EventHandler. It
 *      shall be called before EventHandler can be used.
 *
 *
 *  \return   true if success, false if error
 *
 ****************************************************************************/
bool EventHandlerThreadController::CreateAndInitializeObjects()
{
    qDebug() << (QString)(QString::number(thread()->currentThreadId(), 10) + " -> " + __PRETTY_FUNCTION__);

    m_activeEvents.clear();
    m_eventsAndTypes.clear();

    EventDefsReader edr(this);
	EventDefsReaderErrorType err = edr.loadEvents(&m_eventsAndTypes);

	if (err != ALL_OK) {
		// TODO: emit my own error here!
		qDebug() << (QString)("EVNTH: Cannot load events. Error: " + QString::number(err, 10));
		return false;
	}
/*
    QHash<quint32, QString>::iterator itrt;
    for (itrt = m_eventsAndTypes.begin(); itrt != m_eventsAndTypes.end(); itrt++) {
        QString code = QString::number(itrt.key(), 16);
        //qDebug() << (QString)(" Key: " + code + " Type: " + (QString)itrt.value());
	}
*/
    qDebug() << (QString)("EventHandler init OK.");
	return true;
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
void EventHandlerThreadController::ProcessEvent(const EventEntry &TheEventEntry)
{
    qDebug() << (QString)(QString::number(thread()->currentThreadId(), 10) + " -> " + __PRETTY_FUNCTION__);

    // check if this event is known to the system:
	if (!m_eventsAndTypes.contains(TheEventEntry.GetEventCode())) {
		// TODO: what do we do if event is unknown?
		// so far no action follows, it will be just logged:
		// TODO: add interface to set event type:
		//TheEventEntry.SetEventType(EVTTYPE_UNKNOWN);
		forwardToDataLogger(TheEventEntry);
		return;
	}

	// check if this event needs to be forwarded to Error Handler
	EventType gtype = TheEventEntry.GetEventType();
	if ((gtype == EVTTYPE_FATAL_ERROR) ||
		(gtype == EVTTYPE_ERROR) ||
		(gtype == EVTTYPE_WARNING)) {
		// forward to the error processor
		forwardToErrorHandler(TheEventEntry);
	}

	// TODO: what do we do with UNDEFINED events? they are
	// forwarded to DataLogger without any action so far.

	// forward to data logger
	forwardToDataLogger(TheEventEntry);
}

void EventHandlerThreadController::CleanupAndDestroyObjects()
{
    qDebug() << (QString)(QString::number(thread()->currentThreadId(), 10) + " -> " + __PRETTY_FUNCTION__);
}

void EventHandlerThreadController::OnGoReceived()
{
    qDebug() << (QString)(QString::number(thread()->currentThreadId(), 10) + " -> " + __PRETTY_FUNCTION__);
    //TODO:
    SendTestAction();
}

void EventHandlerThreadController::OnStopReceived()
{
    qDebug() << (QString)(QString::number(thread()->currentThreadId(), 10) + " -> " + __PRETTY_FUNCTION__);
}

void EventHandlerThreadController::ReceiveEventEntry(const EventEntry & /*TheEventEntry*/)
{
    qDebug() << (QString)(QString::number(thread()->currentThreadId(), 10) + " -> " + __PRETTY_FUNCTION__);
}

/****************************************************************************/
/*!
 *  \brief    Get a pointer to an action distribution signal.
 *
 *      This function checks internal list of signal pointers and returns
 *      the next free signal pointer. It also updates the internal hash of
 *      LoggingSource / Signal pairs.
 *
 *  \param    LoggingSource = logging source to which returned Signal
 *                            will be connected
 *
 *  \return   success: pointer to Signal name (NOT to Signal as function!)
 *            failure: NULL
 *
 ****************************************************************************/
const char * EventHandlerThreadController::FetchActionSignal(tSourceType LoggingSource)
{
	// check if there are any signals left:
	if (m_freeActionsList.isEmpty()) {
		// TODO: fatal error -> no available signals anymore
		return NULL;
	}
	// fetch available signal:
	const char *mysignal = m_freeActionsList[0];
	if (mysignal == NULL) {
		// TODO: fatal error -> null in hash
		return NULL;
	}

	// update action tracking lists:
	m_takenActionsList.insert(m_takenActionsList.size(), mysignal);
	m_freeActionsList.removeAt(0);

	quint16 i = m_takenActionsList.size() - 1;
	if (g_pListOfMySignals[i] == NULL) {
		// TODO: fatal error -> null in hash
		return NULL;
	}
	// This hash contains a mapping of outgoing signals to Logging Sources.
	// With this Hash, the ActionSender shall find signal corresponding to
	// particular Logging Source
	g_pMyConnectedSignals.insert(LoggingSource, g_pListOfMySignals[i]);

	qDebug() << (QString)("EVNTH: Fetched Signal: " + QString::number(m_takenActionsList.size(), 10) +
	                      " for LoggingSource: " + QString::number(LoggingSource, 10));

	return mysignal;
}

/****************************************************************************/
/*!
 *  \brief    Initialize action signal structures.
 *
 *      This function initializes two lists:
 *      - list of pointers to signal names/signatures, needed to be able to
 *        connect signals to slots;
 *      - list of function pointers to signals, needed to be able to emit a
 *        signal.
 *
  *  \return
 *
 ****************************************************************************/
void EventHandlerThreadController::initActionLists()
{
	// insert ALL existing action signals:
	m_freeActionsList.clear();
    m_freeActionsList.insert(m_freeActionsList.size(), SIGNAL(sendAction_1(const ActionEntry &)));
    m_freeActionsList.insert(m_freeActionsList.size(), SIGNAL(sendAction_2(const ActionEntry &)));
    m_freeActionsList.insert(m_freeActionsList.size(), SIGNAL(sendAction_3(const ActionEntry &)));
    m_freeActionsList.insert(m_freeActionsList.size(), SIGNAL(sendAction_4(const ActionEntry &)));
    m_freeActionsList.insert(m_freeActionsList.size(), SIGNAL(sendAction_5(const ActionEntry &)));
    m_freeActionsList.insert(m_freeActionsList.size(), SIGNAL(sendAction_6(const ActionEntry &)));
    m_freeActionsList.insert(m_freeActionsList.size(), SIGNAL(sendAction_7(const ActionEntry &)));
    m_freeActionsList.insert(m_freeActionsList.size(), SIGNAL(sendAction_8(const ActionEntry &)));
    m_freeActionsList.insert(m_freeActionsList.size(), SIGNAL(sendAction_9(const ActionEntry &)));

    g_pListOfMySignals.insert(g_pListOfMySignals.size(), &EventHandlerThreadController::sendAction_1);
    g_pListOfMySignals.insert(g_pListOfMySignals.size(), &EventHandlerThreadController::sendAction_2);
    g_pListOfMySignals.insert(g_pListOfMySignals.size(), &EventHandlerThreadController::sendAction_3);
    g_pListOfMySignals.insert(g_pListOfMySignals.size(), &EventHandlerThreadController::sendAction_4);
    g_pListOfMySignals.insert(g_pListOfMySignals.size(), &EventHandlerThreadController::sendAction_5);
    g_pListOfMySignals.insert(g_pListOfMySignals.size(), &EventHandlerThreadController::sendAction_6);
    g_pListOfMySignals.insert(g_pListOfMySignals.size(), &EventHandlerThreadController::sendAction_7);
    g_pListOfMySignals.insert(g_pListOfMySignals.size(), &EventHandlerThreadController::sendAction_8);
    g_pListOfMySignals.insert(g_pListOfMySignals.size(), &EventHandlerThreadController::sendAction_9);

    // clean the list of taken actions:
    m_takenActionsList.clear();
}

void EventHandlerThreadController::ReceiveAction(const ActionEntry &)
{
	qDebug() << (QString)("EVNTH: " + (QString)__PRETTY_FUNCTION__);
}

/****************************************************************************/
/*!
 *  \brief    Test signal pointers.
 *
 *      This is just a test function for checking if pointer-based
 *      signal/slot connections work (and they do).  :)
 *
  *  \return
 *
 ****************************************************************************/
void EventHandlerThreadController::SendTestAction()
{
	ActionEntry action(0, ACTTYPE_CONTINUE, ACTION_DEBUG);

    for (int i = 0; i < 32; i++) {
    	if (g_pMyConnectedSignals.contains(i)) {
    		if (g_pMyConnectedSignals.value(i) != NULL) {
    			// fetch pointer to signal as function...
    			pToSignal mfp = g_pMyConnectedSignals.value(i);
    			// ...and emit it:
				emit (this->*mfp)(action);
    		}
    		else {
    			// ERROR!
    			qDebug() << "EVNTH: Null in Hash!";
    		}
    	}
    }

    qDebug() << "\nEVNTH: Test signals sent!\n";
}

} // namespace EventHandler
