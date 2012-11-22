/****************************************************************************/
/*! \file DataLogging/Source/ComponentTestLoggerConfig.cpp
 *
 *  \brief Implementation file for class ComponentTestLoggerConfig.
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

#include <DataLogging/Include/ComponentTestLoggerConfig.h>

namespace DataLogging {

/****************************************************************************/
ComponentTestLoggerConfig::ComponentTestLoggerConfig() {
}

/****************************************************************************/
ComponentTestLoggerConfig::ComponentTestLoggerConfig(const QString & OperatingMode, const QString &Path) :
    m_OperatingMode(OperatingMode),
    m_Path(Path)
{
}

/****************************************************************************/
ComponentTestLoggerConfig::ComponentTestLoggerConfig(const ComponentTestLoggerConfig &rOther) {
    CopyFrom(rOther);
}

/****************************************************************************/
ComponentTestLoggerConfig::~ComponentTestLoggerConfig() {
}

/****************************************************************************/
void ComponentTestLoggerConfig::CopyFrom(const ComponentTestLoggerConfig &rOther) {
    m_OperatingMode = rOther.m_OperatingMode;
    m_Path = rOther.m_Path;
}

/****************************************************************************/
const ComponentTestLoggerConfig & ComponentTestLoggerConfig::operator = (const ComponentTestLoggerConfig &rOther) {
    if(&rOther != this) {
        CopyFrom(rOther);
    }
    return *this;
}

} // end namespace DataLogging
