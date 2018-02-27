/****************************************************************************/
/*! \file Main.cpp
 *
 *  \brief Main routine for RemoteCareAgent Component.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 29.04.2012
 *  $Author:    $ B. Stach
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

/// Global
#include <Global/Include/SystemPaths.h>
#include <Global/Include/Utils.h>
#include <Global/Include/SignalHandler.h>
/// Qt
#include <QThread>
/// Internal
#include "AgentController.h"

/****************************************************************************/
/*!
 * \brief This is a main loop of the Remote Care Embedded Agent
 * \iparam argc - number of arguments
 * \iparam argv - array of arguments
 * \return      - returns error code
 */
/****************************************************************************/
int main(int argc, char *argv[])
{
    // catch unexpected signals
    Global::SignalHandler signalHandler;
    QString ip = "";
    QString port = "";

    if (argc == 3) {
        ip   = static_cast<QString>(argv[1]);
        port = static_cast<QString>(argv[2]);
    }

    signalHandler.init();

    // create Qt Application for Top level event loop
    QCoreApplication App(argc, argv);

    // set global directories.
    Global::SystemPaths::Instance().SetComponentTestPath("../Tests");
    Global::SystemPaths::Instance().SetFirmwarePath("../Firmware");
    Global::SystemPaths::Instance().SetLogfilesPath("../Logfiles");
    Global::SystemPaths::Instance().SetManualPath("../Manual");
    Global::SystemPaths::Instance().SetSettingsPath("../Settings");
    Global::SystemPaths::Instance().SetUpdatePath("../Update");
    Global::SystemPaths::Instance().SetUploadsPath("../Uploads");
    Global::SystemPaths::Instance().SetTempPath("../Temporary");
    Global::SystemPaths::Instance().SetRollbackPath("../Rollback");
    Global::SystemPaths::Instance().SetTranslationsPath("../Translations");
    Global::SystemPaths::Instance().SetSoundPath("../Sounds");
    Global::SystemPaths::Instance().SetRemoteCarePath("../RemoteCare");

    // create controller for main thread
    RCAgentNamespace::AgentController MainController(ip, port);

    // call its Initialize method
    if (!MainController.Initialize()) {
        qDebug() << "RemoteCareAgent: MainController.Initialize() failed !";
        // exit with error code
        return Global::RETCODE_MASTERTHREAD_INIT;
    }

    // connect signal to close application when main controller stops
    CONNECTSIGNALSLOT(&MainController, SigOperationStopped(), &App, quit());

    // start Main Controller execution
    if(!MainController.Start()) {
        qDebug() << "RemoteCareAgent: MainController.Start() failed !";
        // exit with error code
        return Global::RETCODE_MASTERTHREAD_INIT;
    }

    // start application
    int ReturnCode = App.exec();

    // and exit
    qDebug() << "RemoteCareAgent: exited with return code " << ReturnCode;
    return ReturnCode;
}
