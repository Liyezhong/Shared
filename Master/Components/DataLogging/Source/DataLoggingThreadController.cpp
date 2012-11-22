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

/****************************************************************************/
DataLoggingThreadController::DataLoggingThreadController(Global::gSourceType TheLoggingSource) :
    Threads::ThreadController(TheLoggingSource, "DataLogging"),
    m_oPowerFail(false),
    m_EventLoggerMaxFileSize(0),
    m_DayEventLoggerMaxFileCount(0),
    m_DayEventLogger(this, Global::LoggingSource(GetLoggingSource().GetSource(), SUBCOMPONENT_DAYOPLOGGER)),
    m_EventLogger(this, Global::LoggingSource(GetLoggingSource().GetSource(), SUBCOMPONENT_EVENTLOGGER)),
    m_ComponentTestLogger(this, Global::LoggingSource(GetLoggingSource().GetSource(), SUBCOMPONENT_COMPTESTLOGGER)),
    m_pEventFilterNetworkServer(NULL)
{
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
    m_DayEventLogger.Configure(DayEventLoggerConfig(m_OperatingMode,
                                                            m_SerialNumber,
                                                            Global::SystemPaths::Instance().GetLogfilesPath(),
                                                            m_DayEventLoggerMaxFileCount));
    // now configure the loggers
    m_ComponentTestLogger.Configure(ComponentTestLoggerConfig(m_OperatingMode,
                                                              Global::SystemPaths::Instance().GetComponentTestPath()));
    m_EventLogger.Configure(EventLoggerConfig(m_EventLoggerBaseFileName,
                                              m_OperatingMode,
                                              m_SerialNumber,
                                              Global::SystemPaths::Instance().GetLogfilesPath(),
                                              m_EventLoggerMaxFileSize));


    RegisterCommandForProcessing<DataLogging::CmdForceCaching, DataLoggingThreadController>(&DataLoggingThreadController::OnForceCaching, this);
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
    SendToEventLogger(EventEntry(Global::AdjustedTime::Instance().GetCurrentDateTime(),
                                 GetLoggingSource(), Global::EVTTYPE_FATAL_ERROR,
                                 Global::EVTSTAT_ON, Global::EVENT_GLOBAL_POWER_FAIL));
    // close all files
    m_ComponentTestLogger.CloseFile();
    m_DayEventLogger.CloseFile();
    m_EventLogger.CloseFile();
    // switch to state power fail
    m_oPowerFail = true;
}

/****************************************************************************/
void DataLoggingThreadController::SendToEventLogger(const EventEntry &Entry) {
    if(!m_oPowerFail) {
        // send to event logger
        m_EventLogger.Log(Entry);
    } else {
        // dump to console
        Entry.DumpToConsole();
    }
}

/****************************************************************************/
void DataLoggingThreadController::SendToComponentTestLogger(const ComponentTestEntry &Entry) {
    // silently discard entry if power fails
    if(!m_oPowerFail) {
        m_ComponentTestLogger.Log(Entry);
    }
}

/****************************************************************************/
void DataLoggingThreadController::SendToDayEventLogger(const DayEventEntry &Entry) {
    // silently discard entry if power fails
    if(!m_oPowerFail) {
        m_DayEventLogger.Log(Entry);
    }
}

/****************************************************************************/
void DataLoggingThreadController::OnForceCaching(Global::tRefType Ref, const CmdForceCaching &Cmd) {
    /// \todo implement
    Q_UNUSED(Ref);
    Q_UNUSED(Cmd);
}

} // end namespace DataLogging

