/****************************************************************************/
/*! \file Include/EventHandlerThreadController.h
 *
 *  \brief Frontend for Event processing and logging.
 *
 *         All events shall be sent to this class. It will filter an
 *         incoming event according to its type and forward the event
 *         Error Handler / Data Logger.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Y.Novak
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

#ifndef EVENTHANDLER_EVENTHANDLERTHREADCONTROLLER_H
#define EVENTHANDLER_EVENTHANDLERTHREADCONTROLLER_H

#include <QObject>
#include <Global/Include/GlobalDefines.h>
#include <DataLogging/Include/DayEventEntry.h>
#include <Threads/Include/ThreadController.h>
#include <DataLogging/Include/DataLoggingThreadController.h>
#include <Global/Include/AlarmHandler.h>
#include <Global/Include/EventTranslator.h>
#include <Global/Include/UITranslator.h>
#include <Global/Include/Commands/AckOKNOK.h>
#include <DataManager/Containers/UserSettings/Include/UserSettingsInterface.h>
#include <NetCommands/Include/CmdAcknEventReport.h>
#include <QMetaType>
#include <QThread>
#include <QDebug>

namespace NetCommands {
    class CmdAcknEventReport;
}

namespace NetCommands {
    class EventReportDataStruct;
}



namespace EventHandler {

/****************************************************************************/
/**
 * \brief This is a system level EventHandler.
 *
 * All system level events (e.g. NOT local component events) from all
 * ThreadControllers are sent to this (and only to this!) class. EventHandler
 * will forward events to DataLogger and, if needed, to ErrorHandler.
 */
/****************************************************************************/
class EventHandlerThreadController : public Threads::ThreadController {
    Q_OBJECT
public:

    friend class TestEventHandlerThreadController;

    EventHandlerThreadController(quint32 ThreadID, quint32 RebootCount, QStringList FileList);

   // EventHandlerThreadController(Global::gSourceType TheHeartBeatSource);
    virtual ~EventHandlerThreadController();
    virtual void CreateAndInitializeObjects();
    virtual void CleanupAndDestroyObjects();

    /****************************************************************************/
    /**
     * \brief Function to Log Event.
     *
     * \iparam   &TheEventEntry
     */
    /****************************************************************************/
    inline void LogEvent(const DataLogging::DayEventEntry &TheEventEntry)
    {
        LogEventEntry(TheEventEntry);
    }

    void UpdateEventDataStructures(quint32 EventID,
                                   quint64 EventId64,
                                   const DataLogging::DayEventEntry &EventEntry,
                                   bool Remove,
                                   bool AckByGUI = false) ;

