/****************************************************************************/
/*! \file REServerConnector.cpp
 *
 *  \brief Implementation file for class REServerConnector. This class mainly
 *  initializes the connection to Remote Enterprise Server and evaluates the
 *  callbacks provided by the AgentEmbedded client. The callbacks are forwarded
 *  then to Master.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 29.04.2013
 *  $Author:    $ B. Stach
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

#include <fstream>
#include <QDateTime>
#include <QFile>
#include <QCoreApplication>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/GlobalDefines.h>
#include <Global/Include/Utils.h>

// Commands to Master
#include <NetCommands/Include/CmdRCSetTag.h>
#include <NetCommands/Include/CmdRCSetLogEvent.h>

#include "RCConfigurationWrapper.h"
#include "REServerConnector.h"
#include "RemoteCareEventCodes.h"

using namespace std;

namespace RCAgentNamespace {
/// Configuration
RCConfigurationWrapper          REServerConnector::confWrapper;
/// This is static just because it is used in Callbacks, which need to be static.
QList<Global::CommandShPtr_t>   REServerConnector::incomingCommandQueue;

ofstream                        REServerConnector::ofs;
string                          REServerConnector::downloadPath("../RemoteCare");
AgentStates_t                   REServerConnector::stateAgent(AGENT_NOT_STARTED);

/****************************************************************************/
/*!
 *  \brief   Constructor.
 */
/*****************************************************************************/
REServerConnector::REServerConnector() :
            deviceID(0),
            serverID(0),
            isOnRun(true)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor.
 */
/*****************************************************************************/
REServerConnector::~REServerConnector()
{
    try {
        threadRCAClient.quit();
        (void)threadRCAClient.wait();
    }
    catch (...) {}
}

/****************************************************************************/
/*!
 *  \brief   Initialize connection to Enterprise Server.
 *
 *  \return  true if link is successfully initialized, false otherwise
 */
/****************************************************************************/
bool REServerConnector::Initialize()
{
    try {
        if (!InitializeSettings()){
            return false;
        }

        CONNECTSIGNALSLOT(&threadRCAClient, started(), this, Work());

        this->moveToThread(&threadRCAClient);

        ChangeAgentState(AGENT_IS_INITIALIZED);
    }
    catch (const std::bad_alloc &) {
        Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_BAD_ALLOC);
        return false;
    }
    return true;
}

/****************************************************************************/
/*!
 *  \brief   Initialize settings
 *
 *  \return  true if settings has been successful initialized, false otherwise
 */
/****************************************************************************/
bool REServerConnector::InitializeSettings()
{
    try {
        // 1. read settings from disk
        if (!confWrapper.ReadSettings()) {
            qDebug() << "REServerConnector ERROR: cannot read settings !";
            Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_READ_SETTINGS);
            return false;
        }

        downloadPath = confWrapper.GetDownloadFolder();

        // 2. initialize Agent
        if (!confWrapper.SetSettings()) {
            qDebug() << "REServerConnector ERROR: cannot set settings !";
            Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_SET_SETTINGS);
            return false;
        }

        // 3. Post dataitems
        if (!confWrapper.PostDataItems())
        {
            qDebug() << "REServerConnector ERROR: cannot set settings !";
            Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_POST_DATAITEMS);
            return false;
        }

        serverID = confWrapper.GetConfiguration().serverID;
        deviceID = confWrapper.GetConfiguration().deviceID;

        // register all callbacks
        RegisterAllCallbacks();
    }
    catch (const std::bad_alloc &) {
        Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_BAD_ALLOC);
        return false;
    }
    return true;
}

/****************************************************************************/
/*!
 *  \brief   Start Remote Enterprise Server connection
 */
/*****************************************************************************/
void REServerConnector::Start()
{
    if (stateAgent == AGENT_IS_INITIALIZED)
    {
        qDebug() << "REServerConnector: start AeDRM execution loop!";
        ChangeAgentState(AGENT_IS_RUNNING);
        threadRCAClient.start();
        return;
    }

    Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_NOT_INITIALIZED);
}

