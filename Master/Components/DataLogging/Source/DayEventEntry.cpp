/****************************************************************************/
/*! \file DataLogging/Source/DayEventEntry.cpp
 *
 *  \brief Implementation file for class DayEventEntry.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
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

namespace DataLogging {

/****************************************************************************/
DayEventEntry::DayEventEntry() {
}

/****************************************************************************/
DayEventEntry::DayEventEntry(const DayEventEntry &rOther) {
    CopyFrom(rOther);
}

/****************************************************************************/
DayEventEntry::DayEventEntry(const QDateTime &TimeStamp, const Global::tTranslatableStringList &String) : EventCSVInfo(m_EventCode, m_EventType,  m_Action,
         m_NumberOfRetries,  m_ActionPositive, m_ActionNegative,
         m_ShowInRunLog,  m_Source,  m_LogLevel,
         m_MessageType,  m_AckRequired,  m_AlarmRequired,  m_ButtonType,
         m_StatusBarIcon, m_SourceComponent ),
    m_TimeStamp(TimeStamp),
    m_String(String)
{
}

DayEventEntry::DayEventEntry(const QDateTime &TimeStamp,quint32 EventKey,bool &EventStatus,
                             bool Active,const Global::tTranslatableStringList &String, quint8 count,
                             NetCommands::ClickedButton_t ClickButton, /*Global::AckOKNOK AckValue,*/ Global::tRefType Ref):
    m_TimeStamp(TimeStamp),
    m_EventKey(EventKey),
    m_EventStatus(EventStatus),
    m_Active(Active),
    m_String(String),
    m_count(count),
    m_AckType(ClickButton),
    m_Ref(Ref) {
    //m_AckValue = AckValue.GetStatus();

}

void DayEventEntry::SetEventCSVInfo(EventCSVInfo CSVInfo) {

    SetEventCode(CSVInfo.GetEventCode());
    SetEventType(CSVInfo.GetEventType());
    SetAction(CSVInfo.GetActionType());
    SetRetries(CSVInfo.GetRetryAttempts());
    SetActionNegative(CSVInfo.GetActionNegative());
    SetActionPositive(CSVInfo.GetActionPositive());
    bool ShowInRunLog = CSVInfo.GetRunLogStatus();
    SetRunLogStatus(ShowInRunLog);
    SetLogLevel(CSVInfo.GetLogLevel());
    SetMessageType(CSVInfo.GetMessageType());
    SetAckRequired(CSVInfo.GetAckReqStatus());
    SetAlarmStatus(CSVInfo.GetAlarmStatus());
    SetButtonType(CSVInfo.GetButtonType());
    SetStatusIcon(CSVInfo.GetStatusIcon());
    SetEventSource(CSVInfo.GetEventSource());
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
    m_Active = rOther.m_Active;
    m_EventKey = rOther.m_EventKey;
    m_Ref = rOther.m_Ref;
    m_count = rOther.m_count;
    m_AckType = rOther.m_AckType;

//    m_AckValue = rOther.m_AckValue;
}

/****************************************************************************/
void DayEventEntry::DumpToConsole() const {
    QString LoggingString = m_TimeStamp.toString("yyyy-MM-dd hh:mm:ss.zzz") + ";" +
            QString::number(m_Source.GetSource(), 10) + ";" +
            QString::number(m_Source.GetSubComponent(), 10) + ";" +
            QString::number(AsInt(m_EventType), 10) + ";" +
            //QString::number(AsInt(m_EventStatus), 10) + ";" +
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
} // end namespace DataLogging
