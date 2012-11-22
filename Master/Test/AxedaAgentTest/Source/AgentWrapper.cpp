#include "../Include/AgentWrapper.h"
#include <QTimer>
#include <QDebug>

/******************************************************************************
 * Synopsis (from Axeda demo):
 *
 *     AxedaAgent [-o owner] [-s] [-g] [{-ph|-ps} proxy-host [-pp proxy-port]
 *                [-pu proxy-user] [-pw proxy-password]] [-o owner] url
 *
 * url:   https://<server-name>:<port>/eMessage  (secure communications)
 *
 * Options:
 *
 *     -o owner
 *         Use specified owner (database name).
 *     -s
 *         Use secure communication (SSL).
 *     -g
 *         Enable debug messages.
 *     -ph proxy-host
 *         Use HTTP proxy at proxy-host. Default port is 80.
 *     -ps proxy-host
 *         Use SOCKS proxy at proxy-host. Default port is 1080.
 *     -pp proxy-port
 *         Override default port for proxy.
 *     -pu proxy-user
 *         Use proxy-user as user name for proxy authentication.
 *     -pw proxy-password
 *         Use proxy-password as password for proxy authentication.
 *
 ******************************************************************************/


typedef struct _AeDemoUpload AeDemoUpload;

struct _AeDemoUpload
{
    AeFileUploadSpec **ppUploads;
    AeInt32          iUploadIdx;
    AeFileStat       curFileStat;
    AeFileHandle     iCurFileHandle;
    AeChar           pBuffer[BUFSIZ];
};

AgentWrapper::AgentWrapper(QObject *pParent) :
        QObject(pParent),
        m_DeviceId(5),
        m_ServerId(5),
        m_Active(false)
{
}

AgentWrapper::~AgentWrapper()
{
}

bool AgentWrapper::Initialize(int argc, char *argv[])
{
    AeError rc;

    /* process options */
    if (!AeDemoProcessOptions(&m_config, &argc, argv, (int)1))
    {
        AeDemoUsage(argv[0], (AeChar*)"url");
        return false;
    }

    /* initialize Axeda Agent Embedded */
    if (AeInitialize() != AeEOK) {
        return false;
    }

    /* apply options */
    if (!AeDemoApplyConfig(&m_config)) {
        return false;
    }

    /* configure master device */
    rc = AeDRMAddDevice(AeDRMDeviceMaster, (AeChar*)MODEL_NUMBER, (AeChar*)SERIAL_NUMBER, &m_DeviceId);
    if (rc != AeEOK)
    {
        fprintf(stderr, "Failed to add device (%s)\n", AeGetErrorString(rc));
        return 1;
    }

    /* configure primary DRM server */
    m_pingRate.iSec = PING_RATE;
    m_pingRate.iMicroSec = 0;
    rc = AeDRMAddServer(AeDRMServerConfigPrimary, argv[argc], m_config.pOwner, &m_pingRate, &m_ServerId);
    if (rc != AeEOK)
    {
        fprintf(stderr, "Failed to add server (%s)\n", AeGetErrorString(rc));
        return 1;
    }

    /* set HTTP persistence */
    AeError err= AeWebSetPersistent(m_ServerId, true);
    if (err != AeEOK) {
        qDebug() << "ERROR: AeWebSetPersistent failed ! Error code: " << err;
    }

    /* Add remote session if configured */
    if (m_config.pRemoteSessionName != NULL && m_config.pRemoteSessionType != NULL &&
        m_config.pRemoteSessionHost != NULL && m_config.iRemoteSessionPort != 0)
    {
        rc = AeDRMAddRemoteSession(m_DeviceId, (AeChar*)m_config.pRemoteSessionName, (AeChar*)"",
            (AeChar*)m_config.pRemoteSessionType, (AeChar*)m_config.pRemoteSessionHost, m_config.iRemoteSessionPort);
        if (rc != AeEOK)
        {
            fprintf(stderr, "Failed to add Remote session (%s)\n", AeGetErrorString(rc));
            return 1;
        }
    }

    // set message queue size (in bytes)
    AeDRMSetQueueSize(TEST_QUEUE_SIZE_BYTES);

    //qDebug() << "AgentWrapper :   finished init.";

    return true;
}

void AgentWrapper::run()
{
    qDebug() << "\nAgentWrapper: started single shot timer... \n";
    m_Active = true;
    QTimer::singleShot((int)200, this, SLOT(work()));
}

