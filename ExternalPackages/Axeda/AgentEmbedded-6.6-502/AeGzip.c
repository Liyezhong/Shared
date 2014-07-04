/* $Id: AeGzip.c,v 1.6 2008/05/21 18:24:34 dkhodos Exp $ */

#include "Ae.h"

#if defined(ENABLE_FILE_TRANSFER)

static AeInt32 AeGzipCompress(AeGzipFilter *pFilter, AeChar *pData, AeInt32 iSize);
static AeInt32 AeGzipUncompress(AeGzipFilter *pFilter, AeChar *pData, AeInt32 iSize);
static AeInt32 AeGzipReadHeader(AeGzipFilter *pFilter);
static AeInt32 AeGzipUncompressBlocks(AeGzipFilter *pFilter, AeChar *pData, AeInt32 iSize);
static AeInt32 AeGzipReadTail(AeGzipFilter *pFilter);
static AeInt32 AeGzipWriteHeader(AeGzipFilter *pFilter, AeChar **ppData, AeInt32 *piSize);
static AeInt32 AeGzipCompressBlocks(AeGzipFilter *pFilter, AeChar **ppData, AeInt32 *piSize);
static AeInt32 AeGzipWriteTail(AeGzipFilter *pFilter, AeChar **ppData, AeInt32 *piSize);

static AeGzipFilter *AeGzipFilterOpen(AeBool bEncode);
static AeInt32      AeGzipFilterGetError(AeGzipFilter *pFilter);
static AeChar       *AeGzipFilterGetErrorString(AeInt32 iError);

/******************************************************************************/
AeFilterHandler g_gzipFilterHandler =
{
    (void *(*)(AeBool))                      AeGzipFilterOpen,
    (void (*)(void *))                       AeGzipFilterDestroy,
    (void (*)(void *))                       AeGzipFilterReset,
    (AeInt32 (*)(void *, AeChar *, AeInt32)) AeGzipFilterWrite,
    (AeInt32 (*)(void *, AeChar *, AeInt32)) AeGzipFilterRead,
    (AeBool (*)(void *))                     AeGzipFilterIsEnd,
    (AeInt32 (*)(void *))                    AeGzipFilterGetError,
    (AeChar *(*)(AeInt32))                   AeGzipFilterGetErrorString
};

/******************************************************************************/
AeGzipFilter *AeGzipFilterNew(AeBool bCompress)
{
    AeGzipFilter *pFilter;

    pFilter = (AeGzipFilter *) AeAlloc(sizeof(AeGzipFilter));
    if (!pFilter)
        return NULL;
    memset(pFilter, 0, sizeof(AeGzipFilter));

    pFilter->bCompress = bCompress;

    return pFilter;
}

/******************************************************************************/
void AeGzipFilterDestroy(AeGzipFilter *pFilter)
{
    if (pFilter->pZStream)
    {
        if (pFilter->bCompress)
            deflateEnd(pFilter->pZStream);
        else
            inflateEnd(pFilter->pZStream);

        AeFree(pFilter->pZStream);
    }

    if (pFilter->pBuffer)
        AeBufferDestroy(pFilter->pBuffer);

    AeFree(pFilter);
}

