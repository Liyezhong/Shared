/****************************************************************************/
/*! \file CmdStationSuckDrain.cpp
 *
 *  \brief CmdStationSuckDrain command implementation.
 *
 *   $Version: $ 0.1
*   $Date:    $ 27.03.2013
 *   $Author:  $ Abe Yang
 *
 *  \b Company:
 *
  *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2013 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdStationSuckDrain.h"
namespace MsgClasses {

QString CmdStationSuckDrain::NAME = "MsgClasses::CmdStationSuckDrain";


CmdStationSuckDrain::CmdStationSuckDrain(int timeout, const QString& stationID,
                                         bool isStart, bool isSuck, bool noCleaningProgram) :
    Command(timeout),
    m_StationID(stationID),
    m_IsStart(isStart),
    m_IsSuck(isSuck),
    m_NoCleaningProgram(noCleaningProgram)
{
}

CmdStationSuckDrain::CmdStationSuckDrain():
    Command(0),
    m_StationID(""),
    m_IsStart(false),
    m_IsSuck(false),
    m_NoCleaningProgram(true)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdStationSuckDrain::~CmdStationSuckDrain()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdStationSuckDrain::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses
