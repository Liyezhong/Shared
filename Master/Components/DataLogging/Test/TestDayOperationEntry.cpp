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
     * \brief Test constructor with translatable string
     */
    /****************************************************************************/
    void utConstructorWithTranslatableString();
    /****************************************************************************/
    /**
     * \brief Test copy constructor and assignment operator.
     */
    /****************************************************************************/
    void utCopy();
    /****************************************************************************/
    /**
     * \brief Test get methods.
     */
    /****************************************************************************/
    void utGet();
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
void TestDayOperationEntry::utDefaultConstructor() {
    // test default constructor
    DayEventEntry TestObject;
    QCOMPARE(TestObject.m_TimeStamp.isNull(),               true);
    //QCOMPARE(TestObject.m_String.GetStringID(),             Global::EVENT_GLOBAL_UNKNOWN_STRING_ID);
    //QCOMPARE(TestObject.m_String.GetArgumentList().size(),  0);
}

/****************************************************************************/
void TestDayOperationEntry::utConstructorNoData() {
    // test constructor without additional data
//    QTest::qSleep(DayOpSleepTime);
//    QDateTime TimeStamp1 = Global::AdjustedTime::Instance().GetCurrentDateTime();
//    QTest::qSleep(DayOpSleepTime);
//    QDateTime TimeStamp2 = Global::AdjustedTime::Instance().GetCurrentDateTime();

    // create objects
    //DayEventEntry TestObject1(TimeStamp1, 1);
    //DayEventEntry TestObject2(TimeStamp2, 2);

    // check data
    //QCOMPARE(TestObject1.m_TimeStamp,                       TimeStamp1);
    //QCOMPARE(TestObject1.m_String.GetStringID(),            quint32(1));
    //QCOMPARE(TestObject1.m_String.GetArgumentList().size(), 0);

    //QCOMPARE(TestObject2.m_TimeStamp,                       TimeStamp2);
    //QCOMPARE(TestObject2.m_String.GetStringID(),            quint32(2));
    //QCOMPARE(TestObject2.m_String.GetArgumentList().size(), 0);
}

/****************************************************************************/
void TestDayOperationEntry::utConstructorWithTranslatableString() {
    // test constructor with additional data
    QTest::qSleep(DayOpSleepTime);
    QDateTime TimeStamp1 = Global::AdjustedTime::Instance().GetCurrentDateTime();
    QTest::qSleep(DayOpSleepTime);
    QDateTime TimeStamp2 = Global::AdjustedTime::Instance().GetCurrentDateTime();

    Global::tTranslatableStringList AdditionalData1 = Global::tTranslatableStringList() << "D11" << "D12";
    Global::tTranslatableStringList AdditionalData2 = Global::tTranslatableStringList() << "D21" << "D22" << "D23";
    Global::TranslatableString String1(1, AdditionalData1);
    Global::TranslatableString String2(2, AdditionalData2);

    //DayEventEntry TestObject1(TimeStamp1, String1);
    //DayEventEntry TestObject2(TimeStamp2, String2);

    //QCOMPARE(TestObject1.m_TimeStamp,                       TimeStamp1);
    //QCOMPARE(TestObject1.m_String.GetStringID(),            quint32(1));
    //QCOMPARE(TestObject1.m_String.GetArgumentList().size(), 2);

    //QCOMPARE(TestObject2.m_TimeStamp,                       TimeStamp2);
    //QCOMPARE(TestObject2.m_String.GetStringID(),            quint32(2));
    //QCOMPARE(TestObject2.m_String.GetArgumentList().size(), 3);
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
void TestDayOperationEntry::utGet() {
    // test get methods

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
