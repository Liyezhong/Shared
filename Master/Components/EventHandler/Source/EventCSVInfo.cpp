
/****************************************************************************/
/*! \file EventHandler/Source/EventCSVInfo.cpp
 *
 *  \brief Implementation file for class EventCSVInfo.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-30-11
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

#include <EventHandler/Include/EventCSVInfo.h>

namespace EventHandler {

/****************************************************************************/
EventCSVInfo::EventCSVInfo()
    : m_EventCode(0)
    , m_EventType(Global::EVTTYPE_UNDEFINED)
    , m_ActionPositive(Global::ACNTYPE_NONE)
    , m_FinalAction(Global::ACNTYPE_NONE)
    , m_NumberOfRetries(0)
    , m_ActionNegative(Global::ACNTYPE_NONE)
    , m_ShowInRunLog(false)
    , m_LogLevel(Global::LOGLEVEL_NONE)
    , m_AckRequired(false)
    , m_AlarmRequired(false)
    , m_ButtonType(Global::NOT_SPECIFIED)
    , m_StatusBarIcon(false)
    , m_SourceComponent(Global::EVENTSOURCE_MAIN)
{
}

/****************************************************************************/
EventCSVInfo::EventCSVInfo(const EventCSVInfo &rOther) {
    CopyFrom(rOther);
}

/****************************************************************************/
EventCSVInfo::EventCSVInfo(const quint32 &EventCode,const QString &Eventname, const Global::EventType &EventType, const Global::ActionType &ActionType,
                             qint8 NumofAttempts, Global::ActionType &ActionTypePositive,Global::ActionType &ActionTypeNegative,
                             const bool &ShowInRunLog, const Global::LoggingSource &LoggingSource, const Global::EventLogLevel &LogLevel,
                             const QString &MessageType, const bool &AckRequired, const bool &AlarmRequired, const Global::GuiButtonType &ButtonType,
                             const bool &StatusBarIconDisplay, const Global::EventSourceType &SourceComponent ) :

    m_EventCode(EventCode),
    m_EventMacroName(Eventname),
    m_EventType(EventType),
    m_ActionPositive(ActionTypePositive),
    m_FinalAction(ActionType),
    m_NumberOfRetries(NumofAttempts),
    m_ActionNegative(ActionTypeNegative),
    m_ShowInRunLog(ShowInRunLog),
    m_Source(LoggingSource),
    m_LogLevel(LogLevel),
    m_MessageType(MessageType),
    m_AckRequired(AckRequired),
    m_AlarmRequired(AlarmRequired),
    m_ButtonType(ButtonType),
    m_StatusBarIcon(StatusBarIconDisplay),
    m_SourceComponent(SourceComponent) {
}


/****************************************************************************/
EventCSVInfo::~EventCSVInfo() {
}

///****************************************************************************/
//const EventCSVInfo & EventCSVInfo::operator = (const EventCSVInfo &rOther) {
//    if(this != &rOther) {
//        CopyFrom(rOther);
//    }
//    return *this;
//}

/****************************************************************************/
void EventCSVInfo::CopyFrom(const EventCSVInfo &rOther) {

    m_EventCode = rOther.m_EventCode;
    m_EventMacroName = rOther.m_EventMacroName;
    m_EventType = rOther.m_EventType;
    m_FinalAction = rOther.m_FinalAction;
    m_ActionPositive = rOther.m_ActionPositive;
    m_NumberOfRetries = rOther.m_NumberOfRetries;
    m_ActionNegative = rOther.m_ActionNegative;   
    m_ShowInRunLog = rOther.m_ShowInRunLog;
    m_Source = rOther.m_Source;
    m_LogLevel = rOther.m_LogLevel;
    m_MessageType = rOther.m_MessageType;
    m_AckRequired = rOther.m_AckRequired;
    m_AlarmRequired = rOther.m_AlarmRequired;
    m_ButtonType = rOther.m_ButtonType;
    m_StatusBarIcon = rOther.m_StatusBarIcon;
    m_SourceComponent = rOther.m_SourceComponent;

}


} // end namespace DataLogging
