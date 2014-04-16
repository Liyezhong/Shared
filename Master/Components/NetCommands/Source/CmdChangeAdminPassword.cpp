/****************************************************************************/
/*! \file NetCommands/Source/CmdChangeAdminPassword.cpp
 *
 *  \brief CmdChangeAdminPassword command implementation.
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

#include <NetCommands/Include/CmdChangeAdminPassword.h>

namespace NetCommands {

QString CmdChangeAdminPassword::NAME = "NetCommands::CmdChangeAdminPassword";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout     Timeout for command.
 * \iparam   CmdType     Type of the command
 * \iparam   Password    Password of the admin
 */
/****************************************************************************/
CmdChangeAdminPassword::CmdChangeAdminPassword(int Timeout, const QString &CmdType, const QString &Password) :
    Command(Timeout),
    m_CmdType(CmdType),
    m_Password(Password)
{    
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdChangeAdminPassword::CmdChangeAdminPassword() :
    Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdChangeAdminPassword::~CmdChangeAdminPassword()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdChangeAdminPassword::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns type of command for the admin user,
 *           whether old password is entered or new password is entered
 *
 *  \return  type of the command
 */
/****************************************************************************/
QString CmdChangeAdminPassword::GetCommandType() const
{
    return m_CmdType;
}

/****************************************************************************/
/*!
 *  \brief   This function returns password for the admin user
 *
 *  \return  password
 */
/****************************************************************************/
QString CmdChangeAdminPassword::GetPassword() const
{
    return m_Password;
}

} // end namespace NetCommands
