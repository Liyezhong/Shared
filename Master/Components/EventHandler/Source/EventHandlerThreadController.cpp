/****************************************************************************/
/*! \file Source/EventHandlerThreadController.cpp
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


//#define VALIDATE


/****************************************************************************/
/**
 * \brief Namespace containing all classes for system event handling.
 */
/****************************************************************************/
namespace EventHandler {

const quint32 TIMEOUT_FOR_ALARM = 5000; //!< Alarm plays every 5 seconds

/****************************************************************************/
/*!
 *  \brief    Constructor for the EventHandlerThreadController
 *
 *  \iparam   ThreadID
 *  \iparam   RebootCount
 *  \iparam   FileList
 *
 *  \return
 *
 ****************************************************************************/
EventHandlerThreadController::EventHandlerThreadController(quint32 ThreadID, quint32 RebootCount, QStringList FileList) :
    Threads::ThreadController(ThreadID, "EventHandler"),
    m_EventLoggerMaxFileSize(0),
    m_DayEventLoggerMaxFileCount(0),
    m_MaxAdjustedTimeOffset(0),
    m_GuiAvailable(false),
    m_RebootCount(RebootCount),
    mp_AlarmHandler(NULL)
{
    mp_AlarmHandler = new Global::AlarmHandler(this, TIMEOUT_FOR_ALARM);
    // Register LoggingSource Templates with moc
    qRegisterMetaType<Global::LoggingSource>("Global::LoggingSource");
    qRegisterMetaType<Global::AlternateEventStringUsage>("Global::AlternateEventStringUsage");
    AddActionTypes();
    AddEventTypes();
    AddEventLogLevels();
    AddSourceComponents();

    QStringList TempFileList;
    m_eventList.clear();

    // read all the events from files and store it in a list
    foreach (QString FileName, FileList) {
        // in the list if multiple configuration files with the same file name
        // and same file path then read the file once
        if (!TempFileList.contains(FileName)) {
            (void)ReadConfigFile(FileName);
            TempFileList.append(FileName);
        }
    }
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
    // now register commands
    RegisterCommands();
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
/****************************************************************************/
/*!
 *  \brief   Function to add Event log levels.
 *
 *
 *  \return
 *
 ****************************************************************************/
void EventHandlerThreadController::AddEventLogLevels() {
    m_EventLogLevelEnumMap.insert("NONE", Global::LOGLEVEL_NONE);
    m_EventLogLevelEnumMap.insert("MEDIUM", Global::LOGLEVEL_MEDIUM);
    m_EventLogLevelEnumMap.insert("LOW", Global::LOGLEVEL_LOW);
    m_EventLogLevelEnumMap.insert("HIGH", Global::LOGLEVEL_HIGH);
    m_EventLogLevelEnumMap.insert("",Global::LOGLEVEL_NONE);

}
/****************************************************************************/
/*!
 *  \brief   Function to add source components.
 *
 *
 *  \return
 *
 ****************************************************************************/
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
/****************************************************************************/
/*!
 *  \brief   Function to handle Inactive Events.
 *
 *
 *  \iparam &EventEntry
 *  \iparam &EventId64
 *
 ****************************************************************************/
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

    quint32 PreviousEventKey = m_EventIDKeyHash.take(EventID);
    qDebug()<<"Previous Event Key" << PreviousEventKey;
    EventId64 = EventID;
    EventId64 <<= 32;
    EventId64 |= PreviousEventKey;
    qDebug()<<"EventID64" << EventId64;

    InformAlarmHandler(EventEntry, EventId64, false);
    UpdateEventDataStructures(EventID, EventId64, EventEntry, true, (EventOccurenceCount== 1) ? true : false);
}
/****************************************************************************/
/*!
 *  \brief   Function to create Events entry.
 *
 *
 *  \iparam &EventEntry
 *  \iparam &EventInfo
 *  \iparam EventStatus
 *  \iparam EventID
 *  \iparam &EventStringList
 *  \iparam EventKey
 *  \iparam AltStringUsage
 *
 ****************************************************************************/
void EventHandlerThreadController::CreateEventEntry(DataLogging::DayEventEntry &EventEntry,
                                                    EventCSVInfo &EventInfo,
                                                    const bool EventStatus,
                                                    const quint32 EventID,
                                                    const Global::tTranslatableStringList &EventStringList,
                                                    const quint32 EventKey, const Global::AlternateEventStringUsage AltStringUsage)
{
    if (EventInfo.GetEventId() == 0 || !m_eventList.contains(EventInfo.GetEventId()))
    {
        qDebug()<<"Unknown Event ID \n\n\n";
        EventInfo = m_eventList.value(EVENT_EVENT_ID_MISSING);
        EventEntry.SetString(Global::FmtArgs() << EventID);
    }
    else
    {
        EventEntry.SetString(EventStringList);
    }

    EventEntry.SetEventKey(EventKey);
    EventEntry.SetEventCSVInfo(EventInfo);
    EventEntry.SetDateTime(Global::AdjustedTime::Instance().GetCurrentDateTime());

    EventEntry.SetEventStatus(EventStatus);
    EventEntry.SetAltStringUsage(AltStringUsage);
}
/****************************************************************************/
/*!
 *  \brief   Function to inform Alarm Handler.
 *
 *
 *  \iparam &EventEntry
 *  \iparam EventId64
 *  \iparam StartAlarm
 *
 ****************************************************************************/
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
            mp_AlarmHandler->setAlarm(EventId64, AlarmType, StartAlarm);
        }
    }
}

