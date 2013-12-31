/* $Id: AeDRM.c,v 1.24.2.4 2010/08/13 16:01:38 dkhodos Exp $ */

#include "Ae.h"

const char *g_pAgentEmbeddedVersion = \
    "Axeda AgentEmbedded v" \
    AE_VERSION_STRING(AE_VERSION_MAJOR, AE_VERSION_MINOR, AE_VERSION_BUILD);

static AeTimeValue      g_cycleBeginTime;

AeWebUserAgent   *g_pWebUA = NULL;

static AeInt    AeDRMDeviceCompare(AePointer pData1, AePointer pData2);
static void     AeDRMDeviceRelease(AePointer pObject);
static AeInt    AeDRMServerConfigCompare(AePointer pData1, AePointer pData2);
static void     AeDRMServerConfigRelease(AePointer pObject);
static AeInt    AeDRMServerLocationCompare(AePointer pData1, AePointer pData2);
static void     AeDRMServerLocationRelease(AePointer pObject);
static void     AeDRMDeviceServerRelease2(AePointer pObject);
static void     AeDRMRequestOnError(AeWebRequest *pRequest, AeError iError);
static AeBool   AeDRMRequestOnEntity(AeWebRequest *pRequest, AeInt32 iDataOffset, AeChar *pData, AeInt32 iSize);
static void     AeDRMRequestOnCompleted(AeWebRequest *pRequest);
static void     AeDRMSOAPStatusRelease(AePointer pObject);
static AeBool   AeDRMIsNextCycle(AeTimeValue *pCurrentTime);
static AeBool   AeDRMMaySleep(void);
static AeBool   AeDRMGetWaitInterval(AeTimeValue *pWaitInterval, AeTimeValue *pCurrentTime, AeTimeValue *pBeginTime, AeTimeValue *pTimeLimit);
static void     AeDRMProcessControlMessages(void);
static void     AeDRMProcessPingRate(AeInt32 iConfigId);
static void     AeDRMProcessDeviceRegistration(AeInt32 iDeviceId, AeDRMDeviceServer *pServer);
static void     AeDRMProcessDevicePing(AeInt32 iDeviceId, AeDRMDeviceServer *pServer);
static void     AeDRMProcessDeviceOnline(AeInt32 iDeviceId, AeDRMDeviceServer *pServer);
static void     AeDRMProcessDeviceMaintenancePing(AeInt32 iDeviceId, AeDRMDeviceServer *pServer);
#ifdef __GATEWAY__
static void     AeDRMProcessManagedDevices(AeDRMServerConfig *pConfig);
#endif
static void     AeDRMProcessDataQueue(void);
static AeBool   AeDRMSubmitRequests(void);
static void     AeDRMProcessCommStatus(AeDRMServerLocation *pLocation, AeBool bSuccess);
static void     AeDRMProcessDeviceServerCommStatus(AeInt32 iDeviceId, AeDRMDeviceServer *pServer, AeDRMServerLocation *pLocation, AeBool bSuccess);
static void     AeDRMProcessResponse(AeDRMRequest *pRequest);
static void     AeDRMProcessSOAPMethod(AeDRMRequest *pRequest, AeHandle pMethod);
static void     AeDRMProcessSOAPResponse(AeDRMRequest *pRequest, AeHandle pMethod);
static AeBool   AeDRMProcessSOAPSetTag(AeHandle pMethod, AeDRMDataItem *pDataItem);
static AeBool   AeDRMProcessSOAPSetTime(AeHandle pMethod, AeTimeValue **ppTime, AeInt32 **ppiTZOffset);
static AeBool   AeDRMProcessSOAPRestart(AeHandle pMethod, AeBool *pbHard);
static AeBool   AeDRMProcessSOAPSetPingRate(AeInt32 iConfigId, AeHandle pMethod, AeDRMSOAPCommandStatus *pStatus);
static void     AeDRMPostPingRate(AeInt32 iConfigId, AeTimeValue *pPingRate);
static AeBool   AeDRMProcessSOAPResponseStatus(AeDRMRequest *pRequest);
static void     AeDRMProcessDeviceServerSOAPStatus(AeInt32 iDeviceId, AeDRMDeviceServer *pServer, AeDRMRequest *pRequest, AeDRMSOAPStatus *pStatus);

static AeDRMRequest		*AeDRMGetRequest(AeDRMEMessageType iType, AeDRMDeviceServer *pServer);
static AeBool       AeDRMMayUseDeviceServer(AeDRMDeviceServer *pServer);
static AeBool       AeDRMMayUseServerLocation(AeDRMServerLocation *pLocation);
static void         AeDRMCalcNextRetryTime(AeTimeValue *pNextRetryTime);
static AeBool       AeDRMIsDeviceReady(AeInt iDeviceId);
static AeDRMServerLocation *AeDRMGetNewLocation(AeDRMServerLocation *pOldLocation, AeChar *pURL, AeChar *pDefaultAbsPath);
static AeInt32				AeDRMGetDeviceId(AeChar *pModelNumber, AeChar *pSerialNumber);
static void         AeDRMGetCurrentLocations(AeList *pLocations);

/******************************************************************************/
AeError AeDRMInitialize(void)
{
    g_pDRMDevices = AePtrArrayNew((AeCompareFunc) AeDRMDeviceCompare,
        (AeReleaseFunc) AeDRMDeviceRelease);
    if (!g_pDRMDevices)
        return AeEMemory;

    g_pDRMServerConfigs = AePtrArrayNew((AeCompareFunc) AeDRMServerConfigCompare,
        (AeReleaseFunc) AeDRMServerConfigRelease);
    if (!g_pDRMServerConfigs)
        return AeEMemory;

    g_pDRMServerLocations = AeListNew((AeCompareFunc) AeDRMServerLocationCompare,
        (AeReleaseFunc) AeDRMServerLocationRelease);
    if (!g_pDRMServerLocations)
        return AeEMemory;

    g_pQueue = AeDRMQueueNew();
    if (!g_pQueue)
        return AeEMemory;
    
    g_pWebUA = AeWebUserAgentNew();
    if (!g_pWebUA)
        return AeEMemory;

#if defined(ENABLE_REMOTE_SESSION)
    AeRemoteInitialize();
#endif /* defined(ENABLE_REMOTE_SESSION) */

#if defined(ENABLE_FILE_TRANSFER)
	g_pFileTransferPackages  = AeListNew((AeCompareFunc) AeFileTransferPackageCompare,
                                         (AeReleaseFunc) AeFileTransferPackageDestroy);
    if (!g_pFileTransferPackages)
        return AeEMemory;
#endif

    g_drmSettings.bServerTimeStamp = AeFalse;
    g_drmSettings.iLogLevel = AeLogInfo;
    g_drmSettings.iQueueSize = AE_DRM_QUEUE_DEFAULT_SIZE;
    g_drmSettings.retryPeriod.iSec = AE_DRM_DEFAULT_RETRY_PERIOD;
    g_drmSettings.retryPeriod.iMicroSec = 0;
    g_drmSettings.bYieldOnIdle = AeFalse;

    memset(&g_drmCallbacks, 0, sizeof(AeDRMCallbacks));
    
    memset(&g_cycleBeginTime, 0, sizeof(AeTimeValue));

    AeSRand();

    return AeEOK;
}

/******************************************************************************/
void AeDRMDestroy(void)
{
    AeDRMServerLocation *pLocation;
    AeListItem *pListItem;

    pListItem = AeListFirst(g_pDRMServerLocations);
    while (pListItem)
    {
        pLocation = AeListData(pListItem, AeDRMServerLocation);
        pListItem = AeListNext(pListItem);

        if (pLocation->pPendingRequest)
            AeDRMRequestComplete(pLocation->pPendingRequest, AeTrue);
    }

    if (g_pDRMDevices)
        AePtrArrayDestroy(g_pDRMDevices);

    if (g_pDRMServerConfigs)
        AePtrArrayDestroy(g_pDRMServerConfigs);

    if (g_pDRMServerLocations)
        AeListDestroy(g_pDRMServerLocations);

    if (g_pQueue)
        AeDRMQueueDestroy(g_pQueue);

#if defined(ENABLE_REMOTE_SESSION)
    AeRemoteShutdown();
#endif

#if defined(ENABLE_FILE_TRANSFER)
	if (g_pFileTransferPackages)
        AeListDestroy(g_pFileTransferPackages);
#endif

    if (g_pWebUA)
        AeWebUserAgentDestroy(g_pWebUA);
}

/******************************************************************************/
AeDRMDevice *AeDRMDeviceNew(AeDRMDeviceType iType, AeChar *pModelNumber, AeChar *pSerialNumber)
{
    AeDRMDevice *pDevice;

    pDevice = (AeDRMDevice *) AeAlloc(sizeof(AeDRMDevice));
    if (!pDevice)
        return NULL;
    memset(pDevice, 0, sizeof(AeDRMDevice));

    pDevice->iType = iType;
    AeSetString(&pDevice->pModelNumber, pModelNumber, -1);
    AeSetString(&pDevice->pSerialNumber, pSerialNumber, -1);
    pDevice->bEnabled = AeTrue;
    pDevice->bOnline = AeTrue;

    return pDevice;
}

/******************************************************************************/
void AeDRMDeviceDestroy(AeDRMDevice *pDevice)
{
    if (pDevice->pModelNumber)
        AeFree(pDevice->pModelNumber);
    if (pDevice->pSerialNumber)
        AeFree(pDevice->pSerialNumber);

    AeFree(pDevice);
}

