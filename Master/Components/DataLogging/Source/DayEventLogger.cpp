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

#include <QDirIterator>
#include <QDebug>

namespace DataLogging {

static const int DAYEVENTLOGGER_FORMAT_VERSION = 1;     ///< Format version.

/****************************************************************************/
DayEventLogger::DayEventLogger(Global::EventObject *pParent, const QString & TheLoggingSource, const QString& fileNamePrefix)
    : BaseLoggerReusable(pParent, TheLoggingSource, DAYEVENTLOGGER_FORMAT_VERSION)
    , m_MaxFileCount(0)
    , m_FileNamePrefix(fileNamePrefix)  // e.g. 'ColoradoEvents_'
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
    try {

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


        // translate message
        QString TrEventMessage  = Global::EventTranslator::TranslatorInstance().Translate(Global::TranslatableString(Entry.GetEventCode(), Entry.GetString()));
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
            TrEventMessage = "Resolved:" + TrEventMessage;  // todo: add translated string as prefix instead (Mantis 3674)
        }

        QString ShowInRunLog = Entry.GetShowInRunLogStatus() ? "true" : "false";

        QString ParameterString = "";
        foreach (Global::TranslatableString s, Entry.GetString())
        {
            ParameterString += s.GetString() +";";
        }

        QString LoggingString = TimeStampToString(Entry.GetTimeStamp()) + ";" +
                                QString::number(Entry.GetEventCode(), 10) + ";" +
                                TrEventType + ";" +
                                TrEventMessage + ";" +
                                ShowInRunLog + ";" +
                                ParameterString + "\n";

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
            // check if file ready for logging
            if(!IsLogFileOpen())
            {
                // file not open. throw according exception
                LOGANDTHROW(EVENT_DATALOGGING_ERROR_FILE_NOT_OPEN);
            }
            // append data to file and flush
            //qDebug() << "DayEventLogger::Log" << LoggingString;
            AppendLine(LoggingString);
        }

    } catch(const Global::Exception &E) {
        qDebug() << "DayEventLogger::Log";
        // send error message
        //LOG_EVENT(Global::EVTTYPE_ERROR, Global::LOG_ENABLED,);
    } catch(...) {
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    }
}

/****************************************************************************/
void DayEventLogger::Configure(const DayEventLoggerConfig &Config) {
    try {
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
    } catch(const Global::Exception &E) {
        // send error message
        Global::EventObject::Instance().RaiseException(E);
    } catch(...) {
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    }
}

} // end namespace DataLogging
