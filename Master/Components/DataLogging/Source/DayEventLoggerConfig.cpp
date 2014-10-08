/****************************************************************************/
/*! \file DataLogging/Source/DayEventLoggerConfig.cpp
 *
 *  \brief Implementation file for class DayEventLoggerConfig.
 *
 *\b Description:
 *      This class can be used to deliver the configuration for the DayOperation
 *      logger using the signal / slot mechanism. It contains the operating
 *      mode, serial number and the max file age. If latter one is 0, no monitoring
 *      of file age is done!
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2013-10-16
 *  $Author:    $ Raju
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
    m_MaxFileCount(0) {
}

/****************************************************************************/
DayEventLoggerConfig::DayEventLoggerConfig(const QString &OperatingMode, const QString &SerialNumber,const QString& Version,
                                                   const QString &Path, int MaxFileCount, const QString &BaseFileName) :
    m_OperatingMode(OperatingMode),
    m_SerialNumber(SerialNumber),
    m_SWVersion(Version),
    m_Path(Path),
    m_MaxFileCount(MaxFileCount),
    m_BaseFileName(BaseFileName) {
}

/****************************************************************************/
DayEventLoggerConfig::DayEventLoggerConfig(const DayEventLoggerConfig &Other) :
    m_MaxFileCount(0) {
    CopyFrom(Other);
}

/****************************************************************************/
DayEventLoggerConfig::~DayEventLoggerConfig() {
}

/****************************************************************************/
void DayEventLoggerConfig::CopyFrom(const DayEventLoggerConfig &rOther) {
    m_OperatingMode = rOther.m_OperatingMode;
    m_SerialNumber  = rOther.m_SerialNumber;
    m_SWVersion     = rOther.m_SWVersion;
    m_Path          = rOther.m_Path;
    m_MaxFileCount  = rOther.m_MaxFileCount;
    m_BaseFileName  = rOther.m_BaseFileName;
}

/****************************************************************************/
const DayEventLoggerConfig & DayEventLoggerConfig::operator = (const DayEventLoggerConfig &rOther) {
    if(&rOther != this) {
        CopyFrom(rOther);
    }
    return *this;
}

} // end namespace DataLogging
