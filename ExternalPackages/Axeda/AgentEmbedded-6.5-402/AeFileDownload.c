/* $Id: AeFileDownload.c,v 1.22 2008/05/21 18:24:34 dkhodos Exp $ */

#include "Ae.h"

#if defined(ENABLE_FILE_TRANSFER)

static AeBool AeFileDownloadInitialize(AeFileDownload *pDownload, AeHandle pParameters);
static void AeFileDownloadProcess(AeFileDownload *pDownload);
static void AeFileDownloadDestroy(AeFileDownload *pDownload);
static void AeFileDownloadNotify(AeFileDownload *pDownload, AeFileTransferEvent iEvent);
static AeBool AeFileDownloadIsIdle(AeFileDownload *pDownload);
static AeBool AeFileDownloadIsPendingRequest(AeFileDownload *pDownload);

static void AeFileDownloadReset(AeFileDownload *pDownload);
static AeBool AeFileDownloadDecode(AeFileDownload *pDownload);
static AeBool AeFileDownloadFinalize(AeFileDownload *pDownload);
static AeBool AeFileDownloadOpenFile(AeFileDownload *pDownload);
static AeBool AeFileDownloadWriteFile(AeFileDownload *pDownload, AeChar *pData, AeInt32 iSize);
static AeBool AeFileDownloadIsFatalError(AeFileTransferError iError);
static AeWebRequest *AeFileDownloadRequestNew(AeFileDownload *pDownload);

/* callbacks for download HTTP request */
static void AeFileDownloadRequestOnError(AeWebRequest *pRequest, AeError iError);
static AeBool AeFileDownloadRequestOnEntity(AeWebRequest *pRequest, AeInt32 iDataOffset, AeChar *pData, AeInt32 iSize);
static void AeFileDownloadRequestOnCompleted(AeWebRequest *pRequest);
static AeBool AeFileDownloadRequestOnResponse(AeWebRequest *pRequest, AeInt iStatusCode);

static AeBool AeFileDownloadReadDownloads(AeFileDownload *pDownload, AeHandle pParameters);
static AeFileDownloadSpec *AeFileDownloadSpecNew(void);
static void AeFileDownloadSpecDestroy(AeFileDownloadSpec *pDownloadSpec);

static AeFileInstructionVTable g_fileDownloadVTable =
{
    (AeBool (*)(AeFileInstruction *, AeHandle)) AeFileDownloadInitialize,
    (void (*)(AeFileInstruction *)) AeFileDownloadProcess,
    (void (*)(AeFileInstruction *)) AeFileDownloadDestroy,
    (void (*)(AeFileInstruction *, AeFileTransferEvent)) AeFileDownloadNotify,
    (AeBool (*)(AeFileInstruction *)) AeFileDownloadIsIdle,
    (AeBool (*)(AeFileInstruction *)) AeFileDownloadIsPendingRequest
};

/**************************************************************************/
AeFileDownload *AeFileDownloadNew(void)
{
    AeFileDownload *pDownload = (AeFileDownload *) AeAlloc(sizeof(AeFileDownload));
    if (!pDownload)
        return NULL;
    memset(pDownload, 0, sizeof(AeFileDownload));

    pDownload->parent.pVTable = &g_fileDownloadVTable;
    pDownload->parent.iState = AeFileInstructionInitialized;
    pDownload->parent.iError = AeNOERROR;

    pDownload->pFileList = AeListNew(NULL, (AeReleaseFunc) AeFileDownloadSpecDestroy);
    if (!pDownload->pFileList)
    {
        AeFileDownloadDestroy(pDownload);
        return NULL;
    }

    pDownload->pURL = NULL;
    pDownload->pSessionId = NULL;
    pDownload->pId = NULL;
    pDownload->iDownloadSize = 0;
    pDownload->iMaxRetryCount = 0;
    pDownload->iRetryMinDelay = 0;
    pDownload->iRetryMaxDelay = 0;
    pDownload->iChunkSize = AE_BUFFER_CHUNK;
    pDownload->iChunkDelay = 0;
    pDownload->iMode = AeFileDownloadModeOverwrite;
    pDownload->iCompression = AeFileCompressionTarGzip;
    pDownload->pDestPath = NULL;

    memset(&pDownload->md5Global, 0, sizeof(pDownload->md5Global));
    memset(&pDownload->md5Request, 0, sizeof(pDownload->md5Request));
    pDownload->pFileFilter = NULL;
    pDownload->pFileFilterHandler = NULL;
    pDownload->pRequest = NULL;
    pDownload->iRetryCount = 0;
    pDownload->iDownloadPos = 0;
    pDownload->pCurFileStat = NULL;
    pDownload->iCurFileHandle = AeFileInvalidHandle;
    pDownload->bUserCallback = AeFalse;
    pDownload->pUserData = NULL;

    return pDownload;
}

