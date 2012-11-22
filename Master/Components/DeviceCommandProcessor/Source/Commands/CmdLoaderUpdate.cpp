/****************************************************************************/
/*! \file CmdLoaderUpdate.cpp
 *
 *  \brief Implementation file for class CmdLoaderUpdate.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-06-29
 *  $Author:    $ N.Kamath
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

#include <DeviceCommandProcessor/Include/Commands/CmdLoaderUpdate.h>.h>

namespace DeviceCommandProcessor {

QString CmdLoaderUpdate::NAME             = "DeviceCommandProcessor::CmdLoaderUpdate";
int     CmdLoaderUpdate::m_CmdTimeout     = 3000;

/****************************************************************************/
CmdLoaderUpdate::CmdLoaderUpdate(DeviceControl::DevInstanceID_t DevInstanceID)
    : Global::Command(m_CmdTimeout)
    , m_DevInstanceID(DevInstanceID)
    , m_RackId(0)
    , m_RackUserData(0)
    , m_StationID(DeviceControl::LOADER_ID_RFID_UNDEF)
{
}

/****************************************************************************/
CmdLoaderUpdate::~CmdLoaderUpdate() {
}

} // end namespace DeviceCommandProcessor

