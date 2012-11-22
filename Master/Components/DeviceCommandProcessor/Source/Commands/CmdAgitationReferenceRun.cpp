/****************************************************************************/
/*! \file CmdAgitationReferenceRun.cpp
 *
 *  \brief Implementation file for class CmdAgitationReferenceRun.
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

#include <DeviceCommandProcessor/Include/Commands/CmdAgitationReferenceRun.h>

namespace DeviceCommandProcessor {

QString CmdAgitationReferenceRun::NAME           = "DeviceCommandProcessor::CmdAgitationReferenceRun";
int     CmdAgitationReferenceRun::m_CmdTimeout   = 30000;

/****************************************************************************/
CmdAgitationReferenceRun::CmdAgitationReferenceRun() :
    Global::Command(m_CmdTimeout)
{
}

/****************************************************************************/
CmdAgitationReferenceRun::~CmdAgitationReferenceRun() {
}

} // end namespace DeviceCommandProcessor


