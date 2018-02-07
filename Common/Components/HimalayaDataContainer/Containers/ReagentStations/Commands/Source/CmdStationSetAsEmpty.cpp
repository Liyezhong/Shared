/****************************************************************************/
/*! \file CmdStationSetAsEmpty.cpp
 *
 *  \brief Set the station as full command implementation.
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

#include "HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdStationSetAsEmpty.h"

#include <QBuffer>

namespace MsgClasses {

QString CmdStationSetAsEmpty::NAME = "MsgClasses::CmdStationSetAsEmpty";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \param[in]   timeout     Timeout for command.
 * \param[in]   stationID   Unique reagent id.
 */
/****************************************************************************/
CmdStationSetAsEmpty::CmdStationSetAsEmpty(int timeout,
                                             const QString& stationID)
    : Command(timeout)
    , m_StationID(stationID)
{
}

CmdStationSetAsEmpty::CmdStationSetAsEmpty(void)
    : Command(0)
{
}

CmdStationSetAsEmpty::~CmdStationSetAsEmpty(void)
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdStationSetAsEmpty::GetName(void) const
{
    return NAME;
}

} // end namespace MsgClasses
