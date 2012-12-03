/****************************************************************************/
/*! \file TestMain.cpp
 *
 *  \brief Implementation file for class TestMain.
 *
 *  \b Description:
 *         Checks the Main class implementation
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


namespace PlatformExport {

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFileUserSettings class.
 */
/****************************************************************************/
class TestMain : public QObject {
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
    void utTestMain();

}; // end class TestMain

/****************************************************************************/
void TestMain::initTestCase() {

}

/****************************************************************************/
void TestMain::init() {
}

/****************************************************************************/
void TestMain::cleanup() {
}

/****************************************************************************/
void TestMain::cleanupTestCase() {
}

/****************************************************************************/
void TestMain::utTestMain() {
    
}

} // end namespace PlatformExport

QTEST_MAIN(PlatformExport::TestMain)

#include "TestMain.moc"
