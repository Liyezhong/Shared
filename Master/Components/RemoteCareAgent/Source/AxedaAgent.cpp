/****************************************************************************/
/*! \file AxedaAgent.cpp
 *
 *  \brief Implementation file for class AxedaAgent.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 04.07.2011
 *  $Author:    $ Y.Novak
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

#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QCoreApplication>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/GlobalDefines.h>
#include <RemoteCareAgent/Include/AxedaAgent.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaRemoteSessionRequest.h>

namespace RCAgentNamespace {

/// This is static just because it is used in Callbacks, which need to be static.
AgentConfiguration              AxedaAgent::m_AgentConfig;
/// This is static just because it is used in Callbacks, which need to be static.
QList<AgentUploadContainer*>    AxedaAgent::m_UploadContainers;
/// This is static just because it is used in Callbacks, which need to be static.
QList<Global::CommandShPtr_t>   AxedaAgent::m_IncomingCommandQueue;
/// This is static just because it is used in Callbacks, which need to be static.
AgentCmdFactoryHash_t           AxedaAgent::m_CmdFactories;

const AgentCmdFactoryShPtr_t  NullFactory(NULL); ///< non-existent NULL factory.

/****************************************************************************/
/*!
*  \brief    Constructor.
*
*****************************************************************************/
AxedaAgent::AxedaAgent() :
            m_AgentState(AGENT_NOT_STARTED)
{
    m_UploadContainers.clear();
}

/****************************************************************************/
/*!
*  \brief    Destructor.
*
*****************************************************************************/
AxedaAgent::~AxedaAgent()
{
    try {
        // delete all files which are queued for upload
        for (int i = 0; i < m_UploadContainers.size(); i++) {
            if (m_UploadContainers[i] != NULL) {
                if (m_UploadContainers[i]->FileData != NULL) {
                    qDebug() << "~AxedaAgent: deleting container's data.";
                    delete m_UploadContainers[i]->FileData;
                    m_UploadContainers[i]->FileData = NULL;
                }
                qDebug() << "~AxedaAgent: deleting container Nr." << i;
                delete m_UploadContainers[i];
                m_UploadContainers[i] = NULL;
            }
        }
        m_UploadContainers.clear();

        // clean waiting commands:
        m_IncomingCommandQueue.clear();
//        for (int i = 0; i < m_IncomingCommandQueue.size(); i++) {
//            if (m_IncomingCommandQueue[i] != NULL) {
//                delete m_IncomingCommandQueue[i];
//                m_IncomingCommandQueue.removeAt(i);
//            }
//        }

        ///< \todo: cleanup whatever is left to cleanup
    }
    catch(...) {
        // to please Lint
    }
}

