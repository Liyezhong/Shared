
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
{
}

/****************************************************************************/
EventCSVInfo::EventCSVInfo(const EventCSVInfo &rOther) {
    CopyFrom(rOther);
}

/****************************************************************************/
EventCSVInfo::EventCSVInfo(const quint32 &EventCode, const Global::EventType &EventType, const Global::ActionType &ActionType,
                             qint8 NumofAttempts, Global::ActionType &ActionTypePositive,Global::ActionType &ActionTypeNegative,
                             const bool &ShowInRunLog, const Global::LoggingSource &LoggingSource, const Global::EventLogLevel &LogLevel,
                             const QString &MessageType, const bool &AckRequired, const bool &AlarmRequired, const Global::GuiButtonType &ButtonType,
                             const bool &StatusBarIcon, const Global::EventSourceType &SourceComponent ) :

    m_EventCode(EventCode),
    m_EventType(EventType),
    m_ActionPositive(ActionType),
    m_NumberOfRetries(NumofAttempts),
    m_NextAction(ActionTypePositive),
    m_ActionNegative(ActionTypeNegative),
    m_ShowInRunLog(ShowInRunLog),
    m_Source(LoggingSource),
    m_LogLevel(LogLevel),
    m_MessageType(MessageType),
    m_AckRequired(AckRequired),
    m_AlarmRequired(AlarmRequired),
    m_ButtonType(ButtonType),
    m_StatusBarIcon(StatusBarIcon),
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
    m_EventName = rOther.m_EventName;
    m_EventType = rOther.m_EventType;
    m_ActionPositive = rOther.m_ActionPositive;
    m_NumberOfRetries = rOther.m_NumberOfRetries;
    m_ActionNegative = rOther.m_ActionNegative;
    m_NextAction = rOther.m_NextAction;
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
