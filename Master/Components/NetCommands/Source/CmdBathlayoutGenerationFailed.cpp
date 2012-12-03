/****************************************************************************/
/*! \file CmdBathlayoutGenerationFailed.cpp
 *
 *  \brief CmdBathlayoutGenerationFailed command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 06.11.2012
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

#include <NetCommands/Include/CmdBathlayoutGenerationFailed.h>

namespace NetCommands {

QString CmdBathlayoutGenerationFailed::NAME = "NetCommands::CmdBathlayoutGenerationFailed";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   Bathlayout  The new bath layout.
 */
/****************************************************************************/
CmdBathlayoutGenerationFailed::CmdBathlayoutGenerationFailed(int Timeout, const QString &BathlayoutGenStatus) :
    Command(Timeout),
    m_BathLayoutGenStatus(BathlayoutGenStatus)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   Bathlayout  The new bath layout.
 */
/****************************************************************************/
CmdBathlayoutGenerationFailed::CmdBathlayoutGenerationFailed() :
    Command(5000)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdBathlayoutGenerationFailed::~CmdBathlayoutGenerationFailed()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdBathlayoutGenerationFailed::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns the bath layout.
 *
 *  \return  Bath layout
 */
/****************************************************************************/
QString CmdBathlayoutGenerationFailed::GetBathLayoutStatus() const
{
    return m_BathLayoutGenStatus;
}

} // end namespace NetCommands

