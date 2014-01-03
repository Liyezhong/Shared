/* $Id: AeInterface.c,v 1.19.2.3 2010/06/03 19:53:16 dkhodos Exp $ */

#define _AE_DEFINE_GLOBALS_
#include "Ae.h"
#undef _AE_DEFINE_GLOBALS_

extern void AeInitLocal(void);

/******************************************************************************/
AeError AeInitialize(void)
{
    AeError rc;

    /* initialize system-specific stuff */
    AeInitLocal();

    g_pLogFunc = AeLog;
    g_pLogExFunc = AeLogEx;
    
    rc = AeNetInitialize();
    if (rc != AeEOK)
        return rc;

    rc = AeLogOpen();
    if (rc != AeEOK)
        return rc;

    rc = AeWebInitialize();
    if (rc != AeEOK)
        return rc;

    rc = AeDRMInitialize();
    if (rc != AeEOK)
        return rc;

    return AeEOK;
}

/******************************************************************************/
void AeShutdown(void)
{
    AeDRMDestroy();

    if (g_pWebProxySOCKS)
        AeWebProxyDestroy(g_pWebProxySOCKS);
    if (g_pWebProxyHTTP)
        AeWebProxyDestroy(g_pWebProxyHTTP);

    if (g_pHostNameCache)
        AeListDestroy(g_pHostNameCache);
    AeMutexDestroy(&g_hostNameCacheLock);

    if (g_pWebAuthCache)
        AeListDestroy(g_pWebAuthCache);
    AeMutexDestroy(&g_webAuthCacheLock);

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    if (g_pWebSSLSessionCache)
        AeListDestroy(g_pWebSSLSessionCache);
    AeMutexDestroy(&g_webSSLSessionCacheLock);

    AeWebSSLDestroy();
#endif

    AeLogClose();
}

/******************************************************************************/
void AeSetLogFunc(void (*pLogFunc)(AeLogRecordType iType, AeChar *pFormat, ...))
{
    if (pLogFunc)
        g_pLogFunc = pLogFunc;
    else
        g_pLogFunc = AeLog;
}

/******************************************************************************/
void AeSetLogExFunc(void (*pLogExFunc)(AeLogRecordType iType, AeUInt32 iCategory, AeChar *pFormat, ...))
{
    if (pLogExFunc)
        g_pLogExFunc = pLogExFunc;
    else
        g_pLogExFunc = AeLogEx;
}

/******************************************************************************/
void AeDRMSetQueueSize(AeInt32 iSize)
{
    g_drmSettings.iQueueSize = iSize;
}

/******************************************************************************/
void AeDRMSetRetryPeriod(AeTimeValue *pPeriod)
{
    g_drmSettings.retryPeriod = *pPeriod;
}

/******************************************************************************/
void AeDRMSetTimeStampMode(AeBool bServerMode)
{
    g_drmSettings.bServerTimeStamp = bServerMode;
}

/******************************************************************************/
void AeDRMSetLogLevel(AeLogLevel iLevel)
{
    g_drmSettings.iLogLevel = iLevel;
}

/******************************************************************************/
void AeDRMSetDebug(AeBool bDebug)
{
    if (bDebug)
        g_drmSettings.iLogLevel = AeLogDebug;
    else
        g_drmSettings.iLogLevel = AeLogNone;
}

/******************************************************************************/
void AeDRMSetYieldOnIdle(AeBool bYieldOnIdle)
{
    g_drmSettings.bYieldOnIdle = bYieldOnIdle;
}

/******************************************************************************/
void AeDRMSetOnWebError(void (*pCallback)(AeInt32 iServerId, AeError iError))
{
    g_drmCallbacks.pOnWebError = pCallback;
}

/******************************************************************************/
void AeDRMSetOnDeviceRegistered(void (*pCallback)(AeInt32 iDeviceId))
{
    g_drmCallbacks.pOnDeviceRegistered = pCallback;
}

/******************************************************************************/
void AeDRMSetOnQueueStatus(void (*pCallback)(AeDRMQueueStatus iNewStatus))
{
    g_drmCallbacks.pOnQueueStatus = pCallback;
}

