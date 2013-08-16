/****************************************************************************/
/*! \file TestExportController.cpp
 *
 *  \brief Implementation file for class TestExportController.
 *
 *  \b Description:
 *         Checks the CExportController class implementation
 *
 *  $Version:   $ 0.1
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
#include "ExternalProcessController/Include/ExternalProcessController.h"
#include "ExportController/Include/ExportController.h"


namespace Export {

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFileUserSettings class.
 */
/****************************************************************************/
class TestExportController : public QObject {
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
    void utTestExportController();

}; // end class TestExportController

/****************************************************************************/
void TestExportController::initTestCase() {

}

/****************************************************************************/
void TestExportController::init() {
}

/****************************************************************************/
void TestExportController::cleanup() {
}

/****************************************************************************/
void TestExportController::cleanupTestCase() {
}

/****************************************************************************/
void TestExportController::utTestExportController() {

    QString Name = EXPORT_PROCESS_NAME;
    QCOMPARE(Name, QString("Platform Export"));

    Global::gSourceType SourceType;
    ExportController *Controller = new ExportController(SourceType);

    Controller->CreateAndInitializeObjects();


    delete Controller;


}

} // end namespace Export

QTEST_MAIN(Export::TestExportController)

#include "TestExportController.moc"
