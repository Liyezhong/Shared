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
#include "Global/Include/EventObject.h"
#include "Global/Include/Translator.h"
#include "Global/Include/UITranslator.h"
#include "Global/Include/EventTranslator.h"

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Class used to get the daily run log file names and creating the files.
 *
 * This class creates the daily run log files and file names in the file system
 * depending on the request given by DataLoggingThreadController class
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class DayLogFileInformation {
    friend class TestDayLogFileInformation; ///< friend class for the test
private:
    QString                     m_LogFilePath;                      ///< Path of the event log files
    QString                     m_DailyRunLogPath;                  ///< Pah of the daily run log files
    QString                     m_FileNamePrefix;                   ///< Prefix of the file name
    QList<quint32>              m_ListOfEventIds;                  ///< To store common string event Ids e.g. Resolved, Acknowledged by user
    QList<quint32>              m_ListOfBtnEventIds;               ///< To store button string event Ids e.g. OK, Cancel etc.
    QList<quint32>              m_EventIDs;                         ///< Store the event ids


    /****************************************************************************/
    /**
     * \brief Constructor
     */
    /****************************************************************************/
    DayLogFileInformation();                                                    ///< Not implemented.

    Q_DISABLE_COPY(DayLogFileInformation)

    /****************************************************************************/
    /**
     * \brief Create the daily run log files and updated the list with file names.
     *
     *This creates the files in the file system. And also translates the
     *file content by using the language file
     *
     * \iparam    FileName         List of file names.
     * \oparam    ByteArray        File Data in bytes
     */
    /****************************************************************************/
    void ReadAndTranslateTheFile(const QString &FileName, const QByteArray &ByteArray);

    /****************************************************************************/
    /**
     * \brief Translates the event type
     *
     *Tranlsate the "Error", "warning", "Info" into the current system
     *language
     *
     * \oparam    TranslatedString        String needs to be translated
     */
    /****************************************************************************/
    void TranslateEventType(const QString &TranslatedString);

    /****************************************************************************/
    /**
     * \brief Translates the non active event string text
     *
     *Tranlsate the "Resolved", "Acknowledged by user" into the current
     *system language
     *
     * \oparam    TranslatedString        String needs to be translated
     *
     * \return    On successful (true) or not (false)
     */
    /****************************************************************************/
    bool FindAndTranslateNonActiveText(const QString &TranslatedString);

    /****************************************************************************/
    /**
     * \brief Translates the parameters into current system language
     *
     * \iparam    EventID               Event id that needs to be translated
     * \oparam    TranslateStringList   Translated parameters
     */
    /****************************************************************************/
    void TranslateTheParameters(const quint32 EventID, const Global::tTranslatableStringList &TranslateStringList);

    /****************************************************************************/
    /**
     * \brief Check the event ID and then translate the data into current system
     *        language
     *
     * \oparam    TranslatedString      String needs to be translated
     * \iparam    EventIDList           List of events
     * \iparam    StringList            List of strings
     *
     * \return    On successful (true) or not (false)
     */
    /****************************************************************************/
    bool CheckAndTranslateTheEventID(const QString &TranslatedString, const QList<quint32> &EventIDList,
                                     const QStringList &StringList);

public:
    /****************************************************************************/
    /**
     * \brief Constructor
     *
     * \iparam    FilePath          Path of the log file folder.
     * \iparam    DailyRunLogPath   Path of the daily run log file folder
     * \iparam    FileNamePrefix    Prefix of the file name
     */
    /****************************************************************************/
    DayLogFileInformation(QString FilePath, QString DailyRunLogPath, QString FileNamePrefix);

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
     * \brief Create the daily run log file name.
     *
     *This creates the specified file in the data stream. And also
     *translates the file content into current systemmm language
     *
     * \iparam   FileName                     Name of the file.
     * \oparam   FileContent                  Content of the daily run log file.
     */
    /****************************************************************************/
    void CreateSpecificDailyRunLogFile(const QString &FileName, const QByteArray &FileContent);

    /****************************************************************************/
    /**
     * \brief Create the daily run log files and update the list with file names.
     *
     *This creates the files in the file system. And also translates the
     *file content by using the language file
     *
     * \oparam    FileNames                    List of file names.
     */
    /****************************************************************************/
    void CreateDailyRunLogFiles(const QStringList &FileNames);

};// end class DayLogFileInformation

} // end namespace DataLogging

#endif // DATALOGGING_DAYLOGFILEINFORMATION_H