/**************************************************************************/
void AeFileDownloadDestroy(AeFileDownload *pDownload)
{
    if (pDownload->pURL)
        AeFree(pDownload->pURL);
    if (pDownload->pSessionId)
        AeFree(pDownload->pSessionId);
    if (pDownload->pId)
        AeFree(pDownload->pId);
    if (pDownload->pDestPath)
        AeFree(pDownload->pDestPath);
    if (pDownload->pFileList)
        AeListDestroy(pDownload->pFileList);
    if (pDownload->pFileFilter && pDownload->pFileFilterHandler)
        (*pDownload->pFileFilterHandler->pDestroy)(pDownload->pFileFilter);
    if (pDownload->pRequest)
        AeWebRequestDestroy(pDownload->pRequest);
    if (pDownload->iCurFileHandle != AeFileInvalidHandle)
        AeFileClose(pDownload->iCurFileHandle);

    AeFree(pDownload);
}

/**************************************************************************/
static void AeFileDownloadNotify(AeFileDownload *pDownload, AeFileTransferEvent iEvent)
{
    if (g_drmCallbacks.pOnFileTransferEvent)
        (*g_drmCallbacks.pOnFileTransferEvent)(pDownload->parent.pPackage->iDeviceId,
            iEvent, pDownload->pUserData);
}

/***************************************************************************/
static AeBool AeFileDownloadIsIdle(AeFileDownload *pDownload)
{
    return AeFalse;
}

/***************************************************************************/
static AeBool AeFileDownloadIsPendingRequest(AeFileDownload *pDownload)
{
    return (pDownload->pRequest != NULL);
}

/**************************************************************************/
static void AeFileDownloadReset(AeFileDownload *pDownload)
{
    if (pDownload->pFileFilter && pDownload->pFileFilterHandler)
        (*pDownload->pFileFilterHandler->pReset)(pDownload->pFileFilter);

    if (pDownload->iCurFileHandle != AeFileInvalidHandle)
    {
        AeFileClose(pDownload->iCurFileHandle);
        pDownload->iCurFileHandle = AeFileInvalidHandle;
    }

    MD5_Init(&pDownload->md5Global);

    pDownload->iDownloadPos = 0;
    pDownload->pCurFileStat = NULL;
    pDownload->iCurFileHandle = AeFileInvalidHandle;
    pDownload->bUserCallback = AeFalse;
    pDownload->pUserData = NULL;
}

/**************************************************************************/
static AeBool AeFileDownloadInitialize(AeFileDownload *pDownload, AeHandle pParameters)
{
    AeHandle *pChild;
    AeChar *pTmp;

    AeSetString(&pDownload->pURL,
                AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_URL), -1);

    AeSetString(&pDownload->pSessionId,
                AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_SESSION_ID), -1);
 
    AeSetString(&pDownload->pId,
                AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_PUB_ID), -1);

    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_SIZE);
    if (pTmp)
        pDownload->iDownloadSize =
#ifndef ENABLE_LARGEFILE64
            strtol(pTmp, NULL, 10);
#else
            AeStrToInt64(pTmp, NULL, 10);
