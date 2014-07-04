/* $Id: AeTar.c,v 1.4 2008/05/21 18:24:34 dkhodos Exp $ */

#include "Ae.h"

#if defined(ENABLE_FILE_TRANSFER)

static AeBool  AeTarReadBlock(AeTar *pTar, AeChar **ppData);
static AeBool  AeTarReadHeader(AeTar *pTar);
static AeBool  AeTarWriteHeader(AeTar *pTar);
static AeBool  AeTarWriteTail(AeTar *pTar);
static AeBool  AeTarWritePadding(AeTar *pTar, AeInt32 iSize);
static AeInt32 AeTarCRC(AeUInt8 *pData);
#ifndef ENABLE_LARGEFILE64
static AeInt32 AeTarOctToInt(AeUInt8 *pData, AeInt iSize);
#else
static AeInt64 AeTarOctToInt(AeUInt8 *pData, AeInt iSize);
#endif

/******************************************************************************/
AeFileFilterHandler g_tarFileFilterHandler =
{
    (void (*)(void *))                          AeTarReset,
    (void (*)(void *))                          AeTarDestroy,
    (AeBool (*)(void *))                        AeTarIsEOT,
    (AeBool (*)(void *))                        AeTarIsEOF,
    (AeInt32 (*)(void *, AeChar *, AeInt32))    AeTarWriteRaw,
    (AeBool (*)(void *, AeFileStat **))         AeTarReadFile,
    (AeInt32 (*)(void *, AeChar **))            AeTarReadFileData,
    (AeInt32 (*)(void *, AeChar *, AeInt32))    AeTarReadRaw,
    (AeBool (*)(void *, AeFileStat *))          AeTarWriteFile,
    (AeInt32 (*)(void *, AeChar *, AeInt32))    AeTarWriteFileData,
    (AeInt32 (*)(void *))                       AeTarGetError,
    (AeChar *(*)(void *, AeInt32))              AeTarGetErrorString
};

/******************************************************************************/
AeTar *AeTarNew(AeBool bCreate, AeFilterHandler *pHandler)
{
    AeTar *pTar;

    pTar = (AeTar *) AeAlloc(sizeof(AeTar));
    if (!pTar)
        return NULL;
    memset(pTar, 0, sizeof(AeTar));

    pTar->bCreate = bCreate;
    pTar->pHandler = pHandler;
    pTar->pFilter = NULL;
    pTar->iError = AeTarEOK;
    pTar->iState = AeTarStateHeader;
    pTar->iCurFilePos = 0;

    return pTar;
}

/******************************************************************************/
AeBool AeTarInitialize(AeTar *pTar)
{
    pTar->pFilter = (*pTar->pHandler->pOpen)(pTar->bCreate);
    if (!pTar->pFilter)
        return AeFalse;
        
    pTar->pBuffer = AeBufferNew(NULL, 0, AE_BUFFER_CHUNK);
    if (!pTar->pBuffer)
        return AeFalse;

    if (!AeBufferExpand(pTar->pBuffer, TAR_BLOCK_SIZE))
        return AeFalse;

    return AeTrue;
}

/******************************************************************************/
void AeTarReset(AeTar *pTar)
{
    pTar->iError = AeTarEOK;
    pTar->iState = AeTarStateHeader;
    pTar->iCurFilePos = 0;
    AeBufferReset(pTar->pBuffer);
    (*pTar->pHandler->pReset)(pTar->pFilter);

    AeSetString(&pTar->curFile.pName, NULL, -1);
    memset(&pTar->curFile, 0, sizeof(AeFileStat));
}

/******************************************************************************/
void AeTarDestroy(AeTar *pTar)
{
    if (pTar->pFilter)
        (*pTar->pHandler->pClose)(pTar->pFilter);

    if (pTar->pBuffer)
        AeBufferDestroy(pTar->pBuffer);

    if (pTar->curFile.pName)
        AeFree(pTar->curFile.pName);

    AeFree(pTar);
}

/******************************************************************************/
AeBool AeTarIsEOT(AeTar *pTar)
{
    return (pTar->iState == AeTarStateEnd ? AeTrue : AeFalse);
}

