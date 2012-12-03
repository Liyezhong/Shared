/****************************************************************************/
/*! \file DataLogging/Source/EventObject.cpp
 *
 *  \brief Implementation file for class EventObject.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-10-10
 *  $Author:    $ N.Kamath
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

#include <Global/Include/EventObject.h>
#include <Global/Include/Utils.h>
#include <Global/Include/SystemPaths.h>

#include <QMetaType>
namespace Global {

EventObject EventObject::m_EventObjectInstance; //!< The instance

/****************************************************************************/
EventObject::EventObject() :
    QObject(NULL)
{
    qRegisterMetaType<Global::tTranslatableStringList>("Global::tTranslatableStringList");
}


void EventObject::LogToTemporaryFile(quint32 ErrorCode) {
    m_TemporaryLogFile.setFileName(Global::SystemPaths::Instance().GetLogfilesPath() + "/Logfile.tmp");
    CreateNewFile();
    (void)m_TemporaryLogFile.write(QString("Internal Software Error-Initialization failed with Error Code: %1").arg(ErrorCode).toUtf8());
    (void)m_TemporaryLogFile.flush();
    CloseFile();
}

void EventObject::CreateNewFile() {
    // close old file
    CloseFile();
    // and try to open
    if(!m_TemporaryLogFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        Global::ToConsole("Temporary Log file creation failed.");
        // file open failed. Clean file name.
        m_TemporaryLogFile.setFileName("");
    }
}

} // end namespace EventHandling
