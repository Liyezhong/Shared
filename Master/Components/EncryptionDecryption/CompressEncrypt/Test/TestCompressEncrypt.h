/****************************************************************************/
/*! \file TestCompressEncrypt.h
 *
 *  \brief test CompressEncrypt class for EncryptionDecryption module
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

#ifndef ENCRYPTION_DECRYPTION_TESTCOMPRESSENCRYPT_H
#define ENCRYPTION_DECRYPTION_TESTCOMPRESSENCRYPT_H



#include <QtTest>
#include <QObject>
#include <QTime>
#include "EncryptionDecryption/CompressEncrypt/Include/CompressEncrypt.h"

namespace EncryptionDecryption {

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

}       // end namespace EncryptionDecryption

#endif          // ENCRYPTION_DECRYPTION_TESTCOMPRESSENCRYPT_H
