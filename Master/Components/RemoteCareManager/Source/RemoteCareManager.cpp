/****************************************************************************/
/*! \file RemoteCareManager/Source/RemoteCareManager.cpp
 *
 *  \brief Implementation file for class RemoteCareManager
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2014-03-13
 *  $Author:    $ Ramya GJ
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
#include <RemoteCareManager/Include/RemoteCareManager.h>
#include <RemoteCareController/Include/RemoteCareThreadController.h>
#include <Threads/Include/PlatformThreadIDs.h>
#include <RemoteCareAgent/Include/RemoteCareEventCodes.h>


#include <NetCommands/Include/CmdRCReportEvent.h>
#include <EventHandler/Include/StateHandler.h>
#include <Global/Include/UITranslator.h>
#include <NetCommands/Include/CmdRCNotifyAssetComplete.h>
#include <NetCommands/Include/CmdRCNotifyReconnection.h>
#include <RemoteCareManager/Include/RCManagerEventCodes.h>
#include <NetCommands/Include/CmdRCSoftwareUpdate.h>
#include <NetCommands/Include/CmdRemoteCareState.h>

const QString RC_EVENT_CLASS_ALARM = "1";        //!< event log level, only alarms
const QString RC_EVENT_CLASS_ALARM_WARN = "2";   //!< event log level, alarms+warnings
const QString RC_EVENT_CLASS_ALARM_WARN_INFO = "3"; //!< event log level, only alarms+warnings+info

const QString RC_EVENT_PRIORITY_HIGH = "1";        //!< event priority high
const QString RC_EVENT_PRIORITY_MEDIUM = "2";        //!< event priority medium
const QString RC_EVENT_PRIORITY_LOW = "3";        //!< event priority low

const QString RC_EVENT_SEVERITY_ALARM = "300";        //!< event severity of alarms
const QString RC_EVENT_SEVERITY_WARNING = "200";   //!< event severity of warnings
const QString RC_EVENT_SEVERITY_INFO = "100"; //!< event severity of info

const int INDEX_COMMAND_CHANNEL_GUI = 1; //!< command channel gui

namespace RemoteCare {
#if defined(__arm__) || defined(__TARGET_ARCH_ARM) || defined(_M_ARM)
const quint32 CONFIG_FILE_TIMER_INTERVAL    = 10000; //!< 10[s] for target
#else
const quint32 CONFIG_FILE_TIMER_INTERVAL    = 3000;  //!< 3[s] for desktop
#endif
/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam MasterThreadControllerRef
 *  \iparam RCConfigurationInterface
 *
 */
/****************************************************************************/
RemoteCareManager::RemoteCareManager(Threads::MasterThreadController &MasterThreadControllerRef,
                                     DataManager::CRCConfigurationInterface *RCConfigurationInterface)
    : m_MasterThreadControllerRef(MasterThreadControllerRef)
    , m_EventClass(RC_EVENT_CLASS_ALARM_WARN_INFO)
    , m_EventPriority(Global::LOGLEVEL_NONE)
    , m_RCAAvailable(false)
    , m_SubscriptionStatus(false)
    , m_NumberOfLogFiles(5)
    , mp_RCConfigurationInterface(RCConfigurationInterface)
    , mp_RCConfiguration(NULL)
{
    //This object shall now live on master thread.
    this->setParent(&MasterThreadControllerRef);

    if(mp_RCConfigurationInterface) {
        mp_RCConfiguration = mp_RCConfigurationInterface->GetRCConfiguration();
    }

    m_FctTagNameHash[RC_DATAITEM_REQUEST_REMOTE_SESSION] = &RemoteCareManager::RequestRemoteSessionHandler;
    m_FctTagNameHash[RC_DATAITEM_SET_EVENT_PRIORITY]     = &RemoteCareManager::SetEventPriorityHandler;
    m_FctTagNameHash[RC_DATAITEM_SET_EVENT_CLASS]        = &RemoteCareManager::SetEventClassHandler;
    m_FctTagNameHash[RC_DATAITEM_REQUEST_ASSET_INFO]     = &RemoteCareManager::RequestAssetInfoHandler;
    m_FctTagNameHash[RC_DATAITEM_SET_LOG_NUMBER]         = &RemoteCareManager::SetLogNumberHandler;
    m_FctTagNameHash[RC_DATAITEM_SET_SUBSCRIPTION]       = &RemoteCareManager::SetSubscriptionHandler;
    m_FctTagNameHash[RC_DATAITEM_SET_UPDATE_AVAILABLE]   = &RemoteCareManager::SetUpdateAvailable;
    m_FctTagNameHash[RC_DATAITEM_SET_DOWNLOAD_FINISHED]  = &RemoteCareManager::SetDownloadFinished;

    SetupMaps();
}

