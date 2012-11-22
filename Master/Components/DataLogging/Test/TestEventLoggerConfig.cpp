/****************************************************************************/
/*! \file TestEventLoggerConfig.cpp
 *
 *  \brief Implementation file for class TestEventLoggerConfig.
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
#include <DataLogging/Include/EventLoggerConfig.h>
#include <Global/Include/GlobalEventCodes.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Test class for EventLoggerConfig class.
 */
/****************************************************************************/
class TestEventLoggerConfig : public QObject {
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
}; // end class TestEventLoggerConfig

/****************************************************************************/
void TestEventLoggerConfig::initTestCase() {
}

/****************************************************************************/
void TestEventLoggerConfig::init() {
}

/****************************************************************************/
void TestEventLoggerConfig::cleanup() {
}

/****************************************************************************/
void TestEventLoggerConfig::cleanupTestCase() {
}

/****************************************************************************/
void TestEventLoggerConfig::utDefaultConstructor() {
    // test default constructor
    EventLoggerConfig TestObject;
    QCOMPARE(TestObject.m_OperatingMode,            QString(""));
    QCOMPARE(TestObject.m_EventLoggerBaseFileName,  QString(""));
    QCOMPARE(TestObject.m_SerialNumber,             QString(""));
    QCOMPARE(TestObject.m_Path,                     QString(""));
    QCOMPARE(TestObject.m_MaxFileSize,              qint64(0));
}

/****************************************************************************/
void TestEventLoggerConfig::utConstructorWithData() {
    // test constructor with data
    EventLoggerConfig TestObject1("Assembly",   "assembly",   "SerNum1", "Path1", 1);
    EventLoggerConfig TestObject2("Production", "production", "SerNum2", "Path2", 2);
    EventLoggerConfig TestObject3("Service",    "service",    "SerNum3", "Path3", -1);

    QCOMPARE(TestObject1.m_OperatingMode,           QString("assembly"));
    QCOMPARE(TestObject1.m_EventLoggerBaseFileName, QString("Assembly"));
    QCOMPARE(TestObject1.m_SerialNumber,            QString("SerNum1"));
    QCOMPARE(TestObject1.m_Path,                    QString("Path1"));
    QCOMPARE(TestObject1.m_MaxFileSize,             qint64(1));

    QCOMPARE(TestObject2.m_OperatingMode,           QString("production"));
    QCOMPARE(TestObject2.m_EventLoggerBaseFileName, QString("Production"));
    QCOMPARE(TestObject2.m_SerialNumber,            QString("SerNum2"));
    QCOMPARE(TestObject2.m_Path,                    QString("Path2"));
    QCOMPARE(TestObject2.m_MaxFileSize,             qint64(2));

    QCOMPARE(TestObject3.m_OperatingMode,           QString("service"));
    QCOMPARE(TestObject3.m_EventLoggerBaseFileName, QString("Service"));
    QCOMPARE(TestObject3.m_SerialNumber,            QString("SerNum3"));
    QCOMPARE(TestObject3.m_Path,                    QString("Path3"));
    QCOMPARE(TestObject3.m_MaxFileSize,             qint64(0));
}

/****************************************************************************/
void TestEventLoggerConfig::utCopyConstructor() {
    // test copy constructor
    EventLoggerConfig TestObject1("Assembly",   "assembly",   "SerNum1", "Path1", 1);
    EventLoggerConfig TestObject2("Production", "production", "SerNum2", "Path2", 2);
    EventLoggerConfig TestObject3(TestObject1);
    EventLoggerConfig TestObject4(TestObject2);

    QCOMPARE(TestObject1.m_OperatingMode,           QString("assembly"));
    QCOMPARE(TestObject1.m_EventLoggerBaseFileName, QString("Assembly"));
    QCOMPARE(TestObject1.m_SerialNumber,            QString("SerNum1"));
    QCOMPARE(TestObject1.m_Path,                    QString("Path1"));
    QCOMPARE(TestObject1.m_MaxFileSize,             qint64(1));

    QCOMPARE(TestObject2.m_OperatingMode,           QString("production"));
    QCOMPARE(TestObject2.m_EventLoggerBaseFileName, QString("Production"));
    QCOMPARE(TestObject2.m_SerialNumber,            QString("SerNum2"));
    QCOMPARE(TestObject2.m_Path,                    QString("Path2"));
    QCOMPARE(TestObject2.m_MaxFileSize,             qint64(2));

    QCOMPARE(TestObject3.m_OperatingMode,           QString("assembly"));
    QCOMPARE(TestObject3.m_EventLoggerBaseFileName, QString("Assembly"));
    QCOMPARE(TestObject3.m_SerialNumber,            QString("SerNum1"));
    QCOMPARE(TestObject3.m_Path,                    QString("Path1"));
    QCOMPARE(TestObject3.m_MaxFileSize,             qint64(1));

    QCOMPARE(TestObject4.m_OperatingMode,           QString("production"));
    QCOMPARE(TestObject4.m_EventLoggerBaseFileName, QString("Production"));
    QCOMPARE(TestObject4.m_SerialNumber,            QString("SerNum2"));
    QCOMPARE(TestObject4.m_Path,                    QString("Path2"));
    QCOMPARE(TestObject4.m_MaxFileSize,             qint64(2));
}

