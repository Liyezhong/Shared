/****************************************************************************/
/*! \file TestMonotonicTime.cpp
 *
 *  \brief Implementation file for class MonotonicTime.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-09-20
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
#include <Global/Include/MonotonicTime.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Test class for MonotonicTime class.
 *
 * Since the used moc does not take into consideration the defined Q_OS_*,
 * we have to switch arond in the corresponding utTest methods... :-(
 */
/****************************************************************************/
class TestMonotonicTime : public QObject {
    Q_OBJECT
private:
    /****************************************************************************/
    /**
     * \brief Test of constructors for specific OS.
     */
    /****************************************************************************/
    void utTestConstructorsOSSpecific();
    /****************************************************************************/
    /**
     * \brief Test of the assignment operator for specific OS.
     */
    /****************************************************************************/
    void utTestAssignmentOperatorOSSpecific();
    /****************************************************************************/
    /**
     * \brief Test of the comparison operator for specific OS.
     */
    /****************************************************************************/
    void utTestComparisonOSSpecific();
private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     *
     * Here is the only place where we can test the default constructor.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
     *
     * Reset translator to a defined state.
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
     * \brief Test of constructors.
     */
    /****************************************************************************/
    void utTestConstructors();
    /****************************************************************************/
    /**
     * \brief Test of the assignment operator.
     */
    /****************************************************************************/
    void utTestAssignmentOperator();
    /****************************************************************************/
    /**
     * \brief Test of the comparison operator.
     */
    /****************************************************************************/
    void utTestComparison();
    /****************************************************************************/
    /**
     * \brief Test of Trigger and Elapsed functions.
     */
    /****************************************************************************/
    void utTestElapsed();
}; // end class TestMonotonicTime

/****************************************************************************/
void TestMonotonicTime::initTestCase() {
}

/****************************************************************************/
void TestMonotonicTime::init() {
}

/****************************************************************************/
void TestMonotonicTime::cleanup() {
}

/****************************************************************************/
void TestMonotonicTime::cleanupTestCase() {
}

#if defined(Q_OS_LINUX)

