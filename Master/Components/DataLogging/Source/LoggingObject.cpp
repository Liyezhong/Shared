/****************************************************************************/
/*! \file DataLogging/Source/LoggingObject.cpp
 *
 *  \brief Implementation file for class LoggingObject.
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

#include <DataLogging/Include/LoggingObject.h>
#include <DataLogging/Include/EventFilter.h>
#include <Global/Include/Utils.h>

#include <QMetaType>

namespace DataLogging {

/****************************************************************************/
LoggingObject::LoggingObject(LoggingObject *pParent, const Global::LoggingSource & TheLoggingSource) :
    QObject(pParent),
    m_LoggingSource(TheLoggingSource),
    m_oDisableErrorForwarding(false)
{
    // register data logger classes to allow usage with signal / slot mechanism
    qRegisterMetaType<EventEntry>("DataLogging::EventEntry");
    qRegisterMetaType<ComponentTestEntry>("DataLogging::ComponentTestEntry");
    qRegisterMetaType<DayEventEntry>("DataLogging::DayEventEntry");
    if(pParent != NULL) {
        // register fo logging and error handling in parent
        pParent->RegisterObject(this);
    }
}

/****************************************************************************/
void LoggingObject::RegisterObject(LoggingObject *pLoggingObject) {
    // check for NULL pointer
    CHECKPTR(pLoggingObject);
    // connect EmitError. EmitError is using a Qt::QueuedConnection connection!
//    CONNECTSIGNALSLOTQUEUED(pLoggingObject, EmitError(const DataLogging::EventEntry &),
//                            this, ProcessErrorEvent(const DataLogging::EventEntry &));
    // connect EmitEventEntry
//    CONNECTSIGNALSLOT(pLoggingObject, EmitEventEntry(const DataLogging::EventEntry &),
//                      this, ForwardEventEntry(const DataLogging::EventEntry &));
    // connect EmitDayOperationEntry
    CONNECTSIGNALSLOT(pLoggingObject, EmitDayEventEntry(const DataLogging::DayEventEntry &),
                      this, ForwardDayEventEntry(const DataLogging::DayEventEntry &));
    // connect EmitComponentTestEntry
//    CONNECTSIGNALSLOT(pLoggingObject, EmitComponentTestEntry(const DataLogging::ComponentTestEntry &),
//                      this, ForwardComponentTestEntry(const DataLogging::ComponentTestEntry &));
}

/****************************************************************************/
void LoggingObject::PrivateSendEvent(const DayEventEntry &TheEventEntry) const {
    PrivateSendEventEntry(TheEventEntry);
    // check if it is an error or fatal error
    if((TheEventEntry.GetEventType() == Global::EVTTYPE_FATAL_ERROR) ||
       (TheEventEntry.GetEventType() == Global::EVTTYPE_ERROR))
    {
        PrivateSendError(TheEventEntry);
    }
}

/****************************************************************************/
void LoggingObject::PrivateSendEventEntry(const DayEventEntry &TheEventEntry) const {
    // check if our signals are connected
    if(receivers(SIGNAL(EmitEventEntry(const DataLogging::EventEntry &))) > 0) {
        // signal for logging is connected to at least one receiver
        // and emit
        emit EmitEventEntry(TheEventEntry);
    } else {
        // signal is not connected. Output to std
        TheEventEntry.DumpToConsole();
    }
}

/****************************************************************************/
void LoggingObject::PrivateSendError(const DayEventEntry &TheEventEntry) const {
    if(m_oDisableErrorForwarding) {
        return;
    }
    // check if our signal is connected
    if(receivers(SIGNAL(EmitError(const DataLogging::EventEntry &))) > 0) {
        // signal is connected to at least one receiver
        emit EmitError(TheEventEntry);
    } else {
        // signal is not connected. Send error.
        // do not send it via SEND_FATAL_ERROR or we will enter a infinte recursion:
        // SEND_FATAL_ERROR -> SendEvent -> PrivateSendError -> SEND_FATAL_ERROR -> SendEvent -> PrivateSendError -> ...
        // we send it directly over PrivateSendEventEntry(const EventEntry &TheEventEntry);
        EventEntry NewEventEntry(Global::AdjustedTime::Instance().GetCurrentDateTime(),
                                 m_LoggingSource,
                                 Global::EVTTYPE_FATAL_ERROR,
                                 Global::EVTSTAT_ON,
                                 Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED,
                                 Global::tTranslatableStringList() << "LoggingObject::EmitError(const DataLogging::EventEntry &)");
        // send it directly to logging.
        /*! \todo uncomment after basic functionality works */
        //PrivateSendEventEntry(NewEventEntry);
    }
}

