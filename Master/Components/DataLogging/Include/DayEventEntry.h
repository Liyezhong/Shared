/****************************************************************************/
/*! \file DataLogging/Include/DayEventEntry.h
 *
 *  \brief Definition file for class DayEventEntry.
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

#ifndef DATALOGGING_DAYEVENTENTRY_H
#define DATALOGGING_DAYEVENTENTRY_H

#include <Global/Include/TranslatableString.h>
#include <Global/Include/LoggingSource.h>
#include <QStringList>
#include <Global/Include/Commands/AckOKNOK.h>
#include <NetCommands/Include/CmdAcknEventReport.h>
#include <EventHandler/Include/EventCSVInfo.h>
#include <QDateTime>

//lint -sem(DataLogging::DayEventEntry::CopyFrom,initializer)

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Class used to deliver the content of an DayOperation entry.
 *
 * This content consists of a time stamp a string ID and an argument list. It
 * has to be translatable.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class DayEventEntry  {
    friend class TestDayOperationEntry;
private:
    bool                            m_EventStatus;           ///< true/false - Set/Reset flag for the event
    Global::tRefType                m_Ref;                   ///< Ref for the acknowledgement received
    NetCommands::ClickedButton_t    m_AckType;               ///< Ack status received from GUI
    quint32                         m_EventKey;              ///< Event Key for every event raised. NULL until raised.
    QDateTime                       m_TimeStamp;             ///< TimeStamp for entry.
    quint8                          m_Count;                 ///< Number of times the event has occured
    Global::tTranslatableStringList m_String;                ///< The translatable string.


    quint32 m_EventCode;                                    ///< EVENT CODE
    QString m_EventName;                                    ///< event name - macro
    Global::EventType m_EventType;                          ///< event type

    Global::ActionType m_ActionNegative;                        ///< negative action
    Global::ActionType m_ActionPositive;                        ///< positive action
    Global::ActionType m_ActionFinal;                           ///< default action
    Global::EventSourceType m_EventSoure;                       ///< event source
    bool m_AlarmStatus;                                         ///< alarm status
    Global::AlarmPosType m_AlarmType;                           ///< alarm type
    Global::EventLogLevel m_LogLevel;                          ///< Event log level
    bool m_ShowRunLogStatus;                                  ///< show in user log or not
    Global::GuiButtonType m_ButtonType;                     ///< Button type for message box.
    bool m_StatusIcon;                                      ///< put on status bar or not
    Global::AlternateEventStringUsage m_AltEventStringUsage; ///< Alternate Event string type


    int m_RetryAttempts;                                    ///< times to retry to repair.
    bool m_AckReqStatus;                                    ///< do we need Ack for Event: if have event source, yes,

    quint32 m_Scenario;                                     ///< scenario for event
    QString m_ActionString;                                 ///< Action
    quint32 m_StringID;                                     ///< Message or log string ID

    /****************************************************************************/
    /**
     * \brief Copy other instance to self.
     *
     * \iparam   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    void CopyFrom(const DayEventEntry &rOther);

public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     *
     * Implemented due to signal / slot mechanism.
     */
    /****************************************************************************/
    DayEventEntry();

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam  TimeStamp       Time of the system
     * \iparam  EventKey        Key fo the event
     * \iparam  EventStatus     Status of the event
     * \iparam  String          List of the tranlsated strings
     * \iparam  Count           No. of time event occured
     * \iparam  ClickButton     Clicked button on GUI
     * \iparam  Ref             Reference number of the command
     *
     */
    /****************************************************************************/
    DayEventEntry(const QDateTime &TimeStamp,quint32 EventKey,bool &EventStatus,
                                const Global::tTranslatableStringList &String, quint8 Count,
                                NetCommands::ClickedButton_t ClickButton,
                                Global::tRefType Ref);
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \iparam   rOther  Const reference to other instance.
     */
    /****************************************************************************/
    DayEventEntry(const DayEventEntry &rOther);

    /****************************************************************************/
    /**
     * \brief Constructor with a translatable string.
     *
     * \iparam   TimeStamp       Timestamp.
     * \iparam   String          The complete translatable string.
     */
    /****************************************************************************/
    DayEventEntry(const QDateTime &TimeStamp, const Global::tTranslatableStringList &String);

    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~DayEventEntry();

    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \iparam   rOther  Const reference to other instance.
     *
     * \return   Const reference to self.
     */
    /****************************************************************************/
    const DayEventEntry & operator = (const DayEventEntry &rOther);

    /****************************************************************************/
    /**
     * \brief Dump content to console
     */
    /****************************************************************************/
    void DumpToConsole() const;

    /****************************************************************************/
    /**
     * \brief Set the alternative string usage.
     *
     * \iparam   AltStringUsage  Alternative string usage.
     */
    /****************************************************************************/
    inline void SetAltStringUsage(const Global::AlternateEventStringUsage AltStringUsage) {
        m_AltEventStringUsage = AltStringUsage;
    }

    /****************************************************************************/
    /**
     * \brief Get the alternative string usage.
     *
     * \return   Alternative string usage
     */
    /****************************************************************************/
    inline Global::AlternateEventStringUsage GetAltStringUsageType() const {
        return m_AltEventStringUsage;
    }

    /****************************************************************************/
    /**
     * \brief Get the timestamp.
     *
     * \return  Timestamp.
     */
    /****************************************************************************/
    inline QDateTime GetTimeStamp() const {
        return m_TimeStamp;
    }

    /****************************************************************************/
    /**
     * \brief Set the date time value.
     *
     * \iparam    DateTime  Date time value.
     */
    /****************************************************************************/
    inline void SetDateTime(QDateTime DateTime) {
        m_TimeStamp = DateTime;
    }

    /****************************************************************************/
    /**
     * \brief Get the Event Key.
     *
     * \return  Event Key.
     */
    /****************************************************************************/
    inline quint32 GetEventKey() const {
        return m_EventKey;
    }

    /****************************************************************************/
    /**
     * \brief Set the event key.
     *
     * \iparam    EventKey  Event key value.
     */
    /****************************************************************************/
    inline void SetEventKey(quint32 EventKey) {
        m_EventKey = EventKey;
    }

    /****************************************************************************/
    /**
     * \brief Get the argument list as const reference.
     *
     * \return  Argument list  as const reference.
     */
    /****************************************************************************/
    inline const Global::tTranslatableStringList &GetString() const {
        return m_String;
    }

    /****************************************************************************/
    /**
     * \brief set the argument list .
     *
     * \param  String Argument list.
     */
    /****************************************************************************/
    inline void SetString(const Global::tTranslatableStringList &String) {
        m_String = String;
    }


    /****************************************************************************/
    /**
     * \brief Get the run log status value .
     *
     * \return  Log status value.
     */
    /****************************************************************************/
    bool GetShowInRunLogStatus() const{
        return m_ShowRunLogStatus;
    }

    /****************************************************************************/
    /**
     * \brief Set the run log status value .
     *
     * \param  RunLogStatus Log status value.
     */
    /****************************************************************************/
    void SetShowInRunLogStatus(bool RunLogStatus){
        m_ShowRunLogStatus = RunLogStatus;
    }

    /****************************************************************************/
    /**
     * \brief Get the event type .
     *
     * \return  Event type.
     */
    /****************************************************************************/
    Global::EventType GetEventType() const {
        return m_EventType;
    }

    /****************************************************************************/
    /**
     * \brief Set the event type .
     * \param  EventType event type.
     * \return  void.
     */
    /****************************************************************************/
    void SetEventType(Global::EventType EventType) {
        m_EventType = EventType;
    }


    /****************************************************************************/
    /**
     * \brief Get the event name .
     *
     * \return  Name of the event.
     */
    /****************************************************************************/
    inline QString GetEventName() const {
        return m_EventName;
    }

    /****************************************************************************/
    /**
     * \brief Set the event name .
     *
     * \param   EventName Name of the event.
     */
    /****************************************************************************/
    inline void SetEventName(QString EventName) {
        m_EventName = EventName;
    }

    /****************************************************************************/
    /**
     * \brief Get the event log level .
     *
     * \return  Event log level.
     */
    /****************************************************************************/
    inline Global::EventLogLevel GetLogLevel() const {
        return m_LogLevel;
    }

    /****************************************************************************/
    /**
     * \brief Set the event log level .
     *
     * \param  LogLevel Event log level.
     */
    /****************************************************************************/
    inline void SetLogLevel(Global::EventLogLevel LogLevel) {
        m_LogLevel = LogLevel;
    }

    /****************************************************************************/
    /**
     * \brief Get the event code .
     *
     * \return  Event code.
     */
    /****************************************************************************/
    quint32 GetEventCode () const{
        return m_EventCode;
    }
    /****************************************************************************/
    /**
     * \brief Set the event code
     *
     * \iparam    EventCode     Event code.
     */
    /****************************************************************************/
    inline void SetEventCode(qint32 EventCode){
        m_EventCode= EventCode;
    }

    /****************************************************************************/
    /**
     * \brief Get the alaram status .
     *
     * \return  Alaram status is ON or OFF.
     */
    /****************************************************************************/
    inline bool GetAlarmStatus() const {
        return m_AlarmStatus;
    }

    /****************************************************************************/
    /**
     * \brief Set the alaram status .
     *
     * \param  AlarmStatus Alaram status is ON or OFF.
     */
    /****************************************************************************/
    inline void SetAlarmStatus(bool AlarmStatus) {
        m_AlarmStatus = AlarmStatus;
    }

    /****************************************************************************/
    /**
     * \brief Get the alaram type .
     *
     * \return  Alaram type.
     */
    /****************************************************************************/
    inline Global::AlarmPosType GetAlarmType() const {
        return m_AlarmType;
    }

    /****************************************************************************/
    /**
     * \brief Set the alaram type .
     *
     * \param  AlarmType Alaram status is ON or OFF.
     */
    /****************************************************************************/
    inline void SetAlarmType(Global::AlarmPosType AlarmType) {
        m_AlarmType = AlarmType;
    }

    /****************************************************************************/
    /**
     * \brief Get the number retry attempts .
     *
     * \return  Number of the attempts.
     */
    /****************************************************************************/
    inline int GetRetryAttempts() const {
        return m_RetryAttempts;
    }

    /****************************************************************************/
    /**
     * \brief Get the action negative value .
     *
     * \return  GetActionNegative Action negative value.
     */
    /****************************************************************************/
    inline Global::ActionType GetActionNegative() const {
        return m_ActionNegative;
    }

    /****************************************************************************/
    /**
     * \brief Get the action positive value .
     *
     * \return  Action positive value.
     */
    /****************************************************************************/
    inline Global::ActionType GetActionPositive() const {
        return m_ActionPositive;
    }

    /****************************************************************************/
    /**
     * \brief Get the final action value .
     *
     * \return  Action final value.
     */
    /****************************************************************************/
    inline Global::ActionType GetFinalAction() const {
        return m_ActionFinal;
    }

    /****************************************************************************/
    /**
     * \brief Set the final action value .
     *
     * \iparam Action Action final value.
     */
    /****************************************************************************/
    inline  void SetFinalAction(Global::ActionType Action) {
        m_ActionFinal = Action;
    }
    /****************************************************************************/
    /**
     * \brief Get the Status Icon value .
     *
     * \return  Status icon value is true or false.
     */
    /****************************************************************************/
    inline bool GetStatusIcon() const {
        return m_StatusIcon;
    }

    /****************************************************************************/
    /**
     * \brief Set the Status Icon value .
     *
     * \param  StatusIcon Status icon value is true or false.
     */
    /****************************************************************************/
    inline void SetStatusIcon(bool StatusIcon) {
        m_StatusIcon = StatusIcon;
    }

    /****************************************************************************/
    /**
     * \brief Get the acknowledgement require status .
     *
     * \return  On successful (true) or not (false).
     */
    /****************************************************************************/
    inline bool GetAckReqStatus() const {
        return m_AckReqStatus;
    }

    /****************************************************************************/
    /**
     * \brief Set the acknowledgement require status .
     *
     * \iparam    Ack       Acknowledgement status.
     */
    /****************************************************************************/
    inline void SetAckReqStatus(bool Ack) {
        m_AckReqStatus = Ack;
    }

    /****************************************************************************/
    /**
     * \brief Get the event action type .
     *
     * \return  Event action type.
     */
    /****************************************************************************/
    inline Global::ActionType GetActionType() const {
        return m_ActionFinal;
    }

    /****************************************************************************/
    /**
     * \brief Get the button type .
     *
     * \return  Button type.
     */
    /****************************************************************************/
    inline Global::GuiButtonType GetButtonType() const {
        return m_ButtonType;
    }

    /****************************************************************************/
    /**
     * \brief Set the button type .
     *
     * \param  ButtonType Button type.
     */
    /****************************************************************************/
    inline void SetButtonType(Global::GuiButtonType ButtonType) {
        m_ButtonType = ButtonType;
    }

    /****************************************************************************/
    /**
     * \brief Get the source cmponent .
     *
     * \return  Source component.
     */
    /****************************************************************************/
    inline Global::EventSourceType GetEventSource() const {
        return m_EventSoure;
    }

    /****************************************************************************/
    /**
     * \brief Set the source cmponent .
     *
     * \param  EventSource Source component.
     */
    /****************************************************************************/
    inline void SetEventSource(Global::EventSourceType EventSource) {
        m_EventSoure = EventSource;
    }
    /****************************************************************************/
    /**
     * \brief Get the Event Status
     *
     * \return Event Status as const ref.
     */
    /****************************************************************************/
    inline bool IsEventActive() const {
        return m_EventStatus;
    }

    /****************************************************************************/
    /**
     * \brief Set Event Status
     *
     * \iparam  EventStatus   const ref to Event Status
     */
    /****************************************************************************/
    inline void SetEventStatus(const bool & EventStatus) {
        m_EventStatus = EventStatus;
    }

    /****************************************************************************/
    /**
     * \brief Get the Ack Status
     *
     * \return Ack Status as const ref.
     */
    /****************************************************************************/
    inline NetCommands::ClickedButton_t  GetAckValue() const {
        return m_AckType;
    }

    /****************************************************************************/
    /**
     * \brief Set Ack Status
     *
     * \iparam  AckVal   const ref to Ack Value
     */
    /****************************************************************************/
    inline void SetAckValue(const NetCommands::ClickedButton_t & AckVal) {
        m_AckType = AckVal;

    }

    /****************************************************************************/
    /**
     * \brief Get the Action string .
     *
     * \return  action string.
     */
    /****************************************************************************/
    inline QString GetActionString() const {
        return m_ActionString;
    }

    /****************************************************************************/
    /**
     * \brief Set the action string .
     *
     * \param  ActionString action string.
     */
    /****************************************************************************/
    inline void SetActionString(QString ActionString) {
        m_ActionString = ActionString;
    }

    /****************************************************************************/
    /**
     * \brief Get String ID .
     *
     * \return  String id.
     */
    /****************************************************************************/
    quint32 GetStringID () const{
        return m_StringID;
    }
    /****************************************************************************/
    /**
     * \brief Set string ID
     *
     * \iparam  StringID  string id.
     */
    /****************************************************************************/
    inline void SetStringID(qint32 StringID){
        m_StringID= StringID;
    }

    /****************************************************************************/
    /**
     * \brief Get Scenario .
     *
     * \return  Scenario.
     */
    /****************************************************************************/
    quint32 GetScenario () const{
        return m_Scenario;
    }
    /****************************************************************************/
    /**
     * \brief Set Scenario
     *
     * \iparam Scenario   Scenario.
     */
    /****************************************************************************/
    inline void SetScenario(qint32 Scenario){
        m_Scenario= Scenario;
    }
    /****************************************************************************/
    /**
     * \brief Update Event entry by csv
     *
     * \param EventCsv EventCSVInfo
     */
    /****************************************************************************/
    void SetEventCSVInfo(const EventHandler::EventCSVInfo& EventCsv);

}; // end class DayEventEntry




} // end namespace DataLogging

#endif // DATALOGGING_DAYEVENTENTRY_H