void AgentWrapper::work()
{
    AeDRMDataItem dataItem;
    AeTimeValue   timeLimit;
    AeBool        connected;

    // set time limit for the DRM execution. this is also the data poll rate.
    timeLimit.iSec = DRMEXEC_TIME_LIMIT;
    timeLimit.iMicroSec = 0;

    RegisterAllCallbacks();

    qDebug() << "\nAgentWrapper: entered working function... \n";
    /* execute agent */
    do
    {
        // prepare and post string data item
        dataItem.pName = (AeChar*)"string1";
        AeGetCurrentTime(&dataItem.value.timeStamp);
        /* acquire tag value */
        AeDemoRamp(&dataItem.value);
        if (AeDRMPostDataItem(m_DeviceId, m_ServerId, AeDRMQueuePriorityNormal, &dataItem) != AeEOK) {
            qDebug() << "AgentWrapper ERROR :  could not post DataItem !";
        }

        // this is the execution cycle
        if (AeDRMExecute(&timeLimit) != AeEOK) {
            qDebug() << "AgentWrapper ERROR :  AeDRMExecute returned false !";
        }

        // check if agent is actually connected to the Server
        AeDRMGetServerStatus(m_ServerId, &connected);
        if (!connected) {
            qDebug() << "AgentWrapper WARNING :  Agent is not connected !";
        }

    } while (m_Active == true);

    /* shutdown Axeda Agent Embedded */
    qDebug() << "\nAgentWrapper: exiting working function... \n";
    AeDRMSetDeviceOnline(m_DeviceId, false); /// server does not realize the device is offline though
    AeShutdown();
    emit Finished();
}

void AgentWrapper::stop()
{
    m_Active = false;
    qDebug() << "\nAgentWrapper: m_Active is set to FALSE... \n";
}


/******************************************************************************
 * Slot to trigger Alarms
 ******************************************************************************/
void AgentWrapper::SetupAlarm()
{
    AeDRMDataItem dataItem;
    AeDRMAlarm alarmData;

    /* work the sine */
    dataItem.pName = (AeChar*)"sine1";
    AeGetCurrentTime(&dataItem.value.timeStamp);
    /* acquire tag value */
    AeDemoSine(&dataItem.value);
    /* post at least one alarm */
    AeDemoAlarm(&alarmData, &dataItem);
    alarmData.pCondition = (AeChar*)"Low";
    alarmData.pDescription = (AeChar*)"Demo Low alarm";
    if (AeDRMPostAlarm(m_DeviceId, m_ServerId, AeDRMQueuePriorityUrgent, &alarmData) != AeEOK) {
        qDebug() << "\nAgentWrapper: posting alarm with name :  " << alarmData.pName << " failed !\n";
    }
    else {
        qDebug() << "\nAgentWrapper: posted alarm with name :  " << alarmData.pName << "\n";
    }

    /* work the ramp */
    dataItem.pName = (AeChar*)"ramp1";
    AeGetCurrentTime(&dataItem.value.timeStamp);
    /* acquire tag value */
    AeDemoRamp(&dataItem.value);
    /* conditionally post a second alarm */
    if (AeDemoAlarm(&alarmData, &dataItem) == AeTrue) {
        alarmData.pCondition = (AeChar*)"High";
        alarmData.pDescription = (AeChar*)"Demo High alarm";
        if (AeDRMPostAlarm(m_DeviceId, m_ServerId, AeDRMQueuePriorityUrgent, &alarmData) != AeEOK) {
            qDebug() << "\nAgentWrapper: posting alarm with name :  " << alarmData.pName << " failed !\n";
        }
        else {
            qDebug() << "\nAgentWrapper: posted alarm with name :  " << alarmData.pName << "\n";
        }
    }
}

/******************************************************************************
 * Slot to trigger Events
 ******************************************************************************/
void AgentWrapper::SetupEvent()
{
    AeDRMEvent    drmEvent;

    // prepare and post event
    drmEvent.pName = (AeChar*)"event1";
    drmEvent.pMessage = (AeChar*)"Event with random severity";
    AeGetCurrentTime(&drmEvent.timeStamp);
    drmEvent.iSeverity = (AeInt)((((90 - 10) * qrand()) / RAND_MAX) + 10);
    if (AeDRMPostEvent(m_DeviceId, m_ServerId, AeDRMQueuePriorityNormal, &drmEvent) != AeEOK) {
        qDebug() << "AgentWrapper ERROR :  could not post Event with severity :  " << drmEvent.iSeverity << " !\n";
    }
    else {
        qDebug() << "\nAgentWrapper: posted event with severity :  " << drmEvent.iSeverity << "\n";
    }
}