void EventHandlerThreadController::InformAlarmHandler(Global::EventType EvtType, const quint64 EventId64, bool StartAlarm)
{
    Global::AlarmType AlarmType = Global::ALARM_NONE;
    if (EvtType == Global::EVTTYPE_ERROR)
        AlarmType = Global::ALARM_ERROR;
    else if (EvtType == Global::EVTTYPE_WARNING)
        AlarmType = Global::ALARM_WARNING;

    mp_AlarmHandler->setAlarm(EventId64, AlarmType, StartAlarm);
}

void EventHandlerThreadController::ResetAlarm()
{
    mp_AlarmHandler->reset();
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
    m_ActionTypeEnumMap.insert("REJECTRETRY",Global::ACNTYPE_REJECT_RETRY);
    m_ActionTypeEnumMap.insert("FORWARD",Global::ACNTYPE_FORWARD);
    m_ActionTypeEnumMap.insert("INFO",Global::ACNTYPE_INFO);
    m_ActionTypeEnumMap.insert("ERROR",Global::ACNTYPE_ERROR);
    m_ActionTypeEnumMap.insert("WARNING",Global::ACNTYPE_WARNING);
    m_ActionTypeEnumMap.insert("BUSY",Global::ACNTYPE_BUSY);
    m_ActionTypeEnumMap.insert("IDLE",Global::ACNTYPE_IDLE);
    m_ActionTypeEnumMap.insert("REMOVEALLRACKS",Global::ACNTYPE_REMOVEALLRACKS);
    m_ActionTypeEnumMap.insert("REMOVERACK",Global::ACNTYPE_REMOVERACK);
    m_ActionTypeEnumMap.insert("RESCAN",Global::ACNTYPE_RESCAN);
    m_ActionTypeEnumMap.insert("CHECKOVERFILL",Global::ACNTYPE_CHECK_OVERFILL);
    m_ActionTypeEnumMap.insert("USEUNLOADER",Global::ACNTYPE_USE_UNLOADER);
    m_ActionTypeEnumMap.insert("REINIT",Global::ACNTYPE_REINIT);
    m_ActionTypeEnumMap.insert("FINISHINIT",Global::ACNTYPE_FINISH_INIT);
    m_ActionTypeEnumMap.insert("CONTINUESTAINING",Global::ACNTYPE_CONTINUE_STAINING);
    m_ActionTypeEnumMap.insert("NORESCAN",Global::ACNTYPE_NO_RESCAN);
    m_ActionTypeEnumMap.insert("DONTPROCESSRACK",Global::ACNTYPE_DONT_PROCESS_RACK);
    m_ActionTypeEnumMap.insert("", Global::ACNTYPE_NONE);
}


