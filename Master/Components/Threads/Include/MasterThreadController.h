/****************************************************************************/
/*! \file Threads/Include/MasterThreadController.h
 *
 *  \brief Definition file for class MasterThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
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

#ifndef THREADS_MASTERTHREADCONTROLLER_H
#define THREADS_MASTERTHREADCONTROLLER_H

#include <Threads/Include/BaseThreadController.h>
#include <Threads/Include/CommandFunctors.h>
#include <DataLogging/Include/DataLoggingThreadController.h>
#include <EventHandler/Include/ErrorHandler.h>
#include <EventHandler/Include/RemoteCareHandler.h>
#include <SoftSwitchManager/Include/SoftSwitchManagerThreadController.h>
#include <Global/Include/Commands/CmdSoftSwitchPressed.h>
#include <Global/Include/EventObject.h>
#include <Global/Include/AlarmHandler.h>
#include <QStack>
#include <QPair>
#include <QSet>

namespace DataManager {
    class CDataManagerBase;
    class CUserSettings;
}

namespace EventHandler {
    class EventHandlerThreadController;
    class ActionHandler;
}
namespace Threads {

typedef enum {
    UNDEFINED_BASIC_THREAD,
    DATA_LOGGING_THREAD,
    EVENT_HANDLER_THREAD,
    SOFT_SWTICH_MANAGER_THREAD
} BasicThreads_t;

typedef enum {
    UNDEFINED_PLATFORM_THREAD = 100,
    AXEDA_CONTROLLER_THREAD = 101
} PlatformThreads_t;

typedef Global::SharedPointer<CommandExecuteFunctorAck>     CommandExecuteFunctorAckShPtr_t;    ///< Typedef or a shared pointer of CommandExecuteFunctor.
typedef QHash<QString, CommandExecuteFunctorAckShPtr_t>     CommandExecuteFunctorAckHash_t;     ///< Typedef for the CommandExecuteFunctorShPtr_t functor hash.

typedef QPair<ThreadController *, QThread *>        tControllerPair;            ///< Typedef for a pair consisting of a thread controller and a thread. Both as pointers.
typedef QMap<int, tControllerPair>                  tControllerMap;             ///< Map of tControllerPair and controller number
typedef QHash<QString, CommandChannel *>            tTCCommandChannelHash;      ///< Typedef for the TCCommandRouteFunctor functor hash.
typedef QPair<Global::tRefType, CommandChannel *>   tRefChannelPair;            ///< Typedef for pair of tRefType and CommandChannel *
typedef QHash<Global::tRefType, tRefChannelPair >   tTCAckChannelHash;          ///< Typedef for the TCCommandRouteFunctor functor hash.
typedef QVector<CommandChannel *>                   tCommandChannelVector;      ///< Typedef for command channel vector.

/****************************************************************************/
/**
 * \brief This class is the base class for master threads.
 *
 * It will handle the data logging and event handling threads and their controllers.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class MasterThreadController : public BaseThreadController {

    Q_OBJECT
private:
    QString                                     m_OperatingMode;                    ///< Operating mode.
    QString                                     m_EventLoggerBaseFileName;          ///< Base for file name for event logging.
    QString                                     m_SerialNumber;                     ///< Serial number.
    qint64                                      m_EventLoggerMaxFileSize;           ///< Max file size for event logger.
    int                                         m_DayEventLoggerMaxFileCount;       ///< Max number of files for day operation logger.
    int                                         m_MaxAdjustedTimeOffset;            ///< Max alowed offset to system time [seconds]. 0 means no check has to be done.
    Global::gSourceType                         m_HeartBeatSourceDataLogging;        ///< Heart Beat Source ID of the DataLogging component.
    DataLogging::DataLoggingThreadController    *mp_DataLoggingThreadController;    ///< Pointer to the DataLoggingComponent
    Global::gSourceType                         m_HeartBeatSourceEventHandler;      ///< Heart Beat Source ID of the Event Handler.

    // command executing stuff
    CommandExecuteFunctorAckHash_t              m_CommandExecuteFunctors;           ///< Functors of supported commands.
    CommandExecuteFunctorHash_t                 m_CommandExecuteWithoutAckFunctors; ///< Functors of commands without Ack.
    // command routing stuff
    tTCCommandChannelHash                       m_TCCommandRoutes;                  ///< Supported routing commands.
    tTCAckChannelHash                           m_TCAcknowledgeRoutes;              ///< Acknowledge routing.
    // command broadcasting stuff
    tCommandChannelVector                       m_BroadcastChannels;                ///< Vector of channels for broadcasting commands.
    // heartbeat check stuff
    int                                         m_ControllerHeartbeatTimeout;       ///< Timeout for controller heartbeat. Default = 1000 ms. 0 means no heartbeat signal is send!
    int                                         m_HeartbeatCheckTimeout;            ///< Timeout for checking controller heartbeat. Default = 3000 ms. 0 means no heartbeat signal check is done!
    QTimer                                      m_HeartbeatCheckTimer;              ///< Timer for controller heartbeat check.
    QSet<Global::gSourceType>                   m_HeartbeatSources;                 ///< All registered logging sources for heartbeat check.
    QSet<Global::gSourceType>                   m_ArrivedHeartbeats;                ///< All logging sources for which we received a heartbeat.
    QString                                     m_ShutdownSharedMemName;            ///< Name of shared memory for inducing a system shutdown. Will work only in debug mode!
    QTimer                                      m_ShutdownSharedMemTimer;           ///< Timer for checking external request for system shutdown. Will work only in debug mode!
    SoftSwitchManager::SoftSwitchManagerThreadController    *mp_SoftSwitchManagerThreadController; //!< Thread controller to monitor softswitch
    CommandChannel                              m_CommandChannelSoftSwitch;         //!< Command channel for SoftSwitch Manager
    Global::gSourceType                         m_HeartBeatSourceSoftSwitch;        //!< Heart beat source of softswitch
    tControllerMap                              m_BasicControllersMap;              //!< Basic thread controllers; Key- Controller number, value- tControllerPair
    Threads::CommandChannel                     m_CommandChannelAxeda;                  ///< Command channel for Axeda thread controller.
    Global::gSourceType                         m_HeartBeatSourceAxeda;             //!< Heart Beat source of Axeda Thread Controller

    /****************************************************************************/
    MasterThreadController();                                                       ///< Not implemented.
    MasterThreadController(const MasterThreadController &);                         ///< Not implemented.
    const MasterThreadController & operator = (const MasterThreadController &);     ///< Not implemented.

    /****************************************************************************/
    /**
     * \brief Cleanup all controllers in the reverse order they were initialized.
     *
     * Calls \ref CleanupAndDestroyObjects for each controller.
     */
    /****************************************************************************/
    void CleanupControllers(bool BasicThreadController = false);

    /****************************************************************************/
    /**
     * \brief Wait for thread termination.
     *
     * We wait for a specific time amount THREAD_WAIT_TIME. If the thread does not
     * terminate we throw an exception
     */
    /****************************************************************************/
    void WaitForThreads(bool BasicThreadController = false);
    /****************************************************************************/
    /**
     * \brief Connect data logging signals.
     *
     * We connect the data logging signals (see \ref EventObject) of a controller
     * to the event handler controller (\ref EmitEventEntry) and to the
     * data logger controller (\ref EmitDayOperationEntry and \ref EmitComponentTestEntry)
     *
     * \param[in]   pController     Pointer to controller.
     */
    /****************************************************************************/
    void ConnectDataLoggingSignals(const BaseThreadController *pController);
    /****************************************************************************/
    /**
     * \brief Get command channel for routing by name.
     *
     * Returns NULL if functor not found.
     * \param[in]   CommandName     Name of command.
     * \return                      The command channels.
     */
    /****************************************************************************/
    CommandChannel *GetCommandRouteChannel(const QString &CommandName) const;
    /****************************************************************************/
    /**
     * \brief Register commands.
     *
     * Must be implemented in derived classes.
     */
    /****************************************************************************/
    virtual void RegisterCommands();
    /****************************************************************************/
    /**
     * \brief Data in some data container has changed.
     *
     * A positive acknowledge is send, the command is dispatched to the responsible
     * processing function and a copy of the command is broadcasted to all
     * attached thread controllers.
     * Additional documentation in base class.
     *
     * \param[in]   Ref                 Command reference.
     * \param[in]   Cmd                 Command.
     * \param[in]   AckCommandChannel   Command channel for acknowledges.
     */
    /****************************************************************************/
    virtual void CmdDataChangedReceived(Global::tRefType Ref, const Global::CmdDataChanged &Cmd, CommandChannel &AckCommandChannel);
    /****************************************************************************/
    /**
     * \brief A CmdPowerFail command was received.
     *
     * A copy of the command is broadcasted to all attached thread controllers and
     * the command is dispatched to the responsible processing function
     * Additional documentation in base class.
     *
     * \param[in]   Ref                 Command reference.
     * \param[in]   Cmd                 Command.
     * \param[in]   AckCommandChannel   Command channel for acknowledges.
     */
    /****************************************************************************/
    virtual void CmdPowerFailReceived(Global::tRefType Ref, const Global::CmdPowerFail &Cmd, CommandChannel &AckCommandChannel);
    /****************************************************************************/
    /**
     * \brief Send a data changed command.
     *
     * The command will be broadcasted to all connected thread controllers.
     *
     * \param[in]   Cmd         The command.
     */
    /****************************************************************************/
    virtual void DoSendDataChanged(const Global::CommandShPtr_t &Cmd);
    /****************************************************************************/
    /**
     * \brief Start statemachine.
     *
     * Start own statemachine and start to work.
     * Must be implemented in derived classes.
     */
    /****************************************************************************/
    virtual void StartStatemachine() = 0;


    void OnSoftSwitchPressedAtStartup(Global::tRefType Ref, const Global::CmdSoftSwitchPressed &Cmd,
                                                              Threads::CommandChannel &AckCommandChannel);

