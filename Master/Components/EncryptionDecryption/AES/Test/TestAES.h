/****************************************************************************/
/*! \file TestAES.h
 *
 *  \brief Testclass for AES - variable key and random data tests.
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

#ifndef ENCRYPTION_DECRYPTION_TESTAES_H
#define ENCRYPTION_DECRYPTION_TESTAES_H

#include <QtTest>
#include <QObject>
#include <QTime>
#include "EncryptionDecryption/AES/Include/Aes.h"

namespace EncryptionDecryption {

typedef struct
{
    const char *key; const char *ciphertext;
} VARKEYTEST;

typedef struct
{
    const char *key;
    const char *plain;
    const char *ciphertext;
} RANDTEST;

extern VARKEYTEST VarData[];
extern RANDTEST RandData[];
extern int VarSize;
extern int RandSize;

/*!
 * \brief test class for AES - variable key and random data test
 */

class TestAes: public QObject
{
    Q_OBJECT

    public:
        TestAes();           // default constructor

    private:

    private slots:
        void utAesVariableKeyTest();
        void utAesRandomDataTest();
        void utAesCtrSimpleTest();
        void utAesBenchTest();
};

}       // end namespace EncryptionDecryption

#endif          //ENCRYPTION_DECRYPTION_TESTAES_H
