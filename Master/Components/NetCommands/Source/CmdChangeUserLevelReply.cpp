/****************************************************************************/
/*! \file CmdChangeUserLevelReply.cpp
 *
 *  \brief CmdChangeUserLevelReply command implementation.
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

#include <NetCommands/Include/CmdChangeUserLevelReply.h>

namespace NetCommands {

QString CmdChangeUserLevelReply::NAME = "NetCommands::CmdChangeUserLevelReply";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   UserLevel   Type of the user
 */
/****************************************************************************/
CmdChangeUserLevelReply::CmdChangeUserLevelReply(int Timeout, const Global::GuiUserLevel &UserLevel) :
    Command(Timeout),
    m_UserLevel(UserLevel)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdChangeUserLevelReply::CmdChangeUserLevelReply() :
    Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdChangeUserLevelReply::~CmdChangeUserLevelReply()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdChangeUserLevelReply::GetName() const
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
Global::GuiUserLevel CmdChangeUserLevelReply::GetUserAuthenticatedLevel() const
{
    return m_UserLevel;
}

} // end namespace NetCommands
