/****************************************************************************/
/*! \file CmdExportDayRunLogReply.cpp
 *
 *  \brief CmdExportDayRunLogReply command implementation.
 *
 *  \b Description:
 *          Replies the number of day run log files to Export
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

#include <NetCommands/Include/CmdExportDayRunLogReply.h>

namespace NetCommands {

QString CmdExportDayRunLogReply::NAME = "NetCommands::CmdExportDayRunLogReply";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout     Timeout for command.
 * \iparam   FileList    List file names.
 */
/****************************************************************************/
CmdExportDayRunLogReply::CmdExportDayRunLogReply(int Timeout, const QStringList &FileList) :
    Command(Timeout),
    m_FileNames(FileList)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdExportDayRunLogReply::CmdExportDayRunLogReply() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdExportDayRunLogReply::~CmdExportDayRunLogReply()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdExportDayRunLogReply::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns daily run log file names
 *
 *  \return  file list
 */
/****************************************************************************/
QStringList CmdExportDayRunLogReply::GetFileNames() const
{
    return m_FileNames;
}

} // end namespace NetCommands