/******************************************************************************/
static AeInt AeDRMDeviceCompare(AePointer pData1, AePointer pData2)
{
    return !(!strcmp(((AeDRMDevice *) pData1)->pModelNumber,
        ((AeDRMDevice *) pData2)->pModelNumber) &&
        !strcmp(((AeDRMDevice *) pData1)->pSerialNumber,
        ((AeDRMDevice *) pData2)->pSerialNumber));
}

/******************************************************************************/
static void AeDRMDeviceRelease(AePointer pObject)
{
    AeDRMDeviceDestroy((AeDRMDevice *) pObject);
}

/******************************************************************************/
AeDRMServerConfig *AeDRMServerConfigNew(AeDRMServerConfigType iType, AeChar *pOwner, AeTimeValue *pPingRate)
{
    AeDRMServerConfig *pConfig;

    pConfig = (AeDRMServerConfig *) AeAlloc(sizeof(AeDRMServerConfig));
    if (!pConfig)
        return NULL;
    memset(pConfig, 0, sizeof(AeDRMServerConfig));

    pConfig->pDeviceServers = AePtrArrayNew(NULL,
        (AeReleaseFunc) AeDRMDeviceServerRelease2);
    if (!pConfig->pDeviceServers)
        return NULL;

    pConfig->iId = AE_DRM_PRIMARY_CONFIG_ID;
    pConfig->iType = iType;
    pConfig->pLocation = NULL;
    AeSetString(&pConfig->pOwner, pOwner, -1);
    pConfig->basePingRate = *pPingRate;
    pConfig->effectivePingRate = *pPingRate;
    pConfig->iWebVersion = AeWebHTTP11;
    pConfig->bWebPersistent = AeFalse;
    pConfig->webTimeOut.iSec = AE_HTTP_TIMEOUT;
    pConfig->webTimeOut.iMicroSec = 0;

    return pConfig;
}

/******************************************************************************/
void AeDRMServerConfigDestroy(AeDRMServerConfig *pConfig)
{
    if (pConfig->pDeviceServers)
        AePtrArrayDestroy(pConfig->pDeviceServers);

    if (pConfig->pOwner)
        AeFree(pConfig->pOwner);

    AeFree(pConfig);
}

/******************************************************************************/
AeError AeDRMServerConfigInitialize(AeDRMServerConfig *pConfig, AeChar *pURL)
{
    AeInt32 i;

    pConfig->pLocation = AeDRMServerLocationNew(pURL);
    if (!pConfig->pLocation)
        return AeEMemory;

    if (!pConfig->pLocation->pURL)
    {
        AeDRMServerLocationDestroy(pConfig->pLocation);
        return AeBadURL;
    }

    AeListAppend(g_pDRMServerLocations, pConfig->pLocation);

    for (i = 0; i < AePtrArrayCount(g_pDRMDevices); i++)
    {
        AeDRMDeviceServer *pServer;

        pServer = AeDRMDeviceServerNew(pConfig);
        if (!pServer)
            return AeEMemory;

        if (pConfig->iType != AeDRMServerConfigBackup)
            AeDRMDeviceServerAddRef(pServer);

        AePtrArraySet(pConfig->pDeviceServers, i, pServer);
    }

    return AeEOK;
}

/******************************************************************************/
static AeInt AeDRMServerConfigCompare(AePointer pData1, AePointer pData2)
{
    return !(((AeDRMServerConfig *) pData1)->iType ==
        ((AeDRMServerConfig *) pData2)->iType);
}

/******************************************************************************/
static void  AeDRMServerConfigRelease(AePointer pObject)
{
    AeDRMServerConfigDestroy((AeDRMServerConfig *) pObject);
}

/******************************************************************************/
AeDRMServerLocation *AeDRMServerLocationNew(AeChar *pURL)
{
    AeDRMServerLocation *pLocation;
    AeURL *pNormalURL;

    pLocation = (AeDRMServerLocation *) AeAlloc(sizeof(AeDRMServerLocation));
    if (!pLocation)
        return NULL;
    memset(pLocation, 0, sizeof(AeDRMServerLocation));

    /* normalize URL */
    pNormalURL = AeURLNew();
    if (!pNormalURL)
        return NULL;
    if (AeURLSet(pNormalURL, pURL))
        AeSetString(&pLocation->pURL, AeURLGet(pNormalURL), -1);
    AeURLDestroy(pNormalURL);

    pLocation->bOnline = AeFalse;
    pLocation->pPendingRequest = NULL;
    pLocation->iFailureCount = 0;
    pLocation->nextRetryTime.iSec = 0;
    pLocation->nextRetryTime.iMicroSec = 0;

    return pLocation;
}

/******************************************************************************/
void AeDRMServerLocationDestroy(AeDRMServerLocation *pLocation)
{
    if (pLocation->pURL)
        AeFree(pLocation->pURL);

    AeFree(pLocation);
}

/******************************************************************************/
static AeInt AeDRMServerLocationCompare(AePointer pData1, AePointer pData2)
{
    return strcmp(((AeDRMServerLocation *) pData1)->pURL,
        ((AeDRMServerLocation *) pData2)->pURL);
}

/******************************************************************************/
static void  AeDRMServerLocationRelease(AePointer pObject)
{
    AeDRMServerLocationDestroy((AeDRMServerLocation *) pObject);
}

/******************************************************************************/
AeDRMDeviceServer *AeDRMDeviceServerNew(AeDRMServerConfig *pConfig)
{
    AeDRMDeviceServer *pServer;
    AeTimeValue currentTime;

    pServer = (AeDRMDeviceServer *) AeAlloc(sizeof(AeDRMDeviceServer));
    if (!pServer)
        return NULL;
    memset(pServer, 0, sizeof(AeDRMDeviceServer));

    pServer->pParent = pConfig;
    pServer->pLocation = NULL;
    pServer->pOwner = NULL;
    pServer->bRegistered = AeFalse;
    pServer->bMaintenanceMode = AeFalse;

    AeGetElapsedTime(&currentTime);
    pServer->pingTime = currentTime;
    pServer->maintenancePingTime = currentTime;

    pServer->maintenancePingRate.iSec = AE_DRM_DEFAULT_MAINTENANCE_PING_RATE;
    pServer->iRefCount = 0;
    pServer->nextRetryTime.iSec = 0;
    pServer->nextRetryTime.iMicroSec = 0;

    pServer->bSendMDMessage = AeFalse;
    pServer->bSendDSMessage = AeFalse;

    return pServer;
}

/******************************************************************************/
void AeDRMDeviceServerDestroy(AeDRMDeviceServer *pServer)
{
    if (pServer->pOwner)
        AeFree(pServer->pOwner);

    AeFree(pServer);
}

/******************************************************************************/
static void AeDRMDeviceServerRelease2(AePointer pObject)
{
    AeDRMDeviceServerDestroy((AeDRMDeviceServer *) pObject);
}

/******************************************************************************/
AeDRMRequest *AeDRMRequestNew(AeDRMEMessageType iType, AeDRMDeviceServer *pServer, AeTimeValue *pTimeStamp)
{
    AeDRMRequest *pRequest;

    pRequest = (AeDRMRequest *) AeAlloc(sizeof(AeDRMRequest));
    if (!pRequest)
        return NULL;
    memset(pRequest, 0, sizeof(AeDRMRequest));

    pRequest->pWebRequest = AeWebRequestNew();
    if (!pRequest->pWebRequest)
        return NULL;

    if (pServer->pParent->iWebVersion == AeWebHTTP10)
        AeWebRequestSetVersion(pRequest->pWebRequest, HTTP_VERSION_10);
    else if (pServer->pParent->iWebVersion == AeWebHTTP11)
        AeWebRequestSetVersion(pRequest->pWebRequest, HTTP_VERSION_11);
    AeWebRequestSetMethod(pRequest->pWebRequest, "POST");
    AeWebRequestSetURL(pRequest->pWebRequest, AE_DRM_DEVICE_SERVER_LOCATION(pServer)->pURL);
    AeWebRequestSetPersistent(pRequest->pWebRequest, pServer->pParent->bWebPersistent);
    AeWebRequestSetTimeOut(pRequest->pWebRequest, &pServer->pParent->webTimeOut);
    AeWebRequestSetUserData(pRequest->pWebRequest, pRequest);
    AeWebRequestSetOnError(pRequest->pWebRequest, AeDRMRequestOnError);
    AeWebRequestSetOnEntity(pRequest->pWebRequest, AeDRMRequestOnEntity);
    AeWebRequestSetOnCompleted(pRequest->pWebRequest, AeDRMRequestOnCompleted);

    pRequest->pEMessage = AeDRMEMessageNew(iType, EMESSAGE_MAJOR_VERSION,
        EMESSAGE_MINOR_VERSION, AE_DRM_MAX_MESSAGE_SIZE, pTimeStamp);
    if (!pRequest->pEMessage)
        return NULL;
    
    pRequest->pRawMessage = AeArrayNew(1);
    if (!pRequest->pRawMessage)
        return NULL;

    pRequest->pRawResponse = AeArrayNew(1);
    if (!pRequest->pRawResponse)
        return NULL;

    pRequest->pSOAPStatuses = AeListNew(NULL, (AeReleaseFunc) AeDRMSOAPStatusRelease);
    if (!pRequest->pSOAPStatuses)
        return NULL;

    pRequest->iConfigId = pServer->pParent->iId;
    pRequest->pLocation = AE_DRM_DEVICE_SERVER_LOCATION(pServer);
    pRequest->bOpen = AeTrue;
    pRequest->bActive = AeFalse;

    return pRequest;
}

