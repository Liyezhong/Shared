/****************************************************************************/
/*! \file CmdGrapplerReadRFID.cpp
 *
 *  \brief Implementation file for class CmdGrapplerReadRFID.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-14
 *  $Author:    $ Norbert Wiedmann
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

#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerReadRFID.h>

namespace DeviceCommandProcessor {

QString CmdGrapplerReadRFID::NAME           = "DeviceCommandProcessor::CmdGrapplerReadRFID";
int     CmdGrapplerReadRFID::m_CmdTimeout   = 5000;

/****************************************************************************/
CmdGrapplerReadRFID::CmdGrapplerReadRFID(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Command(m_CmdTimeout),
    m_DevInstanceID(DevInstanceID)
{
}

/****************************************************************************/
CmdGrapplerReadRFID::~CmdGrapplerReadRFID() {
}

} // end namespace DeviceCommandProcessor

