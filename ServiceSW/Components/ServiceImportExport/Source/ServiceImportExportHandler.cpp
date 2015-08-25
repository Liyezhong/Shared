/****************************************************************************/
/*! \file ServiceImportExportHandler.cpp
 *
 *  \brief Implementation file for class ServiceImportExportHandler.
 *
 *  \b Description:
 *         1. Searches and Mounts the device for the Export and Import.
 *         2. Do the pre-tasks (Identifies which files needs to be exported) before
 *            creating the export process
 *         3. Read the Import file and takes a copy of the updated files in Rollback folder
 *         4. Updates the data containers (Programs.xml, ProgramsSequence.xml, Reagents.xml,
 *            UserSettings.xml and Stations.xml)
 *         5. Creates the temporary directories
 *         6. Updates the Rollback folder before importing and after importing the data
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-07-17
 *  $Author:    $ Soumya. D
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

#include "../Include/PlatformServiceEventCodes.h"
#include "ServiceImportExport/Include/ServiceImportExportHandler.h"
#include "Global/Include/Utils.h"
#include <ExportController/Include/ExportController.h>
#include <QMetaType>
#include "Application/Include/LeicaStyle.h"

namespace ImportExport {

// constants for the different files
const QString FILENAME_SERVICEHELPTEXT          = "ServiceHelpText.txt";                ///< const for the ServiceHelpText file name
const QString FILENAME_EXPORTCONFIG             = "ExportConfiguration.xml";            ///< const for the service export configuration file name
const QString FILENAME_TEMPEXPORTCONFIG         = "TempExportConfiguration.xml";        ///< const for the temporary service export configuration file name
const QString FILTER_EXTENSION_LOG              = "*.log";                              ///< const for the log file names

// constant for command name
const QString COMMAND_NAME_IMPORT           = "Import"; ///< constant for the command name

// constants for directory names
const QString DIRECTORY_IMPORT              = "Import"; ///< constant for the Import directory name
const QString DIRECTORY_EXPORT              = "Export"; ///< constant for the Export directory name
const QString DIRECTORY_SETTINGS            = "Settings"; ///< constant for the settings directory name
const QString DIRECTORY_ROLLBACK            = "Rollback"; ///< constant for the Rollback directory name
const QString DIRECTORY_TEMPORARY           = "Temporary"; ///< constant for the Temporary directory name
const QString DIRECTORY_TRANSLATIONS        = "Translations"; ///< constant for the Translations directory name
const QString DIRECTORY_LOGFILES            = "Logfiles"; ///< constant for the Logfiles directory name

// constants for system directory paths
const QString DIRECTORY_SH                  = "/bin/sh"; ///< constant for the shell directory

// constants for the file type
const QString FILETYPE_LPKG                 = "*.lpkg"; ///< constant for the lpkg file type

// constants for the type of Import
const QString TYPEOFIMPORT_SERVICE          = "Service"; ///< constant for the 'Service' of Import

// constant for strings
const QString STRING_UNDEFINED              = "UNDEFINED"; ///< constant for the UNDEFINED string
const QString STRING_SPACE                  = " "; ///< constant string for space
const QString STRING_PERCENTNUMBER          = "%1"; ///< constant string for percent one

// constant for delimiters
const QString DELIMITER_STRING_UNDERSCORE   = "_"; ///< constant for the delimiter underscore

// constants for commands
const QString COMMAND_MKDIR                 = "mkdir "; ///< constant string for the command 'mkdir'
const QString COMMAND_RM                    = "rm "; ///< constant string for the command 'rm'
const QString COMMAND_ARGUMENT_C            = "-c"; ///< constant string for the command argument for shell '-c'
const QString COMMAND_ARGUMENT_R            = "-rf"; ///< constant string for the command argument for recursive files '-r'

// constants for wildcharacters
const QString WILDCHAR_ASTRIK               = "*"; ///< constant for wild char

const char FILEMODE_WRITE                   = 'w'; ///< constant for the file write mode
const char CHAR_DOT                         = '.'; ///< constant for the dot

const quint32 THREAD_ID_EXPORT              = 0x0105;  ///< Export

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
CServiceImportExportHandler::CServiceImportExportHandler() :
    mp_DeviceConfigInterface(NULL),
    mp_ExportConfiguration(NULL),
    m_NewLanguageAdded(false),
    m_CurrentLanguageUpdated(false),
    m_EventRaised(false),
    m_UserExport(false),
    m_SettingsFolderUpdatedFiles(false),
    m_TranslationsFolderUpdatedFiles(false),
    m_EventCode(0),
    m_EventLogFileName(""),
    m_TargetFileName(""),
    m_IsSelectionRequested(false)
{
    m_TakeBackUp = true;
    m_NoOfLogFiles = 5;
}

/****************************************************************************/
/**
 * \brief Constructor.
 * \iparam p_ServiceGUIConnector = Service data connector object
 * \iparam SourceType = Command name
 * \iparam CommandValue = Command value
 */
