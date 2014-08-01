/****************************************************************************/
/*! \file TestWriteArchive.h
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

#ifndef ENCRYPTION_DECRYPTION_TEST_WRITE_ARCHIVE_H
#define ENCRYPTION_DECRYPTION_TEST_WRITE_ARCHIVE_H

#include <QtTest>
#include <QObject>
#include <QString>
#include <QList>
#include <QTime>
#include "EncryptionDecryption/WriteArchive/Include/WriteArchive.h"

namespace EncryptionDecryption {

class TestWriteArchive: public QObject
{
    Q_OBJECT

    public:
        TestWriteArchive();           // default constructor

    private:
        QList<QByteArray> m_workfiles;
        QString DirPath;
        QByteArray getFileContents(const char* filename);
        void createWorkfiles(QList<int> sizes);
        void createFileStringList(QStringList &filelist);
        QString getZipFileName();

    private slots:
        void init();
        void cleanup();
        void utTestNoFiles();
        void utTestPlainFiles();
        void utTestPlainBigFiles();
        void utTestEncryptFiles();
        void utTestEncryptBigFiles();
        void utTestEncryptManyFiles();
};

}       // end namespace EncryptionDecryption

#endif          //ENCRYPTION_DECRYPTION_TEST_WRITE_ARCHIVE_H
