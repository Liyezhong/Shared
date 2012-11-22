/****************************************************************************/
/*! \file DataLogging/Source/EventLogger.cpp
 *
 *  \brief Implementation file for class EventLogger.
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

#include <DataLogging/Include/EventLogger.h>
#include <DataLogging/Include/DataLoggingEventCodes.h>
#include <Global/Include/EventTranslator.h>
#include <Global/Include/Utils.h>

namespace DataLogging {

static const char * const FILE_SUFFIX_LOG       = "log";    ///< File extension for log files.
static const char * const FILE_SUFFIX_BACKUP    = "old";    ///< File extension for backup files.
static const int EVENTLOGGER_FORMAT_VERSION     = 1;        ///< Format version.

/****************************************************************************/
EventLogger::EventLogger(LoggingObject *pParent, const Global::LoggingSource &TheLoggingSource) :
    BaseLoggerReusable(pParent, TheLoggingSource, EVENTLOGGER_FORMAT_VERSION),
    m_MaxFileSize(0)
{
}

/****************************************************************************/
void EventLogger::Log(const EventEntry &Entry) {
    // create logging string from Entry

    // translate event type
    quint32 IDStrEvtType = Global::EVENT_GLOBAL_UNKNOWN_STRING_ID;
    switch(Entry.GetEventType()) {
        case Global::EVTTYPE_UNDEFINED:     IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_UNDEFINED; break;
        case Global::EVTTYPE_FATAL_ERROR:   IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_FATAL_ERROR; break;
        case Global::EVTTYPE_ERROR:         IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_ERROR; break;
        case Global::EVTTYPE_WARNING:       IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_WARNING; break;
        case Global::EVTTYPE_INFO:          IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_INFO; break;
        case Global::EVTTYPE_DEBUG:         IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_DEBUG; break;
        default:                            break;
    }
    QString TrEventType = Global::EventTranslator::TranslatorInstance().Translate(IDStrEvtType);

    // translate event status
    quint32 IDStrEvtStatus = Global::EVENT_GLOBAL_UNKNOWN_STRING_ID;
    switch(Entry.GetEventStatus()) {
        case Global::EVTSTAT_ON:    IDStrEvtStatus = Global::EVENT_GLOBAL_STRING_ID_EVTSTAT_ON; break;
        case Global::EVTSTAT_OFF:   IDStrEvtStatus = Global::EVENT_GLOBAL_STRING_ID_EVTSTAT_OFF; break;
        default:                    break;
    }
    QString TrEventStatus = Global::EventTranslator::TranslatorInstance().Translate(IDStrEvtStatus);

    // translate event message
    QString TrEventMessage = Global::EventTranslator::TranslatorInstance().Translate(Global::TranslatableString(Entry.GetEventCode(),
                                                                                                                Entry.GetAdditionalData()));

    // translate event source
    QString TrEventSource = Global::EventTranslator::TranslatorInstance().Translate(Entry.GetSource().GetSource());

    // assembly logging string
    QString LoggingString = TimeStampToString(Entry.GetTimeStamp()) + ";" +
                            TrEventSource + ";" +
                            TrEventType + ";" +
                            TrEventStatus + ";" +
                            QString::number(Entry.GetEventCode(), 10) + ";" +
                            TrEventMessage ;

    // now try to write logging string to log file
    try {
        // check if we must printout to console (because we sent it to the data logger
        // and we have to avoid a ping pong of error messages)
        if( ((Entry.GetSource()) == GetLoggingSource()) &&
            ((Entry.GetEventType() == Global::EVTTYPE_FATAL_ERROR) || (Entry.GetEventType() == Global::EVTTYPE_ERROR)))
        {
            // we sent this error message.

            // Put it to console since we could not write at that time.
            Global::ToConsole(LoggingString);
        } else {
            // we did not send it, so try to write to file.

            // check if file ready for logging
            if(!IsLogFileOpen()) {
                // file not open. throw according exception
                THROW(EVENT_DATALOGGING_ERROR_FILE_NOT_OPEN);
            }
            // check if new file would be larger than maximum allowed file size
            if((m_MaxFileSize > 0) && (GetFileSize() + LoggingString.size() > m_MaxFileSize)) {
                // file size after logging would exceed maximum allowed file size

                // get current file name
                QString FileName = GetCompleteFileName();
                // make backup of existing file
                BackupFile(FileName);
                // create new file and write header information
                CreateNewFile(FileName);
                // write header infromation
                WriteHeader();
                // trace that file was created.
                LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_DATALOGGING_INFO_FILE_CREATE, FileName,
                          Global::NO_NUMERIC_DATA, false);
            }
            // append data to file and flush
            AppendLine(LoggingString);
        }
    } catch(const Global::Exception &E) {
        // writing error failed. Output logging string to stdio since writing failed...
        Global::ToConsole(LoggingString);
        // and send error message. This will reach us eventually and will be filtered out and printed to console
        //SEND_EXCEPTION(E);
    } catch(...) {
        // writing error failed. Output logging string to stdio since writing failed...
        Global::ToConsole(LoggingString);
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    }
}

/****************************************************************************/
void EventLogger::Configure(const EventLoggerConfig &Config) {
    try {
        // save configuration
        SetConfiguration(Config.GetOperatingMode(), Config.GetSerialNumber(), Config.GetPath());
        m_EventLoggerBaseFileName = Config.GetEventLoggerBaseFileName();
        m_MaxFileSize = Config.GetMaxFileSize();

        // set file name
        QString NewFileName = m_EventLoggerBaseFileName;
        // append serial number and suffix
        NewFileName += "_";
        NewFileName += Config.GetSerialNumber();
        NewFileName += ".";
        NewFileName += FILE_SUFFIX_LOG;

        // Switch to new file making a backup if necessary
        SwitchToFile(NewFileName, true);
    } catch(const Global::Exception &E) {
        // send error message
        SEND_EXCEPTION(E);
    } catch(...) {
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    }
}

/****************************************************************************/
void EventLogger::BackupFile(const QString &FileName) {
    QFileInfo FileInfo(FileName);
    if(!FileInfo.exists()) {
        // file does not exist. throw according exception
        THROWARG(EVENT_DATALOGGING_ERROR_FILE_NOT_EXISTS, FileName);
    }
    // check if file suffix ok
    if(FileInfo.suffix() != FILE_SUFFIX_LOG) {
        // suffix not ok. throw according exception
        THROWARGS(EVENT_DATALOGGING_ERROR_FILE_WRONG_SUFFIX, Global::tTranslatableStringList() << FileName << FILE_SUFFIX_LOG);
    }
    // compute backup file name
    QString BackupFileName = FileName;
    // replace suffixes and only that! We can use strlen and
    BackupFileName.replace(BackupFileName.size() - strlen(FILE_SUFFIX_LOG), strlen(FILE_SUFFIX_LOG), FILE_SUFFIX_BACKUP);

    // remove old backup file if it exists
    if(QFile::exists(BackupFileName)) {
        if(!QFile::remove(BackupFileName)) {
            // removing file failed. throw according exception
            THROWARG(Global::EVENT_GLOBAL_ERROR_FILE_REMOVE, BackupFileName);
        }
    }
    // rename log file to backup file if it exists
    if(!QFile::rename(FileName, BackupFileName)) {
        // renaming file failed. throw according exception
        THROWARGS(Global::EVENT_GLOBAL_ERROR_FILE_RENAME, Global::tTranslatableStringList() << FileName << BackupFileName);
    }
}

} // end namespace DataLogging
