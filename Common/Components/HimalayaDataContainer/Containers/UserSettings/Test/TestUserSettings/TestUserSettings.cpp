#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QLocale>
#include <stdio.h>

#include "DataManager/Containers/UserSettings/Commands/Include/CmdAlarmToneTest.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdChangeUserSettings.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdRmsOnOff.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdSetWaterStations.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdWaterStationUpdate.h"

#include "HimalayaDataContainer/Containers/UserSettings/Commands/Include/CmdParaffinMeltPointChanged.h"
#include "HimalayaDataContainer/Containers/UserSettings/Commands/Include/CmdQuitAppShutdown.h"
#include "HimalayaDataContainer/Containers/UserSettings/Commands/Include/CmdResetOperationHours.h"

#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaUserSettings.h"
#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaSettingsVerifier.h"

class TestUserSettingsTest : public QObject
{
    Q_OBJECT
    
public:
    TestUserSettingsTest();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();

    void utTestCmdTone();
    void utTestSetting();
    void utTestCmdRmsOnoff();
    void utTestCmdWaterStation();
    void utTestCmdStationUpate();

    void utTestCmdParaMelt();
    void utTestCmdAppShutdown();
    void utTestCmdResetOP();

    void utTestUserSetting();
    void utTestUserSettingV();
};

TestUserSettingsTest::TestUserSettingsTest()
{
}

void TestUserSettingsTest::initTestCase()
{
}

void TestUserSettingsTest::cleanupTestCase()
{
}

void TestUserSettingsTest::testCase1()
{
    ;//QVERIFY2(true, "Failure");
}

void TestUserSettingsTest::utTestCmdTone()
{
    MsgClasses::CmdAlarmToneTest tone(10, 0, 5, Global::ALARM_WARNING);

    MsgClasses::CmdAlarmToneTest *almTone = new MsgClasses::CmdAlarmToneTest;
    almTone->setBusyStateAllowed(false);
    almTone->setErrorStateAllowed(false);
    almTone->setIdleStateAllowed(false);
    almTone->setInitFailedStateAllowed(false);
    almTone->setSoftSwitchMonitorStateAllowed(false);
    almTone->setUserActionStateAllowed(false);

    almTone->GetAlarmType();
    almTone->GetName();
    almTone->GetSound();
    almTone->GetTimeout();
    almTone->GetVolume();

    almTone->isBusyStateAllowed();
    almTone->isErrorStateAllowed();
    almTone->isIdleStateAllowed();
    almTone->isInitFailedStateAllowed();
    almTone->isSoftSwitchStateAllowed();
    almTone->isStateAllowed(QString("state"));
    almTone->isUserActionAllowed();

    QDataStream ds;
    ds << (*almTone);
    ds >> (*almTone);

    delete almTone;
}

