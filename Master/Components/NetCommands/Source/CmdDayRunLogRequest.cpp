/****************************************************************************/
/*! \file CmdDayRunLogRequest.cpp
 *
 *  \brief CmdDayRunLogRequest command implementation.
 *
 *  \b Description:
 *          Requests the number daily run log file names
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.03.2010, 06.11.2012
 *   $Author:  $ Y.Novak, Raju
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

#include <NetCommands/Include/CmdDayRunLogRequest.h>

namespace NetCommands {

QString CmdDayRunLogRequest::NAME = "NetCommands::CmdDayRunLogRequest";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \iparam   UserLevel       User level
 *  \iparam   Timeout         Timeout of the command
 *
 */
/****************************************************************************/
CmdDayRunLogRequest::CmdDayRunLogRequest(int Timeout, int UserLevel) :
    Command(Timeout), m_UserLevel(UserLevel)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdDayRunLogRequest::CmdDayRunLogRequest() : Command(0), m_UserLevel(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdDayRunLogRequest::~CmdDayRunLogRequest()
{
}

/****************************************************************************/
/*!
 *  \brief   Get the user level
 *
 *  \return  user level
 */
/****************************************************************************/
int CmdDayRunLogRequest::GetUserLevel() const
{
    return m_UserLevel;
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdDayRunLogRequest::GetName() const
{
    return NAME;
}

} // end namespace NetCommands
