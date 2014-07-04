/* $Id: AeNullFileFilter.c,v 1.2 2008/05/21 18:24:34 dkhodos Exp $ */

#include "Ae.h"

#if defined(ENABLE_FILE_TRANSFER)

/******************************************************************************/
AeFileFilterHandler g_nullFileFilterHandler =
{
    (void (*)(void *))                          AeNullFileFilterReset,
    (void (*)(void *))                          AeNullFileFilterDestroy,
    (AeBool (*)(void *))                        AeNullFileFilterIsEndOfStream,
    (AeBool (*)(void *))                        AeNullFileFilterIsEndOfFile,
    (AeInt32 (*)(void *, AeChar *, AeInt32))    AeNullFileFilterWriteRaw,
    (AeBool (*)(void *, AeFileStat **))         AeNullFileFilterReadFile,
    (AeInt32 (*)(void *, AeChar **))            AeNullFileFilterReadFileData,
    (AeInt32 (*)(void *, AeChar *, AeInt32))    AeNullFileFilterReadRaw,
    (AeBool (*)(void *, AeFileStat *))          AeNullFileFilterWriteFile,
    (AeInt32 (*)(void *, AeChar *, AeInt32))    AeNullFileFilterWriteFileData,
    (AeInt32 (*)(void *))                       AeNullFileFilterGetError,
    (AeChar *(*)(void *, AeInt32))              AeNullFileFilterGetErrorString
};

/******************************************************************************/
AeNullFileFilter *AeNullFileFilterNew(AeBool bEncode)
{
    AeNullFileFilter *pFilter;

    pFilter = (AeNullFileFilter *) AeAlloc(sizeof(AeNullFileFilter));
    if (!pFilter)
        return NULL;
    memset(pFilter, 0, sizeof(AeNullFileFilter));

    pFilter->bEncode = bEncode;
    pFilter->iError = AeNullFileFilterEOK;
    pFilter->iState = AeNullFileFilterStateHead;

    return pFilter;
}

/******************************************************************************/
AeBool AeNullFileFilterInitialize(AeNullFileFilter *pFilter, AeFileStat *pFile)
{
    if (!pFilter->bEncode && !pFile)
    {
        pFilter->iError = AeNullFileFilterENoFile;
        return AeFalse;
    }

    if (pFile)
    {
        pFilter->pFile = (AeFileStat *) AeAlloc(sizeof(AeFileStat));
        if (!pFilter->pFile)
        {
            pFilter->iError = AeNullFileFilterEMemory;
            return AeFalse;
        }

        AeFileStatCopy(pFilter->pFile, pFile);
    }

    return AeTrue;
}

/******************************************************************************/
void AeNullFileFilterReset(AeNullFileFilter *pFilter)
{
    if (pFilter->bEncode && pFilter->pFile)
    {
        AeFileStatDestroy(pFilter->pFile);
        pFilter->pFile = NULL;
    }

    pFilter->iError = AeNullFileFilterEOK;
    pFilter->iState = AeNullFileFilterStateHead;
    pFilter->iWriteCount = 0;
    pFilter->pChunk = NULL;
    pFilter->iChunkSize = 0;
    pFilter->iReadPos = 0;
}

/******************************************************************************/
void AeNullFileFilterDestroy(AeNullFileFilter *pFilter)
{
    if (pFilter->pFile)
        AeFileStatDestroy(pFilter->pFile);

    AeFree(pFilter);
}

/******************************************************************************/
AeBool AeNullFileFilterIsEndOfStream(AeNullFileFilter *pFilter)
{
    return (pFilter->iState == AeNullFileFilterStateEnd ? AeTrue : AeFalse);
}

/******************************************************************************/
AeBool AeNullFileFilterIsEndOfFile(AeNullFileFilter *pFilter)
{
    return (pFilter->iState != AeNullFileFilterStateData ? AeTrue : AeFalse);
}

/******************************************************************************/
AeInt32 AeNullFileFilterWriteRaw(AeNullFileFilter *pFilter, AeChar *pData, AeInt32 iSize)
{
    pFilter->iError = AeNullFileFilterEOK;
    if (pFilter->bEncode)
    {
        pFilter->iError = AeNullFileFilterEDirection;
        return -1;
    }

    if (!pFilter->pFile)
    {
        pFilter->iError = AeNullFileFilterENoFile;
        return -1;
    }

    if (pFilter->iWriteCount + iSize > pFilter->pFile->iSize)
        iSize = (AeInt32) (pFilter->pFile->iSize - pFilter->iWriteCount);

    pFilter->iWriteCount += iSize;

    /* new write discards unread content */
    pFilter->iChunkSize = iSize;
    if (pFilter->iChunkSize)
        pFilter->pChunk = pData;

    return iSize;
}