/****************************************************************************/
/*!
 *  \brief    Reads event definition from file
 *
 *  \iparam    filename
 *
 *  \return    true or false
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

    QTextStream stream ( &file );
    QString line;

    //required once the event is in table, even if nothing is specified in the required column
    while( !stream.atEnd() ) {
        line = stream.readLine();
        if (line.left(1) != "#") {
            QStringList textList = line.split(",");
            EventHandler::EventCSVInfo EventCSVInfoObj;

            //! \ Get EventCode
            int EventId = 0;
            if (textList.count() > 0) {
                EventId = textList.at(0).toInt();

                ret = VerifyEventIDs(EventId);
                EventCSVInfoObj.SetEventId(EventId); // only int check, no other validations done
            }

            //! \ Get EventMacro (1)
            QString EventMacroName = "";
            if (textList.count() > 1) {
                EventMacroName = textList.at(1).trimmed(); // No validations required
                HandleImproperEvents(EventId, VerifyEventMacro(EventMacroName), 1);
            }

            //! \ Get EventType (2)
            if (textList.count() > 2) {
                Global::EventType EventType = m_EventTypeEnumMap.value(textList.at(2).trimmed().toUpper(), Global::EVTTYPE_UNDEFINED);
                HandleImproperEvents(EventId, VerifyEventType(EventType), 2);
                EventCSVInfoObj.SetEventType(EventType);
            }

            //! \ Get ActionType (3)
            if (textList.count() > 3)
            {
                QStringList ActionInfo = textList.at(3).split("+");

                //qDebug() << ActionInfo.at(0).trimmed().toUpper();
                EventCSVInfoObj.SetActionPositive(m_ActionTypeEnumMap.value(ActionInfo.at(0).trimmed().toUpper(), Global::ACNTYPE_UNEXPECTED));

                if (ActionInfo.count() > 1) { // we have a second action in case the first one fails
                    EventCSVInfoObj.SetRetries(ActionInfo.at(1).trimmed().toInt());

                    if (ActionInfo.count() > 2) {
                        EventCSVInfoObj.SetFinalAction(m_ActionTypeEnumMap.value(ActionInfo.at(2).trimmed().toUpper(), Global::ACNTYPE_UNEXPECTED));
                    }
                }

                if (ActionInfo.count() >= 1) {
                    ret = (VerifyAction(EventCSVInfoObj.GetActionPositive()) && VerifyAction(EventCSVInfoObj.GetFinalAction()));
                    if (ret) {
                        if ((EventCSVInfoObj.GetFinalAction() != Global::ACNTYPE_NONE) && (EventCSVInfoObj.GetRetryAttempts() <= 0)) {
                            ret = false;
                        }
                    }
                    HandleImproperEvents(EventId, ret, 3);
                }
            }

            //! \ Get ActionNegative (4)
            if (textList.count() > 4) {
                QStringList ActionInfo = textList.at(4).split("+");

                EventCSVInfoObj.SetActionNegative(m_ActionTypeEnumMap.value(ActionInfo.at(0).trimmed().toUpper(), Global::ACNTYPE_UNEXPECTED));

                HandleImproperEvents(EventId, VerifyAction(EventCSVInfoObj.GetActionNegative()), 4);
            }

            //! \ Get Source(5) component name & Set
            if (textList.count() > 5) {
                QString Source = textList.at(5).trimmed();
                Global::EventSourceType EventSourceType = Global::EVENTSOURCE_NOTEXIST;
                EventSourceType = m_EventSourceMap.value(Source.toUpper(),Global::EVENTSOURCE_NOTEXIST);

                HandleImproperEvents(EventId, VerifySource(EventSourceType), 5);

                EventCSVInfoObj.SetEventSource(EventSourceType);
                // Assumed Ack is required if source component name is set
                if(EventSourceType == Global::EVENTSOURCE_NONE)
                {
                    EventCSVInfoObj.SetAckRequired(false);
                }
                else
                {
                    EventCSVInfoObj.SetAckRequired(true);
                }
            }

            //! \ Get AlarmTone(6) flag
            if (textList.count() > 6) {
                bool AlarmFlag = false;

                if((textList.at(6).trimmed().toUpper().simplified() == "NO") || (textList.at(6).trimmed().toUpper() == "")) {
                    AlarmFlag = false;
                }

                else if(textList.at(6).trimmed().toUpper().simplified() == "YES") {
                    AlarmFlag = true;
                }
                else {
                    HandleImproperEvents(EventId, false, 6);
                }
                EventCSVInfoObj.SetAlarmStatus(AlarmFlag);
            }

            //! \ Get LogLevel(7) flag
            if (textList.count() > 7) {
                Global::EventLogLevel EventLogLevel = Global::LOGLEVEL_UNEXPECTED;
                EventLogLevel = m_EventLogLevelEnumMap.value(textList.at(7).toUpper());
                HandleImproperEvents(EventId, VerifyLogLevel(EventLogLevel), 7);
                EventCSVInfoObj.SetLogLevel(EventLogLevel);
            }

            //! \ Get UserLog(8) flag
            if (textList.count() > 8) {
                bool UserLogFlag = false;
                QString strUserLog = textList.at(8).toUpper().simplified();
                HandleImproperEvents(EventId, VerifyBoolean(strUserLog), 8);
                UserLogFlag = ((textList.at(8).toUpper() == "TRUE") || (textList.at(8).toUpper() == "YES"));
                EventCSVInfoObj.SetRunLogStatus(UserLogFlag);
            }

            //! \ Get EventString(9)
            if (textList.count() > 9) {
                // ! \ EventString comes from the RaiseEvent call, just verify the data is not empty
                QString EventString = textList.at(9);
                HandleImproperEvents(EventId, VerifyStringList(EventString), 9);
            }

            //! \ Get GUIEvent(10) list
            if (textList.count() > 10) {
                Global::GuiButtonType BtnType;
                BtnType = Global::StringToGuiButtonType(textList.at(10).toUpper());
                HandleImproperEvents(EventId, VerifyGuiButtonType(BtnType), 10);
                EventCSVInfoObj.SetButtonType(BtnType);
            }

            //! \ Get StatusBarIcon(11) flag
            if (textList.count() > 11) {
                bool StatusIconFlag = false;
                QString strStatusIcon = textList.at(11).toUpper();
                HandleImproperEvents(EventId, VerifyBoolean(strStatusIcon), 11);
                StatusIconFlag = ((textList.at(11).toUpper() == "TRUE") || (textList.at(11).toUpper() == "YES"));

                EventCSVInfoObj.SetStatusBarIcon(StatusIconFlag);
            }

            // if the same event id already exists -> remove previous one (project specific event settings are allowed to overwrite platform settings)
            if (m_eventList.contains(EventCSVInfoObj.GetEventId())) {
                m_eventList.remove(EventCSVInfoObj.GetEventId());
            }

            m_eventList.insert(EventCSVInfoObj.GetEventId(), EventCSVInfoObj);

            //! \ validate dependencies between the values corresponding to an event.
            HandleImproperEvents(EventId, VerifyUserLogGUIOptionDependency(EventCSVInfoObj), 100);
            HandleImproperEvents(EventId, VerifySourceComponentGUIOptionsDependency(EventCSVInfoObj), 101);
            HandleImproperEvents(EventId, VerifyActionGUIOptionsDependency(EventCSVInfoObj), 102);
            HandleImproperEvents(EventId, VerifyAlarmGUIOptionsDependency(EventCSVInfoObj), 103);
            HandleImproperEvents(EventId, VerifyAlarmEventTypeDependency(EventCSVInfoObj), 104);
            HandleImproperEvents(EventId, VerifyStatusbarGUIOptionDependency(EventCSVInfoObj), 105);
        }
    }
    file.close();

    QHashIterator<quint32, quint8> i(m_improperEvents);
    while (i.hasNext())
    {
        (void)i.next();

        PendingEvent pe;
        pe.EventID = EVENT_CONFIG_FILE;
        pe.EventStringList << QString::number(i.key()) << QString::number(i.value());
        pe.EventKey = 0;
        pe.EventStatus = true;
        pe.AltStringUsuage = Global::NOT_APPLICABLE;
        m_pendingEvents.push_back(pe);// These events, if any, are processed after successfully reading the EventConf file

        //Global::EventObject::Instance().RaiseEvent(i.key(), Global::FmtArgs() << i.value());
    }

    return (m_improperEvents.count() > 0);
}
/****************************************************************************/
/*!
 *  \brief   Function to Verify User Log GUIOption Dependency.
 *
 *
 *  \iparam InfoUserLog
 *
 *  \return true or false
 *
 ****************************************************************************/
