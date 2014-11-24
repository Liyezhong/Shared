/****************************************************************************/
/*! \file Global/Include/EventObject.h
 *
 *  \brief Definition file for class EventObject.
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
 * \warning This object lives in ColoradoMaster Thread
 */
/****************************************************************************/

#ifndef DATALOGGING_LOGGINGOBJECT_H
#define DATALOGGING_LOGGINGOBJECT_H

//#include <DataLogging/Include/DayEventEntry.h>
#include <Global/Include/Exception.h>
#include <Global/Include/FmtArgs.h>
#include <Global/Include/RefManager.h>


#include <QObject>
#include <QFile>
#include <QDebug>
#include <QMutex>

namespace Global {

/****************************************************************************/
/**
 * \brief Singleton class for Collecting and forwarding Events raised
 *
 * \warning This object lives in ColoradoMaster Thread
 */
/****************************************************************************/
class EventObject : public QObject {
    Q_OBJECT
friend class TestEventObject;
private:

    Global::RefManager<quint32>    m_EventKeyManager;                   ///< We use Reference manager for Key Generation
    QFile                   m_TemporaryLogFile; ///< temp log file name
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(EventObject)

    QFile file2; ///< stores execution output
    QHash<QString, long> m_threadList;  ///< hash of thread name and id
    QMutex m_Lock; //!< mutex for thread safety & synchronization


    /****************************************************************************/
    /**
     * \brief Log to temporary file.
     *
     * \note we use this method only in the case when Initialization has failed,
     *       and the availability of datalogging is not gauranteed.
     *       This method does the job of dumping the error code with text to log
     *       file.
     *
     * \iparam ErrorCode
     */
    /****************************************************************************/
    void LogToTemporaryFile(quint32 ErrorCode);
    /****************************************************************************/
    /**
     * \brief Create new temporary log file
     *
     */
    /****************************************************************************/
    void CreateNewFile();

    /****************************************************************************/
    /**
     * \brief Close file.
     *
     * If file is open, it will be flushed and then closed. File name will be cleared.
     */
    /****************************************************************************/
    inline void CloseFile()  {
        if(m_TemporaryLogFile.isOpen()) {
            static_cast<void>(
                    // we DO NOT NEED the return value of flush
                    m_TemporaryLogFile.flush()
            );
        }
        m_TemporaryLogFile.close();
        m_TemporaryLogFile.setFileName("");
    }

public:

    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    EventObject();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~EventObject() {
    }

    /****************************************************************************/
    /**
     * \brief Get reference to instance.
     *
     * \return      Reference to instance.
     */
    /****************************************************************************/
    static EventObject &Instance() {
        static EventObject    m_EventObjectInstance; //!< Static instance
        return m_EventObjectInstance;
    }

    /****************************************************************************/
    /**
     * \brief logs the thread id to the log file
     *
     * \iparam  threadName = name of the thread
     * \iparam  lwp = thread id
     */
    /****************************************************************************/
    void LogThreadId(QString threadName, long lwp);


    /****************************************************************************/
    /**
     * \brief Method to raise event. The client code must generate unique event
     *        key using \see GetEventKey() before calling this function and pass
     *        the returned event key to this function.
     *
     * \iparam EventCode
     * \iparam EventStringList = Global::FmtArgs() << "Arg1" << "Arg2" ...
     * \iparam EventActive = event is active or not
     * \iparam EventKey = unique event key
     * \iparam AltStringUsage = alternative string shall be used or not
     *
     * \return
     */
    /****************************************************************************/
    inline void RaiseEvent(const quint32 EventCode,
                           const Global::tTranslatableStringList &EventStringList,
                           const bool EventActive,
                           const quint32 EventKey,
                           Global::AlternateEventStringUsage AltStringUsage = Global::NOT_APPLICABLE
                           )  {
        emit ForwardEvent(EventCode, EventStringList, EventActive, EventKey, AltStringUsage);
    }

    /****************************************************************************/
    /**
     * \brief Raise Event
     * \iparam EventCode   = 32 bit unique event code
     * \iparam EventStringList = Global::FmtArgs() << "Arg1" << "Arg2" ...
     * \iparam EventActive = True -if event active, False - if event not active
     * \iparam AltStringUsage = alternative string shall be used or not
     * \return
     */
    /****************************************************************************/
    inline void RaiseEvent(const quint32 EventCode,
                           const Global::tTranslatableStringList &EventStringList,
                           const bool EventActive = true,
                           Global::AlternateEventStringUsage AltStringUsage = Global::NOT_APPLICABLE
                           )  {
        quint32 EventKey = GetEventKey();
        emit ForwardEvent(EventCode, EventStringList, EventActive, EventKey, AltStringUsage);
    }