#endif

    if (!pDownload->pURL || !pDownload->pSessionId || pDownload->iDownloadSize == 0)
    {
        pDownload->parent.iState = AeFileInstructionFinished;
        pDownload->parent.iError = AeBAD_FORMAT;
        return AeFalse;
    }

    /* base64 encoded checksum */
    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_CHECKSUM);
    if (pTmp && strlen(pTmp) == 24)
        AeBase64Decode(pTmp, strlen(pTmp), (AeChar *) pDownload->pCheckSum);
    else
    {
        pDownload->parent.iState = AeFileInstructionFinished;
        pDownload->parent.iError = AeBAD_FORMAT;
        return AeFalse;
    }

    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_RETRY_COUNT);
    if (pTmp)
        pDownload->iMaxRetryCount = strtol(pTmp, NULL, 10);

    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_MIN_DELAY);
    if (pTmp)
        pDownload->iRetryMinDelay = strtol(pTmp, NULL, 10);

    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_MAX_DELAY);
    if (pTmp)
        pDownload->iRetryMaxDelay = strtol(pTmp, NULL, 10);

    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_CHUNK_SIZE);
    if (pTmp)
        pDownload->iChunkSize = strtol(pTmp, NULL, 10);

    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_CHUNK_DELAY);
    if (pTmp)
        pDownload->iChunkDelay = strtol(pTmp, NULL, 10);

    /* priority: this is treated by us as a package property */
    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_PRIORITY);
    if (pTmp)
        AeFileTransferPackageSetPriority(pDownload->parent.pPackage, strtol(pTmp, NULL, 10));

    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_MODE);
    if (!strcmp(pTmp, "overwrite"))
        pDownload->iMode = AeFileDownloadModeOverwrite;
    else if (!strcmp(pTmp, "upgrade"))
        pDownload->iMode = AeFileDownloadModeUpgrade;

    /* Now read the download destination */
    pChild = AeDRMSOAPGetParameterFirstChild(pParameters);
    if (pChild && !strcasecmp(AeDRMSOAPGetParameterName(pChild), AE_FILETRANSFER_DESTINATION))
        AeSetString(&pDownload->pDestPath,
                    AeDRMSOAPGetAttributeValueByName(pChild,AE_FILETRANSFER_PATH), -1);
  
    /* Now read the download file list */
    if (!AeFileDownloadReadDownloads(pDownload, pParameters))
    {
        if (g_drmSettings.iLogLevel >= AeLogError)
            AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_DOWNLOAD,
                "Download (id=%s): error processing file specifications",
                pDownload->pId);

        pDownload->parent.iState = AeFileInstructionFinished;
        return AeFalse;
    }

    /* compression type */
    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_COMPRESSION);
    if (pTmp && !strcmp(pTmp, "none"))
    {
        AeFileStat file;

        memset(&file, 0, sizeof(AeFileStat));
        file.pName = AeListData(AeListFirst(pDownload->pFileList), AeFileDownloadSpec)->pName;
        file.iSize = pDownload->iDownloadSize;
        
        /* initialize null filter */
        if ((pDownload->pFileFilter = AeNullFileFilterNew(AeFalse)) &&
            AeNullFileFilterInitialize((AeNullFileFilter *) pDownload->pFileFilter, &file))
        {
            pDownload->pFileFilterHandler = &g_nullFileFilterHandler;
            pDownload->iCompression = AeFileCompressionNone;
        }
    }
    else if (!pTmp || !strcasecmp(pTmp, "tar-gzip"))
    {
        /* initialize tar/gzip filter */
        if ((pDownload->pFileFilter = AeTarNew(AeFalse, &g_gzipFilterHandler)) &&
            AeTarInitialize((AeTar *) pDownload->pFileFilter))
        {
            pDownload->pFileFilterHandler = &g_tarFileFilterHandler;
            pDownload->iCompression = AeFileCompressionTarGzip;
        }
    }
    else
    {
        if (g_drmSettings.iLogLevel >= AeLogError)
            AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_DOWNLOAD,
                "Download (id=%s): unrecognized compression type: %s",
                pDownload->pId, pTmp);
    }

    if (!pDownload->pFileFilter || !pDownload->pFileFilterHandler)
    {
        pDownload->parent.iState = AeFileInstructionFinished;
        pDownload->parent.iError = AeFAILED;
        return AeFalse;
    }

    /* initialize global md5 digest */
    MD5_Init(&pDownload->md5Global);

    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_DOWNLOAD,
            "Download (id=%s): initialized: package=%s, compression=%s",
            pDownload->pId, pDownload->parent.pPackage->pJobId,
            g_pCompressionStrings[pDownload->iCompression]);

    return AeTrue;
}