bool EventHandlerThreadController::VerifyUserLogGUIOptionDependency( EventHandler::EventCSVInfo InfoUserLog ) {

    if ((InfoUserLog.GetRunLogStatus() == false) &&
            (InfoUserLog.GetGUIMessageBoxOptions() != Global::NOT_SPECIFIED) &&
            (InfoUserLog.GetGUIMessageBoxOptions() != Global::WITHOUT_BUTTONS)) {

//        qDebug()<< "Invalid UserLog - GUIOption combination set" << InfoUserLog.GetEventCode()<<"\t"
//                <<InfoUserLog.GetRunLogStatus()<<"\t"<<InfoUserLog.GetGUIMessageBoxOptions()<<"\n";
        return false;
    }
    return true;
}
/****************************************************************************/
/*!
 *  \brief   Function to Verify Source Component GUIOptions Dependency.
 *
 *
 *  \iparam InfoVerifySource
 *
 *  \return true or false
 *
 ****************************************************************************/
bool EventHandlerThreadController::VerifySourceComponentGUIOptionsDependency(EventHandler::EventCSVInfo InfoVerifySource) {

    if((InfoVerifySource.GetSourceComponent() != Global::EVENTSOURCE_NONE) && (InfoVerifySource.GetGUIMessageBoxOptions() == Global::NOT_SPECIFIED )) {

//        qDebug()<< "Invalid SourceComponent - GUIOption combination set"<< InfoVerifySource.GetEventCode()<<"\t"<<
//                                InfoVerifySource.GetSourceComponent()<<"\t"<<InfoVerifySource.GetGUIMessageBoxOptions();
        return false;
    }
    return true;
}
/****************************************************************************/
/*!
 *  \brief   Function to Verify Action GUIOptions Dependency.
 *
 *
 *  \iparam InfoVerifyAction
 *
 *  \return true or false
 *
 ****************************************************************************/
bool EventHandlerThreadController::VerifyActionGUIOptionsDependency(EventHandler::EventCSVInfo InfoVerifyAction) {
    //ActionNegative the action taken on negative acknowledgement by the user ,which means guiOption
    //is has to be filled properly with valid button type other than GUI:NOT_SPECIFIED


    Global::ActionType Acn = InfoVerifyAction.GetActionNegative();
    if(VerifyAction(Acn) && (Acn != Global::ACNTYPE_NONE) && (InfoVerifyAction.GetButtonType() == Global::NOT_SPECIFIED)) {
        /// > This condition implies that there is  positive or negative action, however no GUIMessage, which is not expected.

        if ((Acn != Global::ACNTYPE_BUSY) && (Acn != Global::ACNTYPE_IDLE) && (Acn != Global::ACNTYPE_STOP)) {  // these actions are triggered by firing the event
            return false;
        }
    }

    Acn = InfoVerifyAction.GetActionPositive();
    if(VerifyAction(Acn)&& (Acn != Global::ACNTYPE_NONE) && (InfoVerifyAction.GetButtonType() == Global::NOT_SPECIFIED)) {
        /// > This condition implies that there is  positive or negative action, however no GUIMessage, which is not expected.

        if ((Acn != Global::ACNTYPE_BUSY) && (Acn != Global::ACNTYPE_IDLE) && (Acn != Global::ACNTYPE_STOP)) {  // these actions are triggered by firing the event
            return false;
        }
    }

    return true;
}
/****************************************************************************/
/*!
 *  \brief   Function to Verify Alarm GUIOptions Dependency.
 *
 *
 *  \iparam InfoVerifyAlarm
 *
 *  \return true or false
 *
 ****************************************************************************/