/******************************************************************************/
AeBool AeTarIsEOF(AeTar *pTar)
{
    return (pTar->iState != AeTarStateData ? AeTrue : AeFalse);
}

/******************************************************************************/
AeInt32 AeTarWriteRaw(AeTar *pTar, AeChar *pData, AeInt32 iSize)
{
    if (pTar->bCreate)
    {
        pTar->iError = AeTarEDirection;
        return -1;
    }

    return (*pTar->pHandler->pWrite)(pTar->pFilter, pData, iSize);
}

/******************************************************************************/
AeBool AeTarReadFile(AeTar *pTar, AeFileStat **ppFile)
{
    AeBool bOK;

    bOK = AeFalse;
    pTar->iError = AeTarEOK;
    if (pTar->bCreate)
    {
        pTar->iError = AeTarEDirection;
        return AeFalse;
    }

    /* skip data blocks if any */
    while (pTar->iState == AeTarStateData && AeTarReadBlock(pTar, NULL))
    {
        pTar->iCurFilePos += TAR_BLOCK_SIZE;
        if (pTar->iCurFilePos >= pTar->curFile.iSize)
        {
            pTar->iState = AeTarStateHeader;
            pTar->iCurFilePos = 0;
        }
    }

    /* read tar header */
    if (pTar->iState == AeTarStateHeader)
        bOK = AeTarReadHeader(pTar);

    /* consume input until end */
    if (pTar->iState == AeTarStateTail)
        AeTarReadHeader(pTar);

    if (bOK)
        *ppFile = &pTar->curFile;
 
    return bOK;
}

/******************************************************************************/
AeInt32 AeTarReadFileData(AeTar *pTar, AeChar **ppData)
{
    pTar->iError = AeTarEOK;
    if (pTar->bCreate)
    {
        pTar->iError = AeTarEDirection;
        return -1;
    }

    if (pTar->iState != AeTarStateData)
        return 0;

    /* read one block */
    if (!AeTarReadBlock(pTar, ppData))
        return 0;

    /* detect last block */
    pTar->iCurFilePos += TAR_BLOCK_SIZE;
    if (pTar->iCurFilePos >= pTar->curFile.iSize)
    {
        pTar->iState = AeTarStateHeader;
        pTar->iCurFilePos = 0;

        if (pTar->curFile.iSize % TAR_BLOCK_SIZE)
            return (AeInt32) (pTar->curFile.iSize % TAR_BLOCK_SIZE);
    }

    return TAR_BLOCK_SIZE;
}

/******************************************************************************/
AeInt32 AeTarReadRaw(AeTar *pTar, AeChar *pData, AeInt32 iSize)
{
    AeInt32 rc;

    pTar->iError = AeTarEOK;
    if (!pTar->bCreate)
    {
        pTar->iError = AeTarEDirection;
        return -1;
    }

    if (pTar->iState == AeTarStateEnd)
        return 0;

    rc = (*pTar->pHandler->pRead)(pTar->pFilter, pData, iSize);
    if (rc < 0)
    {
        pTar->iError = (AeTarError) (AeTarEFilter +
                                     (*pTar->pHandler->pGetError)(pTar->pFilter));
        return -1;
    }
    
    if (rc == 0)
    {
        if (pTar->curFile.iSize == -1)
            pTar->iState = AeTarStateEnd;
    }

    return rc;
}

/******************************************************************************/
AeBool AeTarWriteFile(AeTar *pTar, AeFileStat *pFile)
{
    pTar->iError = AeTarEOK;
    if (!pTar->bCreate)
    {
        pTar->iError = AeTarEDirection;
        return AeFalse;
    }

    /* check if we can start new file */
    if (pTar->curFile.iType == AeFileTypeRegular &&
        pTar->iCurFilePos < pTar->curFile.iSize)
    {
        pTar->iError = AeTarEIncomplete;
        return AeFalse;
    }

    /* no file indicates end of archive */
    if (pFile)
    {
        pTar->iCurFilePos = 0;
        AeSetString(&pTar->curFile.pName, pFile->pName, -1);
        pTar->curFile.iType = pFile->iType;
        pTar->curFile.iSize = pFile->iSize;
        pTar->curFile.iMTime = pFile->iMTime;

        /* write header block */
        return AeTarWriteHeader(pTar);
    }
    else
    {
        AeSetString(&pTar->curFile.pName, NULL, -1);
        memset(&pTar->curFile, 0, sizeof(AeFileStat));
        pTar->curFile.iSize = -1;

        /* write tail block(s) */
        return AeTarWriteTail(pTar);
    }
}

