/****************************************************************************/
/*! \file CmdReagentGroupUpdate.cpp
 *
 *  \brief Update reagent group command implementation.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-02-02
 *  $Author:    $ R. Wu
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2010 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "HimalayaDataContainer/Containers/ReagentGroups/Commands/Include/CmdReagentGroupUpdate.h"

#include <QBuffer>

namespace MsgClasses {

QString CmdReagentGroupUpdate::NAME = "MsgClasses::CmdReagentGroupUpdate";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \param[in]   timeOut     Timeout for command.
 * \param[in]   groupId   Unique reagent id.
 * \param ColorId
 */
/****************************************************************************/
CmdReagentGroupUpdate::CmdReagentGroupUpdate(int timeOut,
                                             const QString& groupId,
                                             const QString& ColorId)
    : Command(timeOut)
    , m_Color(ColorId)
    , m_GroupId(groupId)

{
}

CmdReagentGroupUpdate::CmdReagentGroupUpdate(void)
    : Command(0)
{
}

CmdReagentGroupUpdate::~CmdReagentGroupUpdate(void)
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdReagentGroupUpdate::GetName(void) const
{
    return NAME;
}

} // end namespace MsgClasses
