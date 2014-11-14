/****************************************************************************/
/*! \file TestDeviceConfiguration.cpp
 *
 *  \brief Implementation file for class TestDeviceConfiguration.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-10
 *  $Author:    $ Ningu
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
#include <QStringList>

#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfiguration.h"
#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationVerifier.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"


namespace DataManager {

static QString FilesPathWrite;  ///< Path to where we can write some files.
const QString RESOURCE_FILENAME = ":/Xml/DeviceConfiguration.xml"; ///< Resource file path

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFileDeviceConfiguration class.
 */
/****************************************************************************/
class TestDeviceConfiguration : public QObject {
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
     * \brief Test the DeviceConfig.
     */
    /****************************************************************************/
    void utTestDeviceConfiguration();

    /****************************************************************************/
    /**
     * \brief Test write and read of the DeviceConfig Xml.
     */
    /****************************************************************************/
    void utTestWriteReadDeviceConfigurationInterface();

    /****************************************************************************/
    /**
     * \brief Test write and read DeviceConfig of the verifier.
     */
    /****************************************************************************/
    void utTestWriteReadDeviceConfigurationVerifier();
}; // end class TestDeviceConfiguration

/****************************************************************************/
void TestDeviceConfiguration::initTestCase() {
    // set paths

    FilesPathWrite = QCoreApplication::applicationDirPath() + "/";
    // init languages
}

/****************************************************************************/
void TestDeviceConfiguration::init() {
}

/****************************************************************************/
void TestDeviceConfiguration::cleanup() {
}

/****************************************************************************/
void TestDeviceConfiguration::cleanupTestCase() {
}

/****************************************************************************/
void TestDeviceConfiguration::utTestDeviceConfiguration() {

    CDeviceConfiguration *DeviceConfig1 = new CDeviceConfiguration();

    // change all the DeviceConfig
   // DeviceConfig1->SetLanguage(QLocale::German);
    QStringList LangList1,LangList2,LangList3;
    LangList1.clear();
    LangList1.append("English");
    LangList1.append("German");
    LangList1.append("French");
    LangList1.append("Italian");
    LangList1.append("Spanish");
    LangList1.append("Portuguese");
    DeviceConfig1->SetLanguageList(LangList1);
    DeviceConfig1->SetValue("DeviceName", QString("ST8200"));
    DeviceConfig1->SetValue("SerialNumber", QString("12345678"));
    DeviceConfig1->SetValue("CoverSlipperName", QString("CV 8020"));
    DeviceConfig1->SetValue("WorkstationMode", false);
    DeviceConfig1->SetValue("HeatedCuevettes", false);
    DeviceConfig1->SetValue("SlideIdCamera", false);

    // now test DeviceConfig   
    LangList2.clear();
    LangList2 = DeviceConfig1->GetLanguageList();
    //QCOMPARE(DeviceConfig1->GetLanguageList(),LangList);
    QCOMPARE(DeviceConfig1->GetValue("DeviceName") ,QString("ST8200"));
    QCOMPARE(DeviceConfig1->GetValue("SerialNumber") ,QString("12345678"));
    QCOMPARE(DeviceConfig1->GetValue("CoverSlipperName") ,QString("CV 8020"));
    QCOMPARE(DeviceConfig1->GetBoolValue("WorkstationMode") ,false);
    QCOMPARE(DeviceConfig1->GetBoolValue("HeatedCuevettes") ,false);
    QCOMPARE(DeviceConfig1->GetBoolValue("SlideIdCamera") ,false);
   
    // Use copy constructor
    CDeviceConfiguration *DeviceConfig2(DeviceConfig1);
	
    // now test DeviceConfig
    LangList3.clear();
    LangList3 = DeviceConfig1->GetLanguageList();
    LangList2.clear();
    LangList2 = DeviceConfig1->GetLanguageList();
   // QCOMPARE(DeviceConfig1->GetLanguage(), DeviceConfig2->GetLanguage());
    QCOMPARE(DeviceConfig1->GetValue("DeviceName") , DeviceConfig2->GetValue("DeviceName") );
    QCOMPARE(DeviceConfig1->GetValue("SerialNumber") , DeviceConfig2->GetValue("SerialNumber") );
    QCOMPARE(DeviceConfig1->GetBoolValue("WorkstationMode") , DeviceConfig2->GetBoolValue("WorkstationMode") );
    QCOMPARE(DeviceConfig1->GetBoolValue("HeatedCuevettes") , DeviceConfig2->GetBoolValue("HeatedCuevettes") );
    QCOMPARE(DeviceConfig1->GetBoolValue("SlideIdCamera") , DeviceConfig2->GetBoolValue("SlideIdCamera") );
    
}

