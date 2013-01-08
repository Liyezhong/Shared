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

protected:
    quint32                         m_EventCode;             ///< Event code/Event ID for event entry - Also used as String ID to get corresponding string for translation
    QString                         m_EventName;             ///< Name of event
    Global::EventType               m_EventType;             ///< Event type/level of event entry.
    Global::ActionType              m_ActionPositive;                ///< Action type Positive
    qint8                           m_NumberOfRetries;       ///< Retry attempts for the action
    Global::ActionType              m_ActionNegative;        ///< Next Action type for Negative action
    Global::ActionType              m_NextAction;        ///< Next Action type for Positive action
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
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    void CopyFrom(const EventCSVInfo &rOther);
protected:
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
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    EventCSVInfo(const EventCSVInfo &rOther);
    /****************************************************************************/


    /****************************************************************************/
    /**
     * \brief Constructor with a translatable string.
     *

     * \param[in]   EventCode             EventID
     * \param[in]   EventType             Fatal/Warning/Info
     * \param[in]   ActionType            Action for the event
     * \param[in]   Retries               Number of retries allowed
     * \param[in]   ShowInRunLog          true - show , false - hide
     * \param[in]   LoggingSource         Source -Scheduler/main controller etc
     * \param[in]   LogLevel              Enabled, disabled, debug, console
     * \param[in]   EventSource           Source Component for the Event
     * \param[in]   MessageType           Error/Info/Warning/Reboot/Retry
     * \param[in]   Ack                   true - Required, false - Not Required
     * \param[in]   AlarmRequired         true - Required, false - Not Required
     * \param[in]   GuiButtonType         List of GUI buttons for the event
     * \param[in]   StatusBarIcon         true - Required, false - Not Required
     * \param[in]   SourceComponent       Source of event as string, read from EventConf file
     */
    /****************************************************************************/
    EventCSVInfo(const quint32 &EventCode, const Global::EventType &EventType, const Global::ActionType &ActionType,
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
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther  Const reference to other instance.
     */
    /****************************************************************************/
  //  const EventCSVInfo & operator = (const EventCSVInfo &rOther);
    /****************************************************************************/





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

    inline void SetEventType(Global::EventType EventType) {
        m_EventType = EventType;
    }
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
        return (Global::ActionType)m_ActionPositive;
    }

    inline Global::ActionType GetPositiveActionType() const {
        return (Global::ActionType)m_NextAction;
    }

    inline Global::ActionType GetNegativeActionType() const {
        return (Global::ActionType)m_ActionNegative;
    }

    //    inline void SetActionType(Global::ActionType ActionType, qint8 NumberOfAttempts = 0) {
    //        m_ActionType = ActionType;
    //        m_NumberOfRetryAttempts = NumberOfAttempts;
    //    }

    /****************************************************************************/
    /**
     * \brief Get the event code of event entry.
     *
     * \return      Event code.
     */
    /****************************************************************************/
    inline quint32 GetEventCode() const {
        return m_EventCode;
    }

    inline void SetEventCode(quint32 EventCode) {
        m_EventCode = EventCode;
    }

    /****************************************************************************/
    /**
     * \brief Get the button type of event entry.
     *
     * \return      Event code.
     */
    /****************************************************************************/
    inline Global::GuiButtonType GetButtonType() const {
        return m_ButtonType;
    }

    inline void SetButtonType(Global::GuiButtonType buttonType) {
        m_ButtonType = buttonType;
    }

    /****************************************************************************/
    /**
     * \brief Get the status icon flag of event entry.
     *
     * \return      Event code.
     */
    /****************************************************************************/
    inline bool GetStatusIcon() const {
        return m_StatusBarIcon;
    }

    inline void SetStatusIcon(bool statusIcon) {
        m_StatusBarIcon = statusIcon;
    }


    /****************************************************************************/
    /**
     * \brief Get the Name of the event.
     *
     * \return Event source.
     */
    /****************************************************************************/
    inline QString GetEventName() const {
        return m_EventName;
    }

    inline bool StatusBarIcon() const {
        return m_StatusBarIcon;
    }

    /****************************************************************************/
    /**
     * \brief Get the source of the event.
     *
     * \return Event source.
     */
    /****************************************************************************/
    inline Global::EventSourceType GetSourceComponent() const {
        return m_SourceComponent;
    }

    inline Global::LoggingSource GetSource() const {
        return m_Source;
    }

    /****************************************************************************/
    /**
     * \brief Get status of ShowInRunLogStatus
     *
     * \return ShowInRunLog flag.
     */
    /****************************************************************************/
    inline bool GetRunLogStatus() const {
        return m_ShowInRunLog;
    }

    inline void SetRunLogStatus(bool & ShowInRunLog)  {
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

    inline void SetLogLevel(const Global::EventLogLevel & LogLevel) {
        m_LogLevel = LogLevel;
    }

    inline Global::ActionType GetNextAction() const{
        return m_NextAction;
    }

    inline Global::ActionType GetActionPositive() const{

        return m_ActionPositive;

    }
    inline Global::ActionType GetActionNegative() const{

        return m_ActionNegative;

    }
    inline void SetRetries(qint8 NumberOfRetries) {
        m_NumberOfRetries = NumberOfRetries;
    }

    inline void SetActionPositiveInfo(const Global::ActionType & ActionType, qint8 NumberAttempts = 0, const Global::ActionType & ActionTypeNext = Global::ACNTYPE_NONE) {
        m_ActionPositive = ActionType;
        m_NumberOfRetries = NumberAttempts;
        m_NextAction = ActionTypeNext;

    }

    inline void SetActionInfo (const Global::ActionType & ActionType, qint8 NumberAttempts = 0, const Global::ActionType & ActionTypeNext = Global::ACNTYPE_NONE) {
        SetActionPositiveInfo(ActionType, NumberAttempts, ActionTypeNext);
    }

    inline void SetAction(const Global::ActionType & ActionType) {
        m_ActionPositive = ActionType;
    }

    inline void SetActionPositive(const Global::ActionType & ActionType) {
        m_NextAction = ActionType;
    }

    inline void SetActionNegative(const Global::ActionType & ActionType) {
        m_ActionNegative = ActionType;
    }

    /****************************************************************************/
    /**
     * \brief Get the Source Component for the Event
     *
     * \return Source component as const ref.
     */
    /****************************************************************************/
    inline Global::EventSourceType GetEventSource() const {
        return m_SourceComponent;
    }

    /****************************************************************************/
    /**
     * \brief Set Source Component for the Event
     *
     * param[in] const ref to Source Component
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
     * param[in] const ref to Message Type
     */
    /****************************************************************************/

    inline void SetMessageType(const QString & MessageType) {
        m_MessageType = MessageType;
    }





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
     * param[in] const ref to Alarm Status
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
     * param[in] const ref to GUI Options List
     */
    /****************************************************************************/

    inline void SetGUIOptions(const Global::GuiButtonType & GUIOptions) {
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
     * param[in] const ref to status bar icon flag
     */
    /****************************************************************************/

    inline void SetStatusBarIcon(const bool & StatusBarIcon) {
        m_StatusBarIcon = StatusBarIcon;
    }


}; // end class EventCSVInfo




} // end namespace EVENTHANDLER

#endif // EVENTHANDLER_EVENTCSVINFO_H