/***************************************************************************/
static void AeFileDownloadProcess(AeFileDownload *pDownload)
{
    if (pDownload->parent.iState == AeFileInstructionInitialized)
    {
        if (g_drmSettings.iLogLevel >= AeLogInfo)
            AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_FILE_DOWNLOAD,
                "Download (id=%s): started (size: %" AE_LONGINT_FORMAT_MODIFIER "d)",
                pDownload->pId, pDownload->iDownloadSize);

        pDownload->parent.iState = AeFileInstructionExecuting;
    }

    if (pDownload->parent.iState == AeFileInstructionExecuting)
    {
        /* web request in progress: check back later */
        if (pDownload->pRequest)
            return;

        if (pDownload->parent.iError == AeNOERROR)
        {
            if (g_drmSettings.iLogLevel >= AeLogDebug)
                AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_DOWNLOAD,
                    "Download (id=%s): %d%% complete",
                    pDownload->pId, 100 * pDownload->iDownloadPos / pDownload->iDownloadSize);
        }

        /* check if download is complete */
        if (pDownload->iDownloadPos >= pDownload->iDownloadSize)
        {
            /* try to finish the download */
            if (AeFileDownloadFinalize(pDownload))
            {
                /* notify user about success */
                if (pDownload->bUserCallback)
                    (*g_drmCallbacks.pOnFileDownloadEnd)(pDownload->parent.pPackage->iDeviceId,
                                                         AeTrue, pDownload->pUserData);

                if (g_drmSettings.iLogLevel >= AeLogInfo)
                    AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_FILE_DOWNLOAD,
                        "Download (id=%s): success", pDownload->pId);

                pDownload->parent.iState = AeFileInstructionFinished;
                return;
            }
        }

        /* check for errors */
        if (pDownload->parent.iError != AeNOERROR)
        {
            if (g_drmSettings.iLogLevel >= AeLogError)
                AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_DOWNLOAD,
                    "Download (id=%s): failed with code: %d",
                    pDownload->pId, pDownload->parent.iError);

            /* notify the user about the error */
            if (pDownload->bUserCallback)
                (*g_drmCallbacks.pOnFileDownloadEnd)(pDownload->parent.pPackage->iDeviceId,
                                                     AeFalse, pDownload->pUserData);

            if (AeFileDownloadIsFatalError(pDownload->parent.iError) ||
                pDownload->iRetryCount >= pDownload->iMaxRetryCount)
            {
                pDownload->parent.iState = AeFileInstructionFinished;
                return;
            }

            pDownload->iRetryCount += 1;
            AeFileDownloadReset(pDownload);

            if (g_drmSettings.iLogLevel >= AeLogInfo)
                AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_FILE_DOWNLOAD,
                    "Download (id=%s): retry %d/%d",
                    pDownload->pId, pDownload->iRetryCount, pDownload->iMaxRetryCount);

            pDownload->parent.iError = AeNOERROR;
        }

        /* create new web request */
        pDownload->pRequest = AeFileDownloadRequestNew(pDownload);
        if (!pDownload->pRequest)
        {
            pDownload->parent.iState = AeFileInstructionFinished;
            pDownload->parent.iError = AeFAILED;
            return;
        }

        /* initialize request md5 digest */
        MD5_Init(&pDownload->md5Request);

        /* submit web request */
        AeWebUserAgentAsyncExecute(g_pWebUA, &pDownload->pRequest, 1);
    }
}

