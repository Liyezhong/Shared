/****************************************************************************/
/*! \file Threads/Source/MasterThreadController.cpp
 *
 *  \brief Implementation file for class MasterThreadController.
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

#include <Threads/Include/MasterThreadController.h>
#include <Threads/Include/ThreadsEventCodes.h>

#include <Global/Include/Utils.h>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/EventTranslator.h>
#include <Global/Include/UITranslator.h>
#include <Global/Include/Commands/AckOKNOK.h>
#include <Global/Include/Commands/CmdSoftSwitchPressed.h>

#include <DataManager/Include/DataManagerBase.h>
#include <DataManager/Containers/UserSettings/Include/UserSettingsInterface.h>
#include <DataManager/Helper/Include/XmlConfigFileStrings.h>
#include <DataManager/Helper/Include/XmlConfigFileTimeOffset.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>

#include <DeviceControl/Include/DeviceProcessing/DeviceProcessing.h>
#include <EventHandler/Include/StateHandler.h>
#include <AxedaController/Include/AxedaThreadController.h>
/// Axeda commands (from Platform):
#include <RemoteCareAgent/Include/Commands/CmdAxedaAlarm.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaEvent.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaDataItem.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaUpload.h>
#include <EventHandler/Include/EventHandlerThreadController.h>
#include <EventHandler/Include/ActionHandler.h>
#include <QMetaType>
#include <QSharedMemory>
#include <QDebug>

namespace Threads {

static const unsigned long THREAD_WAIT_TIME = 2000;             ///< Time to wait when stopping thread.
static const QString TimeOffsetFileName = "TimeOffset.xml";     ///< Name of file in which time offset is stored.

static const CommandExecuteFunctorAckShPtr_t    NullCommandExecuteFunctor(NULL);    ///< NULL functor for command execution.
static const CommandExecuteFunctorShPtr_t       NullCommandExecuteFunctorWithouAck(NULL); ///< Null functor command execution

/****************************************************************************/
MasterThreadController::MasterThreadController(Global::gSourceType HeartBeatSourceController,
                                               Global::gSourceType HeartBeatSourcesDataLogging,
                                               Global::gSourceType HeartBeatSourceEventHandler,
                                               const QString &ShutdownSharedMemName) :
    BaseThreadController(HeartBeatSourceController),
    m_EventLoggerMaxFileSize(0),
    m_DayEventLoggerMaxFileCount(0),
    m_MaxAdjustedTimeOffset(0),
    m_HeartBeatSourceDataLogging(HeartBeatSourcesDataLogging),
    mp_DataLoggingThreadController(NULL),
    m_HeartBeatSourceEventHandler(HeartBeatSourceEventHandler),
    m_ControllerHeartbeatTimeout(3000),
    m_HeartbeatCheckTimeout(5000),
    m_HeartbeatCheckTimer(this),
    m_ShutdownSharedMemName(ShutdownSharedMemName),
    m_ShutdownSharedMemTimer(this),
    mp_SoftSwitchManagerThreadController(NULL),
    m_CommandChannelSoftSwitch(this, "SoftSwitch", Global::EVENTSOURCE_NONE),
    m_CommandChannelDataLogging(this, "DataLogging", Global::EVENTSOURCE_DATALOGGER),
    m_CommandChannelEventThread(this, "EventHandler", Global::EVENTSOURCE_EVENTHANDLER),
    mp_EventThreadController(NULL),
    m_HeartBeatSourceSoftSwitch(100),
    m_CommandChannelAxeda(this, "Axeda", Global::EVENTSOURCE_NONE),
    m_HeartBeatSourceAxeda(101),
    mp_alarmHandler(new Platform::AlarmHandler(5000, "./")),
    mp_UserSettings(NULL),
    mp_DataManagerBase(NULL)
{
    // register the metytype for gSourceType
    qRegisterMetaType<Global::gSourceType>("Global::gSourceType");
    CONNECTSIGNALSLOT(&m_ShutdownSharedMemTimer, timeout(), this, ExternalMemShutdownCheck());
    m_ShutdownSharedMemTimer.setInterval(500);
    m_ShutdownSharedMemTimer.setSingleShot(false);
    // register CmdDataChanged command
    RegisterCommandForProcessing<Global::CmdDataChanged, MasterThreadController>(&MasterThreadController::CmdDataChangedReceived, this);
    // register CmdPowerFail command
    RegisterCommandForProcessing<Global::CmdPowerFail, MasterThreadController>(&MasterThreadController::CmdPowerFailReceived, this);

#if defined(QT_NO_DEBUG)
    // we are in release mode so we do not start timer
#else
    // we are in debug mode so we can start timer
    m_ShutdownSharedMemTimer.start();
#endif
}

/****************************************************************************/
MasterThreadController::~MasterThreadController() {
    try {
        mp_EventThreadController = NULL;
        m_TCCommandRoutes.clear();
    } catch(...) {
    }
}

/****************************************************************************/
void MasterThreadController::CreateAndInitializeObjects() {
    CHECKPTR(mp_DataManagerBase);
    qDebug()<<" Thread EventObject" << Global::EventObject::Instance().thread();
    qDebug()<<" thread StateHandler" << EventHandler::StateHandler::Instance().thread();
    //Update serial number read from Device configuration xml
    //Serial number will be present in Log files.
    DataManager::CDataContainerCollectionBase const *p_DataContainer = mp_DataManagerBase->GetDataContainer();
    CHECKPTR(p_DataContainer);
    CHECKPTR(p_DataContainer->DeviceConfigurationInterface);
    DataManager::CDeviceConfigurationInterface *p_DeviceConfigInterface = p_DataContainer->DeviceConfigurationInterface;
    CHECKPTR(p_DeviceConfigInterface->GetDeviceConfiguration());
    CHECKPTR(p_DeviceConfigInterface->GetDeviceConfiguration()->GetValue("SerialNumber"))
    SetSerialNumber(p_DeviceConfigInterface->GetDeviceConfiguration()->GetValue("SerialNumber"));

    mp_DataLoggingThreadController->SetOperatingMode(m_OperatingMode);
    mp_DataLoggingThreadController->SetEventLoggerBaseFileName(m_EventLoggerBaseFileName);
    mp_DataLoggingThreadController->SetSerialNumber(m_SerialNumber);
    mp_DataLoggingThreadController->SetEventLoggerMaxFileSize(m_EventLoggerMaxFileSize);
    mp_DataLoggingThreadController->SetDayEventLoggerMaxFileCount(m_DayEventLoggerMaxFileCount);


    MasterThreadController::RegisterCommands();


    CHECKPTR(p_DataContainer->SettingsInterface);
    mp_UserSettings = p_DataContainer->SettingsInterface->GetUserSettings();
    CHECKPTR(mp_UserSettings);
    CHECKPTR(mp_alarmHandler);
    // read event strings. language and fallback language is English
    ReadEventTranslations(QLocale::English, QLocale::English);
    if (mp_UserSettings) {
        // read ui strings. language is user language and fallback language is English
        ReadUITranslations(mp_UserSettings->GetLanguage(), QLocale::English);

        mp_alarmHandler->setSoundNumber(Global::ALARM_ERROR, mp_UserSettings->GetSoundNumberError());
        mp_alarmHandler->setSoundNumber(Global::ALARM_WARNING, mp_UserSettings->GetSoundNumberWarning());
        mp_alarmHandler->setVolume(Global::ALARM_ERROR, mp_UserSettings->GetSoundLevelError());
        mp_alarmHandler->setVolume(Global::ALARM_WARNING, mp_UserSettings->GetSoundLevelWarning());
    }


}

