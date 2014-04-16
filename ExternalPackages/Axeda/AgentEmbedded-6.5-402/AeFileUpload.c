/* $Id: AeFileUpload.c,v 1.21.2.1 2009/12/01 18:04:24 hfeng Exp $ */

#include "Ae.h"

#if defined(ENABLE_FILE_TRANSFER)

static AeBool AeFileUploadInitialize(AeFileUpload *pUpload, AeHandle pParameters);
static void AeFileUploadProcess(AeFileUpload *pUpload);
static void AeFileUploadDestroy(AeFileUpload *pUpload);
static void AeFileUploadNotify(AeFileUpload *pUpload, AeFileTransferEvent iEvent);
static AeBool AeFileUploadIsIdle(AeFileUpload *pUpload);
static AeBool AeFileUploadIsPendingRequest(AeFileUpload *pUpload);

static AeBool AeFileUploadReadUploads(AeFileUpload *pUpload, AeHandle pParameters);
static AeBool AeFileUploadEncode(AeFileUpload *pUpload);
static AeBool AeFileUploadIsFatalError(AeFileTransferError iError);

static AeFileUploadSource *AeFileUploadSourceNew(AeFileUploadSpec **ppUploads);
static AeBool AeFileUploadSourceRead(AeFileUploadSource *pSource, AeFileStat **ppFile, AeChar **ppData, AeInt32 *piSize);
static void AeFileUploadSourceDestroy(AeFileUploadSource *pSource);

static AeWebRequest *AeFileUploadRequestNew(AeFileUpload *pUpload);

/* callbacks for upload HTTP request */
static void AeFileUploadRequestOnError(AeWebRequest *pRequest, AeError iError);
static AeBool AeFileUploadRequestOnEntity(AeWebRequest *pRequest, AeInt32 iDataOffset, AeChar *pData, AeInt32 iSize);
static void AeFileUploadRequestOnCompleted(AeWebRequest *pRequest);
static AeBool AeFileUploadRequestOnResponse(AeWebRequest *pRequest, AeInt iStatusCode);

static AeFileUploadSpec *AeFileUploadSpecNew(void);
static void AeFileUploadSpecDestroy(AeFileUploadSpec *pUploadSpec);

static AeFileInstructionVTable g_fileUploadVTable =
{
    (AeBool (*)(AeFileInstruction *, AeHandle)) AeFileUploadInitialize,
    (void (*)(AeFileInstruction *)) AeFileUploadProcess,
    (void (*)(AeFileInstruction *)) AeFileUploadDestroy,
    (void (*)(AeFileInstruction *, AeFileTransferEvent)) AeFileUploadNotify,
    (AeBool (*)(AeFileInstruction *)) AeFileUploadIsIdle,
    (AeBool (*)(AeFileInstruction *)) AeFileUploadIsPendingRequest
};

/**************************************************************************/
AeFileUpload *AeFileUploadNew(void)
{
    AeTimeValue zeroTime;
    AeFileUpload *pUpload = (AeFileUpload *) AeAlloc(sizeof(AeFileUpload));
    if (!pUpload)
        return NULL;
    memset(pUpload, 0, sizeof(AeFileUpload));
    zeroTime.iSec = 0;
    zeroTime.iMicroSec = 0;

    pUpload->parent.pVTable = &g_fileUploadVTable;
    pUpload->parent.iState = AeFileInstructionInitialized;
    pUpload->parent.iError = AeNOERROR;

    pUpload->pURL = NULL;
    pUpload->pSessionId = NULL;
    pUpload->pId = NULL;
    pUpload->iMaxRetryCount = 0;
    pUpload->iRetryMinDelay = 0;
    pUpload->iRetryMaxDelay = 0;
    pUpload->iChunkSize = AE_BUFFER_CHUNK;
    pUpload->iChunkDelay = 0;
    pUpload->iStartPos = 0;
    pUpload->iCompression = AeFileCompressionTarGzip;
    pUpload->bNoOverallChecksum = AeFalse;
    pUpload->bMultiFile = AeFalse;
    pUpload->ppUploads = NULL;

    memset(&pUpload->md5Global, 0, sizeof(pUpload->md5Global));
    pUpload->pFileFilter = NULL;
    pUpload->pFileFilterHandler = NULL;
    pUpload->pRequest = NULL;
    pUpload->iUploadPos = 0;
    pUpload->iCurFilePos = 0;
    pUpload->iRetryCount = 0;
    pUpload->pEncodedData = NULL;
    pUpload->iEncodedSize = 0;
    pUpload->bUserCallback = AeFalse;
    pUpload->pUserData = NULL;
    pUpload->pSource = NULL;
    pUpload->bDeferred = AeFalse;

    pUpload->lastProcessed = zeroTime;
    pUpload->retryWait = zeroTime;

    return pUpload;
}