/******************************************************************************
 * Function to request a file upload
 ******************************************************************************/
void AgentWrapper::SetupUpload()
{
    AeFileUploadSpec uploadSpec;
    AeFileUploadSpec *ppUploads[2];
    AeFileUploadRequestParam param;

    /* prepare upload specification */
    uploadSpec.pName = (AeChar*)UPLOAD_FILE_NAME;
    uploadSpec.bDelete = AeFalse;

    /* upload specification list must be NULL-terminated */
    ppUploads[0] = &uploadSpec;
    ppUploads[1] = NULL;

    /* no compression for the file */
    param.iMask = 0x0;//AE_FILE_UPLOAD_REQUEST_PARAM_COMPRESSION;
    param.iCompression = AeFileCompressionNone;

    /* submit upload request */
    AeDRMPostFileUploadRequestEx(m_DeviceId, m_ServerId, AeDRMQueuePriorityNormal, ppUploads, &param);
}

/******************************************************************************
 * Function to request a file upload
 ******************************************************************************/
void AgentWrapper::SetupAnotherUpload()
{
    AeFileUploadSpec uploadSpec;
    AeFileUploadSpec *ppUploads[2];
    AeFileUploadRequestParam param;

    /* prepare upload specification */
    uploadSpec.pName = (AeChar*)UPLOAD_FILE_NAME2;
    uploadSpec.bDelete = AeFalse;

    /* upload specification list must be NULL-terminated */
    ppUploads[0] = &uploadSpec;
    ppUploads[1] = NULL;

    /* no compression for the file */
    param.iMask = 0x0;//AE_FILE_UPLOAD_REQUEST_PARAM_COMPRESSION;
    param.iCompression = AeFileCompressionNone;

    /* submit upload request */
    AeDRMPostFileUploadRequestEx(m_DeviceId, m_ServerId, AeDRMQueuePriorityNormal, ppUploads, &param);
}


/******************************************************************************
 * Function to request a file upload
 ******************************************************************************/
void AgentWrapper::SetupTwoUploads()
{
    AeFileUploadSpec uploadSpec1;
    AeFileUploadSpec uploadSpec2;
    AeFileUploadSpec *ppUploads[3];
    AeFileUploadRequestParam param;

    /* prepare upload specification */
    uploadSpec1.pName = (AeChar*)UPLOAD_FILE_NAME;
    uploadSpec1.bDelete = AeFalse;

    uploadSpec2.pName = (AeChar*)UPLOAD_FILE_NAME2;
    uploadSpec2.bDelete = AeFalse;

    /* upload specification list must be NULL-terminated */
    ppUploads[0] = &uploadSpec1;
    ppUploads[1] = &uploadSpec2;
    ppUploads[2] = NULL;

    /* no compression for the file */
    param.iMask = 0x0;//AE_FILE_UPLOAD_REQUEST_PARAM_COMPRESSION;
    param.iCompression = AeFileCompressionNone;

    /* submit upload request */
    AeDRMPostFileUploadRequestEx(m_DeviceId, m_ServerId, AeDRMQueuePriorityNormal, ppUploads, &param);
}

/******************************************************************************
 * Function to assign a sine value to a tag.
 ******************************************************************************/
void AgentWrapper::AeDemoSine(AeDRMDataValue *pValue)
{
    pValue->iType = AeDRMDataAnalog;
    pValue->iQuality = AeDRMDataGood;
    pValue->data.dAnalog = sin(pValue->timeStamp.iSec * 0.1) * 100 / 2 + 50;
}

/******************************************************************************
 * Function to assign a ramp value to a tag.
 ******************************************************************************/
void AgentWrapper::AeDemoRamp(AeDRMDataValue *pValue)
{
    pValue->iType = AeDRMDataAnalog;
    pValue->iQuality = AeDRMDataGood;
    pValue->data.dAnalog = ((AeUInt32) pValue->timeStamp.iSec) % 100;
}

/******************************************************************************
 * Function to assign a string value to a tag.
 ******************************************************************************/
