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
#include <EventHandler/Include/EventHandlerThreadController.h>
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
class TestEventHandlerThreadController : public QObject {
    Q_OBJECT

public:
    TestEventHandlerThreadController()
        : mp_alarmHandler(NULL)
    {

    }

private:

    Global::gSourceType m_HeartBeatSourceEventHandler; ///< HeartBeatSource for EventHandler
    Global::gSourceType m_HeartBeatSourceDataLogging; ///< HeartBeatSource for DataLogging
    EventHandler::EventHandlerThreadController * mp_EventHandlerThreadController; ///< Pointer to EventHandlerThreadController
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
        Global::EventObject::Instance().RaiseEvent(EventInfo.GetEventId(), Global::FmtArgs(), true, EventInfo.GetEventId());

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


    mp_EventHandlerThreadController = new EventHandler::EventHandlerThreadController(THREAD_ID_EVENTHANDLER, 0, QStringList());


    CONNECTSIGNALSLOT(mp_EventHandlerThreadController, LogEventEntry(const DataLogging::DayEventEntry &),
                      mp_DataLoggingThreadController, SendToDayEventLogger(const DataLogging::DayEventEntry &));

    //FOR ACKNOWLEDGEMENT testing

    mp_CommandChannelEventThread = new Threads::CommandChannel(mp_EventHandlerThreadController, "EventHandler", Global::EVENTSOURCE_EVENTHANDLER);

    EventHandler::EventCSVInfo CSVInfo;
    DataLogging::DayEventEntry EventEntry ;
    Global::tTranslatableStringList EventStringList;
    Global::AlternateEventStringUsage AltStringUsage;
    Global::tRefType Ref =0;
    NetCommands::CmdAcknEventReport Ack;
    mp_EventHandlerThreadController->VerifyUserLogGUIOptionDependency(CSVInfo);
    mp_EventHandlerThreadController->VerifySourceComponentGUIOptionsDependency(CSVInfo);
    mp_EventHandlerThreadController->VerifyActionGUIOptionsDependency(CSVInfo);
    mp_EventHandlerThreadController->VerifyAlarmGUIOptionsDependency(CSVInfo);
    mp_EventHandlerThreadController->VerifyAlarmEventTypeDependency(CSVInfo);
    mp_EventHandlerThreadController->VerifyStatusbarGUIOptionDependency(CSVInfo);
    mp_EventHandlerThreadController->InformAlarmHandler(EventEntry,quint64(234),true);
  //  mp_EventHandlerThreadController->CreateEventEntry(EventEntry,CSVInfo,true,quint32 (123),EventStringList,quint32(345),AltStringUsage);
   // mp_EventHandlerThreadController->HandleInactiveEvent(EventEntry,quint64(34));
    mp_EventHandlerThreadController->UpdateEventDataStructures(quint32(123),quint64(3556),EventEntry,true,true);
    mp_EventHandlerThreadController->ProcessEvent(quint32(23),EventStringList,true,quint32(34),AltStringUsage);
    mp_EventHandlerThreadController->SetSystemStateMachine(EventEntry);
    mp_EventHandlerThreadController->InformGUI(EventEntry,quint64(678));
    mp_EventHandlerThreadController->RegisterCommands();
    mp_EventHandlerThreadController->SetGuiAvailable(true);
    mp_EventHandlerThreadController->VerifyEventIDs(quint32(23));
    mp_EventHandlerThreadController->VerifyEventType(Global::EVTTYPE_WARNING);
    mp_EventHandlerThreadController->VerifyEventType(Global::EVTTYPE_ERROR);
    mp_EventHandlerThreadController->VerifySource(Global::EVENTSOURCE_MAIN);
    mp_EventHandlerThreadController->VerifyLogLevel(Global::LOGLEVEL_LOW);
    mp_EventHandlerThreadController->VerifyGuiButtonType(Global::OK);
    mp_EventHandlerThreadController->VerifyEventConfigCSV(QString("/EventConfig.csv"));
    mp_EventHandlerThreadController->VerifyEventCSVFilenameExists(QString("/EventConfig.csv"));
    mp_EventHandlerThreadController->VerifyEventMacro(QString("EventMacroName"));
    mp_EventHandlerThreadController->VerifyBoolean(QString("Text"));
    mp_EventHandlerThreadController->VerifyStringList(QString("EventString"));
    mp_EventHandlerThreadController->OnAcknowledge(Ref,Ack);

    mp_EventHandlerThreadController->OnGoReceived();
    mp_EventHandlerThreadController->CleanupAndDestroyObjects();
    //mp_EventHandlerThreadController->CreateAndInitializeObjects();
    mp_EventHandlerThreadController->OnStopReceived();


    mp_EventHandlerThreadController->SetCountForEventId(quint32(1234),(qint8)32);
    mp_EventHandlerThreadController->SetOperatingMode(QString("xyz"));
    mp_EventHandlerThreadController->SetEventLoggerBaseFileName(QString("Logging File"));
    mp_EventHandlerThreadController->SetSerialNumber(QString("sr123"));

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
