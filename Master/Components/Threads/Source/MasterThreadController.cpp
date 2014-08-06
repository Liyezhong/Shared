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
#include <Global/Include/AlarmPlayer.h>

#include <DataManager/Include/DataManagerBase.h>
#include <DataManager/Containers/UserSettings/Include/UserSettingsInterface.h>
#include <DataManager/Helper/Include/XmlConfigFileStrings.h>
#include <DataManager/Helper/Include/XmlConfigFileTimeOffset.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
//#include <DataManager/Containers/ProcessSettings/Commands/Include/CmdGetProcessSettingsDataContainer.h>
#include <DataManager/Containers/RCConfiguration/Include/RCConfigurationInterface.h>

#include <DeviceControl/Include/DeviceProcessing/DeviceProcessing.h>
#include <EventHandler/Include/StateHandler.h>
#include <GPIOManager/Include/GPIOThreadController.h>
#include <DataLogging/Include/DataLoggingThreadController.h>
#include <RemoteCareManager/Include/RemoteCareManager.h>
#include <NetCommands/Include/CmdRCNotifyShutdown.h>
#include <NetCommands/Include/CmdRequestShutDown.h>

#include <EventHandler/Include/HimalayaEventHandlerThreadController.h>
#include <EventHandler/Include/EventHandlerEventCodes.h>
#include <QMetaType>
#include <QSharedMemory>
#include <QDebug>
#include <Threads/Include/PlatformThreadIDs.h>
#include <SWUpdateManager/Include/SWUpdateManager.h>
#include <QNetworkInterface>



namespace Threads {

static const unsigned long THREAD_WAIT_TIME = 2000;             ///< Time to wait when stopping thread.
static const QString TimeOffsetFileName = "TimeOffset.xml";     ///< Name of file in which time offset is stored.
static const quint32 ALARM_REPEAT_MAX    =  2;                      ///< Play error tone 2 times at boot up

static const CommandExecuteFunctorAckShPtr_t    NullCommandExecuteFunctor(NULL);    ///< NULL functor for command execution.
static const CommandExecuteFunctorShPtr_t       NullCommandExecuteFunctorWithouAck(NULL); ///< Null functor command execution
const AcknowledgeProcessorFunctorShPtr_t    NullAcknowledgeProcessorFunctor(NULL);  ///< NULL functor for acknowledge processing.

/****************************************************************************/
MasterThreadController::MasterThreadController(const QString &ShutdownSharedMemName) :
    BaseThreadController(THREAD_ID_MASTERTHREAD, "MasterThread"),
    m_EventLoggerMaxFileSize(0),
    m_DayEventLoggerMaxFileCount(0),
    m_MaxAdjustedTimeOffset(0),
    m_ThreadIDDataLogging(THREAD_ID_DATALOGGING),
    mp_DataLoggingThreadController(NULL),
    m_ThreadIDEventHandler(THREAD_ID_EVENTHANDLER),
    mp_HeartBeatThreadController(NULL),
    m_ThreadIDHeartBeat(THREAD_ID_HEARTBEAT),
    m_ShutdownSharedMemName(ShutdownSharedMemName),
    m_ShutdownSharedMemTimer(this),
    m_ThreadIDRemoteCare(THREAD_ID_REMOTECARE),
    m_RebootCount(0),
    m_CommandChannelDataLogging(this, "DataLogging", Global::EVENTSOURCE_DATALOGGER),
    m_CommandChannelEventThread(this, "EventHandler", Global::EVENTSOURCE_EVENTHANDLER),
    mp_EventThreadController(NULL),
    mp_UserSettings(NULL),
    mp_DataManagerBase(NULL),
    mp_GPIOThreadController(NULL),
    m_CommandChannelGPIOManager(this, "GPIOManager", Global::EVENTSOURCE_NONE),
    m_ThreadIDGPIOManager(THREAD_ID_GPIO_MANAGER),
    m_CommandChannelHeartBeat(this, "HeartBeat", Global::EVENTSOURCE_NONE),
    m_CommandChannelRemoteCare(this, "RemoteCare", Global::EVENTSOURCE_NONE),
    mp_RemoteCareManager(NULL),
    m_MainRebooted(false),
    m_SWUpdateStatus("NA"),
    m_SWUpdateSuccess(false),
    m_UpdatingRollback(false),
    m_PowerFailed(false),
    m_RaiseGUIEvent(false),
    m_GUIEventStatus(false),
    mp_SWUpdateManager(NULL)

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
    //Create Reboot file if it doesnt exits
    QString RebootPath =  "../Settings/BootConfig.txt";
    QFile BootConfigFile(RebootPath);
    if (!BootConfigFile.exists()) {
        CreateRebootFile(&BootConfigFile);
    }
    ReadBootConfigFile(&BootConfigFile);
    SetHeartbeatTimeout(HeartBeatManager::CONTROLLER_HERATBEAT_TIMEOUT);
}

/****************************************************************************/
MasterThreadController::~MasterThreadController() {
    try {
        mp_EventThreadController = NULL;
        m_TCCommandRoutes.clear();
    }
    CATCHALL_DTOR();
}