/**************************************************************************/
void AeFileUploadDestroy(AeFileUpload *pUpload)
{
    if (pUpload->pURL)
        AeFree(pUpload->pURL);
    if (pUpload->pSessionId)
        AeFree(pUpload->pSessionId);
    if (pUpload->pId)
        AeFree(pUpload->pId);
    if (pUpload->param.pId)
        AeFree(pUpload->param.pId);
    if (pUpload->ppUploads)
    {
        AeInt32 i;

        i = 0;
        while (pUpload->ppUploads[i])
        {
            AeFileUploadSpecDestroy(pUpload->ppUploads[i]);
            i += 1;
        }

        AeFree(pUpload->ppUploads);
    }
    if (pUpload->pFileFilter && pUpload->pFileFilterHandler)
        (*pUpload->pFileFilterHandler->pDestroy)(pUpload->pFileFilter);
    if (pUpload->pRequest)
        AeWebRequestDestroy(pUpload->pRequest);
    if (pUpload->curFileStat.pName)
        AeFree(pUpload->curFileStat.pName);
    if (pUpload->pEncodedData)
        AeFree(pUpload->pEncodedData);
    if (pUpload->pSource)
        AeFileUploadSourceDestroy(pUpload->pSource);

    AeFree(pUpload);
}

/**************************************************************************/
static void AeFileUploadNotify(AeFileUpload *pUpload, AeFileTransferEvent iEvent)
{
    if (g_drmCallbacks.pOnFileTransferEvent)
        (*g_drmCallbacks.pOnFileTransferEvent)(pUpload->parent.pPackage->iDeviceId,
            iEvent, pUpload->pUserData);
}

/**************************************************************************/
static AeBool AeFileUploadIsIdle(AeFileUpload *pUpload)
{
    return pUpload->bDeferred;
}

/***************************************************************************/
static AeBool AeFileUploadIsPendingRequest(AeFileUpload *pUpload)
{
    return (pUpload->pRequest != NULL);
}

/**************************************************************************/
static AeBool AeFileUploadInitialize(AeFileUpload *pUpload, AeHandle pParameters)
{
    AeChar *pTmp;

    AeSetString(&pUpload->pURL,
                AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_URL), -1);

    AeSetString(&pUpload->pSessionId,
                AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_SESSION_ID), -1);
 
    AeSetString(&pUpload->pId,
                AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_PUB_ID), -1);

    if (!pUpload->pURL || !pUpload->pSessionId)
    {
        pUpload->parent.iState = AeFileInstructionFinished;
        pUpload->parent.iError = AeBAD_FORMAT;
        return AeFalse;
    }

    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_RETRY_COUNT);
    if (pTmp)
        pUpload->iMaxRetryCount = strtol(pTmp, NULL, 10);

    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_MIN_DELAY);
    if (pTmp)
        pUpload->iRetryMinDelay = strtol(pTmp, NULL, 10);

    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_MAX_DELAY);
    if (pTmp)
        pUpload->iRetryMaxDelay = strtol(pTmp, NULL, 10);

    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_CHUNK_SIZE);
	if (pTmp)
        pUpload->iChunkSize = strtol(pTmp, NULL, 10);


    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_CHUNK_DELAY);
    if (pTmp)
        pUpload->iChunkDelay = strtol(pTmp, NULL, 10);

    /* starting position. this is valid for uncompressed upload only. */
    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_POSITION);
    if (pTmp)
        pUpload->iStartPos =
#ifndef ENABLE_LARGEFILE64
            strtol(pTmp, NULL, 10);
#else
            AeStrToInt64(pTmp, NULL, 10);
