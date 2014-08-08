/****************************************************************************/
/*! \file Export/Components/ExportData/Source/ExportData.cpp
 *
 *  \brief Implementation file for ExportData class.
 *
 *  \b Description:
 *         Reads the "TempExportConfiguration.xml" file and verfies the export
 *         configuration container. Writes archive files (lpkg and Zip format)
 *         in the mounted device
 *
 *
 *  $Version:   $ 1.0, 2.0
 *  $Date:      $ 2012-07-12, 2013-02-05
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
#include "Global/Include/GlobalExitCodes.h"
#include "EncryptionDecryption/General/Include/General.h"
#include "QProcess"
#include "QDebug"

namespace Export {

// constant file modes
const char FILE_WRITEMODE                   = 'w'; ///< constant for file write mode

// constants for package type
const QString PACKAGE_NONNATIVE             = "nonnative"; ///< constant for the package type

// constants for package type // fix for IN:000970
const QString WILDCHAR_ANY                  = "*.*"; ///< constant for the log files

// constants for package type
const QString COMMAND_ZIP                   = "zip"; ///< constant for the console command

// constants for the xml files
const QString FILENAME_EXPORTCONFIGURATION  = "TempExportConfiguration.xml"; ///< const for the configuration xml file

// constant for delimiters
const QString DELIMITER_UNDERSCORE          = "_"; ///< constant for the underscore

// constant for strings
const QString STRING_COLON                  = ":"; ///< constant for the colon
const QString STRING_Z                      = "z"; ///< constant for the z
const QString STRING_T                      = "T"; ///< constant for the T
const QString STRING_UNDERSCORE             = "_"; ///< constant for the underscore
const QString STRING_EMPTY                  = ""; ///< constant for the empty string
const QString STRING_MINUS                  = "-"; ///< constant for the minus

// constants for directories
const QString DIRECTORY_EXPORT              = "Export"; ///< constant for the Export directory name
const QString DIRECTORY_TEMPORARY           = "../Temporary"; ///< constant for the Temporary directory name

// constants for the file extension type
const QString FILE_EXTENSION_LPKG           = ".lpkg"; ///< constant for the lpkg file type
const QString FILE_EXTENSION_ZIP            = ".zip"; ///< constant for the ZIP file type

// constants for the type of Import
const QString TYPEOFIMPORT_SERVICE          = "Service"; ///< constant for the 'Service' of Import
const QString TYPEOFIMPORT_USER             = "User"; ///< constant for the 'User' of Import


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
 *         Read and verifies the "tempConfiguration.xml" file. Create list of
 *         files which needs to be archived.
 *
 *  \return Error number or success
 */
