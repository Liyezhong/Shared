/****************************************************************************/
/*! \file DataLogging/Include/DayLogFileInformation.h
 *
 *  \brief Definition file for class DayLogFileInformation.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-10-12
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

#ifndef DATALOGGING_DAYLOGFILEINFORMATION_H
#define DATALOGGING_DAYLOGFILEINFORMATION_H

#include <QString>
#include <QHash>
#include <Global/Include/GlobalDefines.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Class used to get the daily run log file names and creating the files.
 *
 * This class created the daily run log files and file names in the file system
 * depending on the request given by DataLoggingThreadController class
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class DayLogFileInformation {
    friend class TestDayLogFileInformation; ///< friend class for the test
private:
    QString                     m_LogFilePath;                      ///< Pah of the vent log files

    DayLogFileInformation();                                                    ///< Not implemented.
    DayLogFileInformation(const DayLogFileInformation &);                       ///< Not implemented.
    const DayLogFileInformation & operator = (const DayLogFileInformation &);   ///< Not implemented.

    /****************************************************************************/
    /**
     * \brief Create the daily run log files and updated the list with file names.
     *        This creates the files in the file system. And also translates the
     *        file content by using the language file
     *
     * \iparam    FileName         List of file names.
     * \iparam    UserRole         the current user role
     * \oparam    ByteArray        File Data in bytes
     */
    /****************************************************************************/
    void ReadAndTranslateTheFile(const QString &FileName, Global::GuiUserLevel UserRole, const QByteArray &ByteArray);

public:
    /****************************************************************************/
    /**
     * \brief Constructor
     *
     * \iparam    FilePath  Path of the log file folder.
     */
    /****************************************************************************/
    DayLogFileInformation(QString FilePath);

    /****************************************************************************/
    /**
     * \brief Destructor
     */
    /****************************************************************************/
    ~DayLogFileInformation() {

    }

    /****************************************************************************/
    /**
     * \brief Create and list the daily run log file names.
     *
     * \oparam   FileNames   List of file names.
     */
    /****************************************************************************/
    void CreateAndListDailyRunLogFileName(const QStringList &FileNames);

    /****************************************************************************/
    /**
     * \brief Create and list the daily run log file names.
     *        This creates the specified file in the data stream. And also translates the
     *        file content by using the language file
     *
     * \iparam   FileName                     Name of the file.
     * \iparam    UserRole         the current user role
     * \oparam   FileContent                  Content of the daily run log file.
     */
    /****************************************************************************/
    void CreateSpecificDailyRunLogFile(const QString &FileName, Global::GuiUserLevel UserRole, const QByteArray &FileContent);

    /****************************************************************************/
    /**
     * \brief Create the daily run log files and updated the list with file names.
     *        This creates the files in the file system. And also translates the
     *        file content by using the language file
     *
     * \oparam    FileNames                    List of file names.
     */
    /****************************************************************************/
    void CreateDailyRunLogFiles(const QStringList &FileNames, Global::GuiUserLevel CurrentUserRole);

};// end class DayLogFileInformation

} // end namespace DataLogging

#endif // DATALOGGING_DAYLOGFILEINFORMATION_H
