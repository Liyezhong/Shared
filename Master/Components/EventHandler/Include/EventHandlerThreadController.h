/****************************************************************************/
/*! \file EventHandlerThreadController.h
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
#include <NetCommands/Include/CmdChangeUserLevel.h>

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

    EventHandlerThreadController(Global::gSourceType TheHeartBeatSource);

   // EventHandlerThreadController(Global::gSourceType TheHeartBeatSource);
    virtual ~EventHandlerThreadController();
    virtual void CreateAndInitializeObjects();
    virtual void CleanupAndDestroyObjects();
    inline void SetAlarmHandler( Global::AlarmHandler *pAlarmHandler) {//, DataManager::CUserSettings *pUserSettings){
        mpAlarmHandler = pAlarmHandler;      
    }

    Global::AlarmHandler * GetAlarmSettings()
    {
        if(mpAlarmHandler)
            return mpAlarmHandler;
        else
            return NULL;
    }

    DataManager::CUserSettings * GetUserSettings()
    {
        if(mpUserSettings)
            return mpUserSettings;
        else
            return NULL;
    }

    inline void LogEvent(const DataLogging::DayEventEntry &TheEventEntry)
    {
        LogEventEntry(TheEventEntry);
    }

    void UpdateEventDataStructures(quint32 EventID,
                                   quint64 EventId64,
                                   const DataLogging::DayEventEntry &EventEntry,
                                   bool Remove,
                                   bool AckByGUI = false,
                                   bool ResponseRes = false) ;

    /****************************************************************************/
    /**
      * \brief Send command to external process.
      *
      * Send a command to the external process. Reference mapping does not
      * have to be done since the acknowledge we receive from the external
      * process corresponds with Ref.
      *
      * \param[in]   Ref                 Command reference.
      * \param[in]   Cmd                 Command to send.
      */
     /****************************************************************************/
    void SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd);

    Global::tRefType GetNewCommandRef();


//    inline QHash<Global::tRefType, quint32>GetEventRefMap()
//    {
//        return m_EventKeyRefMap;
//    }

//    inline void SetEventKeyRefMap(Global::tRefType & Ref, quint32 EventKey){
//        m_EventKeyRefMap.insert(Ref, EventKey);
//    }

    inline void UpdateEventKeyRefMap(quint32  EventKey, Global::tRefType  Ref) {
        Q_UNUSED(EventKey)
        Q_UNUSED(Ref)
    }

    inline void ConnectToEventObject() {
        Global::EventObject *p_EventObject  = &Global::EventObject::Instance();
        CONNECTSIGNALSLOT(p_EventObject, ForwardEvent(const quint32, const Global::tTranslatableStringList &, const bool, const quint32, const Global::AlternateEventStringUsage, Global::tTranslatableStringList,quint64),
                          this, ProcessEvent(const quint32, const Global::tTranslatableStringList &, const bool, const quint32, const Global::AlternateEventStringUsage, Global::tTranslatableStringList, quint64));
    }


protected:

    void OnGoReceived();
    void OnStopReceived();
    virtual void OnPowerFail();

    /****************************************************************************/
    /**
     * Handler for acknowledged.
     *
     * @iparam   ref -- unique command reference.
     * @iparam   ack -- received acknowledge instance.
     */
    /****************************************************************************/
    void OnAcknowledge(Global::tRefType ref, const NetCommands::CmdAcknEventReport &ack);
    void OnUserRoleChanged(Global::tRefType ref, const NetCommands::CmdChangeUserLevel& cmd);


private:

    /**
      *\brief Structure to store events which cannot be processed yet (e.g. because m_eventList has not been created)
      */
    struct PendingEvent
    {
        quint32 EventID;
        Global::tTranslatableStringList EventStringList;
        quint32 EventKey;
        Global::AlternateEventStringUsage AltStringUsuage;
        Global::tTranslatableStringList EventStringListForRD;
        quint64 EventCodeScenario;
        bool IsResolved;
    };

    struct ProcessingEvent
    {
        quint32 EventCode;  ///< or Event code
        quint32 EventKey;
        Global::EventStatus CurrentStatus;
        bool CurrentActionResult;
    };
    typedef  QList<ProcessingEvent> ProcessingEventList;

    QString                                     m_OperatingMode;                    ///< Operating mode.
    QString                                     m_EventLoggerBaseFileName;          ///< Base for file name for event logging.
    QString                                     m_SerialNumber;                     ///< Serial number.
    qint64                                      m_EventLoggerMaxFileSize;           ///< Max file size for event logger.
    int                                         m_DayEventLoggerMaxFileCount;   ///< Max number of files for day operation logger.
    int                                         m_MaxAdjustedTimeOffset;            ///< Max alowed offset to system time [seconds]. 0 means no check has to be done.

    bool                                        m_GuiAvailable;
    QHash<quint32, EventHandler::EventCSVInfo> m_eventList;
