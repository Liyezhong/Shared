/****************************************************************************/
/*! \file CmdGrapplerTransportRackTo.h
 *
 *  \brief Implementation file for class CmdGrapplerTransportRackTo.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-31
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

#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerTransportRackTo.h>

namespace DeviceCommandProcessor {

QString CmdGrapplerTransportRackTo::NAME = "DeviceCommandProcessor::CmdGrapplerTransportRackTo";
int     CmdGrapplerTransportRackTo::m_CmdTimeout   = 30000;

/****************************************************************************/
CmdGrapplerTransportRackTo::CmdGrapplerTransportRackTo(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Command(m_CmdTimeout),
    m_DevInstanceID(DevInstanceID)
{
}

/****************************************************************************/
CmdGrapplerTransportRackTo::~CmdGrapplerTransportRackTo() {
}

} // end namespace DeviceCommandProcessor
