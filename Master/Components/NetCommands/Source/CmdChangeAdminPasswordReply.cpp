/****************************************************************************/
/*! \file NetCommands/Source/CmdChangeAdminPasswordReply.cpp
 *
 *  \brief CmdChangeAdminPasswordReply command implementation.
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

#include <NetCommands/Include/CmdChangeAdminPasswordReply.h>

namespace NetCommands {

QString CmdChangeAdminPasswordReply::NAME = "NetCommands::CmdChangeAdminPasswordReply";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout     Timeout for command.
 * \iparam   CmdType     Type of the command
 */
/****************************************************************************/
CmdChangeAdminPasswordReply::CmdChangeAdminPasswordReply(int Timeout, const QString &CmdType) :
    Command(Timeout),
    m_CmdType(CmdType)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdChangeAdminPasswordReply::CmdChangeAdminPasswordReply() :
    Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdChangeAdminPasswordReply::~CmdChangeAdminPasswordReply()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdChangeAdminPasswordReply::GetName() const
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
QString CmdChangeAdminPasswordReply::GetCommandType() const
{
    return m_CmdType;
}

} // end namespace NetCommands