/***************************************************************************/
static AeBool AeFileDownloadDecode(AeFileDownload *pDownload)
{
    while (1)
    {
        if ((*pDownload->pFileFilterHandler->pIsEndOfFile)(pDownload->pFileFilter))
        {
            if (!(*pDownload->pFileFilterHandler->pReadFile)(pDownload->pFileFilter, &pDownload->pCurFileStat))
                break;

            if (!pDownload->bUserCallback)
            {
                if (!AeFileDownloadOpenFile(pDownload))
                    break;
            }
        }
        else
        {
            AeChar *pChunkData;
            AeInt32 iChunkSize;

            if ((iChunkSize = (*pDownload->pFileFilterHandler->pReadFileData)(pDownload->pFileFilter, &pChunkData)) <= 0)
                break;

            if (pDownload->bUserCallback)
            {
                AeBool bOK;

                /* pass chunk to user callback */
                bOK = (*g_drmCallbacks.pOnFileDownloadData)(pDownload->parent.pPackage->iDeviceId,
                                                            pDownload->pCurFileStat,
                                                            pChunkData, iChunkSize,
                                                            pDownload->pUserData);

                /* indicate end-of-file */
                if (bOK && (*pDownload->pFileFilterHandler->pIsEndOfFile)(pDownload->pFileFilter))
                    bOK = (*g_drmCallbacks.pOnFileDownloadData)(pDownload->parent.pPackage->iDeviceId,
                                                                pDownload->pCurFileStat,
                                                                NULL, 0, pDownload->pUserData);
                    
                if (!bOK)
                {
                    pDownload->parent.iError = AeFAILED;
                    break;
                }
            }
            else
            {
                if (!AeFileDownloadWriteFile(pDownload, pChunkData, iChunkSize))
                    break;
                
                if ((*pDownload->pFileFilterHandler->pIsEndOfFile)(pDownload->pFileFilter))
                {
                    AeFileClose(pDownload->iCurFileHandle);
                    pDownload->iCurFileHandle = AeFileInvalidHandle;
                }
            }
        }
    }

    if ((*pDownload->pFileFilterHandler->pGetError)(pDownload->pFileFilter) != 0)
    {
        if (g_drmSettings.iLogLevel >= AeLogError)
            AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_DOWNLOAD,
                "Download (id=%s): content decoding error: %s",
                pDownload->pId,
                (*pDownload->pFileFilterHandler->pGetErrorString)(pDownload->pFileFilter,
                    (*pDownload->pFileFilterHandler->pGetError)(pDownload->pFileFilter)));

        pDownload->parent.iError = AeARCHIVE_ERROR;
    }

    if (pDownload->parent.iError != AeNOERROR)
        return AeFalse;

    return AeTrue;
}

/***************************************************************************/
static AeBool AeFileDownloadFinalize(AeFileDownload *pDownload)
{
    AeUInt8 pDigest[MD5_DIGEST_LENGTH];

    /* check global md5 digest */
    MD5_Final(pDigest, &pDownload->md5Global);
    if (memcmp(pDigest, pDownload->pCheckSum, MD5_DIGEST_LENGTH))
    {
        if (g_drmSettings.iLogLevel >= AeLogError)
            AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_DOWNLOAD,
                "Download (id=%s): checksum error", pDownload->pId);

        pDownload->parent.iError = AeFILE_CHECKSUM_ERROR;
        return AeFalse;
    }

    /* indicate end of tar/gzip stream */
    (*pDownload->pFileFilterHandler->pWriteRaw)(pDownload->pFileFilter, NULL, 0);

    /* now try to process what we just received */
    return AeFileDownloadDecode(pDownload);
}