#endif

    /* priority: this is treated by us as a package property */
    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_PRIORITY);
    if (pTmp)
        AeFileTransferPackageSetPriority(pUpload->parent.pPackage, strtol(pTmp, NULL, 10));

    /* compression type */
    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_COMPRESSION);
    if (pTmp && !strcasecmp(pTmp, "none"))
    {
        /* initialize null filter */
        if ((pUpload->pFileFilter = AeNullFileFilterNew(AeTrue)) &&
            AeNullFileFilterInitialize((AeNullFileFilter *) pUpload->pFileFilter, NULL))
        {
            pUpload->pFileFilterHandler = &g_nullFileFilterHandler;
			((AeNullFileFilter *)pUpload->pFileFilter)->iWriteCount = pUpload->iStartPos;
            pUpload->iCompression = AeFileCompressionNone;
        }
    }
    else if (!pTmp || !strcasecmp(pTmp, "tar-gzip"))
    {
        /* initialize tar/gzip filter */
        if ((pUpload->pFileFilter = AeTarNew(AeTrue, &g_gzipFilterHandler)) &&
            AeTarInitialize((AeTar *) pUpload->pFileFilter))
        {
            pUpload->pFileFilterHandler = &g_tarFileFilterHandler;
            pUpload->iCompression = AeFileCompressionTarGzip;
        }
    }
    else
    {
        if (g_drmSettings.iLogLevel >= AeLogError)
            AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_UPLOAD,
                "Upload (id=%s): unrecognized compression type: %s",
                pUpload->pId, pTmp);
    }

    if (!pUpload->pFileFilter || !pUpload->pFileFilterHandler)
    {
        pUpload->parent.iState = AeFileInstructionFinished;
        pUpload->parent.iError = AeFAILED;
        return AeFalse;
    }

    /* client id */
    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_CLIENT_ID);
    if (pTmp)
    {
        pUpload->param.iMask |= AE_FILE_UPLOAD_EXECUTE_PARAM_ID;
        AeSetString(&pUpload->param.pId, pTmp, -1);
    }
    
    /* requireOverallMD5 flag (on by default) */
    pTmp = AeDRMSOAPGetAttributeValueByName(pParameters, AE_FILETRANSFER_REQUIRE_OVERALL_MD5);
    if (pTmp && !strcasecmp(pTmp, "false"))
        pUpload->bNoOverallChecksum = AeTrue;
    else
        pUpload->bNoOverallChecksum = AeFalse;

    /* Now read the upload file list */
    if (!AeFileUploadReadUploads(pUpload, pParameters))
    {
        if (g_drmSettings.iLogLevel >= AeLogError)
            AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_UPLOAD,
                "Upload (id=%s): error processing file specifications",
                pUpload->pId);

        pUpload->parent.iState = AeFileInstructionFinished;
        return AeFalse;
    }

    /* specify start position in case of uncompressed upload */
    if (pUpload->iStartPos != 0)
    {
        if (pUpload->iCompression != AeFileCompressionNone)
        {
            if (g_drmSettings.iLogLevel >= AeLogError)
                AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_UPLOAD,
                    "Upload (id=%s): non-zero start position requested for compressed upload",
                    pUpload->pId);

            pUpload->parent.iState = AeFileInstructionFinished;
            pUpload->parent.iError = AeBAD_FORMAT;
            return AeFalse;
        }

        pUpload->ppUploads[0]->iPosition = pUpload->iStartPos;
        pUpload->iUploadPos = pUpload->iStartPos;
		pUpload->iCurFilePos = pUpload->iStartPos;

    }
    
    /* initialize global md5 digest */
    if (!pUpload->bNoOverallChecksum)
        MD5_Init(&pUpload->md5Global);

    /* allocate buffer for encoded data */
    if (!(pUpload->pEncodedData = (AeChar *) AeAlloc(pUpload->iChunkSize)))
    {
        pUpload->parent.iState = AeFileInstructionFinished;
        pUpload->parent.iError = AeFAILED;
        return AeFalse;
    }

    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_UPLOAD,
            "Upload (id=%s): initialized: package=%s, clientId=%s, compression=%s, position=%" AE_LONGINT_FORMAT_MODIFIER "d",
            pUpload->pId, pUpload->parent.pPackage->pJobId,
            (pUpload->param.iMask & AE_FILE_UPLOAD_EXECUTE_PARAM_ID) ? pUpload->param.pId : "(none)",
            g_pCompressionStrings[pUpload->iCompression],
            pUpload->iStartPos);

    return AeTrue;
}

