/****************************************************************************/
/*! \file Export/Components/ExportData/Include/Startup.h
 *
 *  \brief Startup definition.
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

#ifndef PLATFORMEXPORT_STARTUP_H
#define PLATFORMEXPORT_STARTUP_H

#include "ExportData/Include/ExportData.h"


namespace PlatformExport {

/****************************************************************************/
/*!
 * \brief This class handles the initialization for the CExportData class.
 */
/****************************************************************************/
class CStartup : public QObject
{
    Q_OBJECT

public:
    CStartup();

    int Archive();
    virtual ~CStartup();

private:
    //CStartup();
};

} // end namespace PlatformExport

#endif // PLATFORMEXPORT_STARTUP_H
