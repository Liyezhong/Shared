/****************************************************************************/
/*! \file Threads/Source/CommandChannel.cpp
 *
 *  \brief Implementation file for class CommandChannel.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-02-24
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

#include <Threads/Include/CommandChannel.h>
#include <Threads/Include/BaseThreadController.h>
#include <Global/Include/EventTranslator.h>
#include <QDebug>

namespace Threads {

/****************************************************************************/
CommandChannel::CommandChannel(BaseThreadController *pParent, QString name) :
    QObject(pParent),
    m_pBaseThreadController(pParent),
    m_channelName(name)
{
    qDebug() << "xxxxxx CommandChannel::CommandChannel" << m_channelName << name;
    CHECKPTR(m_pBaseThreadController);
}

/****************************************************************************/
void CommandChannel::EmitCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd) {
    // check if our signal is connected
    if(receivers(SIGNAL(CommandChannelTx(Global::tRefType, const Global::CommandShPtr_t &))) > 0)
    {
//        qDebug() << "CommandChannel::EmitCommand" << Ref << Cmd.GetPointerToUserData()->GetName() << "channel" << m_channelName << "receivers" << receivers(SIGNAL(CommandChannelTx(Global::tRefType, const Global::CommandShPtr_t &)));
        // signal connected.
        emit CommandChannelTx(Ref, Cmd);
    }
    else
    {
        // signal is not connected. Send error.
        LOGANDTHROWARGS(Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED,
                  "CommandChannel::CommandChannelTx(Global::tRefType, const Global::CommandShPtr_t &))");
    }
}

/****************************************************************************/
void CommandChannel::EmitAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack)
{
//    qDebug() << "CommandChannel::EmitAcknowledge" << Ref << Ack.GetPointerToUserData()->GetName() << "Channel" << m_channelName;
    // check if our signal is connected
    if(receivers(SIGNAL(CommandChannelRxAck(Global::tRefType, const Global::AcknowledgeShPtr_t &))) > 0) {
        // signal connected.
//        qDebug() << "CommandChannel::EmitAcknowledge" << Ref << Ack.GetPointerToUserData()->GetName() << "Channel" << m_channelName;
        emit CommandChannelRxAck(Ref, Ack);
    } else {
        qDebug() << "CommandChannel::EmitAcknowledge, Signal not connected" << Ref;
        // signal is not connected. Send error.
        LOGANDTHROWARGS(Global::EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED,
                  "CommandChannel::CommandChannelRxAck(Global::tRefType, const Global::AcknowledgeShPtr_t &))");
    }
}

/****************************************************************************/
void CommandChannel::CommandChannelRx(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd) {
    try  {
//        qDebug() << "CommandChannel::CommandChannelRx" << Ref << Cmd.GetPointerToUserData()->GetName() << "channel" << m_channelName;

        CHECKPTR(m_pBaseThreadController);
        // execute command
        m_pBaseThreadController->OnExecuteCommand(Ref, Cmd, *this);
    } catch (...) {
        /// \todo JB: log error, but how?!? Then allow exception throwing in OnExecuteCommand
    }
}

/****************************************************************************/
void CommandChannel::CommandChannelTxAck(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack)
{
//    qDebug() << "CommandChannel::CommandChannelTxAck" << Ref << Ack.GetPointerToUserData()->GetName() << "channel" << m_channelName;
    try
    {
        CHECKPTR(m_pBaseThreadController);
        // acknowledge received
        m_pBaseThreadController->OnProcessAcknowledge(Ref, Ack);
    } catch (...) {
        /// \todo JB: log error, but how?!? Then allow exception throwing in OnProcessAcknowledge
    }
}

} // end namespace Threads