void AgentWrapper::AeDemoString(AeDRMDataValue *pValue)
{
    pValue->iType = AeDRMDataString;
    pValue->iQuality = AeDRMDataGood;
    pValue->data.pString = (AeChar*)(pValue->timeStamp.iSec % 20 < 10 ? "str1" : "STR2");
}

/******************************************************************************
 * Function to set an alarm based on a data item value
 ******************************************************************************/
AeBool AgentWrapper::AeDemoAlarm(AeDRMAlarm *pAlarm, AeDRMDataItem *pDataItem)
{
    if (pDataItem->value.iType != AeDRMDataAnalog)
        return AeFalse;

    pAlarm->pName = pDataItem->pName;
    pAlarm->timeStamp = pDataItem->value.timeStamp;
    pAlarm->bAck = AeFalse;
    pAlarm->bActive = AeTrue;
    pAlarm->pDataItem = pDataItem;

    if (((int) pDataItem->value.data.dAnalog >= 80) && ((int) pDataItem->value.data.dAnalog <= 90))
    {
        pAlarm->iSeverity = 30;
    }
    else if (((int) pDataItem->value.data.dAnalog >= 10) && ((int) pDataItem->value.data.dAnalog <= 20))
    {
        pAlarm->iSeverity = 80;
    }
    else
        return AeFalse;

    return AeTrue;
}

// === Axeda Agent Callbacks : ==============================================

void AgentWrapper::RegisterAllCallbacks()
{
    // status reports related callbacks:
    AeDRMSetOnWebError(&OnWebErrorCallback);
    AeDRMSetOnDeviceRegistered(&OnDeviceRegisteredCallback);
    AeDRMSetOnQueueStatus(&QueueStatusCallback);

    // Server commands related callbacks:
    AeDRMSetOnCommandSetTime(&OnCommandSetTimeCallback);
    AeDRMSetOnCommandRestart(&OnCommandRestartCallback);
    AeDRMSetOnPingRateUpdate(&OnPingRateUpdateCallback);
    AeDRMSetOnCommandSetTag(&OnCommandSetTagCallback);

    // remote session related callbacks:
    AeDRMSetOnRemoteSessionEnd(&RemoteSessionEndCallback);
    AeDRMSetOnRemoteSessionStart(&RemoteSessionStartCallback);

    // SOAP methods related callbacks:
    AeDRMSetOnSOAPMethod(&OnSOAPMethodCallback);
    AeDRMSetOnSOAPMethodEx(&OnSOAPMethodExCallback);

    // file transfer related callbacks:
    AeDRMSetOnFileDownloadBegin(&OnFileDownloadBeginCallback);
    AeDRMSetOnFileDownloadEnd(&OnFileDownloadEndCallback);
    AeDRMSetOnFileUploadEnd(&OnFileUploadEndCallback);
    AeDRMSetOnFileTransferEvent(&OnFileTransferEventCallback);

    // These functions give linker error (even though they are declared in the h-file):
    AeDRMSetOnFileDownloadData(&OnFileDownloadDataCallback);
    AeDRMSetOnFileUploadBegin(&OnFileUploadBeginCallback);
    AeDRMSetOnFileUploadBeginEx(&OnFileUploadBeginExCallback);
    AeDRMSetOnFileUploadData(&OnFileUploadDataCallback);
}

/*
 As soon as a remote session ends, this callback is invoked.
*/
void AgentWrapper::RemoteSessionEndCallback(AeRemoteInterface *pInterface)
{
    qDebug() << "*** CB: RemoteSessionEndCallback. Device = " << pInterface->iDeviceId << " Port =  " << pInterface->iPort \
             << " Server =  " << pInterface->pServer;
}

/*
 Whenever Agent Embedded is about to start a remote session, this callback is invoked.
*/
AeBool AgentWrapper::RemoteSessionStartCallback(AeRemoteInterface *pInterface)
{
    Q_UNUSED(pInterface)
    qDebug() << "*** CB: RemoteSessionStartCallback. Device = " << pInterface->iDeviceId << " Port =  " << pInterface->iPort \
    << " Server =  " << pInterface->pServer;
    return AeTrue;
}

/*
 When a communication error occurs while Agent Embedded is performing pending tasks in
 AeDRMExecute(), this callback is invoked.
*/
void AgentWrapper::OnWebErrorCallback(AeInt32 iServerId, AeError iError)
{
    qDebug() << "*** CB: OnWebErrorCallback. ServerId = " << iServerId << " Error = " << iError;
}

