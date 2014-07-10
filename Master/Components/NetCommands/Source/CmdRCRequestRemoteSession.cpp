/****************************************************************************/
/*! \file NetCommands/Source/CmdRCRequestRemoteSession.cpp
 *
 *  \brief CmdRCRequestRemoteSession command implementation.
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

#include <NetCommands/Include/CmdRCRequestRemoteSession.h>

namespace RemoteCare {

QString CmdRCRequestRemoteSession::NAME = "RemoteCare::CmdRCRequestRemoteSession";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \iparam   Timeout     Timeout for command.
 *  \iparam   RequestType  Request type
 */
/****************************************************************************/
CmdRCRequestRemoteSession::CmdRCRequestRemoteSession(const int& Timeout, const QString& RequestType) :
    Command(Timeout),
    m_RequestType(RequestType)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdRCRequestRemoteSession::CmdRCRequestRemoteSession() :
    Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRCRequestRemoteSession::~CmdRCRequestRemoteSession()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRCRequestRemoteSession::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   Get the request type
 *
 *  \return  session request type
 */
/****************************************************************************/
QString CmdRCRequestRemoteSession::GetRequestType() const
{
    return m_RequestType;
}
} // end namespace NetCommands