    /****************************************************************************/
    /**
      * \brief Send command to external process.
      *
      * Send a command to the external process. Reference mapping does not
      * have to be done since the acknowledge we receive from the external
      * process corresponds with Ref.
      *
      * \iparam   Ref                 Command reference.
      * \iparam   Cmd                 Command to send.
      */
     /****************************************************************************/
    void SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd);
    /****************************************************************************/
    /**
     * \brief Gets new Command Reference.
     *
     * \return  Reference type
     */
    /****************************************************************************/
    Global::tRefType GetNewCommandRef();
    /****************************************************************************/
    /**
     * \brief Gets EventKey count map.
     *
     * \return  m_EventKeyCountMap
     */
    /****************************************************************************/

    inline QHash<quint32, qint8> GetEventKeyCountMap()
    {
        return m_EventKeyCountMap;
    }
    /****************************************************************************/
    /**
     * \brief Gets count for Event id.
     *
     * \iparam  EventKey
     *
     * \return  EventKey
     */
    /****************************************************************************/
    inline qint8 GetCountForEventId(quint32 EventKey) {
        return m_EventKeyCountMap[EventKey];
    }
    /****************************************************************************/
    /**
     * \brief sets count for Event id.
     *
     * \iparam  EventKey
     * \iparam  count
     */
    /****************************************************************************/
    inline void SetCountForEventId(quint32 EventKey, qint8 count) {
        if (count > 0) {
            m_EventKeyCountMap.insert(EventKey, count);
        }
        else {
            m_EventKeyCountMap.remove(EventKey);
        }
    }
    /****************************************************************************/
    /**
     * \brief Gets Event Reference map.
     *
     * \return  m_EventKeyRefMap
     */
    /****************************************************************************/
    inline QHash<Global::tRefType, quint32>GetEventRefMap()
    {
        return m_EventKeyRefMap;
    }
    /****************************************************************************/
    /**
     * \brief sets Eventkey Reference map.
     *
     * \iparam  Ref
     * \iparam  EventKey
     */
    /****************************************************************************/
    inline void SetEventKeyRefMap(Global::tRefType & Ref, quint32 EventKey){
        m_EventKeyRefMap.insert(Ref, EventKey);
    }
    /****************************************************************************/
    /**
     * \brief Updates Eventkey Reference map.
     *
     * \iparam  Ref
     * \iparam  EventKey
     */
    /****************************************************************************/
    inline void UpdateEventKeyRefMap(quint32  EventKey, Global::tRefType  Ref) {
        Q_UNUSED(EventKey)
        Q_UNUSED(Ref)
    }
    /****************************************************************************/
    /**
     * \brief Function to connect to EventObject
     *
     */
    /****************************************************************************/
    virtual inline void ConnectToEventObject() {
        Global::EventObject *p_EventObject  = &Global::EventObject::Instance();
        CONNECTSIGNALSLOT(p_EventObject, ForwardEvent(const quint32, const Global::tTranslatableStringList &, const bool, const quint32, const Global::AlternateEventStringUsage),
                          this, ProcessEvent(const quint32, const Global::tTranslatableStringList &, const bool, const quint32, const Global::AlternateEventStringUsage));
    }


protected:

    virtual void OnGoReceived();
    virtual void OnStopReceived();
    virtual void OnPowerFail(const Global::PowerFailStages PowerFailStage);

    /****************************************************************************/
    /**
     * Handler for acknowledged.
     *
     * \iparam   ref -- unique command reference.
     * \iparam   ack -- received acknowledge instance.
     */
    /****************************************************************************/
    virtual void OnAcknowledge(Global::tRefType ref, const NetCommands::CmdAcknEventReport &ack);

    /****************************************************************************/
    /**
     * Handler for ack and NACK.
     *
     * \iparam   Ref -- unique command reference.
     * \iparam   Ack -- received acknowledge/ NACK instance.
     */
    /****************************************************************************/
    void OnAckOKNOK(Global::tRefType Ref, const Global::AckOKNOK &Ack);

	/**
	* \brief inform alarm handler
	* \iparam EventEntry event entry
	* \iparam EventId64 event id
	* \iparam StartAlarm flag to start alarm
	*/
	void InformAlarmHandler(const DataLogging::DayEventEntry &EventEntry, const quint64 EventId64, bool StartAlarm);

	/**
	* \brief inform alarm handler
	* \iparam EType event type
	* \iparam EventId64 event id
	* \iparam StartAlarm flag to start alarm
	*/
    void InformAlarmHandler(Global::EventType EType, const quint64 EventId64, bool StartAlarm);
	/**
	* \brief reset alarm, turn off all alarms
	*/
    void ResetAlarm();

