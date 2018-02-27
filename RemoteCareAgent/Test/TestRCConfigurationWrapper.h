/****************************************************************************/
/*! \file TestRCConfigurationWrapper.h
 *
 *  \brief Definition file for class TestRCConfigurationWrapper.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-08-01
 *  $Author:    $ B. Stach
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

#ifndef MAIN_TESTRCCONFIGURATIONWRAPPER_H
#define MAIN_TESTRCCONFIGURATIONWRAPPER_H

#include <QObject>

namespace RCAgentNamespace {

/****************************************************************************/
/**
 * \brief Test class for TestRCConfigurationWrapper class
 */
/****************************************************************************/
class TestRCConfigurationWrapper : public QObject {
    Q_OBJECT

public:
    TestRCConfigurationWrapper();    ///< Constructor for Test
    ~TestRCConfigurationWrapper();   ///< Destructor for Test

private slots:
    void initTestCase();            ///< Executed before first test function
    void init();                    ///< Executed before each test funcion
    void cleanup();                 ///< Executeed after each test function
    void testDefaultParameter();    ///< Test for default attribute setup
    void testReadSettings();        ///< Test for read settings
    void testSetSettings();         ///< Test for set settings
    void testStdStringToAeChar();   ///< Test string conversion to aechar
    void testONOFFToAeBool();       ///< Test enum onoffstate conversion to bool
    void testAeBoolToONOFF();       ///< Test bool conversion to enum onoffstate
    void testConvertTime();         ///< Test time conversion
    void cleanupTestCase();         ///< Executed after last test function

}; // end class TestRCConfigurationWrapper

} // end namespace RCAgentNamespace

#endif // MAIN_TESTRCCONFIGURATIONWRAPPER_H
