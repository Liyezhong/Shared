/****************************************************************************/
/*! \file TestREServerConnector.cpp
 *
 *  \brief Implementation file for class TestREServerConnector.
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

#include <QTest>
#include <QSignalSpy>

#include <TestREServerConnector.h>
#include <Global/Include/SystemPaths.h>

#include <REServerConnector.h>

namespace RCAgentNamespace {

/****************************************************************************/
/**
 * \brief Constructor
 */
/****************************************************************************/
TestREServerConnector::TestREServerConnector()
{

}

/****************************************************************************/
/**
 * \brief Destructor
 */
/****************************************************************************/
TestREServerConnector::~TestREServerConnector()
{

}

/****************************************************************************/
/**
 * \brief Called before first testfunction is executed.
 */
/****************************************************************************/
void TestREServerConnector::initTestCase() {
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
void TestREServerConnector::init() {
}

/****************************************************************************/
/**
 * \brief Test state change
 */
/****************************************************************************/
void TestREServerConnector::testChangeState() {
    REServerConnector recon;

    QVERIFY(recon.GetState() == RCAgentNamespace::AGENT_NOT_STARTED);

    recon.Initialize();
    QVERIFY(recon.GetState() == RCAgentNamespace::AGENT_IS_INITIALIZED);

    recon.Start();
    while (recon.GetState() == RCAgentNamespace::AGENT_IS_INITIALIZED){}
    QVERIFY(recon.GetState() == RCAgentNamespace::AGENT_IS_RUNNING);

    recon.Reconnect();
    QVERIFY(recon.GetState() == RCAgentNamespace::AGENT_RESTART);

    recon.Stop();
    sleep(recon.GetExecTime() + 5);
    QVERIFY(recon.GetState() == RCAgentNamespace::AGENT_STOPPED);
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 */
/****************************************************************************/
void TestREServerConnector::cleanup() {
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 */
/****************************************************************************/
void TestREServerConnector::cleanupTestCase() {
}

} // end namespace DataLogging

QTEST_MAIN(RCAgentNamespace::TestREServerConnector)
