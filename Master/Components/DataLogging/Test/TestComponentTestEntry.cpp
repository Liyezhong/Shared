/****************************************************************************/
/*! \file TestComponentTestEntry.cpp
 *
 *  \brief Implementation file for class TestComponentTestEntry.
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
#include <DataLogging/Include/ComponentTestEntry.h>
#include <Global/Include/GlobalEventCodes.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Test class for ComponentTestEntry class.
 */
/****************************************************************************/
class TestComponentTestEntry : public QObject {
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
}; // end class TestComponentTestEntry

/****************************************************************************/
void TestComponentTestEntry::initTestCase() {
}

/****************************************************************************/
void TestComponentTestEntry::init() {
}

/****************************************************************************/
void TestComponentTestEntry::cleanup() {
}

/****************************************************************************/
void TestComponentTestEntry::cleanupTestCase() {
}

/****************************************************************************/
void TestComponentTestEntry::utDefaultConstructor() {
    // test default constructor
    ComponentTestEntry TestObject;
    QCOMPARE(TestObject.m_ComponentName,                QString(""));
    QCOMPARE(TestObject.m_BoardName,                    QString(""));
    QCOMPARE(TestObject.m_BoardSerialNumber,            QString(""));
    QCOMPARE(TestObject.m_TesterName,                   QString(""));
    QCOMPARE(TestObject.m_DeviceSerialNumber,           QString(""));
    QCOMPARE(TestObject.m_ComponentSerialNumber,        QString(""));
    QCOMPARE(TestObject.m_ComponentFirmwareVersion,     QString(""));
    QCOMPARE(TestObject.m_ComponentHardwareVersion,     QString(""));
    QCOMPARE(TestObject.m_MainboardSoftwareVersion,     QString(""));
    QCOMPARE(TestObject.m_TestResults.size(),           0);
}

/****************************************************************************/
void TestComponentTestEntry::utConstructorWithData() {
    // test constructor with additional data
    Global::tTranslatableStringList AdditionalData1 =   Global::tTranslatableStringList() <<
                                                        Global::TranslatableString(11, Global::tTranslatableStringList() << "111") <<
                                                        Global::TranslatableString(12, Global::tTranslatableStringList() << "121" << "122");
    Global::tTranslatableStringList AdditionalData2 =   Global::tTranslatableStringList() <<
                                                        Global::TranslatableString(21, Global::tTranslatableStringList() << "211") <<
                                                        Global::TranslatableString(22, Global::tTranslatableStringList() << "221" << "222") <<
                                                        Global::TranslatableString(23, Global::tTranslatableStringList() << "231" << "232" << "233");
    ComponentTestEntry TestObject1("11", "12", "13", "14", "15", "16", "17", "18", "19", AdditionalData1);
    ComponentTestEntry TestObject2("21", "22", "23", "24", "25", "26", "27", "28", "29", AdditionalData2);

    QCOMPARE(TestObject1.m_ComponentName,                               QString("11"));
    QCOMPARE(TestObject1.m_BoardName,                                   QString("12"));
    QCOMPARE(TestObject1.m_BoardSerialNumber,                           QString("13"));
    QCOMPARE(TestObject1.m_TesterName,                                  QString("14"));
    QCOMPARE(TestObject1.m_DeviceSerialNumber,                          QString("15"));
    QCOMPARE(TestObject1.m_ComponentSerialNumber,                       QString("16"));
    QCOMPARE(TestObject1.m_ComponentFirmwareVersion,                    QString("17"));
    QCOMPARE(TestObject1.m_ComponentHardwareVersion,                    QString("18"));
    QCOMPARE(TestObject1.m_MainboardSoftwareVersion,                    QString("19"));
    QCOMPARE(TestObject1.m_TestResults.size(),                          2);
    QCOMPARE(TestObject1.m_TestResults[0].GetStringID(),                quint32(11));
    QCOMPARE(TestObject1.m_TestResults[0].GetArgumentList().size(),     1);
    QCOMPARE(TestObject1.m_TestResults[1].GetStringID(),                quint32(12));
    QCOMPARE(TestObject1.m_TestResults[1].GetArgumentList().size(),     2);

    QCOMPARE(TestObject2.m_ComponentName,                               QString("21"));
    QCOMPARE(TestObject2.m_BoardName,                                   QString("22"));
    QCOMPARE(TestObject2.m_BoardSerialNumber,                           QString("23"));
    QCOMPARE(TestObject2.m_TesterName,                                  QString("24"));
    QCOMPARE(TestObject2.m_DeviceSerialNumber,                          QString("25"));
    QCOMPARE(TestObject2.m_ComponentSerialNumber,                       QString("26"));
    QCOMPARE(TestObject2.m_ComponentFirmwareVersion,                    QString("27"));
    QCOMPARE(TestObject2.m_ComponentHardwareVersion,                    QString("28"));
    QCOMPARE(TestObject2.m_MainboardSoftwareVersion,                    QString("29"));
    QCOMPARE(TestObject2.m_TestResults.size(),                          3);
    QCOMPARE(TestObject2.m_TestResults[0].GetStringID(),                quint32(21));
    QCOMPARE(TestObject2.m_TestResults[0].GetArgumentList().size(),     1);
    QCOMPARE(TestObject2.m_TestResults[1].GetStringID(),                quint32(22));
    QCOMPARE(TestObject2.m_TestResults[1].GetArgumentList().size(),     2);
    QCOMPARE(TestObject2.m_TestResults[2].GetStringID(),                quint32(23));
    QCOMPARE(TestObject2.m_TestResults[2].GetArgumentList().size(),     3);
}

