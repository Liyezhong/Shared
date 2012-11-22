/****************************************************************************/
/*! \file TestLoggingObject.cpp
 *
 *  \brief Implementation file for class TestLoggingObject.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-09-15
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

#include <QTest>
#include <DataLogging/Include/LoggingObject.h>
#include <DataLogging/Include/DayEventLogger.h>
#include <DataLogging/Include/DataLoggingEventCodes.h>
#include <DataManagement/Include/XmlConfigFileStrings.h>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/EventTranslator.h>
#include <Global/Include/Utils.h>
#include <Threads/Include/ThreadsEventCodes.h>
#include <QMetaType>
#include <QVariant>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief EventEntry signal spy since Qt's QSignalSpy does not work as expected.
 */
/****************************************************************************/
class EventEntrySignalSpy : public QObject, public QList<EventEntry> {
    Q_OBJECT
public slots:
    /****************************************************************************/
    /**
     * \brief The spy slot.
     *
     * \param   E   The event entry.
     */
    /****************************************************************************/
    void TheSlot(const DataLogging::EventEntry &E) {
        // append to list.
        append(E);
    }
}; // end class EventEntrySignalSpy


/****************************************************************************/
/**
 * \brief DayOperationEntry signal spy since Qt's QSignalSpy does not work as expected.
 */
/****************************************************************************/
class DayOperationEntrySignalSpy : public QObject, public QList<DayEventEntry> {
    Q_OBJECT
public slots:
    /****************************************************************************/
    /**
     * \brief The spy slot.
     *
     * \param   E   The day operation entry.
     */
    /****************************************************************************/
    void TheSlot(const DataLogging::DayEventEntry &E) {
        // append to list.
        append(E);
    }
}; // end class DayOperationEntrySignalSpy


/****************************************************************************/
/**
 * \brief ComponentTestEntry signal spy since Qt's QSignalSpy does not work as expected.
 */
/****************************************************************************/
class ComponentTestEntrySignalSpy : public QObject, public QList<ComponentTestEntry> {
    Q_OBJECT
public slots:
    /****************************************************************************/
    /**
     * \brief The spy slot.
     *
     * \param   E   The component test entry.
     */
    /****************************************************************************/
    void TheSlot(const DataLogging::ComponentTestEntry &E) {
        // append to list.
        append(E);
    }
}; // end class ComponentTestEntrySignalSpy


/****************************************************************************/
/**
 * \brief ComponentTestEntry signal spy since Qt's QSignalSpy does not work as expected.
 */
/****************************************************************************/
class ErrorProcessor : public LoggingObject {
    Q_OBJECT
private:
    int     m_ErrorsToProcess;      ///< Numbers of errors to be processed.
    int     m_ErrorsProcessed;      ///< Number of errors already processed.
    /****************************************************************************/
    ErrorProcessor();                                           ///< Not implemented.
    ErrorProcessor(const ErrorProcessor &);                     ///< Not implemented.
    const ErrorProcessor & operator = (const ErrorProcessor &); ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   pParent             Parent for LoggingObject. Must be itself a LoggingObject.
     * \param[in]   TheLoggingSource    Source to set in log entry.
     * \param[in]   ErrorsToProcess     Numbers of errors to process.
     */
    /****************************************************************************/
    ErrorProcessor(LoggingObject *pParent, const Global::LoggingSource &TheLoggingSource, int ErrorsToProcess) :
        LoggingObject(pParent, TheLoggingSource),
        m_ErrorsToProcess(ErrorsToProcess),
        m_ErrorsProcessed(0)
    {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~ErrorProcessor() {
    }
    /****************************************************************************/
    /**
     * \brief Process error event from local object.
     *
     * \param[in]  TheEventEntry = event from one of local objects
     * \return      true if event was processed.
     */
    /****************************************************************************/
    bool LocalProcessErrorEvent(const EventEntry &TheEventEntry) {
        Q_UNUSED(TheEventEntry);
        if(m_ErrorsProcessed < m_ErrorsToProcess) {
            // process error
            m_ErrorsProcessed++;
            return true;
        }
        // no error processing
        return false;
    }
}; // end class ErrorProcessor


/****************************************************************************/
/**
 * \brief Test class for LoggingObject class.
 */
/****************************************************************************/
class TestLoggingObject : public QObject {
    Q_OBJECT
private:
    /****************************************************************************/
    /**
     * \brief reconnect Source::EmitError() and Target::ProcessErrorEvent() with
     * Qt::AutoConnection type to ensure we have something in the ErrorSpies...
     *
     * \param[in, out]  Source      Source object.
     * \param[in, out]  Target      Target object.
     */
    /****************************************************************************/
    void ReconnectEmitError(LoggingObject &Source, LoggingObject &Target);
private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
     */
    /****************************************************************************/
    void init();
    /****************************************************************************/
    /**
     * \brief Called after each testfunction was executed.
     */
    /****************************************************************************/
    void cleanup();
    /****************************************************************************/
    /**
     * \brief Called after last testfunction was executed.
     */
    /****************************************************************************/
    void cleanupTestCase();
//    /****************************************************************************/
//    /**
//     * \brief Test constructor
//     */
//    /****************************************************************************/
//    void utConstructor();
//    /****************************************************************************/
//    /**
//     * \brief Test for unconnected SendEvent.
//     */
//    /****************************************************************************/
//    void utSendEventUnconnected();
//    /****************************************************************************/
//    /**
//     * \brief Test for unconnected SendDayOperation. Emits an error.
//     */
//    /****************************************************************************/
//    void utSendDayOperationUnconnected();
//    /****************************************************************************/
//    /**
//     * \brief Test for unconnected SendComponentTest. Emits an error.
//     */
//    /****************************************************************************/
//    void utSendComponentTestUnconnected();
//    /****************************************************************************/
//    /**
//     * \brief Test for connected SendEvent.
//     */
//    /****************************************************************************/
//    void utSendEvent();
//    /****************************************************************************/
//    /**
//     * \brief Test for connected SendDayOperation.
//     */
//    /****************************************************************************/
//    void utSendDayOperation();
//    /****************************************************************************/
//    /**
//     * \brief Test for connected SendComponentTest.
//     */
//    /****************************************************************************/
//    void utSendComponentTest();
//    /****************************************************************************/
//    /**
//     * \brief Test for error processing with enabled error forwarding.
//     */
//    /****************************************************************************/
//    void utErrorProcessingEnabledErrorForwarding();
//    /****************************************************************************/
//    /**
//     * \brief Test for error processing with disabled error forwarding.
//     */
//    /****************************************************************************/
//    void utErrorProcessingDisabledErrorForwarding();

