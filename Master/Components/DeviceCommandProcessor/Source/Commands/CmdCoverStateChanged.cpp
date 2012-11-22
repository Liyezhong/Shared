/****************************************************************************/
/*! \file CmdCoverStateChanged.cpp
 *
 *  \brief Implementation file for class CmdCoverStateChanged.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-04
 *  $Author:    $ N.Kamath
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

#include <DeviceCommandProcessor/Include/Commands/CmdCoverStateChanged.h>

namespace DeviceCommandProcessor {

QString CmdCoverStateChanged::NAME           = "DeviceCommandProcessor::CmdCoverStateChanged";
int     CmdCoverStateChanged::m_CmdTimeout   = 5000;

/****************************************************************************/
CmdCoverStateChanged::CmdCoverStateChanged(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Command(m_CmdTimeout),
    m_DevInstanceID(DevInstanceID)
{
}

/****************************************************************************/
CmdCoverStateChanged::~CmdCoverStateChanged() {
}

} // end namespace DeviceCommandProcessor


