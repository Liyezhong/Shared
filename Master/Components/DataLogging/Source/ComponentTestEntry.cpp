/****************************************************************************/
/*! \file DataLogging/Source/ComponentTestEntry.cpp
 *
 *  \brief Implementation file for class ComponentTestEntry.
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

#include <DataLogging/Include/ComponentTestEntry.h>

namespace DataLogging {

/****************************************************************************/
ComponentTestEntry::ComponentTestEntry() {
}

/****************************************************************************/
ComponentTestEntry::ComponentTestEntry(const ComponentTestEntry &rOther) {
    CopyFrom(rOther);
}

/****************************************************************************/
ComponentTestEntry::ComponentTestEntry( const QString &ComponentName, const QString &BoardName,
                                        const QString &BoardSerialNumber,
                                        const QString &TesterName, const QString &DeviceSerialNumber,
                                        const QString &ComponentSerialNumber, const QString &ComponentFirmwareVersion,
                                        const QString &ComponentHardwareVersion, const QString &MainboardSoftwareVersion,
                                        const Global::tTranslatableStringList &TestResults) :
    m_ComponentName(ComponentName),
    m_BoardName(BoardName),
    m_BoardSerialNumber(BoardSerialNumber),
    m_TesterName(TesterName),
    m_DeviceSerialNumber(DeviceSerialNumber),
    m_ComponentSerialNumber(ComponentSerialNumber),
    m_ComponentFirmwareVersion(ComponentFirmwareVersion),
    m_ComponentHardwareVersion(ComponentHardwareVersion),
    m_MainboardSoftwareVersion(MainboardSoftwareVersion),
    m_TestResults(TestResults)
{
}

/****************************************************************************/
ComponentTestEntry::~ComponentTestEntry() {
}

/****************************************************************************/
const ComponentTestEntry & ComponentTestEntry::operator = (const ComponentTestEntry &rOther) {
    if(this != &rOther) {
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
void ComponentTestEntry::CopyFrom(const ComponentTestEntry &rOther) {
    m_ComponentName = rOther.m_ComponentName;
    m_BoardName = rOther.m_BoardName;
    m_BoardSerialNumber = rOther.m_BoardSerialNumber;
    m_TesterName = rOther.m_TesterName;
    m_DeviceSerialNumber = rOther.m_DeviceSerialNumber;
    m_ComponentSerialNumber = rOther.m_ComponentSerialNumber;
    m_ComponentFirmwareVersion = rOther.m_ComponentFirmwareVersion;
    m_ComponentHardwareVersion = rOther.m_ComponentHardwareVersion;
    m_MainboardSoftwareVersion = rOther.m_MainboardSoftwareVersion;
    m_TestResults = rOther.m_TestResults;
}

} // end namespace DataLogging
