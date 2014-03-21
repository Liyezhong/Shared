/****************************************************************************/
/*! \file CmdExportDayRunLogRequest.cpp
 *
 *  \brief CmdExportDayRunLogRequest command implementation.
 *
 *  \b Description:
 *          Requests the number daily run log file names
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.12.2012
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

#include <NetCommands/Include/CmdExportDayRunLogRequest.h>

namespace NetCommands {

QString CmdExportDayRunLogRequest::NAME = "NetCommands::CmdExportDayRunLogRequest";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   Request     request for the files.
 */
/****************************************************************************/
CmdExportDayRunLogRequest::CmdExportDayRunLogRequest(int Timeout) :
    Command(Timeout)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdExportDayRunLogRequest::CmdExportDayRunLogRequest(Global::GuiUserLevel CurrentUserRole) :
    Command(0),
    m_CurrentUserRole(CurrentUserRole)

{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdExportDayRunLogRequest::~CmdExportDayRunLogRequest()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdExportDayRunLogRequest::GetName() const
{
    return NAME;
}

Global::GuiUserLevel CmdExportDayRunLogRequest::GetCurrenUserRole() const
{
    return m_CurrentUserRole;
}

} // end namespace NetCommands
