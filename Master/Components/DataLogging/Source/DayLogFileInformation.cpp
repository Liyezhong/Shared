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
#include "DataLogging/Include/DayLogFileInformation.h"

namespace DataLogging {

/****************************************************************************/
DayLogFileInformation::DayLogFileInformation(QString FilePath) :
    m_LogFilePath(FilePath)
{
}

/****************************************************************************/
ListOfLanguageIDs_t DayLogFileInformation::ReadTheLanguageFile(const QString &LanguageFilePath) {

    ListOfLanguageIDs_t LanguageIDs;

    QFile LanguageFile(LanguageFilePath);
    if (!LanguageFile.open(QIODevice::ReadOnly)) {
        return LanguageIDs;
    }

    QXmlStreamReader LanguageXmlReader(&LanguageFile);

    (void)LanguageXmlReader.readElementText(QXmlStreamReader::IncludeChildElements); //lint -e534

    while (!LanguageXmlReader.atEnd()) {
        if (LanguageXmlReader.readNext() == QXmlStreamReader::Invalid) {
            qDebug() << "Reading " << LanguageXmlReader.name() << " at line number: " << LanguageXmlReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << LanguageXmlReader.errorString();
        }
        if (LanguageXmlReader.isStartElement()) {
            // check for the node name <string>
            if (LanguageXmlReader.name().compare("string") == 0) {
                QString ID;
                QString LanguageText;
                // read the language id
                if (LanguageXmlReader.attributes().hasAttribute("id")) {
                    ID = LanguageXmlReader.attributes().value("id").toString();
                }
                // read the langugae text
                if (LanguageXmlReader.attributes().hasAttribute("text")) {
                    LanguageText = LanguageXmlReader.attributes().value("text").toString();
                }
                // check both are having data then only add to the hash table
                if (ID.compare("") != 0 && LanguageText.compare("") != 0) {
                    LanguageIDs.insert(ID, LanguageText);
                }
            }
        }
    }
    LanguageFile.close();

    return LanguageIDs;
}


/****************************************************************************/
void DayLogFileInformation::ReadAndTranslateTheFile(const ListOfLanguageIDs_t &ListLanguageIDs,
                                                                 const QString &FileName, const QByteArray &ByteArray) {

    ListOfLanguageIDs_t& LanguageIDs = const_cast<ListOfLanguageIDs_t&>(ListLanguageIDs);
    QByteArray& FileData = const_cast<QByteArray&>(ByteArray);

    QFile LogFile(FileName);
    if (!LogFile.open(QIODevice::ReadOnly)) {
        return;
    }

    while (!LogFile.atEnd()) {
        QString ReadData = LogFile.readLine();

        if (ReadData.contains(";")) {
            if (ReadData.contains(";true;")) {
                if (LanguageIDs.contains(ReadData.split(';').value(1))) {
                    ReadData.replace(ReadData.split(';').value(3), LanguageIDs.value(ReadData.split(';').value(1)));
                    FileData.append(ReadData);
                }
                else {
                    FileData.append(ReadData);
                }
            }
        }
        else {
            FileData.append(ReadData.replace("ColoradoEvents_", "DailyRunLog_"));
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
void DayLogFileInformation::CreateSpecificDailyRunLogFile(const QString &FileName, const QString &LanguageFilePath,
                                   const QByteArray &FileContent) {

    ListOfLanguageIDs_t LanguageFileDetails = ReadTheLanguageFile(LanguageFilePath);

    QDir LogDirectory(m_LogFilePath);
    QByteArray& FileData = const_cast<QByteArray&>(FileContent);
    // iterate each file and find whcih event log file it is referring to it
    foreach (QString LogFileName, LogDirectory.entryList(QStringList() << "ColoradoEvents_*.log")) {
        if (LogFileName.contains(FileName.split('_').value(FileName.split('_').count() - 1))) {
            ReadAndTranslateTheFile(LanguageFileDetails,
                                    m_LogFilePath
                                    + QDir::separator() + LogFileName, FileData);
            break;
        }
    }
}

/****************************************************************************/
void DayLogFileInformation::CreateDailyRunLogFiles(const QString &LanguageFilePath,
                                   const QStringList &FileNames) {

    // removes the constant cast
    QStringList& ListOfFile = const_cast<QStringList&>(FileNames);
    // set the current directory as log files
    QDir LogDirectory(m_LogFilePath);
    // read the language file
    ListOfLanguageIDs_t LanguageFileDetails = ReadTheLanguageFile(LanguageFilePath);

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
        ReadAndTranslateTheFile(LanguageFileDetails,
                                m_LogFilePath
                                + QDir::separator() + LogFileName, FileData);
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
            DailyRunLogFile.close();
        }
    }
}
}
