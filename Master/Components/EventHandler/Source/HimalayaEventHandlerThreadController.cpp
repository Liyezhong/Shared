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

namespace EventHandler {


HimalayaEventHandlerThreadController::HimalayaEventHandlerThreadController(quint32 ThreadID, quint32 RebootCount, QStringList FileList)
    :EventHandlerThreadController(ThreadID,RebootCount,QStringList())
    ,m_EventManager(FileList, Global::SystemPaths::Instance().GetSettingsPath() + QDir::separator() + "EventMap.xml")
    ,m_GuiAvailable(false)
{
    m_ActiveEvents.clear();
    m_PendingGuiEvent.clear();
}



HimalayaEventHandlerThreadController::~HimalayaEventHandlerThreadController()
{

}

void HimalayaEventHandlerThreadController::CreateAndInitializeObjects()
{
    // now register commands
    (void)m_EventManager.InitXMLInfo();
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

void HimalayaEventHandlerThreadController::ProcessEvent(const quint32 EventKey, const quint64 EventIDScenario,
                                                        const bool Active, const bool ActionResult,
                                                        const Global::tTranslatableStringList &EventStringParList,
                                                        const Global::tTranslatableStringList &EventRDStringParList)
{
    if(Active){
        quint32 EventID = EventIDScenario >> 32;
        quint32 Scenario = EventIDScenario & 0x00000000ffffffff;
        if (EventID == EVENT_GUI_AVAILABLE) {
                SetGuiAvailable(true);
                return;
        }

        const XMLEvent* pEvent = NULL; // current Event
        const EventStep* pCurrentStep = NULL; // current step
        const EventStep* pNextStep = NULL; // Next step
        QString NextStepID = "";
        if(! m_ActiveEvents.contains(EventKey)){ // first coming
            pEvent = m_EventManager.GetEvent(QString("%1").arg(EventID),QString("%1").arg(Scenario));
            if(pEvent){
                pNextStep = pEvent->GetStep(QString("%1").arg(0));
                if(pEvent && pNextStep){ // insert to active event list
                    EventRuntimeInfo_t EventInfo;
                    EventInfo.EventKey = EventKey;
                    EventInfo.EventID = EventID;
                    EventInfo.Scenario = Scenario;
                    EventInfo.CurrentStep = 0;
                    EventInfo.Event = pEvent;
                    EventInfo.EventStringParList = EventStringParList;
                    EventInfo.EventRDStringParList = EventRDStringParList;
                    m_ActiveEvents.insert(EventKey,EventInfo);
                }
            }
        }
        else{ //move to next step
            pEvent = m_ActiveEvents[EventKey].Event;
            pCurrentStep = pEvent->GetStep(QString("%1").arg(m_ActiveEvents[EventKey].CurrentStep));
            if(pEvent && pCurrentStep){ //caculate the next step
                if(pCurrentStep->GetType().compare("ACT") == 0){ //action step
                    if(ActionResult){
                        NextStepID = pCurrentStep->GetNextStepOnSuccess();
                    }
                    else{
                        NextStepID = pCurrentStep->GetNextStepOnFail();
                    }
                    if(!NextStepID.isEmpty()){
                        pNextStep = pEvent->GetStep(NextStepID);
                        bool ok;
                        m_ActiveEvents[EventKey].CurrentStep = NextStepID.toUInt(&ok); //move to next step
                    }
                }
                else{ // this case should be processed in onAcknowledge()
                }
            }
        }

        if(pEvent && pNextStep){ // do next step
            if(pNextStep->GetType().compare("ACT") == 0){ // send cmd to scheduler
                SendACTCommand(EventKey, pEvent, pNextStep);
            }
            else{ //send cmd to GUI
                SendMSGCommand(EventKey, pEvent, pNextStep);
            }
        }

    }
    else{  //incative event: remove from m_ActiveEvents

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
        const XMLEvent* pEvent = NULL; // current Event
        const EventStep* pCurrentStep = NULL; // current step
        const EventStep* pNextStep = NULL; // Next step
        QString NextStepID = "";
        pEvent = m_ActiveEvents[EventKey].Event;
        pCurrentStep = pEvent->GetStep(QString("%1").arg(m_ActiveEvents[EventKey].CurrentStep));
        if(pEvent && pCurrentStep){ //caculate the next step
            if(pCurrentStep->GetType().compare("MSG") == 0){ //msg step
                NetCommands::ClickedButton_t clicked = ack.GetButtonClicked();
                switch(clicked){
                case NetCommands::OK_BUTTON:
                    NextStepID = pCurrentStep->GetNextStepOnTimeOut();
                default: //time out
                    NextStepID = pCurrentStep->GetNextStepOnTimeOut();
                }
                if(!NextStepID.isEmpty()){
                    pNextStep = pEvent->GetStep(NextStepID);
                    bool ok;
                    m_ActiveEvents[EventKey].CurrentStep = NextStepID.toUInt(&ok); //move to next step
                }
            }
            else{ // this case should be processed in processEvent()
            }
        }
        if(pEvent && pNextStep){ // do next step
            if(pNextStep->GetType().compare("ACT") == 0){ // send cmd to scheduler
                SendACTCommand(EventKey, pEvent, pNextStep);
            }
            else{ //send cmd to GUI
                SendMSGCommand(EventKey, pEvent, pNextStep);
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
    bool ok;
    p_CmdSystemAction = new NetCommands::CmdSystemAction();
    p_CmdSystemAction->SetEventKey(EventKey);
    p_CmdSystemAction->SetEventID(pEvent->GetErrorId().toUInt(&ok));
    p_CmdSystemAction->SetSource(Global::EVENTSOURCE_SCHEDULER);
    p_CmdSystemAction->SetActionString(pStep->GetAction());

    Global::tRefType NewRef = GetNewCommandRef();
    SendCommand(NewRef, Global::CommandShPtr_t(p_CmdSystemAction));
    DEBUGWHEREAMI;
}

void HimalayaEventHandlerThreadController::SendMSGCommand(quint32 EventKey, const XMLEvent* pEvent, const EventStep* pStep)
{
    if (pStep && pEvent && (pStep->GetType().compare("MSG") == 0))
    {
        NetCommands::EventReportDataStruct EventReportData;
//        EventReportData.EventStatus = TheEvent.IsEventActive(); //False means event not active, True if event active.
        EventReportData.EventType = Global::EVTTYPE_ERROR;
        EventReportData.ID = EventKey;
        EventReportData.EventKey = EventKey;

//        EventReportData.MsgString = Global::UITranslator::TranslatorInstance().Translate(Global::TranslatableString(TheEvent.GetEventId(), TheEvent.GetString()),
//                                                                                            UseAltEventString, true); //"Event String translated to the set langauge";
        EventReportData.MsgString = pStep->GetStringID();
//        EventReportData.Time = TheEvent.GetTimeStamp().toString();   // Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
        EventReportData.Timeout = pStep->GetTimeOut();
        EventReportData.BtnEnableConditions = pStep->GetButtonEnableConditon();
        EventReportData.BtnType = Global::OK;
        EventReportData.StatusBarIcon = pStep->GetStatusBar().compare("YES");   //true if GUI must set status bar icon.

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
}//end of namespace EventHandler
