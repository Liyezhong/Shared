/****************************************************************************/
/*! \file DataLogging/Include/LoggingObject.h
 *
 *  \brief Definition file for class LoggingObject.
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

#ifndef DATALOGGING_LOGGINGOBJECT_H
#define DATALOGGING_LOGGINGOBJECT_H

#include <DataLogging/Include/EventEntry.h>
#include <DataLogging/Include/DayEventEntry.h>
#include <DataLogging/Include/ComponentTestEntry.h>
#include <Global/Include/Exception.h>

#include <QObject>

namespace DataLogging {


/****************************************************************************/
/**
 * \brief Helper for sending an event of type exception with status = EVTSTAT_ON.
 *
 * The current file name, file number and time stamp are set automatically.
 *
 * \param[in]   exc     The exception.
 */
/****************************************************************************/
#define SEND_EXCEPTION(exc) SendEvent(exc, Global::EVTTYPE_ERROR, Global::EVTSTAT_ON);

/****************************************************************************/
/**
 * \brief Helper for sending an DayOperation info.
 *
 * \param[in]   eventType   info/warning/error/fatal error
 * \param[in]   logLevel    Various logging levels
 * \param[in]   code        String ID.
 * \param[in]   data        Additional data.
 */
/****************************************************************************/
#define LOG_EVENT(eventType, logLevel, code, data, numericData, showInRunLog) SendDayOperation(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), eventType, logLevel, Global::EVTSTAT_ON, code, data, numericData, showInRunLog);

/****************************************************************************/
/**
 * \brief Helper for sending an event of type debug with status = EVTSTAT_ON.
 *
 * The current file name, file number and time stamp are set automatically.
 *
 * \param[in]   data    Additional data.
 */
/****************************************************************************/
#if defined(QT_NO_DEBUG)
    // we are in release mode
    #define SEND_DEBUG(data)
#else
    // we are in debug mode
    #define SEND_DEBUG(data) SendEvent(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_DEBUG, Global::EVTSTAT_ON, Global::EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, data);
#endif

