
/****************************************************************************/
/*! \file CmdLoaderRacksChanged.cpp
 *
 *  \brief Implementation file for class CmdLoaderRacksChanged.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-22
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

#include <DeviceCommandProcessor/Include/Commands/CmdLoaderRacksChanged.h>

namespace DeviceCommandProcessor {

QString CmdLoaderRacksChanged::NAME             = "DeviceCommandProcessor::CmdLoaderRacksChanged";
int     CmdLoaderRacksChanged::m_CmdTimeout     = 3000;

/****************************************************************************/
CmdLoaderRacksChanged::CmdLoaderRacksChanged(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Command(m_CmdTimeout),
    m_DevInstanceID(DevInstanceID)
{
    m_RFIDData[0] = 0;
    m_RFIDData[1] = 0;
    m_RFIDData[2] = 0;
    m_RFIDData[3] = 0;
    m_RFIDData[4] = 0;
}

/****************************************************************************/
CmdLoaderRacksChanged::~CmdLoaderRacksChanged() {
}

} // end namespace DeviceCommandProcessor