/****************************************************************************/
/**
 * \brief This function creates remotecarecontroller
 */
/****************************************************************************/
void RemoteCareManager::Init()
{
    try {
        RemoteCare::RemoteCareController *p_RemoteCareController = new RemoteCare::RemoteCareController(Threads::THREAD_ID_REMOTECARE, "Axeda Client", NetworkBase::NSE_TYPE_AXEDA);
        // Send configuration data to RCA when its connected to Main
        CONNECTSIGNALSLOT(p_RemoteCareController, ProcessExited(const QString &, int), this, ExternalProcessExited(const QString &, int));
        CONNECTSIGNALSLOT(p_RemoteCareController, ProcessStoppedForEver(), this, ExternalProcessStoppedForever());
        CONNECTSIGNALSLOT(p_RemoteCareController, RCAConnected(), this, RCAConnected());

        m_MasterThreadControllerRef.AddAndConnectController
                (p_RemoteCareController, &m_MasterThreadControllerRef.m_CommandChannelRemoteCare, Threads::THREAD_ID_REMOTECARE, false);
        RegisterCommands();
        ReadCurrentSettingsState();
    }
    CATCHALL();
}

/****************************************************************************/
void RemoteCareManager::ReadCurrentSettingsState()
{
    if(mp_RCConfiguration) {
        try {
          m_EventClass          = mp_RCConfiguration->GetDataItemValue(RC_DATAITEM_SET_EVENT_CLASS);
          m_EventPriority       = m_EventLogLevelEnumMap.find(
                                                   mp_RCConfiguration->GetDataItemValue(RC_DATAITEM_SET_EVENT_PRIORITY)).value();
          m_SubscriptionStatus  = static_cast<bool>(mp_RCConfiguration->GetDataItemValue(RC_DATAITEM_SET_SUBSCRIPTION).toInt());
          m_NumberOfLogFiles    = static_cast<quint8>(mp_RCConfiguration->GetDataItemValue(RC_DATAITEM_SET_LOG_NUMBER).toInt());
          m_RCSoftwareAvailable = static_cast<bool>(mp_RCConfiguration->GetDataItemValue(RC_DATAITEM_SET_UPDATE_AVAILABLE).toInt());

          if (m_RCSoftwareAvailable) {
              //broadcast this msg so that gui receives this.
              emit SendRCCmdToGui(Global::CommandShPtr_t(new CmdRCSoftwareUpdate(15000, SWUpdate_Available)));
          }
          qDebug() << "Remote care State " << __FILE__ << __LINE__ << m_EventClass << m_EventPriority << m_SubscriptionStatus << m_NumberOfLogFiles;
        }
        CATCHALL();
    }
}

/****************************************************************************/
void RemoteCareManager::SetupMaps() {
    m_EventLogLevelEnumMap.insert(RC_EVENT_PRIORITY_MEDIUM, Global::LOGLEVEL_MEDIUM);
    m_EventLogLevelEnumMap.insert(RC_EVENT_PRIORITY_LOW, Global::LOGLEVEL_LOW);
    m_EventLogLevelEnumMap.insert(RC_EVENT_PRIORITY_HIGH, Global::LOGLEVEL_HIGH);
}

/****************************************************************************/
void RemoteCareManager::RegisterCommands()
{
    // so far the only registration with real function:
    // to receive remote care events
    m_MasterThreadControllerRef.RegisterCommandForProcessing<NetCommands::CmdRCSetLogEvent, RemoteCareManager>
            (&RemoteCareManager::OnCmdRCSetLogEventHandler, this);

    m_MasterThreadControllerRef.RegisterCommandForProcessing<NetCommands::CmdRCSetTag, RemoteCareManager>
            (&RemoteCareManager::OnCmdRCSetTag, this);

    //register commands from GUI
    m_MasterThreadControllerRef.RegisterCommandForProcessing<RemoteCare::CmdRCRequestRemoteSession, RemoteCareManager>
            (&RemoteCareManager::OnCmdRCRequestRemoteSession, this);

    m_MasterThreadControllerRef.RegisterCommandForProcessing<RemoteCare::CmdRCSoftwareUpdate, RemoteCareManager>
            (&RemoteCareManager::OnCmdRCSoftwareUpdate, this);
}

