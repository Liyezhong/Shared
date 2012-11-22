/****************************************************************************/
/** @file DecryptUncompress.h
 *
 *  @brief Compress and optionally encrypt data stream for ImportExport module
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
 *  last modified by owner: @(#) Aug 24 2011, 12:38:17
 *
 */
/****************************************************************************/

#ifndef IMPORT_DECRYPT_DECOMPRESS_H
#define IMPORT_DECRYPT_DECOMPRESS_H

#include <QByteArray>
#include "ImportExport/CryptoService/Include/CryptoService.h"

namespace ImportExport {

// compress and optionally encrypt data stream

class DecryptUncompress
{
    friend class TestDecryptUncompress;

    public:
        DecryptUncompress(FailSafeOpen* fd,
                        CryptoService& cs,
                        bool encrypt = false);
        QByteArray read(int size, bool hmac = true);

    private:
        // data
        FailSafeOpen* m_fd;
        CryptoService& m_cs;
        bool m_encrypt; ///< To store the encryption flag
        QByteArray m_buffer;

        // methods
        bool readNextChunk();
};

}       // end namespace ImportExport

#endif                  // IMPORT_DECRYPT_DECOMPRESS_H
