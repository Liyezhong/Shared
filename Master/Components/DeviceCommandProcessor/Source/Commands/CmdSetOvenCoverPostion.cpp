/****************************************************************************/
/*! \file CmdSetOvencoverPostion.cpp
 *
 *  \brief Implementation file for class CmdSetOvencoverPostion.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-19
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

#include <DeviceCommandProcessor/Include/Commands/CmdSetOvenCoverPostion.h>

namespace DeviceCommandProcessor {

QString CmdSetOvenCoverPostion::NAME           = "DeviceCommandProcessor::CmdSetOvenCoverPostion";
int     CmdSetOvenCoverPostion::m_CmdTimeout   = 30000;

/****************************************************************************/
CmdSetOvenCoverPostion::CmdSetOvenCoverPostion() :
    Global::Command(m_CmdTimeout),
    m_OvenCoverPosition(DeviceControl::OVEN_COVER_UNDEF)
{

}

/****************************************************************************/
CmdSetOvenCoverPostion::~CmdSetOvenCoverPostion() {
}

} // end namespace DeviceCommandProcessor



