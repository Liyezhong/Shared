/****************************************************************************/
/*! \file TestXmlConfigFile.cpp
 *
 *  \brief Implementation file for class TestXmlConfigFile.
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
#include <DataManager/Helper/Include/XmlConfigFile.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>

namespace DataManager {

static QString FilesPathErr;    ///< Path to xml file that are OK.
static QString FilesPathOK;     ///< Path to xml file that are not OK.
static QString FilesPathWrite;  ///< Path to where we can write some files.

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFile class.
 */
/****************************************************************************/
class TestXmlConfigFile : public QObject {
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
     * \brief Test method InitStreamReader.
     */
    /****************************************************************************/
    void utTestInitStreamReader();
    /****************************************************************************/
    /**
     * \brief Test method ReadAttributeString.
     */
    /****************************************************************************/
    void utTestReadAttributeString();
    /****************************************************************************/
    /**
     * \brief Test method ReadAttributequint32.
     */
    /****************************************************************************/
    void utTestReadAttributequint32();
    /****************************************************************************/
    /**
     * \brief Test method ReadAttributeLanguage.
     */
    /****************************************************************************/
    void utTestReadAttributeLanguage();
    /****************************************************************************/
    /**
     * \brief Test method ReadAttributeOnOff.
     */
    /****************************************************************************/
    void utTestReadAttributeOnOff();
    /****************************************************************************/
    /**
     * \brief Test method ReadStartElement.
     */
    /****************************************************************************/
    void utTestReadStartElement();
    /****************************************************************************/
    /**
     * \brief Test method ReadFormatVersion.
     */
    /****************************************************************************/
    void utTestReadFormatVersion();
    /****************************************************************************/
    /**
     * \brief Test method InitStreamWriter.
     */
    /****************************************************************************/
    void utTestInitStreamWriter();
    /****************************************************************************/
    /**
     * \brief Test method WriteFormatVersion.
     */
    /****************************************************************************/
    void utTestWriteFormatVersion();
}; // end class TestXmlConfigFile

/****************************************************************************/
void TestXmlConfigFile::initTestCase() {
    // set paths
    QString ExePath = QCoreApplication::applicationDirPath();
    FilesPathErr = ExePath + "/../xmlErrFiles/";
    FilesPathOK  = ExePath + "/../xmlOkFiles/";
    FilesPathWrite = ExePath + "/";
    // init languages
    //Global::InitSupportedLanguages();
}

/****************************************************************************/
void TestXmlConfigFile::init() {
}

/****************************************************************************/
void TestXmlConfigFile::cleanup() {
}

/****************************************************************************/
void TestXmlConfigFile::cleanupTestCase() {
}

/****************************************************************************/
void TestXmlConfigFile::utTestInitStreamReader() {

    // open existing file
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathOK + "ConfigFile1.xml");
    } catch(...) {
        QFAIL("You should never get here!");
    }

    // open not existing file
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathOK + "ConfigFileNotHere.xml");
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), Global::EVENT_GLOBAL_ERROR_FILE_OPEN);
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }
}

/****************************************************************************/
void TestXmlConfigFile::utTestReadAttributeString() {
    // read attribute ok
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathOK + "XmlConfigFileReadStartElementOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        QCOMPARE(Obj.ReadAttributeString(Reader, "attrib1"), QString("value1"));
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement2");
        QCOMPARE(Obj.ReadAttributeString(Reader, "attrib1"), QString("value1"));
        QCOMPARE(Obj.ReadAttributeString(Reader, "attrib2"), QString("value2"));
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement3");
        Reader.skipCurrentElement();
        Reader.skipCurrentElement();
    } catch(...) {
        QFAIL("You should never get here!");
    }

    // attribute not found
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathErr + "XmlConfigFileReadStartElementNOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        QCOMPARE(Obj.ReadAttributeString(Reader, "attrib_notexisting"), QString("value1"));
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND);
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }
}

