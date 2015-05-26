/****************************************************************************/
/*! \file DataLogging/Source/DayEventLogger.cpp
 *
 *  \brief Implementation file for class DayEventLogger.
 *
 *\b Description:
 *      Day operation data is translated into the right language (depending on the
 *      operating mode) and written to file.
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

#include <DataLogging/Include/DayEventLogger.h>
#include <DataLogging/Include/DataLoggingEventCodes.h>
#include <Global/Include/EventTranslator.h>
#include <Global/Include/Utils.h>
#include <Global/Include/EventObject.h>

#include <QDirIterator>
#include <QDebug>

namespace DataLogging {

static const int DAYEVENTLOGGER_FORMAT_VERSION = 1;     ///< Format version.

/****************************************************************************/
DayEventLogger::DayEventLogger(QObject *pParent, const QString & TheLoggingSource,
                               const QString& FileNamePrefix)
    : BaseLoggerReusable(pParent, TheLoggingSource, DAYEVENTLOGGER_FORMAT_VERSION)
    , m_MaxFileCount(0)
    , m_FileNamePrefix(FileNamePrefix)  /*e.g. 'Leica_ST_'*/ {
    m_FlushImmediately = false;

}

/****************************************************************************/
QString DayEventLogger::ComputeFileName() const {
    return m_FileNamePrefix + GetSerialNumber() + "_" + GetTimeStampFileName() + ".log";
}

/****************************************************************************/
QString DayEventLogger::ComputeFileNameRegExp() const {
    return m_FileNamePrefix + GetSerialNumber() + "_\?\?\?\?\?\?\?\?.log";
}

/****************************************************************************/
void DayEventLogger::SwitchToNewFile() {
    // check if old files exits and have to be deleted

    // compute file name depending on current date
    QString FileName = ComputeFileName();

    if(m_MaxFileCount != 0) {
        // search for existing files
        QString FileRegExp = ComputeFileNameRegExp();
        // sort in reversed order, so we can delete last entries
        QDir Dir(GetPath(), FileRegExp, QDir::Reversed | QDir::Name, QDir::Files | QDir::CaseSensitive);
        QStringList FileNames = Dir.entryList();
        if (!FileNames.contains(FileName) && (m_MaxFileCount <= FileNames.size())) {
            RemoveFile(Dir.absoluteFilePath(FileNames[m_MaxFileCount -1]));
        }
    }
    // OK, now switch to new file
    SwitchToFile(FileName);
}

/****************************************************************************/
void DayEventLogger::RemoveOutdatedFiles(QString Prefix, quint8 DaysBack)
{
    QDir Dir(QDir::currentPath(), Prefix + "*", QDir::Reversed | QDir::Name, QDir::Files | QDir::CaseSensitive);
    QStringList FileNames = Dir.entryList();

    QDateTime DateTime = QDateTime::currentDateTime();
    QStringList AllowedFiles;
    for (int i = 0; i < DaysBack; i++) {      // preserve files for n-1 days
        QString FileName = Prefix;
        FileName += DateTime.toString().replace(" ", "_").replace(":", "_");
        AllowedFiles.append(FileName.left(Prefix.length() + 10));
        DateTime = DateTime.addDays(-1);
    }

    foreach (QString FileName, FileNames) {
        if (!AllowedFiles.contains(FileName.left(Prefix.length() + 10))) {
            (void)QFile::remove(QDir::currentPath() + "/" + FileName);
        }
    }
}

