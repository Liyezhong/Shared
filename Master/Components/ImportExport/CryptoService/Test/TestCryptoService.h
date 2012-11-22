/****************************************************************************/
/** @file TestCryptoService.h
 *
 *  @brief Testclass for CryptoService
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-07-26
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
 *  last modified by owner: @(#) Aug 24 2011, 12:38:02
 *
 */
/****************************************************************************/

#ifndef IMPORT_EXPORT_TESTKEYMANAGER_H
#define IMPORT_EXPORT_TESTKEYMANAGER_H

#include <QtTest>
#include <QObject>
#include <QString>
#include <QList>
#include <QTime>
#include "ImportExport/CryptoService/Include/CryptoService.h"

namespace ImportExport {

struct KeyMaterial
{
    QByteArray Leicakey;
    QByteArray Viewerkey;
    QByteArray Importkey;
    qint32 counter;
    qint32 sdCounter;   // counter value stored on SD card
};

class TestCryptoService: public QObject
{
    Q_OBJECT

    public:
        TestCryptoService();           // default constructor

    private:
        void readKeyfile(KeyMaterial& keymat);
        

    private slots:
        void init();
        void cleanupTestCase();
        void utTestSingle();
        void utTestInitialKeys();
        void utTestHmacEmpty();
        void utTestHmacData();
        void utTestComputeKeyStart();
        void utTestComputeKeyBench();
        void utTestStepHashChain();
        void utTestHashChainReinit();
        void utTestEncryption();
};

}       // end namespace ImportExport

#endif          //IMPORT_EXPORT_TESTKEYMANAGER_H