/****************************************************************************/
void TestDeviceConfiguration::utTestWriteReadDeviceConfigurationInterface() {

    CDeviceConfigurationInterface DeviceConfigInterface;

    QCOMPARE(DeviceConfigInterface.Read(RESOURCE_FILENAME), true);

    DeviceConfigInterface.SetDataVerificationMode(false);
    QCOMPARE(DeviceConfigInterface.Read(RESOURCE_FILENAME), true);
    DeviceConfigInterface.SetDataVerificationMode(true);

    CDeviceConfiguration *DeviceConfig = new CDeviceConfiguration();
    DeviceConfig = DeviceConfigInterface.GetDeviceConfiguration();

    // check all the values
    //QCOMPARE(DeviceConfig->GetLanguage(),(QLocale::German));
    QCOMPARE(DeviceConfig->GetValue("DeviceName"), QString("ST 8200"));
    QCOMPARE(DeviceConfig->GetValue("SerialNumber"), QString("12345678"));
    QCOMPARE(DeviceConfig->GetValue("CoverSlipperName"), QString("CV 8020"));
    QCOMPARE(DeviceConfig->GetBoolValue("WorkstationMode"), false);
    QCOMPARE(DeviceConfig->GetBoolValue("HeatedCuevettes"), false);
    QCOMPARE(DeviceConfig->GetBoolValue("SlideIdCamera"), false);

    DeviceConfig = DeviceConfigInterface.GetDeviceConfiguration(false);
    QCOMPARE(DeviceConfig->GetBoolValue("SlideIdCamera"), false);

	
    // change all the DeviceConfig
    //DeviceConfig->SetLanguage(QLocale::English);
    DeviceConfig->SetValue("DeviceName", QString("ST 9200"));
    DeviceConfig->SetValue("SerialNumber", QString("99998888"));
    DeviceConfig->SetValue("CoverSlipperName", QString("CV 9020"));
    DeviceConfig->SetValue("WorkstationMode", true);
    DeviceConfig->SetValue("HeatedCuevettes", true);
    DeviceConfig->SetValue("SlideIdCamera", true);
	
    DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig);

    // write the DeviceConfig in to a file
    DeviceConfigInterface.Write(FilesPathWrite + "DeviceConfiguration_Test.xml");

    // change all the DeviceConfig again
   // DeviceConfig->SetLanguage(QLocale::English);
    DeviceConfig->SetValue("DeviceName", QString("ST 8200"));
    DeviceConfig->SetValue("SerialNumber", QString("44446666"));
    DeviceConfig->SetValue("CoverSlipperName", QString("CV 9999"));
    DeviceConfig->SetValue("WorkstationMode", false);
    DeviceConfig->SetValue("HeatedCuevettes", false);
    DeviceConfig->SetValue("SlideIdCamera", false);
	
    // now test DeviceConfig
    //QCOMPARE(DeviceConfig->GetLanguage(),(QLocale::English));
    QCOMPARE(DeviceConfig->GetValue("DeviceName") ,QString("ST 8200"));
    QCOMPARE(DeviceConfig->GetValue("SerialNumber") ,QString("44446666"));
    QCOMPARE(DeviceConfig->GetValue("CoverSlipperName") ,QString("CV 9999"));
    QCOMPARE(DeviceConfig->GetBoolValue("WorkstationMode") ,false);
    QCOMPARE(DeviceConfig->GetBoolValue("HeatedCuevettes") ,false);
    QCOMPARE(DeviceConfig->GetBoolValue("SlideIdCamera") ,false);
	
    // now read DeviceConfig from the file
    DeviceConfigInterface.Read(FilesPathWrite + "DeviceConfiguration_Test.xml");

    DeviceConfig = DeviceConfigInterface.GetDeviceConfiguration();

    // now test DeviceConfig
   // QCOMPARE(DeviceConfig->GetLanguage(),(QLocale::English));
    QCOMPARE(DeviceConfig->GetValue("DeviceName") ,QString("ST 9200"));
    QCOMPARE(DeviceConfig->GetValue("SerialNumber") ,QString("99998888"));
    QCOMPARE(DeviceConfig->GetValue("CoverSlipperName") ,QString("CV 9020"));
    QCOMPARE(DeviceConfig->GetBoolValue("WorkstationMode") ,true);
    QCOMPARE(DeviceConfig->GetBoolValue("HeatedCuevettes") ,true);
    QCOMPARE(DeviceConfig->GetBoolValue("SlideIdCamera") ,true);
	
    // remove dummy file again
    QFile::remove(FilesPathWrite + "DeviceConfiguration_Test.xml");
	
}

