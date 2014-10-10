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
#include <DataLogging/Include/DayEventEntry.h>


namespace EventHandler {

/****************************************************************************/
/**
 * \brief runtime information for event
 *
 */
/****************************************************************************/
struct EventRuntimeInfo_t
{
    quint32 EventKey;                               //!< event key
    quint32 EventID;                                //!< event ID
    quint32 Scenario;                               //!< scenario
    quint64 time;                                   //!< event happen time
    const XMLEvent* Event;                          //!< event info
    quint32 CurrentStep;                            //!< current step info
    bool ActionResult;                              //!< action result
    NetCommands::ClickedButton_t UserSelect;        //!< button selected by user
    Global::tTranslatableStringList EventStringParList;     //!< event parameters strings
    Global::tTranslatableStringList EventRDStringParList;   //!< event RD parameter strings
    bool AlarmActFlag;                                 //!< alarm action flag
};

/****************************************************************************/
/**
 * \brief This is a system level EventHandler.
 *
 * All system level events (e.g. NOT local component events) from all
 * ThreadControllers are sent to this (and only to this!) class. EventHandler
 * will forward events to DataLogger and, if needed, to ErrorHandler.
 */
/****************************************************************************/
class HimalayaEventHandlerThreadController : public EventHandlerThreadController
{ 
public:
    friend class TestHimalayaEventHandlerThreadController;
    /****************************************************************************/
    /*!
     *  \brief    Constructor for the HimalayaEventHandlerThreadController
     *
     *  \iparam ThreadID thread it
     *  \iparam RebootCount reboot count
     *  \iparam FileList  event config file list
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
    /****************************************************************************/
    /*!
     *  \brief  Log the events which are there in queue
     *
     *  \iparam EventInfo EventEntry event entry object
     */
    /****************************************************************************/
    void LogEntry(const EventRuntimeInfo_t& EventInfo);

    /****************************************************************************/
    /*!
     *  \brief  handle alarm action
     *  \iparam EventID:  event id
     *  \iparam Scenario: error scenario
     *  \iparam EventKey: event key
     *  \iparam Active:   alarm action
     */
    /****************************************************************************/
    void HandleAlarm(quint32 EventID, quint32 Scenario, const quint32 EventKey, const bool Active);

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
     *  \iparam    Active
     *  \iparam    ActionResult
     *  \iparam    EventStringParList
     *  \iparam    EventRDStringParList
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
     *  \iparam    EventStringList
     *  \iparam    EventStatus
     *  \iparam    EventKeyRef
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
     *  \brief  Function to send Action to component
     *
     *  \param    EventKey eventkey
     *  \param    EventId64 envent id
     *  \param    pEvent XMLEvent
     *  \param    active on/off
     */
    /****************************************************************************/
    void SendALMCommand(quint32 EventKey, const quint64 EventId64, const XMLEvent* pEvent, bool active);
    /****************************************************************************/
    /*!
     *  \brief    Function to reset remote & local alarm
     *  \param    EventKey eventkey
     *  \param    pEvent XMLEvent
     */
    /****************************************************************************/
    void ResetRmtLocAlarm(quint32 EventKey, const XMLEvent* pEvent);

    /****************************************************************************/
    /*!
     *  \brief  Function to send msg to gui
     *
     *  \param    pEvent XMLEvent
     *  \param    pStep EventStep
     */
    /****************************************************************************/
    void SendMSGCommand(quint32 EventKey, const XMLEvent* pEvent, const EventStep* pStep, bool EventStatus);
    /****************************************************************************/
    /*!
     *  \brief  Function to send msg to gui
     *
     *  \param    pEvent XMLEvent
     *  \param    pStep EventStep
     */
    /****************************************************************************/
    void SendDebugMSG(quint32 EventKey, const XMLEvent* pEvent, const EventStep* pStep);

private:
    DataLogging::DayEventEntry m_EventEntry;
    EventXMLInfo m_EventManager; //!< use to read event configure from xml
    QHash<quint32,EventRuntimeInfo_t> m_ActiveEvents;//!< Hash of event key and pending event
    bool m_GuiAvailable;                        //!< index Gui is available or not
    QList<quint32> m_PendingGuiEvent;           //!< store all eventkey that should be send to Gui after gui available
    QHash<Global::tRefType, quint32> m_EventKeyRefMap;//!< Hash of Event Key Reference Map
    bool m_TestMode; //!< flag for test mode
    QMap<quint32,Global::CommandShPtr_t> m_PendingActions;    //!< all pending action
};

} //end if namespace EventHandler
#endif // HIMALAYAEVENTHANDLERTHREADCONTROLLER_H
