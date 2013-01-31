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
DayEventEntry::DayEventEntry()
    : m_EventKey(0)
{
}

/****************************************************************************/
DayEventEntry::DayEventEntry(const DayEventEntry &rOther) {
    CopyFrom(rOther);
}

/****************************************************************************/
DayEventEntry::DayEventEntry(const QDateTime &TimeStamp, const Global::tTranslatableStringList &String)
    : m_String(String)
    , m_EventKey(0)
    , m_TimeStamp(TimeStamp)
{
}

DayEventEntry::DayEventEntry(const QDateTime &TimeStamp,quint32 EventKey,bool &EventStatus,
                             const Global::tTranslatableStringList &String, quint8 count,
                             NetCommands::ClickedButton_t ClickButton, /*Global::AckOKNOK AckValue,*/ Global::tRefType Ref, EventHandler::EventCSVInfo CSVInfo):
    m_EventCSVInfo(CSVInfo),
    m_String(String),
    m_EventStatus(EventStatus),
    m_Ref(Ref),
    m_AckType(ClickButton),
    m_EventKey(EventKey),
    m_TimeStamp(TimeStamp),
    m_count(count)
{
    //m_AckValue = AckValue.GetStatus();

}

void DayEventEntry::SetEventCSVInfo(EventHandler::EventCSVInfo CSVInfo) {

    m_EventCSVInfo = CSVInfo;
//    m_EventCSVInfo.SetEventCode(CSVInfo.GetEventCode());
//    m_EventCSVInfo.SetEventType(CSVInfo.GetEventType());
//    m_EventCSVInfo.SetAction(CSVInfo.GetActionType());
//    m_EventCSVInfo.SetRetries(CSVInfo.GetRetryAttempts());
//    m_EventCSVInfo.SetActionNegative(CSVInfo.GetActionNegative());
//    m_EventCSVInfo.SetActionPositive(CSVInfo.GetActionPositive());
//    bool ShowInRunLog = CSVInfo.GetRunLogStatus();
//    m_EventCSVInfo.SetRunLogStatus(ShowInRunLog);
//    m_EventCSVInfo.SetLogLevel(CSVInfo.GetLogLevel());
//    m_EventCSVInfo.SetMessageType(CSVInfo.GetMessageType());
//    m_EventCSVInfo.SetAckRequired(CSVInfo.GetAckReqStatus());
//    m_EventCSVInfo. SetAlarmStatus(CSVInfo.GetAlarmStatus());
//    m_EventCSVInfo.SetButtonType(CSVInfo.GetButtonType());
//    m_EventCSVInfo.SetStatusIcon(CSVInfo.GetStatusIcon());
//    m_EventCSVInfo.SetEventSource(CSVInfo.GetEventSource());
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
    m_count = rOther.m_count;
    m_AckType = rOther.m_AckType;
    m_EventCSVInfo = rOther.m_EventCSVInfo;
    m_AltEventStringUsage = rOther.m_AltEventStringUsage;

//    m_AckValue = rOther.m_AckValue;
}

/****************************************************************************/
void DayEventEntry::DumpToConsole() const {
    QString LoggingString = m_TimeStamp.toString("yyyy-MM-dd hh:mm:ss.zzz") + ";"
            /* + QString::number(m_EventCSVInfo.GetSource(), 10) +*/ ";" +
            /*QString::number(m_EventCSVInfo.GetSubComponent(), 10)*/ + ";" +
            QString::number(AsInt(m_EventCSVInfo.GetEventType()), 10) + ";" +
            //QString::number(AsInt(m_EventStatus), 10) + ";" +
            QString::number(m_EventCSVInfo.GetEventCode(), 10);
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
