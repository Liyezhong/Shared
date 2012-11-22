/****************************************************************************/
/** @file TestAbstractFile.h
 *
 *  @brief Testclass for AbstractFile class
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
 *  last modified by owner: @(#) Aug 24 2011, 12:33:52
 *
 */
/****************************************************************************/

#ifndef IMPORT_EXPORT_TEST_ABSTRACT_FILE_H
#define IMPORT_EXPORT_TEST_ABSTRACT_FILE_H

#include <QtTest>
#include <QObject>
#include "ImportExport/AbstractFile/Include/AbstractFile.h"

namespace ImportExport {

class TestAbstractFile: public QObject
{
    Q_OBJECT

    public:
        TestAbstractFile();           // default constructor

    private:
        // data
        const QSet<QString> m_workfiles;

        // methods
        void writeFiles(AbstractFile* fp);
        QByteArray getFileContents(const QString filename);

    private slots:
        void init();
        void cleanup();
        void utTestPlainFiles();
        void utTestRAMFiles();
        void utTestVoidFiles();
};

}       // end namespace ImportExport

#endif          //IMPORT_EXPORT_TEST_ABSTRACT_FILE_H
