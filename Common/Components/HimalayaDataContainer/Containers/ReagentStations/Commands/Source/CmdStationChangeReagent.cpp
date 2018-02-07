/****************************************************************************/
/*! \file CmdStationChangeReagent.cpp
 *
 *  \brief Change reagent in station command definition.
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

#include "HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdStationChangeReagent.h"

#include <QBuffer>

namespace MsgClasses {

QString CmdStationChangeReagent::NAME = "MsgClasses::CmdStationChangeReagent";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \param[in]   timeOut     Timeout for command.
 * \param[in]   stationID   Unique reagent id.
 * \param reagentID
 */
/****************************************************************************/
CmdStationChangeReagent::CmdStationChangeReagent(int timeOut, const QString& stationID, const QString& reagentID)
    : Command(timeOut)
    , m_StationID(stationID)
    , m_ReagentID(reagentID)
{
}

CmdStationChangeReagent::CmdStationChangeReagent(void)
    : Command(0)
{
}

CmdStationChangeReagent::~CmdStationChangeReagent(void)
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdStationChangeReagent::GetName(void) const
{
    return NAME;
}

const QString& CmdStationChangeReagent::StationID()const
{
    return m_StationID;
}

const QString& CmdStationChangeReagent::ReagentID()const
{
    return m_ReagentID;
}

} // end namespace MsgClasses
