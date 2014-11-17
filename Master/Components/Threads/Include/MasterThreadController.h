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
#include <Threads/Include/ThreadController.h>
#include <HeartBeatManager/Include/HeartBeatThread.h>

#include <NetCommands/Include/CmdSystemAction.h>

#include <Global/Include/Commands/CmdSoftSwitchPressed.h>
#include <Global/Include/EventObject.h>
#include <Global/Include/AlarmHandler.h>
#include <QStack>
#include <QPair>
#include <QSet>

#include <NetCommands/Include/CmdRCRequestRemoteSession.h>

namespace DataManager {
    class CDataManagerBase;
    class CUserSettings;
}

namespace EventHandler {
    class EventHandlerThreadController;

}

namespace DataLogging {
    class DataLoggingThreadController;
}

namespace GPIOManager {
    class GPIOThreadController;
}

namespace HeartBeatManager {
    class HeartBeatThreadController;
}

namespace RemoteCare {
    class RemoteCareManager;
}

namespace SWUpdate {
    class SWUpdateManager;
}

namespace Threads {

//! Enumeration for Basic threads
typedef enum {
    UNDEFINED_BASIC_THREAD,
    DATA_LOGGING_THREAD,
    EVENT_HANDLER_THREAD,
    GPIO_MANAGER_THREAD
} BasicThreads_t;

//! Enumeration for Specific threads
typedef enum {
    UNDEFINED_PLATFORM_THREAD = 100,
    AXEDA_CONTROLLER_THREAD = 101
} PlatformThreads_t;

//! Enumeration for CommandChannelSelector
enum CommandChannelSelector_t {
    UNDEFINED = 100
};
typedef Global::SharedPointer<CommandExecuteFunctorAck>     CommandExecuteFunctorAckShPtr_t;    ///< Typedef or a shared pointer of CommandExecuteFunctor.
typedef QHash<QString, CommandExecuteFunctorAckShPtr_t>     CommandExecuteFunctorAckHash_t;     ///< Typedef for the CommandExecuteFunctorShPtr_t functor hash.

typedef QPair<ThreadController *, QThread *>        tControllerPair;            ///< Typedef for a pair consisting of a thread controller and a thread. Both as pointers.
typedef QMap<quint32, tControllerPair>                  tControllerMap;             ///< Map of tControllerPair and controller number
typedef QHash<QString, CommandChannel *>            tTCCommandChannelHash;      ///< Typedef for the TCCommandRouteFunctor functor hash.
typedef QPair<Global::tRefType, CommandChannel *>   tRefChannelPair;            ///< Typedef for pair of tRefType and CommandChannel *
typedef QHash<Global::tRefType, tRefChannelPair >   tTCAckChannelHash;          ///< Typedef for the TCCommandRouteFunctor functor hash.
typedef QVector<CommandChannel *>                   tCommandChannelVector;      ///< Typedef for command channel vector.
typedef QList<const BaseThreadController *>               tBaseThreadControllerList;  ///< Typefef for BaseThreadController pointer list
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
    friend class RemoteCare::RemoteCareManager;
    friend class SWUpdate::SWUpdateManager;

private:
    QString                                     m_OperatingMode;                    ///< Operating mode.
    QString                                     m_EventLoggerBaseFileName;          ///< Base for file name for event logging.
    qint64                                      m_EventLoggerMaxFileSize;           ///< Max file size for event logger.
    int                                         m_DayEventLoggerMaxFileCount;       ///< Max number of files for day operation logger.
    int                                         m_MaxAdjustedTimeOffset;            ///< Max alowed offset to system time [seconds]. 0 means no check has to be done.
    quint32                                     m_ThreadIDDataLogging;              ///< Heart Beat Source ID of the DataLogging component.
    DataLogging::DataLoggingThreadController    *mp_DataLoggingThreadController;    ///< Pointer to the DataLoggingComponent
    quint32                                     m_ThreadIDEventHandler;             ///< Heart Beat Source ID of the Event Handler.
    QThread                                     *mp_HeartBeatThread;                ///< HeartBeat Thread
    HeartBeatManager::HeartBeatThreadController *mp_HeartBeatThreadController;      ///< Pointer to HeartBeatThreadController
    quint32                                     m_ThreadIDHeartBeat;                ///< Heart Beat thread ID.
    // command executing stuff
    CommandExecuteFunctorAckHash_t              m_CommandExecuteFunctors;           ///< Functors of supported commands.
    CommandExecuteFunctorHash_t                 m_CommandExecuteWithoutAckFunctors; ///< Functors of commands without Ack.
    // command routing stuff
    tTCCommandChannelHash                       m_TCCommandRoutes;                  ///< Supported routing commands.
    tTCAckChannelHash                           m_TCAcknowledgeRoutes;              ///< Acknowledge routing.
    // command broadcasting stuff
    tCommandChannelVector                       m_BroadcastChannels;                ///< Vector of channels for broadcasting commands.