/**************************************************************************/
static void AeFileUploadProcess(AeFileUpload *pUpload)
{
    AeInt randVal;
    AeUInt32 randWait;
	AeTimeValue nextCycleBeginTime, currentTime;

    if (pUpload->parent.iState == AeFileInstructionInitialized)
    {
        /* call user callback (if any) and see if it intercepts the upload */
        if (g_drmCallbacks.pOnFileUploadBeginEx)
            pUpload->bUserCallback =
                (*g_drmCallbacks.pOnFileUploadBeginEx)(pUpload->parent.pPackage->iDeviceId,
                                                       pUpload->ppUploads,
                                                       &pUpload->param,
                                                       &pUpload->pUserData);
        else if (g_drmCallbacks.pOnFileUploadBegin)
            pUpload->bUserCallback =
                (*g_drmCallbacks.pOnFileUploadBegin)(pUpload->parent.pPackage->iDeviceId,
                                                     pUpload->ppUploads,
                                                     &pUpload->pUserData);
        else
            pUpload->bUserCallback = AeFalse;

        /* initialize internal upload handler when upload is not intercepted */
        if (pUpload->bUserCallback == AeFalse)
        {
            pUpload->bUserCallback = AeFalse;
            pUpload->pSource = AeFileUploadSourceNew(pUpload->ppUploads);
            if (!pUpload->pSource)
                pUpload->parent.iError = AeFAILED;
        }

        if (g_drmSettings.iLogLevel >= AeLogInfo)
            AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_FILE_UPLOAD,
                "Upload (id=%s): started", pUpload->pId);

        pUpload->parent.iState = AeFileInstructionExecuting;
    }

    if (pUpload->parent.iState == AeFileInstructionExecuting)
    {
        /* web request in progress: check back later */
        if (pUpload->pRequest)
            return;

        if (pUpload->parent.iError == AeNOERROR)
        {
            /* check if upload is complete */
            if ((*pUpload->pFileFilterHandler->pIsEndOfStream)(pUpload->pFileFilter))
            {
                /* notify user about success */
                if (pUpload->bUserCallback)
                    (*g_drmCallbacks.pOnFileUploadEnd)(pUpload->parent.pPackage->iDeviceId,
                                                       AeTrue, pUpload->pUserData);

                if (g_drmSettings.iLogLevel >= AeLogInfo)
                    AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_FILE_UPLOAD,
                        "Upload (id=%s): success (size: %" AE_LONGINT_FORMAT_MODIFIER "d)",
                        pUpload->pId, pUpload->iUploadPos);

                pUpload->parent.iState = AeFileInstructionFinished;
                return;
            }

            /* encode next chunk */
            AeFileUploadEncode(pUpload);
            if (pUpload->bDeferred)
                return;
        }

        /* check for errors */
        if (pUpload->parent.iError != AeNOERROR)
        {
            if (AeFileUploadIsFatalError(pUpload->parent.iError) ||
                pUpload->iRetryCount >= pUpload->iMaxRetryCount)
            {
                if (g_drmSettings.iLogLevel >= AeLogError)
                    AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_UPLOAD,
                        "Upload (id=%s): failed with code: %d",
                        pUpload->pId, pUpload->parent.iError);

                /* notify the user about the error */
                if (pUpload->bUserCallback)
                    (*g_drmCallbacks.pOnFileUploadEnd)(pUpload->parent.pPackage->iDeviceId,
                                                       AeFalse, pUpload->pUserData);

                pUpload->parent.iState = AeFileInstructionFinished;
                return;
            }

            if (pUpload->iRetryCount > 0 )
            {
                AeGetElapsedTime(&currentTime);
                AE_TIME_VALUE_ADD(nextCycleBeginTime, pUpload->lastProcessed, pUpload->retryWait);
                if (AE_TIME_VALUE_LESS_THAN(currentTime, nextCycleBeginTime))
                    return;
            }

            pUpload->iRetryCount += 1;

            if (g_drmSettings.iLogLevel >= AeLogInfo)
                AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_FILE_UPLOAD,
                    "Upload (id=%s): retry %d/%d from position: %" AE_LONGINT_FORMAT_MODIFIER "d",
                    pUpload->pId, pUpload->iRetryCount, pUpload->iMaxRetryCount,
                    pUpload->iUploadPos);

            /* wait for a random number of milliseconds before retrying */
            AeGetElapsedTime(&pUpload->lastProcessed);
            randVal = (AeUInt32)((AeDouble)AeRand() / RAND_MAX * (pUpload->iRetryMaxDelay - pUpload->iRetryMinDelay));
            randWait = randVal + pUpload->iRetryMinDelay;
		    pUpload->retryWait.iSec = randWait / 1000;
            pUpload->retryWait.iMicroSec = (randWait % 1000) * 1000;

            pUpload->parent.iError = AeNOERROR;
        }
        else
            pUpload->iRetryCount = 0;

        /* create new web request */
        pUpload->pRequest = AeFileUploadRequestNew(pUpload);
        if (!pUpload->pRequest)
        {
            pUpload->parent.iState = AeFileInstructionFinished;
            pUpload->parent.iError = AeFAILED;
            return;
        }

        /* submit web request */
        AeWebUserAgentAsyncExecute(g_pWebUA, &pUpload->pRequest, 1);
    }
}

