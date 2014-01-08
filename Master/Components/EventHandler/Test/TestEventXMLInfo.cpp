/****************************************************************************/
/*! \file TestEventXMLInfo.cpp
 *
 *  \brief Implementation file for class TestEventXMLInfo.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-27-12
 *  $Author:    $ Songtao Yu
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2013 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <QTest> 

#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>
#include <EventHandler/Include/EventXMLInfo.h>

namespace EventHandler {

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFile class.
 */
/****************************************************************************/
class TestEventXMLInfo : public QObject {
    Q_OBJECT

private:
    QSharedPointer<EventXMLInfo> m_pEventXMLInfo;
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


void TestEventXMLInfo::UTAll()
{
   // number of XMLEvent
    QCOMPARE( m_pEventXMLInfo->GetEventList().size(), 5);

    //Source in each XMLEvent
    QString source = "Platform";
    QCOMPARE((m_pEventXMLInfo->GetEventList())[513010000]->GetSource(), source);
    QCOMPARE((m_pEventXMLInfo->GetEventList())[513030001]->GetSource(), source);

    //Code of each XMLEvent;
    QString code = "03";
    QCOMPARE((m_pEventXMLInfo->GetEventList())[513010002]->GetCode(), code);
    code = "05";
    QCOMPARE((m_pEventXMLInfo->GetEventList())[513010003]->GetCode(), code);

    //"single" type
   const EventStep* step = m_pEventXMLInfo->GetEvent(500010001, 200)->GetStep(3);
   QString type = "ACT";
   QCOMPARE(step->GetType(), type);


}

/******************************************************************ls**********/
void TestEventXMLInfo::initTestCase()
{
    QStringList list;
    list.append("EventConfigure01.xml");
    list.append("EventConfigure02.xml");
    m_pEventXMLInfo = QSharedPointer<EventXMLInfo>(new EventXMLInfo(list, "ese.xml"));
    m_pEventXMLInfo->InitXMLInfo();
}

/****************************************************************************/
void TestEventXMLInfo::init()
{
}

/****************************************************************************/
void TestEventXMLInfo::cleanup()
{
}

/****************************************************************************/
void TestEventXMLInfo::cleanupTestCase()
{

}

} // end namespace EventHandler

QTEST_MAIN(EventHandler::TestEventXMLInfo)

#include "TestEventXMLInfo.moc"