/***************************************************************************/
static AeBool AeFileDownloadOpenFile(AeFileDownload *pDownload)
{
    AeInt32 iLen;
    AeChar *pName, *ptr;
    AeChar zBackupName[256];
    AeBool bOK;

    bOK = AeTrue;
    iLen = 0;

    /* get rid of trailing PATH_CHAR in the destination path */
    if (pDownload->pDestPath && pDownload->pDestPath[0] != 0)
    {
        iLen = strlen(pDownload->pDestPath);
        if (pDownload->pDestPath[iLen - 1] == '/' || pDownload->pDestPath[iLen - 1] == '\\')
            pDownload->pDestPath[iLen - 1] = 0;
    }

    iLen += strlen(pDownload->pCurFileStat->pName) + 2;
    pName = AeAlloc(iLen);
    if (!pName)
        return AeFalse;

    /* format full path */
    if (pDownload->pDestPath && pDownload->pDestPath[0] != 0)
        snprintf(pName, iLen, "%s%c%s", pDownload->pDestPath, PATH_CHAR,
                 pDownload->pCurFileStat->pName);
    else
        strcpy(pName, pDownload->pCurFileStat->pName);

    /* convert the destination directory to platform specific format */
    AeConvertPath(pName);

    /* back up the file */
    if(pDownload->iMode == AeFileDownloadModeUpgrade)
    {
        strcpy(zBackupName, AXEDA_BACKUP);
        strcat(zBackupName, "\\");
        strcat(zBackupName, pName);
        AeConvertPath(zBackupName);

        ptr = strrchr(zBackupName, PATH_CHAR);
        if (ptr && ptr != zBackupName)
        {
            *ptr = 0;
            bOK = AeMakeDirHierarchy(zBackupName);
            *ptr = PATH_CHAR;
        }

        AeFileDelete(zBackupName);
        AeFileMove(pName, zBackupName);
    }


    /* create directory hierarchy: here we temporarily make directory
     * name out of the full path */ 
    ptr = strrchr(pName, PATH_CHAR);
    if (ptr && ptr != pName)
    {
        *ptr = 0;
        bOK = AeMakeDirHierarchy(pName);
        *ptr = PATH_CHAR;
    }

    /* create file */
    if (bOK)
    {
        pDownload->iCurFileHandle =
            AeFileOpen(pName, AE_OPEN_WRITE_ONLY | AE_OPEN_CREATE | AE_OPEN_TRUNCATE);
        bOK = (pDownload->iCurFileHandle != AeFileInvalidHandle);
    }

    if (!bOK)
    {
        if (g_drmSettings.iLogLevel >= AeLogError)
            AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_DOWNLOAD,
                "Download (id=%s): could not create file: %s",
                pDownload->pId, pName);

        pDownload->parent.iError = AeFAILED;
    }

    AeFree(pName);

    return bOK;
}

/***************************************************************************/
static AeBool AeFileDownloadWriteFile(AeFileDownload *pDownload, AeChar *pData, AeInt32 iSize)
{
    AeInt32 iWritten;

    iWritten = AeFileWrite(pDownload->iCurFileHandle, pData, iSize);
    if (iWritten != iSize)
    {
        pDownload->parent.iError = AeFAILED;
        return AeFalse;
    }

    return AeTrue;
}

/**************************************************************************/
static AeBool AeFileDownloadIsFatalError(AeFileTransferError iError)
{
    /* FIXME: handle more errors */
    if (iError == AeFAILED)
        return AeTrue;

    return AeFalse;
}

