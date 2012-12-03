/****************************************************************************/
/*! \file Hmac.cpp
 *
 *  \brief implementing SHA-1 HMAC for ImportExport component
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

#include "ImportExport/HMAC/Include/Hmac.h"
#include "ImportExport/General/Include/General.h"

namespace ImportExport {
/****************************************************************************/
/*!
 * \brief Hmac constructor - initialize HMAC with key
 *
 * \iparam key - the secret key as QByteArray
 */
/****************************************************************************/
Hmac::Hmac(const QByteArray key):
    m_computed(false),
    m_innerhash(QCryptographicHash::Sha1),
    m_outerhash(QCryptographicHash::Sha1)
{
    char ipad[HASHBLOCKSIZE];
    char opad[HASHBLOCKSIZE];

    // no special treatment for long keys here - will be 128 bit in practice
    if (!(key.size() <= HASHBLOCKSIZE))
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_KEY_SIZE_LESS);
    }

    memset(ipad, 0x36, HASHBLOCKSIZE);
    memset(opad, 0x5c, HASHBLOCKSIZE);

    // XOR the key
    for(int count = 0; count < key.size(); ++count)
    {
        ipad[count] ^= key[count];
        opad[count] ^= key[count];
    }

    // initialize inner and outer hash

    m_innerhash.addData(ipad, HASHBLOCKSIZE);
    m_outerhash.addData(opad, HASHBLOCKSIZE);
}
/****************************************************************************/
/*!
 * \brief update HMAC computation with bytes from 'data'
 *
 * After a call to hmac() method, no update() may be called!
 *
 * \iparam data - data to be hashed
 */
/****************************************************************************/
void Hmac::update(const QByteArray data)
{    
    if (m_computed)
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_HMAC_COMPUTATION_STARTED);
    }
    m_innerhash.addData(data);
}

/****************************************************************************/
/*!
 * \brief Add the inner hash result if the hash is not calculated
 *
 * \return - HMAC value as QByteArray
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 * \brief SHA-1 hash function as service of HMAC class
 *
 * \iparam data - data to be hashed in one run
 *
 * \return - hash digest as QByteArray
 */
/****************************************************************************/
QByteArray Hmac::hash(const QByteArray data)
{
    QCryptographicHash h(QCryptographicHash::Sha1);

    h.addData(data);
    return h.result();
}

}       // end namespace ImportExport
