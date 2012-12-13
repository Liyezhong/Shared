/****************************************************************************/
/*! \file EventHandler.cpp
 *
 *  \brief Frontend for Event processing and logging.
 *
 *         All events shall be sent to this Module. It will filter an
 *         incoming event according to its type and forward the event
 *         to Error Handler / Data Logger.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Y.Novak
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
//#define MY_EVNT_DEBUG

#include <NetCommands/Include/CmdSystemAction.h>
#include<NetCommands/Include/CmdEventReport.h>
#include <EventHandler/Include/ErrorHandler.h>
#include <EventHandler/Include/EventHandlerThreadController.h>
#include <Global/Include/Utils.h>
#include <DataLogging/Include/DayEventEntry.h>
#include <Global/Include/Commands/CmdEventUpdate.h>
#include <NetCommands/Include/CmdEventReport.h>
#include <NetCommands/Include/CmdSystemAction.h>
#include <NetCommands/Include/CmdAcknEventReport.h>
#include <EventHandler/Include/StateHandler.h>
# include <Global/Include/EventObject.h>
#include <QMetaType>
#include <QThread>
#include <QDebug>


/****************************************************************************/
/**
 * \brief Namespace containing all classes for system event handling.
 */
/****************************************************************************/
namespace EventHandler {

/****************************************************************************/
/*!
 *  \brief    Constructor for the EventHandlerThreadController
 *
 *  \return
 *
 ****************************************************************************/
EventHandlerThreadController::EventHandlerThreadController(Global::gSourceType TheHeartBeatSource) :
    Threads::ThreadController(TheHeartBeatSource, "EventHandler")
{
    //    CreateAndInitializeObjects();
    // Register LoggingSource Templates with moc
    qRegisterMetaType<Global::LoggingSource>("Global::LoggingSource");
}


/****************************************************************************/
/*!
 *  \brief    Destructor for the EventHandlerThreadController
 *
 *  \return
 *
 ****************************************************************************/
EventHandlerThreadController::~EventHandlerThreadController()
{

}

/****************************************************************************/
/*!
 *  \brief    API function to call before EventHandler can be used
 *
 *      This function does everything to initialize the EventHandler. It
 *      shall be called before EventHandler can be used.
 *
 ****************************************************************************/
void EventHandlerThreadController::CreateAndInitializeObjects()
{
    AddActionTypes();
    AddEventTypes();
    AddEventLogLevels();
    AddSourceComponents();
    ReadConfigFile("../Settings/EventConfig.csv");

    // now register commands
    RegisterCommands();

    // process pending events
    if (m_eventList.size() > 0)
    {
        foreach (PendingEvent pe, m_pendingEvents)
        {
            ProcessEvent(pe.EventID, pe.EventStringList, pe.EventStatus, pe.EventKey);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief   Map event type enum to Event Type string
 *
 *
 *  \return
 *
 ****************************************************************************/
void EventHandlerThreadController::AddEventTypes(){
    //
    m_EventTypeEnumMap.insert( "INFO", Global::EVTTYPE_INFO);
    m_EventTypeEnumMap.insert("WARNING", Global::EVTTYPE_WARNING);
    m_EventTypeEnumMap.insert("ERROR", Global::EVTTYPE_ERROR);
    m_EventTypeEnumMap.insert("FATAL ERROR", Global::EVTTYPE_FATAL_ERROR);
    m_EventTypeEnumMap.insert("UNDEFINED", Global::EVTTYPE_UNDEFINED);
    m_EventTypeEnumMap.insert("DEBUG", Global::EVTTYPE_DEBUG);
}

void EventHandlerThreadController::AddEventLogLevels() {
    m_EventLogLevelEnumMap.insert("NONE", Global::LOGLEVEL_NONE);
    m_EventLogLevelEnumMap.insert("MEDIUM", Global::LOGLEVEL_MEDIUM);
    m_EventLogLevelEnumMap.insert("LOW", Global::LOGLEVEL_LOW);
    m_EventLogLevelEnumMap.insert("HIGH", Global::LOGLEVEL_HIGH);

}

void EventHandlerThreadController::AddSourceComponents() {
    m_EventSourceMap.insert("SEPIA", Global::EVENTSOURCE_SEPIA);
    m_EventSourceMap.insert("DEVICECOMMANDPROCESSOR", Global::EVENTSOURCE_DEVICECOMMANDPROCESSOR);
    m_EventSourceMap.insert("MAIN", Global::EVENTSOURCE_MAIN);
    m_EventSourceMap.insert("SCHEDULER", Global::EVENTSOURCE_SCHEDULER);
    m_EventSourceMap.insert("EVENTHANDLER", Global::EVENTSOURCE_EVENTHANDLER);
    m_EventSourceMap.insert("EXPORT", Global::EVENTSOURCE_EXPORT);
    m_EventSourceMap.insert("IMPORTEXPORT", Global::EVENTSOURCE_IMPORTEXPORT);
    m_EventSourceMap.insert("BLG", Global::EVENTSOURCE_BLG);
}

/****************************************************************************/
/*!
 *  \brief    Map action type enum to action ( Mesage Type) string
 *
 *
 *  \return
 *
 ****************************************************************************/
void EventHandlerThreadController::AddActionTypes() {
    m_ActionTypeEnumMap.insert("SHUTDOWN",Global::ACNTYPE_SHUTDOWN);
    m_ActionTypeEnumMap.insert("STOP",Global::ACNTYPE_STOP);
    m_ActionTypeEnumMap.insert("REBOOT",Global::ACNTYPE_REBOOT);
    m_ActionTypeEnumMap.insert("RETRY",Global::ACNTYPE_RETRY);
    m_ActionTypeEnumMap.insert("INFO",Global::ACNTYPE_INFO);
    m_ActionTypeEnumMap.insert("ERROR",Global::ACNTYPE_ERROR);
    m_ActionTypeEnumMap.insert("WARNING",Global::ACNTYPE_WARNING);
    m_ActionTypeEnumMap.insert("BUSY",Global::ACNTYPE_BUSY);
    m_ActionTypeEnumMap.insert("IDLE",Global::ACNTYPE_IDLE);
    m_ActionTypeEnumMap.insert("REMOVEALLRACKS",Global::ACNTYPE_REMOVEALLRACKS);
    m_ActionTypeEnumMap.insert("REMOVERACK",Global::ACNTYPE_REMOVERACK);
}


/****************************************************************************/
/*!
 *  \brief    File parser for the event cong csv
 *
 *
 *  \param    valid event config csv file name
 *
 *  \return
 *
 ****************************************************************************/
void EventHandlerThreadController::ReadConfigFile(QString filename)
{
    // !\ Read
    // !\ #EventID,EventCODE,EventType,TargetComponent+,Action,SourceComponent,Yes/No,Error/High/Medium/Low/FatalError,Yes/No,True/False,EnglishString,Yes+No+Cancel,EventInfo,Yes/No
    // !\ #EventId (0),EventMacro (1),EventType(2),Target(3),MessageType(4),Source(5),UIAlarmTone(6),LogLevel(7),Ack(8),UserLog(9),
    //    EventString(10),GUIEvent(11),EventInfo(12),StatusBarIcon(13)
    QFile file(filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text))
        return;

    m_eventList.clear();

    QTextStream stream ( &file );
    QString line;

    //required once the event is in table, even if nothing is specified in the required column
    while( !stream.atEnd() )
    {
        line = stream.readLine();
        if (line.left(1) != "#")
        {
            qint8 NumOfAttempts = 1; // since at least one cycle is
            QStringList textList = line.split(",");
            //! No of event fields from event conf file
            //if (textList.count() == NUM_FIELDS_EVENTCONF)
            //{
            //! \ Get EventId (0)
            EventHandler::EventCSVInfo EventCSVInfo;

            if (textList.count() > 0)
            {
                EventCSVInfo.SetEventCode(textList.at(0).toInt()); // only int check, no other validations done
            }

            //! \ Get EventMacro (1)
            QString EventMacroName = "";
            if (textList.count() > 1)
            {
                EventMacroName = textList.at(1).trimmed(); // No validations required
                //DayEventEntry.SetEventName(EventMacroName);
            }
            Q_UNUSED(EventMacroName);


            //! \ Get EventType (2)
            if (textList.count() > 2)
            {

                Global::EventType EventType = m_EventTypeEnumMap.value(textList.at(2).
                                                                       trimmed().toUpper(),
                                                                       Global::EVTTYPE_UNDEFINED);

                if(EventType == Global::EVTTYPE_UNDEFINED)  {
#ifdef VALIDATE
                    return;// the field is mandatory & only the allowed types are acceptable
#endif
                }
                EventCSVInfo.SetEventType(EventType);
            }

            //! \ Read ActionType(3) and Set Action / Positive ActionType(3)

            Global::ActionType ActionType = Global::ACNTYPE_NONE;;
            Global::ActionType ActionTypePositive = Global::ACNTYPE_NONE;
            if (textList.count() > 3)
            {
                QStringList ActionInfo = textList.at(3).split("+");

                ActionType = m_ActionTypeEnumMap.value(ActionInfo.at(0).
                                                       trimmed().toUpper(),
                                                       Global::ACNTYPE_NONE);

                if(ActionInfo.count() >= 2)
                {
                    NumOfAttempts = ActionInfo.at(1).toInt();//else , already initialized to 1!
                }

                if(ActionInfo.count() == 3)
                {
                    ActionTypePositive = m_ActionTypeEnumMap.value(ActionInfo.at(2).
                                                                   trimmed().toUpper(),
                                                                   Global::ACNTYPE_NONE);
                }

                EventCSVInfo.SetActionInfo(ActionType, NumOfAttempts, ActionTypePositive);

                if(ActionInfo.count() == 2) {

#ifdef VALIDATE
                    return;// error , incomplete data
#endif
                }

            }

            QString ActionNegativeStr = "";
            Global::ActionType ActionNegative = Global::ACNTYPE_NONE;
            if (textList.count() > 4)
            {
                ActionNegativeStr = textList.at(4).trimmed();
                ActionNegative = m_ActionTypeEnumMap.value(ActionNegativeStr.trimmed().toUpper(),
                                                           Global::ACNTYPE_NONE);

            }
            //update
            EventCSVInfo.SetActionNegative(ActionNegative); // no checks

            //! \ Get Source(5) component name & Set

            if (textList.count() > 5)
            {
                QString Source = textList.at(5).trimmed();

                EventCSVInfo.SetEventSource(m_EventSourceMap.value(Source.toUpper(),Global::EVENTSOURCE_NONE));

                if(Source.isEmpty())
                {
                    EventCSVInfo.SetAckRequired(false);

                }
                else
                {
                    EventCSVInfo.SetAckRequired(true);
                }
            }


            //! \ Get AlarmTone(6) flag

            if (textList.count() > 6)
            {
                bool AlarmFlag = textList.at(6).toInt();
                EventCSVInfo.SetAlarmStatus(AlarmFlag);
            }


            //! \ Get LogLevel(7) flag

            if (textList.count() > 7)
            {

                EventCSVInfo.SetLogLevel(m_EventLogLevelEnumMap.value(textList.at(7).toUpper(),Global::LOGLEVEL_HIGH));
            }

            //! \ Get UserLog(8) flag

            if (textList.count() > 8)
            {
                bool UserLogFlag = textList.at(8).toInt();
                EventCSVInfo.SetRunLogStatus(UserLogFlag);
            }

            //! \ Get EventString(9)

            // ! \ EventString comes from the RaiseEvent call.


            //! \ Get GUIEvent(10) list

            if (textList.count() > 10)
            {
                EventCSVInfo.SetButtonType(Global::StringToGuiButtonType(textList.at(10).toUpper()));
            }

            //! \ Get StatusBarIcon(11) flag

            if (textList.count() > 11)
            {
                bool StatusBarIconFlag = textList.at(11).toInt();
                EventCSVInfo.SetStatusBarIcon(StatusBarIconFlag);
            }

            m_eventList.insert(EventCSVInfo.GetEventCode(), EventCSVInfo);

        }
    }
    file.close();
}
/****************************************************************************/
/*!
 *  \brief    Hash Table update for the event count map
 *
 *
 *  \param    valid event & event status
 *
 *  \return
 *
 ****************************************************************************/


void EventHandlerThreadController::UpdateEventKeyCountMap(quint32 EventKey, bool EventStatus) {

    if( m_EventKeyCountMap.count(EventKey)) {
        if(EventStatus == false) {
            m_EventKeyDataMap.remove(EventKey);
            m_EventKeyCountMap.remove(EventKey);
            Global::EventObject::Instance().UnBlockEventKey(EventKey);
        }
        else {
            //get the current count & increment by 1
            int count = m_EventKeyCountMap[EventKey];
            m_EventKeyCountMap[EventKey] = count + 1;
        }

    }
    else
    {
        m_EventKeyCountMap.insert(EventKey, 1);
    }
}

/****************************************************************************/
/*!
 *  \brief    This SLOT handles all incoming events.
 *
 *      This SLOT shall be used to report all events and their current
 *      status. The status might be of two Templates:
 *        - event active
 *        - event closed (e.g. condition which caused event is not
 *          present anymore)
 *
 *      This function forwards the event to ErrorHandler / DataLogger
 *      according to the event type.
 *
 *  \param    TheEventEntry = reference to incoming event
 *
 *  \return
 *
 ****************************************************************************/
void EventHandlerThreadController::ProcessEvent(const quint32 EventID, const Global::tTranslatableStringList &EventStringList, const bool EventStatus, const quint32 EventKey)
{
    qDebug() << "EventHandlerThreadController::ProcessEvent" << EventID;

    m_EventIdKeyMap.insert(EventKey,EventID);

    // if eventList is not available yet, place event into pendingList
    if (m_eventList.size() == 0)
    {
        PendingEvent pe;
        pe.EventID = EventID;
        pe.EventStringList = EventStringList;
        pe.EventKey = EventKey;
        pe.EventStatus = EventStatus;
        m_pendingEvents.push_back(pe);// These events, if any, are processed after successfully reading the EventConf file
        return;
    }

    EventCSVInfo EventInfo = m_eventList.value(EventID);
    DataLogging::DayEventEntry EventEntry;
    EventEntry.SetEventCode(EventID);
    EventEntry.SetEventCSVInfo(EventInfo);
    EventEntry.SetTranslatableStringList(EventStringList);
    EventEntry.SetDateTime(Global::AdjustedTime::Instance().GetCurrentDateTime());
    EventEntry.SetActive(EventStatus);
    m_EventKeyDataMap.insert(EventKey, EventEntry);


    /*! \todo  EventStatus is used to indicate whether Event is resolved or not. If Event is active
        * Event Status is true, when the event is reset, the EventStatus will be false
        *
     */
    UpdateEventKeyCountMap(EventKey, EventStatus);
    //Log if loglevel is not "NONE"
    if (EventEntry.GetLogLevel() != Global::LOGLEVEL_NONE)
    {
        qDebug()<< "Sending event to DataLogger";
        LogEventEntry(EventEntry); //Log the event
    }

//    mpAlarmHandler->setSoundNumber(Global::ALARM_WARNING, mpUserSettings->GetSoundNumberWarning());
    Global::AlarmType alarm= Global::ALARM_NONE;
    if(EventEntry.GetAlarmStatus())
    {

        if(EventStatus) {

            if(EventEntry.GetEventType() == Global::EVTTYPE_ERROR )
                alarm = Global::ALARM_ERROR;
            else if ( EventEntry.GetEventType() == Global::EVTTYPE_WARNING)
                alarm = Global::ALARM_WARNING;

             mpAlarmHandler->setTimeout(5);
             mpAlarmHandler->setAlarm(1,alarm,true);
        }
        else
            mpAlarmHandler->setAlarm(1,alarm, false);

    }

    ForwardToErrorHandler(EventEntry, EventKey);// Send the Error for handling
    /// \todo this is a test of Axeda Remote Care error reporting:
    //emit ErrorHandler::SendErrorToRemoteCare(EventEntry);



}

void EventHandlerThreadController::RegisterCommands()
{

    // register acknowledges
    RegisterAcknowledgeForProcessing<
            NetCommands::CmdAcknEventReport, EventHandlerThreadController
            >(&EventHandlerThreadController::OnAcknowledge, this);
}

void EventHandlerThreadController::OnAcknowledge(Global::tRefType Ref, const NetCommands::CmdAcknEventReport &Ack)
{
    quint32 EventKey = m_EventKeyRefMap[Ref];
    quint32 EventID = m_EventIdKeyMap.value(EventKey);
    EventCSVInfo EventInfo = m_eventList.value(EventID);
    DataLogging::DayEventEntry EventEntry  = m_EventKeyDataMap.value(EventKey);
    EventEntry.SetEventCSVInfo(EventInfo);
    EventEntry.SetAckValue(Ack);
    int Count = GetCountForEventKey(EventKey);

    if(EventEntry.GetLogLevel() != NULL)
    {
        LogEventEntry(EventEntry);
    }

    Global::EventSourceType SourceName = EventEntry.GetSourceComponent();
    if(SourceName == Global::EVENTSOURCE_NONE) {
        return;
    }

    bool IsActive = EventEntry.IsEventActive();
    if( IsActive == false) {
        return;
    }


    if(Count <= EventEntry.GetRetryAttempts())
    {
        if((EventEntry.GetAckValue() ==  NetCommands::OK_BUTTON) || (EventEntry.GetAckValue() ==  NetCommands::YES_BUTTON) || (EventEntry.GetAckValue() == NetCommands::CONTINUE_BUTTON))
        {
            //send +ve command action if defined.
            NetCommands::CmdSystemAction *p_CmdSystemAction;           
            p_CmdSystemAction = new NetCommands::CmdSystemAction();
            Global::ActionType ActionType = EventEntry.GetActionPositive();
            p_CmdSystemAction->SetAction(ActionType);
            p_CmdSystemAction->SetRetryCount(Count);
            EventKey = (Ack.GetEventKey() & 0x00000000ffffffff);
            quint32 EventID = (Ack.GetEventKey() & 0xffffffff00000000) >> 32 ;
            p_CmdSystemAction->SetEventKey(EventKey);
            p_CmdSystemAction->SetEventID(EventID);
            Global::tRefType NewRef = GetNewCommandRef();
            SendCommand(NewRef, Global::CommandShPtr_t(p_CmdSystemAction));
        }
        else
        {
            //send -ve command action if defined.
            NetCommands::CmdSystemAction *p_CmdSystemAction;
            p_CmdSystemAction = new NetCommands::CmdSystemAction();
            Global::ActionType ActionType = EventEntry.GetActionNegative();
            p_CmdSystemAction->SetAction(ActionType);
            p_CmdSystemAction->SetRetryCount(Count);
            EventKey = (Ack.GetEventKey() & 0x00000000ffffffff);
            quint32 EventID = (Ack.GetEventKey() & 0xffffffff00000000) >> 32 ;
            p_CmdSystemAction->SetEventKey(EventKey);
            p_CmdSystemAction->SetEventID(EventID);
            Global::tRefType NewRef = GetNewCommandRef();
            SendCommand(NewRef, Global::CommandShPtr_t(p_CmdSystemAction));
        }

    }
    else
    {
        // if (second action )//second action
        NetCommands::CmdSystemAction *p_CmdSystemAction;
        p_CmdSystemAction = new NetCommands::CmdSystemAction();
        Global::ActionType ActionType = EventEntry.GetActionPositive();
        p_CmdSystemAction->SetAction(ActionType);
        Global::tRefType NewRef = GetNewCommandRef();
        p_CmdSystemAction->SetRetryCount(Count);
        EventKey = (Ack.GetEventKey() & 0x00000000ffffffff);
        quint32 EventID = (Ack.GetEventKey() & 0xffffffff00000000) >> 32 ;
        p_CmdSystemAction->SetEventKey(EventKey);
        p_CmdSystemAction->SetEventID(EventID);
        qDebug() << "EventKEY: " << EventKey << "EventID: " <<EventID;
        SendCommand(NewRef, Global::CommandShPtr_t(p_CmdSystemAction));
        DEBUGWHEREAMI;

    }



    //    //p_CmdSystemAction->SetTargetComponents(TargetComponents);
    //    p_CmdSystemAction->SetRetryCount(Count);
    //     EventKey = (Ack.GetEventKey() & 0x00000000ffffffff);
    //     quint32 EventID = (Ack.GetEventKey() & 0xffffffff00000000) >> 32 ;
    //    p_CmdSystemAction->SetEventKey(EventKey);
    //    p_CmdSystemAction->SetEventID(EventID);
    //    qDebug() << "EventKEY: " << EventKey << "EventID: " <<EventID;
    //    SendCommand(NewRef, Global::CommandShPtr_t(p_CmdSystemAction));
    //    DEBUGWHEREAMI;
}



/****************************************************************************/
/*!
 *  \brief Destroy all used objects and prepare for shutdown.
 *
 *         See detailed description in the Base Class's header
 *
 *  \return
 */
/****************************************************************************/
void EventHandlerThreadController::CleanupAndDestroyObjects()
{
}

/****************************************************************************/
/*!
 *  \brief This method is called when the base class receives Go signal.
 *
 *         See detailed description in the Base Class's header
 *
 *  \return
 */
/****************************************************************************/
void EventHandlerThreadController::OnGoReceived()
{
    mpActionHandler = new ActionHandler((EventHandlerThreadController *)this);

        CHECKPTR(mpActionHandler);
       
        CONNECTSIGNALSLOT(this, ForwardToErrorHandler(const DataLogging::DayEventEntry &, const quint32),
                              mpActionHandler, ReceiveEvent(const DataLogging::DayEventEntry &, const quint32 ));


}

/****************************************************************************/
/*!
 *  \brief This method is called when the base class receives Stop signal.
 *
 *         See detailed description in the Base Class's header
 *
 *  \return
 */
/****************************************************************************/
void EventHandlerThreadController::OnStopReceived()
{

}

/****************************************************************************/
/**
 * \brief Power will fail shortly.
 *
 * Power will fail shortly.
 */
/****************************************************************************/
void EventHandlerThreadController::OnPowerFail()
{
    /// \todo implement
}

/****************************************************************************/
bool EventHandlerThreadController::LocalProcessErrorEvent(const DataLogging::DayEventEntry & /*TheEventEntry*/) {
    /// \todo implement
    return false;
}



void EventHandlerThreadController::SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd)
{
    qDebug() << "EventHandlerThreadController::SendCommand" << Ref;
    Threads::ThreadController::SendCommand(Ref, Cmd);
}

Global::tRefType EventHandlerThreadController::GetNewCommandRef()
{
    return Threads::ThreadController::GetNewCommandRef();
}

} // end namespace EventHandler