/****************************************************************************/
void DayEventLogger::Log(const DayEventEntry &Entry, QString &message) {

    // translate event type
    quint32 IDStrEvtType = Global::EVENT_GLOBAL_UNKNOWN_STRING_ID;
    const Global::EventType EventType = Entry.GetEventType();
    switch(EventType) {
        case Global::EVTTYPE_UNDEFINED:     IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_UNDEFINED; break;
        case Global::EVTTYPE_FATAL_ERROR:   IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_FATAL_ERROR; break;
        case Global::EVTTYPE_ERROR:         IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_ERROR; break;
        case Global::EVTTYPE_WARNING:       IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_WARNING; break;
        case Global::EVTTYPE_INFO:          IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_INFO; break;
        case Global::EVTTYPE_DEBUG:         IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_DEBUG; break;
        default:                            break;
    }
    QString TrEventType = Global::EventTranslator::TranslatorInstance().Translate(IDStrEvtType);

    const Global::AlternateEventStringUsage AltStringUsage = Entry.GetAltStringUsageType();
    bool UseAltEventString = false;
    if (AltStringUsage == Global::LOGGING ||
        AltStringUsage == Global::GUI_MSG_BOX_AND_USER_RESPONSE ||
        AltStringUsage == Global::GUI_MSG_BOX_AND_LOGGING ||
        AltStringUsage == Global::GUI_MSG_BOX_LOGGING_AND_USER_RESPONSE ||
        AltStringUsage == Global::USER_RESPONSE) {
            UseAltEventString = true;
    }
    // translate message
    QString TrEventMessage  = Global::EventTranslator::TranslatorInstance().Translate(Global::TranslatableString(Entry.GetStringID(), Entry.GetString()),
                                                                                      UseAltEventString);

    if (TrEventMessage.length() == 0)
    {
        TrEventMessage = Entry.GetEventName();
        if (Entry.GetString().count() > 0)
        {
            TrEventMessage += " (";
        }
        foreach (Global::TranslatableString s, Entry.GetString())
        {
            TrEventMessage += s.GetString() + " ";
        }
        if (Entry.GetString().count() > 0)
        {
            TrEventMessage += ")";
        }
    }
    if (!Entry.IsEventActive())
    {
        TrEventMessage = Global::EventTranslator::TranslatorInstance().Translate(Global::EVENT_GLOBAL_STRING_ID_RESOLVED) + TrEventMessage;
    }

    QString AcknowledgeString = "";
    if (Entry.GetAckValue() != NetCommands::NOT_SPECIFIED)
    {
        quint32 ButtonEventName = EVENT_USER_ACK_BUTTON_OK;
        switch (Entry.GetAckValue()) {
            case NetCommands::OK_BUTTON:
            ButtonEventName = EVENT_USER_ACK_BUTTON_OK;
            break;
            case NetCommands::CANCEL_BUTTON:
            ButtonEventName = EVENT_USER_ACK_BUTTON_CANCEL;
            break;
            case NetCommands::CONTINUE_BUTTON:
            ButtonEventName = EVENT_USER_ACK_BUTTON_CONTINUE;
            break;
            case NetCommands::STOP_BUTTON:
            ButtonEventName = EVENT_USER_ACK_BUTTON_STOP;
            break;
            case NetCommands::YES_BUTTON:
            ButtonEventName = EVENT_USER_ACK_BUTTON_YES;
            break;
            case NetCommands::NO_BUTTON:
            ButtonEventName = EVENT_USER_ACK_BUTTON_NO;
            break;
        default:
            break;
        }

        TrEventMessage = Global::EventTranslator::TranslatorInstance().Translate(Global::EVENT_GLOBAL_STRING_ID_ACKNOWLEDGED)
                + " "
                + Global::EventTranslator::TranslatorInstance().Translate(ButtonEventName)
                + ": "
                + TrEventMessage;
    }

    QString ShowInRunLog = Entry.GetShowInRunLogStatus() ? "true" : "false";
    QString AlternateString = UseAltEventString ? "true" : "false";

    QString ParameterString = "";
    foreach (Global::TranslatableString s, Entry.GetString())
    {
        ParameterString += s.GetString() +";";
    }

    QString LoggingString = TimeStampToString(Entry.GetTimeStamp()) + ";" +
                            QString::number(Entry.GetEventCode(), 10) + ";" +
                            TrEventType + ";" +
                            QString::number(Entry.GetStringID(),10) + ";" +
                            TrEventMessage + ";" +
                            ShowInRunLog + ";" +
                            AlternateString + ";" +
                            ParameterString + ";" +
                            AcknowledgeString;

    if(EventType == Global::EVTTYPE_DEBUG){
        LoggingString = TimeStampToString(Entry.GetTimeStamp()) + ";" +
                        QString::number(Entry.GetEventCode(), 10) + ";" +
                        TrEventType + ";" +
                        QString::number(Entry.GetStringID(),10) + ";" +
                        TrEventMessage;
    }

    message = TrEventMessage;

    // check if we must printout to console (because we sent it to the data logger
    // and we have to avoid a ping pong of error messages)
    Global::EventSourceType SourceType = Entry.GetEventSource();
    if(( SourceType == Global::EVENTSOURCE_DATALOGGER) &&
        ((Entry.GetEventType() == Global::EVTTYPE_FATAL_ERROR) || (Entry.GetEventType() == Global::EVTTYPE_ERROR)))
    {
        // Put it to console since we could not write at that time.
        Global::ToConsole(LoggingString);
    }
    else
    {
        (void)CheckNewFile();

        /// check if file ready for logging - usually log file opens at the
        /// start of the Data Logging component. Suppose if the system unable to write
        /// the file to the file system or file error
        /// then try to create the temporary log file to log the data
        if((!IsLogFileOpen() && !IsLogFileError()) || IsEventRepeated())
        {
            CreateTemporaryLogFile();
        }
        /// m_FlushImmediately flag sets true then thread (Means who uses data logging)
        /// wants to flush the data immediately, so need not require to change the data
        bool FlushData = m_FlushImmediately;
        if (!FlushData) {
            FlushData = (EventType != Global::EVTTYPE_INFO ? true : false);
        }

        // append data to file and flush
        //qDebug() << "DayEventLogger::Log" << LoggingString;
        AppendLine(LoggingString, FlushData);
    }

}