/******************************************************************************/
void AeDRMRequestDestroy(AeDRMRequest *pRequest)
{
    AeListItem *pListItem1, *pListItem2;
    AeDRMDeviceElement *pDeviceContent;
    AeDRMSOAPStatus *pStatus;

    /* update data queue items in regard of this request */
    pListItem1 = AeListFirst(pRequest->pEMessage->pDeviceElements);
    while (pListItem1)
    {
        pDeviceContent = AeListData(pListItem1, AeDRMDeviceElement);
        pListItem1 = AeListNext(pListItem1);

        if (pDeviceContent->iType != AeDRMData)
            continue;

        pListItem2 = AeListFirst(pRequest->pSOAPStatuses);
        while (pListItem2)
        {
            pStatus = AeListData(pListItem2, AeDRMSOAPStatus);
            pListItem2 = AeListNext(pListItem2);

            if (!pStatus->bGlobal && pStatus->iDeviceId != pDeviceContent->iDeviceId)
                continue;

            /* remove items from data queue in the case of success */
            if (pStatus->iStatus != AE_DRM_SOAP_STATUS_SUCCESS)
                continue;

            AeDRMQueueDeleteMessageItems(g_pQueue, pRequest->pEMessage->iId,
                pStatus->iDeviceId);

            /* if status is global (applies for all devices), do it once */
            if (pStatus->bGlobal)
            {
                pListItem1 = NULL;
                pListItem2 = NULL;
            }
        }
    }

    if (pRequest->pWebRequest)
        AeWebRequestDestroy(pRequest->pWebRequest);
    if (pRequest->pEMessage)
        AeDRMEMessageDestroy(pRequest->pEMessage);
    if (pRequest->pRawMessage)
        AeArrayDestroy(pRequest->pRawMessage);
    if (pRequest->pRawResponse)
        AeArrayDestroy(pRequest->pRawResponse);
    if (pRequest->pSOAPStatuses)
        AeListDestroy(pRequest->pSOAPStatuses);

    AeFree(pRequest);
}

/******************************************************************************/
void AeDRMRequestComplete(AeDRMRequest *pRequest, AeBool bSuccess)
{
    if (!bSuccess)
    {
        AeWebRequestClearStatus(pRequest->pWebRequest);
        AeArrayEmpty(pRequest->pRawResponse);
        AeListFree(pRequest->pSOAPStatuses);
        pRequest->bActive = AeFalse;
    }
    else
    {
        if (pRequest->pLocation->pPendingRequest == pRequest)
            pRequest->pLocation->pPendingRequest = NULL;
        AeDRMRequestDestroy(pRequest);
    }
}

/******************************************************************************/
AeDRMSOAPStatus *AeDRMSOAPStatusNew(AeInt32 iStatus, AeBool bGlobal, AeInt32 iDeviceId)
{
    AeDRMSOAPStatus *pSOAPStatus;

    pSOAPStatus = (AeDRMSOAPStatus *) AeAlloc(sizeof(AeDRMSOAPStatus));
    if (!pSOAPStatus)
        return NULL;
    memset(pSOAPStatus, 0, sizeof(AeDRMSOAPStatus));

    pSOAPStatus->iStatus = iStatus;
    pSOAPStatus->bGlobal = bGlobal;
    pSOAPStatus->iDeviceId = iDeviceId;
    pSOAPStatus->pOwner = NULL;
    pSOAPStatus->iInterval = AE_DRM_DEFAULT_MAINTENANCE_PING_RATE;
    pSOAPStatus->pURL = NULL;

    return pSOAPStatus;
}

/******************************************************************************/
void AeDRMSOAPStatusDestroy(AeDRMSOAPStatus *pStatus)
{
    if (pStatus->pOwner)
        AeFree(pStatus->pOwner);
    if (pStatus->pURL)
        AeFree(pStatus->pURL);

    AeFree(pStatus);
}

/******************************************************************************/
static void AeDRMRequestOnError(AeWebRequest *pRequest, AeError iError)
{
    if (g_drmCallbacks.pOnWebError)
        (*g_drmCallbacks.pOnWebError)(AE_DRM_REQUEST(pRequest)->iConfigId, iError);

    if (g_drmSettings.iLogLevel >= AeLogError)
        AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_NETWORK,
            "HTTP error: %s", AE_ERROR_STRING(iError));

    /* process communication failure */
    AeDRMProcessCommStatus(AE_DRM_REQUEST(pRequest)->pLocation, AeFalse);

    /* self-destruct */
    AeDRMRequestComplete(AE_DRM_REQUEST(pRequest), AeFalse);
}

/******************************************************************************/
static AeBool AeDRMRequestOnEntity(AeWebRequest *pRequest, AeInt32 iDataOffset, AeChar *pData, AeInt32 iSize)
{
    /* copy received chunk */
    AeArrayAppend(AE_DRM_REQUEST(pRequest)->pRawResponse, pData, iSize);

    return AeTrue;
}

/******************************************************************************/
static void AeDRMRequestOnCompleted(AeWebRequest *pRequest)
{
    /* check HTTP response status */
    if (pRequest->iResponseStatus != HTTP_STATUS_OK)
    {
        if (g_drmSettings.iLogLevel >= AeLogError)
            AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_NETWORK,
                "Unexpected HTTP status received while posting to %s: %d",
                AE_DRM_REQUEST(pRequest)->pLocation->pURL, pRequest->iResponseStatus);

        /* process communication failure */
        AeDRMProcessCommStatus(AE_DRM_REQUEST(pRequest)->pLocation, AeFalse);

        /* self-destruct */
        AeDRMRequestComplete(AE_DRM_REQUEST(pRequest), AeFalse);
        return;
    }

    /* print debug message */
    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_NETWORK,
            "eMessage delivered to %s: id=%d",
            AE_DRM_REQUEST(pRequest)->pLocation->pURL,
            AE_DRM_REQUEST(pRequest)->pEMessage->iId);


    AeDRMProcessResponse(AE_DRM_REQUEST(pRequest));
}

/******************************************************************************/
static void AeDRMSOAPStatusRelease(AePointer pObject)
{
    AeDRMSOAPStatusDestroy((AeDRMSOAPStatus *) pObject);
}

/******************************************************************************/
AeError AeDRMExecute(AeTimeValue *pTimeLimit)
{
    AeTimeValue beginTime, currentTime, waitInterval;
    AeError rc;

    /* master device is required */
    if (!AePtrArrayGet(g_pDRMDevices, AE_DRM_MASTER_DEVICE_ID))
        return AeEInvalidArg;

    /* remember the time of beginning of processing */
    AeGetElapsedTime(&beginTime);

    while (1)
    {
        /* note the time */
        AeGetElapsedTime(&currentTime);

        if (AeDRMIsNextCycle(&currentTime))
        {
            /* process data queue */
            AeDRMProcessDataQueue();

            /* gather control messages (Registration, Ping, Maintenance Ping) */
            AeDRMProcessControlMessages();

            /* generate and sumbit HTTP requests resulting from the previous actions */
            AeDRMSubmitRequests();
        }

#if defined(ENABLE_REMOTE_SESSION)
        /* process remote sessions */
	    AeRemoteProcess(&currentTime);
#endif

#if defined(ENABLE_FILE_TRANSFER)
	    /* process file transfer */
	    AeFileTransferProcess();
#endif

        /* calculate wait interval */
        if (pTimeLimit)
        {
            if (!AeDRMGetWaitInterval(&waitInterval, &currentTime, &beginTime, pTimeLimit))
                break;
        }

        /* perform simultaneous HTTP requests */
        if (AeWebUserAgentEventsPending(g_pWebUA))
        {
            rc = AeWebUserAgentDoEvents(g_pWebUA, pTimeLimit ? &waitInterval : NULL);
            if (rc != AeEOK)
                return rc;
        }
        else if (pTimeLimit && !g_drmSettings.bYieldOnIdle)
        {
            if (AeDRMMaySleep())
                AeSleep(&waitInterval);
        }
        else
            break;
    }

    return AeEOK;
}

/******************************************************************************/
AeArray *AeDRMGetFormattedContent(AeXMLContent *pContent)
{
    AeArray *pFormatted;
    
    pFormatted = AeArrayNew(1);
    if (!pFormatted)
        return NULL;

    AeXMLContentWrite(pContent, pFormatted);

    return pFormatted;
}

/******************************************************************************/
AeError _AeDRMPostSOAPCommandStatus(AeInt32 iDeviceId, AeInt32 iServerId, AeDRMQueuePriority iPriority, AeDRMSOAPCommandId *pSOAPId, AeDRMSOAPCommandStatus *pStatus)
{
    AeXMLElement *pElement;
    AeError rc;

    /* format SOAP command status */
    pElement = AeDRMEMessageCreateSOAPCommandStatus(pSOAPId->pCmdId, pSOAPId->pUserId, pStatus);
    if (!pElement)
        return AeEMemory;

    /* put SOAP command status to the queue */
    rc = AeDRMQueueAddItem(g_pQueue, AeDRMQueueItemSOAPStatus, iDeviceId, iServerId,
        AeDRMGetFormattedContent((AeXMLContent *) pElement), iPriority);

    AeXMLContentDestroy((AeXMLContent *) pElement);

    return rc;
}

/******************************************************************************/
AeError _AeDRMPostPingRate(AeInt32 iDeviceId, AeInt32 iServerId, AeDRMQueuePriority iPriority, AeTimeValue *pPingRate)
{
    AeXMLElement *pElement;
    AeError rc;

    /* create ping rate element */
    pElement = AeDRMEMessageCreatePingRate(pPingRate);
    if (!pElement)
        return AeEMemory;

    /* put ping rate to the queue */
    rc = AeDRMQueueAddItem(g_pQueue, AeDRMQueueItemMisc, iDeviceId, iServerId,
        AeDRMGetFormattedContent((AeXMLContent *) pElement), iPriority);

    AeXMLContentDestroy((AeXMLContent *) pElement);

    return rc;
}

