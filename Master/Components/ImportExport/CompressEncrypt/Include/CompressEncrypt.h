/****************************************************************************/
/*! \file CompressEncrypt.h
 *
 *  \brief Compress and optionally encrypt data stream for ImportExport module
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

#ifndef IMPORTEXPORT_COMPRESSENCRYPT_H
#define IMPORTEXPORT_COMPRESSENCRYPT_H

#include <QByteArray>
#include "ImportExport/CryptoService/Include/CryptoService.h"

namespace ImportExport {

/****************************************************************************/
/*!
 * \brief compress and optionally encrypt data stream
 */
/****************************************************************************/
class CompressEncrypt
{
    friend class TestCompressEncrypt;

    public:
        CompressEncrypt(FailSafeOpen* fd,
                        CryptoService& cs,
                        bool encrypt = false, bool compressed = false);
        ~CompressEncrypt();
        void write(const QByteArray& data, bool hmac = true);

    private:
        void writeBuffer(QByteArray data);
        void close();

        // data
        FailSafeOpen* mp_fd; ///< file pointer
        CryptoService& m_cs; ///< crypto service
        bool m_encrypt;      ///< flag for encryption
        bool m_compressed;   ///< flag for compression
        QByteArray m_buffer; ///< buffer to store in the byte array
};

}       // end namespace ImportExport

#endif                  // IMPORTEXPORT_COMPRESSENCRYPT_H
