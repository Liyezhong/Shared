/****************************************************************************/
/*! \file Global/Include/SystemPaths.h
 *
 *  \brief Definition file for class SystemPaths.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-01-19
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

#ifndef GLOBAL_SYSTEMPATHS_H
#define GLOBAL_SYSTEMPATHS_H

#include <Global/Include/GlobalDefines.h>
#include <QString>
#include <QCoreApplication>
#include <QDir>
#include <QReadWriteLock>



namespace Global {

/****************************************************************************/
/**
 * \brief This class used to set and store system's data paths.
 *
 * <b>This class is to be used as a singleton.</b>\n
 * <b>This class is thread safe.</b>
 */
/****************************************************************************/
class SystemPaths {

friend class TestSystemPaths;

private:
    static SystemPaths      m_InstA;                //!< The one and only instance.
    mutable QReadWriteLock  m_SyncObject;           //!< Synchronization object.
    QString                 m_SettingsPath;         //!< Settings directory.
    QString                 m_InstrumentPath;       //!< Instrument specific Settings directory
    QString                 m_FirmwarePath;         //!< Firmware directory.
    QString                 m_UpdatePath;           //!< Update directory.
    QString                 m_LogfilesPath;         //!< Log files directory.
    QString                 m_ComponentTestPath;    //!< Component test directory.
    QString                 m_ManualPath;           //!< manual / help directory.
    QString                 m_UploadsPath;          //!< Folder for Axeda uploads.
    QString                 m_TempPath;             //!< Folder for temporary files.
    QString                 m_RollbackPath;         //!< Folder for Rollback directory
    QString                 m_TranslationsPath;     //!< Translations path( qm files are placed here)
    QString                 m_SoundsPath;            //!< Sounds path( wav files are placed here)
    QString                 m_RemoteCarePath;       //!< RemoteCare path(remote care input/output files are placed here)
    QString                 m_ScriptsPath;          //!< Path for shell scripts
    QString                 m_TranslationsServicePath; //!< Service Translations path
    /****************************************************************************/
    /****************************************************************************/
    /**
     * \brief Disable copy and assignment
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(SystemPaths)
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    SystemPaths();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~SystemPaths();
    /****************************************************************************/
    /**
     * \brief Compute the path for the file.
     *
     * \return  The path to be prepended.
     */
    /****************************************************************************/
    inline static QString ComputePath() {
        return QDir::cleanPath(QCoreApplication::applicationDirPath());
    }
protected:
public:
//    static SystemPaths      m_Instance;             ///< The one and only instance.

    /****************************************************************************/
    /**
     * \brief Get reference to instance.
     *
     * \return      Reference to instance.
     */
    /****************************************************************************/
    static SystemPaths &Instance()
    {
//        static SystemPaths      m_Instance;

        return m_InstA;
    }
    /****************************************************************************/
    /**
     * \brief Get settings path.
     *
     * \return  Settings path.
     */
    /****************************************************************************/
    inline QString GetSettingsPath() const {
        QReadLocker RL(&m_SyncObject);
        return m_SettingsPath;
    }
    /****************************************************************************/
    /**
     * \brief Get instrument specific settings path.
     *
     * \return  Settings path.
     */
    /****************************************************************************/
    inline QString GetInstrumentSettingsPath() const {
        QReadLocker RL(&m_SyncObject);
        return m_InstrumentPath;
    }
    /****************************************************************************/
    /**
     * \brief Set firmware path.
     *
     * \return  Firmware path.
     */
    /****************************************************************************/
    inline QString GetFirmwarePath() const {
        QReadLocker RL(&m_SyncObject);
        return m_FirmwarePath;
    }
    /****************************************************************************/
    /**
     * \brief Set update path.
     *
     * \return  Update path.
     */
    /****************************************************************************/
    inline QString GetUpdatePath() const {
        QReadLocker RL(&m_SyncObject);
        return m_UpdatePath;
    }
    /****************************************************************************/
    /**
     * \brief Set log files path.
     *
     * \return  Log files path.
     */
    /****************************************************************************/
    inline QString GetLogfilesPath() const {
        QReadLocker RL(&m_SyncObject);
        return m_LogfilesPath;
    }
    /****************************************************************************/
    /**
     * \brief Set component test path.
     *
     * \return  Component test path.
     */
    /****************************************************************************/
    inline QString GetComponentTestPath() const {
        QReadLocker RL(&m_SyncObject);
        return m_ComponentTestPath;
    }
    /****************************************************************************/
    /**
     * \brief Set manual / help path.
     *
     * \return  Manual path.
     */
    /****************************************************************************/
    inline QString GetManualPath() const {
        QReadLocker RL(&m_SyncObject);
        return m_ManualPath;
    }
    /****************************************************************************/
    /**
     * \brief Get path to Axeda Uploads.
     *
     * \return  Uploads path.
     */
    /****************************************************************************/
    inline QString GetUploadsPath() const {
        QReadLocker RL(&m_SyncObject);
        return m_UploadsPath;
    }

