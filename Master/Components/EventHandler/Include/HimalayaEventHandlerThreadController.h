/****************************************************************************/
/*! \file Include/EventHandlerThreadController.h
 *
 *  \brief Frontend for Event processing and logging.
 *
 *         All events shall be sent to this class. It will filter an
 *         incoming event according to its type and forward the event
 *         Error Handler / Data Logger.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 01.03.2014
 *   $Author:  $ Logan Yang
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

#ifndef HIMALAYAEVENTHANDLERTHREADCONTROLLER_H
#define HIMALAYAEVENTHANDLERTHREADCONTROLLER_H


#include <QHash>

#include <EventHandler/Include/EventHandlerThreadController.h>
#include <EventHandler/Include/EventXMLInfo.h>


namespace EventHandler {

struct EventRuntimeInfo_t
{
    quint32 EventKey;
    quint32 EventID;
    quint32 Scenario;
    const XMLEvent* Event;
    quint32 CurrentStep;
    Global::tTranslatableStringList EventStringParList;
    Global::tTranslatableStringList EventRDStringParList;
};

class HimalayaEventHandlerThreadController : public EventHandlerThreadController
{
public:
    /****************************************************************************/
    /*!
     *  \brief    Constructor for the HimalayaEventHandlerThreadController
     *
     *  \iparam ThreaID
     *  \iparam RebootCount
     *  \sparam FileList
     *
     ****************************************************************************/
    HimalayaEventHandlerThreadController(quint32 ThreadID, quint32 RebootCount, QStringList FileList);


    /****************************************************************************/
    /*!
     *  \brief    Destructor for the HimalayaEventHandlerThreadController
     *
     *  \return
     *
     ****************************************************************************/
     virtual ~HimalayaEventHandlerThreadController();


    /****************************************************************************/
    /*!
     *  \brief    API function to call before EventHandler can be used
     *
     *      This function does everything to initialize the EventHandler. It
     *      shall be called before EventHandler can be used.
     *
     ****************************************************************************/
     virtual void CreateAndInitializeObjects();
    /****************************************************************************/
    /*!
     *  \brief Destroy all used objects and prepare for shutdown.
     *
     *         See detailed description in the Base Class's header
     *
     *  \return
     */
    /****************************************************************************/
     virtual void CleanupAndDestroyObjects();

    /****************************************************************************/
    /**
     * \brief Function to connect to EventObject
     *
     */
    /****************************************************************************/
    virtual void ConnectToEventObject();

public slots:

    /****************************************************************************/
    /*!
     *  \brief    This SLOT handles all incoming events.
     *
     *      This SLOT shall be used to process all events and their current
     *      status.
     *
     *  \iparam    EventKey
     *  \iparam    EventIDScenario
     *  \bparam    Active
     *  \bparam    ActionResult
     *  \lparam    EventStringParList
     *  \lparam    EventRDStringParList
     *
     *
     ****************************************************************************/
    virtual void ProcessEvent(const quint32 EventKey, const quint64 EventIDScenario,
                              const bool Active, const bool ActionResult,
                              const Global::tTranslatableStringList &EventStringParList,
                              const Global::tTranslatableStringList &EventRDStringParList);
    /****************************************************************************/
    /*!
     *  \brief    This SLOT handles all incoming events.
     *
     *      This SLOT shall be used to report all events and their current
     *      status. The status might be of two Templates:
     *        - event active
     *        - event closed (e.g. condition which caused event is not
     *          present anymore)
     *
     *      This function forwards the event to ErrorHandler / DataLogger
     *      according to the event type.
     *
     *  \iparam    EventID
     *  \iparam    &EventStringList
     *  \iparam    EventStatus
     *  \iparam    EventKey
     *  \iparam    AltStringUsuage
     *
     *
     *
     ****************************************************************************/
    virtual void ProcessEvent(const quint32 EventID,
                      const Global::tTranslatableStringList &EventStringList,
                      const bool EventStatus, const quint32 EventKeyRef, const Global::AlternateEventStringUsage AltStringUsuage);
protected:

    /****************************************************************************/
    /**
     * Handler for acknowledged.
     *
     * \iparam   ref -- unique command reference.
     * \iparam   ack -- received acknowledge instance.
     */
    /****************************************************************************/
    virtual void OnAcknowledge(Global::tRefType ref, const NetCommands::CmdAcknEventReport &ack);

    /****************************************************************************/
    /**
     * Handler for ack and NACK.
     *
     * \iparam   Ref -- unique command reference.
     * \iparam   Ack -- received acknowledge/ NACK instance.
     */
    /****************************************************************************/
    void OnAckOKNOK(Global::tRefType Ref, const Global::AckOKNOK &Ack);

private:
    void RegisterCommands();

    /****************************************************************************/
    /*!
     *  \brief  Function to set Gui as available.
     *
     *  \iparam    active
     */
    /****************************************************************************/
    void SetGuiAvailable(const bool active);
    /****************************************************************************/
    /*!
     *  \brief  Function to send Action to component
     *
     *  \param    pEvent XMLEvent
     *  \param    pStep EventStep
     */
    /****************************************************************************/
    void SendACTCommand(quint32 EventKey, const XMLEvent* pEvent, const EventStep* pStep);
    /****************************************************************************/
    /*!
     *  \brief  Function to send msg to gui
     *
     *  \param    pEvent XMLEvent
     *  \param    pStep EventStep
     */
    /****************************************************************************/
    void SendMSGCommand(quint32 EventKey, const XMLEvent* pEvent, const EventStep* pStep);

private:
    EventXMLInfo m_EventManager; //!< use to read event configure from xml
    QHash<quint32,EventRuntimeInfo_t> m_ActiveEvents;//!< Hash of event key and pending event
    bool m_GuiAvailable;                        //!< index Gui is available or not
    QList<quint32> m_PendingGuiEvent;           //!< store all eventkey that should be send to Gui after gui available
    QHash<Global::tRefType, quint32> m_EventKeyRefMap;//!< Hash of Event Key Reference Map

};
} //end if namespace EventHandler
#endif // HIMALAYAEVENTHANDLERTHREADCONTROLLER_H
