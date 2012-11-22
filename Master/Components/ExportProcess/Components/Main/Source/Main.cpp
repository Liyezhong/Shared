/****************************************************************************/
/*! \file Main.cpp
 *
 *  \brief Implementation file for main routine.
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

#include <Main/Include/Main.h>
#include "ExportData/Include/Startup.h"
#include <Global/Include/SystemPaths.h>
#include <QThread>



/****************************************************************************/
int main(int argc, char *argv[]) {
    // check the number of arguments
    if (argc > 0) {
        // create application object
        QCoreApplication App(argc, argv);
        // create the startup class object
        PlatformExport::CStartup Startup;

        // start application
        int ReturnCode = Startup.Archive();

        // and exit
        return ReturnCode;
    }
    return 1;

}
