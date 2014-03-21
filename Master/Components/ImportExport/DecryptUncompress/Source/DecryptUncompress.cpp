/****************************************************************************/
/*! \file DecryptUncompress.cpp
 *
 *  \brief Optionally decrypt and uncompress data stream for ImportExport module
 *
 *  The data stream is split to chunks; join the chunks.
 *  HMACs are optionally computed of returned data.
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

#include "ImportExport/DecryptUncompress/Include/DecryptUncompress.h"

namespace ImportExport {
/****************************************************************************/
/*!
 * \brief constructor
 *
 * \iparam fd - pointer to FailSafeOpen instance, opened for read
 * \iparam cs - instance of CryptoService
 * \iparam encrypt - true if stream shall be encrypted
 * \iparam compressed - true if stream shall be compressed
 */
/****************************************************************************/
DecryptUncompress::DecryptUncompress(FailSafeOpen* fd,
                                 CryptoService& cs,
                                 bool encrypt, bool compressed): //lint !e578 [Rw]
     mp_fd(fd), m_cs(cs), m_encrypt(encrypt), m_compressed(compressed),
     m_buffer(QByteArray())
{
    if(encrypt)
    {
        m_cs.initAES();
    }
}

/****************************************************************************/
/*!
 * \brief return next 'size' bytes
 *
 * All read data are also put to HMAC computation of the
 * CryptoService instance m_cs.
 *
 * If not enough bytes can be read, an ImexException is thrown.
 *
 * \iparam size - record length to be read in
 * \iparam hmac - update HMAC if true
 *
 * \return read bytes as QByteArray
 */
/****************************************************************************/
QByteArray DecryptUncompress::read(int size, bool hmac)
{    
    QByteArray ret;

    while(m_buffer.size() < size)
    {
        if(!readNextChunk())
        {
            THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_EOF_REACHED);
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

/****************************************************************************/
/*!
 * \brief decrypt/uncompress a chunk and append it to m_buffer
 *
 * Throw ImexException if not enough bytes can be read or a decompression
 * error occurs.
 *
 * \return - false if end of file was reached, true else
 *
 */
/****************************************************************************/
bool DecryptUncompress::readNextChunk()
{
    QByteArray buf = mp_fd->read(4);

    if(buf.isEmpty())
    {
        return false;
    }

    if(m_encrypt)
    {
        m_cs.encrypt(buf);
    }

    int lg = General::byte2int(buf.data());

    buf = mp_fd->read(lg);
    if(buf.size() < lg)
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_INCOMPLETE_CHUNK);
    }

    if(m_encrypt)
    {
        m_cs.encrypt(buf);
    }

    // check the compressed flag
    if (m_compressed) {
        buf = qUncompress(buf);
    }

    if(buf.isEmpty())
    {
        THROWEXCEPTIONNUMBER(ERROR_IMPORTEXPORT_UNCOMPRESSION_ERROR);
    }

    m_buffer += buf;

    return true;
}

}       // end namespace ImportExport

