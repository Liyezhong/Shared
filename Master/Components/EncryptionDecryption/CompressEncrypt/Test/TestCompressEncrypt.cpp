/****************************************************************************/
/*! \file TestCompressEncrypt.cpp
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

#include "EncryptionDecryption/CompressEncrypt/Test/TestCompressEncrypt.h"

namespace {

    const char* OUTFILE = "_out_";
}

namespace EncryptionDecryption {
/****************************************************************************/
/*!
 * \brief empty constructor
 */
/****************************************************************************/
TestCompressEncrypt::TestCompressEncrypt()
{
    ;
}
/****************************************************************************/
/*!
 * \brief init clean keyfile (all bytes null)
 */
/****************************************************************************/
void TestCompressEncrypt::init()
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
void TestCompressEncrypt::cleanupTestCase()
{
    QFile::remove(Constants::keyfile);
    QFile::remove(Constants::counter);
    QFile::remove(OUTFILE);
}

/****************************************************************************/
/*!
 * \brief write small file compressed and not encrypted
 */
/****************************************************************************/
void TestCompressEncrypt::utTestPlainShort()
{
    try
    {
        FailSafeOpen fdo(OUTFILE, 'w');

        CryptoService cs;
        cs.initHmacs();

        CompressEncrypt ce(&fdo, cs);
        QByteArray data(100, 'a');
        ce.write(data);
        ce.close();
        fdo.close();

        FailSafeOpen fdi(OUTFILE, 'r');

        QCOMPARE(fdi.read(),
            QByteArray::fromHex("0000001000000064789c4b4ca43d00007a4725e5"));
        fdi.close();
    }
    catch(...)
    {
        //QFAIL(qPrintable(e.getMsg()));
    }
}

/****************************************************************************/
/*!
 * \brief write small file compressed and encrypted
 */
/****************************************************************************/
void TestCompressEncrypt::utTestEncrShort()
{
    FailSafeOpen fdo(OUTFILE, 'w');

    CryptoService cs;
    cs.initHmacs();

    CompressEncrypt ce(&fdo, cs, true);
    QByteArray data(100, 'a');
    ce.write(data);
    ce.close();
    fdo.close();

    FailSafeOpen fdi(OUTFILE, 'r');

    QCOMPARE(fdi.read(),
             QByteArray::fromHex("7de1fd7a383489edfa3c1b10b1d06fa957cecbb2"));
    fdi.close();
}