/****************************************************************************/
/**
 * \brief Base class for all classes able to send data to the data logging components
 * and to the error handling mechanism.
 *
 * This is done by the following four signals: \ref EmitError, \ref EmitEventEntry,
 * \ref EmitDayOperationEntry and \ref EmitComponentTestEntry.\n
 * <b> To make sure that EventEntries are displayed and even if signal EmitEventEntry
 * is not connected, it is highly recomended to use \ref SendEvent or the according macro
 * for that.
 * Using \ref SendDayOperation and \ref SendComponentTest ensures that an error
 * will be send if the signals are not connected. However, the entries will be
 * discarded!</b>
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class LoggingObject : public QObject {
    Q_OBJECT
friend class TestLoggingObject;
private:
    Global::LoggingSource   m_LoggingSource;            ///< Logging source
    bool                    m_oDisableErrorForwarding;  ///< If true, EmitError signal is not called
    /****************************************************************************/
    LoggingObject();                        ///< Not implemented.
    LoggingObject(const LoggingObject &);   ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Register an object for logging and error handling mechanism.
     *
     * This ensures that:
     * - the object's signal for error handling is forwarded to our \ref ProcessErrorEvent slot.
     * - the object's signal for event logging is forwarded to the correct destination.
     * - the object's signal for day operation logging is forwarded to the correct destination.
     * - the object's signal for component test logging is forwarded to the correct destination.
     * \warning An \ref Global::Exception may be thrown!
     * \warning EmitError uses a Qt::QueuedConnection connection!
     *
     * \param[in]   pLoggingObject      The logging object to connect to our own signals.
     */
    /****************************************************************************/
    virtual void RegisterObject(LoggingObject *pLoggingObject);
    /****************************************************************************/
    /**
     * \brief Try to send event.
     *
     * First it sends the event entry over \ref PrivateSendEventEntry for logging purposes
     * and if it is an error or fatal error it will be sent over \ref PrivateSendError
     * to allow an error processing.
     *
     * \param[in]   TheEventEntry   The event entry to send.
     */
    /****************************************************************************/
    void PrivateSendEvent(const DayEventEntry &TheEventEntry) const;
    /****************************************************************************/
    /**
     * \brief Try to send event entry over EmitEventEntry signal.
     *
     * If the signal EmitEventEntry is connected, the event entry is send.
     * If the signal is not connected, a string containing the event information
     * is written to stdio.
     *
     * \param[in]   TheEventEntry   The event entry to send.
     */
    /****************************************************************************/
    void PrivateSendEventEntry(const DayEventEntry &TheEventEntry) const;
    /****************************************************************************/
    /**
     * \brief Try to send event over EmitError signal.
     *
     * If signal connected check must be done and the signal isn't connected, an
     * error will be send via PrivateSendEventEntry directly to the logger and
     * the current event entry will be dropped!
     *
     * \warning The current event entry will be discarded if the signal is not connected!
     *
     * \param[in]   TheEventEntry   The event entry to send.
     */
    /****************************************************************************/
    void PrivateSendError(const DayEventEntry &TheEventEntry) const;
    /****************************************************************************/
    /**
     * \brief Try to send day operation entry over EmitDayOperationEntry signal.
     *
     * If the signal is connected, a entry is constructed and send.
     * If the signal is not connected, an error will be send. The entry will be discarded.
     *
     * \warning The current day operation entry will be discarded if the signal is not connected!
     *
     * \param[in]   TheDayOperationEntry    TheDayOperationEntry to send.
     */
    /****************************************************************************/
    void PrivateSendDayOperation(const DayEventEntry &TheDayOperationEntry) const;
    /****************************************************************************/
    /**
     * \brief Try to send component test entry over EmitComponentTestEntry signal.
     *
     * If the signal is connected, a entry is constructed and send.
     * If the signal is not connected, send an error. The entry will be discarded.
     *
     * \warning The current component test entry will be discarded if the signal is not connected!
     *
     * \param[in]   TheComponentTestEntry   The component test entry.
     */
    /****************************************************************************/
    void PrivateSendComponentTest(const ComponentTestEntry &TheComponentTestEntry) const;
    /****************************************************************************/
    /**
     * \brief Process error event from local object.
     *
     * This function is called when a event entry of type error or fatal error
     * from a local object is received.
     * No processing is done in this class, the event is just forwarded.
     * If processing of event is needed, this method has to be overloaded.
     *
     * \param[in]  TheEventEntry = event from one of local objects
     * \return      true if event was processed.
     */
    /****************************************************************************/
    virtual bool LocalProcessErrorEvent(const DayEventEntry &TheEventEntry);
