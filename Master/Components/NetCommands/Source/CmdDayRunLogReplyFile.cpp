/****************************************************************************/
/*! \file CmdDayRunLogReplyFile.cpp
 *
 *  \brief CmdDayRunLogReplyFile command implementation.
 *
 *  \b Description:
 *          Replies the data stream of the selected file content to GUI
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

#include <NetCommands/Include/CmdDayRunLogReplyFile.h>
#include <QBuffer>

namespace NetCommands {

QString CmdDayRunLogReplyFile::NAME = "NetCommands::CmdDayRunLogReplyFile";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout     Timeout for command.
 * \iparam   FileData    Information of the file.
 */
/****************************************************************************/
CmdDayRunLogReplyFile::CmdDayRunLogReplyFile(int Timeout, const QDataStream &FileData) :
    Command(Timeout),
    m_FileData(static_cast<QBuffer *>(FileData.device())->data())
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdDayRunLogReplyFile::CmdDayRunLogReplyFile() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdDayRunLogReplyFile::~CmdDayRunLogReplyFile()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdDayRunLogReplyFile::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns the file information
 *
 *  \return  Byte array
 */
/****************************************************************************/
QByteArray const& CmdDayRunLogReplyFile::GetFileData() const
{
    return m_FileData;
}

} // end namespace NetCommands
