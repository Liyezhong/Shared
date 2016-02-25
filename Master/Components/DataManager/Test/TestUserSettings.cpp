/****************************************************************************/
/*! \file TestUserSettings.cpp
 *
 *  \brief Implementation file for class TestUserSettings.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2011-05-27
 *  $Author:    $ J.Bugariu, Raju
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
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "DataManager/Containers/UserSettings/Include/UserSettingsVerifier.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdAlarmToneTest.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdChangeUserSettings.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdRmsOnOff.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdSetWaterStations.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdWaterStationUpdate.h"

namespace DataManager {

const QString RESOURCE_FILENAME = ":/Xml/UserSettings.xml"; ///< Resource file path

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFileUserSettings class.
 */
/****************************************************************************/
class TestUserSettings : public QObject {
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
     * \brief Test the user settings.
     */
    /****************************************************************************/
    void utTestUserSettings();

    /****************************************************************************/
    /**
     * \brief Test write and read of the user settings Xml.
     */
    /****************************************************************************/
    void utTestWriteReadUserSettingsInterface();

    /****************************************************************************/
    /**
     * \brief Test CmdUserSettings
     */
    /****************************************************************************/
    void utTestCmdUserSettings();
    /****************************************************************************/
    /**
     * \brief Test write and read of Sepia user settings
     */
    /****************************************************************************/
}; // end class TestUserSettings

/****************************************************************************/
void TestUserSettings::initTestCase() {
    // init languages
    //Global::InitSupportedLanguages();
}

/****************************************************************************/
void TestUserSettings::init() {
}

/****************************************************************************/
void TestUserSettings::cleanup() {
}

/****************************************************************************/
void TestUserSettings::cleanupTestCase() {
}

