/****************************************************************************/
/*! \file CmdScanStations.cpp
 *
 *  \brief CmdScanStations command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 30-09-2013
 *   $Author:  $ C.Adaragunchi
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

#include <NetCommands/Include/CmdScanStations.h>


namespace NetCommands {

QString CmdScanStations::NAME = "NetCommands::CmdScanStations";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \iparam   Timeout                 Timeout for command.
 */
/****************************************************************************/
CmdScanStations::CmdScanStations(int Timeout) :
    Command(Timeout)
{
    setErrorStateAllowed(false);
    setBusyStateAllowed(false);
    setSoftSwitchMonitorStateAllowed(false);
    setInitFailedStateAllowed(false);
}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdScanStations::CmdScanStations() : Command(0)
{
    setErrorStateAllowed(false);
    setBusyStateAllowed(false);
    setSoftSwitchMonitorStateAllowed(false);
    setInitFailedStateAllowed(false);
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdScanStations::~CmdScanStations()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdScanStations::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses


