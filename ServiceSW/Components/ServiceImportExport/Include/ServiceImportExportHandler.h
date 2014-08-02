/****************************************************************************/
/*! \file ServiceImportExportHandler.h
 *
 *  \brief Definition file for class ServiceImportExportHandler.
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

#ifndef IMPORTEXPORT_SERVICEIMPORTEXPORTHANDLER_H
#define IMPORTEXPORT_SERVICEIMPORTEXPORTHANDLER_H

#include <iostream>

#include "Threads/Include/ThreadController.h"
#include "DataManager/Containers/ExportConfiguration/Include/ExportConfigurationVerifier.h"
#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationInterface.h"
#include "DataManager/Containers/InstrumentHistory/Include/InstrumentHistoryVerifier.h"
#include "Global/Include/SystemPaths.h"
#include "EncryptionDecryption/WriteArchive/Include/WriteArchive.h"
#include "EncryptionDecryption/ReadArchive/Include/ReadArchive.h"
#include <QProcess>
#include <QMutex>
#include <QWaitCondition>

//lint -sem(ImportExport::CServiceImportExportHandler::DoCleanUpObjects,cleanup)

namespace ImportExport {

const qint32 TWENTY_FOUR_HOURS_IN_SECONDS = 86400; ///< const for 24 hours value in seconds

/****************************************************************************/
/**
 * \brief Class for ImportExportThread Controller.
 *        This class used for to Import the data and export the data
 */
/****************************************************************************/
class CServiceImportExportHandler : public QObject {

    Q_OBJECT
private:
    DataManager::CDeviceConfigurationInterface *mp_DeviceConfigInterface;   ///< Device Configuration Interface object
    DataManager::CExportConfiguration* mp_ExportConfiguration;  ///< Store export configuration
    quint32 m_NoOfLogFiles;                                     ///< Store the no of log files value for service export
    QString m_OperationName;                                    ///< Store the Operation name name i.e. either user export or service export
    QString m_OperationType;                                    ///< Store the Operation Type
    QString m_SerialNumber;                                     ///< Store the serial number of the device
    QString m_DeviceName;                                       ///< Store the device name
    bool m_NewLanguageAdded;                                    ///< Store the flag for new language
    bool m_CurrentLanguageUpdated;                              ///< Store the flag for current language
    bool m_EventRaised;                                         ///< Store the event information flag for raise event
    bool m_UserExport;                                          ///< Store the user export flag
    bool m_SettingsFolderUpdatedFiles;                          ///< Settings folder updated with the files
    bool m_TranslationsFolderUpdatedFiles;                      ///< Translations folder updated with the files
    DataManager::CExportConfiguration m_TempExportConfiguration;  ///< Store temporary export configuration
    quint32 m_EventCode;                                        ///< Store the event code
    QString m_EventLogFileName;                                 ///< Store the event log file name
    QString m_TargetFileName;                                   ///< Store the target file name
    bool m_TakeBackUp;                                          ///< Backup true or false
    bool m_IsSelectionRequested;                                  ///< If multiple lpkg files exists, sets to true else false

    /****************************************************************************/
    /**
     * \brief Cleans the "Export" and "Import" directories which are there in
     *        "Temporary" directory
     */
    /****************************************************************************/
    void CleanTempDirectory();

public:

    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    CServiceImportExportHandler();

    /****************************************************************************/
    /**
     * \brief Constructor.
     * \iparam p_ServiceGUIConnector = Service data connector object
     * \iparam SourceType = Command name
     * \iparam CommandValue = Command value
     */
    /****************************************************************************/
    CServiceImportExportHandler(DataManager::CDeviceConfigurationInterface *p_DeviceInterface, QString SourceType, QString CommandValue);

    /****************************************************************************/
    /**
     * \brief Copy Constructor.
     */
    /****************************************************************************/
    CServiceImportExportHandler(const CServiceImportExportHandler &);

    /****************************************************************************/
    /**
     * \brief Assignment Operator which copies from rhs to lhs.
     * \iparam ImportExportHandler
     * \return ServiceImportExportHandler object
     */
    /****************************************************************************/
    const CServiceImportExportHandler & operator = (const CServiceImportExportHandler &ImportExportHandler);

    /****************************************************************************/
    /**
     * \brief Create required directories before proceeding further otherwise the
     *        thread end up with error
     */
    /****************************************************************************/
    void CreateRequiredDirectories();
	
    /****************************************************************************/
    /**
     * \brief Do pre tasks before starting the process.
     *        Creates the RMS_Status.csv file and TempConfiguration.xml file and
     *        copy required files for export from one location to other location
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool DoPretasks();

    /****************************************************************************/
    /**
     * \brief Create the temporary export configuration and write all the required
     *        files in a temporary directory.
     *
     * \return On Successful (true) or not (false)
     */
    /****************************************************************************/
    bool WriteTempExportConfigurationAndFiles();       

    /****************************************************************************/
    /**
     * \brief Copy the configuration files from one location to other location.
     *        Configuration list contains all the file list, so read all the files
     *        from the list and copy the files to target location
     *
     * \iparam           ConfigurationList    Configuration class.
     * \iparam           TargetPath           Target path to copy the files.
     *
     * \return On Successful (true) or not (false)
     */
    /****************************************************************************/
    bool CopyConfigurationFiles(const DataManager::CConfigurationList &ConfigurationList,
                                QString TargetPath);

    /****************************************************************************/
    /**
     * \brief Check the existence of the file and copy the files to Target place
     *        Sometimes Target folder does not delete all the files, so check
     *        the existence of the file first before copying, if it exists delete
     *        and copy the new file
     *
     * \iparam           TargetFileName    Target file name.
     * \iparam           SourceFileName    Source file name.
     *
     * \return On Successful (true) or not (false)
     */
    /****************************************************************************/
    bool CheckTheExistenceAndCopyFile(QString TargetFileName, QString SourceFileName);

