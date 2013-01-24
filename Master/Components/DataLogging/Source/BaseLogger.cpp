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
    m_FormatVersion(FormatVersion),
    m_LogFileError(false)
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
        Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_FILE_CREATE, Global::FmtArgs() << FileName, true);
    }
    m_LogFileError = false;
    // do not log creation of file in here, since it may be the event log file and
    // the header is not created at this point!
}

/****************************************************************************/
void BaseLogger::RemoveFile(const QString &FileName) const {
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
        // check if the event log file name is "Colorado_Events_Tmp.log then raise an error and don't log anything
        if (FileName.contains("Colorado_Events_Tmp.log")) {
            m_LogFileError = true;
            return;
        }
        else {
            Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_FILE_NOT_OPEN, Global::FmtArgs() << FileName, true);
        }
//        // file open failed. Clean file name.
//        m_File.setFileName("");
//        // throw according exception.
//        LOGANDTHROWARG(Global::EVENT_GLOBAL_ERROR_FILE_OPEN, FileName);
    }
    m_LogFileError = false;

}

/****************************************************************************/
void BaseLogger::AppendLine(QString Line) {
    // log file is having some problem i writing the data
    if (IsLogFileError()) {
        return;
    }
    if(!IsLogFileOpen()) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_FILE_NOT_OPEN, Global::FmtArgs() << m_File.fileName(), true);
        return;
    }

    // append newline
    Line += "\n";

    qint64 WrittenSize = m_File.write(Line.toUtf8());
    int WriteSize = Line.toUtf8().size();

    // check if all data was written
    if(WrittenSize != WriteSize) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_FILE_WRITE, Global::FmtArgs() << m_File.fileName(), true);
//        // writing failed. throw according exception
//        THROWARGS(Global::EVENT_GLOBAL_ERROR_FILE_WRITE, Global::tTranslatableStringList() << m_File.fileName() <<
//                  QString::number(WrittenSize, 10) << QString::number(WriteSize, 10));
    }
    // now flush data to disk
    if(!m_File.flush()) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_FILE_FLUSH, Global::FmtArgs() << m_File.fileName(), true);
    }
}

} // end namespace DataLogging