/******************************************************************************/
void AeDRMSetOnSOAPMethod(void (*pCallback)(AeInt32 iDeviceId, AeHandle pMethod, AeDRMSOAPCommandStatus *pStatus))
{
    g_drmCallbacks.pOnSOAPMethod = pCallback;
}

/******************************************************************************/
void AeDRMSetOnSOAPMethodEx(void (*pCallback)(AeInt32 iDeviceId, AeInt32 iServerId, AeHandle pMethod, AeDRMSOAPCommandId *pSOAPId, AeDRMSOAPCommandStatus *pStatus))
{
    g_drmCallbacks.pOnSOAPMethodEx = pCallback;
}

/******************************************************************************/
void AeDRMSetOnCommandSetTag(void (*pCallback)(AeInt32 iDeviceId, AeDRMDataItem *pDataItem, AeDRMSOAPCommandStatus *pStatus))
{
    g_drmCallbacks.pOnCommandSetTag = pCallback;
}

/******************************************************************************/
void AeDRMSetOnCommandSetTime(void (*pCallback)(AeInt32 iDeviceId, AeTimeValue *pTime, AeInt32 *piTZOffset, AeDRMSOAPCommandStatus *pStatus))
{
    g_drmCallbacks.pOnCommandSetTime = pCallback;
}

/******************************************************************************/
void AeDRMSetOnCommandRestart(void (*pCallback)(AeInt32 iDeviceId, AeBool bHard, AeDRMSOAPCommandStatus *pStatus))
{
    g_drmCallbacks.pOnCommandRestart = pCallback;
}

/******************************************************************************/
void AeDRMSetOnPingRateUpdate(void (*pCallback)(AeInt32 iServerId, AeDRMPingRate *pPingRate))
{
    g_drmCallbacks.pOnPingRateUpdate = pCallback;
}

/******************************************************************************/
void AeDRMSetOnFileDownloadBegin(AeBool (*pCallback)(AeInt32 iDeviceId, AePointer *ppUserData))
{
    g_drmCallbacks.pOnFileDownloadBegin = pCallback;
}

/******************************************************************************/
void
#ifndef ENABLE_LARGEFILE64
AeDRMSetOnFileDownloadData
#else
AeDRMSetOnFileDownloadData64
#endif
(AeBool (*pCallback)(AeInt32 iDeviceId, AeFileStat *pFile, AeChar *pData, AeInt32 iSize, AePointer pUserData))
{
    g_drmCallbacks.pOnFileDownloadData = pCallback;
}

/******************************************************************************/
void AeDRMSetOnFileDownloadEnd(void (*pCallback)(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData))
{
    g_drmCallbacks.pOnFileDownloadEnd = pCallback;
}

/******************************************************************************/
void
#ifndef ENABLE_LARGEFILE64
AeDRMSetOnFileUploadBegin
#else
AeDRMSetOnFileUploadBegin64
#endif
(AeBool (*pCallback)(AeInt32 iDeviceId, AeFileUploadSpec **ppUploads, AePointer *ppUserData))
{
    g_drmCallbacks.pOnFileUploadBegin = pCallback;
}

/******************************************************************************/
void
#ifndef ENABLE_LARGEFILE64
AeDRMSetOnFileUploadBeginEx
#else
AeDRMSetOnFileUploadBeginEx64
#endif
(AeBool (*pCallback)(AeInt32 iDeviceId, AeFileUploadSpec **ppUploads, AeFileUploadExecuteParam *pParam, AePointer *ppUserData))
{
    g_drmCallbacks.pOnFileUploadBeginEx = pCallback;
}

/******************************************************************************/
void
#ifndef ENABLE_LARGEFILE64
AeDRMSetOnFileUploadData
#else
AeDRMSetOnFileUploadData64
#endif
(AeBool (*pCallback)(AeInt32 iDeviceId, AeFileStat **ppFile, AeChar **ppData, AeInt32 *piSize, AePointer pUserData))
{
    g_drmCallbacks.pOnFileUploadData = pCallback;
}

