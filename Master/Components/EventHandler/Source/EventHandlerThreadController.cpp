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
#include <EventHandler/Include/EventHandlerThreadController.h>
#include <Global/Include/Utils.h>
#include <DataLogging/Include/DayEventEntry.h>
#include <Global/Include/Commands/CmdEventUpdate.h>
#include <NetCommands/Include/CmdEventReport.h>
#include <NetCommands/Include/CmdSystemAction.h>
#include <NetCommands/Include/CmdAcknEventReport.h>
#include <EventHandler/Include/StateHandler.h>
#include <EventHandler/Include/EventHandlerEventCodes.h>
# include <Global/Include/EventObject.h>
#include <Global/Include/SystemPaths.h>
#include <QMetaType>
#include <QThread>
#include <QDebug>

//#define VALIDATE


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
    AddActionTypes();
    AddEventTypes();
    AddEventLogLevels();
    AddSourceComponents();
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
    bool ret =  ReadConfigFile(Global::SystemPaths::Instance().GetSettingsPath() + "/EventConfig.csv");

    Q_UNUSED(ret);
#ifdef VALIDATE
    if(!ret)
    {
        qDebug()<<"Event Configuration file data verification failed";
         return;
    }
#endif

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
    m_EventLogLevelEnumMap.insert("",Global::LOGLEVEL_NONE);

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
    m_EventSourceMap.insert("",Global::EVENTSOURCE_NONE);
}

void EventHandlerThreadController::HandleInactiveEvent(DataLogging::DayEventEntry &EventEntry, quint64 &EventId64)
{
    quint32 EventID = EventEntry.GetEventCode();
    int EventOccurenceCount = m_EventIDCount.count(EventID);
    if (EventOccurenceCount == 0) {
        //This is a dummy event
        //InformAlarmHandler(EventEntry, EventId64, false);
        //We dont update data structures, because there is nothing to be removed.
        return;
    }
    if (EventOccurenceCount == 1) {
        //Previously this event had occured once and EventStatus was true for the event.
        //We retrieve EventKey of that event.
        quint32 PreviousEventKey = m_EventIDKeyHash.take(EventID);
        qDebug()<<"Previous Event Key" << PreviousEventKey;
        EventId64 = EventID;
        EventId64 <<= 32;
        EventId64 |= PreviousEventKey;
        qDebug()<<"EventID64" << EventId64;
    }
    InformAlarmHandler(EventEntry, EventId64, false);
    UpdateEventDataStructures(EventID, EventId64, EventEntry, true);
}

void EventHandlerThreadController::CreateEventEntry(DataLogging::DayEventEntry &EventEntry,
                                                    EventCSVInfo &EventInfo,
                                                    const bool EventStatus,
                                                    const quint32 EventID,
                                                    const Global::tTranslatableStringList &EventStringList,
                                                    const quint32 EventKey)
{
    if (EventInfo.GetEventCode() == 0)
    {
        qDebug()<<"Unknown Event ID \n\n\n";
        EventInfo = m_eventList.value(EVENT_EVENT_ID_MISSING);
        EventEntry.SetTranslatableStringList(Global::FmtArgs() << EventID);
    }
    else
    {
        EventEntry.SetTranslatableStringList(EventStringList);
    }

    EventEntry.SetEventKey(EventKey);
    EventEntry.SetEventCSVInfo(EventInfo);
    EventEntry.SetDateTime(Global::AdjustedTime::Instance().GetCurrentDateTime());

    EventEntry.SetEventStatus(EventStatus);
}

