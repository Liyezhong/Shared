/****************************************************************************/
/*! \file TestRCConfiguration.cpp
 *
 *  \brief Implementation file for class TestRCConfiguration.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-05-23
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
#include "DataManager/Containers/RCConfiguration/Include/RCConfiguration.h"
#include "DataManager/Containers/RCConfiguration/Include/RCConfigurationVerifier.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"

namespace DataManager {

const QString RESOURCE_FILENAME = ":/Xml/RCConfiguration.xml"; ///< Resource file path

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFileRCConfiguration class.
 */
/****************************************************************************/
class TestRCConfiguration : public QObject {
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
     * \brief Test the RC configuration.
     */
    /****************************************************************************/
    void utTestRCConfiguration();

    /****************************************************************************/
    /**
     * \brief Test write and read of the RCConfiguration Xml.
     */
    /****************************************************************************/
    void utTestWriteReadRCConfigurationInterface();

    /****************************************************************************/
    /**
     * \brief Test write and read RC verifier.
     */
    /****************************************************************************/
    void utTestWriteReadRCConfigurationVerifier();

    /****************************************************************************/
    /**
     * \brief Test write and read rc data item.
     */
    /****************************************************************************/
    void utTestWriteReadRCDataItem();
}; // end class TestRCConfiguration

/****************************************************************************/
void TestRCConfiguration::initTestCase() {
    // init languages
    //Global::InitSupportedLanguages();
}

/****************************************************************************/
void TestRCConfiguration::init() {
}

/****************************************************************************/
void TestRCConfiguration::cleanup() {
}

/****************************************************************************/
void TestRCConfiguration::cleanupTestCase() {
}

