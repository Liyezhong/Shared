/****************************************************************************/
/*! \file Threads/Source/BaseThreadController.cpp
 *
 *  \brief Implementation file for class BaseThreadController.
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

#include <Threads/Include/BaseThreadController.h>
#include <Threads/Include/ThreadsEventCodes.h>

#include <QThread>
#include <QDebug>
#include <QMetaType>
#include <unistd.h>
#include <sys/syscall.h> //for syscall()
namespace Threads {

const Global::gSubComponentType SUBCOMPONENT_THREADCONTROLLER = 0x0000; ///< Subcomponent type for thread controller.

/**
* \brief function NullAcknowledgeProcessorFunctor
* \return AcknowledgeProcessorFunctorShPtr_t
*/
const AcknowledgeProcessorFunctorShPtr_t    NullAcknowledgeProcessorFunctor(NULL);  ///< NULL functor for acknowledge processing.

const TimeoutProcessorFunctorShPtr_t        NullTimeoutProcessorFunctor(NULL);      ///< NULL functor for timeout processing.

const CmdDataChangedFunctorShPtr_t          NullDataChangedFunctor(NULL);           ///< NULL functor for data changed.

/**
* \brief function STOP_TIMEOUT
* \return int
*/
const int STOP_TIMEOUT(1000);         ///< time to wait if thread get's stuck in OnStopReceived()

/****************************************************************************/
BaseThreadController::BaseThreadController(const quint32 ThreadID, const QString ThreadName) :
    m_HeartbeatTimeout(0),
    mp_HeartbeatTimer(NULL),
    m_ThreadID(ThreadID),
    m_ThreadName(ThreadName)

{
    // register commands and acknowledges
    qRegisterMetaType<Global::tRefType>("Global::tRefType");
    qRegisterMetaType<Global::CommandShPtr_t>("Global::CommandShPtr_t");
    qRegisterMetaType<Global::AcknowledgeShPtr_t>("Global::AcknowledgeShPtr_t");
    // connect timer

    // disable error forwarding because we do not have a parent
    //DisableErrorForwarding();
}

/****************************************************************************/
BaseThreadController::~BaseThreadController() {
}

/****************************************************************************/
void BaseThreadController::StartHeartbeatTimer() {
    QMutexLocker Lock(&m_Lock);
    // only start if needed.
    if(m_HeartbeatTimeout > 0) {
        mp_HeartbeatTimer->start(m_HeartbeatTimeout);
    }
}

/****************************************************************************/
void BaseThreadController::StopHeartbeatTimer() {
    QMutexLocker Lock(&m_Lock);
    mp_HeartbeatTimer->stop();
}

/****************************************************************************/
void BaseThreadController::HeartbeatTimer() {
    emit HeartbeatSignal(GetThreadID());
}

/****************************************************************************/
void BaseThreadController::Go() {
    try {
        mp_HeartbeatTimer = new QTimer(this);
        CONNECTSIGNALSLOT(mp_HeartbeatTimer, timeout(), this, HeartbeatTimer());
        emit ThreadControllerStarted(this);
        // call processing method
        OnGoReceived();
        Global::EventObject::Instance().LogThreadId(m_ThreadName , syscall(SYS_gettid));
    }
    CATCHALL();
}

/****************************************************************************/
void BaseThreadController::Stop() {
    try {
        QMutexLocker locker(&m_StopMutex);
        // call processing method
        OnStopReceived();
        // stop heartbeat timer
        StopHeartbeatTimer();
    }
    CATCHALL();

    // quit thread
    QThread::currentThread()->quit();
}

/****************************************************************************/
void BaseThreadController::DoCleanupAndDestroyObjects() {
    m_StopMutex.tryLock(STOP_TIMEOUT);
    CleanupAndDestroyObjects();
    m_StopMutex.unlock();
}

/****************************************************************************/
void BaseThreadController::SetHeartbeatTimeout(int Timeout) {
    // remember new requested value but do not alter timeout.
    QMutexLocker Lock(&m_Lock);
    m_HeartbeatTimeout = Timeout;
}

/****************************************************************************/
void BaseThreadController::DoSendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd, const CommandChannel &CmdChannel) {
    // check pointer
    try {
        CHECKPTR(Cmd.GetPointerToUserData());

        // try to send the command over the according transmit channel
//        qDebug() << "BaseThreadController::DoSendCommand" << Ref << Cmd.GetPointerToUserData()->GetName() << "Channel" << CmdChannel.m_channelName;
        if(Cmd->GetTimeout() != Global::Command::NOTIMEOUT) {
            // create descriptor
            Global::PendingCmdDescriptorShP_t PCDShP(new Global::PendingCmdDescriptor(this, Ref, Cmd->GetName(), Cmd->GetTimeout()));

            if (this->thread() != PCDShP.GetPointerToUserData()->thread())
            {
                qDebug() << "BaseThreadController::DoSendCommand, mismatch of threads" << Cmd.GetPointerToUserData()->GetName() << "current thread" << this->thread() << "PCDShP thread" << PCDShP.GetPointerToUserData()->thread();
            }

            // connect descriptor to timeout slot
            CONNECTSIGNALSLOT(PCDShP.GetPointerToUserData(), TimeoutOccured(Global::tRefType, QString),
                              this, OnProcessTimeoutSlot(Global::tRefType, QString));
            // connect and start descriptor timeouts
            PCDShP->ConnectAndStartTimer();
            // now add to list of pending commands.
//            qDebug() << "BaseThreadController::DoSendCommand, add to pending commands" << Ref;
            static_cast<void>(
                // we DO NOT NEED the return value of insert
                m_PendingCommands.insert(Ref, PCDShP)
            );
        }
        CmdChannel.EmitCommand(Ref, Cmd);
        // everything OK
        return;
    }
    CATCHALL();
}

