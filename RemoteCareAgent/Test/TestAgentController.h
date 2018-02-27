/****************************************************************************/
/*! \file TestAgentController.h
 *
 *  \brief Definition file for class TestAgentController.
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

#ifndef MAIN_TESTAGENTCONTROLLER_H
#define MAIN_TESTAGENTCONTROLLER_H

#include <QObject>
#include <AgentController.h>

namespace RCAgentNamespace {

/****************************************************************************/
/**
 * \brief Test class for TestAgentController class
 */
/****************************************************************************/
class TestAgentController : public QObject {
    Q_OBJECT

public:
    TestAgentController();      ///< Constructor for Test
    ~TestAgentController();     ///< Destructor for Test

private slots:
    void initTestCase();        ///< Executed before first test function
    void init();                ///< Executed before each test funcion
    void cleanup();             ///< Executeed after each test function
    void cleanupTestCase();     ///< Executed after last test function

    void testConstructor();     ///< Constructor test
    void testSignalsAndSlots(); ///< Test function for slots

}; // end class TestAgentController

} // end namespace RCAgentNamespace

#endif // MAIN_TESTAGENTCONTROLLER_H