/****************************************************************************/
/*!
 *  \brief   It triggers reconnection to Remote Enterprise Server
 */
/*****************************************************************************/
void REServerConnector::Reconnect()
{
    if ( stateAgent == AGENT_IS_RUNNING )
    {
        stateAgent = AGENT_RESTART;
    }
    else if ( stateAgent == AGENT_STOPPED )
    {
        stateAgent = AGENT_RESTART;
        Restart();
    }
}

/****************************************************************************/
/*!
 *  \brief   It restarts Remote Enterprise Server connection
 */
/*****************************************************************************/
void REServerConnector::Restart()
{
    qDebug() << "############################### REServerConnector: restart called \n";

    isOnRun = true;
    if (!InitializeSettings()) {
        qDebug() << "REServerConnector: Work ERROR: restart initialization failed !";
        ChangeAgentState(AGENT_STOPPED);
        return;
    }

    qDebug() << "REServerConnector::Work - restart agent !";
    ChangeAgentState(AGENT_IS_RUNNING);
    Work();
}

/****************************************************************************/
/*!
 *  \brief   Remote Enterprise connection main loop.
 *
 *  This function blocks until Agent is requested to quit. Think of it
 *  as of a generic thread's "run" function.
 *  Call QCoreApplication::processEvents() to process pending events.
 */
/*****************************************************************************/
void REServerConnector::Work()
{
    AeBool     connected    = false;
    static int     raise_flag = 0;
    size_t       count       = 0;  // send flag
    AeTimeValue execTime    = confWrapper.GetConfiguration().execTime;
    serverID                = confWrapper.GetConfiguration().serverID;
    AgentStates_t prevState;

    qDebug() << "\nREServerConnector: entered working function... \n";

    do // execute agent
    {
        // this is the execution cycle
        if (AeDRMExecute(&execTime) != AeEOK) {
            qDebug() << "REServerConnector ERROR :  AeDRMExecute returned false !";
        }

        // check if agent is actually connected to the Server
        if (AeDRMGetServerStatus(serverID, &connected) != AeEOK || !connected) {
                qDebug() << "REServerConnector WARNING :  Agent is not connected to RC-EServer !";
				if (raise_flag != 1) {
					Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_SERVER_STATUS);
					raise_flag = 1;
				}
                stateAgent = AGENT_STOPPED;
        } else {
			raise_flag = 2;
            if (count++ % 10 == 0) {
                qDebug() << "REServerConnector INFO :  Agent is connected to RC-EServer !";
                Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_INFO_CONNECTED_ENTERPRISE_SERVER);
            }          
        }

        // check if there are any incoming commands in the queue and execute pending ones
        // call this function before the "processEvents()"!
        // called because of static callback functions - emit functions are not possible inside static functions
        CheckIncomingCommandQueue();

        // Process signals and other events accumulated in the queue while AeDRMExecute was blocking
        QCoreApplication::processEvents();
        qDebug() << "REServerConnector Working :  going into next working cycle... ";
        qDebug() << "AGENT STATE " << stateAgent << "                 ---------------------------------------";
    } while( stateAgent == AGENT_IS_RUNNING && isOnRun );
    prevState = stateAgent;
    ChangeAgentState(AGENT_GOING_DOWN);
    // shutdown Agent Embedded
    AeShutdown();
    qDebug() << "\nREServerConnector: exiting working function... \n";
    ChangeAgentState(prevState);
    Restart();
}

/******************************************************l**********************/
/*!
 *  \brief   Stop Agent operation
 */
/****************************************************************************/
void REServerConnector::Stop()
{
    isOnRun = false;
    qDebug() << "###############################################################################\n";
    qDebug() << "REServerConnector: received request to stop operation... \n";
}

/****************************************************************************/
/*!
 *  \brief   Call this function to change Agent state.
 *
 *  \iparam  state = the state to put Agent into
 */
/*****************************************************************************/
void REServerConnector::ChangeAgentState(const AgentStates_t &state)
{
    stateAgent = state;
}

