/****************************************************************************/
/*! \file DataLogging/Source/BaseLoggerReusable.cpp
 *
 *  \brief Implementation file for class BaseLoggerReusable.
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

#include <DataLogging/Include/BaseLoggerReusable.h>
#include <DataLogging/Include/DataLoggingEventCodes.h>
#include <Global/Include/EventObject.h>
#include <QDir>
#include <QTextStream>


namespace DataLogging {

/****************************************************************************/
BaseLoggerReusable::BaseLoggerReusable(Global::EventObject *pParent, const QString & TheLoggingSource, int FormatVersion) :
    BaseLogger(pParent, TheLoggingSource, FormatVersion)
{
}

/****************************************************************************/
void BaseLoggerReusable::BackupFile(const QString & /*FileName*/) {
}

/****************************************************************************/
void BaseLoggerReusable::SetConfiguration(const QString &OperatingMode, const QString &SerialNumber, const QString &Path) {
    m_OperatingMode = OperatingMode;
    m_SerialNumber = SerialNumber;
    m_Path = Path;
}

/****************************************************************************/
void BaseLoggerReusable::SwitchToFile(const QString &FileName, bool BackupOldFile) {
    // close old file
    CloseFile();
    // compute new file name
    QDir Dir(m_Path);

    QString CompleteFileName(QDir::cleanPath(Dir.absoluteFilePath(FileName)));
    // check if the file exists.
    if(!QFile::exists(CompleteFileName)) {
        // file does not exist

        // create new file and write header information
        CreateNewFile(CompleteFileName);
        WriteHeader();
        // trace that file was created.
        //LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_DATALOGGING_INFO_FILE_CREATE, CompleteFileName
          //        , Global::NO_NUMERIC_DATA, false);
    } else {
        // a file already exists.

        // check if header information in existing file fits
        if(!CheckHeaderFormat(CompleteFileName)) {
            // file header format does not fit

            // make backup of existing file if neccessary
            if(BackupOldFile) {
                // make backup
                BackupFile(CompleteFileName);
            } else {
                // overwrite file
            }
            // create new file and write header information
            CreateNewFile(CompleteFileName);
            WriteHeader();
            // trace that file was created.
            //LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_DATALOGGING_INFO_FILE_CREATE, CompleteFileName
               //       , Global::NO_NUMERIC_DATA, false);
        } else {
            // file version and serial number seems to be ok
            OpenFileForAppend(CompleteFileName);
        }
    }
}

/****************************************************************************/
void BaseLoggerReusable::WriteHeader() {
    // write header information: FileName;TimeStamp;Type;FormatVersion;SerialNumber
    QString HeaderString =  "Format Version: " + QString::number(GetFormatVersion(), 10)  + "\n\n" +
                            "FileName: " + GetCompleteBaseFileName() + "\n\n" +
                            "TimeStamp: " + GetTimeStampHeader() + "\n\n" +
                            "OperatingMode: " + m_OperatingMode + "\n\n" +
                            "Serial Number: " + m_SerialNumber + "\n";
    // append data to file
    AppendLine(HeaderString);
}

/****************************************************************************/
bool BaseLoggerReusable::CheckHeaderFormat(const QString &FileName) {
    // open in readonly mode
    QFile File;
    File.setFileName(FileName);
    // and try to open
    if(!File.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // could not open file
        return false;
    }
    // set reading position to 0
    if(!File.seek(0)) {
        LOGANDTHROWARG(Global::EVENT_GLOBAL_ERROR_FILE_SEEK, FileName);
    }
    // file now open for reading

    // try to read header information
    char buf[200];
    qint64 LineLength = File.readLine(buf, sizeof(buf));
    if(-1 == LineLength) {
        // error reading data
        return false;
    }

    // first line now in buffer. remove trailing \n
    // first line is version string
    QString Line = QString(buf).trimmed();

    QStringList List = Line.split(":");

    // try to convert to integer
    bool ok = false;
    int FormatVersion = List.at(1).toInt(&ok, 10);
    if(!ok) {
        // conversion failed
        return false;
    }

    // version seems to be a at least a number.
    // check if it fits
    if(GetFormatVersion() != FormatVersion) {
        return false;
    }
    // everything OK
    return true;
}

} // end namespace DataLogging