/****************************************************************************/
void MasterThreadController::CreateAndInitializeObjects() {
    CHECKPTR(mp_DataManagerBase);
    //Update serial number read from Device configuration xml
    //Serial number will be present in Log files.
    DataManager::CDataContainerCollectionBase const *p_DataContainer = mp_DataManagerBase->GetDataContainer();
    CHECKPTR(p_DataContainer);
    CHECKPTR(p_DataContainer->DeviceConfigurationInterface);
    DataManager::CDeviceConfigurationInterface *p_DeviceConfigInterface = p_DataContainer->DeviceConfigurationInterface;
    CHECKPTR(p_DeviceConfigInterface->GetDeviceConfiguration());
    CHECKPTR(p_DeviceConfigInterface->GetDeviceConfiguration()->GetValue("SerialNumber"));
    SetSerialNumber(p_DeviceConfigInterface->GetDeviceConfiguration()->GetValue("SerialNumber"));

    mp_DataLoggingThreadController->SetOperatingMode(m_OperatingMode);
    mp_DataLoggingThreadController->SetEventLoggerBaseFileName(m_EventLoggerBaseFileName);
    mp_DataLoggingThreadController->SetSerialNumber(m_SerialNumber);
    mp_DataLoggingThreadController->SetEventLoggerMaxFileSize(m_EventLoggerMaxFileSize);
    mp_DataLoggingThreadController->SetDayEventLoggerMaxFileCount(m_DayEventLoggerMaxFileCount);
    m_InstrumentName = p_DeviceConfigInterface->GetDeviceConfiguration()->GetValue("DEVICENAME");
    QNetworkInterface Eth0(QNetworkInterface::interfaceFromName("eth0"));
    m_Eth0MacAddress = Eth0.hardwareAddress();

    MasterThreadController::RegisterCommands();


    CHECKPTR(p_DataContainer->SettingsInterface);
    mp_UserSettings = p_DataContainer->SettingsInterface->GetUserSettings();
    CHECKPTR(mp_UserSettings);
    // read event strings. language and fallback language is English
    ReadEventTranslations(QLocale::English, QLocale::English);
    if (mp_UserSettings) {
        // read ui strings. language is user language and fallback language is English
        ReadUITranslations(mp_UserSettings->GetLanguage(), QLocale::English);

        Global::AlarmPlayer::Instance().setSoundNumber(Global::ALARM_ERROR, mp_UserSettings->GetSoundNumberError());
        Global::AlarmPlayer::Instance().setSoundNumber(Global::ALARM_WARNING, mp_UserSettings->GetSoundNumberWarning());
        Global::AlarmPlayer::Instance().setVolume(Global::ALARM_ERROR,mp_UserSettings->GetSoundLevelError());
        Global::AlarmPlayer::Instance().setVolume(Global::ALARM_WARNING, mp_UserSettings->GetSoundLevelWarning());
    }

    if (m_SWUpdateStatus == "Failure") {
        Global::EventObject::Instance().RaiseEvent(SWUpdate::EVENT_SW_UPDATE_FAILED);
    }
    else if (mp_DataManagerBase->IsInitialized() && m_SWUpdateStatus == "Success" && mp_SWUpdateManager) {
        m_UpdatingRollback = true;
        mp_SWUpdateManager->UpdateSoftware("-updateRollback", "");
    }


    else if (m_SWUpdateStatus == "HigherVersionNA") {
        Global::EventObject::Instance().RaiseEvent(SWUpdate::EVENT_SW_UPDATE_NOT_PERFORMED);
    }

    if (Global::AppSettings::SettingsRolledBack) {
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_SETTINGS_ROLLED_BACK);
    }

    if (m_BootConfigFileContent.value("PowerFailed") == "Yes") {
        //Now that we know power had failed previously , revert it back to No
        m_BootConfigFileContent.insert("PowerFailed", "No");
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_RECOVERED_FROM_POWER_FAIL);
    }

    mp_GPIOThreadController->SkipSoftSwitchAtBoot(true);

}

/****************************************************************************/
void MasterThreadController::RegisterCommands() {
    qDebug()<<"Registering Command MasterThreadController";
    RegisterCommandForProcessing<Global::CmdSoftSwitchPressed, MasterThreadController>(&MasterThreadController::OnSoftSwitchPressedAtStartup, this);
}

/****************************************************************************/
void MasterThreadController::CleanupAndDestroyObjects() {
    // nothing to do
}

/****************************************************************************/
void MasterThreadController::AddAndConnectController(ThreadController *pController,
                                                     Threads::CommandChannel *pCommandChannel,
                                                     quint32 ControllerNumber,
                                                     bool BasicThreadController) {
    // check pointers
    CHECKPTR(pController);
    CHECKPTR(pCommandChannel);

    if (!BasicThreadController) {
        // add the pair to m_Controllers to ensure it will be destroyed if connecting the signals fails
        // set thread to NULL to ensure everything works in direct calls untill threads have to be started.
        m_ControllerMap.insert(ControllerNumber, tControllerPair(pController, NULL));
    }
    else {
        m_BasicControllersMap.insert(ControllerNumber, tControllerPair(pController, NULL));
    }

    m_channelList.insert(pCommandChannel->m_channelName.simplified().toUpper(), pCommandChannel);
    //Connect stop signals.
    if (!BasicThreadController) {
        CONNECTSIGNALSLOT(this, SendStop(), pController, Stop());
    }
    else {
        CONNECTSIGNALSLOT(this, SendStopToBasicThread(), pController, Stop());
    }
    CONNECTSIGNALSLOT(this, SigHeartBeatTimerStop(), pController, StopHeartbeatTimer());
    CONNECTSIGNALSLOT(this, SigHeartBeatTimerStart(), pController, StartHeartbeatTimer());
    CONNECTSIGNALSLOT(pController, ThreadControllerStarted(const BaseThreadController *),
                      this, OnThreadControllerStarted(const BaseThreadController *));
    // insert command channel into broadcast vector
    m_BroadcastChannels.append(pCommandChannel);
    // connect comannd channel signals
    pController->ConnectToOtherCommandChannel(pCommandChannel);
    pController->SetHeartbeatTimeout(HeartBeatManager::CONTROLLER_HERATBEAT_TIMEOUT);
}

