/****************************************************************************/
/*! \file CmdProgramSequenceBlgTempFile.cpp
 *
 *  \brief CmdProgramSequenceBlgTempFile command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 28.11.2012
 *   $Author:  $ Ningu
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
#include <QBuffer>
#include <NetCommands/Include/CmdProgramSequenceBlgTempFile.h>
#include <QBuffer>


namespace NetCommands {

QString CmdProgramSequenceBlgTempFile::NAME = "NetCommands::CmdProgramSequenceBlgTempFile";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   ProgramSequenceBlgTempFile  The new bath layout.
 */
/****************************************************************************/
CmdProgramSequenceBlgTempFile::CmdProgramSequenceBlgTempFile(int Timeout, const QDataStream &ProgramSequenceBlgTempFile) :
    Command(Timeout),
    m_ProgramSeqBlgTemp(static_cast<QBuffer *> (ProgramSequenceBlgTempFile.device())->data())
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   ProgramSequenceBlgTempFile.
 */
/****************************************************************************/
CmdProgramSequenceBlgTempFile::CmdProgramSequenceBlgTempFile() :
    Command(5000)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdProgramSequenceBlgTempFile::~CmdProgramSequenceBlgTempFile()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdProgramSequenceBlgTempFile::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns the ProgramSequenceBlgTempFile.
 *
 *  \return  ProgramSequenceBlgTempFile
 */
/****************************************************************************/
QByteArray const& CmdProgramSequenceBlgTempFile::GetProgramSequenceBlgTempFile() const
{
    return m_ProgramSeqBlgTemp;
}

} // end namespace NetCommands

