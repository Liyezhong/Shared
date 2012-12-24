/****************************************************************************/
/*! \file CmdUpdateBlgStatus.cpp
 *
 *  \brief CmdUpdateBlgStatus command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 19.12.2012
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

#include <NetCommands/Include/CmdUpdateBlgStatus.h>

namespace NetCommands {

QString CmdUpdateBlgStatus::NAME = "NetCommands::CmdUpdateBlgStatus";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   Bathlayout  The new bath layout.
 */
/****************************************************************************/
CmdUpdateBlgStatus::CmdUpdateBlgStatus(int Timeout,const BlgStatus_t BlgStatusId) :
    Command(Timeout),    
    m_BlgStatusId(BlgStatusId)

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
CmdUpdateBlgStatus::CmdUpdateBlgStatus() :
    Command(5000)
{
}
/****************************************************************************/
/*!
 *  \brief   This function returns the scan message id.
 *
 *  \return  Message Id.
 */
/****************************************************************************/
BlgStatus_t CmdUpdateBlgStatus::GetBlgStatusMsgId() const
{
    return m_BlgStatusId;
}
/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdUpdateBlgStatus::~CmdUpdateBlgStatus()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdUpdateBlgStatus::GetName() const
{
    return NAME;
}

} // end namespace NetCommands


