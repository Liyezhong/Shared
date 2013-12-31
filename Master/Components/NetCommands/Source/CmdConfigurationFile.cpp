/****************************************************************************/
/*! \file CmdConfigurationFile.cpp
 *
 *  \brief CmdConfigurationFile command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 24.02.2012
 *   $Author:  $ M.Scherer
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

#include <NetCommands/Include/CmdConfigurationFile.h>
#include <QBuffer>
#include <QFile>

namespace NetCommands {

QString CmdConfigurationFile::NAME = "NetCommands::CmdConfigurationFile";

/****************************************************************************/
/*!
 *  \brief  Constructor to send commands
 *
 *  \iparam  Timeout     Timeout for command.
 *  \iparam  FileType    Configuration file type.
 *  \iparam  FileContent Configuration file content.
 */
/****************************************************************************/
CmdConfigurationFile::CmdConfigurationFile(int Timeout, const FileType_t FileType, const QDataStream &FileContent) :
    Command(Timeout),
    m_FileType(FileType),
    m_FileContent(static_cast<QBuffer *>(FileContent.device())->data())
{
}

/****************************************************************************/
/*!
 *  \brief  Constructor to receive commands
 *
 */
/****************************************************************************/
CmdConfigurationFile::CmdConfigurationFile() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdConfigurationFile::~CmdConfigurationFile()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdConfigurationFile::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns the configuration file type
 *
 *  \return  Configuration file type
 */
/****************************************************************************/
FileType_t CmdConfigurationFile::GetFileType() const
{
    return m_FileType;
}

/****************************************************************************/
/*!
 *  \brief   This function returns the configuration file content
 *
 *  \return  Configuration file content
 */
/****************************************************************************/
QByteArray const& CmdConfigurationFile::GetFileContent() const
{
    return m_FileContent;
}

} // end namespace NetCommands
