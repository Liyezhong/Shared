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
#include <EventHandler/Include/ErrorHandler.h>
#include <Global/Include/AlarmHandler.h>
#include <Global/Include/EventTranslator.h>
#include <Global/Include/UITranslator.h>
#include <Global/Include/Commands/AckOKNOK.h>
#include <DataManager/Containers/UserSettings/Include/UserSettingsInterface.h>
#include <NetCommands/Include/CmdAcknEventReport.h>
#include <EventHandler/Include/ActionHandler.h>



namespace NetCommands {
    class CmdAcknEventReport;
}


namespace EventHandler
{
class ActionHandler;
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

    void UpdateEventKeyCountMap(quint32 EventKey, bool EventStatus) ;



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

    inline QHash<quint32, qint8> GetEventKeyCountMap()
    {
        return m_EventKeyCountMap;
    }

    inline qint8 GetCountForEventKey(quint32 EventKey) {
        return m_EventKeyCountMap[EventKey];
    }

    inline QHash<Global::tRefType, quint32>GetEventRefMap()
    {
        return m_EventKeyRefMap;
    }

    inline void SetEventKeyRefMap(Global::tRefType & Ref, quint32 EventKey){
        m_EventKeyRefMap.insert(Ref, EventKey);
    }

    inline void UpdateEventKeyRefMap(quint32  EventKey, Global::tRefType  Ref) {
    }

    inline void ConnectToEventObject() {
        Global::EventObject *p_EventObject  = &Global::EventObject::Instance();
        CONNECTSIGNALSLOT(p_EventObject, ForwardEvent(const quint32, const Global::tTranslatableStringList &, const bool, const quint32),
                          this, ProcessEvent(const quint32, const Global::tTranslatableStringList &, const bool, const quint32));
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


private:

    /**
      *\brief Structure to store events which cannot be processed yet (e.g. because m_eventList has not been created)
      */
    struct PendingEvent
    {
        quint32 EventID;
        Global::tTranslatableStringList EventStringList;
        bool EventStatus;
        quint32 EventKey;
    };

    QString                                     m_OperatingMode;                    ///< Operating mode.
    QString                                     m_EventLoggerBaseFileName;          ///< Base for file name for event logging.
    QString                                     m_SerialNumber;                     ///< Serial number.
    qint64                                      m_EventLoggerMaxFileSize;           ///< Max file size for event logger.
    int                                         m_DayEventLoggerMaxFileCount;   ///< Max number of files for day operation logger.
    int                                         m_MaxAdjustedTimeOffset;            ///< Max alowed offset to system time [seconds]. 0 means no check has to be done.

    QHash<quint32, EventHandler::EventCSVInfo> m_eventList;
    QHash<quint32,quint32>m_EventIdKeyMap;
    QHash<QString, Global::EventType> m_EventTypeEnumMap;
    QHash<QString, Global::EventLogLevel> m_EventLogLevelEnumMap;
    QHash<QString, Global::ActionType>m_ActionTypeEnumMap;
    QHash<Global::tRefType, quint32> m_EventKeyRefMap;
    QHash<QString, Global::EventSourceType>m_EventSourceMap;
    QHash<quint32,DataLogging::DayEventEntry>m_EventKeyDataMap;
    QHash<quint32, qint8> m_EventKeyCountMap;
    QVector<PendingEvent> m_pendingEvents;
    EventHandlerThreadController();                                          ///< Not implemented.
    void AddActionTypes();
    void AddEventTypes();
    void AddEventLogLevels();
    void AddSourceComponents();


    // copy constructor & "=" operator
    EventHandlerThreadController(const EventHandlerThreadController &);                      ///< Not implemented.
    const EventHandlerThreadController & operator = (const EventHandlerThreadController &);  ///< Not implemented.

    Global::AlarmHandler *mpAlarmHandler;
    EventHandler::ActionHandler * mpActionHandler;
    DataManager::CUserSettings *mpUserSettings;


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
    void AttachErrorHandler(ErrorHandler *pErrorHandler);

    /**
      @brief Reads event definition from file
      */
    void ReadConfigFile(QString fileName);

    void RegisterCommands();

public slots:


    void ProcessEvent(const quint32 EventID, const Global::tTranslatableStringList &EventStringList, const bool EventStatus, const quint32 EventKey);


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
    void ForwardToErrorHandler(const DataLogging::DayEventEntry &TheDayOperationEntry, const quint32 EventKey);
    void GuiAvailability(bool active);
};

} // end namespace EventHandler

#endif // EVENTHANDLER_EVENTHANDLERTHREADCONTROLLER_H
