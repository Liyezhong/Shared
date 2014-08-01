/****************************************************************************/
/*! \file CryptoService.h
 *
 *  \brief CryptoService for EncryptionDecryption module
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

#ifndef ENCRYPTIONDECRYPTION_CRYPTOSERVICE_H
#define ENCRYPTIONDECRYPTION_CRYPTOSERVICE_H

#include <QtGlobal>
#include <QByteArray>
#include <QFile>
#include <QString>
#include <QMap>

#include "EncryptionDecryption/General/Include/General.h"
#include "EncryptionDecryption/HMAC/Include/Hmac.h"
#include "EncryptionDecryption/AES/Include/Aes.h"

namespace EncryptionDecryption {


/****************************************************************************/
/*!
 * \brief Service class for the cryptography
 */
/****************************************************************************/
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
        /****************************************************************************/
        /*!
         * \brief get the index in hask chain
         *
         * \return index in hask chain
         */
        /****************************************************************************/
        inline int getHashIndex() {return m_hashChainIndex;}
        void stepHashChain();
        static void computeKey(QByteArray& key,
                               const QByteArray& deviceID,
                               int index);

    private:
        // data

        // for computing C from B
        const QByteArray S0;                    //!< to store the key 0

        // for computing An from Bn
        const QByteArray S1;                    //!< to store the key 1

        QMap<QByteArray, QByteArray> m_keys;    //!< keys for HMAC instances
        QMap<QByteArray, Hmac*> m_hmac;         //!< HMAC instances
        AES m_aes;                     			//!< AES instance for en/decryption
        int m_hashChainIndex;                   //!< index in hash chain
        bool m_aesInitialized;                  //!< flag for aes initialization
        bool m_hmacInitialized;                 //!< flag for hmac initialization
};

/****************************************************************************/
/*!
 * \brief helper class - compute HMACs while writing to from file
 */
/****************************************************************************/
class WriteAndHmac
{
    public:
        WriteAndHmac(CryptoService& cs, FailSafeOpen* fd);
        /****************************************************************************/
        /*!
         * \brief read data and update HMACs in parallel
         *
         * \iparam size - data size to be read
         *
         * \return The data byte array read
         */
        /****************************************************************************/
        QByteArray read(int size);
        void write(QByteArray data);
        QMap<QByteArray, QByteArray> getHmacs();

    private:
        CryptoService m_cs; //!< instance for cryptoservice
        FailSafeOpen* mp_fd; //!< instance for the file
};

}       // end namespace EncryptionDecryption

#endif          // ENCRYPTION_DECRYPTION_CRYPTOSERVICE_H
