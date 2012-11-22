/****************************************************************************/
/*! \file CmdExecutionStateChanged.cpp
 *
 *  \brief Implementation file for class CmdExecutionStateChanged.
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

#include <DeviceCommandProcessor/Include/Commands/CmdExecutionStateChanged.h>

namespace DeviceCommandProcessor {

QString CmdExecutionStateChanged::NAME           = "DeviceCommandProcessor::CmdExecutionStateChanged";
int     CmdExecutionStateChanged::m_CmdTimeout   = 5000;

/****************************************************************************/
CmdExecutionStateChanged::CmdExecutionStateChanged(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Command(m_CmdTimeout),
    m_DevInstanceID(DevInstanceID)
{
}

/****************************************************************************/
CmdExecutionStateChanged::~CmdExecutionStateChanged() {
}

} // end namespace DeviceCommandProcessor

