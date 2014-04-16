
/****************************************************************************/
/*! \file Threads/Include/CommandFunctors.h
 *
 *  \brief Definition file for functors needed by command classes.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-03-11
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

#ifndef THREADS_COMMANDFUNCTORS_H
#define THREADS_COMMANDFUNCTORS_H

#include<Threads/Include/CommandChannel.h>
#include<Threads/Include/ThreadsEventCodes.h>
#include<Global/Include/Utils.h>
#include <QDebug>
#include <Global/Include/EventObject.h>

namespace Threads {

/****************************************************************************/
/**
 * \brief Base class for execute functors without acknowledge channels.
 *
 * \warning We use explicit calls to \ref Execute not overloading () operator.
 */
/****************************************************************************/
class CommandExecuteFunctor {
private:
    /****************************************************************************/
    CommandExecuteFunctor(const CommandExecuteFunctor &);                       ///< Not implemented.
    const CommandExecuteFunctor & operator = (const CommandExecuteFunctor &);   ///< Not implemented.
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline CommandExecuteFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CommandExecuteFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Execute the desired function.
     *
     * \iparam   Ref         Command reference.
     * \iparam   pCommand    Pointer to command which has to be executed.
     */
    /****************************************************************************/
    virtual void Execute(Global::tRefType Ref, const Global::Command *pCommand) = 0;
}; // end class CommandExecuteFunctor

/****************************************************************************/
/**
 * \brief Template functor class for specific execute functor without acknowledge channels.
 *
 * This kind of functor is used to "link" commands to specific processing
 * routines. When executed, the needed type casts are done automagically.
 * \warning We use explicit calls to \ref Execute not overloading () operator.
 */
