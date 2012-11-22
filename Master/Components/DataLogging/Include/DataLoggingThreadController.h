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
#include <DataLogging/Include/EventLogger.h>
#include <DataLogging/Include/ComponentTestLogger.h>
#include <DataLogging/Include/DayEventLogger.h>

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
    EventLogger                 m_EventLogger;                      ///< Event logger.
    ComponentTestLogger         m_ComponentTestLogger;              ///< Component test logger.
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
     * \param[in]   Ref                 Command reference.
     * \param[in]   Cmd                 Command.
     */
    /****************************************************************************/
    void OnForceCaching(Global::tRefType Ref, const CmdForceCaching &Cmd);
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   TheLoggingSource    Logging source to be used.
     */
    /****************************************************************************/
    DataLoggingThreadController(Global::gSourceType TheLoggingSource);
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
     * \param[in]   OperatingMode   OperatingMode.
     */
    /****************************************************************************/
    inline void SetOperatingMode(const QString &OperatingMode) {
        m_OperatingMode = OperatingMode;
    }
    /****************************************************************************/
    /**
     * \brief Set base of file name for even logging.
     *
     * \param[in]   EventLoggerBaseFileName     Base of file name for even logging.
     */
    /****************************************************************************/
    inline void SetEventLoggerBaseFileName(const QString &EventLoggerBaseFileName) {
        m_EventLoggerBaseFileName = EventLoggerBaseFileName;
    }
    /****************************************************************************/
    /**
     * \brief Set serial number.
     *
     * \param[in]   SerialNumber    Serial number.
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
     * \param[in]   MaxFileSize     Max file size.
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
     * \param[in]   MaxFileCount    Max file count.
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
     * \brief Log an event entry.
     *
     * The EventEntry is forwarded to the event logger instance.
     *
     * \param[in]   Entry   Event entry to log.
     */
    /****************************************************************************/
    void SendToEventLogger(const DataLogging::EventEntry &Entry);
    /****************************************************************************/
    /**
     * \brief Log an component test entry.
     *
     * The ComponentTestEntry is forwarded to the component test logger instance.
     *
     * \param[in]   Entry   Component test entry to log.
     */
    /****************************************************************************/
    void SendToComponentTestLogger(const DataLogging::ComponentTestEntry &Entry);
    /****************************************************************************/
    /**
     * \brief Log an day operation entry.
     *
     * The DayEventEntry is forwarded to the day operation logger instance.
     *
     * \param[in]   Entry   Day operation entry to log.
     */
    /****************************************************************************/
    void SendToDayEventLogger(const DataLogging::DayEventEntry &Entry);
}; // end class DataLoggingThreadController

} // end namespace DataLogging

#endif // DATALOGGING_DATALOGGINGTHREADCONTROLLER_H