/******************************************************************************/
void AeDRMSetOnRemoteSessionStart(AeBool (*pCallback)(AeRemoteInterface *pInterface))
{
#if defined(ENABLE_REMOTE_SESSION)
    g_drmCallbacks.pOnRemoteSessionStart = pCallback;
#endif
}

/******************************************************************************/
void AeDRMSetOnRemoteSessionEnd(void (*pCallback)(AeRemoteInterface *pInterface))
{
#if defined(ENABLE_REMOTE_SESSION)
    g_drmCallbacks.pOnRemoteSessionEnd = pCallback;
#endif
}

/******************************************************************************/

void AeDRMSetOnFileUploadEnd(void (*pCallback)(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData))
{
    g_drmCallbacks.pOnFileUploadEnd = pCallback;
}

/******************************************************************************/
void AeDRMSetOnFileTransferEvent(void (*pCallback)(AeInt32 iDeviceId, AeFileTransferEvent iEvent, AePointer pUserData))
{
    g_drmCallbacks.pOnFileTransferEvent = pCallback;
}

/******************************************************************************/
AeError AeDRMAddDevice(AeDRMDeviceType iType, AeChar *pModelNumber, AeChar *pSerialNumber, AeInt32 *piId)
{
    AeDRMDevice *pDevice, keyDevice;
    AeInt32 i;

    /* do not allow master device to be added more than once */
    if (iType == AeDRMDeviceMaster &&
        AePtrArrayGet(g_pDRMDevices, AE_DRM_MASTER_DEVICE_ID))
        return AeEExist;

    /* do not allow multiple devices with the same identification */
    keyDevice.pModelNumber = pModelNumber;
    keyDevice.pSerialNumber = pSerialNumber;
    if (AePtrArrayFind(g_pDRMDevices, &keyDevice))
        return AeEExist;

    /* create device descriptor */
    pDevice = AeDRMDeviceNew(iType, pModelNumber, pSerialNumber);
    if (!pDevice)
        return AeEMemory;

    /* add device */
    if (iType == AeDRMDeviceMaster)
    {
        AePtrArraySet(g_pDRMDevices, AE_DRM_MASTER_DEVICE_ID, pDevice);
        *piId = AE_DRM_MASTER_DEVICE_ID;
    }
    else
    {
        AePtrArraySet(g_pDRMDevices, AE_MAX(AE_DRM_MASTER_DEVICE_ID + 1,
            AePtrArrayCount(g_pDRMDevices)), pDevice);
        *piId = AePtrArrayCount(g_pDRMDevices) - 1;
    }

    /* update server configurations */
    for (i = 0; i < AePtrArrayCount(g_pDRMServerConfigs); i++)
    {
        AeDRMServerConfig *pConfig;

        pConfig = (AeDRMServerConfig *) AePtrArrayGet(g_pDRMServerConfigs, i);
        if (pConfig)
        {
            AeDRMDeviceServer *pServer;

            pServer = AeDRMDeviceServerNew(pConfig);
            if (!pServer)
                return AeEMemory;

            if (pConfig->iType != AeDRMServerConfigBackup)
                AeDRMDeviceServerAddRef(pServer);

            AePtrArraySet(pConfig->pDeviceServers, *piId, pServer);
        }
    }

    return AeEOK;
}


/******************************************************************************/
AeError AeDRMGetDeviceId(AeChar *pModelNumber, AeChar *pSerialNumber, AeInt32 *piId)
{
    AeDRMDevice *pDevice;
    AeError rc = AeEInvalidArg;
    int iCount = AePtrArrayCount(g_pDRMDevices);
    int     i;

    *piId = -1;
    for (i = 0; i < iCount; i++)
    {
        pDevice = (AeDRMDevice *) AePtrArrayGet(g_pDRMDevices, i);
        if (pDevice && !strcmp(pDevice->pModelNumber, pModelNumber) && !strcmp(pDevice->pSerialNumber, pSerialNumber))
        {
            *piId = i;
            rc = AeEOK;
            break;
        }
    }

    return rc;
}


