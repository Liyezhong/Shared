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

    EventHandler::EventCSVInfo m_EventCSVInfo;               ///< Event info structure
    Global::tTranslatableStringList m_String;                ///< The translatable string.
    bool                            m_EventStatus;           ///< true/false - Set/Reset flag for the event
    Global::tRefType                m_Ref;                   ///< Ref for the acknowledgement received
    NetCommands::ClickedButton_t    m_AckType;               ///< Ack status received from GUI
    quint32                         m_EventKey;              ///< Event Key for every event raised. NULL until raised.
    QDateTime                       m_TimeStamp;             ///< TimeStamp for entry.
    quint8                          m_Count;                 ///< Number of times the event has occured
    Global::AlternateEventStringUsage m_AltEventStringUsage; ///< Alternate Event string type


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
     * \iparam  CSVInfo         Event info strucutre
     *
     */
    /****************************************************************************/
    DayEventEntry(const QDateTime &TimeStamp,quint32 EventKey,bool &EventStatus,
                                const Global::tTranslatableStringList &String, quint8 Count,
                                NetCommands::ClickedButton_t ClickButton,
                                Global::tRefType Ref, EventHandler::EventCSVInfo CSVInfo);
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
     * \brief Set the event key.
     *
     * \iparam  EventStringList  List of event strings.
     */
    /****************************************************************************/
    inline void SetTranslatableStringList(Global::tTranslatableStringList  EventStringList) {
        m_String = EventStringList;
    }

    /****************************************************************************/
    /**
     * \brief Get the run log status value from CSV structure.
     *
     * \return  Log status value.
     */
    /****************************************************************************/
    bool GetShowInRunLogStatus() const{
        return (m_EventCSVInfo.GetRunLogStatus());
    }

    /****************************************************************************/
    /**
     * \brief Get the event type from CSV structure.
     *
     * \return  Event type.
     */
    /****************************************************************************/
    Global::EventType GetEventType() const {
        return m_EventCSVInfo.GetEventType();
    }

    /****************************************************************************/
    /**
     * \brief Get the event code from CSV structure.
     *
     * \return  Event code.
     */
    /****************************************************************************/
    quint32 GetEventId () const{
        return m_EventCSVInfo.GetEventId();
    }

    /****************************************************************************/
    /**
     * \brief Get the event name from CSV structure.
     *
     * \return  Name of the event.
     */
    /****************************************************************************/
    inline QString GetEventName() const {
        return m_EventCSVInfo.GetEventName();
    }

    /****************************************************************************/
    /**
     * \brief Get the event log level from CSV structure.
     *
     * \return  Event log level.
     */
    /****************************************************************************/
    inline Global::EventLogLevel GetLogLevel() const {
        return m_EventCSVInfo.GetLogLevel();
    }

    /****************************************************************************/
    /**
     * \brief Set the event code in CSV structure.
     *
     * \iparam    EventCode     Event code.
     */
    /****************************************************************************/
    inline void SetEventCode(qint32 EventCode){
        m_EventCSVInfo.SetEventId(EventCode);
    }

    /****************************************************************************/
    /**
     * \brief Get the alaram status from CSV structure.
     *
     * \return  Alaram status is ON or OFF.
     */
    /****************************************************************************/
    inline bool GetAlarmStatus() const {
        return m_EventCSVInfo.GetAlarmStatus();
    }

    /****************************************************************************/
    /**
     * \brief Get the number retry attempts from CSV structure.
     *
     * \return  Number of the attempts.
     */
    /****************************************************************************/
    inline int GetRetryAttempts() const {
        return m_EventCSVInfo.GetRetryAttempts();
    }

    /****************************************************************************/
    /**
     * \brief Get the action negative value from CSV structure.
     *
     * \return  Action negative value.
     */
    /****************************************************************************/
    inline Global::ActionType GetActionNegative() const {
        return m_EventCSVInfo.GetActionNegative();
    }

    /****************************************************************************/
    /**
     * \brief Get the action positive value from CSV structure.
     *
     * \return  Action positive value.
     */
    /****************************************************************************/
    inline Global::ActionType GetActionPositive() const {
        return m_EventCSVInfo.GetActionPositive();
    }

    /****************************************************************************/
    /**
     * \brief Get the final action value from CSV structure.
     *
     * \return  Action final value.
     */
    /****************************************************************************/
    inline Global::ActionType GetFinalAction() const {
        return m_EventCSVInfo.GetFinalAction();
    }

    /****************************************************************************/
    /**
     * \brief Get the Status Icon value from CSV structure.
     *
     * \return  Status icon value is true or false.
     */
    /****************************************************************************/
    inline bool GetStatusIcon() const {
        return m_EventCSVInfo.GetStatusIcon();
    }

    /****************************************************************************/
    /**
     * \brief Get the acknowledgement require status from CSV structure.
     *
     * \return  On successful (true) or not (false).
     */
    /****************************************************************************/
    inline bool GetAckReqStatus() const {
        return m_EventCSVInfo.GetAckReqStatus();
    }

    /****************************************************************************/
    /**
     * \brief Set the acknowledgement require status from CSV structure.
     *
     * \iparam    Ack       Acknowledgement status.
     */
    /****************************************************************************/
    inline void SetAckReqStatus(bool Ack) {
        m_EventCSVInfo.SetAckRequired(Ack);
    }

    /****************************************************************************/
    /**
     * \brief Get the event action type from CSV structure.
     *
     * \return  Event action type.
     */
    /****************************************************************************/
    inline Global::ActionType GetActionType() const {
        return m_EventCSVInfo.GetActionType();
    }

    /****************************************************************************/
    /**
     * \brief Get the button type from CSV structure.
     *
     * \return  Button type.
     */
    /****************************************************************************/
    inline Global::GuiButtonType GetButtonType() const {
        return m_EventCSVInfo.GetButtonType();
    }

    /****************************************************************************/
    /**
     * \brief Get the GUI message box options from CSV structure.
     *
     * \return  GUI message box options.
     */
    /****************************************************************************/
    inline Global::GuiButtonType GetGUIMessageBoxOptions() const {
        return m_EventCSVInfo.GetGUIMessageBoxOptions();
    }

    /****************************************************************************/
    /**
     * \brief Get the source cmponent from CSV structure.
     *
     * \return  Source component.
     */
    /****************************************************************************/
    inline Global::EventSourceType GetSourceComponent() const {
        return m_EventCSVInfo.GetSourceComponent();
    }

    /****************************************************************************/
    /**
     * \brief Get the logging source name from CSV structure.
     *
     * \return  Logging souce name.
     */
    /****************************************************************************/
    inline Global::LoggingSource GetSource() const {
        return m_EventCSVInfo.GetSource();
    }

    /****************************************************************************/
    /**
     * \brief Set the event CSV infromation.
     *
     * \iparam    CSVInfo  CSV infromation of the event.
     */
    /****************************************************************************/
    void SetEventCSVInfo(EventHandler::EventCSVInfo CSVInfo) ;

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
    inline void SetAckValue(const NetCommands::CmdAcknEventReport & AckVal) {
        m_AckType = AckVal.GetButtonClicked();

    }

}; // end class DayEventEntry




} // end namespace DataLogging

#endif // DATALOGGING_DAYEVENTENTRY_H
