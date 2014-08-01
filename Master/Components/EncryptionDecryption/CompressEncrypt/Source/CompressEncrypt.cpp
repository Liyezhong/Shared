/****************************************************************************/
/*! \file CompressEncrypt.cpp
 *
 *  \brief Compress and optionally encrypt data stream for EncryptionDecryption module
 *
 *  Split the data stream to chunks.
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

#include "EncryptionDecryption/CompressEncrypt/Include/CompressEncrypt.h"

namespace EncryptionDecryption {
/****************************************************************************/
/*!
 * \brief constructor
 *
 * \iparam fd - pointer to FailSafeOpen instance, opened for write
 * \iparam cs - instance of CryptoService
 * \iparam encrypt - true if stream shall be encrypted
 * \iparam compressed - true if stream shall be compressed
 */
/****************************************************************************/
CompressEncrypt::CompressEncrypt(FailSafeOpen* fd,
                                 CryptoService& cs,
                                 bool encrypt, bool compressed):         //lint !e578 [Rw]
     mp_fd(fd), m_cs(cs), m_encrypt(encrypt), m_compressed(compressed),
     m_buffer(QByteArray())
{
    if(m_encrypt)
    {
        m_cs.initAES();
    }
}

/****************************************************************************/
/*!
 * \brief destructor
 */
/****************************************************************************/
CompressEncrypt::~CompressEncrypt()
{
    // The pointer mp_fd is not freed by other function as
    // warned by lint; I need a pointer to a QFile instance
    // here to write from inside the class. The pointer is
    // closed from outside when the archive is written.
    close();    //lint !e1551 [Rw]
                // no exception should ever occur here
}               //lint !e1579

/****************************************************************************/
/*!
 * \brief "flush" remaining bytes in buffer
 */
/****************************************************************************/
void CompressEncrypt::close()
{
    if(!m_buffer.isEmpty())
    {
        writeBuffer(m_buffer);
    }

    m_buffer.clear();
}

/****************************************************************************/
/*!
 * \brief collect data in buffer, compress/encrypt and write if filled
 *
 * All written data are also put to HMAC computation of the
 * CryptoService instance m_cs.
 *
 * \iparam data - data to be processed
 * \iparam hmac - update HMAC if true
 */
/****************************************************************************/
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

/****************************************************************************/
/*!
 * \brief compress/encrypt a buffer and write it
 *
 * \iparam data - possibly cut internal buffer
 */
/****************************************************************************/
void CompressEncrypt::writeBuffer(QByteArray data)
{
    if(data.isEmpty())
	{	
		return;
	}

    QByteArray compressed(data);
    // check the compress flag
    if (m_compressed)
    {
        compressed = qCompress(data);
    }

    compressed = General::int2byte(compressed.size()) + compressed;

    if(m_encrypt)
    {
        m_cs.encrypt(compressed);
    }

    mp_fd->write(compressed);
}


}   // end namespace EncryptionDecryption

