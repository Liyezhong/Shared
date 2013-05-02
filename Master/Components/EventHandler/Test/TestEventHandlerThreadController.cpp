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
#include <EventHandler/Include/CrisisEventHandler.h>
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
#include <NetCommands/Include/CmdEventReport.h>
#include <QThread>


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
    class EventHandlerUser : public Threads::ThreadController
    {
    public:
        quint32 EventCode;
        quint32 Scenario;
        quint32 EventKey;
        EventHandlerUser(Global::gSourceType m_HeartBeatSource):
            Threads::ThreadController(m_HeartBeatSource,"EventHandlerUser")
        {
            CommandChannel = new Threads::CommandChannel(this,"EventHandlerUser");
            RegisterCommandForProcessing<NetCommands::CmdSystemAction,EventHandlerUser>
                    (&EventHandlerUser::OnActionCommandReceived, this);

            RegisterCommandForProcessing<NetCommands::CmdEventReport, EventHandlerUser>
                    (&EventHandlerUser::OnReceiveCmdThatShouldBeToGui,this);
        }
        Threads::CommandChannel *CommandChannel;
        void OnActionCommandReceived(Global::tRefType Ref, const NetCommands::CmdSystemAction &Cmd)
        {
            qDebug()<<"OnActionCommandReceived: " << Cmd.GetEventID() << " - " << Cmd.GetScenario() << " - " << Cmd.GetAction();
            EventCode = Cmd.GetEventID();
            Scenario = Cmd.GetScenario();
            EventKey = Cmd.GetEventKey();
//            RaiseEvent(Cmd.GetEventID(),Cmd.GetScenario(),Cmd.GetEventKey(),true);
        }

        void OnReceiveCmdThatShouldBeToGui(Global::tRefType Ref, const NetCommands::CmdEventReport &Cmd)
        {
            qDebug()<<"OnReceiveCmdThatShouldBeToGui: "<< Ref;
        }

        void RaiseEvent(quint32 EventCode, quint32 Scenario)
        {
            EventHandler::CrisisEventHandler::Instance().RaiseEvent(EventCode,Scenario,Global::tTranslatableStringList(),Global::tTranslatableStringList());
        }

        void RaiseEvent(bool isResolved)
        {
            EventHandler::CrisisEventHandler::Instance().RaiseEvent(EventCode,Scenario,EventKey,isResolved);
        }

        void OnGoReceived(){}
        void OnStopReceived(){}
        void OnPowerFail(){}
        void CreateAndInitializeObjects(){}
        void CleanupAndDestroyObjects(){}
    };

    Global::gSourceType m_HeartBeatSourceEventHandler;
    Global::gSourceType m_HeartBeatSourceDataLogging;
    EventHandler::EventHandlerThreadController * mp_EventHandlerThreadController;
    DataLogging::DataLoggingThreadController * mp_DataLoggingThreadController;
    QString                                     m_EventLoggerBaseFileName;
    Global::AlarmHandler * mp_alarmHandler;
    Threads::CommandChannel    *  mp_CommandChannelEventThread;        ///< Command channel for EventHandler.
    QSignalSpy * mp_SpyCommandChannelTx;
    EventHandlerUser* EVUser;

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

    void TestProcessEvents();

public:

}; // end class TestXmlConfigFile


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

//    for (i = mp_EventHandlerThreadController->m_eventList.begin(); i != mp_EventHandlerThreadController->m_eventList.end(); ++i)
//    {

//    }

    EVUser->RaiseEvent(500040401,520000200);
    EVUser->RaiseEvent(true);
    EVUser->RaiseEvent(true);

}



/****************************************************************************/
void TestEventHandlerThreadController::initTestCase() {
    const Global::gSourceType EVENT_GROUP_SOURCENAME  = 0x0000;
    const Global::gSourceType HEARTBEAT_SOURCE_EVENTHANDLER = EVENT_GROUP_SOURCENAME + 0x0003;
    m_HeartBeatSourceDataLogging = EVENT_GROUP_SOURCENAME + 0x0002;
    m_HeartBeatSourceEventHandler =  HEARTBEAT_SOURCE_EVENTHANDLER;

    Global::SystemPaths::Instance().SetLogfilesPath("../Logfiles");
    Global::SystemPaths::Instance().SetSettingsPath("../Settings");
    EventHandler::CrisisEventHandler::Instance().readEventStateConf("../Settings/EventStateError.csv");
    m_EventLoggerBaseFileName = "Base";
    mp_DataLoggingThreadController = new DataLogging::DataLoggingThreadController(m_HeartBeatSourceDataLogging, m_EventLoggerBaseFileName);

    mp_alarmHandler = new Global::AlarmHandler(5000,this);

    mp_EventHandlerThreadController = new EventHandler::EventHandlerThreadController(m_HeartBeatSourceEventHandler);
    mp_EventHandlerThreadController->SetAlarmHandler(mp_alarmHandler);
    mp_EventHandlerThreadController->ConnectToEventObject();

     mp_EventHandlerThreadController->CreateAndInitializeObjects();

    CONNECTSIGNALSLOT(mp_EventHandlerThreadController, LogEventEntry(const DataLogging::DayEventEntry &),
                      mp_DataLoggingThreadController, SendToDayEventLogger(const DataLogging::DayEventEntry &));


    EVUser = new EventHandlerUser(m_HeartBeatSourceEventHandler);

    mp_EventHandlerThreadController->ConnectToOtherCommandChannel(EVUser->CommandChannel);

//    QThread *EventHandlerThread = new QThread();

//    mp_EventHandlerThreadController->moveToThread(EventHandlerThread);
//    EventHandlerThread->start();

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