/****************************************************************************/
void MasterThreadController::RegisterCommands() {
    qDebug()<<"Registering Command MasterThreadController";
    RegisterCommandForProcessing<Global::CmdSoftSwitchPressed, MasterThreadController>(&MasterThreadController::OnSoftSwitchPressedAtStartup, this);
    // tests for Axeda
    RegisterCommandForRouting<RCAgentNamespace::CmdAxedaAlarm>(&m_CommandChannelAxeda);
    RegisterCommandForRouting<RCAgentNamespace::CmdAxedaEvent>(&m_CommandChannelAxeda);
    RegisterCommandForRouting<RCAgentNamespace::CmdAxedaDataItem>(&m_CommandChannelAxeda);
    RegisterCommandForRouting<RCAgentNamespace::CmdAxedaUpload>(&m_CommandChannelAxeda);
}

/****************************************************************************/
void MasterThreadController::CleanupAndDestroyObjects() {
    // nothing to do
}

/****************************************************************************/
void MasterThreadController::ConnectDataLoggingSignals(const BaseThreadController *pController) {
    CHECKPTR(mp_EventThreadController);
    //When an Event is raised, it will be received by Event hanlder.
    //EventHandler does 1) Check does logging if required. If found to be required, updates entry.
    //                  2) If event is of type warning/error/fatal error, its forwarded to errorhandler
    //Note:EventHandler is connected to DataLogger by signal slot mechanism.

    CONNECTSIGNALSLOT(pController, EmitDayEventEntry(const DataLogging::DayEventEntry &),
                      mp_EventThreadController, ProcessEvent(const DataLogging::DayEventEntry &));
}

/****************************************************************************/
void MasterThreadController::AddAndConnectController(ThreadController *pController, Threads::CommandChannel *pCommandChannel, int ControllerNumber, bool BasicThreadController) {
    // check pointers
    CHECKPTR(pController);
    CHECKPTR(pCommandChannel);

    if (!BasicThreadController) {
        // add the pair to m_Controllers to ensure it will be destroyed if connecting the signals fails
        // set thread to NULL to ensure everything works in direct calls untill threads have to be started.
        m_ControllerMap.insert(ControllerNumber, tControllerPair(pController, NULL));
        CONNECTSIGNALSLOT(this, SendGo(), pController, Go());
    }
    else {
        m_BasicControllersMap.insert(ControllerNumber, tControllerPair(pController, NULL));
        CONNECTSIGNALSLOT(this, SendGoToBasicThreads(), pController, Go());
    }

    m_channelList.insert(pCommandChannel->m_channelName.simplified().toUpper(), pCommandChannel);
    // connect some signals
    CONNECTSIGNALSLOT(this, SendStop(), pController, Stop());

    // connect heartbeat signals
    CONNECTSIGNALSLOT(pController, HeartbeatSignal(const Global::gSourceType &), this, HeartbeatSlot(const Global::gSourceType &));
    // and set controllers heartbeat timeout
    pController->SetHeartbeatTimeout(m_ControllerHeartbeatTimeout);
    // remember its ID for heartbeat checks
    m_HeartbeatSources.insert(pController->GetHeartBeatSource());
    // insert command channel into broadcast vector
    m_BroadcastChannels.append(pCommandChannel);
    // connect comannd channel signals
    pController->ConnectToOtherCommandChannel(pCommandChannel);
}

/****************************************************************************/
void MasterThreadController::CreateControllersAndThreads() {
    // now create new objects common to all master threads
    // create and connect axeda controller
    Axeda::AxedaThreadController *pAxedaController = new Axeda::AxedaThreadController(m_HeartBeatSourceAxeda);
    // Connect Remote Care related signals/slots:
    //MasterThreadController::SetRemoteCareConnection(pAxedaController);
    AddAndConnectController(pAxedaController, &m_CommandChannelAxeda, static_cast<int>(AXEDA_CONTROLLER_THREAD));
}

