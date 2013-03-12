/****************************************************************************/
/*! \file CrisisEventHandler.h
 *
 *  \brief Definition file for class CrisisEventHandler
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-28
 *  $Author:    $ Abe Yang
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef EVENTHANDLER_CRISISEVENTHANDLER_H
#define EVENTHANDLER_CRISISEVENTHANDLER_H
#include <QObject>
#include <QMultiHash>
#include <Global/Include/EventObject.h>

namespace EventHandler {


/****************************************************************************/
/**
 * \brief Crisis event handler for application.
 */
/****************************************************************************/
class CrisisEventHandler : public QObject
{
    Q_OBJECT

public:
    /****************************************************************************/
    /**
     * \brief Get reference to instance.
     *
     * \return      Reference to instance.
     */
    /****************************************************************************/
    static inline CrisisEventHandler &Instance()
    {
        return m_EventHandlerInstance;
    }

    /****************************************************************************/
    /**
     * \brief Keep Current system state.
     *
     * \param[in]   the current will be kept
     * \return      
     */
    /****************************************************************************/
    void currentSysState(quint32 stateId);

    /****************************************************************************/
    /**
     * \brief File parser for the eventstateError.csv.
     *
     * \param[in]   valid event config csv file name
     *
     * \return      indicate this operation whether successful or not      
     */
    /****************************************************************************/
    bool readEventStateConf(const QString filename);
    
    /****************************************************************************/
    /**
     * \brief Find the corresponding Error Code based the given event Id .
     *
     * \param[in]   valid event ID
     *
     * \return      Error Code, if error code is 0, means can not find the error code
     */
    /****************************************************************************/
    quint32 findErrorCode(quint32 eventId);

private:
    CrisisEventHandler();
    ~CrisisEventHandler();
    static CrisisEventHandler m_EventHandlerInstance; //!< Static instance
    Q_DISABLE_COPY(CrisisEventHandler ) //!< Disable Copy and assignment
    quint32 m_currentSysState;  //!< Keep current system state
    QMultiHash< quint32, QPair<QStringList,quint32> > m_EventStateErrorHash; //!< QHash<EventId, QPair<StateList, ErrorId>
}; // end class

#define PROCESSCRISISEVENT(EventId) \
{ \
    quint32 errorCode = EventHandler::CrisisEventHandler::Instance().findErrorCode(EventId); \
    if (errorCode){ \
        Global::EventObject::Instance().RaiseEvent(errorCode); \
    } \
}

#define POSTPROCESSCRISISEVENT(ErrorCode, IsResolved) \
{ \
    Global::EventObject::Instance().RaiseEvent(ErrorCode, true, Global::NOT_APPLICABLE, IsResolved, true); \
}

#define PROCESSCRISISEVENTARG(EventId, Arg) \
{ \
    quint32 errorCode = EventHandler::CrisisEventHandler::Instance().findErrorCode(EventId); \
    if (errorCode){ \
       Global::EventObject::Instance().RaiseEvent(errorCode, Global::FmtArgs() << Arg, true); \
    } \
}

#define POSTPROCESSCRISISEVENTARG(ErrorCode, Arg, IsResolved) \
{ \
    Global::EventObject::Instance().RaiseEvent(ErrorCode, Global::FmtArgs() << Arg, true, Global::NOT_APPLICABLE, IsResolved, true); \
}

#define PROCESSCRISISEVENTARGS(EventId, ArgList) \
{ \
    quint32 errorCode = EventHandler::CrisisEventHandler::Instance().findErrorCode(EventId); \
    if (errorCode){ \
        Global::EventObject::Instance().RaiseEvent(errorCode, ArgList, true); \
    } \
}

#define POSTPROCESSCRISISEVENTARGS(ErrorCode, ArgList, IsResolved) \
{ \
    Global::EventObject::Instance().RaiseEvent(ErrorCode, ArgList, true, Global::NOT_APPLICABLE, IsResolved, true); \
}

} // end namespace EventHandler

#endif // EVENTHANDLER_CRISISEVENTHANDLER_H
