/****************************************************************************/
/*! \file DataLogging/Source/EventLoggerConfig.cpp
 *
 *  \brief Implementation file for class EventLoggerConfig.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
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

#include <DataLogging/Include/EventLoggerConfig.h>

namespace DataLogging {

/****************************************************************************/
EventLoggerConfig::EventLoggerConfig() :
    m_MaxFileSize(0)
{
}

/****************************************************************************/
EventLoggerConfig::EventLoggerConfig(const QString &EventLoggerBaseFileName,
                                     const QString &OperatingMode, const QString &SerialNumber,
                                     const QString &Path, qint64 MaxFileSize) :
    m_OperatingMode(OperatingMode),
    m_EventLoggerBaseFileName(EventLoggerBaseFileName),
    m_SerialNumber(SerialNumber),
    m_Path(Path),
    m_MaxFileSize(MaxFileSize)
{
    if(m_MaxFileSize < 0) {
        m_MaxFileSize = 0;
    }
}

/****************************************************************************/
EventLoggerConfig::EventLoggerConfig(const EventLoggerConfig &rOther) :
    m_MaxFileSize(0)
{
    CopyFrom(rOther);
}

/****************************************************************************/
EventLoggerConfig::~EventLoggerConfig() {
}

/****************************************************************************/
void EventLoggerConfig::CopyFrom(const EventLoggerConfig &rOther) {
    m_OperatingMode             = rOther.m_OperatingMode;
    m_EventLoggerBaseFileName   = rOther.m_EventLoggerBaseFileName;
    m_SerialNumber              = rOther.m_SerialNumber;
    m_Path                      = rOther.m_Path;
    m_MaxFileSize               = rOther.m_MaxFileSize;
}

/****************************************************************************/
const EventLoggerConfig & EventLoggerConfig::operator = (const EventLoggerConfig &rOther) {
    if(&rOther != this) {
        CopyFrom(rOther);
    }
    return *this;
}

} // end namespace DataLogging