/****************************************************************************/
void MasterThreadController::CreateBasicControllersAndThreads() {

    // now create new objects which are essential at startup. For now the
    //basic thread controllers are 1) EventHandler and 2)DataLogger 3)SoftswitchManager

    // Create the data logger controller and the data logger thread.
    // Remember pointer to data logging controller to do some automatic connecting.
    mp_DataLoggingThreadController = new DataLogging::DataLoggingThreadController(m_HeartBeatSourceDataLogging, m_EventLoggerBaseFileName);
    // configure it

    // create system's event handler
    // if an exception occures, the instance must be deleted manually!
    mp_EventThreadController = new EventHandler::EventHandlerThreadController(m_HeartBeatSourceEventHandler);

    mp_EventThreadController->ConnectToEventObject();

    //***********************************************
    //This will connect EventHandler to DataLogger.
    //************************************************
    CONNECTSIGNALSLOT(mp_EventThreadController, LogEventEntry(const DataLogging::DayEventEntry &),
                      mp_DataLoggingThreadController, SendToDayEventLogger(const DataLogging::DayEventEntry &));

    //EventHandler shall have knowledge of alarm handler to initiate alarms
    mp_EventThreadController->SetAlarmHandler(mp_alarmHandler);

    mp_SoftSwitchManagerThreadController = new SoftSwitchManager::SoftSwitchManagerThreadController(m_HeartBeatSourceSoftSwitch);

    // Connecting mp_DataLoggingThreadController using AddAndConnectController ensures that
    // errors from it will be send to the error logger.
    // Sending day operation and component test entries lead to an additional call since they
    // will be send to itself and dispatched to the logger instances (which results in one additional
    // function call in the same thread.
    // On the other hand, using the same mechanism for *all* controllers, ensures that
    // if (for example) day operation entries should be send to more than one logger instance,
    // this can be implemented easily.
    try {
        AddAndConnectController(mp_DataLoggingThreadController, &m_CommandChannelDataLogging,
                                static_cast<int>(DATA_LOGGING_THREAD), true);
    } catch(...) {
        // m_pDataLoggingThreadController was not added properly so delete all allocated stuff yourself!
        // delete m_pDataLoggingThreadController
        delete mp_DataLoggingThreadController;
        mp_DataLoggingThreadController = NULL;
        // donn't forget to delete m_pEventThreadController also!
        delete mp_EventThreadController;
        mp_EventThreadController = NULL;

        delete mp_SoftSwitchManagerThreadController;
        mp_SoftSwitchManagerThreadController = NULL;
        // rethrow exception
        throw;
    }

    // connect all common signals and slots of the Event Handler and create its thread:
    try {
        AddAndConnectController(mp_EventThreadController, &m_CommandChannelEventThread,
                                static_cast<int>(EVENT_HANDLER_THREAD), true);
        AttachActionHandler(new EventHandler::ActionHandler(mp_EventThreadController));
    } catch(...) {
        // m_pEventThreadController was not added properly so delete all allocated stuff yourself!
        // delete m_pEventThreadController
        delete mp_EventThreadController;
        mp_EventThreadController = NULL;
        // rethrow exception
        throw;
    }

    try {
        AddAndConnectController(mp_SoftSwitchManagerThreadController, &m_CommandChannelSoftSwitch,
                                static_cast<int>(SOFT_SWTICH_MANAGER_THREAD), true);
    }catch(...) {
        // mp_SoftSwitchManagerThreadController was not added properly so delete all allocated stuff yourself!
        // delete m_pEventThreadController
        delete mp_SoftSwitchManagerThreadController;
        mp_SoftSwitchManagerThreadController = NULL;
        // rethrow exception
        throw;
    }



}

/****************************************************************************/
void MasterThreadController::AttachErrorHandler(EventHandler::ErrorHandler *pErrorHandler) {
    CHECKPTR(pErrorHandler);
    // set ErrorHandler's parent that will move itself and the
    // ErrorHandler to a dedicated thread.
    pErrorHandler->setParent(mp_EventThreadController);
}

void MasterThreadController::AttachActionHandler(EventHandler::ActionHandler *pActionHandler) {
    CHECKPTR(pActionHandler);
    // set ErrorHandler's parent that will move itself and the
    // ActionHandler to a dedicated thread.
    CONNECTSIGNALSLOT(mp_EventThreadController, ForwardToErrorHandler(const DataLogging::DayEventEntry &, const quint32),
                          pActionHandler, ReceiveEvent(const DataLogging::DayEventEntry &, const quint32 ));

}

/****************************************************************************/
void MasterThreadController::SetRemoteCareConnection(const EventHandler::RemoteCareHandler *pRemoteCareHandler) const {
    // connect error receiving slot of RemoteCareHandler
    CONNECTSIGNALSLOT(mp_EventThreadController, ForwardToRemoteCare(const DataLogging::DayEventEntry &),
                      pRemoteCareHandler, ForwardEventToRemoteCare(const DataLogging::DayEventEntry &));
}

/****************************************************************************/
void MasterThreadController::DestroyControllersAndThreads(const bool BasicThreadController) {
   tControllerMap Controllers;
   BasicThreadController ? (Controllers = m_BasicControllersMap) : (Controllers = m_ControllerMap);
   for(tControllerMap::iterator it = m_ControllerMap.begin(); it != m_ControllerMap.end(); ++it) {
        // delete objects
        delete it.value().first;
        delete it.value().second;
    }
    m_ControllerMap.clear();
}

/****************************************************************************/
void MasterThreadController::InitializeControllers(bool BasicThreadController) {
   tControllerMap Controllers;
   BasicThreadController ? (Controllers = m_BasicControllersMap) : (Controllers = m_ControllerMap);
    for(tControllerMap::iterator it = Controllers.begin(); it != Controllers.end(); ++it) {
        // Initialize controllers
        qDebug() << "MasterThreadController::InitializeControllers";

        it.value().first->CreateAndInitializeObjects();
    }
}

/****************************************************************************/
void MasterThreadController::CleanupControllers(bool BasicThreadController) {
    tControllerMap Controllers;
   BasicThreadController ? (Controllers = m_BasicControllersMap) : (Controllers = m_ControllerMap);
    for(tControllerMap::iterator it = Controllers.end(); it != Controllers.begin();) {
        // go backwards
        --it;
        // cleanup object
        it.value().first->CleanupAndDestroyObjects();
    }
}

/****************************************************************************/
void MasterThreadController::AttachControllersAndStartThreads(bool BasicThreadController) {
    tControllerMap Controllers;
    BasicThreadController ? (Controllers = m_BasicControllersMap) : (Controllers = m_ControllerMap);
    for(tControllerMap::iterator it = Controllers.begin(); it != Controllers.end(); ++it) {
        // create new thread
        it.value().second = new QThread();
        // attach controller to thread
        it.value().first->moveToThread(it->second);
        // now start thread and event loop
        //   From: http://qt-project.org/wiki/Threads_Events_QObjects
        //     Note that since Qt 4.4 ... the virtual method QThread::run() ...
        //     simply calls QThread::exec();, which starts the thread’s event loop
        it.value().second->start();
    }
    // start heartbeat check timer only if m_HeartbeatCheckTimeout > 0
   if(m_HeartbeatCheckTimeout > 0) {
       m_HeartbeatCheckTimer.setInterval(m_HeartbeatCheckTimeout);
       m_HeartbeatCheckTimer.setSingleShot(false);
       m_HeartbeatCheckTimer.start();
   } else {
       LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_THREADS_INFO_NO_HEARTBEAT_CHECKING, Global::tTranslatableStringList()
                 , Global::NO_NUMERIC_DATA, false);
   }
    // and send them the Go signal
   BasicThreadController ? (emit SendGoToBasicThreads()): (emit SendGo());
}