/****************************************************************************/
/*!
* \brief write file of size COMPR_ENCR_BUFSIZE compressed and not encrypted
*/
/****************************************************************************/
void TestCompressEncrypt::utTestPlainMaxsize()
{
    FailSafeOpen fdo(OUTFILE, 'w');

    CryptoService cs;
    cs.initHmacs();

    CompressEncrypt ce(&fdo, cs);
    QByteArray data(Constants::COMPR_ENCR_BUFSIZE, 'X');
    ce.write(data);
    ce.close();
    fdo.close();

    FailSafeOpen fdi(OUTFILE, 'r');

    QCOMPARE(fdi.read(),
             QByteArray::fromHex("0000041400100000789cedc13101"
                                 "000000c2a09aeb9fc410be4001") +
             QByteArray(1015, '\0') +
             QByteArray::fromHex("7c06d86a5281")
            );
    fdi.close();
}
/****************************************************************************/
/*!
* \brief write file of size COMPR_ENCR_BUFSIZE compressed and encrypted
*/
/****************************************************************************/
void TestCompressEncrypt::utTestEncrMaxsize()
{
    FailSafeOpen fdo(OUTFILE, 'w');

    CryptoService cs;
    cs.initHmacs();

    CompressEncrypt ce(&fdo, cs, true);
    QByteArray data(Constants::COMPR_ENCR_BUFSIZE, 'X');
    ce.write(data);
    ce.close();
    fdo.close();

    FailSafeOpen fdi(OUTFILE, 'r');

    QCOMPARE(fdi.read(),
       QByteArray::fromHex(
       "7de1f97e38248989fa3cbd9d24ec6fa92d4b4ecd1ab48ef1e3a9f72c63d63890"
       "888f56b6f327e0ac0858ee844743490e350442b089cd29fdd95ff73b27b387b9"
       "60bc7d84585a7060625ee6fb2c2de9dd2cb3f5e9e02907e4b5866bd78ff1f965"
       "fb6cf7dcef7c8b287895c5ceed6a9fec444ef38ece19c87c8df51e78eb5dc89d"
       "26e723cf5bab6d91d2f0bf09d6c737c791e77ba6f1f6ec922cdd1af6accc3d26"
       "9ea92c88698865f4e05abd71d52a1ae91b1abb86715fc589fea119d90e20382f"
       "59ee7bbc925fe6cd8da31583169476a77261d03d2134b7a3b1538a367ef157d4"
       "f4259932b9539ddce4c3ca1a37435b6c755af48d46ce022df4d7ccbaee9ecf84"
       "6f7c395c441a123fec4e5a21eb16fd5864e59c8f82daa38f19e14c9d1e0360f9"
       "fde2961782dbe764b9afd8ddc6a9cc7bb9aab9951c279cd750518cff8c991fac"
       "9fb6f8d0ee8a737765745f55a35932dbb6daf7e92f9d3226ab2987e136a30fe8"
       "9f13c57874f815c23822e3b36c1e595023117b3d11973944d947b62aa532f2b7"
       "980fe891a97c71224833783fa48406624f0b3d252fac1d047969b931a55f699a"
       "0530e4a9b8af0da2ff4ddcb7345a4ba82be534eb619b6c7e783e605f319a637f"
       "d361d2536951652089b69ccbe06a5b45ea9711be677e110af04e87b61be29f06"
       "f032353e3beb2a49adfd7b9b2b47a21caad235cd0112c974f48127be9fd8136a"
       "065632fc242500741647667bcf34a9fc4f8a53d7a04a49d5bdfa7dc464796e34"
       "207f1f62fd8b23a0af6cf594e79fc07e8f7b35fb4a26fb299cd27fb761cd6d80"
       "81e20e87f2b9cef78b6f5c259832d30b10177c71cf0bf98e5995919bfa503cf7"
       "05a070087e8f5c0c068395550c2c8b4de5d614ad5e524632258a4f65a43245f6"
       "edad24b9681a5fbd99dec3ae83db39ec8091dba1a39ebe480d1c29f874b84110"
       "6e9d9c36a41bf17a8576435ec5ed7caabc76ddde76d45802b0a2fd9e9167bb97"
       "91801b8742c7a41cce453ea4d49bcf33227d90e840672e445223978fd86e562b"
       "8b037eaab72ad16787f89da6a7e80006633c6bb8aa02ec4219d99e92942a5f22"
       "d469510e031bf8c75665471f2cca221b06e7d837ddce81e6049c2e6cd686ec27"
       "f30d2dd301ba8e6d9b16523c45f906da84ebfda693cfaa78da2b09c6f291ecff"
       "e9a46b5c3c3cd1841432162569c7658aa1bd35a97e56390c0b7993786b0c760d"
       "8694191ce1fc139e1f9cadf376a9ab3dda1efe6163f08c8ee76ebe83d48e1bb2"
       "65813aa8de875ce5a0c46f63c6c3895f1b55868f60712b2a916aa54305f72b7d"
       "cba26903288028910551af98c680e63829061e78ec9492d450d0b115f12ff534"
       "c6fa81055f733c43efb953eb602867687fd4840f644cc7b9a374b970131c8462"
       "91d098efdb7afef98dbbc1f9a697ec0ac4b6de93157593b9506f5fa87d55252b"
       "8cbc1b854442294e48153f9c625e825f8588a3a77c4186cc"
       )
       );
    fdi.close();
}

/****************************************************************************/
/*!
 * \brief write file of size 2*COMPR_ENCR_BUFSIZE+1 compressed and not encrypted
 */
/****************************************************************************/
void TestCompressEncrypt::utTestPlainLong()
{
    FailSafeOpen fdo(OUTFILE, 'w');

    CryptoService cs;
    cs.initHmacs();

    CompressEncrypt ce(&fdo, cs);
    QByteArray data(2*Constants::COMPR_ENCR_BUFSIZE+1, 'z');
    ce.write(data);
    ce.close();
    fdo.close();

    FailSafeOpen fdi(OUTFILE, 'r');

    QCOMPARE(fdi.read(),
             QByteArray::fromHex("0000041300100000789cedc13101"
                                 "000000c2a0de4b6f085fa0") +
             QByteArray(1016, '\0') +
             QByteArray::fromHex("3e03da3b7261"
                                 "0000041300100000789cedc13101"
                                 "000000c2a0de4b6f085fa0") +
             QByteArray(1016, '\0') +
             QByteArray::fromHex("3e03da3b7261"
                                 "0000000d00000001789cab0200007b007b")
            );
    fdi.close();
}

/****************************************************************************/
/*!
 * \brief write file of size 2*COMPR_ENCR_BUFSIZE+1 compressed and encrypted
 */