/******************************************************************************/
AeError AeDRMAddServer(AeDRMServerConfigType iType, AeChar *pURL, AeChar *pOwner, AeTimeValue *pPingRate, AeInt32 *piId)
{
    AeDRMServerConfig *pConfig;
    AeError rc;

    /* do not allow more than one primary server */
    if (iType == AeDRMServerConfigPrimary &&
        AePtrArrayGet(g_pDRMServerConfigs, AE_DRM_PRIMARY_CONFIG_ID))
        return AeEExist;

    /* do not allow more than one backup server */
    if (iType == AeDRMServerConfigBackup &&
        AePtrArrayGet(g_pDRMServerConfigs, AE_DRM_BACKUP_CONFIG_ID))
        return AeEExist;

    /* create server configuration */
    pConfig = AeDRMServerConfigNew(iType, pOwner, pPingRate);
    if (!pConfig)
        return AeEMemory;

    rc = AeDRMServerConfigInitialize(pConfig, pURL);
    if (rc != AeEOK)
    {
        AeDRMServerConfigDestroy(pConfig);
        return rc;
    }

    /* add server configuration */
    if (iType == AeDRMServerConfigPrimary)
    {
        AePtrArraySet(g_pDRMServerConfigs, AE_DRM_PRIMARY_CONFIG_ID, pConfig);
        *piId = AE_DRM_PRIMARY_CONFIG_ID;
    }
    else if (iType == AeDRMServerConfigBackup)
    {
        AePtrArraySet(g_pDRMServerConfigs, AE_DRM_BACKUP_CONFIG_ID, pConfig);
        *piId = AE_DRM_BACKUP_CONFIG_ID;
    }
    else
    {
        AePtrArraySet(g_pDRMServerConfigs, AE_MAX(AE_DRM_BACKUP_CONFIG_ID + 1,
            AePtrArrayCount(g_pDRMServerConfigs)), pConfig);
        *piId = AePtrArrayCount(g_pDRMServerConfigs) - 1;
    }

    pConfig->iId = *piId;

    return AeEOK;
}


/******************************************************************************/
AeError AeDRMAddRemoteSession(AeInt32 iDeviceId, AeChar *pName, AeChar *pDescription, AeChar *pType, AeChar *pServer, AeUInt16 iPort)
{
#if defined(ENABLE_REMOTE_SESSION)
	AeRemoteInterface *pInterface, keyInterface;

    /* do not allow same interface twice */
	memset(&keyInterface, 0, sizeof(keyInterface));
    keyInterface.iDeviceId = iDeviceId;
    keyInterface.pName = pName;
    if (AeListFind(g_pRemoteInterfaces, &keyInterface))
        return AeEExist;

	pInterface = AeRemoteInterfaceNew(iDeviceId, pName, pDescription, pType, pServer, iPort);
	if(!pInterface)
        return AeEMemory;

    AeListAppend(g_pRemoteInterfaces, pInterface);

    return AeEOK;
#else
    return AeEInternal;
#endif /* ENABLE_REMOTE_SESSION */
}


/******************************************************************************/
AeError AeDRMPostDataItem(AeInt32 iDeviceId, AeInt32 iServerId, AeDRMQueuePriority iPriority, AeDRMDataItem *pDataItem)
{
    AeXMLElement *pContainerElement;
    AeError rc;

    pContainerElement = AeDRMEMessageCreateDataContainer1();
    if (!pContainerElement)
        return AeEMemory;

    AeDRMEMessageAddDataItem(pContainerElement, pDataItem, AeTrue);

    rc = AeDRMQueueAddItem(g_pQueue, AeDRMQueueItemSnapshot, iDeviceId, iServerId,
        AeDRMGetFormattedContent((AeXMLContent *) pContainerElement), iPriority);

    AeXMLContentDestroy((AeXMLContent *) pContainerElement);

    return rc;
}