/****************************************************************************/
void TestEventLoggerConfig::utAssignmentOperator() {
    // test assignment operator
    EventLoggerConfig TestObject1("Assembly",   "assembly",   "SerNum1", "Path1", 1);
    EventLoggerConfig TestObject2("Production", "production", "SerNum2", "Path2", 2);
    EventLoggerConfig TestObject3;
    EventLoggerConfig TestObject4;
    EventLoggerConfig TestObject5;
    TestObject3 = TestObject1;
    TestObject4 = TestObject2;

    QCOMPARE(TestObject1.m_OperatingMode,           QString("assembly"));
    QCOMPARE(TestObject1.m_EventLoggerBaseFileName, QString("Assembly"));
    QCOMPARE(TestObject1.m_SerialNumber,            QString("SerNum1"));
    QCOMPARE(TestObject1.m_Path,                    QString("Path1"));
    QCOMPARE(TestObject1.m_MaxFileSize,             qint64(1));

    QCOMPARE(TestObject2.m_OperatingMode,           QString("production"));
    QCOMPARE(TestObject2.m_EventLoggerBaseFileName, QString("Production"));
    QCOMPARE(TestObject2.m_SerialNumber,            QString("SerNum2"));
    QCOMPARE(TestObject2.m_Path,                    QString("Path2"));
    QCOMPARE(TestObject2.m_MaxFileSize,             qint64(2));

    QCOMPARE(TestObject3.m_OperatingMode,           QString("assembly"));
    QCOMPARE(TestObject3.m_EventLoggerBaseFileName, QString("Assembly"));
    QCOMPARE(TestObject3.m_SerialNumber,            QString("SerNum1"));
    QCOMPARE(TestObject3.m_Path,                    QString("Path1"));
    QCOMPARE(TestObject3.m_MaxFileSize,             qint64(1));

    QCOMPARE(TestObject4.m_OperatingMode,           QString("production"));
    QCOMPARE(TestObject4.m_EventLoggerBaseFileName, QString("Production"));
    QCOMPARE(TestObject4.m_SerialNumber,            QString("SerNum2"));
    QCOMPARE(TestObject4.m_Path,                    QString("Path2"));
    QCOMPARE(TestObject4.m_MaxFileSize,             qint64(2));

    TestObject5 = TestObject4 = TestObject3;
    QCOMPARE(TestObject3.m_OperatingMode,           QString("assembly"));
    QCOMPARE(TestObject3.m_EventLoggerBaseFileName, QString("Assembly"));
    QCOMPARE(TestObject3.m_SerialNumber,            QString("SerNum1"));
    QCOMPARE(TestObject3.m_Path,                    QString("Path1"));
    QCOMPARE(TestObject3.m_MaxFileSize,             qint64(1));

    QCOMPARE(TestObject4.m_OperatingMode,           QString("assembly"));
    QCOMPARE(TestObject4.m_EventLoggerBaseFileName, QString("Assembly"));
    QCOMPARE(TestObject4.m_SerialNumber,            QString("SerNum1"));
    QCOMPARE(TestObject4.m_Path,                    QString("Path1"));
    QCOMPARE(TestObject4.m_MaxFileSize,             qint64(1));

    QCOMPARE(TestObject5.m_OperatingMode,           QString("assembly"));
    QCOMPARE(TestObject5.m_EventLoggerBaseFileName, QString("Assembly"));
    QCOMPARE(TestObject5.m_SerialNumber,            QString("SerNum1"));
    QCOMPARE(TestObject5.m_Path,                    QString("Path1"));
    QCOMPARE(TestObject5.m_MaxFileSize,             qint64(1));

    TestObject5 = TestObject5;
    QCOMPARE(TestObject5.m_OperatingMode,           QString("assembly"));
    QCOMPARE(TestObject5.m_EventLoggerBaseFileName, QString("Assembly"));
    QCOMPARE(TestObject5.m_SerialNumber,            QString("SerNum1"));
    QCOMPARE(TestObject5.m_Path,                    QString("Path1"));
    QCOMPARE(TestObject5.m_MaxFileSize,             qint64(1));
}

/****************************************************************************/
void TestEventLoggerConfig::utGet() {
    // test get methods
    EventLoggerConfig TestObject1("Assembly",   "assembly",   "SerNum1", "Path1", 1);
    EventLoggerConfig TestObject2("Production", "production", "SerNum2", "Path2", 2);

    QCOMPARE(TestObject1.GetOperatingMode(),            QString("assembly"));
    QCOMPARE(TestObject1.GetEventLoggerBaseFileName(),  QString("Assembly"));
    QCOMPARE(TestObject1.GetSerialNumber(),             QString("SerNum1"));
    QCOMPARE(TestObject1.GetPath(),                     QString("Path1"));
    QCOMPARE(TestObject1.GetMaxFileSize(),              qint64(1));

    QCOMPARE(TestObject2.GetOperatingMode(),            QString("production"));
    QCOMPARE(TestObject2.GetEventLoggerBaseFileName(),  QString("Production"));
    QCOMPARE(TestObject2.GetSerialNumber(),             QString("SerNum2"));
    QCOMPARE(TestObject2.GetPath(),                     QString("Path2"));
    QCOMPARE(TestObject2.GetMaxFileSize(),              qint64(2));
}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestEventLoggerConfig)

#include "TestEventLoggerConfig.moc"
