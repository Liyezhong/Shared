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
    Global::LoggingSource LS1(1,2);
    Global::LoggingSource LS2(1,2);
    BaseLogger TestObject1(NULL, LS1, 33);
    BaseLogger TestObject2(NULL, LS2, 42);

    QCOMPARE(TestObject1.GetLoggingSource(),    LS1);
    QCOMPARE(TestObject1.m_FormatVersion,       33);
    QVERIFY(TestObject1.m_File.fileName().isEmpty());
    QVERIFY(!TestObject1.m_File.isOpen());

    QCOMPARE(TestObject2.GetLoggingSource(),    LS2);
    QCOMPARE(TestObject2.m_FormatVersion,       42);
    QVERIFY(TestObject2.m_File.fileName().isEmpty());
    QVERIFY(!TestObject2.m_File.isOpen());

}

/****************************************************************************/
void TestBaseLogger::utCreateNewFileOK() {
    Global::LoggingSource LS(1,2);
    try {
        BaseLogger TestObject1(NULL, LS, 1);
        //*******************************************************
        // - create OK: - a new file is created, no file open yet
        //*******************************************************
        QVERIFY(TestObject1.m_File.fileName().isEmpty());
        QVERIFY(!TestObject1.m_File.isOpen());
        QString FileName = m_DirHelper.AbsoluteFileName("file1.log");
        TestObject1.CreateNewFile(FileName);
        QCOMPARE(TestObject1.GetLoggingSource(),    LS);
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
        QCOMPARE(TestObject1.GetLoggingSource(),    LS);
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
        QCOMPARE(TestObject1.GetLoggingSource(),    LS);
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
        try {
            TestObject1.CloseFile();
            QVERIFY(TestObject1.m_File.fileName().isEmpty());
            QVERIFY(!TestObject1.m_File.isOpen());
            // now try to create file
            TestObject1.CreateNewFile(ReadonlyFileName);
            // check results
            QCOMPARE(TestObject1.GetLoggingSource(),    LS);
            QCOMPARE(TestObject1.m_FormatVersion,       1);
            QCOMPARE(TestObject1.m_File.fileName(),     ReadonlyFileName);
            QVERIFY(TestObject1.m_File.isOpen());
            QVERIFY(QFile::exists(ReadonlyFileName));
            // check size of file1.log
            QCOMPARE(File1.size(), qint64(0));
        } catch(const Global::Exception & /*E*/) {
            QFAIL("You should never get here! This test only works as root!");
        } catch(...) {
            QFAIL("Unknown exception caught!");
        }

        //************************************************************************************
        // - create OK: - a readonly file with that name already exists. A file already open.
        // works as root but not as normal user.
        //************************************************************************************
        try {
            TestObject1.CloseFile();
            QVERIFY(TestObject1.m_File.fileName().isEmpty());
            QVERIFY(!TestObject1.m_File.isOpen());
            // create and open "file1.log"
            QString FileName = m_DirHelper.AbsoluteFileName("file1.log");
            TestObject1.CreateNewFile(FileName);
            QCOMPARE(TestObject1.GetLoggingSource(),    LS);
            QCOMPARE(TestObject1.m_FormatVersion,       1);
            QCOMPARE(TestObject1.m_File.fileName(),     FileName);
            QVERIFY(TestObject1.m_File.isOpen());
            // now try to create and open the readonly file
            TestObject1.CreateNewFile(ReadonlyFileName);
            // check results
            QCOMPARE(TestObject1.GetLoggingSource(),    LS);
            QCOMPARE(TestObject1.m_FormatVersion,       1);
            QCOMPARE(TestObject1.m_File.fileName(),     ReadonlyFileName);
            QVERIFY(TestObject1.m_File.isOpen());
            QVERIFY(QFile::exists(ReadonlyFileName));
        } catch(const Global::Exception & /*E*/) {
            QFAIL("You should never get here! This test only works as root!");
        } catch(...) {
            QFAIL("Unknown exception caught!");
        }
    } catch(const Global::Exception & E) {
        QString ErrorString =   QString("Global::Exception caught!") + QString::number(E.GetErrorCode()) +
                                " " + E.GetFile() + "(" + QString::number(E.GetLine()) + ")";
        QFAIL(ErrorString.toUtf8().constData());
    } catch(...) {
        QFAIL("Exception caught!");
    }
};