/******************************************************************************/
AeError AeDRMPostDataItemSet(AeInt32 iDeviceId, AeInt32 iServerId, AeDRMQueuePriority iPriority, AeDRMDataItem *pDataItems[], AeInt32 iCount)
{
    AeXMLElement *pContainerElement;
    AeTimeValue *pCommonTimeStamp;
    AeError rc;
    AeInt32 i;

    if (iCount < 1)
        return AeEInvalidArg;

    pCommonTimeStamp = &pDataItems[0]->value.timeStamp;
    for (i = 1; i < iCount; i++)
    {
        if (pCommonTimeStamp->iSec != pDataItems[i]->value.timeStamp.iSec ||
            pCommonTimeStamp->iMicroSec != pDataItems[i]->value.timeStamp.iMicroSec)
        {
            pCommonTimeStamp = NULL;
            break;
        }
    }

    if (pCommonTimeStamp)
        pContainerElement = AeDRMEMessageCreateDataContainer2(pCommonTimeStamp);
    else
        pContainerElement = AeDRMEMessageCreateDataContainer1();

    if (!pContainerElement)
        return AeEMemory;

    for (i = 0; i < iCount; i++)
    {
        AeDRMEMessageAddDataItem(pContainerElement, pDataItems[i],
                                 pCommonTimeStamp ? AeFalse : AeTrue);
    }

    rc = AeDRMQueueAddItem(g_pQueue, AeDRMQueueItemSnapshot, iDeviceId, iServerId,
        AeDRMGetFormattedContent((AeXMLContent *) pContainerElement), iPriority);

    AeXMLContentDestroy((AeXMLContent *) pContainerElement);

    return rc;
}

/******************************************************************************/
AeError AeDRMPostAlarm(AeInt32 iDeviceId, AeInt32 iServerId, AeDRMQueuePriority iPriority, AeDRMAlarm *pAlarm)
{
    AeXMLElement *pAlarmElement;
    AeError rc;

    pAlarmElement = AeDRMEMessageCreateAlarm(pAlarm);
    if (!pAlarmElement)
        return AeEMemory;

    rc = AeDRMQueueAddItem(g_pQueue, AeDRMQueueItemAlarm, iDeviceId, iServerId,
        AeDRMGetFormattedContent((AeXMLContent *) pAlarmElement), iPriority);

    AeXMLContentDestroy((AeXMLContent *) pAlarmElement);

    return rc;
}

/******************************************************************************/
AeError AeDRMPostEvent(AeInt32 iDeviceId, AeInt32 iServerId, AeDRMQueuePriority iPriority, AeDRMEvent *pEvent)
{
    AeXMLElement *pEventElement;
    AeError rc;

    pEventElement = AeDRMEMessageCreateEvent(pEvent);
    if (!pEventElement)
        return AeEMemory;

    rc = AeDRMQueueAddItem(g_pQueue, AeDRMQueueItemEvent, iDeviceId, iServerId,
        AeDRMGetFormattedContent((AeXMLContent *) pEventElement), iPriority);

    AeXMLContentDestroy((AeXMLContent *) pEventElement);

    return rc;
}

/******************************************************************************/
AeError AeDRMPostEmail(AeInt32 iDeviceId, AeInt32 iServerId, AeDRMQueuePriority iPriority, AeDRMEmail *pEmail)
{
    AeXMLElement *pEmailElement;
    AeError rc;

    pEmailElement = AeDRMEMessageCreateEmail(pEmail);
    if (!pEmailElement)
        return AeEMemory;

    rc = AeDRMQueueAddItem(g_pQueue, AeDRMQueueItemEmail, iDeviceId, iServerId,
        AeDRMGetFormattedContent((AeXMLContent *) pEmailElement), iPriority);

    AeXMLContentDestroy((AeXMLContent *) pEmailElement);

    return rc;
}

/******************************************************************************/
AeError     AeDRMGetServerStatus(AeInt32 iId, AeBool *pbOnline)
{
    AeDRMServerConfig *pConfig;

    /* set to false */
    *pbOnline = AeFalse;

    /* Get the requested server */
    if(iId >= AE_DRM_PRIMARY_CONFIG_ID && iId <= AE_DRM_BACKUP_CONFIG_ID)
    {
        pConfig = (AeDRMServerConfig *) AePtrArrayGet(g_pDRMServerConfigs, iId);
        if(pConfig && pConfig->pLocation)
            *pbOnline = pConfig->pLocation->bOnline;
    }

    return AeEOK;
}

