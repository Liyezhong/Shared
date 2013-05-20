
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
    , m_FinalAction(Global::ACNTYPE_NONE)
    , m_NumberOfRetries(0)
    , m_LogLevel(Global::LOGLEVEL_NONE)
    , m_LogAuthorityType(Global::LOGAUTHTYPE_NO_SHOW)
    , m_AlarmPosType(Global::ALARMPOS_NONE)
    , m_StatusBarIcon(false)
{
    m_RecoveryActions[0] = Global::ACNTYPE_NONE;
    m_RecoveryActions[1] = Global::ACNTYPE_NONE;
    m_RecoveryActions[2] = Global::ACNTYPE_NONE;
    m_RecoveryActions[3] = Global::ACNTYPE_NONE;
    m_Buttons[0] = Global::NO_BUTTON;
    m_Buttons[1] = Global::NO_BUTTON;
}

/****************************************************************************/
EventCSVInfo::EventCSVInfo(const EventCSVInfo &rOther) {
    CopyFrom(rOther);
}

///****************************************************************************/
//EventCSVInfo::EventCSVInfo(const quint32 &EventCode,const QString &Eventname, const Global::EventType &EventType, const Global::ActionType &ActionType,
//                             qint8 NumofAttempts, Global::ActionType &ActionTypePositive,Global::ActionType &ActionTypeNegative,
//                             Global::LogAuthorityType LogAuthorityType, const Global::LoggingSource &LoggingSource, const Global::EventLogLevel &LogLevel,
//                             const QString &MessageType, const bool &AckRequired, Global::AlarmPosType AlarmPosType, const Global::GuiButtonType &ButtonType,
//                             const bool &StatusBarIcon, const Global::EventSourceType &SourceComponent,
//                             const QString& ErrorOutline, Global::ActionType ResponseType,
//                             const QString& DetailForRD,
//                             Global::ResponseRecoveryType& ResponseRecoveryType ) :

//    m_EventCode(EventCode),
//    m_EventMacroName(Eventname),
//    m_EventType(EventType),
//    m_ActionPositive(ActionTypePositive),
//    m_FinalAction(ActionType),
//    m_NumberOfRetries(NumofAttempts),
//    m_ActionNegative(ActionTypeNegative),
//    m_LogAuthorityType(LogAuthorityType),
//    m_Source(LoggingSource),
//    m_LogLevel(LogLevel),
//    m_MessageType(MessageType),
//    m_AckRequired(AckRequired),
//    m_AlarmPosType(AlarmPosType),
//    m_ButtonType(ButtonType),
//    m_StatusBarIcon(StatusBarIcon),
//    m_SourceComponent(SourceComponent),
//    m_ErrorOutline(ErrorOutline),
//    m_ResponseAction(ResponseType),
//    m_DetailForRD(DetailForRD),
//    m_ResponseRecoveryType(ResponseRecoveryType)
//{
//}


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
    m_NumberOfRetries = rOther.m_NumberOfRetries;  
    m_LogAuthorityType = rOther.m_LogAuthorityType;
    m_Source = rOther.m_Source;
    m_LogLevel = rOther.m_LogLevel;
    m_MessageType = rOther.m_MessageType;
    m_AckRequired = rOther.m_AckRequired;
    m_AlarmPosType = rOther.m_AlarmPosType;
    m_StatusBarIcon = rOther.m_StatusBarIcon;
    m_SourceComponent = rOther.m_SourceComponent;
    m_ErrorOutline = rOther.m_ErrorOutline;
    m_ResponseAction = rOther.m_ResponseAction;
    m_DetailForRD = rOther.m_DetailForRD;
    m_ResponseRecoveryType = rOther.m_ResponseRecoveryType;

    m_RecoveryActions[0] = rOther.GetRecoveryActionOnRspFailUsrPst();
    m_RecoveryActions[1] = rOther.GetRecoveryActionOnRspFailUsrNeg();
    m_RecoveryActions[2] = rOther.GetRecoveryActionOnRspSuccUsrPst();
    m_RecoveryActions[3] = rOther.GetRecoveryActionOnRspSuccUsrNeg();
    m_Buttons[0] = rOther.GetButtonTypeOnRspFail();
    m_Buttons[1] = rOther.GetButtonTypeOnRspSucc();
}


} // end namespace DataLogging
