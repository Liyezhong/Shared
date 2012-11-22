/****************************************************************************/
/*! \file AcknSlideCount.cpp
 *
 *  \brief Implementation file for class AcknSlideCount.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-18
 *  $Author:    $ Michael Thiel
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

#include <DeviceCommandProcessor/Include/Commands/AcknSlideCount.h>

namespace DeviceCommandProcessor {

QString AcknSlideCount::NAME = "DeviceCommandProcessor::AcknSlideCount";

/****************************************************************************/
AcknSlideCount::AcknSlideCount(DeviceControl::DevInstanceID_t DevInstanceID)
    : Global::Acknowledge()
    , m_DevInstanceID(DevInstanceID)
//    , NAME("DeviceCommandProcessor::AcknSlideCount")
{
}

AcknSlideCount::AcknSlideCount(quint32 rackId, qint8 slideNumber)
    : m_slideNumber(slideNumber)
    , m_rackId(rackId)
//    , NAME("DeviceCommandProcessor::AcknSlideCount")
{
}


/****************************************************************************/
AcknSlideCount::~AcknSlideCount()
{
}

} // end namespace DeviceCommandProcessor


