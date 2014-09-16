/****************************************************************************/
/*! \file Source/HimalayaEventHandlerThreadController.cpp
 *
 *  \brief Frontend for Event processing and logging.
 *
 *         All events shall be sent to this Module. It will filter an
 *         incoming event according to its type and forward the event
 *         to Error Handler / Data Logger.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 06.01.2014
 *   $Author:  $ L. Yang
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


#include <NetCommands/Include/CmdEventReport.h>
#include <NetCommands/Include/CmdSystemAction.h>
#include <EventHandler/Include/HimalayaEventHandlerThreadController.h>
#include <EventHandler/Include/EventHandlerEventCodes.h>
#include <QFile>
#include <stdio.h>

namespace EventHandler {

HimalayaEventHandlerThreadController::HimalayaEventHandlerThreadController(quint32 ThreadID, quint32 RebootCount, QStringList FileList)
    :EventHandlerThreadController(ThreadID,RebootCount,QStringList())
    ,m_EventManager(FileList, Global::SystemPaths::Instance().GetSettingsPath() + QDir::separator() + "EventScenarioErrorMap.xml")
    ,m_GuiAvailable(false),m_TestMode(false)
{
    m_ActiveEvents.clear();
    m_PendingGuiEvent.clear();
    m_PendingActions.clear();
    m_TestMode = QFile::exists("TEST_ISSAC");
}



HimalayaEventHandlerThreadController::~HimalayaEventHandlerThreadController()
{

}

void HimalayaEventHandlerThreadController::CreateAndInitializeObjects()
{
    // now register commands
    bool ret = m_EventManager.InitXMLInfo();
    Q_UNUSED(ret);
    RegisterCommands();
}

void HimalayaEventHandlerThreadController::CleanupAndDestroyObjects()
{
}


void HimalayaEventHandlerThreadController::ConnectToEventObject()
{
       Global::EventObject *p_EventObject  = &Global::EventObject::Instance();
       CONNECTSIGNALSLOT(p_EventObject, ForwardEvent(const quint32, const quint64,
                                                     const bool, const bool,
                                                     const Global::tTranslatableStringList &,
                                                     const Global::tTranslatableStringList &),
                         this, ProcessEvent(const quint32, const quint64,
                                         const bool, const bool,
                                         const Global::tTranslatableStringList &,
                                         const Global::tTranslatableStringList &));

       CONNECTSIGNALSLOT(p_EventObject, ForwardEvent(const quint32, const Global::tTranslatableStringList &, const bool, const quint32, const Global::AlternateEventStringUsage),
                         this, ProcessEvent(const quint32, const Global::tTranslatableStringList &, const bool, const quint32, const Global::AlternateEventStringUsage));

}

void HimalayaEventHandlerThreadController::HandleAlarm(quint32 EventID, quint32 Scenario, const quint32 EventKey, const bool Active)
{
    const XMLEvent* pEvent = NULL;
    if(!m_ActiveEvents.contains(EventKey))
        pEvent = m_EventManager.GetEvent(EventID, Scenario);
    else
        pEvent = m_ActiveEvents[EventKey].Event;

    if (pEvent == NULL)
        return ;

    if (pEvent->GetErrorType() == Global::EVTTYPE_UNDEFINED || pEvent->GetErrorType() == Global::EVTTYPE_DEBUG) 
        return ;

    if (pEvent->GetAlarmType() == Global::ALARMPOS_NONE) 
        return ;

    SendALMCommand(EventKey, EventID, pEvent, Active);
}

void HimalayaEventHandlerThreadController::ProcessEvent(const quint32 EventKey, const quint64 EventIDScenario,
                                                        const bool Active, const bool ActionResult,
                                                        const Global::tTranslatableStringList &EventStringParList,
                                                        const Global::tTranslatableStringList &EventRDStringParList)
{
    quint32 EventID = EventIDScenario >> 32;
    quint32 Scenario = EventIDScenario & 0xffffffff;

    if (!Active) {
        m_ActiveEvents.remove(EventKey);
        return ;
    }
    if (EventID == EVENT_GUI_AVAILABLE) {
        SetGuiAvailable(true);
        return;
    }

    const EventStep* pCurrentStep = NULL; // current step
    const EventStep* pNextStep = NULL; // Next step
    quint32 NextStepID = 0;

    const XMLEvent* pEvent = NULL;
    if(!m_ActiveEvents.contains(EventKey)) { // first coming
        pEvent = m_EventManager.GetEvent(EventID, Scenario);
        if(pEvent) {
            pNextStep = pEvent->GetStep(0);
            if(pNextStep) { // insert to active event list
                EventRuntimeInfo_t EventInfo;
                EventInfo.EventKey = EventKey;
                EventInfo.EventID = EventID;
                EventInfo.Scenario = Scenario;
                EventInfo.ActionResult = ActionResult;
                EventInfo.CurrentStep = 0;
                EventInfo.Event = pEvent;
                EventInfo.EventStringParList = EventStringParList;
                EventInfo.EventRDStringParList = EventRDStringParList;
                m_ActiveEvents.insert(EventKey,EventInfo);
                 EventInfo.AlarmActFlag = false;
            }
        }
#if 1
        else {  // only for debug
            NetCommands::EventReportDataStruct EventReportData;
            EventReportData.EventStatus = true;
            EventReportData.EventType = Global::EVTTYPE_DEBUG;
            EventReportData.ID = (((quint64)EventID) << 32) + EventKey;
            EventReportData.EventKey = EventKey;
            EventReportData.MsgString = QString("DBG: Unknow EventID and Scenario: %1, %2").arg(EventID).arg(Scenario);
            EventReportData.Time = Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
            EventReportData.BtnType = Global::OK;
            EventReportData.StatusBarIcon = false;
            if (m_GuiAvailable) {
                Global::tRefType Ref = GetNewCommandRef();
                SendCommand(Ref, Global::CommandShPtr_t(new NetCommands::CmdEventReport(Global::Command::MAXTIMEOUT, EventReportData)));
            }
        }
#endif
    }
    else { //move to next step
        pEvent = m_ActiveEvents[EventKey].Event;
        pCurrentStep = pEvent->GetStep(m_ActiveEvents[EventKey].CurrentStep);
        if(pEvent && pCurrentStep) { //caculate the next step
            m_ActiveEvents[EventKey].ActionResult = ActionResult;
            if(pCurrentStep->GetType().compare("ACT") == 0){ //action step
                LogEntry(m_ActiveEvents[EventKey]); //log the action result
                if(ActionResult){
                    m_ActiveEvents[EventKey].UserSelect = NetCommands::NOT_SPECIFIED;// index not from gui.
                    NextStepID = pCurrentStep->GetNextStepOnSuccess();
                }
                else {
                    NextStepID = pCurrentStep->GetNextStepOnFail();
                }
                if(NextStepID != 0){
                    pNextStep = pEvent->GetStep(NextStepID);
                    m_ActiveEvents[EventKey].CurrentStep = NextStepID; //move to next step
                }
                else { // event finished
                    m_ActiveEvents.remove(EventKey);
                }
            }
        }
    }

    if(pEvent && pNextStep) { // do next step
        if(pNextStep->GetType().compare("ACT") == 0) { // send cmd to scheduler
            SendACTCommand(EventKey, pEvent, pNextStep);
        }
        else { //send cmd to GUI
            m_ActiveEvents[EventKey].UserSelect = NetCommands::NOT_SPECIFIED;// index not from gui.
            LogEntry(m_ActiveEvents[EventKey]);
            if(pNextStep->GetButtonType() != Global::NOT_SPECIFIED){
                if (m_ActiveEvents[EventKey].AlarmActFlag == false) {
                    HandleAlarm(EventID, Scenario, EventKey, Active);
                    m_ActiveEvents[EventKey].AlarmActFlag = true;
                }
                SendMSGCommand(EventKey, pEvent, pNextStep,Active);
            }
            else { // event only for logging
                m_ActiveEvents.remove(EventKey);
            }
        }
    }
}

void HimalayaEventHandlerThreadController::ProcessEvent(const quint32 EventID,
                  const Global::tTranslatableStringList &EventStringList,
                  const bool EventStatus, const quint32 EventKeyRef, const Global::AlternateEventStringUsage AltStringUsuage)
{
    Q_UNUSED(AltStringUsuage)
    quint64 EventIDScenario = EventID;
    EventIDScenario = EventIDScenario << 32;
    ProcessEvent(EventKeyRef,EventIDScenario,EventStatus,true,EventStringList,Global::tTranslatableStringList());
}

void HimalayaEventHandlerThreadController::OnAcknowledge(Global::tRefType ref, const NetCommands::CmdAcknEventReport &ack)
{
    quint32 EventKey = m_EventKeyRefMap.value(ref);
    m_EventKeyRefMap.remove(ref);
    if(m_ActiveEvents.contains(EventKey)){
        if(m_TestMode && m_PendingActions.contains(EventKey))
        {
            Global::tRefType NewRef = GetNewCommandRef();
            SendCommand(NewRef,  m_PendingActions.value(EventKey));
            m_PendingActions.remove(EventKey);
            return;
        }
        const XMLEvent* pEvent = NULL; // current Event
        const EventStep* pCurrentStep = NULL; // current step
        const EventStep* pNextStep = NULL; // Next step
        quint32 NextStepID = 0;
        pEvent = m_ActiveEvents[EventKey].Event;
        pCurrentStep = pEvent->GetStep(m_ActiveEvents[EventKey].CurrentStep);
        if(pEvent && pCurrentStep) { //caculate the next step
            if(pCurrentStep->GetType().compare("MSG") == 0){ //msg step
                m_ActiveEvents[EventKey].UserSelect = ack.GetButtonClicked();
                LogEntry(m_ActiveEvents[EventKey]);
                NetCommands::ClickedButton_t clicked = ack.GetButtonClicked();
                if (NetCommands::TIMEOUT != clicked) {
                    ResetRmtLocAlarm(EventKey, pEvent);
                    ResetAlarm();
                }
                switch(clicked) {
                    case NetCommands::OK_BUTTON:
                        NextStepID = pCurrentStep->GetNextStepOnClickOK();
                        ResetRmtLocAlarm(EventKey, pEvent);
                        ResetAlarm();
                        break;

                    case NetCommands::TIMEOUT:
                    default: //time out
                        qDebug() << "I get the timeout message";
                        NextStepID = pCurrentStep->GetNextStepOnTimeOut();
                }
                if(NextStepID != 0) {
                    pNextStep = pEvent->GetStep(NextStepID);
                    m_ActiveEvents[EventKey].CurrentStep = NextStepID; //move to next step
                }
                else {
                    m_ActiveEvents.remove(EventKey); // remove
                }
            }
            else { // this case should be processed in processEvent()
            }
        }
        if (pEvent && pNextStep) { // do next step
            if(pNextStep->GetType().compare("ACT") == 0) { // send cmd to scheduler
                SendACTCommand(EventKey, pEvent, pNextStep);
            }
            else { //send cmd to GUI
                SendMSGCommand(EventKey, pEvent, pNextStep,true);
            }
        }

    }
}

void HimalayaEventHandlerThreadController::OnAckOKNOK(Global::tRefType Ref, const Global::AckOKNOK &Ack)
{
    Q_UNUSED(Ref);
    Q_UNUSED(Ack);
}

/****************************************************************************/
/*!
 *  \brief  Function to Register Commands
 *
 */
