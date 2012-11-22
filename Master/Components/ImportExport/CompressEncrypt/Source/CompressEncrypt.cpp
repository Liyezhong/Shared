/****************************************************************************/
/** @file CompressEncrypt.cpp
 *
 *  @brief Compress and optionally encrypt data stream for ImportExport module
 *
 *  Split the data stream to chunks.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-07-30
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
 *  last modified by owner: @(#) Aug 31 2011, 14:21:58
 *
 */
/****************************************************************************/

#include "ImportExport/CompressEncrypt/Include/CompressEncrypt.h"

namespace ImportExport {

/**
 * @brief constructor
 *
 * @param fd - pointer to FailSafeOpen instance, opened for write
 * @param cs - instance of CryptoService
 * @param encrypt - true if stream shall be encrypted
 */

CompressEncrypt::CompressEncrypt(FailSafeOpen* fd,
                                 CryptoService& cs,
                                 bool encrypt):         //lint !e578 [Rw]
     m_fd(fd), m_cs(cs), m_encrypt(encrypt),
     m_buffer(QByteArray())
{
    if(m_encrypt)
    {
        m_cs.initAES();
    }
}


/**
 * @brief destructor
 */

CompressEncrypt::~CompressEncrypt()
{
    close();    //lint !e1551 [Rw]
                // no exception should ever occur here
}               //lint !e1579
                // The pointer m_fd is not freed by other function as
                // warned by lint; I need a pointer to a QFile instance
                // here to write from inside the class. The pointer is
                // closed from outside when the archive is written.

/**
 * @brief "flush" remaining bytes in buffer
 */

void CompressEncrypt::close()
{
    if(!m_buffer.isEmpty())
    {
        writeBuffer(m_buffer);
    }

    m_buffer.clear();
}

/**
 * @brief collect data in buffer, compress/encrypt and write if filled
 *
 * All written data are also put to HMAC computation of the
 * CryptoService instance m_cs.
 *
 * @param data - data to be processed
 * @param hmac - update HMAC if true
 */

void CompressEncrypt::write(const QByteArray& data, bool hmac)
{
    if(hmac)
    {
        m_cs.updateHMACs(data);
    }

    m_buffer += data;

    while(m_buffer.size() >= Constants::COMPR_ENCR_BUFSIZE)
    {
        writeBuffer(m_buffer.mid(0, Constants::COMPR_ENCR_BUFSIZE));
        m_buffer = m_buffer.mid(Constants::COMPR_ENCR_BUFSIZE);
    }
}


/**
 * @brief compress/encrypt a buffer and write it
 *
 * @param data - possibly cut internal buffer
 */

void CompressEncrypt::writeBuffer(QByteArray data)
{
    if(data.isEmpty()) return;

    QByteArray compressed = qCompress(data);

    compressed = General::int2byte(compressed.size()) + compressed;

    if(m_encrypt)
    {
        m_cs.encrypt(compressed);
    }

    m_fd->write(compressed);
}


}       // end namespace ImportExport