    void utTestNewLoggingRequirement();
}; // end class TestLoggingObject

/****************************************************************************/
void TestLoggingObject::ReconnectEmitError(LoggingObject &Source, LoggingObject &Target) {
    QVERIFY(QObject::disconnect(&Source, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                                &Target, SLOT(ProcessErrorEvent(const DataLogging::EventEntry &))));
    QVERIFY(QObject::connect(&Source, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                             &Target, SLOT(ProcessErrorEvent(const DataLogging::EventEntry &))));
}

/****************************************************************************/
void TestLoggingObject::initTestCase() {
    qRegisterMetaType<EventEntry>("DataLogging::EventEntry");
    qRegisterMetaType<DayEventEntry>("DataLogging::DayEventEntry");
}

/****************************************************************************/
void TestLoggingObject::init() {
}

/****************************************************************************/
void TestLoggingObject::cleanup() {
}

/****************************************************************************/
void TestLoggingObject::cleanupTestCase() {
}

#if 0
/****************************************************************************/
void TestLoggingObject::utConstructor() {
    Global::LoggingSource LS1(1, 11);
    Global::LoggingSource LS2(2, 22);

    LoggingObject Obj1(NULL, LS1);
    LoggingObject Obj2(NULL, LS2);

    QCOMPARE(Obj1.m_LoggingSource,      LS1);
    QCOMPARE(Obj1.GetLoggingSource(),   LS1);
    QCOMPARE(Obj2.m_LoggingSource,      LS2);
    QCOMPARE(Obj2.GetLoggingSource(),   LS2);
}

/****************************************************************************/
void TestLoggingObject::utSendEventUnconnected() {
    // create first logging object
    Global::LoggingSource LS1(1, 11);
    LoggingObject Obj1(NULL, LS1);
    // create second logging object
    Global::LoggingSource LS2(2, 22);
    LoggingObject Obj2(&Obj1, LS2);
    ReconnectEmitError(Obj2, Obj1);

    EventEntry Entry1;
    Global::Exception Exc1(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), 42);
    try {
        // send an event entry. we can use an "empty" one since nothing will happen...
//        Obj1.PrivateSendEvent(Entry1);
//        Obj1.SendEvent("FileName", 42, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
//                       Global::EVTSTAT_ON, 33, "AdditionalData");
//        Obj1.SendEvent("FileName", 42, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
//                       Global::EVTSTAT_ON, 33, Global::tTranslatableStringList() << "AdditionalData1"  << "AdditionalData2");
//        // send an exception.
//        Obj1.SendEvent(Exc1, Global::EVTTYPE_INFO, Global::EVTSTAT_ON);
//        // send an event entry. we can use an "empty" one since nothing will happen...
//        Obj2.PrivateSendEvent(Entry1);
//        Obj2.SendEvent("FileName", 42, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_DEBUG,
//                       Global::EVTSTAT_ON, 33, "AdditionalData");
//        Obj2.SendEvent("FileName", 42, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_FATAL_ERROR,
//                       Global::EVTSTAT_ON, 33, Global::TranslatableString(101));
//        // send an exception.
//        Obj2.SendEvent(Exc1, Global::EVTTYPE_INFO, Global::EVTSTAT_ON);
    } catch(...) {
        QFAIL("You should never get here!");
    }
}