/****************************************************************************/
/*!
 *  \brief   Get function for AeDrmExecute's execution period
 *
 *  \return  time in seconds
 */
/*****************************************************************************/
AeInt32 REServerConnector::GetExecTime()
{
    return confWrapper.GetConfiguration().execTime.iSec;
}

/****************************************************************************/
/*!
 *  \brief   Get function for Agent's state
 *
 *  \return  Current state of the Agent
 */
/*****************************************************************************/
AgentStates_t REServerConnector::GetState()
{
    return stateAgent;
}

/****************************************************************************/
/*!
 *  \brief   Submit Event to be sent to Server
 *
 *  \iparam  ref        = request's reference
 *  \iparam  nameStr    = event's name
 *  \iparam  messageStr = event's description text
 *  \iparam  severity   = event's severity
 *  \iparam  timestamp  = Master's timestamp value
 *
 *  \warning   If timestamp is in the past, the Axeda Server will not complain
 *             about the message contents, but will silently discard the
 *             message. At least no Event will be shown on the WEB interface.
 */
/*****************************************************************************/
void REServerConnector::SubmitEventRequest(
                                            const Global::tRefType  &ref,
                                            const QString           &nameStr,
                                            const QString           &messageStr,
                                            const QString           &severity,
                                            const QString           &timestamp
                                          )
{
    emit SigRequestStatus(ref, SubmitEventWithoutAck(nameStr, messageStr, severity,timestamp));
}

/****************************************************************************/
/*!
 *  \brief   Submit Event to be sent to Server without sending Acknowledge
 *
 *  \iparam  nameStr    = event's name
 *  \iparam  messageStr = event's description text
 *  \iparam  severity   = event's severity
 *  \iparam  timestamp  = Master's timestamp value
 *
 *  \warning   If timestamp is in the past, the Axeda Server will not complain
 *             about the message contents, but will silently discard the
 *             message. At least no Event will be shown on the WEB interface.
 */
/*****************************************************************************/
bool REServerConnector::SubmitEventWithoutAck(
                                            const QString           &nameStr,
                                            const QString           &messageStr,
                                            const QString           &severity,
                                            const QString           &timestamp
                                          )
{
    AeDRMEvent      drmEvent;
    QByteArray name     = nameStr.toUtf8();
    QByteArray message  = messageStr.toUtf8();

    qDebug() << "REServerConnector::SubmitEventRequest : " << name;

    // set time stamp
    confWrapper.ConvertTime(timestamp, &drmEvent.timeStamp);
    // prepare event
    drmEvent.pName      = const_cast<AeChar*>(name.data());
    drmEvent.pMessage   = const_cast<AeChar*>(message.data());
    bool ok             = false;
    drmEvent.iSeverity  = static_cast<AeInt>(severity.toInt(&ok, 10));

    if (!ok) {
        qDebug() << "REServerConnector ERROR :  could not convert event severity value :  " << severity << " !";
        // use "error" value:
        drmEvent.iSeverity = static_cast<AeInt>(-1);
        Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_INFO_SUBMITEVENT_SEVERITY_CONVERSION);
    }

    // post event
    if ( stateAgent == AGENT_STOPPED ||
         AeDRMPostEvent(deviceID, serverID, AeDRMQueuePriorityNormal, &drmEvent) != AeEOK
       )
    {
        qDebug() << "REServerConnector ERROR :  could not post following Event :  " << name << " !";
        Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_SUBMITEVENT_POST);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief   Submit Data Item to be sent to Server
 *
 *  \iparam  ref        = request's reference
 *  \iparam  name       = parameter's name
 *  \iparam  type       = type of the parameter (analog/digital/string)
 *  \iparam  quality    = quality of the parameter
 *  \iparam  value      = the value of the parameter
 *  \iparam  timestamp  = Master's timestamp value
 *
 *  \warning   If timestamp is in the past, the Remote Server will not complain
 *             about the message contents, but will silently discard the
 *             message.
 */