/****************************************************************************/
void TestXmlConfigFile::utTestReadAttributequint32() {
    // read attribute ok
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathOK + "XmlConfigFileReadStartElementOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement2");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement3");
        QCOMPARE(Obj.ReadAttributequint32(Reader, "numattrib1"), quint32(42));
        QCOMPARE(Obj.ReadAttributequint32(Reader, "numattrib2"), quint32(66));
    } catch(...) {
        QFAIL("You should never get here!");
    }

    // attribute not found
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathErr + "XmlConfigFileReadStartElementNOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement2");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement3");
        QCOMPARE(Obj.ReadAttributequint32(Reader, "numattrib1"), quint32(42));
        QCOMPARE(Obj.ReadAttributequint32(Reader, "numattrib_nothere"), quint32(66));
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND);
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }

    // attribute conversion error
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathErr + "XmlConfigFileReadStartElementNOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement2");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement3");
        QCOMPARE(Obj.ReadAttributequint32(Reader, "numattrib1"), quint32(42));
        QCOMPARE(Obj.ReadAttributequint32(Reader, "numattrib2"), quint32(66));
        QFAIL("You should never get here!");
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), EVENT_DM_ERROR_NO_VALID_NUMBER);
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }
}

/****************************************************************************/
void TestXmlConfigFile::utTestReadAttributeLanguage() {
    // read attribute ok
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathOK + "XmlConfigFileReadStartElementOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement2");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement3");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement4");
        QCOMPARE(Obj.ReadAttributeLanguage(Reader, "langattrib1"), QLocale::German);
        QCOMPARE(Obj.ReadAttributeLanguage(Reader, "langattrib2"), QLocale::English);
    } catch(...) {
        QFAIL("You should never get here!");
    }

    // attribute not found
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathErr + "XmlConfigFileReadStartElementNOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement2");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement3");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement4");
        QCOMPARE(Obj.ReadAttributeLanguage(Reader, "langattrib1"), QLocale::German);
        QCOMPARE(Obj.ReadAttributeLanguage(Reader, "langattrib_nothere"), QLocale::English);
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND);
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }

    // attribute conversion error
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathErr + "XmlConfigFileReadStartElementNOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement2");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement3");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement4");
        QCOMPARE(Obj.ReadAttributeLanguage(Reader, "langattrib1"), QLocale::German);
        QCOMPARE(Obj.ReadAttributeLanguage(Reader, "langattrib2"), QLocale::English);
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), EVENT_DM_ERROR_NOT_SUPPORTED_LANGUAGE);
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }
}

/****************************************************************************/
void TestXmlConfigFile::utTestReadAttributeOnOff() {
    // read attribute ok
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathOK + "XmlConfigFileReadStartElementOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement2");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement3");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement4");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement5");
        QCOMPARE(Obj.ReadAttributeOnOff(Reader, "onoffattrib1"), Global::ONOFFSTATE_ON);
        QCOMPARE(Obj.ReadAttributeOnOff(Reader, "onoffattrib2"), Global::ONOFFSTATE_OFF);
    } catch(...) {
        QFAIL("You should never get here!");
    }

    // attribute not found
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathErr + "XmlConfigFileReadStartElementNOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement2");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement3");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement4");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement5");
        QCOMPARE(Obj.ReadAttributeOnOff(Reader, "onoffattrib1"), Global::ONOFFSTATE_ON);
        QCOMPARE(Obj.ReadAttributeOnOff(Reader, "onoffattrib_nothere"), Global::ONOFFSTATE_OFF);
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND);
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }

    // attribute conversion error
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathErr + "XmlConfigFileReadStartElementNOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement2");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement3");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement4");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement5");
        QCOMPARE(Obj.ReadAttributeOnOff(Reader, "onoffattrib1"), Global::ONOFFSTATE_ON);
        QCOMPARE(Obj.ReadAttributeOnOff(Reader, "onoffattrib2"), Global::ONOFFSTATE_OFF);
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
//        QCOMPARE(E.GetErrorCode(), EVENT_DM_INVALID_ONOFFSTATE);
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }
}

