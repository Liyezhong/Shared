/****************************************************************************/
/*! \file RemoteCareHandler.h
 *
 *  \brief Definition file for class RemoteCareHandler.
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

#ifndef EVENTHANDLER_REMOTECAREHANDLER_H
#define EVENTHANDLER_REMOTECAREHANDLER_H

#include <DataLogging/Include/DayEventEntry.h>
#include <Threads/Include/ThreadController.h>
#include <Global/Include/GlobalDefines.h>

namespace EventHandler {

/****************************************************************************/
/**
 * \brief This is a thread controlling base class for the local
 *        Remote Care client.
 *
 * This class creates, initializes and controls all Remote Care relevant
 * objects. This class handles the system level communications:
 * event and action handling. It also has a direct connection with
 * EventHandler: events which need to be forwarded to Remote Care Server are
 * sent directly to its derived class.
 */
/****************************************************************************/
class RemoteCareHandler : public Threads::ThreadController
{
    Q_OBJECT

public:

    RemoteCareHandler(Global::gSourceType TheHeartBeatSource);
    virtual ~RemoteCareHandler();

protected:

private:

    RemoteCareHandler();
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(RemoteCareHandler)

public slots:
    virtual void ForwardEventToRemoteCare(const DataLogging::DayEventEntry &) = 0;
};

} // end namespace EventHandler

#endif // EVENTHANDLER_REMOTECAREHANDLER_H