/*****************************************************************************/
void REServerConnector::SubmitDataItemRequest(
                                                const Global::tRefType                   &ref,
                                                const QString                            &name,
                                                const RemoteCare::RCDataItemType_t       &type,
                                                const RemoteCare::RCDataItemQuality_t    &quality,
                                                const QString                            &value,
                                                const QString                            &timestamp
                                             )
{
    if ( stateAgent == AGENT_STOPPED ||
         !confWrapper.PostItem(name, type, quality, value, timestamp))
    {
            emit SigRequestStatus(ref, false);
            return;
    }

    // all ok, inform upper layer
    emit SigRequestStatus(ref, true);
}

/****************************************************************************/
/*!
 *  \brief   Submit a file to be uploaded to Server
 *
 *  \iparam  ref          = request's reference
 *  \iparam  filenameStr  = the name of the file including path to upload.
 */
/*****************************************************************************/
void REServerConnector::SubmitUploadRequest(
                                             const Global::tRefType &ref,
                                             const QString          &filenameStr
                                           )
{
    qDebug() << "REServerConnector::SubmitUploadRequest : " << filenameStr;
    QByteArray filename = filenameStr.toUtf8();

    AeFileUploadSpec            uploadSpec;
    AeFileUploadSpec            *ppUploads[2];
    AeFileUploadRequestParam    param;

    // prepare upload data structures
    uploadSpec.pName    = const_cast<AeChar*>(filename.data());
    // automatically deleting the file after upload as per Req. 10799
    uploadSpec.bDelete  = AeTrue;

    // upload specification list must be NULL-terminated
    ppUploads[0] = &uploadSpec;
    ppUploads[1] = NULL;

    // check if compression is required
    if (confWrapper.GetConfiguration().useCompress) {
        // compress the file
        param.iMask         = AE_FILE_UPLOAD_REQUEST_PARAM_COMPRESSION;
        param.iCompression  = AeFileCompressionTarGzip;
    }
    else {
        // no compression for the file
        param.iMask        = 0x0;
        param.iCompression = AeFileCompressionNone;
    }

    // submit upload request
    if ( stateAgent == AGENT_STOPPED ||
         AeDRMPostFileUploadRequestEx(deviceID, serverID, AeDRMQueuePriorityNormal, ppUploads, &param) != AeEOK
       )
    {
        qDebug() << "REServerConnector ERROR :  could not submit file " << *(uploadSpec.pName) << " for upload !";
        emit SigRequestStatus(ref, false);
        Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_UPLOAD_FILE);
        return;
    }

    emit SigRequestStatus(ref, true);
}

/****************************************************************************/
/*!
 *  \brief   This function checks the incoming command queue and acts on
 *           pending commands.
 */
/*****************************************************************************/
void REServerConnector::CheckIncomingCommandQueue()
{
    if (incomingCommandQueue.isEmpty()) {
        // queue is empty: nothing to do.
        return;
    }
    // there are pending commands
    for (int i = 0; i < incomingCommandQueue.size(); i++) {
        emit SigAgentIncomingCommand(incomingCommandQueue[i]);
        incomingCommandQueue.removeAt(i);
    }
}

/****************************************************************************/
/*!
 *  \brief   This function checks if file exists in the "Uploads" folder.
 *
 *  \iparam   filename = the name of the file
 *
 *  \return  True if file exists, False otherwise.
 */
/*****************************************************************************/
bool REServerConnector::CheckIfFileExists(const QString &filename)
{
    return QFile::exists(filename);
}

/****************************************************************************/
/*!
 *  \brief   Reports that Master initialization failed.
 */
/*****************************************************************************/
void REServerConnector::OnMasterInitializationFailed()
{
    QByteArray  name     = QString("Error").toUtf8();
    QByteArray  message  = QString("No connection to Master!").toUtf8();
    QString     severity("300");

    (void)SubmitEventWithoutAck(name, message, severity, "");
}

/****************************************************************************/
/*!
 *  \brief   One function to register all available callbacks.
 */
