/****************************************************************************/
/*! \file TestBaseLoggerReusable.cpp
 *
 *  \brief Implementation file for class TestBaseLoggerReusable.
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

#include <DataLogging/Include/BaseLoggerReusable.h>
#include <DataLogging/Include/DataLoggingEventCodes.h>
#include <DirectoryHelper.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Test class for BaseLogger class.
 */
/****************************************************************************/
class TestBaseLoggerReusable : public QObject {
    Q_OBJECT
private:
    DirectoryHelper     m_DirHelper;    ///< Helper for cleaning up a directory.
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
     *
     * We cleanup the data directory we use.
     */
    /****************************************************************************/
    void init();
    /****************************************************************************/
    /**
     * \brief Called after each testfunction was executed.
     *
     * We cleanup the data directory we use.
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
     * \brief Test constructor.
     */
    /****************************************************************************/
    void utConstructor();
    /****************************************************************************/
    /**
     * \brief Test configuration and get methods.
     */
    /****************************************************************************/
    void utConfigure();
    /****************************************************************************/
    /**
     * \brief Test writing of header.
     */
    /****************************************************************************/
    void utWriteHeader();
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    TestBaseLoggerReusable() :
        m_DirHelper("Data/Logger")
    {
    }
}; // end class TestBaseLoggerReusable

/****************************************************************************/
void TestBaseLoggerReusable::initTestCase() {
    // create directory
    m_DirHelper.CreateDir();
}

/****************************************************************************/
void TestBaseLoggerReusable::init() {
    // purge data directory
    m_DirHelper.CleanupDir();
}

/****************************************************************************/
void TestBaseLoggerReusable::cleanup() {
    // purge data directory
    m_DirHelper.CleanupDir();
}

/****************************************************************************/
void TestBaseLoggerReusable::cleanupTestCase() {
    // remove directory
    m_DirHelper.RemoveDir();
}

/****************************************************************************/
void TestBaseLoggerReusable::utConstructor() {
    // test constructor
    Global::LoggingSource LS1(1,2);
    Global::LoggingSource LS2(3,4);
    BaseLoggerReusable TestObject1(NULL, LS1, 33);
    BaseLoggerReusable TestObject2(NULL, LS2, 42);

    QCOMPARE(TestObject1.GetLoggingSource(),    LS1);
    QCOMPARE(TestObject1.GetFormatVersion(),    33);
    QCOMPARE(TestObject1.m_OperatingMode,       QString(""));
    QCOMPARE(TestObject1.m_SerialNumber,        QString(""));
    QCOMPARE(TestObject1.m_Path,                QString(""));

    QCOMPARE(TestObject2.GetLoggingSource(),    LS2);
    QCOMPARE(TestObject2.GetFormatVersion(),    42);
    QCOMPARE(TestObject2.m_OperatingMode,       QString(""));
    QCOMPARE(TestObject2.m_SerialNumber,        QString(""));
    QCOMPARE(TestObject2.m_Path,                QString(""));
}

/****************************************************************************/
void TestBaseLoggerReusable::utConfigure() {

    Global::LoggingSource LS1(1,2);
    Global::LoggingSource LS2(3,4);
    BaseLoggerReusable TestObject1(NULL, LS1, 33);
    BaseLoggerReusable TestObject2(NULL, LS2, 42);
    // configure
    TestObject1.SetConfiguration("production",  "sernum1", "path1");
    TestObject2.SetConfiguration("service",     "sernum2", "path2");

    // and test
    QCOMPARE(TestObject1.GetLoggingSource(),    LS1);
    QCOMPARE(TestObject1.GetFormatVersion(),    33);
    QCOMPARE(TestObject1.m_OperatingMode,       QString("production"));
    QCOMPARE(TestObject1.m_SerialNumber,        QString("sernum1"));
    QCOMPARE(TestObject1.m_Path,                QString("path1"));
    QCOMPARE(TestObject1.GetOperatingMode(),    QString("production"));
    QCOMPARE(TestObject1.GetSerialNumber(),     QString("sernum1"));
    QCOMPARE(TestObject1.GetPath(),             QString("path1"));

    QCOMPARE(TestObject2.GetLoggingSource(),    LS2);
    QCOMPARE(TestObject2.GetFormatVersion(),    42);
    QCOMPARE(TestObject2.m_OperatingMode,       QString("service"));
    QCOMPARE(TestObject2.m_SerialNumber,        QString("sernum2"));
    QCOMPARE(TestObject2.m_Path,                QString("path2"));
    QCOMPARE(TestObject2.GetOperatingMode(),    QString("service"));
    QCOMPARE(TestObject2.GetSerialNumber(),     QString("sernum2"));
    QCOMPARE(TestObject2.GetPath(),             QString("path2"));
}

/****************************************************************************/
void TestBaseLoggerReusable::utWriteHeader() {

    Global::LoggingSource LS1(1,2);
    BaseLoggerReusable TestObject1(NULL, LS1, 33);
    TestObject1.SetConfiguration("assembly", "sernum1", m_DirHelper.GetDir());
    QString FileName = m_DirHelper.AbsoluteFileName("file1.log");

    try {
        // open file
        TestObject1.OpenFileForAppend(FileName);
        QString TS1 = TestObject1.GetTimeStampHeader();
        // write header
        TestObject1.WriteHeader();
        TestObject1.CloseFile();
        QString TS2 = TestObject1.GetTimeStampHeader();
        // test timestamps.
        QVERIFY2(TS1 <= TS2, "If this failed, the day changed between computing TS1 and TS2. Please repeat the test");

        // now check if header written is OK
        QFile File(FileName);
        QVERIFY(File.open(QIODevice::ReadOnly | QIODevice::Text));
        // read data
        QByteArray Data = File.readAll().trimmed();
        File.close();
        // check data
        QVERIFY(Data.size() > 0);
        QList<QByteArray> Tokens = Data.split(';');
        QCOMPARE(Tokens.size(), 5);
        // check file name
        QCOMPARE(Tokens[0], QByteArray("file1"));
        // check timestamp
        QString TS3(Tokens[1]);
        QVERIFY(TS1 <= TS3);
        QVERIFY(TS3 <= TS2);
        // check operating mode
        QCOMPARE(Tokens[2], QByteArray("assembly"));
        // check format version
        QCOMPARE(Tokens[3], QByteArray("33"));
        // check serial number
        QCOMPARE(Tokens[4], QByteArray("sernum1"));

    } catch(const Global::Exception & E) {
        QString ErrorString =   QString("Global::Exception caught!") + QString::number(E.GetErrorCode()) +
                                " " + E.GetFile() + "(" + QString::number(E.GetLine()) + ")";
        QFAIL(ErrorString.toUtf8().constData());
    } catch(...) {
        QFAIL("Exception caught!");
    }
}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestBaseLoggerReusable)

#include "TestBaseLoggerReusable.moc"
