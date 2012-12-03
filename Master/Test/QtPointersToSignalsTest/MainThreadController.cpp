/****************************************************************************/
/*! \file MainThreadController.cpp
 * 
 *  \brief Implementation file for class MainThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
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

#include <Threads/Include/MainThreadController.h>
#include <Threads/Include/ThreadsEventCodes.h>
#include <Global/Include/Exception.h>
#include <QMetaType>
#include <QDateTime>

#include <iostream>

using namespace DataLogging;
using namespace Global;
using namespace EventHandler;

namespace Threads {

/****************************************************************************/
MainThreadController::MainThreadController(tSourceType LoggingSource,
                         const DataLoggingThreadController::LoggingSourceContainer & LoggingSourcesDataLogging,
                         tSourceType LoggingSourceEventHandler) :
    ThreadController(LoggingSource),
    m_EventLoggerMaxFileSize(0),
    m_DayOperationLoggerMaxFileAge(0),
    m_LoggingSourcesDataLogging(LoggingSourcesDataLogging),
    m_pDataLoggingThreadController(NULL),
    m_LoggingSourceEventHandler(LoggingSourceEventHandler),
    m_pEventThreadController(NULL)
{
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
}

/****************************************************************************/
MainThreadController::~MainThreadController() {
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
    m_pDataLoggingThreadController = NULL;
    m_pEventThreadController = NULL;
}

/****************************************************************************/
void MainThreadController::ConnectDataLoggingSignals(ThreadController *pController) {
    // connect EmitEventEntry -> to the event handler's slot
    if(!QObject::connect(pController, SIGNAL(EmitEventEntry(const EventEntry &)),
                         m_pEventThreadController, SLOT(ProcessEvent(const EventEntry &))))
    {
        THROWARGS(EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT,
                  QStringList() << "pController" << "EmitEventEntry()" <<
                  "m_pEventThreadController" << "ProcessEvent()");
    }

    // connect EmitDayOperationEntry -> m_pDataLoggingThreadController::SendToLoggerInstance(const DayOperationEntry &);
    if(!QObject::connect(pController, SIGNAL(EmitDayOperationEntry(const DayOperationEntry &)),
                         m_pDataLoggingThreadController, SLOT(SendToLoggerInstance(const DayOperationEntry &))))
    {
        THROWARGS(EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT,
                  QStringList() << "pController" << "EmitDayOperationEntry()" <<
                  "m_pDataLoggingThreadController" << "SendToLoggerInstance()");
    }
    // connect EmitComponentTestEntry -> m_pDataLoggingThreadController::SendToLoggerInstance(const ComponentTestEntry &);
    if(!QObject::connect(pController, SIGNAL(EmitComponentTestEntry(const ComponentTestEntry &)),
                         m_pDataLoggingThreadController, SLOT(SendToLoggerInstance(const ComponentTestEntry &))))
    {
        THROWARGS(EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT,
                  QStringList() << "pController" << "EmitComponentTestEntry()" <<
                  "m_pDataLoggingThreadController" << "SendToLoggerInstance()");
    }
}

/****************************************************************************/
void MainThreadController::AddAndConnectController(ThreadController *pController, QThread *pThread) {
    // connect some signals
    // connect Go and Stop signals
    if(!QObject::connect(this, SIGNAL(SendGo()), pController, SLOT(Go()))) {
        THROWARGS(EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT,
                  QStringList() << "MainThreadController" << "SendGo()" << "pController" << "Go()");
    }
    if(!QObject::connect(this, SIGNAL(SendStop()), pController, SLOT(Stop()))) {
        THROWARGS(EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT,
                  QStringList() << "MainThreadController" << "SendStop()" << "pController" << "Stop()");
    }

    // connect logging signals of controller for events, day operation and component tests
    ConnectDataLoggingSignals(pController);

    // TODO: test signal pointers
    const char * signal = m_pEventThreadController->FetchActionSignal(pController->GetLoggingSource());
    if (signal != NULL) {
		if(!QObject::connect(m_pEventThreadController,
							 signal,
							 pController,
							 SLOT(ReceiveAction(const ActionEntry &))))
		{
			THROWARGS(EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT,
					  QStringList() << "MainThreadController" << "SignalPointer()" << "pController" << "ReceiveAction()");
		}
    }
    else {
    	THROWARGS(EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT,
    			  QStringList() << "MainThreadController" << "SignalPointer()" << "pController" << "ReceiveAction()");
    }

    // add the pair to m_Controllers
    m_Controllers.push_back(tControllerPair(pController, pThread));
}

