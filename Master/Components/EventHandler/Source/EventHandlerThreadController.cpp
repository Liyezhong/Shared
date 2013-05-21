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
    Threads::ThreadController(TheHeartBeatSource, "EventHandler"), m_UserRole(Global::OPERATOR)
{
    //    CreateAndInitializeObjects();
    // Register LoggingSource Templates with moc
    qRegisterMetaType<Global::LoggingSource>("Global::LoggingSource");
    qRegisterMetaType<Global::AlternateEventStringUsage>("Global::AlternateEventStringUsage");
    AddActionTypes();
    AddEventTypes();
    AddEventLogLevels();
    AddSourceComponents();
    m_ProcessingEvents.clear();
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
    AddAlarmPosTypes();
    AddLogAuthorityTypes();
    AddResponseRecoveryTypes();
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
    m_EventTypeEnumMap.insert("L_ERROR", Global::EVTTYPE_SYS_ERROR);
    m_EventTypeEnumMap.insert("L_WARNING", Global::EVTTYPE_SYS_WARNING);
    m_EventTypeEnumMap.insert("L_HINT", Global::EVTTYPE_SYS_HINT);
    m_EventTypeEnumMap.insert("L_INFO", Global::EVTTYPE_SYS_HINT);
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
    m_EventSourceMap.insert("SCHEDULERMAIN", Global::EVENTSOURCE_SCHEDULER_MAIN);
    m_EventSourceMap.insert("EVENTHANDLER", Global::EVENTSOURCE_EVENTHANDLER);
    m_EventSourceMap.insert("EXPORT", Global::EVENTSOURCE_EXPORT);
    m_EventSourceMap.insert("IMPORTEXPORT", Global::EVENTSOURCE_IMPORTEXPORT);
    m_EventSourceMap.insert("BLG", Global::EVENTSOURCE_BLG);
    m_EventSourceMap.insert("",Global::EVENTSOURCE_NONE);
}

void EventHandlerThreadController::AddLogAuthorityTypes() {
    m_LogAuthorityTypeEnumMap.insert("TRUE",Global::LOGAUTHTYPE_USER);
    m_LogAuthorityTypeEnumMap.insert("AUTH_USER",Global::LOGAUTHTYPE_USER);
    m_LogAuthorityTypeEnumMap.insert("AUTH_ADMIN",Global::LOGAUTHTYPE_ADMIN);
    m_LogAuthorityTypeEnumMap.insert("AUTH_SERVICE",Global::LOGAUTHTYPE_SERVICE);
}

void EventHandlerThreadController::AddAlarmPosTypes() {
    m_AlarmPosTypeEnumMap.insert("YES", Global::ALARMPOS_DEVICE);
    m_AlarmPosTypeEnumMap.insert("AL_DEVICE", Global::ALARMPOS_DEVICE);
    m_AlarmPosTypeEnumMap.insert("AL_LOCAL", Global::ALARMPOS_LOCAL);
    m_AlarmPosTypeEnumMap.insert("AL_REMOTE", Global::ALARMPOS_REMOTE);
}


void EventHandlerThreadController::AddResponseRecoveryTypes() {
    m_ResponseRecoveryTypeEnumMap.insert("RR_ONLY_REC", Global::RESRECTYPE_ONLY_RECOVERY);
    m_ResponseRecoveryTypeEnumMap.insert("RR_RES_REC", Global::RESRECTYPE_RESPONSE_RECOVERY);
    m_ResponseRecoveryTypeEnumMap.insert("RR_RES_RESULT_REC", Global::RESRECTYPE_RES_RESULT_RECOVERY);
    m_ResponseRecoveryTypeEnumMap.insert("RR_ONLY_RES", Global::RESRECTYPE_ONLY_RESPONSE);
    m_ResponseRecoveryTypeEnumMap.insert("", Global::RESRECTYPE_NONE);
}

void EventHandlerThreadController::HandleInactiveEvent(DataLogging::DayEventEntry &EventEntry, quint64 &EventId64)
{
//    quint32 EventID = EventEntry.GetEventCode();
//    int EventOccurenceCount = m_EventIDCount.count(EventID);
//    if (EventOccurenceCount == 0) {
//        //This is a dummy event
//        //InformAlarmHandler(EventEntry, EventId64, false);
//        //We dont update data structures, because there is nothing to be removed.
//        return;
//    }
//    if (EventOccurenceCount == 1) {
//        //Previously this event had occured once and EventStatus was true for the event.
//        //We retrieve EventKey of that event.
//        quint32 PreviousEventKey = m_EventIDKeyHash.take(EventID);
//        qDebug()<<"Previous Event Key" << PreviousEventKey;
//        EventId64 = EventID;
//        EventId64 <<= 32;
//        EventId64 |= PreviousEventKey;
//        qDebug()<<"EventID64" << EventId64;
//    }
//    InformAlarmHandler(EventEntry, EventId64, false);
//    UpdateEventDataStructures(EventID, EventId64, EventEntry, true, (EventOccurenceCount== 1) ? true : false);
}