protected:
    /****************************************************************************/
    /**
     * \brief Disable error forwarding.
     *
     * Disables the error forwarding to the parent. This may be usefull if parent
     * isn't involved in error processing anymore.
     */
    /****************************************************************************/
    inline void DisableErrorForwarding() {
        m_oDisableErrorForwarding = true;
    }
    /****************************************************************************/
    /**
     * \brief Try to send event over EmitEventEntry signal.
     *
     * If the signal EmitEventEntry is connected, a event entry is constructed and send.
     * If the signal is not connected, a string containing the event information
     * is written to stdio. If the event is of type error or fatal error
     * it will be also sent over EmitError.
     *
     * \param[in]   FileName            File name in which event occured.
     * \param[in]   Line                Line in which event occured.
     * \param[in]   TimeStamp           Time stamp when event occured.
     * \param[in]   TheEventType        Event type.
     * \param[in]   TheEventStatus      Event status.
     * \param[in]   EventCode           Event code.
     * \param[in]   AdditionalData      Additional data.
     */
    /****************************************************************************/
    void SendEvent( const QString &FileName, int Line, const QDateTime &TimeStamp,
                    Global::EventType TheEventType, Global::EventStatus TheEventStatus,
                    quint32 EventCode, const Global::TranslatableString &AdditionalData) const;
    /****************************************************************************/
    /**
     * \brief Try to send event over EmitEventEntry signal.
     *
     * If the signal EmitEventEntry is connected, a event entry is constructed and send.
     * If the signal is not connected, a string containing the event information
     * is written to stdio. If the event is of type error or fatal error
     * it will be also sent over EmitError.
     *
     * \param[in]   FileName            File name in which event occured.
     * \param[in]   Line                Line in which event occured.
     * \param[in]   TimeStamp           Time stamp when event occured.
     * \param[in]   TheEventType        Event type.
     * \param[in]   TheEventStatus      Event status.
     * \param[in]   EventCode           Event code.
     * \param[in]   AdditionalData      Additional data as translatable strings.
     */
    /****************************************************************************/
    void SendEvent( const QString &FileName, int Line, const QDateTime &TimeStamp,
                    Global::EventType TheEventType, Global::EventStatus TheEventStatus,
                    quint32 EventCode, const Global::tTranslatableStringList &AdditionalData) const;
    /****************************************************************************/
    /**
     * \brief Try to send event over EmitEventEntry signal.
     *
     * If the signal is connected, a entry is constructed and send.
     * If the signal is not connected, a string containing the event information
     * is written to stdio. If the event is of type error or fatal error
     * it will be also sent over EmitError.
     *
     * \param[in]   TheException        The exception containing all relevant data to send.
     * \param[in]   TheEventType        Event type.
     * \param[in]   TheEventStatus      Event status.
     */
    /****************************************************************************/
    void SendEvent(const Global::Exception &TheException, Global::EventType TheEventType, Global::EventStatus TheEventStatus) const;

    /****************************************************************************/
    /**
     * \brief Try to send day operation entry over EmitDayOperationEntry signal.
     *
     * If the signal is connected, a entry is constructed and send.
     * If the signal is not connected, an error will be send. The entry will be discarded.
     *
     * \param[in]   StringID    String ID for translatable string.
     * \param[in]   Argument    Argument list for translatable string.
     */
    /****************************************************************************/
    void SendDayOperation(quint32 StringID, const Global::TranslatableString &Argument) const;
    /****************************************************************************/
    /**
     * \brief Try to send day operation entry over EmitDayOperationEntry signal.
     *
     * If the signal is connected, a entry is constructed and send.
     * If the signal is not connected, an error will be send. The entry will be discarded.
     *
     * \param[in]   StringID        String ID for translatable string.
     * \param[in]   ArgumentList    Argument list for translatable string.
     */
    /****************************************************************************/
    void SendDayOperation(quint32 StringID, const Global::tTranslatableStringList &ArgumentList) const;

    void SendDayOperation(const QString &FileName, int Line, const QDateTime &TimeStamp,
                          Global::EventType TheEventType, Global::LogLevel TheLogLevel,
                          Global::EventStatus TheEventStatus, quint32 EventCode,
                          const Global::TranslatableString &EventString, QString NumericData, const bool ShowInRunLog) const;

    void SendDayOperation(const QString &FileName, int Line, const QDateTime &TimeStamp,
                          Global::EventType TheEventType, Global::LogLevel TheLogLevel,
                          Global::EventStatus TheEventStatus, quint32 EventCode,
                          const Global::tTranslatableStringList &EventStringList, QString NumericData
                          , const  bool ShowInRunLog) const;

    /****************************************************************************/
    /**
     * \brief Try to send component test entry over EmitComponentTestEntry signal.
     *
     * If the signal is connected, a entry is constructed and send.
     * If the signal is not connected, send an error. The entry will be discarded.
     *
     * \param[in]   ComponentName               Name of the component.
     * \param[in]   BoardName                   Name of the board.
     * \param[in]   BoardSerialNumber           Serial number of the board.
     * \param[in]   TesterName                  Name of the tester.
     * \param[in]   DeviceSerialNumber          Serial number of whole device.
     * \param[in]   ComponentSerialNumber       Serial number of the tested component.
     * \param[in]   ComponentFirmwareVersion    Firmware version number of the tested component.
     * \param[in]   ComponentHardwareVersion    Hardware version number of the tested component.
     * \param[in]   MainboardSoftwareVersion    Software version number of the mainboard.
     * \param[in]   TestResults                 Content of component test.
     */
    /****************************************************************************/
    void SendComponentTest( const QString &ComponentName, const QString &BoardName,
                            const QString &BoardSerialNumber,
                            const QString &TesterName, const QString &DeviceSerialNumber,
                            const QString &ComponentSerialNumber, const QString &ComponentFirmwareVersion,
                            const QString &ComponentHardwareVersion, const QString &MainboardSoftwareVersion,
                            const Global::tTranslatableStringList &TestResults) const;
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   pParent             Parent for LoggingObject. Must be itself a LoggingObject.
     * \param[in]   TheLoggingSource    Source to set in log entry.
     */
    /****************************************************************************/
    LoggingObject(LoggingObject *pParent, const Global::LoggingSource &TheLoggingSource);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~LoggingObject() {
    }
    /****************************************************************************/
    /**
     * \brief Get logging source.
     *
     * \return      Logging source.
     */
    /****************************************************************************/
    inline Global::LoggingSource GetLoggingSource() const {
        return m_LoggingSource;
    }
