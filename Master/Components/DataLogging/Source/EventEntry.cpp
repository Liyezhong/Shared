/****************************************************************************/
/*! \file DataLogging/Source/EventEntry.cpp
 *
 *  \brief Implementation file for class EventEntry.
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

#include <DataLogging/Include/EventEntry.h>
#include <Global/Include/Utils.h>

namespace DataLogging {

/****************************************************************************/
EventEntry::EventEntry() :
    m_EventType(Global::EVTTYPE_UNDEFINED),
    m_EventStatus(Global::EVTSTAT_ON),
    m_EventCode(Global::EVENT_GLOBAL_UNDEFINED)
{
}

/****************************************************************************/
EventEntry::EventEntry(const EventEntry &rOther) :
    m_EventType(Global::EVTTYPE_UNDEFINED),
    m_EventStatus(Global::EVTSTAT_ON),
    m_EventCode(Global::EVENT_GLOBAL_UNDEFINED)
{
    CopyFrom(rOther);
}

/****************************************************************************/
EventEntry::EventEntry(const QDateTime &TimeStamp, const Global::LoggingSource &Source, Global::EventType TheEventType,
                       Global::EventStatus TheEventStatus, quint32 EventCode) :
    m_TimeStamp(TimeStamp),
    m_Source(Source),
    m_EventType(TheEventType),
    m_EventStatus(TheEventStatus),
    m_EventCode(EventCode)
{
}

/****************************************************************************/
EventEntry::EventEntry(const QDateTime &TimeStamp, const Global::LoggingSource &Source, Global::EventType TheEventType,
                       Global::EventStatus TheEventStatus, quint32 EventCode,
                       const Global::tTranslatableStringList &AdditionalData) :
    m_TimeStamp(TimeStamp),
    m_Source(Source),
    m_EventType(TheEventType),
    m_EventStatus(TheEventStatus),
    m_EventCode(EventCode),
    m_AdditionalData(AdditionalData)
{
}

/****************************************************************************/
EventEntry::~EventEntry() {
}

/****************************************************************************/
void EventEntry::CopyFrom(const EventEntry &rOther) {
    m_TimeStamp = rOther.m_TimeStamp;
    m_Source = rOther.m_Source;
    m_EventType = rOther.m_EventType;
    m_EventStatus = rOther.m_EventStatus;
    m_EventCode = rOther.m_EventCode;
    m_AdditionalData = rOther.m_AdditionalData;
}

/****************************************************************************/
const EventEntry & EventEntry::operator = (const EventEntry &rOther) {
    if(this != &rOther) {
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
void EventEntry::DumpToConsole() const {
    QString LoggingString = m_TimeStamp.toString("yyyy-MM-dd hh:mm:ss.zzz") + ";" +
                            QString::number(m_Source.GetSource(), 10) + ";" +
                            QString::number(m_Source.GetSubComponent(), 10) + ";" +
                            QString::number(AsInt(m_EventType), 10) + ";" +
                            QString::number(AsInt(m_EventStatus), 10) + ";" +
                            QString::number(m_EventCode, 10);
    if(m_AdditionalData.size() > 0) {
        LoggingString += ":";
    }
    // append additional data. Don't fiddle around with recursive AdditionalData strings and string ids.
    for(Global::tTranslatableStringList::const_iterator it = m_AdditionalData.constBegin(); it != m_AdditionalData.constEnd(); ++it) {
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
