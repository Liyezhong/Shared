/****************************************************************************/
/*! \file DataLogging/Source/DayEventEntry.cpp
 *
 *  \brief Implementation file for class DayEventEntry.
 *
 *\b Description:
 *      This content consists of a time stamp a string ID and an argument list. It
 *      has to be translatable.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2013-10-16
 *  $Author:    $ Raju
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

#include <DataLogging/Include/DayEventEntry.h>
#include <Global/Include/Utils.h>

namespace DataLogging {

/****************************************************************************/
DayEventEntry::DayEventEntry()
    : m_EventStatus(false),
      m_Ref(0),
      m_EventKey(0),
      m_TimeStamp(Global::AdjustedTime::Instance().GetCurrentDateTime()),
      m_Count(0),
      m_AltEventStringUsage(Global::NOT_APPLICABLE) {
    m_AckType = NetCommands::NOT_SPECIFIED;
    m_String.clear();
}

/****************************************************************************/
DayEventEntry::DayEventEntry(const DayEventEntry &rOther) {
    CopyFrom(rOther);
}

/****************************************************************************/
DayEventEntry::DayEventEntry(const QDateTime &TimeStamp, const Global::tTranslatableStringList &String)
    : m_EventStatus(false),
      m_Ref(0),
      m_EventKey(0),
      m_TimeStamp(TimeStamp),
      m_Count(0),
      m_String(String),
      m_AltEventStringUsage(Global::NOT_APPLICABLE) {
    m_AckType = NetCommands::NOT_SPECIFIED;
}

/****************************************************************************/
DayEventEntry::DayEventEntry(const QDateTime &TimeStamp, quint32 EventKey, bool &EventStatus,
                             const Global::tTranslatableStringList &String, quint8 Count,
                             NetCommands::ClickedButton_t ClickButton, Global::tRefType Ref):
    m_EventStatus(EventStatus),
    m_Ref(Ref),
    m_AckType(ClickButton),
    m_EventKey(EventKey),
    m_TimeStamp(TimeStamp),
    m_Count(Count),
    m_AltEventStringUsage(Global::NOT_APPLICABLE),
    m_String(String){
    m_AckType = ClickButton;

}


/****************************************************************************/
DayEventEntry::~DayEventEntry() {
}

/****************************************************************************/
const DayEventEntry & DayEventEntry::operator = (const DayEventEntry &rOther) {
    if(this != &rOther) {
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
void DayEventEntry::CopyFrom(const DayEventEntry &rOther) {

    m_TimeStamp = rOther.m_TimeStamp;
    m_String = rOther.m_String;
    m_EventStatus = rOther.m_EventStatus;
    m_EventKey = rOther.m_EventKey;
    m_Ref = rOther.m_Ref;
    m_Count = rOther.m_Count;
    m_AckType = rOther.m_AckType;
    m_AltEventStringUsage = rOther.m_AltEventStringUsage;
    m_StringID = rOther.m_StringID;
    m_Scenario = rOther.m_Scenario;
    m_AckReqStatus = rOther.m_AckReqStatus;
    m_ActionString = rOther.m_ActionString;
    m_AlarmStatus = rOther.m_AlarmStatus;
    m_AlarmType = rOther.m_AlarmType;
    m_EventCode = rOther.m_EventCode;
    m_EventSoure = rOther.m_EventSoure;
    m_EventType = rOther.m_EventType;
    m_EventName = rOther.m_EventName;
    m_LogLevel = rOther.m_LogLevel;
    m_StatusIcon = rOther.m_StatusIcon;
    m_ShowRunLogStatus = rOther.m_ShowRunLogStatus;
    m_ButtonType = rOther.m_ButtonType;

}

/****************************************************************************/
void DayEventEntry::DumpToConsole() const {
    QString LoggingString = m_TimeStamp.toString("yyyy-MM-dd hh:mm:ss.zzz") + ";" +
            QString::number(AsInt(m_EventType), 10) + ";" +
            QString::number(m_EventCode, 10);
    if(m_String.size() > 0) {
        LoggingString += ":";
    }
    // append additional data. Don't fiddle around with recursive AdditionalData strings and string ids.
    for(Global::tTranslatableStringList::const_iterator it = m_String.constBegin(); it != m_String.constEnd(); ++it) {
        Global::TranslatableString String = *it;
        if(String.IsString()) {
            // its of type string. use directly.
            LoggingString += String.GetString();
        } else {
            // take string id and convert to number.
            LoggingString += QString::number(String.GetStringID(), 10);
        }
        LoggingString += " ";
    }
    // output to std
    Global::ToConsole(LoggingString.trimmed());
}

void  DayEventEntry::SetEventCSVInfo(const EventHandler::EventCSVInfo& EventCsv){
    m_EventCode = EventCsv.GetEventId();
    m_EventName = EventCsv.GetEventName();
    m_EventType = EventCsv.GetEventType();
    m_ActionPositive = EventCsv.GetActionPositive();
    m_ActionNegative = EventCsv.GetActionNegative();
    m_EventSoure = EventCsv.GetEventSource();
    m_AlarmStatus = EventCsv.GetAlarmStatus();
    m_LogLevel = EventCsv.GetLogLevel();
    m_ShowRunLogStatus = EventCsv.GetRunLogStatus();
    m_ButtonType = EventCsv.GetButtonType();
    m_StatusIcon = EventCsv.GetStatusBarIcon();
    m_RetryAttempts = EventCsv.GetRetryAttempts();
    m_AckReqStatus = EventCsv.GetAckReqStatus();
}
} // end namespace DataLogging