/****************************************************************************/
void RemoteCareManager::RCAConnected() {

    m_RCAAvailable = true;

    //broadcast this msg so that gui receives this.
    emit SendRCCmdToGui(Global::CommandShPtr_t(new NetCommands::CmdRemoteCareState(15000, true, "RemoteCare")));
}

/****************************************************************************/
void RemoteCareManager::ExternalProcessExited(const QString &name, int code)
{
    qDebug() << "\nExternalProcessController: ExternalProcessExited called.\n" << name << code;

    m_RCAAvailable = false;

    //broadcast this msg so that gui receives this.
    emit SendRCCmdToGui(Global::CommandShPtr_t(new NetCommands::CmdRemoteCareState(15000, false, "RemoteCare")));
}

/****************************************************************************/
void RemoteCareManager::ExternalProcessStoppedForever()
{
    qDebug()<<"\n External Process stopped for ever";

    m_RCAAvailable = false;

    //broadcast this msg so that gui receives this.
    emit SendRCCmdToGui(Global::CommandShPtr_t(new NetCommands::CmdRemoteCareState(15000, false, "RemoteCare")));

    Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_RCA_STOPPED_FOREVER);
}

/****************************************************************************/
void RemoteCareManager::OnCmdRCSetLogEventHandler(const Global::tRefType Ref,
                                       const NetCommands::CmdRCSetLogEvent &Cmd,
                                       Threads::CommandChannel &AckCommandChannel)
{
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);
    NetCommands::RCLogEventReportStruct RCEventData;
    QByteArray EventData(const_cast<QByteArray &>(Cmd.GetEventData()));
    QDataStream EventDataStream(&EventData, QIODevice::ReadWrite);
    EventDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    // stream the data
    EventDataStream >> RCEventData.EventCode;
    EventDataStream >> RCEventData.EventStringList;
    // stream the alternative string usage
    qint32 AltStringUsage;
    EventDataStream >> AltStringUsage;
    RCEventData.AltStringUsage = (Global::AlternateEventStringUsage) AltStringUsage;


    if (RCEventData.EventCode == RCAgentNamespace::EVENT_REMOTECARE_ERROR_WEB_ACCESS) {
        //broadcast this msg so that gui receives this.
        emit SendRCCmdToGui(Global::CommandShPtr_t(new NetCommands::CmdRemoteCareState(15000, false, "RemoteCare")));
    }

    // check the string list count
    if (RCEventData.EventStringList.count() > 0) {
        Global::tTranslatableStringList TrStringList;
        // update the translatable string list
        for (qint32 Counter = 0; Counter < RCEventData.EventStringList.count();
             Counter++) {
            TrStringList << RCEventData.EventStringList.value(Counter);
        }
        // raise event
        Global::EventObject::Instance().RaiseEvent(RCEventData.EventCode, TrStringList, true, RCEventData.AltStringUsage);
        return;
    }

    if (RCEventData.AltStringUsage != Global::NOT_APPLICABLE) {
        // raise event
        Global::EventObject::Instance().RaiseEvent(RCEventData.EventCode, RCEventData.AltStringUsage);
        return;
    }

    Global::EventObject::Instance().RaiseEvent(RCEventData.EventCode);

}

/****************************************************************************/
void RemoteCareManager::SetEventPriorityHandler(const Global::tRefType &Ref,
                                       const NetCommands::CmdRCSetTag &Cmd,
                                       Threads::CommandChannel &AckCommandChannel)
{
    QString EventPriority = Cmd.GetTagValue();

    if(RC_EVENT_PRIORITY_HIGH == EventPriority) {
        m_EventPriority = Global::LOGLEVEL_HIGH;
    }
    else if(RC_EVENT_PRIORITY_MEDIUM == EventPriority) {
        m_EventPriority = Global::LOGLEVEL_MEDIUM;
    }
    else if(RC_EVENT_PRIORITY_LOW == EventPriority) {
        m_EventPriority = Global::LOGLEVEL_LOW;
    }
    else
    {
        //send the NACK
        m_MasterThreadControllerRef.SendAcknowledgeNOK(Ref, AckCommandChannel);

        Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_DATAITEM_VALUE_INVALID,
                                                   Global::tTranslatableStringList() << Cmd.GetTagName()
                                                   << Cmd.GetTagValue());
        return;
    }

    //send the acknowledgement
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);
    UpdateContainerDataItem(Cmd);
}

