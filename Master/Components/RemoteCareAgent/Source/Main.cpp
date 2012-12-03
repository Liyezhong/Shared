/****************************************************************************/
/*! \file Main.cpp
 *
 *  \brief Main routine for RemoteCareAgent Component.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 04.07.2011
 *  $Author:    $ Y.Novak
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

#include <RemoteCareAgent/Include/AgentController.h>
#include <RemoteCareAgent/Include/Main.h>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/Utils.h>

#include <QThread>

/****************************************************************************/
/*! \brief This is a main loop of the Remote Care Embedded Agent */
/****************************************************************************/
int main(int argc, char *argv[])
{
    // create application object
    QCoreApplication App(argc, argv);

    // create controller for main thread
    RCAgentNamespace::AgentController MainController;

    // set directories.
    Global::SystemPaths::Instance().SetComponentTestPath("../Tests");
    Global::SystemPaths::Instance().SetFirmwarePath("../Firmware");
    Global::SystemPaths::Instance().SetLogfilesPath("../Logfiles");
    Global::SystemPaths::Instance().SetManualPath("../Manual");
    Global::SystemPaths::Instance().SetSettingsPath("../Settings");
    Global::SystemPaths::Instance().SetUpdatePath("../Update");
    Global::SystemPaths::Instance().SetUploadsPath("../Uploads");

    // call its Initialize method
    RCAgentNamespace::RCAgentInitCodes_t result = MainController.Initialize(argc, argv);
    if (result == RCAgentNamespace::RCAGENT_INIT_FAILED) {
        qDebug() << "RemoteCareAgent: MainController.Initialize() failed !";
        MainController.Cleanup();
        // exit with error code
        return Global::RETCODE_MASTERTHREAD_INIT;
    }

    // connect signal to close application when main controller stops
    if(!QObject::connect(&MainController, SIGNAL(SigOperationStopped()), &App, SLOT(quit()))) {
        qDebug() << "RemoteCareAgent: connecting MainController::SigOperationStopped() and App::quit() failed !";
        MainController.Cleanup();
        // exit with error code
        return Global::RETCODE_CONNECT_FAILED;
    }

    // start Main Controller execution
    if(!MainController.Start()) {
        qDebug() << "RemoteCareAgent: MainController.Start() failed !";
        MainController.Cleanup();
        // exit with error code
        return Global::RETCODE_MASTERTHREAD_INIT;
    }

    // start application
    int ReturnCode = App.exec();

    // cleanup main controller
    MainController.Cleanup();

    // and exit
    qDebug() << "RemoteCareAgent: exited with return code " << ReturnCode;
    return ReturnCode;
}