/****************************************************************************/
void MasterThreadController::CreateControllersAndThreads() {
    // now create new objects common to all master threads
    // create and connect axeda controller
//    DataManager::CRCConfigurationInterface* mp_RCConfigurationInterface = mp_DataManagerBase->GetRCConfigurationInterface();
    if(!mp_RemoteCareManager) {
//        mp_RemoteCareManager = new RemoteCare::RemoteCareManager(*this, *mp_DataManagerBase);
        mp_RemoteCareManager = new RemoteCare::RemoteCareManager(*this, mp_DataManagerBase->GetRCConfigurationInterface());

        mp_RemoteCareManager->Init();
        // Connect Remote Care related signals/slots:
        CONNECTSIGNALSLOT(mp_EventThreadController, SendEventToRemoteCare(const DataLogging::DayEventEntry&, const quint64),
                          mp_RemoteCareManager, ForwardEventToRemoteCare(const DataLogging::DayEventEntry&, const quint64));
        CONNECTSIGNALSIGNAL(mp_RemoteCareManager, RemoteCareExport(const quint8 &),
                            this, RemoteCareExport(const quint8 &));
        CONNECTSIGNALSLOT(this, RemoteCareExportFinished(const QString),
                          mp_RemoteCareManager, RCExportFinished(const QString));

        CONNECTSIGNALSIGNAL(mp_RemoteCareManager, UpdateSoftwareFromRC(),
                            this, UpdateSoftwareFromRC());
        CONNECTSIGNALSLOT(this, InformRCSWUpdateStatus(bool),
                          mp_RemoteCareManager, SWUpdateStatus(bool));
        CONNECTSIGNALSIGNAL(mp_RemoteCareManager, SendRCCmdToGui(const Global::CommandShPtr_t &),
                            this, SendRCCmdToGui(const Global::CommandShPtr_t &));
    }
}

/****************************************************************************/
void MasterThreadController::CreateBasicControllersAndThreads() {

    // now create new objects which are essential at startup. For now the
    //basic thread controllers are 1) EventHandler and 2)DataLogger 3)GPIOManager

    mp_HeartBeatThreadController = new HeartBeatManager::HeartBeatThreadController(m_ThreadIDHeartBeat);
    CONNECTSIGNALSLOT(this, SigHeartBeatTimerStart(), mp_HeartBeatThreadController, StartHeartBeatCheckTimer());
    CONNECTSIGNALSLOT(this, SigHeartBeatTimerStop(), mp_HeartBeatThreadController, StopHeartBeatCheckTimer());
    CONNECTSIGNALSLOT(this, AddControllerForHeartBeatCheck(quint32), mp_HeartBeatThreadController, AddControllerForHeartBeatCheck(quint32));

    // Create the data logger controller and the data logger thread.
    // Remember pointer to data logging controller to do some automatic connecting.
    mp_DataLoggingThreadController = new DataLogging::DataLoggingThreadController(m_ThreadIDDataLogging,
                                                                                  m_EventLoggerBaseFileName);
    // configure it

    // create system's event handler
    // if an exception occures, the instance must be deleted manually!
    mp_EventThreadController = new EventHandler::HimalayaEventHandlerThreadController(m_ThreadIDEventHandler, m_RebootCount,
                                                                              m_EventStringFileList);

    mp_EventThreadController->ConnectToEventObject();

    //***********************************************
    //This will connect EventHandler to DataLogger.
    //************************************************
    CONNECTSIGNALSLOT(mp_EventThreadController, LogEventEntry(const DataLogging::DayEventEntry &),
                      mp_DataLoggingThreadController, SendToDayEventLogger(const DataLogging::DayEventEntry &));

    // this will check whether logging is enabled or not and the same can inform to GUI
    CONNECTSIGNALSLOT(this, GUIConnected(),
                      mp_DataLoggingThreadController, CheckLoggingEnabled());

    mp_GPIOThreadController = new GPIOManager::GPIOThreadController(m_ThreadIDGPIOManager);

    try {
        try {
            AddAndConnectController(mp_DataLoggingThreadController, &m_CommandChannelDataLogging,
                                    m_ThreadIDDataLogging, true);
        }
        CATCHALL_RETHROW();
    }
    catch(...) {
        delete mp_DataLoggingThreadController;
        mp_DataLoggingThreadController = NULL;
        delete mp_EventThreadController;
        mp_EventThreadController = NULL;

        delete mp_GPIOThreadController;
        mp_GPIOThreadController= NULL;
        // rethrow exception
        throw;
    }

    // connect all common signals and slots of the Event Handler and create its thread:
    try {
        try {
            AddAndConnectController(mp_EventThreadController, &m_CommandChannelEventThread,
                                    m_ThreadIDEventHandler, true);

        }
        CATCHALL_RETHROW();
    }
    catch(...) {
        // delete m_pEventThreadController
        delete mp_EventThreadController;
        mp_EventThreadController = NULL;
        // rethrow exception
        throw;
    }

    try {
        try {
            AddAndConnectController(mp_GPIOThreadController, &m_CommandChannelGPIOManager,
                                    m_ThreadIDGPIOManager, true);
        }
        CATCHALL_RETHROW();
    }
    catch(...) {
        // delete m_pEventThreadController
        delete mp_GPIOThreadController;
        mp_GPIOThreadController = NULL;
        // rethrow exception
        throw;
    }

    //create heartbeat thread;
    try {
        try {
            AddAndConnectController(mp_HeartBeatThreadController, &m_CommandChannelHeartBeat,
                                    m_ThreadIDHeartBeat, true);
        }
        CATCHALL_RETHROW();
    }
    catch(...) {
        delete mp_HeartBeatThreadController;
        mp_HeartBeatThreadController = NULL;
        // rethrow exception
        throw;
    }
}


////****************************************************************************/
void MasterThreadController::SetRemoteCareConnection(const EventHandler::RemoteCareHandler *pRemoteCareHandler) const {
    // connect error receiving slot of RemoteCareHandler
    CONNECTSIGNALSLOT(mp_EventThreadController, ForwardToRemoteCare(const DataLogging::DayEventEntry &),
                      pRemoteCareHandler, ForwardEventToRemoteCare(const DataLogging::DayEventEntry &));
}

/****************************************************************************/
void MasterThreadController::DestroyControllersAndThreads(const bool BasicThreadController) {
    tControllerMap Controllers;
    BasicThreadController ? (Controllers = m_BasicControllersMap) : (Controllers = m_ControllerMap);
    for(tControllerMap::iterator it = Controllers.begin(); it != Controllers.end(); ++it) {

        Threads::ThreadController *pController = ((Threads::ThreadController*)(it.value().first));
        quint32 ThreadID = pController->GetThreadID();
        mp_HeartBeatThreadController->RemoveControllerForHeartBeatCheck(ThreadID);

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
        it.value().first->DoCleanupAndDestroyObjects();
    }
}