//    QHash<quint32,quint32>m_EventKeyIdMap; //!< Hash of Event Key and EventID as value.
//    QHash<quint32, quint32>m_EventIDKeyHash;

    QMap<Global::EventSourceType, ProcessingEventList> m_ProcessingEvents;


    QHash<QString, Global::EventSourceType>m_EventSourceMap;
//    QHash<quint64,DataLogging::DayEventEntry>m_EventKeyDataMap;
    QVector<PendingEvent> m_pendingEvents;
//    QHash<Global::tRefType, quint32> m_EventKeyRefMap;
    QHash<Global::tRefType,DataLogging::DayEventEntry> m_EventRefDataMap;
    QVector<NetCommands::EventReportDataStruct> mPendingGuiEventList;
//    QList<quint32> m_EventIDCount;  //!< Keeps track of event occurence i.e.,the no. of times an event has occured.
    QList<quint64> m_EventIdKeyCombinedList;

    QHash<QString, Global::LogAuthorityType> m_LogAuthorityTypeEnumMap;
    QHash<QString, Global::AlarmPosType> m_AlarmPosTypeEnumMap;
    QHash<QString, Global::ResponseRecoveryType> m_ResponseRecoveryTypeEnumMap;
    QHash<QString, Global::EventType> m_EventTypeEnumMap;
    QHash<QString, Global::EventLogLevel> m_EventLogLevelEnumMap;
    QHash<QString, Global::ActionType>m_ActionTypeEnumMap;
    EventHandlerThreadController();                                          ///< Not implemented.
    void AddActionTypes();
    void AddEventTypes();
    void AddEventLogLevels();
    void AddSourceComponents();
    void AddAlarmPosTypes();
    void AddLogAuthorityTypes();
    void AddResponseRecoveryTypes();
    void HandleInactiveEvent(DataLogging::DayEventEntry &EventEntry, quint64 &EventId64);
    void CreateEventEntry(DataLogging::DayEventEntry &EventEntry,
                          EventCSVInfo &EventInfo,
                          const bool EventStatus,
                          const quint32 EventID,
                          const Global::tTranslatableStringList &EventStringList,
                          Global::tTranslatableStringList &EventStringListForRd,
                          quint64 EventCodeScenario,
                          const quint32 EventKey, const Global::AlternateEventStringUsage AltStringUsage = Global::NOT_APPLICABLE);
    void InformAlarmHandler(const DataLogging::DayEventEntry &EventEntry, bool StartAlarm);
    void SetSystemStateMachine(const DataLogging::DayEventEntry &TheEvent);
    void SetGuiAvailable(const bool active);
    void InformUser(const DataLogging::DayEventEntry &TheEvent, bool RaiseBox = true, bool AckByGui = false);

    // copy constructor & "=" operator
    EventHandlerThreadController(const EventHandlerThreadController &);                      ///< Not implemented.
    const EventHandlerThreadController & operator = (const EventHandlerThreadController &);  ///< Not implemented.

    Global::AlarmHandler *mpAlarmHandler;

    DataManager::CUserSettings *mpUserSettings;
    Global::GuiUserLevel        m_UserRole;


    /****************************************************************************/
    /**
     * \brief Process error event from local object.
     *
     * See detailed description in the base class' header.
     *
     * \param[in]  TheEventEntry = event from one of local objects
     * \return      true if event was processed.
     */
    /****************************************************************************/
    bool LocalProcessErrorEvent(const DataLogging::DayEventEntry & TheDayEventEntry);

    /****************************************************************************/
    /**
     * \brief Set system ErrorHandler's parent and connect it.
     *
     * System's ErrorHandler shall run in the EventHandler context. It is done
     * by setting EventHandler as ErrorHandler's parent. EventHandler will be
     * moved to its thread and its children will be also automatically moved to
     * this thread.
     *
     * Workflow:
     *          create and initialize EventHandler;
     *          create and initialize ErrorHandler;
     *          attach ErrorHandler;
     *          move EventHandler to its Thread.
     *
     * \param[in]  pErrorHandler = pointer to the system's ErrorHandler object
     */
    /****************************************************************************/
    //void AttachErrorHandler(ErrorHandler *pErrorHandler);

    /**
      @brief Reads event definition from file
      */
    bool ReadConfigFile(QString fileName);

    bool VerifyEventConfigCSV(QString filename);

    bool VerifyEventIDs(quint32 EventId);

    bool VerifyEventMacro(QString EventMacroName);

    bool VerifyEventType(Global::EventType EventType);

    bool VerifyAction(Global::ActionType ActionType);

    bool VerifySource(Global::EventSourceType);

    bool VerifyLogLevel(Global::EventLogLevel EventLogLevel);

    bool VerifyBoolean(QString strValue);

    bool VerifyGuiButtonType(Global::GuiButtonType GuiButtonType);

    bool VerifyStringList(QString EventString);

    bool VerifyAlarmEventTypeDependency(EventHandler::EventCSVInfo EventInfo);

    void RegisterCommands();

    bool VerifyEventCSVFilenameExists(QString filename);

    bool VerifyAlarmGUIOptionsDependency(EventHandler::EventCSVInfo EventInfo);

    bool VerifyStatusbarGUIOptionDependency( EventHandler::EventCSVInfo EventInfo);

    bool VerifySourceComponentGUIOptionsDependency(EventHandler::EventCSVInfo EventInfo);

    bool VerifyActionGUIOptionsDependency(EventHandler::EventCSVInfo EventInfo);

    bool VerifyUserLogGUIOptionDependency( EventHandler::EventCSVInfo EventCSVInfo );


    /****************************************************************************/
    /**
     * \brief check and get event current status
     *
     * \param[in]   EventSource   the source of the event
     * \param[in]   EventCode   the EventCode of the event
     * \param[out]   EventStatus   current status of the event
     * \return      true - get EventStatus,  false - error during error
     */
    /****************************************************************************/
    bool HandlingError(const quint32 ErrorCode, const quint32 EventKey,const DataLogging::DayEventEntry& EventEntry);
    void DoResponseAction(const quint32 ErrorCode, const quint32 EventKey,const DataLogging::DayEventEntry& EventEntry);


