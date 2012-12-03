/****************************************************************************/
/*! \file CmdUserAuthenticationReply.cpp
 *
 *  \brief CmdUserAuthenticationReply command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 30.10.2012
 *   $Author:  $ Raju
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

#include <NetCommands/Include/CmdUserAuthenticationReply.h>

namespace NetCommands {

QString CmdUserAuthenticationReply::NAME = "NetCommands::CmdUserAuthenticationReply";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   UserLevel   Type of the user
 */
/****************************************************************************/
CmdUserAuthenticationReply::CmdUserAuthenticationReply(int Timeout, const Global::GuiUserLevel &UserLevel) :
    Command(Timeout),
    m_UserLevel(UserLevel)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdUserAuthenticationReply::CmdUserAuthenticationReply() :
    Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdUserAuthenticationReply::~CmdUserAuthenticationReply()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdUserAuthenticationReply::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns whether user authenticated level
 *
 *  \return  Authenticated user level
 */
/****************************************************************************/
Global::GuiUserLevel CmdUserAuthenticationReply::GetUserAuthenticatedLevel() const
{
    return m_UserLevel;
}

} // end namespace NetCommands
