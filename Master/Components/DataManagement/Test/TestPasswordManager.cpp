/****************************************************************************/
/*! \file TestPasswordManager.cpp
 *
 *  \brief Implementation file for class TestPasswordManager.
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
#include <DataManagement/Include/PasswordManager.h>
#include <DataManagement/Include/DataManagementEventCodes.h>
#include <Global/Include/Exception.h>

namespace DataManager {

/****************************************************************************/
/**
 * \brief Test class for PasswordManager class.
 */
/****************************************************************************/
class TestPasswordManager : public QObject {
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
     * \brief Test Clear method.
     */
    /****************************************************************************/
    void utClear();
    /****************************************************************************/
    /**
     * \brief Test setting and checking passwords.
     */
    /****************************************************************************/
    void utComputeHash();
    /****************************************************************************/
    /**
     * \brief Test password format check OK.
     */
    /****************************************************************************/
    void utPasswordFormat();
    /****************************************************************************/
    /**
     * \brief Data for password format check OK.
     */
    /****************************************************************************/
    void utPasswordFormat_data();
    /****************************************************************************/
    /**
     * \brief Test setting and checking passwords.
     */
    /****************************************************************************/
    void utPasswords();
}; // end class TestPasswordManager

/****************************************************************************/
void TestPasswordManager::initTestCase() {
}

/****************************************************************************/
void TestPasswordManager::init() {
}

/****************************************************************************/
void TestPasswordManager::cleanup() {
}

/****************************************************************************/
void TestPasswordManager::cleanupTestCase() {
}

/****************************************************************************/
void TestPasswordManager::utConstructor() {
    // test constructor
    PasswordManager Obj("MasterPasswordHash");
    QCOMPARE(Obj.m_Passwords.size(),    0);
}

/****************************************************************************/
void TestPasswordManager::utClear() {
    PasswordManager Obj("MasterPasswordHash");
    QCOMPARE(Obj.m_Passwords.size(),    0);

    // insert some values directly
    Obj.m_Passwords.insert("key1", "value1");
    Obj.m_Passwords.insert("key2", "value2");
    Obj.m_Passwords.insert("key3", "value3");
    QCOMPARE(Obj.m_Passwords.size(),    3);

    // call Clear
    Obj.Clear();
    // check results
    QCOMPARE(Obj.m_Passwords.size(),    0);
}

/****************************************************************************/
void TestPasswordManager::utComputeHash() {
    PasswordManager Obj("MasterPasswordHash");
    QCOMPARE(Obj.ComputeHash(""),                       QString("D41D8CD98F00B204E9800998ECF8427E"));
    QCOMPARE(Obj.ComputeHash("12345"),                  QString("0DBCEC4A5FE4AE88C60B2DB1724563E2"));
    QCOMPARE(Obj.ComputeHash("1234512345"),             QString("0DBCEC4A5FE4AE88C60B2DB1724563E2"));
    QCOMPARE(Obj.ComputeHash("123451234512345"),        QString("0DBCEC4A5FE4AE88C60B2DB1724563E2"));
    QCOMPARE(Obj.ComputeHash("1234512345123451"),       QString("0DBCEC4A5FE4AE88C60B2DB1724563E2"));
    QCOMPARE(Obj.ComputeHash("12345123451234519"),      QString("0DBCEC4A5FE4AE88C60B2DB1724563E2"));
    QCOMPARE(Obj.ComputeHash("1234512345123451987"),    QString("0DBCEC4A5FE4AE88C60B2DB1724563E2"));
}

/****************************************************************************/
void TestPasswordManager::utPasswordFormat() {
    QFETCH(QString, Password);
    QFETCH(bool, ExpectedResult);

    QCOMPARE(PasswordManager::CheckPasswordFormat(Password), ExpectedResult);
}

/****************************************************************************/
void TestPasswordManager::utPasswordFormat_data() {
    QTest::addColumn<QString>("Password");
    QTest::addColumn<bool>("ExpectedResult");

    // check for length
    QTest::newRow("Empty password")         << ""                               << false;
    QTest::newRow("password < 4 chars")     << "123"                            << false;
    QTest::newRow("password = 4 chars")     << "1234"                           << true;
    QTest::newRow("password = 10 chars")    << "1234567890"                     << true;
    QTest::newRow("password = 16 chars")    << "1234567890123456"               << true;
    QTest::newRow("password = 17 chars")    << "12345678901234567"              << false;
    QTest::newRow("password = 20 chars")    << "12345678901234567890"           << false;
    // check for printable ASCII >= 0x20 && < 0x7F
    QTest::newRow("ASCII < 0x20")           << QString("12345") + QChar(0x19) + QString("78")   << false;
    QTest::newRow("ASCII = 0x20")           << QString("12345") + QChar(0x20) + QString("78")   << true;
    QTest::newRow("ASCII < 0x7F")           << QString("12345") + QChar(0x7E) + QString("78")   << true;
    QTest::newRow("ASCII = 0x7F")           << QString("12345") + QChar(0x7F) + QString("78")   << false;
    QTest::newRow("ASCII = 0x80")           << QString("12345") + QChar(0x80) + QString("78")   << false;
}

