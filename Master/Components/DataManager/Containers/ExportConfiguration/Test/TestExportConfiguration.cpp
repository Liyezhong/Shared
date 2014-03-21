/****************************************************************************/
/*! \file TestExportConfiguration.cpp
 *
 *  \brief Implementation file for class TestExportConfiguration.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-28
 *  $Author:    $ Raju
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
#include "DataManager/Containers/ExportConfiguration/Include/ExportConfiguration.h"
#include "DataManager/Containers/ExportConfiguration/Include/ExportConfigurationVerifier.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"


namespace DataManager {

const QString RESOURCE_FILENAME = ":/Xml/ExportConfiguration.xml"; ///< Resource file path

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFileUserSettings class.
 */
/****************************************************************************/
class TestExportConfiguration : public QObject {
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
     * \brief Test the service configuration.
     */
    /****************************************************************************/
    void utTestServiceConfiguration();

    /****************************************************************************/
    /**
     * \brief Test the user configuration.
     */
    /****************************************************************************/
    void utTestUserConfiguration();

    /****************************************************************************/
    /**
     * \brief Test write and read Export configuration.
     */
    /****************************************************************************/
    void utTestExportConfiguration();

    /****************************************************************************/
    /**
     * \brief Test write and read Export configuration verifier.
     */
    /****************************************************************************/
    void utTestExportConfigurationVerifier();

}; // end class TestExportConfiguration

/****************************************************************************/
void TestExportConfiguration::initTestCase() {    
}

/****************************************************************************/
void TestExportConfiguration::init() {
}

/****************************************************************************/
void TestExportConfiguration::cleanup() {
}

/****************************************************************************/
void TestExportConfiguration::cleanupTestCase() {
}

/****************************************************************************/
void TestExportConfiguration::utTestServiceConfiguration() {

    CServiceConfiguration Service;

    CConfigurationList Configuration;
    // set all the attributes for the configuration
    Configuration.SetCertifiedFlag(true);
    Configuration.SetCompressedFlag(true);
    Configuration.SetEncryptionFlag(true);
    Configuration.SetGroupFileName("Logging");
    Configuration.SetGroupListFlag(true);
    Configuration.SetPackageType("native");
    Configuration.SetFileList(QStringList("File"));
    // update the configuration in serviceconfiguration
    Service.SetServiceConfigurationList(Configuration);
    // compare each item in the configuration class with service configuration class
    QCOMPARE(Configuration.GetCertifiedFlag(), Service.GetServiceConfigurationList().GetCertifiedFlag());
    QCOMPARE(Configuration.GetCompressedFlag(), Service.GetServiceConfigurationList().GetCompressedFlag());
    QCOMPARE(Configuration.GetEncryptionFlag(), Service.GetServiceConfigurationList().GetEncryptionFlag());
    QCOMPARE(Configuration.GetGroupFileName(), Service.GetServiceConfigurationList().GetGroupFileName());
    QCOMPARE(Configuration.GetGroupListFlag(), Service.GetServiceConfigurationList().GetGroupListFlag());
    QCOMPARE(Configuration.GetPackageType(), Service.GetServiceConfigurationList().GetPackageType());
    QCOMPARE(Configuration.GetFileList(), Service.GetServiceConfigurationList().GetFileList());

    // updating the each data
    Configuration.SetCertifiedFlag(false);
    Service.SetServiceConfigurationList(Configuration);
    QCOMPARE(Configuration.GetCertifiedFlag(), Service.GetServiceConfigurationList().GetCertifiedFlag());

    // check for copy constructor
    CServiceConfiguration Service1(Service);

    QCOMPARE(Service1.GetServiceConfigurationList().GetCertifiedFlag(), Service.GetServiceConfigurationList().GetCertifiedFlag());
    QCOMPARE(Service1.GetServiceConfigurationList().GetCompressedFlag(), Service.GetServiceConfigurationList().GetCompressedFlag());
    QCOMPARE(Service1.GetServiceConfigurationList().GetEncryptionFlag(), Service.GetServiceConfigurationList().GetEncryptionFlag());
    QCOMPARE(Service1.GetServiceConfigurationList().GetGroupFileName(), Service.GetServiceConfigurationList().GetGroupFileName());
    QCOMPARE(Service1.GetServiceConfigurationList().GetGroupListFlag(), Service.GetServiceConfigurationList().GetGroupListFlag());
    QCOMPARE(Service1.GetServiceConfigurationList().GetPackageType(), Service.GetServiceConfigurationList().GetPackageType());
    QCOMPARE(Service1.GetServiceConfigurationList().GetFileList(), Service.GetServiceConfigurationList().GetFileList());

    Service1.GetServiceConfigurationList().SetPackageType("nonnative");
    // check for assignment operator
    CServiceConfiguration Service2;

    Service2 = Service1;

    QCOMPARE(Service2.GetServiceConfigurationList().GetCertifiedFlag(), Service1.GetServiceConfigurationList().GetCertifiedFlag());
    QCOMPARE(Service2.GetServiceConfigurationList().GetCompressedFlag(), Service1.GetServiceConfigurationList().GetCompressedFlag());
    QCOMPARE(Service2.GetServiceConfigurationList().GetEncryptionFlag(), Service1.GetServiceConfigurationList().GetEncryptionFlag());
    QCOMPARE(Service2.GetServiceConfigurationList().GetGroupFileName(), Service1.GetServiceConfigurationList().GetGroupFileName());
    QCOMPARE(Service2.GetServiceConfigurationList().GetGroupListFlag(), Service1.GetServiceConfigurationList().GetGroupListFlag());
    QCOMPARE(Service2.GetServiceConfigurationList().GetPackageType(), Service1.GetServiceConfigurationList().GetPackageType());
    QCOMPARE(Service2.GetServiceConfigurationList().GetFileList(), Service1.GetServiceConfigurationList().GetFileList());

}