/****************************************************************************/
void TestXmlConfigFile::utTestReadStartElement() {

    // ReadStartElement OK
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathOK + "XmlConfigFileReadStartElementOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        Obj.ReadStartElement(Reader, "subelement1");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "subelement2");
        Reader.skipCurrentElement();
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement2");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement3");
        Reader.skipCurrentElement();
        Reader.skipCurrentElement();
    } catch(...) {
        QFAIL("You should never get here!");
    }

    // ReadStartElement NOK
    // readNextStartElement fails
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathErr + "XmlConfigFileReadStartElementNOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        Obj.ReadStartElement(Reader, "subelement1");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "subelement2");
        Reader.skipCurrentElement();
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "startelement2");
        Obj.ReadStartElement(Reader, "nothere");
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), EVENT_DM_ERROR_READING_XML_STARTELEMENT);
    } catch(...) {
        QFAIL("You should never get here!");
    }

    // wrong name
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathErr + "XmlConfigFileReadStartElementNOK.xml");
        Obj.ReadStartElement(Reader, "rootelement");
        Obj.ReadStartElement(Reader, "startelement1");
        Obj.ReadStartElement(Reader, "subelement1");
        Reader.skipCurrentElement();
        Obj.ReadStartElement(Reader, "subelement_nothere");
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), EVENT_DM_ERROR_UNEXPECTED_XML_STARTELEMENT);
    } catch(...) {
        QFAIL("You should never get here!");
    }
}

/****************************************************************************/
void TestXmlConfigFile::utTestReadFormatVersion() {
    // read format ok
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathOK + "XmlConfigFileReadStartElementOK.xml");
        QCOMPARE(Obj.ReadFormatVersion(Reader, "rootelement"), QString("1"));
    } catch(...) {
        QFAIL("You should never get here!");
    }

    // wrong root element
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathErr + "XmlConfigFileReadStartElementNOK.xml");
        QCOMPARE(Obj.ReadFormatVersion(Reader, "therootelement"), QString("1"));
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), EVENT_DM_ERROR_UNEXPECTED_XML_STARTELEMENT);
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }

    // version not found
    try {
        QXmlStreamReader Reader;
        QFile File;
        XmlConfigFile Obj;
        Obj.InitStreamReader(Reader, File, FilesPathErr + "XmlConfigFileReadStartElementNOK.xml");
        QCOMPARE(Obj.ReadFormatVersion(Reader, "rootelement"), QString("1"));
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        QCOMPARE(E.GetErrorCode(), EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND);
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }
}

/****************************************************************************/
void TestXmlConfigFile::utTestInitStreamWriter() {
    // write OK
    QString FileName = FilesPathWrite + "dummy.xml";
    try {
        QXmlStreamWriter Writer;
        QFile File;
        // create file and write some dummy data in it
        File.setFileName(FileName);
        if(!File.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate)) {
            THROWARG(Global::EVENT_GLOBAL_ERROR_FILE_CREATE, FileName);
        }
        qint64 WrittenSize = File.write("this is a dummy file");
        QVERIFY(WrittenSize > 0);
        File.close();
        QVERIFY(File.size() > 0);
        // now call InitStreamWriter and check what happened to already existing file
        XmlConfigFile Obj;
        Obj.InitStreamWriter(Writer, File, FileName);
        // close file
        File.close();
        // check data
        QCOMPARE(File.size(), qint64(0));
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }
    // remove dummy file again
    QFile::remove(FileName);
}

/****************************************************************************/
void TestXmlConfigFile::utTestWriteFormatVersion() {
    // write OK
    QString FileName = FilesPathWrite + "dummy.xml";
    try {
        QXmlStreamWriter Writer;
        QFile File;
        XmlConfigFile Obj;
        // call InitStreamWriter
        Obj.InitStreamWriter(Writer, File, FileName);
        // write start document
        Writer.writeStartDocument("1.0");
        // call WriteFormatVersion
        Obj.WriteFormatVersion(Writer, "roottag", "42");
        // write end document
        Writer.writeEndDocument();
        // close file
        File.close();
        // check data. take into account newlines.
        QVERIFY((File.size() >= qint64(63)) && (File.size() <= qint64(65)));
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }
    // remove dummy file again
    QFile::remove(FileName);
}

} // end namespace DataLogging

QTEST_MAIN(DataManager::TestXmlConfigFile)

#include "TestXmlConfigFile.moc"
