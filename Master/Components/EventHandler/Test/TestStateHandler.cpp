/****************************************************************************/
/*! \file TestStateHandler.cpp
 *
 *  \brief Implementation file for class TestStateHandler.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-05-15
 *  $Author:    $ Shuvasmita
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
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>
#include <EventHandler/Include/StateHandler.h>
#include <DataLogging/Include/DayEventEntry.h>
#include <NetCommands/Include/CmdEventReport.h>
#include <EventHandler/Include/StateHandler.h>
#include <../../../Global/Include/Utils.h>
#include <../../../EventHandler/Include/EventHandlerThreadController.h>
#include <../../../NetCommands/Include/CmdSystemAction.h>

namespace EventHandler {


/****************************************************************************/
/**
 * \brief Test class for StateHandler class.
 */
/****************************************************************************/
class TestStateHandler : public QObject {
    Q_OBJECT

private:
    EventHandler::StateHandler m_StateHandler; //!< instance of StateHandler
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
     * \brief Test for EventCSVInfo
     */
    /****************************************************************************/
    void UTAll();

}; // end class TestStateHandler


void TestStateHandler::UTAll() {

    EventHandler::StateHandler m_StateHandler;
    m_StateHandler.getCurrentOperationState();
    m_StateHandler.getCurrentAvailabilityState();
    m_StateHandler.getInitStageProgress();

    Global::CommandShPtr_t command;
    m_StateHandler.isAllowed(command);

}

/****************************************************************************/
void TestStateHandler::initTestCase() {

    m_StateHandler.setAvailability(true,quint32(23));
    m_StateHandler.setActivityUpdate(true,quint32(223));
    m_StateHandler.setInitState();
    m_StateHandler.setIdleState();
    m_StateHandler.setStateToSoftSwitchMonitorState();
    m_StateHandler.setInitStageProgress(quint8(1234567),true);

    m_StateHandler.onAvailabilityStateChanged();
    m_StateHandler.onOperationStateChanged();
    m_StateHandler.onInitStateEntered();
    m_StateHandler.onSoftSwitchMonitorStateEntered();
    m_StateHandler.SetInitializationFailed();
}

/****************************************************************************/
void TestStateHandler::init()
{
}

/****************************************************************************/
void TestStateHandler::cleanup()
{
}

/****************************************************************************/
void TestStateHandler::cleanupTestCase()
{
}






} // end namespace EventHandler

QTEST_MAIN(EventHandler::TestStateHandler)

#include "TestStateHandler.moc"
