/****************************************************************************/
/*! \file TestXmlConfigFilePasswords.cpp
 *
 *  \brief Implementation file for class TestXmlConfigFilePasswords.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-05-27
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
#include <DataManager/Helper/Include/XmlConfigFilePasswords.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>

namespace DataManager {

static QString FilesPathErr;    ///< Path to xml file that are OK.
static QString FilesPathOK;     ///< Path to xml file that are not OK.
static QString FilesPathWrite;  ///< Path to where we can write some files.

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFilePasswords class.
 */
/****************************************************************************/
class TestXmlConfigFilePasswords : public QObject {
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
     * \brief Test read OK.
     */
    /****************************************************************************/
    void utTestReadPasswordsOK();
    /****************************************************************************/
    /**
     * \brief Test read NOK.
     */
    /****************************************************************************/
    void utTestReadPasswordsNOK();
    /****************************************************************************/
    /**
     * \brief Date for test read NOK.
     */
    /****************************************************************************/
    void utTestReadPasswordsNOK_data();
    /****************************************************************************/
    /**
     * \brief Test write.
     */
    /****************************************************************************/
    void utTestWritePasswords();
}; // end class TestXmlConfigFilePasswords

/****************************************************************************/
void TestXmlConfigFilePasswords::initTestCase() {
    // set paths
    QString ExePath = QCoreApplication::applicationDirPath();
    FilesPathErr = ExePath + "/../xmlErrFiles/";
    FilesPathOK  = ExePath + "/../xmlOkFiles/";
    FilesPathWrite = ExePath + "/";
}

/****************************************************************************/
void TestXmlConfigFilePasswords::init() {
}

/****************************************************************************/
void TestXmlConfigFilePasswords::cleanup() {
}

/****************************************************************************/
void TestXmlConfigFilePasswords::cleanupTestCase() {
}

/****************************************************************************/
void TestXmlConfigFilePasswords::utTestReadPasswordsOK() {
    try {
        XmlConfigFilePasswords Obj("holdrio4711");
        PasswordManager::CPasswordManager PwdMgr("123");
        Obj.ReadPasswords(FilesPathOK + "XmlConfigFilePasswordsOK.xml", PwdMgr);
        // now test results
        QVERIFY(PwdMgr.CheckPassword("User1", "Passwd1"));
        QVERIFY(PwdMgr.CheckPassword("User2", "Passwd2"));
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestXmlConfigFilePasswords::utTestReadPasswordsNOK() {
    QFETCH(QString, FileName);
    QFETCH(quint32, ExpectedErrorCode);

    try {
        XmlConfigFilePasswords Obj("holdrio4711");
        PasswordManager::CPasswordManager PwdMgr("123");
        Obj.ReadPasswords(FilesPathErr + FileName, PwdMgr);
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), ExpectedErrorCode);
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestXmlConfigFilePasswords::utTestReadPasswordsNOK_data() {
    QTest::addColumn<QString>("FileName");
    QTest::addColumn<quint32>("ExpectedErrorCode");

    QTest::newRow("file not found")                                 << "XmlConfigFilePasswordsNOKNotHere.xml"   << EVENT_GLOBAL_ERROR_FILE_OPEN;
    QTest::newRow("buggy root element")                             << "XmlConfigFilePasswordsNOK1.xml"         << EVENT_DM_ERROR_UNEXPECTED_XML_STARTELEMENT;
    QTest::newRow("unsupported format version")                     << "XmlConfigFilePasswordsNOK2.xml"         << EVENT_DM_ERROR_UNSUPPORTED_VERSION;
    QTest::newRow("unexpected element instead of \"serialnumber\"") << "XmlConfigFilePasswordsNOK3.xml"         << EVENT_DM_ERROR_UNEXPECTED_XML_STARTELEMENT;
    QTest::newRow("wrong serial number")                            << "XmlConfigFilePasswordsNOK4.xml"         << EVENT_DM_ERROR_PASSWORD_SERIAL_NUMBER;
    QTest::newRow("unxpected element instead of \"password\"")      << "XmlConfigFilePasswordsNOK5.xml"         << EVENT_DM_ERROR_UNEXPECTED_XML_STARTELEMENT;
    QTest::newRow("attribute \"name\" not found")                   << "XmlConfigFilePasswordsNOK6.xml"         << EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND;
    QTest::newRow("attribute \"hash\" not found")                   << "XmlConfigFilePasswordsNOK7.xml"         << EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND;
}

/****************************************************************************/
void TestXmlConfigFilePasswords::utTestWritePasswords() {
    QString FileName = FilesPathWrite + "dummy.xml";
    try {
        // write passwords
        {
            XmlConfigFilePasswords Obj("sernum");
            PasswordManager::CPasswordManager PwdMgr("123");
            PwdMgr.SetPassword("name1", "passwd1");
            PwdMgr.SetPassword("name2", "passwd2");
            PwdMgr.SetPassword("name3", "passwd3");
            Obj.WritePasswords(FileName, PwdMgr, "sernum");
        }
        // now try to read passwords
        {
            XmlConfigFilePasswords Obj("sernum");
            PasswordManager::CPasswordManager PwdMgr("123");
            // set some passwords
            PwdMgr.SetPassword("name1", "dummy1");
            PwdMgr.SetPassword("name2", "dummy2");
            PwdMgr.SetPassword("name3", "dummy3");
            // check passwords
            QVERIFY(PwdMgr.CheckPassword("name1",  "dummy1"));
            QVERIFY(PwdMgr.CheckPassword("name2",  "dummy2"));
            QVERIFY(PwdMgr.CheckPassword("name3",  "dummy3"));
            QVERIFY(!PwdMgr.CheckPassword("name1", "passwd1"));
            QVERIFY(!PwdMgr.CheckPassword("name2", "passwd2"));
            QVERIFY(!PwdMgr.CheckPassword("name3", "passwd3"));
            // read passwords
            Obj.ReadPasswords(FileName, PwdMgr);
            // Check new passwords. The old ones should not work anymore
            QVERIFY(!PwdMgr.CheckPassword("name1", "dummy1"));
            QVERIFY(!PwdMgr.CheckPassword("name2", "dummy2"));
            QVERIFY(!PwdMgr.CheckPassword("name3", "dummy3"));
            QVERIFY(PwdMgr.CheckPassword("name1", "passwd1"));
            QVERIFY(PwdMgr.CheckPassword("name2", "passwd2"));
            QVERIFY(PwdMgr.CheckPassword("name3", "passwd3"));
        }
    } catch(...) {
        QFAIL("You should never get here!");
    }
    // remove dummy file again
    QFile::remove(FileName);
}

} // end namespace DataLogging

QTEST_MAIN(DataManager::TestXmlConfigFilePasswords)

#include "TestXmlConfigFilePasswords.moc"
