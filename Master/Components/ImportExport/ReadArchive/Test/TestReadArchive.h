/****************************************************************************/
/*! \file TestReadArchive.h
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

#ifndef IMPORT_EXPORT_TEST_READ_ARCHIVE_H
#define IMPORT_EXPORT_TEST_READ_ARCHIVE_H

#include <QtTest>
#include <QObject>
#include <QString>
#include <QList>
#include "ImportExport/ReadArchive/Include/ReadArchive.h"

namespace ImportExport {

class TestReadArchive: public QObject
{
    Q_OBJECT

    public:
        TestReadArchive();           // default constructor

    private:
        QString DirPath;
        QByteArray m_keydata;

    private slots:
        void init();
        void cleanUp();
        void utTestNoFiles();
        void utTestPlainFiles();
        void utTestPlainBigFiles();
        void utTestEncryptFiles();
        void utTestEncryptBigFiles();
        void utTestEncryptManyFiles();
};

}       // end namespace ImportExport

#endif          //IMPORT_EXPORT_TEST_READ_ARCHIVE_H
