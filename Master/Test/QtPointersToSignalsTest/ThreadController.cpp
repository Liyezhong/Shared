/****************************************************************************/
/*! \file ThreadController.cpp
 *
 *  \brief Implementation file for class ThreadController.
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

#include <Threads/Include/ThreadController.h>

#include <iostream>

namespace Threads {

/****************************************************************************/
ThreadController::ThreadController(tSourceType LoggingSource) :
    DataLogging::LoggingObject(NULL, LoggingSource)
{
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
}

/****************************************************************************/
ThreadController::~ThreadController() {
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
}

/****************************************************************************/
void ThreadController::RegisterForDirectEventLogging(QObject *pObject) {
    // check for NULL pointer
    if(pObject == NULL) {
        THROWARGS(Global::EVENT_GLOBAL_ERROR_NULL_POINTER,
                  QStringList() << "pObject" << __FILE__ << QString::number(__LINE__, 10));
    }
    // connect EmitEventEntry
    if(!QObject::connect(pObject, SIGNAL(EmitEventEntry(const EventEntry &)),
                         this, SIGNAL(EmitEventEntry(const EventEntry &))))
    {
        THROWARGS(Global::EVENT_GLOBAL_ERROR_SIGNAL_SIGNAL_CONNECT,
                  QStringList() << "pObject" << "EmitEventEntry()" <<
                  "this" << "EmitEventEntry()");
    }
}

/****************************************************************************/
void ThreadController::RegisterForProcessedEventLogging(QObject *pObject) {
    // check for NULL pointer
    if(pObject == NULL) {
        THROWARGS(Global::EVENT_GLOBAL_ERROR_NULL_POINTER,
                  QStringList() << "pObject" << __FILE__ << QString::number(__LINE__, 10));
    }
    // connect EmitEventEntry
    if(!QObject::connect(pObject, SIGNAL(EmitEventEntry(const EventEntry &)),
                         this, SLOT(ReceiveEventEntry(const EventEntry &))))
    {
        THROWARGS(Global::EVENT_GLOBAL_ERROR_SIGNAL_SIGNAL_CONNECT,
                  QStringList() << "pObject" << "EmitEventEntry()" <<
                  "this" << "ReceiveEventEntry()");
    }
}

/****************************************************************************/
void ThreadController::RegisterForDirectDayOperationLogging(QObject *pObject) {
    // check for NULL pointer
    if(pObject == NULL) {
        THROWARGS(Global::EVENT_GLOBAL_ERROR_NULL_POINTER,
                  QStringList() << "pObject" << __FILE__ << QString::number(__LINE__, 10));
    }
    // connect EmitDayOperationEntry
    if(!QObject::connect(pObject, SIGNAL(EmitDayOperationEntry(const DayOperationEntry &)),
                         this, SIGNAL(EmitDayOperationEntry(const DayOperationEntry &))))
    {
        THROWARGS(Global::EVENT_GLOBAL_ERROR_SIGNAL_SIGNAL_CONNECT,
                  QStringList() << "pObject" << "EmitDayOperationEntry()" <<
                  "this" << "EmitDayOperationEntry()");
    }
}

/****************************************************************************/
void ThreadController::RegisterForDirectComponentTestLogging(QObject *pObject) {
    // check for NULL pointer
    if(pObject == NULL) {
        THROWARGS(Global::EVENT_GLOBAL_ERROR_NULL_POINTER,
                  QStringList() << "pObject" << __FILE__ << QString::number(__LINE__, 10));
    }
    // connect EmitComponentTestEntry
    if(!QObject::connect(pObject, SIGNAL(EmitComponentTestEntry(const ComponentTestEntry &)),
                         this, SIGNAL(EmitComponentTestEntry(const ComponentTestEntry &))))
    {
        THROWARGS(Global::EVENT_GLOBAL_ERROR_SIGNAL_SIGNAL_CONNECT,
                  QStringList() << "pObject" << "EmitComponentTestEntry()" <<
                  "this" << "EmitComponentTestEntry()");
    }
}

/****************************************************************************/
void ThreadController::Go() {
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
    // call processing method
    OnGoReceived();
}

/****************************************************************************/
void ThreadController::Stop() {
    std::cout << thread()->currentThreadId() << " -> " << __PRETTY_FUNCTION__ <<  std::endl;
    // call processing method
    OnStopReceived();
}

/****************************************************************************/
tSourceType ThreadController::GetLoggingSource()
{
	return DataLogging::LoggingObject::GetLoggingSource();
}

} // namespace Threads
