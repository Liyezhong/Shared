/****************************************************************************/
/*! \file NetCommands/Source/CmdRCNotifyReconnection.cpp
 *
 *  \brief CmdRCNotifyReconnection command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16.07.2013
 *   $Author:  $ Ramya GJ
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

#include <NetCommands/Include/CmdRCNotifyReconnection.h>

namespace NetCommands {

QString CmdRCNotifyReconnection::NAME = "NetCommands::CmdRCNotifyReconnection";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \iparam   Timeout     Timeout for command.
 */
/****************************************************************************/
CmdRCNotifyReconnection::CmdRCNotifyReconnection(int Timeout) :
    Command(Timeout)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdRCNotifyReconnection::CmdRCNotifyReconnection() :
    Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRCNotifyReconnection::~CmdRCNotifyReconnection()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRCNotifyReconnection::GetName() const
{
    return NAME;
}
} // end namespace NetCommands
