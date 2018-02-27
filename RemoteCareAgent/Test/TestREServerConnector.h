/****************************************************************************/
/*! \file TestREServerConnector.h
 *
 *  \brief Definition file for class TestREServerConnector.
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

#ifndef MAIN_TESTRESERVERCONNECTOR_H
#define MAIN_TESTRESERVERCONNECTOR_H

#include <QObject>

namespace RCAgentNamespace {

/****************************************************************************/
/**
 * \brief Test class for TestREServerConnector class
 */
/****************************************************************************/
class TestREServerConnector : public QObject {
    Q_OBJECT

public:
    TestREServerConnector();    ///< Constructor for Test
    ~TestREServerConnector();   ///< Destructor for Test

private slots:
    void initTestCase();        ///< Executed before first test function
    void init();                ///< Executed before each test funcion
    void testChangeState();     ///< Test state change
    void cleanup();             ///< Executeed after each test function
    void cleanupTestCase();     ///< Executed after last test function

}; // end class TestREServerConnector

} // end namespace RCAgentNamespace

#endif // MAIN_TESTRESERVERCONNECTOR_H
