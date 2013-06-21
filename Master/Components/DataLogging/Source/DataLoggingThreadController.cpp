/****************************************************************************/
/*! \file DataLogging/Source/DataLoggingThreadController.cpp
 *
 *  \brief Implementation file for class DataLoggingThreadController.
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

#include <DataLogging/Include/DataLoggingThreadController.h>
#include <DataLogging/Include/EventFilterNetworkServer.h>
#include <DataLogging/Include/Commands/CmdForceCaching.h>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/Utils.h>



#include <QMetaType>

namespace DataLogging {

const Global::gSubComponentType SUBCOMPONENT_EVENTLOGGER    = 0x0001;   ///< Subcomponent id for event logger.
const Global::gSubComponentType SUBCOMPONENT_DAYOPLOGGER    = 0x0002;   ///< Subcomponent id for day operation logger.
const Global::gSubComponentType SUBCOMPONENT_COMPTESTLOGGER = 0x0003;   ///< Subcomponent id for component test logger.
const Global::gSourceType DUMMY =100;
/****************************************************************************/
DataLoggingThreadController::DataLoggingThreadController(Global::gSourceType TheHeartBeatSource, const QString& fileNamePrefix) :
    Threads::ThreadController(TheHeartBeatSource, "DataLogging"),
    m_oPowerFail(false),
    m_EventLoggerMaxFileSize(0),
    m_DayEventLoggerMaxFileCount(0),
    m_DayEventLogger(NULL, "DataLogging", fileNamePrefix),
    m_pEventFilterNetworkServer(NULL)
{

    qRegisterMetaType<DataLogging::DayEventEntry>("DataLogging::DayEventEntry");

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
    // first of all create objects
    // pointers should be NULL at this point.
    Q_ASSERT(m_pEventFilterNetworkServer == NULL);

#if !defined QT_NO_DEBUG
    // create network filter server but only in debug mode
    m_pEventFilterNetworkServer = new EventFilterNetworkServer(this, 9876);
#endif

    QDir Directory(Global::SystemPaths::Instance().GetLogfilesPath());
    // check the path of the log directory
    if (!Directory.exists()) {
        if (!Directory.mkdir(Directory.absolutePath())) {
            /// \todo Raise a exception
            // return from the function, there is no point configure the logger
            return;
        }
    }

    m_DayEventLogger.Configure(DayEventLoggerConfig(m_OperatingMode,
                                                            m_SerialNumber,
                                                            Global::SystemPaths::Instance().GetLogfilesPath(),
                                                            m_DayEventLoggerMaxFileCount,
                                                            m_EventLoggerBaseFileName));

    RegisterCommandForProcessing<DataLogging::CmdForceCaching, DataLoggingThreadController>
            (&DataLoggingThreadController::OnForceCaching, this);
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
    // nothing to do
}

/****************************************************************************/
void DataLoggingThreadController::OnStopReceived() {
    // nothing to do
}

/****************************************************************************/
void DataLoggingThreadController::OnPowerFail() {
    // try to log
    //! \todo inform EventHandler
    m_DayEventLogger.CloseFile();
    // switch to state power fail
    m_oPowerFail = true;
}

/****************************************************************************/
void DataLoggingThreadController::SendToDayEventLogger(const DayEventEntry &Entry) {
    // silently discard entry if power fails
    if(!m_oPowerFail) {
        m_DayEventLogger.Log(Entry);
    }
}

/****************************************************************************/
void DataLoggingThreadController::CheckLoggingEnabled() {
    m_DayEventLogger.CheckLoggingEnabled();
}

/****************************************************************************/
void DataLoggingThreadController::OnForceCaching(Global::tRefType Ref, const CmdForceCaching &Cmd) {
    /// \todo implement
    Q_UNUSED(Ref);
    Q_UNUSED(Cmd);
}


/****************************************************************************/
void DataLoggingThreadController::OnRunLogRequest(Global::tRefType Ref, const NetCommands::CmdDayRunLogRequest &Cmd) {

    Q_UNUSED(Cmd);
    // send the acknowledgement
    SendAcknowledgeOK(Ref);
    // create object to create the file
    DayLogFileInformation DayRunFilesInformation(Global::SystemPaths::Instance().GetLogfilesPath());

    QStringList FileNames;
    DayRunFilesInformation.CreateAndListDailyRunLogFileName(FileNames);
    // get the new command reference
    Global::tRefType NewRef = GetNewCommandRef();
    SendCommand(NewRef, Global::CommandShPtr_t(new NetCommands::CmdDayRunLogReply(1000, FileNames)));
}

/****************************************************************************/
void DataLoggingThreadController::OnRunLogRequestFile(Global::tRefType Ref, const NetCommands::CmdDayRunLogRequestFile &Cmd) {

    // send the acknowledgement
    SendAcknowledgeOK(Ref);

    // create object to create the file
    DayLogFileInformation DayRunFilesInformation(Global::SystemPaths::Instance().GetLogfilesPath());

    QByteArray FileContent;

    DayRunFilesInformation.CreateSpecificDailyRunLogFile(Cmd.GetFileName(), FileContent);
    // get the new command reference
    Global::tRefType NewRef = GetNewCommandRef();
    SendCommand(NewRef, Global::CommandShPtr_t(new NetCommands::CmdDayRunLogReplyFile(1000, FileContent)));
}

/****************************************************************************/
void DataLoggingThreadController::OnExportDayRunLogRequest(Global::tRefType Ref, const NetCommands::CmdExportDayRunLogRequest &Cmd) {

    Q_UNUSED(Cmd);
    // send the acknowledgement
    SendAcknowledgeOK(Ref);
    // create object to create the file
    DayLogFileInformation DayRunFilesInformation(Global::SystemPaths::Instance().GetLogfilesPath());

    QStringList FileNames;
    DayRunFilesInformation.CreateDailyRunLogFiles(FileNames);
    // get the new command reference
    Global::tRefType NewRef = GetNewCommandRef();
    SendCommand(NewRef, Global::CommandShPtr_t(new NetCommands::CmdExportDayRunLogReply(1000, FileNames)));
}

} // end namespace DataLogging

