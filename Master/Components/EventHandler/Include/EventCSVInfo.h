/****************************************************************************/
/*! \file EventHandler/Include/EventCSVInfo.h
 *
 *  \brief Definition file for class EventCSVInfo.
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

#ifndef EVENTHANDLER_EVENTCSVINFO_H
#define EVENTHANDLER_EVENTCSVINFO_H

#include <Global/Include/TranslatableString.h>
#include <Global/Include/LoggingSource.h>
#include <Global/Include/Utils.h>
#include <QStringList>

//lint -sem(EventHandler::EventCSVInfo::CopyFrom,initializer)
namespace EventHandler{

/****************************************************************************/
/**
 * \brief Class used to deliver the content of an DayOperation entry.
 *
 * This content consists of a time stamp a string ID and an argument list. It
 * has to be translatable.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class EventCSVInfo {

private:


protected:
    quint32                         m_EventCode;             ///< Event code/Event ID for event entry - Also used as String ID to get corresponding string for translation
    QString                         m_EventMacroName;        ///< Name of event macro
    Global::EventType               m_EventType;             ///< Event type/level of event entry.
    Global::ActionType              m_ActionPositive;        ///< Action type Positive ( action + retry + action pattern)
    Global::ActionType              m_FinalAction;           ///< default action for event
    qint8                           m_NumberOfRetries;       ///< Retry attempts for the action
    Global::ActionType              m_ActionNegative;        ///< Next Action type for Negative action
    bool                            m_ShowInRunLog;          ///< true - show in GUI its and daily run log, else dont show its error log.
    Global::LoggingSource           m_Source;                ///< Source for event entry.
    Global::EventLogLevel           m_LogLevel;              ///< Various log levels
    QString                         m_MessageType;           ///< Error/Info/Warning/Reboot/Retry
    bool                            m_AckRequired;           ///< true - Needs acknowledge
    bool                            m_AlarmRequired;         ///< true - fire alarm
    Global::GuiButtonType           m_ButtonType;            ///< One of the valid options
    bool                            m_StatusBarIcon;         ///< true - Display icon in status bar
    Global::EventSourceType         m_SourceComponent;       ///< SourceComponent that raises the event


    /****************************************************************************/
    /**
     * \brief Copy other instance to self.
     *
     * \iparam   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    void CopyFrom(const EventCSVInfo &rOther);

public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     *
     * Implemented due to signal / slot mechanism.
     */
    /****************************************************************************/
    EventCSVInfo();
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \iparam   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    EventCSVInfo(const EventCSVInfo &rOther);
    /****************************************************************************/


    /****************************************************************************/
    /**
     * \brief Constructor with a translatable string.
     *

     * \iparam   EventCode             EventID
     * \iparam   EventMacroName        Event Macro Name
     * \iparam   EventType             Fatal/Warning/Info
     * \iparam   ActionType            Action for the event
     * \iparam   NumofAttempts         Number of retries allowed
     * \iparam   ActionTypePositive    Action type Positive ( action + retry + action pattern)
     * \iparam   ActionTypeNegative    Next Action type for Negative action
     * \iparam   ShowInRunLog          true - show , false - hide
     * \iparam   LoggingSource         Source -Scheduler/main controller etc
     * \iparam   LogLevel              Enabled, disabled, debug, console
     * \iparam   MessageType           Error/Info/Warning/Reboot/Retry
     * \iparam   AckRequired           true - Required, false - Not Required
     * \iparam   AlarmRequired         true - Required, false - Not Required
     * \iparam   ButtonType            List of GUI buttons for the event
     * \iparam   StatusBarIcon         true - Required, false - Not Required
     * \iparam   SourceComponent       Source of event as string, read from EventConf file
     */
    /****************************************************************************/
    EventCSVInfo(const quint32 &EventCode, const QString &EventMacroName, const Global::EventType &EventType,  const Global::ActionType &ActionType,
                                 qint8 NumofAttempts, Global::ActionType &ActionTypePositive,Global::ActionType &ActionTypeNegative,
                                 const bool &ShowInRunLog, const Global::LoggingSource &LoggingSource, const Global::EventLogLevel &LogLevel,
                                 const QString &MessageType, const bool &AckRequired, const bool &AlarmRequired, const Global::GuiButtonType &ButtonType,
                                 const bool &StatusBarIcon, const Global::EventSourceType &SourceComponent ) ;

    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~EventCSVInfo();

  //  const EventCSVInfo & operator = (const EventCSVInfo &rOther);
    /****************************************************************************/
    /**
     * \brief Get the event type of event entry.
     *
     * \return      Event type.
     */
    /****************************************************************************/
    inline Global::EventType GetEventType() const {
        return m_EventType;
    }
    /****************************************************************************/
    /**
     * \brief Get the event type of event entry.
     *
     * \iparam      EventType
     */
    /****************************************************************************/
    inline void SetEventType(Global::EventType EventType) {
        m_EventType = EventType;
    }
    /****************************************************************************/
    /**
     * \brief Get the event type of event entry.
     *
     * \return      Number of Entries.
     */
    /****************************************************************************/
    inline  int GetRetryAttempts() const {
        return m_NumberOfRetries;
    }

    /****************************************************************************/
    /**
     * \brief Get the action type for the event.
     *
     * \return      Action type.
     */
    /****************************************************************************/
    inline Global::ActionType GetActionType() const {
        return (Global::ActionType)m_FinalAction;
    }
    /****************************************************************************/
    /**
     * \brief Get the Positive action type for the event.
     *
     * \return      Action type.
     */
    /****************************************************************************/
    inline Global::ActionType GetPositiveActionType() const {
        return (Global::ActionType)m_ActionPositive;
    }
    /****************************************************************************/
    /**
     * \brief Get the Negative action type for the event.
     *
     * \return      Action type.
     */
    /****************************************************************************/
    inline Global::ActionType GetNegativeActionType() const {
        return (Global::ActionType)m_ActionNegative;
    }

    /****************************************************************************/
    /**
     * \brief Get the event code of event entry.
     *
     * \return      Event code.
     */
    /****************************************************************************/
    inline quint32 GetEventId() const {
        return m_EventCode;
    }
    /****************************************************************************/
    /**
     * \brief Sets the event code of event entry.
     *
     * \iparam     EventCode
     */
    /****************************************************************************/
    inline void SetEventId(quint32 EventCode) {
        m_EventCode = EventCode;
    }

    /****************************************************************************/
    /**
     * \brief Get the button type of event entry.
     *
     * \return      Event code
     */
    /****************************************************************************/
    inline Global::GuiButtonType GetButtonType() const {
        return m_ButtonType;
    }
    /****************************************************************************/
    /**
     * \brief Sets the button type of event entry.
     *
     * \iparam      buttonType
     */
    /****************************************************************************/
    inline void SetButtonType(Global::GuiButtonType buttonType) {
        m_ButtonType = buttonType;
    }

    /****************************************************************************/
    /**
     * \brief Get the status icon flag of event entry.
     *
     * \return      StatusBar Icon
     */
    /****************************************************************************/
    inline bool GetStatusIcon() const {
        return m_StatusBarIcon;
    }
    /****************************************************************************/
    /**
     * \brief Sets the Status Icon of event entry.
     *
     * \iparam      statusIcon
     */
    /****************************************************************************/
    inline void SetStatusIcon(bool statusIcon) {
        m_StatusBarIcon = statusIcon;
    }


    /****************************************************************************/
    /**
     * \brief Gets the Name of the event.
     *
     *  \return EventMacroName
     */
    /****************************************************************************/
    inline QString GetEventName() const {
        return m_EventMacroName;
    }
    /****************************************************************************/
    /**
     * \brief Sets the Eventname of event entry.
     *
     * \iparam      EventName
     */
    /****************************************************************************/
    inline void SetEventName(QString EventName) {
        m_EventMacroName = EventName;
    }
    /****************************************************************************/
    /**
     * \brief Gets the Name of the event.
     *
     *  \return StatusBarIcon
     */
    /****************************************************************************/
    inline bool StatusBarIcon() const {
        return m_StatusBarIcon;
    }

    /****************************************************************************/
    /**
     * \brief Gets the source of the event.
     *
     * \return m_SourceComponent
     */
    /****************************************************************************/
    inline Global::EventSourceType GetSourceComponent() const {
        return m_SourceComponent;
    }
    /****************************************************************************/
    /**
     * \brief Gets the Loggingsource of the event.
     *
     * \return m_Source
     */
    /****************************************************************************/
    inline Global::LoggingSource GetSource() const {
        return m_Source;
    }
    /****************************************************************************/
    /**
     * \brief Sets the Loggingsource of the event
     *
     * \iparam Source
     */
    /****************************************************************************/
    inline  void SetSource( Global::LoggingSource Source)  {
         m_Source = Source;
    }
    /****************************************************************************/
    /**
     * \brief Get status of ShowInRunLogStatus
     *
     * \return ShowInRunLog flag
     */
    /****************************************************************************/
    inline bool GetRunLogStatus() const {
        return m_ShowInRunLog;
    }
    /****************************************************************************/
    /**
     * \brief Sets the status of ShowInRunLogStatus.
     *
     * \iparam ShowInRunLog
     */
    /****************************************************************************/
    inline void SetRunLogStatus(bool  ShowInRunLog)  {
        m_ShowInRunLog = ShowInRunLog;
    }

    /****************************************************************************/
    /**
     * \brief Get the source of the event.
     *
     * \return Event source.
     */
    /****************************************************************************/

    inline Global::EventLogLevel GetLogLevel() const {
        return m_LogLevel;
    }
    /****************************************************************************/
    /**
     * \brief Sets the source of the event.
     *
     * \iparam LogLevel
     */
    /****************************************************************************/
    inline void SetLogLevel(const Global::EventLogLevel & LogLevel) {
        m_LogLevel = LogLevel;
    }    
    /****************************************************************************/
    /**
     * \brief Gets the Final action type of the event.
     *
     * \return m_FinalAction.
     */
    /****************************************************************************/
    inline Global::ActionType GetFinalAction() const{
        return m_FinalAction;
    }
    /****************************************************************************/
    /**
     * \brief Gets Positive Action type of the event.
     *
     * \return m_ActionPositive.
     */
    /****************************************************************************/
    inline Global::ActionType GetActionPositive() const{
        return m_ActionPositive;
    }
    /****************************************************************************/
    /**
     * \brief Gets Negative Action type of the event.
     *
     * \return m_ActionNegative.
     */
    /****************************************************************************/
    inline Global::ActionType GetActionNegative() const{
        return m_ActionNegative;
    }
    /****************************************************************************/
    /**
     * \brief Sets the Number of Retries.
     *
     * \iparam NumberOfRetries
     */
    /****************************************************************************/
    inline void SetRetries(qint8 NumberOfRetries) {
        m_NumberOfRetries = NumberOfRetries;
    }   
    /****************************************************************************/
    /**
     * \brief Sets the Final Action type of Event.
     *
     * \iparam  ActionType
     */
    /****************************************************************************/
    inline void SetFinalAction(const Global::ActionType & ActionType) {
        m_FinalAction = ActionType;
    }
    /****************************************************************************/
    /**
     * \brief Sets the positive action Type of event.
     *
     * \iparam   ActionType
     */
    /****************************************************************************/
    inline void SetActionPositive(const Global::ActionType & ActionType) {
        m_ActionPositive = ActionType;
    }
    /****************************************************************************/
    /**
     * \brief Sets Negative Action type of the event.
     *
     * \iparam  ActionType
     */
    /****************************************************************************/
    inline void SetActionNegative(const Global::ActionType & ActionType) {
        m_ActionNegative = ActionType;
    }

    /****************************************************************************/
    /**
     * \brief Gets the Source Component for the Event
     *
     * \return Source component as const ref.
     */
    /****************************************************************************/
    inline Global::EventSourceType GetEventSource() const {
        return m_SourceComponent;
    }

    /****************************************************************************/
    /**
     * \brief Sets Source Component for the Event
     *
     * \iparam EventSource
     */
    /****************************************************************************/
    inline void SetEventSource(const Global::EventSourceType & EventSource) {
        m_SourceComponent = EventSource;
    }

    /****************************************************************************/
    /**
     * \brief Get the Message Type for the Event
     *
     * \return Message Type as const ref.
     */
    /****************************************************************************/
    inline QString GetMessageType() const {
        return m_MessageType;
    }

    /****************************************************************************/
    /**
     * \brief Set Message Type for the Event
     *
     * \iparam MessageType
     */
    /****************************************************************************/

    inline void SetMessageType(const QString & MessageType) {
        m_MessageType = MessageType;
    }
    /****************************************************************************/
    /**
     * \brief Set Acknowlegdement Required for the Event
     *
     * \iparam AckReq
     */
    /****************************************************************************/

    inline void SetAckRequired(const bool AckReq) {
        m_AckRequired = AckReq;
    }

    /****************************************************************************/
    /**
     * \brief Get the Ack Status
     *
     * \return Ack Requirement Status as const ref.
     */
    /****************************************************************************/
    inline bool GetAckReqStatus() const {
        return m_AckRequired;
    }



    /****************************************************************************/
    /**
     * \brief Get the Alarm Status
     *
     * \return Alarm Status as const ref.
     */
    /****************************************************************************/
    inline bool GetAlarmStatus() const {
        return m_AlarmRequired;
    }

    /****************************************************************************/
    /**
     * \brief Set Alarm Status
     *
     * \iparam AlarmRequired
     */
    /****************************************************************************/

    inline void SetAlarmStatus(const bool & AlarmRequired) {
        m_AlarmRequired = AlarmRequired;
    }

    /****************************************************************************/
    /**
     * \brief Get the GUI Options to Display in Message Box of GUI
     *
     * \return List of GUIOptions as const ref.
     */
    /****************************************************************************/
    inline Global::GuiButtonType GetGUIMessageBoxOptions() const {
        return m_ButtonType;
    }

    /****************************************************************************/
    /**
     * \brief Set GUI Options
     *
     * \iparam GUIOptions
     */
    /****************************************************************************/

    inline void SetGUIOptions(const Global::GuiButtonType  GUIOptions) {
        m_ButtonType = GUIOptions;
    }

    /****************************************************************************/
    /**
     * \brief Get the Status bar icon flag
     *
     * \return Status bar icon as const ref.
     */
    /****************************************************************************/
    inline bool GetStatusBarIcon() const {
        return m_StatusBarIcon;
    }

    /****************************************************************************/
    /**
     * \brief Set status bar icon in GUI
     *
     * \iparam StatusbarIcon
     */
    /****************************************************************************/

    inline void SetStatusBarIcon(const bool & StatusbarIcon) {
        m_StatusBarIcon = StatusbarIcon;
    }


}; // end class EventCSVInfo




} // end namespace EVENTHANDLER

#endif // EVENTHANDLER_EVENTCSVINFO_H