void TestUserSettingsTest::utTestSetting()
{
    //QDataStream ds;
    //MsgClasses::CmdChangeUserSettings us(10, ds);
    MsgClasses::CmdChangeUserSettings *userSettings = new MsgClasses::CmdChangeUserSettings;

    userSettings->setBusyStateAllowed(false);
    userSettings->setErrorStateAllowed(false);
    userSettings->setIdleStateAllowed(false);
    userSettings->setInitFailedStateAllowed(false);
    userSettings->setSoftSwitchMonitorStateAllowed(false);
    userSettings->setUserActionStateAllowed(false);

    userSettings->GetName();
    userSettings->GetTimeout();

    userSettings->isBusyStateAllowed();
    userSettings->isErrorStateAllowed();
    userSettings->isIdleStateAllowed();
    userSettings->isInitFailedStateAllowed();
    userSettings->isSoftSwitchStateAllowed();
    userSettings->isStateAllowed(QString("state"));
    userSettings->isUserActionAllowed();

    QDataStream ds;
    ds << (*userSettings);
    ds >> (*userSettings);

    delete userSettings;

}
void TestUserSettingsTest::utTestCmdRmsOnoff()
{
    MsgClasses::CmdRmsOnOff *rmsonoff = new MsgClasses::CmdRmsOnOff(10, false);

    rmsonoff->setBusyStateAllowed(false);
    rmsonoff->setErrorStateAllowed(false);
    rmsonoff->setIdleStateAllowed(false);
    rmsonoff->setInitFailedStateAllowed(false);
    rmsonoff->setSoftSwitchMonitorStateAllowed(false);
    rmsonoff->setUserActionStateAllowed(false);

    rmsonoff->GetRmsState();
    rmsonoff->GetName();
    rmsonoff->GetTimeout();

    rmsonoff->isBusyStateAllowed();
    rmsonoff->isErrorStateAllowed();
    rmsonoff->isIdleStateAllowed();
    rmsonoff->isInitFailedStateAllowed();
    rmsonoff->isSoftSwitchStateAllowed();
    rmsonoff->isStateAllowed(QString("state"));
    rmsonoff->isUserActionAllowed();

    QDataStream ds;
    ds << (*rmsonoff);
    ds >> (*rmsonoff);

    delete rmsonoff;
}
void TestUserSettingsTest::utTestCmdWaterStation()
{
    MsgClasses::CmdSetWaterStations *waterStation = new MsgClasses::CmdSetWaterStations(10, QString("reagnetid"), QString("id"));

    waterStation->setBusyStateAllowed(false);
    waterStation->setErrorStateAllowed(false);
    waterStation->setIdleStateAllowed(false);
    waterStation->setInitFailedStateAllowed(false);
    waterStation->setSoftSwitchMonitorStateAllowed(false);
    waterStation->setUserActionStateAllowed(false);

    waterStation->GetGroup();
    waterStation->GetName();
    waterStation->GetTimeout();
    waterStation->GetReagentID();

    waterStation->isBusyStateAllowed();
    waterStation->isErrorStateAllowed();
    waterStation->isIdleStateAllowed();
    waterStation->isInitFailedStateAllowed();
    waterStation->isSoftSwitchStateAllowed();
    waterStation->isStateAllowed(QString("state"));
    waterStation->isUserActionAllowed();

    QDataStream ds;
    ds << (*waterStation);
    ds >> (*waterStation);

    delete waterStation;
}
void TestUserSettingsTest::utTestCmdStationUpate()
{
    MsgClasses::CmdWaterStationUpdate su(10, QString("stationid"), false);
    MsgClasses::CmdWaterStationUpdate *stationUpdate = new MsgClasses::CmdWaterStationUpdate;

    stationUpdate->setBusyStateAllowed(false);
    stationUpdate->setErrorStateAllowed(false);
    stationUpdate->setIdleStateAllowed(false);
    stationUpdate->setInitFailedStateAllowed(false);
    stationUpdate->setSoftSwitchMonitorStateAllowed(false);
    stationUpdate->setUserActionStateAllowed(false);

    stationUpdate->GetName();
    stationUpdate->GetTimeout();
    stationUpdate->GetWaterFlowState();
    stationUpdate->GetWaterStationID();

    stationUpdate->isBusyStateAllowed();
    stationUpdate->isErrorStateAllowed();
    stationUpdate->isIdleStateAllowed();
    stationUpdate->isInitFailedStateAllowed();
    stationUpdate->isSoftSwitchStateAllowed();
    stationUpdate->isStateAllowed(QString("state"));
    stationUpdate->isUserActionAllowed();

    QDataStream ds;
    ds << (*stationUpdate);
    ds >> (*stationUpdate);

    delete stationUpdate;
}

void TestUserSettingsTest::utTestCmdParaMelt()
{
    MsgClasses::CmdParaffinMeltPointChanged para(10, 65, 70);
    MsgClasses::CmdParaffinMeltPointChanged *paraMelt = new MsgClasses::CmdParaffinMeltPointChanged;

    paraMelt->setBusyStateAllowed(false);
    paraMelt->setErrorStateAllowed(false);
    paraMelt->setIdleStateAllowed(false);
    paraMelt->setInitFailedStateAllowed(false);
    paraMelt->setSoftSwitchMonitorStateAllowed(false);
    paraMelt->setUserActionStateAllowed(false);

    paraMelt->GetCurrentMeltPoint();
    paraMelt->GetLastMeltPoint();
    paraMelt->GetName();
    paraMelt->GetTimeout();

    paraMelt->isBusyStateAllowed();
    paraMelt->isErrorStateAllowed();
    paraMelt->isIdleStateAllowed();
    paraMelt->isInitFailedStateAllowed();
    paraMelt->isSoftSwitchStateAllowed();
    paraMelt->isStateAllowed(QString("state"));
    paraMelt->isUserActionAllowed();

    QDataStream ds;
    ds << (*paraMelt);
    ds >> (*paraMelt);

    delete paraMelt;
}