/****************************************************************************/
void TestUserSettings::utTestUserSettings() {

    CUserSettings *Settings1 = new CUserSettings();

    // change all the settings
    Settings1->SetValue("Agitation_Speed", 20);
    Settings1->SetValue("Leica_AgitationSpeed", 4);
    Settings1->SetDateFormat(Global::DATE_US);
    Settings1->SetLanguage(QLocale::English);
    Settings1->SetValue("Oven_StartMode", Global::OvenStartModeToString(Global::OVENSTART_BEFORE_PROGRAM));
    Settings1->SetValue("Oven_Temp", 42);
    Settings1->SetValue("Leica_OvenTemp", 45);
    Settings1->SetValue("RMS_State", Global::OnOffStateToString(Global::ONOFFSTATE_ON));
    Settings1->SetSoundLevelError(6);
    Settings1->SetSoundLevelWarning(7);
    Settings1->SetSoundNumberError(8);
    Settings1->SetSoundNumberWarning(9);
    Settings1->SetTimeFormat(Global::TIME_24);
    Settings1->SetValue("Loader_Reagent1", "L12");
    Settings1->SetValue("Loader_Reagent2", "U25");
    Settings1->SetValue("Loader_Reagent3", "L78");
    Settings1->SetValue("Loader_Reagent4", "U12");
    Settings1->SetValue("Loader_Reagent5", "U97");
    Settings1->SetRemoteCare(Global::ONOFFSTATE_ON);
    Settings1->SetDirectConnection(Global::ONOFFSTATE_OFF);
    Settings1->SetProxyUserName("Colorado");
    Settings1->SetProxyPassword("Colorado@1234");
    Settings1->SetProxyIPAddress("123.234.121.111");
    Settings1->SetProxyIPPort(8080);

    // now test settings
    QCOMPARE(Settings1->GetValue("Agitation_Speed").toInt(),      20);
    QCOMPARE(Settings1->GetValue("Leica_AgitationSpeed").toInt(), 4);
    QCOMPARE(Settings1->GetDateFormat(),          Global::DATE_US);
    QCOMPARE(Settings1->GetLanguage(),            QLocale::English);
    QCOMPARE(Global::StringToOvenStartMode(Settings1->GetValue("Oven_StartMode")),       Global::OVENSTART_BEFORE_PROGRAM);
    QCOMPARE(Settings1->GetValue("Oven_Temp").toInt(),            42);
    QCOMPARE(Settings1->GetValue("Leica_OvenTemp").toInt(),       45);
    QCOMPARE(Settings1->GetValue("Rms_State"),           Global::OnOffStateToString(Global::ONOFFSTATE_ON));
    QCOMPARE(Settings1->GetSoundLevelError(),     6);
    QCOMPARE(Settings1->GetSoundLevelWarning(),   7);
    QCOMPARE(Settings1->GetSoundNumberError(),    8);
    QCOMPARE(Settings1->GetSoundNumberWarning(),  9);
    QCOMPARE(Settings1->GetTimeFormat(),          Global::TIME_24);
    QCOMPARE(Settings1->GetValue("Loader_Reagent1"),      QString("L12"));
    QCOMPARE(Settings1->GetValue("Loader_Reagent2"),      QString("U25"));
    QCOMPARE(Settings1->GetValue("Loader_Reagent3"),      QString("L78"));
    QCOMPARE(Settings1->GetValue("Loader_Reagent4"),      QString("U12"));
    QCOMPARE(Settings1->GetValue("Loader_Reagent5"),      QString("U97"));
    QCOMPARE(Settings1->GetRemoteCare(), Global::ONOFFSTATE_ON);
    QCOMPARE(Settings1->GetDirectConnection(), Global::ONOFFSTATE_OFF);
    QCOMPARE(Settings1->GetProxyUserName(), QString("Colorado"));
    QCOMPARE(Settings1->GetProxyPassword(), QString("Colorado@1234"));
    QCOMPARE(Settings1->GetProxyIPAddress(), QString("123.234.121.111"));
    QCOMPARE(Settings1->GetProxyIPPort(), 8080);

    // Use copy constructor

    CUserSettings *Settings2 = new CUserSettings(*Settings1);


    // now test settings
    QCOMPARE(Settings1->GetValue("Agitation_Speed").toInt(),      Settings2->GetValue("Agitation_Speed").toInt());
    QCOMPARE(Settings1->GetValue("Leica_AgitationSpeed").toInt(), Settings2->GetValue("Leica_AgitationSpeed").toInt());
    QCOMPARE(Settings1->GetDateFormat(),          Settings2->GetDateFormat());
    QCOMPARE(Settings1->GetLanguage(),            Settings2->GetLanguage());
    QCOMPARE(Settings1->GetValue("Oven_StartMode"),       Settings2->GetValue("Oven_StartMode"));
    QCOMPARE(Settings1->GetValue("Oven_Temp").toInt(),            Settings2->GetValue("Oven_Temp").toInt());
    QCOMPARE(Settings1->GetValue("Leica_OvenTemp").toInt(),       Settings2->GetValue("Leica_OvenTemp").toInt());
    QCOMPARE(Settings1->GetValue("Rms_State"),            Settings2->GetValue("Rms_State"));
    QCOMPARE(Settings1->GetSoundLevelError(),     Settings2->GetSoundLevelError());
    QCOMPARE(Settings1->GetSoundLevelWarning(),   Settings2->GetSoundLevelWarning());
    QCOMPARE(Settings1->GetSoundNumberError(),    Settings2->GetSoundNumberError());
    QCOMPARE(Settings1->GetSoundNumberWarning(),  Settings2->GetSoundNumberWarning());
    QCOMPARE(Settings1->GetTimeFormat(),          Settings2->GetTimeFormat());
    QCOMPARE(Settings1->GetValue("Loader_Reagent1"),      Settings2->GetValue("Loader_Reagent1"));
    QCOMPARE(Settings1->GetValue("Loader_Reagent2"),      Settings2->GetValue("Loader_Reagent2"));
    QCOMPARE(Settings1->GetValue("Loader_Reagent3"),      Settings2->GetValue("Loader_Reagent3"));
    QCOMPARE(Settings1->GetValue("Loader_Reagent4"),      Settings2->GetValue("Loader_Reagent4"));
    QCOMPARE(Settings1->GetValue("Loader_Reagent5"),      Settings2->GetValue("Loader_Reagent5"));
    QCOMPARE(Settings1->GetRemoteCare(),  Settings2->GetRemoteCare());
    QCOMPARE(Settings1->GetDirectConnection(), Settings2->GetDirectConnection());
    QCOMPARE(Settings1->GetProxyUserName(), Settings2->GetProxyUserName());
    QCOMPARE(Settings1->GetProxyPassword(), Settings2->GetProxyPassword());
    QCOMPARE(Settings1->GetProxyIPAddress(), Settings2->GetProxyIPAddress());
    QCOMPARE(Settings1->GetProxyIPPort(), Settings2->GetProxyIPPort());

    // create the datastream and check whether copying data is working or not
    QByteArray* p_TempByteArray = new QByteArray();
    QDataStream DataStream(p_TempByteArray, QIODevice::ReadWrite);
    DataStream.setVersion(QDataStream::Qt_4_0);
    p_TempByteArray->clear();
    DataStream << *Settings2;
    DataStream.device()->reset();
    DataStream >> *Settings1;
    QCOMPARE(Settings2->GetProxyIPPort(), Settings1->GetProxyIPPort());
}


