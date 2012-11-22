/****************************************************************************/
/** @file Hmac.h
 *
 *  @brief header for implementing SHA-1 HMAC for ImportExport component
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-06-16
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
 *  last modified by owner: @(#) Aug 12 2011, 18:26:46
 *
 */
/****************************************************************************/

#ifndef IMPORT_EXPORT_HMAC_H
#define IMPORT_EXPORT_HMAC_H

#include <string>
#include <QtGlobal>
#include <QByteArray>
#include <QCryptographicHash>

namespace ImportExport {

/**
 * @brief HMAC computation with SHA-1 digest according to RFC2104
 */

class Hmac
{
    public:
        Hmac(const QByteArray key);
        void update(const QByteArray data);
        QByteArray hmac();
        static QByteArray hash(const QByteArray data);

    private:
        static const int HASHBLOCKSIZE = 64;

        bool m_computed;
        QCryptographicHash m_innerhash;
        QCryptographicHash m_outerhash;
        QByteArray m_result;
};

}       // end namespace ImportExport

#endif  // IMPORT_EXPORT_HMAC_H
