/****************************************************************************/
/*! \file CmdDayRunLogRequest.cpp
 *
 *  \brief CmdDayRunLogRequest command implementation.
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
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   Request     request for the files.
 */
/****************************************************************************/
CmdDayRunLogRequest::CmdDayRunLogRequest(int Timeout, const bool &Request) :
    Command(Timeout),
    m_FileNameRequest(Request)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdDayRunLogRequest::CmdDayRunLogRequest() : Command(0)
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
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdDayRunLogRequest::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns file request flag
 *
 *  \return  On success (true) or not (false)
 */
/****************************************************************************/
bool CmdDayRunLogRequest::GetFileNameRequest() const
{
    return m_FileNameRequest;
}

} // end namespace NetCommands