bool EventHandlerThreadController::VerifyAlarmGUIOptionsDependency(EventHandler::EventCSVInfo InfoVerifyAlarm) {
     /// < If alarm is set then GUIOptions has to be set with a valid value.

    if((InfoVerifyAlarm.GetAlarmStatus() == true) && (InfoVerifyAlarm.GetGUIMessageBoxOptions() == Global::NOT_SPECIFIED))
    {
//        qDebug()<< "Invalid Alarm - GUIOption combination set"<<InfoVerifyAlarm.GetEventCode()<<"\t"
//                <<InfoVerifyAlarm.GetAlarmStatus()<<"\t"<<InfoVerifyAlarm.GetGUIMessageBoxOptions()<<"\n";
        return false;
    }
    return true;
}
/****************************************************************************/
/*!
 *  \brief   Function to Verify Alarm EventType Dependency.
 *
 *
 *  \iparam InfoVerifyAlarmEventType
 *
 *  \return true or false
 *
 ****************************************************************************/
bool EventHandlerThreadController::VerifyAlarmEventTypeDependency(EventHandler::EventCSVInfo InfoVerifyAlarmEventType) {

    //            1 . alarm should not be set if event type is info.

    if((InfoVerifyAlarmEventType.GetEventType() == Global::EVTTYPE_INFO) && (InfoVerifyAlarmEventType.GetAlarmStatus() == true)) {
//        qDebug()<< "Invalid Alarm - EventType combination set"<<InfoVerifyAlarmEventType.GetEventCode()<<"\t"
//                <<InfoVerifyAlarmEventType.GetEventType()<<"\t"<<InfoVerifyAlarmEventType.GetGUIMessageBoxOptions();
        return false;
    }

    return true;

}
/****************************************************************************/
/*!
 *  \brief   Function to Verify Statusbar GUIOption Dependency.
 *
 *
 *  \iparam InfoVerifyStatusBar
 *
 *  \return true false
 *
 ****************************************************************************/
bool EventHandlerThreadController::VerifyStatusbarGUIOptionDependency( EventHandler::EventCSVInfo InfoVerifyStatusBar) {

       if((InfoVerifyStatusBar.GetStatusBarIcon() == true) && InfoVerifyStatusBar.GetGUIMessageBoxOptions() == Global::NOT_SPECIFIED)
       {
//           qDebug()<< "Invalid StatusBar icon GUIOption combination set"<<InfoVerifyStatusBar.GetEventCode()<<"\t"
//                   <<InfoVerifyStatusBar.GetStatusBarIcon()<<"\t"<<InfoVerifyStatusBar.GetGUIMessageBoxOptions()<<"\n";
           return false;
       }
       return true;
}

/****************************************************************************/
/*!
 *  \brief   Update Event DataStructures
 *
 *  \iparam    EventID
 *  \iparam    EventId64
 *  \iparam    &EventEntry
 *  \iparam    Remove
 *  \iparam    AckByGUI
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
        if(EventEntry.GetButtonType() != Global::NOT_SPECIFIED) {
            m_EventIDCount.append(EventID);
            m_EventKeyDataMap.insert(EventId64, EventEntry);
            if (!m_EventIDKeyHash.contains(EventID)) {
                m_EventIDKeyHash.insert(EventID, EventKey);
            }
            m_EventKeyIdMap.insert(EventKey, EventID);
        }
    }
    //qDebug()<<"Event Entry status in UpdateEventDataStructures();"<<EventEntry.IsEventActive();
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
 *  \iparam    EventID
 *  \iparam    EventStringList
 *  \iparam    EventStatus
 *  \iparam    EventKey
 *  \iparam    AltStringUsuage
 *
 ****************************************************************************/
void EventHandlerThreadController::ProcessEvent(const quint32 EventID,
                                                const Global::tTranslatableStringList &EventStringList,
                                                const bool EventStatus,
                                                const quint32 EventKey,
                                                const Global::AlternateEventStringUsage AltStringUsuage)
{
    // to avoid processing 'false' events which not had been 'true' before
    if (EventStatus) {
        m_activeEvents.append(EventID);
    }
    else {
        if (m_activeEvents.contains(EventID)) {
            m_activeEvents.removeOne(EventID);
        }
        else {
            return;
        }
    }
    //qDebug() << "\n\n\n\nEventHandlerThreadController::ProcessEvent, EventID=" << EventID << "EventKey=" << EventKey << "Event status" <<EventStatus;
    // if eventList is not available yet, place event into pendingList
    if (EventID == EVENT_GUI_AVAILABLE) {
            SetGuiAvailable(true);
            //We dont need to log this particular event ,
            //instead we log for eg.EVENT_PROCESS_COLORADO_GUI_CONNECTED
            return;
    }
    else if (m_eventList.size() == 0) {
        qDebug()<<"Event list not available \n\n\n";
        PendingEvent pe;
        pe.EventID = EventID;
        pe.EventStringList = EventStringList;
        pe.EventKey = EventKey;
        pe.EventStatus = EventStatus;
        pe.AltStringUsuage = AltStringUsuage;
        m_pendingEvents.push_back(pe);// These events, if any, are processed after successfully reading the EventConf file
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
        CreateEventEntry(EventEntry, EventInfo, EventStatus, EventID, EventStringList, EventKey, AltStringUsuage);
        if (EventEntry.GetActionPositive() == Global::ACNTYPE_REBOOT && (m_RebootCount + 1) > EventEntry.GetRetryAttempts()) {
            EventEntry.SetAltStringUsage(Global::GUI_MSG_BOX_LOGGING_AND_USER_RESPONSE);
        }
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
            LogEntry(EventEntry);
        }
        InformGUI(EventEntry, EventId64);
        /// \todo this is a test of Axeda Remote Care error reporting:
        emit SendEventToRemoteCare(EventEntry, EventId64);

        if (!EventStatus) {
            SetCountForEventId(EventID, 0);
        }
    }
}