/****************************************************************************/
void RemoteCareManager::SetEventClassHandler(const Global::tRefType &Ref,
                                       const NetCommands::CmdRCSetTag &Cmd,
                                       Threads::CommandChannel &AckCommandChannel)
{
    if(Cmd.GetTagValue() < RC_EVENT_CLASS_ALARM || Cmd.GetTagValue() > RC_EVENT_CLASS_ALARM_WARN_INFO ) {
        //send the NACK
        m_MasterThreadControllerRef.SendAcknowledgeNOK(Ref, AckCommandChannel);

        Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_DATAITEM_VALUE_INVALID,
                                                   Global::tTranslatableStringList() << Cmd.GetTagName()
                                                   << Cmd.GetTagValue());
        return;
    }

    //send the acknowledgement
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);

    m_EventClass = Cmd.GetTagValue();

    UpdateContainerDataItem(Cmd);
}

/****************************************************************************/
void RemoteCareManager::ForwardEventToRemoteCare(const DataLogging::DayEventEntry &TheEvent, const quint64 EventId64)
{
    if (!m_RemoteCareStatus)
        return;

    if (m_RCAAvailable && m_SubscriptionStatus) {
        bool SendEvent = false;

        if(EventId64 == EVENT_RCMANAGER_RECEIVED_COMMAND ||
                EventId64 == EVENT_RCMANAGER_REMOTESESSION_ACCEPTED ||
                EventId64 == EVENT_RCMANAGER_SENT_COMMAND) {
            SendEvent = false;
        }
        //send only those event types registered by Remote Care
        if(RC_EVENT_CLASS_ALARM == m_EventClass) {
            if(Global::EVTTYPE_ERROR == TheEvent.GetEventType() ||
                    Global::EVTTYPE_FATAL_ERROR == TheEvent.GetEventType()) {
                SendEvent = true;
            }
        }
        else if(RC_EVENT_CLASS_ALARM_WARN == m_EventClass) {
            if(Global::EVTTYPE_WARNING == TheEvent.GetEventType() ||
                    Global::EVTTYPE_ERROR == TheEvent.GetEventType() ||
                    Global::EVTTYPE_FATAL_ERROR == TheEvent.GetEventType()) {
                SendEvent = true;
            }
        }
        else if(RC_EVENT_CLASS_ALARM_WARN_INFO == m_EventClass) {
            if(Global::EVTTYPE_INFO == TheEvent.GetEventType() ||
                    Global::EVTTYPE_WARNING == TheEvent.GetEventType() ||
                    Global::EVTTYPE_ERROR == TheEvent.GetEventType() ||
                    Global::EVTTYPE_FATAL_ERROR == TheEvent.GetEventType()) {
                SendEvent = true;
            }
        }
        else {
            SendEvent = false;
        }

        //send only the requested type of events
        if(!SendEvent) {
            return;
        }

//        // send only the requested priority of events
        if(TheEvent.GetLogLevel() != m_EventPriority) {
            return;
        }

        NetCommands::RCEventReportDataStruct EventReportData;

        EventReportData.m_EventName = QString::number(EventId64);

        bool UseAltEventString = false;

        //add msg string
        QString MsgString = Global::UITranslator::TranslatorInstance().Translate(
                    Global::TranslatableString(TheEvent.GetEventCode(), TheEvent.GetString()),
                        UseAltEventString); //"Event String translated to the set langauge";

        EventReportData.m_EventMessage = MsgString;

        //add event severity
        if(Global::EVTTYPE_ERROR == TheEvent.GetEventType() || Global::EVTTYPE_FATAL_ERROR == TheEvent.GetEventType()) {
            EventReportData.m_EventSeverity = RC_EVENT_SEVERITY_ALARM;
        }
        else if(Global::EVTTYPE_WARNING == TheEvent.GetEventType()) {
            EventReportData.m_EventSeverity = RC_EVENT_SEVERITY_WARNING;
        }
        else if(Global::EVTTYPE_INFO == TheEvent.GetEventType()) {
            EventReportData.m_EventSeverity = RC_EVENT_SEVERITY_INFO;
        }

        //add time stamp
        EventReportData.m_TimeStamp = TheEvent.GetTimeStamp().toString(NetworkBase::DATEANDTIME_FORMAT);

        (void)m_MasterThreadControllerRef.SendCommand(
                Global::CommandShPtr_t(new NetCommands::CmdRCReportEvent(15000, EventReportData)),
                m_MasterThreadControllerRef.m_CommandChannelRemoteCare);
    }
}