/******************************************************************************/
AeError
#ifndef ENABLE_LARGEFILE64
AeDRMPostFileUploadRequestEx
#else
AeDRMPostFileUploadRequestEx64
#endif
(AeInt32 iDeviceId, AeInt32 iServerId, AeDRMQueuePriority iPriority, AeFileUploadSpec **ppUploads, AeFileUploadRequestParam *pParam)
{
#if defined(ENABLE_FILE_TRANSFER)
    AeXMLElement *pElement;
    AeError rc;
    AeInt32 i;

    /* create container for upload request */
    pElement = AeDRMEMessageCreateUploadContainer(pParam);
    if (!pElement)
        return AeEMemory;

    i = 0;
    while (ppUploads[i])
    {
        AeDRMEMessageAddUploadItem(pElement, ppUploads[i]->pName, ppUploads[i]->bDelete);
        i += 1;
    }

    rc = AeDRMQueueAddItem(g_pQueue, AeDRMQueueItemMisc, iDeviceId, iServerId,
                           AeDRMGetFormattedContent((AeXMLContent *) pElement),
                           iPriority);

    AeXMLContentDestroy((AeXMLContent *) pElement);

    return rc;
#else
    return AeEInternal;
#endif
}

/******************************************************************************/
AeError
#ifndef ENABLE_LARGEFILE64
AeDRMPostFileUploadRequest
#else
AeDRMPostFileUploadRequest64
#endif
(AeInt32 iDeviceId, AeInt32 iServerId, AeDRMQueuePriority iPriority, AeFileUploadSpec **ppUploads)
{
    return
#ifndef ENABLE_LARGEFILE64
        AeDRMPostFileUploadRequestEx
#else
        AeDRMPostFileUploadRequestEx64
#endif
        (iDeviceId, iServerId, iPriority, ppUploads, NULL);
}

/******************************************************************************/
AeError AeDRMPostSOAPCommandStatus(AeInt32 iDeviceId, AeInt32 iServerId, AeDRMQueuePriority iPriority, AeDRMSOAPCommandId *pSOAPId, AeDRMSOAPCommandStatus *pStatus)
{
    return _AeDRMPostSOAPCommandStatus(iDeviceId, iServerId, iPriority, pSOAPId, pStatus);
}

/******************************************************************************/
AeError AeDRMPostOpaque(AeInt32 iDeviceId, AeInt32 iServerId, AeDRMQueuePriority iPriority, AeChar *pData)
{
    AeArray *pDataCopy;

    pDataCopy = AeArrayNew(1);
    if (!pDataCopy)
        return AeEMemory;

    AeArrayAppend(pDataCopy, pData, strlen(pData));

    return AeDRMQueueAddItem(g_pQueue, AeDRMQueueItemMisc, iDeviceId, iServerId,
        pDataCopy, iPriority);
}

/******************************************************************************/
AeError AeDRMSetDeviceStatus(AeInt32 iDeviceId, AeBool bEnable)
{
    AeDRMDevice *pDevice;
    AeDRMServerConfig *pConfig;
    AeInt32 i;

    pDevice = AePtrArrayGet(g_pDRMDevices, iDeviceId);
    if (!pDevice)
        return AeEInvalidArg;

    pDevice->bEnabled = bEnable;
    pDevice->bOnline = bEnable;

    for (i = 0; i < AePtrArrayCount(g_pDRMServerConfigs); i++)
    {
        pConfig = AePtrArrayGet(g_pDRMServerConfigs, i);
        AE_DRM_DEVICE_SERVER(pConfig, AE_DRM_MASTER_DEVICE_ID)->bSendMDMessage = AeTrue;
        AE_DRM_DEVICE_SERVER(pConfig, iDeviceId)->bSendDSMessage = AeTrue;
    }

    return AeEOK;
}