/****************************************************************************/
void MasterThreadController::WaitForThreads(bool BasicThreadController) {
    tControllerMap Controllers;
    BasicThreadController ? (Controllers = m_BasicControllersMap) : (Controllers = m_ControllerMap);
    // stop heartbeat check timer
    m_HeartbeatCheckTimer.stop();
    for(tControllerMap::iterator it = Controllers.begin(); it !=  Controllers.end(); ++it) {
        // and wait for threads
        if(it.value().second != NULL) {
            if(!it.value().second->wait(THREAD_WAIT_TIME)){
                // thread did not quit within time
                Global::TranslatableString SourceString(it->first->GetHeartBeatSource());
                LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, EVENT_THREADS_ERROR_THREAD_STOP_ARGS,
                          Global::tTranslatableStringList() << SourceString << QString::number(THREAD_WAIT_TIME, 10),
                          Global::NO_NUMERIC_DATA, false);
                Global::ToConsole(QString("Thread %1 did not finish within %2ms").
                                  arg(it->first->GetHeartBeatSource()).
                                  arg(THREAD_WAIT_TIME));
            }
        }
    }
}

/****************************************************************************/
void MasterThreadController::RegisterCommandExecuteFunctor(const QString &CommandName, const CommandExecuteFunctorAckShPtr_t &Functor) {
    // check if already registered
    if(m_CommandExecuteFunctors.contains(CommandName)) {
        LOGANDTHROWARGS(EVENT_THREADS_ERROR_COMMAND_FUNCTOR_ALREADY_REGISTERED, CommandName);
    }
    // everything OK
    static_cast<void>(
                // we DO NOT NEED the return value of insert
                m_CommandExecuteFunctors.insert(CommandName, Functor)
                );
}

/****************************************************************************/
void MasterThreadController::RegisterCommandExecuteFunctor(const QString &CommandName, const CommandExecuteFunctorShPtr_t &Functor) {
    // check if already registered
    if(m_CommandExecuteWithoutAckFunctors.contains(CommandName)) {
        LOGANDTHROWARGS(EVENT_THREADS_ERROR_COMMAND_FUNCTOR_ALREADY_REGISTERED, CommandName);
    }
    // everything OK
    static_cast<void>(
                // we DO NOT NEED the return value of insert
                m_CommandExecuteWithoutAckFunctors.insert(CommandName, Functor)
                );
}

/****************************************************************************/
CommandExecuteFunctorAckShPtr_t MasterThreadController::GetCommandExecuteFunctor(const QString &CommandName) const {
    CommandExecuteFunctorAckHash_t::const_iterator it = m_CommandExecuteFunctors.find(CommandName);
    if(it == m_CommandExecuteFunctors.constEnd()) {
        // functor not found
        // return NULL functor
        return NullCommandExecuteFunctor;
    }
    // return functor
    return it.value();
}

/****************************************************************************/
CommandExecuteFunctorShPtr_t MasterThreadController::GetCommandExecuteFunctorWithoutAck(const QString &CommandName) const {
    CommandExecuteFunctorHash_t::const_iterator it = m_CommandExecuteWithoutAckFunctors.find(CommandName);
    if(it == m_CommandExecuteWithoutAckFunctors.constEnd()) {
        // functor not found
        // return NULL functor
        return NullCommandExecuteFunctorWithouAck;
    }
    // return functor
    return it.value();
}
/****************************************************************************/
void MasterThreadController::RegisterCommandRoutingChannel(const QString &CommandName, CommandChannel *pTargetCommandChannel) {
    CHECKPTR(pTargetCommandChannel);
    // check if already registered
    if(m_TCCommandRoutes.contains(CommandName)) {
        LOGANDTHROWARGS(EVENT_THREADS_ERROR_COMMAND_FUNCTOR_ALREADY_REGISTERED, CommandName);
    }
    // everything OK
    static_cast<void>(
                // we DO NOT NEED the return value of insert
                m_TCCommandRoutes.insert(CommandName, pTargetCommandChannel)
                );
}

/****************************************************************************/
CommandChannel *MasterThreadController::GetCommandRouteChannel(const QString &CommandName) const {
    if(!m_TCCommandRoutes.contains(CommandName)) {
        // functor not found
        return NULL;
    }
    // return command channel
    return m_TCCommandRoutes.value(CommandName);
}

CommandChannel *MasterThreadController::GetComponentRouteChannel(Global::EventSourceType component) const
{
    QHashIterator<QString, Threads::CommandChannel*> i(m_channelList);
    while (i.hasNext())
    {
        i.next();
        if (i.value()->m_componentType == component)
        {
            return i.value();
        }
    }
    return NULL;
}

