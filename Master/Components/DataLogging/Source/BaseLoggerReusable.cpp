/****************************************************************************/
/*! \file DataLogging/Source/BaseLoggerReusable.cpp
 *
 *  \brief Implementation file for class BaseLoggerReusable.
 *
 *  \b Description:
 *      This class implements the functionality needed to work with log files on
 *      which data can be appended: check and write header information, switch to
 *      a new file etc.
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

#include <DataLogging/Include/BaseLoggerReusable.h>
#include <DataLogging/Include/DataLoggingEventCodes.h>
#include <Global/Include/EventObject.h>
#include <QDir>
#include <QTextStream>


namespace DataLogging {

/****************************************************************************/
BaseLoggerReusable::BaseLoggerReusable(QObject *pParent,
                                       const QString & TheLoggingSource, int FormatVersion) :
    BaseLogger(pParent, TheLoggingSource, FormatVersion) {
}


/****************************************************************************/
void BaseLoggerReusable::SetConfiguration(const QString &OperatingMode,
                                          const QString &SerialNumber, const QString &Path) {
    m_OperatingMode = OperatingMode;
    m_SerialNumber = SerialNumber;
    m_Path = Path;
}

/****************************************************************************/
void BaseLoggerReusable::SwitchToFile(const QString &FileName) {
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

    } else {
        // a file already exists.

        // check if header information in existing file fits
        if(!CheckHeaderFormat(CompleteFileName)) {
            // create new file and write header information
            CreateNewFile(CompleteFileName);
            WriteHeader();

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