/******************************************************************************/
AeInt32 AeTarWriteFileData(AeTar *pTar, AeChar *pData, AeInt32 iSize)
{
    AeInt32 iResult;

    pTar->iError = AeTarEOK;
    if (!pTar->bCreate)
    {
        pTar->iError = AeTarEDirection;
        return -1;
    }

    if (pTar->curFile.iType != AeFileTypeRegular)
        return 0;

    /* submitted content may be truncated based on file size */
    if (pTar->iCurFilePos + iSize >= pTar->curFile.iSize)
        iSize = (AeInt32) (pTar->curFile.iSize - pTar->iCurFilePos);

    if (iSize == 0)
        return 0;

    iResult = (*pTar->pHandler->pWrite)(pTar->pFilter, pData, iSize);
    if (iResult != iSize)
    {
        pTar->iError = AeTarEMemory;
        return -1;
    }

    pTar->iCurFilePos += iResult;

    /* pad last block with zeroes */
    if (pTar->iCurFilePos == pTar->curFile.iSize)
    {
        AeInt32 iLastChunkSize;

        iLastChunkSize = (AeInt32) (pTar->curFile.iSize % TAR_BLOCK_SIZE);
        if (iLastChunkSize)
        {
            if (!AeTarWritePadding(pTar, TAR_BLOCK_SIZE - iLastChunkSize))
            {
                pTar->iError = AeTarEMemory;
                return -1;
            }
        }
    }

    return iResult;
}

/******************************************************************************/
AeInt32 AeTarGetError(AeTar *pTar)
{
    return pTar->iError;
}

/******************************************************************************/
AeChar *AeTarGetErrorString(AeTar *pTar, AeTarError iError)
{
    switch (iError)
    {
        case AeTarEOK:
            return "tar: Success";
        case AeTarEMemory:
            return "tar: Out of memory";
        case AeTarEDirection:
            return "tar: Wrong stream direction";
        case AeTarEBadMagic:
            return "tar: Bad magic";
        case AeTarEBadCRC:
            return "tar: CRC error";
        case AeTarEBadFile:
            return "tar: Unsupported file type";
        case AeTarEIncomplete:
            return "tar: Premature end of input";
        default:
            if (iError >= AeTarEFilter)
                return (*pTar->pHandler->pGetErrorString)(iError - AeTarEFilter);
            return "tar: Unknown error";
    }
}

/******************************************************************************/
static AeBool AeTarReadBlock(AeTar *pTar, AeChar **ppData)
{
    AeChar *pBufferData;
    AeInt32 iBufferSize, rc;

    /* check if input is exhausted */
    AeBufferGetReadArea(pTar->pBuffer, &pBufferData, &iBufferSize);
    if (iBufferSize < TAR_BLOCK_SIZE)
    {
        /* read more data */
        AeBufferGetWriteArea(pTar->pBuffer, &pBufferData, &iBufferSize);
        rc = (*pTar->pHandler->pRead)(pTar->pFilter, pBufferData, iBufferSize);
        if (rc < 0)
        {
            pTar->iError = (AeTarError) (AeTarEFilter +
                                         (*pTar->pHandler->pGetError)(pTar->pFilter));
            return AeFalse;
        }
        else if (rc == 0 && (*pTar->pHandler->pIsEnd)(pTar->pFilter))
        {
            pTar->iError = AeTarEIncomplete;
            return AeFalse;
        }

        AeBufferSeekWritePos(pTar->pBuffer, rc, AeTrue);
    }

    /* need whole block */
    AeBufferGetReadArea(pTar->pBuffer, &pBufferData, &iBufferSize);
    if (iBufferSize < TAR_BLOCK_SIZE)
        return AeFalse;

    AeBufferSeekReadPos(pTar->pBuffer, TAR_BLOCK_SIZE, AeTrue);
    *ppData = pBufferData;

    return AeTrue;
}

