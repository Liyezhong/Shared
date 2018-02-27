/****************************************************************************/
/*! \file TestRCConfigurationWrapper.cpp
 *
 *  \brief Implementation file for class TestRCConfigurationWrapper.
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

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/RemoteCareTypes.h>
#include <RCConfigurationWrapper.h>

#include <AeTypes.h>

#include "TestRCConfigurationWrapper.h"

namespace RCAgentNamespace {

/****************************************************************************/
/**
 * \brief Constructor
 */
/****************************************************************************/
TestRCConfigurationWrapper::TestRCConfigurationWrapper()
{

}

/****************************************************************************/
/**
 * \brief Destructor
 */
/****************************************************************************/
TestRCConfigurationWrapper::~TestRCConfigurationWrapper()
{

}

/****************************************************************************/
/**
 * \brief Called before first testfunction is executed.
 */
/****************************************************************************/
void TestRCConfigurationWrapper::initTestCase() {
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
void TestRCConfigurationWrapper::init() {
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 */
/****************************************************************************/
void TestRCConfigurationWrapper::cleanup() {
}

/****************************************************************************/
/**
 * \brief Test for default attribute setup
 */
/****************************************************************************/
void TestRCConfigurationWrapper::testDefaultParameter() {
    RCConfigurationWrapper confWrapper;

    RCConfig_t configuration = confWrapper.GetConfiguration();

    QCOMPARE(configuration.queueSize, static_cast<long>(2000000));
    QCOMPARE(configuration.httpPersist, AeTrue);
    QCOMPARE(configuration.httpEncryptionLevel, AeWebCryptoMedium);

    QCOMPARE(confWrapper.sessionName, static_cast<std::string>(""));
    QCOMPARE(configuration.nameRemoteSession, "");

    QCOMPARE(confWrapper.sessionIPAddr, static_cast<std::string>(""));
    QCOMPARE(configuration.hostRemoteSession, "");

    QCOMPARE(confWrapper.GetDownloadFolder(), static_cast<std::string>("../RemoteCare/"));
}

/****************************************************************************/
/**
 * \brief Test for reading settings
 */
/****************************************************************************/
void TestRCConfigurationWrapper::testReadSettings() {
    RCConfigurationWrapper confWrapper;
    RCConfig_t configuration;

    QVERIFY(confWrapper.ReadSettings() == true);

    configuration = confWrapper.GetConfiguration();

    QCOMPARE(configuration.queueSize, static_cast<long>(1000000));
    QCOMPARE(configuration.httpPersist, AeFalse);
    QCOMPARE(configuration.httpEncryptionLevel, AeWebCryptoHigh);

    QCOMPARE(confWrapper.sessionName, static_cast<std::string>("ColoradoTest"));
    QCOMPARE(configuration.nameRemoteSession, "ColoradoTest");
    QCOMPARE(configuration.modelNumber, "SPECTRAST");

    QCOMPARE(confWrapper.GetDownloadFolder(), static_cast<std::string>("../RemoteCareTest/"));

    RemoteCare::RCDataItemType_t type;
    QVERIFY(confWrapper.GetDataItemType(RemoteCare::RC_DATAITEM_SET_DOWNLOAD_FINISHED, type));

    QCOMPARE(type, static_cast<RemoteCare::RCDataItemType_t>(AeDRMDataDigital));


}

/****************************************************************************/
/**
 * \brief Test for set settings
 */
/****************************************************************************/
void TestRCConfigurationWrapper::testSetSettings() {
    RCConfigurationWrapper confWrapper;

    //! read default should fail because empty strings
    QVERIFY(confWrapper.SetSettings() == false);

    //! read configuration
    QVERIFY(confWrapper.ReadSettings() == true);
    QVERIFY(confWrapper.SetSettings() == true);
}

/****************************************************************************/
/**
 * \brief Test string conversion to aechar
 */
/****************************************************************************/
void TestRCConfigurationWrapper::testStdStringToAeChar() {
    RCConfigurationWrapper confWrapper;

    const QString teststr1 = "Test";
    const AeChar *teststr2 = "Test";

    QCOMPARE(teststr2, confWrapper.StdStringToAeChar(teststr1.toStdString()));
}

/****************************************************************************/
/**
 * \brief Test enum onoffstate conversion to bool
 */
/****************************************************************************/
void TestRCConfigurationWrapper::testONOFFToAeBool() {
    RCConfigurationWrapper confWrapper;

    QCOMPARE(false, confWrapper.ONOFFToAeBool(Global::ONOFFSTATE_OFF));
    QCOMPARE(true, confWrapper.ONOFFToAeBool(Global::ONOFFSTATE_ON));
}

/****************************************************************************/
/**
 * \brief Test bool conversion to enum onoffstate
 */
/****************************************************************************/
void TestRCConfigurationWrapper::testAeBoolToONOFF() {
    RCConfigurationWrapper confWrapper;

    QCOMPARE(Global::ONOFFSTATE_OFF, confWrapper.AeBoolToONOFF(false));
    QCOMPARE(Global::ONOFFSTATE_ON, confWrapper.AeBoolToONOFF(true));
}

/****************************************************************************/
/**
 * \brief Test time conversion
 */
/****************************************************************************/
void TestRCConfigurationWrapper::testConvertTime() {
    RCConfigurationWrapper confWrapper;

    QString qstrtime = "19.08.2013 15:38:22";

    AeTimeValue timeval1, timeval2;
    timeval1.iSec        = 1376919502;
    timeval1.iMicroSec   = 0;

    confWrapper.ConvertTime(qstrtime, &timeval2);

//    QCOMPARE(timeval1.iSec, timeval2.iSec);
    QCOMPARE(timeval1.iMicroSec, timeval2.iMicroSec);
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 */
/****************************************************************************/
void TestRCConfigurationWrapper::cleanupTestCase() {

}

} // end namespace DataLogging

QTEST_MAIN(RCAgentNamespace::TestRCConfigurationWrapper)
