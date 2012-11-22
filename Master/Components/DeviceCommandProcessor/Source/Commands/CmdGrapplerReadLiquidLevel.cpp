/****************************************************************************/
/*! \file CmdGrapplerReadLiquidLevel.cpp
 *
 *  \brief Implementation file for class CmdGrapplerReadLiquidLevel.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-15
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

#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerReadLiquidLevel.h>

namespace DeviceCommandProcessor {

QString CmdGrapplerReadLiquidLevel::NAME            = "DeviceCommandProcessor::CmdGrapplerReadLiquidLevel";
int     CmdGrapplerReadLiquidLevel::m_CmdTimeout    = 5000;

/****************************************************************************/
CmdGrapplerReadLiquidLevel::CmdGrapplerReadLiquidLevel(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Command(m_CmdTimeout),
    m_DevInstanceID(DevInstanceID)
{
}

/****************************************************************************/
CmdGrapplerReadLiquidLevel::~CmdGrapplerReadLiquidLevel() {
}

} // end namespace DeviceCommandProcessor