/******************************************************************************/
static AeBool AeDRMIsNextCycle(AeTimeValue *pCurrentTime)
{
    AeTimeValue sinceCycleBegin, cycleLength = AE_DRM_INTERNAL_CYCLE_LENGTH;

    AE_TIME_VALUE_SUBTRACT(sinceCycleBegin, *pCurrentTime, g_cycleBeginTime);
    if (!AE_TIME_VALUE_GREATER_THAN(cycleLength, sinceCycleBegin))
    {
        g_cycleBeginTime = *pCurrentTime;
        return AeTrue;
    }

    return AeFalse;
}

/******************************************************************************/
static AeBool AeDRMMaySleep(void)
{
#if defined(ENABLE_FILE_TRANSFER)
    if (!AeFileTransferIsIdle())
        return AeFalse;
#endif

    return AeTrue;
}

/******************************************************************************/
static AeBool AeDRMGetWaitInterval(AeTimeValue *pWaitInterval, AeTimeValue *pCurrentTime, AeTimeValue *pBeginTime, AeTimeValue *pTimeLimit)
{
    AeTimeValue nextCycleBeginTime, endTime, cycleLength = AE_DRM_INTERNAL_CYCLE_LENGTH;

    /* calculate next cycle begin time */
    if (AE_TIME_VALUE_IS_NULL(g_cycleBeginTime))
    {
        AE_TIME_VALUE_ADD(nextCycleBeginTime, *pCurrentTime, cycleLength);
    }
    else
    {
        AE_TIME_VALUE_ADD(nextCycleBeginTime, g_cycleBeginTime, cycleLength);
    }

    /* calculate the time when we need to exit */
    AE_TIME_VALUE_ADD(endTime, *pBeginTime, *pTimeLimit);

    /* calculate wait interval (based either on the next cycle time or the end time) */
    if (!AE_TIME_VALUE_GREATER_THAN(endTime, nextCycleBeginTime))
    {
        AE_TIME_VALUE_SUBTRACT(*pWaitInterval, endTime, *pCurrentTime);
        if (AE_TIME_VALUE_IS_NULL(*pWaitInterval))
            return AeFalse;
    }
    else
    {
        AE_TIME_VALUE_SUBTRACT(*pWaitInterval, nextCycleBeginTime, *pCurrentTime);
    }

    return AeTrue;
}

/******************************************************************************/
static void AeDRMProcessControlMessages(void)
{
    AeInt32 i, j;

    for (i = 0; i < AePtrArrayCount(g_pDRMServerConfigs); i++)
    {
        AeDRMServerConfig *pConfig;

        pConfig = AePtrArrayGet(g_pDRMServerConfigs, i);
        if (pConfig)
        {
            AeDRMProcessPingRate(i);

            for (j = 0; j < AePtrArrayCount(pConfig->pDeviceServers); j++)
            {
                AeDRMDeviceServer *pServer;

                pServer = AePtrArrayGet(pConfig->pDeviceServers, j);
                if (!pServer)
                    continue;

                AeDRMProcessDeviceRegistration(j, pServer);
                AeDRMProcessDevicePing(j, pServer);
                AeDRMProcessDeviceOnline(j, pServer);
                AeDRMProcessDeviceMaintenancePing(j, pServer);
            }

#ifdef __GATEWAY__
			AeDRMProcessManagedDevices(pConfig);
#endif
        }
    }
}

/******************************************************************************/
static void AeDRMProcessPingRate(AeInt32 iConfigId)
{
    AeDRMServerConfig *pConfig;
    AeDRMPingRate pingRate;

    pConfig = AE_DRM_CONFIG(iConfigId);

    if (AE_TIME_VALUE_IS_NULL(pConfig->pingRateReversionTime))
        return;

    if (AE_TIME_VALUE_LESS_THAN(g_cycleBeginTime, pConfig->pingRateReversionTime))
        return;

    /* revert the ping rate to the base value */
    pConfig->effectivePingRate = pConfig->basePingRate;
    memset(&pConfig->pingRateReversionTime, 0, sizeof(AeTimeValue));

    pingRate.rate = pConfig->effectivePingRate;
    pingRate.pDuration = NULL;

    /* optionally notify the application */
    if (g_drmCallbacks.pOnPingRateUpdate)
        (*g_drmCallbacks.pOnPingRateUpdate)(iConfigId, &pingRate);

    /* post updated ping rate back to the server */
    AeDRMPostPingRate(iConfigId, &pingRate.rate);
}

/******************************************************************************/
static void AeDRMProcessDeviceRegistration(AeInt32 iDeviceId, AeDRMDeviceServer *pServer)
{
    AeDRMRequest *pRequest;

    if (!AeDRMMayUseDeviceServer(pServer))
        return;

    if (!AeDRMIsDeviceReady(iDeviceId))
        return;

    /* skip if already registered or in maintenance mode */
    if (pServer->bRegistered || pServer->bMaintenanceMode)
        return;

    /* fetch appropriate registration request */
    pRequest = AeDRMGetRequest(EMessageStandard, pServer);
    if (!pRequest)
        return;

    /* add registration element for device */
    AeDRMEMessageAddRegistration(pRequest->pEMessage, iDeviceId,
        AE_DRM_DEVICE_SERVER_OWNER(pServer),
        AE_DRM_DEVICE_SERVER_OWNER(AE_DRM_DEVICE_SERVER(pServer->pParent, AE_DRM_MASTER_DEVICE_ID)),
        &pServer->pParent->basePingRate);

    /* remember last activity time */
    pServer->pingTime = g_cycleBeginTime;
}

/******************************************************************************/
static void AeDRMProcessDevicePing(AeInt32 iDeviceId, AeDRMDeviceServer *pServer)
{
    AeDRMRequest *pRequest;
    AeTimeValue deltaTime;

    if (!AeDRMMayUseDeviceServer(pServer))
        return;

    if (!AeDRMIsDeviceReady(iDeviceId))
        return;

    /* skip if not registered or in maintenance mode, off-line */
    if (!pServer->bRegistered || pServer->bMaintenanceMode ||
        !AE_DRM_DEVICE_SERVER_LOCATION(pServer)->bOnline)
        return;

    /* check if it is time to send ping */
    AE_TIME_VALUE_SUBTRACT(deltaTime, g_cycleBeginTime, pServer->pingTime);
    if (AE_TIME_VALUE_LESS_THAN(deltaTime, pServer->pParent->effectivePingRate))
        return;

    /* fetch appropriate ping request */
    pRequest = AeDRMGetRequest(EMessageStandard, pServer);
    if (!pRequest)
        return;

    /* add ping element for device */
    AeDRMEMessageAddPing(pRequest->pEMessage, iDeviceId,
        AE_DRM_DEVICE_SERVER_OWNER(pServer));

    /* remember last activity time */
    pServer->pingTime = g_cycleBeginTime;
}

/******************************************************************************/
static void AeDRMProcessDeviceOnline(AeInt32 iDeviceId, AeDRMDeviceServer *pServer)
{
    AeDRMRequest *pRequest;

    if (!AeDRMMayUseDeviceServer(pServer))
        return;

    if (!AeDRMIsDeviceReady(iDeviceId))
        return;

    /* skip if not registered or in maintenance mode, off-line */
    if (!pServer->bRegistered || pServer->bMaintenanceMode ||
        !AE_DRM_DEVICE_SERVER_LOCATION(pServer)->bOnline)
        return;

    /* check if device message (online/offline ) is needed */
    if (pServer->bSendDSMessage != AeTrue)
        return;

    /* fetch appropriate request */
    pRequest = AeDRMGetRequest(EMessageStandard, pServer);
    if (!pRequest)
        return;

    /* add on/off line element for device */
    AeDRMEMessageAddOnline(pRequest->pEMessage, iDeviceId,
        AE_DRM_DEVICE_SERVER_OWNER(pServer));

    pServer->bSendDSMessage = AeFalse;
}

/******************************************************************************/
#ifdef __GATEWAY__
static void  AeDRMProcessManagedDevices(AeDRMServerConfig *pConfig)
{
    AeDRMDeviceServer *pMasterDeviceServer;
    AeDRMRequest *pRequest;
    AeList *pDeviceElements;
    AeInt32 i;

    pMasterDeviceServer = AE_DRM_DEVICE_SERVER(pConfig, AE_DRM_MASTER_DEVICE_ID);
	if (pMasterDeviceServer->bSendMDMessage != AeTrue)
        return;

	/* fetch appropriate managed devices request */
	pRequest = AeDRMGetRequest(EMessageStandard, pMasterDeviceServer);
	if (!pRequest)
		return;

    pDeviceElements = AeListNew(NULL, NULL);
    if (!pDeviceElements)
        return;

    /* create list of registered devices */
    for (i = 0; i < AePtrArrayCount(pConfig->pDeviceServers); i++)
    {
        AeDRMDeviceServer *pServer;
        AeXMLElement *pDeviceElement;

        if (i == AE_DRM_MASTER_DEVICE_ID)
            continue;

        /* skip not ready devices */
        if (!AeDRMIsDeviceReady(i))
            continue;

        pServer = AePtrArrayGet(pConfig->pDeviceServers, i);
        if (!pServer)
            continue;

        if (!pServer->bRegistered)
            continue;
    
        pDeviceElement = AeDRMEMessageCreateManagedDeviceDescriptor(i, AE_DRM_DEVICE_SERVER_OWNER(pServer));
        
        AeListAppend(pDeviceElements, pDeviceElement);
    }

	/* add gateway and managed devices devices to the message */
    if (AeListCount(pDeviceElements) > 0)
	    AeDRMEMessageAddManagedDevices(pRequest->pEMessage, AE_DRM_DEVICE_SERVER_OWNER(pMasterDeviceServer),
                                       pDeviceElements);

    AeListDestroy(pDeviceElements);

    pMasterDeviceServer->bSendMDMessage = AeFalse;
}
#endif

