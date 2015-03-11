/****************************************************************************/
/*! \file DataLogging/Source/BaseLogger.cpp
 *
 *  \brief Implementation file for class BaseLogger.
 *
 *  \b Description:
 *      This class encapsulates the base functionality for all logger classes:
 *      close and open files for writing, append a line to a file, etc.
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

#include <DataLogging/Include/BaseLogger.h>
#include <DataLogging/Include/DataLoggingEventCodes.h>
#include <Global/Include/EventObject.h>
#include <Global/Include/Utils.h>
#include <unistd.h>
#include <QTimer>
namespace DataLogging {

/****************************************************************************/
BaseLogger::BaseLogger(QObject *pParent, const QString & LoggingSource, int FormatVersion) :
    QObject(pParent),
    m_LoggingSource(LoggingSource),
    m_FormatVersion(FormatVersion),
    m_FlushEventCount(0),
    m_WriteFileEventCount(0),
    m_LogFileError(false),
    m_IsEventRepeated(false),
    m_RequiredToFlush(false) {
	Q_UNUSED(pParent);
}

/****************************************************************************/
void BaseLogger::CreateNewFile(const QString &FileName) {
    // close old file
    CloseFile();
    // set new file name
    m_File.setFileName(FileName);
    // and try to open
    if(!m_File.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_FILE_CREATE, Global::FmtArgs() << FileName, true);
    }
    m_LogFileError = false;
    m_WriteFileEventCount = 0;
    m_FlushEventCount = 0;
    m_IsEventRepeated = false;
    m_RequiredToFlush = false;
    // flush the data if data is written to file
//    QTimer::singleShot(10000, this, SLOT(FlushToDisk()));

    // do not log creation of file in here, since it may be the event log file and
    // the header is not created at this point!
}

/****************************************************************************/
void BaseLogger::RemoveFile(const QString &FileName) const {
    // if the file is not able to remove then raise the event
    if(!QFile::remove(FileName)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_FILE_REMOVE, Global::FmtArgs() << FileName, true);
    }
}

/****************************************************************************/
void BaseLogger::OpenFileForAppend(const QString &FileName) {
    // first of all close it
    CloseFile();
    // set new file name
    m_File.setFileName(FileName);
    // and try to open
    if(!m_File.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        // check if the event log file name is "Events_Tmp.log then raise an error and don't log anything
        if (FileName.contains(EVENTLOG_TEMP_FILE_NAME_SUFFIX)) {
            m_LogFileError = true;
            // reset all the data because logging is disabled completely except log file error
            m_IsEventRepeated = false;
            m_WriteFileEventCount = 0;
            m_FlushEventCount = 0;
            // don't log the data - disable permanently
            Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_DATA_LOGGING_DISABLED);
            return;
        }
        else {
            Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_FILE_NOT_OPEN, Global::FmtArgs() << FileName, true);
        }
    }
    m_LogFileError = false;
    m_WriteFileEventCount = 0;
    m_FlushEventCount = 0;
    m_IsEventRepeated = false;
    m_RequiredToFlush = false;
}

/****************************************************************************/
void BaseLogger::AppendLine(QString Line, bool FlushData) {
    // log file is having some problem in writing the data
    if (IsLogFileError()) {                
        return;
    }
    if(!IsLogFileOpen()) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_FILE_NOT_OPEN,
                                                   Global::FmtArgs() << m_File.fileName(), true);
        return;
    }

    // append newline
    Line += "\n";

    qint64 WrittenSize = m_File.write(Line.toUtf8());
    int WriteSize = Line.toUtf8().size();
    m_RequiredToFlush = true;
    // check if all data was written
    if(WrittenSize != WriteSize) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_FILE_WRITE,
                                                   Global::FmtArgs() << m_File.fileName(), true);
        m_WriteFileEventCount++;
        // check the event repeat count
        if (m_WriteFileEventCount > 2) {
            CheckEventRepeatingForTempFile();
        }
    }
    else {
        // reset the counter
        m_WriteFileEventCount = 0;
    }
    // for warnings and errors data shall be flushed immediately
    if (FlushData) {
        FlushToDisk();
    }
}

/****************************************************************************/
void BaseLogger::FlushToDisk() {
    if (m_RequiredToFlush) {
        // now flush data to disk
        if(!m_File.flush() || fsync(m_File.handle()) == -1) {
            Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_FILE_FLUSH,
                                                       Global::FmtArgs() << m_File.fileName(), true);
            m_FlushEventCount++;
            // check the event repeat count
            if (m_FlushEventCount > 2) {
                CheckEventRepeatingForTempFile();
            }
        }
        else {
            // reset the counter
            m_FlushEventCount = 0;
        }
    }
}


/****************************************************************************/
void BaseLogger::CheckEventRepeatingForTempFile() {
    m_IsEventRepeated = true;
    // check if the event log file name contains "Events_Tmp.log then raise an error and don't log anything
    if (m_File.fileName().contains(EVENTLOG_TEMP_FILE_NAME_SUFFIX)) {
        m_LogFileError = true;
        // if file is opened then close it
        if (m_File.isOpen()) {
            CloseFile();
        }
        m_IsEventRepeated = false;
        m_FlushEventCount = 0;
        m_WriteFileEventCount = 0;
        // don't log the data - disable permanently
        Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_DATA_LOGGING_DISABLED);
    }
}

} // end namespace DataLogging