/*****************************************************************************/
void REServerConnector::RegisterAllCallbacks()
{
    // Status reports related callbacks:
    AeDRMSetOnWebError(OnWebErrorCallback);
    AeDRMSetOnDeviceRegistered(OnDeviceRegisteredCallback);
    AeDRMSetOnQueueStatus(QueueStatusCallback);

    // Server commands related callbacks:
    AeDRMSetOnCommandSetTime(OnCommandSetTimeCallback);
    AeDRMSetOnCommandRestart(OnCommandRestartCallback);
    AeDRMSetOnPingRateUpdate(OnPingRateUpdateCallback);
    AeDRMSetOnCommandSetTag(OnCommandSetTagCallback);

    // Remote Session related callbacks:
    AeDRMSetOnRemoteSessionStart(OnRemoteSessionStartCallback);
    AeDRMSetOnRemoteSessionEnd(OnRemoteSessionEndCallback);

    // SOAP methods related callbacks:
    AeDRMSetOnSOAPMethod(OnSOAPMethodCallback);
    AeDRMSetOnSOAPMethodEx(OnSOAPMethodExCallback);

    // File download related callbacks;
    AeDRMSetOnFileDownloadBegin(OnFileDownloadBeginCallback);
    AeDRMSetOnFileDownloadData(OnFileDownloadDataCallback);
    AeDRMSetOnFileDownloadEnd(OnFileDownloadEndCallback);
}

/****************************************************************************/
/*!
 *  \brief   As soon as a remote session starts, this callback is invoked.
 *
 *  \iparam  pInterface = a pointer to the information about this remote session
 *
 *  \return  if Remote Session is allowed, return true, of not - return false
 */
/*****************************************************************************/
AeBool REServerConnector::OnRemoteSessionStartCallback(AeRemoteInterface *pInterface)
{
    Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_INFO_START_REMOTESESSION);
    qDebug() << "*** CB: OnRemoteSessionStartCallback. Device = " << pInterface->iDeviceId << " Port =  " << pInterface->iPort;
    return confWrapper.GetConfiguration().setupRemoteSession;
}

/****************************************************************************/
/*!
 *  \brief   As soon as a remote session ends, this callback is invoked.
 *
 *  \iparam  pInterface = a pointer to the information about this remote session
 */
/*****************************************************************************/
void REServerConnector::OnRemoteSessionEndCallback(AeRemoteInterface *pInterface)
{
    Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_INFO_END_REMOTESESSION);
    qDebug() << "*** CB: OnRemoteSessionEndCallback. Device = " << pInterface->iDeviceId << " Port =  " << pInterface->iPort \
             << " Server =  " << pInterface->pServer;

    RemoteCare::RCDataItemType_t type;

    if ( confWrapper.GetDataItemType(RemoteCare::RC_DATAITEM_REQUEST_REMOTE_SESSION, type) )
    {
    	incomingCommandQueue.append(new NetCommands::CmdRCSetTag(2000,
                                                             RemoteCare::RC_DATAITEM_REQUEST_REMOTE_SESSION,
                                                             type,
                                                             "0"));
    }
}

/****************************************************************************/
/*!
 *  \brief   This callback is invoked when a communication error occurs while
 *           Agent Embedded is performing pending tasks in AeDRMExecute().
 *
 *  \iparam  iServerId = server ID of the Enterprise server that caused the error
 *  \iparam  iError = error code
 */
/*****************************************************************************/
void REServerConnector::OnWebErrorCallback(AeInt32 iServerId, AeError iError)
{
    qDebug() << "*** CB: OnWebErrorCallback. ServerId = " << iServerId << " Error = " << static_cast<int>(iError);
    Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_WEB_ACCESS);
    stateAgent = AGENT_STOPPED;
}

/****************************************************************************/
/*!
 *  \brief   This callback is invoked when a device is successfully
 *           registered while in AeDRMExecute().
 *
 *  \iparam  iDeviceId = ID of registered device
 */
/*****************************************************************************/
void REServerConnector::OnDeviceRegisteredCallback(AeInt32 iDeviceId)
{
    qDebug() << "*** CB: OnDeviceRegisteredCallback. DeviceId = " << iDeviceId;
    Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_INFO_DEVICE_REGISTERED);
}

