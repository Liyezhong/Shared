/****************************************************************************/
/*! \file TestDayOperationLoggerConfig.cpp
 *
 *  \brief Implementation file for class TestDayOperationLoggerConfig.
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
#include <DataLogging/Include/DayEventLoggerConfig.h>
#include <Global/Include/GlobalEventCodes.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Test class for DayEventLoggerConfig class.
 */
/****************************************************************************/
class TestDayOperationLoggerConfig : public QObject {
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
}; // end class TestDayOperationLoggerConfig

/****************************************************************************/
void TestDayOperationLoggerConfig::initTestCase() {
}

/****************************************************************************/
void TestDayOperationLoggerConfig::init() {
}

/****************************************************************************/
void TestDayOperationLoggerConfig::cleanup() {
}

/****************************************************************************/
void TestDayOperationLoggerConfig::cleanupTestCase() {
}

/****************************************************************************/
void TestDayOperationLoggerConfig::utDefaultConstructor() {
    // test default constructor
    DayEventLoggerConfig TestObject;
    QCOMPARE(TestObject.m_OperatingMode,    QString(""));
    QCOMPARE(TestObject.m_SerialNumber,     QString(""));
    QCOMPARE(TestObject.m_Path,             QString(""));
    QCOMPARE(TestObject.m_MaxFileCount,     0);
}

/****************************************************************************/
void TestDayOperationLoggerConfig::utConstructorWithData() {
    // test constructor with data
    DayEventLoggerConfig TestObject1("assembly",    "SerNum1", "Path1", 1, "Leica_");
    DayEventLoggerConfig TestObject2("production",  "SerNum2", "Path2", 2, "Leica_ST_");

    QCOMPARE(TestObject1.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject1.m_SerialNumber,    QString("SerNum1"));
    QCOMPARE(TestObject1.m_Path,            QString("Path1"));
    QCOMPARE(TestObject1.m_MaxFileCount,    1);
    QCOMPARE(TestObject1.m_BaseFileName,    QString("Leica_"));

    QCOMPARE(TestObject2.m_OperatingMode,   QString("production"));
    QCOMPARE(TestObject2.m_SerialNumber,    QString("SerNum2"));
    QCOMPARE(TestObject2.m_Path,            QString("Path2"));
    QCOMPARE(TestObject2.m_MaxFileCount,    2);
    QCOMPARE(TestObject2.m_BaseFileName,    QString("Leica_ST_"));
}

/****************************************************************************/
void TestDayOperationLoggerConfig::utCopyConstructor() {
    // test copy constructor
    DayEventLoggerConfig TestObject1("assembly",    "SerNum1", "Path1", 1, "Leica_");
    DayEventLoggerConfig TestObject2("production",  "SerNum2", "Path2", 2, "Leica_ST_");
    DayEventLoggerConfig TestObject3(TestObject1);
    DayEventLoggerConfig TestObject4(TestObject2);

    QCOMPARE(TestObject1.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject1.m_SerialNumber,    QString("SerNum1"));
    QCOMPARE(TestObject1.m_Path,            QString("Path1"));
    QCOMPARE(TestObject1.m_MaxFileCount,    1);
    QCOMPARE(TestObject1.m_BaseFileName,    QString("Leica_"));

    QCOMPARE(TestObject2.m_OperatingMode,   QString("production"));
    QCOMPARE(TestObject2.m_SerialNumber,    QString("SerNum2"));
    QCOMPARE(TestObject2.m_Path,            QString("Path2"));
    QCOMPARE(TestObject2.m_MaxFileCount,    2);
    QCOMPARE(TestObject2.m_BaseFileName,    QString("Leica_ST_"));

    QCOMPARE(TestObject3.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject3.m_SerialNumber,    QString("SerNum1"));
    QCOMPARE(TestObject3.m_Path,            QString("Path1"));
    QCOMPARE(TestObject3.m_MaxFileCount,    1);
    QCOMPARE(TestObject3.m_BaseFileName,    QString("Leica_"));

    QCOMPARE(TestObject4.m_OperatingMode,   QString("production"));
    QCOMPARE(TestObject4.m_SerialNumber,    QString("SerNum2"));
    QCOMPARE(TestObject4.m_Path,            QString("Path2"));
    QCOMPARE(TestObject4.m_MaxFileCount,    2);
    QCOMPARE(TestObject4.m_BaseFileName,    QString("Leica_ST_"));
}

