/****************************************************************************/
/*! \file CmdDayRunLogRequestFile.cpp
 *
 *  \brief CmdDayRunLogRequestFile command implementation.
 *
 *  \b Description:
 *          Requests the selected day run log file content from Main
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

#include <NetCommands/Include/CmdDayRunLogRequestFile.h>

namespace NetCommands {

QString CmdDayRunLogRequestFile::NAME = "NetCommands::CmdDayRunLogRequestFile";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout         Timeout for command.
 * \iparam   UserLevel       User level
 * \iparam   FileName        name of the file.
 */
/****************************************************************************/
CmdDayRunLogRequestFile::CmdDayRunLogRequestFile
(int Timeout, int UserLevel, const QString &FileName) :
    Command(Timeout),
    m_UserLevel(UserLevel),
    m_FileName(FileName)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdDayRunLogRequestFile::CmdDayRunLogRequestFile() : Command(0),
    m_UserLevel(0),
    m_FileName("")
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdDayRunLogRequestFile::~CmdDayRunLogRequestFile()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdDayRunLogRequestFile::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns file name
 *
 *  \return  Name of the file
 */
/****************************************************************************/
QString CmdDayRunLogRequestFile::GetFileName() const
{
    return m_FileName;
}

/****************************************************************************/
/*!
 *  \brief   Get the user level
 *
 *  \return  user level
 */
/****************************************************************************/
int CmdDayRunLogRequestFile::GetUserLevel() const
{
    return m_UserLevel;
}

} // end namespace NetCommands
