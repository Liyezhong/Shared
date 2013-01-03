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

/****************************************************************************/
DayLogFileInformation::DayLogFileInformation(QString FilePath) :
    m_LogFilePath(FilePath)
{
}

/****************************************************************************/
void DayLogFileInformation::ReadAndTranslateTheFile(const QString &FileName, const QByteArray &ByteArray) {

    QByteArray& FileData = const_cast<QByteArray&>(ByteArray);

    QFile LogFile(FileName);
    if (!LogFile.open(QIODevice::ReadOnly)) {
        return;
    }

    while (!LogFile.atEnd()) {
        QString ReadData = LogFile.readLine();

        if (ReadData.contains(";")) {
            if (ReadData.split(';').count() > 4) {
                if (QString(ReadData.split(';').value(4)).compare("true") == 0) {

                    Global::tTranslatableStringList TranslateStringList;
                    // read all the parameters
                    for (int Counter = 5; Counter < ReadData.split(';').count(); Counter++) {
                        if (QString(ReadData.split(';').value(Counter)).compare("") != 0 &&
                                QString(ReadData.split(';').value(Counter)).compare("\n") != 0) {
                            TranslateStringList << ReadData.split(';').value(Counter);
                        }
                    }

                    // translate the data
                    QString EventData = Global::UITranslator::TranslatorInstance().Translate
                            (Global::TranslatableString(QString(ReadData.split(';').value(1)).toInt(), TranslateStringList));

                    if (!(EventData.compare(ReadData.split(';').value(3)) != 0 && EventData.compare("") != 0)) {
                        EventData = Global::EventTranslator::TranslatorInstance().Translate
                                                    (Global::TranslatableString(QString(ReadData.split(';').value(1)).toInt(),
                                                                                TranslateStringList));
                        Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_EVENT_ID_NOT_EXISTS,
                                                                   Global::FmtArgs() << ReadData.split(';').value(1), true);
                    }
                    // join the required data
                    ReadData = QString(ReadData.split(';').value(0)) + ";" + QString(ReadData.split(';').value(1)) + ";" +
                               QString(ReadData.split(';').value(2)) + ";" + EventData + "\n";

                    FileData.append(ReadData);
                    FileData.append("\n");
                }
            }
        }
        else {
            if (ReadData != "\n") {
                if (ReadData.contains("ColoradoEvents_")) {
                    ReadData = "FileName: DailyRunLog_" + ReadData.split('_').value(2);
                }

                FileData.append(ReadData);

                FileData.append("\n");
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
    foreach (QString LogFileName, LogDirectory.entryList(QStringList() << "ColoradoEvents_*.log")) {
        // get the date time value from the event log file name
        QString DateAndTimeValue = LogFileName.split('_').value(LogFileName.split('_').count() - 1);
        // replace the .log extension and put the empty string
        ListOfFile.append("DailyRunLog_" + DateAndTimeValue.replace(".log", ""));
    }
}


/****************************************************************************/
void DayLogFileInformation::CreateSpecificDailyRunLogFile(const QString &FileName,
                                   const QByteArray &FileContent) {

    QDir LogDirectory(m_LogFilePath);
    QByteArray& FileData = const_cast<QByteArray&>(FileContent);
    // iterate each file and find whcih event log file it is referring to it
    foreach (QString LogFileName, LogDirectory.entryList(QStringList() << "ColoradoEvents_*.log")) {
        if (LogFileName.contains(FileName.split('_').value(FileName.split('_').count() - 1))) {
            ReadAndTranslateTheFile(m_LogFilePath + QDir::separator() + LogFileName, FileData);
            break;
        }
    }
}

/****************************************************************************/
void DayLogFileInformation::CreateDailyRunLogFiles(const QStringList &FileNames) {

    // removes the constant cast
    QStringList& ListOfFile = const_cast<QStringList&>(FileNames);
    // set the current directory as log files
    QDir LogDirectory(m_LogFilePath);

    QProcess::execute("mkdir " + m_LogFilePath + QDir::separator() + "DailyRun");

    QByteArray FileData;
    // iterate each file and find whcih event log file it is referring to it
    foreach (QString LogFileName, LogDirectory.entryList(QStringList() << "ColoradoEvents_*.log")) {
        FileData.clear();
        // get the date time value from the event log file name
        QString DateAndTimeValue = LogFileName.split('_').value(LogFileName.split('_').count() - 1);


        QString FullPatheOfTheFile = m_LogFilePath + QDir::separator() + "DailyRun"
                + QDir::separator() + "DailyRunLog_" + DateAndTimeValue;

        // replace the .log extension and put the empty string
        ListOfFile.append(FullPatheOfTheFile);
        // translate the files
        ReadAndTranslateTheFile(m_LogFilePath + QDir::separator() + LogFileName, FileData);
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
