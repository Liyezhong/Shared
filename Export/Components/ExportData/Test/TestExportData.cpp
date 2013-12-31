/****************************************************************************/
/*! \file TestExportData.cpp
 *
 *  \brief Implementation file for class TestExportData.
 *
 *  \b Description:
 *         Checks the CExportData class implementation
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2012-08-10
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
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "ExportData/Include/ExportData.h"
#include "ExportData/Include/Startup.h"
#include "Global/Include/GlobalExitCodes.h"


namespace Export {

static QString FilesPathWrite;  ///< Path to where we can write some files.
//const QString RESOURCE_FILENAME = ":/Xml/ExportConfiguration.xml"; ///< Resource file path

/****************************************************************************/
/**
 * \brief Test class for ExportConfiguration.xml class.
 */
/****************************************************************************/
class TestExportData : public QObject {
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
    void utTestExportData();

}; // end class TestExportData

/****************************************************************************/
void TestExportData::initTestCase() {

}

/****************************************************************************/
void TestExportData::init() {
}

/****************************************************************************/
void TestExportData::cleanup() {
}

/****************************************************************************/
void TestExportData::cleanupTestCase() {
}

/****************************************************************************/
void TestExportData::utTestExportData() {
    CExportData Export;
    CStartup StartUp;

// For No Temporary folder in the Test
    QCOMPARE(Export.CreateArchiveFiles(), Global::EXIT_CODE_EXPORT_UNABLE_TO_READ_FILE_TEMP_EXPORTCONFIGURATION);
    QCOMPARE(Export.CreateArchiveFiles(), StartUp.Archive());

 //For Temporary/Export/TempExportConfiguration
    QCOMPARE(Export.CreateArchiveFiles(), Global::EXIT_CODE_EXPORT_UNABLE_TO_READ_FILE_TEMP_EXPORTCONFIGURATION);
    QCOMPARE(Export.CreateArchiveFiles(), StartUp.Archive());

//For Wrong FileName inside Temporary/Export
    QCOMPARE(Export.CreateArchiveFiles(), Global::EXIT_CODE_EXPORT_UNABLE_TO_READ_FILE_TEMP_EXPORTCONFIGURATION);
    QCOMPARE(Export.CreateArchiveFiles(), StartUp.Archive());

}

} // end namespace PlatformExport

QTEST_MAIN(Export::TestExportData)

#include "TestExportData.moc"