/****************************************************************************/
void TestRCConfiguration::utTestRCConfiguration() {

    CRCConfiguration *Settings1 = new CRCConfiguration();

    Settings1->SetQueueSize(6000);
    Settings1->SetHTTPConnectionPersistence(Global::ONOFFSTATE_ON);
    Settings1->SetDebug(Global::ONOFFSTATE_OFF);

    Settings1->SetHTTPSecureConnection(Global::ONOFFSTATE_ON);
    Settings1->SetEncryptionLevel(RemoteCare::RCWebCryptoMedium);
    Settings1->SetAuthentication(Global::ONOFFSTATE_ON);
    Settings1->SetCertificateFileName("Certificate");

    Settings1->SetExecTime(20);
    Settings1->SetServerType(RemoteCare::RCDRMServerConfigAdditional);
    Settings1->SetTargetDataBase("Target");
    Settings1->SetServerURL("URL");
    Settings1->SetServerId(6000);

    Settings1->SetDeviceType(RemoteCare::RCDRMDeviceManaged);
    Settings1->SetDeviceId(9000);

    // change all the settings
    Settings1->SetProxyProtocol(RemoteCare::RCWebProxyProtoSOCKS);

    Settings1->SetRemoteSessionName("Remote");
    Settings1->SetRemoteSessionIPAddress("123.234.121.222");

    CRCDataItem DataItem;

    DataItem.SetName("SetEventClass");
    DataItem.SetType(RemoteCare::RDI_Analog);
    DataItem.SetQuality(RemoteCare::RDI_DataBad);
    DataItem.SetValue("1");

    QVERIFY(Settings1->AddDataItem("SetEventClass", DataItem)==true);

    DataItem.SetName("RequestRemoteSession");
    DataItem.SetType(RemoteCare::RDI_Digital);
    DataItem.SetQuality(RemoteCare::RDI_DataGood);
    DataItem.SetValue("-1");

    QVERIFY(Settings1->AddDataItem("RequestRemoteSession", DataItem)==true);

    DataItem.SetName("SetDownloadFinished");
    DataItem.SetType(RemoteCare::RDI_Undefined);
    DataItem.SetQuality(RemoteCare::RDI_DataUncertain);
    DataItem.SetValue("0");

    QVERIFY(Settings1->AddDataItem("SetDownloadFinished", DataItem)==true);


    DataItem.SetName("SetLogNumber");
    DataItem.SetType(RemoteCare::RDI_String);
    DataItem.SetQuality(RemoteCare::RDI_DataUncertain);
    DataItem.SetValue("1");

    QVERIFY(Settings1->AddDataItem("SetLogNumber", DataItem)==true);


    // now test settings
    QVERIFY(Settings1->GetQueueSize()==6000);
    QCOMPARE(Settings1->GetHTTPConnectionPersistence(), Global::ONOFFSTATE_ON);
    QCOMPARE(Settings1->GetDebug(), Global::ONOFFSTATE_OFF);

    QCOMPARE(Settings1->GetHTTPSecureConnection(), Global::ONOFFSTATE_ON);
    QCOMPARE(Settings1->GetEncryptionLevel(), RemoteCare::RCWebCryptoMedium);
    QCOMPARE(Settings1->GetAuthentication(), Global::ONOFFSTATE_ON);
    QCOMPARE(Settings1->GetCertificateFileName(), QString("Certificate"));

    QVERIFY(Settings1->GetExecTime()==20);
    QCOMPARE(Settings1->GetServerType(), RemoteCare::RCDRMServerConfigAdditional);
    QCOMPARE(Settings1->GetTargetDataBase(), QString("Target"));
    QCOMPARE(Settings1->GetServerURL(), QString("URL"));
    QVERIFY(Settings1->GetServerId()==6000);

    QCOMPARE(Settings1->GetDeviceType(), RemoteCare::RCDRMDeviceManaged);
    QVERIFY(Settings1->GetDeviceId()==9000);

    QCOMPARE(Settings1->GetProxyProtocol(), RemoteCare::RCWebProxyProtoSOCKS);

    QCOMPARE(Settings1->GetRemoteSessionName(), QString("Remote"));
    QCOMPARE(Settings1->GetRemoteSessionIPAddress(), QString("123.234.121.222"));

    // Use copy constructor

    CRCConfiguration *Settings2 = new CRCConfiguration(*Settings1);

    // create the datastream and check whether copying data is working or not
    QByteArray* p_TempByteArray = new QByteArray();
    QDataStream DataStream(p_TempByteArray, QIODevice::ReadWrite);
    DataStream.setVersion(QDataStream::Qt_4_0);
    p_TempByteArray->clear();
    DataStream << *Settings2;
    DataStream.device()->reset();
    DataStream >> *Settings1;

    QCOMPARE(Settings2->GetProxyProtocol(), RemoteCare::RCWebProxyProtoSOCKS);

    CRCConfiguration *Settings3 = new CRCConfiguration();
    Settings3 = Settings2;

    QCOMPARE(Settings3->GetRemoteSessionName(), QString("Remote"));
    QCOMPARE(Settings3->GetRemoteSessionIPAddress(), QString("123.234.121.222"));

    QCOMPARE(Settings3->GetDataItemCount(), 4);

    CRCDataItem DataItem1;

    QVERIFY(Settings3->GetDataItem("SetEventClass", DataItem1)==true);

    QCOMPARE(DataItem1.GetName(), QString("SetEventClass"));
    QCOMPARE(DataItem1.GetType(), RemoteCare::RDI_Analog);
    QCOMPARE(DataItem1.GetQuality(), RemoteCare::RDI_DataBad);
    QCOMPARE(DataItem1.GetValue(), QString("1"));

    QVERIFY(Settings3->GetDataItem("RequestRemoteSession", DataItem1)==true);

    QCOMPARE(DataItem1.GetName(), QString("RequestRemoteSession"));
    QCOMPARE(DataItem1.GetType(), RemoteCare::RDI_Digital);
    QCOMPARE(DataItem1.GetQuality(), RemoteCare::RDI_DataGood);
    QCOMPARE(DataItem1.GetValue(), QString("-1"));

    QVERIFY(Settings3->GetDataItem("SetDownloadFinished", DataItem1)==true);

    QCOMPARE(DataItem1.GetName(), QString("SetDownloadFinished"));
    QCOMPARE(DataItem1.GetType(), RemoteCare::RDI_Undefined);
    QCOMPARE(DataItem1.GetQuality(), RemoteCare::RDI_DataUncertain);
    QCOMPARE(DataItem1.GetValue(), QString("0"));

    QVERIFY(Settings3->GetDataItem("SetLogNumber", DataItem1)==true);

    QCOMPARE(DataItem1.GetName(), QString("SetLogNumber"));
    QCOMPARE(DataItem1.GetType(), RemoteCare::RDI_String);
    QCOMPARE(DataItem1.GetQuality(), RemoteCare::RDI_DataUncertain);
    QCOMPARE(DataItem1.GetValue(), QString("1"));

    DataItem1.SetName("XXX");
    DataItem1.SetType(RemoteCare::RDI_Analog);
    DataItem1.SetQuality(RemoteCare::RDI_DataGood);
    DataItem1.SetValue("2");

    QVERIFY(Settings3->UpdateDataItem("SetLogNumber", DataItem1)==true);

    QCOMPARE(DataItem1.GetName(), QString("XXX"));
    QCOMPARE(DataItem1.GetType(), RemoteCare::RDI_Analog);
    QCOMPARE(DataItem1.GetQuality(), RemoteCare::RDI_DataGood);
    QCOMPARE(DataItem1.GetValue(), QString("2"));
}


