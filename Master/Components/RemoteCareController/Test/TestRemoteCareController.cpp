/****************************************************************************/
/*! \file TestRemoteCareController.cpp
 *
 *  \brief Implementation file for class TestRemoteCareController.
 *
 *  \b Description:
 *         Checks the CRemoteCareController class implementation
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-09-25
 *  $Author:    $ Ramya GJ
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

#include <QTest>
#include <QFile>
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "ExternalProcessController/Include/ExternalProcessController.h"
#include "RemoteCareController/Include/RemoteCareThreadController.h"
#include "NetCommands/Include/CmdRemoteCareState.h"


namespace RemoteCare {

/****************************************************************************/
/**
 * \brief Test class for CRemoteCareController class.
 */
/****************************************************************************/
class TestRemoteCareController : public QObject {
    Q_OBJECT
private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
     */
    /****************************************************************************/
    void init();
    /****************************************************************************/
    /**
     * \brief Called after each testfunction was executed.
     */
    /****************************************************************************/
    void cleanup();
    /****************************************************************************/
    /**
     * \brief Called after last testfunction was executed.
     */
    /****************************************************************************/
    void cleanupTestCase();

    /****************************************************************************/
    /**
     * \brief Test write and read Export configuration.
     */
    /****************************************************************************/
    void utTestRemoteCareController();

}; // end class TestRemoteCareController

/****************************************************************************/
void TestRemoteCareController::initTestCase() {

}

/****************************************************************************/
void TestRemoteCareController::init() {
}

/****************************************************************************/
void TestRemoteCareController::cleanup() {
}

/****************************************************************************/
void TestRemoteCareController::cleanupTestCase() {
}

/****************************************************************************/
void TestRemoteCareController::utTestRemoteCareController() {

    RemoteCareController *Controller = new RemoteCareController(5, "Axeda Client", NetworkBase::NSE_TYPE_AXEDA);


    try {
        Controller->OnPowerFail(Global::POWER_FAIL_REVERT);
        Controller->OnCmdTimeout(5, "Hi");
        qDebug() << "-----------------OnStopReceived()";
        Global::AckOKNOK Ack;
        Controller->OnCommandAckReceived(5, Ack);
    }
    catch (...) {
        // catch the exception here
    }

    try {
        Controller->CreateAndInitializeObjects();
        Global::tRefType Ref = 1;
        NetCommands::CmdRemoteCareState Cmd(15000, false, "RemoteCare");
        Controller->SendCmdToExternalProcess(Ref, Cmd);
    }
    catch (...) {
        // catch the exception here
    }

    try {
        qDebug() << "------------Controller->OnGoReceived()";
        Controller->OnGoReceived();
    }
    catch (...) {
        // catch the exception here
    }

    try {
        qDebug() << "------------Controller->OnReadyToWork()";
        Controller->OnReadyToWork();
    }
    catch (...) {
        // catch the exception here
    }

    try {
        qDebug() << "------------Controller->OnStopReceived()";
        Controller->OnStopReceived();
    }
    catch (...) {
        // catch the exception here
    }

    try {
        qDebug() << "------------Controller->OnStopWorking()";
        Controller->OnStopWorking();
    }
    catch (...) {
        // catch the exception here
    }

    try {
        qDebug() << "------------Controller->CleanupAndDestroyObjects()";
        Controller->CleanupAndDestroyObjects();
    }
    catch (...) {
        // catch the exception here
    }


    delete Controller;
}

} // end namespace Export

QTEST_MAIN(RemoteCare::TestRemoteCareController)

#include "TestRemoteCareController.moc"