/****************************************************************************/
void RemoteCareManager::SetSubscriptionHandler(const Global::tRefType &Ref, const NetCommands::CmdRCSetTag &Cmd,
                                               Threads::CommandChannel &AckCommandChannel)
{
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);

    m_SubscriptionStatus = Cmd.GetTagValue().toInt();
    qDebug() << "\n\n\n" << __FUNCTION__ << ": m_SubscriptionStatus: " << m_SubscriptionStatus << "\n\n\n";
    UpdateContainerDataItem(Cmd);
}

/****************************************************************************/
void RemoteCareManager::RequestAssetInfoHandler(const Global::tRefType &Ref, const NetCommands::CmdRCSetTag &Cmd,
                                                 Threads::CommandChannel &AckCommandChannel)
{
    //check that the data item type is valid
    if(("1" != Cmd.GetTagValue()))  {
        //send the NACK
        m_MasterThreadControllerRef.SendAcknowledgeNOK(Ref, AckCommandChannel);

        Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_DATAITEM_VALUE_INVALID,
                                                   Global::tTranslatableStringList() << Cmd.GetTagName()
                                                   << Cmd.GetTagValue());
        return;
    }

    //send the acknowledgement
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);

    //emit the signal
    emit RemoteCareExport(m_NumberOfLogFiles);
    UpdateContainerDataItem(Cmd);
}

/****************************************************************************/
void RemoteCareManager::SetLogNumberHandler(const Global::tRefType &Ref, const NetCommands::CmdRCSetTag &Cmd,
                                                 Threads::CommandChannel &AckCommandChannel)
{
    if("1" == Cmd.GetTagValue()) {
        m_NumberOfLogFiles = 5;
    }
    else if("2" == Cmd.GetTagValue()) {
        m_NumberOfLogFiles = 10;
    }
    else if("3" == Cmd.GetTagValue()) {
        m_NumberOfLogFiles = 15;
    }
    else if("4" == Cmd.GetTagValue()) {
        m_NumberOfLogFiles = 30;
    }
    else
    {
        //send the NACK
        m_MasterThreadControllerRef.SendAcknowledgeNOK(Ref, AckCommandChannel);

        Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_DATAITEM_VALUE_INVALID,
                                                   Global::tTranslatableStringList() << Cmd.GetTagName()
                                                   << Cmd.GetTagValue());
        return;
    }

    //send the acknowledgement
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);
    UpdateContainerDataItem(Cmd);
}

/****************************************************************************/
void RemoteCareManager::SetUpdateAvailable(const Global::tRefType &Ref, const NetCommands::CmdRCSetTag &Cmd,
                                                  Threads::CommandChannel &AckCommandChannel)
{
    QString UpdateType;

    if("1" == Cmd.GetTagValue() ) {
        UpdateType = SWUpdate_Available;
    }
    else if("0" == Cmd.GetTagValue()) {
        UpdateType = SWUpdate_NotAvailable;
    }
    else {
        //send the NACK
        m_MasterThreadControllerRef.SendAcknowledgeNOK(Ref, AckCommandChannel);

        Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_DATAITEM_VALUE_INVALID,
                                                   Global::tTranslatableStringList() << Cmd.GetTagName()
                                                   << Cmd.GetTagValue());
        return;
    }

    //send the acknowledgement
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);

    //broadcast this msg so that gui receives this.
    emit SendRCCmdToGui(Global::CommandShPtr_t(new CmdRCSoftwareUpdate(15000, UpdateType)));

    UpdateContainerDataItem(Cmd);
}

/****************************************************************************/
void RemoteCareManager::SetDownloadFinished(const Global::tRefType &Ref, const NetCommands::CmdRCSetTag &Cmd,
                                            Threads::CommandChannel &AckCommandChannel)
{
//    qDebug() << "\n\n\n" <<  __FUNCTION__ << "___Cmd.GetTagName:" << Cmd.GetTagName() << " Cmd.GetTagValue:" << Cmd.GetTagValue() << "\n\n\n";
    if("1" == Cmd.GetTagValue()) {
//        emit SendRCCmdToGui(Global::CommandShPtr_t(new CmdRCSoftwareUpdate(15000, SWUpdate_DownloadSuccess)));
        emit UpdateSoftwareFromRC();
    }
    else if("0" == Cmd.GetTagValue()) {
        //broadcast this msg so that gui receives this.
        emit SendRCCmdToGui(Global::CommandShPtr_t(new CmdRCSoftwareUpdate(15000, SWUpdate_DownloadFailed)));
    }
    else {
        //send the NACK
        m_MasterThreadControllerRef.SendAcknowledgeNOK(Ref, AckCommandChannel);

        Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_DATAITEM_VALUE_INVALID,
                                                   Global::tTranslatableStringList() << Cmd.GetTagName()
                                                   << Cmd.GetTagValue());
        return;
    }

    //send the acknowledgement
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);
    UpdateContainerDataItem(Cmd);
}