/******************************************************************************/
static void AeDRMProcessDeviceMaintenancePing(AeInt32 iDeviceId, AeDRMDeviceServer *pServer)
{
    AeDRMRequest *pRequest;
    AeTimeValue deltaTime;

    if (!AeDRMMayUseDeviceServer(pServer))
        return;

    if (!AeDRMIsDeviceReady(iDeviceId))
        return;

    /* skip if not in maintenance mode */
    if (!pServer->bMaintenanceMode)
        return;

    /* check if it is time to send maintenance ping */
    AE_TIME_VALUE_SUBTRACT(deltaTime, g_cycleBeginTime, pServer->maintenancePingTime);
    if (AE_TIME_VALUE_LESS_THAN(deltaTime, pServer->maintenancePingRate))
        return;

    /* fetch appropriate maintenance ping request */
    pRequest = AeDRMGetRequest(EMessageMaintenancePing, pServer);
    if (!pRequest)
        return;

    /* add maintenance ping element for device */
    AeDRMEMessageAddMaintenancePing(pRequest->pEMessage, iDeviceId,
        AE_DRM_DEVICE_SERVER_OWNER(pServer));

    /* remember last maintenance ping timestamp */
    pServer->maintenancePingTime = g_cycleBeginTime;
}

/******************************************************************************/
static void AeDRMProcessDataQueue(void)
{
    AeList *pValidLocations;
    AeDRMQueueItem *pItem;
    AeDRMServerConfig *pConfig;
    AeDRMDeviceServer *pServer;
    AeDRMRequest *pRequest;
    AeListItem *pListItem;
    AeBool bOK;

    pValidLocations = AeListNew(NULL, NULL);
    AeDRMGetCurrentLocations(pValidLocations);

    pListItem = AeDRMQueueOpen(g_pQueue);
    while (pListItem && AeListCount(pValidLocations) != 0)
    {
        /* fetch next item from queue */
        pItem = AeDRMQueueFetchNext(g_pQueue, &pListItem);
        if (!pItem)
            break;

        /* check device readiness */
        if (!AeDRMIsDeviceReady(pItem->iDeviceId))
            continue;

        /* get appropriate server configuration */
        pConfig = AE_DRM_CONFIG(pItem->iConfigId);

        /* if configuration name is not found use primary configuration */
        if (!pConfig)
            pConfig = AE_DRM_PRIMARY_CONFIG;

        /* get device server for configuration */
        pServer = AE_DRM_DEVICE_SERVER(pConfig, pItem->iDeviceId);
        if (!pServer)
            continue;

        /* handle situation when primary/additional server is unavailable */
        if (!AE_DRM_DEVICE_SERVER_LOCATION(pServer)->bOnline)
        {
            /* try to use backup configuration */
            if (AE_DRM_BACKUP_CONFIG)
            {
                /* get device server for backup configuration */
                pServer = AE_DRM_DEVICE_SERVER(AE_DRM_BACKUP_CONFIG, pItem->iDeviceId);
                if (!pServer)
                    continue;
            }
        }

        /* check if request is allowed for the server */
        if (!AeDRMMayUseDeviceServer(pServer))
            continue;

        /* skip if not registered or in maintenance mode */
        if (!pServer->bRegistered || pServer->bMaintenanceMode)
            continue;

        /* skip off-line locations */
        if (!AE_DRM_DEVICE_SERVER_LOCATION(pServer)->bOnline)
        {
            AeListRemove(pValidLocations, AE_DRM_DEVICE_SERVER_LOCATION(pServer));
            continue;
        }

        /* fetch appropriate data request */
        pRequest = AeDRMGetRequest(EMessageStandard, pServer);
        if (!pRequest)
        {
            AeListRemove(pValidLocations, AE_DRM_DEVICE_SERVER_LOCATION(pServer));
            continue;
        }

        /* add data to message */
        bOK = AeDRMEMessageAddData(pRequest->pEMessage, pItem->iDeviceId,
            AE_DRM_DEVICE_SERVER_OWNER(pServer),
            (AeChar *) AeArrayGet(pItem->pContent, 0), AeArrayCount(pItem->pContent));
        if (!bOK)
        {
            AeListRemove(pValidLocations, AE_DRM_DEVICE_SERVER_LOCATION(pServer));
            continue;
        }

        /* remember message id */
        pItem->iMessageId = pRequest->pEMessage->iId;

        /* remember last activity time */
        pServer->pingTime = g_cycleBeginTime;
    }

    AeDRMQueueClose(g_pQueue);

    AeListDestroy(pValidLocations);
}

/******************************************************************************/
static AeBool AeDRMSubmitRequests(void)
{
    AeWebRequest **ppRequests;
    AeDRMServerLocation *pLocation;
    AeDRMRequest *pRequest;
    AeListItem *pListItem;
    AeInt iRequestCount;

    /* web client accepts requests in the form of array of pointers */
    ppRequests = (AeWebRequest **)
        AeAlloc(sizeof(AeWebRequest *) * AeListCount(g_pDRMServerLocations));
    if (!ppRequests)
        return AeFalse;

    /* go through all server locations */
    pListItem = AeListFirst(g_pDRMServerLocations);
    iRequestCount = 0;
    while (pListItem)
    {
        pLocation = AeListData(pListItem, AeDRMServerLocation);
        pRequest = pLocation->pPendingRequest;
        pListItem = AeListNext(pListItem);

        /* skip locations without requests and with active requests */
        if (!pRequest || pRequest->bActive)
            continue;

        /* check if the location may be used */
        if (!AeDRMMayUseServerLocation(pLocation))
            continue;

        /* prepare new request */
        if (pRequest->bOpen)
        {
            pRequest->bOpen = AeFalse;

            /* perform message formatting */
            AeXMLDocumentWrite(pRequest->pEMessage->pDocument, pRequest->pRawMessage);

            /* print debug message */
            if (g_drmSettings.iLogLevel >= AeLogDebug)
                AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_NETWORK,
                    "Posting message (%d bytes) to %s:\n%s",
                    AeArrayCount(pRequest->pRawMessage),
                    pRequest->pLocation->pURL,
                    (AeChar *) AeArrayGet(pRequest->pRawMessage, 0));

            /* setup request entity */
            AeWebRequestSetEntityData(pRequest->pWebRequest,
                (AeChar *) AeArrayGet(pRequest->pRawMessage, 0));
            AeWebRequestSetEntitySize(pRequest->pWebRequest,
                AeArrayCount(pRequest->pRawMessage));
        }

        /* mark request as active */
        pRequest->bActive = AeTrue;

        /* convert request list to array of pointers to request */
        ppRequests[iRequestCount] = pRequest->pWebRequest;
        iRequestCount += 1;
    }

    /* submit requests to the web client */
    if (iRequestCount > 0)
        AeWebUserAgentAsyncExecute(g_pWebUA, ppRequests, iRequestCount);

    if (ppRequests)
        AeFree(ppRequests);

    return AeTrue;
}

/******************************************************************************/
static void AeDRMProcessCommStatus(AeDRMServerLocation *pLocation, AeBool bSuccess)
{
    AeBool bOnlineBefore, bWentOffline;
    AeInt32 i, j;

    /* remember previous on/off-line status */
    bOnlineBefore = pLocation->bOnline;
    bWentOffline = AeFalse;
    
    if (bSuccess)
    {
        /* clear failure-related properties */
        pLocation->iFailureCount = 0;
        memset(&pLocation->nextRetryTime, 0, sizeof(AeTimeValue));

        /* mark server location on-line */
        pLocation->bOnline = AeTrue;

        if (!bOnlineBefore)
        {
            if (g_drmSettings.iLogLevel >= AeLogInfo)
                AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_SERVER_STATUS,
                    "Server is available: %s", pLocation->pURL);
        }
    }
    else
    {
        /* calculate and set (randomized) next retry time */
        AeDRMCalcNextRetryTime(&pLocation->nextRetryTime);

        /* increment failure count */
        pLocation->iFailureCount += 1;

        /* failure needs to be consistent to mark server location off-line */
        if (pLocation->iFailureCount == AE_DRM_DEFAULT_OFFLINE_FAILURE_COUNT)
        {
            /* mark server location off-line */
            pLocation->bOnline = AeFalse;
            bWentOffline = AeTrue;

            if (g_drmSettings.iLogLevel >= AeLogWarning)
                AE_LOG_EX(AeLogWarning, AE_LOG_CATEGORY_SERVER_STATUS,
                    "Server is unavailable: %s", pLocation->pURL);
        }
    }

    /* process changes only */
    if (pLocation->bOnline == bOnlineBefore && !bWentOffline)
        return;

    for (i = 0; i < AePtrArrayCount(g_pDRMServerConfigs); i++)
    {
        AeDRMServerConfig *pConfig;

        pConfig = AePtrArrayGet(g_pDRMServerConfigs, i);
        if (pConfig)
        {
            for (j = 0; j < AePtrArrayCount(pConfig->pDeviceServers); j++)
            {
                AeDRMDeviceServer *pServer;

                pServer = AePtrArrayGet(pConfig->pDeviceServers, j);
                if (pServer)
                    AeDRMProcessDeviceServerCommStatus(j, pServer, pLocation, bSuccess);
            }
        }
    }
}

