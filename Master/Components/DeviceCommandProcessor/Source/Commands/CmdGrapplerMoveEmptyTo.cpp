/****************************************************************************/
/*! \file CmdGrapplerMoveEmptyTo.cpp
 *
 *  \brief Implementation file for class CmdGrapplerMoveEmptyTo.
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

#include <DeviceCommandProcessor/Include/Commands/CmdGrapplerMoveEmptyTo.h>

namespace DeviceCommandProcessor {

QString CmdGrapplerMoveEmptyTo::NAME            = "DeviceCommandProcessor::CmdGrapplerMoveEmptyTo";
int     CmdGrapplerMoveEmptyTo::m_CmdTimeout    = 30000;

/****************************************************************************/
CmdGrapplerMoveEmptyTo::CmdGrapplerMoveEmptyTo(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Command(m_CmdTimeout),
    m_DevInstanceID(DevInstanceID)
{
}

/****************************************************************************/
CmdGrapplerMoveEmptyTo::~CmdGrapplerMoveEmptyTo() {
}

} // end namespace DeviceCommandProcessor
