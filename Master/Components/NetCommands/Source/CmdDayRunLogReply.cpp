/****************************************************************************/
/*! \file CmdDayRunLogReply.cpp
 *
 *  \brief CmdDayRunLogReply command implementation.
 *
 *  \b Description:
 *          Replies the number of day run log files to GUI
 *
 *   $Version: $ 0.1
 *   $Date:    $ 06.11.2012
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

#include <NetCommands/Include/CmdDayRunLogReply.h>

namespace NetCommands {

QString CmdDayRunLogReply::NAME = "NetCommands::CmdDayRunLogReply";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   FileList    List file names.
 */
/****************************************************************************/
CmdDayRunLogReply::CmdDayRunLogReply(int Timeout, const QStringList &FileList) :
    Command(Timeout),
    m_FileNames(FileList)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdDayRunLogReply::CmdDayRunLogReply() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdDayRunLogReply::~CmdDayRunLogReply()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdDayRunLogReply::GetName() const
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
QStringList CmdDayRunLogReply::GetFileNames() const
{
    return m_FileNames;
}

} // end namespace NetCommands
