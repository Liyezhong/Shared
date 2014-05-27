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

#ifndef EXPORT_STARTUP_H
#define EXPORT_STARTUP_H

#include "ExportData/Include/ExportData.h"


namespace Export {

/****************************************************************************/
/*!
 * \brief This class handles the initialization for the CExportData class.
 */
/****************************************************************************/
class CStartup
{
public:
    CStartup();

    int Archive();
    ~CStartup();

private:
    //CStartup();
    //int temp_for_warrnning;
};

} // end namespace Export

#endif // EXPORT_STARTUP_H