/*
 This is invoked when a device is successfully registered while in AeDRMExecute().
*/
void AgentWrapper::OnDeviceRegisteredCallback(AeInt32 iDeviceId)
{
    qDebug() << "*** CB: OnDeviceRegisteredCallback. DeviceId = " << iDeviceId;
}

/*
 Whenever the status of the internal data queue is changed, this callback is invoked.
*/
void AgentWrapper::QueueStatusCallback(AeDRMQueueStatus iNewStatus)
{
    qDebug() << "*** CB: QueueStatusCallback. Status = " << iNewStatus;
}

/*
 When a SOAP method is received from the Enterprise server and the method is not known (that
 is, not handled by any of the callback functions registered through
 AeDRMSetOnCommandXXX()), this callback is invoked.
*/
void AgentWrapper::OnSOAPMethodCallback(AeInt32 iDeviceId, AeHandle pMethod, AeDRMSOAPCommandStatus *pStatus)
{
    Q_UNUSED(iDeviceId)
    Q_UNUSED(pMethod)
    qDebug() << "*** CB: OnSOAPMethodCallback. StatusCode = " << pStatus->iStatusCode << " Description = " << pStatus->pStatusReason;
}

/*
 This callback is invoked on receiving a generic SOAP method.
 The application can process the received SOAP method using the AeDRMSOAPXXX() family
 of functions.
*/
void AgentWrapper::OnSOAPMethodExCallback(AeInt32 iDeviceId, AeInt32 iServerId, AeHandle pMethod, \
                            AeDRMSOAPCommandId *pSOAPId, AeDRMSOAPCommandStatus *pStatus)
{
    Q_UNUSED(iDeviceId)
    Q_UNUSED(iServerId)
    Q_UNUSED(pMethod)
    qDebug() << "*** CB: OnSOAPMethodExCallback. \nCommandId = " << pSOAPId->pCmdId << " UserId = " << pSOAPId->pUserId \
                                    << "\nStatusCode = " << pStatus->iStatusCode << " Description = " << pStatus->pStatusReason;
}

/*
 This callback is invoked on receiving the SOAP method: DynamicData.SetTag.
*/
void AgentWrapper::OnCommandSetTagCallback(AeInt32 iDeviceId, AeDRMDataItem *pDataItem, AeDRMSOAPCommandStatus *pStatus)
{
    //Q_UNUSED(pDataItem)
    //Q_UNUSED(pStatus)
    qDebug() << "*** CB: OnCommandSetTagCallback. DeiceId = " << iDeviceId << "/ DataItemName =  " << pDataItem->pName \
                                                              << "/ DataItemValueType =  " << pDataItem->value.iType;

    pStatus->iStatusCode = AE_DRM_SOAP_COMMAND_STATUS_SUCCESS;
}

/*
 This callback is invoked when the download for a series of one or
 more files begins. The callback must return AeTrue if the user application intends to perform custom
 processing on the downloaded files. In this case, Agent Embedded will pass the incoming file
 data to the application using the callback registered with AeDRMSetOnFileDownloadData().
 The callback must return AeFalse, if Agent Embedded should process downloaded files
 internally (that is, store them in the local file system).
*/
AeBool AgentWrapper::OnFileDownloadBeginCallback(AeInt32 iDeviceId, AePointer *ppUserData)
{
    Q_UNUSED(ppUserData)
    qDebug() << "*** CB: OnFileDownloadBeginCallback. DeviceId = " << iDeviceId;
    return AeFalse;
}

/*
 This callback is invoked when Agent Embedded receives a portion of a downloaded file.
*/
AeBool AgentWrapper::OnFileDownloadDataCallback(AeInt32 iDeviceId, AeFileStat *pFile, AeChar *pData, AeInt32 iSize, AePointer pUserData)
{
    Q_UNUSED(pFile)
    Q_UNUSED(pData)
    Q_UNUSED(pUserData)
    qDebug() << "*** CB: OnFileDownloadDataCallback. DeviceId =" << iDeviceId << " DataSize = " << iSize;
    return AeTrue;
}

/*
 This callback is invoked when Agent Embedded either encounters an error during the file download,
 or when all files in the series are received successfully. The callback indicates the file download
 status by returning AeTrue in the case of success or AeFalse otherwise.
*/
void AgentWrapper::OnFileDownloadEndCallback(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData)
{
    Q_UNUSED(pUserData)
    qDebug() << "*** CB: OnFileDownloadEndCallback. DeviceID = " << iDeviceId << " Status = " << bOK;
    //return true;
}

