/****************************************************************************/
/*! \file MainThreadController.h
 * 
 *  \brief Definition file for class MainThreadController.
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

#ifndef _MAINTHREADCONTROLLER_H_
#define _MAINTHREADCONTROLLER_H_

#include <Threads/Include/ThreadController.h>
#include <DataLogging/Include/DataLoggingThreadController.h>
#include <EventHandler/Include/EventHandler.h>
#include <EventHandler/Include/ActionEntry.h>

#include <QStack>
#include <QPair>

using namespace DataLogging;
using namespace EventHandler;

namespace Threads {

typedef QPair< ThreadController *, QThread * >  tControllerPair;    ///< Typedef for a pair consisting of a thread controller and a thread. Both as pointers.
typedef QVector<tControllerPair>                tControllerVector;  ///< Vector of tControllerPair.

/****************************************************************************/
/**
 * \brief This class is the base class for main threads.
 *
 * It will handle the data logging and event handling threads and their controllers.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class MainThreadController : public ThreadController {
    Q_OBJECT
private:
    QString                                             m_SettingsPath;                  ///< Settings directory.
    QString                                             m_FirmwarePath;                  ///< Firmware directory.
    QString                                             m_UpdatePath;                    ///< Update directory.
    QString                                             m_LogfilesPath;                  ///< Log files directory.
    QString                                             m_ComponentTestPath;             ///< Component test directory.
    QString                                             m_ManualPath;                    ///< manual / help directory.
    qint64                                              m_EventLoggerMaxFileSize;        ///< Max file size for event logger.
    quint16                                             m_DayOperationLoggerMaxFileAge;  ///< Max day operation logger file age.
    DataLoggingThreadController::LoggingSourceContainer m_LoggingSourcesDataLogging;     ///< Logging sources for DataLogging component.
    DataLoggingThreadController                         *m_pDataLoggingThreadController; ///< Pointer to DataLoggingThreadController.
    tSourceType                                         m_LoggingSourceEventHandler;     ///< Logging Source ID of the Event Handler.
    EventHandlerThreadController                        *m_pEventThreadController;       ///< Pointer to the system event handling object.
    tControllerVector                                   m_Controllers;                   ///< Here be all Controllers...
    /****************************************************************************/
    MainThreadController();                                                 ///< Not implemented.
    MainThreadController(const MainThreadController &);                     ///< Not implemented.
    const MainThreadController & operator = (const MainThreadController &); ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Initialize all controllers in the order they were created.
     *
     * Calls \ref CreateAndInitializeObjects for each.
     */
    /****************************************************************************/
    bool InitializeControllers();
    /****************************************************************************/
    /**
     * \brief Cleanup all controllers in the reverse order they were initialized.
     *
     * Calls \ref CleanupAndDestroyObjects for each controller.
     */
    /****************************************************************************/
    void CleanupControllers();
    /****************************************************************************/
    /**
     * \brief Attach controllers to corresponding threads and start threads.
     */
    /****************************************************************************/
    void AttachControllersAndStartThreads();
    /****************************************************************************/
    /**
     * \brief Stop threads and wait for their termination.
     *
     * We wait indefinitely. It might be a good idea to implement a timeout.
     */
    /****************************************************************************/
    void StopThreadsAndWait();
    /****************************************************************************/
    /**
     * \brief Connect data logging signals.
     *
     * We connect the data logging signals (see \ref LoggingObject) of a controller
     * to the event handler controller (\ref EmitEventEntry) and to the
     * data logger controller (\ref EmitDayOperationEntry and \ref EmitComponentTestEntry)
     */
    /****************************************************************************/
    void ConnectDataLoggingSignals(ThreadController *pController);
signals:
    /****************************************************************************/
    /**
     * \brief Go signal for all connected threads.
     */
    /****************************************************************************/
    void SendGo();
    /****************************************************************************/
    /**
     * \brief Stop signal for all connected threads.
     */
    /****************************************************************************/
    void SendStop();
    /****************************************************************************/
    /**
     * \brief Emit this signal if you want to terminate yourself.
     *
     * It must be processed outside of this object. A good idea is to connect this
     * signal with this controller's thread quit() slot. This could be done in the
     * main function of the application.
     */
    /****************************************************************************/
    void RequestFinish();