/****************************************************************************/
void DayEventLogger::Configure(const DayEventLoggerConfig &Config) {

    // save configuration
    SetConfiguration(Config.GetOperatingMode(), Config.GetSerialNumber(), Config.GetSWVersion(), Config.GetPath());
    m_MaxFileCount = Config.GetMaxFileCount();
    // Time is stored to overcome the problem created when the number of files
    // have reached max count and SwitchToNew file removes the oldest file even
    // though the date is same
    m_LastLogDate = Global::AdjustedTime::Instance().GetCurrentDate();
    m_LastLogDateBackUp = m_LastLogDate;
    m_FileNamePrefix = Config.GetBaseFileName();
    // switch to new file
    SwitchToNewFile();

}

/****************************************************************************/
bool DayEventLogger::CheckNewFile()
{
    // check if date changed
    bool ret = false;
    QDate Now = Global::AdjustedTime::Instance().GetCurrentDate();
    if(m_LastLogDate != Now)
    {
        m_LastLogDateBackUp = m_LastLogDate;
        // remember new date
        m_LastLogDate = Now;
        // switch to new file
        SwitchToNewFile();
        ret = true;
    }
    return ret;
}

/****************************************************************************/
void DayEventLogger::FlushDataToFile(bool Enable) {
    m_FlushImmediately = Enable;
}

/****************************************************************************/
void DayEventLogger::CheckLoggingEnabled() {

    if (IsLogFileError()) {
        // don't log the data - disable permanently
        Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_DATA_LOGGING_DISABLED);
    }

}

/****************************************************************************/
void DayEventLogger::CreateTemporaryLogFile() {

    // compute new file name
    QDir Dir(GetPath());
    QString CompleteFileName(QDir::cleanPath(Dir.absoluteFilePath(m_FileNamePrefix + EVENTLOG_TEMP_FILE_NAME_SUFFIX)));
    bool WriteHeaderInFile = true;
    // check the file existence
    if (QFile::exists(CompleteFileName)) {
        WriteHeaderInFile = false;
    }
    //always try to create the file in append mode
    OpenFileForAppend(CompleteFileName);
    if (WriteHeaderInFile) {
        // write the header of the file
        WriteHeader();
    }

}


} // end namespace DataLogging
