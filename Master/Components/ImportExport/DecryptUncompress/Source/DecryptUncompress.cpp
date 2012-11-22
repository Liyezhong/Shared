/****************************************************************************/
/** @file DecryptUncompress.cpp
 *
 *  @brief Optionally decrypt and uncompress data stream for ImportExport module
 *
 *  The data stream is split to chunks; join the chunks.
 *  HMACs are optionally computed of returned data.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-12
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
 *  last modified by owner: @(#) Aug 16 2011, 12:04:12
 *
 */
/****************************************************************************/

#include "ImportExport/DecryptUncompress/Include/DecryptUncompress.h"

namespace ImportExport {

/**
 * @brief constructor
 *
 * @param fd - pointer to FailSafeOpen instance, opened for read
 * @param cs - instance of CryptoService
 * @param encrypt - true if stream shall be encrypted
 */

DecryptUncompress::DecryptUncompress(FailSafeOpen* fd,
                                 CryptoService& cs,
                                 bool encrypt): //lint !e578 [Rw]
     m_fd(fd), m_cs(cs), m_encrypt(encrypt),
     m_buffer(QByteArray())
{
    if(encrypt)
    {
        m_cs.initAES();
    }
}


/**
 * @brief return next 'size' bytes
 *
 * All read data are also put to HMAC computation of the
 * CryptoService instance m_cs.
 *
 * If not enough bytes can be read, an ImexException is thrown.
 *
 * @param size - record length to be read in
 * @param hmac - update HMAC if true
 * @return read bytes as QByteArray
 */

QByteArray DecryptUncompress::read(int size, bool hmac)
{
    Q_ASSERT(size > 0);
    QByteArray ret;

    while(m_buffer.size() < size)
    {
        if(!readNextChunk())
        {
            THROW("EOF reached during read");
        }
    }

    ret = m_buffer.left(size);
    m_buffer = m_buffer.mid(size);

    if(hmac)
    {
        m_cs.updateHMACs(ret);
    }

    return ret;
}


/**
 * @brief decrypt/uncompress a chunk and append it to m_buffer
 *
 * Throw ImexException if not enough bytes can be read or a decompression
 * error occurs.
 *
 * @return - false if end of file was reached, true else
 *
 */

bool DecryptUncompress::readNextChunk()
{
    QByteArray buf = m_fd->read(4);

    if(buf.isEmpty())
    {
        return false;
    }

    if(m_encrypt)
    {
        m_cs.encrypt(buf);
    }

    int lg = General::byte2int(buf.data());

    // factor 4 is heuristic
    Q_ASSERT(lg > 0 && lg < 4*Constants::COMPR_ENCR_BUFSIZE);

    buf = m_fd->read(lg);
    if(buf.size() < lg)
    {
        THROW("incomplete chunk on read");
    }

    if(m_encrypt)
    {
        m_cs.encrypt(buf);
    }

    buf = qUncompress(buf);

    if(buf.isEmpty())
    {
        THROW("uncompression error on read");
    }

    m_buffer += buf;

    return true;
}

}       // end namespace ImportExport

