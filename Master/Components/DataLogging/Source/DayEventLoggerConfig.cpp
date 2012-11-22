/****************************************************************************/
/*! \file DataLogging/Source/DayEventLoggerConfig.cpp
 *
 *  \brief Implementation file for class DayEventLoggerConfig.
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

#include <DataLogging/Include/DayEventLoggerConfig.h>

namespace DataLogging {

/****************************************************************************/
DayEventLoggerConfig::DayEventLoggerConfig() :
    m_MaxFileCount(0)
{
}

/****************************************************************************/
DayEventLoggerConfig::DayEventLoggerConfig(const QString &OperatingMode, const QString &SerialNumber,
                                                   const QString &Path, int MaxFileCount) :
    m_OperatingMode(OperatingMode),
    m_SerialNumber(SerialNumber),
    m_Path(Path),
    m_MaxFileCount(MaxFileCount)
{
}

/****************************************************************************/
DayEventLoggerConfig::DayEventLoggerConfig(const DayEventLoggerConfig &rOther) :
    m_MaxFileCount(0)
{
    CopyFrom(rOther);
}

/****************************************************************************/
DayEventLoggerConfig::~DayEventLoggerConfig() {
}

/****************************************************************************/
void DayEventLoggerConfig::CopyFrom(const DayEventLoggerConfig &rOther) {
    m_OperatingMode = rOther.m_OperatingMode;
    m_SerialNumber  = rOther.m_SerialNumber;
    m_Path          = rOther.m_Path;
    m_MaxFileCount  = rOther.m_MaxFileCount;
}

/****************************************************************************/
const DayEventLoggerConfig & DayEventLoggerConfig::operator = (const DayEventLoggerConfig &rOther) {
    if(&rOther != this) {
        CopyFrom(rOther);
    }
    return *this;
}

} // end namespace DataLogging
