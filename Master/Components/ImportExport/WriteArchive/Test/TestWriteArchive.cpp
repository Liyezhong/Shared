/****************************************************************************/
/*! \file TestWriteArchive.cpp
 *
 *  \brief Testclass for WriteArchive function
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2012-11-26
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
 *
 */
/****************************************************************************/


#include "ImportExport/WriteArchive/Test/TestWriteArchive.h"

namespace ImportExport {

static const char* Archname = "Colorado_User_deviceID_20110815_205943.lpkg";
static const QString ArchZipName = "Colorado_User_deviceID_%1.zip";
/****************************************************************************/
/*!
 * \brief empty constructor
 */
/****************************************************************************/
TestWriteArchive::TestWriteArchive():
    m_workfiles(QList<QByteArray>())
{
    ;
}
/****************************************************************************/
/*!
 * \brief init files for test
 */
/****************************************************************************/
void TestWriteArchive::init()
{
    // we use different keys for Leica and Viewer (Viewer with value 1)
    QByteArray keybytes(Constants::KEYFILESIZE, 0);
    keybytes[2*Constants::HASH_SIZE-1] = 1;

    FailSafeOpen keyfile(Constants::keyfile, 'w');
    keyfile.write(keybytes);
    keyfile.close();

    FailSafeOpen ctrfile(Constants::counter, 'w');
    ctrfile.write(QByteArray(4, 0));
    ctrfile.close();
}
/****************************************************************************/
/*!
 * \brief delete files after all tests
 */
/****************************************************************************/
void TestWriteArchive::cleanup()
{
    QFile::remove(Constants::keyfile);
    QFile::remove(Constants::counter);
    foreach(QByteArray name, m_workfiles)
    {
        QFile::remove(QString(name));
    }
    m_workfiles.clear();

    // comment this out for analyzing new testcases
    QFile::remove(Archname);
}

/****************************************************************************/
/*!
 * \brief comfort function: return file contents as QByteArray
 *
 * \iparam filename - name of file
 * \return - contents of file as QByteArray
 */
/****************************************************************************/
QByteArray TestWriteArchive::getFileContents(const char *filename)
{
    FailSafeOpen fd(filename, 'r');
    QByteArray qba = fd.read();
    fd.close();
    return qba;
}

/****************************************************************************/
/*!
 * \brief generate workfiles
 *
 * Fill files with bytes with increasing numbers (overflow at 0xff)
 * and add them to the m_workfiles list.
 *
 * \iparam desc - QList of sizes
 */
/****************************************************************************/
void TestWriteArchive::createWorkfiles(QList<int> sizes)
{
    int cnt = 1;
    int no = 0;

    foreach(int sz, sizes)
    {
        ++no;

        QString name = QString("_%1_").arg(no);
        FailSafeOpen fd(name, 'w');

        for(int n=0; n < sz; ++n)
        {
            ++cnt;
            fd.write(QByteArray(1, cnt & 0xff));
        }
        fd.close();

        m_workfiles << qPrintable(name);
    }
}


/****************************************************************************/
/*!
 * \brief create string list for the bytearray list
 *
 * \iparam QStringList - QList of sizes
 */
/****************************************************************************/
void TestWriteArchive::createFileStringList(QStringList &filelist)
{
    // m_workfiles is a byte array, so convert the byte array
    // and make it as a string list
    foreach (QByteArray Value, m_workfiles) {

        filelist << QString(Value);
    }
}

/****************************************************************************/
/*!
 * \brief get the file name of the archive
 *
 * \return QString - file name of the archive
 */
/****************************************************************************/
QString TestWriteArchive::getZipFileName()
{
    // crete the ISODateTime format, replace the data T with underscore
    QString ISODateTime = QDateTime::currentDateTime().toString(Qt::ISODate).replace("T","_");
    // replace the colon (:) because the colon is not supported by windows file system.
    ISODateTime = ISODateTime.replace(":", "").replace("-", "");
    // return the file name
    return ArchZipName.arg(ISODateTime);
}
/****************************************************************************/
/*!
 * \brief write archive with empty filelist and test contents
 */
/****************************************************************************/
void TestWriteArchive::utTestNoFiles()
{
    WriteArchive(Archname, QList<QByteArray>(), false);
    QCOMPARE(getFileContents(Archname), getFileContents("testresults/NoFiles"));
}
/****************************************************************************/
/*!
 * \brief write unencrypted archive with filelist and test contents
 */
/****************************************************************************/
void TestWriteArchive::utTestPlainFiles()
{
    createWorkfiles(QList<int>() << 1 << 3 << 300);

    WriteArchive(Archname, m_workfiles, false);
    QCOMPARE(getFileContents(Archname),
             getFileContents("testresults/PlainFiles"));
}

/****************************************************************************/
/*!
 * \brief write unencrypted archive with big files and test contents
 */
/****************************************************************************/
void TestWriteArchive::utTestPlainBigFiles()
{
    createWorkfiles(QList<int>() << 1000 << 750000 << 3000000);

    WriteArchive(Archname, m_workfiles, false);
    QCOMPARE(getFileContents(Archname),
             getFileContents("testresults/PlainBigFiles"));
}

/****************************************************************************/
/*!
 * \brief write encrypted archive with filelist and test contents
 */
/****************************************************************************/
void TestWriteArchive::utTestEncryptFiles()
{
    createWorkfiles(QList<int>() << 1 << 3 << 300);

    WriteArchive(Archname, m_workfiles, true);
    QCOMPARE(getFileContents(Archname),
             getFileContents("testresults/EncryptFiles"));
}

/****************************************************************************/
/*!
 * \brief write encrypted archive with big files and test contents
 */
/****************************************************************************/
void TestWriteArchive::utTestEncryptBigFiles()
{
    createWorkfiles(QList<int>() << 1000 << 750000 << 3000000);

    WriteArchive(Archname, m_workfiles, true);
    QCOMPARE(getFileContents(Archname),
             getFileContents("testresults/EncryptBigFiles"));
}

/****************************************************************************/
/*!
 * \brief write encrypted archive with many files and test contents
 */
/****************************************************************************/
void TestWriteArchive::utTestEncryptManyFiles()
{
    QList<int> ql;
    for(int n=501; --n;)
    {
        ql << n;
    }

    createWorkfiles(ql);

    WriteArchive(Archname, m_workfiles, true);
    QCOMPARE(getFileContents(Archname),
             getFileContents("testresults/EncryptManyFiles"));
}

}               // end namespace ImportExport

QTEST_MAIN(ImportExport::TestWriteArchive)