/****************************************************************************/
void TestComponentTestEntry::utCopy() {
    // test copy constructor and assignment operator
    Global::tTranslatableStringList AdditionalData1 =   Global::tTranslatableStringList() <<
                                                        Global::TranslatableString(11, Global::tTranslatableStringList() << "111") <<
                                                        Global::TranslatableString(12, Global::tTranslatableStringList() << "121" << "122");
    Global::tTranslatableStringList AdditionalData2 =   Global::tTranslatableStringList() <<
                                                        Global::TranslatableString(21, Global::tTranslatableStringList() << "211") <<
                                                        Global::TranslatableString(22, Global::tTranslatableStringList() << "221" << "222") <<
                                                        Global::TranslatableString(23, Global::tTranslatableStringList() << "231" << "232" << "233");
    ComponentTestEntry TestObject1("11", "12", "13", "14", "15", "16", "17", "18", "19", AdditionalData1);
    ComponentTestEntry TestObject2("21", "22", "23", "24", "25", "26", "27", "28", "29", AdditionalData2);
    ComponentTestEntry TestObject3(TestObject1);
    ComponentTestEntry TestObject4(TestObject2);
    ComponentTestEntry TestObject5;
    ComponentTestEntry TestObject6;
    TestObject5 = TestObject1;
    TestObject6 = TestObject2;

    QCOMPARE(TestObject1.m_ComponentName,                               QString("11"));
    QCOMPARE(TestObject1.m_BoardName,                                   QString("12"));
    QCOMPARE(TestObject1.m_BoardSerialNumber,                           QString("13"));
    QCOMPARE(TestObject1.m_TesterName,                                  QString("14"));
    QCOMPARE(TestObject1.m_DeviceSerialNumber,                          QString("15"));
    QCOMPARE(TestObject1.m_ComponentSerialNumber,                       QString("16"));
    QCOMPARE(TestObject1.m_ComponentFirmwareVersion,                    QString("17"));
    QCOMPARE(TestObject1.m_ComponentHardwareVersion,                    QString("18"));
    QCOMPARE(TestObject1.m_MainboardSoftwareVersion,                    QString("19"));
    QCOMPARE(TestObject1.m_TestResults.size(),                          2);
    QCOMPARE(TestObject1.m_TestResults[0].GetStringID(),                quint32(11));
    QCOMPARE(TestObject1.m_TestResults[0].GetArgumentList().size(),     1);
    QCOMPARE(TestObject1.m_TestResults[1].GetStringID(),                quint32(12));
    QCOMPARE(TestObject1.m_TestResults[1].GetArgumentList().size(),     2);

    QCOMPARE(TestObject2.m_ComponentName,                               QString("21"));
    QCOMPARE(TestObject2.m_BoardName,                                   QString("22"));
    QCOMPARE(TestObject2.m_BoardSerialNumber,                           QString("23"));
    QCOMPARE(TestObject2.m_TesterName,                                  QString("24"));
    QCOMPARE(TestObject2.m_DeviceSerialNumber,                          QString("25"));
    QCOMPARE(TestObject2.m_ComponentSerialNumber,                       QString("26"));
    QCOMPARE(TestObject2.m_ComponentFirmwareVersion,                    QString("27"));
    QCOMPARE(TestObject2.m_ComponentHardwareVersion,                    QString("28"));
    QCOMPARE(TestObject2.m_MainboardSoftwareVersion,                    QString("29"));
    QCOMPARE(TestObject2.m_TestResults.size(),                          3);
    QCOMPARE(TestObject2.m_TestResults[0].GetStringID(),                quint32(21));
    QCOMPARE(TestObject2.m_TestResults[0].GetArgumentList().size(),     1);
    QCOMPARE(TestObject2.m_TestResults[1].GetStringID(),                quint32(22));
    QCOMPARE(TestObject2.m_TestResults[1].GetArgumentList().size(),     2);
    QCOMPARE(TestObject2.m_TestResults[2].GetStringID(),                quint32(23));
    QCOMPARE(TestObject2.m_TestResults[2].GetArgumentList().size(),     3);

    QCOMPARE(TestObject3.m_ComponentName,                               TestObject1.m_ComponentName);
    QCOMPARE(TestObject3.m_BoardName,                                   TestObject1.m_BoardName);
    QCOMPARE(TestObject3.m_BoardSerialNumber,                           TestObject1.m_BoardSerialNumber);
    QCOMPARE(TestObject3.m_TesterName,                                  TestObject1.m_TesterName);
    QCOMPARE(TestObject3.m_DeviceSerialNumber,                          TestObject1.m_DeviceSerialNumber);
    QCOMPARE(TestObject3.m_ComponentSerialNumber,                       TestObject1.m_ComponentSerialNumber);
    QCOMPARE(TestObject3.m_ComponentFirmwareVersion,                    TestObject1.m_ComponentFirmwareVersion);
    QCOMPARE(TestObject3.m_ComponentHardwareVersion,                    TestObject1.m_ComponentHardwareVersion);
    QCOMPARE(TestObject3.m_MainboardSoftwareVersion,                    TestObject1.m_MainboardSoftwareVersion);
    QCOMPARE(TestObject3.m_TestResults.size(),                          TestObject1.m_TestResults.size());
    QCOMPARE(TestObject3.m_TestResults[0].GetStringID(),                TestObject1.m_TestResults[0].GetStringID());
    QCOMPARE(TestObject3.m_TestResults[0].GetArgumentList().size(),     TestObject1.m_TestResults[0].GetArgumentList().size());
    QCOMPARE(TestObject3.m_TestResults[1].GetStringID(),                TestObject1.m_TestResults[1].GetStringID());
    QCOMPARE(TestObject3.m_TestResults[1].GetArgumentList().size(),     TestObject1.m_TestResults[1].GetArgumentList().size());

    QCOMPARE(TestObject4.m_ComponentName,                               TestObject2.m_ComponentName);
    QCOMPARE(TestObject4.m_BoardName,                                   TestObject2.m_BoardName);
    QCOMPARE(TestObject4.m_BoardSerialNumber,                           TestObject2.m_BoardSerialNumber);
    QCOMPARE(TestObject4.m_TesterName,                                  TestObject2.m_TesterName);
    QCOMPARE(TestObject4.m_DeviceSerialNumber,                          TestObject2.m_DeviceSerialNumber);
    QCOMPARE(TestObject4.m_ComponentSerialNumber,                       TestObject2.m_ComponentSerialNumber);
    QCOMPARE(TestObject4.m_ComponentFirmwareVersion,                    TestObject2.m_ComponentFirmwareVersion);
    QCOMPARE(TestObject4.m_ComponentHardwareVersion,                    TestObject2.m_ComponentHardwareVersion);
    QCOMPARE(TestObject4.m_MainboardSoftwareVersion,                    TestObject2.m_MainboardSoftwareVersion);
    QCOMPARE(TestObject4.m_TestResults.size(),                          TestObject2.m_TestResults.size());
    QCOMPARE(TestObject4.m_TestResults[0].GetStringID(),                TestObject2.m_TestResults[0].GetStringID());
    QCOMPARE(TestObject4.m_TestResults[0].GetArgumentList().size(),     TestObject2.m_TestResults[0].GetArgumentList().size());
    QCOMPARE(TestObject4.m_TestResults[1].GetStringID(),                TestObject2.m_TestResults[1].GetStringID());
    QCOMPARE(TestObject4.m_TestResults[1].GetArgumentList().size(),     TestObject2.m_TestResults[1].GetArgumentList().size());

    QCOMPARE(TestObject5.m_ComponentName,                               TestObject1.m_ComponentName);
    QCOMPARE(TestObject5.m_BoardName,                                   TestObject1.m_BoardName);
    QCOMPARE(TestObject5.m_BoardSerialNumber,                           TestObject1.m_BoardSerialNumber);
    QCOMPARE(TestObject5.m_TesterName,                                  TestObject1.m_TesterName);
    QCOMPARE(TestObject5.m_DeviceSerialNumber,                          TestObject1.m_DeviceSerialNumber);
    QCOMPARE(TestObject5.m_ComponentSerialNumber,                       TestObject1.m_ComponentSerialNumber);
    QCOMPARE(TestObject5.m_ComponentFirmwareVersion,                    TestObject1.m_ComponentFirmwareVersion);
    QCOMPARE(TestObject5.m_ComponentHardwareVersion,                    TestObject1.m_ComponentHardwareVersion);
    QCOMPARE(TestObject5.m_MainboardSoftwareVersion,                    TestObject1.m_MainboardSoftwareVersion);
    QCOMPARE(TestObject5.m_TestResults.size(),                          TestObject1.m_TestResults.size());
    QCOMPARE(TestObject5.m_TestResults[0].GetStringID(),                TestObject1.m_TestResults[0].GetStringID());
    QCOMPARE(TestObject5.m_TestResults[0].GetArgumentList().size(),     TestObject1.m_TestResults[0].GetArgumentList().size());
    QCOMPARE(TestObject5.m_TestResults[1].GetStringID(),                TestObject1.m_TestResults[1].GetStringID());
    QCOMPARE(TestObject5.m_TestResults[1].GetArgumentList().size(),     TestObject1.m_TestResults[1].GetArgumentList().size());

    QCOMPARE(TestObject6.m_ComponentName,                               TestObject2.m_ComponentName);
    QCOMPARE(TestObject6.m_BoardName,                                   TestObject2.m_BoardName);
    QCOMPARE(TestObject6.m_BoardSerialNumber,                           TestObject2.m_BoardSerialNumber);
    QCOMPARE(TestObject6.m_TesterName,                                  TestObject2.m_TesterName);
    QCOMPARE(TestObject6.m_DeviceSerialNumber,                          TestObject2.m_DeviceSerialNumber);
    QCOMPARE(TestObject6.m_ComponentSerialNumber,                       TestObject2.m_ComponentSerialNumber);
    QCOMPARE(TestObject6.m_ComponentFirmwareVersion,                    TestObject2.m_ComponentFirmwareVersion);
    QCOMPARE(TestObject6.m_ComponentHardwareVersion,                    TestObject2.m_ComponentHardwareVersion);
    QCOMPARE(TestObject6.m_MainboardSoftwareVersion,                    TestObject2.m_MainboardSoftwareVersion);
    QCOMPARE(TestObject6.m_TestResults.size(),                          TestObject2.m_TestResults.size());
    QCOMPARE(TestObject6.m_TestResults[0].GetStringID(),                TestObject2.m_TestResults[0].GetStringID());
    QCOMPARE(TestObject6.m_TestResults[0].GetArgumentList().size(),     TestObject2.m_TestResults[0].GetArgumentList().size());
    QCOMPARE(TestObject6.m_TestResults[1].GetStringID(),                TestObject2.m_TestResults[1].GetStringID());
    QCOMPARE(TestObject6.m_TestResults[1].GetArgumentList().size(),     TestObject2.m_TestResults[1].GetArgumentList().size());

    // assign to self
    TestObject6 = TestObject6;
    QCOMPARE(TestObject6.m_ComponentName,                               TestObject2.m_ComponentName);
    QCOMPARE(TestObject6.m_BoardName,                                   TestObject2.m_BoardName);
    QCOMPARE(TestObject6.m_BoardSerialNumber,                           TestObject2.m_BoardSerialNumber);
    QCOMPARE(TestObject6.m_TesterName,                                  TestObject2.m_TesterName);
    QCOMPARE(TestObject6.m_DeviceSerialNumber,                          TestObject2.m_DeviceSerialNumber);
    QCOMPARE(TestObject6.m_ComponentSerialNumber,                       TestObject2.m_ComponentSerialNumber);
    QCOMPARE(TestObject6.m_ComponentFirmwareVersion,                    TestObject2.m_ComponentFirmwareVersion);
    QCOMPARE(TestObject6.m_ComponentHardwareVersion,                    TestObject2.m_ComponentHardwareVersion);
    QCOMPARE(TestObject6.m_MainboardSoftwareVersion,                    TestObject2.m_MainboardSoftwareVersion);
    QCOMPARE(TestObject6.m_TestResults.size(),                          TestObject2.m_TestResults.size());
    QCOMPARE(TestObject6.m_TestResults[0].GetStringID(),                TestObject2.m_TestResults[0].GetStringID());
    QCOMPARE(TestObject6.m_TestResults[0].GetArgumentList().size(),     TestObject2.m_TestResults[0].GetArgumentList().size());
    QCOMPARE(TestObject6.m_TestResults[1].GetStringID(),                TestObject2.m_TestResults[1].GetStringID());
    QCOMPARE(TestObject6.m_TestResults[1].GetArgumentList().size(),     TestObject2.m_TestResults[1].GetArgumentList().size());
}

