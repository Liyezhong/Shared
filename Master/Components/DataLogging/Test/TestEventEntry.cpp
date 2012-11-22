/****************************************************************************/
/*! \file TestEventEntry.cpp
 *
 *  \brief Implementation file for class TestEventEntry.
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

#include <QTest>
#include <DataLogging/Include/EventEntry.h>
#include <Global/Include/GlobalEventCodes.h>
#include <Global/Include/LoggingSource.h>
#include <Global/Include/AdjustedTime.h>

namespace DataLogging {

static const int EventEntrySleepTime = 10;      ///< Sleep time to ensure different time stamps [ms].
static const Global::LoggingSource SRC0;        ///< A event source.
static const Global::LoggingSource SRC1(1,2);   ///< A event source.
static const Global::LoggingSource SRC2(3,4);   ///< Another event source.
static const Global::LoggingSource SRC3(5,6);   ///< Another event source.
static const Global::LoggingSource SRC4(7,8);   ///< Another event source.

/****************************************************************************/
/**
 * \brief Test class for EventEntry class.
 */
/****************************************************************************/
class TestEventEntry : public QObject {
    Q_OBJECT
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
    /****************************************************************************/
    /**
     * \brief Test default constructor
     */
    /****************************************************************************/
    void utDefaultConstructor();
    /****************************************************************************/
    /**
     * \brief Test constructor without additional data list
     */
    /****************************************************************************/
    void utConstructorNoData();
    /****************************************************************************/
    /**
     * \brief Test constructor with additional data list
     */
    /****************************************************************************/
    void utConstructorWithData();
    /****************************************************************************/
    /**
     * \brief Test copy constructor.
     */
    /****************************************************************************/
    void utCopyConstructor();
    /****************************************************************************/
    /**
     * \brief Test assignment operator.
     */
    /****************************************************************************/
    void utAssignmentOperator();
    /****************************************************************************/
    /**
     * \brief Test get methods.
     */
    /****************************************************************************/
    void utGet();
}; // end class TestEventEntry

/****************************************************************************/
void TestEventEntry::initTestCase() {
}

/****************************************************************************/
void TestEventEntry::init() {
}

/****************************************************************************/
void TestEventEntry::cleanup() {
}

/****************************************************************************/
void TestEventEntry::cleanupTestCase() {
}

/****************************************************************************/
void TestEventEntry::utDefaultConstructor() {
    // test default constructor
    EventEntry TestObject;
    QCOMPARE(TestObject.m_TimeStamp.isNull(),       true);
    QCOMPARE(TestObject.m_Source,                   SRC0);
    QCOMPARE(TestObject.m_EventType,                Global::EVTTYPE_UNDEFINED);
    QCOMPARE(TestObject.m_EventStatus,              Global::EVTSTAT_ON);
    QCOMPARE(TestObject.m_EventCode,                Global::EVENT_GLOBAL_UNDEFINED);
    QCOMPARE(TestObject.m_AdditionalData.size(),    0);
}

/****************************************************************************/
void TestEventEntry::utConstructorNoData() {
    // test constructor without additional data
    QTest::qSleep(EventEntrySleepTime);
    QDateTime TimeStamp1 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    QTest::qSleep(EventEntrySleepTime);
    QDateTime TimeStamp2 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    EventEntry TestObject1(TimeStamp1, SRC1, Global::EVTTYPE_ERROR,   Global::EVTSTAT_ON,  1);
    EventEntry TestObject2(TimeStamp2, SRC2, Global::EVTTYPE_WARNING, Global::EVTSTAT_OFF, 2);

    QCOMPARE(TestObject1.m_TimeStamp,               TimeStamp1);
    QCOMPARE(TestObject1.m_Source,                  SRC1);
    QCOMPARE(TestObject1.m_EventType,               Global::EVTTYPE_ERROR);
    QCOMPARE(TestObject1.m_EventStatus,             Global::EVTSTAT_ON);
    QCOMPARE(TestObject1.m_EventCode,               quint32(1));
    QCOMPARE(TestObject1.m_AdditionalData.size(),   0);

    QCOMPARE(TestObject2.m_TimeStamp,               TimeStamp2);
    QCOMPARE(TestObject2.m_Source,                  SRC2);
    QCOMPARE(TestObject2.m_EventType,               Global::EVTTYPE_WARNING);
    QCOMPARE(TestObject2.m_EventStatus,             Global::EVTSTAT_OFF);
    QCOMPARE(TestObject2.m_EventCode,               quint32(2));
    QCOMPARE(TestObject2.m_AdditionalData.size(),   0);
}

