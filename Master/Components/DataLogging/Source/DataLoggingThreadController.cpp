/****************************************************************************/
/*! \file DataLogging/Source/DataLoggingThreadController.cpp
 *
 *  \brief Implementation file for class DataLoggingThreadController.
 *
 *\b Description:
 *      This class creates, configures and destroys all the loggers: EventLogger,
 *      ComponentTestLogger and DayEventLogger.
 *
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

#include <DataLogging/Include/DataLoggingThreadController.h>
#include <DataLogging/Include/EventFilterNetworkServer.h>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/Utils.h>
#include <Global/Include/GlobalDefines.h>
#include <DataLogging/Include/DataLoggingEventCodes.h>



#include <QMetaType>

namespace DataLogging {

const Global::gSubComponentType SUBCOMPONENT_EVENTLOGGER    = 0x0001;   ///< Subcomponent id for event logger.
const Global::gSubComponentType SUBCOMPONENT_DAYOPLOGGER    = 0x0002;   ///< Subcomponent id for day operation logger.
const Global::gSubComponentType SUBCOMPONENT_COMPTESTLOGGER = 0x0003;   ///< Subcomponent id for component test logger.
const QString EMPTY_STRING                  = "";///< empty string
const QString FILEEXTENSION_LOG             = ".log";///< Extension for the log files
const QString MULTIPLEFILES                 = "*.log";///< Multiple log files string
const QString STRINGUNDERSCORE              = "_";///< Underscore string



/****************************************************************************/
DataLoggingThreadController::DataLoggingThreadController(quint32 ThreadID,
                                                         const QString& FileNamePrefix) :
    Threads::ThreadController(ThreadID, "DataLogging"),
    m_oPowerFail(false),
    m_EventLoggerMaxFileSize(0),
    m_DayEventLoggerMaxFileCount(0),
    mp_DayEventLogger(NULL),
    m_pEventFilterNetworkServer(NULL) {
    mp_DayEventLogger = new DayEventLogger(this, "DataLogging",FileNamePrefix);
    qRegisterMetaType<DataLogging::DayEventEntry>("DataLogging::DayEventEntry");
    m_ImmediateLog = false;

}

/****************************************************************************/
DataLoggingThreadController::~DataLoggingThreadController() {
    try {
        delete m_pEventFilterNetworkServer;
        m_pEventFilterNetworkServer = NULL;
    } catch(...) {
    }
}

/****************************************************************************/
void DataLoggingThreadController::CreateAndInitializeObjects() {

    Global::EventObject::Instance().RaiseEvent(Global::EVENT_THREAD_CREATE_AND_INITIALIZE, Global::FmtArgs() << "Datalogging controller");
#if !defined QT_NO_DEBUG
    // create network filter server but only in debug mode
    m_pEventFilterNetworkServer = new EventFilterNetworkServer(this, 9876);
#endif

    // create the log files folder
    QDir Directory(Global::SystemPaths::Instance().GetLogfilesPath());
    // check the path of the log directory
    if (!Directory.exists()) {
        if (!Directory.mkdir(Directory.absolutePath())) {
            // disable permanently and inform to GUI
            Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_DATA_LOGGING_DISABLED);
            // return from the function, there is no point configure the logger
            return;
        }
    }

    mp_DayEventLogger->Configure(DayEventLoggerConfig(m_OperatingMode,
                                                    m_SerialNumber,
                                                    m_SWVersion,
                                                    Global::SystemPaths::Instance().GetLogfilesPath(),
                                                    m_DayEventLoggerMaxFileCount,
                                                    m_EventLoggerBaseFileName));
    mp_DayEventLogger->FlushDataToFile(m_ImmediateLog);
    RegisterAcknowledgeForProcessing<Global::AckOKNOK, DataLoggingThreadController>
            (&DataLoggingThreadController::OnAcknowledge, this);
    RegisterCommandForProcessing<NetCommands::CmdDayRunLogRequest, DataLoggingThreadController>
            (&DataLoggingThreadController::OnRunLogRequest, this);
    RegisterCommandForProcessing<NetCommands::CmdDayRunLogRequestFile, DataLoggingThreadController>
            (&DataLoggingThreadController::OnRunLogRequestFile, this);
    RegisterCommandForProcessing<NetCommands::CmdExportDayRunLogRequest, DataLoggingThreadController>
            (&DataLoggingThreadController::OnExportDayRunLogRequest, this);

}

/****************************************************************************/
void DataLoggingThreadController::CleanupAndDestroyObjects() {
}

/****************************************************************************/
void DataLoggingThreadController::OnGoReceived() {
    Global::EventObject::Instance().RaiseEvent(Global::EVENT_THREAD_ON_GO_RECEIVED, Global::FmtArgs() << "Datalogging controller");
    Global::SetThreadPriority(Global::LOW_PRIO);
    // nothing to do
}

/****************************************************************************/
void DataLoggingThreadController::OnStopReceived() {
    mp_DayEventLogger->CloseFile();
}

/****************************************************************************/
void DataLoggingThreadController::OnPowerFail(const Global::PowerFailStages PowerFailStage) {
    // switch to state power fail
    if (PowerFailStage == Global::POWER_FAIL_STAGE_2) {
        mp_DayEventLogger->CloseFile();
        m_oPowerFail = true;
    }
}

/****************************************************************************/
void DataLoggingThreadController::OnAcknowledge(Global::tRefType Ref, const Global::AckOKNOK &Ack)
{
    Q_UNUSED(Ref)
    Q_UNUSED(Ack)
}

