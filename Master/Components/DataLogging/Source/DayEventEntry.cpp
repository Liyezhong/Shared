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
DayEventEntry::DayEventEntry(const QDateTime &TimeStamp, const Global::tTranslatableStringList &String) :
    m_TimeStamp(TimeStamp),
    m_String(String)
{
}

/****************************************************************************/
DayEventEntry::DayEventEntry(const QDateTime &TimeStamp,const quint32 &EventCode,
                                     const Global::EventType &EventType, const Global::tTranslatableStringList &String,
                                     const QString &NumericData, const bool &ShowInRunLog,
                                     const Global::LoggingSource &LoggingSource, const Global::LogLevel &LogLevel) :
    m_TimeStamp(TimeStamp),
    m_EventCode(EventCode),
    m_EventType(EventType),
    m_String(String),
    m_NumericData(NumericData),
    m_ShowInRunLog(ShowInRunLog),
    m_Source(LoggingSource),
    m_LogLevel(LogLevel)
{
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
    m_EventCode = rOther.m_EventCode;
    m_EventType = rOther.m_EventType;
    m_String = rOther.m_String;
    m_NumericData = rOther.m_NumericData;
    m_ShowInRunLog = rOther.m_ShowInRunLog;
    m_Source = rOther.m_Source;
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
