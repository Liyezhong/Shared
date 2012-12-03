/****************************************************************************/
/*! \file TestSystemPaths.cpp
 *
 *  \brief Implementation file for class TestSystemPaths.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-09-10
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
#include <Global/Include/SystemPaths.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Test class for SystemPaths class.
 */
/****************************************************************************/
class TestSystemPaths : public QObject {
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
    void utConstructor();
    /****************************************************************************/
    /**
     * \brief Test Get and Set methods.
     */
    /****************************************************************************/
    void utGetSetMethods();
}; // end class TestSystemPaths

/****************************************************************************/
void TestSystemPaths::initTestCase() {
}

/****************************************************************************/
void TestSystemPaths::init() {
}

/****************************************************************************/
void TestSystemPaths::cleanup() {
}

/****************************************************************************/
void TestSystemPaths::cleanupTestCase() {
}

/****************************************************************************/
void TestSystemPaths::utConstructor() {
    // test constructor
    QCOMPARE(SystemPaths::Instance().m_SettingsPath,        QString(""));
    QCOMPARE(SystemPaths::Instance().m_FirmwarePath,        QString(""));
    QCOMPARE(SystemPaths::Instance().m_UpdatePath,          QString(""));
    QCOMPARE(SystemPaths::Instance().m_LogfilesPath,        QString(""));
    QCOMPARE(SystemPaths::Instance().m_ComponentTestPath,   QString(""));
    QCOMPARE(SystemPaths::Instance().m_ManualPath,          QString(""));
}

/****************************************************************************/
void TestSystemPaths::utGetSetMethods() {
    // check for default values
    QCOMPARE(SystemPaths::Instance().m_SettingsPath,        QString(""));
    QCOMPARE(SystemPaths::Instance().m_FirmwarePath,        QString(""));
    QCOMPARE(SystemPaths::Instance().m_UpdatePath,          QString(""));
    QCOMPARE(SystemPaths::Instance().m_LogfilesPath,        QString(""));
    QCOMPARE(SystemPaths::Instance().m_ComponentTestPath,   QString(""));
    QCOMPARE(SystemPaths::Instance().m_ManualPath,          QString(""));

    // set directories
    SystemPaths::Instance().SetSettingsPath("../SettingsPath/..///SettingsPath/");
    SystemPaths::Instance().SetFirmwarePath("../FirmwarePath/..///FirmwarePath/");
    SystemPaths::Instance().SetUpdatePath("../UpdatePath/..///UpdatePath/");
    SystemPaths::Instance().SetLogfilesPath("../LogfilesPath/..///LogfilesPath/");
    SystemPaths::Instance().SetComponentTestPath("../ComponentTestPath/..///ComponentTestPath/");
    SystemPaths::Instance().SetManualPath("../ManualPath/..///ManualPath/");

    // now check values
    QCOMPARE(SystemPaths::Instance().m_SettingsPath,        QDir::cleanPath(SystemPaths::ComputePath() + "/" + "../SettingsPath"));
    QCOMPARE(SystemPaths::Instance().m_FirmwarePath,        QDir::cleanPath(SystemPaths::ComputePath() + "/" + "../FirmwarePath"));
    QCOMPARE(SystemPaths::Instance().m_UpdatePath,          QDir::cleanPath(SystemPaths::ComputePath() + "/" + "../UpdatePath"));
    QCOMPARE(SystemPaths::Instance().m_LogfilesPath,        QDir::cleanPath(SystemPaths::ComputePath() + "/" + "../LogfilesPath"));
    QCOMPARE(SystemPaths::Instance().m_ComponentTestPath,   QDir::cleanPath(SystemPaths::ComputePath() + "/" + "../ComponentTestPath"));
    QCOMPARE(SystemPaths::Instance().m_ManualPath,          QDir::cleanPath(SystemPaths::ComputePath() + "/" + "../ManualPath"));
    QVERIFY(!SystemPaths::Instance().m_SettingsPath.contains(".."));
    QVERIFY(!SystemPaths::Instance().m_FirmwarePath.contains(".."));
    QVERIFY(!SystemPaths::Instance().m_UpdatePath.contains(".."));
    QVERIFY(!SystemPaths::Instance().m_LogfilesPath.contains(".."));
    QVERIFY(!SystemPaths::Instance().m_ComponentTestPath.contains(".."));
    QVERIFY(!SystemPaths::Instance().m_ManualPath.contains(".."));
    QVERIFY(!SystemPaths::Instance().m_SettingsPath.contains("//"));
    QVERIFY(!SystemPaths::Instance().m_FirmwarePath.contains("//"));
    QVERIFY(!SystemPaths::Instance().m_UpdatePath.contains("//"));
    QVERIFY(!SystemPaths::Instance().m_LogfilesPath.contains("//"));
    QVERIFY(!SystemPaths::Instance().m_ComponentTestPath.contains("//"));
    QVERIFY(!SystemPaths::Instance().m_ManualPath.contains("//"));

    // check get methods
    QCOMPARE(SystemPaths::Instance().GetSettingsPath(),         SystemPaths::Instance().m_SettingsPath);
    QCOMPARE(SystemPaths::Instance().GetFirmwarePath(),         SystemPaths::Instance().m_FirmwarePath);
    QCOMPARE(SystemPaths::Instance().GetUpdatePath(),           SystemPaths::Instance().m_UpdatePath);
    QCOMPARE(SystemPaths::Instance().GetLogfilesPath(),         SystemPaths::Instance().m_LogfilesPath);
    QCOMPARE(SystemPaths::Instance().GetComponentTestPath(),    SystemPaths::Instance().m_ComponentTestPath);
    QCOMPARE(SystemPaths::Instance().GetManualPath(),           SystemPaths::Instance().m_ManualPath);

}

} // end namespace Global

QTEST_MAIN(Global::TestSystemPaths)

#include "TestSystemPaths.moc"