//lint -efunc(613, Threads::MasterThreadController::OnExecuteCommand)
// JB: Since lint does not know that Cmd can not be "NULL" after checking it with IsNull
// it will throw lots of 613 warnings when accessing Cmd, so we disable 613 for
// this method. Remember that 613 is disabled forr all pointers in this function!
/****************************************************************************/
void MasterThreadController::OnExecuteCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    try {
        qDebug() << "MasterThreadController::OnExecuteCommand" << Ref << Cmd.GetPointerToUserData()->GetName();

        if (!IsCommandAllowed(Cmd))
        {
            qDebug() << "MasterThreadController::OnExecuteCommand, command disallowed" << Cmd.GetPointerToUserData()->GetName();
            return;
        }

        // check pointer
        if(Cmd.IsNull()) {
            LOGANDTHROWARGS(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << "Cmd" << FILE_LINE); \
        }
//        SEND_DEBUG(WHEREAMI + " " +
//                   QString("Ref = ") + QString::number(Ref, 10) +
//                   QString("Name = ") + Cmd->GetName());
        // check if this command should be routed
        CommandChannel *pChannel;
        if (Cmd->GetName().contains("CmdSystemAction"))
        {
            NetCommands::CmdSystemAction *actionCommand = (NetCommands::CmdSystemAction*)Cmd.GetPointerToUserData();
            pChannel = GetComponentRouteChannel(actionCommand->GetSource());
        }
        else
        {
            pChannel = GetCommandRouteChannel(Cmd->GetName());
        }
        if(pChannel != NULL) {
            // yes, we must route it
            // get new ref
            Global::tRefType NewRef = GetNewCommandRef();
            // check if has timeout
            if(Cmd->GetTimeout() != Global::Command::NOTIMEOUT) {
                // it has a timeout and also an acknowledge
                tRefChannelPair Pair(Ref, &AckCommandChannel);

                // add new ref to list of routing refs
                static_cast<void> (
                            // we DO NOT NEED the return value of insert
                            m_TCAcknowledgeRoutes.insert(NewRef, Pair)
                            );
            }
            //Check if some processing must be done before sending it to the target channel
            CommandExecuteFunctorShPtr_t Functor = GetCommandExecuteFunctorWithoutAck(Cmd->GetName());
            if(Functor == NullCommandExecuteFunctorWithouAck) {
                // and send command
                DoSendCommand(NewRef, Cmd, *pChannel);
            }
            else {
                //We need to process before sending command to the target channel
                Functor.GetPointerToUserData()->Execute(Ref, Cmd.GetPointerToUserData());
                // and send command
                DoSendCommand(NewRef, Cmd, *pChannel);
            }

        } else {
            // no routing, try to execute
            // Get functor and execute
            CommandExecuteFunctorAckShPtr_t Functor = GetCommandExecuteFunctor(Cmd->GetName());
            if(Functor == NullCommandExecuteFunctor) {
                // throw exception
                qDebug() << "MasterThreadController::OnExecuteCommand, no command functor for" << Cmd->GetName();
                LOGANDTHROWARG(EVENT_THREADS_ERROR_UNSUPPORTED_COMMAND, Cmd->GetName());
            }
            // execute
            Functor.GetPointerToUserData()->Execute(Ref, Cmd.GetPointerToUserData(), AckCommandChannel);
        }
    } catch(const Global::Exception &E) {
        // and send error message
        Global::EventObject::Instance().RaiseException(E);
    } catch(...) {
        // send some error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    }
}

//lint -efunc(613, Threads::MasterThreadController::OnProcessAcknowledge)
// JB: Since lint does not know that Ack can not be "NULL" after checking it with IsNull
// it will throw lots of 613 warnings when accessing Ack, so we disable 613 for
// this method. Remember that 613 is disabled forr all pointers in this function!
/****************************************************************************/
void MasterThreadController::OnProcessAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack)
{
//    qDebug() << "MasterThreadController::OnProcessAcknowledge" << Ref << Ack.GetPointerToUserData()->GetName();

    try {
        // check pointer
        if(Ack.IsNull()) {
            LOGANDTHROWARGS(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << "Ack" << FILE_LINE); \
        }
//        SEND_DEBUG(WHEREAMI + " " +
//                   QString("Ref = ") + QString::number(Ref, 10) +
//                   QString("Name = ") + Ack->GetName());
        if(m_TCAcknowledgeRoutes.contains(Ref))
        {
            // it seems that we must route acknowledge
            // Remove from list of blocked references
            UnblockCommandRef(Ref);
            // remove from list of pending commands in base class
            RemoveFromPendingCommands(Ref);
            // get pointer to acknowledge channel
            tRefChannelPair Pair = m_TCAcknowledgeRoutes[Ref];
            Global::tRefType SendRef = Pair.first;
            CommandChannel *pAckChannel = Pair.second;
            // remove from lists
            static_cast<void>(
                        // we DO NOT NEED the return value of remove
                        m_TCAcknowledgeRoutes.remove(Ref)
                        );
            CHECKPTR(pAckChannel);
            // and send
            SendAcknowledge(SendRef, Ack, *pAckChannel);
        } else {
            // try to process acknoledge
            BaseThreadController::OnProcessAcknowledge(Ref, Ack);
        }
    } catch(const Global::Exception &E) {
        // and send error message
        Global::EventObject::Instance().RaiseException(E);
    } catch(...) {
        // send some error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    }
}

/****************************************************************************/
void MasterThreadController::OnProcessTimeout(Global::tRefType Ref, const QString &CmdName) {
//    SEND_DEBUG(WHEREAMI + " " +
//               QString("Ref = ") + QString::number(Ref, 10) +
//               QString("Name = ") + CmdName);
    if(m_TCAcknowledgeRoutes.contains(Ref)) {
        // send error
        Global::EventObject::Instance().RaiseEvent(EVENT_THREADS_ERROR_COMMAND_TIMEOUT, Global::FmtArgs() << CmdName << Ref, true);

//        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, EVENT_THREADS_ERROR_COMMAND_TIMEOUT, Global::tTranslatableStringList() << CmdName << QString::number(Ref, 10)
//                  , Global::NO_NUMERIC_DATA ,false);
        // remove from list of pending commands
        RemoveFromPendingCommands(Ref);
    } else {
        // try to process timeout
        BaseThreadController::OnProcessTimeout(Ref, CmdName);
    }
}

/****************************************************************************/
void MasterThreadController::BroadcastCommand(const Global::CommandShPtr_t &Cmd) {
    // send command to all connected thread controllers.
    for(tCommandChannelVector::iterator it = m_BroadcastChannels.begin(); it != m_BroadcastChannels.end(); ++it) {
        // get new ref
        Global::tRefType Ref = GetNewCommandRef();
        // and send command to that channel
        DoSendCommand(Ref, Cmd, *(*it));
    }
}

/****************************************************************************/
Global::tRefType MasterThreadController::SendCommand(const Global::CommandShPtr_t &Cmd, CommandChannel &CmdChannel) {
    // get new command ref
    Global::tRefType Ref = GetNewCommandRef();
    // and send command to that channel
    DoSendCommand(Ref, Cmd, CmdChannel);
    // return computed Ref
    return Ref;
}

/****************************************************************************/
void MasterThreadController::SendAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack, CommandChannel &CmdChannel) {
    // send
    DoSendAcknowledge(Ref, Ack, CmdChannel);
}

/****************************************************************************/
void MasterThreadController::SendAcknowledgeOK(Global::tRefType Ref, CommandChannel &CmdChannel) {
    // create acknowledge and send
    SendAcknowledge(Ref, Global::AcknowledgeShPtr_t(new Global::AckOKNOK(true)), CmdChannel);
}

