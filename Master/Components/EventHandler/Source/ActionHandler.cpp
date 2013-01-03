#include <EventHandler/Include/ActionHandler.h>
#include <NetCommands/Include/CmdEventReport.h>
#include <EventHandler/Include/StateHandler.h>


namespace EventHandler {

ActionHandler::ActionHandler(EventHandler::EventHandlerThreadController *p_EventHandlerThreadController)
    : mGuiAvailable(false)
{
    mpEventHandlerThreadController = p_EventHandlerThreadController;
    setParent(mpEventHandlerThreadController);
}

ActionHandler::~ActionHandler() {
}

void ActionHandler::ReceiveEvent(const DataLogging::DayEventEntry &TheEvent, const quint32 EventKey)
{
    if ((TheEvent.GetGUIMessageBoxOptions() != Global::NO_BUTTON) || (TheEvent.GetStatusIcon()))
    {
        NetCommands::EventReportDataStruct EventReportData;
        EventReportData.EventStatus = TheEvent.IsEventActive(); //False means event not active, True if event active.
        EventReportData.EventType = TheEvent.GetEventType();  // Global::EVTTYPE_ERROR;
        quint64 reportId = TheEvent.GetEventCode();
        reportId = reportId << 32;
        EventReportData.ID = reportId | EventKey;
        EventReportData.EventKey = EventKey;
        EventReportData.MsgString = Global::EventTranslator::TranslatorInstance().Translate(Global::TranslatableString(TheEvent.GetEventCode(), TheEvent.GetString())); //"Event String translated to the set langauge";
        EventReportData.Time = TheEvent.GetTimeStamp().toString();   // Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
        EventReportData.BtnType = TheEvent.GetButtonType();
        EventReportData.StatusBarIcon = TheEvent.GetStatusIcon();   //true if GUI must set status bar icon.

        if (mGuiAvailable)
        {
            Global::tRefType Ref = mpEventHandlerThreadController->GetNewCommandRef();
            mpEventHandlerThreadController->SetEventKeyRefMap(Ref,EventKey);
            mpEventHandlerThreadController->SendCommand(Ref, Global::CommandShPtr_t(new NetCommands::CmdEventReport(Global::Command::MAXTIMEOUT, EventReportData)));
        }
        else    // GUI is not around yet => remember data and send later
        {
            mPendingGuiEventList.push_back(EventReportData);
        }
    }

    if ((TheEvent.GetActionType()== Global::ACNTYPE_BUSY) || (TheEvent.GetActionType()== Global::ACNTYPE_IDLE))
    {
        if(TheEvent.GetString().count() > 0) { // e.g. Rack inserted
            EventHandler::StateHandler::Instance().setActivityUpdate(true, (TheEvent.GetString()).at(0).GetString().toInt());
        }
    }

    if (TheEvent.GetActionType() == Global::ACNTYPE_STOP)
    {
        EventHandler::StateHandler::Instance().setAvailability(TheEvent.IsEventActive(), TheEvent.GetEventCode());
    }
}

void ActionHandler::SetGuiAvailable(bool active)
{
    qDebug() << "ActionHandler::SetGuiAvailable" << active;
    mGuiAvailable = active;

    if (active)
    {
        foreach(NetCommands::EventReportDataStruct EventReportData, mPendingGuiEventList)
        {
            Global::tRefType Ref = mpEventHandlerThreadController->GetNewCommandRef();
            mpEventHandlerThreadController->SetEventKeyRefMap(Ref,EventReportData.EventKey);
            mpEventHandlerThreadController->SendCommand(Ref, Global::CommandShPtr_t(new NetCommands::CmdEventReport(Global::Command::MAXTIMEOUT, EventReportData)));
        }
        mPendingGuiEventList.clear();
    }
}


}