    /****************************************************************************/
    /**
     * \brief Raise Event
     * \iparam EventCode   = 32 bit unique event code
     * \iparam AltStringUsage = alternative string shall be used or not
     * \return
     */
    /****************************************************************************/
    inline void RaiseEvent(const quint32 EventCode,
                           Global::AlternateEventStringUsage AltStringUsage = Global::NOT_APPLICABLE)
    {
        quint32 EventKey = GetEventKey();
        emit ForwardEvent(EventCode, Global::tTranslatableStringList(), true, EventKey, AltStringUsage);
        //qDebug()<<"EventObject Thread"<<this->thread();
    }

    /****************************************************************************/
    /**
     * \brief Raise Event
     * \iparam EventCode   = 32 bit unique event code
     * \iparam EventActive = True -if event active, False - if event not active
     * \iparam AltStringUsage = alternative string shall be used or not
     */
    /****************************************************************************/
    inline void RaiseEvent(const quint32 EventCode,
                           const bool EventActive,
                           Global::AlternateEventStringUsage AltStringUsage = Global::NOT_APPLICABLE)
    {
        quint32 EventKey = GetEventKey();
        emit ForwardEvent(EventCode, Global::tTranslatableStringList(), EventActive, EventKey, AltStringUsage);
    }

    /****************************************************************************/
    /**
     * \brief Raise Event.
     * \iparam TheException
     *
     */
    /****************************************************************************/
    inline void RaiseEvent(const Global::Exception &TheException)
    {
        //! When exception is raised , Append Filename and line numbers into Argumentlist for logging purpose
        Global::tTranslatableStringList StringList = TheException.GetAdditionalData();
        StringList << TheException.GetFile() << TheException.GetLine();
        emit ForwardEvent(TheException.GetErrorCode(), StringList, true, GetEventKey(), Global::NOT_APPLICABLE);
    }

    /****************************************************************************/
    /**
     * \brief Raise Event (this version should be used to log catched exception in a destructor)
     * \iparam EventCode    = 32 bit unique event code
     * \iparam File         = string with file path to destructor
     * \iparam Line         = line number inside destructor
     * \return
     */
    /****************************************************************************/
    inline void RaiseEventDtor(const quint32 EventCode, const char * File, const int Line)
    {
        quint32 EventKey = GetEventKey();
        Global::tTranslatableStringList StringList = Global::tTranslatableStringList();
        StringList << File << QString::number(Line);
        emit ForwardEvent(EventCode, StringList, true, EventKey, Global::NOT_APPLICABLE);
    }

    /****************************************************************************/
    /**
     * \brief Raise Event (this version should be used to log catched exception in a destructor)
     * \iparam EventCode    = 32 bit unique event code
     * \iparam What         = string identifying exception
     * \iparam File         = string with file path to destructor
     * \iparam Line         = line number inside destructor
     * \return
     */
    /****************************************************************************/
    inline void RaiseEventDtor(const quint32 EventCode, const char * What, const char * File, const int Line)
    {
        quint32 EventKey = GetEventKey();
        Global::tTranslatableStringList StringList = Global::tTranslatableStringList();
        StringList << What << File << QString::number(Line);
        emit ForwardEvent(EventCode, StringList, true, EventKey, Global::NOT_APPLICABLE);
    }

    /****************************************************************************/
    /*!
     *  \brief    This SLOT handles all incoming events.
     *
     *      This SLOT shall be used to process all events and their current
     *      status.
     *
     *  \iparam    EventKey
     *  \iparam    EventID
     *  \iparam    Scenario
     *  \iparam    Active
     *  \iparam    ActionResult
     *  \iparam    EventStringParList
     *  \iparam    EventRDStringParList
     *
     *
     ****************************************************************************/
    virtual void RaiseEvent(const quint32 EventKey, const quint32 EventID,
                              const bool ActionResult, const bool Active = true,
                              const Global::tTranslatableStringList &EventStringParList = Global::tTranslatableStringList(),
                              const Global::tTranslatableStringList &EventRDStringParList = Global::tTranslatableStringList())
    {
        quint32 key = EventKey;
        if(EventKey <= 0){
            key = GetEventKey();
        }
        emit ForwardEvent(key, EventID, Active, ActionResult,EventStringParList,EventRDStringParList);
    }

