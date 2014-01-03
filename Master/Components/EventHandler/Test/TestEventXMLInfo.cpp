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
    QCOMPARE( m_pEventXMLInfo->getEventList().size(), 2);

    //Source in each XMLEvent
    QString source = "Platform";
    QCOMPARE((m_pEventXMLInfo->getEventList())["01"]->getSource(), source);
    QCOMPARE((m_pEventXMLInfo->getEventList())["02"]->getSource(), source);

    //Code of each XMLEvent;
    QString code = "01";
    QCOMPARE((m_pEventXMLInfo->getEventList())["01"]->getCode(), code);
    code = "02";
    QCOMPARE((m_pEventXMLInfo->getEventList())["02"]->getCode(), code);

    //size of EventStep list in XMLEvent
    int size1 = m_pEventXMLInfo->getEventList()["01"]->getEventStepList().size();
    QCOMPARE(size1,6);
    int size2 = m_pEventXMLInfo->getEventList()["02"]->getEventStepList().size();
    QCOMPARE(size2,7);

    // EventStep IDs in the first XMLEvent
    for (int i=0; i<size1; i++)
    {
        QString id = (m_pEventXMLInfo->getEventList()["01"]->getEventStepList())[QString::number(i)]->getId();
        QCOMPARE(id,QString::number(i));
    }

    // EventStep IDs in the second XMLEvent
    for (int i=0; i<size2; i++)
    {
        QString id = (m_pEventXMLInfo->getEventList()["02"]->getEventStepList())[QString::number(i)]->getId();
        QCOMPARE(id,QString::number(i));
    }

    //"single" type
    QString errorId = m_pEventXMLInfo->getESEErrorCode("500030001", "520000-000200");
    QString id = "513030001";
    QCOMPARE(errorId, id);
    //"range" type
    errorId = m_pEventXMLInfo->getESEErrorCode("500010001", "520000-000211");
    id = "513010001";
    QCOMPARE(errorId, id);
    //"all" type
    errorId = m_pEventXMLInfo->getESEErrorCode("500050001");
    id = "513050001";
    QCOMPARE(errorId, id);

    //error case;
    errorId = m_pEventXMLInfo->getESEErrorCode("500030021", "520000-000276");
    id = "";
    QCOMPARE(errorId, id);

}

/******************************************************************ls**********/
void TestEventXMLInfo::initTestCase()
{
    m_pEventXMLInfo = QSharedPointer<EventXMLInfo>(new EventXMLInfo("EventConfigure.xml"));
    m_pEventXMLInfo->initXMLInfo();
    m_pEventXMLInfo->initESEXMLInfoParser("ese.xml");
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
