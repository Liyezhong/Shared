/****************************************************************************/
/*! \file TestComponentTestLoggerConfig.cpp
 *
 *  \brief Implementation file for class TestComponentTestLoggerConfig.
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
#include <DataLogging/Include/ComponentTestLoggerConfig.h>
#include <Global/Include/GlobalEventCodes.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Test class for ComponentTestLoggerConfig class.
 */
/****************************************************************************/
class TestComponentTestLoggerConfig : public QObject {
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
}; // end class TestComponentTestLoggerConfig

/****************************************************************************/
void TestComponentTestLoggerConfig::initTestCase() {
}

/****************************************************************************/
void TestComponentTestLoggerConfig::init() {
}

/****************************************************************************/
void TestComponentTestLoggerConfig::cleanup() {
}

/****************************************************************************/
void TestComponentTestLoggerConfig::cleanupTestCase() {
}

/****************************************************************************/
void TestComponentTestLoggerConfig::utDefaultConstructor() {
    // test default constructor
    ComponentTestLoggerConfig TestObject;
    QCOMPARE(TestObject.m_OperatingMode,    QString(""));
    QCOMPARE(TestObject.m_Path,             QString(""));
}

/****************************************************************************/
void TestComponentTestLoggerConfig::utConstructorWithData() {
    // test constructor with data
    ComponentTestLoggerConfig TestObject1("assembly",   "Path1");
    ComponentTestLoggerConfig TestObject2("production", "Path2");

    QCOMPARE(TestObject1.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject1.m_Path,            QString("Path1"));

    QCOMPARE(TestObject2.m_OperatingMode,   QString("production"));
    QCOMPARE(TestObject2.m_Path,            QString("Path2"));
}

/****************************************************************************/
void TestComponentTestLoggerConfig::utCopyConstructor() {
    // test copy constructor
    ComponentTestLoggerConfig TestObject1("assembly",   "Path1");
    ComponentTestLoggerConfig TestObject2("production", "Path2");
    ComponentTestLoggerConfig TestObject3(TestObject1);
    ComponentTestLoggerConfig TestObject4(TestObject2);

    QCOMPARE(TestObject1.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject1.m_Path,            QString("Path1"));

    QCOMPARE(TestObject2.m_OperatingMode,   QString("production"));
    QCOMPARE(TestObject2.m_Path,            QString("Path2"));

    QCOMPARE(TestObject3.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject3.m_Path,            QString("Path1"));

    QCOMPARE(TestObject4.m_OperatingMode,   QString("production"));
    QCOMPARE(TestObject4.m_Path,            QString("Path2"));
}

/****************************************************************************/
void TestComponentTestLoggerConfig::utAssignmentOperator() {
    // test assignment operator
    ComponentTestLoggerConfig TestObject1("assembly",   "Path1");
    ComponentTestLoggerConfig TestObject2("production", "Path2");
    ComponentTestLoggerConfig TestObject3;
    ComponentTestLoggerConfig TestObject4;
    ComponentTestLoggerConfig TestObject5;
    TestObject3 = TestObject1;
    TestObject4 = TestObject2;

    QCOMPARE(TestObject1.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject1.m_Path,            QString("Path1"));

    QCOMPARE(TestObject2.m_OperatingMode,   QString("production"));
    QCOMPARE(TestObject2.m_Path,            QString("Path2"));

    QCOMPARE(TestObject3.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject3.m_Path,            QString("Path1"));

    QCOMPARE(TestObject4.m_OperatingMode,   QString("production"));
    QCOMPARE(TestObject4.m_Path,            QString("Path2"));

    TestObject5 = TestObject4 = TestObject3;
    QCOMPARE(TestObject3.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject3.m_Path,            QString("Path1"));

    QCOMPARE(TestObject4.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject4.m_Path,            QString("Path1"));

    QCOMPARE(TestObject5.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject5.m_Path,            QString("Path1"));

    TestObject5 = TestObject5;
    QCOMPARE(TestObject5.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject5.m_Path,            QString("Path1"));
}

/****************************************************************************/
void TestComponentTestLoggerConfig::utGet() {
    // test get methods
    ComponentTestLoggerConfig TestObject1("assembly",   "Path1");
    ComponentTestLoggerConfig TestObject2("production", "Path2");

    QCOMPARE(TestObject1.GetOperatingMode(),    QString("assembly"));
    QCOMPARE(TestObject1.GetPath(),             QString("Path1"));

    QCOMPARE(TestObject2.GetOperatingMode(),    QString("production"));
    QCOMPARE(TestObject2.GetPath(),             QString("Path2"));
}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestComponentTestLoggerConfig)

#include "TestComponentTestLoggerConfig.moc"