/****************************************************************************/
void MasterThreadController::AttachControllersAndStartBasicThreads() {
    for(tControllerMap::iterator it = m_BasicControllersMap.begin(); it != m_BasicControllersMap.end(); ++it) {
        // create new thread
        it.value().second = new QThread();
        // attach controller to thread
        it.value().first->moveToThread(it->second);
        CONNECTSIGNALSLOT(it.value().second, started(), it.value().first, Go());
        Threads::BaseThreadController *pController = ((Threads::BaseThreadController*)(it.value().first));
        pController->SetHeartbeatTimeout(HeartBeatManager::CONTROLLER_HERATBEAT_TIMEOUT);
        // now start thread and event loop
        //   From: http://qt-project.org/wiki/Threads_Events_QObjects
        //     Note that since Qt 4.4 ... the virtual method QThread::run() ...
        //     simply calls QThread::exec();, which starts the thread’s event loop
        it.value().second->start();
    }
}


/****************************************************************************/
void MasterThreadController::WaitForThreads(bool BasicThreadController) {
    tControllerMap Controllers;
    BasicThreadController ? (Controllers = m_BasicControllersMap) : (Controllers = m_ControllerMap);
    // stop heartbeat check timer
    //mp_HeartBeatManager->StopHeartBeatCheckTimer();

    for(tControllerMap::iterator it = Controllers.begin(); it !=  Controllers.end(); ++it) {
        // and wait for threads
        if(it.value().second != NULL) {
            if(!it.value().second->wait(THREAD_WAIT_TIME)){
                // thread did not quit within time
                Global::EventObject::Instance().RaiseEvent(EVENT_THREADS_ERROR_THREAD_STOP_ARGS,
                                                           Global::FmtArgs() << it->first->GetThreadID() << QString::number(THREAD_WAIT_TIME, 10));
                Global::ToConsole(QString("Thread %1 did not finish within %2ms").
                                  arg(it->first->GetThreadID()).
                                  arg(THREAD_WAIT_TIME));
            }
        }
    }
}

/****************************************************************************/
void MasterThreadController::RegisterCommandExecuteFunctor(const QString &CommandName, const CommandExecuteFunctorAckShPtr_t &Functor) {
    // check if already registered
    if(m_CommandExecuteFunctors.contains(CommandName)) {
        LOGANDTHROWARGS(EVENT_THREADS_ERROR_COMMAND_FUNCTOR_ALREADY_REGISTERED, CommandName)
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
        LOGANDTHROWARGS(EVENT_THREADS_ERROR_COMMAND_FUNCTOR_ALREADY_REGISTERED, CommandName)
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
        LOGANDTHROWARGS(EVENT_THREADS_ERROR_COMMAND_FUNCTOR_ALREADY_REGISTERED, CommandName)
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

CommandChannel *MasterThreadController::GetComponentRouteChannel(Global::EventSourceType component) const {
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
        //qDebug() << "MasterThreadController::OnExecuteCommand" << Ref << Cmd.GetPointerToUserData()->GetName();

        if (!IsCommandAllowed(Cmd))
        {
            SendAcknowledgeNOK(Ref, AckCommandChannel, "", Global::GUIMSGTYPE_ERROR);
            qDebug() << "MasterThreadController::OnExecuteCommand, command disallowed" << Cmd.GetPointerToUserData()->GetName();
            return;
        }

        // check pointer
        CHECKPTR(Cmd.GetPointerToUserData());

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
            if (Functor == NullCommandExecuteFunctorWithouAck) {
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
                LOGANDTHROWARG(EVENT_THREADS_ERROR_UNSUPPORTED_COMMAND, Cmd->GetName())
            }
            // execute
            Functor.GetPointerToUserData()->Execute(Ref, Cmd.GetPointerToUserData(), AckCommandChannel);
        }
    }
    CATCHALL();
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
        CHECKPTR(Ack.GetPointerToUserData());

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
            if (m_ListOfAcksToRouteAndProcess.contains(Ack->GetName())) {
                // Check if main needs to process Acknowledge
                AcknowledgeProcessorFunctorShPtr_t Functor = GetAcknowledgeProcessorFunctor(Ack->GetName());
                if(Functor == NullAcknowledgeProcessorFunctor) {
                    Global::EventObject::Instance().RaiseEvent(EVENT_THREADS_ERROR_UNSUPPORTED_ACKNOWLEDGE, Global::FmtArgs() << Ack->GetName() << GetThreadID(), true);
                }
                // execute
                if (Functor.GetPointerToUserData()) {
                    Functor.GetPointerToUserData()->Process(Ref, Ack.GetPointerToUserData());
                }
            }
            // and send
            SendAcknowledge(SendRef, Ack, *pAckChannel);
        } else {
            // try to process acknoledge
            BaseThreadController::OnProcessAcknowledge(Ref, Ack);
        }
    }
    CATCHALL();
}

