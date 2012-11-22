/****************************************************************************/
/*! \file CmdGrapplerReferenceRun.cpp
 *
 *  \brief Implementation file for class CmdGrapplerReferenceRun.
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

#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerReferenceRun.h>

namespace DeviceCommandProcessor {

QString CmdGrapplerReferenceRun::NAME           = "DeviceCommandProcessor::CmdGrapplerReferenceRun";
int     CmdGrapplerReferenceRun::m_CmdTimeout   = 40000;

/****************************************************************************/
CmdGrapplerReferenceRun::CmdGrapplerReferenceRun(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Command(m_CmdTimeout),
    m_DevInstanceID(DevInstanceID)
{
}

/****************************************************************************/
CmdGrapplerReferenceRun::~CmdGrapplerReferenceRun() {
}

} // end namespace DeviceCommandProcessor
