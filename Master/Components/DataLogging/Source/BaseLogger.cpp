/****************************************************************************/
/*! \file DataLogging/Source/BaseLogger.cpp
 *
 *  \brief Implementation file for class BaseLogger.
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

#include <DataLogging/Include/BaseLogger.h>
#include <DataLogging/Include/DataLoggingEventCodes.h>
#include <Global/Include/EventObject.h>

namespace DataLogging {

/****************************************************************************/
BaseLogger::BaseLogger(Global::EventObject *pParent, const QString & LoggingSource, int FormatVersion) :
    m_LoggingSource(LoggingSource),
    m_FormatVersion(FormatVersion)
{
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
        // file open failed. Clean file name.
        m_File.setFileName("");
        // throw according exception
        LOGANDTHROWARG(Global::EVENT_GLOBAL_ERROR_FILE_CREATE, FileName);
    }
    // do not log creation of file in here, since it may be the event log file and
    // the header is not created at this point!
}

/****************************************************************************/
void BaseLogger::RemoveFile(const QString &FileName) const {
    if(!QFile::remove(FileName)) {
        // removing file failed. throw according exception
        LOGANDTHROWARG(Global::EVENT_GLOBAL_ERROR_FILE_REMOVE, FileName);
    }
    // trace that file was removed.
    LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_DATALOGGING_INFO_FILE_REMOVE, FileName
              , Global::NO_NUMERIC_DATA, false);
}

/****************************************************************************/
void BaseLogger::OpenFileForAppend(const QString &FileName) {
    // first of all close it
    CloseFile();
    // set new file name
    m_File.setFileName(FileName);
    // and try to open
    if(!m_File.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        // file open failed. Clean file name.
        m_File.setFileName("");
        // throw according exception.
        LOGANDTHROWARG(Global::EVENT_GLOBAL_ERROR_FILE_OPEN, FileName);
    }
    // trace that file was opened.
    LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_DATALOGGING_INFO_FILE_OPEN, FileName
              , Global::NO_NUMERIC_DATA, false);
}

/****************************************************************************/
void BaseLogger::AppendLine(QString Line) {
    if(!IsLogFileOpen()) {
        // file not open. Throw according exception.
        LOGANDTHROW(EVENT_DATALOGGING_ERROR_FILE_NOT_OPEN);
    }

    // append newline
    Line += "\n";

    qint64 WrittenSize = m_File.write(Line.toUtf8());
    int WriteSize = Line.toUtf8().size();

    // check if all data was written
    if(WrittenSize != WriteSize) {
        // writing failed. throw according exception
        THROWARGS(Global::EVENT_GLOBAL_ERROR_FILE_WRITE, Global::tTranslatableStringList() << m_File.fileName() <<
                  QString::number(WrittenSize, 10) << QString::number(WriteSize, 10));
    }

    // now flush data to disk
    if(!m_File.flush()) {
        // flush failed. throw according exception
        LOGANDTHROWARG(Global::EVENT_GLOBAL_ERROR_FILE_FLUSH, m_File.fileName());
    }
}

} // end namespace DataLogging
