/****************************************************************************/
/*! \file DataLogging/Include/DayEventLogger.h
 *
 *  \brief Definition file for class DayEventLogger.
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

#ifndef DATALOGGING_DAYOPERATIONLOGGER_H
#define DATALOGGING_DAYOPERATIONLOGGER_H

#include <DataLogging/Include/DayEventEntry.h>
#include <DataLogging/Include/BaseLoggerReusable.h>
#include <DataLogging/Include/DayEventLoggerConfig.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Class used to log day operation data to file.
 *
 * Day operation data is translated into the right language (depending on the
 * operating mode) and written to file.\n
 * <b>This class should be used only with the signal / slot mechanism.</b>
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class DayEventLogger : public BaseLoggerReusable {

private:
    QDate   m_LastLogDate;      ///< Date when last logging was done.
    QDate   m_LastLogDateBackUp;      ///< Backup of lastlog date
    int     m_MaxFileCount;     ///< Maximal file count. 0 means no maximal file count monitoring!
    QString m_FileNamePrefix;   ///< Prefix of the file name
    bool m_FlushImmediately;           ///< Flush the data to disk as soon as it receives the data
    /****************************************************************************/
    /****************************************************************************/
    /**
     * \brief Constructor
     */
    /****************************************************************************/
    DayEventLogger();                                                   ///< Not implemented.

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(DayEventLogger)

    /****************************************************************************/
    /**
     * \brief Compute new file name.
     *
     * \return      New file name.
     */
    /****************************************************************************/
    QString ComputeFileName() const;

    /****************************************************************************/
    /**
     * \brief Compute file name regular expression for searching old files.
     *
     * \return      File name regular expression.
     */
    /****************************************************************************/
    QString ComputeFileNameRegExp() const;

    /****************************************************************************/
    /**
     * \brief Switch to new file.
     *
     * For that, we check if old files exits and have to be deleted and delete them
     * if necessary. Then we compute the new file name depending on current date and
     * Switch to new file making a backup if necessary
     */
    /****************************************************************************/
    void SwitchToNewFile();


    /****************************************************************************/
    /**
     * \brief Creates the temporary log file
     *
     * If any error occurs while creating the log files or if any event repeated
     * which is belonging to data logging component(for file opening, flushing and
     * writing the data) then it created the temporary log file
     */
    /****************************************************************************/
    void CreateTemporaryLogFile();
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   pParent             Parent.
     * \iparam   TheLoggingSource    Source to set in log entry.
     * \iparam   FileNamePrefix      Prefix of the file name
     */
    /****************************************************************************/
    DayEventLogger(QObject *pParent, const QString & TheLoggingSource, const QString& FileNamePrefix);

    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~DayEventLogger() {
    }

    /****************************************************************************/
    /**
     * \brief Change configuration.
     *
     * After the configuration is saved switch to new log file.
     *
     * \iparam   Config      New configuration for event logger.
     */
    /****************************************************************************/
    void Configure(const DataLogging::DayEventLoggerConfig &Config);

    /****************************************************************************/
    /**
     * \brief Flush the data immediately
     *
     * \iparam   Enable      Logs the data immediately.
     */
    /****************************************************************************/
    void FlushDataToFile(bool Enable);


    /****************************************************************************/
    /**
     * \brief Log an event entry.
     *
     * The string to log is created depending on Entry data and appended to the
     * log file. If date changed it will be switched to a new log file.
     *
     * \iparam   Entry   Event entry to log.
     */
    /****************************************************************************/
    void Log(const DataLogging::DayEventEntry &Entry);

    /****************************************************************************/
    /**
     * \brief Checks whether data logging enabled. If not enabled then it raises
     *        event to GUI
     *
     */
    /****************************************************************************/
    void CheckLoggingEnabled();

    /****************************************************************************/
    /**
     * \brief Removes outdated files from the system
     *
     * \iparam      Prefix      Prefix of the file name
     * \iparam      DaysBack    Number of days
     */
    /****************************************************************************/
    static void RemoveOutdatedFiles(QString Prefix, quint8 DaysBack);

}; // end class DayEventLogger

} // end namespace DataLogging

#endif // DATALOGGING_DAYEVENTLOGGER_H
