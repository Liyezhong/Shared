/****************************************************************************/
/*! \file ErrorHandler.cpp
 *
 *  \brief Base class of the Error Processing Module.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Y.Novak
 *
 *  \b Description:
 *
 *       This Module shall be used to process all events of type
 *         - Fatal Error
 *         - Temporary Error
 *         - Warning
 *
 *       This Module keeps track of all occurring error conditions (e.g.
 *       error occurred or error was fixed) and triggers appropriate actions.
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

#include <EventHandler/Include/ErrorHandler.h>

namespace EventHandler {


/****************************************************************************/
/*!
 *  \brief    Constructor for the ErrorHandler
 *
 *
 *  \param    pParent = pointer to the parent
 *  \param    LoggingSource = logging source to use for events/actions
 *
 *  \return
 *
 ****************************************************************************/
ErrorHandler::ErrorHandler()
{
}

/****************************************************************************/
/*!
 *  \brief    Destructor for the ErrorHandler
 *
 *
 *  \return
 *
 ****************************************************************************/
ErrorHandler::~ErrorHandler()
{
}


} // end namespace EventHandler
