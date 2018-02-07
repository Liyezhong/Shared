/****************************************************************************/
/*! \file CmdStationResetData.cpp
 *
 *  \brief Reset station data command implementation.
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

#include "HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdStationResetData.h"

#include <QBuffer>

namespace MsgClasses {

QString CmdStationResetData::NAME = "MsgClasses::CmdStationResetData";


CmdStationResetData::CmdStationResetData(int timeOut, const QString& stationID)
    : Command(timeOut)
    , m_StationID(stationID)
{
}

CmdStationResetData::CmdStationResetData(void)
    : Command(0)
{
}

CmdStationResetData::~CmdStationResetData(void)
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdStationResetData::GetName(void) const
{
    return NAME;
}

} // end namespace MsgClasses
