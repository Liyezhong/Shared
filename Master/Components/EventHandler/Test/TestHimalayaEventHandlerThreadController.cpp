/****************************************************************************/
/*! \file TestEventHandlerThreadController.cpp
 *
 *  \brief Implementation file for class TestEventHandlerThreadController.
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
#include <QVector>
#include <QPair>
//#include <qdom.h>
#include <QString>
#include <QSignalSpy>

#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>
#include <EventHandler/Include/HimalayaEventHandlerThreadController.h>
#include <DataLogging/Include/DataLoggingThreadController.h>
#include <Global/Include/AlarmHandler.h>

#include <Global/Include/GlobalDefines.h>
#include <DataLogging/Include/DayEventEntry.h>
#include <Threads/Include/ThreadController.h>
#include <DataLogging/Include/DataLoggingThreadController.h>
#include <Global/Include/AlarmHandler.h>
#include <Global/Include/EventTranslator.h>
#include <Global/Include/UITranslator.h>
#include <Global/Include/Commands/AckOKNOK.h>
#include <DataManager/Containers/UserSettings/Include/UserSettingsInterface.h>
#include <NetCommands/Include/CmdAcknEventReport.h>
#include <Threads/Include/PlatformThreadIDs.h>

#include <Threads/Include/MasterThreadController.h>
#include <Global/Include/AlarmHandler.h>

using namespace Global;
using namespace Threads;

namespace EventHandler {

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFile class.
 */
/****************************************************************************/
class TestHimalayaEventHandlerThreadController : public QObject {
    Q_OBJECT

public:
    TestHimalayaEventHandlerThreadController()
    {
    }

private:

    EventHandler::HimalayaEventHandlerThreadController * mp_HimalayaEventHandlerThreadController; ///< Pointer to EventHandlerThreadController
    DataLogging::DataLoggingThreadController * mp_DataLoggingThreadController; ///< Pointer to DataLoggingThreadController
    QString                                     m_EventLoggerBaseFileName; ///< EventLoggerBase FileName
    Threads::CommandChannel    *  mp_CommandChannelEventThread;        ///< Command channel for EventHandler.
    QSignalSpy * mp_SpyCommandChannelTx;

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
     * \brief Test for ReadEventConfigurationFile
     */
    /****************************************************************************/
    void TestReadEventConfigurationFile();
    /****************************************************************************/
    /**
     * \brief Test for ProcessEvents
     */
    /****************************************************************************/
    void TestProcessEvents();
    /****************************************************************************/
    /**
     * \brief Test for OnAcknowledge
     */
    /****************************************************************************/
    void TestOnAcknowledge();


}; // end class TestXmlConfigFile



void TestHimalayaEventHandlerThreadController::TestOnAcknowledge() {
    NetCommands::CmdAcknEventReport ack;
    EventRuntimeInfo_t ActiveEvent;
    ActiveEvent.EventID = 500030001;
    ActiveEvent.CurrentStep = 2;
    ActiveEvent.EventKey = 2;
    ActiveEvent.Event = NULL;
    ActiveEvent.Event = mp_HimalayaEventHandlerThreadController->m_EventManager.GetEvent(520060003);
    QVERIFY(ActiveEvent.Event != NULL);
    mp_HimalayaEventHandlerThreadController->m_ActiveEvents.insert(2,ActiveEvent);
    mp_HimalayaEventHandlerThreadController->m_EventKeyRefMap.insert(2,2);
    mp_HimalayaEventHandlerThreadController->OnAcknowledge(2,ack);
    EventRuntimeInfo_t evt = mp_HimalayaEventHandlerThreadController->m_ActiveEvents.value(2);
    QVERIFY(evt.CurrentStep == 2);

    Global::tRefType Ref;
    Global::AckOKNOK Ack;
    mp_HimalayaEventHandlerThreadController->OnAckOKNOK(Ref, Ack);
}

void TestHimalayaEventHandlerThreadController::TestProcessEvents(){

    Global::EventObject::Instance().RaiseEvent(0,520060003,true,true);
    QCOMPARE(mp_HimalayaEventHandlerThreadController->m_ActiveEvents.size(),1);
}

void TestHimalayaEventHandlerThreadController::TestReadEventConfigurationFile() {

}

/****************************************************************************/
void TestHimalayaEventHandlerThreadController::initTestCase() {


}

/****************************************************************************/
void TestHimalayaEventHandlerThreadController::init()
{
    Global::EventObject::Instance();
    m_EventLoggerBaseFileName = "Base";
    mp_DataLoggingThreadController = new DataLogging::DataLoggingThreadController(THREAD_ID_DATALOGGING, m_EventLoggerBaseFileName);
    Global::SystemPaths::Instance().SetSettingsPath("../../../../../../Himalaya/HimalayaMain/Master/Components/Main/Build/Settings/");
    mp_HimalayaEventHandlerThreadController = new EventHandler::HimalayaEventHandlerThreadController(THREAD_ID_EVENTHANDLER, 0,
								    QStringList() << Global::SystemPaths::Instance().GetSettingsPath() + QDir::separator() + "EventConfig.xml");
    mp_HimalayaEventHandlerThreadController->CreateAndInitializeObjects();
    mp_HimalayaEventHandlerThreadController->ConnectToEventObject();

}
/****************************************************************************/
void TestHimalayaEventHandlerThreadController::cleanup()
{
}


/****************************************************************************/
void TestHimalayaEventHandlerThreadController::cleanupTestCase()
{
	if (mp_HimalayaEventHandlerThreadController) {
		mp_HimalayaEventHandlerThreadController->CleanupAndDestroyObjects();
		delete mp_HimalayaEventHandlerThreadController;
	}

	if (mp_DataLoggingThreadController)
		delete mp_DataLoggingThreadController;
}


} // end namespace EventHandler

QTEST_MAIN(EventHandler::TestHimalayaEventHandlerThreadController)

#include "TestHimalayaEventHandlerThreadController.moc"
