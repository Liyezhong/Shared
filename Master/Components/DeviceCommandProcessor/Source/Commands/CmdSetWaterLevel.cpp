/****************************************************************************/
/*! \file CmdSetWaterLevel.cpp
 *
 *  \brief Implementation file for class CmdSetWaterLevel.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-06-13
 *  $Author:    $ Michael Thiel
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <DeviceCommandProcessor/Include/Commands/CmdSetWaterLevel.h>

namespace DeviceCommandProcessor {

QString CmdSetWaterLevel::NAME           = "DeviceCommandProcessor::CmdSetWaterLevel";
int     CmdSetWaterLevel::m_CmdTimeout   = 5000;

/****************************************************************************/
CmdSetWaterLevel::CmdSetWaterLevel(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Command(m_CmdTimeout),
    m_DevInstanceID(DevInstanceID)
{
}

/****************************************************************************/
CmdSetWaterLevel::~CmdSetWaterLevel() {
}

} // end namespace DeviceCommandProcessor

