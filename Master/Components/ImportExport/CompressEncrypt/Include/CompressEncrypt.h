/****************************************************************************/
/** @file CompressEncrypt.h
 *
 *  @brief Compress and optionally encrypt data stream for ImportExport module
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
 *  last modified by owner: @(#) Aug 24 2011, 12:34:12
 *
 */
/****************************************************************************/

#ifndef IMPORT_COMPRESS_ENCRYPT_H
#define IMPORT_COMPRESS_ENCRYPT_H

#include <QByteArray>
#include "ImportExport/CryptoService/Include/CryptoService.h"

namespace ImportExport {

// compress and optionally encrypt data stream

class CompressEncrypt
{
    friend class TestCompressEncrypt;

    public:
        CompressEncrypt(FailSafeOpen* fd,
                        CryptoService& cs,
                        bool encrypt = false);
        ~CompressEncrypt();
        void write(const QByteArray& data, bool hmac = true);

    private:
        void writeBuffer(QByteArray data);
        void close();

        // data
        FailSafeOpen* m_fd;
        CryptoService& m_cs;
        bool m_encrypt;
        QByteArray m_buffer;
};

}       // end namespace ImportExport

#endif                  // IMPORT_COMPRESS_ENCRYPT_H
