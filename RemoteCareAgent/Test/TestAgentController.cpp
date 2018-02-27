/****************************************************************************/
/*! \file TestAgentController.cpp
 *
 *  \brief Implementation file for class TestAgentController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-07-10
 *  $Author:    $ B.Stach
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

#include <QThread>
#include <QTest>
#include <TestAgentController.h>
#include <Global/Include/SystemPaths.h>
#include <QSignalSpy>

namespace RCAgentNamespace {

/****************************************************************************/
/**
 * \brief Constructor
 */
/****************************************************************************/
TestAgentController::TestAgentController()
{

}

/****************************************************************************/
/**
 * \brief Destructor
 */
/****************************************************************************/
TestAgentController::~TestAgentController()
{

}

/****************************************************************************/
/**
 * \brief Called before first testfunction is executed.
 */
/****************************************************************************/
void TestAgentController::initTestCase() {
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
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 */
/****************************************************************************/
void TestAgentController::init() {
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 */
/****************************************************************************/
void TestAgentController::cleanup() {
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 */
/****************************************************************************/
void TestAgentController::cleanupTestCase() {
}

/****************************************************************************/
/**
 * \brief Test the contructor of the class
 */
/****************************************************************************/
void TestAgentController::testConstructor() {
    RCAgentNamespace::AgentController mainController(0, NULL);

    QVERIFY(mainController.initStatus == RCAgentNamespace::RCAGENT_INIT_FAILED);

    QVERIFY(mainController.Initialize() == true);

    QVERIFY(mainController.initStatus == RCAgentNamespace::RCAGENT_INITIALIZED_OK);

    mainController.ExitNow();
}

/****************************************************************************/
/**
 * \brief Test the contructor of the class
 */
/****************************************************************************/
void TestAgentController::testSignalsAndSlots() {
    RCAgentNamespace::AgentController mainController(0,NULL);

    QVERIFY(mainController.Initialize() == true);

    QSignalSpy spy(&mainController, SIGNAL(SigOperationStopped()));
    QSignalSpy spy2(&mainController, SIGNAL(SigStopRemoteCareAgent()));
    QSignalSpy spy3(&mainController, SIGNAL(SigAckToMaster(const Global::tRefType&, const bool &)));

    QVERIFY(mainController.Start() == true);

    mainController.OnReconnect(1);
    QCOMPARE(spy3.count(),1);

    QList<QVariant> args = spy3.takeFirst();

    bool ok;

    QVERIFY(args.at(0).toInt(&ok) == 1);
    QVERIFY(args.at(1).toBool()   == true);

    sleep(10);

    mainController.ExitNow();

    QCOMPARE(spy.count(),1);
    QCOMPARE(spy2.count(),1);
}

} // end namespace TestAgentController

QTEST_MAIN(RCAgentNamespace::TestAgentController)