/****************************************************************************/
CServiceImportExportHandler::CServiceImportExportHandler(DataManager::CDeviceConfigurationInterface *p_DeviceConfigInterface, QString SourceType, QString CommandValue) :
    mp_DeviceConfigInterface(p_DeviceConfigInterface),
    mp_ExportConfiguration(NULL),
    m_OperationName(SourceType),
    m_OperationType(CommandValue),
    m_SerialNumber(STRING_UNDEFINED),
    m_DeviceName(STRING_UNDEFINED),
    m_NewLanguageAdded(false),
    m_CurrentLanguageUpdated(false),
    m_EventRaised(false),
    m_UserExport(false),
    m_SettingsFolderUpdatedFiles(false),
    m_TranslationsFolderUpdatedFiles(false),
    m_EventCode(0),
    m_EventLogFileName(""),
    m_TargetFileName("") {
    m_TakeBackUp = true;
    m_NoOfLogFiles = 5;
}


/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
CServiceImportExportHandler::~CServiceImportExportHandler() {
    try {
        // clear the objects
        DoCleanUpObjects();
    }
    catch (...) {
        // do nothing
    }
}

/****************************************************************************/
void CServiceImportExportHandler::CreateAndInitializeObjects() {

    // get the device configuration so that we can use the device name
    // and serial number of the device for the Export component

    if (mp_DeviceConfigInterface != NULL) {
        DataManager::CDeviceConfiguration *p_DeviceConfiguration = NULL;
        p_DeviceConfiguration = mp_DeviceConfigInterface->GetDeviceConfiguration();

        if (p_DeviceConfiguration != NULL) {
            m_DeviceName = p_DeviceConfiguration->GetValue("DeviceName").remove(QRegExp("\\s"));
            m_SerialNumber = p_DeviceConfiguration->GetValue("SerialNumber");
        }
    }

    StartImportExportProcess();

}

/****************************************************************************/
void CServiceImportExportHandler::CleanupDestroyObjects() {
    // call stop received event
    FinishImportExportProcess();
}

/****************************************************************************/
void CServiceImportExportHandler::StartImportExportProcess() {

    CreateRequiredDirectories();
    m_IsSelectionRequested = false;

    if (!m_OperationName.contains(COMMAND_NAME_IMPORT)) {
        bool ErrorInExecution = true;
        if (MountDevice()) {
            // do pre-tasks before emitting the Export process signal
            if (DoPretasks()) {
                // for user export we need to know the daily run log files
                if (!m_UserExport) {
                    // clear the objects
                    DoCleanUpObjects();

                    // emit the export process signal
                    emit StartExportProcess(m_TargetFileName);
                }
                ErrorInExecution = false;
            }
        }

        if (ErrorInExecution) {
            // if the event is not raised then display an error due to any reason
            if (!m_EventRaised) {
                //Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_EXPORT_FAILED, true);
                m_EventCode = EVENT_SERVICE_EXPORT_FAILED;
            }
            // emit the thread finished flag - with error code
            emit ThreadFinished(m_EventCode, false,false);
        }
    }
    else {
        bool IsImported = false;
        // mount the USB device

        if (MountDevice(true)) {
            // to store the files in the directory
            QStringList DirFileNames;
            //            QDir Dir(qApp->applicationDirPath());
            QDir Dir(Global::DIRECTORY_MNT_STORAGE);

            // get the lpkg files from the directory
            DirFileNames = Dir.entryList(QStringList(m_DeviceName + FILETYPE_LPKG), QDir::Files, QDir::Name | QDir::Time);

            switch(DirFileNames.count()) {
            case 0:
                m_EventRaised = true;
                //Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_NO_FILES_TO_IMPORT);
                m_EventCode = EVENT_SERVICE_IMPORT_NO_FILES_TO_IMPORT;
                IsImported = false;
                break;
            case 1:
                // get the return code
                StartImportingFiles(DirFileNames);
                IsImported = true;
                break;
            default:
                m_IsSelectionRequested = true;
                // sort the files
                qSort(DirFileNames.begin(), DirFileNames.end(), qGreater<QString>());
                emit RequestFileSelectionToImport(DirFileNames);
                break;
            }

        }
        if (!m_IsSelectionRequested) {
            // if the import is not successful then raise event
            if (!IsImported) {
                // if the event is not raised then display an error due to any reason
                if (!m_EventRaised) {
                    //Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_FAILED, true);
                    m_EventCode = EVENT_SERVICE_IMPORT_FAILED;
                }
            }
            // emit the thread finished flag
            emit ThreadFinished(m_EventCode, true,false);
        }
    }
}

