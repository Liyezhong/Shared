/****************************************************************************/
/*! \file DirectoryHelper.h
 *
 *  \brief Implementation file for class DirectoryHelper.
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

#ifndef DATALOGGING_DIRECTORYHELPER_H
#define DATALOGGING_DIRECTORYHELPER_H

#include <QTest>
#include <QDir>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Helper class for cleaning up a directory.
 */
/****************************************************************************/
class DirectoryHelper {
private:
    QString m_Dir;      ///< The relative path to executable path.
    /****************************************************************************/
    DirectoryHelper();                                              ///< Not implemented.
    DirectoryHelper(const DirectoryHelper &);                       ///< Not implemented.
    const DirectoryHelper & operator = (const DirectoryHelper &);   ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Recursive cleanup a directory.
     *
     * All files and subdirectories are removed.
     *
     * \param[in]   DirName     Name of directory to cleanup.
     */
    /****************************************************************************/
    inline void RecursiveCleanupDir(const QString &DirName) {
        QDir DataDir(DirName);
        // find all files and directories.
        QFileInfoList FileInfoList = DataDir.entryInfoList(QStringList() << "*", QDir::AllEntries | QDir::NoDotAndDotDot);
        for(QFileInfoList::iterator it = FileInfoList.begin(); it != FileInfoList.end(); ++it) {
            if(it->isDir()) {
                // cleanup directory content
                RecursiveCleanupDir(it->absoluteFilePath());
                // delete directory
                QDir TheDir;
                QVERIFY(TheDir.rmdir(it->absoluteFilePath()));
            } else if(it->isFile()) {
                // set permission to writable to enable deletion
                QFile TheFile(it->absoluteFilePath());
                QVERIFY(TheFile.setPermissions(QFile::WriteOwner));
                // delete file
                QVERIFY(TheFile.remove());
            }
        }
    }
protected:
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * A directory (relative to executyble path) is created.
     *
     * \param[in]   RelativeDir     Name of directory relative to executable directory.
     */
    /****************************************************************************/
    inline DirectoryHelper(const QString &RelativeDir) {
        QString ExePath = QCoreApplication::applicationDirPath();
        m_Dir = ExePath + "/" + RelativeDir;
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~DirectoryHelper() {
    }
    /****************************************************************************/
    /**
     * \brief Get data directory.
     *
     * \return  Data directory.
     */
    /****************************************************************************/
    inline QString GetDir() const {
        return m_Dir;
    }
    /****************************************************************************/
    /**
     * \brief Create data directory.
     *
     * The function will create all parent directories necessary to create the
     * directory.
     */
    /****************************************************************************/
    inline void CreateDir() {
        QDir TheDir;
        QVERIFY(TheDir.mkpath(m_Dir));
    }
    /****************************************************************************/
    /**
     * \brief Recursive cleanup data directory.
     *
     * All files and subdirectories are removed. Calls \ref RecursiveCleanupDir
     */
    /****************************************************************************/
    inline void CleanupDir() {
        RecursiveCleanupDir(m_Dir);
    }
    /****************************************************************************/
    /**
     * \brief Remove directory.
     *
     * \warning Parent directories will not be deleted!
     */
    /****************************************************************************/
    inline void RemoveDir() {
        RecursiveCleanupDir(m_Dir);
        QDir TheDir;
        QVERIFY(TheDir.rmdir(m_Dir));
    }
    /****************************************************************************/
    /**
     * \brief Create absolute file name from relative file name.
     *
     * \param   FileName    Relative file name.
     * \return              Absolute file name.
     */
    /****************************************************************************/
    inline QString AbsoluteFileName(const QString &FileName) {
        return m_Dir+"/"+FileName;
    }
    /****************************************************************************/
    /**
     * \brief Create a readonly file.
     *
     * \param   FileName    File name.
     */
    /****************************************************************************/
    inline void CreateReadonlyFile(const QString &FileName) {
        // create file
        QFile TheFile(FileName);
        QVERIFY(TheFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
        // set permissions to none
        QVERIFY(TheFile.setPermissions(0));
    }
}; // end class DirectoryHelper

} // end namespace DataLogging

#endif // DATALOGGING_DIRECTORYHELPER_H