/***************************************************************************/
static AeWebRequest *AeFileDownloadRequestNew(AeFileDownload *pDownload)
{
    AeWebRequest *pRequest;
    AeDRMDevice *pDevice;
    AeDRMServerConfig *pConfig;
    AeChar pURL[256];

    pDevice = AE_DRM_DEVICE(pDownload->parent.pPackage->iDeviceId);
    pConfig = AE_DRM_CONFIG(pDownload->parent.pPackage->iConfigId);
    if (!pDevice || !pConfig)
        return NULL;

    snprintf(pURL, sizeof(pURL), AE_FILE_DOWNLOAD_URL_FORMAT, 
             pDownload->pURL, pDevice->pModelNumber, pDevice->pSerialNumber,
             pConfig->pOwner, pDownload->pSessionId, pDownload->iDownloadPos,
             pDownload->iChunkSize);

    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_NETWORK | AE_LOG_CATEGORY_FILE_DOWNLOAD,
            "Download (id=%s): URL: %s",
            pDownload->pId, pURL);

    pRequest = AeWebRequestNew();
    if (!pRequest)
        return NULL;

    AeWebRequestSetMethod(pRequest, "GET");
    if (!AeWebRequestSetURL(pRequest, pURL))
    {
        AeWebRequestDestroy(pRequest);
        return NULL;
    }

    AeWebRequestSetOnError(pRequest, AeFileDownloadRequestOnError);
    AeWebRequestSetOnEntity(pRequest, AeFileDownloadRequestOnEntity);
    AeWebRequestSetOnCompleted(pRequest, AeFileDownloadRequestOnCompleted);
    AeWebRequestSetOnResponse(pRequest, AeFileDownloadRequestOnResponse);

    AeWebRequestSetUserData(pRequest, pDownload);

    return pRequest;
}

/***************************************************************************/
static void AeFileDownloadRequestOnError(AeWebRequest *pRequest, AeError iError)
{
    AeFileDownload *pDownload;
    
    pDownload = (AeFileDownload *) pRequest->pUserData;
    pDownload->parent.iError = AeFileTransferTranslateError(iError);

    if (g_drmSettings.iLogLevel >= AeLogError)
        AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_NETWORK | AE_LOG_CATEGORY_FILE_DOWNLOAD,
            "Download (id=%s): %s: %s",
            pDownload->pId, pDownload->pURL, AE_ERROR_STRING(iError));

    AeWebRequestDestroy(pRequest);
    pDownload->pRequest = NULL;
}

/***************************************************************************/
static AeBool AeFileDownloadRequestOnEntity(AeWebRequest *pRequest, AeInt32 iDataOffset, AeChar *pData, AeInt32 iSize)
{
    AeFileDownload *pDownload;
    
    pDownload = (AeFileDownload *) pRequest->pUserData;

    /* process beginning of the download */
    if (pDownload->iDownloadPos == 0)
    {
        /* call user callback (if any) and see if it requests
         * custom file processing */
        if (g_drmCallbacks.pOnFileDownloadBegin)
            pDownload->bUserCallback =
                (*g_drmCallbacks.pOnFileDownloadBegin)(pDownload->parent.pPackage->iDeviceId,
                                                       &pDownload->pUserData);
        else
            pDownload->bUserCallback = AeFalse;
    }

    /* write raw data to tar/gzip filter */
    (*pDownload->pFileFilterHandler->pWriteRaw)(pDownload->pFileFilter, pData, iSize);

    /* now try to process what we just received */
    if (!AeFileDownloadDecode(pDownload))
    {
        AeWebRequestDestroy(pRequest);
        pDownload->pRequest = NULL;

        return AeFalse;    
    }

    /* update md5 digests */
    MD5_Update(&pDownload->md5Global, (AeUInt8 *) pData, iSize);
    MD5_Update(&pDownload->md5Request, (AeUInt8 *) pData, iSize);
    pDownload->iDownloadPos += iSize;

    return AeTrue;
}

/***************************************************************************/
static void AeFileDownloadRequestOnCompleted(AeWebRequest *pRequest)
{
    AeFileDownload *pDownload;
    AeUInt8 pCheckSum[MD5_DIGEST_LENGTH], pDigest[MD5_DIGEST_LENGTH];
    AeChar *pTmp;

    pDownload = (AeFileDownload *) pRequest->pUserData;
    MD5_Final(pDigest, &pDownload->md5Request);

    /* check request md5 digest */
    pTmp = AeWebRequestGetResponseHeader(pRequest, HTTP_CONTENT_MD5);
    if (pTmp && strlen(pTmp) == 24)
    {
        AeBase64Decode(pTmp, strlen(pTmp), (AeChar *) pCheckSum);
        if (memcmp(pCheckSum, pDigest, MD5_DIGEST_LENGTH))
            pDownload->parent.iError = AeCHUNK_CHECKSUM_ERROR;
    }

    AeWebRequestDestroy(pRequest);
    pDownload->pRequest = NULL;
}

