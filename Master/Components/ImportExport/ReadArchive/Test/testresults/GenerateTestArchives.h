/****************************************************************************/
/** @file GenerateTestArchives.h
 *
 *  @brief generate testfiles for ReadArchive
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-16
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
 *  last modified by owner: @(#) Aug 24 2011, 12:44:31
 *
 */
/****************************************************************************/

#ifndef IMPORT_EXPORT_GENERATE_TEST_ARCHIVES_H
#define IMPORT_EXPORT_GENERATE_TEST_ARCHIVES_H

#include <QString>
#include <QList>
#include <QTime>
#include "ImportExport/WriteArchive/Include/WriteArchive.h"

namespace ImportExport {

class GenerateTestArchives
{
    public:
        GenerateTestArchives();           // default constructor

    private:
        QList<QByteArray> m_workfiles;
        void createWorkfiles(QList<int> sizes);

        void init();
        void cleanup();
        void CreateNoFiles();
        void CreatePlainFiles();
        void CreatePlainBigFiles();
        void CreateEncryptFiles();
        void CreateEncryptBigFiles();
        void CreateEncryptManyFiles();
};

}       // end namespace ImportExport

#endif          //IMPORT_EXPORT_GENERATE_TEST_ARCHIVES_H
