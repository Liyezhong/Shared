/****************************************************************************/
/** @file TestHmac.h
 *
 *  @brief Testclass for HMAC computation with SHA-1.
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
 *  last modified by owner: @(#) Aug 24 2011, 12:40:54
 *
 */
/****************************************************************************/

#ifndef IMPORT_EXPORT_TESTHMAC_H
#define IMPORT_EXPORT_TESTHMAC_H

#include <QtTest>
#include <QObject>
#include <QString>
#include <QList>
#include <QTime>
#include "ImportExport/HMAC/Include/Hmac.h"

namespace ImportExport {

struct OneTest
{
    OneTest(const QByteArray k, const char *s, const char *d):
        key(k), data(s), hmac(d) {};
    OneTest(const QByteArray k, const QByteArray s, const char *d):
        key(k), data(s), hmac(d) {};

    QByteArray key;
    QByteArray data;
    QByteArray hmac;
};

class TestHmac: public QObject
{
    Q_OBJECT

    public:
        TestHmac();           // default constructor

    private:

    private slots:
        void utHmacTestVectors();
        void utHashTest();
        void utHmacBench();
};

}       // end namespace ImportExport

#endif          //IMPORT_EXPORT_TESTHMAC_H