private:
    /****************************************************************************/
    /**
      *\brief Structure to store events which cannot be processed yet (e.g. because m_eventList has not been created)
      */
     /****************************************************************************/
    struct PendingEvent
    {
        quint32 EventID;    ///< Pending Events Event ID.
        Global::tTranslatableStringList EventStringList; ///< Eventstringlist
        bool EventStatus; ///< Status of Pending Events.
        quint32 EventKey; ///< Pending Events Event Key
        Global::AlternateEventStringUsage AltStringUsuage;///< Alternate strings
    };

    QString                                     m_OperatingMode;                    ///< Operating mode.
    QString                                     m_EventLoggerBaseFileName;          ///< Base for file name for event logging.
    QString                                     m_SerialNumber;                     ///< Serial number.
    qint64                                      m_EventLoggerMaxFileSize;           ///< Max file size for event logger.
    int                                         m_DayEventLoggerMaxFileCount;   ///< Max number of files for day operation logger.
    int                                         m_MaxAdjustedTimeOffset;            ///< Max alowed offset to system time [seconds]. 0 means no check has to be done.

    bool                                        m_GuiAvailable; //!< Gui availability status
    qint8                                       m_RebootCount; //!< Number of times the main s/w has rebooted.
    QHash<quint32, EventHandler::EventCSVInfo> m_eventList;  //!< Hash of Eventlist
    QHash<quint32,quint32>m_EventKeyIdMap; //!< Hash of Event Key and EventID as value.
    QHash<quint32, quint32>m_EventIDKeyHash; //!< Hash of EventID as value


    QHash<QString, Global::EventSourceType>m_EventSourceMap;//!< Hash of Event Source Map
    QHash<quint64,DataLogging::DayEventEntry>m_EventKeyDataMap; //!< Hash of Event Key data
    QHash<quint32, qint8> m_EventKeyCountMap; //!< Hash of Event Key Count
    QVector<PendingEvent> m_pendingEvents; //!< Vector of Pending Events
    QHash<Global::tRefType, quint32> m_EventKeyRefMap;//!< Hash of Event Key Reference Map
    QVector<NetCommands::EventReportDataStruct> mPendingGuiEventList;//!< Vector of Pending Gui Event list
    QList<quint32> m_EventIDCount;  //!< Keeps track of event occurence i.e.,the no. of times an event has occured.
    QList<quint64> m_EventIdKeyCombinedList; //!< List og Event Id and Key list.
    QList<quint32> m_activeEvents;      //!< List of active Event Ids

    QHash<QString, Global::EventType> m_EventTypeEnumMap;//!< Hash of Event Type enum Map
    QHash<QString, Global::EventLogLevel> m_EventLogLevelEnumMap;//!< Hash of Event Log Level enum map
    QHash<QString, Global::ActionType>m_ActionTypeEnumMap; //!< Hash of Action Type map.
    Global::AlarmHandler *mp_AlarmHandler; //!< Pointer to Alarm Handler
    QList<DataLogging::DayEventEntry> m_DayEventEntryList; //!< Store list of event entries
    QHash<quint32, quint8> m_improperEvents;            //!< Store list of improper event entries

	
	/**
	* \brief Event Handler Thread Controller
	*/
    EventHandlerThreadController();  ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(EventHandlerThreadController)


	/**
	* \brief add action type
	*/
    void AddActionTypes();
	/**
	* \brief add event type
	*/
    void AddEventTypes();
	/**
	* \brief add event log level
	*/
    void AddEventLogLevels();
	/**
	* \brief add source components
	*/
    void AddSourceComponents();
	/**
	* \brief add action type
	* \iparam EventEntry event entry
	* \iparam EventId64 event id
	*/
    void HandleInactiveEvent(DataLogging::DayEventEntry &EventEntry, quint64 &EventId64);

	/**
	* \brief create event entry
	* \iparam EventEntry event entry
	* \iparam EventStatus event status
	* \iparam EventID event id
	* \iparam EventKey event key
	* \iparam AltStringUsage alt string usage
	*/
    void CreateEventEntry(DataLogging::DayEventEntry &EventEntry,
                          EventCSVInfo &EventInfo,
                          const bool EventStatus,
                          const quint32 EventID,
                          const Global::tTranslatableStringList &EventStringList,
                          const quint32 EventKey, const Global::AlternateEventStringUsage AltStringUsage = Global::NOT_APPLICABLE);


	/**
	* \brief  set statemachine
	* \iparam TheEvent event
	*/
    void SetSystemStateMachine(const DataLogging::DayEventEntry &TheEvent);

	/**
	* \brief  SetGuiAvailable
	* \iparam active active flag
	*/
    void SetGuiAvailable(const bool active);

	/**
	* \brief  inform GUI
	* \iparam TheEvent event
	* \iparam EventId64 event id
	*/
    void InformGUI(const DataLogging::DayEventEntry &TheEvent, const quint64 EventId64);

	/**
	* \brief  log entry
	* \iparam EventEntry event
	*/
    void LogEntry(const DataLogging::DayEventEntry &EventEntry);

	/**
	* \brief  HandleImproperEvents
	* \iparam eventId event id 
	* \iparam column column
	*/
    void HandleImproperEvents(quint32 eventId, bool success, quint8 column);

    /****************************************************************************/
    /**
     * \brief Process error event from local object.
     *
     * See detailed description in the base class' header.
     *
     * \iparam   TheDayEventEntry = event from one of local objects
     * \return   true if event was processed.
     */
    /****************************************************************************/
    bool LocalProcessErrorEvent(const DataLogging::DayEventEntry & TheDayEventEntry);

	/**
	* \brief  ReadConfigFile
	* \iparam filename file
	* \return bool
	*/
    bool ReadConfigFile(QString filename);

	/**
	* \brief  VerifyEventConfigCSV
	* \iparam filename file
	* \return bool
	*/
    bool VerifyEventConfigCSV(QString filename);

	/**
	* \brief  VerifyEventIDs
	* \iparam EventId  event id
	* \return bool
	*/
    bool VerifyEventIDs(quint32 EventId);

	/**
	* \brief  VerifyEventMacro
	* \iparam EventMacroName  event id
	* \return bool
	*/
    bool VerifyEventMacro(QString EventMacroName);

	/**
	* \brief  VerifyEventType
	* \iparam EventType  event type
	* \return bool
	*/
    bool VerifyEventType(Global::EventType EventType);

	/**
	* \brief  VerifyAction
	* \iparam ActionType  action type
	* \return bool
	*/
    bool VerifyAction(Global::ActionType ActionType);

	/**
	* \brief  VerifySource
	* \iparam EventSourceType  source type
	* \return bool
	*/
    bool VerifySource(Global::EventSourceType);

	/**
	* \brief  VerifyLogLevel
	* \iparam EventLogLevel  log leval
	* \return bool
	*/
    bool VerifyLogLevel(Global::EventLogLevel EventLogLevel);

	/**
	* \brief  VerifyBoolean
	* \iparam strValue  value
	* \return bool
	*/
    bool VerifyBoolean(QString strValue);

	/**
	* \brief  VerifyGuiButtonType
	* \iparam GuiButtonType  button type
	* \return bool
	*/
    bool VerifyGuiButtonType(Global::GuiButtonType GuiButtonType);

	/**
	* \brief  VerifyStringList
	* \iparam EventString  event
	* \return bool
	*/
    bool VerifyStringList(QString EventString);

	/**
	* \brief  VerifyAlarmEventTypeDependency
	* \iparam EventInfo  event info
	* \return bool
	*/
    bool VerifyAlarmEventTypeDependency(EventHandler::EventCSVInfo EventInfo);

	/**
	* \brief  RegisterCommands
	* \return bool
	*/
    void RegisterCommands();

	/**
	* \brief  VerifyAlarmEventTypeDependency
	* \iparam filename  file
	* \return bool
	*/
    bool VerifyEventCSVFilenameExists(QString filename);

	/**
	* \brief  VerifyAlarmGUIOptionsDependency
	* \iparam EventInfo  event info
	* \return bool
	*/
    bool VerifyAlarmGUIOptionsDependency(EventHandler::EventCSVInfo EventInfo);

	/**
	* \brief  VerifyStatusbarGUIOptionDependency
	* \iparam EventInfo  event info
	* \return bool
	*/
    bool VerifyStatusbarGUIOptionDependency( EventHandler::EventCSVInfo EventInfo);

	/**
	* \brief  VerifySourceComponentGUIOptionsDependency
	* \iparam EventInfo  event info
	* \return bool
	*/
    bool VerifySourceComponentGUIOptionsDependency(EventHandler::EventCSVInfo EventInfo);

	/**
	* \brief  VerifyActionGUIOptionsDependency
	* \iparam EventInfo  event info
	* \return bool
	*/
    bool VerifyActionGUIOptionsDependency(EventHandler::EventCSVInfo EventInfo);

	/**
	* \brief  VerifyUserLogGUIOptionDependency
	* \iparam EventCSVInfo  event csv info
	* \return bool
	*/
    bool VerifyUserLogGUIOptionDependency( EventHandler::EventCSVInfo EventCSVInfo );

