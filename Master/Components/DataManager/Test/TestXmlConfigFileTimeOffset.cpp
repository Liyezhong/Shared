/****************************************************************************/
/*! \file TestXmlConfigFileTimeOffset.cpp
 *
 *  \brief Implementation file for class TestXmlConfigFileTimeOffset.
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
#include <DataManager/Helper/Include/XmlConfigFileTimeOffset.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>

namespace DataManager {

static QString FilesPathErr;    ///< Path to xml file that are OK.
static QString FilesPathOK;     ///< Path to xml file that are not OK.
static QString FilesPathWrite;  ///< Path to where we can write some files.

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFileTimeOffset class.
 */
/****************************************************************************/
class TestXmlConfigFileTimeOffset : public QObject {
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
    void utTestReadTimeOffsetOK();
    /****************************************************************************/
    /**
     * \brief Test read NOK.
     */
    /****************************************************************************/
    void utTestReadTimeOffsetNOK();
    /****************************************************************************/
    /**
     * \brief Data for test read NOK.
     */
    /****************************************************************************/
    void utTestReadTimeOffsetNOK_data();
    /****************************************************************************/
    /**
     * \brief Test write.
     */
    /****************************************************************************/
    void utTestWriteTimeOffset();
}; // end class TestXmlConfigFileTimeOffset

/****************************************************************************/
void TestXmlConfigFileTimeOffset::initTestCase() {
    // set paths
    QString ExePath = QCoreApplication::applicationDirPath();
    FilesPathErr = ExePath + "/../xmlErrFiles/";
    FilesPathOK  = ExePath + "/../xmlOkFiles/";
    FilesPathWrite = ExePath + "/";
}

/****************************************************************************/
void TestXmlConfigFileTimeOffset::init() {
}

/****************************************************************************/
void TestXmlConfigFileTimeOffset::cleanup() {
}

/****************************************************************************/
void TestXmlConfigFileTimeOffset::cleanupTestCase() {
}

/****************************************************************************/
void TestXmlConfigFileTimeOffset::utTestReadTimeOffsetOK() {
    try {
        XmlConfigFileTimeOffset Obj;
        int Offset = 0;
        Obj.ReadTimeOffset(FilesPathOK + "XmlConfigFileTimeOffsetOK.xml", Offset);
        // now test results
        QCOMPARE(Offset, -42);
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestXmlConfigFileTimeOffset::utTestReadTimeOffsetNOK() {
    QFETCH(QString, FileName);
    QFETCH(quint32, ExpectedErrorCode);

    try {
        XmlConfigFileTimeOffset Obj;
        int Offset = 0;
        Obj.ReadTimeOffset(FilesPathErr + FileName, Offset);
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), ExpectedErrorCode);
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestXmlConfigFileTimeOffset::utTestReadTimeOffsetNOK_data() {
    QTest::addColumn<QString>("FileName");
    QTest::addColumn<quint32>("ExpectedErrorCode");

    QTest::newRow("file not found")                             << "XmlConfigFileTimeOffsetNOKNotHere.xml"  << Global::EVENT_GLOBAL_ERROR_FILE_OPEN;
    QTest::newRow("buggy root element")                         << "XmlConfigFileTimeOffsetNOK1.xml"        << EVENT_DATAMANAGER_ERROR_UNEXPECTED_XML_STARTELEMENT;
    QTest::newRow("unsupported format version")                 << "XmlConfigFileTimeOffsetNOK2.xml"        << EVENT_DATAMANAGER_ERROR_UNSUPPORTED_VERSION;
    QTest::newRow("not expected element instead of \"offset\"") << "XmlConfigFileTimeOffsetNOK3.xml"        << EVENT_DATAMANAGER_ERROR_UNEXPECTED_XML_STARTELEMENT;
}

/****************************************************************************/
void TestXmlConfigFileTimeOffset::utTestWriteTimeOffset() {
    QString FileName = FilesPathWrite + "dummy.xml";
    try {
        // write time offset
        {
            XmlConfigFileTimeOffset Obj;
            Obj.WriteTimeOffset(FileName, 42);
        }
        // now try to read time offset
        {
            XmlConfigFileTimeOffset Obj;
            int Offset = 0;
            Obj.ReadTimeOffset(FileName, Offset);
            // now test result
            QCOMPARE(Offset, 42);
        }
    } catch(...) {
        QFAIL("You should never get here!");
    }
    // remove dummy file again
    QFile::remove(FileName);
}

} // end namespace DataLogging

QTEST_MAIN(DataManager::TestXmlConfigFileTimeOffset)

#include "TestXmlConfigFileTimeOffset.moc"