void EventHandlerThreadController::InformAlarmHandler(const DataLogging::DayEventEntry &EventEntry, const quint64 EventId64, bool StartAlarm)
{
    Global::AlarmType AlarmType = Global::ALARM_NONE;

    if((EventEntry.GetEventType() == Global::EVTTYPE_ERROR || EventEntry.GetEventType() == Global::EVTTYPE_WARNING)) {
        if (EventEntry.GetAlarmStatus())
        {
            //We need Alarm
            if (EventEntry.GetEventType() == Global::EVTTYPE_ERROR ) {
                AlarmType = Global::ALARM_ERROR;
            }
            else if ( EventEntry.GetEventType() == Global::EVTTYPE_WARNING) {
                AlarmType = Global::ALARM_WARNING;
            }
            mpAlarmHandler->setTimeout(1000);// to be set after final discussion with team / Michael

            mpAlarmHandler->setAlarm(EventId64, AlarmType, StartAlarm);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Map action type enum to action ( Mesage Type) string
 *
 *
 *  \return
 *
 ****************************************************************************/
void EventHandlerThreadController::AddActionTypes()
{
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
    m_ActionTypeEnumMap.insert("", Global::ACNTYPE_NONE);
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
bool EventHandlerThreadController::ReadConfigFile(QString filename)
{
    // !\ Read
    // !\ #EventID,EventCODE,EventType,TargetComponent+,Action,SourceComponent,Yes/No,Error/High/Medium/Low/FatalError,Yes/No,True/False,EnglishString,Yes+No+Cancel,EventInfo,Yes/No
    // !\ #EventId (0),EventMacro (1),EventType(2),Target(3),MessageType(4),Source(5),UIAlarmTone(6),LogLevel(7),Ack(8),UserLog(9),
    //    EventString(10),GUIEvent(11),EventInfo(12),StatusBarIcon(13)

    bool ret = false;
    qDebug()<<"EventConfig path"<<filename;
    QFile file(filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"File Open Failed" << file.errorString();
        return false;
    }

    m_eventList.clear();

    QTextStream stream ( &file );
    QString line;

    //required once the event is in table, even if nothing is specified in the required column
    while( !stream.atEnd() )
    {
        line = stream.readLine();
        if (line.left(1) != "#")
        {
            QStringList textList = line.split(",");
            EventHandler::EventCSVInfo EventCSVInfo;

            //Get EventCode
            if (textList.count() > 0)
            {
                int EventCode = textList.at(0).toInt();

                ret = VerifyEventIDs(EventCode) ;

#ifdef VALIDATE
                if(!ret)
                {
                    return ret;
                }
#endif

                EventCSVInfo.SetEventCode(EventCode); // only int check, no other validations done
            }


            //Get EventMacro
            //! \ Get EventMacro (1)
            QString EventMacroName = "";
            if (textList.count() > 1)
            {
                EventMacroName = textList.at(1).trimmed(); // No validations required
                ret = VerifyEventMacro(EventMacroName);
#ifdef VALIDATE
                if(!ret)
                {
                    return ret;
                }
#endif
            }

            //Get EventType
            //! \ Get EventType (2)
            if (textList.count() > 2)
            {

                Global::EventType EventType = m_EventTypeEnumMap.value(textList.at(2).
                                                                       trimmed().toUpper(),
                                                                       Global::EVTTYPE_UNDEFINED);
                ret = VerifyEventType(EventType);
#ifdef VALIDATE
                if(!ret)
                {
                    return ret;
                }
#endif
                EventCSVInfo.SetEventType(EventType);
            }

            //Get ActionType
            if (textList.count() > 3)
            {
                QStringList ActionInfo = textList.at(3).split("+");

                EventCSVInfo.SetActionPositive(m_ActionTypeEnumMap.value(ActionInfo.at(0).trimmed().toUpper(), Global::ACNTYPE_UNEXPECTED));

                if (ActionInfo.count() > 1) { // we have a second action in case the first one fails
                    EventCSVInfo.SetRetries(ActionInfo.at(1).trimmed().toInt());

                    if (ActionInfo.count() > 2) {
                        EventCSVInfo.SetFinalAction(m_ActionTypeEnumMap.value(ActionInfo.at(0).trimmed().toUpper(), Global::ACNTYPE_UNEXPECTED));
                    }
                }

                if (ActionInfo.count() >= 1) {
                    ret = (VerifyAction(EventCSVInfo.GetActionPositive()) && VerifyAction(EventCSVInfo.GetFinalAction()));
                    if (ret) {
                        if ((EventCSVInfo.GetFinalAction() != Global::ACNTYPE_NONE) && (EventCSVInfo.GetRetryAttempts() <= 0)) {
                            ret = false;
                        }
                    }
#ifdef VALIDATE
                    if (!ret) {
                        return ret;
                    }
#endif
                }
            }

            //Get ActionNegative
            if (textList.count() > 4)
            {
                QStringList ActionInfo = textList.at(4).split("+");

                EventCSVInfo.SetActionNegative(m_ActionTypeEnumMap.value(ActionInfo.at(0).trimmed().toUpper(), Global::ACNTYPE_UNEXPECTED));

                ret = VerifyAction(EventCSVInfo.GetActionNegative());
#ifdef VALIDATE
                if(!ret)
                {
                    return ret;
                }
#endif
            }

            //! \ Get Source(5) component name & Set
            if (textList.count() > 5)
            {
                QString Source = textList.at(5).trimmed();

                Global::EventSourceType EventSourceType = Global::EVENTSOURCE_NOTEXIST;

                EventSourceType = m_EventSourceMap.value(Source.toUpper(),Global::EVENTSOURCE_NOTEXIST);

                ret = VerifySource(EventSourceType);

#ifdef VALIDATE
                if(!ret)
                {
                    return ret;
                }
#endif

                EventCSVInfo.SetEventSource(EventSourceType);

                // Assumed Ack is required if source component name is set
                if(EventSourceType == Global::EVENTSOURCE_NONE)
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
                bool AlarmFlag = false;

                if((textList.at(6).trimmed().toUpper() == "NO") || (textList.at(6).trimmed().toUpper() == "")) {
                    AlarmFlag = false;
                }

                else if(textList.at(6).trimmed().toUpper() == "YES") {
                    AlarmFlag = true;
                }
                else
                {
#ifdef VALIDATE
                    return false;
# endif
                }

                EventCSVInfo.SetAlarmStatus(AlarmFlag);
            }


            //! \ Get LogLevel(7) flag

            if (textList.count() > 7)
            {

                Global::EventLogLevel EventLogLevel = Global::LOGLEVEL_UNEXPECTED;
                EventLogLevel = m_EventLogLevelEnumMap.value(textList.at(7).toUpper());
                ret = VerifyLogLevel(EventLogLevel);
#ifdef VALIDATE
                if(!ret)
                {
                    return ret;
                }
#endif
                EventCSVInfo.SetLogLevel(EventLogLevel);
            }

            //! \ Get UserLog(8) flag

            if (textList.count() > 8)
            {
                bool UserLogFlag = false;
                QString strUserLog = textList.at(8).toUpper();
                ret = VerifyBoolean(strUserLog);
#ifdef VALIDATE
                if(!ret)
                {
                    return ret;
                }
#endif
                UserLogFlag = ((textList.at(8).toUpper() == "TRUE") || (textList.at(8).toUpper() == "YES"));

                EventCSVInfo.SetRunLogStatus(UserLogFlag);
            }

            //! \ Get EventString(9)

               if (textList.count() > 9)
               {
                    // ! \ EventString comes from the RaiseEvent call, just verify the data is not empty
                    QString EventString = textList.at(9);
                    ret = VerifyStringList(EventString);
               }


            //! \ Get GUIEvent(10) list

            if (textList.count() > 10)
            {

                Global::GuiButtonType BtnType;
                BtnType = Global::StringToGuiButtonType(textList.at(10).toUpper());
                ret = VerifyGuiButtonType(BtnType);
#ifdef VALIDATE
                if(!ret)
                {
                    return ret;
                }
#endif
                EventCSVInfo.SetButtonType(BtnType);
            }

            //! \ Get StatusBarIcon(11) flag
            if (textList.count() > 11)
            {

                bool StatusIconFlag = false;
                QString strStatusIcon = textList.at(11).toUpper();
                ret = VerifyBoolean(strStatusIcon);
#ifdef VALIDATE
                if(!ret)
                {
                    return ret;
                }
#endif
                StatusIconFlag = ((textList.at(8).toUpper() == "TRUE") || (textList.at(8).toUpper() == "YES"));

                EventCSVInfo.SetStatusBarIcon(StatusIconFlag);
            }

            m_eventList.insert(EventCSVInfo.GetEventCode(), EventCSVInfo);

 //! \ validate dependencies between the values corresponding to an event.

           ret = VerifyUserLogGUIOptionDependency( EventCSVInfo);

#ifdef VALIDATE

           if(!ret)
           {
               return ret;
           }
#endif
           ret = VerifySourceComponentGUIOptionsDependency(EventCSVInfo);

#ifdef VALIDATE

           if(!ret)
           {
               return ret;
           }
#endif
           ret = VerifyActionGUIOptionsDependency(EventCSVInfo);

#ifdef VALIDATE

           if(!ret)
           {
               return ret;
           }
#endif

           ret = VerifyAlarmGUIOptionsDependency(EventCSVInfo);

#ifdef VALIDATE

           if(!ret)
           {
               return ret;
           }
#endif

           ret = VerifyAlarmEventTypeDependency(EventCSVInfo);
#ifdef VALIDATE

           if(!ret)
           {
               return ret;
           }
#endif
           ret = VerifyStatusbarGUIOptionDependency(EventCSVInfo);
#ifdef VALIDATE

           if(!ret)
           {
               return ret;
           }
#endif


        }
    }
#ifdef VALIDATE
    qDebug()<<"Reading event configuration file & data verification successful \n\n\n";
#elseif
    qDebug()<<"Reading event configuration verification disabled, end of ReadConfigFile reached \n\n\n";
#endif




    file.close();

    return true;
}

bool EventHandlerThreadController::VerifyUserLogGUIOptionDependency( EventHandler::EventCSVInfo EventCSVInfo ) {

    if((EventCSVInfo.GetRunLogStatus() == true) && (EventCSVInfo.GetGUIMessageBoxOptions() == Global::NO_BUTTON)) {

        qDebug()<< "Invalid UserLog - GUIOption combination set" << EventCSVInfo.GetEventCode()<<"\t"
                <<EventCSVInfo.GetRunLogStatus()<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions()<<"\n";
        return false;

    }
    return true;


}

bool EventHandlerThreadController::VerifySourceComponentGUIOptionsDependency(EventHandler::EventCSVInfo EventCSVInfo) {

    if((EventCSVInfo.GetSourceComponent() != Global::EVENTSOURCE_NONE) && (EventCSVInfo.GetGUIMessageBoxOptions() == Global::NO_BUTTON )) {

        qDebug()<< "Invalid SourceComponent - GUIOption combination set"<< EventCSVInfo.GetEventCode()<<"\t"<<
                                EventCSVInfo.GetSourceComponent()<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions();
        return false;

    }
    return true;

}

bool EventHandlerThreadController::VerifyActionGUIOptionsDependency(EventHandler::EventCSVInfo EventCSVInfo) {
    //ActionNegative the action taken on negative acknowledgement by the user ,which means guiOption
    //is has to be filled properly with valid button type other than GUI:NO_BUTTON


    Global::ActionType Acn = EventCSVInfo.GetActionNegative();
    if(VerifyAction(Acn) && (Acn != Global::ACNTYPE_NONE) && (EventCSVInfo.GetButtonType() == Global::NO_BUTTON)) {
        /// > This condition implies that there is  positive or negative action, however no GUIMessage, which is not expected.

        qDebug()<< "Invalid ActionNegative - GUIOption combination set" <<EventCSVInfo.GetEventCode()<<"\t"
                <<Acn<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions()<<"\n";
        return false;
    }

    Acn = EventCSVInfo.GetActionPositive();
    if(VerifyAction(Acn)&& (Acn != Global::ACNTYPE_NONE) && (EventCSVInfo.GetButtonType() == Global::NO_BUTTON)) {
        /// > This condition implies that there is  positive or negative action, however no GUIMessage, which is not expected.

        qDebug()<< "Invalid ActionPositive - GUIOption combination set" <<EventCSVInfo.GetEventCode()<<"\t"
                <<Acn<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions()<<"\n";
        return false;
    }

    return true;
}

bool EventHandlerThreadController::VerifyAlarmGUIOptionsDependency(EventHandler::EventCSVInfo EventCSVInfo) {
     /// < If alarm is set then GUIOptions has to be set with a valid value.

    if((EventCSVInfo.GetAlarmStatus() == true) && (EventCSVInfo.GetGUIMessageBoxOptions() == Global::NO_BUTTON))
    {
        qDebug()<< "Invalid Alarm - GUIOption combination set"<<EventCSVInfo.GetEventCode()<<"\t"
                <<EventCSVInfo.GetAlarmStatus()<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions()<<"\n";
        return false;
    }
    return true;
}

bool EventHandlerThreadController::VerifyAlarmEventTypeDependency(EventHandler::EventCSVInfo EventCSVInfo) {

    //            1 . alarm should not be set if event type is info.

        if((EventCSVInfo.GetEventType() == Global::EVTTYPE_INFO) && (EventCSVInfo.GetAlarmStatus() == true)) {
             qDebug()<< "Invalid Alarm - EventType combination set"<<EventCSVInfo.GetEventCode()<<"\t"
                     <<EventCSVInfo.GetEventType()<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions();
                return false;
        }

            return true;

}

bool EventHandlerThreadController::VerifyStatusbarGUIOptionDependency( EventHandler::EventCSVInfo EventCSVInfo) {

       if((EventCSVInfo.GetStatusBarIcon() == true) && EventCSVInfo.GetGUIMessageBoxOptions() == Global::NO_BUTTON)
       {
           qDebug()<< "Invalid StatusBar icon GUIOption combination set"<<EventCSVInfo.GetEventCode()<<"\t"
                   <<EventCSVInfo.GetStatusBarIcon()<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions()<<"\n";
           return false;
       }
       return true;
}

/****************************************************************************/
/*!
 *  \brief    Hash Table update for the event count map
 *
 *  \param    valid event & event status
 */
/****************************************************************************/
void EventHandlerThreadController::UpdateEventDataStructures(quint32 EventID,
                                                             quint64 EventId64,
                                                             const DataLogging::DayEventEntry &EventEntry,
                                                             bool Remove,
                                                             bool AckByGUI)
{
//    quint32 EventKey = (quint32)EventId64 >> 32;
    quint32 EventKey = (quint32)(EventId64 & 0x00000000FFFFFFFF);
    if (Remove) {
        if (AckByGUI) {
            //We no longer need these data
            m_EventKeyDataMap.remove(EventId64);
            qint32 Index = m_EventIDCount.indexOf(EventID);
            if (Index != -1) {
                m_EventIDCount.removeAt(Index);
            }
            //quint32 EventKey = static_cast<quint32>(EventId64 >> 32);
            m_EventIDKeyHash.remove(EventID);
            //Unblock this EventKey so that it can be reused.
            Global::EventObject::Instance().UnBlockEventKey(EventKey);

            m_EventKeyIdMap.remove(EventKey);
        }
    }
    else {
        //New event, add only if the event has to be sent to GUI
        if(EventEntry.GetGUIMessageBoxOptions() != Global::NO_BUTTON) {
            m_EventIDCount.append(EventID);
            m_EventKeyDataMap.insert(EventId64, EventEntry);
            if (!m_EventIDKeyHash.contains(EventID)) {
                m_EventIDKeyHash.insert(EventID, EventKey);
            }
            m_EventKeyIdMap.insert(EventKey, EventID);
        }
    }
    qDebug()<<"Event Entry status in UpdateEventDataStructures();"<<EventEntry.IsEventActive();
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
void EventHandlerThreadController::ProcessEvent(const quint32 EventID, const Global::tTranslatableStringList &EventStringList,
                                                const bool EventStatus, const quint32 EventKey)
{
    qDebug() << "EventHandlerThreadController::ProcessEvent, EventID=" << EventID << "EventKey=" << EventKey << "Event status" <<EventStatus;

    if(!m_eventList.contains(EventID)){
        qDebug()<<"Error in processing Event : " << EventID << "\t doesn't exist \n";
        return;
    }

   // m_EventIdKeyMap.insert(EventKey,EventID);


    // if eventList is not available yet, place event into pendingList
    if (m_eventList.size() == 0) {
        qDebug()<<"Event list not available \n\n\n";
        PendingEvent pe;
        pe.EventID = EventID;
        pe.EventStringList = EventStringList;
        pe.EventKey = EventKey;
        pe.EventStatus = EventStatus;
        m_pendingEvents.push_back(pe);// These events, if any, are processed after successfully reading the EventConf file
        return;
    }
    else if (EventID == EVENT_GUI_AVAILABLE)
    {
        SetGuiAvailable(true);
        //We dont need to log this particular event ,
        //instead we log for eg.EVENT_PROCESS_COLORADO_GUI_CONNECTED
        return;
    }
    else {
        quint64 EventId64 = EventID;
        EventId64 <<= 32;
        EventId64 |= EventKey;
        DataLogging::DayEventEntry EventEntry;
        EventCSVInfo EventInfo = m_eventList.value(EventID);
        //EventEntry encapsulates EventInfo. EventEntry is also
        //passed to the logging component.
        CreateEventEntry(EventEntry, EventInfo, EventStatus, EventID, EventStringList, EventKey);

        SetSystemStateMachine(EventEntry);
        if (!EventStatus) {
            HandleInactiveEvent(EventEntry, EventId64);
        }
        else {
            InformAlarmHandler(EventEntry, EventId64, true);
            UpdateEventDataStructures(EventID, EventId64, EventEntry, false);
        }

        //Log if loglevel is not "NONE"
        if (EventEntry.GetLogLevel() != Global::LOGLEVEL_NONE)
        {
            qDebug()<< "Sending event to DataLogger";
            emit LogEventEntry(EventEntry); //Log the event
        }
        InformGUI(EventEntry, EventId64);// Send the Error for handling
        /// \todo this is a test of Axeda Remote Care error reporting:
        //emit ErrorHandler::SendErrorToRemoteCare(EventEntry);
    }
}

void EventHandlerThreadController::SetSystemStateMachine(const DataLogging::DayEventEntry &TheEvent)
{
    if ((TheEvent.GetActionType() == Global::ACNTYPE_BUSY)
            || (TheEvent.GetActionType() == Global::ACNTYPE_IDLE)) {
        if(TheEvent.GetString().count() > 0) { // e.g. Rack inserted
            // TheEvent.GetString()).at(0).GetString().toInt() = RACK RFID
            EventHandler::StateHandler::Instance().setActivityUpdate(true, (TheEvent.GetString()).at(0).GetString().toInt());
        }
    }

    if (TheEvent.GetActionType() == Global::ACNTYPE_STOP) {
        EventHandler::StateHandler::Instance().setAvailability(TheEvent.IsEventActive(), TheEvent.GetEventCode());
    }
}

void EventHandlerThreadController::InformGUI(const DataLogging::DayEventEntry &TheEvent, const quint64 EventId64)
{
    if ((TheEvent.GetGUIMessageBoxOptions() != Global::NO_BUTTON) || (TheEvent.GetStatusIcon()))
    {
        NetCommands::EventReportDataStruct EventReportData;
        EventReportData.EventStatus = TheEvent.IsEventActive(); //False means event not active, True if event active.
        EventReportData.EventType = TheEvent.GetEventType();  // Global::EVTTYPE_ERROR;
        EventReportData.ID = EventId64;
//        EventReportData.EventKey = (quint32)(EventId64 >> 32);
        EventReportData.EventKey = (quint32)(EventId64 & 0x00000000FFFFFFFF);
        EventReportData.MsgString = Global::EventTranslator::TranslatorInstance().Translate(Global::TranslatableString(TheEvent.GetEventCode(), TheEvent.GetString())); //"Event String translated to the set langauge";
        EventReportData.Time = TheEvent.GetTimeStamp().toString();   // Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
        EventReportData.BtnType = TheEvent.GetButtonType();
        EventReportData.StatusBarIcon = TheEvent.GetStatusIcon();   //true if GUI must set status bar icon.

        if (m_GuiAvailable)
        {
            Global::tRefType Ref = GetNewCommandRef();
            m_EventKeyRefMap.insert(Ref, EventReportData.EventKey);
            SendCommand(Ref, Global::CommandShPtr_t(new NetCommands::CmdEventReport(Global::Command::MAXTIMEOUT, EventReportData)));
        }
        else    // GUI is not around yet => remember data and send later
        {
            mPendingGuiEventList.push_back(EventReportData);
        }
    }
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
    qDebug()<<"EventKey In Acknowledge" << EventKey;
    quint32 EventID = m_EventKeyIdMap.value(EventKey);
    qDebug()<<"Event ID In Acknowledge" << EventID;
    EventCSVInfo EventInfo = m_eventList.value(EventID);

    quint64 EventId64 = EventID;
    EventId64 <<= 32;
    EventId64 |= EventKey;

    DataLogging::DayEventEntry EventEntry = m_EventKeyDataMap.value(EventId64);

    if (EventEntry.GetEventKey() == 0) {
        qDebug() << "EventHandlerThreadController::OnAcknowledge, couldn't find Event" << EventId64 << EventID;
        return;
    }

    EventEntry.SetEventCSVInfo(EventInfo);
    EventEntry.SetAckValue(Ack);
    EventEntry.SetDateTime(Global::AdjustedTime::Instance().GetCurrentDateTime());

    int Count = GetCountForEventKey(EventKey);

    if(EventEntry.GetLogLevel() != Global::LOGLEVEL_NONE)
    {
        qDebug()<<"OnAcknowledge() <<EventEntry" <<EventEntry.GetEventCode();
        LogEventEntry(EventEntry);
    }

    Global::EventSourceType SourceName = EventEntry.GetSourceComponent();
    if(SourceName == Global::EVENTSOURCE_NONE) {
        return;
    }

//    quint64 EventId64 = EventEntry.GetEventCode();
//    EventId64 <<= 32;
//    EventId64 |= EventKey;
    InformAlarmHandler(EventEntry, EventId64, false);
    UpdateEventDataStructures(EventEntry.GetEventCode(), EventId64, EventEntry, true, true);
    bool IsActive = EventEntry.IsEventActive();
    if( IsActive == false) {
        return;
    }

    NetCommands::CmdSystemAction *p_CmdSystemAction;
    p_CmdSystemAction = new NetCommands::CmdSystemAction();
    EventKey = (Ack.GetEventKey() & 0x00000000ffffffff);
    EventID = (quint32)((Ack.GetEventKey() & 0xffffffff00000000) >> 32) ;
    p_CmdSystemAction->SetEventKey(EventKey);
    p_CmdSystemAction->SetEventID(EventID);
    p_CmdSystemAction->SetSource(EventEntry.GetSourceComponent());
    p_CmdSystemAction->SetRetryCount(Count);
    p_CmdSystemAction->SetStringList(EventEntry.GetString());
    Global::tRefType NewRef = GetNewCommandRef();


    if(Count <= EventEntry.GetRetryAttempts())
    {
        //send -ve command action if defined.
       if((EventEntry.GetAckValue() ==  NetCommands::OK_BUTTON) || (EventEntry.GetAckValue() ==  NetCommands::YES_BUTTON) || (EventEntry.GetAckValue() == NetCommands::CONTINUE_BUTTON))
        {
            //send +ve command action if defined.
            p_CmdSystemAction->SetAction(EventEntry.GetActionPositive());
        }
        else
        {
            //send -ve command action if defined.
            p_CmdSystemAction->SetAction(EventEntry.GetActionNegative());
        }       
    }
    else
    {
        // if (second action )//second action
        p_CmdSystemAction->SetAction(EventEntry.GetActionPositive());

    }

    qDebug() << "EventHandlerThreadController::OnAcknowledge, EventKey=" << EventKey << "EventID=" <<EventID;
    SendCommand(NewRef, Global::CommandShPtr_t(p_CmdSystemAction));
    DEBUGWHEREAMI;
}

void EventHandlerThreadController::SetGuiAvailable(const bool active)
{
    qDebug() << "ActionHandler::SetGuiAvailable" << active;
    m_GuiAvailable = active;

    if (active)
    {
        foreach(NetCommands::EventReportDataStruct EventReportData, mPendingGuiEventList)
        {
            Global::tRefType Ref = GetNewCommandRef();
            m_EventKeyRefMap.insert(Ref,EventReportData.EventKey);
            SendCommand(Ref, Global::CommandShPtr_t(new NetCommands::CmdEventReport(Global::Command::MAXTIMEOUT, EventReportData)));
        }
        mPendingGuiEventList.clear();
    }
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

bool EventHandlerThreadController::VerifyEventConfigCSV(QString filename) {

    return(VerifyEventCSVFilenameExists(filename));


}

bool EventHandlerThreadController::VerifyEventCSVFilenameExists(QString strFilename) {

    if(strFilename.isEmpty())
    {
         QFile file (strFilename);
        if (!file.open (QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug()<<"File Open Failed" << file.errorString();
            return false;
        }
        file.close();

        return true;
    }
    else
        return false;
}


bool EventHandlerThreadController::VerifyEventIDs(quint32 EventId) {

    /// < Only check on EventID as of now
    if(EventId <= 0)
    {
        return false;

    }
     return true;
}

bool EventHandlerThreadController::VerifyEventMacro(QString EventMacroName) {

    /// < Only check on EventMacroName as of now
    if(EventMacroName.isEmpty())
    {
        return false;

    }
     return true;
}

bool EventHandlerThreadController::VerifyEventType(Global::EventType EventType) {

    /// < Should be one of -  “ERROR”, ”Info”, “WARNING” else raise an error

    switch (EventType)
    {
    case Global::EVTTYPE_ERROR:
    case Global::EVTTYPE_WARNING:
    case Global::EVTTYPE_INFO:
        return true;
    default:
        return false;
    }

}

bool EventHandlerThreadController::VerifyAction(Global::ActionType ActionType) {

    switch (ActionType)
    {
    case Global::ACNTYPE_ERROR:
    case Global::ACNTYPE_RETRY:
    case Global::ACNTYPE_INFO:
    case Global::ACNTYPE_SHUTDOWN:
    case Global::ACNTYPE_STOP:
    case Global::ACNTYPE_REBOOT:
    case Global::ACNTYPE_WARNING:
    case Global::ACNTYPE_BUSY:
    case Global::ACNTYPE_IDLE:
    case Global::ACNTYPE_REMOVEALLRACKS:
    case Global::ACNTYPE_REMOVERACK:
    case Global::ACNTYPE_NONE:
    return true;
   case Global::ACNTYPE_UNEXPECTED:
    default:
        return false;
    }
}

bool EventHandlerThreadController::VerifySource(Global::EventSourceType EventSourceType) {

    switch (EventSourceType)
    {
    case Global::EVENTSOURCE_MAIN:
    case Global::EVENTSOURCE_DEVICECOMMANDPROCESSOR:
    case Global::EVENTSOURCE_SCHEDULER:
    case Global::EVENTSOURCE_EXPORT:
    case Global::EVENTSOURCE_IMPORTEXPORT:
    case Global::EVENTSOURCE_BLG:
    case Global::EVENTSOURCE_EVENTHANDLER:
    case Global::EVENTSOURCE_SEPIA:
    case Global::EVENTSOURCE_NONE:
    case Global::EVENTSOURCE_DATALOGGER:
        return true;
    case Global::EVENTSOURCE_NOTEXIST:
    default:
        return false;
    }
}

bool EventHandlerThreadController::VerifyLogLevel(Global::EventLogLevel EventLogLevel) {

    switch(EventLogLevel) {
    case Global::LOGLEVEL_NONE:
    case Global::LOGLEVEL_LOW:
    case Global::LOGLEVEL_MEDIUM:
    case Global::LOGLEVEL_HIGH:
        return true;
    case Global::LOGLEVEL_UNEXPECTED:
    default:
        return false;
    }
}

bool EventHandlerThreadController::VerifyBoolean(QString UserText) {

    return((UserText == "TRUE") || (UserText == "YES") || (UserText == "FALSE") || (UserText == "NO") || (UserText.isEmpty() ));

}

bool EventHandlerThreadController::VerifyGuiButtonType(Global::GuiButtonType GuiButtonType ) {

    switch(GuiButtonType) {
    case Global::OK:
    case Global::OK_CANCEL:
    case Global::YES_NO:
    case Global::CONTINUE_STOP:
    case Global::NO_BUTTON:
        return true;
    default:
        return false;
    }

}

bool EventHandlerThreadController::VerifyStringList(QString EventString) {

    if(EventString.isEmpty())
        return false;
    return true;
}

} // end namespace EventHandler