/****************************************************************************/
void TestRCConfiguration::utTestWriteReadRCConfigurationInterface() {

    CRCConfigurationInterface SettingsInterface;
    QString FilesPathWrite;

    // store the application path to write the test files
    FilesPathWrite = QCoreApplication::applicationDirPath() + "/";

    QCOMPARE(SettingsInterface.Read(RESOURCE_FILENAME), true);

    SettingsInterface.SetDataVerificationMode(false);
    QCOMPARE(SettingsInterface.Read(RESOURCE_FILENAME), true);
    SettingsInterface.SetDataVerificationMode(true);

    CRCConfiguration *Settings = new CRCConfiguration();
    Settings = SettingsInterface.GetRCConfiguration();

    // check all the values

    QVERIFY(Settings->GetQueueSize()==2000000);
    QCOMPARE(Settings->GetHTTPConnectionPersistence(), Global::ONOFFSTATE_OFF);
    QCOMPARE(Settings->GetDebug(), Global::ONOFFSTATE_ON);

    QCOMPARE(Settings->GetHTTPSecureConnection(), Global::ONOFFSTATE_OFF);
    QCOMPARE(Settings->GetEncryptionLevel(), RemoteCare::RCWebCryptoNone);
    QCOMPARE(Settings->GetAuthentication(), Global::ONOFFSTATE_ON);
    QCOMPARE(Settings->GetCertificateFileName(), QString("Certificate file name"));

    QVERIFY(Settings->GetExecTime()==20);
    QCOMPARE(Settings->GetServerType(), RemoteCare::RCDRMServerConfigPrimary);
    QCOMPARE(Settings->GetTargetDataBase(), QString("target databse name"));
    QCOMPARE(Settings->GetServerURL(), QString("http://server url"));
    QVERIFY(Settings->GetServerId()==30);

    QCOMPARE(Settings->GetDeviceType(), RemoteCare::RCDRMDeviceMaster);
    QVERIFY(Settings->GetDeviceId()==30);

    QCOMPARE(Settings->GetProxyProtocol(), RemoteCare::RCWebProxyProtoSOCKS);

    QCOMPARE(Settings->GetRemoteSessionName(), QString("Remote Session name"));
    QCOMPARE(Settings->GetRemoteSessionIPAddress(), QString("127.0.0.1"));

    QCOMPARE(Settings->GetDataItemCount(), 2);
    CRCDataItem DataItem1;

    QVERIFY(Settings->GetDataItem("RequestRemoteSession", DataItem1)==true);

    QCOMPARE(DataItem1.GetName(), QString("RequestRemoteSession"));
    QCOMPARE(DataItem1.GetType(), RemoteCare::RDI_Analog);
    QCOMPARE(DataItem1.GetQuality(), RemoteCare::RDI_DataGood);
    QCOMPARE(DataItem1.GetValue(), QString("1"));

    QVERIFY(Settings->GetDataItem("SetEventClass", DataItem1)==true);

    QCOMPARE(DataItem1.GetName(), QString("SetEventClass"));
    QCOMPARE(DataItem1.GetType(), RemoteCare::RDI_Digital);
    QCOMPARE(DataItem1.GetQuality(), RemoteCare::RDI_DataBad);
    QCOMPARE(DataItem1.GetValue(), QString("0"));

//    // change all the settings
    Settings->SetQueueSize(6000);
    Settings->SetHTTPConnectionPersistence(Global::ONOFFSTATE_ON);
    Settings->SetDebug(Global::ONOFFSTATE_OFF);

    Settings->SetHTTPSecureConnection(Global::ONOFFSTATE_ON);
    Settings->SetEncryptionLevel(RemoteCare::RCWebCryptoMedium);
    Settings->SetAuthentication(Global::ONOFFSTATE_ON);
    Settings->SetCertificateFileName("Certificate");

    Settings->SetExecTime(20);
    Settings->SetServerType(RemoteCare::RCDRMServerConfigAdditional);
    Settings->SetTargetDataBase("Target");
    Settings->SetServerURL("URL");
    Settings->SetServerId(6000);

    Settings->SetDeviceType(RemoteCare::RCDRMDeviceManaged);
    Settings->SetDeviceId(9000);

    // change all the settings
    Settings->SetProxyProtocol(RemoteCare::RCWebProxyProtoSOCKS);

    Settings->SetRemoteSessionName("Remote");
    Settings->SetRemoteSessionIPAddress("123.234.121.222");
    SettingsInterface.UpdateRCConfiguration(Settings);

    DataItem1.SetName("SetDownloadFinished");
    DataItem1.SetType(RemoteCare::RDI_Undefined);
    DataItem1.SetQuality(RemoteCare::RDI_DataUncertain);
    DataItem1.SetValue("0");

    QVERIFY(Settings->AddDataItem("SetDownloadFinished", DataItem1)==true);

    DataItem1.SetName("SetLogNumber");
    DataItem1.SetType(RemoteCare::RDI_String);
    DataItem1.SetQuality(RemoteCare::RDI_DataUncertain);
    DataItem1.SetValue("1");

    QVERIFY(Settings->AddDataItem("SetLogNumber", DataItem1)==true);

    // write the settings in to a file
    SettingsInterface.Write(FilesPathWrite + "RCConfiguration_Test.xml");

//    // change all the settings again
    Settings->SetQueueSize(2001);
    Settings->SetHTTPConnectionPersistence(Global::ONOFFSTATE_OFF);
    Settings->SetDebug(Global::ONOFFSTATE_ON);

    Settings->SetHTTPSecureConnection(Global::ONOFFSTATE_OFF);
    Settings->SetEncryptionLevel(RemoteCare::RCWebCryptoHigh);
    Settings->SetAuthentication(Global::ONOFFSTATE_OFF);
    Settings->SetCertificateFileName("Certificate1");

    Settings->SetExecTime(15);
    Settings->SetServerType(RemoteCare::RCDRMServerConfigPrimary);
    Settings->SetTargetDataBase("Target1");
    Settings->SetServerURL("URL1");
    Settings->SetServerId(6001);

    Settings->SetDeviceType(RemoteCare::RCDRMDeviceMaster);
    Settings->SetDeviceId(9001);

    // change all the settings
    Settings->SetProxyProtocol(RemoteCare::RCWebProxyProtoHTTP);

    Settings->SetRemoteSessionName("Remote1");
    Settings->SetRemoteSessionIPAddress("123.234.121.221");
    SettingsInterface.UpdateRCConfiguration(Settings);

//    // now test settings
    QVERIFY(Settings->GetQueueSize()==2001);
    QCOMPARE(Settings->GetHTTPConnectionPersistence(), Global::ONOFFSTATE_OFF);
    QCOMPARE(Settings->GetDebug(), Global::ONOFFSTATE_ON);

    QCOMPARE(Settings->GetHTTPSecureConnection(), Global::ONOFFSTATE_OFF);
    QCOMPARE(Settings->GetEncryptionLevel(), RemoteCare::RCWebCryptoHigh);
    QCOMPARE(Settings->GetAuthentication(), Global::ONOFFSTATE_OFF);
    QCOMPARE(Settings->GetCertificateFileName(), QString("Certificate1"));

    QVERIFY(Settings->GetExecTime()==15);
    QCOMPARE(Settings->GetServerType(), RemoteCare::RCDRMServerConfigPrimary);
    QCOMPARE(Settings->GetTargetDataBase(), QString("Target1"));
    QCOMPARE(Settings->GetServerURL(), QString("URL1"));
    QVERIFY(Settings->GetServerId()==6001);

    QCOMPARE(Settings->GetDeviceType(), RemoteCare::RCDRMDeviceMaster);
    QVERIFY(Settings->GetDeviceId()==9001);

    QCOMPARE(Settings->GetProxyProtocol(), RemoteCare::RCWebProxyProtoHTTP);

    QCOMPARE(Settings->GetRemoteSessionName(), QString("Remote1"));
    QCOMPARE(Settings->GetRemoteSessionIPAddress(), QString("123.234.121.221"));

//    // now read settings from the file
    SettingsInterface.Read(FilesPathWrite + "RCConfiguration_Test.xml");

    Settings = SettingsInterface.GetRCConfiguration();
//    // now test settings
    QVERIFY(Settings->GetQueueSize()==6000);
    QCOMPARE(Settings->GetHTTPConnectionPersistence(), Global::ONOFFSTATE_ON);
    QCOMPARE(Settings->GetDebug(), Global::ONOFFSTATE_OFF);

    QCOMPARE(Settings->GetHTTPSecureConnection(), Global::ONOFFSTATE_ON);
    QCOMPARE(Settings->GetEncryptionLevel(), RemoteCare::RCWebCryptoMedium);
    QCOMPARE(Settings->GetAuthentication(), Global::ONOFFSTATE_ON);
    QCOMPARE(Settings->GetCertificateFileName(), QString("Certificate"));

    QVERIFY(Settings->GetExecTime()==20);
    QCOMPARE(Settings->GetServerType(), RemoteCare::RCDRMServerConfigAdditional);
    QCOMPARE(Settings->GetTargetDataBase(), QString("Target"));
    QCOMPARE(Settings->GetServerURL(), QString("URL"));
    QVERIFY(Settings->GetServerId()==6000);

    QCOMPARE(Settings->GetDeviceType(), RemoteCare::RCDRMDeviceManaged);
    QVERIFY(Settings->GetDeviceId()==9000);

    QCOMPARE(Settings->GetProxyProtocol(), RemoteCare::RCWebProxyProtoSOCKS);

    QCOMPARE(Settings->GetRemoteSessionName(), QString("Remote"));
    QCOMPARE(Settings->GetRemoteSessionIPAddress(), QString("123.234.121.222"));


    QVERIFY(Settings->GetDataItem("SetEventClass", DataItem1)==true);

    QCOMPARE(DataItem1.GetName(), QString("SetEventClass"));
    QCOMPARE(DataItem1.GetType(), RemoteCare::RDI_Digital);
    QCOMPARE(DataItem1.GetQuality(), RemoteCare::RDI_DataBad);
    QCOMPARE(DataItem1.GetValue(), QString("0"));

    QVERIFY(Settings->GetDataItem("RequestRemoteSession", DataItem1)==true);

    QCOMPARE(DataItem1.GetName(), QString("RequestRemoteSession"));
    QCOMPARE(DataItem1.GetType(), RemoteCare::RDI_Analog);
    QCOMPARE(DataItem1.GetQuality(), RemoteCare::RDI_DataGood);
    QCOMPARE(DataItem1.GetValue(), QString("1"));

    QVERIFY(Settings->GetDataItem("SetDownloadFinished", DataItem1)==true);

    QCOMPARE(DataItem1.GetName(), QString("SetDownloadFinished"));
    QCOMPARE(DataItem1.GetType(), RemoteCare::RDI_Undefined);
    QCOMPARE(DataItem1.GetQuality(), RemoteCare::RDI_DataUncertain);
    QCOMPARE(DataItem1.GetValue(), QString("0"));

    QVERIFY(Settings->GetDataItem("SetLogNumber", DataItem1)==true);

    QCOMPARE(DataItem1.GetName(), QString("SetLogNumber"));
    QCOMPARE(DataItem1.GetType(), RemoteCare::RDI_String);
    QCOMPARE(DataItem1.GetQuality(), RemoteCare::RDI_DataUncertain);
    QCOMPARE(DataItem1.GetValue(), QString("1"));

    QVERIFY(Settings->DeleteDataItem("SetLogNumber")==true);
    QVERIFY(Settings->GetDataItem("SetLogNumber", DataItem1)==false);

    // remove dummy file again
    QFile::remove(FilesPathWrite + "RCConfiguration_Test.xml");
}

