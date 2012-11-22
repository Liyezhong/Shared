/****************************************************************************/
/** @file CryptoService.cpp
 *
 *  @brief CryptoService for ImportExport module
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-07-02
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
 *  last modified by owner: @(#) Aug 31 2011, 14:23:52
 *
 */
/****************************************************************************/

#include "ImportExport/CryptoService/Include/CryptoService.h"

namespace {

static bool CryptoServiceRunning = false;

}

namespace ImportExport {

/**
 * @brief constructor
 *
 * Read keys and init three HMAC instances.
 * On initial call (if hash index in NAND is 0),
 *
 * @param deviceID - serial number used for archive names and key generation
 * @param keydata - optional field containing keys, if empty:
 *                      keys read from file Constants::keyfile
 * @param version - the version field, version of keys - unused yet
 */

CryptoService::CryptoService(const QByteArray deviceID,
                             QByteArray keydata,
                             int version):
    S0(QByteArray("3,1415926535897932@Leica")),   // mind the comma
    S1(QByteArray("1.6180339887498949@Aciel")),   // mind the dot
    m_aesInitialized(false), m_hmacInitialized(false)
{
    Q_ASSERT(!version);
    Q_UNUSED(version);

    bool keydataGiven = true;

    if(CryptoServiceRunning)
    {
        THROW("try to run more than one CryptoService");
    }

    CryptoServiceRunning = true;

    if(keydata.isEmpty())
    {
        keydataGiven = false;

        // read the keys
        FailSafeOpen keyfile(Constants::keyfile, 'r');
        keydata = keyfile.read();
        keyfile.close();
    }

    Q_ASSERT(keydata.size() == Constants::KEYFILESIZE);

    m_keys["Leica"] = keydata.mid(0, Constants::HASH_SIZE);
    m_keys["Viewer"] = keydata.mid(Constants::HASH_SIZE, Constants::HASH_SIZE);
    m_keys["Import"] = keydata.mid(2*Constants::HASH_SIZE,
                                   Constants::HASH_SIZE);

    char* bctr = keydata.mid(3*Constants::HASH_SIZE, 4).data();
    m_hashChainIndex = General::byte2int(bctr);

    int sdIndex;

    // A comparison with the counter on SD card makes only sense
    // in the normal run where the keydata parameter is empty on call.

    if(!keydataGiven)
    {
        // read counter value from SD card
        FailSafeOpen ctrfile(Constants::counter, 'r');
        char* ctrdata = ctrfile.read().data();
        ctrfile.close();

        sdIndex = General::byte2int(ctrdata);
    }
    else
    {
        sdIndex = m_hashChainIndex;
    }

    // XXX check on reasonable values for counters (unsigned anyway)

    if(0 == m_hashChainIndex && 0 == sdIndex)
    {
        // initial state, new device:
        // init C from B and S0, compute A0 and B0
        m_keys["Import"] = Hmac::hash(m_keys["Viewer"] + S0);
        m_keys["Leica"] = Hmac::hash(m_keys["Leica"] + deviceID);
        m_keys["Viewer"] = Hmac::hash(m_keys["Viewer"] + deviceID);
    }
    else if(0 == m_hashChainIndex && 0 < sdIndex)
    {
        // new E-box, SD card taken from defect box: re-init keys
        m_keys["Import"] = Hmac::hash(m_keys["Import"] + S0);
        computeKey(m_keys["Leica"], deviceID, sdIndex);
        computeKey(m_keys["Viewer"], deviceID, sdIndex);
        m_hashChainIndex = sdIndex;
        // XXX log entry?
    }
    else if(0 < m_hashChainIndex && m_hashChainIndex == sdIndex)
    {
        // default case: keys are initialized, SD card is sync'ed
        ;
    }
    else if(0 < m_hashChainIndex && 0 == sdIndex)
    {
        // a new SD card was inserted into a used E-box, write log entry
        ; // XXX @TODO warning to log
    }
    else if(0 < m_hashChainIndex && 0 < sdIndex)
    {
        // error: used E-box but also used SD card, cannot sync
        // both indices cannot be equal, was tested above
        ; // XXX @TODO error
    }
}


/**
 * @brief destructor
 */

CryptoService::~CryptoService()
{
    CryptoServiceRunning = false;

    if(m_hmacInitialized)
    {
        foreach(Hmac* hmac, m_hmac)     //lint !e665 !e1551 [Rw]
                                        // strange lint warning
        {
            delete hmac;                //lint !e1551
                                        // no exception should be possible
                                        // here!
        }
    }
}


/**
 * @brief update all HMAC instances with data
 *
 * @param data - data for update
 */

void CryptoService::updateHMACs(QByteArray data)
{
    Q_ASSERT(m_hmacInitialized);

    foreach(QByteArray key, Constants::keynames)
    {
        m_hmac[key]->update(data);
    }
}

/**
 * @brief return HMAC values of internal HMAC instances
 */

QMap<QByteArray, QByteArray> CryptoService::getHmacs()
{
    Q_ASSERT(m_hmacInitialized);

    QMap<QByteArray, QByteArray> result;

    foreach(QByteArray key, Constants::keynames)
    {
        result[key] = m_hmac[key]->hmac();
    }

    return result;
}


/**
 * @brief re-initialize HMAC instances
 */

void CryptoService::initHmacs()
{
    // init Hmac instances with no data yet

    foreach(QByteArray name, Constants::keynames)
    {
        if(m_hmacInitialized)
        {
            delete m_hmac[name];
        }
        m_hmac[name] = new Hmac(m_keys[name]);
    }

    m_hmacInitialized = true;
}

/**
 * @brief initialize AES instance with key
 */

void CryptoService::initAES()
{
    m_aes = AES();
    QByteArray key = m_keys["Viewer"];
    key += S1;
    key = Hmac::hash(key).left(AES::AES_SIZE);
    m_aes.init(key);
    m_aesInitialized = true;
}

/**
 * @brief encrypt/decrypt data with AES-CTR
 *
 * Encryption and decryption are the same operation.
 * The key computed in initAES() is used.
 */

void CryptoService::encrypt(QByteArray& data)
{
    Q_ASSERT(m_aesInitialized);
    m_aes.AesCtr(data);
}

/**
 * @brief comfort function: compute An or Bn for given n
 *
 * Key must be the original secret A or B and is replaced first by
 * hash(key || deviceID) and then hashed index-times.
 *
 * @param key - original secret (of size Constants::HASH_SIZE);
 *              is replaced by An or Bn
 * @param deviceID - deviceID
 * @param index - index in hash chain
 */

void CryptoService::computeKey(QByteArray& key,
                            const QByteArray& deviceID,
                            int index)
{                                               //lint !e578 [Rw]
    key = Hmac::hash(key + deviceID);

    while(index--)
    {
        key = Hmac::hash(key);
    }
}

/**
 * @brief step forward in hash chain
 *
 * hash keys "Leica" and "Viewer", increment index, write back to
 * NAND, write index also to SD card
 */

void CryptoService::stepHashChain()
{
    m_keys["Leica"] = Hmac::hash(m_keys["Leica"]);
    m_keys["Viewer"] = Hmac::hash(m_keys["Viewer"]);
    ++m_hashChainIndex;

    FailSafeOpen keyfile(Constants::keyfile, 'w');
    keyfile.write(m_keys["Leica"]);
    keyfile.write(m_keys["Viewer"]);
    keyfile.write(m_keys["Import"]);

    QByteArray ctrbytes = General::int2byte(m_hashChainIndex);
    keyfile.write(ctrbytes);
    keyfile.close();

    // write m_hashChainIndex to SD card
    FailSafeOpen ctrfile(Constants::counter, 'w');
    ctrfile.write(ctrbytes);
    ctrfile.close();
}


// WriteAndHmac class  ****************************************

/**
 * @brief constructor
 *
 * @param cs - CryptoService instance
 * @param fd - FailSafeOpen instance open for write
 */

WriteAndHmac::WriteAndHmac(CryptoService& cs, FailSafeOpen* fd):
 m_cs(cs), m_fd(fd)
{
    m_cs.initHmacs();
}


/**
 * @brief write data and update HMACs in parallel
 *
 * @param data - data to be written and hashed
 */

void WriteAndHmac::write(QByteArray data)
{
    m_fd->write(data);
    m_cs.updateHMACs(data);
}


/**
 * @brief return list of HMACs computed over written data
 */

QMap<QByteArray, QByteArray> WriteAndHmac::getHmacs()
{
    return m_cs.getHmacs();
}


}       // end namespace ImportExport
