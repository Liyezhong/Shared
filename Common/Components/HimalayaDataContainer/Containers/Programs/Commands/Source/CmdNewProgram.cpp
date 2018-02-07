/****************************************************************************/
/*! \file CmdNewProgram.cpp
 *
 *  \brief CmdNewProgram command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 12.04.2012
 *   $Author:  $ N.Kamath
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

#include "HimalayaDataContainer/Containers/Programs/Commands/Include/CmdNewProgram.h"
#include <QBuffer>
namespace MsgClasses {

QString CmdNewProgram::NAME = "MsgClasses::CmdNewProgram";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param  ProgramDataStream
 * \param  ProgramSequenceDataStream
 */
/****************************************************************************/
CmdNewProgram::CmdNewProgram(int Timeout, const QDataStream &ProgramDataStream,
                             const QDataStream &ProgramSequenceDataStream)
    :Command(Timeout),
    m_ProgramByteArray(static_cast< QBuffer *>(ProgramDataStream.device())->data()),
    m_ProgramSequenceByteArray(static_cast<QBuffer *>(ProgramSequenceDataStream.device())->data())
{
    setIdleStateAllowed(true);
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   ProgramDataStream
 */
/****************************************************************************/
CmdNewProgram::CmdNewProgram(int Timeout, const QDataStream &ProgramDataStream)
    :Command(Timeout),
    m_ProgramByteArray(static_cast< QBuffer *>(ProgramDataStream.device())->data())
{
    setIdleStateAllowed(true);
}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdNewProgram::CmdNewProgram() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdNewProgram::~CmdNewProgram()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdNewProgram::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