void EventHandlerThreadController::ProcessEvent(const quint32 EventKey, const quint64 EventIDScenario,
                          const bool Active, const bool ActionResult,
                          const Global::tTranslatableStringList &EventStringParList,
                          const Global::tTranslatableStringList &EventRDStringParList)
{
    //should be overload
    Q_UNUSED(EventKey)
    Q_UNUSED(EventIDScenario)
    Q_UNUSED(Active)
    Q_UNUSED(ActionResult)
    Q_UNUSED(EventStringParList)
    Q_UNUSED(EventRDStringParList)
}

/****************************************************************************/
/*!
 *  \brief  Log the events which are there in queue
 *
 *  \iparam    &EventEntry
 */
/****************************************************************************/
void EventHandlerThreadController::LogEntry(const DataLogging::DayEventEntry &EventEntry)
{
    /// check the state of the sytem if it is in default state
    /// then system is not intialized data logging object
    /// If the system is in Soft switch monitor state then data logging is nitialized
    /// and ready to log the data in log file
    if (EventHandler::StateHandler::Instance().getCurrentOperationState().compare("DefaultState") == 0) {
        m_DayEventEntryList.append(EventEntry);
    }
    else {
        // emit all the pending entries
        if (m_DayEventEntryList.count() > 0) {
            for (int Counter = 0; Counter < m_DayEventEntryList.count(); Counter++) {
                emit LogEventEntry(m_DayEventEntryList.value(Counter));
            }
            m_DayEventEntryList.clear();
        }
        //qDebug()<< "Sending event to DataLogger";
        emit LogEventEntry(EventEntry); //Log the event
    }
}


/****************************************************************************/
/*!
 *  \brief  Sets System StateMachine
 *
 *  \iparam    &TheEvent
 */
/****************************************************************************/
void EventHandlerThreadController::SetSystemStateMachine(const DataLogging::DayEventEntry &TheEvent)
{
    if ((TheEvent.GetActionPositive() == Global::ACNTYPE_BUSY)
            || (TheEvent.GetActionPositive() == Global::ACNTYPE_IDLE)) {
        if(TheEvent.GetString().count() > 0) { // e.g. Rack inserted
            // TheEvent.GetString()).at(0).GetString().toInt() = RACK RFID
            //qDebug()<<"Setting Statemachine";
            EventHandler::StateHandler::Instance().setActivityUpdate((TheEvent.GetActionPositive() == Global::ACNTYPE_BUSY),
                                                                     (TheEvent.GetString()).at(0).GetString().toInt());
        }
    }

    if (TheEvent.GetActionPositive() == Global::ACNTYPE_STOP) {
        EventHandler::StateHandler::Instance().setAvailability(TheEvent.IsEventActive(), TheEvent.GetEventCode());
    }
}
/****************************************************************************/
/*!
 *  \brief  Function to inform GUI
 *
 *  \iparam    &TheEvent
 *  \iparam    EventId64
 *
 */
/****************************************************************************/
void EventHandlerThreadController::InformGUI(const DataLogging::DayEventEntry &TheEvent, const quint64 EventId64)
{
    if ((TheEvent.GetButtonType() != Global::NOT_SPECIFIED) || (TheEvent.GetStatusIcon()))
    {
        NetCommands::EventReportDataStruct EventReportData;
        EventReportData.EventStatus = TheEvent.IsEventActive(); //False means event not active, True if event active.
        EventReportData.EventType = TheEvent.GetEventType();  // Global::EVTTYPE_ERROR;
        EventReportData.ID = EventId64;
        EventReportData.EventKey = (quint32)(EventId64 & 0x00000000FFFFFFFF);

        const Global::AlternateEventStringUsage AltStringUsage = TheEvent.GetAltStringUsageType();
        bool UseAltEventString = false;
        if (AltStringUsage == Global::GUI_MSG_BOX ||
            AltStringUsage == Global::GUI_MSG_BOX_AND_LOGGING ||
            AltStringUsage == Global::GUI_MSG_BOX_LOGGING_AND_USER_RESPONSE) {
                UseAltEventString = true;
        }
        EventReportData.MsgString = Global::UITranslator::TranslatorInstance().Translate(Global::TranslatableString(TheEvent.GetEventCode(), TheEvent.GetString()),
                                                                                            UseAltEventString, true); //"Event String translated to the set langauge";
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
            /// \todo If GUI is not available need to see where to display a message
            //if (TheEvent.GetActionPositive() != Global::ACNTYPE_REBOOT) {
                mPendingGuiEventList.push_back(EventReportData);
            //}
            //else {
            //for reboot action we need gui to be connected. If GUI is not available, print Information on the console

            //}
        }
    }
}
/****************************************************************************/
/*!
 *  \brief  Function to Register Commands
 *
 */
