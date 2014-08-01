/****************************************************************************/
/*! \file TestHmac.h
 *
 *  \brief Testclass for HMAC computation with SHA-1.
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

#ifndef ENCRYPTION_DECRYPTION_TESTHMAC_H
#define ENCRYPTION_DECRYPTION_TESTHMAC_H

#include <QtTest>
#include <QObject>
#include <QString>
#include <QList>
#include <QTime>
#include "EncryptionDecryption/HMAC/Include/Hmac.h"

namespace EncryptionDecryption {

struct OneTest
{
    OneTest(const QByteArray k, const char *s, const char *d):
        key(k), data(s), hmac(d) {};
    OneTest(const QByteArray k, const QByteArray s, const char *d):
        key(k), data(s), hmac(d) {};

    QByteArray key;
    QByteArray data;
    QByteArray hmac;
};

class TestHmac: public QObject
{
    Q_OBJECT

    public:
        TestHmac();           // default constructor

    private:

    private slots:
        void utHmacTestVectors();
        void utHashTest();
        void utHmacBench();
};

}       // end namespace EncryptionDecryption

#endif          //ENCRYPTION_DECRYPTION_TESTHMAC_H
