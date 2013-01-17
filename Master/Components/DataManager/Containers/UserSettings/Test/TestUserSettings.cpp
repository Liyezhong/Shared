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
     * \brief Test write and read user settings of the verifier.
     */
    /****************************************************************************/
    void utTestWriteReadUserSettingsVerifier();
}; // end class TestUserSettings

/****************************************************************************/
void TestUserSettings::initTestCase() {    
    // init languages
    Global::InitSupportedLanguages();
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
    //SettingsInterface.UpdateUserSettings(Settings);

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
    CUserSettings *Settings2(Settings1);


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
}


/****************************************************************************/
void TestUserSettings::utTestWriteReadUserSettingsInterface() {

    CUserSettingsInterface SettingsInterface;
    QString FilesPathWrite;

    // store the application path to write the test files
    FilesPathWrite = QCoreApplication::applicationDirPath() + "/";

    QCOMPARE(SettingsInterface.Read(RESOURCE_FILENAME), true);

    SettingsInterface.SetDataVerificationMode(false);
    QCOMPARE(SettingsInterface.Read(RESOURCE_FILENAME), true);
    SettingsInterface.SetDataVerificationMode(true);

    CUserSettings *Settings = new CUserSettings();
    Settings = SettingsInterface.GetUserSettings();

    // check all the values
    QCOMPARE(Settings->GetLanguage(), QLocale::English);
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
    QCOMPARE(Settings->GetProxyIPAddress(), QString("253.156.189.012"));
    QCOMPARE(Settings->GetProxyIPPort(), 1234);

    // change all the settings
    Settings->SetValue("Agitation_Speed", 5);
    Settings->SetValue("Leica_AgitationSpeed", 3);
    Settings->SetDateFormat(Global::DATE_INTERNATIONAL);
    Settings->SetLanguage(QLocale::German);
    Settings->SetValue("Oven_StartMode", Global::OvenStartModeToString(Global::OVENSTART_AFTER_STARTUP));
    Settings->SetValue("Oven_Temp", 40);
    Settings->SetValue("Leica_OvenTemp", 40);
    Settings->SetValue("RMS_State", Global::OnOffStateToString(Global::ONOFFSTATE_OFF));
    Settings->SetSoundLevelError(5);
    Settings->SetSoundLevelWarning(6);
    Settings->SetSoundNumberError(7);
    Settings->SetSoundNumberWarning(8);
    Settings->SetTimeFormat(Global::TIME_12);
    Settings->SetRemoteCare(Global::ONOFFSTATE_ON);
    Settings->SetDirectConnection(Global::ONOFFSTATE_OFF);
    Settings->SetProxyUserName("Colorado");
    Settings->SetProxyPassword("Colorado@1234");
    Settings->SetProxyIPAddress("123.234.121.111");
    Settings->SetProxyIPPort(8080);
    SettingsInterface.UpdateUserSettings(Settings);

    // write the settings in to a file
    SettingsInterface.Write(FilesPathWrite + "UserSettings_Test.xml");

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
    QCOMPARE(Settings->GetValue("Agitation_Speed").toInt(),      5);
    QCOMPARE(Settings->GetValue("Leica_AgitationSpeed").toInt(), 3);
    QCOMPARE(Settings->GetDateFormat(),          Global::DATE_INTERNATIONAL);
    QCOMPARE(Settings->GetLanguage(),            QLocale::German);
    QCOMPARE(Settings->GetValue("Oven_StartMode"),      Global::OvenStartModeToString(Global::OVENSTART_AFTER_STARTUP));
    QCOMPARE(Settings->GetValue("Oven_Temp").toInt(),            40);
    QCOMPARE(Settings->GetValue("Leica_OvenTemp").toInt(),       40);
    QCOMPARE(Settings->GetValue("Rms_State"),           Global::OnOffStateToString(Global::ONOFFSTATE_OFF));
    QCOMPARE(Settings->GetSoundLevelError(),     5);
    QCOMPARE(Settings->GetSoundLevelWarning(),   6);
    QCOMPARE(Settings->GetSoundNumberError(),    7);
    QCOMPARE(Settings->GetSoundNumberWarning(),  8);
    QCOMPARE(Settings->GetTimeFormat(),          Global::TIME_12);
    QCOMPARE(Settings->GetRemoteCare(),  Global::ONOFFSTATE_OFF);
    QCOMPARE(Settings->GetDirectConnection(), Global::ONOFFSTATE_ON);
    QCOMPARE(Settings->GetProxyUserName(), QString("Colorado"));
    QCOMPARE(Settings->GetProxyPassword(), QString("Colorado@1234"));
    QCOMPARE(Settings->GetProxyIPAddress(), QString("123.234.121.111"));
    QCOMPARE(Settings->GetProxyIPPort(), 8080);


    // remove dummy file again
    QFile::remove(FilesPathWrite + "UserSettings_Test.xml");
}

