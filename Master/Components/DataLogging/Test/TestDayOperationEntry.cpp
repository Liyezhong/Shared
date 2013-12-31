/****************************************************************************/
/*! \file TestDayOperationEntry.cpp
 *
 *  \brief Implementation file for class TestDayOperationEntry.
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
#include <DataLogging/Include/DayEventEntry.h>
#include <Global/Include/AdjustedTime.h>
#include <Global/Include/GlobalEventCodes.h>

namespace DataLogging {

static const int DayOpSleepTime = 10;   ///< Sleep time to ensure different time stamps [ms].

/****************************************************************************/
/**
 * \brief Test class for DayEventEntry class.
 */
/****************************************************************************/
class TestDayOperationEntry : public QObject {
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
     * \brief Test constructor
     */
    /****************************************************************************/
    void utConstructor();

    /****************************************************************************/
    /**
     * \brief Test copy constructor and assignment operator.
     */
    /****************************************************************************/
    void utCopy();
    /****************************************************************************/
    /**
     * \brief Test set and get methods.
     */
    /****************************************************************************/
    void utSetAndGet();
}; // end class TestDayOperationEntry

/****************************************************************************/
void TestDayOperationEntry::initTestCase() {
}

/****************************************************************************/
void TestDayOperationEntry::init() {
}

/****************************************************************************/
void TestDayOperationEntry::cleanup() {
}

/****************************************************************************/
void TestDayOperationEntry::cleanupTestCase() {
}

/****************************************************************************/
void TestDayOperationEntry::utConstructor() {
    // test default constructor
    DayEventEntry TestObject1;
    QCOMPARE(TestObject1.m_TimeStamp.isNull(),               false);
    QCOMPARE(TestObject1.m_String.length(),             0);

    QDateTime DateTime = Global::AdjustedTime::Instance().GetCurrentDateTime();
    Global::tTranslatableStringList StringList;
    StringList<< "String1" << "String2";
    DayEventEntry TestObject2(DateTime, StringList);
    QCOMPARE(TestObject2.m_TimeStamp, DateTime);
    QCOMPARE(TestObject2.m_String.count(), StringList.count());

    for (qint32 Counter = 0; Counter < StringList.count(); Counter++) {
        QCOMPARE(((Global::TranslatableString)TestObject2.m_String.value(Counter)).GetString(),
                  ((Global::TranslatableString)StringList.value(Counter)).GetString());
    }

}

/****************************************************************************/
void TestDayOperationEntry::utCopy() {
    // test copy constructor and assignment operator
    QTest::qSleep(DayOpSleepTime);
    QDateTime TimeStamp1 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    QTest::qSleep(DayOpSleepTime);
    QDateTime TimeStamp2 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    Global::TranslatableString TS1(1, Global::tTranslatableStringList() << "D11" << "D12");
    Global::TranslatableString TS2(2, Global::tTranslatableStringList() << "D21" << "D22" << "D23");

      //DayEventEntry TestObject1(TimeStamp1, TS1);
    //DayEventEntry TestObject2(TimeStamp2, TS2);
    //DayEventEntry TestObject3(TestObject1);
    //DayEventEntry TestObject4(TestObject2);
//    DayEventEntry TestObject5;
//    TestObject5 = TestObject1;
//    DayEventEntry TestObject6;
//    TestObject6 = TestObject2;
#if 0
    //Commented by NKN
    QCOMPARE(TestObject1.m_TimeStamp,                       TimeStamp1);
    //QCOMPARE(TestObject1.m_String.GetStringID(),            quint32(1));
    //QCOMPARE(TestObject1.m_String.GetArgumentList().size(), 2);

    QCOMPARE(TestObject2.m_TimeStamp,                       TimeStamp2);
    //QCOMPARE(TestObject2.m_String.GetStringID(),            quint32(2));
    //QCOMPARE(TestObject2.m_String.GetArgumentList().size(), 3);

    QCOMPARE(TestObject3.m_TimeStamp,                       TestObject1.m_TimeStamp);
    //QCOMPARE(TestObject3.m_String.GetStringID(),            TestObject1.m_String.GetStringID());
    //QCOMPARE(TestObject3.m_String.GetArgumentList().size(), TestObject1.m_String.GetArgumentList().size());

    QCOMPARE(TestObject4.m_TimeStamp,                       TestObject2.m_TimeStamp);
    //QCOMPARE(TestObject4.m_String.GetStringID(),            TestObject2.m_String.GetStringID());
    //QCOMPARE(TestObject4.m_String.GetArgumentList().size(), TestObject2.m_String.GetArgumentList().size());

    QCOMPARE(TestObject5.m_TimeStamp,                       TestObject1.m_TimeStamp);
    //QCOMPARE(TestObject5.m_String.GetStringID(),            TestObject1.m_String.GetStringID());
    //QCOMPARE(TestObject5.m_String.GetArgumentList().size(), TestObject1.m_String.GetArgumentList().size());

    QCOMPARE(TestObject6.m_TimeStamp,                       TestObject2.m_TimeStamp);
    //QCOMPARE(TestObject6.m_String.GetStringID(),            TestObject2.m_String.GetStringID());
    //QCOMPARE(TestObject6.m_String.GetArgumentList().size(), TestObject2.m_String.GetArgumentList().size());

    // assign to self
    TestObject6 = TestObject6;
    QCOMPARE(TestObject6.m_TimeStamp,                       TestObject2.m_TimeStamp);
    //QCOMPARE(TestObject6.m_String.GetStringID(),            TestObject2.m_String.GetStringID());
    //QCOMPARE(TestObject6.m_String.GetArgumentList().size(), TestObject2.m_String.GetArgumentList().size());
#endif
}