/****************************************************************************/
void TestLoggingObject::utSendDayOperationUnconnected() {
    EventEntrySignalSpy EventSpy;
    EventEntrySignalSpy ErrorSpy;
    // create first logging object
    Global::LoggingSource LS1(1, 11);
    LoggingObject Obj1(NULL, LS1);
    // attach signal spies
    QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                             &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
    QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                             &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
    // create second logging object
    Global::LoggingSource LS2(2, 22);
    LoggingObject Obj2(&Obj1, LS2);
    ReconnectEmitError(Obj2, Obj1);

    try {
        // send day operation entries
//        Obj1.SendDayOperation(111);
//        Obj1.SendDayOperation(1111, Global::TranslatableString("1-11"));
//        Obj2.SendDayOperation(222);
//        Obj2.SendDayOperation(2222, Global::tTranslatableStringList() << "2-11" << "2-22");
        // check signal spies
        QCOMPARE(EventSpy.count(), 4);
        QCOMPARE(ErrorSpy.count(), 4);
        // extract event entries and check them
        EventEntry Result = EventSpy.at(0);
        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED);
        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_FATAL_ERROR);
        QCOMPARE(Result.GetSource(),                LS1);
        QCOMPARE(Result.GetAdditionalData().size(), 1);
        Result = ErrorSpy.at(0);
        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED);
        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_FATAL_ERROR);
        QCOMPARE(Result.GetSource(),                LS1);
        QCOMPARE(Result.GetAdditionalData().size(), 1);
        Result = EventSpy.at(1);
        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED);
        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_FATAL_ERROR);
        QCOMPARE(Result.GetSource(),                LS1);
        QCOMPARE(Result.GetAdditionalData().size(), 1);
        Result = ErrorSpy.at(1);
        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED);
        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_FATAL_ERROR);
        QCOMPARE(Result.GetSource(),                LS1);
        QCOMPARE(Result.GetAdditionalData().size(), 1);
        // the first objects also sends the following errors
        Result = EventSpy.at(2);
        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED);
        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_FATAL_ERROR);
        QCOMPARE(Result.GetSource(),                LS1);
        QCOMPARE(Result.GetAdditionalData().size(), 1);
        Result = ErrorSpy.at(2);
        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED);
        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_FATAL_ERROR);
        QCOMPARE(Result.GetSource(),                LS1);
        QCOMPARE(Result.GetAdditionalData().size(), 1);
        Result = EventSpy.at(3);
        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED);
        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_FATAL_ERROR);
        QCOMPARE(Result.GetSource(),                LS1);
        QCOMPARE(Result.GetAdditionalData().size(), 1);
        Result = ErrorSpy.at(3);
        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED);
        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_FATAL_ERROR);
        QCOMPARE(Result.GetSource(),                LS1);
        QCOMPARE(Result.GetAdditionalData().size(), 1);
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestLoggingObject::utSendComponentTestUnconnected() {
    EventEntrySignalSpy EventSpy;
    EventEntrySignalSpy ErrorSpy;
    // create first logging object
    Global::LoggingSource LS1(1, 11);
    LoggingObject Obj1(NULL, LS1);
    // create second logging object
    Global::LoggingSource LS2(2, 22);
    LoggingObject Obj2(&Obj1, LS2);
    ReconnectEmitError(Obj2, Obj1);
    // attach signal spies
    QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                             &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
    QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                             &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));

    try {
        // send component test entry
        Obj1.SendComponentTest("ComponentName1", "BoardName1", "BoardSerialNumber1", "TesterName1", "DeviceSerialNumber1",
                               "ComponentSerialNumber1", "ComponentFirmwareVersion1","ComponentHardwareVersion1",
                               "MainboardSoftwareVersion1", Global::tTranslatableStringList());
        Obj2.SendComponentTest("ComponentName2", "BoardName2", "BoardSerialNumber2", "TesterName2", "DeviceSerialNumber2",
                               "ComponentSerialNumber2", "ComponentFirmwareVersion2","ComponentHardwareVersion2",
                               "MainboardSoftwareVersion", Global::tTranslatableStringList());
        // check signal spy
        QCOMPARE(EventSpy.count(), 2);
        QCOMPARE(ErrorSpy.count(), 2);

        // extract event entries and check them
        EventEntry Result = EventSpy.at(0);
        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED);
        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_FATAL_ERROR);
        QCOMPARE(Result.GetSource(),                LS1);
        QCOMPARE(Result.GetAdditionalData().size(), 1);
        Result = ErrorSpy.at(0);
        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED);
        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_FATAL_ERROR);
        QCOMPARE(Result.GetSource(),                LS1);
        QCOMPARE(Result.GetAdditionalData().size(), 1);
        // the first objects also sends the following error
        Result = EventSpy.at(1);
        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED);
        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_FATAL_ERROR);
        QCOMPARE(Result.GetSource(),                LS1);
        QCOMPARE(Result.GetAdditionalData().size(), 1);
        Result = ErrorSpy.at(1);
        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED);
        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_FATAL_ERROR);
        QCOMPARE(Result.GetSource(),                LS1);
        QCOMPARE(Result.GetAdditionalData().size(), 1);
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestLoggingObject::utSendEvent() {
    EventEntrySignalSpy EventSpy;
    EventEntrySignalSpy ErrorSpy;
    Global::LoggingSource LS0;
    // create first logging object
    Global::LoggingSource LS1(1, 11);
    LoggingObject Obj1(NULL, LS1);
    // create second logging object
    Global::LoggingSource LS2(2, 22);
    LoggingObject Obj2(&Obj1, LS2);
    ReconnectEmitError(Obj2, Obj1);
    // attach signal spies
    QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                             &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
    QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                             &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));

    EventEntry Entry1;
    Global::Exception Exc1(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), 42);
    Global::Exception Exc2(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), 43);
    try {
        // send an event entry. we can use an "empty" one since nothing will happen...
//        Obj1.PrivateSendEvent(Entry1);
//        Obj1.SendEvent("FileName1", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
//                       Global::EVTSTAT_ON, 33, "AdditionalData1");
//        Obj1.SendEvent("FileName2", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
//                       Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData2"  << "AdditionalData3");
//        // send an exception.
//        Obj1.SendEvent(Exc1, Global::EVTTYPE_INFO, Global::EVTSTAT_ON);
//
//        Obj2.PrivateSendEvent(Entry1);
//        Obj2.SendEvent("FileName3", 102, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
//                       Global::EVTSTAT_ON, 35, "AdditionalData4");
//        Obj2.SendEvent("FileName4", 103, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
//                       Global::EVTSTAT_OFF, 36, Global::tTranslatableStringList() << "AdditionalData5"  << "AdditionalData6");
//        // send an exception.
//        Obj2.SendEvent(Exc2, Global::EVTTYPE_INFO, Global::EVTSTAT_ON);
//
//        // check signal spies
//        QCOMPARE(EventSpy.count(), 8);
//        QCOMPARE(ErrorSpy.count(), 2);
//        // extract event entries and check them
//        EventEntry Result = EventSpy.at(0);
//        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_UNDEFINED);
//        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
//        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_UNDEFINED);
//        QCOMPARE(Result.GetSource(),                LS0);
//        QCOMPARE(Result.GetAdditionalData().size(), 0);
//        Result = EventSpy.at(1);
//        QCOMPARE(Result.GetEventCode(),             quint32(33));
//        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
//        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_INFO);
//        QCOMPARE(Result.GetSource(),                LS1);
//        QCOMPARE(Result.GetAdditionalData().size(), 1);
//        QCOMPARE(Result.GetAdditionalData().at(0).GetString(),  QString("AdditionalData1"));
//        Result = EventSpy.at(2);
//        QCOMPARE(Result.GetEventCode(),             quint32(34));
//        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_OFF);
//        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_ERROR);
//        QCOMPARE(Result.GetSource(),                LS1);
//        QCOMPARE(Result.GetAdditionalData().size(), 2);
//        QCOMPARE(Result.GetAdditionalData().at(0).GetString(),  QString("AdditionalData2"));
//        QCOMPARE(Result.GetAdditionalData().at(1).GetString(),  QString("AdditionalData3"));
//        Result = ErrorSpy.at(0);
//        QCOMPARE(Result.GetEventCode(),             quint32(34));
//        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_OFF);
//        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_ERROR);
//        QCOMPARE(Result.GetSource(),                LS1);
//        QCOMPARE(Result.GetAdditionalData().size(), 2);
//        QCOMPARE(Result.GetAdditionalData().at(0).GetString(),  QString("AdditionalData2"));
//        QCOMPARE(Result.GetAdditionalData().at(1).GetString(),  QString("AdditionalData3"));
//        Result = EventSpy.at(3);
//        QCOMPARE(Result.GetEventCode(),             quint32(42));
//        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
//        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_INFO);
//        QCOMPARE(Result.GetSource(),                LS1);
//        QCOMPARE(Result.GetAdditionalData().size(), 0);
//        Result = EventSpy.at(4);
//        QCOMPARE(Result.GetEventCode(),             Global::EVENT_GLOBAL_UNDEFINED);
//        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
//        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_UNDEFINED);
//        QCOMPARE(Result.GetSource(),                LS0);
//        QCOMPARE(Result.GetAdditionalData().size(), 0);
//        Result = EventSpy.at(5);
//        QCOMPARE(Result.GetEventCode(),             quint32(35));
//        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
//        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_INFO);
//        QCOMPARE(Result.GetSource(),                LS2);
//        QCOMPARE(Result.GetAdditionalData().size(), 1);
//        QCOMPARE(Result.GetAdditionalData().at(0).GetString(),  QString("AdditionalData4"));
//        Result = EventSpy.at(6);
//        QCOMPARE(Result.GetEventCode(),             quint32(36));
//        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_OFF);
//        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_ERROR);
//        QCOMPARE(Result.GetSource(),                LS2);
//        QCOMPARE(Result.GetAdditionalData().size(), 2);
//        QCOMPARE(Result.GetAdditionalData().at(0).GetString(),  QString("AdditionalData5"));
//        QCOMPARE(Result.GetAdditionalData().at(1).GetString(),  QString("AdditionalData6"));
//        Result = ErrorSpy.at(1);
//        QCOMPARE(Result.GetEventCode(),             quint32(36));
//        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_OFF);
//        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_ERROR);
//        QCOMPARE(Result.GetSource(),                LS2);
//        QCOMPARE(Result.GetAdditionalData().size(), 2);
//        QCOMPARE(Result.GetAdditionalData().at(0).GetString(),  QString("AdditionalData5"));
//        QCOMPARE(Result.GetAdditionalData().at(1).GetString(),  QString("AdditionalData6"));
//        Result = EventSpy.at(7);
//        QCOMPARE(Result.GetEventCode(),             quint32(43));
//        QCOMPARE(Result.GetEventStatus(),           Global::EVTSTAT_ON);
//        QCOMPARE(Result.GetEventType(),             Global::EVTTYPE_INFO);
//        QCOMPARE(Result.GetSource(),                LS2);
//        QCOMPARE(Result.GetAdditionalData().size(), 0);
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestLoggingObject::utSendDayOperation() {
    EventEntrySignalSpy EventSpy;
    EventEntrySignalSpy ErrorSpy;
    DayOperationEntrySignalSpy DayOpSpy;
    // create first logging object
    Global::LoggingSource LS1(1, 11);
    LoggingObject Obj1(NULL, LS1);
    // create second logging object
    Global::LoggingSource LS2(2, 22);
    LoggingObject Obj2(&Obj1, LS2);
    ReconnectEmitError(Obj2, Obj1);
    // attach signal spies
    QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                             &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
    QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                             &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
    QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitDayOperationEntry(const DataLogging::DayOperationEntry &)),
                             &DayOpSpy, SLOT(TheSlot(const DataLogging::DayOperationEntry &))));

    try {
        // send day operation entries
//        Obj1.SendDayOperation(42);
//        Obj1.SendDayOperation(66, Global::tTranslatableStringList() << "1-11" << "1-22");
//        Obj2.SendDayOperation(43);
//        Obj2.SendDayOperation(67, Global::tTranslatableStringList() << "2-11" << "2-22");
        // check signal spies
        QCOMPARE(EventSpy.count(),  0);
        QCOMPARE(ErrorSpy.count(),  0);
        QCOMPARE(DayOpSpy.count(),  4);
//        // extract entries and check them
//        DayOperationEntry Result = DayOpSpy.at(0);
//        QCOMPARE(Result.GetString().GetStringID(),              quint32(42));
//        QCOMPARE(Result.GetString().GetArgumentList().size(),   0);
//        Result = DayOpSpy.at(1);
//        QCOMPARE(Result.GetString().GetStringID(),              quint32(66));
//        QCOMPARE(Result.GetString().GetArgumentList().size(),   2);
//        QCOMPARE(Result.GetString().GetArgumentList().at(0).GetString(),    QString("1-11"));
//        QCOMPARE(Result.GetString().GetArgumentList().at(1).GetString(),    QString("1-22"));
//        Result = DayOpSpy.at(2);
//        QCOMPARE(Result.GetString().GetStringID(),              quint32(43));
//        QCOMPARE(Result.GetString().GetArgumentList().size(),   0);
//        Result = DayOpSpy.at(3);
//        QCOMPARE(Result.GetString().GetStringID(),              quint32(67));
//        QCOMPARE(Result.GetString().GetArgumentList().size(),   2);
//        QCOMPARE(Result.GetString().GetArgumentList().at(0).GetString(),    QString("2-11"));
//        QCOMPARE(Result.GetString().GetArgumentList().at(1).GetString(),    QString("2-22"));
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestLoggingObject::utSendComponentTest() {
    EventEntrySignalSpy EventSpy;
    EventEntrySignalSpy ErrorSpy;
    ComponentTestEntrySignalSpy CompTestSpy;
    // create first logging object
    Global::LoggingSource LS1(1, 11);
    LoggingObject Obj1(NULL, LS1);
    // create second logging object
    Global::LoggingSource LS2(2, 22);
    LoggingObject Obj2(&Obj1, LS2);
    ReconnectEmitError(Obj2, Obj1);
    // attach signal spies
    QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                             &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
    QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                             &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
    QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)),
                             &CompTestSpy, SLOT(TheSlot(const DataLogging::ComponentTestEntry &))));

    try {
        // send component test entries
        Obj1.SendComponentTest("ComponentName1", "BoardName1", "BoardSerialNumber1", "TesterName1", "DeviceSerialNumber1",
                               "ComponentSerialNumber1", "ComponentFirmwareVersion1","ComponentHardwareVersion1",
                               "MainboardSoftwareVersion1", Global::tTranslatableStringList() << 1 << 2);
        Obj2.SendComponentTest("ComponentName2", "BoardName2", "BoardSerialNumber2", "TesterName2", "DeviceSerialNumber2",
                               "ComponentSerialNumber2", "ComponentFirmwareVersion2","ComponentHardwareVersion2",
                               "MainboardSoftwareVersion2", Global::tTranslatableStringList() << 1 << 2 << 3);
        // check signal spies
        QCOMPARE(EventSpy.count(),      0);
        QCOMPARE(ErrorSpy.count(),      0);
        QCOMPARE(CompTestSpy.count(),   2);
        // extract entries and check them
        ComponentTestEntry Result = CompTestSpy.at(0);
        QCOMPARE(Result.GetComponentName(),                     QString("ComponentName1"));
        QCOMPARE(Result.GetBoardName(),                         QString("BoardName1"));
        QCOMPARE(Result.GetBoardSerialNumber(),                 QString("BoardSerialNumber1"));
        QCOMPARE(Result.GetTesterName(),                        QString("TesterName1"));
        QCOMPARE(Result.GetDeviceSerialNumber(),                QString("DeviceSerialNumber1"));
        QCOMPARE(Result.GetComponentSerialNumber(),             QString("ComponentSerialNumber1"));
        QCOMPARE(Result.GetComponentFirmwareVersion(),          QString("ComponentFirmwareVersion1"));
        QCOMPARE(Result.GetComponentHardwareVersion(),          QString("ComponentHardwareVersion1"));
        QCOMPARE(Result.GetMainboardSoftwareVersion(),          QString("MainboardSoftwareVersion1"));
        QCOMPARE(Result.GetTestResults().size(),                2);
        QCOMPARE(Result.GetTestResults().at(0).GetStringID(),   quint32(1));
        QCOMPARE(Result.GetTestResults().at(1).GetStringID(),   quint32(2));
        Result = CompTestSpy.at(1);
        QCOMPARE(Result.GetComponentName(),                     QString("ComponentName2"));
        QCOMPARE(Result.GetBoardName(),                         QString("BoardName2"));
        QCOMPARE(Result.GetBoardSerialNumber(),                 QString("BoardSerialNumber2"));
        QCOMPARE(Result.GetTesterName(),                        QString("TesterName2"));
        QCOMPARE(Result.GetDeviceSerialNumber(),                QString("DeviceSerialNumber2"));
        QCOMPARE(Result.GetComponentSerialNumber(),             QString("ComponentSerialNumber2"));
        QCOMPARE(Result.GetComponentFirmwareVersion(),          QString("ComponentFirmwareVersion2"));
        QCOMPARE(Result.GetComponentHardwareVersion(),          QString("ComponentHardwareVersion2"));
        QCOMPARE(Result.GetMainboardSoftwareVersion(),          QString("MainboardSoftwareVersion2"));
        QCOMPARE(Result.GetTestResults().size(),                3);
        QCOMPARE(Result.GetTestResults().at(0).GetStringID(),   quint32(1));
        QCOMPARE(Result.GetTestResults().at(1).GetStringID(),   quint32(2));
        QCOMPARE(Result.GetTestResults().at(2).GetStringID(),   quint32(3));
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestLoggingObject::utErrorProcessingEnabledErrorForwarding() {
    // send two errors, none processed
    {
        EventEntrySignalSpy EventSpy;
        EventEntrySignalSpy ErrorSpy;
        ComponentTestEntrySignalSpy CompTestSpy;
        // create first logging object
        Global::LoggingSource LS1(1, 11);
        ErrorProcessor Obj1(NULL, LS1, 0);
        // create second logging object
        Global::LoggingSource LS2(2, 22);
        ErrorProcessor Obj2(&Obj1, LS2, 0);
        ReconnectEmitError(Obj2, Obj1);
        // create third logging object
        Global::LoggingSource LS3(3, 33);
        LoggingObject Obj3(&Obj2, LS3);
        ReconnectEmitError(Obj3, Obj2);
        // attach signal spies
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                                 &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                                 &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)),
                                 &CompTestSpy, SLOT(TheSlot(const DataLogging::ComponentTestEntry &))));
        try {
            Obj3.SendEvent("FileName1", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
                           Global::EVTSTAT_ON, 33, "AdditionalData1");
            Obj3.SendEvent("FileName2", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData2"  << "AdditionalData3");
            Obj3.SendEvent("FileName3", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_UNDEFINED,
                           Global::EVTSTAT_ON, 33, "AdditionalData4");
            Obj3.SendEvent("FileName4", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData5"  << "AdditionalData6");
            QCOMPARE(EventSpy.count(),      4);
            QCOMPARE(ErrorSpy.count(),      2);
        } catch(...) {
            QFAIL("You should never get here!");
        }
    }

    // send two errors, one will be processed by Obj2
    {
        EventEntrySignalSpy EventSpy;
        EventEntrySignalSpy ErrorSpy;
        ComponentTestEntrySignalSpy CompTestSpy;
        // create first logging object
        Global::LoggingSource LS1(1, 11);
        ErrorProcessor Obj1(NULL, LS1, 0);
        // create second logging object
        Global::LoggingSource LS2(2, 22);
        ErrorProcessor Obj2(&Obj1, LS2, 1);
        ReconnectEmitError(Obj2, Obj1);
        // create third logging object
        Global::LoggingSource LS3(3, 33);
        LoggingObject Obj3(&Obj2, LS3);
        ReconnectEmitError(Obj3, Obj2);
        // attach signal spies
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                                 &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                                 &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)),
                                 &CompTestSpy, SLOT(TheSlot(const DataLogging::ComponentTestEntry &))));
        try {
            Obj3.SendEvent("FileName1", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
                           Global::EVTSTAT_ON, 33, "AdditionalData1");
            Obj3.SendEvent("FileName2", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData2"  << "AdditionalData3");
            Obj3.SendEvent("FileName3", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_UNDEFINED,
                           Global::EVTSTAT_ON, 33, "AdditionalData4");
            Obj3.SendEvent("FileName4", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData5"  << "AdditionalData6");
            QCOMPARE(EventSpy.count(),      4);
            QCOMPARE(ErrorSpy.count(),      1);
        } catch(...) {
            QFAIL("You should never get here!");
        }
    }

    // send two errors, one will be processed by Obj1
    {
        EventEntrySignalSpy EventSpy;
        EventEntrySignalSpy ErrorSpy;
        ComponentTestEntrySignalSpy CompTestSpy;
        // create first logging object
        Global::LoggingSource LS1(1, 11);
        ErrorProcessor Obj1(NULL, LS1, 1);
        // create second logging object
        Global::LoggingSource LS2(2, 22);
        ErrorProcessor Obj2(&Obj1, LS2, 0);
        ReconnectEmitError(Obj2, Obj1);
        // create third logging object
        Global::LoggingSource LS3(3, 33);
        LoggingObject Obj3(&Obj2, LS3);
        ReconnectEmitError(Obj3, Obj2);
        // attach signal spies
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                                 &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                                 &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)),
                                 &CompTestSpy, SLOT(TheSlot(const DataLogging::ComponentTestEntry &))));
        try {
            Obj3.SendEvent("FileName1", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
                           Global::EVTSTAT_ON, 33, "AdditionalData1");
            Obj3.SendEvent("FileName2", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData2"  << "AdditionalData3");
            Obj3.SendEvent("FileName3", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_UNDEFINED,
                           Global::EVTSTAT_ON, 33, "AdditionalData4");
            Obj3.SendEvent("FileName4", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData5"  << "AdditionalData6");
            QCOMPARE(EventSpy.count(),      4);
            QCOMPARE(ErrorSpy.count(),      1);
        } catch(...) {
            QFAIL("You should never get here!");
        }
    }

    // send two errors, both will be processed by Obj2
    {
        EventEntrySignalSpy EventSpy;
        EventEntrySignalSpy ErrorSpy;
        ComponentTestEntrySignalSpy CompTestSpy;
        // create first logging object
        Global::LoggingSource LS1(1, 11);
        ErrorProcessor Obj1(NULL, LS1, 0);
        // create second logging object
        Global::LoggingSource LS2(2, 22);
        ErrorProcessor Obj2(&Obj1, LS2, 2);
        ReconnectEmitError(Obj2, Obj1);
        // create third logging object
        Global::LoggingSource LS3(3, 33);
        LoggingObject Obj3(&Obj2, LS3);
        ReconnectEmitError(Obj3, Obj2);
        // attach signal spies
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                                 &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                                 &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)),
                                 &CompTestSpy, SLOT(TheSlot(const DataLogging::ComponentTestEntry &))));
        try {
            Obj3.SendEvent("FileName1", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
                           Global::EVTSTAT_ON, 33, "AdditionalData1");
            Obj3.SendEvent("FileName2", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData2"  << "AdditionalData3");
            Obj3.SendEvent("FileName3", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_UNDEFINED,
                           Global::EVTSTAT_ON, 33, "AdditionalData4");
            Obj3.SendEvent("FileName4", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData5"  << "AdditionalData6");
            QCOMPARE(EventSpy.count(),      4);
            QCOMPARE(ErrorSpy.count(),      0);
        } catch(...) {
            QFAIL("You should never get here!");
        }
    }

    // send two errors, one will be processed by Obj1 and one by Obj2
    {
        EventEntrySignalSpy EventSpy;
        EventEntrySignalSpy ErrorSpy;
        ComponentTestEntrySignalSpy CompTestSpy;
        // create first logging object
        Global::LoggingSource LS1(1, 11);
        ErrorProcessor Obj1(NULL, LS1, 1);
        // create second logging object
        Global::LoggingSource LS2(2, 22);
        ErrorProcessor Obj2(&Obj1, LS2, 1);
        ReconnectEmitError(Obj2, Obj1);
        // create third logging object
        Global::LoggingSource LS3(3, 33);
        LoggingObject Obj3(&Obj2, LS3);
        ReconnectEmitError(Obj3, Obj2);
        // attach signal spies
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                                 &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                                 &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)),
                                 &CompTestSpy, SLOT(TheSlot(const DataLogging::ComponentTestEntry &))));
        try {
            Obj3.SendEvent("FileName1", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
                           Global::EVTSTAT_ON, 33, "AdditionalData1");
            Obj3.SendEvent("FileName2", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData2"  << "AdditionalData3");
            Obj3.SendEvent("FileName3", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_UNDEFINED,
                           Global::EVTSTAT_ON, 33, "AdditionalData4");
            Obj3.SendEvent("FileName4", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData5"  << "AdditionalData6");
            QCOMPARE(EventSpy.count(),      4);
            QCOMPARE(ErrorSpy.count(),      0);
        } catch(...) {
            QFAIL("You should never get here!");
        }
    }

    // send two errors, both will be processed by Obj1
    {
        EventEntrySignalSpy EventSpy;
        EventEntrySignalSpy ErrorSpy;
        ComponentTestEntrySignalSpy CompTestSpy;
        // create first logging object
        Global::LoggingSource LS1(1, 11);
        ErrorProcessor Obj1(NULL, LS1, 2);
        // create second logging object
        Global::LoggingSource LS2(2, 22);
        ErrorProcessor Obj2(&Obj1, LS2, 0);
        ReconnectEmitError(Obj2, Obj1);
        // create third logging object
        Global::LoggingSource LS3(3, 33);
        LoggingObject Obj3(&Obj2, LS3);
        ReconnectEmitError(Obj3, Obj2);
        // attach signal spies
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                                 &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                                 &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)),
                                 &CompTestSpy, SLOT(TheSlot(const DataLogging::ComponentTestEntry &))));
        try {
            Obj3.SendEvent("FileName1", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
                           Global::EVTSTAT_ON, 33, "AdditionalData1");
            Obj3.SendEvent("FileName2", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData2"  << "AdditionalData3");
            Obj3.SendEvent("FileName3", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_UNDEFINED,
                           Global::EVTSTAT_ON, 33, "AdditionalData4");
            Obj3.SendEvent("FileName4", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData5"  << "AdditionalData6");
            QCOMPARE(EventSpy.count(),      4);
            QCOMPARE(ErrorSpy.count(),      0);
        } catch(...) {
            QFAIL("You should never get here!");
        }
    }
}

