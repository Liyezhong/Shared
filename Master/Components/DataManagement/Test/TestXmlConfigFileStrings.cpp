/****************************************************************************/
/*! \file TestXmlConfigFileStrings.cpp
 *
 *  \brief Implementation file for class TestXmlConfigFileStrings.
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
#include <DataManagement/Include/XmlConfigFileStrings.h>
#include <DataManagement/Include/DataManagementEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>

namespace DataManager {

static QString FilesPathErr;    ///< Path to xml file that are OK.
static QString FilesPathOK;     ///< Path to xml file that are not OK.

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFileStrings class.
 */
/****************************************************************************/
class TestXmlConfigFileStrings : public QObject {
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
    void utTestReadStringsOK();
    /****************************************************************************/
    /**
     * \brief Test read NOK.
     */
    /****************************************************************************/
    void utTestReadStringsNOK();
    /****************************************************************************/
    /**
     * \brief Data for test read NOK.
     */
    /****************************************************************************/
    void utTestReadStringsNOK_data();
}; // end class TestXmlConfigFileStrings

/****************************************************************************/
void TestXmlConfigFileStrings::initTestCase() {
    // set paths
    QString ExePath = QCoreApplication::applicationDirPath();
    FilesPathErr = ExePath + "/../xmlErrFiles/";
    FilesPathOK  = ExePath + "/../xmlOkFiles/";
    // init languages
    Global::InitSupportedLanguages();
}

/****************************************************************************/
void TestXmlConfigFileStrings::init() {
}

/****************************************************************************/
void TestXmlConfigFileStrings::cleanup() {
}

/****************************************************************************/
void TestXmlConfigFileStrings::cleanupTestCase() {
}

/****************************************************************************/
void TestXmlConfigFileStrings::utTestReadStringsOK() {
    try {
        XmlConfigFileStrings Obj;
        QSet<QLocale::Language> LanguageList;
        LanguageList << QLocale::English << QLocale::German << QLocale::Greek;
        Obj.ReadStrings(FilesPathOK + "XmlConfigFileStringsOK.xml", LanguageList);
        // now test results
        QCOMPARE(LanguageList.size(),                       1);
        QCOMPARE(Obj.Data().size(),                         2);
        QCOMPARE(Obj.m_Data.value(QLocale::English).size(), 3);
        QCOMPARE(Obj.m_Data.value(QLocale::German).size(),  3);
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestXmlConfigFileStrings::utTestReadStringsNOK() {
    QFETCH(QString, FileName);
    QFETCH(quint32, ExpectedErrorCode);

    try {
        XmlConfigFileStrings Obj;
        QSet<QLocale::Language> LanguageList;
        LanguageList << QLocale::English << QLocale::German << QLocale::Greek;
        Obj.ReadStrings(FilesPathErr + FileName, LanguageList);
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), ExpectedErrorCode);
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestXmlConfigFileStrings::utTestReadStringsNOK_data() {
    QTest::addColumn<QString>("FileName");
    QTest::addColumn<quint32>("ExpectedErrorCode");

    QTest::newRow("file not found")                             << "XmlConfigFileStringsNOKNotHere.xml" << Global::EVENT_GLOBAL_ERROR_FILE_OPEN;
    QTest::newRow("buggy root element")                         << "XmlConfigFileStringsNOK1.xml"       << EVENT_DATAMANAGER_ERROR_UNEXPECTED_XML_STARTELEMENT;
    QTest::newRow("unsupported format version")                 << "XmlConfigFileStringsNOK2.xml"       << EVENT_DATAMANAGER_ERROR_UNSUPPORTED_VERSION;
    QTest::newRow("unexpected element instead of \"language\"") << "XmlConfigFileStringsNOK3.xml"       << EVENT_DATAMANAGER_ERROR_UNEXPECTED_XML_STARTELEMENT;
    QTest::newRow("unexpected element instead of \"string\"")   << "XmlConfigFileStringsNOK4.xml"       << EVENT_DATAMANAGER_ERROR_UNEXPECTED_XML_STARTELEMENT;
    QTest::newRow("attribute \"id\" not found")                 << "XmlConfigFileStringsNOK5.xml"       << EVENT_DATAMANAGER_ERROR_XML_ATTRIBUTE_NOT_FOUND;
    QTest::newRow("attribute \"text\" not found")               << "XmlConfigFileStringsNOK6.xml"       << EVENT_DATAMANAGER_ERROR_XML_ATTRIBUTE_NOT_FOUND;
    QTest::newRow("not supported language \"Chinese\"")         << "XmlConfigFileStringsNOK7.xml"       << EVENT_DATAMANAGER_ERROR_NOT_SUPPORTED_LANGUAGE;
}

} // end namespace DataLogging

QTEST_MAIN(DataManager::TestXmlConfigFileStrings)

#include "TestXmlConfigFileStrings.moc"