signals:
    /****************************************************************************/
    /**
     * \brief Emit error for error processing in connected object.
     *
     * \param[in]   TheEventEntry   Event entry to emit.
     */
    /****************************************************************************/
    void EmitError(const DataLogging::DayEventEntry &TheEventEntry) const;
    /****************************************************************************/
    /**
     * \brief Emit specific event entry.
     *
     * \param[in]   TheEventEntry   Event entry to emit.
     */
    /****************************************************************************/
    void EmitEventEntry(const DataLogging::DayEventEntry &TheEventEntry) const;
    /****************************************************************************/
    /**
     * \brief Emit specific day operation entry.
     *
     * \param[in]   TheDayOperationEntry    Day operation entry to emit.
     */
    /****************************************************************************/
    void EmitDayEventEntry(const DataLogging::DayEventEntry &TheDayOperationEntry) const;
    /****************************************************************************/
    /**
     * \brief Emit specific component test entry.
     *
     * \param[in]   TheComponentTestEntry   Component test entry to emit.
     */
    /****************************************************************************/
    void EmitComponentTestEntry(const DataLogging::ComponentTestEntry &TheComponentTestEntry) const;
public slots:
    /****************************************************************************/
    /**
     * \brief API for receiving local system events of type error or fatal error.
     *
     *        Signal Emitter of the Thread Controller's local object shall be
     *        connected to this slot if Thread Controller needs to re-map
     *        outgoing local event to global event or if some action shall be
     *        triggered internally before forwarding outgoing event to
     *        System's global event handler. This is done by registering the object
     *        through a call of \ref RegisterObject.
     *
     * \param[in]  TheEventEntry = event from one of local objects
     */
    /****************************************************************************/
    void ProcessErrorEvent(const DataLogging::DayEventEntry &TheEventEntry);
    /****************************************************************************/
    /**
     * \brief Forward an event entry.
     *
     * \param[in]   TheEventEntry       Event entry to emit.
     */
    /****************************************************************************/
    void ForwardEventEntry(const DataLogging::DayEventEntry &TheEventEntry) const;
    /****************************************************************************/
    /**
     * \brief Forward an day operation entry.
     *
     * \param[in]   TheDayOperationEntry    Day operation entry to emit.
     */
    /****************************************************************************/
    void ForwardDayEventEntry(const DataLogging::DayEventEntry &TheDayOperationEntry) const;
    /****************************************************************************/
    /**
     * \brief Forward an component test entry.
     *
     * \param[in]   TheComponentTestEntry   Component test entry to emit.
     */
    /****************************************************************************/
    void ForwardComponentTestEntry(const DataLogging::ComponentTestEntry &TheComponentTestEntry) const;
}; // end class LoggingObject

} // end namespace DataLogging

#endif // DATALOGGING_LOGGINGOBJECT_H