void TestRCConfiguration::utTestWriteReadRCConfigurationVerifier() {

    CRCConfigurationInterface SettingsInterface;
    IVerifierInterface *p_RCConfigurationVerifier;
    p_RCConfigurationVerifier = new CRCConfigurationVerifier();
    SettingsInterface.AddVerifier(p_RCConfigurationVerifier);

    QCOMPARE(SettingsInterface.Read(RESOURCE_FILENAME), true);
    // make the data verification mode to false, so that whenever it reads a file then
    // it won't verify the details again and again.
    //SettingsInterface.SetDataVerificationMode(false);

    CRCConfiguration Settings = *(SettingsInterface.GetRCConfiguration());

    // boundary values for queue size
    Settings.SetQueueSize(MIN_QUEUE_SIZE-2);
    SettingsInterface.SetDataVerificationMode(true);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetQueueSize(MIN_QUEUE_SIZE+2);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetQueueSize(MAX_QUEUE_SIZE+2);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetQueueSize(MAX_QUEUE_SIZE-2);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // on off values for HTTPConnectionPersistence
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetHTTPConnectionPersistence(Global::ONOFFSTATE_UNDEFINED);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetHTTPConnectionPersistence((Global::OnOffState)(10));
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetHTTPConnectionPersistence(Global::ONOFFSTATE_OFF);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetHTTPConnectionPersistence(Global::ONOFFSTATE_ON);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // on off values for debug
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetDebug(Global::ONOFFSTATE_UNDEFINED);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetDebug((Global::OnOffState)(10));
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetDebug(Global::ONOFFSTATE_OFF);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetDebug(Global::ONOFFSTATE_ON);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // on off values for HTTPSecureConnection
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetHTTPSecureConnection(Global::ONOFFSTATE_UNDEFINED);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetHTTPSecureConnection((Global::OnOffState)(10));
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetHTTPSecureConnection(Global::ONOFFSTATE_OFF);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetHTTPSecureConnection(Global::ONOFFSTATE_ON);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // on off values for Authentication
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetAuthentication(Global::ONOFFSTATE_UNDEFINED);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetAuthentication((Global::OnOffState)(10));
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetAuthentication(Global::ONOFFSTATE_OFF);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetAuthentication(Global::ONOFFSTATE_ON);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // encryption level
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetEncryptionLevel((RemoteCare::RCWebCryptoLevel_t)(10));
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetEncryptionLevel(RemoteCare::RCWebCryptoInvalid);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetEncryptionLevel(RemoteCare::RCWebCryptoNone);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetEncryptionLevel(RemoteCare::RCWebCryptoHigh);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetEncryptionLevel(RemoteCare::RCWebCryptoMedium);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetEncryptionLevel(RemoteCare::RCWebCryptoLow);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // lenght check for CertificateFileName
    QByteArray CertificateFileName;
    CertificateFileName = CertificateFileName.fill('A', MIN_CERT_FILENAME_LENGTH-1);
    Settings.SetCertificateFileName(CertificateFileName);
    SettingsInterface.SetDataVerificationMode(true);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    CertificateFileName = CertificateFileName.fill('A', MAX_CERT_FILENAME_LENGTH+1);
    Settings.SetCertificateFileName(CertificateFileName);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    CertificateFileName = CertificateFileName.fill('A', MIN_CERT_FILENAME_LENGTH);
    Settings.SetCertificateFileName(CertificateFileName);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    CertificateFileName = CertificateFileName.fill('A', MAX_CERT_FILENAME_LENGTH);
    Settings.SetCertificateFileName(CertificateFileName);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // boundary values for exec time
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetExecTime(MIN_EXEC_TIME-1);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetExecTime(MIN_EXEC_TIME+2);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetExecTime(MAX_EXEC_TIME+1);
    SettingsInterface.SetDataVerificationMode(true);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetExecTime(MAX_EXEC_TIME-2);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // on off values server type
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetServerType((RemoteCare::RCDRMServerConfigType_t)(10));
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetServerType(RemoteCare::RCDRMServerConfigInvalid);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetServerType(RemoteCare::RCDRMServerConfigPrimary);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetServerType(RemoteCare::RCDRMServerConfigAdditional);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetServerType(RemoteCare::RCDRMServerConfigBackup);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // lenght check for target data base
    QByteArray TargetDataBase;
    TargetDataBase = TargetDataBase.fill('A', MIN_TARGET_DATABASE_LENGTH-1);
    Settings.SetTargetDataBase(TargetDataBase);
    SettingsInterface.SetDataVerificationMode(true);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    TargetDataBase = TargetDataBase.fill('A', MAX_TARGET_DATABASE_LENGTH+1);
    Settings.SetTargetDataBase(TargetDataBase);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    TargetDataBase = TargetDataBase.fill('A', MIN_TARGET_DATABASE_LENGTH);
    Settings.SetTargetDataBase(TargetDataBase);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    TargetDataBase = TargetDataBase.fill('A', MAX_TARGET_DATABASE_LENGTH);
    Settings.SetTargetDataBase(TargetDataBase);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // lenght check for server URL
    QByteArray ServerURL;
    ServerURL = ServerURL.fill('A', MIN_SERVER_URL_LENGTH-1);
    Settings.SetServerURL(ServerURL);
    SettingsInterface.SetDataVerificationMode(true);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    ServerURL = ServerURL.fill('A', MAX_SERVER_URL_LENGTH+1);
    Settings.SetServerURL(ServerURL);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    ServerURL = ServerURL.fill('A', MIN_SERVER_URL_LENGTH);
    Settings.SetServerURL(ServerURL);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    ServerURL = ServerURL.fill('A', MAX_SERVER_URL_LENGTH);
    Settings.SetServerURL(ServerURL);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // boundary values for server id
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetServerId(MIN_SERVER_ID-1);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetServerId(MIN_SERVER_ID+2);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetServerId(MAX_SERVER_ID+1);
    SettingsInterface.SetDataVerificationMode(true);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetServerId(MAX_SERVER_ID-2);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);


    // on off values device type
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetDeviceType((RemoteCare::RCDRMDeviceType_t)(10));
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetDeviceType(RemoteCare::RCDRMDeviceInvalid);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetDeviceType(RemoteCare::RCDRMDeviceMaster);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetDeviceType(RemoteCare::RCDRMDeviceManaged);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // boundary values for device id
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetDeviceId(MIN_DEVICE_ID-1);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetDeviceId(MIN_DEVICE_ID+2);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetDeviceId(MAX_DEVICE_ID+1);
    SettingsInterface.SetDataVerificationMode(true);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetDeviceId(MAX_DEVICE_ID-2);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // proxy protocol type
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetProxyProtocol((RemoteCare::RCWebProxyProtocol_t)(10));
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetProxyProtocol(RemoteCare::RCWebProxyProtoInvalid);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetProxyProtocol(RemoteCare::RCWebProxyProtoNone);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetProxyProtocol(RemoteCare::RCWebProxyProtoSOCKS);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetProxyProtocol(RemoteCare::RCWebProxyProtoHTTP);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // lenght check for RemoteSessionName
    QByteArray RemoteSessionName;
    SettingsInterface.SetDataVerificationMode(true);
    RemoteSessionName = RemoteSessionName.fill('A', MIN_REMOTESESSION_NAME_LENGTH-1);
    Settings.SetRemoteSessionName(RemoteSessionName);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    RemoteSessionName = RemoteSessionName.fill('A', MAX_REMOTESESSION_NAME_LENGTH+1);
    Settings.SetRemoteSessionName(RemoteSessionName);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    RemoteSessionName = RemoteSessionName.fill('A', MIN_REMOTESESSION_NAME_LENGTH);
    Settings.SetRemoteSessionName(RemoteSessionName);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    RemoteSessionName = RemoteSessionName.fill('A', MAX_REMOTESESSION_NAME_LENGTH);
    Settings.SetRemoteSessionName(RemoteSessionName);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // check remote session IP Address
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetRemoteSessionIPAddress("256.121.111.011");
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetRemoteSessionIPAddress("123.256.111.011");
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetRemoteSessionIPAddress("123.255.256.011");
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetRemoteSessionIPAddress("123.254.111.256");
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetRemoteSessionIPAddress("123.256.111.011.123");
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetRemoteSessionIPAddress("123.121.111.011");
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // on off values for compression
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetCompression(Global::ONOFFSTATE_UNDEFINED);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetCompression((Global::OnOffState)(10));
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetCompression(Global::ONOFFSTATE_OFF);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetCompression(Global::ONOFFSTATE_ON);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    // boundary values for maximum chunk size
    SettingsInterface.SetDataVerificationMode(true);
    Settings.SetMaxChunkSize(MIN_MAXCHUNKSIZE-1);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetMaxChunkSize(MIN_MAXCHUNKSIZE+2);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    Settings.SetMaxChunkSize(MAX_MAXCHUNKSIZE+1);
    SettingsInterface.SetDataVerificationMode(true);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    Settings.SetMaxChunkSize(MAX_MAXCHUNKSIZE-2);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    CRCDataItem DataItem1;

    QVERIFY(Settings.GetDataItem("RequestRemoteSession", DataItem1)==true);
    DataItem1.SetType(RemoteCare::RDI_Analog);
    Settings.UpdateDataItem("RequestRemoteSession", DataItem1);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    DataItem1.SetType(RemoteCare::RDI_TypeInvalid);
    Settings.UpdateDataItem("RequestRemoteSession", DataItem1);
    SettingsInterface.SetDataVerificationMode(true);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    DataItem1.SetType(RemoteCare::RDI_Digital);
    Settings.UpdateDataItem("RequestRemoteSession", DataItem1);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);

    DataItem1.SetQuality(RemoteCare::RDI_DataGood);
    Settings.UpdateDataItem("RequestRemoteSession", DataItem1);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
    DataItem1.SetQuality(RemoteCare::RDI_DataInvalid);
    Settings.UpdateDataItem("RequestRemoteSession", DataItem1);
    SettingsInterface.SetDataVerificationMode(true);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), false);
    DataItem1.SetQuality(RemoteCare::RDI_DataBad);
    Settings.UpdateDataItem("RequestRemoteSession", DataItem1);
    QCOMPARE(SettingsInterface.UpdateRCConfiguration(&Settings), true);
}

