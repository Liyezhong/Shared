/****************************************************************************/
/*! \file CmdGrapplerLetDownRack.cpp
 *
 *  \brief Implementation file for class CmdGrapplerLetDownRack.
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

#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerLetDownRack.h>

namespace DeviceCommandProcessor {

QString CmdGrapplerLetDownRack::NAME            = "DeviceCommandProcessor::CmdGrapplerLetDownRack";
int     CmdGrapplerLetDownRack::m_CmdTimeout    = 10000;

/****************************************************************************/
CmdGrapplerLetDownRack::CmdGrapplerLetDownRack(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Command(m_CmdTimeout),
    m_DevInstanceID(DevInstanceID)
{
}

/****************************************************************************/
CmdGrapplerLetDownRack::~CmdGrapplerLetDownRack() {
}

} // end namespace DeviceCommandProcessor
