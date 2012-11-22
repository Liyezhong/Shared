/****************************************************************************/
/*! \file CmdOvenCoverReferenceRun.cpp
 *
 *  \brief Implementation file for class CmdOvenCoverReferenceRun.
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

#include <DeviceCommandProcessor/Include/Commands/CmdOvenCoverReferenceRun.h>

namespace DeviceCommandProcessor {

QString CmdOvenCoverReferenceRun::NAME           = "DeviceCommandProcessor::CmdOvenCoverReferenceRun";
int     CmdOvenCoverReferenceRun::m_CmdTimeout   = 30000;

/****************************************************************************/
CmdOvenCoverReferenceRun::CmdOvenCoverReferenceRun() :
    Global::Command(m_CmdTimeout)
{
}

/****************************************************************************/
CmdOvenCoverReferenceRun::~CmdOvenCoverReferenceRun() {
}

} // end namespace DeviceCommandProcessor


