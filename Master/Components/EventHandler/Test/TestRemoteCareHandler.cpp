/****************************************************************************/
/*! \file TestActionHandler.cpp
 *
 *  \brief Implementation file for class TestActionHandler.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-03-03
 *  $Author:    $ Aparna
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
#include <DataLogging/Include/DayEventEntry.h>
#include <NetCommands/Include/CmdEventReport.h>
#include "EventHandler/Include/RemoteCareHandler.h"
#include <../../../Global/Include/Utils.h>
#include <../../../EventHandler/Include/EventHandlerThreadController.h>
#include <../../../NetCommands/Include/CmdSystemAction.h>

namespace EventHandler {

//class Foo : public RemoteCareHandler {
//	Q_OBJECT
//public:
//	Foo():RemoteCareHandler(3)
//	{
//	}

//	virtual ~Foo()
//	{
//	}

//public slots:
//	virtual void ForwardEventToRemoteCare(const DataLogging::DayEventEntry &a)
//	{

//	}
//};

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFile class.
 */
/****************************************************************************/
class TestRemoteCareHandler : public QObject {
    Q_OBJECT

public:
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
}; // end class TestRemoteCareHandler

/****************************************************************************/
void TestRemoteCareHandler::initTestCase()
{
//	Foo foo;
}

/****************************************************************************/
void TestRemoteCareHandler::init()
{
}

/****************************************************************************/
void TestRemoteCareHandler::cleanup()
{
}

/****************************************************************************/
void TestRemoteCareHandler::cleanupTestCase()
{
}

} // end namespace EventHandler

QTEST_MAIN(EventHandler::TestRemoteCareHandler)

#include "TestRemoteCareHandler.moc"