/****************************************************************************/
void RemoteCareManager::OnCmdRCRequestRemoteSession(const Global::tRefType Ref,
                                       const RemoteCare::CmdRCRequestRemoteSession &Cmd,
                                       Threads::CommandChannel &AckCommandChannel)
{
    //send the acknowledgement
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);

    //check whether user accepted the request or not
    //if thesystem is in busy state or if the remote care is disabled, then deny the session
    if (RemoteCare::RemoteSession_Accepted == Cmd.GetRequestType())
    {
        UpdateContainerDataItem(RC_DATAITEM_REQUEST_REMOTE_SESSION, RDI_DataGood, "1");

        SendNotifyReconnectToRemoteCare();

        Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_REMOTESESSION_ACCEPTED);
    }
    else
    {
        // denied, inform remote care
        SendNotifyDataItem(RC_DATAITEM_REQUEST_REMOTE_SESSION, RDI_DataGood, "0");

        Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_REMOTESESSION_DENIED,
                                                   Global::tTranslatableStringList() << "User denied");
    }
}

/****************************************************************************/
void RemoteCareManager::OnCmdRCSoftwareUpdate(const Global::tRefType Ref,
                                       const RemoteCare::CmdRCSoftwareUpdate &Cmd,
                                       Threads::CommandChannel &AckCommandChannel)
{
    //send the acknowledgement
    m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);

    if (RemoteCare::SWUpdate_StartDownload == Cmd.GetUpdateType())
    {
        // inform remote care to download the new sw package
        SendNotifyDataItem(RC_DATAITEM_ENABLE_SOFTWARE_UPDATE, RDI_DataGood, "1");
    }
}

/****************************************************************************/
void RemoteCareManager::SWUpdateStatus(bool Status)
{
    if (Status) {
        // SW update is success, inform remote care
        SendNotifyDataItem(RC_DATAITEM_ENABLE_SOFTWARE_UPDATE, RDI_DataGood, "0");
        SendNotifyDataItem(RC_DATAITEM_SET_DOWNLOAD_FINISHED, RDI_DataGood, "0");
        SendNotifyDataItem(RC_DATAITEM_SET_UPDATE_AVAILABLE, RDI_DataGood, "0");
    }
    else {
        // SW update is success, inform remote care
        SendNotifyDataItem(RC_DATAITEM_ENABLE_SOFTWARE_UPDATE, RDI_DataGood, "0");
        SendNotifyDataItem(RC_DATAITEM_SET_DOWNLOAD_FINISHED, RDI_DataGood, "0");
        SendNotifyDataItem(RC_DATAITEM_SET_UPDATE_AVAILABLE, RDI_DataGood, "1");
    }
}

/****************************************************************************/
void RemoteCareManager::SendNotifyReconnectToRemoteCare()
{
    //send command only if RCA is connected
    if(!m_RCAAvailable) {
        return;
    }

    // inform remote care
    (void)m_MasterThreadControllerRef.SendCommand(
                Global::CommandShPtr_t(new NetCommands::CmdRCNotifyReconnection(15000)),
                m_MasterThreadControllerRef.m_CommandChannelRemoteCare);

    //log command sent.
    Global::FmtArgs  Arguments;
    Arguments << "CmdRCNotifyReconnection";
    Arguments << "NA";
    Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_SENT_COMMAND, Arguments);
}

/****************************************************************************/
void RemoteCareManager::RCExportFinished(const QString FileName)
{
    //send command only if RCA is connected
    if(!m_RCAAvailable) {
        return;
    }
    //the import export thread controller has sent only the part of the file name
    //search in the remotecare directory for the latest file of the containing this FileName
    //sort in reversed order based on date time, so we can take the first entry
    QDir Dir(Global::SystemPaths::Instance().GetRemoteCarePath(), FileName,
             QDir::Reversed | QDir::Name, QDir::Files | QDir::CaseSensitive);
    QStringList FileNames = Dir.entryList();

    if (!FileNames.isEmpty())
    {
        // inform remote care
        (void)m_MasterThreadControllerRef.SendCommand(
                    Global::CommandShPtr_t(new NetCommands::CmdRCNotifyAssetComplete(15000, Global::SystemPaths::Instance().GetRemoteCarePath() + "/"+ FileNames.value(0))),
                    m_MasterThreadControllerRef.m_CommandChannelRemoteCare);

        //log command sent.
        Global::FmtArgs  Arguments;
        Arguments << "CmdRCNotifyAssetComplete";
        Arguments << FileNames.value(0);
        Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_SENT_COMMAND, Arguments);
    }
    else
    {
       // should not happen actually, but who knows
       Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, Global::FmtArgs() << "RCManager: No files found for export");
    }
}

