/****************************************************************************/
/*! \file DecryptUncompress.h
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

#ifndef IMPORTEXPORT_DECRYPTUNCOMPRESS_H
#define IMPORTEXPORT_DECRYPTUNCOMPRESS_H

#include <QByteArray>
#include "ImportExport/CryptoService/Include/CryptoService.h"

namespace ImportExport {

/****************************************************************************/
/*!
 * \brief compress and optionally encrypt data stream
 */
/****************************************************************************/
class DecryptUncompress
{
    friend class TestDecryptUncompress;

    public:
        DecryptUncompress(FailSafeOpen* fd,
                        CryptoService& cs,
                        bool encrypt = false, bool compressed = false);
        QByteArray read(int size, bool hmac = true);

    private:
        // data
        FailSafeOpen* mp_fd; ///< To open the files
        CryptoService& m_cs; ///< for crypto service
        bool m_encrypt; ///< To store the encryption flag
        bool m_compressed; ///< To store the compressed flag
        QByteArray m_buffer; ///< buffer to store the data

        // methods
        bool readNextChunk();
};

}       // end namespace ImportExport

#endif                  // IMPORTEXPORT_DECRYPTUNCOMPRESS_H