/******************************************************************************/
static void AeDRMProcessDeviceServerCommStatus(AeInt32 iDeviceId, AeDRMDeviceServer *pServer, AeDRMServerLocation *pLocation, AeBool bSuccess)
{
    /* update device server descriptor only if its location matches
       the one used in request */
    if (AE_DRM_DEVICE_SERVER_LOCATION(pServer) != pLocation)
        return;

    /* do not switch to backup server if in maintenance mode */
    if (pServer->bMaintenanceMode)
        return;

    /* do not perform further processing for backup server */
    if (pServer->pParent->iType == AeDRMServerConfigBackup)
        return;

    /* control usage of backup device server */
    if (AE_DRM_BACKUP_CONFIG)
    {
        AeDRMDeviceServer *pBackupServer;

        pBackupServer = AE_DRM_DEVICE_SERVER(AE_DRM_BACKUP_CONFIG, iDeviceId);

        /* grab or release backup server */
        if (bSuccess)
        {
            AeDRMDeviceServerRelease(pBackupServer);

            if (g_drmSettings.iLogLevel >= AeLogInfo)
                AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_SERVER_STATUS,
                    "Switching to main server (%s) for device: model: %s, serial number: %s",
                    AE_DRM_DEVICE_SERVER_LOCATION(pServer)->pURL,
                    AE_DRM_DEVICE(iDeviceId)->pModelNumber,
                    AE_DRM_DEVICE(iDeviceId)->pSerialNumber);
        }
        else
        {
            AeDRMDeviceServerAddRef(pBackupServer);

            /* mark backup server unregistered to initiate registration attempts */
            pBackupServer->bRegistered = AeFalse;

            if (g_drmSettings.iLogLevel >= AeLogInfo)
                AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_SERVER_STATUS,
                    "Switching to backup server (%s) for device: model: %s, serial number: %s",
                    AE_DRM_DEVICE_SERVER_LOCATION(pServer)->pURL,
                    AE_DRM_DEVICE(iDeviceId)->pModelNumber,
                    AE_DRM_DEVICE(iDeviceId)->pSerialNumber);
        }
    }
}

/******************************************************************************/
static void AeDRMProcessResponse(AeDRMRequest *pRequest)
{
    AeDRMSOAP *pSOAP;
    AeHandle pMethod;
    AeBool bOK;

    pSOAP = AeDRMSOAPNew();
    if (!pSOAP)
        return;

    /* parse SOAP response */
    if (AeDRMSOAPParse(pSOAP, AeArrayGet(pRequest->pRawResponse, 0),
        AeArrayCount(pRequest->pRawResponse)))
    {
        pMethod = AeDRMSOAPGetFirstMethod(pSOAP);
        while (pMethod)
        {
            if (!strcasecmp(AeDRMSOAPGetMethodName(pMethod), "EEnterpriseProxy.Response"))
                AeDRMProcessSOAPResponse(pRequest, pMethod);

            else
                AeDRMProcessSOAPMethod(pRequest, pMethod);

            pMethod = AeDRMSOAPGetNextMethod(pMethod);
        }
    }

    bOK = AeDRMProcessSOAPResponseStatus(pRequest);

    AeDRMSOAPDestroy(pSOAP);

    AeDRMRequestComplete(pRequest, bOK);
}

/******************************************************************************/
static void AeDRMProcessSOAPMethod(AeDRMRequest *pRequest, AeHandle pMethod)
{
    AeDRMSOAPCommandStatus status;
    AeDRMSOAPCommandId soapId;

    AeChar *pModelNumber, *pSerialNumber;
    AeInt32 iDeviceId;
    AeBool bMethodOK;

    /* extract attributes: model number, serial number, command id, user id */
    pModelNumber = AeDRMSOAPGetAttributeValueByName(pMethod,
        AE_DRM_SOAP_ATTR_MN);
    pSerialNumber = AeDRMSOAPGetAttributeValueByName(pMethod,
        AE_DRM_SOAP_ATTR_SN);
    soapId.pCmdId = AeDRMSOAPGetAttributeValueByName(pMethod,
        AE_DRM_SOAP_ATTR_CMDID);
    soapId.pUserId = AeDRMSOAPGetAttributeValueByName(pMethod,
        AE_DRM_SOAP_ATTR_USERID);

    /* lookup target device */
    if (pModelNumber && pSerialNumber)
        iDeviceId = AeDRMGetDeviceId(pModelNumber, pSerialNumber);
    else
        iDeviceId = AE_DRM_MASTER_DEVICE_ID;
    if (iDeviceId == AE_DRM_INVALID_DEVICE_ID)
        return;

    status.iStatusCode = AE_DRM_SOAP_COMMAND_STATUS_NOT_IMPLEMENTED;
    status.pStatusReason = AE_DRM_SOAP_COMMAND_REASON_NOT_IMPLEMENTED;
    bMethodOK = AeTrue;

    if (!strcasecmp(AeDRMSOAPGetMethodName(pMethod), "DynamicData.SetTag"))
    {
        if (g_drmCallbacks.pOnCommandSetTag)
        {
            AeDRMDataItem dataItem;

             bMethodOK = AeDRMProcessSOAPSetTag(pMethod, &dataItem);
             if (bMethodOK)
                (*g_drmCallbacks.pOnCommandSetTag)(iDeviceId, &dataItem, &status);
        }
    }

    else if (!strcasecmp(AeDRMSOAPGetMethodName(pMethod), "EEnterpriseProxy.St"))
    {
        if (g_drmCallbacks.pOnCommandSetTime)
        {
            AeTimeValue *pTime;
            AeInt32 *piTZOffset;

            bMethodOK = AeDRMProcessSOAPSetTime(pMethod, &pTime, &piTZOffset);
            if (bMethodOK)
                (*g_drmCallbacks.pOnCommandSetTime)(iDeviceId, pTime, piTZOffset, &status);

            if (pTime)
                AeFree(pTime);
            if (piTZOffset)
                AeFree(piTZOffset);
        }
    }
    else if (!strcasecmp(AeDRMSOAPGetMethodName(pMethod), "EEnterpriseProxy.Rs"))
    {
        if (g_drmCallbacks.pOnCommandRestart)
        {
            AeBool bHard;

            bMethodOK = AeDRMProcessSOAPRestart(pMethod, &bHard);
            if (bMethodOK)
                (*g_drmCallbacks.pOnCommandRestart)(iDeviceId, bHard, &status);
        }
    }
    else if (!strcasecmp(AeDRMSOAPGetMethodName(pMethod), "EEnterpriseProxy.Pu"))
        bMethodOK = AeDRMProcessSOAPSetPingRate(pRequest->iConfigId, pMethod, &status);

#if defined(ENABLE_REMOTE_SESSION)
    else if (!strcasecmp(AeDRMSOAPGetMethodName(pMethod), "RConnection.StartInterface") ||
             !strcasecmp(AeDRMSOAPGetMethodName(pMethod), "RConnection.StartApplication"))
        bMethodOK = AeRemoteStartSession(iDeviceId, pRequest->iConfigId, pMethod, pRequest->pWebRequest->bSecure, &status);
#endif

#if defined(ENABLE_FILE_TRANSFER)
    else if (!strcasecmp(AeDRMSOAPGetMethodName(pMethod), "Package.Execute"))
        bMethodOK = AeFileTransferStart(iDeviceId, pRequest->iConfigId, pMethod, &status);
    else if (!strcasecmp(AeDRMSOAPGetMethodName(pMethod), "Package.CancelJob"))
        bMethodOK = AeFileTransferStop(iDeviceId, pMethod, AeFalse, &status);
    else if (!strcasecmp(AeDRMSOAPGetMethodName(pMethod), "Package.PauseJob"))
        bMethodOK = AeFileTransferStop(iDeviceId, pMethod, AeTrue, &status);
#endif

    else if (g_drmCallbacks.pOnSOAPMethodEx)
        (*g_drmCallbacks.pOnSOAPMethodEx)(iDeviceId, pRequest->iConfigId, pMethod, &soapId, &status);
    else if (g_drmCallbacks.pOnSOAPMethod)
        (*g_drmCallbacks.pOnSOAPMethod)(iDeviceId, pMethod, &status);

    if (!bMethodOK)
    {
        status.iStatusCode = AE_DRM_SOAP_COMMAND_STATUS_INVALID_PARAMS;
        status.pStatusReason = AE_DRM_SOAP_COMMAND_REASON_INVALID_PARAMS;
    }
    
    if (status.iStatusCode != AE_DRM_SOAP_COMMAND_STATUS_DEFERRED)
    {
        _AeDRMPostSOAPCommandStatus(iDeviceId, pRequest->iConfigId, AeDRMQueuePriorityNormal,
            &soapId, &status);
    }
}

