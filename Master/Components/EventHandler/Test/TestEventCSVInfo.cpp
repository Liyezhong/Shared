/****************************************************************************/
/*! \file TestEventCSVInfo.cpp
 *
 *  \brief Implementation file for class TestEventCSVInfo.
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
#include <EventHandler/Include/EventCSVInfo.h>

namespace EventHandler {




/****************************************************************************/
/**
 * \brief Test class for XmlConfigFile class.
 */
/****************************************************************************/
class TestEventCSVInfo : public QObject {
    Q_OBJECT

private:
    EventHandler::EventCSVInfo m_EventCSVInfo;
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

    void UTAll();

}; // end class TestEventCSVInfo


void TestEventCSVInfo::UTAll() {
    EventHandler::EventCSVInfo theEventInfo = m_EventCSVInfo;
    QCOMPARE(theEventInfo.GetAckReqStatus(), false);
    QCOMPARE(theEventInfo.GetActionNegative(),Global::ACNTYPE_NONE);
    QCOMPARE(theEventInfo.GetActionPositive(),Global::ACNTYPE_ERROR);
    QCOMPARE(theEventInfo.GetActionType(),Global::ACNTYPE_NONE);
    QCOMPARE(theEventInfo.GetAlarmStatus(),false);
    QCOMPARE(theEventInfo.GetGUIMessageBoxOptions(),Global::YES_NO);
    //QCOMPARE(theEventInfo.GetEventCode(),1234567);
    //QCOMPARE(theEventInfo.GetEventName(),"MyEventName");
    QCOMPARE(theEventInfo.GetEventSource(),Global::EVENTSOURCE_EXPORT);
    QCOMPARE(theEventInfo.GetRetryAttempts(),1);
    QCOMPARE(theEventInfo.GetRunLogStatus(), false);
    QCOMPARE(theEventInfo.GetEventSource(), Global::EVENTSOURCE_EXPORT);
    QCOMPARE(theEventInfo.GetStatusBarIcon(),false);
    QString str = "MyEventName";
    QCOMPARE(theEventInfo.GetEventName(), str);

}

/****************************************************************************/
void TestEventCSVInfo::initTestCase() {

    m_EventCSVInfo.SetAckRequired(false);
    m_EventCSVInfo.SetActionNegative(Global::ACNTYPE_NONE);
    m_EventCSVInfo.SetActionPositive(Global::ACNTYPE_ERROR);
    m_EventCSVInfo.SetAlarmStatus(false);
    m_EventCSVInfo.SetGUIOptions(Global::YES_NO);
    m_EventCSVInfo.SetEventCode(1234567);
    m_EventCSVInfo.SetEventType(Global::EVTTYPE_ERROR);
    m_EventCSVInfo.SetLogLevel(Global::LOGLEVEL_LOW);
    m_EventCSVInfo.SetMessageType("Error");
    m_EventCSVInfo.SetFinalAction(Global::ACNTYPE_NONE);
    m_EventCSVInfo.SetRetries(1);
    m_EventCSVInfo.SetRunLogStatus(false);
    m_EventCSVInfo.SetEventSource(Global::EVENTSOURCE_EXPORT);
    m_EventCSVInfo.SetStatusIcon(false);
    m_EventCSVInfo.SetEventName("MyEventName");

}

/****************************************************************************/
void TestEventCSVInfo::init() {


}

/****************************************************************************/
void TestEventCSVInfo::cleanup() {
}

/****************************************************************************/
void TestEventCSVInfo::cleanupTestCase() {
}


 
 
 

} // end namespace EventHandler

QTEST_MAIN(EventHandler::TestEventCSVInfo)

#include "TestEventCSVInfo.moc"
