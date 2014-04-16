/****************************************************************************/
/*! \file CmdEventStrings.cpp
 *
 *  \brief CmdEventStrings command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 10.08.2012
 *   $Author:  $ N.Kamath
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

#include <NetCommands/Include/CmdEventStrings.h>

namespace NetCommands {

QString CmdEventStrings::NAME = "NetCommands::CmdEventStrings";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \iparam   Timeout                  Timeout for command.
 * \iparam   EventStringsDataStream   Datastream of Event Strings Data.
 */
/****************************************************************************/
CmdEventStrings::CmdEventStrings(int Timeout, const QDataStream &EventStringsDataStream) :
    Command(Timeout),
    m_EventStringsByteArray(EventStringsDataStream.device()->readAll())
{
}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdEventStrings::CmdEventStrings() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdEventStrings::~CmdEventStrings()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdEventStrings::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses
