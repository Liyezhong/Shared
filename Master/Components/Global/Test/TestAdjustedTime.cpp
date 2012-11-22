/****************************************************************************/
/*! \file TestAdjustedTime.cpp
 *
 *  \brief Implementation file for class TestAdjustedTime.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-01-25
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
#include <Global/Include/AdjustedTime.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Test class for AdjustedTime class.
 */
/****************************************************************************/
class TestAdjustedTime : public QObject {
    Q_OBJECT
private:
    /****************************************************************************/
    /**
     * \brief Helper function to test difference in seconds for datetime
     *
     * \param   TheOffset   Offset in seconds.
     */
    /****************************************************************************/
    void TestDiffDateTime(int TheOffset);
    /****************************************************************************/
    /**
     * \brief Helper function to test difference in days for date
     *
     * \param   TheOffsetDays   Offset in days.
     */
    /****************************************************************************/
    void TestDiffDate(int TheOffsetDays);
    /****************************************************************************/
    /**
     * \brief Helper function to test difference in seconds for time
     *
     * \param   TheOffset   Offset in seconds.
     */
    /****************************************************************************/
    void TestDiffTime(int TheOffset);
    /****************************************************************************/
    /**
     * \brief Helper function to test adjust to date
     *
     * \param   NewDateTime     New datetime.
     */
    /****************************************************************************/
    void TestAdjust(const QDateTime &NewDateTime);
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
     * \brief Test constructor.
     */
    /****************************************************************************/
    void utConstructor();
    /****************************************************************************/
    /**
     * \brief Test get and set OffsetSeconds methods.
     */
    /****************************************************************************/
    void utGetSetOffsetSeconds();
    /****************************************************************************/
    /**
     * \brief Test GetCurrentDateTime.
     */
    /****************************************************************************/
    void utGetCurrentDateTime();
    /****************************************************************************/
    /**
     * \brief Test GetCurrentDate.
     */
    /****************************************************************************/
    void utGetCurrentDate();
    /****************************************************************************/
    /**
     * \brief Test GetCurrentTime.
     */
    /****************************************************************************/
    void utGetCurrentTime();
    /****************************************************************************/
    /**
     * \brief Test AdjustToDateTime.
     */
    /****************************************************************************/
    void utAdjustToDateTime();
    /****************************************************************************/
    /**
     * \brief Test ComputeOffsetSeconds.
     */
    /****************************************************************************/
    void ComputeOffsetSeconds();
}; // end class TestAdjustedTime

/****************************************************************************/
void TestAdjustedTime::initTestCase() {
}

/****************************************************************************/
void TestAdjustedTime::init() {
}

/****************************************************************************/
void TestAdjustedTime::cleanup() {
}

/****************************************************************************/
void TestAdjustedTime::cleanupTestCase() {
}

/****************************************************************************/
void TestAdjustedTime::utConstructor() {
    // test constructor
    QCOMPARE(AdjustedTime::Instance().m_OffsetSeconds,      0);
}

/****************************************************************************/
void TestAdjustedTime::utGetSetOffsetSeconds() {
    AdjustedTime::Instance().m_OffsetSeconds = 0;
    QCOMPARE(AdjustedTime::Instance().m_OffsetSeconds,      0);
    QCOMPARE(AdjustedTime::Instance().GetOffsetSeconds(),   AdjustedTime::Instance().m_OffsetSeconds);
    AdjustedTime::Instance().SetOffsetSeconds(1);
    QCOMPARE(AdjustedTime::Instance().m_OffsetSeconds,      1);
    QCOMPARE(AdjustedTime::Instance().GetOffsetSeconds(),   AdjustedTime::Instance().m_OffsetSeconds);
    AdjustedTime::Instance().SetOffsetSeconds(-1);
    QCOMPARE(AdjustedTime::Instance().m_OffsetSeconds,      -1);
    QCOMPARE(AdjustedTime::Instance().GetOffsetSeconds(),   AdjustedTime::Instance().m_OffsetSeconds);
    AdjustedTime::Instance().SetOffsetSeconds(1000000);
    QCOMPARE(AdjustedTime::Instance().m_OffsetSeconds,      1000000);
    QCOMPARE(AdjustedTime::Instance().GetOffsetSeconds(),   AdjustedTime::Instance().m_OffsetSeconds);
    AdjustedTime::Instance().SetOffsetSeconds(-1000000);
    QCOMPARE(AdjustedTime::Instance().m_OffsetSeconds,      -1000000);
    QCOMPARE(AdjustedTime::Instance().GetOffsetSeconds(),   AdjustedTime::Instance().m_OffsetSeconds);
}

/****************************************************************************/
void TestAdjustedTime::TestDiffDateTime(int TheOffset) {
    AdjustedTime::Instance().m_OffsetSeconds = TheOffset;
    QDateTime Date1 = QDateTime::currentDateTime().toUTC();
    QDateTime Date = AdjustedTime::Instance().GetCurrentDateTime();
    QDateTime Date2 = QDateTime::currentDateTime().toUTC();
    // at least one of them has to fit
    QVERIFY((Date1.secsTo(Date) == TheOffset) || (Date2.secsTo(Date) == TheOffset));
}

/****************************************************************************/
void TestAdjustedTime::TestDiffDate(int TheOffsetDays) {
    AdjustedTime::Instance().m_OffsetSeconds = TheOffsetDays*24*3600;
    QDate Date1 = QDateTime::currentDateTime().toUTC().date().addDays(TheOffsetDays);
    QDate Date = AdjustedTime::Instance().GetCurrentDate();
    QDate Date2 = QDateTime::currentDateTime().toUTC().date().addDays(TheOffsetDays);;
    // at least one of them has to fit
    QVERIFY((Date1 == Date) || (Date2 == Date));
}