void EventHandlerThreadController::CreateEventEntry(DataLogging::DayEventEntry &EventEntry,
                                                    EventCSVInfo &EventInfo,
                                                    const bool EventStatus,
                                                    const quint32 EventID,
                                                    const Global::tTranslatableStringList &EventStringList,
                                                    Global::tTranslatableStringList &EventStringListForRd,
                                                    quint64 EventCodeScenario,
                                                    const quint32 EventKey, const Global::AlternateEventStringUsage AltStringUsage)
{
    if (EventInfo.GetEventCode() == 0 || !m_eventList.contains(EventInfo.GetEventCode()))
    {
        qDebug()<<"Unknown Event ID \n\n\n";
        EventInfo = m_eventList.value(EVENT_EVENT_ID_MISSING);
        EventEntry.SetTranslatableStringList(Global::FmtArgs() << EventID);
    }
    else
    {
        EventEntry.SetTranslatableStringList(EventStringList);
    }

    EventEntry.SetStringForRd(EventStringListForRd);
    EventEntry.SetEventCodeFromCom(EventCodeScenario >> 32);
    EventEntry.SetScenario(EventCodeScenario &0xffffffff);

    EventEntry.SetEventKey(EventKey);
    EventEntry.SetEventCSVInfo(EventInfo);
    EventEntry.SetDateTime(Global::AdjustedTime::Instance().GetCurrentDateTime());
    EventEntry.SetEventStatus(EventStatus);
    EventEntry.SetAltStringUsage(AltStringUsage);
}

