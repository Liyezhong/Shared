/****************************************************************************/
/*! \file CmdWaterStationUpdate.cpp
 *
 *  \brief CmdWaterStationUpdate command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 04.04.2012
 *   $Author:  $ V.Raghunath
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

#include "DataManager/Containers/UserSettings/Commands/Include/CmdWaterStationUpdate.h"

namespace MsgClasses {

QString CmdWaterStationUpdate::NAME = "MsgClasses::CmdWaterStationUpdate";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout             Timeout for command.
 * \param[in]   StationID           Rack RFID.
 * \param[in]   WaterFlowState      Water Flow State.
 */
/****************************************************************************/
CmdWaterStationUpdate::CmdWaterStationUpdate(int Timeout,
                                             QString StationID,
                                             const bool &WaterFlowState) :
    Command(Timeout),
    m_StationID(StationID),
    m_WaterFlowState(WaterFlowState)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdWaterStationUpdate::~CmdWaterStationUpdate()
{
}

/****************************************************************************/
/*!
 *  \brief  Constructor to receive commands
 */
/****************************************************************************/
CmdWaterStationUpdate::CmdWaterStationUpdate() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdWaterStationUpdate::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   Get Water station group
 *
 *  \return  water station ID
 */
/****************************************************************************/
QString CmdWaterStationUpdate::GetWaterStationID() const
{
    return m_StationID;
}

/****************************************************************************/
/*!
 *  \brief   Get Water flow state
 *
 *  \return  water source is present or not
 */
/****************************************************************************/
bool CmdWaterStationUpdate::GetWaterFlowState() const
{
    return m_WaterFlowState;

}

}//End of namespace MsgClasses


