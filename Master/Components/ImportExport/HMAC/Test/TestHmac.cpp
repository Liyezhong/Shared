/****************************************************************************/
/** @file TestHmac.cpp
 *
 *  @brief Testclass for HMAC computation with SHA-1.
 *
 *  Test vectors are taken from RFC 2202.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-06-19
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
 *  last modified by owner: @(#) Aug 24 2011, 12:39:41
 *
 */
/****************************************************************************/


#include "ImportExport/HMAC/Test/TestHmac.h"

namespace ImportExport {

/**
 * @brief empty constructor
 */

TestHmac::TestHmac()
{
    ;
}

/**
 * @brief check on test vectors taken from RFC 2202
 */

void TestHmac::utHmacTestVectors()
{
    QList<OneTest> Testcases;

    Testcases << 

    OneTest(QByteArray(20, 0x0b), "Hi There",
        "0xb617318655057264e28bc0b6fb378c8ef146be00") <<

    OneTest(QByteArray("Jefe"), "what do ya want for nothing?",
            "0xeffcdf6ae5eb2fa2d27416d5f184df9c259a7c79") <<

    OneTest(QByteArray(20, 0xaa), QByteArray(50, 0xdd),
                       "0x125d7342b9ac11cd91a39af48aa17b4f63f175d3") <<

    OneTest(QByteArray::fromHex(QByteArray(
                       "0102030405060708090a0b0c0d0e0f10111213141516171819")),
                       QByteArray(50, 0xcd),
                       "0x4c9007f4026250c6bc8414f9bf50c86c2d7235da") <<

    OneTest(QByteArray(20, 0x0c), "Test With Truncation",
                       "0x4c1a03424b55e07fe7f27be1d58bb9324a9a5a04");

    foreach(OneTest test, Testcases)
    {
        Hmac h(test.key);
        h.update(test.data);
        QVERIFY(h.hmac().toHex().insert(0, "0x") == test.hmac);
    }
}


/**
 * @brief check on SHA-1 test vectors taken from FIPS 180-2, A2 and A3
 */

void TestHmac::utHashTest()
{
    QVERIFY(Hmac::hash(
            "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") ==
            QByteArray::fromHex("84983e441c3bd26ebaae4aa1f95129e5e54670f1"));
    QVERIFY(Hmac::hash(QByteArray(1000000, 'a')) ==
            QByteArray::fromHex("34aa973cd4c4daa4f61eeb2bdbad27316534016f"));
}


/**
 * @brief HMAC benchmark
 */

void TestHmac::utHmacBench()
{
    const int CHUNK = 1000;     // hashing in chunks
    const int STREAMLEN = 1000*1000*100;
                                // # of bytes to be hashed

    QByteArray key(8, 0x5c);
    Hmac h(key);

    QByteArray chunk(CHUNK, 0);

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

        h.update(chunk);
    }

    h.hmac();
    //h.update("aha");  // will throw error!

    double bpsec = STREAMLEN/(t.elapsed()/1000.);
    qDebug("\nHMAC speed:\n\t%.1f MB/second\n",
           bpsec/1024./1024.);
}

}               // end namespace ImportExport

QTEST_MAIN(ImportExport::TestHmac)