void TestDeviceConfiguration::utTestWriteReadDeviceConfigurationVerifier() {

    CDeviceConfigurationInterface DeviceConfigInterface;
    IVerifierInterface *p_DeviceConfigurationVerifier;
    p_DeviceConfigurationVerifier = new CDeviceConfigurationVerifier();
    DeviceConfigInterface.AddVerifier(p_DeviceConfigurationVerifier);

    QCOMPARE(DeviceConfigInterface.Read(RESOURCE_FILENAME), true);
    // make the data verification mode to false, so that whenever it reads a file then
    // it won't verify the details again and again.
    //DeviceConfigInterface.SetDataVerificationMode(false);

    CDeviceConfiguration *DeviceConfig = DeviceConfigInterface.GetDeviceConfiguration();

    // data is modified, so reload the file from the resource
    DeviceConfigInterface.Read(RESOURCE_FILENAME);
    DeviceConfig = DeviceConfigInterface.GetDeviceConfiguration();

    //********************* Device Name ********************
    // checking the boundry values
    DeviceConfig->SetValue("DeviceName", "ST8200");
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);
    DeviceConfig->SetValue("DeviceName", "AB9999");
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);

    // checking the outside the boundry values
//    DeviceConfig->SetValue("DeviceName", "ST 8200000000000000000ABABABABABABTaaa");
//    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), false);
//    DeviceConfig->SetValue("DeviceName", "");
//    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), false);
 
    // data is modified, so reload the file from the resource
    DeviceConfigInterface.Read(RESOURCE_FILENAME);
    DeviceConfig = DeviceConfigInterface.GetDeviceConfiguration();

    //********************* Stainer Serial Number ********************
    // checking the boundry values
    DeviceConfig->SetValue("SerialNumber", "12");
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);
    DeviceConfig->SetValue("SerialNumber", "AA");
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);

    // checking the outside the boundry values
    DeviceConfig->SetValue("SerialNumber", "12088200000000000000000ABABABABABABT");
//    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), false);


    // data is modified, so reload the file from the resource
    DeviceConfigInterface.Read(RESOURCE_FILENAME);
    DeviceConfig = DeviceConfigInterface.GetDeviceConfiguration();
	
    //********************* Coverslipper Device Name ********************
    // checking the boundry values
    DeviceConfig->SetValue("CoverSlipperName", "CV 8200");
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);
    DeviceConfig->SetValue("CoverSlipperName", "CV9999");
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);
   
    // checking the outside the boundry values
    DeviceConfig->SetValue("CoverSlipperName", "CV 8200000000000000000ABABABABABABT");
//    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), false);

  
    // data is modified, so reload the file from the resource
    DeviceConfigInterface.Read(RESOURCE_FILENAME);
    DeviceConfig = DeviceConfigInterface.GetDeviceConfiguration();
	
    //********************* Workstation availability ********************
    // checking the boundry values
    DeviceConfig->SetValue("WorkstationMode", true);
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);
    DeviceConfig->SetValue("WorkstationMode", false);
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);
   
    // checking the outside the boundry values
    DeviceConfig->SetValue("WorkstationMode", -0);
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);
    DeviceConfig->SetValue("WorkstationMode", -1);
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);

    // data is modified, so reload the file from the resource
    DeviceConfigInterface.Read(RESOURCE_FILENAME);
    DeviceConfig = DeviceConfigInterface.GetDeviceConfiguration();
	
    //********************* Heated Cuevettes availability ********************
    // checking the boundry values
    DeviceConfig->SetValue("HeatedCuevettes", true);
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);
    DeviceConfig->SetValue("HeatedCuevettes", false);
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);

   
    // checking the outside the boundry values
    DeviceConfig->SetValue("HeatedCuevettes", -0);
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);
    DeviceConfig->SetValue("HeatedCuevettes", -1);
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);
  
    // data is modified, so reload the file from the resource
    DeviceConfigInterface.Read(RESOURCE_FILENAME);
    DeviceConfig = DeviceConfigInterface.GetDeviceConfiguration();
	
    //********************* Camera Slide availability ********************
    // checking the boundry values
    DeviceConfig->SetValue("SlideIdCamera", true);
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);
    DeviceConfig->SetValue("SlideIdCamera", false);
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);

   
    // checking the outside the boundry values
    DeviceConfig->SetValue("SlideIdCamera", -0);
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);
    DeviceConfig->SetValue("SlideIdCamera", -1);
    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), true);
     
    // data is modified, so reload the file from the resource
    DeviceConfigInterface.Read(RESOURCE_FILENAME);
    DeviceConfig = DeviceConfigInterface.GetDeviceConfiguration();
	

    // checking some random scenarios 
    DeviceConfig->SetValue("DeviceName", QString("1234567891011212131415161718192021"));
    DeviceConfig->SetValue("SerialNumber", QString("1234567891011212131415161718192021"));
    DeviceConfig->SetValue("CoverSlipperName", QString("1234567891011212131415161718192021"));
    // update the DeviceConfig in the wrapper class
//    QCOMPARE(DeviceConfigInterface.UpdateDeviceConfiguration(DeviceConfig), false);

}

} // end namespace DataManagement

QTEST_MAIN(DataManager::TestDeviceConfiguration)

#include "TestDeviceConfiguration.moc"