/****************************************************************************/
void EventHandlerThreadController::RegisterCommands()
{

    // register acknowledges
    RegisterAcknowledgeForProcessing<
            NetCommands::CmdAcknEventReport, EventHandlerThreadController
            >(&EventHandlerThreadController::OnAcknowledge, this);

    RegisterAcknowledgeForProcessing<
            Global::AckOKNOK, EventHandlerThreadController
            >(&EventHandlerThreadController::OnAckOKNOK, this);

}

void EventHandlerThreadController::OnAckOKNOK(Global::tRefType Ref, const Global::AckOKNOK &Ack)
{
    Q_UNUSED(Ref);
    Q_UNUSED(Ack);
}

void EventHandlerThreadController::OnAcknowledge(Global::tRefType Ref, const NetCommands::CmdAcknEventReport &Ack)
{
    quint32 EventKey = m_EventKeyRefMap[Ref];
    //qDebug()<<"EventKey In Acknowledge" << EventKey;
    quint32 EventID = m_EventKeyIdMap.value(EventKey);
    //qDebug()<<"Event ID In Acknowledge" << EventID;
    EventCSVInfo EventInfo = m_eventList.value(EventID);

    quint64 EventId64 = EventID;
    EventId64 <<= 32;
    EventId64 |= EventKey;

    DataLogging::DayEventEntry EventEntry = m_EventKeyDataMap.value(EventId64);

    //remove the stored reference
    m_EventKeyRefMap.remove(Ref);

    if (EventEntry.GetEventKey() == 0) {
        qDebug() << "EventHandlerThreadController::OnAcknowledge, couldn't find Event" << EventId64 << EventID;
        return;
    }

    EventEntry.SetEventCSVInfo(EventInfo);
    EventEntry.SetAckValue(Ack.GetButtonClicked());
    EventEntry.SetDateTime(Global::AdjustedTime::Instance().GetCurrentDateTime());

    int Count = GetCountForEventId(EventID);
    SetCountForEventId(EventID, Count + 1);

    if(EventEntry.GetLogLevel() != Global::LOGLEVEL_NONE)
    {
        qDebug()<<"OnAcknowledge() <<EventEntry" <<EventEntry.GetEventCode();
        LogEventEntry(EventEntry);
    }
    InformAlarmHandler(EventEntry, EventId64, false);
    if (!EventEntry.GetStatusIcon()) {      // if event is present in status bar, it should not be removed yet because inactive event needs to get the correct key in order to update status list
        UpdateEventDataStructures(EventEntry.GetEventCode(), EventId64, EventEntry, true, true);
    }
    Global::EventSourceType SourceName = EventEntry.GetEventSource();
    if(SourceName == Global::EVENTSOURCE_NONE) {
        return;
    }

    bool IsActive = EventEntry.IsEventActive();
    if( IsActive == false) {
        return;
    }

    NetCommands::CmdSystemAction *p_CmdSystemAction;
    p_CmdSystemAction = new NetCommands::CmdSystemAction();
    EventKey = (Ack.GetEventKey() & 0x00000000ffffffff);
    /*lint -e435 */
    EventID = (quint32)((Ack.GetEventKey() & 0xffffffff00000000) >> 32) ;
    p_CmdSystemAction->SetEventKey(EventKey);
    p_CmdSystemAction->SetEventID(EventID);
    p_CmdSystemAction->SetSource(EventEntry.GetEventSource());
    p_CmdSystemAction->SetRetryCount(Count);
    p_CmdSystemAction->SetStringList(EventEntry.GetString());
    Global::tRefType NewRef = GetNewCommandRef();

    if (EventEntry.GetActionPositive() == Global::ACNTYPE_REBOOT) {
        Count = m_RebootCount;
    }
    if ((Count < EventEntry.GetRetryAttempts()) || (EventEntry.GetRetryAttempts() == 0))
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
        p_CmdSystemAction->SetAction(EventEntry.GetFinalAction());
        SetCountForEventId(EventID, 0);
    }

    qDebug() << "EventHandlerThreadController::OnAcknowledge, EventKey=" << EventKey << "EventID=" <<EventID;
    SendCommand(NewRef, Global::CommandShPtr_t(p_CmdSystemAction));
    DEBUGWHEREAMI;
}
/****************************************************************************/
/*!
 *  \brief  Function to set Gui as available.
 *
 *  \iparam    active
 */
/****************************************************************************/
void EventHandlerThreadController::SetGuiAvailable(const bool active)
{
    qDebug() << "EventHandlerThreadController::SetGuiAvailable" << active;
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
    // process pending events
    if (m_eventList.size() > 0)
    {
        foreach (PendingEvent pe, m_pendingEvents)
        {
            ProcessEvent(pe.EventID, pe.EventStringList, pe.EventStatus, pe.EventKey, pe.AltStringUsuage);
        }
    }
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
 * \iparam  PowerFailStage = power fail stage
 */
/****************************************************************************/
void EventHandlerThreadController::OnPowerFail(const Global::PowerFailStages PowerFailStage)
{
    Q_UNUSED(PowerFailStage)
}


void EventHandlerThreadController::SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd)
{
    //qDebug() << "EventHandlerThreadController::SendCommand" << Ref;
    Threads::ThreadController::SendCommand(Ref, Cmd);
}

