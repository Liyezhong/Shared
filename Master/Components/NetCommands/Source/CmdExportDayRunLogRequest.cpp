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
 * \iparam   Timeout     Timeout for command.
 * \iparam   Path        Path of the file
 */
/****************************************************************************/
CmdExportDayRunLogRequest::CmdExportDayRunLogRequest(int Timeout, QString Path) :
    Command(Timeout), m_FolderPath(Path)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdExportDayRunLogRequest::CmdExportDayRunLogRequest() : Command(0)
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

/****************************************************************************/
/*!
 *  \brief   Get folder path
 *
 *  \return  folder path as string
 */
/****************************************************************************/
QString CmdExportDayRunLogRequest::GetFolderPath() const
{
    return m_FolderPath;
}

} // end namespace NetCommands