/****************************************************************************/
void TestDayOperationLoggerConfig::utAssignmentOperator() {
    // test assignment operator
    DayEventLoggerConfig TestObject1("assembly",    "SerNum1", "Path1", 1, "Leica_");
    DayEventLoggerConfig TestObject2("production",  "SerNum2", "Path2", 2, "Leica_ST_");
    DayEventLoggerConfig TestObject3;
    DayEventLoggerConfig TestObject4;
    DayEventLoggerConfig TestObject5;
    TestObject3 = TestObject1;
    TestObject4 = TestObject2;

    QCOMPARE(TestObject1.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject1.m_SerialNumber,    QString("SerNum1"));
    QCOMPARE(TestObject1.m_Path,            QString("Path1"));
    QCOMPARE(TestObject1.m_MaxFileCount,    1);
    QCOMPARE(TestObject1.m_BaseFileName,    QString("Leica_"));

    QCOMPARE(TestObject2.m_OperatingMode,   QString("production"));
    QCOMPARE(TestObject2.m_SerialNumber,    QString("SerNum2"));
    QCOMPARE(TestObject2.m_Path,            QString("Path2"));
    QCOMPARE(TestObject2.m_MaxFileCount,    2);
    QCOMPARE(TestObject2.m_BaseFileName,    QString("Leica_ST_"));

    QCOMPARE(TestObject3.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject3.m_SerialNumber,    QString("SerNum1"));
    QCOMPARE(TestObject3.m_Path,            QString("Path1"));
    QCOMPARE(TestObject3.m_MaxFileCount,    1);
    QCOMPARE(TestObject3.m_BaseFileName,    QString("Leica_"));

    QCOMPARE(TestObject4.m_OperatingMode,   QString("production"));
    QCOMPARE(TestObject4.m_SerialNumber,    QString("SerNum2"));
    QCOMPARE(TestObject4.m_Path,            QString("Path2"));
    QCOMPARE(TestObject4.m_MaxFileCount,    2);
    QCOMPARE(TestObject4.m_BaseFileName,    QString("Leica_ST_"));

    TestObject5 = TestObject4 = TestObject3;
    QCOMPARE(TestObject3.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject3.m_SerialNumber,    QString("SerNum1"));
    QCOMPARE(TestObject3.m_Path,            QString("Path1"));
    QCOMPARE(TestObject3.m_MaxFileCount,    1);

    QCOMPARE(TestObject4.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject4.m_SerialNumber,    QString("SerNum1"));
    QCOMPARE(TestObject4.m_Path,            QString("Path1"));
    QCOMPARE(TestObject4.m_MaxFileCount,    1);

    QCOMPARE(TestObject5.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject5.m_SerialNumber,    QString("SerNum1"));
    QCOMPARE(TestObject5.m_Path,            QString("Path1"));
    QCOMPARE(TestObject5.m_MaxFileCount,    1);

    TestObject5 = TestObject5;
    QCOMPARE(TestObject5.m_OperatingMode,   QString("assembly"));
    QCOMPARE(TestObject5.m_SerialNumber,    QString("SerNum1"));
    QCOMPARE(TestObject5.m_Path,            QString("Path1"));
    QCOMPARE(TestObject5.m_MaxFileCount,    1);
}

/****************************************************************************/
void TestDayOperationLoggerConfig::utGet() {
    // test get methods
    DayEventLoggerConfig TestObject1("assembly",    "SerNum1", "Path1", 1, "Leica_");
    DayEventLoggerConfig TestObject2("production",  "SerNum2", "Path2", 2, "Leica_ST_");

    QCOMPARE(TestObject1.GetOperatingMode(),    QString("assembly"));
    QCOMPARE(TestObject1.GetSerialNumber(),     QString("SerNum1"));
    QCOMPARE(TestObject1.GetPath(),             QString("Path1"));
    QCOMPARE(TestObject1.GetMaxFileCount(),     1);
    QCOMPARE(TestObject1.GetBaseFileName(),     QString("Leica_"));

    QCOMPARE(TestObject2.GetOperatingMode(),    QString("production"));
    QCOMPARE(TestObject2.GetSerialNumber(),     QString("SerNum2"));
    QCOMPARE(TestObject2.GetPath(),             QString("Path2"));
    QCOMPARE(TestObject2.GetMaxFileCount(),     2);
    QCOMPARE(TestObject2.GetBaseFileName(),     QString("Leica_ST_"));
}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestDayOperationLoggerConfig)

#include "TestDayOperationLoggerConfig.moc"
