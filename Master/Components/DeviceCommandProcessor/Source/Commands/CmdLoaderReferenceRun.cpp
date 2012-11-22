/****************************************************************************/
/*! \file CmdLoaderReferenceRun.cpp
 *
 *  \brief Implementation file for class CmdLoaderReferenceRun.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-11
 *  $Author:    $ N.Wiedmann
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

#include <DeviceCommandProcessor/Include/Commands/CmdLoaderReferenceRun.h>

namespace DeviceCommandProcessor {

QString CmdLoaderReferenceRun::NAME           = "DeviceCommandProcessor::CmdLoaderReferenceRun";
int     CmdLoaderReferenceRun::m_CmdTimeout   = 30000;

/****************************************************************************/
CmdLoaderReferenceRun::CmdLoaderReferenceRun(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Command(m_CmdTimeout),
    m_DevInstanceID(DevInstanceID)
{
}

/****************************************************************************/
CmdLoaderReferenceRun::~CmdLoaderReferenceRun() {
}

} // end namespace DeviceCommandProcessor