void EventHandlerThreadController::InformAlarmHandler(const DataLogging::DayEventEntry &EventEntry, bool StartAlarm)
{
    Global::AlarmType AlarmType = Global::ALARM_NONE;

    if((EventEntry.GetEventType() == Global::EVTTYPE_ERROR ||
        EventEntry.GetEventType() == Global::EVTTYPE_WARNING ||
        EventEntry.GetEventType() == Global::EVTTYPE_SYS_ERROR ||
        EventEntry.GetEventType() == Global::EVTTYPE_SYS_WARNING)) {
        if (EventEntry.GetAlarmPosType() != Global::ALARMPOS_NONE)
        {
            //We need Alarm
            if ((EventEntry.GetEventType() == Global::EVTTYPE_ERROR) ||
                    (EventEntry.GetEventType() == Global::EVTTYPE_SYS_ERROR)) {
                AlarmType = Global::ALARM_ERROR;
            }
            else if ( (EventEntry.GetEventType() == Global::EVTTYPE_WARNING) ||
                      (EventEntry.GetEventType() == Global::EVTTYPE_SYS_WARNING)) {
                AlarmType = Global::ALARM_WARNING;
            }
            mpAlarmHandler->emitSetTimeout(1000);// to be set after final discussion with team / Michael
            mpAlarmHandler->setAlarm(EventEntry.GetEventCode(), AlarmType, EventEntry.GetAlarmPosType(), StartAlarm);
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

    // Recovery Action
    m_ActionTypeEnumMap.insert("RC_INIT_ROTARYVALVE", Global::ACNTYPE_RC_INIT_ROTARYVALVE);
    m_ActionTypeEnumMap.insert("RC_BOTTLECHECK_I", Global::ACNTYPE_RC_BOTTLECHECK_I);
    m_ActionTypeEnumMap.insert("RC_RESTART", Global::ACNTYPE_RC_RESTART);
    m_ActionTypeEnumMap.insert("RC_MAINTENANCE_AIRSTEM", Global::ACNTYPE_RC_MAINTENANCE_AIRSYSTEM);
    m_ActionTypeEnumMap.insert("RC_CHECKREAGENT_RV", Global::ACNTYPE_RC_CHECKREAGENT_RV);
    m_ActionTypeEnumMap.insert("RC_LEVELSENSOR_HEATING_OVERTIME", Global::ACNTYPE_RC_LEVELSENSOR_HEATING_OVERTIME);
    m_ActionTypeEnumMap.insert("RC_REPORT", Global::ACNTYPE_RC_REPORT);

    // Response Action
    m_ActionTypeEnumMap.insert("RS_RESET",Global::ACNTYPE_RS_RESET);
    m_ActionTypeEnumMap.insert("RS_STOPLATER",Global::ACNTYPE_RS_STOPLATER);
    m_ActionTypeEnumMap.insert("RS_STOPATONCE",Global::ACNTYPE_RS_STOPATONCE);
    m_ActionTypeEnumMap.insert("RS_DRAINATONCE",Global::ACNTYPE_RS_DRAINATONCE);
    m_ActionTypeEnumMap.insert("RS_CHECK_BLOCKAGE",Global::ACNTYPE_RS_CHECK_BLOCKAGE);
    m_ActionTypeEnumMap.insert("RS_AIRSYSTEM_FLUSH",Global::ACNTYPE_RS_AIRSYSTEM_FLUSH);
    m_ActionTypeEnumMap.insert("RS_RELEASING",Global::ACNTYPE_RS_RELEASING);
    m_ActionTypeEnumMap.insert("RS_RV_GETORIGINALPOSITIONAGAIN",Global::ACNTYPE_RS_RV_GETORIGINALPOSITIONAGAIN);
    m_ActionTypeEnumMap.insert("RS_STANDBY",Global::ACNTYPE_RS_STANDBY);
    m_ActionTypeEnumMap.insert("RS_RV_MOVETOPOSITION_P3_5",Global::ACNTYPE_RS_RV_MOVETOPOSITION_P3_5);
    m_ActionTypeEnumMap.insert("RS_HEATINGERR_3S_RETRY",Global::ACNTYPE_RS_HEATINGERR_3S_RETRY);
    m_ActionTypeEnumMap.insert("RS_FILLINGAFTERFLUSH",Global::ACNTYPE_RS_FILLINGAFTERFLUSH);
    m_ActionTypeEnumMap.insert("RS_REAGENTCHECK",Global::ACNTYPE_RS_REAGENTCHECK);

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
    enum
    {
        IN_EVENT_ID = 0,
        IN_MACRO = 1,
        IN_EVENT_TYPE = 2,
        IN_RSP_ACTION = 3,
        IN_REC_ACTION = 4,
        IN_BTN_TYPE = 5,
        IN_SOURCE = 6,
        IN_ALARM_TYPE = 7,
        IN_LOG = 8,
        IN_AUTH_USER = 9,
        IN_STATUS_BAR = 10,
        IN_ALT_STR = 11,
        IN_STR = 12,
        IN_RD_STR = 13
    };
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

            if(textList.at(0).isEmpty())
            continue;

            EventHandler::EventCSVInfo EventCSVInfo;

            //Get EventCode
            if (textList.count() > IN_EVENT_ID)
            {
                int EventCode = textList.at(IN_EVENT_ID).toInt();

                ret = VerifyEventIDs(EventCode) ;
                EventCSVInfo.SetEventCode(EventCode); // only int check, no other validations done
            }


            //Get EventMacro
            //! \ Get EventMacro
            QString EventMacroName = "";
            if (textList.count() > IN_MACRO)
            {
                EventMacroName = textList.at(IN_MACRO).trimmed(); // No validations required
                ret = VerifyEventMacro(EventMacroName);
                EventCSVInfo.SetEventName(EventMacroName);
            }

            //Get EventType
            //! \ Get EventType
            if (textList.count() > IN_EVENT_TYPE)
            {

                Global::EventType EventType = m_EventTypeEnumMap.value(textList.at(IN_EVENT_TYPE).
                                                                       trimmed().toUpper(),
                                                                       Global::EVTTYPE_UNDEFINED);
                ret = VerifyEventType(EventType);
                EventCSVInfo.SetEventType(EventType);
            }


            //Get Response ActionType
            if (textList.count() > IN_RSP_ACTION)
            {
                QString RspAction = textList.at(IN_RSP_ACTION);
                EventCSVInfo.SetResponseAction(m_ActionTypeEnumMap.value(RspAction.trimmed().toUpper(), Global::ACNTYPE_NONE));

            }

            //Get Recovery ActionType
            // |----Rsp Fail--------------|-----Rsp Succ-------------|
            // PstBtn1Action+NegBtn2Action&PstBtn1Action+NegBtn2Action
            if (textList.count() > IN_REC_ACTION)
            {
                QStringList ActionInfo = textList.at(IN_REC_ACTION).split("&");
                QString ActionStr[4] = {"","","",""};

                if(ActionInfo.count() >= 1)//rsp fail action
                {
                    QStringList RspFailActions = ActionInfo.at(0).split("+");
                    if(RspFailActions.count() >= 1)
                    {
                        ActionStr[0] = RspFailActions.at(0); //rsp fail, postive btn
                    }
                    if(RspFailActions.count() >= 2)
                    {
                        ActionStr[1] = RspFailActions.at(1); //rsp fail, negative btn
                    }
                }

                if(ActionInfo.count() >= 2)//rsp succ action
                {
                    QStringList RspSuccActions = ActionInfo.at(1).split("+");
                    if(RspSuccActions.count() >= 1)
                    {
                        ActionStr[2] = RspSuccActions.at(0); //rsp succ, postive btn
                    }
                    if(RspSuccActions.count() >= 2)
                    {
                        ActionStr[3] = RspSuccActions.at(1); //rsp succ, negative btn
                    }
                }
                EventCSVInfo.SetRecoveryActionOnRspFailUsrPst(m_ActionTypeEnumMap.value(ActionStr[0].trimmed().toUpper(), Global::ACNTYPE_NONE));
                EventCSVInfo.SetRecoveryActionOnRspFailUsrNeg(m_ActionTypeEnumMap.value(ActionStr[1].trimmed().toUpper(), Global::ACNTYPE_NONE));
                EventCSVInfo.SetRecoveryActionOnRspSuccUsrPst(m_ActionTypeEnumMap.value(ActionStr[2].trimmed().toUpper(), Global::ACNTYPE_NONE));
                EventCSVInfo.SetRecoveryActionOnRspSuccUsrNeg(m_ActionTypeEnumMap.value(ActionStr[3].trimmed().toUpper(), Global::ACNTYPE_NONE));
            }


            //Get button type
            if (textList.count() > IN_BTN_TYPE)
            {

                QStringList Btns = textList.at(IN_BTN_TYPE).split("&");
                Global::GuiButtonType BtnType;
                if(Btns.count() >= 1) // response fail button type
                {
                    BtnType = Global::StringToGuiButtonType(Btns.at(0).toUpper());
                    ret = VerifyGuiButtonType(BtnType);
                    if(ret)
                    {
                        EventCSVInfo.SetButtonTypeOnRspFail(BtnType);
                    }
                }
                if(Btns.count() >= 2) // response succ button type
                {
                    BtnType = Global::StringToGuiButtonType(Btns.at(1).toUpper());
                    ret = VerifyGuiButtonType(BtnType);
                    if(ret)
                    {
                        EventCSVInfo.SetButtonTypeOnRspSucc(BtnType);
                    }
                }
            }

            //! \ Get Source
            if (textList.count() > IN_SOURCE)
            {
                QString Source = textList.at(IN_SOURCE).trimmed();

                Global::EventSourceType EventSourceType = Global::EVENTSOURCE_NOTEXIST;

                EventSourceType = m_EventSourceMap.value(Source.toUpper(),Global::EVENTSOURCE_NOTEXIST);

                ret = VerifySource(EventSourceType);

                EventCSVInfo.SetSourceComponent(EventSourceType);

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


            //! \ Get alarmPosType flag

            if (textList.count() > IN_ALARM_TYPE)
            {
                EventCSVInfo.SetAlarmPosType(m_AlarmPosTypeEnumMap.value(textList.at(IN_ALARM_TYPE).trimmed().toUpper(),Global::ALARMPOS_NONE));
            }


            //! \ Get LogLevel flag

            if (textList.count() > IN_LOG)
            {

                Global::EventLogLevel EventLogLevel = Global::LOGLEVEL_UNEXPECTED;
                EventLogLevel = m_EventLogLevelEnumMap.value(textList.at(IN_LOG).trimmed().toUpper());
                ret = VerifyLogLevel(EventLogLevel);
                EventCSVInfo.SetLogLevel(EventLogLevel);
            }

            //! \ Get UserLog flag

            if (textList.count() > IN_AUTH_USER)
            {
                EventCSVInfo.SetLogAuthorityType(m_LogAuthorityTypeEnumMap.value(textList.at(IN_AUTH_USER).trimmed().toUpper(), Global::LOGAUTHTYPE_NO_SHOW));
            }


               //! \ Get StatusBarIcon flag
               if (textList.count() > IN_STATUS_BAR)
               {

                   bool StatusIconFlag = false;
                   QString strStatusIcon = textList.at(IN_STATUS_BAR).toUpper();
                   ret = VerifyBoolean(strStatusIcon);

                   StatusIconFlag = ((textList.at(IN_STATUS_BAR).toUpper() == "TRUE") || (textList.at(IN_STATUS_BAR).toUpper() == "YES"));

                   EventCSVInfo.SetStatusIcon(StatusIconFlag);
               }


               //! \ Get Alternate String

               //! \ Get EventString

                  if (textList.count() > IN_STR)
                  {
                       // ! \ EventString comes from the RaiseEvent call, just verify the data is not empty
                       QString EventString = textList.at(IN_STR);
                       ret = VerifyStringList(EventString);
                  }

               //! \ Get String For ID

                  if (textList.count() > IN_RD_STR)
                  {
                       // ! \ EventString comes from the RaiseEvent call, just verify the data is not empty
                       QString EventString = textList.at(IN_RD_STR);
                       EventCSVInfo.SetDetailForRD(EventString);
                  }

            m_eventList.insert(EventCSVInfo.GetEventCode(), EventCSVInfo);
        }// end of if
    }// end of while

 //! \ validate dependencies between the values corresponding to an event.
            //TODO
    file.close();

    return true;
}

/****************************************************************************/
/**
 * \brief check and get event current status
 *
 * \param[in]   EventSource   the source of the event
 * \param[in]   EventCode   the EventCode of the event
 * \param[out]   EventStatus   current status of the event
 * \return      true - There ara Action should be done,  false - no actions
 */
/****************************************************************************/
bool EventHandlerThreadController::HandlingError(const quint32 ErorCode, const quint32 EventKey,
                                                    const DataLogging::DayEventEntry& EventEntry)
{
    ProcessingEvent ProEvent;
    Global::EventSourceType EventSource = EventEntry.GetSourceComponent();

    if(EventSource == Global::EVENTSOURCE_NONE ||
            (EventEntry.GetResponseAction() == Global::ACNTYPE_NONE &&
                        EventEntry.GetRecoveryActionOnRspFailUsrPst() == Global::ACNTYPE_NONE)) // no actions
    {
        return false;
    }

    if(!m_ProcessingEvents.contains(EventSource)) // First comming
    {
        ProEvent.EventCode = ErorCode;
        ProEvent.EventKey = EventKey;
        ProEvent.CurrentStatus = Global::EVTSTAT_OFF;
        if(EventEntry.GetResponseAction() != Global::ACNTYPE_NONE) // response firstly
        {
            ProEvent.CurrentStatus = Global::EVTSTAT_RESPONSE;
            DoResponseAction(ErorCode,EventKey,EventEntry);
            ProEvent.CurrentActionResult = EventEntry.IsEventActive();
            m_ProcessingEvents[EventSource].append(ProEvent);
        }
        else if(EventEntry.GetRecoveryActionOnRspFailUsrPst() != Global::ACNTYPE_NONE)// Recovery Directly
        {
            ProEvent.CurrentStatus = Global::EVTSTAT_RECOVERY;
            ProEvent.CurrentActionResult = EventEntry.IsEventActive();
            m_ProcessingEvents[EventSource].append(ProEvent);
        }

    }
    else
    {
        if(m_ProcessingEvents[EventSource].count() == 1  &&
                m_ProcessingEvents[EventSource][0].EventCode == ErorCode &&
                m_ProcessingEvents[EventSource][0].EventKey == EventKey)
        {
            ProEvent =  m_ProcessingEvents[EventSource][0];
            if(ProEvent.CurrentStatus == Global::EVTSTAT_RESPONSE_ACK)// ACK of response
            {
                m_ProcessingEvents[EventSource][0].CurrentActionResult = EventEntry.IsEventActive();
            }
            else if(ProEvent.CurrentStatus == Global::EVTSTAT_RECOVERY_ACK) // ACK of recovery
            {
                m_ProcessingEvents[EventSource][0].CurrentActionResult = EventEntry.IsEventActive();
            }
        }
        else // error in error
        {
            m_ProcessingEvents[EventSource][0].CurrentStatus = Global::EVTSTAT_ERROR_IN_ACTION;
        }
    }

    return true;
}

/****************************************************************************/
/**
 * \brief Do Response Action
 *
 */
/****************************************************************************/
void EventHandlerThreadController::DoResponseAction(const quint32 EventCode, const quint32 EventKey,const DataLogging::DayEventEntry& EventEntry)
{
    NetCommands::CmdSystemAction *p_CmdSystemAction;
    p_CmdSystemAction = new NetCommands::CmdSystemAction();
    p_CmdSystemAction->SetRetryCount(0);
    p_CmdSystemAction->SetEventKey(EventKey);
    p_CmdSystemAction->SetEventID(EventEntry.GetEventCodeFromCom());
    p_CmdSystemAction->SetScenario(EventEntry.GetScenario());
    p_CmdSystemAction->SetSource(EventEntry.GetSourceComponent());
//    p_CmdSystemAction->SetStringList(EventStringList);
    Global::tRefType NewRef = this->GetNewCommandRef();

    p_CmdSystemAction->SetAction(EventEntry.GetResponseAction());
    this->SendCommand(NewRef, Global::CommandShPtr_t(p_CmdSystemAction));//response
    return;
}

bool EventHandlerThreadController::VerifyUserLogGUIOptionDependency( EventHandler::EventCSVInfo EventCSVInfo ) {

//    if((EventCSVInfo.GetLogAuthorityType() != Global::LOGAUTHTYPE_NO_SHOW) && (EventCSVInfo.GetGUIMessageBoxOptions() == Global::NO_BUTTON)) {

//        qDebug()<< "Invalid UserLog - GUIOption combination set" << EventCSVInfo.GetEventCode()<<"\t"
//                <<EventCSVInfo.GetLogAuthorityType()<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions()<<"\n";
//        return false;
//    }
    return true;
}

bool EventHandlerThreadController::VerifySourceComponentGUIOptionsDependency(EventHandler::EventCSVInfo EventCSVInfo) {

//    if((EventCSVInfo.GetSourceComponent() != Global::EVENTSOURCE_NONE) && (EventCSVInfo.GetGUIMessageBoxOptions() == Global::NO_BUTTON )) {

//        qDebug()<< "Invalid SourceComponent - GUIOption combination set"<< EventCSVInfo.GetEventCode()<<"\t"<<
//                                EventCSVInfo.GetSourceComponent()<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions();
//        return false;
//    }
    return true;
}

bool EventHandlerThreadController::VerifyActionGUIOptionsDependency(EventHandler::EventCSVInfo EventCSVInfo) {
    //ActionNegative the action taken on negative acknowledgement by the user ,which means guiOption
    //is has to be filled properly with valid button type other than GUI:NO_BUTTON


//    Global::ActionType Acn = EventCSVInfo.GetActionNegative();
//    if(VerifyAction(Acn) && (Acn != Global::ACNTYPE_NONE) && (EventCSVInfo.GetButtonType() == Global::NO_BUTTON)) {
//        /// > This condition implies that there is  positive or negative action, however no GUIMessage, which is not expected.

//        qDebug()<< "Invalid ActionNegative - GUIOption combination set" <<EventCSVInfo.GetEventCode()<<"\t"
//                <<Acn<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions()<<"\n";
//        return false;
//    }

//    Acn = EventCSVInfo.GetActionPositive();
//    if(VerifyAction(Acn)&& (Acn != Global::ACNTYPE_NONE) && (EventCSVInfo.GetButtonType() == Global::NO_BUTTON)) {
//        /// > This condition implies that there is  positive or negative action, however no GUIMessage, which is not expected.

//        qDebug()<< "Invalid ActionPositive - GUIOption combination set" <<EventCSVInfo.GetEventCode()<<"\t"
//                <<Acn<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions()<<"\n";
//        return false;
//    }

    return true;
}

bool EventHandlerThreadController::VerifyAlarmGUIOptionsDependency(EventHandler::EventCSVInfo EventCSVInfo) {
     /// < If alarm is set then GUIOptions has to be set with a valid value.

//    if((EventCSVInfo.GetAlarmPosType() != Global::ALARMPOS_NONE) && (EventCSVInfo.GetGUIMessageBoxOptions() == Global::NO_BUTTON))
//    {
//        qDebug()<< "Invalid Alarm - GUIOption combination set"<<EventCSVInfo.GetEventCode()<<"\t"
//                <<EventCSVInfo.GetAlarmPosType()<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions()<<"\n";
//        return false;
//    }
    return true;
}

bool EventHandlerThreadController::VerifyAlarmEventTypeDependency(EventHandler::EventCSVInfo EventCSVInfo) {

    //            1 . alarm should not be set if event type is info.

//        if((EventCSVInfo.GetEventType() == Global::EVTTYPE_INFO) && (EventCSVInfo.GetAlarmPosType() != Global::ALARMPOS_NONE)) {
//             qDebug()<< "Invalid Alarm - EventType combination set"<<EventCSVInfo.GetEventCode()<<"\t"
//                     <<EventCSVInfo.GetEventType()<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions();
//                return false;
//        }

            return true;

}

bool EventHandlerThreadController::VerifyStatusbarGUIOptionDependency( EventHandler::EventCSVInfo EventCSVInfo) {

//       if((EventCSVInfo.GetStatusBarIcon() == true) && EventCSVInfo.GetGUIMessageBoxOptions() == Global::NO_BUTTON)
//       {
//           qDebug()<< "Invalid StatusBar icon GUIOption combination set"<<EventCSVInfo.GetEventCode()<<"\t"
//                   <<EventCSVInfo.GetStatusBarIcon()<<"\t"<<EventCSVInfo.GetGUIMessageBoxOptions()<<"\n";
//           return false;
//       }
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
                                                             bool AckByGUI,
                                                             bool ResponseRes)
{
////    quint32 EventKey = (quint32)EventId64 >> 32;
//    quint32 EventKey = (quint32)(EventId64 & 0x00000000FFFFFFFF);
//    if (Remove) {
//        if (AckByGUI) {
//            //We no longer need these data
//            m_EventKeyDataMap.remove(EventId64);
//            qint32 Index = m_EventIDCount.indexOf(EventID);
//            if (Index != -1) {
//                m_EventIDCount.removeAt(Index);
//            }
//            //quint32 EventKey = static_cast<quint32>(EventId64 >> 32);
//            m_EventIDKeyHash.remove(EventID);
//            //Unblock this EventKey so that it can be reused.
//            Global::EventObject::Instance().UnBlockEventKey(EventKey);

//            m_EventKeyIdMap.remove(EventKey);
//        }
//    }
//    else {
//        //New event, add only if the event has to be sent to GUI
//        Global::GuiButtonType btn = EventEntry.GetButtonTypeOnRspFail();
//        if(ResponseRes)
//        {
//            btn = EventEntry.GetButtonTypeOnRspSucc();
//        }
//        if(btn != Global::NO_BUTTON) {
//            m_EventIDCount.append(EventID);
//            m_EventKeyDataMap.insert(EventId64, EventEntry);
//            if (!m_EventIDKeyHash.contains(EventID)) {
//                m_EventIDKeyHash.insert(EventID, EventKey);
//            }
//            m_EventKeyIdMap.insert(EventKey, EventID);
//        }
//    }
//    qDebug()<<"Event Entry status in UpdateEventDataStructures();"<<EventEntry.IsEventActive();
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
void EventHandlerThreadController::ProcessEvent(const quint32 ErrorCode,
                                                const Global::tTranslatableStringList &EventStringList,
                                                const bool IsResolved,
                                                const quint32 EventKey,
                                                const Global::AlternateEventStringUsage AltStringUsuage,
                                                Global::tTranslatableStringList EventStringListForRD,
                                                quint64 EventCodeScenario)
{
    qDebug() << "\n\n\n\nEventHandlerThreadController::ProcessEvent, EventID=" << ErrorCode << "EventKey=" << EventKey << "Event status" <<IsResolved;

    // if eventList is not available yet, place event into pendingList
    if (ErrorCode == EVENT_GUI_AVAILABLE)
    {
            SetGuiAvailable(true);
            //We dont need to log this particular event ,
            //instead we log for eg.EVENT_PROCESS_HIMALAYA_GUI_CONNECTED
            return;
    }
    else if (m_eventList.size() == 0)
    {
        qDebug()<<"Event list not available \n\n\n";
        PendingEvent pe;
        pe.EventID = ErrorCode;
        pe.EventStringList = EventStringList;
        pe.EventKey = EventKey;
        pe.IsResolved = IsResolved;
        pe.AltStringUsuage = AltStringUsuage;
        pe.EventStringListForRD = EventStringListForRD;
        pe.EventCodeScenario = EventCodeScenario;
        m_pendingEvents.push_back(pe);// These events, if any, are processed after successfully reading the EventConf file
        return;
    }
    else
    {
        QHash<quint32, EventHandler::EventCSVInfo>::iterator i = m_eventList.find(ErrorCode);
        if (i == m_eventList.end())
        {
                qDebug()<< "\n The event item: "<< ErrorCode<< " has not been appended into the config table!";
                return;
        }

        DataLogging::DayEventEntry EventEntry;
        EventCSVInfo EventInfo = m_eventList.value(ErrorCode);
        //EventEntry encapsulates EventInfo. EventEntry is also
        //passed to the logging component.
        CreateEventEntry(EventEntry, EventInfo, IsResolved, ErrorCode, EventStringList, EventStringListForRD, EventCodeScenario, EventKey, AltStringUsuage);

        //Log if loglevel is not "NONE"
        if (EventEntry.GetLogLevel() != Global::LOGLEVEL_NONE)
        {
            qDebug()<< "Sending event to DataLogger";
            emit LogEventEntry(EventEntry); //Log the event
        }

        //
        //Do Response Action
        bool ResponseRes = false; // if no Response Action, we think ResponseRes as false
        bool RecoveryRes = false;
        bool RaiseBox = !IsResolved;
        bool haveActions = HandlingError(ErrorCode,EventKey, EventEntry);
        if(haveActions)
        {
            ProcessingEvent& proEvent = m_ProcessingEvents[EventInfo.GetSourceComponent()][0];
            EventEntry.SetCurrentStatus(proEvent.CurrentStatus);
            switch(proEvent.CurrentStatus)
            {
                case Global::EVTSTAT_RESPONSE:
                    proEvent.CurrentStatus = Global::EVTSTAT_RESPONSE_ACK;
                    return; // waiting Response result
                    break;
                case Global::EVTSTAT_RESPONSE_ACK:
                    ResponseRes = IsResolved;
                    if(ResponseRes &&
                            EventInfo.GetRecoveryActionOnRspSuccUsrNeg() == Global::ACNTYPE_NONE &&
                            EventInfo.GetRecoveryActionOnRspSuccUsrPst() == Global::ACNTYPE_NONE)
                    {
                        m_ProcessingEvents.remove(EventInfo.GetSourceComponent());
                        return;
                    }
                    else if(!ResponseRes &&
                            EventInfo.GetRecoveryActionOnRspFailUsrNeg() == Global::ACNTYPE_NONE &&
                            EventInfo.GetRecoveryActionOnRspFailUsrPst() == Global::ACNTYPE_NONE )
                    {
                        m_ProcessingEvents.remove(EventInfo.GetSourceComponent());
                        return;
                    }
                    else // have recovery action
                    {
                        RaiseBox = true;
                        proEvent.CurrentStatus = Global::EVTSTAT_RECOVERY_ACK;
                    }
                    break;
                case Global::EVTSTAT_RECOVERY:
                    proEvent.CurrentStatus = Global::EVTSTAT_RECOVERY_ACK;
                    break;
                case Global::EVTSTAT_RECOVERY_ACK:
                    RecoveryRes = IsResolved;
                    if(RecoveryRes)
                    {
                        m_ProcessingEvents.remove(EventInfo.GetSourceComponent());
                        RaiseBox = false;
                        return;

                    }
                    else // Recovery fail
                    {
                        RaiseBox = true;
                    }
                    break;
                case Global::EVTSTAT_OFF: // no actions for  the event
                    break;
                case Global::EVTSTAT_ON:
                    break;
                case Global::EVTSTAT_ERROR_IN_ACTION: // ERROR IN ERRO
                    RaiseBox = true;
                    break;
            }
        }
//        SetSystemStateMachine(EventEntry);
        InformUser(EventEntry,RaiseBox);
    }
}

void EventHandlerThreadController::SetSystemStateMachine(const DataLogging::DayEventEntry &TheEvent)
{
//    if ((TheEvent.GetActionPositive() == Global::ACNTYPE_BUSY)
//            || (TheEvent.GetActionPositive() == Global::ACNTYPE_IDLE)) {
//        if(TheEvent.GetString().count() > 0) { // e.g. Rack inserted
//            // TheEvent.GetString()).at(0).GetString().toInt() = RACK RFID
//            qDebug()<<"Setting Statemachine";
//            EventHandler::StateHandler::Instance().setActivityUpdate(true, (TheEvent.GetString()).at(0).GetString().toInt());
//        }
//    }

//    if (TheEvent.GetActionType() == Global::ACNTYPE_STOP) {
//        EventHandler::StateHandler::Instance().setAvailability(TheEvent.IsEventActive(), TheEvent.GetEventCode());
//    }
}

void EventHandlerThreadController::InformUser(const DataLogging::DayEventEntry &TheEvent,bool RaiseBox, bool AckByGui)
{
    Global::GuiUserLevel userLevel = Global::OPERATOR;
    Global::LogAuthorityType logAuth = TheEvent.GetLogAuthorityType();
    if (Global::LOGAUTHTYPE_ADMIN == logAuth)
    {
        userLevel = Global::ADMIN;
    } else if (Global::LOGAUTHTYPE_USER == logAuth)
    {
        userLevel = Global::OPERATOR;
    } else if (Global::LOGAUTHTYPE_SERVICE == logAuth)
    {
        userLevel = Global::SERVICE;
    }

    Global::GuiButtonType btn = TheEvent.GetButtonTypeOnRspFail();
    if(TheEvent.IsEventActive())
    {
        btn = TheEvent.GetButtonTypeOnRspSucc();
    }



    if ((m_UserRole >= userLevel) && ((btn != Global::NO_BUTTON) || (TheEvent.GetStatusIcon())))
    {
        NetCommands::EventReportDataStruct EventReportData;
        EventReportData.EventStatus = RaiseBox; //False means event not active, True if event active.
        EventReportData.EventType = TheEvent.GetEventType();  // Global::EVTTYPE_ERROR;
        EventReportData.ID = TheEvent.GetEventCode();
        EventReportData.EventKey = TheEvent.GetEventKey();

        const Global::AlternateEventStringUsage AltStringUsage = TheEvent.GetAltStringUsageType();
        bool UseAltEventString = false;
        if (AltStringUsage == Global::GUI_MSG_BOX ||
            AltStringUsage == Global::GUI_MSG_BOX_AND_LOGGING ||
            AltStringUsage == Global::GUI_MSG_BOX_LOGGING_AND_USER_RESPONSE) {
                UseAltEventString = true;
        }
        EventReportData.MsgString = Global::EventTranslator::TranslatorInstance().Translate(Global::TranslatableString(TheEvent.GetEventCode(), TheEvent.GetString()),
                                                                                            UseAltEventString); //"Event String translated to the set langauge";
        EventReportData.Time = TheEvent.GetTimeStamp().toString();   // Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
        EventReportData.BtnType = btn;
        EventReportData.StatusBarIcon = TheEvent.GetStatusIcon();   //true if GUI must set status bar icon.


        if(TheEvent.GetCurrentStatus() == Global::EVTSTAT_RESPONSE_ACK ||
                TheEvent.GetCurrentStatus() == Global::EVTSTAT_RECOVERY)//status before Recovery Ack comming
        {
            if(AckByGui)// call from OnAcknowledge for RC_REPORT
            {
                EventReportData.BtnType = Global::NO_BUTTON;
            }
            else
            {
                EventReportData.StatusBarIcon = false;
            }
        }
        else if(TheEvent.GetCurrentStatus() == Global::EVTSTAT_RECOVERY_ACK)
        {
            if(!TheEvent.IsEventActive())// recovery fail
            {
                EventReportData.StatusBarIcon = true;
                EventReportData.MsgString += Global::EventTranslator::TranslatorInstance().Translate(Global::TranslatableString(EVENT_RECOVERY_FAIL_PLEASE_CONTACT_SERVICE,
                                                                                                                                Global::tTranslatableStringList()<<QString().number(TheEvent.GetEventCodeFromCom())
                                                                                                                                                                 <<QString().number(TheEvent.GetScenario())));
                EventReportData.BtnType = Global::NO_BUTTON;
            }
            else
            {
                return;
            }

        }
        else if(TheEvent.GetCurrentStatus() == Global::EVTSTAT_ERROR_IN_ACTION)
        {
            EventReportData.StatusBarIcon = true;
            EventReportData.MsgString += Global::EventTranslator::TranslatorInstance().Translate(Global::TranslatableString(EVENT_RECOVERY_FAIL_PLEASE_CONTACT_SERVICE,
                                                                                                                            Global::tTranslatableStringList()<<QString().number(TheEvent.GetEventCodeFromCom())
                                                                                                                                                             <<QString().number(TheEvent.GetScenario())));

            EventReportData.BtnType = Global::NO_BUTTON;
        }


        if (m_GuiAvailable)
        {
            Global::tRefType Ref = GetNewCommandRef();
            if(RaiseBox)
            {
                m_EventRefDataMap.insert(Ref,TheEvent);
            }
            SendCommand(Ref, Global::CommandShPtr_t(new NetCommands::CmdEventReport(Global::Command::MAXTIMEOUT, EventReportData)));
            if(!AckByGui && TheEvent.GetCurrentStatus() != Global::EVTSTAT_RECOVERY_ACK)
            {
                InformAlarmHandler(TheEvent,RaiseBox);
            }
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
    RegisterCommandForProcessing<
            NetCommands::CmdChangeUserLevel, EventHandlerThreadController
            >(&EventHandlerThreadController::OnUserRoleChanged, this);

}

void EventHandlerThreadController::OnUserRoleChanged(Global::tRefType Ref, const NetCommands::CmdChangeUserLevel& cmd)
{
    if (cmd.GetUserLevel() != m_UserRole)
        m_UserRole = cmd.GetUserLevel();
}

void EventHandlerThreadController::OnAcknowledge(Global::tRefType Ref, const NetCommands::CmdAcknEventReport &Ack)
{

    DataLogging::DayEventEntry EventEntry = m_EventRefDataMap.value(Ref);

    if (EventEntry.GetEventKey() == 0) {
        qDebug() << "EventHandlerThreadController::OnAcknowledge, couldn't find Event" << EventEntry.GetEventCode();
        return;
    }
    EventEntry.SetAckValue(Ack);
    EventEntry.SetDateTime(Global::AdjustedTime::Instance().GetCurrentDateTime());

    if(EventEntry.GetLogLevel() != Global::LOGLEVEL_NONE)
    {
        qDebug()<<"OnAcknowledge() <<EventEntry" <<EventEntry.GetEventCode();
        LogEventEntry(EventEntry);
    }

    Global::ActionType Act = Global::ACNTYPE_NONE;
    if(EventEntry.IsEventActive())
    {
        if((EventEntry.GetAckValue() ==  NetCommands::OK_BUTTON) ||
                 (EventEntry.GetAckValue() ==  NetCommands::YES_BUTTON) ||
                 (EventEntry.GetAckValue() == NetCommands::CONTINUE_BUTTON) ||
                 (EventEntry.GetAckValue() == NetCommands::RECOVERYNOW))
         {
            Act = EventEntry.GetRecoveryActionOnRspSuccUsrPst();
         }
         else
         {
             Act = EventEntry.GetRecoveryActionOnRspSuccUsrNeg();
         }
    }
    else
    {
        if((EventEntry.GetAckValue() ==  NetCommands::OK_BUTTON) ||
                 (EventEntry.GetAckValue() ==  NetCommands::YES_BUTTON) ||
                 (EventEntry.GetAckValue() == NetCommands::CONTINUE_BUTTON) ||
                 (EventEntry.GetAckValue() == NetCommands::RECOVERYNOW))
         {
            Act = EventEntry.GetRecoveryActionOnRspFailUsrPst();
         }
         else
         {
             Act = EventEntry.GetRecoveryActionOnRspFailUsrNeg();
         }
    }

    m_EventRefDataMap.remove(Ref);
    InformAlarmHandler(EventEntry,false);//stop alarm
    if(Act == Global::ACNTYPE_NONE)
    {  
        return;
    }
    else if(Act == Global::ACNTYPE_RC_REPORT)
    {
        InformUser(EventEntry,true,true);
        return;
    }

    NetCommands::CmdSystemAction *p_CmdSystemAction;
    p_CmdSystemAction = new NetCommands::CmdSystemAction();
    p_CmdSystemAction->SetEventKey(EventEntry.GetEventKey());
    p_CmdSystemAction->SetEventID(EventEntry.GetEventCodeFromCom());
    p_CmdSystemAction->SetScenario(EventEntry.GetScenario());
    p_CmdSystemAction->SetSource(EventEntry.GetSourceComponent());
    p_CmdSystemAction->SetRetryCount(EventEntry.GetRetryAttempts());
    p_CmdSystemAction->SetStringList(EventEntry.GetString());
    Global::tRefType NewRef = GetNewCommandRef();

    p_CmdSystemAction->SetAction(Act);

    qDebug() << "EventHandlerThreadController::OnAcknowledge, EventKey=" << EventEntry.GetEventKey() << "EventID=" <<EventEntry.GetEventCode();
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
            ProcessEvent(pe.EventID, pe.EventStringList, pe.IsResolved, pe.EventKey, pe.AltStringUsuage,pe.EventStringListForRD);
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
    case Global::EVTTYPE_SYS_HINT:
    case Global::EVTTYPE_SYS_WARNING:
    case Global::EVTTYPE_SYS_ERROR:
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
    case Global::RECOVERYLATER_RECOVERYNOW:
    case Global::RECOVERYNOW:
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
