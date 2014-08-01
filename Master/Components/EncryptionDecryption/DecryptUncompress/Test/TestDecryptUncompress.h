/****************************************************************************/
/*! \file TestDecryptUncompress.h
 *
 *  \brief test DecryptUncompress class for EncryptionDecryption module
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
#include "EncryptionDecryption/DecryptUncompress/Include/DecryptUncompress.h"

namespace EncryptionDecryption {

class TestDecryptUncompress: public QObject
{
    Q_OBJECT

    public:
        TestDecryptUncompress();

    private slots:
        void init();
        void cleanupTestCase();
        void utTestPlainShort();
        void utTestEncrShort();
        void utTestPlainMaxsize();
        void utTestEncrMaxsize();
        void utTestPlainLong();
        void utTestEncrLong();
};

}       // end namespace EncryptionDecryption

#endif          // ENCRYPTION_DECRYPTION_TESTCOMPRESSENCRYPT_H
