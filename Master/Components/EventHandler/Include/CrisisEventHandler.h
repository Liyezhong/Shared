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
#include <QStringList>
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
     * \brief Raise new Event
     *
     * \return
     */
    /****************************************************************************/
    inline void RaiseEvent(quint32 EventCode, quint32 Scenario,
                           const Global::tTranslatableStringList &msgArgs,
                           Global::tTranslatableStringList msgRdArgs,
                           bool isResolved = false)
    {
        quint32 ErrorCode = EventHandler::CrisisEventHandler::Instance().findErrorCode(EventCode,Scenario);

        quint64 EventCodeScenario = EventCode;
        EventCodeScenario <<= 32;
        EventCodeScenario |=Scenario;
        if(ErrorCode)
        {
            Global::EventObject::Instance().RaiseEvent(ErrorCode, msgArgs, isResolved,Global::NOT_APPLICABLE, msgRdArgs,EventCodeScenario);
        }
    }

    /****************************************************************************/
    /**
     * \brief Raise Event result
     *
     * \return
     */
    /****************************************************************************/
    inline void RaiseEvent(quint32 EventCode, quint32 Scenario, quint32 EventKey, bool isResolved)
    {
        quint32 ErrorCode = EventHandler::CrisisEventHandler::Instance().findErrorCode(EventCode,Scenario);
        quint64 EventCodeScenario = EventCode;
        EventCodeScenario <<= 32;
        EventCodeScenario |=Scenario;

        if(ErrorCode)
        {
            Global::EventObject::Instance().RaiseEvent(ErrorCode,
                                                       Global::tTranslatableStringList(),
                                                       isResolved,EventKey,
                                                       Global::NOT_APPLICABLE,
                                                       Global::tTranslatableStringList(),
                                                       EventCodeScenario);
        }
    }

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
    quint32 findErrorCode(quint32 eventId,quint32 Scenario);

    QStringList& RaiseLog();


private:
    CrisisEventHandler();
    ~CrisisEventHandler();
    static CrisisEventHandler m_EventHandlerInstance; //!< Static instance
    Q_DISABLE_COPY(CrisisEventHandler ) //!< Disable Copy and assignment
    quint32 m_currentSysState;  //!< Keep current system state
    QMultiHash< quint32, QPair<QStringList,quint32> > m_EventStateErrorHash; //!< QHash<EventId, QPair<StateList, ErrorId>
    QStringList OneLogInfo;
}; // end class



#define LOG_HW_PAR() CrisisEventHandler::Instance().RaiseLog()

} // end namespace EventHandler

#endif // EVENTHANDLER_CRISISEVENTHANDLER_H