    /****************************************************************************/
    /**
     * \brief Raise Event (this version should be used to log catched exception in a destructor)
     * \iparam EventCode    = 32 bit unique event code
     * \iparam ExcEventCode = 32 bit unique event code of catched exception
     * \iparam ExcWhere     = file and line number at which catched exception have been thrown
     * \iparam File         = string with file path to destructor
     * \iparam Line         = line number inside destructor
     * \return
     */
    /****************************************************************************/
    inline void RaiseEventDtor(const quint32 EventCode, QString ExcEventCode, QString ExcWhere,
                               const char * File, const int Line)
    {
        quint32 EventKey = GetEventKey();
        Global::tTranslatableStringList StringList = Global::tTranslatableStringList();
        StringList << ExcEventCode << ExcWhere << File << QString::number(Line);
        emit ForwardEvent(EventCode, StringList, true, EventKey, Global::NOT_APPLICABLE);
    }

    /****************************************************************************/
    /**
     * \brief This method is used to generate an unique Event Key
     * \return Unique event key
     */
    /****************************************************************************/
    quint32 GetEventKey() {
        QMutexLocker Lock(&m_Lock);
        return m_EventKeyManager.GetNewRef();
    }

    /****************************************************************************/
    /**
     * \brief This method is used to remove the event key from EvenKey Manager.
     *        This allows Event Key manager to reuse the unblocked key.
     *
     * \iparam EventKey = Event Key to Unblock
     */
    /****************************************************************************/
    void UnBlockEventKey(quint32 EventKey) {
        m_EventKeyManager.UnblockReference(EventKey);
    }

    /****************************************************************************/
    /**
     * \brief Log to temporary file and throw exception.This exception shall be
     *        caught by Main.
     *
     * \note we use this method only in the case when Initialization has failed,
     *       and the availability of datalogging is not gauranteed.
     *       This method does the job of dumping the error code with text to log
     *       file.
     *
     * \iparam ErrorCode
     */
    /****************************************************************************/
    void LogToTemporaryFileAndThrowArgs(const quint32 ErrorCode) {
        LogToTemporaryFile(ErrorCode);
        Global::Exception E(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), ErrorCode,
                            Global::FmtArgs());
        throw E;
    }

signals:
    /****************************************************************************/
    /**
     * \brief This signal is used to connect EventObject to Event Handler thread
     *        controller
     * \param EventKey event key
     */
    /****************************************************************************/
    void ForwardEvent(const quint32, const Global::tTranslatableStringList &, const bool, quint32 EventKey, const Global::AlternateEventStringUsage);

    /****************************************************************************/
    /**
     * \brief This signal is used to connect EventObject to Event Handler thread
     *        controller
     * \param  EventKey Event Key
     * \param  Event id
     * \param  Active active flag
     * \param  ActionResult action result
     * \param  EventStringParList param list
     * \param  EventRDStringParList RD param list
     */
    /****************************************************************************/
    void ForwardEvent(const quint32 EventKey, const quint32 Event,
                      const bool Active, const bool ActionResult,
                      const Global::tTranslatableStringList &EventStringParList,
                      const Global::tTranslatableStringList &EventRDStringParList);

}; // end class EventObject

/****************************************************************************/
/**
 * \brief Helper for logging an throwing error code exception
 *
 * \iparam   ErrorCode   error code
 */
/****************************************************************************/
#define LOGANDTHROW(ErrorCode) \
{ \
    Global::Exception E(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), ErrorCode); \
    Global::EventObject::Instance().RaiseEvent(E); \
    throw E; \
}

/****************************************************************************/
/**
 * \brief Helper for logging an throwing error code exception with argument
 *
 * \iparam   ErrorCode   error code
 * \iparam   Arg         Argument
 */
/****************************************************************************/
#define LOGANDTHROWARG(ErrorCode, Arg) \
{ \
    Global::Exception E(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), ErrorCode, Arg); \
    Global::EventObject::Instance().RaiseEvent(E); \
    throw E; \
}

/****************************************************************************/
/**
 * \brief Helper for logging an throwing error code exception with argument list
 *
 * \iparam   ErrorCode   error code
 * \iparam   ArgList     Argument List
 */
/****************************************************************************/
#define LOGANDTHROWARGS(ErrorCode, ArgList) \
{ \
    Global::Exception E(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), ErrorCode, ArgList); \
    Global::EventObject::Instance().RaiseEvent(E); \
    throw E; \
}

} // end namespace Global

#endif // DATALOGGING_EventObject_H