/****************************************************************************/
int CExportData::CreateArchiveFiles()
{
    // set the temporary file directory so that it can read the data from the directory
    // all the temporay files will be created in a folder called "Temporary"
    Global::SystemPaths::Instance().SetTempPath(DIRECTORY_TEMPORARY);
    QString FileName = Global::SystemPaths::Instance().GetTempPath() + QDir::separator() + DIRECTORY_EXPORT +
            QDir::separator() + FILENAME_EXPORTCONFIGURATION;

    if (!QFile::exists(FileName)) {
        return Global::EXIT_CODE_EXPORT_UNABLE_TO_READ_FILE_TEMP_EXPORTCONFIGURATION;
    }
    DataManager::CExportConfiguration ExportFile;
    ExportFile.SetDataVerificationMode(false);

    // fix for IN:000966
    if (!ExportFile.Read(FileName)) {
        return Global::EXIT_CODE_EXPORT_INIT_CONTAINER_FAILED;
    }
    m_PairList.clear();
    // add a verifier to the list
    DataManager::IVerifierInterface* Verifier = new DataManager::CExportConfigurationVerifier();
    (void)ExportFile.AddVerifier(Verifier); // to avoid pc-lint e534
    ExportFile.SetDataVerificationMode(true);

    QStringList Parts = ExportFile.GetTargetFileName().split(DELIMITER_UNDERSCORE);
    // if the count is not greater than then verifier fails, so no need of else condition
    if (Parts.count() > 0) {
        // this is part added ony for verification purpose. Verifier only verifies the data for the service export and
        // user export
        if (Parts.value(1).compare(TYPEOFIMPORT_SERVICE) == 0) {
            ExportFile.SetServiceConfigurationFlag(true);
        }
        else if (Parts.value(1).compare(TYPEOFIMPORT_USER) == 0) {
            ExportFile.SetUserConfigurationFlag(true);
        }
        else {
            // this case is taken care at the down
        }
    }

    if (!ExportFile.VerifyData()) {
        return Global::EXIT_CODE_EXPORT_VERIFICATION_CONTAINER_FAILED;
    }

    QDir Directory;

    if (!Directory.exists(ExportFile.GetSourceDir())) {
        // if the source directory does not exist then return error
        return Global::EXIT_CODE_EXPORT_SOURCE_DIRECTORY_NOT_EXISTS;
    }

    if (!Directory.exists(ExportFile.GetTargetDir())) {
        // if the target directory does not exist then return error
        return Global::EXIT_CODE_EXPORT_TARGET_DIRECTORY_NOT_EXISTS;
    }

    QString TargetFileName = ExportFile.GetTargetDir() + QDir::separator() + ExportFile.GetTargetFileName();    

    if (Parts.count() <= 3 || Parts.value(Parts.count() - 1).compare("%1") != 0) {
        // file does not contain the format
        return Global::EXIT_CODE_EXPORT_TARGET_FILE_FORMAT_IS_WRONG;
    }

    if (Parts.value(1).compare(TYPEOFIMPORT_SERVICE) == 0) {
        QString PackageType = ExportFile.GetServiceConfiguration().GetServiceConfigurationList().GetPackageType();

        // for nonnative type the file is zip
        if (PackageType.compare(PACKAGE_NONNATIVE) == 0) {
            m_PairList.insert(TargetFileName + FILE_EXTENSION_ZIP,
                            ExportFile.GetServiceConfiguration().GetServiceConfigurationList().GetFileList());
        }
        else {
            m_PairList.insert(TargetFileName + FILE_EXTENSION_LPKG,
                            ExportFile.GetServiceConfiguration().GetServiceConfigurationList().GetFileList());
        }
    }
    else if (Parts.value(1).compare(TYPEOFIMPORT_USER) == 0) {
        QString PackageType = ExportFile.GetUserConfiguration().GetUserConfigurationList().GetPackageType();

        // for nonnative type the file is zip
        if (PackageType.compare(PACKAGE_NONNATIVE) == 0) {
            m_PairList.insert(TargetFileName + FILE_EXTENSION_ZIP,
                            ExportFile.GetUserConfiguration().GetUserConfigurationList().GetFileList());
        }
        else {
            m_PairList.insert(TargetFileName + FILE_EXTENSION_LPKG,
                            ExportFile.GetUserConfiguration().GetUserConfigurationList().GetFileList());
        }

        PackageType = ExportFile.GetUserConfiguration().GetUserReportList().GetPackageType();

        if (PackageType.compare(PACKAGE_NONNATIVE) == 0) {
            m_PairList.insert(TargetFileName + FILE_EXTENSION_ZIP,
                            ExportFile.GetUserConfiguration().GetUserReportList().GetFileList());
        }
        else {
            m_PairList.insert(TargetFileName + FILE_EXTENSION_LPKG,
                            ExportFile.GetUserConfiguration().GetUserReportList().GetFileList());
        }
    }
    else {        
        return Global::EXIT_CODE_EXPORT_INVALID_EXPORT;
    }

    if(!CheckUSBSpace(ExportFile.GetTargetDir())){
        return Global::EXIT_CODE_EXPORT_NO_ENOUGH_SPACE_ON_USB;
    }
    return StartPackTheFiles(ExportFile);
}

/****************************************************************************/
/*!
 *  \brief Pack the files either Zip format or lpkg format
 *
 *  \iparam ExportConfiguration - Export configuration class
 *
 *  \return Error number or success
 */
