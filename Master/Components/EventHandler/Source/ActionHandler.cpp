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

}



