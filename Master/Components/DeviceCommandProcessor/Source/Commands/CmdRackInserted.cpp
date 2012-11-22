
/****************************************************************************/
/*! \file CmdRackInserted.cpp
 *
 *  \brief Implementation file for class CmdRackInserted.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-06-13
 *  $Author:    $ Michael Thiel
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

#include <DeviceCommandProcessor/Include/Commands/CmdRackInserted.h>

namespace DeviceCommandProcessor {

QString CmdRackInserted::NAME             = "DeviceCommandProcessor::CmdRackInserted";
int     CmdRackInserted::m_CmdTimeout     = 3000;

/****************************************************************************/
CmdRackInserted::CmdRackInserted(DeviceControl::DevInstanceID_t DevInstanceID)
    : Global::Command(m_CmdTimeout)
    , m_Rack(1,0)
    , m_DevInstanceID(DevInstanceID)
    , m_OvenTemperature(0)
{
}

/****************************************************************************/
CmdRackInserted::~CmdRackInserted() {
}

} // end namespace DeviceCommandProcessor
