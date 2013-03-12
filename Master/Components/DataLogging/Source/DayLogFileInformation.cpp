/****************************************************************************/
/*! \file DataLogging/Source/DayLogFileInformation.cpp
 *
 *  \brief Implementation file for class DayLogFileInformation.
 *
 *  \b Description:
 *          This class creates the Daily run log files in the file system for the
 *          Export component and creates the Daily run log file stream in memory
 *          for the GUI component and also send the file names to the GUI.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
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

#include <QFile>
#include <QDir>
#include <QXmlStreamReader>
#include <QDebug>
#include <QProcess>
#include "DataLogging/Include/DataLoggingEventCodes.h"
#include "DataLogging/Include/DayLogFileInformation.h"
#include "Global/Include/EventObject.h"
#include "Global/Include/Translator.h"
#include "Global/Include/UITranslator.h"
#include "Global/Include/EventTranslator.h"

namespace DataLogging {

const char DELIMITER_SEMICOLON              = ';';///< Delimiter for semicolon
const char DELIMITER_UNDERSCORE             = '_';///< Delimiter for underscore
const QString FLAG_VALUE                    = "true";///< user log flag value
const QString EMPTY_STRING                  = "";///< empty string
const QString STRING_NEWLINE                = "\n";///< new line
const QString STRING_SEMICOLON              = ";";///< string for semicolon
const QString TRANSLATE_RETURN_VALUE_1      = "\"0\":";///< translator return value
const QString EVENTLOGFILE_STARTING_NAME    = "HimalayaEvents_";///< Start name of event log file
const QString DAILYRUNLOG_FILE_FIRSTLINE    = "FileName: DailyRunLog_";///< Filrst line of the DailyRunLog file
const QString DAILYRUNLOG_FILENAME          = "DailyRunLog_";///< DailyRunLog file name
const QString FILEEXTENSION_LOG             = ".log";///< Extension for the log files
const QString STRING_DAILYRUN               = "DailyRun";///< String for DailyRunLog
const QString HIMALAYAEVENTS_MULTIPLEFILES  = "HimalayaEvents_*.log";///< Multiple log files string
const QString COMMAND_RMDIR                 = "rmdir "; ///< constant string for the command 'rmdir'
const QString COMMAND_ARGUMENT_C            = "-c"; ///< constant string for the command argument for shell '-c'
const QString COMMAND_ARGUMENT_R            = "-rf"; ///< constant string for the command argument for recursive files '-r'
const QString COMMAND_RM                    = "rm "; ///< constant string for the command 'rm'
// constants for wildcharacters
const QString WILDCHAR_ASTRIK               = "*"; ///< constant for wild char
const QString DIRECTORY_SH                  = "/bin/sh"; ///< constant for the shell directory
const QString STRING_SPACE                  = " "; ///< constant string for space

const qint32 EVENTSTRING_TIMESTAMP          = 0; ///< event log timestamp number if the event is splitted
const qint32 EVENTSTRING_EVENTID            = 1; ///< event log event ID number if the event is splitted
const qint32 EVENTSTRING_EVENTTYPE          = 2; ///< event log event type number if the event is splitted
const qint32 EVENTSTRING_EVENTSTRING        = 3; ///< event log event string number if the event is splitted
const qint32 EVENTSTRING_USERLOG            = 4; ///< event log user log flag number if the event is splitted
const qint32 EVENTSTRING_ALTERNATETEXT      = 5; ///< event log alternate text flag number if the event is splitted
const qint32 EVENTSTRING_PARAMETERS         = 5; ///< event log parameters number if the event is splitted

/****************************************************************************/
DayLogFileInformation::DayLogFileInformation(QString FilePath) :
    m_LogFilePath(FilePath)
{
}