/**************************************************************************/
static AeBool AeFileUploadReadUploads(AeFileUpload *pUpload, AeHandle pParameters)
{
    AePtrArray *pUploads;
    AeHandle *pChild;

    pUploads = AePtrArrayNew(NULL, NULL);
    if (!pUploads)
    {
        pUpload->parent.iError = AeFAILED;
        return AeFalse;
    }

    pChild = AeDRMSOAPGetParameterFirstChild(pParameters);
    while (pChild)
    {
        AeFileUploadSpec *pUploadSpec;
        AeChar *pTmp;

        /* look for "File" element with "name" attribute */
        if (!strcasecmp(AeDRMSOAPGetParameterName(pChild), AE_FILETRANSFER_FILE) &&
            (pTmp = AeDRMSOAPGetAttributeValueByName(pChild, AE_FILETRANSFER_NAME)))
        {
            pUploadSpec = AeFileUploadSpecNew();
            if (!pUploadSpec)
            {
                pUpload->parent.iError = AeFAILED;
                break;
            }
        
            AeSetString(&pUploadSpec->pName, pTmp , -1);

            /* "delete" attribute */
            pTmp = AeDRMSOAPGetAttributeValueByName(pChild, AE_FILETRANSFER_DELETE);
            if (pTmp && !strcasecmp(pTmp, "true"))
                pUploadSpec->bDelete = AeTrue;

            AePtrArrayAppend(pUploads, pUploadSpec);

            if (g_drmSettings.iLogLevel >= AeLogDebug)
                AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_UPLOAD,
                    "Upload (id=%s): added file specification: file=%s, delete=%s",
                    pUpload->pId, pUploadSpec->pName, pUploadSpec->bDelete ? "true" : "false");
        }

        pChild = AeDRMSOAPGetNextParameter(pChild);
    }

    if (pUpload->parent.iError == AeNOERROR)
    {
        if (AePtrArrayCount(pUploads) > 0)
        {
            if (AePtrArrayCount(pUploads) > 1)
                pUpload->bMultiFile = AeTrue;
            
            /* multiple files may not be requested with uncompressed option */
            if (pUpload->iCompression == AeFileCompressionNone &&
                pUpload->bMultiFile)
            {
                if (g_drmSettings.iLogLevel >= AeLogError)
                    AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_UPLOAD,
                        "Upload (id=%s): multiple files requested for uncompressed upload",
                        pUpload->pId);

                pUpload->parent.iError = AeMULTIFILE_UNCOMPRESSED;
            }
            else
            {
                pUpload->ppUploads = (AeFileUploadSpec **)
                    AeAlloc((AePtrArrayCount(pUploads) + 1) * sizeof(AePointer));
                if (pUpload->ppUploads)
                {
                    /* copy pointers and zero-terminate */
                    memcpy(pUpload->ppUploads, pUploads->pData,
                           AePtrArrayCount(pUploads) * sizeof(AePointer));
                    pUpload->ppUploads[AePtrArrayCount(pUploads)] = NULL;
                }
            }
        }
        else
        {
            if (g_drmSettings.iLogLevel >= AeLogError)
                AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_UPLOAD,
                    "Upload (id=%s): empty file specification list",
                    pUpload->pId);

            pUpload->parent.iError = AeBAD_FORMAT;
        }
    }

    AePtrArrayDestroy(pUploads);

    if (pUpload->parent.iError != AeNOERROR)
        return AeFalse;
    
    return AeTrue;
}