/******************************************************************************/
static void AeDRMProcessSOAPResponse(AeDRMRequest *pRequest, AeHandle pMethod)
{
    AeHandle pParameter, pChild;
    AeDRMSOAPStatus *pSOAPStatus;
    AeChar *pValue;
#ifdef __GATEWAY__
    AeChar *pModelNumber, *pSerialNumber;
#endif
    AeInt32 iValue, iDeviceId;
    AeBool bGlobal;

    pParameter = AeDRMSOAPGetFirstParameter(pMethod);
    for (;pParameter; pParameter = AeDRMSOAPGetNextParameter(pParameter))
    {
        /* the only known parameter is "Status" */
        if (strcasecmp(AeDRMSOAPGetParameterName(pParameter), "Status"))
            continue;

        /* fetch status value */
        pValue = AeDRMSOAPGetParameterValue(pParameter);
        if (pValue)
            iValue = strtol(pValue, NULL, 10);
        else
            continue;

        /* fetch model/serial numbers */
        bGlobal = AeTrue;
        iDeviceId = AE_DRM_INVALID_DEVICE_ID;
#ifndef __GATEWAY__
        iDeviceId = AE_DRM_MASTER_DEVICE_ID;
#else
        pModelNumber = AeDRMSOAPGetAttributeValueByName(pParameter,
            AE_DRM_SOAP_ATTR_MN);
        pSerialNumber = AeDRMSOAPGetAttributeValueByName(pParameter,
            AE_DRM_SOAP_ATTR_SN);
        if (pModelNumber && pSerialNumber)
        {
            /* check if device with such name exists */
            iDeviceId = AeDRMGetDeviceId(pModelNumber, pSerialNumber);
            if (iDeviceId == AE_DRM_INVALID_DEVICE_ID)
                return;

            bGlobal = AeFalse;
        }
#endif

        /* create status object */
        pSOAPStatus = AeDRMSOAPStatusNew(iValue, bGlobal, iDeviceId);
        if (!pSOAPStatus)
            return;

        /* fetch owner */
        pValue = AeDRMSOAPGetAttributeValueByName(pParameter, "owner");
        if (pValue)
            AeSetString(&pSOAPStatus->pOwner, pValue, -1);

        /* look for instructions */
        pChild = AeDRMSOAPGetParameterFirstChild(pParameter);
        if (pChild && !strcasecmp(AeDRMSOAPGetParameterName(pChild), "Inst"))
        {
            /* fetch interval */
            pValue = AeDRMSOAPGetAttributeValueByName(pChild, "intv");
            if (pValue)
                pSOAPStatus->iInterval = strtol(pValue, NULL, 10);

            /* fetch URL */
            pValue = AeDRMSOAPGetAttributeValueByName(pChild, "url");
            if (pValue)
                AeSetString(&pSOAPStatus->pURL, pValue, -1);
        }

        /* add SOAP status to the list */
        AeListAppend(pRequest->pSOAPStatuses, pSOAPStatus);
    }
}

/******************************************************************************/
static AeBool AeDRMProcessSOAPSetTag(AeHandle pMethod, AeDRMDataItem *pDataItem)
{
    AeHandle pParameter;
    AeChar *pType, *pValue;

    pParameter = AeDRMSOAPGetParameterByName(pMethod, "DataItem");
    if (!pParameter)
        return AeFalse;

    pDataItem->pName = AeDRMSOAPGetAttributeValueByName(pParameter, "name");
    if (!pDataItem->pName)
        return AeFalse;

    pType = AeDRMSOAPGetAttributeValueByName(pParameter, "type");
    if (!pType)
        return AeFalse;

    pValue = AeDRMSOAPGetParameterValue(pParameter);
    if (!pValue)
        return AeFalse;

    if (!strcasecmp(pType, "STRING"))
    {
        pDataItem->value.iType = AeDRMDataString;
        pDataItem->value.data.pString = pValue;
    }
    else if (!strcasecmp(pType, "DIGITAL"))
    {
        pDataItem->value.iType = AeDRMDataDigital;
        pDataItem->value.data.bDigital = strtol(pValue, NULL, 10);
    }
    else if (!strcasecmp(pType, "ANALOG"))
    {
        pDataItem->value.iType = AeDRMDataAnalog;
        pDataItem->value.data.dAnalog = strtod(pValue, NULL);
    }
    else
        return AeFalse;

    pDataItem->value.iQuality = AeDRMDataGood;
    AeGetCurrentTime(&pDataItem->value.timeStamp);

    return AeTrue;
}

/******************************************************************************/
static AeBool AeDRMProcessSOAPSetTime(AeHandle pMethod, AeTimeValue **ppTime, AeInt32 **ppiTZOffset)
{
    AeChar *pValue;

    *ppTime = NULL;
    *ppiTZOffset = NULL;
    
    pValue = AeDRMSOAPGetParameterValueByName(pMethod, "Ti");
    if (pValue)
    {
        *ppTime = AeAlloc(sizeof(AeTimeValue));
        memset(*ppTime, 0, sizeof(AeTimeValue));
        (*ppTime)->iSec = strtol(pValue, NULL, 10);
    }

    pValue = AeDRMSOAPGetParameterValueByName(pMethod, "Tz");
    if (pValue)
    {
        *ppiTZOffset = AeAlloc(sizeof(AeInt32));
        **ppiTZOffset = strtol(pValue, NULL, 10);
    }

    if (!(*ppTime) && !(*ppiTZOffset))
        return AeFalse;

    return AeTrue;
}

/******************************************************************************/
static AeBool AeDRMProcessSOAPRestart(AeHandle pMethod, AeBool *pbHard)
{
    AeChar *pValue;

    pValue = AeDRMSOAPGetParameterValueByName(pMethod, "ha");
    if (!pValue)
        return AeFalse;

    *pbHard = (strtol(pValue, NULL, 10) ? AeTrue : AeFalse);

    return AeTrue;
}

/******************************************************************************/
static AeBool AeDRMProcessSOAPSetPingRate(AeInt32 iConfigId, AeHandle pMethod, AeDRMSOAPCommandStatus *pStatus)
{
    AeDRMServerConfig *pConfig;
    AeDRMPingRate pingRate;
    AeTimeValue duration;
    AeHandle *pParameter;
    AeChar *pValue;
    AeInt32 iNumber;
    
    pParameter = AeDRMSOAPGetParameterByName(pMethod, "Ra");
    if (!pParameter)
        return AeFalse;

    pValue = AeDRMSOAPGetParameterValue(pParameter);
    if (!pValue)
        return AeFalse;

    /* ping rate value */
    iNumber = strtol(pValue, NULL, 10);
    pingRate.rate.iSec = iNumber / 1000;
    pingRate.rate.iMicroSec = (iNumber % 1000) * 1000;

    /* optional duration */
    pValue = AeDRMSOAPGetAttributeValueByName(pParameter, "duration");
    if (!pValue)
        pingRate.pDuration = NULL;
    else
    {
        iNumber = strtol(pValue, NULL, 10);
        duration.iSec = iNumber / 1000;
        duration.iMicroSec = (iNumber % 1000) * 1000;
        pingRate.pDuration = &duration;
    }

    pStatus->iStatusCode = 0;
    pStatus->pStatusReason = NULL;

    pConfig = AE_DRM_CONFIG(iConfigId);

    /* apply the new ping rate */
    pConfig->effectivePingRate = pingRate.rate;
    if (pingRate.pDuration)
    {
        AE_TIME_VALUE_ADD(pConfig->pingRateReversionTime, g_cycleBeginTime, *pingRate.pDuration);
    }
    else
    {
        pConfig->basePingRate = pingRate.rate;
        memset(&pConfig->pingRateReversionTime, 0, sizeof(AeTimeValue));
    }

    /* optionally notify the application */
    if (g_drmCallbacks.pOnPingRateUpdate)
        (*g_drmCallbacks.pOnPingRateUpdate)(iConfigId, &pingRate);

    /* post updated ping rate back to the server */
    AeDRMPostPingRate(iConfigId, &pingRate.rate);

    return AeTrue;
}

/******************************************************************************/
static void AeDRMPostPingRate(AeInt32 iConfigId, AeTimeValue *pPingRate)
{
    /* [FIXME]: is this to be repeated for managed devices? */
    _AeDRMPostPingRate(AE_DRM_MASTER_DEVICE_ID, iConfigId, AeDRMQueuePriorityNormal, pPingRate);
}

/******************************************************************************/
static AeBool AeDRMProcessSOAPResponseStatus(AeDRMRequest *pRequest)
{
    AeListItem *pListItem;
    AeInt32 i, j;

    /* check if there're statuses received */
    if (AeListCount(pRequest->pSOAPStatuses) == 0)
    {
        if (g_drmSettings.iLogLevel >= AeLogError)
            AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_NETWORK,
                "Invalid response to message sent to %s: id=%d",
                pRequest->pLocation->pURL, pRequest->pEMessage->iId);

        /* consider invalid response as communication failure */
        AeDRMProcessCommStatus(pRequest->pLocation, AeFalse);

        return AeFalse;
    }

    /* process communication success */
    AeDRMProcessCommStatus(pRequest->pLocation, AeTrue);

    pListItem = AeListFirst(pRequest->pSOAPStatuses);
    while (pListItem)
    {
        AeDRMSOAPStatus *pStatus;
        
        pStatus = AeListData(pListItem, AeDRMSOAPStatus);
        pListItem = AeListNext(pListItem);

        for (i = 0; i < AePtrArrayCount(g_pDRMServerConfigs); i++)
        {
            AeDRMServerConfig *pConfig;

            pConfig = AePtrArrayGet(g_pDRMServerConfigs, i);
            if (pConfig)
            {
                for (j = 0; j < AePtrArrayCount(pConfig->pDeviceServers); j++)
                {
                    AeDRMDeviceServer *pServer;

                    if (!pStatus->bGlobal && j != pStatus->iDeviceId)
                        continue;
                    
                    pServer = AePtrArrayGet(pConfig->pDeviceServers, j);
                    if (pServer)
                        AeDRMProcessDeviceServerSOAPStatus(j, pServer, pRequest, pStatus);
                }
            }
        }
    }

    return AeTrue;
}