/******************************************************************************/
AeBool AeGzipFilterInitialize(AeGzipFilter *pFilter)
{ 
    int iErr;

    pFilter->iError = AeGzipEMemory;

    pFilter->pZStream = AeAlloc(sizeof(z_stream));
    if (!pFilter->pZStream)
        return AeFalse;
    memset(pFilter->pZStream, 0, sizeof(z_stream));

    pFilter->pBuffer = AeBufferNew(NULL, 0, AE_BUFFER_CHUNK);
    if (!pFilter->pBuffer)
        return AeFalse;

    if (pFilter->bCompress)
        iErr = deflateInit2(pFilter->pZStream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                            -GZIP_WBITS, GZIP_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    else
        iErr = inflateInit2(pFilter->pZStream, -MAX_WBITS);
    if (iErr != Z_OK)
        return AeFalse;

    AeGzipFilterReset(pFilter);

    return AeTrue;
}

/******************************************************************************/
void AeGzipFilterReset(AeGzipFilter *pFilter)
{
    pFilter->iError = AeGzipEOK;
    pFilter->iState = AeGzipStateHeader;
    pFilter->bFinish = AeFalse;
    pFilter->iCRC = crc32(0, NULL, 0);
    AeBufferReset(pFilter->pBuffer);

    pFilter->pZStream->next_in = NULL;
    pFilter->pZStream->avail_in = 0;
    if (pFilter->bCompress)
        deflateReset(pFilter->pZStream);
    else
        inflateReset(pFilter->pZStream);
}

/******************************************************************************/
AeInt32 AeGzipFilterWrite(AeGzipFilter *pFilter, AeChar *pData, AeInt32 iSize)
{
    /* empty block indicates end of data */
    if (!pData || iSize == 0)
    {
        pFilter->bFinish = AeTrue;
        return 0;
    }

    /* update the checksum when compressing */
    if (pFilter->bCompress)
        pFilter->iCRC = crc32(pFilter->iCRC, (Bytef *) pData, iSize);

    return AeBufferWrite(pFilter->pBuffer, pData, iSize);
}

/******************************************************************************/
AeInt32 AeGzipFilterRead(AeGzipFilter *pFilter, AeChar *pData, AeInt32 iSize)
{
    if (pFilter->bCompress)
        return AeGzipCompress(pFilter, pData, iSize);
    else
        return AeGzipUncompress(pFilter, pData, iSize);
}

/******************************************************************************/
AeBool AeGzipFilterIsEnd(AeGzipFilter *pFilter)
{
    return (pFilter->iState == AeGzipStateEnd);
}

/******************************************************************************/
void AeGzipFilterSetLevel(AeGzipFilter *pFilter, AeInt32 iLevel)
{
    deflateParams(pFilter->pZStream, (int) iLevel, Z_DEFAULT_STRATEGY);
}

/******************************************************************************/
static AeInt32 AeGzipCompress(AeGzipFilter *pFilter, AeChar *pData, AeInt32 iSize)
{
    AeInt32 iResult, rc;

    iResult = 0;
    pFilter->iError = AeGzipEOK;

    /* write gzip header */
    if (pFilter->iState == AeGzipStateHeader)
    {
        rc = AeGzipWriteHeader(pFilter, &pData, &iSize);
        if (rc < 0)
            return rc;
        iResult += rc;
    }

    /* compress blocks */
    if (pFilter->iState == AeGzipStateBlocks)
    {
        rc = AeGzipCompressBlocks(pFilter, &pData, &iSize);
        if (rc < 0)
            return rc;
        iResult += rc;
    }

    /* write gzip tail */
    if (pFilter->iState == AeGzipStateTail)
    {
        rc = AeGzipWriteTail(pFilter, &pData, &iSize);
        if (rc < 0)
            return rc;
        iResult += rc;
    }

    return iResult;
}

/******************************************************************************/
static AeInt32 AeGzipUncompress(AeGzipFilter *pFilter, AeChar *pData, AeInt32 iSize)
{
    AeInt32 iResult, rc;

    iResult = 0;
    pFilter->iError = AeGzipEOK;

    /* check gzip header */
    if (pFilter->iState == AeGzipStateHeader)
    {
        rc = AeGzipReadHeader(pFilter);
        if (rc <= 0)
            return rc;
    }

    /* uncompress blocks */
    if (pFilter->iState == AeGzipStateBlocks)
    {
        iResult = AeGzipUncompressBlocks(pFilter, pData, iSize);
        if (iResult <= 0)
            return iResult;
    }

    /* check gzip tail */
    if (pFilter->iState == AeGzipStateTail)
    {
        rc = AeGzipReadTail(pFilter);
        if (rc <= 0)
            return rc;
    }

    return iResult;
}

/******************************************************************************/
static AeInt32 AeGzipReadHeader(AeGzipFilter *pFilter)
{
    AeUInt8 *pBufferData;
    AeChar *_pBufferData;
    AeInt32 iBufferSize, iPos;

    AeBufferGetReadArea(pFilter->pBuffer, &_pBufferData, &iBufferSize);
    pBufferData = (AeUInt8 *) _pBufferData;

    /* gzip header is at least 10 bytes long  */
    if (iBufferSize < 10)
        return 0;

    /* check magic */
    if (pBufferData[0] != GZIP_MAGIC_1 || pBufferData[1] != GZIP_MAGIC_2)
    {
        pFilter->iError = AeGzipEBadMagic;
        return -1;
    }

    /* check method */
    if (pBufferData[2] != Z_DEFLATED)
    {
        pFilter->iError = AeGzipEBadMethod;
        return -1;
    }

    iPos = 10;

    /* skip extra field */
    if (pBufferData[3] & GZIP_EXTRA_FIELD)
    {
        /* two-byte extra field length field */
        iPos += 2;
        if (iPos > iBufferSize)
            return 0;

        /* extra field content */
        iPos += pBufferData[iPos] + (pBufferData[iPos + 1] << 8);
        if (iPos > iBufferSize)
            return 0;
    }

    /* skip file name field */
    if (pBufferData[3] & GZIP_ORIG_NAME)
    {
        while (iPos < iBufferSize && pBufferData[iPos] != 0)
            iPos += 1;
        if (iPos++ > iBufferSize)
            return 0;
    }

    /* skip comment field */
    if (pBufferData[3] & GZIP_COMMENT)
    {
        while (iPos < iBufferSize && pBufferData[iPos] != 0)
            iPos += 1;
        if (iPos++ > iBufferSize)
            return 0;
    }

    /* skip header CRC field */
    if (pBufferData[3] & GZIP_HEAD_CRC)
    {
        iPos += 2;
        if (iPos > iBufferSize)
            return 0;
    }

    pFilter->iState = AeGzipStateBlocks;
    AeBufferSeekReadPos(pFilter->pBuffer, iPos, AeTrue);

    return iPos;
}

/******************************************************************************/
static AeInt32 AeGzipReadTail(AeGzipFilter *pFilter)
{
    AeUInt8 *pBufferData;
    AeChar *_pBufferData;
    AeInt32 iBufferSize;

    AeBufferGetReadArea(pFilter->pBuffer, &_pBufferData, &iBufferSize);
    pBufferData = (AeUInt8 *) _pBufferData;

    /* gzip tail is 8 bytes long  */
    if (iBufferSize < 8)
        return 0;

    /* validate the checksum (32 bit LSB) */
    if (pFilter->iCRC !=
        ((AeUInt32) pBufferData[0] + ((AeUInt32) pBufferData[1] << 8) +
         ((AeUInt32) pBufferData[2] << 16) + ((AeUInt32) pBufferData[3] << 24)))
    {
        pFilter->iError = AeGzipEBadCRC;
        return -1;
    }

    /* check total length (32 bit LSB) */
    if (pFilter->pZStream->total_out !=
        ((AeUInt32) pBufferData[4] + ((AeUInt32) pBufferData[5] << 8) +
         ((AeUInt32) pBufferData[6] << 16) + ((AeUInt32) pBufferData[7] << 24)))
    {
        pFilter->iError = AeGzipEBadLength;
        return -1;
    }

    pFilter->iState = AeGzipStateEnd;
    AeBufferSeekReadPos(pFilter->pBuffer, 8, AeTrue);

    return 8;
}

/******************************************************************************/
static AeInt32 AeGzipUncompressBlocks(AeGzipFilter *pFilter, AeChar *pData, AeInt32 iSize)
{
    AeChar *pBufferData;
    AeInt32 iBufferSize, rc;

    pFilter->pZStream->next_out = (Bytef *) pData;
    pFilter->pZStream->avail_out = iSize;
    while (pFilter->pZStream->avail_out != 0)
    {
        /* provide more input if needed */
        if (pFilter->pZStream->avail_in == 0)
        {
            AeBufferGetReadArea(pFilter->pBuffer, &pBufferData, &iBufferSize);
            if (iBufferSize == 0)
                break;

            pFilter->pZStream->next_in = (Bytef *) pBufferData;
            pFilter->pZStream->avail_in = iBufferSize;
        }
        iBufferSize = (AeInt32) pFilter->pZStream->avail_in;
            
        /* uncompress */
        rc = inflate(pFilter->pZStream, Z_NO_FLUSH);

        /* update input buffer position */
        iBufferSize -= pFilter->pZStream->avail_in;
        AeBufferSeekReadPos(pFilter->pBuffer, iBufferSize, AeTrue);

        if (rc == Z_STREAM_END)
        {
            pFilter->iState = AeGzipStateTail;
            break;
        }
        else if (rc != Z_OK)
        {
            pFilter->iError = AeGzipEBadBlock;
            return -1;
        }
    }

    rc = iSize - pFilter->pZStream->avail_out;

    /* update the checksum */
    if (rc != 0)
        pFilter->iCRC = crc32(pFilter->iCRC, (Bytef *) pData, rc);

    return rc;
}

/******************************************************************************/
static AeInt32 AeGzipWriteHeader(AeGzipFilter *pFilter, AeChar **ppData, AeInt32 *piSize)
{
    /* gzip header is 10 bytes long */
    if (*piSize < 10)
        return 0;

    /* simple gzip header (from zlib/gzio.c) */
    memset(*ppData, 0, 10);
    ((AeUInt8 *) *ppData)[0] = GZIP_MAGIC_1;    /* magic 1 */
    ((AeUInt8 *) *ppData)[1] = GZIP_MAGIC_2;    /* magic 2 */
    ((AeUInt8 *) *ppData)[2] = Z_DEFLATED;      /* method */
    ((AeUInt8 *) *ppData)[9] = GZIP_OS_CODE;    /* os code */

    *ppData += 10;
    *piSize -= 10;
    pFilter->iState = AeGzipStateBlocks;

    return 10;
}

/******************************************************************************/
static AeInt32 AeGzipCompressBlocks(AeGzipFilter *pFilter, AeChar **ppData, AeInt32 *piSize)
{
    AeChar *pBufferData;
    AeInt32 iBufferSize, rc;
    int iFlush;

    iFlush = Z_NO_FLUSH;
    pFilter->pZStream->next_out = (Bytef *) *ppData;
    pFilter->pZStream->avail_out = *piSize;
    while (pFilter->pZStream->avail_out != 0)
    {
        /* provide more input if needed */
        if (pFilter->pZStream->avail_in == 0)
        {
            AeBufferGetReadArea(pFilter->pBuffer, &pBufferData, &iBufferSize);
            if (iBufferSize == 0)
            {
                if (pFilter->bFinish)
                    iFlush = Z_FINISH;
                else
                    break;
            }

            pFilter->pZStream->next_in = (Bytef *) pBufferData;
            pFilter->pZStream->avail_in = iBufferSize;
        }
        iBufferSize = (AeInt32) pFilter->pZStream->avail_in;
            
        /* compress */
        rc = deflate(pFilter->pZStream, iFlush);

        /* update input buffer position */
        iBufferSize -= pFilter->pZStream->avail_in;
        AeBufferSeekReadPos(pFilter->pBuffer, iBufferSize, AeTrue);

        if (rc == Z_STREAM_END)
        {
            pFilter->iState = AeGzipStateTail;
            break;
        }
        else if (rc != Z_OK)
        {
            pFilter->iError = AeGzipEBadBlock;
            return -1;
        }
    }

    rc = *piSize - pFilter->pZStream->avail_out;
    *ppData += rc;
    *piSize -= rc;

    return rc;
}

/******************************************************************************/
static AeInt32 AeGzipWriteTail(AeGzipFilter *pFilter, AeChar **ppData, AeInt32 *piSize)
{
    AeUInt32 iNumber;

    /* gzip tail is 8 bytes long  */
    if (*piSize < 8)
        return 0;

    /* checksum (32 bit LSB) */
    iNumber = AeNetHToLEL(pFilter->iCRC);
    memcpy(*ppData, &iNumber, 4);

    /* uncompressed length (32 bit LSB) */
    iNumber = AeNetHToLEL(pFilter->pZStream->total_in);
    memcpy(*ppData + 4, &iNumber, 4);

    *ppData += 8;
    *piSize -= 8;
    pFilter->iState = AeGzipStateEnd;

    return 8;
}

/******************************************************************************/
static AeGzipFilter *AeGzipFilterOpen(AeBool bEncode)
{
    AeGzipFilter *pFilter;

    pFilter = AeGzipFilterNew(bEncode);
    if (!pFilter)
        return NULL;

    if (!AeGzipFilterInitialize(pFilter))
    {
        AeGzipFilterDestroy(pFilter);
        return NULL;
    }

    return pFilter;
}

/******************************************************************************/
static AeInt32 AeGzipFilterGetError(AeGzipFilter *pFilter)
{
    return (AeInt32) pFilter->iError;
}

/******************************************************************************/
static AeChar *AeGzipFilterGetErrorString(AeInt32 iError)
{
    switch ((AeGzipError) iError)
    {
        case AeGzipEOK:
            return "gzip: Success";
        case AeGzipEMemory:
            return "gzip: Out of memory";
        case AeGzipEBadMagic:
            return "gzip: Bad magic";
        case AeGzipEBadMethod:
            return "gzip: Unsupported method";
        case AeGzipEBadBlock:
            return "gzip: Bad block";
        case AeGzipEBadCRC:
            return "gzip: CRC error";
        case AeGzipEBadLength:
            return "gzip: Incorrect length";
        default:
            return "gzip: Unknown error";
    }
}

#endif /* ENABLE_FILE_TRANSFER */
