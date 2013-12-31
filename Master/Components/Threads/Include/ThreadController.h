/****************************************************************************/
/*! \file Threads/Include/ThreadController.h
 *
 *  \brief Definition file for class ThreadController.
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

#ifndef THREADS_THREADCONTROLLER_H
#define THREADS_THREADCONTROLLER_H

#include <Threads/Include/BaseThreadController.h>

namespace Threads {

typedef Global::SharedPointer<CommandExecuteFunctor>    CommandExecuteFunctorShPtr_t;   ///< Typedef or a shared pointer of CommandExecuteFunctor.
typedef QHash<QString, CommandExecuteFunctorShPtr_t>    CommandExecuteFunctorHash_t;    ///< Typedef for the CommandExecuteFunctorShPtr_t functor hash.

/****************************************************************************/
/**
 * \brief Base class for thread controllers
 *
 * This class is the base class for all specific thread controllers, except
 * the master thread controller. It basicly contains only the pointer to the
 * onae and only command channel.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class ThreadController : public BaseThreadController {
    Q_OBJECT
private:
    CommandExecuteFunctorHash_t     m_CommandExecuteFunctors;   ///< Functors of supported commands.

    /****************************************************************************/
    ThreadController();                                             ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(ThreadController)
    /****************************************************************************/
    /**
     * \brief Data in some data container has changed.
     *
     * A positive acknowledge is send and the command is dispatched to the responsible
     * processing function.
     * Additional documentation in base class.
     *
     * \iparam   Ref     Command reference.
     * \iparam   Cmd     Command.
     */
    /****************************************************************************/
    virtual void CmdDataChangedReceived(Global::tRefType Ref, const Global::CmdDataChanged &Cmd);
    /****************************************************************************/
    /**
     * \brief A CmdPowerFail command was received.
     *
     * The command is dispatched to the responsible processing function.
     * Additional documentation in base class.
     *
     * \iparam   Ref     Command reference.
     * \iparam   Cmd     Command.
     */
    /****************************************************************************/
    virtual void CmdPowerFailReceived(Global::tRefType Ref, const Global::CmdPowerFail &Cmd);
    /****************************************************************************/
    /**
     * \brief Send a data changed command.
     *
     * The command will be send via \ref SendCommand.
     *
     * \iparam   Cmd     The command.
     */
    /****************************************************************************/
    virtual void DoSendDataChanged(const Global::CommandShPtr_t &Cmd);
protected:
    CommandChannel                  m_CommandChannel;           ///< Command channel.
    /****************************************************************************/
    /**
     * \brief Send a command over its command channel.
     *
     *
     * \iparam   Cmd     The command.
     * \iparam   Ref     The command reference.
     */
    /****************************************************************************/
    virtual void SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd);
    /****************************************************************************/
    /**
     * \brief Send a acknowledge over its command channel.
     *
     * \iparam   Ref     Acknowledge reference.
     * \iparam   Ack     The acknowledge.
     */
    /****************************************************************************/
    virtual void SendAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack);
    /****************************************************************************/
    /**
     * \brief Register a command execution functor.
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
    CommandExecuteFunctorShPtr_t GetCommandExecuteFunctor(const QString &CommandName) const;
    /****************************************************************************/
    /**
     * \brief Register a command for processing.
     *
     * \iparam   pCommandProcessor   Pointer to thread controller instance which processes the command.
     * \iparam   FunctionPointer     Function which processes the command.
     */
    /****************************************************************************/
    template<class CmdClass, class CommandProcessorClass>
    void RegisterCommandForProcessing(void(CommandProcessorClass::*FunctionPointer)(Global::tRefType, const CmdClass &),
                                      CommandProcessorClass *pCommandProcessor)
    {
        // create functor
        CommandExecuteFunctorShPtr_t Functor(new TemplateCommandExecuteFunctor<CommandProcessorClass, CmdClass> (pCommandProcessor, FunctionPointer));
        // and register
        RegisterCommandExecuteFunctor(CmdClass::NAME, Functor);
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
public:

    /****************************************************************************/
    /**
     * \brief Send a positive acknowledge over its command channel.
     *
     * \note Create a positive acknwoledge of type \ref Global::AckOKNOK and send it.
     *
     * \iparam   Ref     Acknowledge reference.
     * \iparam   status  OK/NOK
     */
    /****************************************************************************/
    void SendAcknowledgeOKNOK(Global::tRefType Ref, bool status);
    /****************************************************************************/
    /**
     * \brief Send a positive acknowledge over its command channel.
     *
     * Create a positive acknwoledge of type \ref Global::AckOKNOK and send it.
     *
     * \iparam   Ref     Acknowledge reference.
     */
    /****************************************************************************/
    void SendAcknowledgeOK(Global::tRefType Ref);
    /****************************************************************************/
    /**
     * \brief Send a negative acknowledge over its command channel.
     *
     * \note Create a negative acknwoledge of type \ref Global::AckOKNOK and send it.
     *
     * \iparam   Ref     Acknowledge reference.
     * \iparam   Text        Text of message.
     * \iparam   Type        Type of message.
     */
    /****************************************************************************/
    void SendAcknowledgeNOK(Global::tRefType Ref, const QString &Text = "", Global::GUIMessageType Type = Global::GUIMSGTYPE_ERROR);
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   ThreadID   Thread Id
     * \iparam   name
     */
    /****************************************************************************/
    ThreadController(quint32 ThreadID, QString name);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~ThreadController();
    /****************************************************************************/
    /**
     * \brief Connect to another command channel.
     *
     * Connect to another command channel.
     *
     * \iparam   pCommandChannel Command channel to connect to.
     */
    /****************************************************************************/
    void ConnectToOtherCommandChannel(CommandChannel *pCommandChannel) const;




}; // end class ThreadController

} // end namespace Threads

#endif // THREADS_THREADCONTROLLER_H