/******************************************************************************/
static void AeDRMProcessDeviceServerSOAPStatus(AeInt32 iDeviceId, AeDRMDeviceServer *pServer, AeDRMRequest *pRequest, AeDRMSOAPStatus *pStatus)
{
    AeDRMDeviceElement *pDeviceElement;
    AeDRMServerLocation *pNewLocation;

    /* skip currently unused device server */
    if (pServer->iRefCount <= 0)
        return;

    /* update device server descriptor only if its location matches
       the one used in request */
    if (AE_DRM_DEVICE_SERVER_LOCATION(pServer) != pRequest->pLocation)
        return;

    /* update device server descriptor only if there was a device element in request */
    pDeviceElement = AeDRMEMessageLookupDeviceElement(pRequest->pEMessage, iDeviceId);
    if (!pDeviceElement)
        return;

    switch (pStatus->iStatus)
    {
        case AE_DRM_SOAP_STATUS_SUCCESS:
            /* process response to registration */
            if (pDeviceElement->iType == AeDRMRegistration)
            {
                /* update device server descriptor */
                pServer->bRegistered = AeTrue;

#ifdef __GATEWAY__
                /* indicate the need to send MD message */
                if (iDeviceId != AE_DRM_MASTER_DEVICE_ID)
                  AE_DRM_DEVICE_SERVER(pServer->pParent, AE_DRM_MASTER_DEVICE_ID)->bSendMDMessage = AeTrue;
#endif

                pServer->bSendDSMessage = AeTrue;

                /* perform post-registration processing */
                if (g_drmCallbacks.pOnDeviceRegistered)
                    (*g_drmCallbacks.pOnDeviceRegistered)(iDeviceId);

                if (g_drmSettings.iLogLevel >= AeLogInfo)
                    AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_SERVER_STATUS,
                        "Device registered with server %s: model: %s, serial number: %s",
                        pRequest->pLocation->pURL,
                        AE_DRM_DEVICE(iDeviceId)->pModelNumber,
                        AE_DRM_DEVICE(iDeviceId)->pSerialNumber);
            }

            break;
        case AE_DRM_SOAP_STATUS_NOT_REGISTERED:
            pServer->bRegistered = AeFalse;

            /* calculate and set (randomized) time for next post */
            AeDRMCalcNextRetryTime(&pServer->nextRetryTime);

            if (g_drmSettings.iLogLevel >= AeLogInfo)
                AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_SERVER_STATUS,
                    "Device has not been registered with server %s: model: %s, serial number: %s",
                    pRequest->pLocation->pURL,
                    AE_DRM_DEVICE(iDeviceId)->pModelNumber,
                    AE_DRM_DEVICE(iDeviceId)->pSerialNumber);

            break;
        case AE_DRM_SOAP_STATUS_ENTER_MAINTENANCE_MODE:
            /* update server location */
            pNewLocation = AeDRMGetNewLocation(pRequest->pLocation, pStatus->pURL,
                "/MtMessage");
            if (!pNewLocation)
                return;
            pServer->pLocation = pNewLocation;

            /* update device server descriptor */
            pServer->bMaintenanceMode = AeTrue;
            pServer->maintenancePingRate.iSec = pStatus->iInterval;
            pServer->bRegistered = AeFalse;

            if (g_drmSettings.iLogLevel >= AeLogInfo)
                AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_SERVER_STATUS,
                    "Entering maintenance mode with server %s for device: model: %s, serial number: %s",
                    pRequest->pLocation->pURL,
                    AE_DRM_DEVICE(iDeviceId)->pModelNumber,
                    AE_DRM_DEVICE(iDeviceId)->pSerialNumber);
            break;
        case AE_DRM_SOAP_STATUS_STAY_IN_MAINTENANCE_MODE:
            break;
        case AE_DRM_SOAP_STATUS_EXIT_MAINTENANCE_MODE:
            /* update server location if needed */
            pNewLocation = AeDRMGetNewLocation(pRequest->pLocation, pStatus->pURL,
                "/eMessage");
            if (!pNewLocation)
                return;
            pServer->pLocation = pNewLocation;

            /* update device server descriptor */
            pServer->bMaintenanceMode = AeFalse;

            /* update owner */
            if (pStatus->pOwner)
                AeSetString(&pServer->pOwner, pStatus->pOwner, -1);

            /* calculate and set (randomized) time for next post */
            AeDRMCalcNextRetryTime(&pServer->nextRetryTime);

            if (g_drmSettings.iLogLevel >= AeLogInfo)
                AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_SERVER_STATUS,
                    "Exiting maintenance mode with server %s for device: model: %s, serial number: %s",
                    pRequest->pLocation->pURL,
                    AE_DRM_DEVICE(iDeviceId)->pModelNumber,
                    AE_DRM_DEVICE(iDeviceId)->pSerialNumber);
            break;
    }
}

/******************************************************************************/
static AeDRMRequest *AeDRMGetRequest(AeDRMEMessageType iType, AeDRMDeviceServer *pServer)
{
    AeDRMRequest *pRequest;
    AeDRMServerLocation *pLocation;

    pLocation = AE_DRM_DEVICE_SERVER_LOCATION(pServer);

    pRequest = pLocation->pPendingRequest;
    if (!pRequest)
    {
        AeTimeValue timeStamp;
        AeGetCurrentTime(&timeStamp);

        pRequest = AeDRMRequestNew(iType, pServer, &timeStamp);
        pLocation->pPendingRequest = pRequest;
    }
    else if (!pRequest->bOpen)
        pRequest = NULL;

    return pRequest;
}

/******************************************************************************/
static AeBool AeDRMMayUseDeviceServer(AeDRMDeviceServer *pServer)
{
    /* skip currently unused device server */
    if (pServer->iRefCount <= 0)
        return AeFalse;

    /* do not send messages to device server until it is time for the next retry */
    if (pServer->nextRetryTime.iSec != 0)
    {
        if (AE_TIME_VALUE_LESS_THAN(g_cycleBeginTime, pServer->nextRetryTime))
            return AeFalse;

        /* indicate that the delay is over */
        memset(&pServer->nextRetryTime, 0, sizeof(AeTimeValue));
    }

    return AeTrue;
}

/******************************************************************************/
static AeBool AeDRMMayUseServerLocation(AeDRMServerLocation *pLocation)
{
    /* do not send messages to the location until it is time for the next retry */
    if (pLocation->nextRetryTime.iSec != 0)
    {
        if (AE_TIME_VALUE_LESS_THAN(g_cycleBeginTime, pLocation->nextRetryTime))
            return AeFalse;
    }

    return AeTrue;
}

/******************************************************************************/
static void AeDRMCalcNextRetryTime(AeTimeValue *pNextRetryTime)
{
    AeGetElapsedTime(pNextRetryTime);
    AE_TIME_VALUE_ADD(*pNextRetryTime, *pNextRetryTime, g_drmSettings.retryPeriod);
}

/******************************************************************************/
static AeBool AeDRMIsDeviceReady(AeInt iDeviceId)
{
    /* master device is always online */
    if (iDeviceId == AE_DRM_MASTER_DEVICE_ID)
        return AeTrue;

    if (AE_DRM_DEVICE(iDeviceId))
        return (AE_DRM_DEVICE(iDeviceId)->bEnabled);

    return AeFalse;
}

/******************************************************************************/
static AeDRMServerLocation *AeDRMGetNewLocation(AeDRMServerLocation *pOldLocation, AeChar *pURL, AeChar *pDefaultAbsPath)
{
    AeDRMServerLocation *pNewLocation;
    AeURL *pOldURL, *pNewURL;
    AeListItem *pListItem;

    /* decompose old URL */
    pOldURL = AeURLNew();
    if (!pOldURL)
        return NULL;
    AeURLSet(pOldURL, pOldLocation->pURL);

    /* decompose new URL */
    pNewURL = AeURLNew();
    if (!pNewURL)
        return NULL;
    if (pURL)
        AeURLSet(pNewURL, pURL);

    if (!pNewURL->pHost)
        AeURLSetHost(pNewURL, pOldURL->pHost);

    if (!pNewURL->iPort)
        AeURLSetPort(pNewURL, pOldURL->iPort);

    if (!pNewURL->pAbsPath || *pNewURL->pAbsPath == 0)
        AeURLSetAbsPath(pNewURL, pDefaultAbsPath);

    pNewLocation = AeDRMServerLocationNew(AeURLGet(pNewURL));
    if (!pNewLocation)
        return NULL;

    if (!pNewLocation->pURL)
    {
        AeDRMServerLocationDestroy(pNewLocation);
        return NULL;
    }

    /* search for existing server location */
    pListItem = AeListFind(g_pDRMServerLocations, pNewLocation);
    if (pListItem)
    {
        /* if identical server location exists, use it */
        AeDRMServerLocationDestroy(pNewLocation);
        pNewLocation = AeListData(pListItem, AeDRMServerLocation);
    }
    else
        AeListAppend(g_pDRMServerLocations, pNewLocation);

    AeURLDestroy(pOldURL);
    AeURLDestroy(pNewURL);

    return pNewLocation;
}

/******************************************************************************/
static AeInt32 AeDRMGetDeviceId(AeChar *pModelNumber, AeChar *pSerialNumber)
{
    AeDRMDevice *pDevice;
    AeInt32 iCount, i;

    iCount = AePtrArrayCount(g_pDRMDevices);
    for (i = 0; i < iCount; i++)
    {
        pDevice = (AeDRMDevice *) AePtrArrayGet(g_pDRMDevices, i);
        if (pDevice &&
            !strcmp(pDevice->pModelNumber, pModelNumber) &&
            !strcmp(pDevice->pSerialNumber, pSerialNumber))
            return i;
    }

    return AE_DRM_INVALID_DEVICE_ID;
}

/******************************************************************************/
static void AeDRMGetCurrentLocations(AeList *pLocations)
{
    AeDRMServerLocation *pLocation;
    AeListItem *pItem;

    pItem = AeListFirst(g_pDRMServerLocations);
    while (pItem)
    {
        pLocation = AeListData(pItem, AeDRMServerLocation);
        pItem = AeListNext(pItem);

        AeListAppend(pLocations, pLocation);
    }
}