/****************************************************************************/
int CExportData::StartPackTheFiles(const DataManager::CExportConfiguration &ExportConfiguration)
{
    DataManager::CExportConfiguration& ExportFile = const_cast<DataManager::CExportConfiguration&>(ExportConfiguration);

    foreach (QString KeyName, m_PairList.keys()) {
        // T shall be replaced with undesrcore e.g. 2001-05-18_122345 (YYYY-MM-DD_HHMMSS)
        QString DateValue = Global::AdjustedTime::Instance().GetCurrentDateTime().toString(Qt::ISODate).
                replace(STRING_T, STRING_UNDERSCORE).replace(STRING_MINUS, STRING_EMPTY);
        // colon shall be replaced with empty string - for ISO8601 'z' will be added in the end of time
        DateValue = DateValue.replace(STRING_COLON, STRING_EMPTY).replace(STRING_Z, STRING_EMPTY, Qt::CaseInsensitive);

        if (KeyName.contains(FILE_EXTENSION_ZIP)) {

            int ErrorNumber = WriteZipFile(ExportFile, KeyName, DateValue);

            if (ErrorNumber != Global::EXIT_CODE_EXPORT_SUCCESS) {
                RemoveFiles();
                return ErrorNumber;
            }
        }
        else {
            // if any error occurs then error number will be returned, this is a for loop
            // so it should not break the loop until unless it is a error
            int ErrorNumber = WritelpkgFile(ExportFile, KeyName, DateValue);

            // remove files if it exists it
            (void)QFile::remove(EncryptionDecryption::Constants::keyfile); //to avoid lint-534
            (void)QFile::remove(EncryptionDecryption::Constants::counter); //to avoid lint-534

            if (ErrorNumber != Global::EXIT_CODE_EXPORT_SUCCESS) {
                RemoveFiles();
                return ErrorNumber;
            }
        }
    }
    // if the function succeeded then return 1
    return Global::EXIT_CODE_EXPORT_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief Write the zip file by using the unix command on the target device
 *
 *  \iparam ExportConfiguration - Export configuration class
 *  \iparam KeyName             - Name of the key
 *  \iparam DateValue           - Date value in the customized format
 *
 *  \return Error number or success
 */
/****************************************************************************/
int CExportData::WriteZipFile(const DataManager::CExportConfiguration &ExportConfiguration,
                               const QString &KeyName, const QString &DateValue)
{
    DataManager::CExportConfiguration& ExportFile = const_cast<DataManager::CExportConfiguration&>(ExportConfiguration);

    QStringList FileList;

    foreach (QString FileName, m_PairList.value(KeyName)) {
        FileList << (FileName);
    }

    // always user configuration will have the list
    if (ExportFile.GetUserConfigurationFlag()) {        
        // <UserConfig> will not have the <Group> tag
        if (ExportFile.GetUserConfiguration().GetUserReportList().GetPackageType().compare(PACKAGE_NONNATIVE) == 0) {
            // set the log directory path - Only <userreport> will have the <Group> tag not the <userConfig>
            QDir LogDirectory(ExportFile.GetSourceDir() + QDir::separator() +
                              ExportFile.GetUserConfiguration().GetUserReportList().GetGroupFileName());
            if (LogDirectory.exists()) {
                foreach (QString FileName, LogDirectory.entryList(QStringList() << WILDCHAR_ANY, QDir::Files)) {
                    FileList << (LogDirectory.absolutePath() + QDir::separator() + FileName);
                }
            }
            else {
                 return Global::EXIT_CODE_EXPORT_LOG_DIRECTORY_NOT_EXISTS;
            }
        }
    }    
    else if (ExportFile.GetServiceConfigurationFlag()) {
        // set the log directory path - <serviceconfig> will have the <Group> tag
        QDir LogDirectory(ExportFile.GetSourceDir() + QDir::separator() +
                          ExportFile.GetServiceConfiguration().GetServiceConfigurationList().GetGroupFileName());
        if (LogDirectory.exists()) {
            foreach (QString FileName, LogDirectory.entryList(QStringList() << WILDCHAR_ANY, QDir::Files)) {
                FileList << (LogDirectory.absolutePath() + QDir::separator() + FileName);
            }
        }
        else {
             return Global::EXIT_CODE_EXPORT_LOG_DIRECTORY_NOT_EXISTS;
        }
    }
    const_cast<QString&>(KeyName) = KeyName.arg(DateValue);
    // store the created file names so that if any error occurs it will delete all the created files
    m_CreatedFileList.append(KeyName);
    FileList.insert(0, KeyName);

    QProcess ZipProcess;

    ZipProcess.setProcessChannelMode(QProcess::MergedChannels);

    // set the working directory so that all the file paths will not saved in the Zip file
    ZipProcess.setWorkingDirectory(ExportFile.GetSourceDir());

    FileList.insert(0, "-j");
    // create the archive with the zip file so that user can open the file with any
    // compression sw. Use a command line argument of Linux operating system
    ZipProcess.start(COMMAND_ZIP , FileList);

    if (!(ZipProcess.waitForFinished())) {
        // save the data in temporay string for the zip process it can be error or success
        QString StdOutData(ZipProcess.readAllStandardOutput());
        if (StdOutData.length() == 0) {
            return Global::EXIT_CODE_EXPORT_ZIP_COMMAND_NOT_FOUND;
        }
        else {
            return Global::EXIT_CODE_EXPORT_ZIP_IS_TAKING_LONGTIME;
        }
    }
    else {
        // save the data in temporay string for the zip process it can be error or success
        QString StdOutData(ZipProcess.readAllStandardOutput());
        // fix for IN:000967
        if (StdOutData.contains("zip error:") || StdOutData.contains("zip warning: name not matched")) {
            return Global::EXIT_CODE_EXPORT_ZIP_ERROR;
        }
        // else means zip command executed successfully
    }

    return Global::EXIT_CODE_EXPORT_SUCCESS;

}

/****************************************************************************/
/*!
 *  \brief Write the lpkg file by using HMAC(Sha) hash algorithm on the target device
 *
 *  \iparam ExportConfiguration - Export configuration class
 *  \iparam KeyName             - Name of the key
 *  \iparam DateValue           - Date value in the customized format
 *
 *  \return Error number or success
 */
/****************************************************************************/
int CExportData::WritelpkgFile(const DataManager::CExportConfiguration &ExportConfiguration,
                               const QString &KeyName, const QString &DateValue)
{
    DataManager::CExportConfiguration& ExportFile = const_cast<DataManager::CExportConfiguration&>(ExportConfiguration);
    // always make encryption to true create the archive file and with HMAC
    QList<QByteArray> Files;

    foreach (QString FileName, m_PairList.value(KeyName)) {
        Files << qPrintable(ExportFile.GetSourceDir() + QDir::separator() + FileName);
    }

    bool Encryption = true;
    bool Compressed = true;

    if (ExportFile.GetServiceConfigurationFlag()) {
        // set the log directory path
        QDir LogDirectory(ExportFile.GetSourceDir() + QDir::separator() +
                          ExportFile.GetServiceConfiguration().GetServiceConfigurationList().GetGroupFileName());

        if (LogDirectory.exists()) {

            foreach (QString FileName, LogDirectory.entryList(QStringList() << WILDCHAR_ANY, QDir::Files)) {
                Files << qPrintable(LogDirectory.absolutePath() + QDir::separator() + FileName);
            }
        }
        else {
            return Global::EXIT_CODE_EXPORT_LOG_DIRECTORY_NOT_EXISTS;
        }

        Encryption = ExportFile.GetServiceConfiguration().GetServiceConfigurationList().GetEncryptionFlag();
        Compressed = ExportFile.GetServiceConfiguration().GetServiceConfigurationList().GetCompressedFlag();
    }
    else if (ExportFile.GetUserConfigurationFlag()) {
        // check which list has the non native package value
        if (ExportFile.GetUserConfiguration().GetUserConfigurationList().GetPackageType().compare(PACKAGE_NONNATIVE) != 0) {
            Encryption = ExportFile.GetUserConfiguration().GetUserConfigurationList().GetEncryptionFlag();
            Compressed = ExportFile.GetUserConfiguration().GetUserConfigurationList().GetCompressedFlag();
        }
        // fix for IN:000964 and IN:000965
        // always the user report will have the group of files
        if (ExportFile.GetUserConfiguration().GetUserReportList().GetPackageType().compare(PACKAGE_NONNATIVE) != 0) {
            // set the log directory path
            QDir LogDirectory(ExportFile.GetSourceDir() + QDir::separator() +
                              ExportFile.GetUserConfiguration().GetUserReportList().GetGroupFileName());

            if (LogDirectory.exists()) {

                foreach (QString FileName, LogDirectory.entryList(QStringList() << WILDCHAR_ANY, QDir::Files)) {
                    Files << qPrintable(LogDirectory.absolutePath() + QDir::separator() + FileName);
                }
            }
            else {
                return Global::EXIT_CODE_EXPORT_LOG_DIRECTORY_NOT_EXISTS;
            }
            Encryption = ExportFile.GetUserConfiguration().GetUserReportList().GetEncryptionFlag();
            Compressed = ExportFile.GetUserConfiguration().GetUserReportList().GetCompressedFlag();
        }
    }

    const_cast<QString&>(KeyName) = KeyName.arg(DateValue);
    // these files are required to create the archive
    QByteArray keybytes(EncryptionDecryption::Constants::KEYFILESIZE, 0);
    keybytes[2*EncryptionDecryption::Constants::HASH_SIZE-1] = 1;
    // first create the key file. These files will be used by Writearchieve method
    EncryptionDecryption::FailSafeOpen keyfile(EncryptionDecryption::Constants::keyfile, FILE_WRITEMODE);
    keyfile.write(keybytes);
    keyfile.close();
    // create the counter file
    EncryptionDecryption::FailSafeOpen ctrfile(EncryptionDecryption::Constants::counter, FILE_WRITEMODE);
    ctrfile.write(QByteArray(4, 0));
    ctrfile.close();

    // if the successful then return 0
    return WriteArchiveFile(KeyName, Files, Encryption, Compressed);
}

/****************************************************************************/
/*!
 *  \brief Write the archive file in lpkg format
 *
 *  \iparam KeyName             - Name of the key
 *  \iparam Files               - list of files
 *  \iparam Encryption          - Encryption flag
 *  \iparam Compressed          - Compressed flag
 *
 *  \return Error number or success
 */
/****************************************************************************/
int CExportData::WriteArchiveFile(const QString &KeyName, const QList<QByteArray> &Files,
                               const bool &Encryption, const bool &Compressed)
{
    // try to catach all the errors while archiving the files
    try {
        // write the archive file
        EncryptionDecryption::WriteArchive(qPrintable(KeyName), Files, 1, Encryption, Compressed);
        // store the created file name
        m_CreatedFileList.append(KeyName);
    }
    catch (EncryptionDecryption::ExceptionNumber ExNumber) {
        // if error then remove the file also
        (void)QFile::remove(KeyName); //to avoid lint-534

        qint32 ExitCode = Global::EXIT_CODE_EXPORT_UNABLE_ARCHIVE_FILES;

        switch(ExNumber.getErrorNumber()) {
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_CRYTOSERVICE_RUNNING:
                ExitCode = Global::EXIT_CODE_EXPORT_CRYTOSERVICE_RUNNING;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_CANNOT_OPEN_FILE_FOR_READ:
                ExitCode = Global::EXIT_CODE_EXPORT_CANNOT_OPEN_FILE_FOR_READ;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_CANNOT_OPEN_FILE_FOR_WRITE:
                ExitCode = Global::EXIT_CODE_EXPORT_CANNOT_OPEN_FILE_FOR_WRITE;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_ERROR_TO_READ:
                ExitCode = Global::EXIT_CODE_EXPORT_ERROR_TO_READ;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_ERROR_TO_WRITE:
                ExitCode = Global::EXIT_CODE_EXPORT_ERROR_TO_WRITE;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_ARCHIVEFILE_FORMAT_WRONG:
                ExitCode = Global::EXIT_CODE_EXPORT_TARGET_FILE_FORMAT_IS_WRONG;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_INDEX_IS_MATCHING:
                ExitCode = Global::EXIT_CODE_EXPORT_INDEX_IS_MATCHING;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_KEY_SIZE_LESS:
                ExitCode = Global::EXIT_CODE_EXPORT_KEY_SIZE_LESS;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_KEYDATA_SIZE_IS_NOT_MATCHING:
                ExitCode = Global::EXIT_CODE_EXPORT_KEYDATA_SIZE_IS_NOT_MATCHING;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_HMAC_NOT_INITIALIZED:
                ExitCode = Global::EXIT_CODE_EXPORT_HMAC_NOT_INITIALIZED;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_AES_NOT_INITIALIZED:
                ExitCode = Global::EXIT_CODE_EXPORT_AES_NOT_INITIALIZED;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_INTEGER_SIZE_IS_MORE:
                ExitCode = Global::EXIT_CODE_EXPORT_INTEGER_SIZE_IS_MORE;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_MSB_BIT_IS_NOT_SET :
                ExitCode = Global::EXIT_CODE_EXPORT_MSB_BIT_IS_NOT_SET;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_INVALID_FILE_MODE:
                ExitCode = Global::EXIT_CODE_EXPORT_INVALID_FILE_MODE;
                break;
            case EncryptionDecryption::ERROR_ENCRYPTIONDECRYPTION_HMAC_COMPUTATION_STARTED:
                ExitCode = Global::EXIT_CODE_EXPORT_HMAC_COMPUTATION_STARTED;
                break;
        }

        return ExitCode;
    }
    return Global::EXIT_CODE_EXPORT_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief Remove the files if any error occured while creating the files
 */
/****************************************************************************/
void CExportData::RemoveFiles()
{
    if (m_CreatedFileList.count() > 0) {
        for (qint32 Counter = 0; Counter < m_CreatedFileList.count(); Counter++) {
            if (QString(m_CreatedFileList.value(Counter)).compare("") != 0) {
                // if error then remove the lpkg file also
                (void)QFile::remove(m_CreatedFileList.value(Counter));  //to avoid lint-534
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Remove the files if any error occured while creating the files
 *
 *  \iparam Destination device to be checked.
 *
 *  \return true: USB has enough space to export, or false
 */
/****************************************************************************/
bool CExportData::CheckUSBSpace(QString Destination)
{
#if defined(__arm__)
    QString CommandExpSize = "du -s " + Global::SystemPaths::Instance().GetTempPath()
            + QDir::separator() + DIRECTORY_EXPORT + "| cut -f1 >size.txt";
    if(system(CommandExpSize.toLatin1().data())){
        system("rm size.txt");
        return false;
    }

    QString CheckRCExport = "df | grep " + Destination;
    QString CommandAvailableSize = "df | grep " + Destination + " | awk '{print $4}' >>size.txt";
    if(!system(CheckRCExport.toLatin1().data())){
        if(system(CommandAvailableSize.toLatin1().data())){
            system("rm size.txt");
            return false;
        }
    }else{ // RC Export
        return true;
    }
    QFile Size("size.txt");
    if(Size.open(QFile::ReadOnly | QFile::Text)){
        QTextStream in(&Size);

        qlonglong ExportSize = 0;
        qlonglong AvailSize = 0;
        bool ok;

        QString line;
        if (!in.atEnd()) {
            line = in.readLine();
            ExportSize = line.toLongLong(&ok);
         }
         if (!in.atEnd()) {
             line = in.readLine();
             AvailSize = line.toLongLong(&ok);
          }
         Size.close();
          if(ExportSize != 0 && AvailSize != 0 && ExportSize < AvailSize){
              system("rm size.txt");
              return true;
          }
          else{
              system("rm size.txt");
              return false;
          }
    }
    else{
        return false;
    }
#else
    Q_UNUSED(Destination)
    return true;
#endif
}

} // end namespace Export
