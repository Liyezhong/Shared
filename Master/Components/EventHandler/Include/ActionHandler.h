
/****************************************************************************/
/*! \file ErrorHandler.h
 *
 *  \brief Base class of the Error Processing Module.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.11.2012
 *   $Author:  $ Aparna A L
 *
 *  \b Description:
 *
 *       This Module shall be used to process all Event Actions
 *
 *
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

#ifndef EVENTHANDLER_ACTIONHANDLER_H
#define EVENTHANDLER_ACTIONHANDLER_H

#include <Global/Include/Utils.h>

#include <DataLogging/Include/DayEventEntry.h>
#include <EventHandler/Include/ErrorHandler.h>
#include <EventHandler/Include/EventHandlerThreadController.h>
#include <NetCommands/Include/CmdSystemAction.h>

#include <QHash>
#include <QStringList>
#include <QFile>

#include <QThread>
#include <QDebug>
#include <QVector>

#include <DataLogging/Include/DayEventEntry.h>

namespace EventHandler
{
class EventHandlerThreadController;
}

namespace NetCommands
{
    class EventReportDataStruct;
}

namespace EventHandler {

class ActionHandler : public  EventHandler::ErrorHandler {
    Q_OBJECT
public:
    ActionHandler(EventHandlerThreadController *p_EventHandlerThreadController);
    ~ActionHandler();
private:

    EventHandlerThreadController * mpEventHandlerThreadController;

    bool mGuiAvailable;

//public slots:
//    void ReceiveEvent(const DataLogging::DayEventEntry &TheEvent, const quint64 EventId64);
//    void SetGuiAvailable(bool active);

};
}// end namespace EventHandler

#endif // ACTIONHANDLER_H
