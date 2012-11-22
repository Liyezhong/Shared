/****************************************************************************/
/*! \file DataLogging/Include/EventLogger.h
 *
 *  \brief Definition file for class EventLogger.
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

#ifndef DATALOGGING_EVENTLOGGER_H
#define DATALOGGING_EVENTLOGGER_H

#include <DataLogging/Include/BaseLoggerReusable.h>
#include <DataLogging/Include/EventLoggerConfig.h>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Class used to log a received event entry to file.
 *
 * This class is used to save events to file. If necessary backups of existing
 * files are done.\n
 * <b>This class should be used only with the signal / slot mechanism.</b>
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class EventLogger : public BaseLoggerReusable {
private:
    QString         m_EventLoggerBaseFileName;  ///< Base for file name.
    qint64          m_MaxFileSize;              ///< Maximal file size. 0 means no maximal file size monitoring!
    /****************************************************************************/
    EventLogger();                                          ///< Not implemented.
    EventLogger(const EventLogger &);                       ///< Not implemented.
    const EventLogger & operator = (const EventLogger &);   ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Create a backup file.
     *
     * Create a backup file for a specific log file. The backup is done by deleting
     * the old backup file and renaming the file from xxx.log to xxx.old. If the
     * extension of the file to back up is not .log no action is done.
     *
     * \param[in]   FileName    File name of file to backup.
     */
    /****************************************************************************/
    virtual void BackupFile(const QString &FileName);
protected:
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   pParent             Parent.
     * \param[in]   TheLoggingSource    Source to set in log entry.
     */
    /****************************************************************************/
    EventLogger(LoggingObject *pParent, const Global::LoggingSource &TheLoggingSource);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~EventLogger() {
    }
    /****************************************************************************/
    /**
     * \brief Log an event entry.
     *
     * The string to log is created depending on Entry data. If size would
     * exceed maximal allowed size, a backup is made, a new file created and the
     * log string is appended to it.
     * <b>If we are the sender of this event, and the event is of type
     * Global::EVTTYPE_ERROR or Global::EVTTYPE_FATAL_ERROR, we do not try to write
     * to file, since last try resulted in this error. We put it directly to
     * console.</b>
     *
     * \param[in]   Entry   Event entry to log.
     */
    /****************************************************************************/
    void Log(const DataLogging::EventEntry &Entry);
    /****************************************************************************/
    /**
     * \brief Change configuration.
     *
     * After the configuration is saved switch to new log file.
     *
     * \param[in]   Config      New configuration for event logger.
     */
    /****************************************************************************/
    void Configure(const DataLogging::EventLoggerConfig &Config);
}; // end class EventLogger

} // end namespace DataLogging

#endif // DATALOGGING_EVENTLOGGER_H