Global::tRefType EventHandlerThreadController::GetNewCommandRef()
{
    return Threads::ThreadController::GetNewCommandRef();
}

/****************************************************************************/
/**
 * \brief Function to Verify EventConfigCSV file.
 *
 * \iparam filename
 *
 * \return  status of file
 */
/****************************************************************************/
bool EventHandlerThreadController::VerifyEventConfigCSV(QString filename) {

    return (VerifyEventCSVFilenameExists(filename));
}

/****************************************************************************/
/**
 * \brief Function to Verify EventConfigCSV file.
 *
 * \iparam  strFilename
 *
 * \return  status of file
 */
/****************************************************************************/
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

/****************************************************************************/
/**
 * \brief Function to handle improper events
 *
 * \iparam  eventId
 * \iparam  success = whether event is proper or not
 * \iparam  column = the column number in eventconfig which has wrong data
 *
 */
/****************************************************************************/
void EventHandlerThreadController::HandleImproperEvents(quint32 eventId, bool success, quint8 column)
{
    if (!success) {
        m_improperEvents.insert(eventId, column);
    }
}

/****************************************************************************/
/**
 * \brief Function to Verify EventConfigCSV file.
 *
 * \iparam  EventId
 *
 * \return  status of file
 */
/****************************************************************************/

bool EventHandlerThreadController::VerifyEventIDs(quint32 EventId)
{
    /// < Only check on EventID as of now
    if(EventId <= 0)
    {
        return false;
    }
    return true;
}
/****************************************************************************/
/**
 * \brief Function to Verify EventMacro Name.
 *
 * \iparam  EventMacroName
 *
 * \return  true or false
 */
/****************************************************************************/
bool EventHandlerThreadController::VerifyEventMacro(QString EventMacroName) {

    /// < Only check on EventMacroName as of now
    if(EventMacroName.isEmpty())
    {
        //THROWARG(EVENT_CONFIG_FILE, Global::FmtArgs() << EventMacroName);
        return false;

    }
    return true;
}
/****************************************************************************/
/**
 * \brief Function to Verify EventType
 *
 * \iparam  EventType
 *
 * \return  true or false
 */
/****************************************************************************/
bool EventHandlerThreadController::VerifyEventType(Global::EventType EventType) {

    /// < Should be one of -  “ERROR”, ”Info”, “WARNING” else raise an error

    switch (EventType)
    {
    case Global::EVTTYPE_ERROR:
    case Global::EVTTYPE_WARNING:
    case Global::EVTTYPE_INFO:
        return true;
    default:
        //THROW(EVENT_CONFIG_FILE);
        return false;
    }

}
/****************************************************************************/
/**
 * \brief Function to Verify Action Type.
 *
 * \iparam  ActionType
 *
 * \return  true or false
 */
/****************************************************************************/
bool EventHandlerThreadController::VerifyAction(Global::ActionType ActionType)
{
    if (ActionType == Global::ACNTYPE_UNEXPECTED) {
        return false;
    }
    return true;
}
/****************************************************************************/
/**
 * \brief Function to Verify Source
 *
 * \iparam  EventSourceType
 *
 * \return  true or false
 */
/****************************************************************************/
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
/****************************************************************************/
/**
 * \brief Function to Verify LogLevel
 *
 * \iparam EventLogLevel
 *
 * \return  status of LogLevel
 */
/****************************************************************************/
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
/****************************************************************************/
/**
 * \brief Function to Verify Boolean
 *
 * \iparam  UserText
 *
 * \return  status
 */
/****************************************************************************/
bool EventHandlerThreadController::VerifyBoolean(QString UserText) {

    return((UserText == "TRUE") || (UserText == "YES") || (UserText == "FALSE") || (UserText == "NO") || (UserText.isEmpty() ));

}
/****************************************************************************/
/**
 * \brief Function to Verify GuiButton Type.
 *
 * \iparam  GuiButtonType
 *
 * \return  true or false
 */
/****************************************************************************/
bool EventHandlerThreadController::VerifyGuiButtonType(Global::GuiButtonType GuiButtonType ) {

    switch(GuiButtonType) {
    case Global::OK:
    case Global::OK_CANCEL:
    case Global::YES_NO:
    case Global::CONTINUE_STOP:
    case Global::NOT_SPECIFIED:
    case Global::WITHOUT_BUTTONS:
    case Global::INVISIBLE:
        return true;
    default:
        return false;
    }

}
/****************************************************************************/
/**
 * \brief Function to Verify StringList
 *
 * \iparam  EventString
 *
 * \return  true or false
 */
/****************************************************************************/
bool EventHandlerThreadController::VerifyStringList(QString EventString) {

    if(EventString.isEmpty())
        return false;
    return true;
}

} // end namespace EventHandler