bool CServiceImportExportHandler::MountDevice(bool IsImport) {

    QString FileName("");

    if (IsImport) {
        FileName = m_DeviceName + DELIMITER_STRING_UNDERSCORE + FILETYPE_LPKG;
        //        FileName = qApp->applicationDirPath() + QDir::separator() + m_DeviceName + "*.lpkg";
    }

    // check for the file existence in the mounted device.
    qint32 MountedValue = Global::MountStorageDevice(FileName);

    switch (MountedValue) {
    default:
        m_EventRaised = true;
        if (m_OperationName.contains(COMMAND_NAME_IMPORT)) {
            // log the event code
            //Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORTEXPORT_IMPORT_NO_USB);
            m_EventCode = EVENT_SERVICE_IMPORTEXPORT_IMPORT_NO_USB;
        }
        else {
            // log the event code
            //Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORTEXPORT_EXPORT_NO_USB);
            m_EventCode = EVENT_SERVICE_IMPORTEXPORT_EXPORT_NO_USB;
        }
        break;

    case 3:
        if (IsImport) {
            m_EventRaised = true;
            //Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_NO_FILES_TO_IMPORT);
            m_EventCode = EVENT_SERVICE_IMPORT_NO_FILES_TO_IMPORT;
        }
        break;

    case 0:
        return true;
    }
    return false;
}

/****************************************************************************/
void CServiceImportExportHandler::FinishImportExportProcess() {

    // unmount the device
    UnMountDevice();
    CleanTempDirectory();

    // this is to make sure all the xpresobjects are cleared.
    DoCleanUpObjects();
}

/****************************************************************************/
void CServiceImportExportHandler::UnMountDevice() {
    Global::UnMountStorageDevice();
}

/****************************************************************************/
void CServiceImportExportHandler::OnPowerFail(const Global::PowerFailStages PowerFailStage) {
    Q_UNUSED(PowerFailStage);
}

/****************************************************************************/
void CServiceImportExportHandler::CreateRequiredDirectories() {
    // make sure that all the directories available to do further - Otherwise end up with error
    QDir DirectoryExist;
    // check the existence of the rollback directory
    if (!DirectoryExist.exists(Global::SystemPaths::Instance().GetRollbackPath())) {
        // create the Rollback folder, if not exists
        (void)QProcess::execute(COMMAND_MKDIR + Global::SystemPaths::Instance().GetRollbackPath());  //to avoid lint-534
    }

    // check the existence of the Temporary directory
    if (!DirectoryExist.exists(Global::SystemPaths::Instance().GetTempPath())) {
        // create the Translations directory, if not exists
        (void)QProcess::execute(COMMAND_MKDIR + Global::SystemPaths::Instance().GetTempPath());  //to avoid lint-534

    }
    else {
        QStringList Options;

        // -c option for shell and remove directory
        Options << COMMAND_ARGUMENT_C << COMMAND_RM + COMMAND_ARGUMENT_R + STRING_SPACE
                   + Global::SystemPaths::Instance().GetTempPath()
                   + QDir::separator() + WILDCHAR_ASTRIK;
        // execute the process with "/bin/sh"
        // remove all the files from the temporary directory "rm -rf Temporary/Import" or "rm -rf Temporary/Export"
        (void)QProcess::execute(DIRECTORY_SH, Options);
    }

    // check the existence of the Rollback/Settings directory
    if (!DirectoryExist.exists(Global::SystemPaths::Instance().GetRollbackPath()
                               + QDir::separator() + DIRECTORY_SETTINGS)) {
        // create the Rollback/Settings directory, if not exists
        (void)QProcess::execute(COMMAND_MKDIR + Global::SystemPaths::Instance().GetRollbackPath()
                                + QDir::separator() + DIRECTORY_SETTINGS);  //to avoid lint-534
    }

    // check the existence of the Rollback/Translations directory
    if (!DirectoryExist.exists(Global::SystemPaths::Instance().GetRollbackPath()
                               + QDir::separator() + DIRECTORY_TRANSLATIONS)) {
        // create the Rollback/Translations folder, if not exists
        (void)QProcess::execute(COMMAND_MKDIR + Global::SystemPaths::Instance().GetRollbackPath()
                                + QDir::separator() + DIRECTORY_TRANSLATIONS);  //to avoid lint-534
    }
}

/****************************************************************************/
void CServiceImportExportHandler::CleanTempDirectory() {
    QString DirectoryName;
    // this temporary directory will be used everybody so dont use the command line arguments
    if (!m_OperationName.contains(COMMAND_NAME_IMPORT)) {
        DirectoryName = DIRECTORY_EXPORT;
    }
    else {
        DirectoryName = DIRECTORY_IMPORT;
    }

    QStringList Options;
    // -c option for shell and remove directory
    Options << COMMAND_ARGUMENT_C << COMMAND_RM + COMMAND_ARGUMENT_R + STRING_SPACE
               + Global::SystemPaths::Instance().GetTempPath()
               + QDir::separator() + DirectoryName;
    // execute the process with "/bin/sh"
    // remove all the files from the temporary directory "rm -rf Temporary/Import" or "rm -rf Temporary/Export"
    (void)QProcess::execute(DIRECTORY_SH, Options);
}

