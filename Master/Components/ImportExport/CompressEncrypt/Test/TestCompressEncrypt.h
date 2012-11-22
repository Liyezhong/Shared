/****************************************************************************/
/** @file TestCompressEncrypt.h
 *
 *  @brief test CompressEncrypt class for ImportExport module
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-07-30
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
 *  last modified by owner: @(#) Aug 24 2011, 12:35:11
 *
 */
/****************************************************************************/

#ifndef IMPORT_EXPORT_TESTCOMPRESSENCRYPT_H
#define IMPORT_EXPORT_TESTCOMPRESSENCRYPT_H



#include <QtTest>
#include <QObject>
#include <QTime>
#include "ImportExport/CompressEncrypt/Include/CompressEncrypt.h"

namespace ImportExport {

class TestCompressEncrypt: public QObject
{
    Q_OBJECT

    public:
        TestCompressEncrypt();

    private slots:
        void init();
        void cleanupTestCase();
        void utTestPlainShort();
        void utTestEncrShort();
        void utTestPlainMaxsize();
        void utTestEncrMaxsize();
        void utTestPlainLong();
        void utTestEncrLong();
        void utTestBenchmark();
};

}       // end namespace ImportExport

#endif          // IMPORT_EXPORT_TESTCOMPRESSENCRYPT_H
