/****************************************************************************/
/*! \file TestAbstractFile.cpp
 *
 *  \brief Testclass for AbstractFile
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


#include <QtAlgorithms>
#include "EncryptionDecryption/AbstractFile/Test/TestAbstractFile.h"

namespace EncryptionDecryption {

/****************************************************************************/
/*!
 * \brief default constructor
 */
/****************************************************************************/
TestAbstractFile::TestAbstractFile():
        m_workfiles(QSet<QString>() << "_1_" << "_2_" << "_3_")
{
    ;
}
/****************************************************************************/
/*!
 * \brief init files for test
 */
/****************************************************************************/
void TestAbstractFile::init()
{
}
/****************************************************************************/
/*!
 * \brief delete files after all tests
 */
/****************************************************************************/
void TestAbstractFile::cleanup()
{
    foreach(QString name, m_workfiles)
    {
        QFile::remove(name);    // return value is desperately not checked
    }
}

/****************************************************************************/
/*!
 * \brief write three files with their names as contents
 */
/****************************************************************************/
void TestAbstractFile::writeFiles(AbstractFile* fp)
{
    foreach(QString name, m_workfiles)
    {
        fp->open(name);
        fp->write(name.toAscii());
        fp->close();
    }
}

/****************************************************************************/
/*!
 * \brief comfort function: return file contents as QByteArray
 *
 * \iparam filename - name of file
 * \return - contents of file as QByteArray
 */
/****************************************************************************/
QByteArray TestAbstractFile::getFileContents(const QString filename)
{
    FailSafeOpen fd(filename, 'r');
    QByteArray qba = fd.read();
    fd.close();
    return qba;
}

/****************************************************************************/
/*!
 * \brief write files to plain file, check
 */
/****************************************************************************/
void TestAbstractFile::utTestPlainFiles()
{
    PlainFile fd;
    writeFiles(&fd);

    foreach(QString name, m_workfiles)
    {
        QCOMPARE(getFileContents(name), name.toAscii());
    }
}

/****************************************************************************/
/*!
 * \brief write files to RAM file (hashtable), check
 */
/****************************************************************************/
void TestAbstractFile::utTestRAMFiles()
{
    RAMFile fd;
    writeFiles(&fd);

    QHash<QString, QByteArray> result = fd.getFiles();

    foreach(QString name, result)
    {
        QCOMPARE(result[name], name.toAscii());
    }

    // qSort needs a reference to a container, so explicit assignments
    // are necessary

    QList<QString> keys = result.keys(); 
    qSort(keys);

    QList<QString> worknames = m_workfiles.toList();
    qSort(worknames);

    QCOMPARE(keys, worknames);
}

/****************************************************************************/
/*!
 * \brief write files to Void file (dummy), check namelist
 */
/****************************************************************************/
void TestAbstractFile::utTestVoidFiles()
{
    VoidFile fd;
    writeFiles(&fd);

    // qSort needs a reference to a container, so explicit assignments
    // are necessary

    QList<QString> names = fd.getFilenames().toList();
    qSort(names);

    QList<QString> worknames = m_workfiles.toList();
    qSort(worknames);

    QCOMPARE(names, worknames);
}


}               // end namespace EncryptionDecryption

QTEST_MAIN(EncryptionDecryption::TestAbstractFile)