/****************************************************************************/
void MasterThreadController::SendAcknowledgeNOK(Global::tRefType Ref, CommandChannel &CmdChannel, const QString &Text, Global::GUIMessageType Type) {
    // create acknowledge and send
    SendAcknowledge(Ref, Global::AcknowledgeShPtr_t(new Global::AckOKNOK(false, Text, Type)), CmdChannel);
}

/****************************************************************************/
void MasterThreadController::ReadEventTranslations(QLocale::Language Language, QLocale::Language FallbackLanguage) const {
    const QString StringsFileName = Global::SystemPaths::Instance().GetSettingsPath() + "/EventStrings.xml";
    // cleanup translator strings. For event strings.
    Global::EventTranslator::TranslatorInstance().Reset();
    // read strings
    DataManager::XmlConfigFileStrings TranslatorDataFile;
    // Create list of used languages. Language and FallbackLanguage can be the same, since we are
    // working wit a QSet
    QSet<QLocale::Language> LanguageList;
    LanguageList << Language << FallbackLanguage;
    TranslatorDataFile.ReadStrings(StringsFileName, LanguageList);
    // check if there is still a language in LanguageList
    if(!LanguageList.isEmpty()) {
        // Uh oh... some languages could not be read.
        // send some error messages.
        for(QSet<QLocale::Language>::const_iterator it = LanguageList.constBegin(); it != LanguageList.constEnd(); ++it) {
            LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, DataManager::EVENT_DATAMANAGER_ERROR_LANG_NOT_FOUND, Global::LanguageToString(*it)
                      , Global::NO_NUMERIC_DATA, false);
        }
    }
    // now configure translator with read languages.
    for(Global::tTranslations::const_iterator it = TranslatorDataFile.Data().constBegin();
        it != TranslatorDataFile.Data().constEnd();
        ++it)
    {
        // Set language data. No default no fallback.
        Global::EventTranslator::TranslatorInstance().SetLanguageData(it.key(), it.value(), false, false);
    }
    // set default language
    Global::EventTranslator::TranslatorInstance().SetDefaultLanguage(Language);
    // set fallback language
    Global::EventTranslator::TranslatorInstance().SetFallbackLanguage(FallbackLanguage);
}

/****************************************************************************/
void MasterThreadController::ReadUITranslations(QLocale::Language UserLanguage, QLocale::Language FallbackLanguage) const {
    // cleanup translator strings. For UI strings.
    Global::UITranslator::TranslatorInstance().Reset();
    // Create list of used languages. Language and FallbackLanguage can be the same, since we are
    // working wit a QSet
    QSet<QLocale::Language> LanguageList;
    LanguageList << UserLanguage << FallbackLanguage;
    for(QSet<QLocale::Language>::const_iterator itl = LanguageList.constBegin(); itl != LanguageList.constEnd(); ++itl) {
        const QString FileName = Global::SystemPaths::Instance().GetSettingsPath() + "/EventStrings_" +
                Global::LanguageToLanguageCode(*itl) + ".xml";
        try {
            // read strings for specified language
            DataManager::XmlConfigFileStrings TranslatorDataFile;
            TranslatorDataFile.ReadStrings(FileName, QSet<QLocale::Language>() << *itl);
            // now configure translator with read languages.
            for(Global::tTranslations::const_iterator itt = TranslatorDataFile.Data().constBegin();
                itt != TranslatorDataFile.Data().constEnd();
                ++itt)
            {
                // Set language data. No default no fallback.
                Global::UITranslator::TranslatorInstance().SetLanguageData(itt.key(), itt.value(), false, false);
            }
        } catch(const Global::Exception &E) {
            // and send error message
            //RaiseException(E);
            LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_READ_CONFIG_FILE, FileName
                      , Global::NO_NUMERIC_DATA, false);
        } catch(...) {
            // send some error message
            LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                      , Global::NO_NUMERIC_DATA, false);
        }
    }
    // set default language
    Global::UITranslator::TranslatorInstance().SetDefaultLanguage(UserLanguage);
    // set fallback language
    Global::UITranslator::TranslatorInstance().SetFallbackLanguage(FallbackLanguage);
}

/****************************************************************************/
bool MasterThreadController::ReadTimeOffsetFile() {
    DataManager::XmlConfigFileTimeOffset ConfigFileTimeOffset;
    QString FileName = Global::SystemPaths::Instance().GetSettingsPath() + "/" + TimeOffsetFileName;
    try {
        int NewTimeOffset = 0;
        // read data
        ConfigFileTimeOffset.ReadTimeOffset(FileName, NewTimeOffset);
        // do some plausibility checks
        if((m_MaxAdjustedTimeOffset != 0) && (abs(NewTimeOffset) > m_MaxAdjustedTimeOffset)) {
            // offset must be checked and is outside allowed range.
            return false;
        }
        // set new offset
        Global::AdjustedTime::Instance().SetOffsetSeconds(NewTimeOffset);
        // everything OK
        return true;
    } catch(const Global::Exception &E) {
        // send error message
        Global::EventObject::Instance().RaiseException(E);
    } catch(...) {
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    }
    // something went wrong
    // error reading
    Global::ToConsole("XmlConfigFileTimeOffset.ReadFile() failed");
    Global::ToConsole("Trying to create new time offset file");
    try {
        // try to create new file
        ConfigFileTimeOffset.WriteTimeOffset(FileName, Global::AdjustedTime::Instance().GetOffsetSeconds());
        return true;
    } catch(const Global::Exception &E) {
        // send error message
        Global::EventObject::Instance().RaiseException(E);
    } catch(...) {
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  ,Global::NO_NUMERIC_DATA, false);
    }
    return false;
}