    /****************************************************************************/
    /**
     * \brief Cleans up the objects.
     */
    /****************************************************************************/
    void DoCleanUpObjects();
	
    /****************************************************************************/
    /**
     * \brief Imports files from the archive file.
     *
     * \iparam    ImportType  Type of Import
     * \iparam    FileName    Name of the file
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool ImportArchiveFiles(const QString &ImportType, QString FileName);

    /****************************************************************************/
    /**
     * \brief Adds required files to string list, so that importing can be done
     *        little easier .
     *
     * \iparam           TypeOfImport  Type of Import
     * \oparam           ListOfFiles   List of files to be imported
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool AddFilesForImportType(const QString &TypeOfImport, const QStringList &ListOfFiles);

    /****************************************************************************/
    /**
     * \brief Writes the files in a temporary loaction and imports the data into
     *        data containers.
     *
     * \iparam           TypeOfImport  Type of Import
     * \iparam           FileList      List of files to be imported
     * \iparam           RamFile       Object of the RAM file
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool WriteFilesAndImportData(const QString &TypeOfImport, const QStringList &FileList,
                                 const EncryptionDecryption::RAMFile &RamFile);

    /****************************************************************************/
    /**
     * \brief Check for the files updated or not.
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool CheckForFilesUpdateError();

    /****************************************************************************/
    /**
     * \brief Update the settings folder with rollback folder configuration files.
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool UpdateSettingsWithRollbackFolder();    

    /****************************************************************************/
    /**
     * \brief Mounts the USB device.
     *
     *
     * \iparam     IsImport     Import flag
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool MountDevice(bool IsImport = false);

    /****************************************************************************/
    /**
     * \brief Unmount the device if the device is mounted.
     */
    /****************************************************************************/
    void UnMountDevice();


    /****************************************************************************/
    /**
     * \brief Update the folder with the files from the source to target folder.
     *
     * \iparam     FileList      Files in the list
     * \iparam     TargetPath    Target path of the folder
     * \iparam     SourcePath    Source path of the folder
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool UpdateFolderWithFiles(QStringList FileList, QString TargetPath,
                               QString SourcePath);

    /****************************************************************************/
    /**
     * \brief Write the configuration files in the settings folder.
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool WriteFilesInSettingsFolder();       

protected:
    /****************************************************************************/
    /**
     * \brief This method is called to start Import/Export
     *
     * This means that everything is fine and normal operation started.
     * We are running in our own thread now.\n
     * We do not have anything special to do, since objects are already configured
     * and ready for normal operation.
     */
    /****************************************************************************/
    void StartImportExportProcess();

    /****************************************************************************/
    /**
     * \brief This method is called on completion of Import/Export
     *
     * This means that normal operation will stop after processing this signal.
     * We are still running in our own thread.\n
     * We do not have anything special to do.
     */
    /****************************************************************************/
     void FinishImportExportProcess();

    /****************************************************************************/
    /**
     * \brief Power will fail shortly.
     *
     * We try to log the power fail into the event logger and close all open files.
     * After that we switch to a PowerFail state where only events are processed
     * and dumped to console.
     *
     * \iparam PowerFailStage   Power fail stages
     */
    /****************************************************************************/
    virtual void OnPowerFail(const Global::PowerFailStages PowerFailStage);

public:

    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~CServiceImportExportHandler();

    /****************************************************************************/
    /**
     * \brief Create and configure the loggers.
     *
     * If something goes wrong, the already created loggers are deleted.
     */
    /****************************************************************************/
    void CreateAndInitializeObjects();

    /****************************************************************************/
    /**
     * \brief Creates the containers and update the containers.
     *
     * \iparam       TypeOfImport    Type of Import
     * \iparam       FilePath        Path of the file to save
     *
     * \return On successful (true) or not (false)
     */
    /****************************************************************************/
    bool CreateAndUpdateContainers(const QString TypeOfImport, const QString FilePath);

signals:
    /****************************************************************************/
    /**
     * \brief Signal for starting the export process.
     * \iparam FileName = File name
     * As soon as this signal received by Main it starts the Export process
     */
    /****************************************************************************/
    void StartExportProcess(QString FileName);

    /****************************************************************************/
    /**
     * \brief Signal for Thread finished its task.
     *  This signals main that this thread exited with some code and also updates
     *  about the language import
     *
     * \iparam       EventCode                   Event code
     * \iparam       TypeofOperation           True for Import,False for Export
     * \iparam       IsAborted                 When ImportedFile is null,It is made true
     *
     */
    /****************************************************************************/
    void ThreadFinished(quint32 EventCode, bool TypeofOperation, bool IsAborted);

    /****************************************************************************/
    /**
     * \brief Signal for selecting required files to import.
     *  This signals main that import detected multiple files, Main requests
     *  to display selection screen in GUI side
     *
     */
    /****************************************************************************/
    void RequestFileSelectionToImport(QStringList);

public slots:   

    /****************************************************************************/
    /**
     * \brief Slot for importing files.
     *
     * \iparam FileList - List of files
     *
     */
    /****************************************************************************/
    void StartImportingFiles(const QStringList FileList);  

    /****************************************************************************/
    /**
     * \brief Cleanup and destroy the loggers.
     */
    /****************************************************************************/
    void CleanupDestroyObjects();


};

} // end namespace ImportExport

#endif // IMPORTEXPORT_SERVICEIMPORTEXPORTHANDLER_H