/****************************************************************************/
/*!
 *  \brief   This callback is invoked whenever the status of the internal
 *           data queue is changed.
 *
 *  \iparam  iNewStatus = the actual queue status
 */
/*****************************************************************************/
void REServerConnector::QueueStatusCallback(AeDRMQueueStatus iNewStatus)
{
    qDebug() << "*** CB: QueueStatusCallback. Status = " << static_cast<int>(iNewStatus);
}

/****************************************************************************/
/*!
 *  \brief   This callback is invoked when a SOAP method is received from
 *           server and the method is not known (that is, not handled by any
 *           of the callback functions registered through AeDRMSetOnCommandXXX()).
 *
 *  \iparam  iDeviceId = ID of target device
 *  \iparam  pMethod = handle to the SOAP method received from server
 *  \iparam  pStatus = pointer to the structure for invocation results
 */
/*****************************************************************************/
void REServerConnector::OnSOAPMethodCallback(AeInt32 iDeviceId, AeHandle pMethod, AeDRMSOAPCommandStatus *pStatus)
{
    Q_UNUSED(iDeviceId)
    Q_UNUSED(pMethod)
    qDebug() << "*** CB: OnSOAPMethodCallback. StatusCode = " << pStatus->iStatusCode << " Description = " << pStatus->pStatusReason;
}

/****************************************************************************/
/*!
 *  \brief   This callback is invoked whenever generic SOAP method is received.
 *           The application can process the received SOAP method using the
 *           AeDRMSOAPXXX() family of functions.
 *
 *  \iparam  iDeviceId = ID of target device
 *  \iparam  iServerId = ID of the Enterprise server that sent the SOAP method
 *  \iparam  pMethod = handle to the SOAP method received from server
 *  \iparam  pSOAPId = handle to the SOAP method ID
 *  \iparam  pStatus = pointer to the structure for invocation results
 */
/*****************************************************************************/
void REServerConnector::OnSOAPMethodExCallback(AeInt32 iDeviceId, AeInt32 iServerId, AeHandle pMethod, \
                                        AeDRMSOAPCommandId *pSOAPId, AeDRMSOAPCommandStatus *pStatus)
{
    Q_UNUSED(iDeviceId)
    Q_UNUSED(iServerId)
    Q_UNUSED(pMethod)
    qDebug() << "*** CB: OnSOAPMethodExCallback. \nCommandId = " << pSOAPId->pCmdId << " UserId = " << pSOAPId->pUserId \
                                    << "\nStatusCode = " << pStatus->iStatusCode << " Description = " << pStatus->pStatusReason;
}

/****************************************************************************/
/*!
 *  \brief   This callback is invoked on receiving the SOAP method:
 *           DynamicData.SetTag.
 *
 *  \iparam  iDeviceId  = ID of the target device
 *  \iparam  pDataItem  = name of the data item and its new value
 *  \iparam  pStatus    = pointer to the structure for invocation results
 */
/*****************************************************************************/
void REServerConnector::OnCommandSetTagCallback(AeInt32 iDeviceId, AeDRMDataItem *pDataItem, AeDRMSOAPCommandStatus *pStatus)
{
    qDebug() << "*** CB: OnCommandSetTagCallback. DeviceId = " << iDeviceId << "/ DataItemName =  " << pDataItem->pName \
                                                              << "/ DataItemValueType =  " << static_cast<int>(pDataItem->value.iType);

    pStatus->iStatusCode        = AE_DRM_SOAP_COMMAND_STATUS_SUCCESS;
    AeDRMDataType ParameterType = pDataItem->value.iType;
    QString ParameterName       = static_cast<QString>(pDataItem->pName);
    QString ParameterValue      = "";

    switch (pDataItem->value.iType) {
        case AeDRMDataAnalog:
            ParameterValue = QString::number((double)(pDataItem->value.data.dAnalog), 'G', 6);
            break;
        case AeDRMDataDigital:
            ParameterValue = QString::number((quint8)(pDataItem->value.data.bDigital), 10);
            break;
        case AeDRMDataString:
            ParameterValue = (QString)pDataItem->value.data.pString;
            break;
        default:
            pStatus->iStatusCode = AE_DRM_SOAP_COMMAND_STATUS_INVALID_PARAMS;
            qDebug() << "*** CB: OnCommandSetTagCallback ERROR: unknown data type !";
            break;
    }

    // if conversion succeeded, forward request to the Controller.
    if (pStatus->iStatusCode == AE_DRM_SOAP_COMMAND_STATUS_SUCCESS) {

        incomingCommandQueue.append(new NetCommands::CmdRCSetTag(2000,
                                                                 ParameterName,
                                                                 static_cast<RemoteCare::RCDataItemType_t>(ParameterType),
                                                                 ParameterValue));
        Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_INFO_SET_TAG_RECEIVED);
        return;
    }

    Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_ERROR_SET_TAG);
}

