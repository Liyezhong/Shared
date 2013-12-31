/****************************************************************************/
/*! \file RemoteCareHandler.cpp
 *
 *  \brief Implementation file for class RemoteCareHandler.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Y.Novak
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


#include <EventHandler/Include/RemoteCareHandler.h>

namespace EventHandler {

/****************************************************************************/
/*!
 *  \brief    Constructor for the RemoteCareHandler
 *
 *  \iparam   TheHeartBeatSource
 *
 *  \return
 *
 ****************************************************************************/
RemoteCareHandler::RemoteCareHandler(Global::gSourceType TheHeartBeatSource) :
    ThreadController(TheHeartBeatSource, "RemoteCare")
{
}

/****************************************************************************/
/*!
 *  \brief    Destructor for the RemoteCareHandler
 *
 *  \return
 *
 ****************************************************************************/
RemoteCareHandler::~RemoteCareHandler()
{
}


/****************************************************************************/
/**
 * \brief This function sends a System Event to the RemoteCare local Client.
 *
 *        It is required to send all system Errors to Leica Remote Care
 *        Server. This SLOT shall be connected to EventHandler's signal so
 *        the EventHandler can forward system errors to the local Axeda
 *        Client Controller.
 *
 * \iparam  &DayEventEntrydata = event (originally) from the system error handler
 */
/****************************************************************************/
void RemoteCareHandler::ForwardEventToRemoteCare(const DataLogging::DayEventEntry &DayEventEntrydata)
{
    ///< WARNING: Implement this SLOT in your derived class!!
    Q_UNUSED(DayEventEntrydata)
}

} // end namespace EventHandler
