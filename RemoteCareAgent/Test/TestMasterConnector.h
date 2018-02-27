/****************************************************************************/
/*! \file TestMasterConnector.h
 *
 *  \brief Definition file for class TestMasterConnector.
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

#ifndef MAIN_TESTMASTERCONNECTOR_H
#define MAIN_TESTMASTERCONNECTOR_H

#include <QObject>
#include <QEventLoop>

namespace RCAgentNamespace {

/****************************************************************************/
/**
 * \brief Test class for TestMasterConnector class
 */
/****************************************************************************/
class TestMasterConnector : public QObject {
    Q_OBJECT

public:
    TestMasterConnector();    ///< Constructor for Test
    ~TestMasterConnector();   ///< Destructor for Test

private slots:
    void initTestCase();        ///< Executed before first test function
    void init();                ///< Executed before each test funcion
    void testEventReport();     ///< Test raising event
    void cleanup();             ///< Executeed after each test function
    void cleanupTestCase();     ///< Executed after last test function

}; // end class TestMasterConnector

} // end namespace RCAgentNamespace

#endif // MAIN_TESTMASTERCONNECTOR_H
