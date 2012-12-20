/****************************************************************************/
/*! \file ExternalProcessController.cpp
 *
 *  \brief Implementation file for class ExternalProcessController.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 07.12.2010
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

#include <ExternalProcessController/Include/ExternalProcessController.h>
#include <ExternalProcessController/Include/ConfigLoader.h>
#include <Global/Include/Utils.h>
#include <Global/Include/SystemPaths.h>
#include <ExternalProcessController/Include/ExternalProcessStates/CommunicationRetry.h>
#include <ExternalProcessController/Include/ExternalProcessStates/ExtProcessStartRetry.h>
#include <ExternalProcessController/Include/ExternalProcessStates/FatalError.h>
#include <ExternalProcessController/Include/ExternalProcessStates/Final.h>
#include <ExternalProcessController/Include/ExternalProcessStates/Initial.h>
#include <ExternalProcessController/Include/ExternalProcessStates/Working.h>
#include <ExternalProcessController/Include/ExternalProcessStates/WaitState.h>

namespace ExternalProcessControl {

const CreatorFunctorShPtr_t     NullCreatorFunctor(NULL);   ///< NULL functor.

/****************************************************************************/
/**
 * \brief Constructor.
 *
 * \param[in]   prname - the name of the process
 * \param[in]   TheHeartBeatSource - Logging source to be used.
 */