/***************************************************************************/
static AeBool AeFileDownloadRequestOnResponse(AeWebRequest *pRequest, AeInt iStatusCode)
{
    AeFileDownload *pDownload;
    
    if (iStatusCode == HTTP_STATUS_OK)
        return AeTrue;

    pDownload = (AeFileDownload *) pRequest->pUserData;
    pDownload->parent.iError = AeHTTP_STATUS_ERROR;

    if (g_drmSettings.iLogLevel >= AeLogError)
        AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_NETWORK | AE_LOG_CATEGORY_FILE_DOWNLOAD,
            "Download (id=%s): HTTP status: %d", pDownload->pId, iStatusCode);

    AeWebRequestDestroy(pRequest);
    pDownload->pRequest = NULL;

    return AeFalse;
}

/**************************************************************************/
static AeBool AeFileDownloadReadDownloads(AeFileDownload *pDownload, AeHandle pParameters)
{
    AeHandle *pChild, *pGrandChild;

    pChild = AeDRMSOAPGetParameterFirstChild(pParameters);
    while (pChild)
    {
        AeFileDownloadSpec *pDownloadSpec;
        AeChar *pTmp;

        /* look for "FileList" element */
        if (!strcasecmp(AeDRMSOAPGetParameterName(pChild), AE_FILETRANSFER_FILE_LIST))
        {
            pGrandChild = AeDRMSOAPGetParameterFirstChild(pChild);
            while (pGrandChild)
            {
                /* look for "File" element with "name" attribute */
                if (!strcasecmp(AeDRMSOAPGetParameterName(pGrandChild), AE_FILETRANSFER_FILE) &&
                    (pTmp = AeDRMSOAPGetAttributeValueByName(pGrandChild, AE_FILETRANSFER_NAME)))
                {
                    pDownloadSpec = AeFileDownloadSpecNew();
                    if (!pDownloadSpec)
                    {
                        pDownload->parent.iError = AeFAILED;
                        break;
                    }
        
                    AeSetString(&pDownloadSpec->pName, pTmp , -1);

                    AeListAppend(pDownload->pFileList, pDownloadSpec);

                    if (g_drmSettings.iLogLevel >= AeLogDebug)
                        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_DOWNLOAD,
                            "Dowload (id=%s): added file specification: file=%s",
                            pDownload->pId, pDownloadSpec->pName);
                }

                pGrandChild = AeDRMSOAPGetNextParameter(pGrandChild);
            }
        }

        pChild = AeDRMSOAPGetNextParameter(pChild);
    }

    if (pDownload->parent.iError == AeNOERROR)
    {
        if (AePtrArrayCount(pDownload->pFileList) == 0)
        {
            if (g_drmSettings.iLogLevel >= AeLogError)
                AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_DOWNLOAD,
                    "Download (id=%s): empty file specification list",
                    pDownload->pId);

            pDownload->parent.iError = AeBAD_FORMAT;
        }
    }

    if (pDownload->parent.iError != AeNOERROR)
        return AeFalse;

    return AeTrue;
}

/**************************************************************************/
static AeFileDownloadSpec *AeFileDownloadSpecNew(void)
{
    AeFileDownloadSpec *pDownloadSpec = (AeFileDownloadSpec *) AeAlloc(sizeof(AeFileDownloadSpec));
    if (!pDownloadSpec)
        return NULL;
    memset(pDownloadSpec, 0, sizeof(AeFileDownloadSpec));

    return pDownloadSpec;
}

/**************************************************************************/
static void AeFileDownloadSpecDestroy(AeFileDownloadSpec *pDownloadSpec)
{
    AeFree(pDownloadSpec->pName);
    AeFree(pDownloadSpec);
}

#endif /* defined(ENABLE_FILE_TRANSFER) */
