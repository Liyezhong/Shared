/****************************************************************************/
/*! \file NetCommands/Source/CmdRCNotifyShutdown.cpp
 *
 *  \brief CmdRCNotifyShutdown command implementation.
 *
 *   $Version: $ 1.0
 *   $Date:    $ 2014-03-13
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

#include <NetCommands/Include/CmdRCNotifyShutdown.h>

namespace NetCommands {

QString CmdRCNotifyShutdown::NAME = "NetCommands::CmdRCNotifyShutdown";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \iparam   Timeout     Timeout for command.
 */
/****************************************************************************/
CmdRCNotifyShutdown::CmdRCNotifyShutdown(const int& Timeout) :
    Command(Timeout)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdRCNotifyShutdown::CmdRCNotifyShutdown() :
    Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRCNotifyShutdown::~CmdRCNotifyShutdown()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRCNotifyShutdown::GetName() const
{
    return NAME;
}
} // end namespace NetCommands