/******************************************************************************/
AeError AeDRMSetDeviceOnline(AeInt32 iDeviceId, AeBool bOnline)
{
    AeDRMDevice *pDevice;
    AeDRMServerConfig *pConfig;
    AeInt32 i;
    
    pDevice = AePtrArrayGet(g_pDRMDevices, iDeviceId);
    if (!pDevice)
        return AeEInvalidArg;

    pDevice->bOnline = bOnline;

    for (i = 0; i < AePtrArrayCount(g_pDRMServerConfigs); i++)
    {
        pConfig = AePtrArrayGet(g_pDRMServerConfigs, i);
        AE_DRM_DEVICE_SERVER(pConfig, iDeviceId)->bSendDSMessage = AeTrue;
    }

    return AeEOK;
}

/******************************************************************************/
AeError AeWebSetVersion(AeInt32 iServerId, AeWebVersion iVersion)
{
    AeDRMServerConfig *pConfig;

    pConfig = AE_DRM_CONFIG(iServerId);
    if (!pConfig)
        return AeEInvalidArg;

    if (iVersion != AeWebHTTP10 && iVersion != AeWebHTTP11)
        return AeEInvalidArg;

    pConfig->iWebVersion = iVersion;

    return AeEOK;
}

/******************************************************************************/
AeError AeWebSetPersistent(AeInt32 iServerId, AeBool bPersistent)
{
    AeDRMServerConfig *pConfig;

    pConfig = AE_DRM_CONFIG(iServerId);
    if (!pConfig)
        return AeEInvalidArg;

    pConfig->bWebPersistent = bPersistent;

    return AeEOK;
}

/******************************************************************************/
AeError AeWebSetTimeout(AeInt32 iServerId, AeTimeValue *pTimeOut)
{
    AeDRMServerConfig *pConfig;

    pConfig = AE_DRM_CONFIG(iServerId);
    if (!pConfig)
        return AeEInvalidArg;

    pConfig->webTimeOut = *pTimeOut;

    return AeEOK;
}

/******************************************************************************/
AeError AeWebSetProxy(AeWebProxyProtocol iProto, AeChar *pHost, AeUInt16 iPort, AeChar *pUser, AeChar *pPassword)
{
    if (iProto == AeWebProxyProtoSOCKS)
        AeWebProxySet(&g_pWebProxySOCKS, iProto, pHost, iPort, pUser, pPassword);
    else if (iProto == AeWebProxyProtoHTTP)
        AeWebProxySet(&g_pWebProxyHTTP, iProto, pHost, iPort, pUser, pPassword);
    else
        return AeEInvalidArg;

    return AeEOK;
}

/******************************************************************************/
AeError AeWebSetSSL(AeWebCryptoLevel iLevel, AeBool bServerAuth, AeChar *pCACertFile)
{
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    const SSL_METHOD *sslMethod;
    int rc;

    rc = 1;

    /* remember crypto level */
    g_webSSLInfo.iCryptoLevel = iLevel;
    if (iLevel == AeWebCryptoNone)
        return AeEOK;

    sslMethod = SSLv23_method();

    /* create SSL context */
    if (!g_webSSLInfo.pContext)
    {
        g_webSSLInfo.pContext = SSL_CTX_new(sslMethod);
        if (!g_webSSLInfo.pContext)
            return AeESSLGeneral;

        SSL_CTX_set_mode(g_webSSLInfo.pContext, SSL_MODE_ENABLE_PARTIAL_WRITE);
    }

    /* initialize cipher suites in accordance with crypto level */
    switch (iLevel)
    {
        case AeWebCryptoLow:
            rc = SSL_CTX_set_cipher_list(g_webSSLInfo.pContext,
                                         "EXP1024-RC4-SHA:EXP1024-RC4-MD5");
            break;
        case AeWebCryptoMedium:
            rc = SSL_CTX_set_cipher_list(g_webSSLInfo.pContext,
                                         "EXP1024-RC4-SHA:EXP1024-RC4-MD5:AES128-SHA:RC4-SHA:RC4-MD5");
            break;
        case AeWebCryptoHigh:
            rc = SSL_CTX_set_cipher_list(g_webSSLInfo.pContext,
                                         "EXP1024-RC4-SHA:EXP1024-RC4-MD5:AES128-SHA:RC4-SHA:RC4-MD5:AES256-SHA:DES-CBC3-SHA");
            break;
        default:
            break;
    }

    if (!rc)
        return AeESSLGeneral;

    if (bServerAuth && pCACertFile)
    {
        rc = SSL_CTX_load_verify_locations(g_webSSLInfo.pContext, pCACertFile, NULL);
        if (!rc)
            return AeESSLGeneral;

        rc = SSL_CTX_set_default_verify_paths(g_webSSLInfo.pContext);
        if (!rc)
            return AeESSLGeneral;

        g_webSSLInfo.bServerAuth = bServerAuth;
    }

    return AeEOK;
#else
    return AeESSLGeneral;
#endif
}

