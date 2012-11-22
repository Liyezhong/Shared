/****************************************************************************/
/*! \file Threads/Include/BaseThreadController.h
 *
 *  \brief Definition file for class ThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-02-22
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

#ifndef THREADS_BASETHREADCONTROLLER_H
#define THREADS_BASETHREADCONTROLLER_H

#include <DataLogging/Include/LoggingObject.h>
#include <Global/Include/RefManager.h>
#include <Global/Include/Commands/CmdDataChanged.h>
#include <Global/Include/Commands/CmdPowerFail.h>
#include <Global/Include/Commands/PendingCmdDescriptor.h>
#include <Threads/Include/CommandFunctors.h>

#include <QSet>

#include <QThread>

/****************************************************************************/
/**
 * \brief This namespace contains thread related classes.
 *
 * See http://labs.trolltech.com/blogs/2010/06/17/youre-doing-it-wrong/
 * and
 * http://labs.trolltech.com/blogs/2006/12/04/threading-without-the-headache/
 * for the reason we start threads like we do and not how stated in the
 * official Qt documentation.
 */
/****************************************************************************/
namespace Threads {

typedef Global::SharedPointer<AcknowledgeProcessorFunctor>      AcknowledgeProcessorFunctorShPtr_t; ///< Typedef or a shared pointer of AcknowledgeProcessorFunctor.
typedef Global::SharedPointer<TimeoutProcessorFunctor>          TimeoutProcessorFunctorShPtr_t;     ///< Typedef or a shared pointer of TimeoutProcessorFunctor.
typedef Global::SharedPointer<Global::CmdDataChangedFunctor>    CmdDataChangedFunctorShPtr_t;       ///< Typedef or a shared pointer of CmdDataChangedFunctor.

typedef QHash<QString, AcknowledgeProcessorFunctorShPtr_t>  AcknowledgeProcessorFunctorHash_t;  ///< Typedef for the AcknowledgeProcessorFunctorShPtr_t functor hash.
typedef QHash<QString, TimeoutProcessorFunctorShPtr_t>      TimeoutProcessorFunctorHash_t;      ///< Typedef for the TimeoutProcessorFunctorShPtr_t functor hash.
typedef QHash<QString, CmdDataChangedFunctorShPtr_t>        CmdDataChangedFunctorHash_t;        ///< Typedef for the CmdDataChangedFunctorShPtr_t functor hash.


/****************************************************************************/
/**
 * \brief This class is the base class for thread controlling classes.
 *
 * This class is used instead of subclassing from QThread, which has to be
 * used only to provide an event loop. Instead of subclassing, we use this
 * controller which will "live" in stated thread. This controller is
 * responsible for creating, configuring and destroying all objects that "live"
 * in this thread.\n
 * See http://labs.trolltech.com/blogs/2010/06/17/youre-doing-it-wrong/
 * and
 * http://labs.trolltech.com/blogs/2006/12/04/threading-without-the-headache/
 * for more details.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class BaseThreadController : public DataLogging::LoggingObject {
    Q_OBJECT
friend void CommandChannel::CommandChannelRx(Global::tRefType, const Global::CommandShPtr_t &);         ///< For calling \ref OnExecuteCommand.
friend void CommandChannel::CommandChannelTxAck(Global::tRefType, const Global::AcknowledgeShPtr_t &);  ///< For calling \ref OnProcessAcknowledge.
private:
    int                                     m_HeartbeatTimeout;             ///< Timeout for heartbeat functionality. Default = 0 ms = off.
    QTimer                                  m_HeartbeatTimer;               ///< Timer for heartbeat functionality.
    Global::RefManager<Global::tRefType>    m_RefManager;                   ///< Manager for command references.
    Global::PendingCmdDescriptorPtrHash_t   m_PendingCommands;              ///< Commands waiting for acknowledge.
    AcknowledgeProcessorFunctorHash_t       m_AcknowledgeProcessorFunctors; ///< Functors of supported acknowledges.
    TimeoutProcessorFunctorHash_t           m_TimeoutProcessorFunctors;     ///< Functors of supported command timeouts.
    CmdDataChangedFunctorHash_t             m_DataChangedFunctors;          ///< Functors of supported CmdDataChanged commands.
    /****************************************************************************/
    BaseThreadController();                                                 ///< Not implemented.
    BaseThreadController(const BaseThreadController &);                     ///< Not implemented.
    const BaseThreadController & operator = (const BaseThreadController &); ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Start the heartbeat timer.
     */
    /****************************************************************************/
    void StartHeartbeatTimer();
    /****************************************************************************/
    /**
     * \brief Stop the heartbeat timer.
     */
    /****************************************************************************/
    void StopHeartbeatTimer();
    /****************************************************************************/
    /**
     * \brief Send a data changed command.
     *
     * Must be implemented in derived classes.
     *
     * \param[in]   Cmd     The command.
     */
    /****************************************************************************/
    virtual void DoSendDataChanged(const Global::CommandShPtr_t &Cmd) = 0;
signals:
    /****************************************************************************/
    /**
     * \brief This signal is emitted periodically to tell listeners that we are
     * still alive.
     *
     * The period can be changed by calling \ref SetHeartbeatTimeout.
     *
     * \param[in]   TheLoggingSource    The logging source of emiting thread.
     */
    /****************************************************************************/
    void HeartbeatSignal(const Global::gSourceType &TheLoggingSource);
private slots:
    /****************************************************************************/
    /**
     * \brief Slot for receiving heartbeat timer signals.
     */
    /****************************************************************************/
    void HeartbeatTimer();
    /****************************************************************************/
    /**
     * \brief An timeout for an send command was received.
     *
     * Is called when an command timeout has to be processed.
     *
     * \param[in]       Ref         The command reference.
     * \param[in]       CmdName     Name of command.
     */
    /****************************************************************************/
    void OnProcessTimeoutSlot(Global::tRefType Ref, QString CmdName);
protected:
    /****************************************************************************/
    /**
     * \brief Send a command over a specific command channel.
     *
     * After the command was send, the descriptor is created and its timeout signal
     * connected (if timeout > 0). After that, the descriptor is added to the list
     * of pending commands.
     *
     * \param[in]   Ref         The command reference.
     * \param[in]   Cmd         The command.
     * \param[in]   CmdChannel  The command channel.
     */
    /****************************************************************************/
    void DoSendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd, CommandChannel &CmdChannel);
    /****************************************************************************/
    /**
     * \brief Send an acknowledge over a specific command channel.
     *
     * Send an acknowledge over a specific command channel
     *
     * \param[in]   Ref         The acknowledge reference.
     * \param[in]   Ack         The acknowledge.
     * \param[in]   CmdChannel  The command channel.
     */
    /****************************************************************************/
    void DoSendAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack, CommandChannel &CmdChannel);
    /****************************************************************************/
    /**
     * \brief Thread command processing method.
     *
     * Is called when an command has to be processed.
     * \warning This method should be called only from within \ref CommandChannel::CommandChannelRx
     * \warning Do not let exceptions escape this method!
     *
     * \param[in]       Ref                 The command reference.
     * \param[in]       Cmd                 The command.
     * \param[in, out]  AckCommandChannel   The command channel for acknowledges.
     */
    /****************************************************************************/
    virtual void OnExecuteCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd, CommandChannel &AckCommandChannel) = 0;
    /****************************************************************************/
    /**
     * \brief An acknowledge for an send command was received.
     *
     * Is called when an command acknowledge has to be processed.
     * \warning This method should be called only from within \ref CommandChannel::CommandChannelTxAck
     * \warning Do not let exceptions escape this method!
     *
     * \param[in]       Ref         The command reference.
     * \param[in]       Ack         The received acknowledge.
     */
    /****************************************************************************/
    virtual void OnProcessAcknowledge(Global::tRefType Ref, const Global::AcknowledgeShPtr_t &Ack);
    /****************************************************************************/
    /**
     * \brief An timeout for an send command was received.
     *
     * Is called when an command timeout has to be processed.
     *
     * \param[in]       Ref         The command reference.
     * \param[in]       CmdName     Name of command.
     */
    /****************************************************************************/
    virtual void OnProcessTimeout(Global::tRefType Ref, const QString &CmdName);
    /****************************************************************************/
    /**
     * \brief This method is called when we received the \ref Go signal.
     *
     * This means that everything is fine and normal operation started.
     * We are running in our own thread now.\n
     * Do whatever you must do when normal operation started.\n
     * Has to be implemented in derived classes.
     */
    /****************************************************************************/
    virtual void OnGoReceived() = 0;
    /****************************************************************************/
    /**
     * \brief This method is called when we received the \ref Stop signal.
     *
     * This means that normal operation will stop after processing this signal.
     * We are still running in our own thread.\n
     * Do whatever you must do before normal operation ends.\n
     * Has to be implemented in derived classes.
     */
    /****************************************************************************/
    virtual void OnStopReceived() = 0;
    /****************************************************************************/
    /**
     * \brief Power will fail shortly.
     *
     * Must be implemented in derived classes.
     */
    /****************************************************************************/
    virtual void OnPowerFail() = 0;
    /****************************************************************************/
    /**
     * \brief Remove a command from Pending commands.
     *
     * \param[in]   Ref     Command reference.
     */
    /****************************************************************************/
    inline void RemoveFromPendingCommands(Global::tRefType Ref) {
        // remove from list of pending commands
        m_PendingCommands.remove(Ref);
    }
    /****************************************************************************/
    /**
     * \brief Register a acknowledge processor functor.
     *
     * \param[in]   AckName         Name of acknowledge.
     * \param[in]   Functor         Shared pointer of functor to register.
     */
    /****************************************************************************/
    void RegisterAcknowledgeProcessorFunctor(const QString &AckName, const AcknowledgeProcessorFunctorShPtr_t &Functor);
    /****************************************************************************/
    /**
     * \brief Get acknowledge processor functor by name.
     *
     * Get acknowledge processor functor by name. If functor is not found
     * NullAcknowledgeProcessorFunctor will be returned.
     *
     * \param[in]   AckName     Name of acknowledge.
     * \return                  The functor or NullAcknowledgeProcessorFunctor.
     */
    /****************************************************************************/
    AcknowledgeProcessorFunctorShPtr_t GetAcknowledgeProcessorFunctor(const QString &AckName) const;
    /****************************************************************************/
    /**
     * \brief Register a command timeout functor.
     *
     * \param[in]   CmdName         Name of command.
     * \param[in]   Functor         Shared pointer of functor to register.
     */
    /****************************************************************************/
    void RegisterTimeoutProcessingFunctor(const QString &CmdName, const TimeoutProcessorFunctorShPtr_t &Functor);
    /****************************************************************************/
    /**
     * \brief Get timeout processor functor by name.
     *
     * Get timeout processor functor by name. If functor is not found
     * NullTimeoutProcessorFunctor will be returned.
     *
     * \param[in]   CmdName     Name of command.
     * \return                  The functor or NullTimeoutProcessorFunctor.
     */
    /****************************************************************************/
    TimeoutProcessorFunctorShPtr_t GetTimeoutProcessorFunctor(const QString &CmdName) const;
    /****************************************************************************/
    /**
     * \brief Register a functor for processing DataChanged command for a specific data container.
     *
     * \param[in]   DataContainerName   Name of data container with changed data.
     * \param[in]   Functor             Shared pointer of functor to register.
     */
    /****************************************************************************/
    void RegisterOnDataChangedProcessingFunctor(const QString &DataContainerName, const CmdDataChangedFunctorShPtr_t &Functor);
    /****************************************************************************/
    /**
     * \brief Get functor for processing DataChanged command for a specific data container.
     *
     * Get functor for processing DataChanged command for a specific data container.
     *
     * \param[in]   DataContainerName   Name of data container.
     * \return                          The functor.
     */
    /****************************************************************************/
    CmdDataChangedFunctorShPtr_t GetDataChangedProcessingFunctor(const QString &DataContainerName) const;
    /****************************************************************************/
    /**
     * \brief Register a acknowledge for processing.
     *
     * \param[in]   pAcknowledgeProcessor   Pointer to thread controller instance which processes the command acknowledge.
     * \param[in]   FunctionPointer         Function which processes the command acknowledge.
     */
    /****************************************************************************/
    template<class AckClass, class AcknowledgeProcessorClass>
    void RegisterAcknowledgeForProcessing(void(AcknowledgeProcessorClass::*FunctionPointer)(Global::tRefType, const AckClass &),
                                          AcknowledgeProcessorClass *pAcknowledgeProcessor)
    {
        // create functor
        AcknowledgeProcessorFunctorShPtr_t Functor(new TemplateAcknowledgeProcessorFunctor<AcknowledgeProcessorClass, AckClass> (pAcknowledgeProcessor, FunctionPointer));
        // register functor
        RegisterAcknowledgeProcessorFunctor(AckClass::NAME, Functor);
    }
    /****************************************************************************/
    /**
     * \brief Register a command timeout for processing.
     *
     * \param[in]   pTimeoutProcessor       Pointer to thread controller instance which processes the command timeout.
     * \param[in]   FunctionPointer         Function which processes the command timeout.
     */
    /****************************************************************************/
    template<class CmdClass, class TimeoutProcessorClass>
    void RegisterTimeoutForProcessing(void(TimeoutProcessorClass::*FunctionPointer)(Global::tRefType, const QString &),
                                      TimeoutProcessorClass *pTimeoutProcessor)
    {
        // create functor
        TimeoutProcessorFunctorShPtr_t Functor(new TemplateTimeoutProcessorFunctor<TimeoutProcessorClass> (pTimeoutProcessor, FunctionPointer));
        // and register
        RegisterTimeoutProcessingFunctor(CmdClass::NAME, Functor);
    }
    /****************************************************************************/
    /**
     * \brief Register a function for processing DataChanged command for a specific data container.
     *
     * \param[in]   pDataChangedProcessor   Pointer to thread controller instance which processes the DataChanged command.
     * \param[in]   FunctionPointer         Function which processes the specific DataChanged command.
     */
    /****************************************************************************/
    template<class TDataContainerClass, class TDataChangedProcessorClass>
    void RegisterOnDataChanged(void(TDataChangedProcessorClass::*FunctionPointer)(),
                               TDataChangedProcessorClass *pDataChangedProcessor)
    {
        // create functor
        CmdDataChangedFunctorShPtr_t Functor(new Global::TCDataChangedProcessFunctor<TDataChangedProcessorClass> (pDataChangedProcessor, FunctionPointer));
        // and register
        RegisterOnDataChangedProcessingFunctor(TDataContainerClass::NAME, Functor);
    }
    /****************************************************************************/
    /**
     * \brief Dispatch the data changed command to the responsible functions.
     *
     * Depending on the type of data container, the responsible function is determined
     * and called.
     *
     * \param[in]   Cmd     The command.
     */
    /****************************************************************************/
    void DispatchDataChangedCommand(const Global::CmdDataChanged &Cmd) const;
    /****************************************************************************/
    /**
     * \brief Default handler for command timeouts.
     *
     * Will be called when no other handler is installed. Does nothing.
     *
     * \param[in]       Ref         The command reference.
     * \param[in]       CmdName     Name of command.
     */
    /****************************************************************************/
    virtual void OnCmdTimeout(Global::tRefType Ref, const QString &CmdName) {
        // do nothing
        Q_UNUSED(Ref);
        Q_UNUSED(CmdName);
    }

    virtual bool IsCommandAllowed(const Global::CommandShPtr_t &Cmd) {
        Q_UNUSED(Cmd);
        return true;
    }