/*
 This callback is invoked on receiving the File Upload SOAP method. The
 callback must return AeTrue if the user application intends to provide data for the uploaded
 files. In this case, Agent Embedded will request content of the uploaded files from the
 application through a callback registered with AeDRMSetOnFileUploadData(). The callback
 must return AeFalse if Agent Embedded should process uploaded files internally (that is,
 retrieve them from the local file system).
*/
AeBool AgentWrapper::OnFileUploadBeginCallback(AeInt32 iDeviceId, AeFileUploadSpec **ppUploads, AePointer *ppUserData)
{
//    Q_UNUSED(ppUploads)
//    Q_UNUSED(ppUserData)
//    qDebug() << "*** CB: OnFileUploadBeginCallback. DeviceId = " << iDeviceId;
//
//    // -- DEMO start --
//    AeDemoUpload *pUpload;
//
//    /* allocate upload descriptor */
//    pUpload = (AeDemoUpload *) malloc(sizeof(AeDemoUpload));
//    if (!pUpload)
//        return AeFalse;
//
//    /* initialize */
//    memset(pUpload, 0, sizeof(AeDemoUpload));
//    pUpload->ppUploads = ppUploads;
//    pUpload->iUploadIdx = 0;
//    pUpload->iCurFileHandle = AeFileInvalidHandle;
//
//    *ppUserData = pUpload;

    return AeTrue;
    // -- DEMO end --

    //return AeFalse;
}

/*
 This callback is invoked when Agent Embedded receives the File Upload
 SOAP method, like AeDRMSetOnFileUploadBegin(). The difference from those functions lies in
 the callbacks, which accept additional upload parameters.
 If the callback for either of these functions returns AeTrue, the user application intends to
 provide data for uploaded files. If the callback returns AeFalse, Agent Embedded should
 process uploaded files internally.
*/
AeBool AgentWrapper::OnFileUploadBeginExCallback(AeInt32 iDeviceId, AeFileUploadSpec **ppUploads, \
                                   AeFileUploadExecuteParam *pParam, AePointer *ppUserData)
{
    //Q_UNUSED(ppUploads)
    //Q_UNUSED(pParam)
    Q_UNUSED(ppUserData)
    qDebug() << "*** CB: OnFileUploadBeginExCallback. DeviceID = " << iDeviceId << " FileName = " << (*ppUploads)->pName << \
                                             " pID = " << pParam->pId;

    // -- DEMO start --
    AeDemoUpload *pUpload;

    /* allocate upload descriptor */
    pUpload = (AeDemoUpload *) malloc(sizeof(AeDemoUpload));
    if (!pUpload)
        return AeFalse;

    /* initialize */
    memset(pUpload, 0, sizeof(AeDemoUpload));
    pUpload->ppUploads = ppUploads;
    pUpload->iUploadIdx = 0;
    pUpload->iCurFileHandle = AeFileInvalidHandle;

    *ppUserData = pUpload;

    return AeTrue;
    // -- DEMO end --


    //return AeFalse;
    //return AeTrue;
}