/******************************************************************************/
static AeBool AeTarReadHeader(AeTar *pTar)
{
    AeChar *pData;
    AeInt32 iSize;
    AeTarHeader *pHeader;

    /* read raw block */
    if (!AeTarReadBlock(pTar, &pData))
        return AeFalse;

    pHeader = (AeTarHeader *) pData;

    /* check for zero block */
    if (pHeader->name[0] == 0)
    {
#ifdef AE_TAR_DOUBLE_EOT
        if (pTar->iState != AeTarStateTail)
            pTar->iState = AeTarStateTail;
        else
#endif
            pTar->iState = AeTarStateEnd;
        return AeFalse;
    }

    /* check magic */
    if (strncmp(pHeader->magic, TAR_TMAGIC, strlen(TAR_TMAGIC)))
    {
        pTar->iError = AeTarEBadMagic;
        return AeFalse;
    }

    /* check CRC (octal number string) */
    if (((AeInt32) AeTarOctToInt((AeUInt8 *) pHeader->chksum, sizeof(pHeader->chksum))) !=
        AeTarCRC((AeUInt8 *) pData))
    {
        pTar->iError = AeTarEBadCRC;
        return AeFalse;
    }

    /* read file name (NUL-terminated if NUL fits) */
    if (pHeader->name[sizeof(pHeader->name) - 1] == 0)
        iSize = -1;
    else
        iSize = sizeof(pHeader->name);
    AeSetString(&pTar->curFile.pName, pHeader->name, iSize);

    /* read size (octal number string) */
    pTar->curFile.iSize = AeTarOctToInt((AeUInt8 *) pHeader->size, sizeof(pHeader->size));

    /* read mtime (octal number string) */
    pTar->curFile.iMTime = (time_t) AeTarOctToInt((AeUInt8 *) pHeader->mtime, sizeof(pHeader->mtime));

    /* read type */
    switch (pHeader->typeflag[0])
    {
        case TAR_REGTYPE:
        case TAR_AREGTYPE:
            pTar->curFile.iType = AeFileTypeRegular;
            break;
        case TAR_DIRTYPE:
            pTar->curFile.iType = AeFileTypeDirectory;
            break;
        default:
            pTar->curFile.iType = AeFileTypeUnknown;
            break;
    }

    /* enter data blocks mode only for regular files */
    if (pTar->curFile.iType == AeFileTypeRegular)
    {
        pTar->iState = AeTarStateData;
        pTar->iCurFilePos = 0;
    }

    return AeTrue;
}