void TestUserSettingsTest::utTestCmdAppShutdown()
{
    MsgClasses::CmdQuitAppShutdown shutdown(10, DataManager::QUITAPPSHUTDOWNACTIONTYPE_UNDEFINED);
    MsgClasses::CmdQuitAppShutdown *quitApp = new MsgClasses::CmdQuitAppShutdown;

    quitApp->setBusyStateAllowed(false);
    quitApp->setErrorStateAllowed(false);
    quitApp->setIdleStateAllowed(false);
    quitApp->setInitFailedStateAllowed(false);
    quitApp->setSoftSwitchMonitorStateAllowed(false);
    quitApp->setUserActionStateAllowed(false);

    quitApp->GetName();
    quitApp->GetTimeout();

    quitApp->isBusyStateAllowed();
    quitApp->isErrorStateAllowed();
    quitApp->isIdleStateAllowed();
    quitApp->isInitFailedStateAllowed();
    quitApp->isSoftSwitchStateAllowed();
    quitApp->isStateAllowed(QString("state"));
    quitApp->isUserActionAllowed();

    QDataStream ds;
    ds << (*quitApp);
    ds >> (*quitApp);

    delete quitApp;
}

void TestUserSettingsTest::utTestCmdResetOP()
{
    MsgClasses::CmdResetOperationHours rst(10, DataManager::RESETOPERATIONHOURS_UNDEFINED);
    MsgClasses::CmdResetOperationHours *rstop = new MsgClasses::CmdResetOperationHours;

    rstop->setBusyStateAllowed(false);
    rstop->setErrorStateAllowed(false);
    rstop->setIdleStateAllowed(false);
    rstop->setInitFailedStateAllowed(false);
    rstop->setSoftSwitchMonitorStateAllowed(false);
    rstop->setUserActionStateAllowed(false);

    rstop->GetName();
    rstop->GetTimeout();

    rstop->isBusyStateAllowed();
    rstop->isErrorStateAllowed();
    rstop->isIdleStateAllowed();
    rstop->isInitFailedStateAllowed();
    rstop->isSoftSwitchStateAllowed();
    rstop->isStateAllowed(QString("state"));
    rstop->isUserActionAllowed();

    QDataStream ds;
    ds << (*rstop);
    ds >> (*rstop);

    delete rstop;
}

