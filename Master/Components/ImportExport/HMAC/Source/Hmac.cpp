/****************************************************************************/
/** @file Hmac.cpp
 *
 *  @brief implementing SHA-1 HMAC for ImportExport component
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-06-16
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
 *  last modified by owner: @(#) Aug 24 2011, 12:49:52
 *
 */
/****************************************************************************/

#include "ImportExport/HMAC/Include/Hmac.h"

namespace ImportExport {

/**
 * @brief Hmac constructor - initialize HMAC with key
 *
 * @param key - the secret key as QByteArray
 */

Hmac::Hmac(const QByteArray key):
    m_computed(false),
    m_innerhash(QCryptographicHash::Sha1),
    m_outerhash(QCryptographicHash::Sha1)
{
    // no special treatment for long keys here - will be 128 bit in practice
    Q_ASSERT(key.size() <= HASHBLOCKSIZE);

    char ipad[HASHBLOCKSIZE];
    char opad[HASHBLOCKSIZE];

    memset(ipad, 0x36, HASHBLOCKSIZE);
    memset(opad, 0x5c, HASHBLOCKSIZE);

    // XOR the key

    for(int i=0; i < key.size(); ++i)
    {
        ipad[i] ^= key[i];
        opad[i] ^= key[i];
    }

    // initialize inner and outer hash

    m_innerhash.addData(ipad, HASHBLOCKSIZE);
    m_outerhash.addData(opad, HASHBLOCKSIZE);
}

/**
 * @brief update HMAC computation with bytes from 'data'
 *
 * After a call to hmac() method, no update() may be called!
 *
 * @param data - data to be hashed
 */

void Hmac::update(const QByteArray data)
{
    Q_ASSERT(!m_computed);
    m_innerhash.addData(data);
}


/**
 * @brief return HMAC as QByteArray
 *
 * if already computed, return the previous result
 *
 * @return - HMAC value as QByteArray
 */

QByteArray Hmac::hmac()
{
    if(!m_computed)
    {
        m_outerhash.addData(m_innerhash.result());
        m_computed = true;
        m_result = m_outerhash.result();
    }
    return m_result;
}

/**
 * @brief SHA-1 hash function as service of HMAC class
 *
 * @param data - data to be hashed in one run
 * @return - hash digest as QByteArray
 */

QByteArray Hmac::hash(const QByteArray data)
{
    QCryptographicHash h(QCryptographicHash::Sha1);

    h.addData(data);
    return h.result();
}

}       // end namespace ImportExport