signals:
    /****************************************************************************/
    /**
     * \brief Go signal for all connected threads.
     */
    /****************************************************************************/
    void SendGo();
    /****************************************************************************/
    /**
     * \brief Go signal for all connected Basic threads.
     */
    /****************************************************************************/
    void SendGoToBasicThreads();
    /****************************************************************************/
    /**
     * \brief Stop signal for all connected threads.
     */
    /****************************************************************************/
    void SendStop();
    /****************************************************************************/
    /**
     * \brief Emit this signal if you want to terminate yourself.
     *
     * It must be processed outside of this object. A good idea is to connect this
     * signal with this controller's thread quit() slot. This could be done in the
     * main function of the application.
     */
    /****************************************************************************/
    void RequestFinish();

    /****************************************************************************/
    /**
     * \brief Emit this signal if you want to create the export process.
     */
    /****************************************************************************/
    void CreateExportProcess(QString);

private slots:
    /****************************************************************************/
    /**
     * \brief Check if all controllers have send their heartbeat signals.
     */
    /****************************************************************************/
    void HeartbeatCheck();
    /****************************************************************************/
    /**
     * \brief Checks if an external request for system shutdown arrived.
     * \warning This slot is called only in debug mode!
     */
    /****************************************************************************/
    void ExternalMemShutdownCheck();

