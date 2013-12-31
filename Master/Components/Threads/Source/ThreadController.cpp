/****************************************************************************/
/*! \file Threads/Source/ThreadController.cpp
 *
 *  \brief Implementation file for class ThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-02-22
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

#include <Threads/Include/ThreadController.h>
#include <Global/Include/Commands/AckOKNOK.h>
#include <QDebug>

#include <unistd.h>


namespace Threads {

static const CommandExecuteFunctorShPtr_t          NullCommandExecuteFunctor(NULL);        ///< NULL functor for command execution.

/****************************************************************************/
ThreadController::ThreadController(quint32 ThreadID, QString name) :
    BaseThreadController(ThreadID, name),
    m_CommandChannel(this, name)
{
    qDebug() << "xxxxxx ThreadController::ThreadController" << name;
    // register CmdDataChanged command
    RegisterCommandForProcessing<Global::CmdDataChanged, ThreadController>(&ThreadController::CmdDataChangedReceived, this);
    // register CmdPowerFail command
    RegisterCommandForProcessing<Global::CmdPowerFail, ThreadController>(&ThreadController::CmdPowerFailReceived, this);
}

/****************************************************************************/
ThreadController::~ThreadController() {
}

/****************************************************************************/
void ThreadController::ConnectToOtherCommandChannel(CommandChannel *pCommandChannel) const
{
    // connect command channels
    CHECKPTR(pCommandChannel);
    CONNECTSIGNALSLOT(pCommandChannel, CommandChannelTx(Global::tRefType, const Global::CommandShPtr_t &),
                      &m_CommandChannel, CommandChannelRx(Global::tRefType, const Global::CommandShPtr_t &));
    CONNECTSIGNALSLOT(&m_CommandChannel, CommandChannelTx(Global::tRefType, const Global::CommandShPtr_t &),
                      pCommandChannel, CommandChannelRx(Global::tRefType, const Global::CommandShPtr_t &));
    CONNECTSIGNALSLOT(pCommandChannel, CommandChannelRxAck(Global::tRefType, const Global::AcknowledgeShPtr_t &),
                      &m_CommandChannel, CommandChannelTxAck(Global::tRefType, const Global::AcknowledgeShPtr_t &));
    CONNECTSIGNALSLOT(&m_CommandChannel, CommandChannelRxAck(Global::tRefType, const Global::AcknowledgeShPtr_t &),
                      pCommandChannel, CommandChannelTxAck(Global::tRefType, const Global::AcknowledgeShPtr_t &));
}

/****************************************************************************/
void ThreadController::CmdDataChangedReceived(Global::tRefType /*Ref*/, const Global::CmdDataChanged &Cmd) {
    // check if command has timeout
    if(Cmd.GetTimeout() != Global::Command::NOTIMEOUT) {
        LOGANDTHROWARG(EVENT_THREADS_ERROR_COMMAND_HAS_TIMEOUT, Cmd.GetName())
    }
    // process command
    DispatchDataChangedCommand(Cmd);
    // no acknowledge for CmdDataChanged required
}

/****************************************************************************/
void ThreadController::CmdPowerFailReceived(Global::tRefType /*Ref*/, const Global::CmdPowerFail &Cmd) {
    // check if command has timeout
    if(Cmd.GetTimeout() != Global::Command::NOTIMEOUT) {
        LOGANDTHROWARG(EVENT_THREADS_ERROR_COMMAND_HAS_TIMEOUT, Cmd.GetName())
    }
    // process command
    OnPowerFail(Cmd.m_PowerFailStage);
    // no acknowledge for CmdPowerFailReceived required
}

/****************************************************************************/
void ThreadController::DoSendDataChanged(const Global::CommandShPtr_t &Cmd) {
    DEBUGWHEREAMI;

    // send command with new ref
    static_cast<void>(
            // we DO NOT NEED the return value of SendCommand
                SendCommand(GetNewCommandRef(), Cmd)
    );
}

/****************************************************************************/
void ThreadController::SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd)
{
    // send command using own command channel
    qDebug() << "Threads::ThreadController::SendCommand, Time=" << Global::AdjustedTime::Instance().GetCurrentDateTime().toString()
             << ", Ref=" << Ref
             << ", Command=" << Cmd.GetPointerToUserData()->GetName()
             << ", Channel=" << m_CommandChannel.m_channelName
             << ", Timeout=" << Cmd.GetPointerToUserData()->GetTimeout()
             << ", Thread=" << this->thread();



    DoSendCommand(Ref, Cmd, m_CommandChannel);
    // return computed Ref
}

