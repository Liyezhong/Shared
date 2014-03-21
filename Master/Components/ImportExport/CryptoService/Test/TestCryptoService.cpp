/****************************************************************************/
/*! \file TestCryptoService.cpp
 *
 *  \brief Testclass for CryptoService
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


#include "ImportExport/CryptoService/Test/TestCryptoService.h"

namespace ImportExport {
/****************************************************************************/
/*!
 * \brief empty constructor
 */
/****************************************************************************/
TestCryptoService::TestCryptoService()
{
    ;
}
/****************************************************************************/
/*!
 * \brief init clean keyfile (all bytes null)
 */
/****************************************************************************/
void TestCryptoService::init()
{
    FailSafeOpen keyfile(Constants::keyfile, 'w');
    keyfile.write(QByteArray(Constants::KEYFILESIZE, 0));
    keyfile.close();

    FailSafeOpen ctrfile(Constants::counter, 'w');
    ctrfile.write(QByteArray(4, 0));
    ctrfile.close();
}
/****************************************************************************/
/*!
 * \brief delete the keyfile after all tests
 */
/****************************************************************************/
void TestCryptoService::cleanupTestCase()
{
    QFile::remove(Constants::keyfile);
    QFile::remove(Constants::counter);
}

/****************************************************************************/
/*!
 * \brief help function reading the keyfile
 *
 * \iparam keymat - reference to struct KeyMaterial, will be filled
 */
/****************************************************************************/
void TestCryptoService::readKeyfile(KeyMaterial& keymat)
{
    FailSafeOpen keyfile(Constants::keyfile, 'r');
    QByteArray qdata = keyfile.read();
    keyfile.close();

    Q_ASSERT(qdata.size() == Constants::KEYFILESIZE);

    keymat.Leicakey = qdata.mid(0, Constants::HASH_SIZE);
    keymat.Viewerkey = qdata.mid(Constants::HASH_SIZE, Constants::HASH_SIZE);
    keymat.Importkey = qdata.mid(2*Constants::HASH_SIZE, Constants::HASH_SIZE);

    uchar *ctrbytes = reinterpret_cast<uchar*>(
                                   qdata.mid(3*Constants::HASH_SIZE, 4).data());
    keymat.counter = qFromBigEndian<qint32>(ctrbytes);

    FailSafeOpen ctrfile(Constants::counter, 'r');
    qdata = ctrfile.read();
    ctrfile.close();

    Q_ASSERT(qdata.size() == 4);

    keymat.sdCounter = General::byte2int(qdata.data());
}
/****************************************************************************/
/*!
 * \brief test that only a single instance of CryptoService can run
 */
/****************************************************************************/
void TestCryptoService::utTestSingle()
{
    CryptoService cs;
    try
    {
        CryptoService cs2;
        QFAIL("exception expected in utTestSingle");
    }
    catch(...)
    {
        // nothing to do
    }
}
/****************************************************************************/
/*!
 * \brief check on keys = SHA1(20*'\0')
 */
/****************************************************************************/
void TestCryptoService::utTestInitialKeys()
{
    CryptoService cs;
    QCOMPARE(cs.m_keys["Viewer"], cs.m_keys["Leica"]);
    QCOMPARE(cs.m_keys["Viewer"],
            QByteArray::fromHex("6768033e216468247bd031a0a2d9876d79818f8f"));
    QCOMPARE(cs.m_keys["Import"],
            QByteArray::fromHex("b145670ab697fee2979c22b562a98d2067e5fce8"));
}
/****************************************************************************/
/*!
 * \brief check on HMAC values with initial keys and no data
 */
/****************************************************************************/
void TestCryptoService::utTestHmacEmpty()
{
    CryptoService cs;
    cs.initHmacs();
    QCOMPARE(cs.m_hmac["Viewer"]->hmac(), cs.m_hmac["Leica"]->hmac());

    // check with no data
    QCOMPARE(cs.m_hmac["Viewer"]->hmac(),
            QByteArray::fromHex("4673a1f15a381e0b1c38258d5f1c2e0cae1ee426"));

}

/****************************************************************************/
/*!
 * \brief check on HMAC values with initial keys and some data and after reset
 */