/****************************************************************************/
void HimalayaEventHandlerThreadController::RegisterCommands()
{

    qDebug() << "HimalayaEventHandlerThreadController::RegisterCommands";
    RegisterAcknowledgeForProcessing<
            NetCommands::CmdAcknEventReport, HimalayaEventHandlerThreadController
            >(&HimalayaEventHandlerThreadController::OnAcknowledge, this);

    RegisterAcknowledgeForProcessing<
            Global::AckOKNOK, HimalayaEventHandlerThreadController
            >(&HimalayaEventHandlerThreadController::OnAckOKNOK, this);

}

void HimalayaEventHandlerThreadController::SendACTCommand(quint32 EventKey, const XMLEvent* pEvent, const EventStep* pStep)
{
    NetCommands::CmdSystemAction *p_CmdSystemAction;
    p_CmdSystemAction = new NetCommands::CmdSystemAction();
    p_CmdSystemAction->SetEventKey(EventKey);
    p_CmdSystemAction->SetEventID(pEvent->GetErrorId());
    p_CmdSystemAction->SetSource(pEvent->GetEventSource());
    p_CmdSystemAction->SetActionString(pStep->GetAction());

    if(!m_TestMode)
    {
        Global::tRefType NewRef = GetNewCommandRef();
        SendCommand(NewRef, Global::CommandShPtr_t(p_CmdSystemAction));
        DEBUGWHEREAMI;
        LogEntry(m_ActiveEvents[EventKey]);
    }
    else
    {
        m_PendingActions.insert(EventKey, Global::CommandShPtr_t(p_CmdSystemAction));
        SendDebugMSG(EventKey,pEvent,pStep);
    }
}