/****************************************************************************/
void TestExportConfiguration::utTestUserConfiguration() {
    CUserConfiguration User;

    CConfigurationList Configuration;
    CConfigurationList ReportConfiguration;
    // set all the attributes for the configuration
    Configuration.SetCertifiedFlag(true);
    Configuration.SetCompressedFlag(true);
    Configuration.SetEncryptionFlag(true);
    Configuration.SetGroupFileName("Logging");
    Configuration.SetGroupListFlag(false);
    Configuration.SetPackageType("native");
    Configuration.SetFileList(QStringList("File"));

    // set for report configuration
    ReportConfiguration.SetCertifiedFlag(true);
    ReportConfiguration.SetCompressedFlag(true);
    ReportConfiguration.SetEncryptionFlag(true);
    ReportConfiguration.SetGroupFileName("Logging");
    ReportConfiguration.SetGroupListFlag(true);
    ReportConfiguration.SetPackageType("nonative");
    ReportConfiguration.SetFileList(QStringList("File"));

    // update the configuration in user configuration
    User.SetUserConfigurationList(Configuration);
    // compare each item in the configuration class with user configuration class
    QCOMPARE(Configuration.GetCertifiedFlag(), User.GetUserConfigurationList().GetCertifiedFlag());
    QCOMPARE(Configuration.GetCompressedFlag(), User.GetUserConfigurationList().GetCompressedFlag());
    QCOMPARE(Configuration.GetEncryptionFlag(), User.GetUserConfigurationList().GetEncryptionFlag());
    QCOMPARE(QString(""), User.GetUserConfigurationList().GetGroupFileName());
    QCOMPARE(Configuration.GetGroupListFlag(), User.GetUserConfigurationList().GetGroupListFlag());
    QCOMPARE(Configuration.GetPackageType(), User.GetUserConfigurationList().GetPackageType());
    QCOMPARE(Configuration.GetFileList(), User.GetUserConfigurationList().GetFileList());

    // update the user report list
    User.SetUserReportList(ReportConfiguration);

    // compare each item in the report configuration class with user configuration class
    QCOMPARE(ReportConfiguration.GetCertifiedFlag(), User.GetUserReportList().GetCertifiedFlag());
    QCOMPARE(ReportConfiguration.GetCompressedFlag(), User.GetUserReportList().GetCompressedFlag());
    QCOMPARE(ReportConfiguration.GetEncryptionFlag(), User.GetUserReportList().GetEncryptionFlag());
    QCOMPARE(ReportConfiguration.GetGroupFileName(), User.GetUserReportList().GetGroupFileName());
    QCOMPARE(ReportConfiguration.GetGroupListFlag(), User.GetUserReportList().GetGroupListFlag());
    QCOMPARE(ReportConfiguration.GetPackageType(), User.GetUserReportList().GetPackageType());
    QCOMPARE(ReportConfiguration.GetFileList(), User.GetUserReportList().GetFileList());

    // updating the data
    Configuration.SetCertifiedFlag(false);
    User.SetUserConfigurationList(Configuration);
    QCOMPARE(Configuration.GetCertifiedFlag(), User.GetUserConfigurationList().GetCertifiedFlag());

    // updating report the data
    ReportConfiguration.SetCompressedFlag(false);
    ReportConfiguration.SetEncryptionFlag(false);
    User.SetUserReportList(ReportConfiguration);
    QCOMPARE(ReportConfiguration.GetCompressedFlag(), User.GetUserReportList().GetCompressedFlag());
    QCOMPARE(ReportConfiguration.GetEncryptionFlag(), User.GetUserReportList().GetEncryptionFlag());

    // check for copy constructor
    CUserConfiguration User1(User);
    // updated the configuration
    QCOMPARE(User1.GetUserConfigurationList().GetCertifiedFlag(), User.GetUserConfigurationList().GetCertifiedFlag());
    QCOMPARE(User1.GetUserConfigurationList().GetCompressedFlag(), User.GetUserConfigurationList().GetCompressedFlag());
    QCOMPARE(User1.GetUserConfigurationList().GetEncryptionFlag(), User.GetUserConfigurationList().GetEncryptionFlag());
    QCOMPARE(User1.GetUserConfigurationList().GetGroupFileName(), User.GetUserConfigurationList().GetGroupFileName());
    QCOMPARE(User1.GetUserConfigurationList().GetGroupListFlag(), User.GetUserConfigurationList().GetGroupListFlag());
    QCOMPARE(User1.GetUserConfigurationList().GetPackageType(), User.GetUserConfigurationList().GetPackageType());
    QCOMPARE(User1.GetUserConfigurationList().GetFileList(), User.GetUserConfigurationList().GetFileList());

    // compare each item in the configuration class with service configuration class
    QCOMPARE(User1.GetUserReportList().GetCertifiedFlag(), User.GetUserReportList().GetCertifiedFlag());
    QCOMPARE(User1.GetUserReportList().GetCompressedFlag(), User.GetUserReportList().GetCompressedFlag());
    QCOMPARE(User1.GetUserReportList().GetEncryptionFlag(), User.GetUserReportList().GetEncryptionFlag());
    QCOMPARE(User1.GetUserReportList().GetGroupFileName(), User.GetUserReportList().GetGroupFileName());
    QCOMPARE(User1.GetUserReportList().GetGroupListFlag(), User.GetUserReportList().GetGroupListFlag());
    QCOMPARE(User1.GetUserReportList().GetPackageType(), User.GetUserReportList().GetPackageType());
    QCOMPARE(User1.GetUserReportList().GetFileList(), User.GetUserReportList().GetFileList());

    // do some modifications in the User1
    User1.GetUserConfigurationList().SetCertifiedFlag(false);
    QStringList FileList;
    FileList << "File Name1" << "File Name 2" << "File Name 3";
    User1.GetUserConfigurationList().SetFileList(FileList);
    FileList.clear();
    FileList << "Report File Name1" << "Report File Name 2" << "Report File Name 3";
    User1.GetUserReportList().SetFileList(FileList);

    CUserConfiguration User2;
    // checkin the assignment operator
    User2 = User1;

    // updated the configuration
    QCOMPARE(User2.GetUserConfigurationList().GetCertifiedFlag(), User1.GetUserConfigurationList().GetCertifiedFlag());
    QCOMPARE(User2.GetUserConfigurationList().GetCompressedFlag(), User1.GetUserConfigurationList().GetCompressedFlag());
    QCOMPARE(User2.GetUserConfigurationList().GetEncryptionFlag(), User1.GetUserConfigurationList().GetEncryptionFlag());
    QCOMPARE(User2.GetUserConfigurationList().GetGroupFileName(), User1.GetUserConfigurationList().GetGroupFileName());
    QCOMPARE(User2.GetUserConfigurationList().GetGroupListFlag(), User1.GetUserConfigurationList().GetGroupListFlag());
    QCOMPARE(User2.GetUserConfigurationList().GetPackageType(), User1.GetUserConfigurationList().GetPackageType());
    QCOMPARE(User2.GetUserConfigurationList().GetFileList(), User1.GetUserConfigurationList().GetFileList());

    // compare each item in the configuration class with service configuration class
    QCOMPARE(User2.GetUserReportList().GetCertifiedFlag(), User1.GetUserReportList().GetCertifiedFlag());
    QCOMPARE(User2.GetUserReportList().GetCompressedFlag(), User1.GetUserReportList().GetCompressedFlag());
    QCOMPARE(User2.GetUserReportList().GetEncryptionFlag(), User1.GetUserReportList().GetEncryptionFlag());
    QCOMPARE(User2.GetUserReportList().GetGroupFileName(), User1.GetUserReportList().GetGroupFileName());
    QCOMPARE(User2.GetUserReportList().GetGroupListFlag(), User1.GetUserReportList().GetGroupListFlag());
    QCOMPARE(User2.GetUserReportList().GetPackageType(), User1.GetUserReportList().GetPackageType());
    QCOMPARE(User2.GetUserReportList().GetFileList(), User1.GetUserReportList().GetFileList());

}

