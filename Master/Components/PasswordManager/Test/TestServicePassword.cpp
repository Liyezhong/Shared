/****************************************************************************/
/*! \file TestServicePassword.cpp
 *
 *  \brief Implementation file for class TestServicePassword.
 *
 *  \b Description:
 *      Checks all the information related to service passwoed. Reads the
 *      'LBSaccess.key" file and compares the information with the entered
 *      data by user.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-06
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
#include <PasswordManager/Include/ServicePassword.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include <Global/Include/Exception.h>

namespace PasswordManager {

/****************************************************************************/
/**
 * \brief Test class for PasswordManager class.
 */
/****************************************************************************/
class TestServicePassword : public QObject {
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
     * \brief Test constructors.
     */
    /****************************************************************************/
    void utConstructor();
    /****************************************************************************/
    /**
     * \brief Test Clear method.
     */
    /****************************************************************************/
    void utClear();

}; // end class TestServicePassword

/****************************************************************************/
void TestServicePassword::initTestCase() {
}

/****************************************************************************/
void TestServicePassword::init() {
}

/****************************************************************************/
void TestServicePassword::cleanup() {
}

/****************************************************************************/
void TestServicePassword::cleanupTestCase() {
}

/****************************************************************************/
void TestServicePassword::utConstructor() {    

}

/****************************************************************************/
void TestServicePassword::utClear() {

}

} // end namespace PasswordManager

QTEST_MAIN(PasswordManager::TestServicePassword)

#include "TestServicePassword.moc"
