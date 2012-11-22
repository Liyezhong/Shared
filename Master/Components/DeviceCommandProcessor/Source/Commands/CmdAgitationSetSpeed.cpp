/****************************************************************************/
/*! \file CmdAgitationSetSpeed.cpp
 *
 *  \brief Implementation file for class CmdAgitationSetSpeed.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-21
 *  $Author:    $ N.Wiedmann
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

#include <DeviceCommandProcessor/Include/Commands/CmdAgitationSetSpeed.h>

namespace DeviceCommandProcessor {

QString CmdAgitationSetSpeed::NAME           = "DeviceCommandProcessor::CmdAgitationSetSpeed";
int     CmdAgitationSetSpeed::m_CmdTimeout   = 3000;

/****************************************************************************/
CmdAgitationSetSpeed::CmdAgitationSetSpeed() :
    Global::Command(m_CmdTimeout),
    m_AgitationSpeed(DeviceControl::AGITATION_SPEED_UNDEF)
{
}

/****************************************************************************/
CmdAgitationSetSpeed::~CmdAgitationSetSpeed() {
}

} // end namespace DeviceCommandProcessor