/****************************************************************************/
void TestExportConfiguration::utTestExportConfiguration() {
    CExportConfiguration Export;
    CServiceConfiguration Service;
    CConfigurationList Configuration;
    CUserConfiguration User;
    CConfigurationList ReportConfiguration;
    QString FilesPathWrite;

    // store the application path to write the test files
    FilesPathWrite = QCoreApplication::applicationDirPath() + "/";

    Export.SetSourceDir("../Source");
    Export.SetTargetDir("../Target");
    Export.SetTargetFileName("../Target/dd.txt");
    Export.SetServiceConfigurationFlag(true);
    Export.SetUserConfigurationFlag(true);

    QCOMPARE(Export.GetSourceDir(), QString("../Source"));
    QCOMPARE(Export.GetTargetDir(), QString("../Target"));
    QCOMPARE(Export.GetTargetFileName(), QString("../Target/dd.txt"));
    QCOMPARE(Export.GetServiceConfigurationFlag(), true);
    QCOMPARE(Export.GetUserConfigurationFlag(), true);

    // set the service configuration
    // set all the attributes for the configuration
    Configuration.SetCertifiedFlag(true);
    Configuration.SetCompressedFlag(true);
    Configuration.SetEncryptionFlag(true);
    Configuration.SetGroupFileName("Logging");
    Configuration.SetGroupListFlag(true);
    Configuration.SetPackageType("native");
    Configuration.SetFileList(QStringList("File"));
    Service.SetServiceConfigurationList(Configuration);
    // set the service configuration
    Export.SetServiceConfiguration(Service);

    QCOMPARE(Configuration.GetCertifiedFlag(),
                Export.GetServiceConfiguration().GetServiceConfigurationList().GetCertifiedFlag());
    QCOMPARE(Configuration.GetEncryptionFlag(),
                Export.GetServiceConfiguration().GetServiceConfigurationList().GetEncryptionFlag());
    QCOMPARE(Configuration.GetCompressedFlag(),
                Export.GetServiceConfiguration().GetServiceConfigurationList().GetCompressedFlag());
    QCOMPARE(Configuration.GetGroupFileName(),
                Export.GetServiceConfiguration().GetServiceConfigurationList().GetGroupFileName());
    QCOMPARE(Configuration.GetGroupListFlag(),
                Export.GetServiceConfiguration().GetServiceConfigurationList().GetGroupListFlag());
    QCOMPARE(Configuration.GetPackageType(),
                Export.GetServiceConfiguration().GetServiceConfigurationList().GetPackageType());
    QCOMPARE(Configuration.GetFileList(),
                Export.GetServiceConfiguration().GetServiceConfigurationList().GetFileList());


    // set the user configuration
    // set all the attributes for the configuration
    Configuration.SetCertifiedFlag(true);
    Configuration.SetCompressedFlag(true);
    Configuration.SetEncryptionFlag(true);
    Configuration.SetGroupFileName("Logging");
    Configuration.SetGroupListFlag(false);
    Configuration.SetPackageType("native");
    Configuration.SetFileList(QStringList("File"));
    User.SetUserConfigurationList(Configuration);
    Export.SetUserConfiguration(User);

    QCOMPARE(Configuration.GetCertifiedFlag(),
                Export.GetUserConfiguration().GetUserConfigurationList().GetCertifiedFlag());
    QCOMPARE(Configuration.GetEncryptionFlag(),
                Export.GetUserConfiguration().GetUserConfigurationList().GetEncryptionFlag());
    QCOMPARE(Configuration.GetCompressedFlag(),
                Export.GetUserConfiguration().GetUserConfigurationList().GetCompressedFlag());
    QCOMPARE(QString(""),
                Export.GetUserConfiguration().GetUserConfigurationList().GetGroupFileName());
    QCOMPARE(Configuration.GetGroupListFlag(),
                Export.GetUserConfiguration().GetUserConfigurationList().GetGroupListFlag());
    QCOMPARE(Configuration.GetPackageType(),
                Export.GetUserConfiguration().GetUserConfigurationList().GetPackageType());
    QCOMPARE(Configuration.GetFileList(),
               Export.GetUserConfiguration().GetUserConfigurationList().GetFileList());


    CExportConfiguration Export1;
    CUserConfiguration User1;
    CConfigurationList UserConfiguration;
    QStringList FileList;
    FileList << "File.txt" << "File1.txt" << "File2.txt";
    Configuration.SetFileList(FileList);
    Configuration.SetGroupFileName("Logging");
    Configuration.SetGroupListFlag(true);
    User1.SetUserConfigurationList(Configuration);
    Export1.SetUserConfiguration(User1);
    QCOMPARE(Configuration.GetGroupFileName(),
               Export1.GetUserConfiguration().GetUserConfigurationList().GetGroupFileName());
    QCOMPARE(Configuration.GetFileList(),
                Export1.GetUserConfiguration().GetUserConfigurationList().GetFileList());

    UserConfiguration.SetFileList(QStringList("File"));
    //QCOMPARE(Configuration.GetFileList(), UserConfiguration.GetFileList());


    // set for report configuration
    ReportConfiguration.SetCertifiedFlag(true);
    ReportConfiguration.SetCompressedFlag(true);
    ReportConfiguration.SetEncryptionFlag(true);
    ReportConfiguration.SetGroupFileName("Logging");
    ReportConfiguration.SetGroupListFlag(true);
    ReportConfiguration.SetPackageType("nonative");
    ReportConfiguration.SetFileList(FileList);

    User.SetUserReportList(ReportConfiguration);
    Export.SetUserConfiguration(User);

    QCOMPARE(ReportConfiguration.GetCertifiedFlag(),
                Export.GetUserConfiguration().GetUserReportList().GetCertifiedFlag());
    QCOMPARE(ReportConfiguration.GetEncryptionFlag(),
                Export.GetUserConfiguration().GetUserReportList().GetEncryptionFlag());
    QCOMPARE(ReportConfiguration.GetCompressedFlag(),
                Export.GetUserConfiguration().GetUserReportList().GetCompressedFlag());
    QCOMPARE(ReportConfiguration.GetGroupFileName(),
                Export.GetUserConfiguration().GetUserReportList().GetGroupFileName());
    QCOMPARE(ReportConfiguration.GetGroupListFlag(),
                Export.GetUserConfiguration().GetUserReportList().GetGroupListFlag());
    QCOMPARE(ReportConfiguration.GetPackageType(),
                Export.GetUserConfiguration().GetUserReportList().GetPackageType());
    QCOMPARE(ReportConfiguration.GetFileList(),
                Export.GetUserConfiguration().GetUserReportList().GetFileList());

    Export.Write(FilesPathWrite + "ExportConfiguration_Test.xml");
    CExportConfiguration Export2;

    // checkin the assignment operator
    Export2 = Export;

    // updated the configuration
    QCOMPARE(Export2.GetUserConfiguration().GetUserConfigurationList().GetCertifiedFlag(),
                Export.GetUserConfiguration().GetUserConfigurationList().GetCertifiedFlag());
    QCOMPARE(Export2.GetUserConfiguration().GetUserConfigurationList().GetCompressedFlag(),
                Export.GetUserConfiguration().GetUserConfigurationList().GetCompressedFlag());
    QCOMPARE(Export2.GetUserConfiguration().GetUserConfigurationList().GetEncryptionFlag(),
                Export.GetUserConfiguration().GetUserConfigurationList().GetEncryptionFlag());
    QCOMPARE(Export2.GetUserConfiguration().GetUserConfigurationList().GetGroupFileName(),
                Export.GetUserConfiguration().GetUserConfigurationList().GetGroupFileName());
    QCOMPARE(Export2.GetUserConfiguration().GetUserConfigurationList().GetGroupListFlag(),
                Export.GetUserConfiguration().GetUserConfigurationList().GetGroupListFlag());
    QCOMPARE(Export2.GetUserConfiguration().GetUserConfigurationList().GetPackageType(),
                Export.GetUserConfiguration().GetUserConfigurationList().GetPackageType());
    QCOMPARE(Export2.GetUserConfiguration().GetUserConfigurationList().GetFileList(),
                Export.GetUserConfiguration().GetUserConfigurationList().GetFileList());

    // compare each item in the configuration class with service configuration class
    QCOMPARE(Export2.GetUserConfiguration().GetUserReportList().GetCertifiedFlag(),
                Export.GetUserConfiguration().GetUserReportList().GetCertifiedFlag());
    QCOMPARE(Export2.GetUserConfiguration().GetUserReportList().GetCompressedFlag(),
                Export.GetUserConfiguration().GetUserReportList().GetCompressedFlag());
    QCOMPARE(Export2.GetUserConfiguration().GetUserReportList().GetEncryptionFlag(),
                Export.GetUserConfiguration().GetUserReportList().GetEncryptionFlag());
    QCOMPARE(Export2.GetUserConfiguration().GetUserReportList().GetGroupFileName(),
                Export.GetUserConfiguration().GetUserReportList().GetGroupFileName());
    QCOMPARE(Export2.GetUserConfiguration().GetUserReportList().GetGroupListFlag(),
                Export.GetUserConfiguration().GetUserReportList().GetGroupListFlag());
    QCOMPARE(Export2.GetUserConfiguration().GetUserReportList().GetPackageType(),
                Export.GetUserConfiguration().GetUserReportList().GetPackageType());
    QCOMPARE(Export2.GetUserConfiguration().GetUserReportList().GetFileList(),
                Export.GetUserConfiguration().GetUserReportList().GetFileList());

    QFile::remove(FilesPathWrite + "ExportConfiguration_Test.xml");

}