/****************************************************************************/
void TestMonotonicTime::utTestConstructorsOSSpecific() {
    try {
        // test default constructor
        MonotonicTime Obj1;
        QTest::qSleep(100);
        MonotonicTime Obj2;

        // verify that instances are different
        QVERIFY(    (Obj1.m_Time.tv_sec != Obj2.m_Time.tv_sec) ||
                    (Obj1.m_Time.tv_nsec != Obj2.m_Time.tv_nsec));

        // test copy constructor
        MonotonicTime Obj3(Obj1);
        QCOMPARE(Obj3.m_Time.tv_sec,    Obj1.m_Time.tv_sec);
        QCOMPARE(Obj3.m_Time.tv_nsec,   Obj1.m_Time.tv_nsec);

    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestMonotonicTime::utTestAssignmentOperatorOSSpecific() {
    try {
        // test assignment operator
        MonotonicTime Obj1;
        QTest::qSleep(100);
        MonotonicTime Obj2;

        // verify that instances are different
        QVERIFY(    (Obj1.m_Time.tv_sec != Obj2.m_Time.tv_sec) ||
                    (Obj1.m_Time.tv_nsec != Obj2.m_Time.tv_nsec));

        // test assignment operator
        Obj2 = Obj1;
        QCOMPARE(Obj2.m_Time.tv_sec,    Obj1.m_Time.tv_sec);
        QCOMPARE(Obj2.m_Time.tv_nsec,   Obj1.m_Time.tv_nsec);
        // test assignment operator on self
        Obj2 = Obj2;
        QCOMPARE(Obj2.m_Time.tv_sec,    Obj1.m_Time.tv_sec);
        QCOMPARE(Obj2.m_Time.tv_nsec,   Obj1.m_Time.tv_nsec);
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestMonotonicTime::utTestComparisonOSSpecific() {
    MonotonicTime Obj1;
    MonotonicTime Obj2;
    Obj1.m_Time.tv_sec = 0;
    Obj1.m_Time.tv_nsec = 0;
    Obj2.m_Time.tv_sec = 0;
    Obj2.m_Time.tv_nsec = 0;

    QVERIFY(Obj1 == Obj2);
    QVERIFY(!(Obj1 != Obj2));
    // Obj1.seconds < Obj2.seconds
    Obj1.m_Time.tv_sec = 1;
    Obj2.m_Time.tv_sec = 2;
    QVERIFY(!(Obj1 == Obj2));
    QVERIFY(Obj1 != Obj2);
    // Obj1.seconds > Obj2.seconds
    Obj1.m_Time.tv_sec = 3;
    Obj2.m_Time.tv_sec = 2;
    QVERIFY(!(Obj1 == Obj2));
    QVERIFY(Obj1 != Obj2);
    // Obj1.seconds = Obj2.seconds
    Obj1.m_Time.tv_sec = 2;
    Obj2.m_Time.tv_sec = 2;
    // Obj1.nseconds < Obj2.nseconds
    Obj1.m_Time.tv_nsec = 10;
    Obj2.m_Time.tv_nsec = 11;
    QVERIFY(!(Obj1 == Obj2));
    QVERIFY(Obj1 != Obj2);
    // Obj1.nseconds > Obj2.nseconds
    Obj1.m_Time.tv_nsec = 12;
    Obj2.m_Time.tv_nsec = 11;
    QVERIFY(!(Obj1 == Obj2));
    QVERIFY(Obj1 != Obj2);
    // Obj1.nseconds = Obj2.nseconds
    Obj1.m_Time.tv_nsec = 12;
    Obj2.m_Time.tv_nsec = 12;
    QVERIFY(Obj1 == Obj2);
    QVERIFY(!(Obj1 != Obj2));
}

#elif defined(Q_OS_WIN32)

/****************************************************************************/
void TestMonotonicTime::utTestConstructorsOSSpecific() {
    try {
        // test default constructor
        MonotonicTime Obj1;
        QTest::qSleep(100);
        MonotonicTime Obj2;

        // verify that instances are different
        QVERIFY(Obj1.m_Ticks != Obj2.m_Ticks);

        // test copy constructor
        MonotonicTime Obj3(Obj1);
        QCOMPARE(Obj3.m_Ticks,  Obj1.m_Ticks);
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestMonotonicTime::utTestAssignmentOperatorOSSpecific() {
    try {
        // test assignment operator
        MonotonicTime Obj1;
        QTest::qSleep(100);
        MonotonicTime Obj2;

        // verify that instances are different
        QVERIFY(Obj1.m_Ticks != Obj2.m_Ticks);

        // test assignment operator
        Obj2 = Obj1;
        QCOMPARE(Obj2.m_Ticks,  Obj1.m_Ticks);
        // test assignment operator on self
        Obj2 = Obj2;
        QCOMPARE(Obj2.m_Ticks,  Obj1.m_Ticks);
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestMonotonicTime::utTestComparisonOSSpecific() {
    MonotonicTime Obj1;
    MonotonicTime Obj2;
    Obj1.m_Ticks = 0;
    Obj2.m_Ticks = 0;

    QVERIFY(Obj1 == Obj2);
    QVERIFY(!(Obj1 != Obj2));
    // Obj1.m_Ticks < Obj2.m_Ticks
    Obj1.m_Ticks = 1;
    Obj2.m_Ticks = 2;
    QVERIFY(!(Obj1 == Obj2));
    QVERIFY(Obj1 != Obj2);
    // Obj1.m_Ticks > Obj2.m_Ticks
    Obj1.m_Ticks = 3;
    Obj2.m_Ticks = 2;
    QVERIFY(!(Obj1 == Obj2));
    QVERIFY(Obj1 != Obj2);
    // Obj1.seconds = Obj2.seconds
    Obj1.m_Ticks = 2;
    Obj2.m_Ticks = 2;
    QVERIFY(Obj1 == Obj2);
    QVERIFY(!(Obj1 != Obj2));
}

#else
    #error Unsupported operating system
#endif

/****************************************************************************/
void TestMonotonicTime::utTestConstructors() {
    utTestConstructorsOSSpecific();
}

/****************************************************************************/
void TestMonotonicTime::utTestAssignmentOperator() {
    utTestAssignmentOperatorOSSpecific();
}

/****************************************************************************/
void TestMonotonicTime::utTestComparison() {
    utTestComparisonOSSpecific();
}

/****************************************************************************/
void TestMonotonicTime::utTestElapsed() {
    try {
        // ...
        MonotonicTime Obj1;
        // sleep for about 500 ms
        QTest::qSleep(500);
        // check for some reasonable values +/- 10 % should be OK even for virtual machines
        quint64 Elapsed = Obj1.Elapsed();
        QVERIFY((Elapsed >= 450) && (Elapsed < 550));
        // trigger
        Obj1.Trigger();
        // check for some reasonable values +/- 10 % should be OK even for virtual machines
        Elapsed = Obj1.Elapsed();
        QVERIFY(Elapsed <= 50);
        // sleep for about 1500 ms
        QTest::qSleep(1500);
        // check for some reasonable values +/- 50 ms should be OK even for virtual machines
        Elapsed = Obj1.Elapsed();
        QVERIFY((Elapsed >= 1450) && (Elapsed < 1550));
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

} // end namespace Global

QTEST_MAIN(Global::TestMonotonicTime)

#include "TestMonotonicTime.moc"