/****************************************************************************/
/*!
*  \brief   Initialize AxedaAgent
*
*  \return  true if successfully initialized, false otherwise
*
*****************************************************************************/
bool AxedaAgent::Initialize()
{
    try {
        // 1. read settings from disk
        if (!ReadSettings()) {
            qDebug() << "AxedaAgent ERROR: cannot read settings !";
            return false;
        }
        // 2. initialize Agent
        if (!SetSettings()) {
            qDebug() << "AxedaAgent ERROR: cannot set settings !";
            return false;
        }

        // register messages
        RegisterAgentMessage<CmdAxedaRemoteSessionRequest>();
    }
    catch(...) {
        // does not matter what it is, the agent controller failed to initialize.
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
*  \brief   Read AxedaAgent settings from filesystem
*
*  \return  true if successfully read settings, false otherwise
*
*****************************************************************************/
bool AxedaAgent::ReadSettings()
{
    /// \todo: implement! the settings are hardcoded so far (in constructor)

    return true;
}

/****************************************************************************/
/*!
*  \brief   Get function for AeDrmExecute's execution period
*
*  \return  time in seconds
*
*****************************************************************************/
AeInt32 AxedaAgent::GetExecTime()
{
    return m_AgentConfig.ExecTime.iSec;
}

/****************************************************************************/
/*!
*  \brief   Apply AxedaAgent settings
*
*  \return  true if successfully applied settings, false otherwise
*
*****************************************************************************/
bool AxedaAgent::SetSettings()
{
    AeError rc;

    // run the default Axeda Agent Embedded initialization
    if (AeInitialize() != AeEOK) {
        qDebug() << "AxedaAgent ERROR: failed to run the default Agent init !";
        return false;
    }

    // set message queue size
    AeDRMSetQueueSize(m_AgentConfig.QueueSize);

    // add device, which shall be managed by Agent
    rc = AeDRMAddDevice(m_AgentConfig.DeviceType, m_AgentConfig.pModelNumber, m_AgentConfig.pSerialNumber, &m_AgentConfig.DeviceId);
    qDebug() << "AxedaAgent: got DeviceId = " << m_AgentConfig.DeviceId;
    if (rc != AeEOK)
    {
        qDebug() << "AxedaAgent ERROR: failed to add device " << m_AgentConfig.DeviceId << " with error: " << AeGetErrorString(rc);
        return false;
    }

    // tell Agent which Remote Care server to use
    rc = AeDRMAddServer(m_AgentConfig.ServerType, m_AgentConfig.pServerUrl, m_AgentConfig.pOwner, &m_AgentConfig.PingRate, &m_AgentConfig.ServerId);
    qDebug() << "AxedaAgent: got ServerId = " << m_AgentConfig.ServerId;
    if (rc != AeEOK)
    {
        qDebug() << "AxedaAgent ERROR: failed to add server " << m_AgentConfig.pServerUrl << " with error: " << AeGetErrorString(rc);
        return false;
    }

    // set HTTP persistence
    if (m_AgentConfig.bPersist == true) {
        AeError err= AeWebSetPersistent(m_AgentConfig.ServerId, true);
        if (err != AeEOK) {
            qDebug() << "AxedaAgent ERROR: AeWebSetPersistent failed ! Error code: " << static_cast<int>(err);
            return false;
        }
    }

    // set remote session parameters
    if (m_AgentConfig.bRemoteSessionSetup == true) {
        if ((m_AgentConfig.pRemoteSessionName != NULL) && (m_AgentConfig.pRemoteSessionType != NULL) &&
            (m_AgentConfig.pRemoteSessionHost != NULL) && (m_AgentConfig.RemoteSessionPort != 0))
        {
            rc = AeDRMAddRemoteSession(m_AgentConfig.DeviceId, m_AgentConfig.pRemoteSessionName, (AeChar*)"", \
                                       m_AgentConfig.pRemoteSessionType, m_AgentConfig.pRemoteSessionHost, m_AgentConfig.RemoteSessionPort);
            if (rc != AeEOK)
            {
                qDebug() << "AxedaAgent ERROR: Failed to add Remote session, error: " << AeGetErrorString(rc);
                return false;
            }

            // set the runtime flag for Remote Session (default false)
            ///< \todo: (future) check other conditions here if needed
            m_AgentConfig.bAllowRemoteSession = AeFalse;
        }
    }

    // set SSL settings
    if (m_AgentConfig.bSecure == true)
    {
        rc = AeWebSetSSL(m_AgentConfig.CryptLevel, m_AgentConfig.bAuthenticate, m_AgentConfig.pCertFile);
        if (rc != AeEOK)
        {
            qDebug() << "AxedaAgent ERROR: Failed to set SSL parameters, error: " << AeGetErrorString(rc);
            return false;
        }
    }

    // set proxy server settings
    if (m_AgentConfig.bUseProxy == true)
    {
        rc = AeWebSetProxy(m_AgentConfig.ProxyProtocol, m_AgentConfig.pProxyHost, m_AgentConfig.ProxyPort, \
                           m_AgentConfig.pProxyUser, m_AgentConfig.pProxyPassword);
        if (rc != AeEOK)
        {
            qDebug() << "AxedaAgent ERROR: Failed to configure proxy, error: " << AeGetErrorString(rc);
            return false;
        }
    }

    // enable/disable debug messages
    if (m_AgentConfig.bDebug == true)
    {
        AeDRMSetLogLevel(AeLogDebug);
    }
    else {
        AeDRMSetLogLevel(AeLogNone);
    }

    // register all callbacks
    RegisterAllCallbacks();

    return true;
}

/****************************************************************************/
/*!
*  \brief   Axeda Agent main loop.
*
*      This function blocks until Agent is requested to quit. Think of it
*      as of a generic thread's "run" function.
*      Call QCoreApplication::processEvents() to process pending events.
*
*****************************************************************************/
void AxedaAgent::Work()
{
    AeBool        connected;

    ChangeAgentState(AGENT_IS_RUNNING);
    qDebug() << "\nAxedaAgent: entered working function... \n";

    do  // execute agent
    {
        // this is the execution cycle
        if (AeDRMExecute(&m_AgentConfig.ExecTime) != AeEOK) {
            qDebug() << "AxedaAgent ERROR :  AeDRMExecute returned false !";
        }

        // check if agent is actually connected to the Server
        if (AeDRMGetServerStatus(m_AgentConfig.ServerId, &connected)) {
            if (!connected) {
                ///< \todo: do we need to do anything here?
                qDebug() << "AxedaAgent WARNING :  Agent is not connected to RC-EServer !";
            }
        }

        // check if there are any incoming commands in the queue and execute pending ones
        // call this function before the "processEvents()"!
        CheckIncomingCommandQueue();

        // Process signals and other events accumulated in the queue while AeDRMExecute was blocking
        QCoreApplication::processEvents();
        qDebug() << "AxedaAgent Working :  going into next working cycle... ";

    } while (m_AgentState == AGENT_IS_RUNNING);

    // shutdown Axeda Agent Embedded
    AeShutdown();
    qDebug() << "\nAxedaAgent: exiting working function... \n";
    ChangeAgentState(AGENT_STOPPED);
}

/****************************************************************************/
/*!
*  \brief   Stop Axeda Agent operation
*
*****************************************************************************/
void AxedaAgent::Stop()
{
    ChangeAgentState(AGENT_GOING_DOWN);
    qDebug() << "\nAxedaAgent: received request to stop operation... \n";
}

/****************************************************************************/
/*!
*  \brief   Get function for Agent's state
*
*  \return  Current state of the Agent
*
*****************************************************************************/
AgentStates_t AxedaAgent::GetState()
{
    return m_AgentState;
}

/****************************************************************************/
/*!
*  \brief   Convert Master timestamp into Agent time format
*
*  \param[in]  timevalue = Master's timestamp value
*  \param[out] timestamp = pointer to Agent's timestamp structure
*
*****************************************************************************/
void AxedaAgent::ConvertTime(QString timevalue, AeTimeValue *timestamp)
{
    QDateTime dt = QDateTime::fromString(timevalue, RCAgentNamespace::DATEANDTIME_FORMAT);
    timestamp->iSec = static_cast<AeInt32>(dt.toTime_t());
    timestamp->iMicroSec = 0;
}

/****************************************************************************/
/*!
*  \brief   Submit Alarm to be sent to Server
*
*  \param[in] ref = request's reference
*  \param[in] name = alarm's name
*  \param[in] message = alarm's description text
*  \param[in] severity = alarm's severity
*  \param[in] condition = alarm's condition
*  \param[in] timestamp = Master's timestamp value
*
*  \warning   If timestamp is in the past, the Axeda Server will not complain
*             about the message contents, but will silently discard the
*             message. At least no Alarm will be shown on the WEB interface.
*
*****************************************************************************/
void AxedaAgent::SubmitAlarmRequest(quint64 ref, QByteArray name, QByteArray message, const QString &severity, \
                                    QByteArray condition, const QString &timestamp)
{
    AeDRMAlarm alarmData;

    qDebug() << "AxedaAgent::SubmitAlarmRequest : " << QString(name);

    // set time stamp
    ConvertTime(timestamp, &alarmData.timeStamp);
    // prepare the alarm
    alarmData.pName = static_cast<AeChar*>(name.data());
    alarmData.pDescription = static_cast<AeChar*>(message.data());
    alarmData.pCondition = static_cast<AeChar*>(condition.data());
    alarmData.bActive = AeTrue;
    alarmData.bAck = AeFalse;
    alarmData.pDataItem = NULL;
    bool ok = false;
    alarmData.iSeverity = (AeInt)severity.toInt(&ok, 10);
    if (!ok) {
        qDebug() << "AxedaAgent ERROR :  could not convert alarm severity value :  " << severity << " !";
        // use "error" value:
        alarmData.iSeverity = (AeInt)(-1);
    }
    // post alarm
    if (AeDRMPostAlarm(m_AgentConfig.DeviceId, m_AgentConfig.ServerId, AeDRMQueuePriorityUrgent, &alarmData) != AeEOK) {
        qDebug() << "AxedaAgent: posting alarm with name :  " << name << " failed !\n";
        emit SigRequestStatus(ref, AGENT_REQUEST_FAILED);
        return;
    }

    emit SigRequestStatus(ref, AGENT_REQUEST_OK);
}

/****************************************************************************/
/*!
*  \brief   Submit Event to be sent to Server
*
*  \param[in] ref = request's reference
*  \param[in] name = event's name
*  \param[in] message = event's description text
*  \param[in] severity = event's severity
*  \param[in] timestamp = Master's timestamp value
*
*  \warning   If timestamp is in the past, the Axeda Server will not complain
*             about the message contents, but will silently discard the
*             message. At least no Event will be shown on the WEB interface.
*
*****************************************************************************/
void AxedaAgent::SubmitEventRequest(quint64 ref, QByteArray name, QByteArray message, const QString &severity, const QString &timestamp)
{
    AeDRMEvent    drmEvent;

    qDebug() << "AxedaAgent::SubmitEventRequest : " << QString(name);

    // set time stamp
    ConvertTime(timestamp, &drmEvent.timeStamp);
    // prepare event
    drmEvent.pName = static_cast<AeChar*>(name.data());
    drmEvent.pMessage = static_cast<AeChar*>(message.data());
    bool ok = false;
    drmEvent.iSeverity = (AeInt)severity.toInt(&ok, 10);
    if (!ok) {
        qDebug() << "AxedaAgent ERROR :  could not convert event severity value :  " << severity << " !";
        // use "error" value:
        drmEvent.iSeverity = (AeInt)(-1);
    }
    // post event
    if (AeDRMPostEvent(m_AgentConfig.DeviceId, m_AgentConfig.ServerId, AeDRMQueuePriorityNormal, &drmEvent) != AeEOK) {
        qDebug() << "AxedaAgent ERROR :  could not post following Event :  " << name << " !";
        emit SigRequestStatus(ref, AGENT_REQUEST_FAILED);
        return;
    }

    emit SigRequestStatus(ref, AGENT_REQUEST_OK);
}

/****************************************************************************/
/*!
*  \brief   Submit Data Item to be sent to Server
*
*  \param[in] ref = request's reference
*  \param[in] name = parameter's name
*  \param[in] type = type of the parameter (analog/digital/string)
*  \param[in] quality = quality of the parameter
*  \param[in] value = the value of the parameter
*  \param[in] timestamp = Master's timestamp value
*
*  \warning   If timestamp is in the past, the Axeda Server will not complain
*             about the message contents, but will silently discard the
*             message.
*
*****************************************************************************/
void AxedaAgent::SubmitDataItemRequest(quint64 ref, QByteArray name, AxedaDataItemType_t type, AxedaDataItemQuality_t quality, \
                                       QByteArray value, const QString &timestamp)
{
    AeDRMDataItem dataItem;
    bool ok = true;

    qDebug() << "AxedaAgent::SubmitDataItemRequest : " << QString(name);

    // set time stamp
    ConvertTime(timestamp, &dataItem.value.timeStamp);
    // prepare data item
    dataItem.pName = static_cast<AeChar*>(name.data());
    dataItem.value.iType = static_cast<AeDRMDataType>(type);
    dataItem.value.iQuality = static_cast<AeDRMDataQuality>(quality);
    switch (dataItem.value.iType) {
    case AeDRMDataAnalog:
        dataItem.value.data.dAnalog = value.toFloat(&ok);
        if (!ok) {
            qDebug() << "AxedaAgent ERROR :  cannot convert value of type " << static_cast<int>(type) << " !";
            qDebug() << "AxedaAgent ERROR :  converted value =" << dataItem.value.data.dAnalog;
        }
        break;
    case AeDRMDataDigital:
        dataItem.value.data.bDigital = value.toShort(&ok, 10);
        if ((!ok) || (dataItem.value.data.bDigital > 1) || (dataItem.value.data.bDigital < 0)) {
            qDebug() << "AxedaAgent ERROR :  cannot convert value of type " << static_cast<int>(type) << " !";
            ok = false;
        }
        break;
    case AeDRMDataString:
        dataItem.value.data.pString = static_cast<AeChar*>(value.data());
        break;
    default:
        qDebug() << "AxedaAgent ERROR :  do not know the dataItem type " << static_cast<int>(type) << " !";
        ok = false;
        break;
    }
    // check if data conversion was ok
    if (ok) {
        // submit data to outgoing queue:
        if (AeDRMPostDataItem(m_AgentConfig.DeviceId, m_AgentConfig.ServerId, AeDRMQueuePriorityNormal, &dataItem) != AeEOK) {
            qDebug() << "AxedaAgent ERROR :  could not post DataItem " << name << " for upload !";
            ok = false;
        }
    }
    // check if request was submitted
    if (!ok) {
        emit SigRequestStatus(ref, AGENT_REQUEST_FAILED);
        return;
    }
    // all ok, inform upper layer
    emit SigRequestStatus(ref, AGENT_REQUEST_OK);
}

/****************************************************************************/
/*!
*  \brief   Submit a file to be uploaded to Server
*
*  \param[in] ref = request's reference
*  \param[in] filename = the name of the file, which shall be located in "Uploads".
*
*****************************************************************************/
void AxedaAgent::SubmitUploadRequest(quint64 ref, QByteArray filename)
{
    qDebug() << "AxedaAgent::SubmitUploadRequest : " << QString(filename);

    AeFileUploadSpec uploadSpec;
    AeFileUploadSpec *ppUploads[2];
    AeFileUploadRequestParam param;

    // prepare upload data structures
    uploadSpec.pName = static_cast<AeChar*>(filename.data());
    uploadSpec.bDelete = AeFalse; ///< \todo: maybe this one shall be set to "true" for automatic deletion

    // upload specification list must be NULL-terminated
    ppUploads[0] = &uploadSpec;
    ppUploads[1] = NULL;

    // check if compression is required
    if (m_AgentConfig.bCompress == true) {
        // compress the file
        param.iMask = AE_FILE_UPLOAD_REQUEST_PARAM_COMPRESSION;
        param.iCompression = AeFileCompressionTarGzip;
    }
    else {
        // no compression for the file
        param.iMask = 0x0;
        param.iCompression = AeFileCompressionNone;
    }

    // submit upload request
    if (AeDRMPostFileUploadRequestEx(m_AgentConfig.DeviceId, m_AgentConfig.ServerId, AeDRMQueuePriorityNormal, ppUploads, &param) != AeEOK) {
        qDebug() << "AxedaAgent ERROR :  could not submit file " << *(uploadSpec.pName) << " for upload !";
        emit SigRequestStatus(ref, AGENT_REQUEST_FAILED);
        return;
    }

    emit SigRequestStatus(ref, AGENT_REQUEST_OK);
}

/****************************************************************************/
/*!
 *  \brief   This slot sets Remote Session Status on Axeda Agent
 *
 *  \param[in] status = true for enable, false for disable
 *
 *****************************************************************************/
void AxedaAgent::SetRemoteSessionStatus(bool status)
{
    qDebug() << "*** AxedaAgent: setting RemoteSessionStatus to " << status;
    m_AgentConfig.bAllowRemoteSession = status;
}

/****************************************************************************/
/*!
*  \brief  Create a creation hash entry for a particular command
*
*  \param[in]  cmdname = the name of the command served by this factory
*  \param[in]  factory = the factory for this command
*
*****************************************************************************/
void AxedaAgent::RegisterCmdFactory(const QString &cmdname, const AgentCmdFactoryShPtr_t &factory)
{
    // check if already registered
    if (m_CmdFactories.contains(cmdname)) {
        /// \todo: inform Master?
        qDebug() << "AxedaAgent: Command Factory for " << cmdname << " already registered";
        return;
    }
    // everything OK
    static_cast<void> (
        // we DO NOT NEED the return value of insert
        m_CmdFactories.insert(cmdname, factory)
    );
}

/****************************************************************************/
/*!
*  \brief  Get access to a previously created command creating factory
*
*  \param[in]  cmdname = the name of the command to fetch a factory for
*
*  \return  command factory access pointer on success, NULL-Factory on failure
*
*****************************************************************************/
AgentCmdFactoryShPtr_t AxedaAgent::GetCmdFactory(const QString &cmdname)
{
    AgentCmdFactoryHash_t::const_iterator it = m_CmdFactories.find(cmdname);
    if (it == m_CmdFactories.constEnd()) {
        // functor not found
        // return NULL factory
        return NullFactory;
    }
    // return functor
    return it.value();
}

/****************************************************************************/
/*!
*  \brief   One function to register all available callbacks.
*
*****************************************************************************/
void AxedaAgent::RegisterAllCallbacks()
{
    // Status reports related callbacks:
    AeDRMSetOnWebError(&OnWebErrorCallback);
    AeDRMSetOnDeviceRegistered(&OnDeviceRegisteredCallback);
    AeDRMSetOnQueueStatus(&QueueStatusCallback);

    // Server commands related callbacks:
    AeDRMSetOnCommandSetTime(&OnCommandSetTimeCallback);
    AeDRMSetOnCommandRestart(&OnCommandRestartCallback);
    AeDRMSetOnPingRateUpdate(&OnPingRateUpdateCallback);
    AeDRMSetOnCommandSetTag(&OnCommandSetTagCallback);

    // Remote Session related callbacks:
    AeDRMSetOnRemoteSessionEnd(&OnRemoteSessionEndCallback);
    AeDRMSetOnRemoteSessionStart(&OnRemoteSessionStartCallback);

    // SOAP methods related callbacks:
    AeDRMSetOnSOAPMethod(&OnSOAPMethodCallback);
    AeDRMSetOnSOAPMethodEx(&OnSOAPMethodExCallback);

    // File transfer related callbacks:
    AeDRMSetOnFileDownloadBegin(&OnFileDownloadBeginCallback);
    AeDRMSetOnFileDownloadEnd(&OnFileDownloadEndCallback);
    AeDRMSetOnFileUploadEnd(&OnFileUploadEndCallback);
    AeDRMSetOnFileTransferEvent(&OnFileTransferEventCallback);
    AeDRMSetOnFileDownloadData(&OnFileDownloadDataCallback);
    AeDRMSetOnFileUploadBegin(&OnFileUploadBeginCallback);
    AeDRMSetOnFileUploadBeginEx(&OnFileUploadBeginExCallback);
    AeDRMSetOnFileUploadData(&OnFileUploadDataCallback);
}

/****************************************************************************/
/*!
*  \brief   As soon as a remote session starts, this callback is invoked.
*
*  \param[in] pInterface = a pointer to the information about this remote session
*
*  \return  if Remote Session is allowed, return true, of not - return false
*
*****************************************************************************/
AeBool AxedaAgent::OnRemoteSessionStartCallback(AeRemoteInterface *pInterface)
{
/*!
      \footnote
      AgentEmbedded (at least its version 6.1.355) does not have an API to cancel a running RemoteSession.
      But we can do it with linux system() call to iptables.
        Open port:
          sudo iptables -A INPUT -p tcp --dport 5900 -j ACCEPT
          sudo iptables -A OUTPUT -p tcp --dport 5900 -j ACCEPT
        Close port:
          sudo iptables -A INPUT -p tcp --dport 5900 -j DROP
          sudo iptables -A OUTPUT -p tcp --dport 5900 -j DROP
*/

    qDebug() << "*** CB: OnRemoteSessionStartCallback. Device = " << pInterface->iDeviceId << " Port =  " << pInterface->iPort \
    << " Server =  " << pInterface->pServer;
    if (m_AgentConfig.bAllowRemoteSession == AeTrue) {
        // session can start, but check the parameters first:
        if ((m_AgentConfig.DeviceId == pInterface->iDeviceId) &&
            (*(m_AgentConfig.pRemoteSessionType) == *(pInterface->pRemoteType)) &&
            (*(m_AgentConfig.pRemoteSessionName) == *(pInterface->pName))) {
            return AeTrue;
        }
        // session is allowed, but is not configured
        return AeFalse;
    }
    // session is currently not allowed to start
    return AeFalse;
}

/****************************************************************************/
/*!
*  \brief   As soon as a remote session ends, this callback is invoked.
*
*  \param[in] pInterface = a pointer to the information about this remote session
*
*****************************************************************************/
void AxedaAgent::OnRemoteSessionEndCallback(AeRemoteInterface *pInterface)
{
    qDebug() << "*** CB: OnRemoteSessionEndCallback. Device = " << pInterface->iDeviceId << " Port =  " << pInterface->iPort \
             << " Server =  " << pInterface->pServer;

    /// \todo: check if this is ok -->
    /// disable remote sessions. each new remote session shall be explicitly approved by user.
    m_AgentConfig.bAllowRemoteSession = AeFalse;
    qDebug() << "*** AxedaAgent: 've set RemoteSessionStatus to false.";
}

/****************************************************************************/
/*!
*  \brief   This callback is invoked when a communication error occurs while
*           Agent Embedded is performing pending tasks in AeDRMExecute().
*
*  \param[in] iServerId = server ID of the Enterprise server that caused the error
*  \param[in] iError = error code
*
*****************************************************************************/
void AxedaAgent::OnWebErrorCallback(AeInt32 iServerId, AeError iError)
{
    qDebug() << "*** CB: OnWebErrorCallback. ServerId = " << iServerId << " Error = " << static_cast<int>(iError);

    ///< \todo: handle web error (?)
}

/****************************************************************************/
/*!
*  \brief   This callback is invoked when a device is successfully
*           registered while in AeDRMExecute().
*
*  \param[in] iDeviceId = ID of registered device
*
*****************************************************************************/
void AxedaAgent::OnDeviceRegisteredCallback(AeInt32 iDeviceId)
{
    qDebug() << "*** CB: OnDeviceRegisteredCallback. DeviceId = " << iDeviceId;
}

/****************************************************************************/
/*!
*  \brief   This callback is invoked whenever the status of the internal
*           data queue is changed.
*
*  \param[in] iNewStatus = the actual queue status
*
*****************************************************************************/
void AxedaAgent::QueueStatusCallback(AeDRMQueueStatus iNewStatus)
{
    qDebug() << "*** CB: QueueStatusCallback. Status = " << static_cast<int>(iNewStatus);
}

/****************************************************************************/
/*!
*  \brief   This callback is invoked when a SOAP method is received from
*           server and the method is not known (that is, not handled by any
*           of the callback functions registered through AeDRMSetOnCommandXXX()).
*
*  \param[in] iDeviceId = ID of target device
*  \param[in] pMethod = handle to the SOAP method received from server
*  \param[in] pStatus = pointer to the structure for invocation results
*
*****************************************************************************/
void AxedaAgent::OnSOAPMethodCallback(AeInt32 iDeviceId, AeHandle pMethod, AeDRMSOAPCommandStatus *pStatus)
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
*  \param[in] iDeviceId = ID of target device
*  \param[in] iServerId = ID of the Enterprise server that sent the SOAP method
*  \param[in] pMethod = handle to the SOAP method received from server
*  \param[in] pSOAPId = handle to the SOAP method ID
*  \param[in] pStatus = pointer to the structure for invocation results
*
*****************************************************************************/
void AxedaAgent::OnSOAPMethodExCallback(AeInt32 iDeviceId, AeInt32 iServerId, AeHandle pMethod, \
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
*  \param[in] iDeviceId = ID of the target device
*  \param[in] pDataItem = name of the data item and its new value
*  \param[in] pStatus = pointer to the structure for invocation results
*
*****************************************************************************/
void AxedaAgent::OnCommandSetTagCallback(AeInt32 iDeviceId, AeDRMDataItem *pDataItem, AeDRMSOAPCommandStatus *pStatus)
{
    qDebug() << "*** CB: OnCommandSetTagCallback. DeiceId = " << iDeviceId << "/ DataItemName =  " << pDataItem->pName \
                                                              << "/ DataItemValueType =  " << static_cast<int>(pDataItem->value.iType);

    pStatus->iStatusCode = AE_DRM_SOAP_COMMAND_STATUS_SUCCESS;
    QString ParameterName = AGENT_COMMAND_PREFIX + QString(pDataItem->pName);
    AeDRMDataType ParameterType = pDataItem->value.iType;
    QString ParameterValue = "";
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
        SubmitSetTagCommand(ParameterName, (AxedaDataItemType_t)(ParameterType), ParameterValue);
    }
}

/****************************************************************************/
/*!
*  \brief   This function submits the Server's SetTag command to the
*           waiting queue.
*
*     As soon as Agent exits the AeDRMExecute, the Queue will be
*     revisited and new requests will be processed.
*
*  \param   name = name of the parameter which Server wants to set/change
*  \param   type = parameter type (digital/analog/string)
*  \param   value = parameter value
*
*****************************************************************************/
void AxedaAgent::SubmitSetTagCommand(const QString &name, AxedaDataItemType_t type, const QString &value)
{
    //CmdAxedaRemoteSessionRequest
    AgentCmdFactoryShPtr_t Factory = (const AgentCmdFactoryShPtr_t) GetCmdFactory(name);
    if (Factory == NullFactory) {
        /// \todo: report error?
        qDebug() << "AxedaAgent::SubmitSetTagCommand ERROR: cannot find appropriate command factory !";
        return;
    }
    Global::CommandShPtr_t CmdShPtr = Factory.GetPointerToUserData()->CreateInstance();
    if (CmdShPtr.GetPointerToUserData() == NULL) {
        /// \todo: report error?
        qDebug() << "AxedaAgent::SubmitSetTagCommand ERROR: factory returned a broken Instance !";
        return;
    }
    // check if this is really a SetTagCommand
    const CmdAxedaSetTag *pCmd = dynamic_cast<const CmdAxedaSetTag*>(CmdShPtr.GetPointerToUserData());
    if (pCmd == NULL) {
        // no luck
        /// \todo: report error?
        qDebug() << "AxedaAgent::SubmitSetTagCommand ERROR: failed to dynamic_cast the Instance !";
        return;
    }
    // set the command data
    ((CmdAxedaSetTag*)(CmdShPtr.GetPointerToUserData()))->SetCommandData(type, value);
    // submit command to the queue
    m_IncomingCommandQueue.append(CmdShPtr);
    pCmd = NULL;
}

/****************************************************************************/
/*!
*  \brief   This function checks the incoming command queue and acts on
*           pending commands.
*
*****************************************************************************/
void AxedaAgent::CheckIncomingCommandQueue()
{
    if (m_IncomingCommandQueue.isEmpty()) {
        // queue is empty: nothing to do.
        return;
    }
    // there are pending commands
    for (int i = 0; i < m_IncomingCommandQueue.size(); i++) {
        /// \todo: what if we have too many pending commands?
        ///  the time spent in this function shall not cause the next AeDRMExecute cycle
        ///  to shift outside of the Server ping window.
        SigAgentIncomingCommand(m_IncomingCommandQueue[i]);
        m_IncomingCommandQueue.removeAt(i);
    }
}

/****************************************************************************/
/*!
*  \brief   This callback is invoked when the download for a series of one or
*           more files begins. The callback must return AeTrue if the user
*           application intends to perform custom processing on the downloaded
*           files. In this case, Agent Embedded will pass the incoming file
*           data to the application using the callback registered with
*           AeDRMSetOnFileDownloadData(). The callback must return AeFalse,
*           if Agent Embedded should process downloaded files internally
*           (that is, store them in the local file system).
*
*  \param[in] iDeviceId = ID of the target device
*  \param[in] ppUserData = (optional) output parameter, completed by this callback
*
*  \return  AeTrue if custom processing of downloaded files shall be performed,
*           AeFalse otherwise.
*
*****************************************************************************/
AeBool AxedaAgent::OnFileDownloadBeginCallback(AeInt32 iDeviceId, AePointer *ppUserData)
{
    Q_UNUSED(ppUserData)
    qDebug() << "*** CB: OnFileDownloadBeginCallback. DeviceId = " << iDeviceId;
    return AeFalse;
}

/****************************************************************************/
/*!
*  \brief   This callback is invoked when Agent Embedded receives a portion
*           of a downloaded file
*
*  \param[in] iDeviceId = ID of the target device
*  \param[in] pFile = pointer to the description of the file for download
*  \param[in] pData = pointer to the block of received file data
*  \param[in] iSize = pointer to the number of bytes in the block of received file data
*  \param[in] pUserData = pointer previously returned by the application via callback
*
*  \return  AeTrue if data processing status is successful, AeFalse otherwise.
*
*****************************************************************************/
AeBool AxedaAgent::OnFileDownloadDataCallback(AeInt32 iDeviceId, AeFileStat *pFile, AeChar *pData, \
                                              AeInt32 iSize, AePointer pUserData)
{
    Q_UNUSED(pFile)
    Q_UNUSED(pData)
    Q_UNUSED(pUserData)
    qDebug() << "*** CB: OnFileDownloadDataCallback. DeviceId =" << iDeviceId << " DataSize = " << iSize;
    return AeTrue;
}

/****************************************************************************/
/*!
*  \brief   This callback is invoked when Agent Embedded either encounters
*           an error during the file download, or when all files in the
*           series are received successfully. The callback indicates the
*           file download status by returning AeTrue in the case of success
*           or AeFalse otherwise.
*
*  \param[in] iDeviceId = ID of the target device
*  \param[in] bOK = identifies if the download failed (AeFalse) or was successful (AeTrue)
*  \param[in] pUserData = pointer previously returned by the application via callback
*
*****************************************************************************/
void AxedaAgent::OnFileDownloadEndCallback(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData)
{
    Q_UNUSED(pUserData)
    qDebug() << "*** CB: OnFileDownloadEndCallback. DeviceID = " << iDeviceId << " Status = " << bOK;

    ///< \todo: if error occured -> handle it... but how? :-)  Can we inform Server?
}

/****************************************************************************/
/*!
*  \brief   This callback is invoked on receiving the File Upload SOAP method.
*           The callback must return AeTrue if the user application intends
*           to provide data for the uploaded files. In this case, Agent
*           Embedded will request content of the uploaded files from the
*           application through a callback registered with
*           AeDRMSetOnFileUploadData(). The callback must return AeFalse
*           if Agent Embedded should process uploaded files internally (that is,
*           retrieve them from the local file system).
*
*  \param[in]  iDeviceId = target device id
*  \param[in]  ppUploads = NULL-terminated array of file specifications for the upload
*  \param[out] ppUserData = user structure associated with the file upload
*
*  \return  AeFalse if Agent Embedded should process the uploaded files
*           internally, AeTrue otherwise.
*
*****************************************************************************/
AeBool AxedaAgent::OnFileUploadBeginCallback(AeInt32 iDeviceId, AeFileUploadSpec **ppUploads, AePointer *ppUserData)
{
    Q_UNUSED(iDeviceId)
    qDebug() << "*** CB: OnFileUploadBeginCallback. FileName = " << (*ppUploads)->pName;

    try {
        QString filename = Global::SystemPaths::Instance().GetUploadsPath() + AGENT_SLASH_IN_PATH + (QString)((*ppUploads)->pName);
        if (!CheckIfFileExists(filename)) {
            qDebug() << "*** CB: OnFileUploadBeginCallback ERROR. File " << filename << " does not exist !";
             /// we only return TRUE in this function because otherwise Agent will try to handle the Upload by itself.
            *ppUserData = NULL;
            return AeTrue;
        }

        AgentUploadContainer *container = new AgentUploadContainer();
        container->UploadSpecList = ppUploads;
        container->FileStat.pName =  (*ppUploads)->pName;
        m_UploadContainers.append(container);

        // remember the container (AxedaAgent shall handle this)
        *ppUserData = (void*)(container);
    }
    catch (const std::bad_alloc &) {
        ///< \todo: log error and inform Master
        *ppUserData = NULL;
    }

    return AeTrue; ///< we only return TRUE in this function because otherwise Agent will try to handle the Upload by itself.
}

/****************************************************************************/
/*!
*  \brief   This callback is invoked when Agent Embedded receives the File
*           Upload SOAP method, like AeDRMSetOnFileUploadBegin(). The difference
*           from those functions lies in the callbacks, which accept additional
*           upload parameters. If the callback for either of these functions
*           returns AeTrue, the user application intends to provide data for
*           uploaded files. If the callback returns AeFalse, Agent Embedded
*           should process uploaded files internally.
*
*  \param[in]  iDeviceId = target device id
*  \param[in]  ppUploads = NULL-terminated array of file specifications for the upload
*  \param[in]  pParam = additional upload parameters
*  \param[out] ppUserData = user structure associated with the file upload
*
*  \return  AeFalse if Agent Embedded should process the uploaded files
*           internally, AeTrue otherwise.
*
*****************************************************************************/
AeBool AxedaAgent::OnFileUploadBeginExCallback(AeInt32 iDeviceId, AeFileUploadSpec **ppUploads, \
                                               AeFileUploadExecuteParam *pParam, AePointer *ppUserData)
{
    Q_UNUSED(iDeviceId)
    qDebug() << "*** CB: OnFileUploadBeginExCallback. FileName = " << (*ppUploads)->pName;

    try {
        QString filename = Global::SystemPaths::Instance().GetUploadsPath() + AGENT_SLASH_IN_PATH + (QString)((*ppUploads)->pName);
        if (!CheckIfFileExists(filename)) {
            qDebug() << "*** CB: OnFileUploadBeginExCallback ERROR. File " << filename << " does not exist !";
            /// we only return TRUE in this function because otherwise Agent will try to handle the Upload by itself.
           *ppUserData = NULL;
           return AeTrue;
        }

        AgentUploadContainer *container = new AgentUploadContainer();
        container->UploadSpecList = ppUploads;
        container->FileStat.pName = (*ppUploads)->pName;
        m_UploadContainers.append(container);

        // remember the container (AxedaAgent shall handle this)
        *ppUserData = (void*)(container);

        pParam = NULL;
        Q_UNUSED(pParam)     // 4 Lint

        return AeTrue;
    }
    catch (const std::bad_alloc &) {
        ///< \todo: log error and inform Master
        *ppUserData = NULL;
    }

    return AeTrue; ///< we only return TRUE in this function because otherwise Agent will try to handle the Upload by itself.
}

/****************************************************************************/
/*!
*  \brief   This function checks if file exists in the "Uploads" folder.
*
*  \param[in]  filename = the name of the file
*
*  \return  True if file exists, False otherwise.
*
*****************************************************************************/
bool AxedaAgent::CheckIfFileExists(const QString &filename)
{
    return QFile::exists(filename);
}

/****************************************************************************/
/*!
*  \brief   This callback is invoked when Agent Embedded requires more file
*           data for the upload. If the callback function returns AeTrue,
*           the next block of file data is available for upload (successful);
*           if it returns AeFalse, an error occurred.
*
*  \param[in] iDeviceId = target device id
*  \param[in] ppFile = pointer to the file descriptor, provided by application
*  \param[in] ppData = buffer that contains the next block of data, provided
*                      by application
*  \param[in] piSize = size of the buffer that contains the next block of data,
*                      provided by application
*  \param[in] pUserData = pointer previously returned by the application
*                         through the callback registered with
*                         AeDRMSetOnFileUploadBegin()
*
*  \return  AeTrue if next data block is available, AeFalse otherwise.
*
*****************************************************************************/
AeBool AxedaAgent::OnFileUploadDataCallback(AeInt32 iDeviceId, AeFileStat **ppFile, AeChar **ppData, AeInt32 *piSize, \
                                            AePointer pUserData)
{
    Q_UNUSED(iDeviceId)
    qDebug() << "*** CB: OnFile-Upload-DataCallback.";

    // set working data to null
    *ppFile = NULL;
    *ppData = NULL;
    *piSize = 0;

    qDebug() << "*** CB: OnFileUploadDataCallback. pUserData value = " << pUserData;

    // check if we got a valid pointer
    if (pUserData == NULL) {
        ///< \todo: report problem to Master
        return AeFalse;
    }

    /// if some other type of container is used for upload, this will go KABOOM
    AgentUploadContainer *container = (AgentUploadContainer *)pUserData;

    // check if Agent has finished with this container
    if (container->IsUploaded == AeTrue) {
        return AeTrue;
    }

    // we have a non-finished container. process it

    // do all necessary upload file operations
    if (!ProcessUploadContainer(container, piSize)) {
        qDebug() << "AxedaAgent ERROR: cannot process an upload container !";
        ///< \todo: report problem to Master
        // get rid of the faulty container
        DestroyContainer(container);
        return AeFalse;
    }

    // update callback data
    *ppFile = &(container->FileStat);
    *ppData = container->FileData;

    return AeTrue;
}

/****************************************************************************/
/*!
*  \brief   This function performs all necessary file operations for upload
*
*  \param[in]  container = all data needed to fetch and process a local file
*  \param[out] chunksize = size of the current upload chunk
*
*  \return  AeTrue if file processing was successful, AeFalse otherwise.
*
*****************************************************************************/
AeBool AxedaAgent::ProcessUploadContainer(AgentUploadContainer *container, AeInt32 *chunksize)
{
    // get file name
    QString filename = Global::SystemPaths::Instance().GetUploadsPath() + AGENT_SLASH_IN_PATH + QString(static_cast<const char *>((*container->UploadSpecList)->pName));
    qDebug() << "*** CB: ProcessUploadContainer: processing file -->  " << filename;
    // Load file
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "AxedaAgent ERROR: Cannot open file: " << filename;
        /// \todo: report problem to Master
        return AeFalse;
    }
    // get the file size
    qint64 TheFileSize = file.size();
    // check if we need to allocate data container (or if it was already allocated before)
    if (container->FileData == NULL) {
        // create array to store file contents to. Agent will access it in the AeDrmExecute
        container->FileData = static_cast<AeChar *>(malloc(m_AgentConfig.MaxChunkSize));
        if (container->FileData == NULL) {
            qDebug() << "AxedaAgent ERROR: Cannot allocate memory for char array: " << filename;
            ///< \todo: report problem to Master
            file.close();
            return AeFalse;
        }
    }
    // clear memory contents
    memset(container->FileData, 0, m_AgentConfig.MaxChunkSize);

    // read file chunk into container's array
    // start reading from where the last chunk was read -> use the "file contents pointer"
    qint64 startpoint = m_AgentConfig.MaxChunkSize * container->ContentsCounter;
    // check if contents pointer went out of limits:
    if (startpoint >= TheFileSize) {
        // yes it did. Error!
        qDebug() << "AxedaAgent ERROR: file pointer out of borders! File: " << filename << " FileSize: " << TheFileSize << " StartPoint: " << startpoint;
        ///< \todo: report problem to Master
        file.close();
        return AeFalse;
    }
    // contents pointer is OK, jump to the file point where we previously stopped
    if (!file.seek(startpoint)) {
        // cannot find the stop point
        qDebug() << "AxedaAgent ERROR: cannot set file reading point to " << startpoint << " !";
        ///< \todo: report problem to Master
        file.close();
        return AeFalse;
    }
    // found stop point, read the data
    qint64 result = file.peek(container->FileData, m_AgentConfig.MaxChunkSize);
    // file is not needed anymore (at least untill the next chunk is read), close it
    file.close();
    // check if we've read the whole file already or if there is anything wrong with reading process
    if (!CheckFileUploadStatus(result, TheFileSize, startpoint, container)) {
        qDebug() << "AxedaAgent ERROR: file uploading process failed !";
        ///< \todo: report problem to Master
        return AeFalse;
    }

    // all OK, we can continue reading this file
    // update the current "file contents pointer"
    container->ContentsCounter += 1;

    // Set working data
    container->FileStat.pName = static_cast<AeChar*>((*container->UploadSpecList)->pName);
    container->FileStat.iType = AeFileTypeRegular;
    container->FileStat.iMTime = 0;
    container->FileStat.iSize = TheFileSize;
    *chunksize = static_cast<AeInt32>(result);

    return AeTrue;
}

/****************************************************************************/
/*!
*  \brief   This function checks current file's upload status
*
*  \param[in] ReadResult = amount of bytes read from file during last reading
*  \param[in] FileSize = total size of file in bytes
*  \param[in] startposition = contents read start position in the file
*  \param[in] container = the container of the processed file
*
*  \return  AeTrue if file processing was successful, AeFalse if file uploading
*           process failed and shall be stopped
*
*****************************************************************************/
AeBool AxedaAgent::CheckFileUploadStatus(qint64 ReadResult, qint64 FileSize, qint64 startposition, AgentUploadContainer *container)
{
    // calculate total read bytes of this file
    qint64 totalread = ReadResult + startposition;
    // check if end of the file is reached
    if (totalread == FileSize) {
        // yes, file contents are read successfully
        ///< \brief: do not forget to set the IsUploaded flag here, otherwise
        ///          Agent will fail to upload correctly.
        container->IsUploaded = AeTrue;
        return AeTrue;
    }
    // check if we (somehow) read too much
    if (totalread > FileSize) {
        // something is wrong, we've read more data than there is --> ERROR
        qDebug() << "AxedaAgent ERROR: file uploading process read too much data !";
        qDebug() << "AxedaAgent ERROR: totalread = " << totalread;
        qDebug() << "AxedaAgent ERROR: FileSize = " << FileSize;
        return AeFalse;
    }
    // check if we should have read the file by now
    qint64 shouldhaveread = startposition + m_AgentConfig.MaxChunkSize;
    if (shouldhaveread >= FileSize) {
        // something is wrong, we should have read the file by now --> ERROR
        qDebug() << "AxedaAgent ERROR: file uploading process did not manage to read all data !";
        return AeFalse;
    }
    // all OK so far, just continue reading the file
    return AeTrue;
}

/****************************************************************************/
/*!
*  \brief   This callback is invoked when Agent Embedded either encounters
*           an error during the file upload, or when all files are uploaded
*           successfully.
*
*  \param[in] iDeviceId = target device id
*  \param[in] bOK = false if upload failed, true otherwise
*  \param[in] pUserData = pointer previously returned by the application
*             through the callback registered with AeDRMSetOnFileUploadBegin()
*
*****************************************************************************/
void AxedaAgent::OnFileUploadEndCallback(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData)
{
    Q_UNUSED(iDeviceId)
    Q_UNUSED(pUserData)
    qDebug() << "*** CB: OnFileUploadEndCallback. Status = " << bOK;

    if (bOK == false) {
        /// \todo: inform Master about failed operation
    }
    if (pUserData == NULL) {
        // this is bad. we cannot clean up now.
        qDebug() << "*** CB: OnFileUploadEndCallback. Pointer to container is NULL !";
        /// \todo: handle error
        return;
    }
    /// if some other type of container is used for upload, this will go KABOOM
    AgentUploadContainer *container = (AgentUploadContainer*)pUserData;
    // clean up the finished container
    DestroyContainer(container);
}

/****************************************************************************/
/*!
*  \brief   This callback is invoked when Agent Embedded encounters an
*           event during the file transfer.
*
*  \param[in] iDeviceId = ID of the target device
*  \param[in] iEvent = identifies the type of event that occurred during the file transfer
*  \param[in] pUserData = pointer previously returned by the application via callback
*
*****************************************************************************/
void AxedaAgent::OnFileTransferEventCallback(AeInt32 iDeviceId, AeFileTransferEvent iEvent, AePointer pUserData)
{
    Q_UNUSED(pUserData)
    AgentUploadContainer *container = NULL;
    qDebug() << "*** CB: OnFileTransferEventCallback. DeviceId = " << iDeviceId << " Event = " << static_cast<int>(iEvent);
    switch (iEvent) {
    case AeFileTransferCancelled:
        /// if some other type of container is used for upload, this will go KABOOM
        container = (AgentUploadContainer *)pUserData;
        // check if we got a valid pointer
        if (container == NULL) {
            /// \todo: handle error?
        }
        else {
            DestroyContainer(container);
        }
        break;
    default:
    break;
    }
    qDebug() << "*** CB: OnFileTransferEventCallback exited";
}

/****************************************************************************/
/*!
*  \brief   This function does everything to get rid of a file upload container
*
*  \param[in] container = the container to destroy
*
*****************************************************************************/
void AxedaAgent::DestroyContainer(AgentUploadContainer *container)
{
    // remove container from the containers list
    for (int i = 0; i < m_UploadContainers.size(); i++) {
        if (m_UploadContainers.at(i) == container) {
            m_UploadContainers.removeAt(i);
            break;
        }
    }
    // destroy the container itself
    if (container != NULL) {
        qDebug() << "AxedaAgent: about to delete container->FileData.";
        // delete the array with file contents
        if (container->FileData != NULL) {
            delete container->FileData;
            container->FileData = NULL;
        }
        qDebug() << "AxedaAgent: about to delete container:" << container;
        // delete the container itself
        delete container;
        container = NULL;
        Q_UNUSED(container) // 4 Lint
    }
}

/****************************************************************************/
/*!
*  \brief   This callback is invoked when Agent Embedded receives the
*           Set Time SOAP Method: EEnterpriseProxy.St.
*
*  \param[in] iDeviceId = ID of the target device
*  \param[in] pTime = new time value to set on the device
*  \param[in] piTZOffset = new time zone offset
*  \param[in] pStatus = pointer to the structure for invocation results
*
*****************************************************************************/
void AxedaAgent::OnCommandSetTimeCallback(AeInt32 iDeviceId, AeTimeValue *pTime, AeInt32 *piTZOffset, \
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
*  \param[in] iDeviceId = ID of the target device
*  \param[in] bHard = flag indicating whether hard or soft restart is requested
*  \param[in] pStatus = pointer to the structure for invocation results
*
*****************************************************************************/
void AxedaAgent::OnCommandRestartCallback(AeInt32 iDeviceId, AeBool bHard, AeDRMSOAPCommandStatus *pStatus)
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
*  \param[in] iServerId = ID of the Enterprise server that sent the message
*  \param[in] pPingRate = the new ping rate
*
*****************************************************************************/
void AxedaAgent::OnPingRateUpdateCallback(AeInt32 iServerId, AeDRMPingRate *pPingRate)
{
    Q_UNUSED(pPingRate)
    qDebug() << "*** CB: OnPingRateUpdateCallback. ServerId = " << iServerId;
}


} // end namespace