/****************************************************************************/
void BaseThreadController::DoSendAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack, CommandChannel &CmdChannel)
{
//    qDebug() << "BaseThreadController::DoSendAcknowledge" << Ref << Ack.GetPointerToUserData()->GetName() << "Channel" << CmdChannel.m_channelName;
    // check pointer
    CHECKPTR(Ack.GetPointerToUserData());

    try {
        // try to send the command over the according transmit channel
        CmdChannel.EmitAcknowledge(Ref, Ack);
        // everything OK
        return;
    }
    CATCHALL();

// some error occured
//    LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, EVENT_THREADS_ERROR_SEND_ACKNOWLEDGE, Global::tTranslatableStringList() << Ack->GetName() << Ref,
//              Global::NO_NUMERIC_DATA, false);
    Global::EventObject::Instance().RaiseEvent(EVENT_THREADS_ERROR_SEND_ACKNOWLEDGE,  Global::tTranslatableStringList() << Ack->GetName() << Ref);
}

/****************************************************************************/
void BaseThreadController::RegisterAcknowledgeProcessorFunctor(const QString &AckName, const AcknowledgeProcessorFunctorShPtr_t &Functor) {
    // check if already registered
    if(m_AcknowledgeProcessorFunctors.contains(AckName)) {
        LOGANDTHROWARGS(EVENT_THREADS_ERROR_ACKNOWLEDGE_FUNCTOR_ALREADY_REGISTERED, AckName)
    }
    // everything OK
    static_cast<void>(
        // we DO NOT NEED the return value of insert
        m_AcknowledgeProcessorFunctors.insert(AckName, Functor)
    );
}

/****************************************************************************/
AcknowledgeProcessorFunctorShPtr_t BaseThreadController::GetAcknowledgeProcessorFunctor(const QString &AckName) const
{
//    qDebug() << "BaseThreadController::GetAcknowledgeProcessorFunctor" << AckName;
    AcknowledgeProcessorFunctorHash_t::const_iterator it = m_AcknowledgeProcessorFunctors.find(AckName);
    if(it == m_AcknowledgeProcessorFunctors.constEnd())
    {
        qDebug() << "BaseThreadController::GetAcknowledgeProcessorFunctor, no functor found for" << AckName;
        // functor not found
        // return NULL functor
        return NullAcknowledgeProcessorFunctor;
    }
    // return functor
    return it.value();
}

/****************************************************************************/
void BaseThreadController::RegisterTimeoutProcessingFunctor(const QString &CmdName, const TimeoutProcessorFunctorShPtr_t &Functor) {
    // check if already registered
    if(m_TimeoutProcessorFunctors.contains(CmdName)) {
        LOGANDTHROWARGS(EVENT_THREADS_ERROR_TIMEOUT_FUNCTOR_ALREADY_REGISTERED, CmdName)
    }
    // everything OK
    static_cast<void>(
        // we DO NOT NEED the return value of insert
        m_TimeoutProcessorFunctors.insert(CmdName, Functor)
    );
}

/****************************************************************************/
TimeoutProcessorFunctorShPtr_t BaseThreadController::GetTimeoutProcessorFunctor(const QString &CmdName) const {
    TimeoutProcessorFunctorHash_t::const_iterator it = m_TimeoutProcessorFunctors.find(CmdName);
    if(it == m_TimeoutProcessorFunctors.constEnd()) {
        // functor not found
        // return NULL functor
        return NullTimeoutProcessorFunctor;
    }
    // return functor
    return it.value();
}

/****************************************************************************/
void BaseThreadController::RegisterOnDataChangedProcessingFunctor(const QString &DataContainerName, const CmdDataChangedFunctorShPtr_t &Functor) {
    // check if already registered
    if(m_DataChangedFunctors.contains(DataContainerName)) {
        LOGANDTHROWARGS(EVENT_THREADS_ERROR_ACKNOWLEDGE_FUNCTOR_ALREADY_REGISTERED, DataContainerName)
    }
    // everything OK
    static_cast<void>(
        // we DO NOT NEED the return value of insert
        m_DataChangedFunctors.insert(DataContainerName, Functor)
    );
}