/****************************************************************************/
bool MainThreadController::CreateControllersAndThreads() {
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
    try {
        // now create new objects common to all main threads

        // Create the data logger controller and the data logger thread.
        // Remember pointer to data logging controller to do some automatic connecting.
        m_pDataLoggingThreadController = new DataLoggingThreadController(m_LoggingSourcesDataLogging);
        // configure it
        m_pDataLoggingThreadController->SetLogfilesPath(m_LogfilesPath);
        m_pDataLoggingThreadController->SetComponentTestPath(m_ComponentTestPath);

        // create system's event handler
        m_pEventThreadController = new EventHandlerThreadController(m_LoggingSourceEventHandler);

        // connect event handler's event forwarding signal with data logger's event slot.
        // all other connections are common among thread controllers and are handled
        // in the "AddAndConnectController" function.
        if(!QObject::connect(m_pEventThreadController, SIGNAL(forwardToDataLogger(const EventEntry &)),
                             m_pDataLoggingThreadController, SLOT(SendToLoggerInstance(const EventEntry &)))) {
            THROWARGS(EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT,
                      QStringList() << "m_pEventThreadController" << "forwardToDataLogger(..)" <<
                      "m_pDataLoggingThreadController" << "SendToLoggerInstance(..)");
        }

        // we have to connect out own logging signals to ensure we can send events.
        ConnectDataLoggingSignals(this);

        // Connecting m_pDataLoggingThreadController using AddAndConnectController ensures that
        // errors from it will be send to the error logger.
        // Sending day operation and component test entries lead to an additional call since they
        // will be send to itself and dispatched to the logger instances (which results in one additional
        // function call in the same thread.
        // On the other hand, using the same mechanism for *all* controllers, ensures that
        // if (for example) day operation entries should be send to more than one logger instance,
        // this can be implemnted easily.
        AddAndConnectController(m_pDataLoggingThreadController, new QThread());
        // connect all common signals and slots of the Event Handler and create its thread:
        AddAndConnectController(m_pEventThreadController, new QThread());
    } catch (const Exception &E) {
        // send error message
        SendEvent(E, Global::EVTTYPE_FATAL_ERROR, Global::EVTSTAT_ON);
        // and exit
        return false;
    } catch (const std::bad_alloc &) {
        // send error message
        SEND_FATAL_ERROR(Global::EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION, QStringList() << __FILE__ << QString::number(__LINE__, 10));
        // and exit
        return false;
    } catch (...) {
        // Send error message
        SEND_FATAL_ERROR(Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, QStringList() << __FILE__ << QString::number(__LINE__, 10));
        // and exit
        return false;
    }
    // everything OK
    return true;
}

/****************************************************************************/
void MainThreadController::DestroyControllersAndThreads() {
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
    for(tControllerVector::iterator it = m_Controllers.begin(); it != m_Controllers.end(); ++it) {
        // delete objects
        delete it->first;
        delete it->second;
    }
    m_Controllers.clear();
}

/****************************************************************************/
bool MainThreadController::InitializeControllers() {
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
    for(tControllerVector::iterator it = m_Controllers.begin(); it != m_Controllers.end(); ++it) {
        // Initialize controllers
        if(!it->first->CreateAndInitializeObjects()) {
            return false;
        }
    }
    return true;
}

/****************************************************************************/
void MainThreadController::CleanupControllers() {
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
    for(tControllerVector::iterator it = m_Controllers.end(); it != m_Controllers.begin();) {
        // go backwards
        --it;
        // cleanup object
        it->first->CleanupAndDestroyObjects();
    }
}

/****************************************************************************/
void MainThreadController::AttachControllersAndStartThreads() {
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
    for(tControllerVector::iterator it = m_Controllers.begin(); it != m_Controllers.end(); ++it) {
        // attach controller to thread
        it->first->moveToThread(it->second);
        // now start all threads
        it->second->start();
    }
    // and send them the Go signal
    emit SendGo();
}

/****************************************************************************/
void MainThreadController::StopThreadsAndWait() {
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
    for(tControllerVector::iterator it = m_Controllers.begin(); it != m_Controllers.end(); ++it) {
        // stop thread
        it->second->quit();
        // and wait for it
        it->second->wait();
    }
}

/****************************************************************************/
void MainThreadController::OnGoReceived() {
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
    // all objects created from now on, live in this thread so they are served by the
    // signal/slot mechanism in this event loop.
    try {
        // first of all create controllers and threads
        if(!CreateControllersAndThreads()) {
            // creation of controllers failed.
            // destroy already created controllers and threads
            DestroyControllersAndThreads();
            // throw exception
            THROW(EVENT_THREADS_ERROR_MAINTHREAD_STARTUP);
        }
        // Initialize controllers
        if(!InitializeControllers()) {
            // something went wrong. Cleanup
            CleanupControllers();
            // throw exception
            THROW(EVENT_THREADS_ERROR_MAINTHREAD_STARTUP);
        }

        // now start threads
        AttachControllersAndStartThreads();

    } catch (const Exception &E) {
        // destroy controllers and threads
        DestroyControllersAndThreads();
        // send error message
        SendEvent(E, Global::EVTTYPE_ERROR, Global::EVTSTAT_ON);
        // and request exit
        Terminate();
    } catch (const std::bad_alloc &) {
        // destroy controllers and threads
        DestroyControllersAndThreads();
        // send error message
        SEND_FATAL_ERROR(Global::EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION, QStringList() << __FILE__ << QString::number(__LINE__, 10));
        // and request exit
        Terminate();
    } catch (...) {
        // destroy controllers and threads
        DestroyControllersAndThreads();
        // Send error message
        SEND_ERROR(Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, QStringList() << __FILE__ << QString::number(__LINE__, 10));
        // and request exit
        Terminate();
    }
}

/****************************************************************************/
void MainThreadController::OnStopReceived() {
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;

    // Cleanup controllers
    CleanupControllers();

    // destroy controllers and threads
    DestroyControllersAndThreads();
}

/****************************************************************************/
void MainThreadController::Terminate() {
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
    // send Stop signal to all thread controllers
    emit SendStop();

    // stop all threads and wait for them
    StopThreadsAndWait();

    // call own Stop method
    Stop();

    // send request to finish main thread
    emit RequestFinish();
}

/****************************************************************************/
void MainThreadController::ReceiveAction(const ActionEntry &)
{
	qDebug() << (QString)("MAINCTL: " + (QString)__PRETTY_FUNCTION__);
}

} // namespace Threads
