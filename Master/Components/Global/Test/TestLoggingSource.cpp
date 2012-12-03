/****************************************************************************/
/*! \file TestLoggingSource.cpp
 *
 *  \brief Implementation file for class TestLoggingSource.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-08-05
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
#include <Global/Include/LoggingSource.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Test class for LoggingSource class.
 */
/****************************************************************************/
class TestLoggingSource : public QObject {
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
     * \brief Test constructors.
     */
    /****************************************************************************/
    void utConstructors();
    /****************************************************************************/
    /**
     * \brief Test assignment operator.
     */
    /****************************************************************************/
    void utAssignmentOperator();
    /****************************************************************************/
    /**
     * \brief Test assignment operator.
     */
    /****************************************************************************/
    void utComparison();
    /****************************************************************************/
    /**
     * \brief Test Get and Set methods.
     */
    /****************************************************************************/
    void utGetSetMethods();
}; // end class TestLoggingSource

/****************************************************************************/
void TestLoggingSource::initTestCase() {
}

/****************************************************************************/
void TestLoggingSource::init() {
}

/****************************************************************************/
void TestLoggingSource::cleanup() {
}

/****************************************************************************/
void TestLoggingSource::cleanupTestCase() {
}

/****************************************************************************/
void TestLoggingSource::utConstructors() {
    // test constructors

    LoggingSource LS1;
    LoggingSource LS2(1,2);
    LoggingSource LS3(3,4);
    LoggingSource LS4(LS1);

    QCOMPARE(LS1.m_Source,          gSourceType(0));
    QCOMPARE(LS1.m_SubComponent,    gSubComponentType(0));

    QCOMPARE(LS2.m_Source,          gSourceType(1));
    QCOMPARE(LS2.m_SubComponent,    gSubComponentType(2));

    QCOMPARE(LS3.m_Source,          gSourceType(3));
    QCOMPARE(LS3.m_SubComponent,    gSubComponentType(4));

    QCOMPARE(LS4.m_Source,          LS1.m_Source);
    QCOMPARE(LS4.m_SubComponent,    LS1.m_SubComponent);
}

/****************************************************************************/
void TestLoggingSource::utAssignmentOperator() {
    LoggingSource LS1;
    LoggingSource LS2(1,2);
    LoggingSource LS3;

    QCOMPARE(LS1.m_Source,          gSourceType(0));
    QCOMPARE(LS1.m_SubComponent,    gSubComponentType(0));

    QCOMPARE(LS2.m_Source,          gSourceType(1));
    QCOMPARE(LS2.m_SubComponent,    gSubComponentType(2));

    QCOMPARE(LS3.m_Source,          gSourceType(0));
    QCOMPARE(LS3.m_SubComponent,    gSourceType(0));

    // set to LS1
    LS3 = LS1;
    QCOMPARE(LS1.m_Source,          gSourceType(0));
    QCOMPARE(LS1.m_SubComponent,    gSubComponentType(0));

    QCOMPARE(LS2.m_Source,          gSourceType(1));
    QCOMPARE(LS2.m_SubComponent,    gSubComponentType(2));

    QCOMPARE(LS3.m_Source,          LS1.m_Source);
    QCOMPARE(LS3.m_SubComponent,    LS1.m_SubComponent);

    // set to LS2
    LS3 = LS2;
    QCOMPARE(LS1.m_Source,          gSourceType(0));
    QCOMPARE(LS1.m_SubComponent,    gSubComponentType(0));

    QCOMPARE(LS2.m_Source,          gSourceType(1));
    QCOMPARE(LS2.m_SubComponent,    gSubComponentType(2));

    QCOMPARE(LS3.m_Source,          LS2.m_Source);
    QCOMPARE(LS3.m_SubComponent,    LS2.m_SubComponent);

    // set to self
    LS3 = LS3;
    QCOMPARE(LS3.m_Source,          LS2.m_Source);
    QCOMPARE(LS3.m_SubComponent,    LS2.m_SubComponent);
}

/****************************************************************************/
void TestLoggingSource::utGetSetMethods() {
    LoggingSource LS1;
    LoggingSource LS2(1,2);
    LoggingSource LS3(3,4);

    // test Get Functions
    QCOMPARE(LS1.GetSource(),       gSourceType(0));
    QCOMPARE(LS1.GetSubComponent(), gSubComponentType(0));

    QCOMPARE(LS2.GetSource(),       gSourceType(1));
    QCOMPARE(LS2.GetSubComponent(), gSubComponentType(2));

    QCOMPARE(LS3.GetSource(),       gSourceType(3));
    QCOMPARE(LS3.GetSubComponent(), gSubComponentType(4));

    // test Set Functions
    LS1.SetSource(7);
    LS1.SetSubComponent(8);
    QCOMPARE(LS1.GetSource(),       gSourceType(7));
    QCOMPARE(LS1.GetSubComponent(), gSubComponentType(8));

}

/****************************************************************************/
void TestLoggingSource::utComparison() {
    LoggingSource LS1(1,2);
    LoggingSource LS2(1,4);
    LoggingSource LS3(3,2);
    LoggingSource LS4(3,4);
    LoggingSource LS5(1,2);
    QVERIFY(!(LS1 == LS2));
    QVERIFY(!(LS1 == LS3));
    QVERIFY(!(LS1 == LS4));
    QVERIFY(LS1 == LS5);
}

} // end namespace Global

QTEST_MAIN(Global::TestLoggingSource)

#include "TestLoggingSource.moc"
