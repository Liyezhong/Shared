/****************************************************************************/
/*! \file TestAES.cpp
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

#include "EncryptionDecryption/AES/Test/TestAES.h"

namespace EncryptionDecryption {
/****************************************************************************/
/*!
 * \brief empty constructor
 */
/****************************************************************************/
TestAes::TestAes()
{
    ;
}
/****************************************************************************/
/*!
 * \brief do the variable key test (plaintext 0, key variable)
 */
/****************************************************************************/
void TestAes::utAesVariableKeyTest()
{
    VARKEYTEST test;

    for(int i=0; i < VarSize; ++i)
    {

        test = VarData[i];

        QByteArray plain(16, '\0');
        QByteArray key(QByteArray::fromHex(QByteArray(test.key)));

        AES aes;
        aes.init(key);

        aes.RijndaelEncrypt(
                    static_cast<quint8*>(
                               static_cast<void*>(
                                          plain.data()
                                                 )
                                         )
                           );

        QCOMPARE(test.ciphertext, plain.toHex().constData());
    }
}
/****************************************************************************/
/*!
 * \brief do the random data test (plaintext and key variable)
 */
/****************************************************************************/
void TestAes::utAesRandomDataTest()
{
    RANDTEST test;

    for(int i=0; i < RandSize; ++i)
    {

        test = RandData[i];

        QByteArray plain(QByteArray::fromHex(QByteArray(test.plain)));
        QByteArray key(QByteArray::fromHex(QByteArray(test.key)));

        AES aes;
        aes.init(key);

        aes.RijndaelEncrypt(
                    static_cast<quint8*>(
                               static_cast<void*>(
                                          plain.data()
                                                 )
                                         )
                           );

        QCOMPARE(test.ciphertext, plain.toHex().constData());
    }
}
/****************************************************************************/
/*!
 * \brief test one AES block with CTR encryption
 *
 * Check against variable plaintext test with key 0, plaintext 1
 * (testcase 128 in NIST submission, vt.txt); check encryption and
 * check that decryption yields null plaintext again.
 */
/****************************************************************************/
void TestAes::utAesCtrSimpleTest()
{
    QByteArray key(AES::AES_SIZE, 0);
    QByteArray plain(AES::AES_SIZE, 0);

    // encrypt with CTR block "1"
    {
        AES a;
        a.init(key);
        a.AesCtr(plain);
    }

    QCOMPARE(plain.toHex().constData(),
        "58e2fccefa7e3061367f1d57a4e7455a");

    {
        AES a;
        a.init(key);
        a.AesCtr(plain);
    }

    char *p;
    int i;
    for(p = plain.data(), i = AES::AES_SIZE; i--; ++p)
    {
        QVERIFY(*p == '\0');
    }
}

/****************************************************************************/
/*!
 * \brief encrypt and decrypt a long plaintext as stream, benchmark it
 */
/****************************************************************************/
void TestAes::utAesBenchTest()
{
    const int CHUNK = 1000;     // encryption and decryption in chunks
    const int STREAMLEN = 1000*1000*10;
                                // # of bytes to be en/decrypted

    QByteArray key(AES::AES_SIZE, 0x5c);
    AES enc;
    enc.init(key);
    AES dec;
    dec.init(key);

    QByteArray chunk(CHUNK, 0);
    quint8 plain[CHUNK];

    int ctr = 0;
    QTime t;
    t.start();

    while(ctr < STREAMLEN)
    {
        for(int i=0; i < CHUNK; ++i)
        {
            chunk[i] = i+ctr;
        }

        ctr += CHUNK;

        memcpy(plain, chunk.data(), CHUNK);

        enc.AesCtr(chunk);
        dec.AesCtr(chunk);
        QVERIFY(!memcmp(chunk.data(), plain, AES::AES_SIZE));
    }

    double bpsec = STREAMLEN/(t.elapsed()/1000.);
    qDebug("encryption+decryption speed:\n\t%.1f MB/second\n",
           bpsec/1024./1024.);
}

}       // end namespace EncryptionDecryption

QTEST_MAIN(EncryptionDecryption::TestAes)
