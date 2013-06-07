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
 * \warning This object lives in HimalayaMaster Thread
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

namespace Global {


/****************************************************************************/
/**
 * \brief Helper for sending an event of type exception with status = EVTSTAT_ON.
 *
 * The current file name, file number and time stamp are set automatically.
 *
 * \param[in]   exc     The exception.
 */
/****************************************************************************/
#define SEND_EXCEPTION(exc) SendEvent(exc, Global::EVTTYPE_ERROR, Global::EVTSTAT_ON);

/****************************************************************************/
///**
// * \brief Helper for sending an DayOperation info.
// *
// * \param[in]   eventType   info/warning/error/fatal error
// * \param[in]   logLevel    Various logging levels
// * \param[in]   code        String ID.
// * \param[in]   data        Additional data.
// */
///****************************************************************************/
#define LOG_EVENT(eventType, logLevel, code, data, numericData, showInRunLog)
#define SEND_EVENT(eventType, logLevel, code, data, numericData, showInRunLog)



/****************************************************************************/
//**
// * \brief Helper for sending an Event To Event Handler


/****************************************************************************/

/**
 * \brief Helper for sending an event of type debug with status = EVTSTAT_ON.
 *
 * The current file name, file number and time stamp are set automatically.
 *
 * \param[in]   data    Additional data.
 */
/****************************************************************************/
#if defined(QT_NO_DEBUG)
    // we are in release mode
    #define SEND_DEBUG(data)
#else
    // we are in debug mode
    #define SEND_DEBUG(data) SendEvent(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), Global::EVTTYPE_DEBUG, Global::EVTSTAT_ON, EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, data);
#endif

/****************************************************************************/
/**
 * \brief Singleton class for Collecting and forwarding Events raised
 *
 * \warning This object lives in HimalayaMaster Thread
 */
/****************************************************************************/
class EventObject : public QObject {
    Q_OBJECT
friend class TestEventObject;
private:
    static EventObject    m_EventObjectInstance; //!< Static instance
    Global::RefManager<quint32>    m_EventKeyManager;                   ///< We use Reference manager for Key Generation
    QFile                   m_TemporaryLogFile;
    EventObject(const EventObject &);   ///< Not implemented.

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
    static inline EventObject &Instance() {
        return m_EventObjectInstance;
    }

    /****************************************************************************/
    /**
     * \brief Method to raise event. The client code must generate unique event
     *        key using \see GetEventKey() before calling this function and pass
     *        the returned event key to this function.
     *
     * \iparam EventCode
     * \iparam EventStringList = Global::FmtArgs() << "Arg1" << "Arg2" ...
     * \iparam EventStatus
     * \iparam EventKey = unique event key
     *
     * \return
     */
    /****************************************************************************/
    inline void RaiseEvent(const quint32 EventCode,
                           const Global::tTranslatableStringList &EventStringList,
                           const bool IsResolved,
                           const quint32 EventKey,
                           Global::AlternateEventStringUsage AltStringUsage = Global::NOT_APPLICABLE,
                           Global::tTranslatableStringList EventStringListForRd = Global::tTranslatableStringList(),
                           quint64 EventCodeScenario = 0
                           )  {
        emit ForwardEvent(EventCode, EventStringList, IsResolved, EventKey, AltStringUsage,EventStringListForRd, EventCodeScenario);
    }

    /****************************************************************************/
    /**
     * \brief Raise Event
     *
     * \return
     */
    /****************************************************************************/
    inline void RaiseEvent(const quint32 EventCode,
                           const Global::tTranslatableStringList &EventStringList,
                           const bool IsResolved = true,
                           Global::AlternateEventStringUsage AltStringUsage = Global::NOT_APPLICABLE,
                           Global::tTranslatableStringList EventStringListForRD = Global::tTranslatableStringList(),
                           quint64 EventCodeScenario = 0
                           )  {
        quint32 EventKey = GetEventKey();
        emit ForwardEvent(EventCode, EventStringList, IsResolved, EventKey, AltStringUsage,EventStringListForRD, EventCodeScenario);
    }

    /****************************************************************************/
    /**
     * \brief Raise Event
     *
     * \return
     */
    /****************************************************************************/
    inline void RaiseEvent(const quint32 EventCode,  Global::AlternateEventStringUsage AltStringUsage = Global::NOT_APPLICABLE,
                            bool IsResolved = false)
    {
        quint32 EventKey = GetEventKey();
        emit ForwardEvent(EventCode, Global::tTranslatableStringList(), IsResolved, EventKey, AltStringUsage,Global::tTranslatableStringList(),0);
        qDebug()<<"EventObject Thread"<<this->thread();
    }

    /****************************************************************************/
    /**
     * \brief Raise Event
     * \iparam EventCode   = 32 bit unique event code
     * \iparam EventStatus = True -if event active, False - if event not active
     */
    /****************************************************************************/
    inline void RaiseEvent(const quint32 EventCode, const bool IsResolved,
                           Global::AlternateEventStringUsage AltStringUsage = Global::NOT_APPLICABLE)
    {
        quint32 EventKey = GetEventKey();
        emit ForwardEvent(EventCode, Global::tTranslatableStringList(), IsResolved, EventKey, AltStringUsage, Global::tTranslatableStringList(),0);
        qDebug()<<"EventObject Thread"<<this->thread();
    }

    /****************************************************************************/
    /**
     * \brief Raise Exception.
     *
     */
    /****************************************************************************/
    inline void RaiseException(const Global::Exception &TheException)
    {
        //! When exception is raised , Append Filename and line numbers into Argumentlist for logging purpose
        Global::tTranslatableStringList StringList = TheException.GetAdditionalData();
        StringList << TheException.GetFile() << TheException.GetLine();
        RaiseEvent(TheException.GetErrorCode(), StringList, true);
    }

    /****************************************************************************/
    /**
     * \brief This method is used to generate an unique Event Key
     * \return Unique event key
     */
    /****************************************************************************/
    quint32 GetEventKey() {
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

    inline void RaiseLog(QString info)
    {
        emit ForwardLog(info);
    }

signals:
    /****************************************************************************/
    /**
     * \brief This signal is used to connect EventObject to Event Handler thread
     *        controller
     */
    /****************************************************************************/
    void ForwardEvent(const quint32, const Global::tTranslatableStringList &, const bool, quint32 EventKey, const Global::AlternateEventStringUsage, const Global::tTranslatableStringList,quint64);

    void ForwardLog(QString info);
}; // end class EventObject

#define LOGANDTHROW(ErrorCode) \
{ \
    Global::Exception E(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), ErrorCode); \
    Global::EventObject::Instance().RaiseException(E); \
    throw E; \
}

#define LOGANDTHROWARG(ErrorCode, Arg) \
{ \
    Global::Exception E(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), ErrorCode, Arg); \
    Global::EventObject::Instance().RaiseException(E); \
    throw E; \
}

#define LOGANDTHROWARGS(ErrorCode, ArgList) \
{ \
    Global::Exception E(__FILE__, __LINE__, Global::AdjustedTime::Instance().GetCurrentDateTime(), ErrorCode, ArgList); \
    Global::EventObject::Instance().RaiseException(E); \
    throw E; \
}

} // end namespace Global

#endif // DATALOGGING_EventObject_H