/****************************************************************************/
void RemoteCareManager::OnCmdRCSetTag(const Global::tRefType Ref,
                                       const NetCommands::CmdRCSetTag &Cmd,
                                       Threads::CommandChannel &AckCommandChannel)
{
    FctTagName callbackFct;
    //log command received.
    Global::FmtArgs  Arguments;
    Arguments << Cmd.GetName() << Cmd.GetTagName() << Cmd.GetTagType() << Cmd.GetTagValue();

//   qDebug() << "\n\n\n\n" << __FUNCTION__ << " , " << __LINE__ << "Cmd.GetTagName: " << Cmd.GetTagName();

    Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_RECEIVED_COMMAND, Arguments);

    //now check for the data item
    DataManager::CRCDataItem DataItem1;

    if((NULL == mp_RCConfiguration) || !(mp_RCConfiguration->GetDataItem(Cmd.GetTagName(), DataItem1)))
    {
        //send the NACK
        m_MasterThreadControllerRef.SendAcknowledgeNOK(Ref, AckCommandChannel);

        Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_DATAITEM_INAVLID,
                                                   Global::tTranslatableStringList() << Cmd.GetTagName());
        return;
    }

    //check that the data item type is valid
    if(Cmd.GetTagType()!= DataItem1.GetType()) {
        //send the NACK
        m_MasterThreadControllerRef.SendAcknowledgeNOK(Ref, AckCommandChannel);

        Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_DATAITEM_TYPE_INVALID,
                                                   Global::tTranslatableStringList() << Cmd.GetTagName()
                                                   << QString::number((int)Cmd.GetTagType()));
        return;
    }

    //donothing if subscription is deactivated except listen to subscription status
    if(!m_SubscriptionStatus) {
        if(RC_DATAITEM_SET_SUBSCRIPTION == Cmd.GetTagName()) {
            SetSubscriptionHandler(Ref, Cmd, AckCommandChannel);
            return;
        }
        else {
            //send the NACK
            m_MasterThreadControllerRef.SendAcknowledgeNOK(Ref, AckCommandChannel);

            Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_INVALID_REQUEST_SUBSCRIPTION_NOTSET);
            return;
        }
    }


    if (m_FctTagNameHash.contains(Cmd.GetTagName()))
    {
        callbackFct = m_FctTagNameHash.value(Cmd.GetTagName());
        if ( callbackFct != NULL )
        {
            (*this.*callbackFct)(Ref, Cmd, AckCommandChannel);
        }
    }
    else
    {
        // since we checked the xml for the data item we can update the date container
        UpdateContainerDataItem(Cmd);
    }
}

/****************************************************************************/
void RemoteCareManager::RequestRemoteSessionHandler(const Global::tRefType &Ref,
                                       const NetCommands::CmdRCSetTag &Cmd,
                                       Threads::CommandChannel &AckCommandChannel)
{
    if("1" == Cmd.GetTagValue()) {

        //send the acknowledgement
        m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);

        QString CurrentSytemState = EventHandler::StateHandler::Instance().getCurrentOperationState();

        //if the system is in busy state or if the remote care is disabled, then deny the session
        if (CurrentSytemState == "BusyState") {
            // inform remote care

            SendNotifyDataItem(RC_DATAITEM_REQUEST_REMOTE_SESSION, RDI_DataGood, "0");

            Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_REMOTESESSION_DENIED,
                                                       Global::tTranslatableStringList() << "System Busy");

        }
        else {
            //broadcast this msg so that gui receives this.
            emit SendRCCmdToGui(Global::CommandShPtr_t(new CmdRCRequestRemoteSession(15000, RemoteSession_Requested)));

            UpdateContainerDataItem(Cmd, false);
        }
    }
    else if("0" == Cmd.GetTagValue()) {

        //send the acknowledgement
        m_MasterThreadControllerRef.SendAcknowledgeOK(Ref, AckCommandChannel);

        //broadcast this msg so that gui receives this.
        emit SendRCCmdToGui(Global::CommandShPtr_t(new CmdRCRequestRemoteSession(15000, RemoteSession_Ended)));

        UpdateContainerDataItem(Cmd);

        SendNotifyReconnectToRemoteCare();
    }
    else {
        //send the NACK
        m_MasterThreadControllerRef.SendAcknowledgeNOK(Ref, AckCommandChannel);

        Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_DATAITEM_VALUE_INVALID,
                                                   Global::tTranslatableStringList() << Cmd.GetTagName()
                                                   << Cmd.GetTagValue());
    }
}

