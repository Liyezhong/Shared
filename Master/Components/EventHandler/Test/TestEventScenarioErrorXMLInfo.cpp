/****************************************************************************/
/*! \file TestEventScenarioErrorXMLInfo.cpp
 *
 *  \brief Implementation file for class TestEventScenarioErrorXMLInfo.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2014-2-1
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
#include <EventHandler/Include/EventScenarioErrorXMLInfo.h>

namespace EventHandler {

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFile class.
 */
/****************************************************************************/
class TestEventScenarioErrorXMLInfo: public QObject {
    Q_OBJECT

private:
    QSharedPointer<EventScenarioErrXMLInfo> m_pESEXMLInfo;
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


void TestEventScenarioErrorXMLInfo::UTAll()
{
    QHash< quint32,QSharedPointer<ESEInfo> > ESEInfoList = m_pESEXMLInfo->GetHashESEInfoList();
    QHash< quint32,QSharedPointer<ESEInfo> >::iterator iter = ESEInfoList.begin();
    while (iter != ESEInfoList.end())
    {
     // qDebug()<<"key is:"<<iter.key();
      //qDebug()<<"size of scenario/error pair is:"<<iter.value()->getScenarioErrorList().size();
      iter++;
    }
    //qDebug()<<"size of ESEInfo list is:"<<m_pESEXMLInfo->getESEInfoList().size();

    //"single" type
    quint32 errorId = m_pESEXMLInfo->GetErrorCode(500030001, 200);
    quint32 id = 513030001;
    QCOMPARE(errorId, id);

    //"range" type
    errorId = m_pESEXMLInfo->GetErrorCode(500010201, 211);
    id = 513010200;
    QCOMPARE(errorId, id);

    //"all" type
    errorId = m_pESEXMLInfo->GetErrorCode(500050001);
    id = 513050001;
    QCOMPARE(errorId, id);

    //error case;
    errorId = m_pESEXMLInfo->GetErrorCode(500030021, 250);
    id = 0;
    QCOMPARE(errorId, id);
}

/******************************************************************ls**********/
void TestEventScenarioErrorXMLInfo::initTestCase()
{
    m_pESEXMLInfo = QSharedPointer<EventScenarioErrXMLInfo>(new EventScenarioErrXMLInfo("../../../../../../Himalaya/HimalayaMain/Master/Components/Main/Build/Settings/EventScenarioErrorMap.xml"));
    bool ret = m_pESEXMLInfo->InitXMLInfo();
    QVERIFY(ret);
}

/****************************************************************************/
void TestEventScenarioErrorXMLInfo::init()
{
}

/****************************************************************************/
void TestEventScenarioErrorXMLInfo::cleanup()
{
}

/****************************************************************************/
void TestEventScenarioErrorXMLInfo::cleanupTestCase()
{

}

} // end namespace EventHandler

QTEST_MAIN(EventHandler::TestEventScenarioErrorXMLInfo)

#include "TestEventScenarioErrorXMLInfo.moc"