/****************************************************************************/
bool CServiceImportExportHandler::DoPretasks() {

    // Create the export configuration object
    mp_ExportConfiguration =  new DataManager::CExportConfiguration();
    mp_ExportConfiguration->SetDataVerificationMode(false);

    if (mp_ExportConfiguration->Read(Global::SystemPaths::Instance().GetSettingsPath()
                                     + QDir::separator() + FILENAME_EXPORTCONFIG)) {
        CleanTempDirectory();

        // set the target directory path "/mnt/USB"
        //        mp_ExportConfiguration->SetTargetDir(qApp->applicationDirPath());
        mp_ExportConfiguration->SetTargetDir(Global::DIRECTORY_MNT_STORAGE);

        // create the Export directory in "Temporary" folder
        if (QProcess::execute(COMMAND_MKDIR + Global::SystemPaths::Instance().GetTempPath()
                              + QDir::separator() + DIRECTORY_EXPORT) >= 0) {

            // form the trget file name format is devicename_typeofexport_serialnumber_datetimestamp
            QString TargetFileName = m_DeviceName + DELIMITER_STRING_UNDERSCORE;

            if (m_OperationType.compare(TYPEOFIMPORT_SERVICE) == 0) {
                mp_ExportConfiguration->SetServiceConfigurationFlag(true);
                // concatenate other informtion to the target file name
                TargetFileName += TYPEOFIMPORT_SERVICE + DELIMITER_STRING_UNDERSCORE;
            }
            else {
                return false;
            }

            // concatenate other information to the target file name, dont add the time stamp here.
            // This will be done by export process
            TargetFileName += m_SerialNumber + DELIMITER_STRING_UNDERSCORE + STRING_PERCENTNUMBER;
            m_TargetFileName = TargetFileName;
            mp_ExportConfiguration->SetTargetFileName(TargetFileName);

            if (!WriteTempExportConfigurationAndFiles()) {
                return false;
            }
        }
        else {
            //Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_EXPORT_DIRECTORY_CREATION_FAILED, true);
            m_EventCode = EVENT_SERVICE_EXPORT_DIRECTORY_CREATION_FAILED;
            m_EventRaised = true;
            return false;
        }
    }
    else {
        return false;
    }
    return true;
}

/****************************************************************************/
bool CServiceImportExportHandler::WriteTempExportConfigurationAndFiles() {

    DataManager::CConfigurationList ConfigurationList = mp_ExportConfiguration->GetServiceConfiguration().GetServiceConfigurationList();
    QStringList FileList = ConfigurationList.GetFileList();
    if (Application::CLeicaStyle::GetCurrentDeviceType() == Application::DEVICE_SEPIA) {
        if (FileList.contains("CMS_Status.csv")) {
            (void) FileList.removeOne("CMS_Status.csv");
        }
    } else {
        if (FileList.contains("RMS_Status.csv")) {
            (void) FileList.removeOne("RMS_Status.csv");
        }
    }
    ConfigurationList.SetFileList(FileList);

    DataManager::CServiceConfiguration ServiceConfiguration = mp_ExportConfiguration->GetServiceConfiguration();
    ServiceConfiguration.SetServiceConfigurationList(ConfigurationList);

    mp_ExportConfiguration->SetServiceConfiguration(ServiceConfiguration);

    if (mp_ExportConfiguration != NULL) {
        // do a deep copy of the data
        m_TempExportConfiguration = *mp_ExportConfiguration;
    }
    // set the destination directory as temp "Temporary/Export"
    m_TempExportConfiguration.SetSourceDir(Global::SystemPaths::Instance().GetTempPath() + QDir::separator() + DIRECTORY_EXPORT);

    if (m_TempExportConfiguration.GetServiceConfigurationFlag()) {
        // copy all the files in a temporary directory
        if (!CopyConfigurationFiles(mp_ExportConfiguration->GetServiceConfiguration().GetServiceConfigurationList(),
                                    m_TempExportConfiguration.GetSourceDir())) {
            //Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_EXPORT_FILES_NOT_COPIED, true);
            m_EventCode = EVENT_SERVICE_EXPORT_FILES_NOT_COPIED;
            m_EventRaised = true;
            return false;
        }


        // write the Export configuration file
        if (!m_TempExportConfiguration.Write(m_TempExportConfiguration.GetSourceDir() + QDir::separator() + FILENAME_TEMPEXPORTCONFIG)) {
            //Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_EXPORT_UNABLE_TO_CREATE_FILE_TEMP_EXPORTCONFIGURATION, true);
            m_EventCode = EVENT_SERVICE_EXPORT_UNABLE_TO_CREATE_FILE_TEMP_EXPORTCONFIGURATION;
            m_EventRaised = true;
            return false;
        }
    }

    m_EventCode = EVENT_SERVICE_EXPORT_SUCCESS;
    return true;
}

