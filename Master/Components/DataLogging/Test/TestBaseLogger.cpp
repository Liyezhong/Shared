/****************************************************************************/
/*! \file TestBaseLogger.cpp
 *
 *  \brief Implementation file for class TestBaseLogger.
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
#include <TestBaseLogger.h>
#include <DataLogging/Include/BaseLogger.h>
#include <DataLogging/Include/DataLoggingEventCodes.h>
#include <DirectoryHelper.h>

namespace DataLogging {

/****************************************************************************/
void TestBaseLogger::initTestCase() {
    // create directory
    m_DirHelper.CreateDir();
}

/****************************************************************************/
void TestBaseLogger::init() {
    // purge data directory
    m_DirHelper.CleanupDir();
}

/****************************************************************************/
void TestBaseLogger::cleanup() {
    // purge data directory
    m_DirHelper.CleanupDir();
}

/****************************************************************************/
void TestBaseLogger::cleanupTestCase() {
    // remove directory
    m_DirHelper.RemoveDir();
}

/****************************************************************************/
void TestBaseLogger::utConstructor() {
    // test default constructor
    QString LogSource1("Logger1");
    QString LogSource2("Logger2");
    BaseLogger TestObject1(NULL, LogSource1, 33);
    BaseLogger TestObject2(NULL, LogSource2, 42);

    TestObject1.CheckEventRepeatingForTempFile();
    TestObject2.CheckEventRepeatingForTempFile();

    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource1);
    QCOMPARE(TestObject1.GetFormatVersion(),       33);
    QVERIFY(TestObject1.m_File.fileName().isEmpty());
    QVERIFY(!TestObject1.IsLogFileOpen());
    QCOMPARE(TestObject1.IsEventRepeated(), true);
    QCOMPARE(TestObject1.IsLogFileError(), false);

    QCOMPARE(TestObject2.GetLoggingSource(),    LogSource2);
    QCOMPARE(TestObject2.GetFormatVersion(),       42);
    QVERIFY(TestObject2.m_File.fileName().isEmpty());
    QVERIFY(!TestObject2.m_File.isOpen());

}

/****************************************************************************/
void TestBaseLogger::utCreateNewFileOK() {
    QString LogSource("Logger");
    BaseLogger TestObject1(NULL, LogSource, 1);
    //*******************************************************
    // - create OK: - a new file is created, no file open yet
    //*******************************************************
    QVERIFY(TestObject1.m_File.fileName().isEmpty());
    QVERIFY(!TestObject1.m_File.isOpen());
    QString FileName = m_DirHelper.AbsoluteFileName("file1.log");
    TestObject1.CreateNewFile(FileName);
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     FileName);
    QVERIFY(TestObject1.m_File.isOpen());
    QVERIFY(QFile::exists(FileName));
    // file size will be checked when file was closed.

    //*************************************************************************
    // - create OK: - a new file is created. a file was open
    //*************************************************************************
    QVERIFY(!TestObject1.m_File.fileName().isEmpty());
    QVERIFY(TestObject1.m_File.isOpen());
    QString FileName2 = m_DirHelper.AbsoluteFileName("file2.log");
    TestObject1.CreateNewFile(FileName2);
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     FileName2);
    QVERIFY(TestObject1.m_File.isOpen());
    QVERIFY(QFile::exists(FileName2));

    // check size of file1.log
    QFile File1(FileName);
    QCOMPARE(File1.size(), qint64(0));
    QVERIFY(File1.open(QIODevice::Append));
    // now append some bytes
    QVERIFY(File1.write("12345\n54321\n") > 0);
    QVERIFY(File1.size() > 0);
    File1.close();

    //*************************************************************************
    // - create OK: - a new file is created overwriting an already existing one, a file was open
    //*************************************************************************
    // we open file1.log again and check what happens with the size
    QVERIFY(!TestObject1.m_File.fileName().isEmpty());
    QVERIFY(TestObject1.m_File.isOpen());
    TestObject1.CreateNewFile(FileName);
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     FileName);
    QVERIFY(TestObject1.m_File.isOpen());
    QVERIFY(QFile::exists(FileName));
    // check size of file1.log
    QCOMPARE(File1.size(), qint64(0));

    // create a readonly file
    QString Exist1FileName("exist1.log");
    QString ReadonlyFileName = m_DirHelper.AbsoluteFileName(Exist1FileName);
    m_DirHelper.CreateReadonlyFile(ReadonlyFileName);
    QVERIFY(QFile::exists(ReadonlyFileName));
    //*********************************************************************************
    // - create OK: - a readonly file with that name already exists. No file open yet.
    // works as root but not as normal user.
    //*********************************************************************************
    TestObject1.CloseFile();
    QVERIFY(TestObject1.m_File.fileName().isEmpty());
    QVERIFY(!TestObject1.m_File.isOpen());
    // now try to create file
    TestObject1.CreateNewFile(ReadonlyFileName);
    // check results
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     ReadonlyFileName);
    QVERIFY(QFile::exists(ReadonlyFileName));
    // check size of file1.log
    QCOMPARE(File1.size(), qint64(0));

    //************************************************************************************
    // - create OK: - a readonly file with that name already exists. A file already open.
    // works as root but not as normal user.
    //************************************************************************************
    TestObject1.CloseFile();
    QVERIFY(TestObject1.m_File.fileName().isEmpty());
    QVERIFY(!TestObject1.m_File.isOpen());
    // create and open "file1.log"
    FileName = m_DirHelper.AbsoluteFileName("file1.log");
    TestObject1.CreateNewFile(FileName);
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     FileName);
    QVERIFY(TestObject1.m_File.isOpen());
    // now try to create and open the readonly file
    TestObject1.CreateNewFile(ReadonlyFileName);
    // check results
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     ReadonlyFileName);
    QVERIFY(QFile::exists(ReadonlyFileName));
}

