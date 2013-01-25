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
#include <EventHandler/Include/EventHandlerThreadController.h>
#include <DataLogging/Include/DataLoggingThreadController.h>
#include <Global/Include/AlarmHandler.h>

#include <Global/Include/GlobalDefines.h>
#include <DataLogging/Include/DayEventEntry.h>
#include <Threads/Include/ThreadController.h>
#include <DataLogging/Include/DataLoggingThreadController.h>
#include <EventHandler/Include/ErrorHandler.h>
#include <Global/Include/AlarmHandler.h>
#include <Global/Include/EventTranslator.h>
#include <Global/Include/UITranslator.h>
#include <Global/Include/Commands/AckOKNOK.h>
#include <DataManager/Containers/UserSettings/Include/UserSettingsInterface.h>
#include <NetCommands/Include/CmdAcknEventReport.h>
#include <EventHandler/Include/ActionHandler.h>

#include <Threads/Include/CommandChannel.h>
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
class TestEventHandlerThreadController : public QObject {
    Q_OBJECT

private:

    Global::gSourceType m_HeartBeatSourceEventHandler;
    Global::gSourceType m_HeartBeatSourceDataLogging;
    EventHandler::EventHandlerThreadController * mp_EventHandlerThreadController;
    DataLogging::DataLoggingThreadController * mp_DataLoggingThreadController;
    QString                                     m_EventLoggerBaseFileName;
    Global::AlarmHandler * mp_alarmHandler;
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

    void TestReadEventConfigurationFile();

    void TestProcessEvents();

    void TestOnAcknowledge();

    void TestUpdateEventKeyCountMap();

}; // end class TestXmlConfigFile

void TestEventHandlerThreadController::TestUpdateEventKeyCountMap()
{
//    int count = mp_EventHandlerThreadController->m_EventKeyCountMap.count();
//    mp_EventHandlerThreadController->UpdateEventKeyCountMap(1,true);
//    mp_EventHandlerThreadController->UpdateEventKeyCountMap(1,true);
//    mp_EventHandlerThreadController->UpdateEventKeyCountMap(1,true);
//    quint32 val =  mp_EventHandlerThreadController->m_EventKeyCountMap.value(1);
//    QCOMPARE(mp_EventHandlerThreadController->m_EventKeyCountMap.count(), count+1);
//    mp_EventHandlerThreadController->UpdateEventKeyCountMap(1,false);
//    QCOMPARE(mp_EventHandlerThreadController->m_EventKeyCountMap.count(), count);
//    val =  mp_EventHandlerThreadController->m_EventKeyCountMap.value(1);
    //qDebug()<<val<<"after false";

}

void TestEventHandlerThreadController::TestOnAcknowledge() {
}

void TestEventHandlerThreadController::TestProcessEvents(){

    // use signal spy to attach the signals here

    const char * regstr = "DataLogging::DayEventEntry";

    qRegisterMetaType<DataLogging::DayEventEntry>(regstr);

    QSignalSpy spyLogEvent(mp_EventHandlerThreadController, SIGNAL(LogEventEntry(DataLogging::DayEventEntry)));

    //QSignalSpy spyForwardToErrorHandler(mp_EventHandlerThreadController, SIGNAL(ForwardToErrorHandler(DataLogging::DayEventEntry,quint32)));

    //QSignalSpy spyGuiAvailability( mp_EventHandlerThreadController,SIGNAL(GuiAvailability(bool)));


    QHash<quint32, EventHandler::EventCSVInfo>::iterator i;


    int cntlogevent = 0;

    int cntactionhandler = 0;

    for (i = mp_EventHandlerThreadController->m_eventList.begin(); i != mp_EventHandlerThreadController->m_eventList.end(); ++i)
    {

        EventHandler::EventCSVInfo EventInfo = i.value();

        if(EventInfo.GetLogLevel() != Global::LOGLEVEL_NONE)
        {
            cntlogevent += 1;
        }

        cntactionhandler += 1;
        Global::EventObject::Instance().RaiseEvent(EventInfo.GetEventCode(), Global::FmtArgs(), true, EventInfo.GetEventCode());

    }


    //verify if the signals have been called correctly

    //QCOMPARE(spyGuiAvailability.count(),1);
    //qDebug()<< "count for spyGuiAvailability"<<spyGuiAvailability.count();
    //QCOMPARE(spyLogEvent.count(), cntlogevent -4);
    qDebug()<< "count for LogEvent"<<spyLogEvent.count()-4;
    //QCOMPARE(spyForwardToErrorHandler.count(), cntactionhandler-4);
    //qDebug()<< "count for spyForwardToErrorHandler"<<spyForwardToErrorHandler.count()-4;

}

void TestEventHandlerThreadController::TestReadEventConfigurationFile() {

    QDir dir;
    QString strfilepath = dir.absolutePath();
    strfilepath += "/EventConfig.csv";
    mp_EventHandlerThreadController->ReadConfigFile(strfilepath);
    //QCOMPARE(mp_EventHandlerThreadController->m_eventList.count(), 481);

}

/****************************************************************************/
void TestEventHandlerThreadController::initTestCase() {
    const Global::gSourceType EVENT_GROUP_SOURCENAME  = 0x0000;
    const Global::gSourceType HEARTBEAT_SOURCE_EVENTHANDLER = EVENT_GROUP_SOURCENAME + 0x0003;
    m_HeartBeatSourceDataLogging = EVENT_GROUP_SOURCENAME + 0x0002;
    m_HeartBeatSourceEventHandler =  HEARTBEAT_SOURCE_EVENTHANDLER;

    m_EventLoggerBaseFileName = "Base";
    mp_DataLoggingThreadController = new DataLogging::DataLoggingThreadController(m_HeartBeatSourceDataLogging, m_EventLoggerBaseFileName);

    mp_alarmHandler = new Global::AlarmHandler(5000);

    mp_EventHandlerThreadController = new EventHandler::EventHandlerThreadController(m_HeartBeatSourceEventHandler);
    mp_EventHandlerThreadController->SetAlarmHandler(mp_alarmHandler);

    Global::EventObject *p_EventObject  = &Global::EventObject::Instance();

    CONNECTSIGNALSLOT(p_EventObject, ForwardEvent(const quint32, const Global::tTranslatableStringList &, const bool, const quint32),
                      mp_EventHandlerThreadController, ProcessEvent(const quint32, const Global::tTranslatableStringList &, const bool, const quint32));

    //mp_EventHandlerThreadController->CreateAndInitializeObjects();

    CONNECTSIGNALSLOT(mp_EventHandlerThreadController, LogEventEntry(const DataLogging::DayEventEntry &),
                      mp_DataLoggingThreadController, SendToDayEventLogger(const DataLogging::DayEventEntry &));

    //FOR ACKNOWLEDGEMENT testing

    mp_CommandChannelEventThread = new Threads::CommandChannel(mp_EventHandlerThreadController, "EventHandler", Global::EVENTSOURCE_EVENTHANDLER);

}

/****************************************************************************/
void TestEventHandlerThreadController::init()
{

}
/****************************************************************************/
void TestEventHandlerThreadController::cleanup() {


}


/****************************************************************************/
void TestEventHandlerThreadController::cleanupTestCase() {

    if(mp_EventHandlerThreadController) {
        delete mp_EventHandlerThreadController;
        mp_EventHandlerThreadController = NULL;
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

QTEST_MAIN(EventHandler::TestEventHandlerThreadController)

#include "TestEventHandlerThreadController.moc"