/****************************************************************************/
ExternalProcessController::ExternalProcessController(const QString &prname, Global::gSourceType TheHeartBeatSource) :
    Threads::ThreadController(TheHeartBeatSource, prname),
    m_myDevice(NULL),
    m_myProcess(NULL),
    m_myCommand(""),
    m_ProcessName(prname),
    m_RemoteLoginEnabled(""),
    m_RemoteLoginTimeout(-1),
    m_myStateManager(NULL),
    m_InitialState(NULL),
    m_ExtProcessStartRetryState(NULL),
    m_FatalErrorState(NULL),
    m_WorkingState(NULL),
    m_CommunicationRetryState(NULL),
    m_FinalState(NULL),
    m_WaitState(NULL),
    m_RestartProcess(true)
{
    qDebug() << "ExternalProcessController::ExternalProcessController" << prname;
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
ExternalProcessController::~ExternalProcessController()
{
    try {
        DeleteActiveObjects();
    } catch (...) {
        // to please PCLint...
    }
}

/****************************************************************************/
/**
 * \brief This function destroys all created objects.
 */
/****************************************************************************/
void ExternalProcessController::DeleteActiveObjects()
{
    DeleteActiveStateObjects();
    if (m_myProcess != NULL) {
        delete m_myProcess;
    }
    m_myProcess = NULL;
    m_myDevice = NULL;
}

/****************************************************************************/
/**
 * \brief This function destroys all created state objects.
 */
/****************************************************************************/
void ExternalProcessController::DeleteActiveStateObjects()
{
    try {
        delete m_myStateManager;
        m_myStateManager = NULL;
        delete m_InitialState;
        m_InitialState = NULL;
        delete m_ExtProcessStartRetryState;
        m_ExtProcessStartRetryState = NULL;
        delete m_FatalErrorState;
        m_FatalErrorState = NULL;
        delete m_WorkingState;
        m_WorkingState = NULL;
        delete m_CommunicationRetryState;
        m_CommunicationRetryState = NULL;
        delete m_FinalState;
        m_FinalState = NULL;
        delete m_WaitState;
        m_WaitState = NULL;
    } catch (...) {
        // to please PCLint...
    }
}

/****************************************************************************/
/**
 * \brief Register ExternalProcessDevice.
 *
 *        ExternalProcessDevice shall be created by the object of the derived class.
 *
 * \param[in]    gd = pointer to the ExternalProcessDevice object
 *
 * \warning
 *    call this function in the derived calss before calling the
 *    CreateAndInitializeObjects() of base class!
 */
/****************************************************************************/
void ExternalProcessController::RegisterExternalProcessDevice(ExternalProcessDevice *gd)
{
    // check for NULL pointer
    CHECKPTR(gd);
    // set pointer
    m_myDevice = gd;
}

/****************************************************************************/
/**
 * \brief Create and configure needed objects.
 *
 * If something goes wrong, the already created objects must be deleted.
 *
 * \warning
 *    call RegisterExternalProcessDevice function in the derived calss before calling
 *    CreateAndInitializeObjects() of base class!
 */
/****************************************************************************/
void ExternalProcessController::CreateAndInitializeObjects() {
    if (!LoadSettings()) {
        LOGANDTHROW(EVENT_EXTERNALPROCESSCONTROL_ERROR_LOADING_SETTINGS);
    }

    if (!InitializeExternalProcessDevice(m_RemoteLoginEnabled, m_RemoteLoginTimeout)) {
        LOGANDTHROW(EVENT_EXTERNALPROCESSCONTROL_ERROR_INITIALIZE_EXTERNAL_PROCESS_DEVICE);
    }

    if (m_RemoteLoginEnabled == "Yes") {

        // If remote login is enabled, there is no need for ProcessManager.
        // We just need a simplified State Machine and ExternalProcessDevice.

        if (!InitializeSimplifiedStateMachine()) {
            LOGANDTHROW(EVENT_EXTERNALPROCESSCONTROL_ERROR_INITIALIZE_SIMPLE_STATE_MACHINE);
        }
    } else {

        // full initialization for local process management.

        if (!InitializeFullStateMachine()) {
            LOGANDTHROW(EVENT_EXTERNALPROCESSCONTROL_ERROR_INITIALIZE_FULL_STATE_MACHINE);
        }

        if (!InitializeProcessManager()) {
            LOGANDTHROW(EVENT_EXTERNALPROCESSCONTROL_ERROR_INITIALIZE_PROCESS_MANAGER);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot is called when ExternalProcess terminates.
 *
 *  \param[in]    name = internal name of the process
 *  \param[in]    code = process exit code
 *
 ****************************************************************************/
void ExternalProcessController::ExternalProcessExited(const QString &name, int code)
{
    Q_UNUSED(name)
    Q_UNUSED(code)
    qDebug() << "\nExternalProcessController: ExternalProcessExited called.\n";

    CHECKPTR(m_myStateManager);
    // some process on demand it starts and exits, so we check a flag whether process need to be restarted or not
    // depending on that we set the state manager
    if (m_RestartProcess) {
        if (!m_myStateManager->DispatchEvent(Global::AsInt(EP_EXTPROCESS_EXITED))) {
            /// \todo do we handle error here?
            return;
        }
    }
    else {
        // tell the process on stop received so we are closing the process
        if (!m_myStateManager->DispatchEvent(Global::AsInt(EP_ONSTOP_RECEIVED))) {
            /// \todo do we handle error here?
            return;
        }
    }
    // this is to forward the signal to derived class
    emit ProcessExited(name, code);
}

/****************************************************************************/
/*!
 *  \brief    This function loads Process settings from disk.
 *
 *
 *  \return   true if loading settings succeeded, false otherwise
 *
 ****************************************************************************/
bool ExternalProcessController::LoadSettings()
{
    qDebug() << "\nExternalProcessController: loading settings...\n";

    ConfigLoader cfgLdr(m_ProcessName);

    if (cfgLdr.ReadSettings(&m_myCommand, &m_RemoteLoginEnabled, &m_RemoteLoginTimeout) != CLD_ALL_OK)
    {
        /// \todo do we handle error here?
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief    This slot is called when external process successfully starts.
 *
 *  \param[in]    name = internal name of the process
 *
 ****************************************************************************/
void ExternalProcessController::ExternalProcessStarted(const QString &name)
{
    Q_UNUSED(name)
    /*! \warning: this does not mean the ExternalProcess has started. The "nice" command starts
       first and this will be reported as success. BUT, if the parameters passed to "nice"
       are wrong, the actual ExternalProcess process will not start, it will be reported as nice's
       process error via ExternalProcessExited slot.
     */
    // start timer to check if this slot call will be followed by ExternalProcessExited:
    // m_StartupGuardTimer.start(EXTPROCESS_GUARD_TIME);
}

/****************************************************************************/
/*!
 *  \brief    This slot is called when external process reports an error.
 *
 *  \param[in]    err = the error which occured
 *
 ****************************************************************************/
void ExternalProcessController::ExternalProcessError(int err)
{
    /// \todo evaluate error
    qDebug() << (QString)("ExternalProcessController: ext.process reported error: " + QString::number(err, 10) + " !");
}

/****************************************************************************/
/*!
 *  \brief Initialize the ExternalProcessDevice to handle inter-process communication.
 *
 *  \param[in]  remoteLoginEnabled = remote login enabled or not
 *  \param[in]  remoteLoginTimeout = timeout for remote login if it is enabled
 *
 *  \return     true if success, false otherwise
 */
/****************************************************************************/
bool ExternalProcessController::InitializeExternalProcessDevice(const QString &remoteLoginEnabled, int remoteLoginTimeout)
{
    try {
        if (m_myDevice == NULL) {
            /// \todo log error
            qDebug() << "ExternalProcessController: my ExternalProcessDevice pointer is NULL !";
            return false;
        }

        if (!m_myDevice->InitializeDeviceWithParameters(remoteLoginEnabled, remoteLoginTimeout)) {
            /// \todo log error
            qDebug() << "ExternalProcessController: Cannot initialize ExternalProcessDevice!";
            return false;
        }
        CONNECTSIGNALSLOT(m_myDevice, SigPeerConnected(const QString &), this, ExternalProcessConnected(const QString &));
        CONNECTSIGNALSLOT(m_myDevice, SigPeerDisconnected(const QString &), this, ExternalProcessDisconnected(const QString &));
        CONNECTSIGNALSLOT(m_myDevice, SigLoginTimeout(), this, LoginTimeout());
    } catch (...) {
        return false;
    }

    // everything OK
    return true;
}

/****************************************************************************/
/*!
 *  \brief Initialize the External Process Manager to handle external process.
 *
 *
 *  \return  true if success, false otherwise
 */
/****************************************************************************/
bool ExternalProcessController::InitializeProcessManager()
{
    qDebug() << "ExternalProcessController: initializing ExternalProcess Manager...";

    try {

        // check if process already exists and delete it if it does
        if (m_myProcess != NULL) {
            delete m_myProcess;
        }

        /// \todo set correct subsource:
        m_myProcess = new ExternalProcess(m_ProcessName, this);
        m_myProcess->Initialize();
        // connect ProcessManager's start/exit/error signals:
        CONNECTSIGNALSLOT(m_myProcess, ProcessExited(const QString &, int), this, ExternalProcessExited(const QString &, int));
        CONNECTSIGNALSLOT(m_myProcess, ProcessStarted(const QString &), this, ExternalProcessStarted(const QString &));
        CONNECTSIGNALSLOT(m_myProcess, ProcessError(int), this, ExternalProcessError(int));
    } catch(const std::bad_alloc &) {
        // send some error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
        return false;
    } catch(const Global::Exception &E) {
        // send error message
        Global::EventObject::Instance().RaiseException(E);
        return false;
    } catch(...) {
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Initialize the full StateMachine of the ExternalProcessController.
 *
 *  \b Description: this state machine is used when external process
 *     is started and managed by Master. ExternalProcessController takes
 *     care of the local process management and network connection.
 *
 *  \return     true if success, false otherwise
 */
/****************************************************************************/
bool ExternalProcessController::InitializeFullStateMachine()
{
    qDebug() << "ExternalProcessController: initializing StateMachine...";
    try {
        // get rid of existing objects (if any)
        DeleteActiveStateObjects();
        // create the state manager
        m_myStateManager = new StateMachines::StateMachineEngine(this);
        // create all possible states
        m_InitialState = new Initial(this);
        m_ExtProcessStartRetryState = new ExtProcessStartRetry(this);
        m_FatalErrorState = new FatalError(this);
        m_WorkingState = new Working(this);
        m_CommunicationRetryState = new CommunicationRetry(this);
        m_FinalState = new Final(this);
        // add all possible states to the engine
        m_myStateManager->AddState(m_InitialState);
        m_myStateManager->AddState(m_ExtProcessStartRetryState);
        m_myStateManager->AddState(m_FatalErrorState);
        m_myStateManager->AddState(m_WorkingState);
        m_myStateManager->AddState(m_CommunicationRetryState);
        m_myStateManager->AddState(m_FinalState);
        // add transitions from Initial state to other states
        m_myStateManager->AddTransition(m_InitialState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_NULL_CTRL_POINTER);
        m_myStateManager->AddTransition(m_InitialState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_CANNOT_START_DEVICE);
        m_myStateManager->AddTransition(m_InitialState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_CANNOT_KILL_EXTPROCESS);
        m_myStateManager->AddTransition(m_InitialState, m_ExtProcessStartRetryState, (StateMachines::StateEventIndexType_t)EP_CANNOT_START_EXTPROCESS);
        m_myStateManager->AddTransition(m_InitialState, m_ExtProcessStartRetryState, (StateMachines::StateEventIndexType_t)EP_EXTPROCESS_EXITED);
        m_myStateManager->AddTransition(m_InitialState, m_WorkingState, (StateMachines::StateEventIndexType_t)EP_EXTPROCESS_CONNECTED);
        m_myStateManager->AddTransition(m_InitialState, m_FinalState, (StateMachines::StateEventIndexType_t)EP_POWERDOWN);
        // add transitions from Working state to other states
        m_myStateManager->AddTransition(m_WorkingState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_NULL_CTRL_POINTER);
        m_myStateManager->AddTransition(m_WorkingState, m_FinalState, (StateMachines::StateEventIndexType_t)EP_POWERDOWN);
        m_myStateManager->AddTransition(m_WorkingState, m_FinalState, (StateMachines::StateEventIndexType_t)EP_ONSTOP_RECEIVED);
        m_myStateManager->AddTransition(m_WorkingState, m_CommunicationRetryState, (StateMachines::StateEventIndexType_t)EP_EXTPROCESS_DISCONNECTED);
        m_myStateManager->AddTransition(m_WorkingState, m_ExtProcessStartRetryState, (StateMachines::StateEventIndexType_t)EP_EXTPROCESS_EXITED);
        // add transitions from ExtProcessStartRetry state to other states
        m_myStateManager->AddTransition(m_ExtProcessStartRetryState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_NULL_CTRL_POINTER);
        m_myStateManager->AddTransition(m_ExtProcessStartRetryState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_CANNOT_START_EXTPROCESS);
        m_myStateManager->AddTransition(m_ExtProcessStartRetryState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_CANNOT_KILL_EXTPROCESS);
        m_myStateManager->AddTransition(m_ExtProcessStartRetryState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_EXTPROCESS_RESTART_FAILED);
        m_myStateManager->AddTransition(m_ExtProcessStartRetryState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_SIGNALCONNECT_FAILED);
        m_myStateManager->AddTransition(m_ExtProcessStartRetryState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_TOO_MANY_RESTARTS);
        m_myStateManager->AddTransition(m_ExtProcessStartRetryState, m_WorkingState, (StateMachines::StateEventIndexType_t)EP_EXTPROCESS_CONNECTED);
        m_myStateManager->AddTransition(m_ExtProcessStartRetryState, m_FinalState, (StateMachines::StateEventIndexType_t)EP_POWERDOWN);
        // add transitions from CommunicationRetry state to other states
        m_myStateManager->AddTransition(m_CommunicationRetryState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_NULL_CTRL_POINTER);
        m_myStateManager->AddTransition(m_CommunicationRetryState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_CANNOT_KILL_EXTPROCESS);
        m_myStateManager->AddTransition(m_CommunicationRetryState, m_ExtProcessStartRetryState, (StateMachines::StateEventIndexType_t)EP_EXTPROCESS_EXITED);
        m_myStateManager->AddTransition(m_CommunicationRetryState, m_WorkingState, (StateMachines::StateEventIndexType_t)EP_EXTPROCESS_CONNECTED);
        m_myStateManager->AddTransition(m_CommunicationRetryState, m_FinalState, (StateMachines::StateEventIndexType_t)EP_POWERDOWN);
    } catch(const std::bad_alloc &) {
        // send some error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
        return false;
    } catch(const Global::Exception &E) {
        // send error message
        Global::EventObject::Instance().RaiseException(E);
        return false;
    } catch(...) {
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Initialize the simple StateMachine of the ExternalProcessController.
 *
 *  \b Description: this state machine is used when external process
 *     is not started and managed by Master. ExternalProcessController just
 *     takes care of the network connection in this case.
 *
 *  \return     true if success, false otherwise
 */
/****************************************************************************/
bool ExternalProcessController::InitializeSimplifiedStateMachine()
{
    qDebug() << "ExternalProcessController: initializing StateMachine...";
    try {
        // get rid of existing objects (if any)
        DeleteActiveStateObjects();
        // create the state manager
        m_myStateManager = new StateMachines::StateMachineEngine(this);
        // create all possible states
        m_FatalErrorState = new FatalError(this);
        m_WorkingState = new Working(this);
        m_FinalState = new Final(this);
        m_WaitState = new WaitState(this);

        // add all possible states to the engine
        m_myStateManager->AddState(m_WaitState);
        m_myStateManager->AddState(m_FatalErrorState);
        m_myStateManager->AddState(m_WorkingState);
        m_myStateManager->AddState(m_FinalState);
        // add transitions from WaitState to other states
        m_myStateManager->AddTransition(m_WaitState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_NULL_CTRL_POINTER);
        m_myStateManager->AddTransition(m_WaitState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_CANNOT_START_DEVICE);
        m_myStateManager->AddTransition(m_WaitState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_EXTPROCESS_LOGIN_TIMEOUT);
        m_myStateManager->AddTransition(m_WaitState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_TOO_MANY_RESTARTS);
        m_myStateManager->AddTransition(m_WaitState, m_FinalState, (StateMachines::StateEventIndexType_t)EP_POWERDOWN);
        m_myStateManager->AddTransition(m_WaitState, m_WorkingState, (StateMachines::StateEventIndexType_t)EP_EXTPROCESS_CONNECTED);
        // add transitions from Working state to other states
        m_myStateManager->AddTransition(m_WorkingState, m_FatalErrorState, (StateMachines::StateEventIndexType_t)EP_NULL_CTRL_POINTER);
        m_myStateManager->AddTransition(m_WorkingState, m_FinalState, (StateMachines::StateEventIndexType_t)EP_POWERDOWN);
        m_myStateManager->AddTransition(m_WorkingState, m_FinalState, (StateMachines::StateEventIndexType_t)EP_ONSTOP_RECEIVED);
        m_myStateManager->AddTransition(m_WorkingState, m_WaitState, (StateMachines::StateEventIndexType_t)EP_EXTPROCESS_DISCONNECTED);
    } catch(const std::bad_alloc &) {
        // send some error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
        return false;
    } catch(const Global::Exception &E) {
        // send error message
        Global::EventObject::Instance().RaiseException(E);
        return false;
    } catch(...) {
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief    This slot is called when external ExternalProcess connects.
 *
 *  \param[in]    str = ExternalProcess name
 *
 ****************************************************************************/
void ExternalProcessController::ExternalProcessConnected(const QString &str)
{
    qDebug() << "ExternalProcessController: peer connected with string: " + str;

    CHECKPTR(m_myStateManager);
    if (!m_myStateManager->DispatchEvent(Global::AsInt(EP_EXTPROCESS_CONNECTED))) {
        /// \todo do we handle error here?
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot is called when external ExternalProcess disconnects.
 *
 *  \param[in]    str = ExternalProcess name
 *
 ****************************************************************************/
void ExternalProcessController::ExternalProcessDisconnected(const QString &str)
{
    qDebug() << "ExternalProcessController: peer disconnected with string: " + str;

    CHECKPTR(m_myStateManager);
    if (!m_myStateManager->DispatchEvent(Global::AsInt(EP_EXTPROCESS_DISCONNECTED))) {
        /// \todo do we handle error here?
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief    This slot is called when external ExternalProcess login times out.
 *
 *
 ****************************************************************************/
void ExternalProcessController::LoginTimeout()
{
    qDebug() << "ExternalProcessController: LoginTimeout called !";

    CHECKPTR(m_myStateManager);
    if (!m_myStateManager->DispatchEvent(Global::AsInt(EP_EXTPROCESS_LOGIN_TIMEOUT))) {
        /// \todo do we handle error here?
        return;
    }
}

/****************************************************************************/
/**
 * \brief Cleanup and destroy the created objects.
 */
/****************************************************************************/
void ExternalProcessController::CleanupAndDestroyObjects() {
    // destroy all objects
    /// \todo implement
}

/****************************************************************************/
/**
 * \brief This method is called when the base class received the \ref Go signal.
 *
 * This means that everything is fine and normal operation started.
 * We are running in our own thread now.\n
 */
/****************************************************************************/
void ExternalProcessController::OnGoReceived() {
    /// \todo implement

    CHECKPTR(m_myStateManager);
    if (m_RemoteLoginEnabled == "No") {
        if (m_InitialState == NULL) {
            /// \todo log error
            return;
        }
        if (!m_myStateManager->Start(m_InitialState->GetName(), Global::AsInt(EP_START_OPERATION))) {
            /// \todo log error
        }
    }
    else {
        if (m_WaitState == NULL) {
            /// \todo log error
            return;
        }
        if (!m_myStateManager->Start(m_WaitState->GetName(), Global::AsInt(EP_START_OPERATION))) {
            /// \todo log error
        }
    }
}

/****************************************************************************/
/**
 * \brief This method is called when the base class received the \ref Stop signal.
 *
 * This means that normal operation will stop after processing this signal.
 * We are still running in our own thread.\n
 */
/****************************************************************************/
void ExternalProcessController::OnStopReceived() {
    // at this point the StateMachine shall be in "Final" state and
    // the GUI shall be already "down": final screen shall be shown to the user.
    // all we need to do is kill the device and kill the GUI.

    // stop state Engine:
    CHECKPTR(m_myStateManager);
    // kill external process:
    if (m_myProcess != NULL) {
        static_cast<void> (
        // cannot use return value here anyway
                m_myProcess->KillProcess()
        );
    }

    // destroy device:
    if (m_myDevice != NULL) {
        m_myDevice->DestroyDevice();
    }
}

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
bool ExternalProcessController::LocalProcessErrorEvent(const DataLogging::DayEventEntry & /*TheEventEntry*/) {
    // no processing
    return false;
}

/****************************************************************************/
/**
 * \brief Register a creator functor.
 *
 * Register a creator functor for a specific class.
 *
 * \param[in]   ClassName   Name of class.
 * \param[in]   Functor     Shared pointer of functor to register.
 */
/****************************************************************************/
void ExternalProcessController::RegisterCreatorFunctor(const QString &ClassName, const CreatorFunctorShPtr_t &Functor) {
    // check if already registered
    if(m_CreatorFunctors.contains(ClassName)) {
        LOGANDTHROWARGS(EVENT_EXTERNALPROCESSCONTROL_ERROR_CREATOR_FUNCTOR_ALREADY_REGISTERED, ClassName);
    }
    // everything OK
    static_cast<void>(
        // we DO NOT NEED the return value of insert
        m_CreatorFunctors.insert(ClassName, Functor)
    );
}

/****************************************************************************/
/**
 * \brief Get the creator functor for a specific class.
 *
 * Get the creator functor for a specific class. If the functor is not found
 * a NullFunctor is returned.
 *
 * \param[in]   ClassName   Name of class.
 * \return                  The functor or NullFunctor if not found.
 */
/****************************************************************************/
CreatorFunctorShPtr_t ExternalProcessController::GetCreatorFunctor(const QString &ClassName) const {
    CreatorFunctorHash_t::const_iterator it = m_CreatorFunctors.find(ClassName);
    if(it == m_CreatorFunctors.constEnd()) {
        // functor not found
        // return NULL functor
        return NullCreatorFunctor;
    }
    // return functor
    return it.value();
}

/****************************************************************************/
/**
 * \brief Create a class instance which was serialized in a byte array.
 *
 * Create a class instance which was serialized in a byte array. The creator
 * functor for this class is called (creates class and calls processing function)
 *
 * \param[in]   Data    Byte array in which class is serialized.
 */
/****************************************************************************/
void ExternalProcessController::CreateAndDeserialize(const QByteArray &Data) {
//    qDebug() << "ExternalProcessController::CreateAndDeserialize";
    // create data stream in readonly mode
    QDataStream DS(Data);
    // set version
    DS.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    // read class name
    QString ClassName;
    DS >> ClassName;
    // read Ref
    Global::tRefType Ref = 0;
    DS >> Ref;

    // OK, now get functor and execute
    CreatorFunctorShPtr_t Functor = GetCreatorFunctor(ClassName);
    if(Functor == NullCreatorFunctor) {
        // throw exception
        LOGANDTHROWARG(EVENT_EXTERNALPROCESSCONTROL_ERROR_NO_CREATOR_FUNCTOR_REGISTERED, ClassName);
    }
    // execute
    Functor.GetPointerToUserData()->CreateAndDeserialize(Ref, DS);
}

/****************************************************************************/
/*!
 *  \brief  This method forwards incoming message to internal recepient(s).
 *
 *  \param[in] bArr - serialized command
 *
 *  \return  true if success, false otherwise
 */
/****************************************************************************/
bool ExternalProcessController::ForwardMsgToRecepient(const QByteArray &bArr)
{
    try {
        CreateAndDeserialize(bArr);
    } catch(const Global::Exception &E) {
        // send error message
        Global::EventObject::Instance().RaiseException(E);
        return false;
    } catch(...) {
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
        return false;
    }
    // no exceptions --> return true
    return true;
}

} // end namespace ExternalProcessControl