    QString                                     m_ShutdownSharedMemName;            ///< Name of shared memory for inducing a system shutdown. Will work only in debug mode!
    QTimer                                      m_ShutdownSharedMemTimer;           ///< Timer for checking external request for system shutdown. Will work only in debug mode!

    tControllerMap                              m_BasicControllersMap;              //!< Basic thread controllers; Key- Controller number, value- tControllerPair
    quint32                                     m_ThreadIDRemoteCare;               ///< Heart Beat source of remote care Thread Controller
    quint32                                     m_RebootCount;                      ///< Number of times the system has rebooted
    QStringList                                 m_EventStringFileList;              ///< Store all the event string file list
    tBaseThreadControllerList                   m_PendingForHeartBeatCheck;         ///< List of threadcontrollers that are yet to be added for heartbeat check


    /****************************************************************************/
    MasterThreadController();                                                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(MasterThreadController)

    /****************************************************************************/
    /**
     * \brief Cleanup all controllers in the reverse order they were initialized.
     *
     * Calls \ref CleanupAndDestroyObjects for each controller.
     *
     * \iparam BasicThreadController
     */
    /****************************************************************************/
    void CleanupControllers(bool BasicThreadController = false);

    /****************************************************************************/
    /**
     * \brief Wait for thread termination.
     *
     * We wait for a specific time amount THREAD_WAIT_TIME. If the thread does not
     * terminate we throw an exception
     *
     * \iparam BasicThreadController
     */
    /****************************************************************************/
    void WaitForThreads(bool BasicThreadController = false);
    /****************************************************************************/
    /**
     * \brief Get command channel for routing by command name.
     *
     * Returns NULL if functor not found.
     * \iparam   CommandName     Name of command.
     * \return                      The command channels.
     */
    /****************************************************************************/
    CommandChannel *GetCommandRouteChannel(const QString &CommandName) const;
    /****************************************************************************/
    /**
     * \brief Get command channel for routing by component type.
     *
     * Returns NULL if functor not found.
     * \iparam   component     Type of component.
     * \return                      The command channels.
     */
    /****************************************************************************/
    CommandChannel *GetComponentRouteChannel(Global::EventSourceType component) const;
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
     * \iparam   Ref                 Command reference.
     * \iparam   Cmd                 Command.
     * \iparam   AckCommandChannel   Command channel for acknowledges.
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
     * \iparam   Ref                 Command reference.
     * \iparam   Cmd                 Command.
     * \iparam   AckCommandChannel   Command channel for acknowledges.
     */
    /****************************************************************************/
    virtual void CmdPowerFailReceived(Global::tRefType Ref, const Global::CmdPowerFail &Cmd, CommandChannel &AckCommandChannel);
    /****************************************************************************/
    /**
     * \brief Send a data changed command.
     *
     * The command will be broadcasted to all connected thread controllers.
     *
     * \iparam   Cmd         The command.
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

    /****************************************************************************/
    /**
     * \brief Function is called on SoftSwitch pressed at startUp.
     *
     * \iparam Ref
     * \iparam Cmd
     * \iparam AckCommandChannel
     */
    /****************************************************************************/
    void OnSoftSwitchPressedAtStartup(Global::tRefType Ref, const Global::CmdSoftSwitchPressed &Cmd,
                                                              Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief Function for adding a thread controller for heartbeat check.
     *         - Doesn't add heartbeat thread for check
     *
     * \iparam p_Controller = Thread controller pointer
     *
     */
    /****************************************************************************/
    void AddControllerForHeartBeatCheck(const Threads::BaseThreadController *p_Controller);

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
     * \brief Stop signal for all basic threads
     */
    /****************************************************************************/
    void SendStopToBasicThread();
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

    /****************************************************************************/
    /**
     * \brief Emit this signal whenever the Main connects to GUI. So that who-ever
     *        connects this signal can send the data as soon as GUI is available
     */
    /****************************************************************************/
    void GUIConnected();

    /****************************************************************************/
    /**
     * \brief Signals when export is finished
     *
     * \iparam FileName - File name of the log file
     */
    /****************************************************************************/
    void RemoteCareExportFinished(const QString FileName);

    /****************************************************************************/
    /**
     * \brief Signals when remote care requests for export
     *
     * \iparam LogFiles - Number of log files
     */
    /****************************************************************************/
    void RemoteCareExport(const quint8 &LogFiles);

    /****************************************************************************/
    /**
     * \brief plays error alarm or sound tone
     *
     * \iparam AlarmTypeFlag = true if alram, or false
     * \iparam AlarmVolume = volume
     * \iparam AlarmNumber = alarm number
     */
    /****************************************************************************/
    void PlayAlarmTone(bool AlarmTypeFlag, quint8 AlarmVolume, quint8 AlarmNumber);

    /****************************************************************************/
    /**
     * \brief Signals when sw from remote care is requested
     */
    /****************************************************************************/
    void UpdateSoftwareFromRC();

    /****************************************************************************/
    /**
     * \brief Signals when SW update is success or failure
     *
     * \iparam Status - true or false
     */
    /****************************************************************************/
    void InformRCSWUpdateStatus(bool Status);

    /****************************************************************************/
    /**
     * \brief This signal is to send command to GUI from RemoteCareManager
     *
     *  \iparam Cmd = command to be sent to gui
     */
    /****************************************************************************/
    void SendRCCmdToGui(const Global::CommandShPtr_t &Cmd);

    /****************************************************************************/
    /**
     * \brief Signals thread controllers to start heartbeat timer.
     *
     */
    /****************************************************************************/
    void SigHeartBeatTimerStart();
    /****************************************************************************/
    /**
     * \brief Signals thread controllers to stop heartbeat timer.
     *
     */
    /****************************************************************************/
    void SigHeartBeatTimerStop();
    /****************************************************************************/
    /**
     * \brief Signal heartbeat thread to monitor thread with Id passed in arg
     * \iparam ThreadID = unique thread ID of the thread to be monitored
     */
    /****************************************************************************/
    void AddControllerForHeartBeatCheck(quint32 ThreadID) ;

    /****************************************************************************/
    /**
     * \brief This signal is to send command to GUI
     *
     *  \iparam Cmd = command to be sent to gui
     */
    /****************************************************************************/
    void SendCmdToGui(const Global::CommandShPtr_t &Cmd);

private slots:
    /****************************************************************************/
    /**
     * \brief Checks if an external request for system shutdown arrived.
     * \warning This slot is called only in debug mode!
     */
    /****************************************************************************/
    void ExternalMemShutdownCheck();

    /****************************************************************************/
    /**
     * \brief Slot called when a thread controller is started. At the moment, this
     *        function shall add this thread controller for heart beat monitoring.
     *        any new features which should be introduced on thread start can be
     *        added here.
     *  \iparam p_BaseThreadController = pointer to the thread controller which
     *                                   was started
     */
    /****************************************************************************/
    void OnThreadControllerStarted(const BaseThreadController *p_BaseThreadController);
    void OnMissingHeartBeats(const QSet<quint32> Missing);

protected: 
    tControllerMap                              m_ControllerMap;                ///< Thread controller
    CommandChannel                              m_CommandChannelDataLogging;    ///< Command channel for DataLogging.
    CommandChannel                              m_CommandChannelEventThread;    ///< Command channel for EventHandler.
    EventHandler::EventHandlerThreadController  *mp_EventThreadController;      ///< Pointer to the system event handling object.
    QHash<QString, Threads::CommandChannel*>    m_channelList;                  ///< Hash of command channels connected related to its name
    DataManager::CUserSettings                  *mp_UserSettings;               ///< The user settings.
    DataManager::CDataManagerBase               *mp_DataManagerBase;            //!< The DataManager.\warning Dont delete this, Pointer points to address in stack.
    GPIOManager::GPIOThreadController           *mp_GPIOThreadController;       //!< Thread controller to monitor softswitch
    CommandChannel                              m_CommandChannelGPIOManager;    //!< Command channel for SoftSwitch Manager
    quint32                                     m_ThreadIDGPIOManager;          //!< Heart beat source of softswitch
    CommandChannel                              m_CommandChannelHeartBeat;      //!< Command channel for SoftSwitch Manager
    Threads::CommandChannel                     m_CommandChannelRemoteCare;     ///< Command channel for remote care thread controller.
    RemoteCare::RemoteCareManager               *mp_RemoteCareManager;          //!< pointer to remote care manager
    bool                                        m_MainRebooted;                 //!< Flag indicating if the Main Software rebooted
    QString                                     m_SWUpdateStatus;               //!< SWUpdate status \note - Doesnt indicate the success of updating Rollback folder.
    bool                                        m_SWUpdateSuccess;              //!< true indicate sw update is success , including Rollback , else false
    QMap<QString, QString>                      m_BootConfigFileContent;            //!< Map containing reboot file content.
    bool                                        m_UpdatingRollback;             ///< true- Indicates update rollback is in progress.
    bool                                        m_PowerFailed;                  ///< true- Power failed during previous run , false - no power failure
    bool                                        m_RaiseGUIEvent;                        ///< Store GUI event flag
    bool                                        m_GUIEventStatus;                       ///< Store GUI event status
    SWUpdate::SWUpdateManager                   *mp_SWUpdateManager;            //!< The SWUpdate Manager
    QString                                     m_InstrumentName;               //!< Instrument name
    QString                                     m_InstrumentType;               //!< Instrument type
    QString                                     m_Eth0MacAddress;               //!< MAC address of eth0 network interface
    QString                                     m_SerialNumber;                     //!< Serial number.
    QString                                     m_SWVersion;                    //!< SW version


    /****************************************************************************/
    /**
     * \brief Set event string file list
     *
     * \iparam FileList = list of the files
     */
    /****************************************************************************/
    inline void SetEventStringFileList(QStringList FileList) {
        m_EventStringFileList = FileList;
    }

    /****************************************************************************/
    /**
     * \brief Add a new EventConfig.
     *
     * \iparam File = EventConfig file
     */
    /****************************************************************************/
    inline void AppendEventStringFileList(QString File) {
        m_EventStringFileList.append(File);
    }

    /****************************************************************************/
    /**
     * \brief Get event string file list
     *
     * \return List of the files
     */
    /****************************************************************************/
    inline QStringList GetEventStringFileList() const {
        return m_EventStringFileList;
    }

    /****************************************************************************/
    /**
     * \brief Set S/W update status
     *
     * \iparam Status = True for success, false for failure
     */
    /****************************************************************************/
    inline void SetSWUpdateStatus(const bool Status) {
        m_SWUpdateSuccess = Status;
    }

    /****************************************************************************/
    /**
     * \brief Get S/W update status
     *
     * \return True for success, false for failure
     */
    /****************************************************************************/
    inline bool GetSWUpdateStatus() const {
        return m_SWUpdateSuccess;
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
    /****************************************************************************/
    /**
     * \brief Gets Basic Controllers Count
     *
     * \return Controller map count
     */
    /****************************************************************************/
    inline int GetBasicControllersCount() const {
        return m_BasicControllersMap.count();
    }
    /****************************************************************************/
    /**
     * \brief Stop the specified controllers and threads.
     *
     * \iparam ControllerNumber
     * \iparam BasicThreadController
     */
    /****************************************************************************/
    void StopSpecificThreadController(const quint32 ControllerNumber, const bool BasicThreadController = false);

    /****************************************************************************/
    /**
     * \brief Remove controllers and threads from the list.
     *
     * \iparam ControllerNumber = Controller number
     * \iparam p_Channel = Thread command channel
     * \iparam BasicThreadController = Base thread control number
     */
    /****************************************************************************/
    void RemoveSpecificThreadController(const quint32 ControllerNumber,
                                        CommandChannel *p_Channel = NULL,
                                        const bool BasicThreadController = false);

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
     * \iparam ControllerNumber
     *
     ****************************************************************************/
    void StartSpecificThreadController(const quint32 ControllerNumber);

    /****************************************************************************/
    /**
     * \brief Destroy all registered controllers and threads.
     *
     * \iparam BasicThreadController
     */
    /****************************************************************************/
    void DestroyControllersAndThreads(const bool BasicThreadController = false);

    /****************************************************************************/
    /**
     * \brief Add controller and thread to list of handled controllers.
     *
     * This list is used to perform common stuff to all controllers and threads:
     * connecting their \ref Go and \ref Stop slots, connecting them to the
     * data logging mechanism and so on.
     *
     * \iparam   pController         Pointer to controller.
     * \iparam   pCommandChannel     Pointer to command channel.
     * \iparam   ControllerNumber    Controller Number
     * \iparam   BasicThreadController
     */
    /****************************************************************************/
    void AddAndConnectController(ThreadController *pController, CommandChannel *pCommandChannel, quint32 ControllerNumber, bool BasicThreadController = false);

    /****************************************************************************/
    /**
     * \brief Read set of needed event string translations.
     *
     * These consist of the language resulting from current operation mode and the
     * fallback language.
     *
     * \iparam   Language            The desired language.
     * \iparam   FallbackLanguage    The desired fallback language.
     */
    /****************************************************************************/
    void ReadEventTranslations(QLocale::Language Language, QLocale::Language FallbackLanguage) const ;
    /****************************************************************************/
    /**
     * \brief Read set of needed UI string translations.
     *
     * \iparam       UserLanguage        The desired user language.
     * \iparam       FallbackLanguage    The desired fallback language.
     */
    /****************************************************************************/
    void ReadUITranslations(QLocale::Language UserLanguage, QLocale::Language FallbackLanguage) const;

    /****************************************************************************/
    /**
     * \brief Register a command execution functor.
     *
     * \iparam   CommandName     Name of command.
     * \iparam   Functor         Shared pointer of functor to register.
     */
    /****************************************************************************/
    void RegisterCommandExecuteFunctor(const QString &CommandName, const CommandExecuteFunctorAckShPtr_t &Functor);
    /****************************************************************************/
    /**
     * \brief Register a command execution functor without Ack.
     *
     * \iparam   CommandName     Name of command.
     * \iparam   Functor         Shared pointer of functor to register.
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
     * \iparam   CommandName     Name of command.
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
     * \iparam   CommandName     Name of command.
     * \return                      The functor or NullCommandExecuteFunctor.
     */
    /****************************************************************************/
    CommandExecuteFunctorShPtr_t GetCommandExecuteFunctorWithoutAck(const QString &CommandName) const;
    /****************************************************************************/
    /**
     * \brief Register a command route functor.
     *
     * \iparam   CommandName             Name of command.
     * \iparam   pTargetCommandChannel   The target command channel.
     */
    /****************************************************************************/
    void RegisterCommandRoutingChannel(const QString &CommandName, CommandChannel *pTargetCommandChannel);
    /****************************************************************************/
    /**
     * \brief Register a command for routing.
     *
     * \iparam   pTargetCommandChannel   Pointer to command channel to which command has to be routed.
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
     * \iparam   pCommandProcessor   Pointer to thread controller instance which processes the command.
     * \iparam   FunctionPointer     Function which processes the command.
     * \iparam   pTargetCommandChannel   Pointer to command channel to which command has to be routed.
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
     * \iparam       Ref                 The command reference.
     * \iparam       Cmd                 The command.
     * \iparam  AckCommandChannel   The command channel for acknowledges.
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
     * \iparam       Ref         The command reference.
     * \iparam       Ack         The received acknowledge.
     */
    /****************************************************************************/
    virtual void OnProcessAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack);
    /****************************************************************************/
    /**
     * \brief An timeout for an send command was received.
     *
     * Is called when an command timeout has to be processed.
     *
     * \iparam       Ref         The command reference.
     * \iparam       CmdName     Name of command.
     */
    /****************************************************************************/
    virtual void OnProcessTimeout(Global::tRefType Ref, const QString &CmdName);

    /****************************************************************************/
    /**
     * \brief Send an acknowledge over a specific command channel.
     *
     * \iparam   Ref         The acknowledge reference.
     * \iparam   Ack         The acknowledge.
     * \iparam   CmdChannel  The command channel for the command
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
     *
     * \iparam BasicThreadController
     */
    /****************************************************************************/
    void InitializeControllers(bool BasicThreadController = false);
    /****************************************************************************/
    /**
     * \brief Attach controllers to corresponding threads and start threads.
     */
    /****************************************************************************/
    void AttachControllersAndStartBasicThreads();

    /****************************************************************************/
    /**
     * \brief Reboot
     */
    /****************************************************************************/
    virtual void Reboot() {}
    /****************************************************************************/
    /**
     * \brief Creates Reboot file
     *
     * \iparam p_RebootFile
     */
    /****************************************************************************/
    void CreateRebootFile(QFile *p_RebootFile);

    /****************************************************************************/
    /**
     * \brief Reads Reboot file
     *
     * \iparam p_RebootFile Pointer to reboot file.
     *
     */
    /****************************************************************************/
    void ReadBootConfigFile(QFile *p_RebootFile);

    /****************************************************************************/
    /**
     * \brief Send data container list to the command channel passes
     *
     * \iparam rCommandChannel = command channel
     *
     */
    /****************************************************************************/
    virtual void SendDCLContainerTo(Threads::CommandChannel &rCommandChannel);

    /****************************************************************************/
    /**
     * \brief Power will fail shortly.
     * \iparam PowerFailStage = Type of power fail stage
     */
    /****************************************************************************/
    virtual void OnPowerFail(const Global::PowerFailStages PowerFailStage);


public:
    /****************************************************************************/
    /**
     * \brief Initiate the shutdown process.
     *
     * Initiate the shutdown process. Make some project specific tasks the call
     * \ref Shutdown to shut down software.
     * Must be implemented in derived classes.
     *
     * \iparam Reboot
     */
    /****************************************************************************/
    virtual void InitiateShutdown(bool Reboot = false) = 0;

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
     * \brief Try to set new date and time offset.
     *
     * The offset from the current date time is compared to the maximal allowed
     * offset and then set.
     * \iparam   NewDateTime     New date time.
     * \return                      True on success
     */
    /****************************************************************************/
    bool SetAdjustedDateTimeOffset(const QDateTime &NewDateTime);

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   ShutdownSharedMemName       Name for shared memory used for shutdown. For debugging purposes only.
     */
    /****************************************************************************/
    MasterThreadController(const QString &ShutdownSharedMemName);
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
     * \brief Get base of file name for even logging.
     *
     * \return   Base of file name for even logging.
     */
    /****************************************************************************/
    inline QString GetEventLoggerBaseFileName() {
        return m_EventLoggerBaseFileName;
    }
    /****************************************************************************/
    /**
     * \brief Set the serial number
     *
     * \iparam   SerialNumber
     */
    /****************************************************************************/
    inline void SetSerialNumber(const QString &SerialNumber) {
        m_SerialNumber = SerialNumber;
    }

    /****************************************************************************/
    /**
     * \brief Set the SV version
     *
     * \iparam   Version SW version
     */
    /****************************************************************************/
    inline void SetSWVersion(const QString &Version) {
        m_SWVersion = Version;
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
     * \iparam   MaxFileSize     Max file size.
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
     * \iparam   MaxFileCount    Max file count.
     */
    /****************************************************************************/
    inline void SetDayEventLoggerMaxFileCount(int MaxFileCount) {
        Q_ASSERT(MaxFileCount >= 0);
        m_DayEventLoggerMaxFileCount = MaxFileCount;
    }
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
     * \brief Set max alowed offset to system time.
     *
     * 0 means no check has to be done!
     * \iparam   MaxAdjustedTimeOffset   Max alowed offset to system time [seconds]. 0 means no check has to be done.
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
     * \iparam   pCommandProcessor   Pointer to thread controller instance which processes the command.
     * \iparam   FunctionPointer     Function which processes the command.
     */
    /****************************************************************************/
    template<class CmdClass, class CommandProcessorClass>
    /****************************************************************************/
    /**
     * \brief Register a command for processing.
     *
     * \iparam   pCommandProcessor   Pointer to thread controller instance which processes the command.
     * \iparam   FunctionPointer     Function which processes the command.
     */
    /****************************************************************************/
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
     * \iparam       Cmd         The command.
     */
    /****************************************************************************/
    void BroadcastCommand(const Global::CommandShPtr_t &Cmd);
    /****************************************************************************/
    /**
     * \brief Send a positive acknowledge over its command channel.
     *
     * Create a positive acknwoledge of type \ref Global::AckOKNOK and send it.
     *
     * \iparam   Ref         Acknowledge reference.
     * \iparam   CmdChannel  The command channel for the command
     */
    /****************************************************************************/
    void SendAcknowledgeOK(Global::tRefType Ref, CommandChannel &CmdChannel);
    /****************************************************************************/
    /**
     * \brief Send a negative acknowledge over its command channel.
     *
     * Create a negative acknwoledge of type \ref Global::AckOKNOK and send it.
     *
     * \iparam   Ref         Acknowledge reference.
     * \iparam   CmdChannel  The command channel for the command
     * \iparam   Text        Text of message.
     * \iparam   Type        Type of message.
     */
    /****************************************************************************/
    void SendAcknowledgeNOK(Global::tRefType Ref, CommandChannel &CmdChannel, const QString &Text = "", Global::GUIMessageType Type = Global::GUIMSGTYPE_ERROR);
    /****************************************************************************/
    /**
     * \brief Send a command over a specific command channel.
     *
     *
     * \iparam   Cmd         The command.
     * \iparam   CmdChannel  The command channel for the command
     * \return                  The command reference.
     */
    /****************************************************************************/
    Global::tRefType SendCommand(const Global::CommandShPtr_t &Cmd, const CommandChannel &CmdChannel);

    /**
     * \brief Send a command over a specific command channel.
     *
     *
     * \iparam   Ref         The command reference.
     * \iparam   Cmd         The command.
     * \iparam   CmdChannel  The command channel for the command
     */
    /****************************************************************************/
    void SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd, const CommandChannel &CmdChannel);

     /****************************************************************************/
    /**
     * \brief Get Pointer to DataManager Object
     * \return DataManager Pointer
     */
    /****************************************************************************/
    virtual const DataManager::CDataManagerBase *GetDataManager() { return mp_DataManagerBase; }
    /****************************************************************************/
    /**
     * \brief Return the command channel object requested
     * \iparam CommandChannelSelector = Command channel to return
     * \return CommandChannel object
     */
    /****************************************************************************/
    virtual const Threads::CommandChannel & GetCommandChannel(const int CommandChannelSelector) =0;

    /****************************************************************************/
    /**
     * \brief Get Pointer to Remote Care Manager Object
     * \return RemoteCareManager Pointer
     */
    /****************************************************************************/
    const RemoteCare::RemoteCareManager *GetRemoteCareManager() { return mp_RemoteCareManager; }

private slots:
    /****************************************************************************/
    /**
     * \brief Plays error alram tone during boot up, every second.
     */
    /****************************************************************************/
    void AlarmTest();

    /****************************************************************************/
    /**
     * \brief Slot for raising an event when GUI available
     *
     */
    /****************************************************************************/
    void SendEventReportToGUI();
}; // end class MasterThreadController

} // end namespace Threads

#endif // THREADS_MASTERTHREADCONTROLLER_H