/****************************************************************************/
void TestDayOperationEntry::utSetAndGet() {
    QDateTime DateTime = Global::AdjustedTime::Instance().GetCurrentDateTime();
    Global::tTranslatableStringList StringList;
    StringList<< "String1" << "String2";
    EventHandler::EventCSVInfo CSV;
    Global::LoggingSource LogSource;
    CSV.SetActionNegative(Global::ACNTYPE_STOP);
    CSV.SetActionPositive(Global::ACNTYPE_IDLE);
    CSV.SetAlarmStatus(false);
    CSV.SetButtonType(Global::OK);

    CSV.SetEventName("Test event");
    CSV.SetEventSource(Global::EVENTSOURCE_NONE);
    CSV.SetEventType(Global::EVTTYPE_INFO);
    CSV.SetFinalAction(Global::ACNTYPE_SHUTDOWN);
    CSV.SetGUIOptions(Global::YES_NO);
    CSV.SetLogLevel(Global::LOGLEVEL_NONE);
    CSV.SetMessageType("Info");
    CSV.SetRetries(3);
    CSV.SetRunLogStatus(true);
    CSV.SetSource(LogSource);
    CSV.SetStatusBarIcon(true);
    CSV.SetStatusIcon(false);

    DayEventEntry TestObject(DateTime, StringList);
    TestObject.SetEventCSVInfo(CSV);
    TestObject.SetAltStringUsage(Global::NOT_APPLICABLE);
    TestObject.SetDateTime(DateTime);
    TestObject.SetEventKey(1234);
    TestObject.SetTranslatableStringList(StringList);
    TestObject.SetEventCode(5678);
    TestObject.SetAckReqStatus(false);


    QCOMPARE(TestObject.GetAltStringUsageType(), Global::NOT_APPLICABLE);
    QCOMPARE(TestObject.GetTimeStamp(), DateTime);
    QCOMPARE(TestObject.GetString().count(), StringList.count());
    QCOMPARE(TestObject.GetAckValue(), NetCommands::NOT_SPECIFIED);
    QCOMPARE(TestObject.GetEventKey(), quint32(1234));
    QCOMPARE(TestObject.IsEventActive(), false);

    QCOMPARE(TestObject.GetShowInRunLogStatus(), true);
    QCOMPARE(TestObject.GetEventType(), Global::EVTTYPE_INFO);
    QCOMPARE(TestObject.GetEventId(), quint32(5678));
    QCOMPARE(TestObject.GetEventName(), QString("Test event"));
    QCOMPARE(TestObject.GetLogLevel(), Global::LOGLEVEL_NONE);
    QCOMPARE(TestObject.GetAlarmStatus(), false);
    QCOMPARE(TestObject.GetRetryAttempts(), 3);
    QCOMPARE(TestObject.GetActionNegative(), Global::ACNTYPE_STOP);
    QCOMPARE(TestObject.GetActionPositive(), Global::ACNTYPE_IDLE);
    QCOMPARE(TestObject.GetFinalAction(), Global::ACNTYPE_SHUTDOWN);
    QCOMPARE(TestObject.GetStatusIcon(), false);
    QCOMPARE(TestObject.GetAckReqStatus(), false);
    QCOMPARE(TestObject.GetActionType(),  Global::ACNTYPE_SHUTDOWN);
    QCOMPARE(TestObject.GetButtonType(), Global::YES_NO);
    QCOMPARE(TestObject.GetGUIMessageBoxOptions(), Global::YES_NO);
    QCOMPARE(TestObject.GetSourceComponent(), Global::EVENTSOURCE_NONE);



#if 0
    QCOMPARE(TestObject1.GetTimeStamp(),                    TimeStamp1);
    //QCOMPARE(TestObject1.m_String.GetStringID(),            quint32(1));
    //QCOMPARE(TestObject1.m_String.GetArgumentList().size(), 2);
    QCOMPARE(TestObject2.GetTimeStamp(),                    TimeStamp2);
    //QCOMPARE(TestObject2.m_String.GetStringID(),            quint32(2));
    //QCOMPARE(TestObject2.m_String.GetArgumentList().size(), 3);
    QCOMPARE(TestObject3.GetTimeStamp(),                    TimeStamp1);
    //QCOMPARE(TestObject3.m_String.GetStringID(),            quint32(1));
    //QCOMPARE(TestObject3.m_String.GetArgumentList().size(), 2);
    QCOMPARE(TestObject4.GetTimeStamp(),                    TimeStamp2);
    //QCOMPARE(TestObject4.m_String.GetStringID(),            quint32(2));
    //QCOMPARE(TestObject4.m_String.GetArgumentList().size(), 3);
#endif
}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestDayOperationEntry)

#include "TestDayOperationEntry.moc"