/****************************************************************************/
void TestLoggingObject::utErrorProcessingDisabledErrorForwarding() {
    // send two errors, none processed
    {
        EventEntrySignalSpy EventSpy;
        EventEntrySignalSpy ErrorSpy;
        ComponentTestEntrySignalSpy CompTestSpy;
        // create first logging object
        Global::LoggingSource LS1(1, 11);
        ErrorProcessor Obj1(NULL, LS1, 0);
        // create second logging object
        Global::LoggingSource LS2(2, 22);
        ErrorProcessor Obj2(&Obj1, LS2, 0);
        ReconnectEmitError(Obj2, Obj1);
        // create third logging object
        Global::LoggingSource LS3(3, 33);
        LoggingObject Obj3(&Obj2, LS3);
        ReconnectEmitError(Obj3, Obj2);
        // disable error forwarding in Obj3
        Obj3.DisableErrorForwarding();
        // attach signal spies
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                                 &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                                 &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)),
                                 &CompTestSpy, SLOT(TheSlot(const DataLogging::ComponentTestEntry &))));
        try {
            Obj3.SendEvent("FileName1", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
                           Global::EVTSTAT_ON, 33, "AdditionalData1");
            Obj3.SendEvent("FileName2", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData2"  << "AdditionalData3");
            Obj3.SendEvent("FileName3", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_UNDEFINED,
                           Global::EVTSTAT_ON, 33, "AdditionalData4");
            Obj3.SendEvent("FileName4", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData5"  << "AdditionalData6");
            QCOMPARE(EventSpy.count(),      4);
            QCOMPARE(ErrorSpy.count(),      0);
        } catch(...) {
            QFAIL("You should never get here!");
        }
    }

    // send two errors, one will be processed by Obj2
    {
        EventEntrySignalSpy EventSpy;
        EventEntrySignalSpy ErrorSpy;
        ComponentTestEntrySignalSpy CompTestSpy;
        // create first logging object
        Global::LoggingSource LS1(1, 11);
        ErrorProcessor Obj1(NULL, LS1, 0);
        // create second logging object
        Global::LoggingSource LS2(2, 22);
        ErrorProcessor Obj2(&Obj1, LS2, 1);
        ReconnectEmitError(Obj2, Obj1);
        // create third logging object
        Global::LoggingSource LS3(3, 33);
        LoggingObject Obj3(&Obj2, LS3);
        ReconnectEmitError(Obj3, Obj2);
        // disable error forwarding in Obj3
        Obj3.DisableErrorForwarding();
        // attach signal spies
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                                 &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                                 &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)),
                                 &CompTestSpy, SLOT(TheSlot(const DataLogging::ComponentTestEntry &))));
        try {
            Obj3.SendEvent("FileName1", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
                           Global::EVTSTAT_ON, 33, "AdditionalData1");
            Obj3.SendEvent("FileName2", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData2"  << "AdditionalData3");
            Obj3.SendEvent("FileName3", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_UNDEFINED,
                           Global::EVTSTAT_ON, 33, "AdditionalData4");
            Obj3.SendEvent("FileName4", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData5"  << "AdditionalData6");
            QCOMPARE(EventSpy.count(),      4);
            QCOMPARE(ErrorSpy.count(),      0);
        } catch(...) {
            QFAIL("You should never get here!");
        }
    }

    // send two errors, one will be processed by Obj1
    {
        EventEntrySignalSpy EventSpy;
        EventEntrySignalSpy ErrorSpy;
        ComponentTestEntrySignalSpy CompTestSpy;
        // create first logging object
        Global::LoggingSource LS1(1, 11);
        ErrorProcessor Obj1(NULL, LS1, 1);
        // create second logging object
        Global::LoggingSource LS2(2, 22);
        ErrorProcessor Obj2(&Obj1, LS2, 0);
        ReconnectEmitError(Obj2, Obj1);
        // create third logging object
        Global::LoggingSource LS3(3, 33);
        LoggingObject Obj3(&Obj2, LS3);
        ReconnectEmitError(Obj3, Obj2);
        // disable error forwarding in Obj3
        Obj3.DisableErrorForwarding();
        // attach signal spies
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                                 &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                                 &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)),
                                 &CompTestSpy, SLOT(TheSlot(const DataLogging::ComponentTestEntry &))));
        try {
            Obj3.SendEvent("FileName1", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
                           Global::EVTSTAT_ON, 33, "AdditionalData1");
            Obj3.SendEvent("FileName2", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData2"  << "AdditionalData3");
            Obj3.SendEvent("FileName3", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_UNDEFINED,
                           Global::EVTSTAT_ON, 33, "AdditionalData4");
            Obj3.SendEvent("FileName4", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData5"  << "AdditionalData6");
            QCOMPARE(EventSpy.count(),      4);
            QCOMPARE(ErrorSpy.count(),      0);
        } catch(...) {
            QFAIL("You should never get here!");
        }
    }

    // send two errors, both will be processed by Obj2
    {
        EventEntrySignalSpy EventSpy;
        EventEntrySignalSpy ErrorSpy;
        ComponentTestEntrySignalSpy CompTestSpy;
        // create first logging object
        Global::LoggingSource LS1(1, 11);
        ErrorProcessor Obj1(NULL, LS1, 0);
        // create second logging object
        Global::LoggingSource LS2(2, 22);
        ErrorProcessor Obj2(&Obj1, LS2, 2);
        ReconnectEmitError(Obj2, Obj1);
        // create third logging object
        Global::LoggingSource LS3(3, 33);
        LoggingObject Obj3(&Obj2, LS3);
        ReconnectEmitError(Obj3, Obj2);
        // disable error forwarding in Obj3
        Obj3.DisableErrorForwarding();
        // attach signal spies
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                                 &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                                 &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)),
                                 &CompTestSpy, SLOT(TheSlot(const DataLogging::ComponentTestEntry &))));
        try {
            Obj3.SendEvent("FileName1", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
                           Global::EVTSTAT_ON, 33, "AdditionalData1");
            Obj3.SendEvent("FileName2", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData2"  << "AdditionalData3");
            Obj3.SendEvent("FileName3", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_UNDEFINED,
                           Global::EVTSTAT_ON, 33, "AdditionalData4");
            Obj3.SendEvent("FileName4", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData5"  << "AdditionalData6");
            QCOMPARE(EventSpy.count(),      4);
            QCOMPARE(ErrorSpy.count(),      0);
        } catch(...) {
            QFAIL("You should never get here!");
        }
    }

    // send two errors, one will be processed by Obj1 and one by Obj2
    {
        EventEntrySignalSpy EventSpy;
        EventEntrySignalSpy ErrorSpy;
        ComponentTestEntrySignalSpy CompTestSpy;
        // create first logging object
        Global::LoggingSource LS1(1, 11);
        ErrorProcessor Obj1(NULL, LS1, 1);
        // create second logging object
        Global::LoggingSource LS2(2, 22);
        ErrorProcessor Obj2(&Obj1, LS2, 1);
        ReconnectEmitError(Obj2, Obj1);
        // create third logging object
        Global::LoggingSource LS3(3, 33);
        LoggingObject Obj3(&Obj2, LS3);
        ReconnectEmitError(Obj3, Obj2);
        // disable error forwarding in Obj3
        Obj3.DisableErrorForwarding();
        // attach signal spies
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                                 &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                                 &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)),
                                 &CompTestSpy, SLOT(TheSlot(const DataLogging::ComponentTestEntry &))));
        try {
            Obj3.SendEvent("FileName1", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
                           Global::EVTSTAT_ON, 33, "AdditionalData1");
            Obj3.SendEvent("FileName2", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData2"  << "AdditionalData3");
            Obj3.SendEvent("FileName3", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_UNDEFINED,
                           Global::EVTSTAT_ON, 33, "AdditionalData4");
            Obj3.SendEvent("FileName4", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData5"  << "AdditionalData6");
            QCOMPARE(EventSpy.count(),      4);
            QCOMPARE(ErrorSpy.count(),      0);
        } catch(...) {
            QFAIL("You should never get here!");
        }
    }

    // send two errors, both will be processed by Obj1
    {
        EventEntrySignalSpy EventSpy;
        EventEntrySignalSpy ErrorSpy;
        ComponentTestEntrySignalSpy CompTestSpy;
        // create first logging object
        Global::LoggingSource LS1(1, 11);
        ErrorProcessor Obj1(NULL, LS1, 2);
        // create second logging object
        Global::LoggingSource LS2(2, 22);
        ErrorProcessor Obj2(&Obj1, LS2, 0);
        ReconnectEmitError(Obj2, Obj1);
        // create third logging object
        Global::LoggingSource LS3(3, 33);
        LoggingObject Obj3(&Obj2, LS3);
        ReconnectEmitError(Obj3, Obj2);
        // disable error forwarding in Obj3
        Obj3.DisableErrorForwarding();
        // attach signal spies
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitEventEntry(const DataLogging::EventEntry &)),
                                 &EventSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitError(const DataLogging::EventEntry &)),
                                 &ErrorSpy, SLOT(TheSlot(const DataLogging::EventEntry &))));
        QVERIFY(QObject::connect(&Obj1, SIGNAL(EmitComponentTestEntry(const DataLogging::ComponentTestEntry &)),
                                 &CompTestSpy, SLOT(TheSlot(const DataLogging::ComponentTestEntry &))));
        try {
            Obj3.SendEvent("FileName1", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_INFO,
                           Global::EVTSTAT_ON, 33, "AdditionalData1");
            Obj3.SendEvent("FileName2", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData2"  << "AdditionalData3");
            Obj3.SendEvent("FileName3", 100, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_UNDEFINED,
                           Global::EVTSTAT_ON, 33, "AdditionalData4");
            Obj3.SendEvent("FileName4", 101, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_ERROR,
                           Global::EVTSTAT_OFF, 34, Global::tTranslatableStringList() << "AdditionalData5"  << "AdditionalData6");
            QCOMPARE(EventSpy.count(),      4);
            QCOMPARE(ErrorSpy.count(),      0);
        } catch(...) {
            QFAIL("You should never get here!");
        }
    }
}