public slots:

	/**
	* \brief  ProcessEvent
	* \iparam EventID  event id
	* \iparam EventStringList  event list
	* \iparam EventStatus  event status
	* \iparam EventKey  event key
	* \iparam AltStringUsuage  alt string usage
	*/
    virtual void ProcessEvent(const quint32 EventID,
                      const Global::tTranslatableStringList &EventStringList,
                      const bool EventStatus, const quint32 EventKey, const Global::AlternateEventStringUsage AltStringUsuage);


    /****************************************************************************/
    /*!
     *  \brief    This SLOT handles all incoming events.
     *
     *      This SLOT shall be used to process all events and their current
     *      status.
     *
     *  \iparam    EventKey
     *  \iparam    Event
     *  \iparam    Active
     *  \iparam    ActionResult
     *  \iparam    EventStringParList
     *  \iparam    EventRDStringParList
     *
     *
     ****************************************************************************/
    virtual void ProcessEvent(const quint32 EventKey, const quint32 Event,
                              const bool Active, const bool ActionResult,
                              const Global::tTranslatableStringList &EventStringParList,
                              const Global::tTranslatableStringList &EventRDStringParList);
    /****************************************************************************/
    /**
     * \brief Set operating mode string.
     *
     * \iparam   OperatingMode   the operating mode
     */
    /****************************************************************************/
    inline void SetOperatingMode(const QString &OperatingMode) {
        m_OperatingMode = OperatingMode;
    }
    /****************************************************************************/
    /**
     * \brief Set base of file name for even logging.
     *
     * \iparam   EventLoggerBaseFileName     Base of file name for even logging.
     */
    /****************************************************************************/
    inline void SetEventLoggerBaseFileName(const QString &EventLoggerBaseFileName) {
        m_EventLoggerBaseFileName = EventLoggerBaseFileName;
    }
    /****************************************************************************/
    /**
     * \brief Sets the SerialNumber.
     *
     * \iparam   &SerialNumber
     */
    /****************************************************************************/
    inline void SetSerialNumber(const QString &SerialNumber) {
        m_SerialNumber = SerialNumber;
    }

signals:
    /****************************************************************************/
    /**
     * \brief Signal for Log Event Entry
     *
     * \iparam   &TheDayOperationEntry
     */
    /****************************************************************************/
    void LogEventEntry(const DataLogging::DayEventEntry &TheDayOperationEntry);
    /****************************************************************************/
    /**
     * \brief Signal for forwarding to Event Handler
     *
     * \iparam   &TheDayOperationEntry
     * \iparam   EventKey
     */
    /****************************************************************************/
    void ForwardToErrorHandler(const DataLogging::DayEventEntry &TheDayOperationEntry, const quint64 EventKey);
    /****************************************************************************/
    /**
     * \brief Signal to check Gui availability
     *
     * \iparam   active
     */
    /****************************************************************************/
    void GuiAvailability(bool active);
    /****************************************************************************/
    /**
     * \brief Signal for forwarding to Remote Care
     *
     * \iparam   &TheEvent
     * \iparam   EventId64
     */
    /****************************************************************************/
    void SendEventToRemoteCare(const DataLogging::DayEventEntry &TheEvent, const quint64 EventId64);
};

} // end namespace EventHandler

#endif // EVENTHANDLER_EVENTHANDLERTHREADCONTROLLER_H