/******************************************************************************/
static AeBool AeTarWriteHeader(AeTar *pTar)
{
    AeChar *pBufferData;
    AeInt32 iBufferSize;
    AeTarHeader *pHeader;

    /* prepare buffer */
    AeBufferReset(pTar->pBuffer);
    AeBufferGetWriteArea(pTar->pBuffer, &pBufferData, &iBufferSize);
    memset(pBufferData, 0, TAR_BLOCK_SIZE);

    pHeader = (AeTarHeader *) pBufferData;

    /* write type */
    switch (pTar->curFile.iType)
    {
        case AeFileTypeRegular:
            pHeader->typeflag[0] = TAR_REGTYPE;
            break;
        case AeFileTypeDirectory:
            pHeader->typeflag[0] = TAR_DIRTYPE;
            break;
        default:
            pTar->iError = AeTarEBadFile;
            return AeFalse;
    }

    /* write file name */
    strncpy(pHeader->name, pTar->curFile.pName, sizeof(pHeader->name));

    /* write size (octal number string) */
    snprintf(pHeader->size, sizeof(pHeader->size), "%011" AE_SIZE_FORMAT_MODIFIER "o", pTar->curFile.iSize);

    /* write mtime (octal number string) */
    snprintf(pHeader->mtime, sizeof(pHeader->mtime), "%011" AE_TIME_FORMAT_MODIFIER "o", pTar->curFile.iMTime);

    /* write default permissions (octal number string) */
    strncpy(pHeader->mode, AE_TAR_DEFAULT_MODE, sizeof(pHeader->mode));

    /* write default uid (octal number string) */
    strncpy(pHeader->uid, AE_TAR_DEFAULT_UID, sizeof(pHeader->uid));

    /* write default gid (octal number string) */
    strncpy(pHeader->gid, AE_TAR_DEFAULT_GID, sizeof(pHeader->gid));

    /* write magic */
    strncpy(pHeader->magic, TAR_TMAGIC, sizeof(pHeader->magic));

    /* write version */
    strncpy(pHeader->version, TAR_TVERSION, sizeof(pHeader->version));

    /* write default user name */
    strncpy(pHeader->uname, AE_TAR_DEFAULT_UNAME, sizeof(pHeader->uname));

    /* write default group name */
    strncpy(pHeader->gname, AE_TAR_DEFAULT_GNAME, sizeof(pHeader->gname));

    /* write checksum (octal number string) */
    snprintf(pHeader->chksum, sizeof(pHeader->chksum), "%07" AE_INT32_FORMAT_MODIFIER "o",
             AeTarCRC((AeUInt8 *) pBufferData));

    /* write to filter */
    if ((*pTar->pHandler->pWrite)(pTar->pFilter, pBufferData, TAR_BLOCK_SIZE) !=
        TAR_BLOCK_SIZE)
    {
        pTar->iError = AeTarEMemory;
        return AeFalse;
    }

    return AeTrue;
}

/******************************************************************************/
static AeBool AeTarWriteTail(AeTar *pTar)
{
    /* write 2 zero blocks */
    if (!AeTarWritePadding(pTar, TAR_BLOCK_SIZE))
        return AeFalse;
    if (!AeTarWritePadding(pTar, TAR_BLOCK_SIZE))
        return AeFalse;

    /* indicate end of data */
    (*pTar->pHandler->pWrite)(pTar->pFilter, NULL, 0);

    return AeTrue;
}

/******************************************************************************/
static AeBool AeTarWritePadding(AeTar *pTar, AeInt32 iSize)
{
    AeChar *pBufferData;
    AeInt32 iBufferSize, iResult;
    
    /* prepare buffer */
    AeBufferReset(pTar->pBuffer);
    AeBufferGetWriteArea(pTar->pBuffer, &pBufferData, &iBufferSize);
    memset(pBufferData, 0, iSize);

    iResult = (*pTar->pHandler->pWrite)(pTar->pFilter, pBufferData, iSize);

    return (iResult == iSize ? AeTrue : AeFalse);
}

/******************************************************************************/
/*
   chksum contains the sum of all 512 bytes in the header block,
   treating each byte as an 8-bit unsigned value and treating the 8
   bytes of chksum as blank characters.
*/
static AeInt32 AeTarCRC(AeUInt8 *pData)
{
    AeInt32 i, iSum;

    iSum = 0;
    for (i = 0; i < TAR_BLOCK_SIZE; i++)
    {
        if (i >= 148 && i < 156)
            iSum += ' ';
        else
            iSum += pData[i];
    }

    return iSum;
}

/******************************************************************************/
#ifndef ENABLE_LARGEFILE64
static AeInt32 AeTarOctToInt(AeUInt8 *pData, AeInt iSize)
#else
static AeInt64 AeTarOctToInt(AeUInt8 *pData, AeInt iSize)
#endif
{
#ifndef ENABLE_LARGEFILE64
    AeInt32 iResult;
#else
    AeInt64 iResult;
#endif
    AeBool bNumber;

    iResult = 0;
    bNumber = AeFalse;
    while (iSize--)
    {
        if (*pData >= '0' && *pData < '8')
        {
            iResult = (iResult << 3) + (*pData - '0');
            bNumber = AeTrue;
        }
        else if (*pData == ' ')
        {
            if (bNumber)
                break;
        }
        else
            break;

        pData += 1;
    }

    return iResult;
}

#endif /* ENABLE_FILE_TRANSFER */