/****************************************************************************/
void DayLogFileInformation::ReadAndTranslateTheFile(const QString &FileName, Global::GuiUserLevel UserRole, const QByteArray &ByteArray) {

    QByteArray& FileData = const_cast<QByteArray&>(ByteArray);

    QFile LogFile(FileName);
    // check the file existence
    if (!LogFile.open(QIODevice::ReadOnly)) {
        return;
    }
    // read entire file
    while (!LogFile.atEnd()) {
        QString ReadData = LogFile.readLine();

        if (ReadData.contains(STRING_SEMICOLON)) {
            if (ReadData.split(DELIMITER_SEMICOLON).count() > EVENTSTRING_USERLOG) {
                //if (QString(ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_USERLOG)).compare(FLAG_VALUE) == 0) {
                bool acceptIt = false;
                QString authrityType = ReadData.split(DELIMITER_SEMICOLON).value(4);
                if (Global::OPERATOR == UserRole && authrityType.compare("1") == 0)
                    acceptIt = true;
                else if (Global::ADMIN == UserRole && (authrityType.compare("1") == 0
                                                       || authrityType.compare("2") == 0))
                    acceptIt = true;
                else if (Global::SERVICE == UserRole && authrityType.toInt() >= 1)
                    acceptIt = true;
                if (acceptIt) {
                    Global::tTranslatableStringList TranslateStringList;
                    // read all the parameters
                    for (int Counter = EVENTSTRING_PARAMETERS; Counter < ReadData.split(DELIMITER_SEMICOLON).count();
                         Counter++) {
                        if (QString(ReadData.split(DELIMITER_SEMICOLON).value(Counter)).compare(EMPTY_STRING) != 0 &&
                                QString(ReadData.split(DELIMITER_SEMICOLON).value(Counter)).
                                compare(STRING_NEWLINE) != 0) {
                            TranslateStringList << ReadData.split(DELIMITER_SEMICOLON).value(Counter);
                        }
                    }
//                    // used for alternate text
//                    bool UseAlternateText = false;
//                    if (QString(ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_ALTERNATETEXT)).
//                            compare(FLAG_VALUE) == 0) {
//                        UseAlternateText = true;
//                    }

                    // translate the data
                    QString EventData = Global::UITranslator::TranslatorInstance().Translate
                            (Global::TranslatableString(QString(ReadData.split(DELIMITER_SEMICOLON).value(1)).toInt(),
                                                        TranslateStringList)/*, UseAlternateText*/);

                    if (EventData.compare(EMPTY_STRING) == 0 || EventData.contains(TRANSLATE_RETURN_VALUE_1) ||
                          EventData.compare("\"" + QString(ReadData.split(DELIMITER_SEMICOLON).
                                                           value(EVENTSTRING_EVENTID)) +"\":") == 0) {
                        EventData = ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_EVENTSTRING);
//                        EventData = Global::EventTranslator::TranslatorInstance().Translate
//                                                    (Global::TranslatableString(QString(ReadData.split(DELIMITER_SEMICOLON).value(1)).toInt(),
//                                                                                TranslateStringList));
                        Global::EventObject::Instance().RaiseEvent
                                (EVENT_DATALOGGING_ERROR_EVENT_ID_NOT_EXISTS,
                                 Global::FmtArgs() << ReadData.split(DELIMITER_SEMICOLON).value
                                 (EVENTSTRING_EVENTID), true);
                    }
                    // join the required data
                    ReadData = QString(ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_TIMESTAMP))
                               + STRING_SEMICOLON +
                               QString(ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_EVENTID))
                               + STRING_SEMICOLON +
                               QString(ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_EVENTTYPE))
                               + STRING_SEMICOLON + EventData + STRING_NEWLINE;

                    FileData.append(ReadData);
                    FileData.append(STRING_NEWLINE);
                }
            }
        }
        else {
            if (ReadData != STRING_NEWLINE) {
                if (ReadData.contains(EVENTLOGFILE_STARTING_NAME)) {
                    ReadData = DAILYRUNLOG_FILE_FIRSTLINE + ReadData.split(DELIMITER_UNDERSCORE).value(2);
                }

                FileData.append(ReadData);

                FileData.append(STRING_NEWLINE);
            }
        }
    }
    LogFile.close();
}


