/****************************************************************************/
/*! \file TestServicePassword.cpp
 *
 *  \brief Implementation file for class TestServicePassword.
 *
 *  \b Description:
 *      Checks all the information related to service passwoed. Reads the
 *      'LBSaccess.key" file and compares the information with the entered
 *      data by user.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-06
 *  $Author:    $ Raju
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
#include <PasswordManager/Include/ServicePassword.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include <Global/Include/Exception.h>

namespace PasswordManager {

/****************************************************************************/
/**
 * \brief Test class for PasswordManager class.
 */
/****************************************************************************/
class TestServicePassword : public QObject {
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
     * \brief Test service data.
     */
    /****************************************************************************/
    void utServiceData();

}; // end class TestServicePassword

/****************************************************************************/
void TestServicePassword::initTestCase() {
}

/****************************************************************************/
void TestServicePassword::init() {
}

/****************************************************************************/
void TestServicePassword::cleanup() {
}

/****************************************************************************/
void TestServicePassword::cleanupTestCase() {
}

/****************************************************************************/
void TestServicePassword::utServiceData() {
    CServicePassword Service("1234", "ST8200");
    QDateTime Today;

    // check the date comaprision
    Service.m_ServiceFileContent = "<validto>0000000</validto>";
    QCOMPARE(Service.CompareDate(), false);
    Service.m_ServiceFileContent.clear();
    Service.m_ServiceFileContent.append(QString("<validto>" + Today.currentDateTime().toString("yyyyMMdd") + "</validto>"));
    QCOMPARE(Service.CompareDate(), true);

    // check the service ID value
    Service.m_ServiceFileContent.clear();
    Service.m_ServiceFileContent = "<mobileID>123456</mobileID>";
    QCOMPARE(Service.ReadServiceID(), QString("123456"));

    Service.m_ServiceFileContent.clear();
    Service.m_ServiceFileContent = "<mobileID>789654</mobileID>";
    QCOMPARE(Service.ReadServiceID(), QString("789654"));

    Service.m_ServiceFileContent.clear();
    Service.m_ServiceFileContent = "<mobileID>00000000</mobileID>";
    QCOMPARE(Service.ReadServiceID(), QString("00000000"));

    // check the Device tag name existence
    Service.m_ServiceFileContent.clear();
    Service.m_DeviceName = "ST8200";
    Service.m_ServiceFileContent = "<instruments><instrument><name>ST8200</name><basic>1</basic><advanced>0</advanced></instrument></instruments>";
    QCOMPARE(Service.ReadDeviceNameTagsExistence(), true);

    Service.m_DeviceName = "ST 8200";
    QCOMPARE(Service.ReadDeviceNameTagsExistence(), false);

    Service.m_ServiceFileContent.clear();
    Service.m_DeviceName = "ST8200";
    Service.m_ServiceFileContent = "<instruments><instrument><name>ST8200</name><basic>0</basic><advanced>0</advanced></instrument></instruments>";
    QCOMPARE(Service.ReadDeviceNameTagsExistence(), false);

    Service.m_ServiceFileContent.clear();
    Service.m_DeviceName = "ST8200";
    Service.m_ServiceFileContent = "<instruments><instrument><name>ST 8200</name><basic>1</basic><advanced>0</advanced></instrument></instruments>";
    QCOMPARE(Service.ReadDeviceNameTagsExistence(), false);

    Service.m_ServiceFileContent.clear();
    Service.m_DeviceName = "ST8200";
    Service.m_ServiceFileContent = "<instruments><instrument><name>ST 8200</name><basic>0</basic><advanced>0</advanced></instrument></instruments>";
    QCOMPARE(Service.ReadDeviceNameTagsExistence(), false);

}

} // end namespace PasswordManager

QTEST_MAIN(PasswordManager::TestServicePassword)

#include "TestServicePassword.moc"
