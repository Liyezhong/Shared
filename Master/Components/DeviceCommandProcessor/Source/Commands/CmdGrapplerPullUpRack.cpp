/****************************************************************************/
/*! \file CmdGrapplerPullUpRack.cpp
 *
 *  \brief Implementation file for class CmdGrapplerPullUpRack.
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

#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerPullUpRack.h>

namespace DeviceCommandProcessor {

QString CmdGrapplerPullUpRack::NAME         = "DeviceCommandProcessor::CmdGrapplerPullUpRack";
int     CmdGrapplerPullUpRack::m_CmdTimeout = 10000;

/****************************************************************************/
CmdGrapplerPullUpRack::CmdGrapplerPullUpRack(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Command(m_CmdTimeout),
    m_DevInstanceID(DevInstanceID)
{
}

/****************************************************************************/
CmdGrapplerPullUpRack::~CmdGrapplerPullUpRack() {
}

} // end namespace DeviceCommandProcessor
