/****************************************************************************/
/*! \file CmdUpdateStationReagentStatus.cpp
 *
 *  \brief when the running Program finishes with 1 Reagent, the Reagent status should be updated.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-03-20
 *  $Author:    $ Abe Yang
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2013 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdUpdateStationReagentStatus.h"

#include <QBuffer>

namespace MsgClasses {

QString CmdUpdateStationReagentStatus::NAME = "MsgClasses::CmdUpdateStationReagentStatus";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \param[in]   timeOut     Timeout for command.
 * \param[in]   stationIDs   Unique Station id.
 * \param[in]   cassetteCount   if RMS os Cassete, we should give CassetteCount
 */
/****************************************************************************/
CmdUpdateStationReagentStatus::CmdUpdateStationReagentStatus(int timeOut, const QStringList& stationIDs, int cassetteCount)
    : Command(timeOut)
    , m_CassetteCount(cassetteCount)
    , m_StationIDs(stationIDs)
{
}

CmdUpdateStationReagentStatus::CmdUpdateStationReagentStatus(void)
    : Command(0), m_CassetteCount(0)
{
}

CmdUpdateStationReagentStatus::~CmdUpdateStationReagentStatus(void)
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdUpdateStationReagentStatus::GetName(void) const
{
    return NAME;
}

} // end namespace MsgClasses