/****************************************************************************/
/*!
 *  \brief   This callback is invoked when Agent Embedded receives the
 *           Set Time SOAP Method: EEnterpriseProxy.St.
 *
 *  \iparam  iDeviceId  = ID of the target device
 *  \iparam  pTime      = new time value to set on the device
 *  \iparam  piTZOffset = new time zone offset
 *  \iparam  pStatus    = pointer to the structure for invocation results
 */
/*****************************************************************************/
void REServerConnector::OnCommandSetTimeCallback(AeInt32 iDeviceId, AeTimeValue *pTime, AeInt32 *piTZOffset, \
                                          AeDRMSOAPCommandStatus *pStatus)
{
    Q_UNUSED(pTime)
    Q_UNUSED(piTZOffset)
    Q_UNUSED(pStatus)
    qDebug() << "*** CB: OnCommandSetTimeCallback. DeviceId = " << iDeviceId;
}

/****************************************************************************/
/*!
 *  \brief   This callback is invoked when Agent Embedded receives the
 *           Restart SOAP method: EEnterpriseProxy.Rs.
 *
 *  \iparam  iDeviceId  = ID of the target device
 *  \iparam  bHard      = flag indicating whether hard or soft restart is requested
 *  \iparam  pStatus    = pointer to the structure for invocation results
 */
/*****************************************************************************/
void REServerConnector::OnCommandRestartCallback(AeInt32 iDeviceId, AeBool bHard, AeDRMSOAPCommandStatus *pStatus)
{
    Q_UNUSED(bHard)
    Q_UNUSED(pStatus)
    qDebug() << "*** CB: OnCommandRestartCallback. DeviceId = " << iDeviceId;
}

/****************************************************************************/
/*!
 *  \brief   This callback is invoked on receiving Set Ping Rate SOAP method:
 *           EEnterpriseProxy.Pu.
 *           The callback is also invoked by Agent Embedded when the ping
 *           rate is restored to the original after the time period for a
 *           temporary ping rate elapses.
 *
 *  \iparam  iServerId = ID of the Enterprise server that sent the message
 *  \iparam  pPingRate = the new ping rate
 */
/*****************************************************************************/
void REServerConnector::OnPingRateUpdateCallback(AeInt32 iServerId, AeDRMPingRate *pPingRate)
{
    qDebug() << "*** CB: OnPingRateUpdateCallback. ServerId = " << iServerId << "PingRate: " <<  QString::number(pPingRate->rate.iSec);

    RemoteCare::RCDataItemType_t type;

    if ( confWrapper.GetDataItemType(RemoteCare::RC_DATAITEM_SET_PING_RATE, type) )
    {
    	incomingCommandQueue.append(new NetCommands::CmdRCSetTag(2000,
                                                             RemoteCare::RC_DATAITEM_SET_PING_RATE,
                                                             type,
                                                             QString::number(pPingRate->rate.iSec)));
    }
}