/****************************************************************************/
void TestUserSettings::utTestWriteReadUserSettingsInterface() {

    CUserSettingsInterface SettingsInterface;
    QString FilesPathWrite;

    // store the application path to write the test files
    FilesPathWrite = QCoreApplication::applicationDirPath() + "/";

    //QCOMPARE(SettingsInterface.Read(RESOURCE_FILENAME), true);

    SettingsInterface.SetDataVerificationMode(false);
    QCOMPARE(SettingsInterface.Read(":/Xml/UserSettings.xml"), true);
    SettingsInterface.SetDataVerificationMode(true);

    CUserSettings *Settings = new CUserSettings();
    Settings = SettingsInterface.GetUserSettings();

    // check all the values

    QCOMPARE(Settings->GetDateFormat(), Global::DATE_ISO);
    QCOMPARE(Settings->GetTimeFormat(), Global::TIME_24);
    QCOMPARE(Settings->GetTemperatureFormat(), Global::TEMP_FORMAT_CELSIUS);
    QCOMPARE(Settings->GetSoundNumberError(), 1);
    QCOMPARE(Settings->GetSoundNumberWarning(), 2);
    QCOMPARE(Settings->GetSoundLevelError(), 9);
    QCOMPARE(Settings->GetSoundLevelWarning(), 9);
    QCOMPARE(Settings->GetValue("Agitation_Speed").toInt(), 1);
    QCOMPARE(Settings->GetValue("Leica_AgitationSpeed").toInt(), 1);
    QCOMPARE(Settings->GetValue("Oven_StartMode"), Global::OvenStartModeToString(Global::OVENSTART_BEFORE_PROGRAM));
    QCOMPARE(Settings->GetValue("Oven_Temp").toInt(), 70);
    QCOMPARE(Settings->GetValue("Leica_OvenTemp").toInt(), 45);
    QCOMPARE(Settings->GetValue("Rms_State"), Global::OnOffStateToString(Global::ONOFFSTATE_ON));
    QCOMPARE(Settings->GetValue("Water_Type"), Global::WaterTypeToString(Global::WATER_TYPE_TAP));
    QCOMPARE(Settings->GetRemoteCare(),  Global::ONOFFSTATE_ON);
    QCOMPARE(Settings->GetDirectConnection(), Global::ONOFFSTATE_OFF);
    QCOMPARE(Settings->GetProxyUserName(), QString("Colorado"));
    QCOMPARE(Settings->GetProxyPassword(), QString("Colorado"));
    QCOMPARE(Settings->GetProxyIPAddress(), QString("123.145.046.60"));
    QCOMPARE(Settings->GetProxyIPPort(), 1234);

    // write the settings in to a file
    //SettingsInterface.Write("UserSettings_Test.xml");

    QCOMPARE(SettingsInterface.Write("UserSettings_Test.xml"), true);

    // change all the settings again
    Settings->SetValue("Agitation_Speed", 20);
    Settings->SetValue("Leica_AgitationSpeed", 2);
    Settings->SetDateFormat(Global::DATE_US);
    Settings->SetLanguage(QLocale::English);
    Settings->SetValue("Oven_StartMode", Global::OvenStartModeToString(Global::OVENSTART_BEFORE_PROGRAM));
    Settings->SetValue("Oven_Temp", 42);
    Settings->SetValue("Leica_OvenTemp", 47);
    Settings->SetValue("RMS_State", Global::OnOffStateToString(Global::ONOFFSTATE_ON));
    Settings->SetSoundLevelError(6);
    Settings->SetSoundLevelWarning(7);
    Settings->SetSoundNumberError(8);
    Settings->SetSoundNumberWarning(9);
    Settings->SetTimeFormat(Global::TIME_24);
    Settings->SetRemoteCare(Global::ONOFFSTATE_OFF);
    Settings->SetDirectConnection(Global::ONOFFSTATE_ON);
    Settings->SetProxyUserName("Colorado_Sepia");
    Settings->SetProxyPassword("Sepia@1234");
    Settings->SetProxyIPAddress("113.134.199.001");
    Settings->SetProxyIPPort(2380);

    // now test settings
    QCOMPARE(Settings->GetValue("Agitation_Speed").toInt(),      20);
    QCOMPARE(Settings->GetValue("Leica_AgitationSpeed").toInt(), 2);
    QCOMPARE(Settings->GetDateFormat(),          Global::DATE_US);
    QCOMPARE(Settings->GetLanguage(),            QLocale::English);
    QCOMPARE(Settings->GetValue("Oven_StartMode"),       Global::OvenStartModeToString(Global::OVENSTART_BEFORE_PROGRAM));
    QCOMPARE(Settings->GetValue("Oven_Temp").toInt(),            42);
    QCOMPARE(Settings->GetValue("Leica_OvenTemp").toInt(),       47);
    QCOMPARE(Settings->GetValue("Rms_State"),            Global::OnOffStateToString(Global::ONOFFSTATE_ON));
    QCOMPARE(Settings->GetSoundLevelError(),     6);
    QCOMPARE(Settings->GetSoundLevelWarning(),   7);
    QCOMPARE(Settings->GetSoundNumberError(),    8);
    QCOMPARE(Settings->GetSoundNumberWarning(),  9);
    QCOMPARE(Settings->GetTimeFormat(),          Global::TIME_24);
    QCOMPARE(Settings->GetRemoteCare(),  Global::ONOFFSTATE_OFF);
    QCOMPARE(Settings->GetDirectConnection(), Global::ONOFFSTATE_ON);
    QCOMPARE(Settings->GetProxyUserName(), QString("Colorado_Sepia"));
    QCOMPARE(Settings->GetProxyPassword(), QString("Sepia@1234"));
    QCOMPARE(Settings->GetProxyIPAddress(), QString("113.134.199.001"));
    QCOMPARE(Settings->GetProxyIPPort(), 2380);

    // now read settings from the file
    SettingsInterface.Read(FilesPathWrite + "UserSettings_Test.xml");

    Settings = SettingsInterface.GetUserSettings();
    // now test settings

    QCOMPARE(Settings->GetDateFormat(), Global::DATE_US);
    QCOMPARE(Settings->GetTimeFormat(), Global::TIME_24);
    QCOMPARE(Settings->GetTemperatureFormat(), Global::TEMP_FORMAT_CELSIUS);
    QCOMPARE(Settings->GetSoundNumberError(), 1);
    QCOMPARE(Settings->GetSoundNumberWarning(), 2);
    QCOMPARE(Settings->GetSoundLevelError(), 9);
    QCOMPARE(Settings->GetSoundLevelWarning(), 9);
    QCOMPARE(Settings->GetValue("Agitation_Speed").toInt(), 1);
    QCOMPARE(Settings->GetValue("Leica_AgitationSpeed").toInt(), 1);
    QCOMPARE(Settings->GetValue("Oven_StartMode"), Global::OvenStartModeToString(Global::OVENSTART_BEFORE_PROGRAM));
    QCOMPARE(Settings->GetValue("Oven_Temp").toInt(), 70);
    QCOMPARE(Settings->GetValue("Leica_OvenTemp").toInt(), 45);
    QCOMPARE(Settings->GetValue("Rms_State"), Global::OnOffStateToString(Global::ONOFFSTATE_ON));
    QCOMPARE(Settings->GetValue("Water_Type"), Global::WaterTypeToString(Global::WATER_TYPE_TAP));
    QCOMPARE(Settings->GetRemoteCare(),  Global::ONOFFSTATE_ON);
    QCOMPARE(Settings->GetDirectConnection(), Global::ONOFFSTATE_OFF);
    QCOMPARE(Settings->GetProxyUserName(), QString("Colorado"));
    QCOMPARE(Settings->GetProxyPassword(), QString("Colorado"));
    QCOMPARE(Settings->GetProxyIPAddress(), QString("123.145.046.60"));
    QCOMPARE(Settings->GetProxyIPPort(), 1234);

    // remove dummy file again
    QFile::remove(FilesPathWrite + "UserSettings_Test.xml");

}