void HimalayaEventHandlerThreadController::SendALMCommand(quint32 EventKey, const quint64 EventId64, const XMLEvent* pEvent, bool active)
{
    if (pEvent == NULL)
        return ;

    NetCommands::CmdSystemAction *p_CmdSystemAction;
    p_CmdSystemAction = new NetCommands::CmdSystemAction();
    p_CmdSystemAction->SetEventKey(EventKey);
    p_CmdSystemAction->SetEventID(pEvent->GetErrorId());
    p_CmdSystemAction->SetSource(pEvent->GetEventSource());

    // opcode:
    // bit2 bit1  bit0
    // 00 device  0: off, 1: on
    // 01 local
    // 10 remote

    // 0: device off
    // 1: device on
    // 2: local  off
    // 3: local  on
    // 4: remote off
    // 5: remote on

     // -1: alarm off

    int opcode = -1;
    QString ActionString = "ALARM_";
    switch (pEvent->GetAlarmType()) {
    case Global::ALARMPOS_REMOTE:
        opcode = active ? 5 : 4;
        break;
    case Global::ALARMPOS_LOCAL:
        opcode = active ? 3 : 2;
        break;
    case Global::ALARMPOS_DEVICE:
        opcode = active ? 1 : 0;
        break;
    }

    if (opcode == 1 || opcode == 3 || opcode == 5) {
        InformAlarmHandler(pEvent->GetErrorType(), EventId64, true);
    }
    else {
        InformAlarmHandler(pEvent->GetErrorType(), EventId64, false);
    }

    // Turn off remote & local alarm
    if (opcode == 1 || opcode == 0) {
        opcode = -1;
    }
    
    ActionString += QString("%1").arg(opcode);
    p_CmdSystemAction->SetActionString(ActionString);

    Global::tRefType NewRef = GetNewCommandRef();
    SendCommand(NewRef, Global::CommandShPtr_t(p_CmdSystemAction));
}