/****************************************************************************/
void TestEventEntry::utConstructorWithData() {
    // test constructor with additional data
    QTest::qSleep(EventEntrySleepTime);
    QDateTime TimeStamp1 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    QTest::qSleep(EventEntrySleepTime);
    QDateTime TimeStamp2 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    Global::tTranslatableStringList AdditionalData1 = Global::tTranslatableStringList() << "D11" << "D12";
    Global::tTranslatableStringList AdditionalData2 = Global::tTranslatableStringList() << "D21" << "D22" << "D23";
    EventEntry TestObject1(TimeStamp1, SRC1, Global::EVTTYPE_ERROR,   Global::EVTSTAT_ON,  1, AdditionalData1);
    EventEntry TestObject2(TimeStamp2, SRC2, Global::EVTTYPE_WARNING, Global::EVTSTAT_OFF, 2, AdditionalData2);

    QCOMPARE(TestObject1.m_TimeStamp,               TimeStamp1);
    QCOMPARE(TestObject1.m_Source,                  SRC1);
    QCOMPARE(TestObject1.m_EventType,               Global::EVTTYPE_ERROR);
    QCOMPARE(TestObject1.m_EventStatus,             Global::EVTSTAT_ON);
    QCOMPARE(TestObject1.m_EventCode,               quint32(1));
    QCOMPARE(TestObject1.m_AdditionalData.size(),   2);

    QCOMPARE(TestObject2.m_TimeStamp,               TimeStamp2);
    QCOMPARE(TestObject2.m_Source,                  SRC2);
    QCOMPARE(TestObject2.m_EventType,               Global::EVTTYPE_WARNING);
    QCOMPARE(TestObject2.m_EventStatus,             Global::EVTSTAT_OFF);
    QCOMPARE(TestObject2.m_EventCode,               quint32(2));
    QCOMPARE(TestObject2.m_AdditionalData.size(),   3);
}

/****************************************************************************/
void TestEventEntry::utCopyConstructor() {
    // test copy constructor
    QTest::qSleep(EventEntrySleepTime);
    QDateTime TimeStamp1 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    QTest::qSleep(EventEntrySleepTime);
    QDateTime TimeStamp2 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    Global::tTranslatableStringList AdditionalData2 = Global::tTranslatableStringList() << "D21" << "D22" << "D23";
    EventEntry TestObject1(TimeStamp1, SRC1, Global::EVTTYPE_ERROR,   Global::EVTSTAT_ON,  1);
    EventEntry TestObject2(TimeStamp2, SRC2, Global::EVTTYPE_WARNING, Global::EVTSTAT_OFF, 2, AdditionalData2);
    EventEntry TestObject3(TestObject1);
    EventEntry TestObject4(TestObject2);

    QCOMPARE(TestObject3.m_TimeStamp,               TimeStamp1);
    QCOMPARE(TestObject3.m_Source,                  SRC1);
    QCOMPARE(TestObject3.m_EventType,               Global::EVTTYPE_ERROR);
    QCOMPARE(TestObject3.m_EventStatus,             Global::EVTSTAT_ON);
    QCOMPARE(TestObject3.m_EventCode,               quint32(1));
    QCOMPARE(TestObject3.m_AdditionalData.size(),   0);

    QCOMPARE(TestObject4.m_TimeStamp,               TimeStamp2);
    QCOMPARE(TestObject4.m_Source,                  SRC2);
    QCOMPARE(TestObject4.m_EventType,               Global::EVTTYPE_WARNING);
    QCOMPARE(TestObject4.m_EventStatus,             Global::EVTSTAT_OFF);
    QCOMPARE(TestObject4.m_EventCode,               quint32(2));
    QCOMPARE(TestObject4.m_AdditionalData.size(),   3);
}