/****************************************************************************/
void TestCompressEncrypt::utTestEncrLong()
{
    FailSafeOpen fdo(OUTFILE, 'w');

    CryptoService cs;
    cs.initHmacs();

    CompressEncrypt ce(&fdo, cs, true);
    QByteArray data(2*Constants::COMPR_ENCR_BUFSIZE+1, 'z');
    ce.write(data);
    ce.close();
    fdo.close();

    FailSafeOpen fdi(OUTFILE, 'r');

    QCOMPARE(fdi.read(),
             QByteArray::fromHex(
      "7de1f97938248989fa3cbd9d24ec6fa92d4b4e89ba4442befde9f62c63d63890"
      "888f56b6f327e0ac0858ee844743490e350442b089cd29fdd95ff73b27b387b9"
      "60bc7d84585a7060625ee6fb2c2de9dd2cb3f5e9e02907e4b5866bd78ff1f965"
      "fb6cf7dcef7c8b287895c5ceed6a9fec444ef38ece19c87c8df51e78eb5dc89d"
      "26e723cf5bab6d91d2f0bf09d6c737c791e77ba6f1f6ec922cdd1af6accc3d26"
      "9ea92c88698865f4e05abd71d52a1ae91b1abb86715fc589fea119d90e20382f"
      "59ee7bbc925fe6cd8da31583169476a77261d03d2134b7a3b1538a367ef157d4"
      "f4259932b9539ddce4c3ca1a37435b6c755af48d46ce022df4d7ccbaee9ecf84"
      "6f7c395c441a123fec4e5a21eb16fd5864e59c8f82daa38f19e14c9d1e0360f9"
      "fde2961782dbe764b9afd8ddc6a9cc7bb9aab9951c279cd750518cff8c991fac"
      "9fb6f8d0ee8a737765745f55a35932dbb6daf7e92f9d3226ab2987e136a30fe8"
      "9f13c57874f815c23822e3b36c1e595023117b3d11973944d947b62aa532f2b7"
      "980fe891a97c71224833783fa48406624f0b3d252fac1d047969b931a55f699a"
      "0530e4a9b8af0da2ff4ddcb7345a4ba82be534eb619b6c7e783e605f319a637f"
      "d361d2536951652089b69ccbe06a5b45ea9711be677e110af04e87b61be29f06"
      "f032353e3beb2a49adfd7b9b2b47a21caad235cd0112c974f48127be9fd8136a"
      "065632fc242500741647667bcf34a9fc4f8a53d7a04a49d5bdfa7dc464796e34"
      "207f1f62fd8b23a0af6cf594e79fc07e8f7b35fb4a26fb299cd27fb761cd6d80"
      "81e20e87f2b9cef78b6f5c259832d30b10177c71cf0bf98e5995919bfa503cf7"
      "05a070087e8f5c0c068395550c2c8b4de5d614ad5e524632258a4f65a43245f6"
      "edad24b9681a5fbd99dec3ae83db39ec8091dba1a39ebe480d1c29f874b84110"
      "6e9d9c36a41bf17a8576435ec5ed7caabc76ddde76d45802b0a2fd9e9167bb97"
      "91801b8742c7a41cce453ea4d49bcf33227d90e840672e445223978fd86e562b"
      "8b037eaab72ad16787f89da6a7e80006633c6bb8aa02ec4219d99e92942a5f22"
      "d469510e031bf8c75665471f2cca221b06e7d837ddce81e6049c2e6cd686ec27"
      "f30d2dd301ba8e6d9b16523c45f906da84ebfda693cfaa78da2b09c6f291ecff"
      "e9a46b5c3c3cd1841432162569c7658aa1bd35a97e56390c0b7993786b0c760d"
      "8694191ce1fc139e1f9cadf376a9ab3dda1efe6163f08c8ee76ebe83d48e1bb2"
      "65813aa8de875ce5a0c46f63c6c3895f1b55868f60712b2a916aa54305f72b7d"
      "cba26903288028910551af98c680e63829061e78ec9492d450d0b115f12ff534"
      "c6fa81055f733c43efb953eb602867687fd4840f644cc7b9a374b970131c8462"
      "91d098efdb7afef98dbbc1f9a697ec0ac4b6de93157593b9506f5fa87d55252b"
      "8cbc1b854442294e48153f9c625e825f85b6dc7b9f59b5"
      "4db272dc3a934eca39fb6d18f97a4e03ae9fe5300eaf59c7fd1398ebe29fbbec"
      "9fcabd1bebacc9021f54d387e80eca6e6f0a98adc1325d04506e6d85ef1a95f1"
      "a6d0ce3b862bb9e604007e34fc0212ccd54e4c5887f13c91891855dc214a114f"
      "1c0d7a4bd64400bd3cfc491bf98a63ca1fe7b51b892c9b65bbefff58066b8be8"
      "2bdffe3bb2dc7b2753b9a274710d280ea7286472b0441d0742b95e20c7a73236"
      "8595c6b1aa0e0d8628e328fe5bfe2cdad184f1ce747be823a689ca0fbdc79737"
      "5db8cbb698e07f926341bc7d42c1cd30440f4e8d155995e9abc9304854dfeb68"
      "6fa946476ca442f94ebd94ec77d64af983ffb9b71104619b9d9e9687fa305e51"
      "f5ab5f7a62c8676ed9574e42ce7e201b9a3055d88e847132e65b92ed8be52dae"
      "cfc3b9b9c1e6735450253cf77c15f8618c5bd802f674f581ed75a35608a49bfa"
      "3ac8bc6a16abb0929f419a7e8db0a43d049627abccf345c7ddb6dae4a978d668"
      "5fee43e31ad115a83377da6345a048c5c4c207ddce572add7d3d1b893ba4987e"
      "6765b23624611b423ece4c2f5a70522cadeff1dc964e3f932b1dd231a0d28b4b"
      "e398b670f044f9eaa548a224af6b99543249b1fcbec58b3f6683baf0652d404e"
      "c9dd8b38147447ec9f4a5a81aef8b21bcbd7085f20f28d49d46e0d71be5583e4"
      "4d1ea1da20397f80403bc4da4c058458e018d112720d52fba9c950768d7c417e"
      "93ea9964937231614f455a387f900313490e38a02f5afbfcdc072bb5f45677d6"
      "9781cb7bb1908e4a024fa2a4e886e4b36f5ce69fb90518147ff1ee58aa67ddf6"
      "3e494f115f44ed9e071bd1bb392d9da7032c7e529aac8b09b4e9e6c023dd9a17"
      "5fe1397f7e1601602c25cb6bc2ffcf33810a27f9c45c1fc024d7b13acc37dfbd"
      "99f033e97d24c8dadc6dc75f9e0177fe4da1f59a7ece4c81c02bcc34e641468b"
      "4bb54f3c6a17e5448aed542a6fb5fe32ca596cd34fc7861e41e40e225d3b638c"
      "ba64070836e7374be98a12c2b103d3480be2d9bd7886e57e10974bbcca1d370e"
      "b57ada10f030ba7b7ce7bf1503a64da819fd4f2bd238b9f5603d67e46c67b096"
      "eb044d4dda954e235470b505a3394d0786a4d790cdfd39ceb262bfb7179d9165"
      "59aca618ec52d97efd8529e358dc7703a8a9c5132594fe58ebac341df0d410d9"
      "3e4a13d97359eda7a0cbcd48d6b9094c19e96acaac9a1ee9de44bbd8daa3b3d9"
      "1def13f9bf46899127216a24313af9cf8e9868aeae0e1cc295765b796510995e"
      "ce90d65331a73e56ed6c0f9fa37b2a9e11350c61f46b1baff69d1147fb171b88"
      "fd8ada1149eaf594d69cb1756718eb1dda1801f2bcdcb789dfce29e60ef6a007"
      "c2794da63652e6681c4174e43d73507d154628f4f55ede3131c66f67063e9070"
      "053673085124f2a3c896f65817630052cd02d849aa55f06a030c777162b5dc5f"
      "592a6108f64c6d7a6b8dadddb3d76a0584d5c4fa2614e6"
      "46a5aee2ee861d981ca039d51ee61077db"
      )
      );
    fdi.close();
}

/****************************************************************************/
/*!
 * \brief benchmark encryption without and with compression
 */
/****************************************************************************/
void TestCompressEncrypt::utTestBenchmark()
{
    const int STREAMLEN = 100*1024*1024;

    FailSafeOpen fdo(OUTFILE, 'w');

    CryptoService cs;
    cs.initHmacs();

    QTime t;

    {
        CompressEncrypt ce(&fdo, cs);
        QByteArray data(STREAMLEN, 'z');
        t.start();
        ce.write(data);
        ce.close();

        double bpsec = STREAMLEN/(t.elapsed()/1000.);
        qDebug("\nEncryptCompress without encryption: %.1f MB/sec\n",
               bpsec/1024./1024.);
    }

    {
        CompressEncrypt ce(&fdo, cs, true);
        QByteArray data(STREAMLEN, 'z');
        t.start();
        ce.write(data);
        ce.close();

        double bpsec = STREAMLEN/(t.elapsed()/1000.);
        qDebug("\nEncryptCompress with encryption: %.1f MB/sec\n",
               bpsec/1024./1024.);
    }

    fdo.close();
}

}       // end namespace EncryptionDecryption

QTEST_MAIN(EncryptionDecryption::TestCompressEncrypt)