void HimalayaEventHandlerThreadController::ResetRmtLocAlarm(quint32 EventKey, const XMLEvent* pEvent)
{
    if (pEvent == NULL)
        return ;

    NetCommands::CmdSystemAction *p_CmdSystemAction;
    p_CmdSystemAction = new NetCommands::CmdSystemAction();
    p_CmdSystemAction->SetEventKey(EventKey);
    p_CmdSystemAction->SetEventID(pEvent->GetErrorId());
    p_CmdSystemAction->SetSource(pEvent->GetEventSource());

    int opcode = -1;
    QString ActionString = "ALARM_";
    ActionString += QString("%1").arg(opcode);
    p_CmdSystemAction->SetActionString(ActionString);

    Global::tRefType NewRef = GetNewCommandRef();
    SendCommand(NewRef, Global::CommandShPtr_t(p_CmdSystemAction));
}

void HimalayaEventHandlerThreadController::SendDebugMSG(quint32 EventKey, const XMLEvent* pEvent, const EventStep* pStep)
{
    if (pStep && pEvent)
    {
        NetCommands::EventReportDataStruct EventReportData;
        EventReportData.EventStatus = true; //False means event not active, True if event active.
        EventReportData.EventType =Global::EVTTYPE_DEBUG;
        EventReportData.ID = (((quint64)pEvent->GetErrorId()) << 32) | (quint64)EventKey;
        EventReportData.EventKey = EventKey;

        EventReportData.MsgString = QString("DBG  Event: %1  Scenario: %2  Next Action: %3").arg(m_ActiveEvents[EventKey].EventID)
                .arg(m_ActiveEvents[EventKey].Scenario).arg(pStep->GetAction());
        EventReportData.Time = Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
        EventReportData.Timeout = "";
        EventReportData.BtnEnableConditions = "";
        EventReportData.BtnType = Global::OK;
        EventReportData.StatusBarIcon = false;   //true if GUI must set status bar icon.
        if (m_GuiAvailable)
        {
            Global::tRefType Ref = GetNewCommandRef();
            m_EventKeyRefMap.insert(Ref,EventKey);
            SendCommand(Ref, Global::CommandShPtr_t(new NetCommands::CmdEventReport(Global::Command::MAXTIMEOUT, EventReportData)));
        }
    }
}