/****************************************************************************/
void TestPasswordManager::utPasswords() {
    // start with empty test object
    PasswordManager Obj("0DBCEC4A5FE4AE88C60B2DB1724563E2");

    // check default administrator password
    QCOMPARE(Obj.CheckPassword("Administrator", "1234"),        false);
    QCOMPARE(Obj.CheckPassword("Administratos", "12345"),       false);
    QCOMPARE(Obj.CheckPassword("Administrator", "12345"),       true);
    QCOMPARE(Obj.CheckPassword("Administrator", "1234512345"),  true);

    // check some not existing passwords
    QCOMPARE(Obj.CheckPassword("name1", "password1"),   false);
    QCOMPARE(Obj.CheckPassword("name2", "password2"),   false);
    QCOMPARE(Obj.CheckPassword("name3", "password3"),   false);
    QCOMPARE(Obj.CheckPassword("name4", "password4"),   false);
    QCOMPARE(Obj.CheckPassword("name5", "password5"),   false);
    QCOMPARE(Obj.CheckPassword("name6", "password6"),   false);

    // add passwords with SetPassword
    Obj.SetPassword("name1", "password1");
    Obj.SetPassword("name4", "password4");
    // check
    QCOMPARE(Obj.CheckPassword("name1", "password1"),   true);
    QCOMPARE(Obj.CheckPassword("name2", "password2"),   false);
    QCOMPARE(Obj.CheckPassword("name3", "password3"),   false);
    QCOMPARE(Obj.CheckPassword("name4", "password4"),   true);
    QCOMPARE(Obj.CheckPassword("name5", "password5"),   false);
    QCOMPARE(Obj.CheckPassword("name6", "password6"),   false);

    // add passwords with SetPasswordHash
    Obj.SetPasswordHash("name2", "2F5D5AC6C8717BC3CA60AC15F6A67BBD");
    Obj.SetPasswordHash("name5", "72B66F8729A8724AF62C5499482DBBF8");
    // check
    QCOMPARE(Obj.CheckPassword("name1", "password1"),   true);
    QCOMPARE(Obj.CheckPassword("name2", "password2"),   true);
    QCOMPARE(Obj.CheckPassword("name3", "password3"),   false);
    QCOMPARE(Obj.CheckPassword("name4", "password4"),   true);
    QCOMPARE(Obj.CheckPassword("name5", "password5"),   true);
    QCOMPARE(Obj.CheckPassword("name6", "password6"),   false);

    // override passwords
    Obj.SetPassword("name1", "password11");
    Obj.SetPasswordHash("name2", "D8E33AF3592E80CE22678281B906AF42");
    QCOMPARE(Obj.CheckPassword("name1", "password1"),   false);
    QCOMPARE(Obj.CheckPassword("name1", "password11"),  true);
    QCOMPARE(Obj.CheckPassword("name2", "password2"),   false);
    QCOMPARE(Obj.CheckPassword("name2", "password22"),  true);
    QCOMPARE(Obj.CheckPassword("name3", "password3"),   false);
    QCOMPARE(Obj.CheckPassword("name4", "password4"),   true);
    QCOMPARE(Obj.CheckPassword("name5", "password5"),   true);
    QCOMPARE(Obj.CheckPassword("name6", "password6"),   false);

    // check for case sensitivity
    QCOMPARE(Obj.CheckPassword("name4", "password4"),   true);
    QCOMPARE(Obj.CheckPassword("name4", "Password4"),   false);
    QCOMPARE(Obj.CheckPassword("Name4", "password4"),   false);
    QCOMPARE(Obj.CheckPassword("Name4", "Password4"),   false);

    // invalid password
    try {
        Obj.SetPassword("123", "123");
        QFAIL("You should never get here!");
    } catch(const Global::Exception &E) {
        QCOMPARE(E.GetErrorCode(), EVENT_DATAMANAGER_ERROR_PASSWORD_FORMAT);
    } catch (...) {
        QFAIL("You should never get here!");
    }
}

} // end namespace DataManager

QTEST_MAIN(DataManager::TestPasswordManager)

#include "TestPasswordManager.moc"