/****************************************************************************/
void TestEventEntry::utAssignmentOperator() {
    QTest::qSleep(EventEntrySleepTime);
    QDateTime TimeStamp1 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    QTest::qSleep(EventEntrySleepTime);
    QDateTime TimeStamp2 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    QTest::qSleep(EventEntrySleepTime);
    QDateTime TimeStamp3 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    QTest::qSleep(EventEntrySleepTime);
    QDateTime TimeStamp4 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    Global::tTranslatableStringList AdditionalData2 = Global::tTranslatableStringList() << "D21" << "D22" << "D23";
    Global::tTranslatableStringList AdditionalData3 = Global::tTranslatableStringList() << "D41" << "D42";
    EventEntry TestObject1(TimeStamp1, SRC1, Global::EVTTYPE_ERROR,   Global::EVTSTAT_ON,  1);
    EventEntry TestObject2(TimeStamp2, SRC2, Global::EVTTYPE_WARNING, Global::EVTSTAT_OFF, 2, AdditionalData2);
    EventEntry TestObject3(TimeStamp3, SRC3, Global::EVTTYPE_INFO,    Global::EVTSTAT_OFF, 3, AdditionalData3);
    EventEntry TestObject4(TimeStamp4, SRC4, Global::EVTTYPE_DEBUG,   Global::EVTSTAT_ON,  4);

    QCOMPARE(TestObject3.m_TimeStamp,               TimeStamp3);
    QCOMPARE(TestObject3.m_Source,                  SRC3);
    QCOMPARE(TestObject3.m_EventType,               Global::EVTTYPE_INFO);
    QCOMPARE(TestObject3.m_EventStatus,             Global::EVTSTAT_OFF);
    QCOMPARE(TestObject3.m_EventCode,               quint32(3));
    QCOMPARE(TestObject3.m_AdditionalData.size(),   2);

    QCOMPARE(TestObject4.m_TimeStamp,               TimeStamp4);
    QCOMPARE(TestObject4.m_Source,                  SRC4);
    QCOMPARE(TestObject4.m_EventType,               Global::EVTTYPE_DEBUG);
    QCOMPARE(TestObject4.m_EventStatus,             Global::EVTSTAT_ON);
    QCOMPARE(TestObject4.m_EventCode,               quint32(4));
    QCOMPARE(TestObject4.m_AdditionalData.size(),   0);

    // now assign data
    TestObject3 = TestObject1;
    TestObject4 = TestObject2;

    QCOMPARE(TestObject3.m_TimeStamp,               TimeStamp1);
    QCOMPARE(TestObject3.m_Source,                  SRC1);
    QCOMPARE(TestObject3.m_EventType,               Global::EVTTYPE_ERROR);
    QCOMPARE(TestObject3.m_EventStatus,             Global::EVTSTAT_ON);
    QCOMPARE(TestObject3.m_EventCode,               quint32(1));
    QCOMPARE(TestObject3.m_AdditionalData.size(),   0);

    QCOMPARE(TestObject4.m_TimeStamp,               TimeStamp2);
    QCOMPARE(TestObject4.m_Source,                  SRC2);
    QCOMPARE(TestObject4.m_EventType,               Global::EVTTYPE_WARNING);
    QCOMPARE(TestObject4.m_EventStatus,             Global::EVTSTAT_OFF);
    QCOMPARE(TestObject4.m_EventCode,               quint32(2));
    QCOMPARE(TestObject4.m_AdditionalData.size(),   3);

    // assign to self
    TestObject4 = TestObject4;
    QCOMPARE(TestObject4.m_TimeStamp,               TimeStamp2);
    QCOMPARE(TestObject4.m_Source,                  SRC2);
    QCOMPARE(TestObject4.m_EventType,               Global::EVTTYPE_WARNING);
    QCOMPARE(TestObject4.m_EventStatus,             Global::EVTSTAT_OFF);
    QCOMPARE(TestObject4.m_EventCode,               quint32(2));
    QCOMPARE(TestObject4.m_AdditionalData.size(),   3);
}

/****************************************************************************/
void TestEventEntry::utGet() {
    // test get methods
    QTest::qSleep(EventEntrySleepTime);
    QDateTime TimeStamp1 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    QTest::qSleep(EventEntrySleepTime);
    QDateTime TimeStamp2 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    Global::tTranslatableStringList AdditionalData2 = Global::tTranslatableStringList() << "D21" << "D22" << "D23";
    EventEntry TestObject1(TimeStamp1, SRC1, Global::EVTTYPE_ERROR,   Global::EVTSTAT_ON,  1);
    EventEntry TestObject2(TimeStamp2, SRC2, Global::EVTTYPE_WARNING, Global::EVTSTAT_OFF, 2, AdditionalData2);

    QCOMPARE(TestObject1.GetTimeStamp(),                TimeStamp1);
    QCOMPARE(TestObject1.GetSource(),                   SRC1);
    QCOMPARE(TestObject1.GetEventType(),                Global::EVTTYPE_ERROR);
    QCOMPARE(TestObject1.GetEventStatus(),              Global::EVTSTAT_ON);
    QCOMPARE(TestObject1.GetEventCode(),                quint32(1));
    QCOMPARE(TestObject1.GetAdditionalData().size(),    0);

    QCOMPARE(TestObject2.GetTimeStamp(),                TimeStamp2);
    QCOMPARE(TestObject2.GetSource(),                   SRC2);
    QCOMPARE(TestObject2.GetEventType(),                Global::EVTTYPE_WARNING);
    QCOMPARE(TestObject2.GetEventStatus(),              Global::EVTSTAT_OFF);
    QCOMPARE(TestObject2.GetEventCode(),                quint32(2));
    QCOMPARE(TestObject2.GetAdditionalData().size(),    3);
}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestEventEntry)

#include "TestEventEntry.moc"