/****************************************************************************/
void MasterThreadController::OnProcessTimeout(Global::tRefType Ref, const QString &CmdName) {
    //    SEND_DEBUG(WHEREAMI + " " +
    //               QString("Ref = ") + QString::number(Ref, 10) +
    //               QString("Name = ") + CmdName);
    if(m_TCAcknowledgeRoutes.contains(Ref)) {
        // send error
        Global::EventObject::Instance().RaiseEvent(EVENT_THREADS_ERROR_COMMAND_TIMEOUT, Global::FmtArgs() << CmdName << Ref, true);
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
void MasterThreadController::SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd, const CommandChannel &CmdChannel) {
    // and send command to that channel
    DoSendCommand(Ref, Cmd, CmdChannel);
}

/****************************************************************************/
Global::tRefType MasterThreadController::SendCommand(const Global::CommandShPtr_t &Cmd, const CommandChannel &CmdChannel) {
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
    const QString StringsFileName = Global::SystemPaths::Instance().GetTranslationsPath()
            + "/EventStrings_" + Global::LanguageToLanguageCode(Language) + ".xml";
    // cleanup translator strings. For event strings.
    Global::EventTranslator::TranslatorInstance().Reset();

    // read strings
    DataManager::XmlConfigFileStrings TranslatorDataFile;
    // Create list of used languages. Language and FallbackLanguage can be the same, since we are
    // working wit a QSet
    QSet<QLocale::Language> LanguageList;
    LanguageList << Language << FallbackLanguage;

    // try to read the file
    try {
        TranslatorDataFile.ReadStrings(StringsFileName, LanguageList);
    }
    CATCHALL();

    // check if there is still a language in LanguageList
    if(!LanguageList.isEmpty()) {
        // Uh oh... some languages could not be read.
        // send some error messages.
        for(QSet<QLocale::Language>::const_iterator it = LanguageList.constBegin(); it != LanguageList.constEnd(); ++it) {
            //            LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, DataManager::EVENT_DM_ERROR_LANG_NOT_FOUND, Global::LanguageToString(*it)
            //                      , Global::NO_NUMERIC_DATA, false);
            Global::EventObject::Instance().RaiseEvent(DataManager::EVENT_DM_ERROR_LANG_NOT_FOUND,
                                                       Global::FmtArgs() << Global::LanguageToString(*it));
        }
    }
    // now configure translator with read languages.
    for(Global::tTranslations::const_iterator it = TranslatorDataFile.Data().constBegin();
        it != TranslatorDataFile.Data().constEnd(); ++it) {
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

    // read all the languages which are available in the translations directory
    QDir TheDir(Global::SystemPaths::Instance().GetTranslationsPath());
    QStringList FileNames = TheDir.entryList(QStringList("*.qm"));

    // Create list of used languages. Language and FallbackLanguage can be the same, since we are
    // working wit a QSet
    QSet<QLocale::Language> LanguageList;

    LanguageList << UserLanguage << FallbackLanguage;

    for (int Counter = 0; Counter < FileNames.size(); ++Counter)
    {
        // get locale extracted by filename
        QString Locale;
        Locale = FileNames[Counter];                  // "Colorado_de.qm"
        Locale.truncate(Locale.lastIndexOf('.'));   // "Colorado_de"
        Locale.remove(0, Locale.indexOf('_') + 1);   // "de"
        LanguageList << QLocale(Locale).language();
    }

    for(QSet<QLocale::Language>::const_iterator itl = LanguageList.constBegin(); itl != LanguageList.constEnd(); ++itl) {

        QString FileName = Global::SystemPaths::Instance().GetTranslationsPath() + "/EventStrings_" +
                Global::LanguageToLanguageCode(*itl) + ".xml";
        try {
            // read strings for specified language
            DataManager::XmlConfigFileStrings TranslatorDataFile;
            TranslatorDataFile.ReadStrings(FileName, QSet<QLocale::Language>() << *itl);
            // now configure translator with read languages.
            for(Global::tTranslations::const_iterator itt = TranslatorDataFile.Data().constBegin();
                itt != TranslatorDataFile.Data().constEnd(); ++itt) {
                // Set language data. No default no fallback.
                Global::UITranslator::TranslatorInstance().SetLanguageData(itt.key(), itt.value(), false, false);
            }
        }
        CATCHALL();
    }
    // set default language
    Global::UITranslator::TranslatorInstance().SetDefaultLanguage(UserLanguage);
    // set fallback language
    Global::UITranslator::TranslatorInstance().SetFallbackLanguage(FallbackLanguage);
}

/**********************************************************
    m_PowerFailed(false),******************/
bool MasterThreadController::ReadTimeOffsetFile() {
    DataManager::XmlConfigFileTimeOffset ConfigFileTimeOffset;
    QString FileName = Global::SystemPaths::Instance().GetSettingsPath() + "/" + TimeOffsetFileName;
    try {
        int NewTimeOffset = 0;
        // read data
        ConfigFileTimeOffset.ReadTimeOffset(FileName, NewTimeOffset);
        // do some plausibility checks
        if(((abs(NewTimeOffset) > m_MaxAdjustedTimeOffset))
                || (m_MaxAdjustedTimeOffset < 0 && (abs(NewTimeOffset) > m_MaxAdjustedTimeOffset))) {
            // offset must be checked and is outside allowed range.
            return false;
        }
        // set new offset
        Global::AdjustedTime::Instance().SetOffsetSeconds(NewTimeOffset);
        // everything OK
        return true;
    }
    CATCHALL();

    // something went wrong
    // error reading
    Global::ToConsole("XmlConfigFileTimeOffset.ReadFile() failed");
    Global::ToConsole("Trying to create new time offset file");
    try {
        // try to create new file
        ConfigFileTimeOffset.WriteTimeOffset(FileName, Global::AdjustedTime::Instance().GetOffsetSeconds());
        return true;
    }
    CATCHALL();
    return false;
}

/****************************************************************************/
bool MasterThreadController::SetAdjustedDateTimeOffset(const QDateTime &NewDateTime) {
    // check if inside limits
    int NewOffset = Global::AdjustedTime::ComputeOffsetSeconds(NewDateTime);
    // time can be set 0-24 hours range not more than that
    if(((abs(NewOffset) > m_MaxAdjustedTimeOffset))
            || (m_MaxAdjustedTimeOffset < 0 && (abs(NewOffset) > m_MaxAdjustedTimeOffset))) {
        return false;
    }

    // set date and time
    Global::AdjustedTime::Instance().AdjustToDateTime(NewDateTime);

    // save new offset to file
    DataManager::XmlConfigFileTimeOffset ConfigFileTimeOffset;
    ConfigFileTimeOffset.WriteTimeOffset(Global::SystemPaths::Instance().GetSettingsPath() + "/" + TimeOffsetFileName,
                                         Global::AdjustedTime::Instance().GetOffsetSeconds());
    return true;
}

/****************************************************************************/
void MasterThreadController::OnGoReceived()
{
    // all objects created from now on, live in this thread so they are served by the
    // signal/slot mechanism in this event loop.
    try {
        // Initialize controllers
        InitializeControllers(true);
        // now start threads
        AttachControllersAndStartBasicThreads();
        AddControllerForHeartBeatCheck(this);
        return;
    }
    CATCHALL();

    Shutdown();
}

void MasterThreadController::OnSoftSwitchPressedAtStartup(Global::tRefType Ref, const Global::CmdSoftSwitchPressed &Cmd,
                                                          Threads::CommandChannel &AckCommandChannel) {
    Q_UNUSED(Cmd);
    // all objects created from now on, live in this thread so they are served by the
    // signal/slot mechanism in this event loop.
    try {
        SendAcknowledgeOK(Ref, AckCommandChannel);
        // Play test tone
        //AlarmTest();
        //Initialize GUI
        InitializeGUI();
#if defined(__arm__) //Required only on Target
        system("./clearframebuffer &"); //clear screen ->LCD
#endif
        qDebug() << "MasterThreadController::OnSoftSwitchPressedAtStartup" << EventHandler::StateHandler::Instance().getCurrentOperationState();
        return;
    }
    CATCHALL();

    Shutdown();
}

 /****************************************************************************/
void MasterThreadController::OnStopReceived() {
    // Cleanup controllers
    CleanupControllers();
    // destroy controllers and threads
    DestroyControllersAndThreads();
    // cleanup & destroy basic controllers and threads
    emit SendStopToBasicThread();
    WaitForThreads(true);
    CleanupControllers(true);
    DestroyControllersAndThreads(true);
}

/****************************************************************************/
void MasterThreadController::Shutdown()
{
    if (!m_PowerFailed) {
        const QString MD5sumGenerator = QString("%1%2").arg(Global::SystemPaths::Instance().GetScriptsPath()).
                                        arg(QString("/EBox-Utils.sh update_md5sum_for_settings"));
        (void)system(MD5sumGenerator.toStdString().c_str());
    }

    std::cout <<"\n\n Shutdown Start time " << Global::AdjustedTime::Instance().GetCurrentTime().toString().toStdString();
    Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_STRING_TERMINATING, Global::tTranslatableStringList() <<"");
    //write buffered data to disk-> refer man pages for sync
    (void)QProcess::startDetached("sync &");
    //(void)QProcess::startDetached("lcd off");
    //send shutdown signal to RemoteCarecontroller
    if (mp_RemoteCareManager) {
        /// arthur 2014/07/14
        mp_RemoteCareManager->SendCommandToRemoteCare(
                    Global::CommandShPtr_t(new NetCommands::CmdRCNotifyShutdown(Global::Command::NOTIMEOUT)));
    }
    mp_HeartBeatThreadController->DontCheckHeartBeat(true);

    // send Stop signal to all thread controllers
    emit SendStop();
    // stop all threads and wait for them
    WaitForThreads();

    // Cleanup controllers
    CleanupControllers();
    // destroy controllers and threads
    DestroyControllersAndThreads();

    // call own Stop method
//    Stop();
    for (qint32 I = 0; I < m_BroadcastChannels.count(); I++ ) {
        Threads::CommandChannel *p_Channel = m_BroadcastChannels.at(I);
        p_Channel->setParent(NULL);
    }

    // send request to finish master thread
    emit RequestFinish();
    std::cout <<"\n\n Shutdown End time " << Global::AdjustedTime::Instance().GetCurrentTime().toString().toStdString();
}


/****************************************************************************/
void MasterThreadController::CmdDataChangedReceived(Global::tRefType /*Ref*/, const Global::CmdDataChanged &Cmd, CommandChannel &/*AckCommandChannel*/) {
    // check if command has timeout
    if(Cmd.GetTimeout() != Global::Command::NOTIMEOUT) {
        LOGANDTHROWARG(EVENT_THREADS_ERROR_COMMAND_HAS_TIMEOUT, Cmd.GetName())
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
        LOGANDTHROWARG(EVENT_THREADS_ERROR_COMMAND_HAS_TIMEOUT, Cmd.GetName())
    }
    // first broadcast to all
    BroadcastCommand(Global::CommandShPtr_t(new Global::CmdPowerFail(Cmd)));
    // then process command
    OnPowerFail(Cmd.m_PowerFailStage);
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

void MasterThreadController::SendDCLContainerTo(Threads::CommandChannel &rCommandChannel) {

    Q_UNUSED(rCommandChannel)
    if (mp_DataManagerBase) {
//        (void)SendCommand(Global::CommandShPtr_t(new DataManager::CmdGetProcessSettingsDataContainer(*mp_DataManagerBase->GetProcessSettings())), rCommandChannel);
    }
}


void MasterThreadController::OnPowerFail(const Global::PowerFailStages PowerFailStage)
{
    if (PowerFailStage == Global::POWER_FAIL_STAGE_1) {
        (void)system("lcd off"); // to avoid PC-lint
        if(mp_DataManagerBase) {
            mp_DataManagerBase->SaveDataOnShutdown();
        }
    }

    if (PowerFailStage == Global::POWER_FAIL_REVERT) {
        (void)system("lcd on"); // to avoid PC-lint
    }

    if (PowerFailStage == Global::POWER_FAIL_STAGE_2) {
        if (mp_SWUpdateManager) {
            //Halt SW update
            mp_SWUpdateManager->PowerFailed();
        }

        m_PowerFailed = true;
        // Power fail occured so no need of monitoring the heart beat
        //StopHeartbeatCheck();

        /*! \warning Md5sum is generated in the next few steps. Updation of files(e.g. files in Settings)
         *  after generation of Md5sum can lead to md5sum check failure and eventually rollback.
         */
        if (EventHandler::StateHandler::Instance().getCurrentOperationState() == "SoftSwitchMonitorState") {
            const QString MD5sumGenerator = QString("%1%2").arg(Global::SystemPaths::Instance().GetScriptsPath()).
                                            arg(QString("/EBox-Utils.sh update_md5sum_for_settings"));
            (void)system(MD5sumGenerator.toStdString().c_str());
            InitiateShutdown();
        }
        else {
            m_BootConfigFileContent.insert("PowerFailed", "Yes");
            Global::UpdateRebootFile(m_BootConfigFileContent);
            const QString MD5sumGenerator = QString("%1%2").arg(Global::SystemPaths::Instance().GetScriptsPath()).
                                            arg(QString("/EBox-Utils.sh update_md5sum_for_settings"));
            (void)system(MD5sumGenerator.toStdString().c_str());
        }
    }
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

void MasterThreadController::OnThreadControllerStarted(const BaseThreadController *p_BaseThreadController)
{
    static bool HeartBeatThreadStarted = false;
    if (HeartBeatThreadStarted || p_BaseThreadController == mp_HeartBeatThreadController) {
        // This means heartbeat thread is started or the thread started is heartbeat thread.
        // now we can add other threads for heart beat monitoring.
        if (p_BaseThreadController != this) {
            // Monitor heart beat of the thread started
            AddControllerForHeartBeatCheck(p_BaseThreadController);
        }
        if (THREAD_ID_HEARTBEAT == p_BaseThreadController->GetThreadID()) {
            HeartBeatThreadStarted = true;
            static bool Added = false; // prevent adding of master thread for heartbeat check multiple times
            if (!Added) {
                //Add master Thread for heart beat monitoring
                AddControllerForHeartBeatCheck(this);
            }
            if (!m_PendingForHeartBeatCheck.isEmpty()) {
                //Add pending thread
                QListIterator<const BaseThreadController *>  Itr(m_PendingForHeartBeatCheck);
                while (Itr.hasNext()) {
                    AddControllerForHeartBeatCheck(Itr.next());
                }
                m_PendingForHeartBeatCheck.clear();
            }
        }
    }
    else {
        m_PendingForHeartBeatCheck.append(p_BaseThreadController);
    }
}


/****************************************************************************/
void MasterThreadController::StopSpecificThreadController(const quint32 ControllerNumber, const bool BasicThreadController) {
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

        Threads::ThreadController *pController = ((Threads::ThreadController*)(ControllerPair.first));
        quint32 ThreadID = pController->GetThreadID();
        mp_HeartBeatThreadController->RemoveControllerForHeartBeatCheck(ThreadID);

        ControllerPair.first->DoCleanupAndDestroyObjects();
        // block all the signals
        ControllerPair.second->blockSignals(true);
        ControllerPair.second->quit();
        ControllerPair.second->wait(2000);
    }
}

/****************************************************************************/
void MasterThreadController::RemoveSpecificThreadController(const quint32 ControllerNumber,
                                                            CommandChannel *p_Channel,
                                                            const bool BasicThreadController) {
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
        // remove the channel from the broadcast list
        if (p_Channel) {
            if (m_BroadcastChannels.contains(p_Channel)) {
                m_BroadcastChannels.remove(m_BroadcastChannels.indexOf(p_Channel));
            }
        }
        // destroy all the objects
        delete ControllerPair.first;
        delete ControllerPair.second;

    }
}

void MasterThreadController::StartSpecificThreadController(const quint32 ControllerNumber) {
    bool ThreadFound = false;
    tControllerPair ControllerPair;

    if (m_ControllerMap.contains(ControllerNumber)) {
        ControllerPair = m_ControllerMap.value(ControllerNumber);
        ThreadFound = true;
    }

    if (ThreadFound) {
        ControllerPair.first->CreateAndInitializeObjects();
        ControllerPair.second = new QThread();
        m_ControllerMap.insert(ControllerNumber, ControllerPair);
        ControllerPair.first->moveToThread(ControllerPair.second);
        CONNECTSIGNALSLOT(ControllerPair.second, started(), ControllerPair.first, Go());
        ControllerPair.second->start();
    }
}

void MasterThreadController::CreateRebootFile(QFile *p_BootConfigFile) {
    if (p_BootConfigFile) {
        if(!p_BootConfigFile->open(QIODevice::ReadWrite | QIODevice::Text)) {
            //!< todo raise event.
            qDebug()<<"bootConfig file open failed";
        }
        QTextStream BootConfigFileStream(p_BootConfigFile);
        BootConfigFileStream.setFieldAlignment(QTextStream::AlignLeft);
        BootConfigFileStream << "Main_Rebooted:" << "No" << "\n" << left;
        BootConfigFileStream << "Reboot_Count:" << "0" << "\n" << left;
        BootConfigFileStream << "Software_Update_Status:" << "NA" << "\n" << left;
        BootConfigFileStream << "PowrFailed:"<< "No" << "\n" << left;
        p_BootConfigFile->close();
    }
}

void MasterThreadController::ReadBootConfigFile(QFile *p_BootConfigFile) {
    if (p_BootConfigFile) {
        if(!p_BootConfigFile->open(QIODevice::ReadWrite | QIODevice::Text)) {
            //!< todo raise event.
            qDebug()<<"Reboot file open failed";
        }
        QString Line;
        QTextStream BootConfigFileStream(p_BootConfigFile);
        do {
            Line = BootConfigFileStream.readLine().simplified();
            QString RebootCount("0");
            if (Line.contains("Main_Rebooted", Qt::CaseInsensitive)) {
                QStringList LineFields = Line.split(":", QString::SkipEmptyParts);
                if (LineFields.count() == 2) {
                    (LineFields[1] == "Yes") ? (m_MainRebooted = true) : (m_MainRebooted = false);
                }
                m_BootConfigFileContent.insert("Main_Rebooted", LineFields[1]);
            }
            else if (Line.contains("Reboot_Count", Qt::CaseInsensitive)) {
                QStringList LineFields = Line.split(":", QString::SkipEmptyParts);
                if (LineFields.count() == 2) {
                    RebootCount = LineFields[1];
                    m_RebootCount = RebootCount.toUInt();
                    m_BootConfigFileContent.insert("Reboot_Count", QString::number(m_RebootCount));
                }

            }
            else if (Line.contains("Software_Update_Status", Qt::CaseInsensitive)) {
                QStringList LineFields = Line.split(":", QString::SkipEmptyParts);
                if (LineFields.count() == 2) {
                    m_SWUpdateStatus = LineFields[1];
                }
                if (LineFields[1] == "Success" || LineFields[1] == "Failure" ||
                        LineFields[1] == "HigherVersionNA") {
                    // Skip softswitch if software update was success/failure/No update was performed
                    m_MainRebooted = true;
                }
                if (m_SWUpdateStatus != "Success") { //For Success, status is updated after updating Rollback
                    //Reset the status
                    m_BootConfigFileContent.insert("Software_Update_Status", "NA");
                }
            }
            else if (Line.contains("PowerFailed", Qt::CaseInsensitive)) {
                QStringList LineFields = Line.split(":", QString::SkipEmptyParts);
                if (LineFields.count() == 2) {
                    if (LineFields[1] == "Yes") {
                        // Skip softswitch if system is recovering from power failure
                        m_MainRebooted = true;
                    }
                }
                m_BootConfigFileContent.insert("PowerFailed", LineFields[1]);
            }
        } while (!Line.isNull());

        p_BootConfigFile->close();
    }
}


void MasterThreadController::AlarmTest()
{
    connect(this, SIGNAL(PlayAlarmTone(bool, quint8, quint8)),
            &Global::AlarmPlayer::Instance(), SLOT(playTestTone(bool, quint8, quint8)));
    DataManager::CUserSettingsInterface *p_UserSettingsInterface = mp_DataManagerBase->GetUserSettingsInterface();
    if (p_UserSettingsInterface) {
        DataManager::CUserSettings *p_Settings = p_UserSettingsInterface->GetUserSettings();
        if (p_Settings) {
            quint32 Count = 0;
            do {
                emit PlayAlarmTone(false, 6, p_Settings->GetSoundNumberError());
                Count++;
            } while (ALARM_REPEAT_MAX > Count);
        }
    }
}

void MasterThreadController::SendEventReportToGUI() {
    if (m_RaiseGUIEvent) {
        Global::EventObject::Instance().RaiseEvent(EVENT_GUI_AVAILABLE, m_GUIEventStatus);
    }
}

void MasterThreadController::AddControllerForHeartBeatCheck(const Threads::BaseThreadController *p_Controller){
    //heartbeat thread itself is not monitored
    if (mp_HeartBeatThreadController != p_Controller) {
        try {
            const quint32 ThreadID = p_Controller->GetThreadID();
            CONNECTSIGNALSLOT(p_Controller, HeartbeatSignal(quint32), mp_HeartBeatThreadController, HeartBeatSlot(quint32));
            mp_HeartBeatThreadController->DontCheckHeartBeat(true);
            //Stop master heartbeat timer
            //Stop heart beat timers of threadcontroller
            StopHeartbeatTimer();
            emit SigHeartBeatTimerStop();
            emit AddControllerForHeartBeatCheck(ThreadID);
            //Restart heart beat timers
            mp_HeartBeatThreadController->DontCheckHeartBeat(false);
            StartHeartbeatTimer();
            emit SigHeartBeatTimerStart();
        }
        CATCHALL()
    }
}

void MasterThreadController::OnMissingHeartBeats(const QSet<quint32> Missing)
{
    //heart beats might be missing from many threads
    //no matter which heart beat is missing, ask user to shutdown using softswitch

    //heartbeat missing may come again and again, inform user only if not informed
    static bool InformedUserAlreadyOnGUI = false;
    static bool InformedUserAlreadyOnConsole = false;

    if((!Missing.contains(THREAD_ID_GUI)) && (m_GUIEventStatus)) {
        //send command to GUI asking user to shutdown the system if gpio thread controller is available
        if(!Missing.contains(THREAD_ID_GPIO_MANAGER)) {
            if(!InformedUserAlreadyOnGUI) {
                //log the event
                Global::EventObject().Instance().RaiseEvent(Global::EVENT_SHUTDOWN_REQUEST_TOUSER, true);

                //broadcast this msg so that gui receives this.
                emit SendCmdToGui(Global::CommandShPtr_t(new NetCommands::CmdRequestShutDown(15000, false)));

                InformedUserAlreadyOnGUI = true;
            }
        }
        //gpio thread controller is not available, send command to GUI asking user to inform that system
        //will be shutdown.
        else {
            //system will be shut down after user acknowledges this message,
            //hence stop checking for heartbeat
            mp_HeartBeatThreadController->DontCheckHeartBeat(true);

            //log the event
            Global::EventObject().Instance().RaiseEvent(Global::EVENT_SHUTDOWN_ON_SYSTEM_ERROR, true);

            //broadcast this msg so that gui receives this.
            emit SendCmdToGui(Global::CommandShPtr_t(new NetCommands::CmdRequestShutDown(15000, true)));
        }
    }
    //GUI is not available
    else {
        //ask user to shutdown the system using console if gpio thread controller is available
        if(!Missing.contains(THREAD_ID_GPIO_MANAGER)) {
            if(!InformedUserAlreadyOnConsole) {
                //log the event
                Global::EventObject().Instance().RaiseEvent(Global::EVENT_SHUTDOWN_REQUEST_TOUSER, true);

                std::cout << "Heart beat missing for one or more threads. Please shutdown using softswitch";

                InformedUserAlreadyOnConsole = true;
            }
        }
        // inform user through console and shutdown the system
        else {
            //stop checking for heartbeat
            mp_HeartBeatThreadController->DontCheckHeartBeat(true);

            //log the event
            Global::EventObject().Instance().RaiseEvent(Global::EVENT_SHUTDOWN_ON_SYSTEM_ERROR, true);

            std::cout << "Heart beat missing for one or more threads. Shutting down the system..";

            //shutdown the system
            InitiateShutdown();
        }
    }
}



} // end namespace Threads