void TestUserSettings::utTestWriteReadUserSettingsVerifier() {

    CUserSettingsInterface SettingsInterface;
    IVerifierInterface *p_UserSettingsVerifier;
    p_UserSettingsVerifier = new CUserSettingsVerifier();
    SettingsInterface.AddVerifier(p_UserSettingsVerifier);

    QCOMPARE(SettingsInterface.Read(RESOURCE_FILENAME), true);
    // make the data verification mode to false, so that whenever it reads a file then
    // it won't verify the details again and again.
    //SettingsInterface.SetDataVerificationMode(false);

    CUserSettings *Settings = SettingsInterface.GetUserSettings();

    //********************* Language ********************
    // change to all supported languages and verify it
    Settings->SetLanguage(QLocale::English);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetLanguage(QLocale::German);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetLanguage(QLocale::French);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetLanguage(QLocale::Italian);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetLanguage(QLocale::Spanish);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetLanguage(QLocale::Portuguese);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetLanguage(QLocale::Dutch);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetLanguage(QLocale::Swedish);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetLanguage(QLocale::Norwegian);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetLanguage(QLocale::Danish);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetLanguage(QLocale::Polish);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetLanguage(QLocale::Czech);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetLanguage(QLocale::Russian);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);

    // change the language which is not supported by the application
    Settings->SetLanguage(QLocale::Hindi);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetLanguage(QLocale::Arabic);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetLanguage(QLocale::Telugu);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetLanguage(QLocale::Kannada);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetLanguage(QLocale::Korean);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetLanguage(QLocale::Chinese);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* Date format ********************
    // change to all supported date formats and verify it
    Settings->SetDateFormat(Global::DATE_INTERNATIONAL);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetDateFormat(Global::DATE_ISO);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetDateFormat(Global::DATE_US);

    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);

    // change the date format which is not supported by the application
    Settings->SetDateFormat(Global::DATE_UNDEFINED);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetDateFormat((Global::DateFormat)5);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* Time format ********************
    // change to all supported time formats and verify it
    Settings->SetTimeFormat(Global::TIME_12);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetTimeFormat(Global::TIME_24);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);

    // change the time format which is not supported by the application
    Settings->SetTimeFormat(Global::TIME_UNDEFINED);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetTimeFormat((Global::TimeFormat)10);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* Temperature format ********************
    // change to all supported temperature formats and verify it
    Settings->SetTemperatureFormat(Global::TEMP_FORMAT_CELSIUS);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetTemperatureFormat(Global::TEMP_FORMAT_FAHRENHEIT);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);

    // change the temperature format which is not supported by the application
    Settings->SetTemperatureFormat(Global::TEMP_FORMAT_UNDEFINED);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetTemperatureFormat((Global::TemperatureFormat)8);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* Sound Error Number ********************
    // checking the boundry values
    Settings->SetSoundNumberError(6);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetSoundNumberError(1);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetSoundNumberError(3);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    // checking the outside the boundry values
    Settings->SetSoundNumberError(0);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetSoundNumberError(7);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetSoundNumberError(25);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetSoundNumberError(-7);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* Sound Error Level ********************
    // checking the boundry values
    Settings->SetSoundLevelError(9);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetSoundLevelError(2);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetSoundLevelError(5);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    // checking the outside the boundry values
    Settings->SetSoundLevelError(1);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetSoundLevelError(10);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetSoundLevelError(15);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetSoundLevelError(0);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* Sound Warning Number ********************
    // checking the boundry values
    Settings->SetSoundNumberWarning(6);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetSoundNumberWarning(1);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetSoundNumberWarning(3);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    // checking the outside the boundry values
    Settings->SetSoundNumberWarning(0);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetSoundNumberWarning(7);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetSoundNumberWarning(25);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetSoundNumberWarning(-7);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* Sound Warning Level ********************
    // checking the boundry values
    Settings->SetSoundLevelWarning(9);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetSoundLevelWarning(2);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetSoundLevelWarning(5);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    // checking the outside the boundry values
    Settings->SetSoundLevelWarning(1);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetSoundLevelWarning(10);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetSoundLevelWarning(15);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetSoundLevelWarning(0);    
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* Agitation Speed ********************
    // checking the boundry values
    Settings->SetValue("Agitation_Speed", 5);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Agitation_Speed", 0);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Agitation_Speed", 3);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    // checking the outside the boundry values
    Settings->SetValue("Agitation_Speed", -1);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Agitation_Speed", 6);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Agitation_Speed", 10);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Agitation_Speed", -5);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* Oven start mode ********************
    // change to all supported oven start modes and verify it
    Settings->SetValue("Oven_StartMode", Global::OVENSTART_AFTER_STARTUP);
 //   QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Oven_StartMode", Global::OVENSTART_BEFORE_PROGRAM);
 //   QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);

    // change the oven start mode which is not supported by the application
    Settings->SetValue("Oven_StartMode", Global::OVENSTART_UNDEFINED);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Oven_StartMode", (Global::OvenStartMode)8);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* Oven temperature ********************
    // checking the boundry values
    Settings->SetValue("Oven_Temp", 70);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Oven_Temp", 40);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Oven_Temp", 60);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    // checking the outside the boundry values
    Settings->SetValue("Oven_Temp", 71);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Oven_Temp", 39);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Oven_Temp", 25);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Oven_Temp", 90);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    // temperature values increases by the step of 5
    Settings->SetValue("Oven_Temp", 66);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Oven_Temp", 59);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* RMS State ********************
    // change to all supported RMS states and verify it
    Settings->SetValue("RMS_State", Global::ONOFFSTATE_OFF);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("RMS_State", Global::ONOFFSTATE_ON);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);

    // change the RMS states which is not supported by the application
    Settings->SetValue("RMS_State", Global::ONOFFSTATE_UNDEFINED);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("RMS_State", (Global::OnOffState)4);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* Leica Agitation Speed ********************
    // checking the boundry values
    Settings->SetValue("Leica_AgitationSpeed", 5);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Leica_AgitationSpeed", 0);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Leica_AgitationSpeed", 3);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    // checking the outside the boundry values
    Settings->SetValue("Leica_AgitationSpeed", -1);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Leica_AgitationSpeed", 6);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Leica_AgitationSpeed", 10);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Leica_AgitationSpeed", -5);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* Leica Oven temperature ********************
    // checking the boundry values
    Settings->SetValue("Leica_OvenTemp", 70);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Leica_OvenTemp", 40);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Leica_OvenTemp", 60);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    // checking the outside the boundry values
    Settings->SetValue("Leica_OvenTemp", 71);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Leica_OvenTemp", 39);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Leica_OvenTemp", 25);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Leica_OvenTemp", 90);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    // temperature values increases by the step of 5
    Settings->SetValue("Leica_OvenTemp", 66);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Leica_OvenTemp", 59);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    //********************* Water type ********************
    // change to all supported water types and verify it
    Settings->SetValue("Water_Type", Global::WaterTypeToString(Global::WATER_TYPE_DISTILLED));
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Water_Type", Global::WaterTypeToString(Global::WATER_TYPE_TAP));
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);

    // change the water types which is not supported by the application
    Settings->SetValue("Water_Type", Global::WaterTypeToString((Global::WaterType)-1));
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Water_Type", Global::WaterTypeToString((Global::WaterType)4));
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();
    // check the Loader reagent IDs
    Settings->SetValue("Loader_Reagent1", "");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Loader_Reagent1", "R89");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Loader_Reagent1", "S1");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Loader_Reagent1", "L45");

    Settings->SetValue("Loader_Reagent2", "");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Loader_Reagent2", "R89");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Loader_Reagent2", "S1");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Loader_Reagent2", "U45");

    Settings->SetValue("Loader_Reagent3", "");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Loader_Reagent3", "R89");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Loader_Reagent3", "S1");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Loader_Reagent3", "U12");

    Settings->SetValue("Loader_Reagent4", "");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Loader_Reagent4", "R89");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Loader_Reagent4", "S1");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Loader_Reagent4", "L98");

    Settings->SetValue("Loader_Reagent5", "");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetValue("Loader_Reagent5", "R89");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Loader_Reagent5", "S1");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetValue("Loader_Reagent5", "L12");

    //********************* Network Settings ********************
    // change to all supported RemoteCare states and verify it
    Settings->SetRemoteCare(Global::ONOFFSTATE_ON);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetRemoteCare(Global::ONOFFSTATE_OFF);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);

    // change the RemoteCare states which is not supported by the application
    Settings->SetRemoteCare(Global::ONOFFSTATE_UNDEFINED);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    // change to all supported DirectConnection states and verify it
    Settings->SetDirectConnection(Global::ONOFFSTATE_ON);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetDirectConnection(Global::ONOFFSTATE_OFF);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);

    // change the DirectConnection states which is not supported by the application
    Settings->SetDirectConnection(Global::ONOFFSTATE_UNDEFINED);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    // check Proxy UserName.
    Settings->SetProxyUserName("Colorado_Sepia");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetProxyUserName(" ");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetProxyUserName("Colorado_Sepia1234");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    // check Proxy Password.
    Settings->SetProxyPassword("Colorado");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetProxyPassword(" ");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetProxyPassword("Col");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetProxyPassword("Colorado_Sepia123421");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    // check Proxy IP Address
    Settings->SetProxyIPAddress("123.121.111.011");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetProxyIPAddress("256.121.111.011");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetProxyIPAddress("123.256.111.011");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetProxyIPAddress("123.255.256.011");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetProxyIPAddress("123.254.111.256");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
    Settings->SetProxyIPAddress("123.256.111.011.123");
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    // check Proxy IP Port number
    Settings->SetProxyIPPort(001);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetProxyIPPort(65535);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);
    Settings->SetProxyIPPort(65536);
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), true);

    // data is modified, so reload the file from the resource
    SettingsInterface.Read(RESOURCE_FILENAME);
    Settings = SettingsInterface.GetUserSettings();

    // checking some random scenarios
    Settings->SetValue("Agitation_Speed", 500);
    Settings->SetValue("Leica_AgitationSpeed", 250);
    Settings->SetDateFormat(Global::DATE_UNDEFINED);
    Settings->SetLanguage(QLocale::Japanese);
    Settings->SetValue("Oven_StartMode", Global::OVENSTART_UNDEFINED);
    Settings->SetValue("Oven_Temp", 500);
    Settings->SetValue("Leica_OvenTemp", 500);
    Settings->SetValue("RMS_State", Global::ONOFFSTATE_UNDEFINED);
    Settings->SetSoundLevelError(98);
    Settings->SetSoundLevelWarning(78);
    Settings->SetSoundNumberError(98);
    Settings->SetSoundNumberWarning(75);
    Settings->SetTimeFormat(Global::TIME_UNDEFINED);
    Settings->SetRemoteCare(Global::ONOFFSTATE_UNDEFINED);
    Settings->SetDirectConnection(Global::ONOFFSTATE_UNDEFINED);
    Settings->SetProxyUserName("      ");
    Settings->SetProxyPassword("");
    Settings->SetProxyIPAddress("123.234.121.111.122");
    Settings->SetProxyIPPort(655123);
    SettingsInterface.UpdateUserSettings(Settings);
    // update the user settings in the wrapper class
    QCOMPARE(SettingsInterface.UpdateUserSettings(Settings), false);
}

} // end namespace DataManagement

QTEST_MAIN(DataManager::TestUserSettings)

#include "TestUserSettings.moc"