protected:
    tControllerMap                              m_ControllerMap;                    ///< Thread controller
    CommandChannel                              m_CommandChannelDataLogging;        ///< Command channel for DataLogging.
    CommandChannel                              m_CommandChannelEventThread;        ///< Command channel for EventHandler.
    EventHandler::EventHandlerThreadController  *mp_EventThreadController;          ///< Pointer to the system event handling object.
    QHash<QString, Threads::CommandChannel*>    m_channelList;                      ///< Hash of command channels connected related to its name
    Platform::AlarmHandler                      *mp_alarmHandler;                       ///< The Alarm handler
    DataManager::CUserSettings                  *mp_UserSettings;                        ///< The user settings.
    DataManager::CDataManagerBase               *mp_DataManagerBase; //!< The DataManager.\warning Dont delete this, Pointer points to address in stack.

    /****************************************************************************/
    /**
     * \brief Get serial number.
     *
     * \return Serial number.
     */
    /****************************************************************************/
    inline QString GetSerialNumber() const {
        return m_SerialNumber;
    }

    /****************************************************************************/
    /**
     * \brief Get the controllers count.
     *
     * \return controller count.
     */
    /****************************************************************************/
    inline int GetControllersCount() const {
        return m_ControllerMap.count();
    }

    inline int GetBasicControllersCount() const {
        return m_BasicControllersMap.count();
    }
    /****************************************************************************/
    /**
     * \brief Stop the specified controllers and threads.
     */
    /****************************************************************************/
    void StopSpecificThreadController(const int ControllerNumber, const bool BasicThreadController = false);

    /****************************************************************************/
    /**
     * \brief Remove controllers and threads from the list.
     */
    /****************************************************************************/
    void RemoveSpecificThreadController(const int ControllerNumber, const bool BasicThreadController = false);

    /****************************************************************************/
    /**
     * \brief This method is called when the base received the \ref Go signal.
     *
     * This means that everything is fine and normal operation started.
     * We are running in our own thread.\n
     * We create all controllers used (the according method in derived classes
     * \ref CreateControllersAndThreads is also called), then initialize the controllers
     * and finally attach them to the corresponding threads and start them.\n
     * If something goes wrong, the master thread (and application) will be stopped.
     */
    /****************************************************************************/
    virtual void OnGoReceived();
    /****************************************************************************/
    /**
     * \brief This method is called when the base class received the \ref Stop signal.
     *
     * This means that normal operation will stop after processing this signal.
     * We are still running in our own thread.\n
     * We make some cleanup work (all used controllers) and finally destroy all
     * controllers and threads.
     */
    /****************************************************************************/
    virtual void OnStopReceived();
    /****************************************************************************/
    /**
     * \brief Create controllers and threads handled by us.
     *
     * We create all controllers handled by us
     * and connect them.
     *
     * \note One can create thread controllers which are platform in nature over
     *        here
     */
    /****************************************************************************/
    virtual void CreateControllersAndThreads();

    /****************************************************************************/
    /**
     * \brief Create all controllers and threads handled by us
     *
     * We create all controllers handled by us (data logging and event handler)
     * and connect them.
     */
    /****************************************************************************/
    virtual void CreateBasicControllersAndThreads();

    /****************************************************************************/
    /**
     * \brief Starts the export controller thread
     *
     ****************************************************************************/
    void StartSpecificThreadController(const int ControllerNumber, const bool BasicThreadController = false);

    /****************************************************************************/
    /**
     * \brief Destroy all registered controllers and threads.
     */
    /****************************************************************************/
    void DestroyControllersAndThreads(const bool BasicThreadController = false);
    /****************************************************************************/
    /**
     * \brief Shutdown.
     *
     * Try to shutdown in a graceful way: call stop on all threads, terminate them
     * and wait for their termination. After that we call out own Stop method
     * and send a terminate request.
     * \warning  Other termination tasks should have been done already!
     */
    /****************************************************************************/
    void Shutdown();
    /****************************************************************************/
    /**
     * \brief Initiate the shutdown process.
     *
     * Initiate the shutdown process. Make some project specific tasks the call
     * \ref Shutdown to shut down software.
     * Must be implemented in derived classes.
     */
    /****************************************************************************/
    virtual void InitiateShutdown() = 0;
    /****************************************************************************/
    /**
     * \brief Add controller and thread to list of handled controllers.
     *
     * This list is used to perform common stuff to all controllers and threads:
     * connecting their \ref Go and \ref Stop slots, connecting them to the
     * data logging mechanism and so on.
     *
     * \param[in]   pController         Pointer to controller.
     * \param[in]   pCommandChannel     Pointer to command channel.
     */
    /****************************************************************************/
    void AddAndConnectController(ThreadController *pController, CommandChannel *pCommandChannel, int ControllerNumber, bool BasicThreadController = false);
    /****************************************************************************/
    /**
     * \brief Set system ErrorHandler's parent and connect it.
     *
     * System's ErrorHandler shall run in the EventHandler context. It is done
     * by setting EventHandler as ErrorHandler's parent. EventHandler will be
     * moved to its thread and its children will be also automatically moved to
     * this thread.
     *
     * WARNING! Call this function to attach ErrorHandler before
     *          moving EventHandler to its thread!
     * Workflow:
     *          create and initialize EventHandler;
     *          create and initialize ErrorHandler;
     *          attach ErrorHandler;
     *          move EventHandler to its Thread.
     *
     * \param[in]  pErrorHandler = pointer to the system's ErrorHandler object
     */
    /****************************************************************************/
    void AttachErrorHandler(EventHandler::ErrorHandler *pErrorHandler);
    /****************************************************************************/


    void AttachActionHandler(EventHandler::ActionHandler *pActionHandler);
    /**
     * \brief Set connection between EventHandler and RemoteCare Controller.
     *
     *    It is required to send all system Errors to Leica Remote Care
     *    Server. This function shall connect EventHandler's signal so
     *    the EventHandler can forward system errors to the local Remote
     *    Care Controller.
     *
     * WARNING! Call this function to be able to report errors to Remote
     *          Care Server!
     * Workflow:
     *          create and initialize EventHandler;
     *          create and initialize RemoteCareHandler's derived class object;
     *          set the connection;
     *
     * \param[in]  pRemoteCareHandler = pointer to the system's RemoteCare Controller object
     */
    /****************************************************************************/
    void SetRemoteCareConnection(const EventHandler::RemoteCareHandler *pRemoteCareHandler) const;
    /****************************************************************************/
    /**
     * \brief Read set of needed event string translations.
     *
     * These consist of the language resulting from current operation mode and the
     * fallback language.
     *
     * \param[in]   Language            The desired language.
     * \param[in]   FallbackLanguage    The desired fallback language.
     */
    /****************************************************************************/
    void ReadEventTranslations(QLocale::Language Language, QLocale::Language FallbackLanguage) const ;
    /****************************************************************************/
    /**
     * \brief Read set of needed UI string translations.
     *
     * \param[in]       UserLanguage        The desired user language.
     * \param[in]       FallbackLanguage    The desired fallback language.
     */
    /****************************************************************************/
    void ReadUITranslations(QLocale::Language UserLanguage, QLocale::Language FallbackLanguage) const;
    /****************************************************************************/
    /**
     * \brief Try to set new date and time offset.
     *
     * The offset from the current date time is compared to the maximal allowed
     * offset and then set.
     * \param[in]   NewDateTime     New date time.
     * \return                      True on success
     */
    /****************************************************************************/
    bool SetAdjustedDateTimeOffset(const QDateTime &NewDateTime);
    /****************************************************************************/
    /**
     * \brief Register a command execution functor.
     *
     * \param[in]   CommandName     Name of command.
     * \param[in]   Functor         Shared pointer of functor to register.
     */
    /****************************************************************************/
    void RegisterCommandExecuteFunctor(const QString &CommandName, const CommandExecuteFunctorAckShPtr_t &Functor);
    /****************************************************************************/
    /**
     * \brief Register a command execution functor without Ack.
     *
     * \param[in]   CommandName     Name of command.
     * \param[in]   Functor         Shared pointer of functor to register.
     */
    /****************************************************************************/
    void RegisterCommandExecuteFunctor(const QString &CommandName, const CommandExecuteFunctorShPtr_t &Functor);
    /****************************************************************************/
    /**
     * \brief Get command execute functor by name.
     *
     * Get command execute functor by name. If functor is not found
     * NullCommandExecuteFunctor will be returned.
     *
     * \param[in]   CommandName     Name of command.
     * \return                      The functor or NullCommandExecuteFunctor.
     */
    /****************************************************************************/
    CommandExecuteFunctorAckShPtr_t GetCommandExecuteFunctor(const QString &CommandName) const;
    /****************************************************************************/
    /**
     * \brief Get command execute functor by name.
     *
     * Get command execute functor by name. If functor is not found
     * NullCommandExecuteFunctor will be returned.
     *
     * \param[in]   CommandName     Name of command.
     * \return                      The functor or NullCommandExecuteFunctor.
     */
    /****************************************************************************/
    CommandExecuteFunctorShPtr_t GetCommandExecuteFunctorWithoutAck(const QString &CommandName) const;
    /****************************************************************************/
    /**
     * \brief Register a command route functor.
     *
     * \param[in]   CommandName             Name of command.
     * \param[in]   pTargetCommandChannel   The target command channel.
     */
    /****************************************************************************/
    void RegisterCommandRoutingChannel(const QString &CommandName, CommandChannel *pTargetCommandChannel);
    /****************************************************************************/
    /**
     * \brief Register a command for routing.
     *
     * \param[in]   pTargetCommandChannel   Pointer to command channel to which command has to be routed.
     */
    /****************************************************************************/
    template<class TCCmdClass>
    void RegisterCommandForRouting(CommandChannel *pTargetCommandChannel) {
        // register
        RegisterCommandRoutingChannel(TCCmdClass::NAME, pTargetCommandChannel);
    }
    /****************************************************************************/
    /**
     * \brief Register a command for processing.
     *
     * \param[in]   pCommandProcessor   Pointer to thread controller instance which processes the command.
     * \param[in]   FunctionPointer     Function which processes the command.
     * \param[in]   pTargetCommandChannel   Pointer to command channel to which command has to be routed.
     *
     */
    /****************************************************************************/
    template<class TCCmdClass, class CommandProcessorClass>
    void RegisterCmdForProcessingAndRouting(void(CommandProcessorClass::*FunctionPointer)(Global::tRefType, const TCCmdClass &),
                                            CommandProcessorClass *pCommandProcessor, CommandChannel *pTargetCommandChannel) {
        //Functor without Ack , Ack will be sent by the processor class in the target channel
        CommandExecuteFunctorShPtr_t Functor(new TemplateCommandExecuteFunctor<CommandProcessorClass, TCCmdClass>(pCommandProcessor, FunctionPointer));
        RegisterCommandExecuteFunctor(TCCmdClass::NAME, Functor);
        RegisterCommandRoutingChannel(TCCmdClass::NAME, pTargetCommandChannel);
    }
    /****************************************************************************/
    /**
     * \brief Thread command processing method.
     *
     * Is called when an command has to be processed.
     * \warning This method should be called only from within \ref CommandChannel::CommandChannelRx
     * \warning Do not let exceptions escape this method!
     *
     * \param[in]       Ref                 The command reference.
     * \param[in]       Cmd                 The command.
     * \param[in, out]  AckCommandChannel   The command channel for acknowledges.
     */
    /****************************************************************************/
    virtual void OnExecuteCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd, CommandChannel &AckCommandChannel);
    /****************************************************************************/
    /**
     * \brief An acknowledge for an send command was received.
     *
     * Is called when an command acknowledge has to be processed.
     * \warning This method should be called only from within \ref CommandChannel::CommandChannelTxAck
     * \warning Do not let exceptions escape this method!
     *
     * \param[in]       Ref         The command reference.
     * \param[in]       Ack         The received acknowledge.
     */
    /****************************************************************************/
    virtual void OnProcessAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack);
    /****************************************************************************/
    /**
     * \brief An timeout for an send command was received.
     *
     * Is called when an command timeout has to be processed.
     *
     * \param[in]       Ref         The command reference.
     * \param[in]       CmdName     Name of command.
     */
    /****************************************************************************/
    virtual void OnProcessTimeout(Global::tRefType Ref, const QString &CmdName);

    /****************************************************************************/
    /**
     * \brief Send an acknowledge over a specific command channel.
     *
     * \param[in]   Ref         The acknowledge reference.
     * \param[in]   Ack         The acknowledge.
     * \param[in]   CmdChannel  The command channel for the command
     */
    /****************************************************************************/
    void SendAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack, CommandChannel &CmdChannel);
    /****************************************************************************/
    /**
     * \brief Send DataContainers to scheduler
     *
     ****************************************************************************/
    virtual void SendContainersToScheduler() {}

    /****************************************************************************/
    /**
     * \brief Create GUI controllers, which inturn starts GUI process .
     *
     * \note All the XML files are sent to GUI.
     */
    /****************************************************************************/
    virtual void InitializeGUI() = 0;

    /****************************************************************************/
    /**
     * \brief Initialize all controllers in the order they were created.
     *
     * Calls \ref CreateAndInitializeObjects for each.
     */
    /****************************************************************************/
    void InitializeControllers(bool BasicThreadController = false);
    /****************************************************************************/
    /**
     * \brief Attach controllers to corresponding threads and start threads.
     */
    /****************************************************************************/
    void AttachControllersAndStartThreads(bool BasicController = false);