/****************************************************************************/
void RemoteCareManager::UpdateContainerDataItem(const NetCommands::CmdRCSetTag &Cmd,
                                                                        const bool &WriteToDisk)
{
    if(mp_RCConfigurationInterface && mp_RCConfiguration) {
        DataManager::CRCDataItem DataItem1;

        if(mp_RCConfiguration->GetDataItem(Cmd.GetTagName(), DataItem1))
        {
            DataItem1.SetValue(Cmd.GetTagValue());
            if(mp_RCConfiguration->UpdateDataItem(Cmd.GetTagName(), DataItem1)) {
                if(mp_RCConfigurationInterface->UpdateRCConfiguration(mp_RCConfiguration)) {

                    if(WriteToDisk)
                    {
                        //inform remotecare agent
                        (void)m_MasterThreadControllerRef.SendCommand(
                                    Global::CommandShPtr_t(new NetCommands::CmdRCNotifyDataItem(
                                                               15000,
                                                               DataItem1.GetName(),
                                                               DataItem1.GetType(),
                                                               DataItem1.GetQuality(),
                                                               DataItem1.GetValue(),
                                    Global::AdjustedTime::Instance().GetCurrentDateTime().toString(NetworkBase::DATEANDTIME_FORMAT))),
                                    m_MasterThreadControllerRef.m_CommandChannelRemoteCare);

                        (void)mp_RCConfigurationInterface->Write();                        
                    }
                }
            }
        }
    }
}

/****************************************************************************/
RemoteCare::RCDataItemType_t RemoteCareManager::UpdateContainerDataItem(const QString &Name,
                             const RemoteCare::RCDataItemQuality_t &Quality, const QString &Value,
                                                                        const bool &WriteToDisk)
{
    if(mp_RCConfigurationInterface && mp_RCConfiguration) {
        DataManager::CRCDataItem DataItem1;

        if(mp_RCConfiguration->GetDataItem(Name, DataItem1))
        {
            DataItem1.SetValue(Value);
            DataItem1.SetQuality(Quality);
            if(mp_RCConfiguration->UpdateDataItem(Name, DataItem1)) {
                if(mp_RCConfigurationInterface->UpdateRCConfiguration(mp_RCConfiguration)) {

                    if(WriteToDisk)
                    {
                        (void)mp_RCConfigurationInterface->Write();
                    }

                    return DataItem1.GetType();
                }
            }
        }
    }
    return RDI_TypeInvalid;
}

/****************************************************************************/
void RemoteCareManager::SendNotifyDataItem(const QString &Name, const RemoteCare::RCDataItemQuality_t &Quality, const QString &Value,
                                           const bool &WriteToDisk)
{
    //send command only if RCA is connected
    if(!m_RCAAvailable) {
        return;
    }
    RemoteCare::RCDataItemType_t Type = RDI_TypeInvalid;

    Type = UpdateContainerDataItem(Name, Quality, Value, WriteToDisk);

    (void)m_MasterThreadControllerRef.SendCommand(
                Global::CommandShPtr_t(new NetCommands::CmdRCNotifyDataItem(15000, Name,Type, Quality, Value,
                         Global::AdjustedTime::Instance().GetCurrentDateTime().toString())),
                m_MasterThreadControllerRef.m_CommandChannelRemoteCare);

    //log command sent.
    Global::FmtArgs  Arguments;
    Arguments << "CmdRCNotifyDataItem" << Name << Type << Quality << Value;

    Global::EventObject::Instance().RaiseEvent(EVENT_RCMANAGER_SENT_COMMAND, Arguments);
}

/****************************************************************************/
/**
 * \brief Send command to Remote care channel only if RCA is connected.
 *
 */
/****************************************************************************/
void RemoteCareManager::SendCommandToRemoteCare(const Global::CommandShPtr_t &Cmd) {
    if(m_RCAAvailable) {
        (void)m_MasterThreadControllerRef.SendCommand(Cmd, m_MasterThreadControllerRef.m_CommandChannelRemoteCare);
    }
}

}