/****************************************************************************/
void DayLogFileInformation::CreateAndListDailyRunLogFileName(const QStringList &FileNames) {
    // removes the constant cast
    QStringList& ListOfFile = const_cast<QStringList&>(FileNames);

    QDir LogDirectory(m_LogFilePath);
    // iterate each file and change the name to daily run log file
    foreach (QString LogFileName, LogDirectory.entryList(QStringList() << HIMALAYAEVENTS_MULTIPLEFILES)) {
        // get the date time value from the event log file name
        QString DateAndTimeValue = LogFileName.split(DELIMITER_UNDERSCORE).value
                (LogFileName.split(DELIMITER_UNDERSCORE).count() - 1);
        // replace the .log extension and put the empty string
        ListOfFile.append(DAILYRUNLOG_FILENAME + DateAndTimeValue.replace(FILEEXTENSION_LOG, EMPTY_STRING));
    }
    qSort(ListOfFile.begin(), ListOfFile.end(), qGreater<QString>());
}


/****************************************************************************/
void DayLogFileInformation::CreateSpecificDailyRunLogFile(const QString &FileName,
                                                          Global::GuiUserLevel UserRole,
                                   const QByteArray &FileContent) {

    QDir LogDirectory(m_LogFilePath);
    QByteArray& FileData = const_cast<QByteArray&>(FileContent);
    // iterate each file and find whcih event log file it is referring to it
    foreach (QString LogFileName, LogDirectory.entryList(QStringList() << HIMALAYAEVENTS_MULTIPLEFILES)) {
        if (LogFileName.contains(FileName.split(DELIMITER_UNDERSCORE).value
                                 (FileName.split(DELIMITER_UNDERSCORE).count() - 1))) {
            ReadAndTranslateTheFile(m_LogFilePath + QDir::separator() + LogFileName, UserRole, FileData);
            break;
        }
    }
}

/****************************************************************************/
void DayLogFileInformation::CreateDailyRunLogFiles(const QStringList &FileNames, Global::GuiUserLevel CurrenUserRole) {

    // removes the constant cast
    QStringList& ListOfFile = const_cast<QStringList&>(FileNames);
    // set the current directory as log files
    QDir LogDirectory(m_LogFilePath);

    // delete all the daily run log files before creating the new files
    // otherwise old daily run log files will also be copied
    QStringList Options;
    // -c option for shell and remove directory
    Options << COMMAND_ARGUMENT_C << COMMAND_RM + COMMAND_ARGUMENT_R + STRING_SPACE +
               COMMAND_RM + LogDirectory.absolutePath() + QDir::separator() + STRING_DAILYRUN + QDir::separator() + WILDCHAR_ASTRIK;
    // execute the process with "/bin/sh"
    // remove all the files from the "rm -rf DailyRun/*"
    if (QProcess::execute(DIRECTORY_SH, Options) >= 0) {
        (void)QProcess::execute(COMMAND_RMDIR + LogDirectory.absolutePath());  //to avoid lint-534
    }

    QProcess::execute("mkdir " + m_LogFilePath + QDir::separator() + STRING_DAILYRUN);

    QByteArray FileData;
    // iterate each file and find whcih event log file it is referring to it
    foreach (QString LogFileName, LogDirectory.entryList(QStringList() << HIMALAYAEVENTS_MULTIPLEFILES)) {
        FileData.clear();
        // get the date time value from the event log file name
        QString DateAndTimeValue = LogFileName.split(DELIMITER_UNDERSCORE).value
                (LogFileName.split(DELIMITER_UNDERSCORE).count() - 1);

        QString FullPatheOfTheFile = m_LogFilePath + QDir::separator() + STRING_DAILYRUN
                + QDir::separator() + DAILYRUNLOG_FILENAME + DateAndTimeValue;

        // replace the .log extension and put the empty string
        ListOfFile.append(FullPatheOfTheFile);
        // translate the files
        ReadAndTranslateTheFile(m_LogFilePath + QDir::separator() + LogFileName, CurrenUserRole, FileData);
        if (FileData.count() > 0) {
            // create the daily run log file
            QFile DailyRunLogFile(FullPatheOfTheFile);
            if (DailyRunLogFile.open(QIODevice::WriteOnly)) {
                qint64 LengthOfBytesWritten = 0;
                // write the data to the file
                while (LengthOfBytesWritten != FileData.count()) {
                   LengthOfBytesWritten = DailyRunLogFile.write(FileData.mid(LengthOfBytesWritten));
                }
            }
            else {
                Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_FILE_NOT_OPEN);
                return;
            }
            DailyRunLogFile.close();
        }
    }    
}
}