/****************************************************************************/
void ThreadController::SendAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack)
{
//    qDebug() << "ThreadController::SendAcknowledge" << Ref << Ack.GetPointerToUserData()->GetName() << "channel" << m_CommandChannel.m_channelName;
    // send command using own command channel
    //Global::EventObject::Instance().LogThreadId(this->m_CommandChannel.m_channelName , syscall(SYS_gettid));

    DoSendAcknowledge(Ref, Ack, m_CommandChannel);
}

/****************************************************************************/
void ThreadController::RegisterCommandExecuteFunctor(const QString &CommandName, const CommandExecuteFunctorShPtr_t &Functor)
{
    // check if already registered
    if(m_CommandExecuteFunctors.contains(CommandName)) {
        LOGANDTHROWARGS(EVENT_THREADS_ERROR_COMMAND_FUNCTOR_ALREADY_REGISTERED, CommandName)
    }

    qDebug() << "ThreadController::RegisterCommandExecuteFunctor" << CommandName;

    // everything OK
    static_cast<void>(
        // we DO NOT NEED the return value of insert
        m_CommandExecuteFunctors.insert(CommandName, Functor)
    );
}

/****************************************************************************/
CommandExecuteFunctorShPtr_t ThreadController::GetCommandExecuteFunctor(const QString &CommandName) const
{
    //qDebug() << "ThreadController::GetCommandExecuteFunctor" << CommandName;
    CommandExecuteFunctorHash_t::const_iterator it = m_CommandExecuteFunctors.find(CommandName);
    if(it == m_CommandExecuteFunctors.constEnd()) {
        // functor not found
        // return NULL functor
        return NullCommandExecuteFunctor;
    }
    // return functor
    return it.value();
}

//lint -efunc(613, Threads::ThreadController::OnExecuteCommand)
// JB: Since lint does not know that Cmd can not be "NULL" after checking it with IsNull
// it will throw lots of 613 warnings when accessing Cmd, so we disable 613 for
// this method. Remember that 613 is disabled forr all pointers in this function!
/****************************************************************************/
void ThreadController::OnExecuteCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd, CommandChannel &AckCommandChannel)
{
//    qDebug() << "Threads::ThreadController::OnExecuteCommand" << Ref << Cmd.GetPointerToUserData()->GetName();
    Q_UNUSED(AckCommandChannel);
    try {
        if (IsCommandAllowed(Cmd))
        {
            // get command pointer
            CHECKPTR(Cmd.GetPointerToUserData());

//            SEND_DEBUG(WHEREAMI + " " +
//                       QString("Ref = ") + QString::number(Ref, 10) +
//                       QString("Name = ") + Cmd->GetName());
            // OK, now get functor and execute
            CommandExecuteFunctorShPtr_t Functor = GetCommandExecuteFunctor(Cmd->GetName());
            if(Functor == NullCommandExecuteFunctor) {
                qDebug()<<"ThreadController" << Cmd->GetName();
                // throw exception
                LOGANDTHROWARG(EVENT_THREADS_ERROR_UNSUPPORTED_COMMAND, Cmd->GetName())                
            }
            // execute
            Functor.GetPointerToUserData()->Execute(Ref, Cmd.GetPointerToUserData());
        }
    }
    CATCHALL();
}

/****************************************************************************/
void ThreadController::SendAcknowledgeOKNOK(Global::tRefType Ref, bool status) {
    // create acknowledge and send
    SendAcknowledge(Ref, Global::AcknowledgeShPtr_t(new Global::AckOKNOK(status)));
}

/****************************************************************************/
void ThreadController::SendAcknowledgeOK(Global::tRefType Ref) {
    // create acknowledge and send
    SendAcknowledge(Ref, Global::AcknowledgeShPtr_t(new Global::AckOKNOK(true)));
}

/****************************************************************************/
void ThreadController::SendAcknowledgeNOK(Global::tRefType Ref, const QString &Text, Global::GUIMessageType Type) {
    // create acknowledge and send
    SendAcknowledge(Ref, Global::AcknowledgeShPtr_t(new Global::AckOKNOK(false, Text, Type)));
}

} // end namespace Threads
