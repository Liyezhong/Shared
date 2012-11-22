/****************************************************************************/
/*! \file Threads/Include/CommandChannel.h
 *
 *  \brief Definition file for class CommandChannel.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-02-24
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

#ifndef THREADS_COMMANDCHANNEL_H
#define THREADS_COMMANDCHANNEL_H

#include <Global/Include/Commands/Acknowledge.h>
#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>

#include <QObject>

namespace Threads {

class BaseThreadController;

/****************************************************************************/
/**
 * \brief Thread command channel class.
 *
 * This class is used for sending and receiving commands. It is a
 * thread controller to command channel consisting of
 * all needed signal slot pairs for sending and receiving commands and acknowledges.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CommandChannel : public QObject {
    Q_OBJECT
friend class BaseThreadController;
friend class MasterThreadController;
private:
    BaseThreadController    *m_pBaseThreadController;   ///< Pointer to thread controller.
    /****************************************************************************/
    CommandChannel();                                           ///< Not implemented.
    CommandChannel(const CommandChannel &);                     ///< Not implemented.
    const CommandChannel & operator = (const CommandChannel &); ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Emit signal for commands.
     *
     * If signal connected emit it, else throw an exception.
     * Called from BaseThreadController::SendCommand or from
     * MasterThreadController::SendCommand.
     *
     * \param[in]   Ref         Reference of command.
     * \param[in]   Cmd         Command.
     */
    /****************************************************************************/
    void EmitCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd);
    /****************************************************************************/
    /**
     * \brief Emit signal for command acknowledges.
     *
     * If signal connected emit it, else throw an exception.
     * Called from BaseThreadController::SendAcknowledge or from
     * MasterThreadController::SendAcknowledge.
     *
     * \param[in]   Ref         Reference of command.
     * \param[in]   Ack         Acknowledge for command.
     */
    /****************************************************************************/
    void EmitAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack);
signals:
    /****************************************************************************/
    /**
     * \brief Signal for sending a command acknowledge.
     *
     * \param[in]   Ref         Reference of command.
     * \param[in]   Ack         Acknowledge for command.
     */
    /****************************************************************************/
    void CommandChannelRxAck(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack);
    /****************************************************************************/
    /**
     * \brief Signal for sending a command.
     *
     * \param[in]   Ref         Reference of command.
     * \param[in]   Cmd         Command.
     */
    /****************************************************************************/
    void CommandChannelTx(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   pParent     Parent object.
     */
    /****************************************************************************/
    CommandChannel(BaseThreadController *pParent, QString name);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CommandChannel() {
        m_pBaseThreadController = NULL;
    }

    QString m_channelName;
public slots:
    /****************************************************************************/
    /**
     * \brief Receive command to be executed.
     *
     * Receive a command from a different thread controller which shall be executed
     * in own thread controller.
     * The command is identified by the reference Ref.
     *
     * \param[in]   Ref         Reference of command.
     * \param[in]   Cmd         The command to execute.
     */
    /****************************************************************************/
    void CommandChannelRx(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd);
    /****************************************************************************/
    /**
     * \brief Receive acknowledge.
     *
     * Receive an acknowledge from a different thread controller which executed a command
     * sent by us.
     * The command is identified by the reference Ref.
     *
     * \param[in]   Ref         Reference of command.
     * \param[in]   Ack         Acknowledge for command.
     */
    /****************************************************************************/
    void CommandChannelTxAck(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack);
}; // end class CommandChannel

} // end namespace Threads

#endif // THREADS_COMMANDCHANNEL_H