/****************************************************************************/
void LoggingObject::SendEvent(  const QString &FileName, int Line, const QDateTime &TimeStamp,
                                Global::EventType TheEventType, Global::EventStatus TheEventStatus,
                                quint32 EventCode, const Global::TranslatableString &AdditionalData) const
{
    // call other SendEvent function
    SendEvent(FileName, Line, TimeStamp, TheEventType, TheEventStatus,
              EventCode, Global::tTranslatableStringList() << AdditionalData);
}

/****************************************************************************/
void LoggingObject::SendEvent(  const QString &FileName, int Line, const QDateTime &TimeStamp,
                                Global::EventType TheEventType, Global::EventStatus TheEventStatus,
                                quint32 EventCode, const Global::tTranslatableStringList &EventStringList ) const
{
    // check if we are allowed to log
    bool MustSend = true;

    #if defined(QT_NO_DEBUG)
        // we are in release mode

        // supress compiler warnings
        Q_UNUSED(FileName);
        Q_UNUSED(Line);

        if(TheEventType == Global::EVTTYPE_DEBUG) {
            // debug messages will not be send
            MustSend = false;
        }
    #else
        // we are in debug mode

        // check which debug message has to be send.
        if(TheEventType == Global::EVTTYPE_DEBUG) {
            MustSend = EventFilter::Instance().MustSend(FileName, Line);
        }
    #endif

    // check if send allowed
    if(!MustSend) {
        return;
    }

    // create event entry
    /*! \todo Change the name from DayEventEntry to EventEntry */
//    DayEventEntry TheEntry(TimeStamp, EventCode, TheEventType,
//                               EventStringList,"", false, m_LoggingSource);
//    // and send
//    PrivateSendEvent(TheEntry);
}

/****************************************************************************/
void LoggingObject::SendEvent(const Global::Exception &TheException, Global::EventType TheEventType,
                              Global::EventStatus TheEventStatus)  const
{
    // call SendEvent with exception data extracted
    SendEvent(TheException.GetFile(), TheException.GetLine(), TheException.GetTimeStamp(),
              TheEventType, TheEventStatus, TheException.GetErrorCode(), TheException.GetAdditionalData());
}

/****************************************************************************/
void LoggingObject::PrivateSendDayOperation(const DayEventEntry &TheDayOperationEntry) const {
    // check if our signal is connected
    if(receivers(SIGNAL(EmitDayEventEntry(const DataLogging::DayEventEntry &))) > 0) {
        // signal is connected to at least one receiver
        emit EmitDayEventEntry(TheDayOperationEntry);
    } else {
        // signal is not connected. Send error.
//        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED,
//                  "LoggingObject::EmitDayOperationEntry(const DataLogging::DayEventEntry &)", Global::NO_NUMERIC_DATA
//                  , false);
    }
}


void LoggingObject::SendDayOperation(const QString &FileName, int Line, const QDateTime &TimeStamp,
                                     Global::EventType TheEventType, Global::LogLevel TheLogLevel,
                                     Global::EventStatus TheEventStatus, quint32 EventCode,
                                     const Global::TranslatableString &EventString, const QString NumericData
                                     , const bool ShowInRunLog) const {
    // call other SendEvent function
    SendDayOperation(FileName, Line, TimeStamp, TheEventType, TheLogLevel, TheEventStatus,
              EventCode, Global::tTranslatableStringList() << EventString, NumericData, ShowInRunLog);
}

