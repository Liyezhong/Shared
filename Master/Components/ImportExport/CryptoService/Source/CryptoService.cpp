/****************************************************************************/
/*! \file CryptoService.cpp
 *
 *  \brief CryptoService for ImportExport module
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

#include "ImportExport/CryptoService/Include/CryptoService.h"
#include "ImportExport/General/Include/General.h"

namespace {

static bool CryptoServiceRunning = false; ///< flag for crypto service

}

namespace ImportExport {
/****************************************************************************/
/*!
 * \brief constructor
 *
 * Read keys and init three HMAC instances.
 * On initial call (if hash index in NAND is 0),
 *
 * \iparam deviceID - serial number used for archive names and key generation
 * \iparam keydata - optional field containing keys, if empty:
 *                      keys read from file Constants::keyfile
 * \iparam version - the version field, version of keys - unused yet
 */
/****************************************************************************/
CryptoService::CryptoService(const QByteArray deviceID,
                             QByteArray keydata,
                             int version):
    S0(QByteArray("3,1415926535897932@Leica")),   // mind the comma
    S1(QByteArray("1.6180339887498949@Aciel")),   // mind the dot
    m_aesInitialized(false), m_hmacInitialized(false)
{
    Q_UNUSED(version);

    bool keydataGiven = true;

    if(CryptoServiceRunning)
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_CRYTOSERVICE_RUNNING);
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

    if (keydata.size() != Constants::KEYFILESIZE)
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_KEYDATA_SIZE_IS_NOT_MATCHING);
    }

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
    }
    else if(0 < m_hashChainIndex && m_hashChainIndex == sdIndex)
    {
        // default case: keys are initialized, SD card is sync'ed
    }
    else if(0 < m_hashChainIndex && 0 == sdIndex)
    {
        // a new SD card was inserted into a used E-box
    }
    else if(0 < m_hashChainIndex && 0 < sdIndex)
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_INDEX_IS_MATCHING);
    }
}

/****************************************************************************/
/*!
 * \brief destructor
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 * \brief update all HMAC instances with data
 *
 * \iparam data - data for update
 */
/****************************************************************************/
void CryptoService::updateHMACs(QByteArray data)
{
    if (!m_hmacInitialized)
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_HMAC_NOT_INITIALIZED);
    }
    foreach(QByteArray key, Constants::keynames)
    {
        m_hmac[key]->update(data);
    }
}

/****************************************************************************/
/*!
 * \brief return HMAC values of internal HMAC instances
 *
 * \return Hmacs collection
 */
/****************************************************************************/
QMap<QByteArray, QByteArray> CryptoService::getHmacs()
{
    QMap<QByteArray, QByteArray> result;

    if (!m_hmacInitialized)
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_HMAC_NOT_INITIALIZED);
    }

    foreach(QByteArray key, Constants::keynames)
    {
        result[key] = m_hmac[key]->hmac();
    }

    return result;
}

/****************************************************************************/
/*!
 * \brief re-initialize HMAC instances
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 * \brief initialize AES instance with key
 */
/****************************************************************************/
void CryptoService::initAES()
{
    m_aes = AES();
    QByteArray key = m_keys["Viewer"];
    key += S1;
    key = Hmac::hash(key).left(AES::AES_SIZE);
    m_aes.init(key);
    m_aesInitialized = true;
}

/****************************************************************************/
/*!
 * \brief encrypt/decrypt data with AES-CTR
 *
 * Encryption and decryption are the same operation.
 * The key computed in initAES() is used.
 *
 * \iparam data - data for the encryption
 */
/****************************************************************************/
void CryptoService::encrypt(QByteArray& data)
{
    if (!m_aesInitialized)
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_AES_NOT_INITIALIZED);
    }
    m_aes.AesCtr(data);
}

/****************************************************************************/
/*!
 * \brief comfort function: compute An or Bn for given n
 *
 * Key must be the original secret A or B and is replaced first by
 * hash(key || deviceID) and then hashed index-times.
 *
 * \iparam key - original secret (of size Constants::HASH_SIZE);
 *              is replaced by An or Bn
 * \iparam deviceID - deviceID
 * \iparam index - index in hash chain
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 * \brief step forward in hash chain
 *
 * hash keys "Leica" and "Viewer", increment index, write back to
 * NAND, write index also to SD card
 */
/****************************************************************************/
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
/****************************************************************************/
/*!
 * \brief constructor
 *
 * \iparam cs - CryptoService instance
 * \iparam fd - FailSafeOpen instance open for write
 */
/****************************************************************************/
WriteAndHmac::WriteAndHmac(CryptoService& cs, FailSafeOpen* fd):
 m_cs(cs), mp_fd(fd)
{
    m_cs.initHmacs();
}

/****************************************************************************/
/*!
 * \brief write data and update HMACs in parallel
 *
 * \iparam data - data to be written and hashed
 */
/****************************************************************************/
void WriteAndHmac::write(QByteArray data)
{
    mp_fd->write(data);
    m_cs.updateHMACs(data);
}

/****************************************************************************/
/*!
 * \brief return list of HMACs computed over written data
 *
 * \return Hmac collection
 */
/****************************************************************************/
QMap<QByteArray, QByteArray> WriteAndHmac::getHmacs()
{
    return m_cs.getHmacs();
}


}       // end namespace ImportExport
