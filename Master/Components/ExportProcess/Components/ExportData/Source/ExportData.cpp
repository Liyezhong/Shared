/****************************************************************************/
/*! \file ExportData.cpp
 *
 *  \brief Implementation file for ExportData class.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-12
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

#include "ExportData/Include/ExportData.h"
#include "Global/Include/AdjustedTime.h"
#include "Global/Include/SystemPaths.h"
#include "DataManager/Containers/ExportConfiguration/Include/ExportConfiguration.h"
#include "QProcess"
#include "QDebug"

namespace PlatformExport {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CExportData::CExportData()
{
}

/****************************************************************************/
/*!
 *  \brief Creates the archive files on destination directory
 *
 *  \return Error number
 */
/****************************************************************************/
int CExportData::CreateArchiveFiles()
{

    qDebug() <<  "############################################Export process started#####################";

    // set the temporary file directory so that it can read the data from the directory
    // all the temporay files will be created in a folder called "Temporary"
    Global::SystemPaths::Instance().SetTempPath("../Temporary");

    // read the file name from the temporary directory
    QString FileName = Global::SystemPaths::Instance().GetTempPath() + QDir::separator() + "Export" + QDir::separator() + "TempExportConfiguration.xml";
    // check the file existence
    if (!QFile::exists(FileName)) {
        // if the file does not exist then return 1
        return 1;
    }
    // check the export configuration file
    DataManager::CExportConfiguration ExportFile;

    // this is the test code
    QStringList FileList;
    // make the data verification flag as false, in final it should be true
    ExportFile.SetDataVerificationMode(false);
    // read the file
    ExportFile.Read(FileName);
    PairList.clear();

    if (ExportFile.GetTargetFileName().split("_").value(1).compare("Service") == 0) {
        ExportFile.SetServiceConfigurationFlag(true);
        QString PackageType = ExportFile.GetServiceConfiguration().GetServiceConfigurationList().GetPackageType();
        // for nonnative type the file is zip
        if (PackageType.compare("nonnative") == 0) {
            // insert the file list in the hash table
            PairList.insert(ExportFile.GetTargetFileName() + ".zip", ExportFile.GetServiceConfiguration().GetServiceConfigurationList().GetFileList());
        }
        else {
            // insert the file list in the hash table
            PairList.insert(ExportFile.GetTargetFileName() + ".lpkg", ExportFile.GetServiceConfiguration().GetServiceConfigurationList().GetFileList());
        }
    }
    else if (ExportFile.GetTargetFileName().split("_").value(1).compare("User") == 0) {
        ExportFile.SetUserConfigurationFlag(true);
        QString PackageType = ExportFile.GetUserConfiguration().GetUserConfigurationList().GetPackageType();
        // for nonnative type the file is zip
        if (PackageType.compare("nonnative") == 0) {
            // insert the file list in the hash table
            PairList.insert(ExportFile.GetTargetFileName() + ".zip", ExportFile.GetUserConfiguration().GetUserConfigurationList().GetFileList());
        }
        else {
            // insert the file list in the hash table
            PairList.insert(ExportFile.GetTargetFileName() + ".lpkg", ExportFile.GetUserConfiguration().GetUserConfigurationList().GetFileList());
        }
        // for nonnative type the file is zip
        PackageType = ExportFile.GetUserConfiguration().GetUserReportList().GetPackageType();
        if (PackageType.compare("nonnative") == 0) {
            PairList.insert(ExportFile.GetTargetFileName() + ".zip", ExportFile.GetUserConfiguration().GetUserReportList().GetFileList());
        }
        else {
            PairList.insert(ExportFile.GetTargetFileName() + ".lpkg", ExportFile.GetUserConfiguration().GetUserReportList().GetFileList());
        }
    }

    // iterate the keys and create the archive files
    foreach (QString KeyName, PairList.keys()) {
        // T shall be replaced with undesrcore e.g. 2001-05-18_122345 (YYYY-MM-DD_HHMMSS)
        QString DateValue = Global::AdjustedTime::Instance().GetCurrentDateTime().toString(Qt::ISODate).replace("T", "_");
        // colon shall be replaced with empty string
        DateValue = DateValue.replace(":", "");
        if (KeyName.contains(".zip")) {
            //FileList = PairList.value(KeyName);
            foreach (QString FileName, PairList.value(KeyName)) {
                FileList << (ExportFile.GetSourceDir() + FileName);
            }
            KeyName = KeyName.arg(DateValue);
            FileList.insert(0, KeyName);
            // create the archive with the zip file so that user can open the file with any
            // compression sw. Use a command line argument of Linux operating system
            int ProcessCode = QProcess::execute("zip" , FileList);
            // unable to start the process
            if (!(ProcessCode >= 0)) {
                return false;
            }

        }
        else {
            // always make encryption to true
            // create the archive file and with HMAC
            QList<QByteArray> Files;
            foreach (QString FileName, PairList.value(KeyName)) {
                Files << qPrintable(ExportFile.GetSourceDir() + FileName);
            }
            KeyName = KeyName.arg(DateValue);

            // these files are required to create the archive
            QByteArray keybytes(ImportExport::Constants::KEYFILESIZE, 0);
            keybytes[2*ImportExport::Constants::HASH_SIZE-1] = 1;
            // first create the key file. These files will be used by Writearchieve method
            ImportExport::FailSafeOpen keyfile(ImportExport::Constants::keyfile, 'w');
            keyfile.write(keybytes);
            keyfile.close();
            // create the counter file
            ImportExport::FailSafeOpen ctrfile(ImportExport::Constants::counter, 'w');
            ctrfile.write(QByteArray(4, 0));
            ctrfile.close();
            /// \todo unable to write the file in destination path ExportFile.GetTargetDir() + QDir::separator() +
            ImportExport::WriteArchive(qPrintable(KeyName), Files, 1, true);
            // remove the key file
            QFile::remove(ImportExport::Constants::keyfile);
            // remove the counter file
            QFile::remove(ImportExport::Constants::counter);
        }
    }
    qDebug() <<  "############################################Export process ended#####################";
    // if the function succeeded then return 0
    return 0;
}

} // end namespace PlatformExport