/****************************************************************************/
void TestComponentTestEntry::utGet() {
    // test get methods
    Global::tTranslatableStringList AdditionalData1 =   Global::tTranslatableStringList() <<
                                                        Global::TranslatableString(11, Global::tTranslatableStringList() << "111") <<
                                                        Global::TranslatableString(12, Global::tTranslatableStringList() << "121" << "122");
    Global::tTranslatableStringList AdditionalData2 =   Global::tTranslatableStringList() <<
                                                        Global::TranslatableString(21, Global::tTranslatableStringList() << "211") <<
                                                        Global::TranslatableString(22, Global::tTranslatableStringList() << "221" << "222") <<
                                                        Global::TranslatableString(23, Global::tTranslatableStringList() << "231" << "232" << "233");
    ComponentTestEntry TestObject1("11", "12", "13", "14", "15", "16", "17", "18", "19", AdditionalData1);
    ComponentTestEntry TestObject2("21", "22", "23", "24", "25", "26", "27", "28", "29", AdditionalData2);

    QCOMPARE(TestObject1.GetComponentName(),                            QString("11"));
    QCOMPARE(TestObject1.GetBoardName(),                                QString("12"));
    QCOMPARE(TestObject1.GetBoardSerialNumber(),                        QString("13"));
    QCOMPARE(TestObject1.GetTesterName(),                               QString("14"));
    QCOMPARE(TestObject1.GetDeviceSerialNumber(),                       QString("15"));
    QCOMPARE(TestObject1.GetComponentSerialNumber(),                    QString("16"));
    QCOMPARE(TestObject1.GetComponentFirmwareVersion(),                 QString("17"));
    QCOMPARE(TestObject1.GetComponentHardwareVersion(),                 QString("18"));
    QCOMPARE(TestObject1.GetMainboardSoftwareVersion(),                 QString("19"));
    QCOMPARE(TestObject1.GetTestResults().size(),                       2);
    QCOMPARE(TestObject1.GetTestResults()[0].GetStringID(),             quint32(11));
    QCOMPARE(TestObject1.GetTestResults()[0].GetArgumentList().size(),  1);
    QCOMPARE(TestObject1.GetTestResults()[1].GetStringID(),             quint32(12));
    QCOMPARE(TestObject1.GetTestResults()[1].GetArgumentList().size(),  2);

    QCOMPARE(TestObject2.GetComponentName(),                            QString("21"));
    QCOMPARE(TestObject2.GetBoardName(),                                QString("22"));
    QCOMPARE(TestObject2.GetBoardSerialNumber(),                        QString("23"));
    QCOMPARE(TestObject2.GetTesterName(),                               QString("24"));
    QCOMPARE(TestObject2.GetDeviceSerialNumber(),                       QString("25"));
    QCOMPARE(TestObject2.GetComponentSerialNumber(),                    QString("26"));
    QCOMPARE(TestObject2.GetComponentFirmwareVersion(),                 QString("27"));
    QCOMPARE(TestObject2.GetComponentHardwareVersion(),                 QString("28"));
    QCOMPARE(TestObject2.GetMainboardSoftwareVersion(),                 QString("29"));
    QCOMPARE(TestObject2.GetTestResults().size(),                       3);
    QCOMPARE(TestObject2.GetTestResults()[0].GetStringID(),             quint32(21));
    QCOMPARE(TestObject2.GetTestResults()[0].GetArgumentList().size(),  1);
    QCOMPARE(TestObject2.GetTestResults()[1].GetStringID(),             quint32(22));
    QCOMPARE(TestObject2.GetTestResults()[1].GetArgumentList().size(),  2);
    QCOMPARE(TestObject2.GetTestResults()[2].GetStringID(),             quint32(23));
    QCOMPARE(TestObject2.GetTestResults()[2].GetArgumentList().size(),  3);
}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestComponentTestEntry)

#include "TestComponentTestEntry.moc"