void TestUserSettingsTest::utTestUserSetting()
{
    DataManager::CUserSettings us;
    DataManager::CHimalayaUserSettings hus1(us);
    DataManager::CHimalayaUserSettings hus2(hus1);

    DataManager::CHimalayaUserSettings *himUserSetting = new DataManager::CHimalayaUserSettings;

    himUserSetting->SetActiveCarbonHours(50);

    QString rstdate = QString("restdate");
    himUserSetting->SetActiveCarbonLastResetDate(rstdate);

    himUserSetting->SetCassetteCount(100);
    himUserSetting->SetDateFormat(Global::DATE_ISO);
    himUserSetting->SetDefaultAttributes();
    himUserSetting->SetDirectConnection(Global::ONOFFSTATE_ON);
    himUserSetting->SetFrequencyPV(50);
    himUserSetting->SetLanguage(QLocale::AnyLanguage);
    himUserSetting->SetModeRMSCleaning(Global::RMS_CASSETTES);
    himUserSetting->SetModeRMSProcessing(Global::RMS_CASSETTES);
    himUserSetting->SetOperationHours(20);

    himUserSetting->SetOperationLastResetDate(rstdate);
    himUserSetting->SetProxyIPAddress(QString("proxyip"));
    himUserSetting->SetProxyUserName(QString("proxyusername"));

    QString reagentid = QString("reagentid");
    himUserSetting->SetReagentIdOfLastStep(reagentid);
    himUserSetting->SetRemoteCare(Global::ONOFFSTATE_OFF);

    himUserSetting->SetSoundLevelError(7);
    himUserSetting->SetSoundLevelInformation(7);
    himUserSetting->SetSoundLevelWarning(7);
    himUserSetting->SetSoundNumberError(5);
    himUserSetting->SetSoundNumberInformation(5);
    himUserSetting->SetSoundNumberWarning(5);
    himUserSetting->SetSoundPeriodicInformation(false);
    himUserSetting->SetSoundPeriodicTimeError(6);
    himUserSetting->SetSoundPeriodicTimeInformation(6);
    himUserSetting->SetSoundPeriodicTimeWarning(6);
    himUserSetting->SetSoundPeriodicWarning(false);

    himUserSetting->SetTemperatureFormat(Global::TEMP_FORMAT_CELSIUS);
    himUserSetting->SetTemperatureParaffinBath(75);
    himUserSetting->SetTimeFormat(Global::TIME_24);
    himUserSetting->SetUseExhaustSystem(1);
    himUserSetting->SetValue(QString("key"), 20);

    himUserSetting->GetActiveCarbonHours();
    himUserSetting->GetActiveCarbonLastResetDate();
    himUserSetting->GetCassetteCount();

    DataManager::CorrectionModule_t mt;
    mt.ModuleId = QString("moduleid");
    mt.ModuleLength40 = QString("len40");
    mt.ModuleLength50 = QString("len50");
    mt.ModuleLength55 = QString("len55");
    mt.ModuleLength60 = QString("len60");

    //himUserSetting->GetCorrectionModuleInfo(0, mt);
    himUserSetting->GetCorrectionModuleInfo(QString("moduleid"), mt);
    himUserSetting->UpdateCorrcetionModuleInfo(QString("moduleid"), mt);

    himUserSetting->GetDateFormat();
    himUserSetting->GetDirectConnection();
    himUserSetting->GetErrors();
    himUserSetting->GetFrequencyPV();
    himUserSetting->GetLanguage();
    himUserSetting->GetModeRMSCleaning();
    himUserSetting->GetModeRMSProcessing();
    himUserSetting->GetNumberOfCorrectionModules();
    himUserSetting->GetOperationHours();
    himUserSetting->GetOperationLastResetDate();
    himUserSetting->GetProxyIPAddress();
    himUserSetting->GetProxyIPPort();
    himUserSetting->GetProxyPassword();
    himUserSetting->GetProxyUserName();
    himUserSetting->GetReagentIdOfLastStep();
    himUserSetting->GetRemoteCare();
    himUserSetting->GetSoundLevelError();
    himUserSetting->GetSoundLevelInformation();
    himUserSetting->GetSoundLevelWarning();
    himUserSetting->GetSoundNumberError();
    himUserSetting->GetSoundNumberInformation();
    himUserSetting->GetSoundNumberWarning();
    himUserSetting->GetSoundPeriodicError();
    himUserSetting->GetSoundPeriodicInformation();
    himUserSetting->GetSoundPeriodicTimeError();
    himUserSetting->GetSoundPeriodicTimeInformation();
    himUserSetting->GetSoundPeriodicTimeWarning();
    himUserSetting->GetSoundPeriodicWarning();
    himUserSetting->GetTemperatureFormat();
    himUserSetting->GetTemperatureParaffinBath();
    himUserSetting->GetTimeFormat();
    himUserSetting->GetUseExhaustSystem();
    himUserSetting->GetValue(QString("key"));
    himUserSetting->GetValueList();
    himUserSetting->GetValueListCount();
    himUserSetting->GetVersion();

    himUserSetting->AddCorrectionModuleInfo(QString("id"), QString("Length40"), QString("Length50"), QString("Length55"), QString("Length60"));
    DataManager::CUserSettings us2;
    himUserSetting->CopyFromOther(us2);
    himUserSetting->DeleteAllCorrectionModules();

    delete himUserSetting;
}

void TestUserSettingsTest::utTestUserSettingV()
{
    DataManager::CHimalayaSettingsVerifier hsv;
    hsv.GetErrors();
    hsv.IsLocalVerifier();
    hsv.ResetErrors();
    hsv.VerifyData(NULL);
}

QTEST_APPLESS_MAIN(TestUserSettingsTest)

#include "TestUserSettings.moc"