    /****************************************************************************/
    /**
     * \brief Get path to Temporary Uploads.
     *
     * \return  Uploads path.
     */
    /****************************************************************************/
    inline QString GetTempPath() const {
        QReadLocker RL(&m_SyncObject);
        return m_TempPath;
    }

    /****************************************************************************/
    /**
     * \brief Get path to Rollback Uploads.
     *
     * \return  Uploads path.
     */
    /****************************************************************************/
    inline QString GetRollbackPath() const {
        QReadLocker RL(&m_SyncObject);
        return m_RollbackPath;
    }

    /****************************************************************************/
    /**
     * \brief Get path to Translation files (qm files).
     *
     * \return  Uploads path.
     */
    /****************************************************************************/
    inline QString GetTranslationsPath() const {
        QReadLocker RL(&m_SyncObject);
        return m_TranslationsPath;
    }

    /****************************************************************************/
    /**
     * \brief Set settings path.
     *
     * Usually "Settings".
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   SettingsPath    The path.
     */
    /****************************************************************************/
    inline void SetSettingsPath(const QString &SettingsPath) {
        QWriteLocker WL(&m_SyncObject);
        m_SettingsPath = QDir::cleanPath(ComputePath() + "/" + SettingsPath);
    }
    /****************************************************************************/
    /**
     * \brief Set instrument specific settings path.
     *
     * Usually "Settings/Instrument".
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   SettingsPath    The path.
     */
    /****************************************************************************/
    inline void SetInstrumentSettingsPath(const QString &SettingsPath) {
        QWriteLocker WL(&m_SyncObject);
        m_InstrumentPath = QDir::cleanPath(ComputePath() + "/" + SettingsPath);
    }
    /****************************************************************************/
    /**
     * \brief Set firmware path.
     *
     * Usually "Firmware".
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   FirmwarePath    The path.
     */
    /****************************************************************************/
    inline void SetFirmwarePath(const QString &FirmwarePath) {
        QWriteLocker WL(&m_SyncObject);
        m_FirmwarePath = QDir::cleanPath(ComputePath() + "/" + FirmwarePath);
    }
    /****************************************************************************/
    /**
     * \brief Set update path.
     *
     * Usually "Update".
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   UpdatePath      The path.
     */
    /****************************************************************************/
    inline void SetUpdatePath(const QString &UpdatePath) {
        QWriteLocker WL(&m_SyncObject);
        m_UpdatePath = QDir::cleanPath(ComputePath() + "/" + UpdatePath);
    }
    /****************************************************************************/
    /**
     * \brief Set log files path.
     *
     * Usually "Logfiles".
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   LogfilesPath    The path.
     */
    /****************************************************************************/
    inline void SetLogfilesPath(const QString &LogfilesPath) {
        QWriteLocker WL(&m_SyncObject);
        m_LogfilesPath = QDir::cleanPath(ComputePath() + "/" + LogfilesPath);
    }
    /****************************************************************************/
    /**
     * \brief Set component test path.
     *
     * Usually "Tests".
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   ComponentTestPath   The path.
     */
    /****************************************************************************/
    inline void SetComponentTestPath(const QString &ComponentTestPath) {
        QWriteLocker WL(&m_SyncObject);
        m_ComponentTestPath = QDir::cleanPath(ComputePath() + "/" + ComponentTestPath);
    }
    /****************************************************************************/
    /**
     * \brief Set manual / help path.
     *
     * Usually "Manual".
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   ManualPath      The path.
     */
    /****************************************************************************/
    inline void SetManualPath(const QString &ManualPath) {
        QWriteLocker WL(&m_SyncObject);
        m_ManualPath = QDir::cleanPath(ComputePath() + "/" + ManualPath);
    }
    /****************************************************************************/
    /**
     * \brief Set path to Axeda Uploads.
     *
     * Usually "Uploads".
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   UploadsPath      The path.
     */
    /****************************************************************************/
    inline void SetUploadsPath(const QString &UploadsPath) {
        QWriteLocker WL(&m_SyncObject);
        m_UploadsPath = QDir::cleanPath(ComputePath() + "/" + UploadsPath);
    }
    /****************************************************************************/
    /**
     * \brief Set path to create temporary files.
     *
     * Usually "Temporary".
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   TempPath      The path.
     */
    /****************************************************************************/
    inline void SetTempPath(const QString &TempPath) {
        QWriteLocker WL(&m_SyncObject);
        m_TempPath = QDir::cleanPath(ComputePath() + "/" + TempPath);
    }

