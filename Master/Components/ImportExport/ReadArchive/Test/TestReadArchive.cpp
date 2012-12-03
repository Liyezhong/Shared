/****************************************************************************/
/*! \file TestReadArchive.cpp
 *
 *  \brief Testclass for ReadArchive function
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


#include "ImportExport/ReadArchive/Test/TestReadArchive.h"

namespace ImportExport {

static const char* Archname = "Colorado_arch_deviceID_20110815T20:59:43";
/****************************************************************************/
/*!
 * \brief empty constructor
 */
/****************************************************************************/
TestReadArchive::TestReadArchive()
{
    ;
}
/****************************************************************************/
/*!
 * \brief init files for test
 */
/****************************************************************************/
void TestReadArchive::init()
{
    // we use different keys for Leica and Viewer (Viewer with value 1)
    QByteArray keybytes(Constants::KEYFILESIZE, 0);
    keybytes[2*Constants::HASH_SIZE-1] = 1;
    m_keydata = keybytes;
}

/****************************************************************************/
/*!
 * \brief remove workfile
 */
/****************************************************************************/
void TestReadArchive::cleanUp()
{
    QFile::remove(Archname);
}

/****************************************************************************/
/*!
 * \brief archive with empty filelist and test contents
 */
/****************************************************************************/
void TestReadArchive::utTestNoFiles()
{
    RAMFile fp;

    QFile::remove(Archname);
    QFile::copy("testresults/NoFiles", Archname);

    try
    {
        ReadArchive(Archname, &fp, "Import", m_keydata);
    }
    catch(...)
    {
        //QFAIL(e.getMsg().toAscii().data());
    }
    QVERIFY(fp.getFiles().size() == 0);
}
/****************************************************************************/
/*!
 * \brief write unencrypted archive with filelist and test contents
 */
/****************************************************************************/
void TestReadArchive::utTestPlainFiles()
{
    RAMFile fp;

    QFile::remove(Archname);
    QFile::copy("testresults/PlainFiles", Archname);

    try
    {
        ReadArchive(Archname, &fp, "Import", m_keydata);
    }
    catch(...)
    {
        //QFAIL(e.getMsg().toAscii().data());
    }

    // test contents

    QHash<QString, QByteArray> res = fp.getFiles();

    foreach(QString key, res.keys())
    {
        qDebug("file %s: %d bytes", key.toAscii().data(), res[key].size());
    }
}

/****************************************************************************/
/*!
 * \brief write unencrypted archive with big files and test contents
 */
/****************************************************************************/
void TestReadArchive::utTestPlainBigFiles()
{
    RAMFile fp;

    QFile::remove(Archname);
    QFile::copy("testresults/PlainBigFiles", Archname);

    try
    {
        ReadArchive(Archname, &fp, "Import", m_keydata);
    }
    catch(...)
    {
        //QFAIL(e.getMsg().toAscii().data());
    }
    // test contents

    QHash<QString, QByteArray> res = fp.getFiles();

    foreach(QString key, res.keys())
    {
        qDebug("file %s: %d bytes", key.toAscii().data(), res[key].size());
    }
}

/****************************************************************************/
/*!
 * \brief write encrypted archive with filelist and test contents
 */
/****************************************************************************/
void TestReadArchive::utTestEncryptFiles()
{
    RAMFile fp;

    QFile::remove(Archname);
    QFile::copy("testresults/EncryptFiles", Archname);

    try
    {
        ReadArchive(Archname, &fp, "Import", m_keydata);
    }
    catch(...)
    {
        //QFAIL(e.getMsg().toAscii().data());
    }
    // test contents

    QHash<QString, QByteArray> res = fp.getFiles();

    foreach(QString key, res.keys())
    {
        qDebug("file %s: %d bytes", key.toAscii().data(), res[key].size());
    }
}

/****************************************************************************/
/*!
 * \brief write encrypted archive with big files and test contents
 */
/****************************************************************************/
void TestReadArchive::utTestEncryptBigFiles()
{
    RAMFile fp;

    QFile::remove(Archname);
    QFile::copy("testresults/EncryptBigFiles", Archname);

    try
    {
        ReadArchive(Archname, &fp, "Import", m_keydata);
    }
    catch(...)
    {
        //QFAIL(e.getMsg().toAscii().data());
    }

    // test contents

    QHash<QString, QByteArray> res = fp.getFiles();

    foreach(QString key, res.keys())
    {
        qDebug("file %s: %d bytes", key.toAscii().data(), res[key].size());
    }
}

/****************************************************************************/
/*!
 * \brief write encrypted archive with many files and test contents
 */
/****************************************************************************/
void TestReadArchive::utTestEncryptManyFiles()
{
    VoidFile fp;

    QFile::remove(Archname);
    QFile::copy("testresults/EncryptManyFiles", Archname);

    try
    {
        ReadArchive(Archname, &fp, "Import", m_keydata);
    }
    catch(...)
    {
        //QFAIL(e.getMsg().toAscii().data());
    }

    // test contents

    QSet<QString> res = fp.getFilenames();

    QCOMPARE(res.size(), 500);
}

}               // end namespace ImportExport

QTEST_MAIN(ImportExport::TestReadArchive)
