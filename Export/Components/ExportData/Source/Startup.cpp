/****************************************************************************/
/*! \file Export/Components/ExportData/Source/Startup.cpp
 *
 *  \brief Startup implementation.
 *
 *   \b Description:
 *         Startup class for the Main and it creates the CExportData class to
 *         create archive files.
 *
 *   $Version: $ 1.0
 *   $Date:    $ 2012-07-12
 *   $Author:  $ Raju
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

#include "ExportData/Include/Startup.h"
#include <stdlib.h>


namespace Export {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CStartup::CStartup()
{

}

/****************************************************************************/
/*!
 *  \brief Calls the archive file
 *
 *  \return Error number or success
 */
/****************************************************************************/
int CStartup::Archive()
{
    // create the Export object and do the neccessary tasks and exit it
    CExportData ArchiveFile;
    // create archive files
    return ArchiveFile.CreateArchiveFiles();
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CStartup::~CStartup()
{    
}

} // end namespace Export
