/****************************************************************************/
/*! \file CmdLoaderSetBlockingState.cpp
 *
 *  \brief Implementation file for class CmdLoaderSetBlockingState.
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

#include <DeviceCommandProcessor/Include/Commands/CmdLoaderSetBlockingState.h>

namespace DeviceCommandProcessor {

QString CmdLoaderSetBlockingState::NAME           = "DeviceCommandProcessor::CmdLoaderSetBlockingState";
int     CmdLoaderSetBlockingState::m_CmdTimeout   = 3000;

/****************************************************************************/
CmdLoaderSetBlockingState::CmdLoaderSetBlockingState(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Command(m_CmdTimeout),
    m_DevInstanceID(DevInstanceID),
    m_BlockingState(DeviceControl::BLOCKING_STATE_UNDEF)
{
}

/****************************************************************************/
CmdLoaderSetBlockingState::~CmdLoaderSetBlockingState() {
}

} // end namespace DeviceCommandProcessor