/****************************************************************************/
void TestCryptoService::utTestHmacData()
{
    CryptoService cs;
    cs.initHmacs();

    cs.updateHMACs("some junk");
    QCOMPARE(cs.getHmacs()["Viewer"],
            QByteArray::fromHex("781494db31c75ae3f58a4eb2b25401090135c989"));
    cs.initHmacs();
    QCOMPARE(cs.getHmacs()["Viewer"],
            QByteArray::fromHex("4673a1f15a381e0b1c38258d5f1c2e0cae1ee426"));

}

/****************************************************************************/
/*!
 * \brief check computeKey function for index 0
 */
/****************************************************************************/
void TestCryptoService::utTestComputeKeyStart()
{
    QByteArray key(20, '\0');
    CryptoService::computeKey(key, "deviceID", 0);
    QCOMPARE(key,
             QByteArray::fromHex("a9334ea2d56c15e5184ecdd924487610fb1d8888"));

}

/****************************************************************************/
/*!
 * \brief check computeKey function for index 100000
 */
/****************************************************************************/
void TestCryptoService::utTestComputeKeyBench()
{
    QByteArray key(20, '\0');

    QTime t;
    t.start();

    CryptoService::computeKey(key, "deviceID", 1000000);
    QCOMPARE(key,
             QByteArray::fromHex("aca4b0145ebabbe8d97eb13e4907f9003429fbd9"));

    qDebug("\ncomputeKey speed: %.0f updates/seconds\n",
           1.e+7/(t.elapsed()/1000.));

}

/****************************************************************************/
/*!
 * \brief test stepHashChain
 */
/****************************************************************************/
void TestCryptoService::utTestStepHashChain()
{
    CryptoService cs;
    cs.stepHashChain();

    KeyMaterial keymat;

    readKeyfile(keymat);

    QCOMPARE(keymat.Leicakey,
             QByteArray::fromHex("24d5d1ed739e0825bc6d9d44a7dd0d720454eafa"));
    QCOMPARE(keymat.Viewerkey, keymat.Leicakey);
    QCOMPARE(keymat.Importkey,
             QByteArray::fromHex("b145670ab697fee2979c22b562a98d2067e5fce8"));
    QCOMPARE(keymat.counter, 1);
    QCOMPARE(keymat.sdCounter, 1);

    cs.stepHashChain();

    readKeyfile(keymat);

    QCOMPARE(keymat.Leicakey,
             QByteArray::fromHex("2be310f6065ab0ab65ff7af8ec628fcbda47743c"));
    QCOMPARE(keymat.Viewerkey, keymat.Leicakey);
    QCOMPARE(keymat.Importkey,
             QByteArray::fromHex("b145670ab697fee2979c22b562a98d2067e5fce8"));
    QCOMPARE(keymat.counter, 2);
}

/****************************************************************************/
/*!
 * \brief test re-initialization of hash chain: index on SD card > 0, new ebox
 */
/****************************************************************************/
void TestCryptoService::utTestHashChainReinit()
{
    QByteArray sdindex(4,0);
    sdindex[3] = '\2';

    FailSafeOpen ctrfile(Constants::counter, 'w');
    ctrfile.write(sdindex);
    ctrfile.close();

    CryptoService cs;

    QCOMPARE(cs.m_keys["Viewer"], cs.m_keys["Leica"]);
    QCOMPARE(cs.m_keys["Viewer"],
            QByteArray::fromHex("2be310f6065ab0ab65ff7af8ec628fcbda47743c"));
    QCOMPARE(cs.m_keys["Import"],
            QByteArray::fromHex("b145670ab697fee2979c22b562a98d2067e5fce8"));
    QCOMPARE(cs.m_hashChainIndex, 2);
}

/****************************************************************************/
/*!
 * \brief encryption test with null block
 */
/****************************************************************************/
void TestCryptoService::utTestEncryption()
{
    QByteArray plain(AES::AES_SIZE, 0);

    {
        CryptoService cs;
        cs.initAES();

        cs.encrypt(plain);

        QCOMPARE(plain.toHex().constData(), "7de1fd6a3834898982a0505c15ed6fa9");
    }

    // decryption test
    {
        CryptoService cs;
        cs.initAES();
        cs.encrypt(plain);
    }

    char *p;
    int i;
    for(p = plain.data(), i = AES::AES_SIZE; i--; ++p)
    {
        QCOMPARE(*p, '\0');
    }

}

}               // end namespace ImportExport

QTEST_MAIN(ImportExport::TestCryptoService)