/****************************************************************************/
bool CServiceImportExportHandler::CopyConfigurationFiles(const DataManager::CConfigurationList &ConfigurationList,
                                                         QString TargetPath) {
    // remove the constant cast
    DataManager::CConfigurationList& Configuration = const_cast<DataManager::CConfigurationList&>(ConfigurationList);

    QStringList FileNames = Configuration.GetFileList();

    if (Application::CLeicaStyle::GetCurrentDeviceType() == Application::DEVICE_SEPIA) {
        if (FileNames.contains("CMS_Status.csv")) {
            (void) FileNames.removeOne("CMS_Status.csv");
        }
    } else {
        if (FileNames.contains("RMS_Status.csv")) {
            (void) FileNames.removeOne("RMS_Status.csv");
        }
    }

    Configuration.SetFileList(FileNames);
    foreach (QString FileName, Configuration.GetFileList()) {
        if (mp_ExportConfiguration != NULL) {

            // check whether file exists in the "Settings" directory before start copying the files
            if (!QFile::exists(mp_ExportConfiguration->GetSourceDir() + QDir::separator() + FileName)) {
                qDebug() << "File does not exist";
                return false;
            }

            // file name can contain folder name also, create folder name
            if (FileName.contains(QDir::separator())) {
                QString DirectoryName(FileName.mid(0, FileName.indexOf(QDir::separator())));
                QDir Directory(TargetPath + QDir::separator() + DirectoryName);
                // create directory if not exists
                if (!Directory.exists()) {
                    (void)Directory.mkdir(Directory.absolutePath());
                }
            }

            // copy the files and QDir::seperator uses generic symbol depending on the OS i.e. "/" for unix and for windows "\"
            if (!CheckTheExistenceAndCopyFile(TargetPath + QDir::separator() + FileName, mp_ExportConfiguration->GetSourceDir() +
                                              QDir::separator() + FileName)) {
                return false;
            }
        }
        else {
            return false;
        }
    }

    // check the group flag, if it is true then start copying the files from the directory
    if (Configuration.GetGroupListFlag()) {
        // for log directory
        QDir LogDirectory;
        QString LogFiles;
        bool UserLogDirectory = false;


        if (Configuration.GetGroupFileName().compare(DIRECTORY_LOGFILES, Qt::CaseInsensitive) == 0) {
            (void)LogDirectory.setPath(Global::SystemPaths::Instance().GetLogfilesPath()); //to avoid lint-534
            LogFiles = FILTER_EXTENSION_LOG;

        }
        else {
            UserLogDirectory = true;
            (void)LogDirectory.setPath(Global::SystemPaths::Instance().GetLogfilesPath() + QDir::separator() +
                                       Configuration.GetGroupFileName()); //to avoid lint-534
        }

        if (LogDirectory.exists()) {
            // create one more directory to save the LogFiles
            if (QProcess::execute(COMMAND_MKDIR + TargetPath + QDir::separator()
                                  + Configuration.GetGroupFileName()) >= 0) {

                QStringList LogFilesList = LogDirectory.entryList(QStringList() << LogFiles);
                if (!UserLogDirectory) {
                    if (m_EventLogFileName.compare("") != 0) {
                        // for event log files we need to copy only the specified number of files
                        QStringList EventFileList = LogDirectory.entryList
                                (QStringList() << m_EventLogFileName + FILTER_EXTENSION_LOG,
                                 QDir::Files, QDir::Name | QDir::Time);

                        for (int Counter = m_NoOfLogFiles; Counter < EventFileList.count(); Counter++) {
                            (void)LogFilesList.removeOne(EventFileList.value(Counter));
                        }
                    }
                }

                foreach (QString FileName, LogFilesList) {

                    // copy the files and QDir::seperator uses generic symbol depending on the OS i.e. "/" for unix and for windows "\"
                    if (!CheckTheExistenceAndCopyFile(TargetPath + QDir::separator() + Configuration.GetGroupFileName() +
                                                      QDir::separator() + FileName, LogDirectory.absolutePath() +
                                                      QDir::separator() + FileName)) {
                        return false;
                    }
                }
            }
        }
        else {
            return false;
        }
    }
    return true;
}

/****************************************************************************/
bool CServiceImportExportHandler::CheckTheExistenceAndCopyFile(QString TargetFileName, QString SourceFileName) {
    // if some reason the Export directory is not deleted then files cannot be copied
    // so delete the files which are in Export directory
    if (QFile::exists(TargetFileName)) {
        (void)QFile::remove(TargetFileName); //to avoid lint-534
    }

    if (!QFile::copy(SourceFileName, TargetFileName)) {
        qDebug() << "Unable to copy the files";
        return false;
    }
    return true;
}

