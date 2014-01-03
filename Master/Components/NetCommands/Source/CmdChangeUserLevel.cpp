/****************************************************************************/
/*! \file NetCommands/Source/CmdChangeUserLevel.cpp
 *
 *  \brief CmdChangeUserLevel command implementation.
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

#include <NetCommands/Include/CmdChangeUserLevel.h>

namespace NetCommands {

QString CmdChangeUserLevel::NAME = "NetCommands::CmdChangeUserLevel";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout     Timeout for command.
 * \iparam   UserLevel   Type of the user
 * \iparam   Password    Password of the admin
 */
/****************************************************************************/
CmdChangeUserLevel::CmdChangeUserLevel(int Timeout, const Global::GuiUserLevel &UserLevel, const QString &Password) :
    Command(Timeout),
    m_UserLevel(UserLevel),
    m_Password(Password)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdChangeUserLevel::CmdChangeUserLevel() :
    Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdChangeUserLevel::~CmdChangeUserLevel()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdChangeUserLevel::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns type of user,
 *           whether it is a Supervisior or service or User
 *
 *  \return  type of the user
 */
/****************************************************************************/
Global::GuiUserLevel CmdChangeUserLevel::GetUserLevel() const
{
    return m_UserLevel;
}

/****************************************************************************/
/*!
 *  \brief   This function returns password for the selected user
 *
 *  \return  password
 */
/****************************************************************************/
QString CmdChangeUserLevel::GetPassword() const
{
    return m_Password;
}

} // end namespace NetCommands
