/****************************************************************************/
/*! \file DataLogging/Include/DataLoggingThreadController.h
 *
 *  \brief Definition file for class DataLoggingThreadController.
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

#ifndef DATALOGGING_DATALOGGINGTHREADCONTROLLER_H
#define DATALOGGING_DATALOGGINGTHREADCONTROLLER_H

#include <Threads/Include/ThreadController.h>
#include <DataLogging/Include/DayEventLogger.h>
#include <DataLogging/Include/DayLogFileInformation.h>
#include <NetCommands/Include/CmdDayRunLogRequest.h>
#include <NetCommands/Include/CmdDayRunLogRequestFile.h>
#include <NetCommands/Include/CmdDayRunLogReply.h>
#include <NetCommands/Include/CmdDayRunLogReplyFile.h>

// DestroyObjects is the cleanup function for class DataLoggingThreadController,
// so we inform lint about this.
//
//lint -sem(DataLogging::DataLoggingThreadController::DestroyObjects, cleanup)

namespace DataLogging {

class EventFilterNetworkServer;
class CmdForceCaching;

/****************************************************************************/
/**
 * \brief This class is the controller class for the data logging thread.
 *
 * It creates, configures and destroys all the loggers: EventLogger,
 * ComponentTestLogger and DayEventLogger.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class DataLoggingThreadController : public Threads::ThreadController {
    Q_OBJECT
private:
    bool                        m_oPowerFail;                       ///< Flag indicating if power fail state active.
    QString                     m_OperatingMode;                    ///< Operating mode as string.
    QString                     m_EventLoggerBaseFileName;          ///< Base for file name for event logging.
    QString                     m_SerialNumber;                     ///< Serial number.
    qint64                      m_EventLoggerMaxFileSize;           ///< Max file size for event logger.
    int                         m_DayEventLoggerMaxFileCount;   ///< Max number of files for day operation logger.
    DayEventLogger              m_DayEventLogger;               ///< Day operation logger.
    EventFilterNetworkServer    *m_pEventFilterNetworkServer;       ///< Socket server for EventFilter.
    /****************************************************************************/
    DataLoggingThreadController();                                                          ///< Not implemented.
    DataLoggingThreadController(const DataLoggingThreadController &);                       ///< Not implemented.
    const DataLoggingThreadController & operator = (const DataLoggingThreadController &);   ///< Not implemented.