void LoggingObject::SendDayOperation(const QString &FileName, int Line, const QDateTime &TimeStamp,
                                     Global::EventType TheEventType, Global::LogLevel TheLogLevel,
                                     Global::EventStatus TheEventStatus, quint32 EventCode,
                                     const Global::tTranslatableStringList &EventStringList, const QString NumericData
                                     , const bool ShowInRunLog) const {
    // check if we are allowed to log
    bool MustSend = true;

    /*! \todo Do we need this */
    #if defined(QT_NO_DEBUG)
        // we are in release mode

        // supress compiler warnings
        Q_UNUSED(FileName);
        Q_UNUSED(Line);

        if(TheEventType == Global::EVTTYPE_DEBUG) {
            // debug messages will not be send
            MustSend = false;
        }
    #else
        // we are in debug mode

        // check which debug message has to be send.
        if(TheEventType == Global::EVTTYPE_DEBUG) {
            MustSend = EventFilter::Instance().MustSend(FileName, Line);
        }
    #endif

    // check if send allowed
    if(!MustSend) {
        return;
    }

    DayEventEntry TheEntry(TimeStamp, EventCode, TheEventType,
                               EventStringList, NumericData, true, m_LoggingSource, TheLogLevel);
    PrivateSendDayOperation(TheEntry);

}


/****************************************************************************/
void LoggingObject::PrivateSendComponentTest(const ComponentTestEntry &TheComponentTestEntry) const {
    // check if our signal is connected
    if(receivers(SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &))) > 0) {
        // signal is connected to at least one receiver
        emit EmitComponentTestEntry(TheComponentTestEntry);
    } else {
        // signal is not connected. Send error.
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED,
                         "LoggingObject::EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)"
                         , Global::NO_NUMERIC_DATA, false);
    }
}

/****************************************************************************/
void LoggingObject::SendComponentTest(  const QString &ComponentName, const QString &BoardName,
                                        const QString &BoardSerialNumber,
                                        const QString &TesterName, const QString &DeviceSerialNumber,
                                        const QString &ComponentSerialNumber, const QString &ComponentFirmwareVersion,
                                        const QString &ComponentHardwareVersion, const QString &MainboardSoftwareVersion,
                                        const Global::tTranslatableStringList &TestResults) const
{
    // create entry
    ComponentTestEntry TheEntry(ComponentName, BoardName, BoardSerialNumber,
                                TesterName, DeviceSerialNumber, ComponentSerialNumber,
                                ComponentFirmwareVersion, ComponentHardwareVersion,
                                MainboardSoftwareVersion, TestResults);
    // and send
    PrivateSendComponentTest(TheEntry);
}

/****************************************************************************/
bool LoggingObject::LocalProcessErrorEvent(const DayEventEntry &/*TheEventEntry*/) {
    // No processing done
    return false;
}

/****************************************************************************/
void LoggingObject::ProcessErrorEvent(const DataLogging::DayEventEntry &TheEventEntry) {
    Q_ASSERT(   (TheEventEntry.GetEventType() == Global::EVTTYPE_FATAL_ERROR) ||
                (TheEventEntry.GetEventType() == Global::EVTTYPE_ERROR));
    // call local error processing method
    if(LocalProcessErrorEvent(TheEventEntry)) {
        // event was processed
        // forget it.
    } else {
        // event was not processed. Forward it.
        PrivateSendError(TheEventEntry);
    }
}

/****************************************************************************/
void LoggingObject::ForwardEventEntry(const DataLogging::DayEventEntry &TheEventEntry) const {
    PrivateSendEventEntry(TheEventEntry);
}

/****************************************************************************/
void LoggingObject::ForwardDayEventEntry(const DayEventEntry &TheDayOperationEntry) const {
    PrivateSendDayOperation(TheDayOperationEntry);
}

/****************************************************************************/
void LoggingObject::ForwardComponentTestEntry(const ComponentTestEntry &TheComponentTestEntry) const {
    PrivateSendComponentTest(TheComponentTestEntry);
}

} // end namespace DataLogging