    /****************************************************************************/
    /**
     * \brief Set path to create rollback files.
     *
     * Usually "Rollback".
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   RollbackPath      The path.
     */
    /****************************************************************************/
    inline void SetRollbackPath(const QString &RollbackPath) {
        QWriteLocker WL(&m_SyncObject);
        m_RollbackPath = QDir::cleanPath(ComputePath() + "/" + RollbackPath);
    }

    /****************************************************************************/
    /**
     * \brief Set path to Langauage files
     *
     * Usually "Translation".
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   TranslationPath      The path.
     */
    /****************************************************************************/
    inline void SetTranslationsPath(const QString &TranslationPath) {
        QWriteLocker WL(&m_SyncObject);
        m_TranslationsPath = QDir::cleanPath(ComputePath() + "/" + TranslationPath);
    }

    /****************************************************************************/
    /**
     * \brief Get path to Sound files (wav files).
     *
     * \return  Uploads path.
     */
    /****************************************************************************/
    inline QString GetSoundPath() const {
        QReadLocker RL(&m_SyncObject);
        return m_SoundsPath;
    }

    /****************************************************************************/
    /**
     * \brief Set path to Sound files
     *
     * Usually "Translation".
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   SoundPath      The path.
     */
    /****************************************************************************/
    inline void SetSoundPath(const QString &SoundPath) {
        QWriteLocker WL(&m_SyncObject);
        m_SoundsPath = QDir::cleanPath(ComputePath() + "/" + SoundPath);
    }

    /****************************************************************************/
    /**
     * \brief Get path to remote care path
     *
     * \return  Uploads path.
     */
    /****************************************************************************/
    inline QString GetRemoteCarePath() const {
        QReadLocker RL(&m_SyncObject);
        return m_RemoteCarePath;
    }

    /****************************************************************************/
    /**
     * \brief Set path to remote care files
     *
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   RemoteCarePath      The path.
     */
    /****************************************************************************/
    inline void SetRemoteCarePath(const QString &RemoteCarePath) {
        QWriteLocker WL(&m_SyncObject);
        m_RemoteCarePath = QDir::cleanPath(ComputePath() + "/" + RemoteCarePath);
    }

    /****************************************************************************/
    /**
     * \brief Set path to Script files
     *
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   ScriptsPath      The path.
     */
    /****************************************************************************/
    inline void SetScriptsPath(const QString &ScriptsPath) {
        QWriteLocker WL(&m_SyncObject);
        m_ScriptsPath = QDir::cleanPath(ComputePath() + "/" + ScriptsPath);
    }

    /****************************************************************************/
    /**
     * \brief Get path to Script files
     *
     * \return  Scripts path.
     */
    /****************************************************************************/
    inline QString GetScriptsPath() const {
        QReadLocker RL(&m_SyncObject);
        return m_ScriptsPath;
    }

    /****************************************************************************/
    /**
     * \brief Set  path to Service Translations
     *
     * <b>The application path is prepended automatically!</b>
     *
     * \iparam   ServiceTranslationsPath The path.
     */
    /****************************************************************************/
    inline void SetServiceTranslationsPath(const QString &ServiceTranslationsPath) {
        QWriteLocker WL(&m_SyncObject);
        m_TranslationsServicePath = QDir::cleanPath(ComputePath() + "/" + ServiceTranslationsPath);
    }

    /****************************************************************************/
    /**
     * \brief Get path to Translations Files
     *
     * \return  Translations Service Path
     */
    /****************************************************************************/
    inline QString GetServiceTranslationsPath() const {
        QReadLocker RL(&m_SyncObject);
        return m_TranslationsServicePath;
    }




}; // end class SystemPaths

} // end namespace Global

#endif // GLOBAL_SYSTEMPATHS_H