/****************************************************************************/
CmdDataChangedFunctorShPtr_t BaseThreadController::GetDataChangedProcessingFunctor(const QString &DataContainerName) const {
    CmdDataChangedFunctorHash_t::const_iterator it = m_DataChangedFunctors.find(DataContainerName);
    if(it == m_DataChangedFunctors.constEnd()) {
        // functor not found
        // return NULL functor
        return NullDataChangedFunctor;
    }
    // return functor
    return it.value();
}

/****************************************************************************/
void BaseThreadController::OnProcessAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack)
{
//    qDebug() << "BaseThreadController::OnProcessAcknowledge" << Ref << Ack.GetPointerToUserData()->GetName();
    try
    {
        CHECKPTR(Ack.GetPointerToUserData());

//        SEND_DEBUG(WHEREAMI + " " +
//                   QString("Ref = ") + QString::number(Ref, 10) +
//                   QString("Name = ") + Ack->GetName());
        if(!m_PendingCommands.contains(Ref)) {
            // unknown command reference. maybe already timed out.
//            qDebug() << "BaseThreadController::OnProcessAcknowledge, unknown command ref" << Ref;
            Global::EventObject::Instance().RaiseEvent(EVENT_THREADS_ERROR_UNKNOWN_COMMAND_REF, Global::FmtArgs() << Ref << m_ThreadID << Ack->GetName(), true);
//            LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_THREADS_ERROR_UNKNOWN_COMMAND_REF, Global::tTranslatableStringList() << QString::number(Ref, 10)
//                      , Global::NO_NUMERIC_DATA, false);
            return;
        }
        // Remove from list of blocked references
        UnblockCommandRef(Ref);
        // remove from list of pending commands
        RemoveFromPendingCommands(Ref);
        // OK, now get functor and execute
        AcknowledgeProcessorFunctorShPtr_t Functor = GetAcknowledgeProcessorFunctor(Ack->GetName());
        if(Functor == NullAcknowledgeProcessorFunctor) {
//            qDebug() << "BaseThreadController::OnProcessAcknowledge, NullAcknowledgeProcessorFunctor" << Ref;
            Global::EventObject::Instance().RaiseEvent(EVENT_THREADS_ERROR_UNSUPPORTED_ACKNOWLEDGE, Global::FmtArgs() << Ack->GetName() << m_ThreadID, true);
        }
        // check the pointer user data,sometimes it is becoming null and its randomly appears
        if (Functor.GetPointerToUserData()) {
            // execute
            Functor.GetPointerToUserData()->Process(Ref, Ack.GetPointerToUserData());
        }
    }
    CATCHALL();
}

/****************************************************************************/
void BaseThreadController::OnProcessTimeoutSlot(Global::tRefType Ref, QString CmdName) {
    // call processing routine
    OnProcessTimeout(Ref, CmdName);
}

/****************************************************************************/
void BaseThreadController::OnProcessTimeout(Global::tRefType Ref, const QString &CmdName) {
//    SEND_DEBUG(WHEREAMI + " " +
//               QString("Ref = ") + QString::number(Ref, 10) +
//               QString("Name = ") + CmdName);
    if(!m_PendingCommands.contains(Ref)) {
        // unknown command reference. maybe already processed?.
        qDebug() << "BaseThreadController::OnProcessTimeout, unknown command ref" << Ref;
        Global::EventObject::Instance().RaiseEvent(EVENT_THREADS_ERROR_UNKNOWN_COMMAND_REF, Global::FmtArgs() << Ref << m_ThreadID << CmdName, true);
//        LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_THREADS_ERROR_UNKNOWN_COMMAND_REF, QString::number(Ref, 10)
//                  , Global::NO_NUMERIC_DATA, false);
        return;
    }
    // send error
    Global::EventObject::Instance().RaiseEvent(EVENT_THREADS_ERROR_COMMAND_TIMEOUT, Global::FmtArgs() << CmdName << Ref, true);
//    LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, EVENT_THREADS_ERROR_COMMAND_TIMEOUT, Global::tTranslatableStringList() << CmdName << QString::number(Ref, 10)
//              , Global::NO_NUMERIC_DATA, false);
    // remove from list of pending commands

    RemoveFromPendingCommands(Ref);
    // OK, now get functor and execute
    TimeoutProcessorFunctorShPtr_t Functor = GetTimeoutProcessorFunctor(CmdName);
    if(Functor == NullTimeoutProcessorFunctor)
    {
        // take default OnCmdTimeout function
        OnCmdTimeout(Ref, CmdName);
    } else
    {
        // execute
        Functor.GetPointerToUserData()->Process(Ref, CmdName);
    }
}

/****************************************************************************/
void BaseThreadController::DispatchDataChangedCommand(const Global::CmdDataChanged &Cmd) const {
    // OK, now get functor and execute
    CmdDataChangedFunctorShPtr_t Functor = GetDataChangedProcessingFunctor(Cmd.GetDataContainerName());
    if(Functor == NullDataChangedFunctor) {
        // do nothing
    } else {
        // execute
        Functor.GetPointerToUserData()->Process();
    }
}

} // end namespace Threads
