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
#include <qdom.h>
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
        : mp_alarmHandler(NULL)
    {

    }

private:

    EventHandler::HimalayaEventHandlerThreadController * mp_HimalayaEventHandlerThreadController; ///< Pointer to EventHandlerThreadController
    DataLogging::DataLoggingThreadController * mp_DataLoggingThreadController; ///< Pointer to DataLoggingThreadController
    QString                                     m_EventLoggerBaseFileName; ///< EventLoggerBase FileName
    Global::AlarmHandler * mp_alarmHandler; ///< Pointer to AlarmHandler
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
}

void TestHimalayaEventHandlerThreadController::TestProcessEvents(){

    mp_HimalayaEventHandlerThreadController->ConnectToEventObject();
    Global::EventObject::Instance().RaiseEvent(0,500030001,200,true);
    QCOMPARE(mp_HimalayaEventHandlerThreadController->m_ActiveEvents.size(),1);
}

void TestHimalayaEventHandlerThreadController::TestReadEventConfigurationFile() {

}

/****************************************************************************/
void TestHimalayaEventHandlerThreadController::initTestCase() {


    Global::EventObject::Instance();
    m_EventLoggerBaseFileName = "Base";
    mp_DataLoggingThreadController = new DataLogging::DataLoggingThreadController(THREAD_ID_DATALOGGING, m_EventLoggerBaseFileName);
    Global::SystemPaths::Instance().SetSettingsPath("../../../../../../Himalaya/HimalayaMain/Master/Components/Main/Build/Settings/");
    mp_HimalayaEventHandlerThreadController = new EventHandler::HimalayaEventHandlerThreadController(THREAD_ID_EVENTHANDLER, 0,
                                                                    QStringList() << Global::SystemPaths::Instance().GetSettingsPath() + QDir::separator() + "EventConfigure.xml");
    mp_HimalayaEventHandlerThreadController->CreateAndInitializeObjects();



}

/****************************************************************************/
void TestHimalayaEventHandlerThreadController::init()
{
}
/****************************************************************************/
void TestHimalayaEventHandlerThreadController::cleanup() {


}


/****************************************************************************/
void TestHimalayaEventHandlerThreadController::cleanupTestCase() {

    if(mp_HimalayaEventHandlerThreadController) {
        delete mp_HimalayaEventHandlerThreadController;
        mp_HimalayaEventHandlerThreadController = NULL;
    }

    if(mp_alarmHandler) {
        delete mp_alarmHandler;
        mp_alarmHandler = NULL;
    }

    if(mp_DataLoggingThreadController) {
        delete mp_DataLoggingThreadController;
        mp_DataLoggingThreadController = NULL;
    }
}


} // end namespace EventHandler

QTEST_MAIN(EventHandler::TestHimalayaEventHandlerThreadController)

#include "TestHimalayaEventHandlerThreadController.moc"
