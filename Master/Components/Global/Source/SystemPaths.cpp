/****************************************************************************/
/*! \file Global/Source/SystemPaths.cpp
 *
 *  \brief Implementation file for class SystemPaths.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-01-19
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

#include <Global/Include/SystemPaths.h>

#include <QReadLocker>
#include <QWriteLocker>

namespace Global {

SystemPaths SystemPaths::m_InstA;

/****************************************************************************/
SystemPaths::SystemPaths() :
        m_SettingsPath(""),
        m_FirmwarePath(""),
        m_UpdatePath(""),
        m_LogfilesPath(""),
        m_ComponentTestPath(""),
        m_ManualPath("")
{
}

/****************************************************************************/
SystemPaths::~SystemPaths() {
}



} // end namespace Global