/****************************************************************************/
void TestBaseLogger::utCreateNewFileNOK() {
    QString LogSource("Logger");
    BaseLogger TestObject1(NULL, LogSource, 1);
    QString NotExistingFileName = m_DirHelper.AbsoluteFileName("NotExistingPath/file1.log");

    QString FileName = m_DirHelper.AbsoluteFileName("file1.log");
    //**************************************************************
    // - create file in an not existing directory. No file open yet.
    //**************************************************************
    QVERIFY(TestObject1.m_File.fileName().isEmpty());
    QVERIFY(!TestObject1.m_File.isOpen());
    // now try to create file in not existing directory
    TestObject1.CreateNewFile(NotExistingFileName);

    //*****************************************************************
    // - create file in an not existing directory. A file already open.
    //*****************************************************************
    QVERIFY(!TestObject1.m_File.fileName().isEmpty());
    QVERIFY(!TestObject1.m_File.isOpen());
    // create and open "file1.log"
    TestObject1.CreateNewFile(FileName);
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     FileName);
    QVERIFY(TestObject1.m_File.isOpen());
    // now try to create file in not existing directory
    TestObject1.CreateNewFile(NotExistingFileName);
}

/****************************************************************************/
void TestBaseLogger::utRemoveFileOK() {
    // create a readonly file
    QString Exist1FileName("exist1.log");
    QString ReadonlyFileName = m_DirHelper.AbsoluteFileName(Exist1FileName);
    m_DirHelper.CreateReadonlyFile(ReadonlyFileName);
    QVERIFY(QFile::exists(ReadonlyFileName));

    //*******************************************
    // - remove OK: - an existing file is removed
    //*******************************************
    QString LogSource("Logger");
    QString FileName = m_DirHelper.AbsoluteFileName("file1.log");
    {
        BaseLogger TestObject1(NULL, LogSource, 1);
        // create new file
        QVERIFY(TestObject1.m_File.fileName().isEmpty());
        QVERIFY(!TestObject1.m_File.isOpen());
        TestObject1.CreateNewFile(FileName);
        QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
        QCOMPARE(TestObject1.m_FormatVersion,       1);
        QCOMPARE(TestObject1.m_File.fileName(),     FileName);
        QVERIFY(TestObject1.m_File.isOpen());
        QVERIFY(QFile::exists(FileName));
    }
    // now remove file
    BaseLogger tmp(NULL, LogSource, 0);
    tmp.RemoveFile(FileName);
    // and check results
    QVERIFY(!QFile::exists(FileName));

    //******************************************************
    // - remove OK: try to remove an existing readonly file
    //******************************************************
    // remove file
    BaseLogger tmp1(NULL, LogSource, 0);
    tmp1.RemoveFile(ReadonlyFileName);
    QVERIFY(!QFile::exists(ReadonlyFileName));

}

/****************************************************************************/
void TestBaseLogger::utRemoveFileNOK() {
    QString NotExistingFileName = m_DirHelper.AbsoluteFileName("NotExistingPath/file1.log");

    //************************************************
    // - remove NOK: try to remove an inexisting file.
    //************************************************
    // remove file
    QString LogSource("Logger");
    BaseLogger tmp(NULL, LogSource, 0);
    tmp.RemoveFile(NotExistingFileName);

}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestBaseLogger)
