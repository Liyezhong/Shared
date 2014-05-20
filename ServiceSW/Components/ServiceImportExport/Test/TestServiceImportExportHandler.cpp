    /****************************************************************************/
/*! \file TestServiceImportExportHandler.cpp
 *
 *  \brief Implementation file for class TestServiceImportExportHandler.
 *
 *  \b Description:
 *         Checks the CTestServiceImportExportHandler class implementation
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-10-17
 *  $Author:    $ Soumya. D
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
#include "MainMenu/Include/MainWindow.h"
#include "ServiceImportExport/Include/ServiceImportExportHandler.h"


namespace ImportExport {

/****************************************************************************/
/**
 * \brief Test class for TestServiceImportExportHandler class.
 */
/****************************************************************************/
class CTestServiceImportExportHandler : public QObject {
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
    void TestServiceImportExportHandler();

}; // end class TestExportController

/****************************************************************************/
void CTestServiceImportExportHandler::initTestCase() {

}

/****************************************************************************/
void CTestServiceImportExportHandler::init() {
}

/****************************************************************************/
void CTestServiceImportExportHandler::cleanup() {
}

/****************************************************************************/
void CTestServiceImportExportHandler::cleanupTestCase() {
}

/****************************************************************************/
void CTestServiceImportExportHandler::TestServiceImportExportHandler() {
    Global::SystemPaths::Instance().SetComponentTestPath("../Tests");
    Global::SystemPaths::Instance().SetFirmwarePath("../Firmware");
    Global::SystemPaths::Instance().SetLogfilesPath("../Logfiles");
    Global::SystemPaths::Instance().SetManualPath("../Manual");
    Global::SystemPaths::Instance().SetSettingsPath("../Settings");
    Global::SystemPaths::Instance().SetUpdatePath("../Update");
    Global::SystemPaths::Instance().SetUploadsPath("../Uploads");
    Global::SystemPaths::Instance().SetTempPath("../Temporary");
    Global::SystemPaths::Instance().SetRollbackPath("../Rollback");
    Global::SystemPaths::Instance().SetTranslationsPath("../TranslationsService");
    Global::SystemPaths::Instance().SetSoundPath("../Sounds");
    Global::SystemPaths::Instance().SetInstrumentSettingsPath("../Settings/Instrument");

    MainMenu::CMainWindow *p_MainWindow = new MainMenu::CMainWindow();

    //Core::CServiceGUIConnector *p_DataConnector = new Core::CServiceGUIConnector(p_MainWindow);
    DataManager::CDeviceConfigurationInterface *p_DeviceConfigInterface = new DataManager::CDeviceConfigurationInterface();
    ImportExport::CServiceImportExportHandler *p_ImportExportHandler =
            new ImportExport::CServiceImportExportHandler(p_DeviceConfigInterface, "Export", "Service");

    p_ImportExportHandler->CreateAndInitializeObjects();
    QCOMPARE(p_ImportExportHandler->DoPretasks(), false);
    p_ImportExportHandler->CreateRequiredDirectories();

    EncryptionDecryption::RAMFile RamFile;
    QCOMPARE(p_ImportExportHandler->WriteFilesAndImportData("UnknownImport", QStringList(), RamFile), true);

    DataManager::CConfigurationList ConfigurationList;
    ConfigurationList.SetGroupFileName("testfolder");

    QCOMPARE(p_ImportExportHandler->CopyConfigurationFiles(ConfigurationList, "testfolder"), false);
    QDir Directory("testfolder");
    Directory.rmdir(Directory.absolutePath());
    QCOMPARE(p_ImportExportHandler->CheckTheExistenceAndCopyFile("ConfigurationList", "checkfolder"), false);
    QCOMPARE(p_ImportExportHandler->ImportArchiveFiles("Service", "Importfolder"), false);

    QCOMPARE(p_ImportExportHandler->CheckForFilesUpdateError(), false);
    QCOMPARE(p_ImportExportHandler->UpdateSettingsWithRollbackFolder(), false);
    QCOMPARE(p_ImportExportHandler->MountDevice(), false);
    QCOMPARE(p_ImportExportHandler->UpdateFolderWithFiles(QStringList(), "TargetPath", "SourcePath"), true);
    QCOMPARE(p_ImportExportHandler->WriteFilesInSettingsFolder(), false);

    p_ImportExportHandler->UnMountDevice();
    p_ImportExportHandler->StartImportingFiles(QStringList());
    p_ImportExportHandler->DoCleanUpObjects();

    ImportExport::CServiceImportExportHandler *p_ServiceImportExport = new
            ImportExport::CServiceImportExportHandler();

    QCOMPARE(p_ServiceImportExport->MountDevice(), false);
    p_ServiceImportExport->AddFilesForImportType("Service", QStringList());
    p_ServiceImportExport->UnMountDevice();
    p_ServiceImportExport->CleanupDestroyObjects();

    delete p_ServiceImportExport;
}

} // end namespace ImportExport

QTEST_MAIN(ImportExport::CTestServiceImportExportHandler)

#include "TestServiceImportExportHandler.moc"
