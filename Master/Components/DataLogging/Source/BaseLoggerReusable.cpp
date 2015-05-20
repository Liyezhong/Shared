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
#include "Global/Include/SystemPaths.h"


namespace DataLogging {

/****************************************************************************/
BaseLoggerReusable::BaseLoggerReusable(QObject *pParent,
                                       const QString & TheLoggingSource, int FormatVersion) :
    BaseLogger(pParent, TheLoggingSource, FormatVersion) {
}


/****************************************************************************/
void BaseLoggerReusable::SetConfiguration(const QString &OperatingMode,
                                          const QString &SerialNumber, const QString &Version, const QString &Path) {
    m_OperatingMode = OperatingMode;
    m_SerialNumber = SerialNumber;
    m_SWVersion    = Version;
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
                            "Serial Number: " + m_SerialNumber + "\n\n" +
                            "SW Version: " + m_SWVersion + "\n\n";

    // Get hardware version for asb board
    QString FilePCBVersion = Global::SystemPaths::Instance().GetSettingsPath() + "/Slave_HW_Version.txt";
    if(QFile::exists(FilePCBVersion))
    {
        QFile HWVersion(FilePCBVersion);
        if (HWVersion.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&HWVersion);
            while(!in.atEnd())
            {
                QStringList line = in.readLine().split(",");
                if(line.size() > 2)
                {
                    HeaderString += QString("%1: %2\n\n").arg(line[0].split("_")[0]).arg(line[1]);
                }
            }
            HWVersion.close();
        }
    }


    HeaderString += QString("                            T_RV1    T_RV2    T_OvT   T_OvB1   T_OvB2");
    HeaderString += QString("   T_RtB1   T_RtB2   T_RtSd     T_LS    T_Tb1    T_Tb2");
    HeaderString += QString("    Press CurRV CurRt CurLT  Cur3  Cur5 Cur15");
    HeaderString += QString("   RvP RtLk OvLid");

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