/****************************************************************************/
/*!
*  \brief This callback is invoked when the download for a series of one or
*         more files begins. The callback must return AeTrue if the user
*         application intends to perform custom processing on the downloaded
*         files. In this case, Agent Embedded will pass the incoming file
*         data to the application using the callback registered with
*         AeDRMSetOnFileDownloadData(). The callback must return AeFalse,
*         if Agent Embedded should process downloaded files internally. In this
*         case the Data and EndCallback will not be called.
*
*  \iparam  iDeviceId  = ID of the target device
*  \iparam  ppUserData = (optional) output parameter, completed by this callback
*
*  \return AeTrue if custom processing of downloaded files shall be performed, AeFalse otherwise.
*
*/
/*****************************************************************************/
AeBool REServerConnector::OnFileDownloadBeginCallback(AeInt32 iDeviceId, AePointer *ppUserData)
{
    Q_UNUSED(ppUserData);

    Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_INFO_START_DOWNLOAD);
    qDebug() << "*** CB: OnFileDownloadBeginCallback. DeviceId = " << iDeviceId;

    return AeTrue;
}

/****************************************************************************/
/*!
*  \brief This callback is invoked when Agent Embedded receives a portion of a downloaded file
*
*  \iparam  iDeviceId = ID of the target device
*  \iparam  pFile     = pointer to the description of the file for download
*  \iparam  pData     = pointer to the block of received file data
*  \iparam  iSize     = pointer to the number of bytes in the block of received file data
*  \iparam  pUserData = pointer previously returned by the application via callback
*
*  \return AeTrue if data processing status is successful, AeFalse otherwise.
*
*/
/******************************************************************************/
AeBool REServerConnector::OnFileDownloadDataCallback(AeInt32 iDeviceId, AeFileStat *pFile, AeChar *pData, AeInt32 iSize, AePointer pUserData)
{
    Q_UNUSED(pUserData)
    static bool isInitialized = false;

    if (pFile == NULL)
    {
        qDebug() << "*** CB: OnFileDownloadDataCallback. pFile is NULL";
        return AeFalse;
    }

    if (!isInitialized)
    {
        string filename(pFile->pName);
        string tmpDownloadPath;
        tmpDownloadPath = downloadPath + "/" + filename;

        qDebug() << "*** CB: OnFileDownloadDataCallback. DeviceId =" << iDeviceId
                 << " DataSize = " << iSize
                 << "Name " << pFile->pName
                 << "Path " << tmpDownloadPath.c_str();

        ofs.open(tmpDownloadPath.c_str() , ios::out | ios::binary);
        isInitialized = true;
    }

    (void)ofs.write(pData, iSize);

    ///< End of file
    if (iSize == 0)
    {
        qDebug() << "*** CB: OnFileDownloadDataCallback. End of file" ;
        isInitialized = false;
    }

    return AeTrue;
}

/****************************************************************************/
/*!
 *  \brief   This callback is invoked when Agent Embedded receives the
 *           File download end callback
 *
 *  \iparam  iDeviceId  = ID of the target device
 *  \iparam  bOK        = success of the download
 *  \iparam  pUserData  = pointer to the structure for invocation results
 */
/*****************************************************************************/
void REServerConnector::OnFileDownloadEndCallback(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData)
{
    Q_UNUSED(pUserData);

    Global::EventObject::Instance().RaiseEvent(EVENT_REMOTECARE_INFO_END_DOWNLOAD);

    qDebug() << "*** CB: OnFileDownloadEndCallback. ServerId = " << iDeviceId << "OK or not" << bOK;

    ofs.close();

    RemoteCare::RCDataItemType_t type;

    if ( confWrapper.GetDataItemType(RemoteCare::RC_DATAITEM_SET_DOWNLOAD_FINISHED, type) )
    {
        qDebug() << "*** CB: OnFileDownloadEndCallback. Type =" << static_cast<int>(type);

    	incomingCommandQueue.append(new NetCommands::CmdRCSetTag(2000,
                                                             RemoteCare::RC_DATAITEM_SET_DOWNLOAD_FINISHED,
                                                             type,
                                                             QString::number(bOK)));
    }
}

} // end namespace