#endif

void TestLoggingObject::utTestNewLoggingRequirement() {
//    Global::InitSupportedLanguages();
//
//    // cleanup translator strings. For event strings.
//    Global::EventTranslator::TranslatorInstance().Reset();
//    // read strings
//    DataManagement::XmlConfigFileStrings TranslatorDataFile;
//    // Create list of used languages. Language and FallbackLanguage can be the same, since we are
//    // working wit a QSet
//    QSet<QLocale::Language> LanguageList;
//    LanguageList << QLocale::English<< QLocale::English;
//    TranslatorDataFile.ReadStrings(":/xml/EventStrings.xml", LanguageList);
//    // check if there is still a language in LanguageList
//    if(!LanguageList.isEmpty()) {
//        // Uh oh... some languages could not be read.
//        // send some error messages.
//        for(QSet<QLocale::Language>::const_iterator it = LanguageList.constBegin(); it != LanguageList.constEnd(); ++it) {
//            //SEND_FATAL_ERROR(DataManagement::EVENT_DATAMANAGEMENT_ERROR_LANG_NOT_FOUND, Global::LanguageToString(*it));
//        }
//    }
//    // now configure translator with read languages.
//    for(Global::tTranslations::const_iterator it = TranslatorDataFile.Data().constBegin();
//        it != TranslatorDataFile.Data().constEnd();
//        ++it)
//    {
//        // Set language data. No default no fallback.
//        Global::EventTranslator::TranslatorInstance().SetLanguageData(it.key(), it.value(), false, false);
//    }
//    // set default language
//    Global::EventTranslator::TranslatorInstance().SetDefaultLanguage(QLocale::English);
//    // set fallback language
//    Global::EventTranslator::TranslatorInstance().SetFallbackLanguage(QLocale::English);
//
//    // create first logging object
//    Global::LoggingSource LS1(1, 11);
//    Global::LoggingSource LS2(2, 10);
//    LoggingObject Obj1(NULL, LS1);
//    LoggingObject Obj2(NULL, LS2);
//    DayOperationLogger DayOperationLogger(&Obj2, LS2);
//    DayOperationLoggerConfig LogConfiguration("Production", "1234", ".", 14);
//    DayOperationLogger.Configure(LogConfiguration);
//    DayOperationEntry Entry(Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVENT_GLOBAL_ERROR_FALLBACK_LANGUAGE,
//                            Global::EVTTYPE_INFO, Global::tTranslatableStringList() << "English","1010", false, LS1);
//
//    DayOperationLogger.Log(Entry);
//    DayOperationEntry Entry2(Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVENT_GLOBAL_POWER_FAIL,
//                            Global::EVTTYPE_INFO, Global::tTranslatableStringList(), "1111", false, LS1);
//    DayOperationLogger.Log(Entry2);
//    DayOperationEntry Entry3(Global::AdjustedTime::Instance().GetCurrentDateTime(),Threads::EVENT_THREADS_INFO_STARTING_STATE_MACHINE,
//                            Global::EVTTYPE_INFO, Global::tTranslatableStringList(), "11", true, LS1);
//    DayOperationLogger.Log(Entry3);
}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestLoggingObject)


#include "TestLoggingObject.moc"