/**************************************************************************/
static AeBool AeFileUploadEncode(AeFileUpload *pUpload)
{
    AeFileStat *pFileStat;
    AeChar *pData;
    AeInt32 iSize;
    AeBool bOK;

    if (!pUpload->bDeferred)
        pUpload->iEncodedSize = 0;
    else
        pUpload->bDeferred = AeFalse;

    /* encode until full chunk is generated or end-of-stream */
    while (pUpload->iEncodedSize < pUpload->iChunkSize)
    {
        /* try to read available encoded data (resulted from us writing to
           the other end of the filter) */
        iSize = (*pUpload->pFileFilterHandler->pReadRaw)(pUpload->pFileFilter,
            pUpload->pEncodedData + pUpload->iEncodedSize,
            pUpload->iChunkSize - pUpload->iEncodedSize);

        /* encoded data is available. just indicate how much. */
        if (iSize > 0)
            pUpload->iEncodedSize += iSize;
        
        /* encoded data is not available. encode some. */
        else if (iSize == 0)
        {
            /* check for end-of-stream */
            if ((*pUpload->pFileFilterHandler->pIsEndOfStream)(pUpload->pFileFilter))
                break;

            pFileStat = NULL;
            pData = NULL;
            iSize = 0;

            /* read file data: from application or through the internal handler */
            if (pUpload->bUserCallback)
                bOK = (*g_drmCallbacks.pOnFileUploadData)(pUpload->parent.pPackage->iDeviceId,
                                                          &pFileStat, &pData, &iSize,
                                                          pUpload->pUserData);
            else
                bOK = AeFileUploadSourceRead(pUpload->pSource, &pFileStat, &pData, &iSize);

            /* bail out on read failure */
            if (!bOK)
            {
                pUpload->parent.iError = AeSOME_FILES_NOT_FOUND;
                break;
            }

            /* end-of-stream was indicated by read operation. do the same with
               the file filter. */
            if (!pFileStat)
            {
                if (!(*pUpload->pFileFilterHandler->pWriteFile)(pUpload->pFileFilter, NULL))
                    break;
            }
            
            /* application indicates that file content is temporarily unavailable */
            else if (!pFileStat->pName ||
                     (pFileStat->iType == AeFileTypeRegular && (!pData || iSize == 0)))
            {
                pUpload->bDeferred = AeTrue;
                return AeTrue;
            }

            else
            {
                /* start new file */
                if (!pUpload->curFileStat.pName ||
                    strcmp(pUpload->curFileStat.pName, pFileStat->pName) != 0)
                {
                    pUpload->iCurFilePos = pUpload->iStartPos;

                    /* store returned file descriptor */
                    AeFileStatCopy(&pUpload->curFileStat, pFileStat);

                    if (!(*pUpload->pFileFilterHandler->pWriteFile)(pUpload->pFileFilter, pFileStat))
                        break;
                }
                else
                {
                    /* for uncompressed upload file size is allowed to change,
                       so keep internal structures updated */
                    if (pUpload->iCompression == AeFileCompressionNone)
                    {
                        ((AeNullFileFilter *) pUpload->pFileFilter)->pFile->iSize = pFileStat->iSize;
                        pUpload->curFileStat.iSize = pFileStat->iSize;
                    }
                }

                /* write file data if current file position does not exceed file size */
                if (pFileStat->iType == AeFileTypeRegular &&
                    pUpload->iCurFilePos < pUpload->curFileStat.iSize)
                {
                    if ((*pUpload->pFileFilterHandler->pWriteFileData)(pUpload->pFileFilter, pData, iSize) < 0)
                        break;

                    pUpload->iCurFilePos += iSize;
                }
            }
        }
    }

    if ((*pUpload->pFileFilterHandler->pGetError)(pUpload->pFileFilter) != 0)
    {
        if (g_drmSettings.iLogLevel >= AeLogError)
            AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_UPLOAD,
                "Upload (id=%s): content encoding error: %s",
                pUpload->pId,
                (*pUpload->pFileFilterHandler->pGetErrorString)(pUpload->pFileFilter,
                    (*pUpload->pFileFilterHandler->pGetError)(pUpload->pFileFilter)));

        pUpload->parent.iError = AeARCHIVE_ERROR;
    }

    if (pUpload->parent.iError != AeNOERROR)
        return AeFalse;

    /* update the overall digest */
    if (!pUpload->bNoOverallChecksum)
        MD5_Update(&pUpload->md5Global, (AeUInt8 *) pUpload->pEncodedData,
                   pUpload->iEncodedSize);

    return AeTrue;
}

/**************************************************************************/
static AeBool AeFileUploadIsFatalError(AeFileTransferError iError)
{
    /* FIXME: handle more errors */
    if (iError == AeFAILED ||
        iError == AeARCHIVE_ERROR ||
        iError == AeFILE_CHECKSUM_ERROR ||
        iError == AeNO_FILES_FOUND ||
        iError == AeSOME_FILES_NOT_FOUND)
        return AeTrue;

    return AeFalse;
}

/***************************************************************************/
static AeFileUploadSource *AeFileUploadSourceNew(AeFileUploadSpec **ppUploads)
{
    AeFileUploadSource *pSource;

    /* allocate upload handler */
    pSource = (AeFileUploadSource *) AeAlloc(sizeof(AeFileUploadSource));
    if (!pSource)
        return NULL;

    /* initialize */
    memset(pSource, 0, sizeof(AeFileUploadSource));
    pSource->ppUploads = ppUploads;
    pSource->iUploadIdx = 0;
    pSource->iCurFileHandle = AeFileInvalidHandle;

    return pSource;
}

/***************************************************************************/
static AeBool AeFileUploadSourceRead(AeFileUploadSource *pSource, AeFileStat **ppFile, AeChar **ppData, AeInt32 *piSize)
{
    *ppFile = NULL;
    *ppData = NULL;
    *piSize = 0;

    /* no more files to upload: indicate that */
    if (!pSource->ppUploads[pSource->iUploadIdx])
        return AeTrue;

    /* initialize next file */
    if (pSource->iCurFileHandle == AeFileInvalidHandle)
    {
        /* open file */
        pSource->iCurFileHandle = AeFileOpen(pSource->ppUploads[pSource->iUploadIdx]->pName,
                                             AE_OPEN_READ_ONLY);
        if (pSource->iCurFileHandle == AeFileInvalidHandle)
            return AeFalse;

        pSource->curFileStat.pName = pSource->ppUploads[pSource->iUploadIdx]->pName;
        pSource->curFileStat.iType = AeFileTypeRegular;
        pSource->curFileStat.iSize =
#ifndef ENABLE_LARGEFILE64
            AeFileGetSize
#else
            AeFileGetSize64
#endif
            (pSource->ppUploads[pSource->iUploadIdx]->pName);
        pSource->curFileStat.iMTime = 0;
    }

    *ppFile = &pSource->curFileStat;

    /* try to read another portion of the file */
    *piSize = AeFileRead(pSource->iCurFileHandle, pSource->pBuffer, sizeof(pSource->pBuffer));
    if (*piSize < 0)
        return AeFalse;
    else if (*piSize == 0)
    {
        AeFileClose(pSource->iCurFileHandle);
        pSource->iCurFileHandle = AeFileInvalidHandle;

        if (pSource->ppUploads[pSource->iUploadIdx]->bDelete)
            AeFileDelete(pSource->ppUploads[pSource->iUploadIdx]->pName);

        pSource->iUploadIdx += 1;
    }
    else if (*piSize > 0)
        *ppData = pSource->pBuffer;

    return AeTrue;
}