/*
 This callback is invoked when Agent Embedded requires more file data for
 the upload. If the callback function returns AeTrue, the next block of file data is available for
 upload (successful); if it returns AeFalse, an error occurred.
*/
AeBool AgentWrapper::OnFileUploadDataCallback(AeInt32 iDeviceId, AeFileStat **ppFile, AeChar **ppData, \
                                AeInt32 *piSize, AePointer pUserData)
{
    Q_UNUSED(ppFile)
    Q_UNUSED(ppData)
    Q_UNUSED(pUserData)
    qDebug() << "*** CB: OnFileUploadDataCallback. DeviceId = " << iDeviceId << " DataSize = " << *piSize;


    // -- DEMO start --
    AeDemoUpload *pUpload;

    *ppFile = NULL;
    *ppData = NULL;
    *piSize = 0;

    pUpload = (AeDemoUpload *) pUserData;
    if (!pUpload)
        return AeFalse;

    /* no more files to upload: indicate that */
    if (!pUpload->ppUploads[pUpload->iUploadIdx])
        return AeTrue;

    /* initialize next file */
    if (pUpload->iCurFileHandle == AeFileInvalidHandle)
    {
        /* open file */
        pUpload->iCurFileHandle = AeFileOpen(pUpload->ppUploads[pUpload->iUploadIdx]->pName,
                                             AE_OPEN_READ_ONLY);
        if (pUpload->iCurFileHandle == AeFileInvalidHandle)
            return AeFalse;

        pUpload->curFileStat.pName = pUpload->ppUploads[pUpload->iUploadIdx]->pName;
        pUpload->curFileStat.iType = AeFileTypeRegular;
        pUpload->curFileStat.iSize = AeFileGetSize(pUpload->ppUploads[pUpload->iUploadIdx]->pName);
        pUpload->curFileStat.iMTime = 0;
    }

    *ppFile = &pUpload->curFileStat;

    /* try to read another portion of the file */
    *piSize = AeFileRead(pUpload->iCurFileHandle, pUpload->pBuffer, sizeof(pUpload->pBuffer));
    if (*piSize < 0)
        return AeFalse;
    else if (*piSize == 0)
    {
        AeFileClose(pUpload->iCurFileHandle);
        pUpload->iCurFileHandle = AeFileInvalidHandle;

        if (pUpload->ppUploads[pUpload->iUploadIdx]->bDelete)
            AeFileDelete(pUpload->ppUploads[pUpload->iUploadIdx]->pName);

        pUpload->iUploadIdx += 1;
    }
    else if (*piSize > 0)
        *ppData = pUpload->pBuffer;

    return AeTrue;
    // -- DEMO  end --

    //return AeTrue;
    //return AeFalse;
}

/*
 This callback is invoked when Agent Embedded either encounters an error
 during the file upload, or when all files are uploaded successfully.
*/
void AgentWrapper::OnFileUploadEndCallback(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData)
{
    Q_UNUSED(pUserData)
    qDebug() << "*** CB: OnFileUploadEndCallback. DeviceId = " << iDeviceId << " Status = " << bOK;

    // -- DEMO start --
    AeDemoUpload *pUpload;

    pUpload = (AeDemoUpload *) pUserData;
    if (!pUpload)
        return;

    if (pUpload->iCurFileHandle != AeFileInvalidHandle)
        AeFileClose(pUpload->iCurFileHandle);

    /// ---- AeFree(pUpload);
    if (pUpload != NULL) delete pUpload;
    // -- DEMO end --
}

/*
 This callback is invoked when Agent Embedded encounters an event during the file transfer.
*/
void AgentWrapper::OnFileTransferEventCallback(AeInt32 iDeviceId, AeFileTransferEvent iEvent, AePointer pUserData)
{
    Q_UNUSED(pUserData)
    qDebug() << "*** CB: OnFileTransferEventCallback. DeviceId = " << iDeviceId << " Event = " << iEvent;
}

/*
 This callback is invoked when Agent Embedded receives the Set Time SOAP Method: EEnterpriseProxy.St.
*/
void AgentWrapper::OnCommandSetTimeCallback(AeInt32 iDeviceId, AeTimeValue *pTime, AeInt32 *piTZOffset, AeDRMSOAPCommandStatus *pStatus)
{
    Q_UNUSED(pTime)
    Q_UNUSED(piTZOffset)
    Q_UNUSED(pStatus)
    qDebug() << "*** CB: OnCommandSetTimeCallback. DeviceId = " << iDeviceId;
}

/*
 This callback is invoked when Agent Embedded receives the Restart SOAP method: EEnterpriseProxy.Rs.
*/
void AgentWrapper::OnCommandRestartCallback(AeInt32 iDeviceId, AeBool bHard, AeDRMSOAPCommandStatus *pStatus)
{
    Q_UNUSED(bHard)
    Q_UNUSED(pStatus)
    qDebug() << "*** CB: OnCommandRestartCallback. DeviceId = " << iDeviceId;
}

/*
 This callback is invoked on receiving Set Ping Rate SOAP method: EEnterpriseProxy.Pu.
 The callback is also invoked by Agent Embedded when the ping rate is restored to the original
 after the time period for a temporary ping rate elapses.
*/
void AgentWrapper::OnPingRateUpdateCallback(AeInt32 iServerId, AeDRMPingRate *pPingRate)
{
    Q_UNUSED(pPingRate)
    qDebug() << "*** CB: OnPingRateUpdateCallback. ServerId = " << iServerId;
}