protected:
    /****************************************************************************/
    /**
     * \brief This method is called when the base class received the \ref Go signal.
     *
     * This means that everything is fine and normal operation started.
     * We are running in our own thread now.\n
     * We do not have anything special to do, since objects are already configured
     * and ready for normal operation.
     */
    /****************************************************************************/
    virtual void OnGoReceived();
    /****************************************************************************/
    /**
     * \brief This method is called when the base class received the \ref Stop signal.
     *
     * This means that normal operation will stop after processing this signal.
     * We are still running in our own thread.\n
     * We do not have anything special to do.
     */
    /****************************************************************************/
    virtual void OnStopReceived();
    /****************************************************************************/
    /**
     * \brief Power will fail shortly.
     *
     * We try to log the power fail into the event logger and close all open files.
     * After that we switch to a PowerFail state where only events are processed
     * and dumped to console.
     */
    /****************************************************************************/
    virtual void OnPowerFail();
    /****************************************************************************/
    /**
     * \brief Force caching.
     *
     * \iparam   Ref                 Command reference.
     * \iparam   Cmd                 Command.
     */
    /****************************************************************************/
    void OnForceCaching(Global::tRefType Ref, const CmdForceCaching &Cmd);
    /****************************************************************************/
    /**
     * \brief Request for daily run log file names.
     *
     * \iparam   Ref                 Command reference.
     * \iparam   Cmd                 Command.
     */
    /****************************************************************************/
    void OnRunLogRequest(Global::tRefType Ref, const NetCommands::CmdDayRunLogRequest &Cmd);
    /****************************************************************************/
    /**
     * \brief Request for daily run log file content.
     *
     * \iparam   Ref                 Command reference.
     * \iparam   Cmd                 Command.
     */
    /****************************************************************************/
    void OnRunLogRequestFile(Global::tRefType Ref, const NetCommands::CmdDayRunLogRequestFile &Cmd);
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   TheHeartBeatSource    Logging source to be used.
     */
    /****************************************************************************/
    DataLoggingThreadController(Global::gSourceType TheHeartBeatSource, const QString& fileNamePrefix);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~DataLoggingThreadController();
    /****************************************************************************/
    /**
     * \brief Set operating mode.
     *
     * \iparam   OperatingMode   OperatingMode.
     */
    /****************************************************************************/
    inline void SetOperatingMode(const QString &OperatingMode) {
        m_OperatingMode = OperatingMode;
    }
    /****************************************************************************/
    /**
     * \brief Set base of file name for even logging.
     *
     * \iparam   EventLoggerBaseFileName     Base of file name for even logging.
     */
    /****************************************************************************/
    inline void SetEventLoggerBaseFileName(const QString &EventLoggerBaseFileName) {
        m_EventLoggerBaseFileName = EventLoggerBaseFileName;
    }
    /****************************************************************************/
    /**
     * \brief Set serial number.
     *
     * \iparam   SerialNumber    Serial number.
     */
    /****************************************************************************/
    inline void SetSerialNumber(const QString &SerialNumber) {
        m_SerialNumber = SerialNumber;
    }
    /****************************************************************************/
    /**
     * \brief Set maximal file size for event logger.
     *
     * 0 means no maximal file size monitoring!
     * \iparam   MaxFileSize     Max file size.
     */
    /****************************************************************************/
    inline void SetEventLoggerMaxFileSize(qint64 MaxFileSize) {
        m_EventLoggerMaxFileSize = MaxFileSize;
    }
    /****************************************************************************/
    /**
     * \brief Set maximal file count for day operation logger.
     *
     * 0 means no maximal file count monitoring!
     * \iparam   MaxFileCount    Max file count.
     */
    /****************************************************************************/
    inline void SetDayEventLoggerMaxFileCount(int MaxFileCount) {
        m_DayEventLoggerMaxFileCount = MaxFileCount;
    }
    /****************************************************************************/
    /**
     * \brief Create and configure the loggers.
     *
     * If something goes wrong, the already created loggers are deleted.
     */
    /****************************************************************************/
    virtual void CreateAndInitializeObjects();
    /****************************************************************************/
    /**
     * \brief Cleanup and destroy the loggers.
     */
    /****************************************************************************/
    virtual void CleanupAndDestroyObjects();
public slots:
    /****************************************************************************/
    /**
     * \brief Log an day operation entry.
     *
     * The DayEventEntry is forwarded to the day operation logger instance.
     *
     * \iparam   Entry   Day operation entry to log.
     */
    /****************************************************************************/
    void SendToDayEventLogger(const DataLogging::DayEventEntry &Entry);


//    /****************************************************************************/
//    /**
//     * \brief Create and list the daily run log file names.
//     *
//     * \oparam   FileNames   List of file names.
//     */
//    /****************************************************************************/
//    void CreateAndListDailyRunLogFileName(const QStringList &FileNames);


//    /****************************************************************************/
//    /**
//     * \brief Create and list the daily run log file names.
//     *        This creates the specified file in the data stream. And also translates the
//     *        file content by using the language file
//     *
//     * \iparam   FileName          Name of the file.
//     * \iparam   LanguageFilePath  Path of the language file and path of the file.
//     * \oparam   FileContent       Content of the daily run log file.
//     */
//    /****************************************************************************/
//    void CreateSpecificDailyRunLogFile(const QString &FileName, const QString &LanguageFilePath,
//                                       const QDataStream &FileContent);

//    /****************************************************************************/
//    /**
//     * \brief Create the daily run log files and updated the list with file names.
//     *        This creates the files in the file system. And also translates the
//     *        file content by using the language file
//     *
//     * \iparam    LanguageFilePath  Path of the language file and path of the file.
//     * \oparam    FileNames         List of file names.
//     */
//    /****************************************************************************/
//    void CreateDailyRunLogFiles(const QString &LanguageFilePath,
//                                       const QStringList &FileNames);



}; // end class DataLoggingThreadController

} // end namespace DataLogging

#endif // DATALOGGING_DATALOGGINGTHREADCONTROLLER_H
