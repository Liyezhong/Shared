/****************************************************************************/
/*! \file AcknGrapplerReadLiquidLevel.cpp
 *
 *  \brief Implementation file for class AcknGrapplerReadLiquidLevel.
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

#include <DeviceCommandProcessor/Include/Commands/AcknGrapplerReadLiquidLevel.h>

namespace DeviceCommandProcessor {

QString AcknGrapplerReadLiquidLevel::NAME = "DeviceCommandProcessor::AcknGrapplerReadLiquidLevel";

/****************************************************************************/
AcknGrapplerReadLiquidLevel::AcknGrapplerReadLiquidLevel(DeviceControl::DevInstanceID_t DevInstanceID) :
    Global::Acknowledge(),
    m_DevInstanceID(DevInstanceID)
{
}

/****************************************************************************/
AcknGrapplerReadLiquidLevel::~AcknGrapplerReadLiquidLevel() {
}

} // end namespace DeviceCommandProcessor



