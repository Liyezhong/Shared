/****************************************************************************/
/*! \file CmdDeviceProcessingCleanup.cpp
 *
 *  \brief Implementation file for class CmdDeviceProcessingCleanup.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-01
 *  $Author:    $ Norbert Wiedmann
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

#include <DeviceCommandProcessor/Include/Commands/CmdDeviceProcessingCleanup.h>

namespace DeviceCommandProcessor {

QString CmdDeviceProcessingCleanup::NAME            = "DeviceCommandProcessor::CmdDeviceProcessingCleanup";
int     CmdDeviceProcessingCleanup::m_CmdTimeout    = 10000;

/****************************************************************************/
CmdDeviceProcessingCleanup::CmdDeviceProcessingCleanup() :
    Global::Command(m_CmdTimeout)
{
}

/****************************************************************************/
CmdDeviceProcessingCleanup::~CmdDeviceProcessingCleanup() {
}

} // end namespace DeviceCommandProcessor