/***************************************************************************/
static void AeFileUploadSourceDestroy(AeFileUploadSource *pSource)
{
    if (pSource->iCurFileHandle != AeFileInvalidHandle)
        AeFileClose(pSource->iCurFileHandle);

    AeFree(pSource);
}

/***************************************************************************/
static AeChar *AeFileUploadCreateURL(AeFileUpload *pUpload)
{
    AeDRMServerConfig *pConfig;
    AeDRMDevice *pDevice;
    AeChar *pURL, pDigestString[MD5_DIGEST_LENGTH * 2 + 1];
    AeUInt8 pDigest[MD5_DIGEST_LENGTH];
#ifndef ENABLE_LARGEFILE64
    AeInt32 iTotalSize;
#else
    AeInt64 iTotalSize;
#endif

    pDevice = AE_DRM_DEVICE(pUpload->parent.pPackage->iDeviceId);
    pConfig = AE_DRM_CONFIG(pUpload->parent.pPackage->iConfigId);
    if (!pDevice || !pConfig)
        return NULL;

    memset(pDigest, 0, sizeof(pDigest));

    if ((*pUpload->pFileFilterHandler->pIsEndOfStream)(pUpload->pFileFilter))
    {
        iTotalSize = pUpload->iUploadPos + pUpload->iEncodedSize;
        
        if (!pUpload->bNoOverallChecksum)
            MD5_Final(pDigest, &pUpload->md5Global);
    }
    else
    {
        /* because in case of compressed upload the total size is unknown
           until the end, when transferring an intermediate chunk, forge
           the total size to make Enterprise expect one more chunk */
        if (pUpload->iCompression != AeFileCompressionNone)
            iTotalSize = pUpload->iUploadPos + pUpload->iChunkSize + 1;
        else
            iTotalSize = pUpload->curFileStat.iSize;
    }

    /* generate global digest string (checksum) */
    AeBinToHex(pDigest, sizeof(pDigest), pDigestString);

    pURL = AeAlloc(AE_FILETRANSFER_MAX_URL_LENGTH);
    if (!pURL)
        return NULL;
    
    /* format the base part of the URL */
    snprintf(pURL, AE_FILETRANSFER_MAX_URL_LENGTH, AE_FILE_UPLOAD_URL_FORMAT,
             pUpload->pURL, pDevice->pModelNumber, pDevice->pSerialNumber,
             pConfig->pOwner, pUpload->pSessionId, pDigestString, pUpload->iUploadPos,
             iTotalSize, g_pCompressionStrings[pUpload->iCompression]);

    /* for compressed single-file upload, add parameters indicating the source
       file size and how many bytes of the file were encoded */
    if (pUpload->iCompression != AeFileCompressionNone && !pUpload->bMultiFile)
    {
        AeChar pFileParams[128];
        AeInt iMaxSize;

        snprintf(pFileParams, sizeof(pFileParams), "&file_total=%" AE_LONGINT_FORMAT_MODIFIER "d&file_encoded=%" AE_LONGINT_FORMAT_MODIFIER "d",
                 pUpload->curFileStat.iSize,
                 pUpload->iCurFilePos ? pUpload->iCurFilePos : pUpload->curFileStat.iSize);

        iMaxSize = AE_FILETRANSFER_MAX_URL_LENGTH - strlen(pURL) - 1;
        if (((AeInt) strlen(pFileParams)) <= iMaxSize)
            strncat(pURL, pFileParams, iMaxSize);
    }
    
    return pURL;
}