void HimalayaEventHandlerThreadController::SendMSGCommand(quint32 EventKey, const XMLEvent* pEvent, const EventStep* pStep, bool EventStatus)
{
    if (pStep && pEvent && (pStep->GetType().compare("MSG") == 0))
    {
        NetCommands::EventReportDataStruct EventReportData;
        EventReportData.EventStatus = EventStatus; //False means event not active, True if event active.
        EventReportData.EventType = pEvent->GetErrorType();
        EventReportData.ID = (((quint64)pEvent->GetErrorId()) << 32) | (quint64)EventKey;
        EventReportData.EventKey = EventKey;

        EventReportData.MsgString = Global::UITranslator::TranslatorInstance().Translate(Global::TranslatableString(pStep->GetStringID(), m_ActiveEvents[EventKey].EventStringParList)); //"Event String translated to the set langauge";
//        EventReportData.MsgString = pStep->GetStringID();
        EventReportData.Time = Global::AdjustedTime::Instance().GetCurrentDateTime().toString();

        EventReportData.Timeout = pStep->GetTimeOut();
        EventReportData.BtnEnableConditions = pStep->GetButtonEnableConditon();
        EventReportData.BtnType = pStep->GetButtonType();
        EventReportData.StatusBarIcon = pStep->GetStatusBar();   //true if GUI must set status bar icon.

        if (m_GuiAvailable)
        {
            Global::tRefType Ref = GetNewCommandRef();
            m_EventKeyRefMap.insert(Ref,EventKey);
            SendCommand(Ref, Global::CommandShPtr_t(new NetCommands::CmdEventReport(Global::Command::MAXTIMEOUT, EventReportData)));
        }
        else
        {
                m_PendingGuiEvent.push_back(EventKey);
        }
    }
}