/****************************************************************************/
void TestExportConfiguration::utTestExportConfigurationVerifier() {
    IVerifierInterface *ConfigVerifier;

    ConfigVerifier = new CExportConfigurationVerifier();

    CExportConfiguration Export;
    CConfigurationList Configuration;
    CUserConfiguration User;
    CServiceConfiguration Service;
    CConfigurationList ReportConfiguration;
    Export.SetSourceDir("../Source");
    Export.SetTargetDir("../Target");
    Export.SetTargetFileName("../Target/dd.txt");
    Export.SetServiceConfigurationFlag(true);
    Export.SetUserConfigurationFlag(true);
    QCOMPARE(ConfigVerifier->VerifyData(&Export), true);

    CExportConfiguration Export1;
    Export1.SetSourceDir("../Sggg");
    Export1.SetTargetDir("../Targhh");
    Export1.SetTargetFileName("../ff/kghkg");
    Export1.SetServiceConfigurationFlag(true);
    Export1.SetUserConfigurationFlag(true);

    QCOMPARE(ConfigVerifier->VerifyData(&Export1), false);

    QStringList FileList;
    FileList << "File.txt" << "File1.txt" << "File2.txt";
    Configuration.SetFileList(FileList);
    //Configuration.SetGroupFileName("Logging");
    Configuration.SetGroupListFlag(true);
    User.SetUserConfigurationList(Configuration);
    Export.SetUserConfiguration(User);
    QCOMPARE(ConfigVerifier->VerifyData(&Export), true);
    QCOMPARE(ConfigVerifier->VerifyData(&Export1), false);

    Service.SetServiceConfigurationList(Configuration);
    Export1.SetServiceConfiguration(Service);
    QCOMPARE(ConfigVerifier->VerifyData(&Export1), false);
    QCOMPARE(ConfigVerifier->VerifyData(&Export), true);

    ReportConfiguration.SetFileList(FileList);
    User.SetUserReportList(ReportConfiguration);
    Export.SetUserConfiguration(User);
    QCOMPARE(ConfigVerifier->VerifyData(&Export1), false);
    QCOMPARE(ConfigVerifier->VerifyData(&Export), true);


}

} // end namespace DataManagement

QTEST_MAIN(DataManager::TestExportConfiguration)

#include "TestExportConfiguration.moc"