/***************************************************************************/
static AeWebRequest *AeFileUploadRequestNew(AeFileUpload *pUpload)
{
    AeWebRequest *pRequest;
    AeDRMServerConfig *pConfig;
    MD5_CTX md5Request;
    AeChar *pURL, pDigestString[(MD5_DIGEST_LENGTH + 2) / 3 * 4 + 1];
    AeUInt8 pDigest[MD5_DIGEST_LENGTH];

    pConfig = AE_DRM_CONFIG(pUpload->parent.pPackage->iConfigId);
    if (!pConfig)
        return NULL;

    pURL = AeFileUploadCreateURL(pUpload);
    if (!pURL)
        return NULL;

    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_NETWORK | AE_LOG_CATEGORY_FILE_UPLOAD,
            "Upload (id=%s): URL: %s",
            pUpload->pId, pURL);

    /* generate digest string for this request */
    MD5_Init(&md5Request);
    MD5_Update(&md5Request, (AeUInt8 *) pUpload->pEncodedData, pUpload->iEncodedSize);
    MD5_Final(pDigest, &md5Request);
    AeBase64Encode((AeChar *) pDigest, sizeof(pDigest), pDigestString);

    pRequest = AeWebRequestNew();
    if (pRequest)
    {
        AeWebRequestSetMethod(pRequest, "POST");
        if (AeWebRequestSetURL(pRequest, pURL))
        {
            AeWebRequestSetEntityData(pRequest, pUpload->pEncodedData);
            AeWebRequestSetEntitySize(pRequest, pUpload->iEncodedSize);
            AeWebRequestSetRequestHeader(pRequest, HTTP_CONTENT_MD5, pDigestString);
	        AeWebRequestSetPersistent(pRequest, pConfig->bWebPersistent);
            AeWebRequestSetOnError(pRequest, AeFileUploadRequestOnError);
            AeWebRequestSetOnEntity(pRequest, AeFileUploadRequestOnEntity);
            AeWebRequestSetOnCompleted(pRequest, AeFileUploadRequestOnCompleted);
            AeWebRequestSetOnResponse(pRequest, AeFileUploadRequestOnResponse);

            AeWebRequestSetUserData(pRequest, pUpload);
        }
        else
        {
            AeWebRequestDestroy(pRequest);
            pRequest = NULL;
        }
    }

    AeFree(pURL);

    return pRequest;
}

/**************************************************************************/
static void AeFileUploadRequestOnError(AeWebRequest *pRequest, AeError iError)
{
    AeFileUpload *pUpload;
    
    pUpload = (AeFileUpload *) pRequest->pUserData;
    pUpload->parent.iError = AeFileTransferTranslateError(iError);

    if (g_drmSettings.iLogLevel >= AeLogError)
        AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_NETWORK | AE_LOG_CATEGORY_FILE_UPLOAD,
            "Upload (id=%s): %s: %s",
            pUpload->pId, pUpload->pURL, AE_ERROR_STRING(iError));

    AeWebRequestDestroy(pRequest);
    pUpload->pRequest = NULL;
}

/**************************************************************************/
static AeBool AeFileUploadRequestOnEntity(AeWebRequest *pRequest, AeInt32 iDataOffset, AeChar *pData, AeInt32 iSize)
{
    return AeTrue;
}

/**************************************************************************/
static void AeFileUploadRequestOnCompleted(AeWebRequest *pRequest)
{
    AeFileUpload *pUpload;
    
    pUpload = (AeFileUpload *) pRequest->pUserData;

    pUpload->iUploadPos += pRequest->iEntitySize;

    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_UPLOAD,
            "Upload (id=%s): %" AE_LONGINT_FORMAT_MODIFIER "d bytes uploaded",
            pUpload->pId, pUpload->iUploadPos);

    AeWebRequestDestroy(pRequest);
    pUpload->pRequest = NULL;
}

/**************************************************************************/
static AeBool AeFileUploadRequestOnResponse(AeWebRequest *pRequest, AeInt iStatusCode)
{
    AeFileUpload *pUpload;
    
    if (iStatusCode == HTTP_STATUS_OK)
        return AeTrue;

    pUpload = (AeFileUpload *) pRequest->pUserData;
    
    if (iStatusCode == 571)
        pUpload->parent.iError = AeFILE_CHECKSUM_ERROR;
    else
        pUpload->parent.iError = AeHTTP_STATUS_ERROR;

    if (g_drmSettings.iLogLevel >= AeLogError)
        AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_NETWORK | AE_LOG_CATEGORY_FILE_UPLOAD,
            "Upload (id=%s): HTTP status: %d", pUpload->pId, iStatusCode);

    AeWebRequestDestroy(pRequest);
    pUpload->pRequest = NULL;

    return AeFalse;
}

/**************************************************************************/
static AeFileUploadSpec *AeFileUploadSpecNew(void)
{
    AeFileUploadSpec *pUploadSpec = (AeFileUploadSpec *) AeAlloc(sizeof(AeFileUploadSpec));
    if (!pUploadSpec)
        return NULL;
    memset(pUploadSpec, 0, sizeof(AeFileUploadSpec));

    return pUploadSpec;
}

/**************************************************************************/
static void AeFileUploadSpecDestroy(AeFileUploadSpec *pUploadSpec)
{
    AeFree(pUploadSpec->pName);
    AeFree(pUploadSpec);
}

#endif /* defined(ENABLE_FILE_TRANSFER) */