/****************************************************************************/
bool MasterThreadController::SetAdjustedDateTimeOffset(const QDateTime &NewDateTime) {
    // check if inside limits
    int NewOffset = Global::AdjustedTime::ComputeOffsetSeconds(NewDateTime);
    if((m_MaxAdjustedTimeOffset != 0) && (abs(NewOffset) > m_MaxAdjustedTimeOffset)) {
        // offset must be checked and is outside allowed range.
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_TIME_OFFSET_TOO_LARGE, Global::tTranslatableStringList() <<
                  QString::number(NewOffset, 10) <<
                  QString::number(m_MaxAdjustedTimeOffset, 10)
                  , Global::NO_NUMERIC_DATA, false);
        return false;
    }
    // log setting new time offset
    LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, Global::EVENT_GLOBAL_SET_DATE_TIME, NewDateTime.toUTC().toString(Qt::ISODate)
              , Global::NO_NUMERIC_DATA, false);
    // set date and time
    Global::AdjustedTime::Instance().AdjustToDateTime(NewDateTime);
    // log new time offset as info
    LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, Global::EVENT_GLOBAL_CURRENT_TIME_OFFSET, QString::number(Global::AdjustedTime::Instance().GetOffsetSeconds(), 10)
              , Global::NO_NUMERIC_DATA, false);
    // save new offset to file
    try {
        DataManager::XmlConfigFileTimeOffset ConfigFileTimeOffset;
        ConfigFileTimeOffset.WriteTimeOffset(Global::SystemPaths::Instance().GetSettingsPath() + "/" + TimeOffsetFileName,
                                             Global::AdjustedTime::Instance().GetOffsetSeconds());
        // everything OK
        // return true;
    } catch(const Global::Exception &E) {
        // send error message
        Global::EventObject::Instance().RaiseException(E);
    } catch(...) {
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    }
    return false;
}

/****************************************************************************/
void MasterThreadController::OnGoReceived()
{
    // all objects created from now on, live in this thread so they are served by the
    // signal/slot mechanism in this event loop.
    try {
        // Initialize controllers
        InitializeControllers(true);

        // connect heartbeat check timer
        CONNECTSIGNALSLOT(&m_HeartbeatCheckTimer, timeout(), this, HeartbeatCheck());

        // now start threads
        AttachControllersAndStartThreads(true);
    } catch(const Global::Exception &E) {
        // destroy controllers and threads
        // send error message
        Global::EventObject::Instance().RaiseException(E);
        // and request exit
        Shutdown();
    } catch(const std::bad_alloc &) {
        // destroy controllers and threads
        // send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION, FILE_LINE_LIST,
                  Global::NO_NUMERIC_DATA, false);
        // and request exit
        Shutdown();
    } catch(...) {
        // destroy controllers and threads
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST,
                  Global::NO_NUMERIC_DATA, false);
        // and request exit
        Shutdown();
    }
}

void MasterThreadController::OnSoftSwitchPressedAtStartup(Global::tRefType Ref, const Global::CmdSoftSwitchPressed &Cmd,
                                                          Threads::CommandChannel &AckCommandChannel) {
    qDebug()<<"At Start OnSoftSwitchPressedAtStartup()";
    // all objects created from now on, live in this thread so they are served by the
    // signal/slot mechanism in this event loop.
    try {
        SendAcknowledgeOK(Ref, AckCommandChannel);

        //Initialize GUI
        InitializeGUI();
//        // first of all create controllers and threads
//        CreateControllersAndThreads();

//        // Initialize controllers
//        InitializeControllers();

//        // now start threads
//        AttachControllersAndStartThreads();

//        // log current time offset as info
//        LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, Global::EVENT_GLOBAL_CURRENT_TIME_OFFSET,
//                  QString::number(Global::AdjustedTime::Instance().GetOffsetSeconds(), 10)
//                  , Global::NO_NUMERIC_DATA, false);

//        // send infos that state machine is starting
//        LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_THREADS_INFO_STARTING_STATE_MACHINE, Global::tTranslatableStringList()
//                  , Global::NO_NUMERIC_DATA, false);
//        LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_DEBUG, EVENT_THREADS_INFO_STARTING_STATE_MACHINE, Global::tTranslatableStringList()
//                  , Global::NO_NUMERIC_DATA, false);
//        // start own statemachine
//        StartStatemachine();
//        //! \todo Move the below code out of platform code - N.Kamath
//        SendContainersToScheduler();
        qDebug()<<"OnSoftSwitchPressedAtStartup();";
    } catch(const Global::Exception &E) {
        // destroy controllers and threads
        // send error message
        Global::EventObject::Instance().RaiseException(E);
        // and request exit
        Shutdown();
    } catch(const std::bad_alloc &) {
        // destroy controllers and threads
        // send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION, FILE_LINE_LIST,
                  Global::NO_NUMERIC_DATA, false);
        // and request exit
        Shutdown();
    } catch(...) {
        // destroy controllers and threads
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST,
                  Global::NO_NUMERIC_DATA, false);
        // and request exit
        Shutdown();
    }
}
 /****************************************************************************/
void MasterThreadController::OnStopReceived() {
    // Cleanup controllers
    CleanupControllers();
    CleanupControllers(true);

    // destroy controllers and threads
    DestroyControllersAndThreads();
    DestroyControllersAndThreads(true);
}

/****************************************************************************/
void MasterThreadController::Shutdown() {
    LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, Global::EVENT_GLOBAL_STRING_TERMINATING, Global::tTranslatableStringList()
              , Global::NO_NUMERIC_DATA, false);

    // send Stop signal to all thread controllers
    emit SendStop();

    // stop all threads and wait for them
    WaitForThreads();
    WaitForThreads(true);

    // call own Stop method
    Stop();

    // send request to finish master thread
    emit RequestFinish();
}

/****************************************************************************/
void MasterThreadController::HeartbeatCheck() {
//    qDebug() << "MasterThreadController::HeartbeatCheck";
    // check if all registered controllers have send their heartbeat signal
    if(m_HeartbeatSources == m_ArrivedHeartbeats) {
        // everything OK
    }
    // check missing sources
    QSet<Global::gSourceType> Missing = m_HeartbeatSources - m_ArrivedHeartbeats;
    for(QSet<Global::gSourceType>::iterator it = Missing.begin(); it != Missing.end(); ++it) {
        Global::TranslatableString SourceString(*it);
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, EVENT_THREADS_ERROR_NO_HEARTBEAT, SourceString,
                  Global::NO_NUMERIC_DATA, false);
    }
    // check for unknown heart beats
    QSet<Global::gSourceType> NotRegistered = m_ArrivedHeartbeats - m_HeartbeatSources;
    for(QSet<Global::gSourceType>::iterator it = NotRegistered.begin(); it != NotRegistered.end(); ++it) {
        Global::TranslatableString SourceString(*it);
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, EVENT_THREADS_ERROR_UNKNOWN_HEARTBEAT, SourceString
                  , Global::NO_NUMERIC_DATA, false);
    }

    // clear arrived heartbeat sources
    m_ArrivedHeartbeats.clear();
}