/****************************************************************************/
void TestUserSettings::utTestCmdUserSettings()
{
    // Command AlarmTestTone
    MsgClasses::CmdAlarmToneTest *p_CmdAlarmToneTest = new MsgClasses::CmdAlarmToneTest();
    p_CmdAlarmToneTest->NAME = "MsgClasses::CmdAlarmToneTest";

    QCOMPARE(p_CmdAlarmToneTest->GetName(), QString(tr("MsgClasses::CmdAlarmToneTest")));


    MsgClasses::CmdAlarmToneTest *p_CmdAlarmToneTestParam = new MsgClasses::CmdAlarmToneTest(1000,(quint32)3,(quint32)4,Global::ALARM_WARNING);
    QCOMPARE(p_CmdAlarmToneTestParam->GetName(), QString(tr("MsgClasses::CmdAlarmToneTest")));
    delete p_CmdAlarmToneTestParam;
    delete p_CmdAlarmToneTest;

    //Command ChangeUserSettins
    MsgClasses::CmdChangeUserSettings *p_CmdChangeUserSettings = new MsgClasses::CmdChangeUserSettings();
    p_CmdChangeUserSettings->NAME = "MsgClasses::CmdChangeUserSettings";

    QCOMPARE(p_CmdChangeUserSettings->GetName(), QString(tr("MsgClasses::CmdChangeUserSettings")));

    delete p_CmdChangeUserSettings;


    //Command CmdWaterStationUpdate
    MsgClasses::CmdWaterStationUpdate *p_CmdWaterStationUpdate = new MsgClasses::CmdWaterStationUpdate();
    p_CmdWaterStationUpdate->NAME = "MsgClasses::CmdWaterStationUpdate";

    QCOMPARE(p_CmdWaterStationUpdate->GetName(), QString(tr("MsgClasses::CmdWaterStationUpdate")));

    MsgClasses::CmdWaterStationUpdate *p_CmdSetWaterStationsParam = new MsgClasses::CmdWaterStationUpdate(1000,"Id",false);
    QCOMPARE(p_CmdSetWaterStationsParam->GetName(), QString(tr("MsgClasses::CmdWaterStationUpdate")));
    delete p_CmdSetWaterStationsParam;
    delete p_CmdWaterStationUpdate;


}

} // end namespace DataManagement

QTEST_MAIN(DataManager::TestUserSettings)

#include "TestUserSettings.moc"
