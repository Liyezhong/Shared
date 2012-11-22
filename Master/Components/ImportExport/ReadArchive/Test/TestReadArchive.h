/****************************************************************************/
/** @file TestReadArchive.h
 *
 *  @brief Testclass for ReadArchive function
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-15
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
 *  last modified by owner: @(#) Aug 24 2011, 12:42:24
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