public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * Although this class is indirectly derived from QObject, we don't provide a
     * parent Object in the constructor, but set it to NULL. This allows calls to
     * moveToThread without reparenting us.
     *
     * \param[in]   TheLoggingSource    Source to set in log entry.
     */
    /****************************************************************************/
    BaseThreadController(Global::gSourceType TheLoggingSource);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~BaseThreadController();
    /****************************************************************************/
    /**
     * \brief Compute new command reference.
     *
     * return   New command reference.
     */
    /****************************************************************************/
    virtual Global::tRefType GetNewCommandRef()
    {
        return m_RefManager.GetNewRef();
    }
    /****************************************************************************/
    /**
     * \brief Unblock command reference.
     *
     * \param[in]   Ref     Command reference to unblock.
     */
    /****************************************************************************/
    inline void UnblockCommandRef(Global::tRefType Ref) {
        m_RefManager.UnblockReference(Ref);
    }
    /****************************************************************************/
    /**
     * \brief Create and initialize used objects.
     *
     * Create and configure all needed objects. <b>If something goes wrong,
     * cleanup yourself to avoid memory leaks!</b> \n
     * <b>Remember, you are still in the thread that called this method. No own thread
     * running now.</b>\n
     * Has to be implemented in derived classes.
     */
    /****************************************************************************/
    virtual void CreateAndInitializeObjects() = 0;
    /****************************************************************************/
    /**
     * \brief Cleanup and destroy used objects.
     *
     * Destroy all created objects.
     * <b>Remember, you are not running in your own thread, but in the thread that called
     * this method.</b>\n
     * Has to be implemented in derived classes.
     */
    /****************************************************************************/
    virtual void CleanupAndDestroyObjects() = 0;
    /****************************************************************************/
    /**
     * \brief Send a data changed command with specific type.
     *
     * Send a data changed command with specific type. A command is constructed
     */
    /****************************************************************************/
    template<class TDataContainerClass> inline void SendDataChanged() {
        DEBUGWHEREAMI;
        // create command and send it
        DoSendDataChanged(Global::CommandShPtr_t(new Global::CmdDataChanged(TDataContainerClass::NAME)));
    }
public slots:
    /****************************************************************************/
    /**
     * \brief Normal multithreaded operation started. We are ready for signal / slot operation.
     *
     * Basicly we call \ref OnGoReceived.
     */
    /****************************************************************************/
    void Go();
    /****************************************************************************/
    /**
     * \brief Normal multithreaded operation will stop after processing.
     *
     * Basicly we call \ref OnStopReceived.
     */
    /****************************************************************************/
    void Stop();
    /****************************************************************************/
    /**
     * \brief Change the heartbeat timeout.
     *
     * Setting it to 0 disables the heartbeat functionality.
     * \warning Changing the timeout after we received the \ref Go signal will not
     * affect the timeout in any way!
     *
     * \param[in]   Timeout     The new timeout in ms. 0 means disabled.
     */
    /****************************************************************************/
    void SetHeartbeatTimeout(int Timeout);
}; // end class BaseThreadController

} // end namespace Threads

#endif // THREADS_BASETHREADCONTROLLER_H
