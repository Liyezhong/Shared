/****************************************************************************/
/*! \file CmdDeviceProcessingInit.cpp
 *
 *  \brief Implementation file for class CmdDeviceProcessingInit.
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

#include <DeviceCommandProcessor/Include/Commands/CmdDeviceProcessingInit.h>

namespace DeviceCommandProcessor {

QString CmdDeviceProcessingInit::NAME           = "DeviceCommandProcessor::CmdDeviceProcessingInit";
int     CmdDeviceProcessingInit::m_CmdTimeout   = 30000;

/****************************************************************************/
CmdDeviceProcessingInit::CmdDeviceProcessingInit() :
    Global::Command(m_CmdTimeout)
{
}

/****************************************************************************/
CmdDeviceProcessingInit::~CmdDeviceProcessingInit() {
}

} // end namespace DeviceCommandProcessor