/****************************************************************************/
void TestBaseLogger::utCreateNewFileNOK() {
    Global::LoggingSource LS(1,2);
    BaseLogger TestObject1(NULL, LS, 1);
    QString NotExistingFileName = m_DirHelper.AbsoluteFileName("NotExistingPath/file1.log");

    QString FileName = m_DirHelper.AbsoluteFileName("file1.log");
    //**************************************************************
    // - create file in an not existing directory. No file open yet.
    //**************************************************************
    try {
        QVERIFY(TestObject1.m_File.fileName().isEmpty());
        QVERIFY(!TestObject1.m_File.isOpen());
        // now try to create file in not existing directory
        TestObject1.CreateNewFile(NotExistingFileName);
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        // check results
        QCOMPARE(E.GetErrorCode(),                  EVENT_GLOBAL_ERROR_FILE_CREATE);
        QCOMPARE(TestObject1.GetLoggingSource(),    LS);
        QCOMPARE(TestObject1.m_FormatVersion,       1);
        QVERIFY(TestObject1.m_File.fileName().isEmpty());
        QVERIFY(!TestObject1.m_File.isOpen());
        QVERIFY(!QFile::exists(NotExistingFileName));
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }

    //*****************************************************************
    // - create file in an not existing directory. A file already open.
    //*****************************************************************
    try {
        QVERIFY(TestObject1.m_File.fileName().isEmpty());
        QVERIFY(!TestObject1.m_File.isOpen());
        // create and open "file1.log"
        TestObject1.CreateNewFile(FileName);
        QCOMPARE(TestObject1.GetLoggingSource(),    LS);
        QCOMPARE(TestObject1.m_FormatVersion,       1);
        QCOMPARE(TestObject1.m_File.fileName(),     FileName);
        QVERIFY(TestObject1.m_File.isOpen());
        // now try to create file in not existing directory
        TestObject1.CreateNewFile(NotExistingFileName);
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        // check results
        QCOMPARE(E.GetErrorCode(),                  EVENT_GLOBAL_ERROR_FILE_CREATE);
        QCOMPARE(TestObject1.GetLoggingSource(),    LS);
        QCOMPARE(TestObject1.m_FormatVersion,       1);
        QVERIFY(TestObject1.m_File.fileName().isEmpty());
        QVERIFY(!TestObject1.m_File.isOpen());
        QVERIFY(QFile::exists(FileName));
        QVERIFY(!QFile::exists(NotExistingFileName));
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }
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
    Global::LoggingSource LS(1,2);
    try {
        QString FileName = m_DirHelper.AbsoluteFileName("file1.log");
        {
            BaseLogger TestObject1(NULL, LS, 1);
            // create new file
            QVERIFY(TestObject1.m_File.fileName().isEmpty());
            QVERIFY(!TestObject1.m_File.isOpen());
            TestObject1.CreateNewFile(FileName);
            QCOMPARE(TestObject1.GetLoggingSource(),    LS);
            QCOMPARE(TestObject1.m_FormatVersion,       1);
            QCOMPARE(TestObject1.m_File.fileName(),     FileName);
            QVERIFY(TestObject1.m_File.isOpen());
            QVERIFY(QFile::exists(FileName));
        }
        // now remove file
        BaseLogger tmp(NULL, LS, 0);
        tmp.RemoveFile(FileName);
        // and check results
        QVERIFY(!QFile::exists(FileName));
    } catch(const Global::Exception & E) {
        QString ErrorString =   QString("Global::Exception caught!") + QString::number(E.GetErrorCode()) +
                                " " + E.GetFile() + "(" + QString::number(E.GetLine()) + ")";
        QFAIL(ErrorString.toUtf8().constData());
    } catch(...) {
        QFAIL("Exception caught!");
    }

    //******************************************************
    // - remove OK: try to remove an existing readonly file
    //******************************************************
    try {
        // remove file
        BaseLogger tmp(NULL, LS, 0);
        tmp.RemoveFile(ReadonlyFileName);
        QVERIFY(!QFile::exists(ReadonlyFileName));
    } catch(const Global::Exception & E) {
        QString ErrorString =   QString("Global::Exception caught!") + QString::number(E.GetErrorCode()) +
                                " " + E.GetFile() + "(" + QString::number(E.GetLine()) + ")";
        QFAIL(ErrorString.toUtf8().constData());
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }

}

/****************************************************************************/
void TestBaseLogger::utRemoveFileNOK() {
    QString NotExistingFileName = m_DirHelper.AbsoluteFileName("NotExistingPath/file1.log");

    //************************************************
    // - remove NOK: try to remove an inexisting file.
    //************************************************
    try {
        // remove file
        Global::LoggingSource LS(1,2);
        BaseLogger tmp(NULL, LS, 0);
        tmp.RemoveFile(NotExistingFileName);
        QFAIL("You should never get here!");
    } catch(const Global::Exception & E) {
        // check results
        QCOMPARE(E.GetErrorCode(), EVENT_GLOBAL_ERROR_FILE_REMOVE);
    } catch(...) {
        QFAIL("Unknown exception caught!");
    }

}

} // end namespace DataLogging

QTEST_MAIN(DataLogging::TestBaseLogger)