/****************************************************************************/
void MasterThreadController::HeartbeatSlot(const Global::gSourceType &TheHeartBeatSource) {
    // remember received heartbeat logging source.
    m_ArrivedHeartbeats.insert(TheHeartBeatSource);
}

/****************************************************************************/
void MasterThreadController::CmdDataChangedReceived(Global::tRefType /*Ref*/, const Global::CmdDataChanged &Cmd, CommandChannel &/*AckCommandChannel*/) {
    // check if command has timeout
    if(Cmd.GetTimeout() != Global::Command::NOTIMEOUT) {
        LOGANDTHROWARG(EVENT_THREADS_ERROR_COMMAND_HAS_TIMEOUT, Cmd.GetName());
    }
    // process command
    DispatchDataChangedCommand(Cmd);
    // Broadcast to all
    BroadcastCommand(Global::CommandShPtr_t(new Global::CmdDataChanged(Cmd)));
    // no acknowledge for CmdDataChanged required
}

/****************************************************************************/
void MasterThreadController::CmdPowerFailReceived(Global::tRefType /*Ref*/, const Global::CmdPowerFail &Cmd, CommandChannel &/*AckCommandChannel*/) {
    // check if command has timeout
    if(Cmd.GetTimeout() != Global::Command::NOTIMEOUT) {
        LOGANDTHROWARG(EVENT_THREADS_ERROR_COMMAND_HAS_TIMEOUT, Cmd.GetName());
    }
    // first broadcast to all
    BroadcastCommand(Global::CommandShPtr_t(new Global::CmdPowerFail(Cmd)));
    // then process command
    OnPowerFail();
    // no acknowledge for CmdPowerFail required
}

/****************************************************************************/
void MasterThreadController::DoSendDataChanged(const Global::CommandShPtr_t &Cmd) {
    // process command to ensure local OnDataChanged methods are called
    const Global::CmdDataChanged *pCommand = dynamic_cast<Global::CmdDataChanged *>(Cmd.GetPointerToUserData());
    CHECKPTR(pCommand);
    DispatchDataChangedCommand(*pCommand);
    // broadcast command to all
    BroadcastCommand(Cmd);
}

/****************************************************************************/
void MasterThreadController::ExternalMemShutdownCheck() {
#if defined(QT_NO_DEBUG)
    // we are in release mode so we exit
    return;
#else
    // we are in debug mode
    bool oTerminate = false;
    {
        // create shared memory object
        QSharedMemory ShutdownSharedMem(this);
        ShutdownSharedMem.setKey(m_ShutdownSharedMemName);
        // try to attach
        if(ShutdownSharedMem.attach()) {
            // attach OK
            if(ShutdownSharedMem.lock()) {
                // read data
                unsigned char *pbData = reinterpret_cast<unsigned char *>(ShutdownSharedMem.data());
                if(pbData != NULL) {
                    oTerminate = (*pbData != 0);
                    // reset
                    *pbData = 0;
                }
                static_cast<void> (
                            // we DO NOT NEED the return value of next
                            ShutdownSharedMem.unlock()
                            );
            }
        }
    }
    // check if termination requested.
    if(oTerminate) {
        InitiateShutdown();
    }
#endif
}


/****************************************************************************/
void MasterThreadController::StopSpecificThreadController(const int ControllerNumber, const bool BasicThreadController) {
    // Cleanup controllers
    // create the thread controller and monitor the for the command
    bool ThreadFound = false;
    tControllerPair ControllerPair;
    if (!BasicThreadController) {
        if (m_ControllerMap.contains(ControllerNumber)) {
            ControllerPair = m_ControllerMap.value(ControllerNumber);
            ThreadFound = true;
        }
    }
    else  {
        if (m_BasicControllersMap.contains(ControllerNumber)) {
            ControllerPair = m_BasicControllersMap.value(ControllerNumber);
            ThreadFound = true;
        }
    }

    if (ThreadFound) {
        ControllerPair.first->CleanupAndDestroyObjects();
        // block all the signals
        ControllerPair.second->blockSignals(true);
        ControllerPair.second->quit();
    }
}

/****************************************************************************/
void MasterThreadController::RemoveSpecificThreadController(const int ControllerNumber, const bool BasicThreadController) {
    bool ThreadFound = false;
    tControllerPair ControllerPair;
    if (!BasicThreadController) {
        if (m_ControllerMap.contains(ControllerNumber)) {
            ControllerPair = m_ControllerMap.value(ControllerNumber);
            m_ControllerMap.remove(ControllerNumber);
            ThreadFound = true;
        }
    }
    else  {
        if (m_BasicControllersMap.contains(ControllerNumber)) {
            ControllerPair = m_BasicControllersMap.value(ControllerNumber);
            m_BasicControllersMap.remove(ControllerNumber);
            ThreadFound = true;
        }
    }
    if (ThreadFound) {
            // destroy all the objects
            delete ControllerPair.first;
            delete ControllerPair.second;

    }
}

void MasterThreadController::StartSpecificThreadController(const int ControllerNumber, const bool BasicThreadController) {
    bool ThreadFound = false;
    tControllerPair ControllerPair;
    if (!BasicThreadController) {
        if (m_ControllerMap.contains(ControllerNumber)) {
            ControllerPair = m_ControllerMap.value(ControllerNumber);
            ThreadFound = true;
        }
    }
    else  {
        if (m_BasicControllersMap.contains(ControllerNumber)) {
            ControllerPair = m_BasicControllersMap.value(ControllerNumber);
            ThreadFound = true;
        }
    }
    if (ThreadFound) {
        ControllerPair.first->CreateAndInitializeObjects();
        ControllerPair.second = new QThread();
        m_ControllerMap.insert(ControllerNumber, ControllerPair);
        ControllerPair.first->moveToThread(ControllerPair.second);
        CONNECTSIGNALSLOT(ControllerPair.second, started(), ControllerPair.first, Go());
        ControllerPair.second->start();
        //BasicThreadController ? (emit SendGoToBasicThreads()): (emit SendGo());
    }
}

} // end namespace Threads