/****************************************************************************/
void CServiceImportExportHandler::DoCleanUpObjects() {

    // destroy export configuration container
    try {
        delete mp_ExportConfiguration;
    }
    catch(...) {
        // nothing to do
    }

    mp_ExportConfiguration = NULL;

}

// Import
/****************************************************************************/
#if 0
void CServiceImportExportHandler::StartImportingFiles(const QStringList FileList) {

    QStringList ImportTypeList;
    QString TypeOfImport;
    bool IsImport = true;

    ImportTypeList.clear();


    if (FileList.count() == 0) {
        //File list is Null so we are aborting the operation
        emit ThreadFinished(m_EventCode, true,true);
        return;
    }

    for (qint32 Counter = 0; Counter < FileList.count(); Counter++) {
        if (!ImportArchiveFiles(TypeOfImport, FileList.value(Counter))) {
            IsImport = false;
            break;
        }
        ImportTypeList.append(TypeOfImport);
    }

    if (IsImport && ImportTypeList.count() > 0) {
        // if everything goes well then update the files and take a backup of the files
        if (ImportTypeList.contains(TYPEOFIMPORT_SERVICE)) {
            // before updating take a back-up of the configuration files
            QStringList FileNameList;
            FileNameList << FILENAME_SERVICEHELPTEXT;

            // check for the files updation error
            if (!CheckForFilesUpdateError()) {
                IsImport = false;
            }

            // update the Rollback folder with latest files after the Import is successful
            if (!UpdateFolderWithFiles(FileNameList, Global::SystemPaths::Instance().GetRollbackPath()
                                       + QDir::separator() + DIRECTORY_SETTINGS + QDir::separator(),
                                       Global::SystemPaths::Instance().GetSettingsPath()
                                       + QDir::separator())) {
                Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_UPDATE_ROLLBACK_FAILED, true);
                m_EventCode = EVENT_SERVICE_IMPORT_UPDATE_ROLLBACK_FAILED;
                m_EventRaised = true;
                IsImport = false;
            }
        }
    }

    if (!IsImport  && ImportTypeList.count() > 0) {
        if (ImportTypeList.contains(TYPEOFIMPORT_SERVICE) && m_SettingsFolderUpdatedFiles) {
            (void)UpdateSettingsWithRollbackFolder();
        }
    }

    if(m_IsSelectionRequested) {
        emit ThreadFinished(m_EventCode, true,false);
        m_IsSelectionRequested = false;
    }
}
#endif

/****************************************************************************/
bool CServiceImportExportHandler::ImportArchiveFiles(const QString &ImportType, QString FileName) {
    // remove the const cast
    QString& TypeOfImport = const_cast<QString&>(ImportType);
    // this is used for Import command
    bool IsImported = true;
    // we use different keys for Leica and Viewer (Viewer with value 1)
    QByteArray KeyBytes(EncryptionDecryption::Constants::KEYFILESIZE, 0);
    KeyBytes[2 * EncryptionDecryption::Constants::HASH_SIZE - 1] = 1;
    // create the RAM file
    EncryptionDecryption::RAMFile RFile;
    // to store the file list
    QStringList FileList;
    //        QDir Dir(qApp->applicationDirPath());
    QDir Dir(Global::DIRECTORY_MNT_STORAGE);

    // check the file format - consider the first file
    if (FileName.split(DELIMITER_STRING_UNDERSCORE).count() > 4) {
        // check whether device name matches with the file name
        if (FileName.split("_").value(0).compare(m_DeviceName, Qt::CaseInsensitive) == 0) {
            // second value of the file name will be the type of Import
            TypeOfImport = FileName.split("_").value(1);

            IsImported = AddFilesForImportType(TypeOfImport, FileList);

            if (IsImported) {
                try {
                    // read the archive file - add try catch
                    EncryptionDecryption::ReadArchive(qPrintable(Dir.absoluteFilePath(FileName)), &RFile,
                                                      "Import", KeyBytes, FileList, Global::SystemPaths::Instance().GetTempPath()
                                                      + QDir::separator() + DIRECTORY_IMPORT + QDir::separator());
                }
                catch (...) {
                    m_EventCode = EVENT_SERVICE_IMPORT_TAMPERED_ARCHIVE_FILE;
                    IsImported = false;
                }
            }
        }
        else {
            // device name is not matching
            m_EventCode = EVENT_SERVICE_IMPORT_DEVICE_NAME_NOT_MATCHING;
            IsImported = false;
        }
    }
    else {
        //Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_ARCHIVE_FILE_FORMAT_NOT_PROPER, true);
        m_EventCode = EVENT_SERVICE_IMPORT_ARCHIVE_FILE_FORMAT_NOT_PROPER;
        m_EventRaised = true;
        IsImported = false;
    }

    if (IsImported) {
        // check the type of Import, for the language file we cannot predict how many files can be present
        // so we hard code the value at least two should be present one is SW_Version.xml and other one can
        // be either QM or event string XML file
        if (!WriteFilesAndImportData(TypeOfImport, FileList, RFile)) {
            IsImported = false;
        }
    }

    return IsImported;
}

