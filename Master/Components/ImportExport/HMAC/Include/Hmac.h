/****************************************************************************/
/*! \file Hmac.h
 *
 *  \brief header for implementing SHA-1 HMAC for ImportExport component
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

#ifndef IMPORTEXPORT_HMAC_H
#define IMPORTEXPORT_HMAC_H

#include <string>
#include <QtGlobal>
#include <QByteArray>
#include <QCryptographicHash>

namespace ImportExport {
/****************************************************************************/
/*!
 * \brief HMAC computation with SHA-1 digest according to RFC2104
 */
/****************************************************************************/
class Hmac
{
    public:
        Hmac(const QByteArray key);
        void update(const QByteArray data);
        QByteArray hmac();
        static QByteArray hash(const QByteArray data);

    private:
        static const int HASHBLOCKSIZE = 64;    ///< size of hash block

        bool m_computed; ///< flag for the computation of the hmac
        QCryptographicHash m_innerhash; ///< inner cryptographic hash
        QCryptographicHash m_outerhash; ///< outer cryptographic hash
        QByteArray m_result; ///< to store the result
};

}       // end namespace ImportExport

#endif  // IMPORTEXPORT_HMAC_H
