/****************************************************************************/
/*! \file AcknEstimatedTime.cpp
 *
 *  \brief Implementation file for class AcknEstimatedTime.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-06-18
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

#include <DeviceCommandProcessor/Include/Commands/AcknEstimatedTime.h>

namespace DeviceCommandProcessor {

QString AcknEstimatedTime::NAME = "DeviceCommandProcessor::AcknEstimatedTime";

/****************************************************************************/
AcknEstimatedTime::AcknEstimatedTime(DeviceControl::DevInstanceID_t DevInstanceID)
    : Global::Acknowledge()
    , m_DevInstanceID(DevInstanceID)
//    , NAME("DeviceCommandProcessor::AcknEstimatedTime")
{
}

AcknEstimatedTime::AcknEstimatedTime(QByteArray configData, QByteArray duration)
    : m_configData(configData)
    , m_duration(duration)
//    , NAME("DeviceCommandProcessor::AcknEstimatedTime")
{
}


/****************************************************************************/
AcknEstimatedTime::~AcknEstimatedTime()
{
}

} // end namespace DeviceCommandProcessor


