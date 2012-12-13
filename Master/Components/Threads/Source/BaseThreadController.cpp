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

namespace Threads {

const Global::gSubComponentType SUBCOMPONENT_THREADCONTROLLER = 0x0000; ///< Subcomponent type for thread controller.

const AcknowledgeProcessorFunctorShPtr_t    NullAcknowledgeProcessorFunctor(NULL);  ///< NULL functor for acknowledge processing.
const TimeoutProcessorFunctorShPtr_t        NullTimeoutProcessorFunctor(NULL);      ///< NULL functor for timeout processing.
const CmdDataChangedFunctorShPtr_t          NullDataChangedFunctor(NULL);           ///< NULL functor for data changed.

/****************************************************************************/
BaseThreadController::BaseThreadController(Global::gSourceType TheHeartBeatSource) :
    m_HeartBeatSource(TheHeartBeatSource),
    m_HeartbeatTimeout(0),
    m_HeartbeatTimer(this)
{
    // register commands and acknowledges
    qRegisterMetaType<Global::tRefType>("Global::tRefType");
    qRegisterMetaType<Global::CommandShPtr_t>("Global::CommandShPtr_t");
    qRegisterMetaType<Global::AcknowledgeShPtr_t>("Global::AcknowledgeShPtr_t");
    // connect timer
    CONNECTSIGNALSLOT(&m_HeartbeatTimer, timeout(), this, HeartbeatTimer());
    // disable error forwarding because we do not have a parent
    //DisableErrorForwarding();
}

/****************************************************************************/
BaseThreadController::~BaseThreadController() {
}

/****************************************************************************/
void BaseThreadController::StartHeartbeatTimer() {
    // only start if needed.
    if(m_HeartbeatTimeout > 0) {
        m_HeartbeatTimer.setInterval(m_HeartbeatTimeout);
        m_HeartbeatTimer.setSingleShot(false);
        m_HeartbeatTimer.start();
    }
}

/****************************************************************************/
void BaseThreadController::StopHeartbeatTimer() {
    m_HeartbeatTimer.stop();
}

/****************************************************************************/
void BaseThreadController::HeartbeatTimer() {
    emit HeartbeatSignal(GetHeartBeatSource());
}

/****************************************************************************/
void BaseThreadController::Go() {
    try {
        // start heartbeat timer
        StartHeartbeatTimer();
        // call processing method
        OnGoReceived();
    } catch(const std::bad_alloc &) {
        // send some error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION, FILE_LINE_LIST,
                  Global::NO_NUMERIC_DATA, false);
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
void BaseThreadController::Stop() {
    try {
        // call processing method
        OnStopReceived();
        // stop heartbeat timer
        StopHeartbeatTimer();
    } catch(const std::bad_alloc &) {
        // send some error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    } catch(const Global::Exception &E) {
        // and send error message
        Global::EventObject::Instance().RaiseException(E);
    } catch(...) {
        // send some error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    }
    // quit thread
    QThread::currentThread()->quit();
}

/****************************************************************************/
void BaseThreadController::SetHeartbeatTimeout(int Timeout) {
    // remember new requested value but do not alter timeout.
    m_HeartbeatTimeout = Timeout;
}

/****************************************************************************/
void BaseThreadController::DoSendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd, CommandChannel &CmdChannel) {
    // check pointer
    if(Cmd.IsNull())
    {
        LOGANDTHROWARGS(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << "Cmd" << FILE_LINE); \
    }
    try {
        // try to send the command over the according transmit channel
//        qDebug() << "BaseThreadController::DoSendCommand" << Ref << Cmd.GetPointerToUserData()->GetName() << "Channel" << CmdChannel.m_channelName;
        if(Cmd->GetTimeout() != Global::Command::NOTIMEOUT) {
            // create descriptor
            Global::PendingCmdDescriptorShP_t PCDShP(new Global::PendingCmdDescriptor(this, Ref, Cmd->GetName(), Cmd->GetTimeout()));

            if (this->thread() != PCDShP.GetPointerToUserData()->thread())
            {
                qDebug() << "BaseThreadController::DoSendCommand" << Cmd.GetPointerToUserData()->GetName() << "current thread" << this->thread() << "PCDShP thread" << PCDShP.GetPointerToUserData()->thread();
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
    } catch(const Global::Exception &E) {
        // and send error message
        Global::EventObject::Instance().RaiseException(E);
    } catch(...) {
        // send some error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    }
    // some error occured
    LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, EVENT_THREADS_ERROR_SEND_COMMAND, Global::tTranslatableStringList() << Cmd->GetName() << Ref,
              Global::NO_NUMERIC_DATA, false);
}

/****************************************************************************/
void BaseThreadController::DoSendAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack, CommandChannel &CmdChannel)
{
//    qDebug() << "BaseThreadController::DoSendAcknowledge" << Ref << Ack.GetPointerToUserData()->GetName() << "Channel" << CmdChannel.m_channelName;
    // check pointer
    if(Ack.IsNull())
    {
        LOGANDTHROWARGS(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << "Ack" << FILE_LINE); \
    }
    try {
        // try to send the command over the according transmit channel
        CmdChannel.EmitAcknowledge(Ref, Ack);
        // everything OK
        return;
    } catch(const Global::Exception &E) {
        // and send error message
      Global::EventObject::Instance().RaiseException(E);
    } catch(...) {
        // send some error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST,
                  Global::NO_NUMERIC_DATA, false);
    }
    // some error occured
    LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, EVENT_THREADS_ERROR_SEND_ACKNOWLEDGE, Global::tTranslatableStringList() << Ack->GetName() << Ref,
              Global::NO_NUMERIC_DATA, false);
}

/****************************************************************************/
void BaseThreadController::RegisterAcknowledgeProcessorFunctor(const QString &AckName, const AcknowledgeProcessorFunctorShPtr_t &Functor) {
    // check if already registered
    if(m_AcknowledgeProcessorFunctors.contains(AckName)) {
        LOGANDTHROWARGS(EVENT_THREADS_ERROR_ACKNOWLEDGE_FUNCTOR_ALREADY_REGISTERED, AckName);
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
        qDebug() << "... no functor found";
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
        LOGANDTHROWARGS(EVENT_THREADS_ERROR_TIMEOUT_FUNCTOR_ALREADY_REGISTERED, CmdName);
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
        LOGANDTHROWARGS(EVENT_THREADS_ERROR_ACKNOWLEDGE_FUNCTOR_ALREADY_REGISTERED, DataContainerName);
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
        if(Ack.IsNull()) {
            LOGANDTHROWARGS(Global::EVENT_GLOBAL_ERROR_NULL_POINTER, Global::tTranslatableStringList() << "Ack" << FILE_LINE); \
        }
//        SEND_DEBUG(WHEREAMI + " " +
//                   QString("Ref = ") + QString::number(Ref, 10) +
//                   QString("Name = ") + Ack->GetName());
        if(!m_PendingCommands.contains(Ref)) {
            // unknown command reference. maybe already timed out.
            qDebug() << "BaseThreadController::OnProcessAcknowledge, unknown command ref" << Ref;
            Global::EventObject::Instance().RaiseEvent(EVENT_THREADS_ERROR_UNKNOWN_COMMAND_REF, Global::FmtArgs() << Ref, true);
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
            qDebug() << "BaseThreadController::OnProcessAcknowledge, NullAcknowledgeProcessorFunctor" << Ref;
            // throw exception
            LOGANDTHROWARG(EVENT_THREADS_ERROR_UNSUPPORTED_ACKNOWLEDGE, Ack->GetName());
        }
        // execute
        Functor.GetPointerToUserData()->Process(Ref, Ack.GetPointerToUserData());
    } catch(const Global::Exception &E) {
        // and send error message
        qDebug() << "Exception has been caught";
        //Global::EventObject::Instance().RaiseException(E);
    } catch(...) {
        // send some error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    }
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
        Global::EventObject::Instance().RaiseEvent(EVENT_THREADS_ERROR_UNKNOWN_COMMAND_REF, Global::FmtArgs() << Ref, true);
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