public slots:

    void ProcessEvent(const quint32 ErrorCode,
                      const Global::tTranslatableStringList &EventStringList,
                      const bool IsResolved, const quint32 EventKeyRef,
                      const Global::AlternateEventStringUsage AltStringUsuage,
                      Global::tTranslatableStringList EventStringListForRD = Global::tTranslatableStringList(),
                      quint64 EventCodeScenario = 0);


    /****************************************************************************/
    /**
     * \brief Set operating mode string.
     *
     * \param[in]   OperatingMode   the operating mode
     */
    /****************************************************************************/
    inline void SetOperatingMode(const QString &OperatingMode) {
        m_OperatingMode = OperatingMode;
    }
    /****************************************************************************/
    /**
     * \brief Set base of file name for even logging.
     *
     * \param[in]   EventLoggerBaseFileName     Base of file name for even logging.
     */
    /****************************************************************************/
    inline void SetEventLoggerBaseFileName(const QString &EventLoggerBaseFileName) {
        m_EventLoggerBaseFileName = EventLoggerBaseFileName;
    }

    inline void SetSerialNumber(const QString &SerialNumber) {
        m_SerialNumber = SerialNumber;
    }


signals:
    void LogEventEntry(const DataLogging::DayEventEntry &TheDayOperationEntry);
    void ForwardToErrorHandler(const DataLogging::DayEventEntry &TheDayOperationEntry, const quint64 EventKey);
    void GuiAvailability(bool active);
    void GuiMsgHint(bool active);

};

} // end namespace EventHandler

#endif // EVENTHANDLER_EVENTHANDLERTHREADCONTROLLER_H