// Import
/****************************************************************************/
bool CServiceImportExportHandler::AddFilesForImportType(const QString &TypeOfImport, const QStringList &ListOfFiles)
{
    QStringList& FileList = const_cast<QStringList&>(ListOfFiles);
    // check the type of Import
    if (TypeOfImport.compare(TYPEOFIMPORT_SERVICE, Qt::CaseInsensitive) == 0) {
        // for the type of import "user" or "Service"
        FileList << FILENAME_SERVICEHELPTEXT;
    } else {
        m_EventCode = EVENT_SERVICE_IMPORT_TYPEOFIMPORTNOTVALID;
        return false;
    }
    return true;
}

// Import
/****************************************************************************/
bool CServiceImportExportHandler::WriteFilesAndImportData(const QString &TypeOfImport, const QStringList &FileList,
                                                          const EncryptionDecryption::RAMFile &RamFile) {
    bool RequiredFilesImported = false;
    EncryptionDecryption::RAMFile RFile = const_cast<EncryptionDecryption::RAMFile&>(RamFile);
    // check the type of Import
    // for other import check for the file count
    if (RFile.getFiles().count() == FileList.count()) {
        RequiredFilesImported = true;
    }

    // check file count
    if (RequiredFilesImported) {
        // create the temporary directory
        if (QProcess::execute(COMMAND_MKDIR + Global::SystemPaths::Instance().GetTempPath()
                              + QDir::separator() + DIRECTORY_IMPORT) >= 0) {
            // write all the files
            foreach(QString KeyName, RFile.getFiles().uniqueKeys()) {
                foreach (QByteArray FileData, RFile.getFiles().values(KeyName)) {
                    if (KeyName.contains(DIRECTORY_IMPORT)) {
                        // try to create the files
                        try {
                            // remove the full file path
                            EncryptionDecryption::FailSafeOpen FileWrite(KeyName, FILEMODE_WRITE);
                            FileWrite.write(FileData);
                            FileWrite.close();
                            //                            return true;
                        }
                        catch (...) {
                            // got a exception
                            return false;
                        }
                    }
                }
            }
#if 0
            if ((CreateAndUpdateContainers(TypeOfImport, QString(Global::SystemPaths::Instance().GetTempPath()
                                                                 + QDir::separator() + DIRECTORY_IMPORT)))) {
                // if everything is success then return code is positive
                return true;
            }
#else
            return true;
#endif
        }
    }
    else {
        //Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_REQUIRED_FILES_NOT_AVAILABLE, true);
        m_EventCode = EVENT_SERVICE_IMPORT_REQUIRED_FILES_NOT_AVAILABLE;
        m_EventRaised = true;
    }
    return true;
}


/****************************************************************************/
bool CServiceImportExportHandler::CreateAndUpdateContainers(const QString TypeOfImport,
                                                            const QString FilePath) {

    Q_UNUSED(FilePath);
    // check the type of Import
    if ((TypeOfImport.compare(TYPEOFIMPORT_SERVICE, Qt::CaseInsensitive) == 0)) {

        // before updating take a back-up of the configuration files
        QStringList FileList;
        FileList << FILENAME_SERVICEHELPTEXT;

        if (m_TakeBackUp) {
            // save the data containers before copying to "Rollback" folder
            if (!WriteFilesInSettingsFolder()) {
                m_EventCode = EVENT_SERVICE_IMPORT_UNABLE_TO_WRITE_FILES;
                return false;
            }
            // updated the rollback folder so that before overwriting we can have a backup
            if (!UpdateFolderWithFiles(FileList, Global::SystemPaths::Instance().GetRollbackPath()
                                       + QDir::separator() + DIRECTORY_SETTINGS + QDir::separator(),
                                       Global::SystemPaths::Instance().GetSettingsPath()
                                       + QDir::separator())) {
                //Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_UPDATE_ROLLBACK_FAILED, true);
                m_EventCode = EVENT_SERVICE_IMPORT_UPDATE_ROLLBACK_FAILED;
                m_EventRaised = true;
                return false;
            }
            // only first time back up is required
            m_TakeBackUp = false;
        }
    }
    else {
        // unknown Import is found
        m_EventCode = EVENT_SERVICE_IMPORT_TYPEOFIMPORTNOTVALID;
        return false;
    }
    return true;
}

