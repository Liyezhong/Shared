/****************************************************************************/
/** @file CryptoService.h
 *
 *  @brief CryptoService for ImportExport module
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-07-02
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
 *  last modified by owner: @(#) Aug 24 2011, 12:49:43
 *
 */
/****************************************************************************/

#ifndef IMPORT_EXPORT_CRYPTOSERVICE_H
#define IMPORT_EXPORT_CRYPTOSERVICE_H

#include <QtGlobal>
#include <QByteArray>
#include <QFile>
#include <QString>
#include <QMap>

#include "ImportExport/General/Include/General.h"
#include "ImportExport/HMAC/Include/Hmac.h"
#include "ImportExport/AES/Include/Aes.h"

namespace ImportExport {

class CryptoService
{
    friend class TestCryptoService;

    public:
        CryptoService(const QByteArray deviceID = QByteArray(),
                      const QByteArray keydata = QByteArray(),
                      int version = 0);
        ~CryptoService();

        void updateHMACs(QByteArray data);
        QMap<QByteArray, QByteArray> getHmacs();
        void initHmacs();
        void initAES();
        void encrypt(QByteArray& data);
        inline int getHashIndex() {return m_hashChainIndex;}
        void stepHashChain();
        static void computeKey(QByteArray& key,
                               const QByteArray& deviceID,
                               int index);

    private:
        // data

        // for computing C from B
        const QByteArray S0;

        // for computing An from Bn
        const QByteArray S1;

        QMap<QByteArray, QByteArray> m_keys;    ///< keys for HMAC instances
        QMap<QByteArray, Hmac*> m_hmac;         ///< HMAC instances
        AES m_aes;                     ///< AES instance for en/decryption
        int m_hashChainIndex;                   ///< index in hash chain
        bool m_aesInitialized;
        bool m_hmacInitialized;
};


/**
 * @brief helper class - compute HMACs while writing to from file
 */

class WriteAndHmac
{
    public:
        WriteAndHmac(CryptoService& cs, FailSafeOpen* fd);
        QByteArray read(int size);
        void write(QByteArray data);
        QMap<QByteArray, QByteArray> getHmacs();

    private:
        CryptoService m_cs;
        FailSafeOpen* m_fd;
};

}       // end namespace ImportExport

#endif          // IMPORT_EXPORT_CRYPTOSERVICE_H