public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   LoggingSourceController     Source for thread controller.
     * \param[in]   LoggingSourceDataLogging    Sources for data logging component.
     * \param[in]   LoggingSourceEventHandler   Source for event handler component.
     * \param[in]   ShutdownSharedMemName       Name for shared memory used for shutdown. For debugging purposes only.
     */
    /****************************************************************************/
    MasterThreadController(Global::gSourceType HeartBeatSourceController,
                           Global::gSourceType HeartBeatSourcesDataLogging,
                           Global::gSourceType HeartBeatSourceEventHandler,
                           const QString &ShutdownSharedMemName);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~MasterThreadController();
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
    /****************************************************************************/
    /**
     * \brief Create and initialize used objects.
     *
     * We try to read the hardware configuration file.
     */
    /****************************************************************************/
    virtual void CreateAndInitializeObjects();
    /****************************************************************************/
    /**
     * \brief Cleanup and destroy used objects.
     */
    /****************************************************************************/
    virtual void CleanupAndDestroyObjects();
    /****************************************************************************/
    /**
     * \brief Set maximal file size for event logger.
     *
     * 0 means no maximal file size monitoring!
     * \param[in]   MaxFileSize     Max file size.
     */
    /****************************************************************************/
    inline void SetEventLoggerMaxFileSize(qint64 MaxFileSize) {
        m_EventLoggerMaxFileSize = MaxFileSize;
    }
    /****************************************************************************/
    /**
     * \brief Set maximal file count for day operation logger.
     *
     * 0 means no maximal file count monitoring!
     * \param[in]   MaxFileCount    Max file count.
     */
    /****************************************************************************/
    inline void SetDayEventLoggerMaxFileCount(int MaxFileCount) {
        Q_ASSERT(MaxFileCount >= 0);
        m_DayEventLoggerMaxFileCount = MaxFileCount;
    }
    /****************************************************************************/
    /**
     * \brief Set max alowed offset to system time.
     *
     * 0 means no check has to be done!
     * \param[in]   MaxAdjustedTimeOffset   Max alowed offset to system time [seconds]. 0 means no check has to be done.
     */
    /****************************************************************************/
    inline void SetMaxAdjustedTimeOffset(int MaxAdjustedTimeOffset) {
        m_MaxAdjustedTimeOffset = MaxAdjustedTimeOffset;
    }
    /****************************************************************************/
    /**
     * \brief Read time offset file.
     *
     * Reads (and if it not exists creates) the time offset file and sets the
     * offset acordingly.
     * \return  True on success.
     */
    /****************************************************************************/
    bool ReadTimeOffsetFile();
    /****************************************************************************/
    /**
     * \brief Register a command for processing.
     *
     * \param[in]   pCommandProcessor   Pointer to thread controller instance which processes the command.
     * \param[in]   FunctionPointer     Function which processes the command.
     */
    /****************************************************************************/
    template<class CmdClass, class CommandProcessorClass>
    void RegisterCommandForProcessing(void(CommandProcessorClass::*FunctionPointer)(Global::tRefType, const CmdClass &, CommandChannel &),
                                      CommandProcessorClass *pCommandProcessor)
    {
        // create functor
        CommandExecuteFunctorAckShPtr_t Functor(new TemplateCommandExecuteFunctorAck<CommandProcessorClass, CmdClass> (pCommandProcessor, FunctionPointer));
        // and register
        RegisterCommandExecuteFunctor(CmdClass::NAME, Functor);
    }
    /****************************************************************************/
    /**
     * \brief Broadcast a command to all controllers.
     *
     * \param[in]       Cmd         The command.
     */
    /****************************************************************************/
    void BroadcastCommand(const Global::CommandShPtr_t &Cmd);
    /****************************************************************************/
    /**
     * \brief Send a positive acknowledge over its command channel.
     *
     * Create a positive acknwoledge of type \ref Global::AckOKNOK and send it.
     *
     * \param[in]   Ref         Acknowledge reference.
     * \param[in]   CmdChannel  The command channel for the command
     */
    /****************************************************************************/
    void SendAcknowledgeOK(Global::tRefType Ref, CommandChannel &CmdChannel);
    /****************************************************************************/
    /**
     * \brief Send a negative acknowledge over its command channel.
     *
     * Create a negative acknwoledge of type \ref Global::AckOKNOK and send it.
     *
     * \param[in]   Ref         Acknowledge reference.
     * \param[in]   CmdChannel  The command channel for the command
     * \param[in]   Text        Text of message.
     * \param[in]   Type        Type of message.
     */
    /****************************************************************************/
    void SendAcknowledgeNOK(Global::tRefType Ref, CommandChannel &CmdChannel, const QString &Text = "", Global::GUIMessageType Type = Global::GUIMSGTYPE_ERROR);
    /****************************************************************************/
    /**
     * \brief Send a command over a specific command channel.
     *
     *
     * \param[in]   Cmd         The command.
     * \param[in]   CmdChannel  The command channel for the command
     * \return                  The command reference.
     */
    /****************************************************************************/
    Global::tRefType SendCommand(const Global::CommandShPtr_t &Cmd, CommandChannel &CmdChannel);

    Platform::AlarmHandler* GetAlarmHandler() {return mp_alarmHandler; }

    /****************************************************************************/
    /**
     * \brief Get Pointer to DataManager Object
     * \return DataManager Pointer
     */
    /****************************************************************************/
    virtual const DataManager::CDataManagerBase *GetDataManager() { return mp_DataManagerBase; }

public slots:
    /****************************************************************************/
    /**
     * \brief Receive a heartbeat signals.
     *
     * \param[in]   TheHeartBeatSource    Logging source of sender.
     */
    /****************************************************************************/
    void HeartbeatSlot(const Global::gSourceType &TheHeartBeatSource);
}; // end class MasterThreadController

} // end namespace Threads

#endif // THREADS_MASTERTHREADCONTROLLER_H