/****************************************************************************/
void TestRCConfiguration::utTestWriteReadRCDataItem() {

    CRCDataItem *p_DataItem = new CRCDataItem();

    p_DataItem->SetName("SetEventLevel");
    p_DataItem->SetType(RemoteCare::RDI_Analog);
    p_DataItem->SetQuality(RemoteCare::RDI_DataBad);
    p_DataItem->SetValue("1");

    // now test settings
    QVERIFY(p_DataItem->GetName() == "SetEventLevel");
    QCOMPARE(p_DataItem->GetType(), RemoteCare::RDI_Analog);
    QCOMPARE(p_DataItem->GetQuality(), RemoteCare::RDI_DataBad);
    QVERIFY(p_DataItem->GetValue() == "1");

    // Use copy constructor

    CRCDataItem *p_DataItem2 = new CRCDataItem(*p_DataItem);

    // create the datastream and check whether copying data is working or not
    QByteArray* p_TempByteArray = new QByteArray();
    QDataStream DataStream(p_TempByteArray, QIODevice::ReadWrite);
    DataStream.setVersion(QDataStream::Qt_4_0);
    p_TempByteArray->clear();
    DataStream << *p_DataItem2;
    DataStream.device()->reset();
    DataStream >> *p_DataItem;

    QCOMPARE(p_DataItem->GetType(), RemoteCare::RDI_Analog);

    CRCDataItem *p_DataItem3 = new CRCDataItem();
    p_DataItem3 = p_DataItem2;

    QVERIFY(p_DataItem3->GetName() == "SetEventLevel");
    QCOMPARE(p_DataItem3->GetType(), RemoteCare::RDI_Analog);
    QCOMPARE(p_DataItem3->GetQuality(), RemoteCare::RDI_DataBad);
    QVERIFY(p_DataItem3->GetValue() == "1");
}


} // end namespace DataManagement

QTEST_MAIN(DataManager::TestRCConfiguration)

#include "TestRCConfiguration.moc"