/****************************************************************************/
template<class CommandProcessorClass, class CmdClass> class TemplateCommandExecuteFunctor : public CommandExecuteFunctor {
private:
    CommandProcessorClass           *m_pCommandProcessor;                                       ///< Pointer to command processor instance.
    void (CommandProcessorClass::   *m_FunctionPointer)(Global::tRefType, const CmdClass &);    ///< Pointer to member function.
    /****************************************************************************/
    TemplateCommandExecuteFunctor();                                                            ///< Not implemented.
    TemplateCommandExecuteFunctor(const TemplateCommandExecuteFunctor &);                       ///< Not implemented.
    const TemplateCommandExecuteFunctor & operator = (const TemplateCommandExecuteFunctor &);   ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   pCommandProcessor       Pointer to command processor instance to which the function pointer belongs.
     * \iparam   FunctionPointer         The function pointer.
     */
    /****************************************************************************/
    TemplateCommandExecuteFunctor(CommandProcessorClass *pCommandProcessor,
                                  void(CommandProcessorClass::*FunctionPointer)(Global::tRefType, const CmdClass &)) :
        m_pCommandProcessor(pCommandProcessor),
        m_FunctionPointer(FunctionPointer)
    {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~TemplateCommandExecuteFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Execute the desired function.
     *
     * \iparam   Ref         Command reference.
     * \iparam   pCommand    Pointer to command which has to be executed.
     */
    /****************************************************************************/
    virtual void Execute(Global::tRefType Ref, const Global::Command *pCommand) {
        CHECKPTR(pCommand);
        // type cast to correct command
        const CmdClass *pExecCmd = dynamic_cast<const CmdClass *>(pCommand);
        // check result
        if(pExecCmd == NULL) {
            // throw error
            LOGANDTHROWARG(EVENT_THREADS_ERROR_UNSUPPORTED_COMMAND, pCommand->GetName())
            qDebug()<<"CommandFunctors" << pCommand->GetName();
            return;
        }
        // execute command
        (*m_pCommandProcessor.*m_FunctionPointer)(Ref, *pExecCmd);
    }
}; // end class TemplateCommandExecuteFunctor

/****************************************************************************/
/**
 * \brief Base class for execute functors with acknowlege channel.
 *
 * \warning We use explicit calls to \ref Execute not overloading () operator.
 */
/****************************************************************************/
class CommandExecuteFunctorAck {
private:
    /****************************************************************************/
    CommandExecuteFunctorAck(const CommandExecuteFunctorAck &);                     ///< Not implemented.
    const CommandExecuteFunctorAck & operator = (const CommandExecuteFunctorAck &); ///< Not implemented.
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline CommandExecuteFunctorAck() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CommandExecuteFunctorAck() {
    }
    /****************************************************************************/
    /**
     * \brief Execute the desired function.
     *
     * \iparam       Ref                 Command reference.
     * \iparam       pCommand            Pointer to command which has to be executed.
     * \iparam  AckCommandChannel   The command channel for acknowledges.
     */
    /****************************************************************************/
    virtual void Execute(Global::tRefType Ref, const Global::Command *pCommand, CommandChannel &AckCommandChannel) = 0;
}; // end class CommandExecuteFunctor

/****************************************************************************/
/**
 * \brief Template functor class for specific execute functor with acknowledge channel.
 *
 * This kind of functor is used to "link" commands to specific processing
 * routines. When executed, the needed type casts are done automagically.
 * \warning We use explicit calls to \ref Execute not overloading () operator.
 */
/****************************************************************************/
template<class CommandProcessorClass, class CmdClass> class TemplateCommandExecuteFunctorAck : public CommandExecuteFunctorAck {
private:
    CommandProcessorClass           *m_pCommandProcessor;                                                       ///< Pointer to command processor instance.
    void (CommandProcessorClass::   *m_FunctionPointer)(Global::tRefType, const CmdClass &, CommandChannel &);  ///< Pointer to member function.
    /****************************************************************************/
    TemplateCommandExecuteFunctorAck();                                                             ///< Not implemented.
    TemplateCommandExecuteFunctorAck(const TemplateCommandExecuteFunctorAck &);                     ///< Not implemented.
    const TemplateCommandExecuteFunctorAck & operator = (const TemplateCommandExecuteFunctorAck &); ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   pCommandProcessor       Pointer to command processor instance to which the function pointer belongs.
     * \iparam   FunctionPointer         The function pointer.
     */
    /****************************************************************************/
    TemplateCommandExecuteFunctorAck(CommandProcessorClass *pCommandProcessor,
                                     void(CommandProcessorClass::*FunctionPointer)(Global::tRefType, const CmdClass &, CommandChannel &)) :
        m_pCommandProcessor(pCommandProcessor),
        m_FunctionPointer(FunctionPointer)
    {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~TemplateCommandExecuteFunctorAck() {
    }
    /****************************************************************************/
    /**
     * \brief Execute the desired function.
     *
     * \iparam       Ref                 Command reference.
     * \iparam       pCommand            Pointer to command which has to be executed.
     * \iparam  AckCommandChannel   The command channel for acknowledges.
     */
    /****************************************************************************/
    virtual void Execute(Global::tRefType Ref, const Global::Command *pCommand, CommandChannel &AckCommandChannel) {
        CHECKPTR(pCommand);
        // type cast to correct command
        const CmdClass *pExecCmd = dynamic_cast<const CmdClass *>(pCommand);
        // check result
        if(pExecCmd == NULL) {
            // throw error
            LOGANDTHROWARG(EVENT_THREADS_ERROR_UNSUPPORTED_COMMAND, pCommand->GetName())
            qDebug()<<"CommandFunctors" << pCommand->GetName();
            return;
        }
        // execute command
        (*m_pCommandProcessor.*m_FunctionPointer)(Ref, *pExecCmd, AckCommandChannel);
    }
}; // end class TemplateCommandExecuteFunctor

/****************************************************************************/
/**
 * \brief Base functor class for command acknowledges.
 *
 * This kind of functor is used to "link" acknowledges to specific processing
 * routines. When executed, the needed type casts are done automagically.
 * \warning We use explicit calls to \ref Process not overloading () operator.
 */
/****************************************************************************/
class AcknowledgeProcessorFunctor {
private:
    /****************************************************************************/
    AcknowledgeProcessorFunctor(const AcknowledgeProcessorFunctor &);                       ///< Not implemented.
    const AcknowledgeProcessorFunctor & operator = (const AcknowledgeProcessorFunctor &);   ///< Not implemented.
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline AcknowledgeProcessorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~AcknowledgeProcessorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Process the desired acknowledge.
     *
     * \iparam       Ref                 Command reference.
     * \iparam       pAcknowledge        Pointer to acknowledge which has to be processed.
     */
    /****************************************************************************/
    virtual void Process(Global::tRefType Ref, const Global::Acknowledge *pAcknowledge) = 0;
}; // end class AcknowledgeProcessorFunctor

/****************************************************************************/
/**
 * \brief Template functor class for specific acknowledge processing functor.
 *
 * This kind of functor is used to "link" Acknowledges to specific processing
 * routines. When executed, the needed type casts are done automagically.
 * \warning We use explicit calls to \ref Process not overloading () operator.
 */
/****************************************************************************/
template<class AcknowledgeProcessorClass, class AckClass> class TemplateAcknowledgeProcessorFunctor : public AcknowledgeProcessorFunctor {
private:
    AcknowledgeProcessorClass          *m_pAcknowledgeProcessor;                                ///< Pointer to acknowledge processor instance.
    void (AcknowledgeProcessorClass::  *m_FunctionPointer)(Global::tRefType, const AckClass &); ///< Pointer to member function.
    /****************************************************************************/
    TemplateAcknowledgeProcessorFunctor();                                                                  ///< Not implemented.
    TemplateAcknowledgeProcessorFunctor(const TemplateAcknowledgeProcessorFunctor &);                       ///< Not implemented.
    const TemplateAcknowledgeProcessorFunctor & operator = (const TemplateAcknowledgeProcessorFunctor &);   ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   pAcknowledgeProcessor   Pointer to acknowledge processor instance to which the function pointer belongs.
     * \iparam   FunctionPointer         The function pointer.
     */
    /****************************************************************************/
    TemplateAcknowledgeProcessorFunctor(AcknowledgeProcessorClass *pAcknowledgeProcessor,
                                        void(AcknowledgeProcessorClass::*FunctionPointer)(Global::tRefType, const AckClass &)) :
        m_pAcknowledgeProcessor(pAcknowledgeProcessor),
        m_FunctionPointer(FunctionPointer)
    {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~TemplateAcknowledgeProcessorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Process the desired acknowledge.
     *
     * \iparam       Ref                 Command reference.
     * \iparam       pAcknowledge        Pointer to acknowledge which has to be processed.
     */
    /****************************************************************************/
    virtual void Process(Global::tRefType Ref, const Global::Acknowledge *pAcknowledge) {
        CHECKPTR(pAcknowledge);
        // type cast to correct acknkowledge
        const AckClass *pDerivedAck = dynamic_cast<const AckClass *>(pAcknowledge);
        // check result
        if(pAcknowledge == NULL) {
            // throw error
            LOGANDTHROWARG(EVENT_THREADS_ERROR_UNSUPPORTED_ACKNOWLEDGE, pAcknowledge->GetName())
            return;
        }
        // process acknowledge
        (*m_pAcknowledgeProcessor.*m_FunctionPointer)(Ref, *pDerivedAck);
    }
}; // end class TemplateAcknowledgeProcessorFunctor

/****************************************************************************/
/**
 * \brief Base functor class for command timeouts.
 *
 * This kind of functor is used to "link" timeouts to specific processing
 * routines.
 * \warning We use explicit calls to \ref Process not overloading () operator.
 */
/****************************************************************************/
class TimeoutProcessorFunctor {
private:
    /****************************************************************************/
    TimeoutProcessorFunctor(const TimeoutProcessorFunctor &);                       ///< Not implemented.
    const TimeoutProcessorFunctor & operator = (const TimeoutProcessorFunctor &);   ///< Not implemented.
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline TimeoutProcessorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~TimeoutProcessorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Process the desired acknowledge.
     *
     * \iparam       Ref         Command reference.
     * \iparam       Name        Name of command.
     */
    /****************************************************************************/
    virtual void Process(Global::tRefType Ref, const QString &Name) = 0;
}; // end class TimeoutProcessorFunctor

/****************************************************************************/
/**
 * \brief Template functor class for specific timeout processing functor.
 *
 * This kind of functor is used to "link" timeouts to specific processing
 * routines.
 * \warning We use explicit calls to \ref Process not overloading () operator.
 */
/****************************************************************************/
template<class TimeoutProcessorClass> class TemplateTimeoutProcessorFunctor : public TimeoutProcessorFunctor {
private:
    TimeoutProcessorClass           *m_pTimeoutProcessor;                                   ///< Pointer to acknowledge processor instance.
    void (TimeoutProcessorClass::   *m_FunctionPointer)(Global::tRefType, const QString &); ///< Pointer to member function.
    /****************************************************************************/
    TemplateTimeoutProcessorFunctor();                                                              ///< Not implemented.
    TemplateTimeoutProcessorFunctor(const TemplateTimeoutProcessorFunctor &);                       ///< Not implemented.
    const TemplateTimeoutProcessorFunctor & operator = (const TemplateTimeoutProcessorFunctor &);   ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   pTimeoutProcessor       Pointer to timeout processor instance to which the function pointer belongs.
     * \iparam   FunctionPointer         The function pointer.
     */
    /****************************************************************************/
    TemplateTimeoutProcessorFunctor(TimeoutProcessorClass *pTimeoutProcessor,
                                  void(TimeoutProcessorClass::*FunctionPointer)(Global::tRefType, const QString &)) :
        m_pTimeoutProcessor(pTimeoutProcessor),
        m_FunctionPointer(FunctionPointer)
    {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~TemplateTimeoutProcessorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Process the desired acknowledge.
     *
     * \iparam       Ref     Command reference.
     * \iparam       Name    Name of command.
     */
    /****************************************************************************/
    virtual void Process(Global::tRefType Ref, const QString &Name) {
        // process timeout
        (*m_pTimeoutProcessor.*m_FunctionPointer)(Ref, Name);
    }
}; // end class TemplateTimeoutProcessorFunctor

/****************************************************************************/
/**
 * \brief Base functor class for creating and deserializing commands and acknowledges.
 *
 * This kind of functor is used to "link" creation and deserialization to specific methods.
 * \warning We use explicit calls to \ref CreateAndDeserialize not overloading () operator.
 */
/****************************************************************************/
class CreatorFunctor {
private:
    /****************************************************************************/
    CreatorFunctor(const CreatorFunctor &);                     ///< Not implemented.
    const CreatorFunctor & operator = (const CreatorFunctor &); ///< Not implemented.
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline CreatorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CreatorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Create instance an deserialize it.
     *
     * \iparam       Ref         Reference.
     * \iparam  DataStream  Data stream from which deserialization is done.
     */
    /****************************************************************************/
    virtual void CreateAndDeserialize(Global::tRefType Ref, QDataStream &DataStream) = 0;
}; // end class CreatorFunctor

/****************************************************************************/
/**
 * \brief Template functor class for specific creating and deserializing.
 *
 * This kind of functor is used to "link" creation and deserialization to specific methods.
 * \warning We use explicit calls to \ref CreateAndDeserialize not overloading () operator.
 */
/****************************************************************************/
template<class TheClass, class TheProcessor> class TemplateCreatorFunctor : public CreatorFunctor {
private:
    TheProcessor            *m_pTheProcessor;                                   ///< Pointer to processor class.
    void (TheProcessor::    *m_FunctionPointer)(Global::tRefType, TheClass *);  ///< Pointer to member function.
    /****************************************************************************/
    TemplateCreatorFunctor();                                                   ///< Not implemented.
    TemplateCreatorFunctor(const TemplateCreatorFunctor &);                     ///< Not implemented.
    const TemplateCreatorFunctor & operator = (const TemplateCreatorFunctor &); ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   pTheProcessor       Processor class.
     * \iparam   FunctionPointer     The function pointer.
     */
    /****************************************************************************/
    TemplateCreatorFunctor(TheProcessor *pTheProcessor,
                           void(TheProcessor::*FunctionPointer)(Global::tRefType, TheClass *)) :
        m_pTheProcessor(pTheProcessor),
        m_FunctionPointer(FunctionPointer)
    {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~TemplateCreatorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Create instance an deserialize it.
     *
     * \warning You must ensure correct deletion of the returned pointer!
     *
     * \iparam       Ref         Reference.
     * \iparam  DataStream  Data stream from which deserialization is done.
     */
    /****************************************************************************/
    virtual void CreateAndDeserialize(Global::tRefType Ref, QDataStream &DataStream) {
//        qDebug() << "CommandFunctors::CreateAndDeserialize";
        // create temporary instance
        TheClass *pInstance = new TheClass;
        // stream data into it
        DataStream >> (*pInstance);
        // pass new created instance to consumer
        (*m_pTheProcessor.*m_FunctionPointer)(Ref, pInstance);
    }
}; // end class TemplateCreatorFunctor

} // end namespace Threads

#endif // THREADS_COMMANDFUNCTORS_H