void HimalayaEventHandlerThreadController::SetGuiAvailable(const bool active)
{
    qDebug() << "HimalayaEventHandlerThreadController::SetGuiAvailable" << active;
    m_GuiAvailable = active;

    if (active)
    {
        foreach(quint32 EventKey, m_PendingGuiEvent)
        {
            Global::tRefType Ref = GetNewCommandRef();
            NetCommands::EventReportDataStruct EventReportData;
            EventRuntimeInfo_t Eventinfo = m_ActiveEvents.value(EventKey);
            //todo init EventReportData by Eventinfo

            SendCommand(Ref, Global::CommandShPtr_t(new NetCommands::CmdEventReport(Global::Command::MAXTIMEOUT, EventReportData)));
        }
        m_PendingGuiEvent.clear();
    }
}


void HimalayaEventHandlerThreadController::LogEntry(const EventRuntimeInfo_t& EventInfo)
{

    m_EventEntry.SetEventKey(EventInfo.EventKey);
    m_EventEntry.SetDateTime(Global::AdjustedTime::Instance().GetCurrentDateTime());
    m_EventEntry.SetEventStatus(EventInfo.ActionResult);


    //m_EventEntry.SetEventCode(EventInfo.EventID);
    m_EventEntry.SetEventCode(EventInfo.Event->GetErrorId());
    m_EventEntry.SetEventName(EventInfo.Event->GetEventName());
    m_EventEntry.SetEventType(EventInfo.Event->GetErrorType());
    m_EventEntry.SetEventSource(EventInfo.Event->GetEventSource());
    m_EventEntry.SetAlarmType(EventInfo.Event->GetAlarmType());
    m_EventEntry.SetLogLevel(EventInfo.Event->GetStep(EventInfo.CurrentStep)->GetLogLevel());
    m_EventEntry.SetShowInRunLogStatus(EventInfo.Event->GetStep(EventInfo.CurrentStep)->GetUserLog());
    m_EventEntry.SetStatusIcon(EventInfo.Event->GetStep(EventInfo.CurrentStep)->GetStatusBar());


    m_EventEntry.SetScenario(EventInfo.Scenario);

    if(EventInfo.Event->GetStep(EventInfo.CurrentStep)->GetType() == "MSG"){
        m_EventEntry.SetStringID(EventInfo.Event->GetStep(EventInfo.CurrentStep)->GetStringID());
        m_EventEntry.SetString(EventInfo.EventStringParList);
        m_EventEntry.SetButtonType(EventInfo.Event->GetStep(EventInfo.CurrentStep)->GetButtonType());
        m_EventEntry.SetAckValue(EventInfo.UserSelect);
    }
    else{
        quint32 Result =  EventInfo.ActionResult ? Global::EVENT_GLOBAL_STRING_ID_RESOLVED : Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_ERROR;
        /*lint -e539 */
        m_EventEntry.SetStringID(STR_SCHEDULER_EVENT_SCENARIO_ACTION_RESULT);
        m_EventEntry.SetString(Global::tTranslatableStringList() <<QString("%1").arg(EventInfo.EventID)
                                             << QString("%1").arg(EventInfo.Scenario)
                                             << EventInfo.Event->GetStep(EventInfo.CurrentStep)->GetAction()
                                             << Result);

        m_EventEntry.SetButtonType(Global::NOT_SPECIFIED);
    }

    emit LogEventEntry(m_EventEntry); //Log the event

    // check signal connection
    if (receivers(SIGNAL(SendEventToRemoteCare(const DataLogging::DayEventEntry&, const quint64))) == 0)
        return;

    if (EventInfo.Event->GetErrorType() == Global::EVTTYPE_DEBUG || EventInfo.Event->GetErrorType() == Global::EVTTYPE_UNDEFINED)
        return;
//    if (EventInfo.Event->GetAlarmType() != Global::ALARMPOS_REMOTE)
//        return;

    quint64 EventId64 = ((quint64)EventInfo.EventID << 32) | EventInfo.EventKey;

    /// \todo this is a test of Axeda Remote Care error reporting:
    emit SendEventToRemoteCare(m_EventEntry, EventId64);
}

}//end of namespace EventHandler
