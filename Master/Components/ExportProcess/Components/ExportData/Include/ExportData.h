/****************************************************************************/
/*! \file ExportData.h
 *
 *  \brief Include file for ExportData class.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-12
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
 */
/****************************************************************************/
#ifndef EXPORTDATA_H
#define EXPORTDATA_H

#include "ImportExport/WriteArchive/Include/WriteArchive.h"



namespace PlatformExport {

typedef QHash<QString, QStringList> FileList_t; ///< to store the archive file name and related file list to archive
/****************************************************************************/
/**
 * \brief This class handles the archiving the files.
 */
/****************************************************************************/
class CExportData {
private:
    FileList_t PairList; ///< pair list for the strings

    /****************************************************************************/
    /*!
     *  \brief Copy Constructor
     */
    /****************************************************************************/
    CExportData(const CExportData &);

    /****************************************************************************/
    /*!
     *  \brief Assignment Operator which copies from rhs to lhs.
     */
    /****************************************************************************/
    const CExportData & operator = (const CExportData &);

public:
    CExportData();
    int CreateArchiveFiles();

};

} // end namespace PlatformExport


#endif // EXPORTDATA_H