/****************************************************************************/
bool CServiceImportExportHandler::WriteFilesInSettingsFolder() {

    QString SourcePath = Global::SystemPaths::Instance().GetTempPath() + QDir::separator() + DIRECTORY_IMPORT
            + QDir::separator() + FILENAME_SERVICEHELPTEXT;
    QString TargetPath = Global::SystemPaths::Instance().GetSettingsPath() + QDir::separator()
            + FILENAME_SERVICEHELPTEXT;

    (void)QFile::remove(TargetPath); //to avoid lint-534

    if (!QFile::copy(SourcePath, TargetPath)) {
        qDebug() << "File cannot be copied to Settings folder";
        return false;
    }

    return true;
}

/****************************************************************************/
bool CServiceImportExportHandler::CheckForFilesUpdateError() {

    bool Rollback = true;

    // if rollback is true then put back all original files and exit
    if (Rollback) {
        qDebug() << "Rollback true" << endl;
        if (!UpdateSettingsWithRollbackFolder()) {
            return false;
        }
    }

    return true;
}

/****************************************************************************/
bool CServiceImportExportHandler::UpdateSettingsWithRollbackFolder() {

    QStringList FileList;

    FileList << FILENAME_SERVICEHELPTEXT;
    // update the settings folder with the rollback folder files
    if (!UpdateFolderWithFiles(FileList, Global::SystemPaths::Instance().GetSettingsPath() + QDir::separator(),
                               Global::SystemPaths::Instance().GetRollbackPath() + QDir::separator()
                               + DIRECTORY_SETTINGS + QDir::separator())) {
        //Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_ROLLBACK_FAILED, true);
        m_EventCode = EVENT_SERVICE_IMPORT_ROLLBACK_FAILED;
        m_EventRaised = true;
        return false;
    }

    return true;
}


/****************************************************************************/
bool CServiceImportExportHandler::UpdateFolderWithFiles(QStringList FileList, QString TargetPath,
                                                        QString SourcePath) {
    // copy all the files to a rollback folder
    foreach (QString FileName, FileList) {
        // search in the file name  "*."
        if (FileName.contains(WILDCHAR_ASTRIK + QString(CHAR_DOT))) {
            QDir SourceDirectory(SourcePath);
            foreach (QString WildCharFileName, SourceDirectory.entryList(QStringList() << FileName)) {
                // remove the file then try to write it
                (void)QFile::remove(TargetPath + WildCharFileName); //to avoid lint-534
                // not able to take backup of the files
                if (!QFile::copy(SourcePath + WildCharFileName,
                                 TargetPath + WildCharFileName)) {
                    /// this never happens - If it happens something wrong with file writing in the flash
                    return false;
                }
            }
        }
        else {
            // remove the file then try to write it
            (void)QFile::remove(TargetPath + FileName); //to avoid lint-534
            // not able to take backup of the files
            if (!QFile::copy(SourcePath + FileName,
                             TargetPath + FileName)) {
                qDebug() << "File cannot be copied to Settings folder";
                /// this never happens - If it happens something wrong with file writing in the flash
                return false;
            }
        }
    }
    return true;
}

/****************************************************************************/
void CServiceImportExportHandler::StartImportingFiles(const QStringList FileList) {

    QStringList ImportTypeList;
    QString TypeOfImport;
    bool ErrorFlag = false;

    if(FileList.empty())
    {
        emit ThreadFinished(m_EventCode, true,true);
        return;
    }
    ImportTypeList.clear();

    // multiple files can be imported. At max three files can be imported.
    for (qint32 Counter = 0; Counter < FileList.count(); Counter++) {
        if (!ImportArchiveFiles(TypeOfImport, FileList.value(Counter))) {
            ErrorFlag = true;
            break;
        }
        ImportTypeList.append(TypeOfImport);
    }


    if(!ErrorFlag){
        ErrorFlag = ! WriteFilesInSettingsFolder();
    }
    if (ErrorFlag) {
        if (!ImportTypeList.contains(TYPEOFIMPORT_SERVICE)) {
            (void)UpdateSettingsWithRollbackFolder();
            ImportTypeList.clear();
        }
    }

    // update the rollback checksum value
    QProcess Md5sumProcess;
    Md5sumProcess.start(Global::SystemPaths::Instance().GetScriptsPath()
                        + QString("/EBox-Utils.sh") , QStringList() <<
                        QString("update_md5sum_for_settings service"));
    (void)Md5sumProcess.waitForFinished();
    // update the rollback directory also
    Md5sumProcess.start(Global::SystemPaths::Instance().GetScriptsPath()
                        + QString("/EBox-Utils.sh") , QStringList() <<
                        QString("update_settings_to_rollback"));
    (void)Md5sumProcess.waitForFinished();


    (void)QProcess::startDetached("sync &");
    // emit the thread finished flag
    if(m_IsSelectionRequested) {
        emit ThreadFinished(m_EventCode, true,false);
        m_IsSelectionRequested = false;
    }
}

}   // end of namespace ImportExport
