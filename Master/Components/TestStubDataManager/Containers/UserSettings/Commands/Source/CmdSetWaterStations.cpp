/****************************************************************************/
/*! \file CmdSetWaterStations.cpp
 *
 *  \brief CmdSetWaterStations command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.03.2010
 *   $Author:  $ Y.Novak
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

#include "TestStubDataManager/Containers/UserSettings/Commands/Include/CmdSetWaterStations.h"

namespace MsgClasses {

QString CmdSetWaterStations::NAME = "MsgClasses::CmdSetWaterStations";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   ReagentID   Reagent ID.
 * \param[in]   Timeout     Group.
 */
/****************************************************************************/
CmdSetWaterStations::CmdSetWaterStations(int Timeout, const QString &ReagentID, const QString &Group) :
    Command(Timeout),
    m_ReagentId(ReagentID),
    m_Group(Group)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdSetWaterStations::~CmdSetWaterStations()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdSetWaterStations::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns Reagent ID for water stations group
 *
 *  \return  reagent id
 */
/****************************************************************************/
QString CmdSetWaterStations::GetReagentID() const
{
    return m_ReagentId;
}

/****************************************************************************/
/*!
 *  \brief   This function returns water stations group to set reagent for
 *
 *  \return  water stations group
 */
/****************************************************************************/
QString CmdSetWaterStations::GetGroup() const
{
    return m_Group;
}

} // end namespace MsgClasses