/****************************************************************************/
void TestAdjustedTime::TestDiffTime(int TheOffset) {
    AdjustedTime::Instance().m_OffsetSeconds = TheOffset;
    QTime Time1 = QDateTime::currentDateTime().toUTC().time();
    QTime Time = AdjustedTime::Instance().GetCurrentTime();
    QTime Time2 = QDateTime::currentDateTime().toUTC().time();
    // at least one of them has to fit.
    int Secs1 = Time1.secsTo(Time);
    while(Secs1 < 0) {
        Secs1 += 24*3600;
    }
    int Secs2 = Time2.secsTo(Time);
    while(Secs2 < 0) {
        Secs2 += 24*3600;
    }
    int ModOffset = TheOffset;
    while(ModOffset < 0) {
        ModOffset += 24*3600;
    }
    while(ModOffset >= 24*3600) {
        ModOffset -= 24*3600;
    }
    QVERIFY((Secs1 == ModOffset) || (Secs2 == ModOffset));
}

/****************************************************************************/
void TestAdjustedTime::TestAdjust(const QDateTime &NewDateTime) {
    AdjustedTime::Instance().m_OffsetSeconds = 0;
    QCOMPARE(AdjustedTime::Instance().m_OffsetSeconds, 0);
    QDateTime Date1 = QDateTime::currentDateTime().toUTC();
    AdjustedTime::Instance().AdjustToDateTime(NewDateTime);
    QDateTime Date2 = QDateTime::currentDateTime().toUTC();
    QVERIFY((AdjustedTime::Instance().m_OffsetSeconds == Date1.secsTo(NewDateTime)) ||
            (AdjustedTime::Instance().m_OffsetSeconds == Date2.secsTo(NewDateTime)));
}

/****************************************************************************/
void TestAdjustedTime::utGetCurrentDateTime() {
    // offset = 0
    TestDiffDateTime(0);
    // offset = 1
    TestDiffDateTime(1);
    // offset = -1
    TestDiffDateTime(-1);
    // offset = 1000000
    TestDiffDateTime(1000000);
    // offset = -1000000
    TestDiffDateTime(-1000000);
}

/****************************************************************************/
void TestAdjustedTime::utGetCurrentDate() {
    // offset = 0
    TestDiffDate(0);
    // offset = +1 day
    TestDiffDate(1);
    // offset = -1 day
    TestDiffDate(-1);
    // offset = +100 days
    TestDiffDate(100);
    // offset = -100 days
    TestDiffDate(-100);
    // offset = +365 days
    TestDiffDate(365);
    // offset = -365 days
    TestDiffDate(-365);
    // offset = +366 days
    TestDiffDate(366);
    // offset = -366 days
    TestDiffDate(-366);
}

/****************************************************************************/
void TestAdjustedTime::utGetCurrentTime() {
    // offset = 0
    TestDiffTime(0);
    // offset = 1
    TestDiffTime(1);
    // offset = -1
    TestDiffTime(-1);
    // offset = 1000000
    TestDiffTime(1000000);
    // offset = -1000000
    TestDiffTime(-1000000);
    // offset = 1 day
    TestDiffTime(24*3600);
    // offset = -1 day
    TestDiffTime(-24*3600);
    // offset = 1 day 1 hour 1 second
    TestDiffTime(24*3600 + 3600 + 1);
    // offset = -1 day 1 hour 1 second
    TestDiffTime(-24*3600 - 3600 - 1);
}

/****************************************************************************/
void TestAdjustedTime::utAdjustToDateTime() {
    // current
    TestAdjust(QDateTime::currentDateTime().toUTC());
    // -1 second
    TestAdjust(QDateTime::currentDateTime().toUTC().addSecs(-1));
    // +1 second
    TestAdjust(QDateTime::currentDateTime().toUTC().addSecs(+1));
    // -1 day
    TestAdjust(QDateTime::currentDateTime().toUTC().addDays(-1));
    // +1 day
    TestAdjust(QDateTime::currentDateTime().toUTC().addDays(+1));
    // -1 year
    TestAdjust(QDateTime::currentDateTime().toUTC().addYears(-1));
    // +1 year
    TestAdjust(QDateTime::currentDateTime().toUTC().addYears(+1));
}

/****************************************************************************/
void TestAdjustedTime::ComputeOffsetSeconds() {
    QDateTime Date1 = QDateTime::currentDateTime().toUTC();
    QDateTime Date2 = Date1.addSecs(0);
    QDateTime Date3 = Date1.addSecs(-10);
    QDateTime Date4 = Date1.addSecs(10);
    AdjustedTime::Instance().AdjustToDateTime(Date1);
    QVERIFY(AdjustedTime::Instance().ComputeOffsetSeconds(Date2) >=  -1);
    QVERIFY(AdjustedTime::Instance().ComputeOffsetSeconds(Date2) <=   1);
    QVERIFY(AdjustedTime::Instance().ComputeOffsetSeconds(Date3) >= -11);
    QVERIFY(AdjustedTime::Instance().ComputeOffsetSeconds(Date3) <=  -9);
    QVERIFY(AdjustedTime::Instance().ComputeOffsetSeconds(Date4) >=   9);
    QVERIFY(AdjustedTime::Instance().ComputeOffsetSeconds(Date4) <=  11);
}

} // end namespace Global

QTEST_MAIN(Global::TestAdjustedTime)

#include "TestAdjustedTime.moc"