protected:
    /****************************************************************************/
    /**
     * \brief This method is called when the base received the \ref Go signal.
     *
     * This means that everything is fine and normal operation started.
     * We are running in our own thread.\n
     * We create all controllers used (the according method in derived classes
     * \ref CreateControllersAndThreads is also called), then initialize the controllers
     * and finally attach them to the corresponding threads and start them.\n
     * If something goes wrong, the main thread (and application) will be stopped.
     */
    /****************************************************************************/
    virtual void OnGoReceived();
    /****************************************************************************/
    /**
     * \brief This method is called when the base class received the \ref Stop signal.
     *
     * This means that normal operation will stop after processing this signal.
     * We are still running in our own thread.\n
     * We make some cleanup work (all used controllers) and finally destroy all
     * controllers and threads.
     */
    /****************************************************************************/
    virtual void OnStopReceived();
    /****************************************************************************/
    /**
     * \brief Create all controllers and threads handled by us.
     *
     * We create all controllers handled by us (data logging and event handler)
     * and connect them.
     *
     * \return  True on success.
     */
    /****************************************************************************/
    virtual bool CreateControllersAndThreads();
    /****************************************************************************/
    /**
     * \brief Destroy all registered controllers and threads.
     */
    /****************************************************************************/
    void DestroyControllersAndThreads();
    /****************************************************************************/
    /**
     * \brief Add controller and thread to list of handled controllers.
     *
     * This list is used to perform common stuff to all controllers and threads:
     * connecting their \ref Go and \ref Stop slots, connecting them to the
     * data logging mechanism and so on.
     */
    /****************************************************************************/
    void AddAndConnectController(ThreadController *pController, QThread *pThread);
    /****************************************************************************/
    /**
     * \brief Get settings path.
     *
     * \return  Settings path.
     */
    /****************************************************************************/
    inline QString GetSettingsPath() const {
        return m_SettingsPath;
    }
    /****************************************************************************/
    /**
     * \brief Set firmware path.
     *
     * \return  Firmware path.
     */
    /****************************************************************************/
    inline QString GetFirmwarePath() const {
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
        return m_ManualPath;
    }
    /****************************************************************************/
    /**
     * \brief Get maximal file size for event logger.
     *
     * \return  Max file size.
     */
    /****************************************************************************/
    inline qint64 GetEventLoggerMaxFileSize() const {
        return m_EventLoggerMaxFileSize;
    }
    /****************************************************************************/
    /**
     * \brief Get maximal file age for day operation logger.
     *
     * \return  Max file age.
     */
    /****************************************************************************/
    inline qint16 GetDayOperationLoggerMaxFileAge() const {
        return m_DayOperationLoggerMaxFileAge;
    }
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   LoggingSource               Source to set in log entry.
     * \param[in]   LoggingSourcesDataLogging   Sources for data logging component.
     * \param[in]   LoggingSourceEventHandler   Source for event handler component.
     */
    /****************************************************************************/
    MainThreadController(tSourceType LoggingSource,
                         const DataLoggingThreadController::LoggingSourceContainer & LoggingSourcesDataLogging,
                         tSourceType LoggingSourceEventHandler);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~MainThreadController();
    /****************************************************************************/
    /**
     * \brief Create and initialize used objects.
     *
     * Create and configure all needed objects, but no controllers!
     * \return      true on success
     */
    /****************************************************************************/
    inline virtual bool CreateAndInitializeObjects() {
        // nothing to do at this point.
        return true;
    }
    /****************************************************************************/
    /**
     * \brief Cleanup and destroy used objects.
     */
    /****************************************************************************/
    inline virtual void CleanupAndDestroyObjects() {
        // nothing to do
    }
    /****************************************************************************/
    /**
     * \brief Set settings path.
     *
     * Usually "Settings".
     * <b>The application path is prepended automatically!</b>
     *
     * \param   SettingsPath    The path.
     */
    /****************************************************************************/
    inline void SetSettingsPath(const QString &SettingsPath) {
        m_SettingsPath = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/" + SettingsPath);
    }
    /****************************************************************************/
    /**
     * \brief Set firmware path.
     *
     * Usually "Firmware".
     * <b>The application path is prepended automatically!</b>
     *
     * \param   FirmwarePath    The path.
     */
    /****************************************************************************/
    inline void SetFirmwarePath(const QString &FirmwarePath) {
        m_FirmwarePath = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/" + FirmwarePath);
    }
    /****************************************************************************/
    /**
     * \brief Set update path.
     *
     * Usually "Update".
     * <b>The application path is prepended automatically!</b>
     *
     * \param   UpdatePath      The path.
     */
    /****************************************************************************/
    inline void SetUpdatePath(const QString &UpdatePath){
        m_UpdatePath = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/" + UpdatePath);
    }
    /****************************************************************************/
    /**
     * \brief Set log files path.
     *
     * Usually "Logfiles".
     * <b>The application path is prepended automatically!</b>
     *
     * \param   LogfilesPath    The path.
     */
    /****************************************************************************/
    inline void SetLogfilesPath(const QString &LogfilesPath) {
        m_LogfilesPath = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/" + LogfilesPath);
    }
    /****************************************************************************/
    /**
     * \brief Set component test path.
     *
     * Usually "Tests".
     * <b>The application path is prepended automatically!</b>
     *
     * \param   ComponentTestPath   The path.
     */
    /****************************************************************************/
    inline void SetComponentTestPath(const QString &ComponentTestPath) {
        m_ComponentTestPath = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/" + ComponentTestPath);
    }
    /****************************************************************************/
    /**
     * \brief Set manual / help path.
     *
     * Usually "Manual".
     * <b>The application path is prepended automatically!</b>
     *
     * \param   ManualPath      The path.
     */
    /****************************************************************************/
    inline void SetManualPath(const QString &ManualPath) {
        m_ManualPath = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/" + ManualPath);
    }
    /****************************************************************************/
    /**
     * \brief Set maximal file size for event logger.
     *
     * 0 means no maximal file size monitoring!
     * \param   MaxFileSize     Max file size.
     */
    /****************************************************************************/
    inline void SetEventLoggerMaxFileSize(qint64 MaxFileSize) {
        m_EventLoggerMaxFileSize = MaxFileSize;
    }
    /****************************************************************************/
    /**
     * \brief Set maximal file age for day operation logger.
     *
     * 0 means no maximal file age monitoring!
     * \param   MaxFileAge     Max file age.
     */
    /****************************************************************************/
    inline void SetDayOperationLoggerMaxFileAge(qint16 MaxFileAge) {
        m_DayOperationLoggerMaxFileAge = MaxFileAge;
    }
public slots:
    /****************************************************************************/
    /**
     * \brief Terminate.
     *
     * Try to shutdown in a graceful way: call stop on all threads, terminate them
     * and wait for their termination. After that we call out own Stop method
     * and send a terminate request.
     */
    /****************************************************************************/
    void Terminate();
    /****************************************************************************/
    /**
     * \brief   API for receiving local system events.
     *
     *          See detailed description in the Base Class's header.
     *
     * \param[in]  TheEventEntry = event from one of local objects
     */
    /****************************************************************************/
    virtual void ReceiveEventEntry(const DataLogging::EventEntry & TheEventEntry) = 0;

    /****************************************************************************/
    /**
     * \brief API for receiving global system action.
     *
     *        See detailed description in the Base Class's header.
     *
     * \param[in]  ActionEntry = action from the system event handler
     */
    /****************************************************************************/
    void ReceiveAction(const ActionEntry &);
};

} // namespace Threads {

#endif // _MAINTHREADCONTROLLER_H_
