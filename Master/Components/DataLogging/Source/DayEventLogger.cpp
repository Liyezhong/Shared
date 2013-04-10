/****************************************************************************/
/*! \file DataLogging/Source/DayEventLogger.cpp
 *
 *  \brief Implementation file for class DayEventLogger.
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

#include <DataLogging/Include/DayEventLogger.h>
#include <DataLogging/Include/DataLoggingEventCodes.h>
#include <Global/Include/EventTranslator.h>
#include <Global/Include/Utils.h>
#include <Global/Include/EventObject.h>

#include <QDirIterator>
#include <QDebug>

namespace DataLogging {

static const int DAYEVENTLOGGER_FORMAT_VERSION = 1;     ///< Format version.
const QString EVENTLOG_TEMP_FILE_NAME = "Himalaya_Events_Tmp.log"; ///< Event log temporary file name

/****************************************************************************/
DayEventLogger::DayEventLogger(Global::EventObject *pParent, const QString & TheLoggingSource, const QString& fileNamePrefix)
    : BaseLoggerReusable(pParent, TheLoggingSource, DAYEVENTLOGGER_FORMAT_VERSION)
    , m_MaxFileCount(0)
    , m_FileNamePrefix(fileNamePrefix)  // e.g. 'HimalayaEvents_'
{
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
    // OK, now switch to new file making a backup if necessary
    SwitchToFile(FileName, true);
}



/****************************************************************************/
void DayEventLogger::Log(const DayEventEntry &Entry) {

        // translate event type
        quint32 IDStrEvtType = Global::EVENT_GLOBAL_UNKNOWN_STRING_ID;
        const Global::EventType EventType = Entry.GetEventType();
        switch(EventType) {
            case Global::EVTTYPE_UNDEFINED:     IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_UNDEFINED; break;
            case Global::EVTTYPE_FATAL_ERROR:
            case Global::EVTTYPE_SYS_ERROR:
                                                IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_FATAL_ERROR; break;
            case Global::EVTTYPE_ERROR:         IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_ERROR; break;
            case Global::EVTTYPE_WARNING:
            case Global::EVTTYPE_SYS_WARNING:                                                
                                                IDStrEvtType = Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_WARNING; break;
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
    qDebug()<<"\n\n\nUse Alternate Strings" << UseAltEventString;
        // translate message
    QString TrEventMessage  = Global::EventTranslator::TranslatorInstance().Translate(Global::TranslatableString(Entry.GetEventCode(), Entry.GetString()),
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
            TrEventMessage = "Event Acknowledged by the User:" + TrEventMessage;  // todo: add translated string as prefix instead (Mantis 3674)
        }

        if (Entry.IsPostProcess())
        {
            TrEventMessage = "PostProcess: " + TrEventMessage;  // todo: add translated string as prefix instead (Mantis 3674)
         }

        QString AlternateString = UseAltEventString ? "true" : "false";
        QString ParameterString = "";
        foreach (Global::TranslatableString s, Entry.GetString())
        {
            ParameterString += s.GetString() +";";
        }

        QString LoggingString = TimeStampToString(Entry.GetTimeStamp()) + ";" +
                                QString::number(Entry.GetEventCode(), 10) + ";" +
                                TrEventType + ";" +
                                TrEventMessage + ";" + //Message
                                QString::number(AsInt(Entry.GetLogAuthorityType()), 10) + ";";//log level for show in GUI

        if (NetCommands::No_Set != Entry.GetAckValue())
        {
            LoggingString = LoggingString + "AckSel" + QString::number(Entry.GetAckValue(), 10) + ";";//which process options in the msg box the user has selected
        }

        if(!ParameterString.isEmpty())
        {
             LoggingString = LoggingString + ParameterString + "\n";
        }

        // check if we must printout to console (because we sent it to the data logger
        // and we have to avoid a ping pong of error messages)
        Global::EventSourceType SourceType = Entry.GetSourceComponent();
        if(( SourceType == Global::EVENTSOURCE_DATALOGGER) &&
            ((Entry.GetEventType() == Global::EVTTYPE_FATAL_ERROR) || (Entry.GetEventType() == Global::EVTTYPE_ERROR)))
        {
            // we sent this error message.

            // Put it to console since we could not write at that time.
            Global::ToConsole(LoggingString);
        }
        else
        {
            // check if date changed
            QDate Now = Global::AdjustedTime::Instance().GetCurrentDate();
            if(m_LastLogDate != Now)
            {
                m_LastLogDateBackUp = m_LastLogDate;
                // remember new date
                m_LastLogDate = Now;
                // switch to new file
                SwitchToNewFile();
            }
        // check if file ready for logging - usually this log file open at the
        // start of the Data Logging component now this causes the recursive call
        // so create the temporay file to log the data
        if(!IsLogFileOpen() && !IsLogFileError())
            {
            // compute new file name
            QDir Dir(GetPath());
            QString CompleteFileName(QDir::cleanPath(Dir.absoluteFilePath(EVENTLOG_TEMP_FILE_NAME)));
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
            // append data to file and flush
            //qDebug() << "DayEventLogger::Log" << LoggingString;
            AppendLine(LoggingString);
        }

}

/****************************************************************************/
void DayEventLogger::Configure(const DayEventLoggerConfig &Config) {

        // save configuration
        SetConfiguration(Config.GetOperatingMode(), Config.GetSerialNumber(), Config.GetPath());
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
void DayEventLogger::CheckLoggingEnabled() {

    if (IsLogFileError()) {
        // don't log the data - disable permanently
        Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_DATA_LOGGING_DISABLED);
    }

}

} // end namespace DataLogging
