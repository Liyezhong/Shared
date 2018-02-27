/****************************************************************************/
/*! \file TestMasterConnector.cpp
 *
 *  \brief Implementation file for class TestMasterConnector.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-07-10
 *  $Author:    $ B.Stach
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

#include "TestMasterConnector.h"

#include <QTest>
#include <QSignalSpy>
#include <QMetaType>

#include <Global/Include/SystemPaths.h>
#include <Global/Include/EventObject.h>
#include <Global/Include/Exception.h>

#include <MasterConnector.h>

namespace RCAgentNamespace {

/****************************************************************************/
/**
 * \brief Constructor
 */
/****************************************************************************/
TestMasterConnector::TestMasterConnector()
{
    qRegisterMetaType<Global::AlternateEventStringUsage>("Global::AlternateEventStringUsage");
}

/****************************************************************************/
/**
 * \brief Destructor
 */
/****************************************************************************/
TestMasterConnector::~TestMasterConnector()
{

}

/****************************************************************************/
/**
 * \brief Called before first testfunction is executed.
 */
/****************************************************************************/
void TestMasterConnector::initTestCase() {

}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 */
/****************************************************************************/
void TestMasterConnector::init() {
}

/****************************************************************************/
/**
 * \brief Test raise event
 */
/****************************************************************************/
void TestMasterConnector::testEventReport() {
    MasterConnector masterConnector("","");

    QVERIFY(masterConnector.Initialize() == true);

    CONNECTSIGNALSLOTGUI(&Global::EventObject::Instance(),
                      ForwardEvent(const quint32, const Global::tTranslatableStringList &,
                                   const bool, quint32 , const Global::AlternateEventStringUsage), &masterConnector,
                      ForwardEventReportCommand(const quint32, const Global::tTranslatableStringList &,
                                   const bool, quint32 , const Global::AlternateEventStringUsage))

    QSignalSpy spy(&Global::EventObject::Instance(), SIGNAL(ForwardEvent(const quint32, const Global::tTranslatableStringList &,
                                                       const bool, quint32 , const Global::AlternateEventStringUsage)));

    Global::EventObject::Instance().RaiseEvent(23);

    QCOMPARE(spy.count(),1);
}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 */
/****************************************************************************/
void TestMasterConnector::cleanup() {
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 */
/****************************************************************************/
void TestMasterConnector::cleanupTestCase() {
}

} // end namespace RCAgentNamespace

QTEST_MAIN(RCAgentNamespace::TestMasterConnector)