/******************************************************************************/
AeBool AeNullFileFilterReadFile(AeNullFileFilter *pFilter, AeFileStat **ppFile)
{
    pFilter->iError = AeNullFileFilterEOK;
    if (pFilter->bEncode)
    {
        pFilter->iError = AeNullFileFilterEDirection;
        return AeFalse;
    }

    if (!pFilter->pFile)
    {
        pFilter->iError = AeNullFileFilterENoFile;
        return AeFalse;
    }

    if (pFilter->iState != AeNullFileFilterStateHead)
        return AeFalse;

    *ppFile = pFilter->pFile;
 
    pFilter->iState = AeNullFileFilterStateData;

    return AeTrue;
}

/******************************************************************************/
AeInt32 AeNullFileFilterReadFileData(AeNullFileFilter *pFilter, AeChar **ppData)
{
    AeInt32 rc;
    
    pFilter->iError = AeNullFileFilterEOK;
    if (pFilter->bEncode)
    {
        pFilter->iError = AeNullFileFilterEDirection;
        return -1;
    }

    if (!pFilter->pFile)
    {
        pFilter->iError = AeNullFileFilterENoFile;
        return -1;
    }

    rc = pFilter->iChunkSize;
    if (pFilter->iChunkSize)
    {
        *ppData = pFilter->pChunk;
        pFilter->pChunk = NULL;
        pFilter->iChunkSize = 0;

        if (pFilter->iWriteCount < pFilter->pFile->iSize)
            pFilter->iState = AeNullFileFilterStateData;
        else
            pFilter->iState = AeNullFileFilterStateEnd;
    }

    return rc;
}

/******************************************************************************/
AeInt32 AeNullFileFilterReadRaw(AeNullFileFilter *pFilter, AeChar *pData, AeInt32 iSize)
{
    pFilter->iError = AeNullFileFilterEOK;
    if (!pFilter->bEncode)
    {
        pFilter->iError = AeNullFileFilterEDirection;
        return -1;
    }

    if (!pFilter->pFile)
        return 0;

    if (iSize > pFilter->iChunkSize - pFilter->iReadPos)
        iSize = pFilter->iChunkSize - pFilter->iReadPos;

    if (iSize)
    {
        memcpy(pData, pFilter->pChunk + pFilter->iReadPos, iSize);
        pFilter->iReadPos += iSize;

        if (pFilter->iReadPos == pFilter->iChunkSize)
        {
            pFilter->pChunk = NULL;
            pFilter->iChunkSize = 0;
            pFilter->iReadPos = 0;
        }

        if (pFilter->iWriteCount < pFilter->pFile->iSize)
            pFilter->iState = AeNullFileFilterStateData;
        else
            pFilter->iState = AeNullFileFilterStateEnd;
    }

    return iSize;
}

/******************************************************************************/
AeBool AeNullFileFilterWriteFile(AeNullFileFilter *pFilter, AeFileStat *pFile)
{
    pFilter->iError = AeNullFileFilterEOK;
    if (!pFilter->bEncode)
    {
        pFilter->iError = AeNullFileFilterEDirection;
        return AeFalse;
    }

    if (!pFile)
        return AeTrue;
    
    if (pFilter->pFile)
    {
        pFilter->iError = AeNullFileFilterEMultipleFiles;
        return AeFalse;
    }

    pFilter->pFile = (AeFileStat *) AeAlloc(sizeof(AeFileStat));
    if (!pFilter->pFile)
    {
        pFilter->iError = AeNullFileFilterEMemory;
        return AeFalse;
    }

    AeFileStatCopy(pFilter->pFile, pFile);

    return AeTrue;
}

/******************************************************************************/
AeInt32 AeNullFileFilterWriteFileData(AeNullFileFilter *pFilter, AeChar *pData, AeInt32 iSize)
{
    pFilter->iError = AeNullFileFilterEOK;
    if (!pFilter->bEncode)
    {
        pFilter->iError = AeNullFileFilterEDirection;
        return -1;
    }

    if (!pFilter->pFile)
    {
        pFilter->iError = AeNullFileFilterENoFile;
        return -1;
    }

    if (pFilter->iWriteCount + iSize > pFilter->pFile->iSize)
        iSize = (AeInt32) (pFilter->pFile->iSize - pFilter->iWriteCount);

    pFilter->iWriteCount += iSize;

    /* new write discards unread content */
    pFilter->iChunkSize = iSize;
    if (pFilter->iChunkSize)
        pFilter->pChunk = pData;

    return iSize;
}

/******************************************************************************/
AeNullFileFilterError AeNullFileFilterGetError(AeNullFileFilter *pFilter)
{
    return pFilter->iError;
}

/******************************************************************************/
AeChar *AeNullFileFilterGetErrorString(AeNullFileFilter *pFilter, AeNullFileFilterError iError)
{
    switch (iError)
    {
        case AeNullFileFilterEOK:
            return "null filter: Success";
        case AeNullFileFilterEMemory:
            return "null filter: Out of memory";
        case AeNullFileFilterEDirection:
            return "null filter: Wrong stream direction";
        case AeNullFileFilterENoFile:
            return "null filter: No file description";
        case AeNullFileFilterEMultipleFiles:
            return "null filter: Multiple files not supported";
        default:
            return "null filter: Unknown error";
    }
}

#endif /* ENABLE_FILE_TRANSFER */