/******************************************************************************/
AeError AeWebSyncExecute(AeWebRequest *pRequest)
{
    AeWebUserAgent *pUA;
    AeError rc;

    pUA = AeWebUserAgentNew();
    if (!pUA)
        return AeEMemory;

    /* submit requests to the web client */
    rc = AeWebUserAgentSyncExecute(pUA, &pRequest, 1);
    if (rc != AeEOK)
        return rc;

    AeWebUserAgentDestroy(pUA);

    return AeEOK;
}

/******************************************************************************/
AeError AeWebAsyncExecute(AeHandle *ppHandle, AeWebRequest *pRequest, AeTimeValue *pTimeLimit, AeBool *pbComplete)
{
    AeWebUserAgent *pUA;
    AeTimeValue startTime;
    AeError rc;
    
    if (!(*ppHandle))
    {
        pUA = AeWebUserAgentNew();
        if (!pUA)
            return AeEMemory;

        /* submit requests to the web client */
        rc = AeWebUserAgentAsyncExecute(pUA, &pRequest, 1);
        if (rc != AeEOK)
            return rc;

        *ppHandle = pUA;
    }
    else
        pUA = (AeWebUserAgent *) *ppHandle;
        
    AeGetElapsedTime(&startTime);

    /* perform request processing */
    rc = AeEOK;
    while (!(*pbComplete = !AeWebUserAgentEventsPending(pUA)))
    {
        /* process events */
        rc = AeWebUserAgentDoEvents(pUA, pTimeLimit);
        if (rc != AeEOK)
            break;

        if (pTimeLimit)
        {
            AeTimeValue currentTime, elapsedTime;
        
            /* calculate elapsed time since the beginning */
            AeGetElapsedTime(&currentTime);
            AE_TIME_VALUE_SUBTRACT(elapsedTime, currentTime, startTime);

            /* do not stay here for longer than the "time limit" */
            if (!AE_TIME_VALUE_GREATER_THAN(*pTimeLimit, elapsedTime))
                break;
        }
    }

    if (*pbComplete)
    {
        AeWebUserAgentDestroy(pUA);
        *ppHandle = NULL;
    }

    return rc;
}

/******************************************************************************/
AeChar *AeGetErrorString(AeError iError)
{
    return AE_ERROR_STRING(iError);
}

/******************************************************************************/
void AeGetLogCategoryString(AeUInt32 iCategory, AeChar *pBuffer, AeInt32 iMaxLength)
{
    AeUInt32 iMask;
    AeInt32 iLength, i;
    
    if (iMaxLength <= 0)
        return;
    
    iLength = 0;
    iMask = iCategory;
    pBuffer[0] = 0;
    for (i = 0; i < 32; i++)
    {
        if (iMask & 1)
        {
            AeInt32 iElementLength; 
            
            if (iLength != 0)
            {
                if (iLength < iMaxLength)
                {
                    strcat(pBuffer + iLength, " ");
                    iLength += 1;
                }
                else
                    break;
            }

            iElementLength = strlen(g_pLogCategoryStrings[i]);
            if ((iLength + iElementLength) < iMaxLength)
            {
                strcat(pBuffer + iLength, g_pLogCategoryStrings[i]);
                iLength += iElementLength;
            }
            else
                break;
        }

        iMask >>= 1;
        if (iMask == 0)
            break;
    }
}
