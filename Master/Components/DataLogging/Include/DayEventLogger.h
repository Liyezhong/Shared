/****************************************************************************/
/*! \file DataLogging/Include/DayEventLogger.h
 *
 *  \brief Definition file for class DayEventLogger.
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
    bool    m_RemoveOldestFile; ///< If Set to true - delete oldest file
    QString m_FileNamePrefix;
    /****************************************************************************/
    DayEventLogger();                                                   ///< Not implemented.
    DayEventLogger(const DayEventLogger &);                         ///< Not implemented.
    const DayEventLogger & operator = (const DayEventLogger &);     ///< Not implemented.
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
protected:
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   pParent             Parent.
     * \iparam   TheLoggingSource    Source to set in log entry.
     */
    /****************************************************************************/
    DayEventLogger(Global::EventObject *pParent, const QString & TheLoggingSource, const QString& fileNamePrefix);
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
     */
    /****************************************************************************/
    void CheckLoggingEnabled();


}; // end class DayEventLogger

} // end namespace DataLogging

#endif // DATALOGGING_DAYEVENTLOGGER_H