/****************************************************************************/
void DataLoggingThreadController::SendToDayEventLogger(const DayEventEntry &Entry) {
    // silently discard entry if power fails
    if(!m_oPowerFail) {
        QString TrEventMessage;
        mp_DayEventLogger->Log(Entry, TrEventMessage);
        // check signal connection
        if (receivers(SIGNAL(ForwardEventToRemoteCare(const DataLogging::DayEventEntry&, const QString &))) == 0)
            return;

        emit ForwardEventToRemoteCare(Entry, TrEventMessage);
    }
}

/****************************************************************************/
void DataLoggingThreadController::CheckLoggingEnabled() {
    mp_DayEventLogger->CheckLoggingEnabled();
}


/****************************************************************************/
void DataLoggingThreadController::OnRunLogRequest(Global::tRefType Ref,
                                                  const NetCommands::CmdDayRunLogRequest &Cmd) {

    Q_UNUSED(Cmd);
    // send the acknowledgement
    SendAcknowledgeOK(Ref);

    QStringList FileNames;
    QDir LogDirectory(Global::SystemPaths::Instance().GetLogfilesPath());

    // check the user level - for service user raw data(event log) shall be displayed
    switch(Cmd.GetUserLevel()) {
        case Global::SERVICE:
            // iterate each file and remove the log extension
            foreach (QString LogFileName,
                     LogDirectory.entryList(QStringList() << (m_EventLoggerBaseFileName + m_SerialNumber + STRINGUNDERSCORE + MULTIPLEFILES))) {
                LogFileName.replace(FILEEXTENSION_LOG, EMPTY_STRING);
                // replace the .log extension and put the empty string
                FileNames.append(LogFileName);
            }
            qSort(FileNames.begin(), FileNames.end(), qGreater<QString>());
            break;
        default:
        {
            DayLogFileInformation *p_DayRunFilesInformation = NULL;
            // create object to create the file
            p_DayRunFilesInformation = new DayLogFileInformation(Global::SystemPaths::Instance().GetLogfilesPath(),
                                                                 Global::SystemPaths::Instance().GetLogfilesPath(),
                                                                 m_EventLoggerBaseFileName + m_SerialNumber + STRINGUNDERSCORE);

            p_DayRunFilesInformation->CreateAndListDailyRunLogFileName(FileNames);
            delete p_DayRunFilesInformation;
            break;
        }
    }

    // get the new command reference
    Global::tRefType NewRef = GetNewCommandRef();
    SendCommand(NewRef, Global::CommandShPtr_t(new NetCommands::CmdDayRunLogReply(1000, FileNames)));
}

/****************************************************************************/
void DataLoggingThreadController::OnRunLogRequestFile(Global::tRefType Ref,
                                                      const NetCommands::CmdDayRunLogRequestFile &Cmd) {

    QByteArray FileContent;

    QFile File;
    // first flush the data to disk then do further processing
    mp_DayEventLogger->FlushToDisk();
    // check the user level - for service user raw data (event log) shall be displayed
    switch(Cmd.GetUserLevel()) {
        case Global::SERVICE:
            File.setFileName(Global::SystemPaths::Instance().GetLogfilesPath() + QDir::separator()
                             + Cmd.GetFileName() + FILEEXTENSION_LOG);
            if (File.open(QIODevice::ReadOnly)) {
                FileContent = File.readAll();
            }
            break;
        default:
        {            
            DayLogFileInformation *p_DayRunFilesInformation = NULL;
            // create object to create the file
            p_DayRunFilesInformation = new DayLogFileInformation(Global::SystemPaths::Instance().GetLogfilesPath(),
                                                                 Global::SystemPaths::Instance().GetLogfilesPath(),
                                                                 m_EventLoggerBaseFileName + m_SerialNumber + STRINGUNDERSCORE);

            p_DayRunFilesInformation->CreateSpecificDailyRunLogFile(Cmd.GetFileName(), FileContent);
            delete p_DayRunFilesInformation;
            break;
        }
    }
    // get the new command reference
    Global::tRefType NewRef = GetNewCommandRef();
    SendCommand(NewRef, Global::CommandShPtr_t(new NetCommands::CmdDayRunLogReplyFile(1000, FileContent)));
    // send the acknowledgement
    SendAcknowledgeOK(Ref);
}

/****************************************************************************/
void DataLoggingThreadController::OnExportDayRunLogRequest(Global::tRefType Ref,
                                                           const NetCommands::CmdExportDayRunLogRequest &Cmd) {

    Q_UNUSED(Cmd);
    // send the acknowledgement
    SendAcknowledgeOK(Ref);
    // first flush the data to disk then do further processing
    mp_DayEventLogger->FlushToDisk();
    // create object to create the file
    DayLogFileInformation DayRunFilesInformation(Global::SystemPaths::Instance().GetLogfilesPath(),
                                                 Cmd.GetFolderPath(), m_EventLoggerBaseFileName + m_SerialNumber + STRINGUNDERSCORE);

    QStringList FileNames;
    DayRunFilesInformation.CreateDailyRunLogFiles(FileNames);
    // get the new command reference
    Global::tRefType NewRef = GetNewCommandRef();
    SendCommand(NewRef, Global::CommandShPtr_t(new NetCommands::CmdExportDayRunLogReply(5000, FileNames)));
}

} // end namespace DataLogging

