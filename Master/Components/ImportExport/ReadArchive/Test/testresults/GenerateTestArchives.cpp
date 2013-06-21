/****************************************************************************/
/** @file GenerateTestArchives.cpp
 *
 *  @brief generate testfiles for ReadArchive
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-16
 *  $Author:    $ R.Wobst
 *
 *  @b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 *  last modified by owner: @(#) Aug 30 2011, 12:20:16
 *
 */
/****************************************************************************/


#include "ImportExport/ReadArchive/Test/testresults/GenerateTestArchives.h"

namespace ImportExport {

static const char* Archname = "Himalaya_arch_deviceID_20110815T20:59:43";

/**
 * @brief empty constructor
 */

GenerateTestArchives::GenerateTestArchives():
    m_workfiles(QList<QByteArray>())
{
    CreateNoFiles();
    CreatePlainFiles();
    CreatePlainBigFiles();
    CreateEncryptFiles();
    CreateEncryptBigFiles();
    CreateEncryptManyFiles();
}

/**
 * @brief init key file
 */

void GenerateTestArchives::init()
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

/**
 * @brief delete files after all tests
 */

void GenerateTestArchives::cleanup()
{
    QFile::remove(Constants::keyfile);
    QFile::remove(Constants::counter);
    foreach(QByteArray name, m_workfiles)
    {
        QFile::remove(QString(name));
    }
    m_workfiles.clear();
}


/**
 * @brief generate workfiles
 *
 * Fill files with bytes with increasing numbers (overflow at 0xff)
 * and add them to the m_workfiles list.
 *
 * @param desc - QList of sizes
 */

void GenerateTestArchives::createWorkfiles(QList<int> sizes)
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


/**
 * @brief write archive with empty filelist and test contents
 */

void GenerateTestArchives::CreateNoFiles()
{
    init();
    WriteArchive(Archname, QList<QByteArray>());
    cleanup();
    QFile::remove("NoFiles");
    QFile::rename(Archname, "NoFiles");
}

/**
 * @brief write unencrypted archive with filelist and test contents
 */

void GenerateTestArchives::CreatePlainFiles()
{
    createWorkfiles(QList<int>() << 1 << 3 << 300);

    init();
    WriteArchive(Archname, m_workfiles);
    cleanup();
    QFile::remove("PlainFiles");
    QFile::rename(Archname, "PlainFiles");
}


/**
 * @brief write unencrypted archive with big files and test contents
 */

void GenerateTestArchives::CreatePlainBigFiles()
{
    createWorkfiles(QList<int>() << 1000 << 750000 << 3000000);

    init();
    WriteArchive(Archname, m_workfiles);
    cleanup();
    QFile::remove("PlainBigFiles");
    QFile::rename(Archname, "PlainBigFiles");
}


/**
 * @brief write encrypted archive with filelist and test contents
 */

void GenerateTestArchives::CreateEncryptFiles()
{
    createWorkfiles(QList<int>() << 1 << 3 << 300);

    init();
    WriteArchive(Archname, m_workfiles, true);
    cleanup();
    QFile::remove("EncryptFiles");
    QFile::rename(Archname, "EncryptFiles");
}


/**
 * @brief write encrypted archive with big files and test contents
 */

void GenerateTestArchives::CreateEncryptBigFiles()
{
    createWorkfiles(QList<int>() << 1000 << 750000 << 3000000);

    init();
    WriteArchive(Archname, m_workfiles, true);
    cleanup();
    QFile::remove("EncryptBigFiles");
    QFile::rename(Archname, "EncryptBigFiles");
}


/**
 * @brief write encrypted archive with many files and test contents
 */

void GenerateTestArchives::CreateEncryptManyFiles()
{
    QList<int> ql;
    for(int n=501; --n;)
    {
        ql << n;
    }

    createWorkfiles(ql);

    init();
    WriteArchive(Archname, m_workfiles, true);
    cleanup();
    QFile::remove("EncryptManyFiles");
    QFile::rename(Archname, "EncryptManyFiles");
}

}               // end namespace ImportExport

int main()
{
    ImportExport::GenerateTestArchives gt;
    return 0;
}
