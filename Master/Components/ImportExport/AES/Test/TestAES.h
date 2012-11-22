/****************************************************************************/
/** @file TestAES.h
 *
 *  @brief Testclass for AES - variable key and random data tests.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-06-17
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
 *  last modified by owner: @(#) Aug 24 2011, 12:32:15
 *
 */
/****************************************************************************/

#ifndef IMPORT_EXPORT_TESTAES_H
#define IMPORT_EXPORT_TESTAES_H

#include <QtTest>
#include <QObject>
#include <QTime>
#include "ImportExport/AES/Include/Aes.h"

namespace ImportExport {

typedef struct
{
    const char *key; const char *ciphertext;
} VARKEYTEST;

typedef struct
{
    const char *key;
    const char *plain;
    const char *ciphertext;
} RANDTEST;

extern VARKEYTEST VarData[];
extern RANDTEST RandData[];
extern int VarSize;
extern int RandSize;

/**
 * @brief test class for AES - variable key and random data test
 */

class TestAes: public QObject
{
    Q_OBJECT

    public:
        TestAes();           // default constructor

    private:

    private slots:
        void utAesVariableKeyTest();
        void utAesRandomDataTest();
        void utAesCtrSimpleTest();
        void utAesBenchTest();
};

}       // end namespace ImportExport

#endif          //IMPORT_EXPORT_TESTAES_H
